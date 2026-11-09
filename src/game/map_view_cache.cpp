/* Bounded, persistent viewport images for the optional GPU camera.
 * The ordinary tile painter owns layering; this cache only clips/reuses its
 * output. Separate offscreen MapViews never own or mutate a game window. */
#include <stdint.h>
extern "C" {
#include "../host/host.h"
}
#include "game.h"
#include "../platform/hd.h"
EXTERN_C void plat_log(const char *,...);
EXTERN_C void plat_fatal(const char *,...);

enum { CELL=128, SLOTS=3, MAX_CELLS=1024, PAD=128 };
struct CachedView {
    MapView *view;
    GameFont label_font;
    int label_font_size;
    int cols,rows,civ;
    unsigned used;
    unsigned char valid[MAX_CELLS];
};
static CachedView entries[SLOTS];
static MapView *painting;
static unsigned clock_used;
static int key_width,key_height,key_density,key_civ,key_hidden,key_reveal,key_options,key_palette;
static int key_map_w,key_map_h,key_rules;
static int key_detail;
static uint64_t rendered,reused,hits,misses;

int map_view_cache_rendering(void) { return painting!=0; }
int map_view_cache_surface(void *surface) { return painting==surface; }
static int allowed(MapView *m)
{
    const char *opt=getenv("CIV2_CAMERA_CACHE");
    return !painting && map_camera_owns(m) && hd_gpu_active && game.multiplayer<3 &&
        (!opt || *opt!='0');
}
static int minimum(int a,int b) { return a<b ? a : b; }
static int maximum(int a,int b) { return a>b ? a : b; }
static int wrap_delta(int x)
{
    if(!(game.rules & RULE_FLAT_WORLD)) {
        if(x>map.width/2)x-=map.width;
        if(x<-map.width/2)x+=map.width;
    }
    return x;
}
static void destroy(CachedView &e)
{
    if(e.view) {
        e.label_font.destroy();
        e.view->tile_font.destroy();e.view->tile_small_font.destroy();
        surface_dtor(&e.view->map_surface);surface_dtor(&e.view->overlay_surface);
        surface_dtor(e.view);free(e.view);
    }
    memset(&e,0,sizeof e);
}
void map_view_cache_clear(void)
{
    for(int i=0;i<SLOTS;i++)destroy(entries[i]);
}
static void check_key(MapView *m,int civ)
{
    int detail=map_camera_detail_zoom(m,m->zoom);
    if(key_detail!=detail) {
        /* A detail-band change affects pixels, not allocations or geometry. */
        for(int i=0;i<SLOTS;i++)memset(entries[i].valid,0,sizeof entries[i].valid);
        key_detail=detail;
    }
    int palette=((GamePalette *)&game_palette_block.game_palette)->get_version();
    if(key_width!=m->client_width || key_height!=m->client_height || key_density!=hd_density ||
       key_civ!=civ || key_hidden!=map_show_hidden_terrain || key_reveal!=game.reveal_map ||
       key_options!=(int)game.game_options || key_palette!=palette ||
       key_map_w!=(int)map.width || key_map_h!=(int)map.height || key_rules!=(int)game.rules) {
        map_view_cache_clear();
        key_width=m->client_width;key_height=m->client_height;key_density=hd_density;
        key_civ=civ;key_hidden=map_show_hidden_terrain;key_reveal=game.reveal_map;
        key_options=game.game_options;key_palette=palette;
        key_map_w=map.width;key_map_h=map.height;key_rules=game.rules;
    }
}
/* Explicitly keep both planes coherent. The old DibSurface blitter does not
 * support overlapping indexed copies; the GPU copy uses its own scratch target. */
static void copy_pixels(DibSurface *dst,DibSurface *src,int x,int y,int sx,int sy,int w,int h)
{
    if(w<=0 || h<=0)return;
    hd_copy((void *)dst->bits,(void *)src->bits,x,y,sx,sy,w,h);
    int first=0,end=h,step=1;
    if(dst==src && y>sy) { first=h-1;end=-1;step=-1; }
    for(int row=first;row!=end;row+=step)
        host_memory_move((void *)dst->pixel_ptr(x,y+row),(void *)src->pixel_ptr(sx,sy+row),w);
    hd_commit_rect((void *)dst->bits,x,y,w,h);
}
static RECT cell_rect(CachedView &e,int col,int row)
{
    RECT r={col*CELL,row*CELL,minimum((col+1)*CELL,e.view->client_width),
                            minimum((row+1)*CELL,e.view->client_height)};
    return r;
}
static int valid_rect(const CachedView &e,RECT r)
{
    if(r.left<0 || r.top<0 || r.right>e.view->client_width || r.bottom>e.view->client_height)return 0;
    for(int y=r.top/CELL;y<=(r.bottom-1)/CELL;y++)
        for(int x=r.left/CELL;x<=(r.right-1)/CELL;x++)if(!e.valid[y*e.cols+x])return 0;
    return 1;
}
static void crop(MapView *m,MapView *v,RECT &r)
{
    r.left=v->margin_x-m->margin_x+wrap_delta(m->origin_x-v->origin_x)*m->half_tile_width;
    r.top=v->margin_y-m->margin_y+(m->origin_y-v->origin_y)*m->half_tile_height;
    r.right=r.left+m->client_width;r.bottom=r.top+m->client_height;
}
static CachedView *acquire(MapView *m,int zoom,int civ)
{
    auto saved_scale=gfx_scale;
    ON_SCOPE_EXIT(gfx_set_scale_ratio(saved_scale.num,saved_scale.den));
    check_key(m,civ);
    int w=m->client_width+2*PAD,h=m->client_height+2*PAD;
    uint64_t bytes=(uint64_t)w*h*hd_density*hd_density*4;
    int slots=minimum(SLOTS,(int)((128u*1024*1024)/bytes));
    if(!slots || w*hd_density>4096 || h*hd_density>4096 ||
       ((w+CELL-1)/CELL)*((h+CELL-1)/CELL)>MAX_CELLS ||
       (slots<2 && zoom!=m->zoom))return 0;
    CachedView *e=0;
    for(int i=0;i<slots;i++)if(entries[i].view && entries[i].view->zoom==zoom) { e=&entries[i];break; }
    if(!e) {
        e=&entries[0];
        for(int i=1;i<slots;i++)if(entries[i].used<e->used)e=&entries[i];
        MapView *v=e->view;
        if(!v) {
            v=(MapView *)calloc(1,sizeof(MapView));
            if(!v)return 0;
            e->view=v;e->cols=(w+CELL-1)/CELL;e->rows=(h+CELL-1)/CELL;
            e->label_font.init_empty();e->label_font_size=-99;
            surface_ctor(v);surface_ctor(&v->map_surface);surface_ctor(&v->overlay_surface);
            v->tile_font.init_empty();v->tile_small_font.init_empty();
            v->DibSurface::create(w,h);v->apply_palette((GamePalette *)&game_palette_block.game_palette);
            v->map_view_index=~0u;v->client_width=w;v->client_height=h;
            SetRect((RECT *)&v->client_rect,0,0,w,h);v->cached_zoom=-999;
        }
        /* Recycle the allocation when evicting a zoom, instead of creating
         * another large GPU target and HD shadow in the input frame. */
        memset(e->valid,0,sizeof e->valid);e->civ=civ;v->zoom=(i16)zoom;
        v->center_x=m->center_x;v->center_y=m->center_y;
        v->update_layout();
    }
    e->used=++clock_used;
    return e;
}
static void recenter(CachedView &e,MapView *m)
{
    MapView *v=e.view;
    RECT r;crop(m,v,r);
    /* A centered whole-world image has a fixed blank border, not infinite
     * overscan. Keep its cut at the same world position as the live view. */
    if(!v->margin_x && !v->margin_y &&
       r.left>=0 && r.top>=0 && r.right<=v->client_width && r.bottom<=v->client_height)return;
    int ox=v->origin_x,oy=v->origin_y,mx=v->margin_x,my=v->margin_y;
    v->center_x=m->center_x;v->center_y=m->center_y;v->update_layout();
    int dx=wrap_delta(ox-v->origin_x)*v->half_tile_width+v->margin_x-mx;
    int dy=(oy-v->origin_y)*v->half_tile_height+v->margin_y-my;
    if(!dx && !dy)return;
    unsigned char valid[MAX_CELLS]={0};
    for(int y=0;y<e.rows;y++)for(int x=0;x<e.cols;x++) {
        RECT next=cell_rect(e,x,y),old=next;OffsetRect(&old,-dx,-dy);
        valid[y*e.cols+x]=(unsigned char)valid_rect(e,old);
        /* Blank margins and the cylinder's cut do not translate with terrain.
         * Repaint these cells; only reuse pixels safely inside both images.
         * The guard also covers sprites straddling the edge and repeated edge
         * tiles in the legacy whole-world traversal. Flat maps use no wrap. */
        int guard=v->tile_width+32;
        if((mx || v->margin_x) &&
           (old.left<mx+guard || old.right>mx+map.width*v->half_tile_width-guard ||
            next.left<v->margin_x+guard || next.right>v->margin_x+map.width*v->half_tile_width-guard))
            valid[y*e.cols+x]=0;
        if((my || v->margin_y) &&
           (old.top<my+guard || old.bottom>my+map.height*v->half_tile_height-guard ||
            next.top<v->margin_y+guard || next.bottom>v->margin_y+map.height*v->half_tile_height-guard))
            valid[y*e.cols+x]=0;
    }
    int x=maximum(dx,0),y=maximum(dy,0),sx=maximum(-dx,0),sy=maximum(-dy,0);
    copy_pixels(v,v,x,y,sx,sy,v->client_width-abs(dx),v->client_height-abs(dy));
    memcpy(e.valid,valid,sizeof valid);
}
/* The legacy unit/city painters use one global label font. Temporarily bind
 * this view's font without destroying either handle. No events are dispatched
 * while painting, and the second swap restores the live drawing state. */
static void swap_label_font(CachedView &e)
{
    GameFont *font=&status_window_block.map_label_font[0];
    GameFont saved=*font;*font=e.label_font;e.label_font=saved;
    int size=map_label_font_size;map_label_font_size=e.label_font_size;e.label_font_size=size;
}
static void paint_rect(CachedView &e,RECT r,int reference=0)
{
    MapView *v=e.view;
    auto saved_text=text_style;auto saved_draw=draw_target;
    auto saved_scale=gfx_scale;auto saved_target=gfx_draw_target;
    int saved_color=ctrl_colors.gfx_text;
    swap_label_font(e);
    painting=v;
    ON_SCOPE_EXIT(
        hd_gpu_text_end();v->reset_clip();painting=0;
        swap_label_font(e);
        text_style=saved_text;draw_target=saved_draw;gfx_draw_target=saved_target;
        ctrl_colors.gfx_text=saved_color;gfx_set_scale_ratio(saved_scale.num,saved_scale.den);
    );
    hd_gpu_text_begin();v->set_clip(&r);v->fill(10);
    if(reference) { v->draw_all_tiles(e.civ);return; }
    /* Include every tile that can overlap this rectangle, in the original
     * row order. Units/flags extend above their terrain diamond. */
    for(int row=0;row<(int)(v->field_2fc+v->field_304+1);row++) {
        int sy=v->margin_y+(row-1)*v->half_tile_height;
        /* Status fonts retain a minimum pixel height even at 1/8 zoom. */
        int pad=v->zoom<=-4 ? 32 : 16;
        if(sy+v->tile_height+pad<r.top || sy-v->half_tile_height-pad>=r.bottom)continue;
        int count=(row&1) ? v->half_view_width : v->tile_columns;
        for(int col=0;col<count;col++) {
            int x=map_wrap_x(v->origin_x+col*2+(row&1)),sx,draw_y;
            /* When a whole cylindrical world fits, the extra edge column
             * wraps onto the first column. Cull at the painter's actual
             * position so its repeated alpha layers remain identical. */
            v->tile_to_screen((iptr)&sx,(iptr)&draw_y,x,v->origin_y+row);
            if(sx+v->tile_width+pad<r.left || sx-pad>=r.right)continue;
            v->draw_tile(x,v->origin_y+row,e.civ);
        }
    }
    v->draw_city_names(0,0,-1);
    rendered+=(uint64_t)(r.right-r.left)*(r.bottom-r.top);
}
static int fill(CachedView &e,RECT required,uint64_t deadline)
{
    int count=0;
    for(int y=0;y<e.rows;y++)for(int x=0;x<e.cols;x++) {
        if(e.valid[y*e.cols+x])continue;
        RECT r=cell_rect(e,x,y),part;
        if(!IntersectRect(&part,&r,&required))continue;
        int right=x+1,bottom=y+1;
        if(!deadline) {
            while(right<e.cols && !e.valid[y*e.cols+right] && right*CELL<required.right)right++;
            for(;bottom<e.rows && bottom*CELL<required.bottom;bottom++) {
                int col=x;
                for(;col<right;col++)if(e.valid[bottom*e.cols+col])break;
                if(col<right)break;
            }
            r.right=minimum(right*CELL,e.view->client_width);
            r.bottom=minimum(bottom*CELL,e.view->client_height);
        }
        paint_rect(e,r);
        for(int row=y;row<bottom;row++)for(int col=x;col<right;col++) {
            e.valid[row*e.cols+col]=1;count++;
        }
        if(deadline && host_time_us()>=deadline)return count;
    }
    return count;
}
void map_view_cache_invalidate(void *view,int x,int y,int radius)
{
    if(painting || map_camera_busy() || !map_camera_owns(view))return;
    for(int i=0;i<SLOTS;i++) {
        CachedView &e=entries[i];MapView *v=e.view;
        if(!v)continue;
        if(radius<0) { memset(e.valid,0,sizeof e.valid);continue; }
        RECT dirty;v->tile_radius_rect(x,y,radius,(iptr)&dirty);
        for(int row=0;row<e.rows;row++)for(int col=0;col<e.cols;col++) {
            RECT r=cell_rect(e,col,row),part;
            if(IntersectRect(&part,&dirty,&r))e.valid[row*e.cols+col]=0;
        }
    }
}
int map_view_cache_draw(void *view,int civ)
{
    MapView *m=(MapView *)view;
    if(!allowed(m) || !map_camera_busy())return 0;
    uint64_t started=host_time_us();
    CachedView *e=acquire(m,m->zoom,civ);
    if(!e)return 0;
    recenter(*e,m);
    RECT r;crop(m,e->view,r);
    if(r.left<0 || r.top<0 || r.right>e->view->client_width || r.bottom>e->view->client_height)return 0;
    uint64_t ready=host_time_us();
    int count=fill(*e,r,0);
    uint64_t painted=host_time_us();
    if(getenv("CIV2_CAMERA_CACHE_VERIFY")) {
        uint32_t *before=hd_debug_capture_rect((void *)e->view->bits,r.left,r.top,m->client_width,m->client_height);
        /* Repaint with the unmodified full tile traversal, not the cache's
         * clipped/culling path, then compare the complete requested crop. */
        RECT all={0,0,(int)e->view->client_width,(int)e->view->client_height};
        paint_rect(*e,all,1);
        /* Clipping atlas triangles changes bilinear rounding. Use the same
         * three-level tolerance as the atlas regression; private textures
         * retain exact comparisons. This is only an opt-in diagnostic. */
        int matches=hd_debug_compare_rect((void *)e->view->bits,r.left,r.top,m->client_width,m->client_height,
                                         before,getenv("CIV2_GPU_NO_ATLAS") ? 0 : 3);
        free(before);
        if(!matches)plat_fatal("Viewport cache differs from a fresh render (zoom %d)",m->zoom);
        memset(e->valid,1,sizeof e->valid);
        plat_log("camera cache verified: zoom=%d\n",m->zoom);
    }
    if(count)misses++;else hits++;
    copy_pixels(m,e->view,m->client_x,m->client_y,r.left,r.top,m->client_width,m->client_height);
    reused+=(uint64_t)m->client_width*m->client_height;
    if(getenv("CIV2_CAMERA_TRACE"))plat_log("camera cache: hit=%llu miss=%llu painted=%llu copied=%llu cells=%d zoom=%d\n",
        hits,misses,rendered,reused,count,m->zoom);
    if(getenv("CIV2_CAMERA_TRACE"))plat_log("camera cache cost: setup=%llu paint=%llu copy=%llu us\n",
        ready-started,painted-ready,host_time_us()-painted);
    return 1;
}
static void prefetch(MapView *m,int zoom,int civ,uint64_t deadline)
{
    if(host_time_us()>=deadline || zoom<-7 || zoom>8 || zoom==m->zoom)return;
    CachedView *e=acquire(m,zoom,civ);
    if(!e)return;
    /* Compute geometry at the future zoom without borrowing ownership of any
     * masks/fonts. Matching the cached sizes makes update_layout geometry-only. */
    MapView target=*m;
    target.map_view_index=~0u;target.zoom=(i16)zoom;target.cached_zoom=zoom;
    int tile_h=gfx_zoom_scale(32,zoom);
    target.tile_font_size=tile_h*3/5;target.tile_small_font_size=tile_h/4;
    target.update_layout();recenter(*e,&target);
    RECT needed;crop(&target,e->view,needed);
    fill(*e,needed,deadline);
    if(host_time_us()>=deadline)return;
    RECT all={0,0,(int)e->view->client_width,(int)e->view->client_height};
    fill(*e,all,deadline);
}
void map_view_cache_idle(void *view,int zoom,int zooming)
{
    MapView *m=(MapView *)view;
    if(!allowed(m) || net.ui_input_busy || city_turn.processing)return;
    auto saved_scale=gfx_scale;
    ON_SCOPE_EXIT(gfx_set_scale_ratio(saved_scale.num,saved_scale.den));
    int civ=game.reveal_map ? -1 : human_civ[0];
    uint64_t deadline=host_time_us()+2000;
    if(zooming && zoom!=m->zoom) { prefetch(m,zoom,civ,deadline);return; }
    CachedView *e=acquire(m,m->zoom,civ);
    if(!e)return;
    recenter(*e,m);
    RECT all={0,0,(int)e->view->client_width,(int)e->view->client_height};
    fill(*e,all,deadline);
    prefetch(m,zoom,civ,deadline);
}

/* Opt-in replay regression: modify and restore rendered state through the
 * same partial-redraw boundary as gameplay, checking every retained zoom. */
void map_view_cache_probe(void)
{
    if(!getenv("CIV2_CAMERA_CACHE_VERIFY"))return;
    MapView *m=&map_views_block.map_view_recs[0];
    if(!allowed(m))return;
    auto verify=[]() {
        for(int i=0;i<SLOTS;i++)if(entries[i].view) {
            CachedView &e=entries[i];MapView *v=e.view;
            RECT all={0,0,(int)v->client_width,(int)v->client_height};
            fill(e,all,0);
            uint32_t *before=hd_debug_capture_rect((void *)v->bits,0,0,v->client_width,v->client_height);
            paint_rect(e,all,1);
            int matches=hd_debug_compare_rect((void *)v->bits,0,0,v->client_width,v->client_height,
                                             before,getenv("CIV2_GPU_NO_ATLAS") ? 0 : 3);
            free(before);
            if(!matches)plat_fatal("Viewport cache invalidation failed (zoom %d)",v->zoom);
            memset(e.valid,1,sizeof e.valid);
        }
    };
    verify();
    int x=m->center_x,y=m->center_y,civ=human_civ[0],units_checked=0,cities_checked=0;
    if(map_in_bounds(x,y)) {
        byte *tile=(byte *)tile_at(x,y);
        for(int index=0;index<=4;index+=4) {
            byte saved=tile[index];tile[index]^=index ? (1<<civ) : 0x80;
            m->redraw_area(x,y,2,civ,0);verify();
            tile[index]=saved;m->redraw_area(x,y,2,civ,0);verify();
        }
    }
    for(int i=0;i<game.num_units;i++)if(units[i].id && m->tile_in_view(units[i].x,units[i].y)) {
        byte saved=units[i].damage;units[i].damage=saved ? 0 : 1;
        m->redraw_area(units[i].x,units[i].y,0,civ,0);verify();
        units[i].damage=saved;m->redraw_area(units[i].x,units[i].y,0,civ,0);verify();
        units_checked++;break;
    }
    for(int i=0;i<game.num_cities;i++)if(cities[i].id && m->tile_in_view(cities[i].x,cities[i].y)) {
        auto saved=cities[i];cities[i].size=saved.size==1 ? 2 : 1;
        cities[i].revealed_size[civ]=cities[i].size;
        m->redraw_area(cities[i].x,cities[i].y,3,civ,0);verify();
        cities[i]=saved;m->redraw_area(cities[i].x,cities[i].y,3,civ,0);verify();
        cities_checked++;break;
    }
    plat_log("camera cache mutations verified: terrain/fog, %d unit, %d city\n",units_checked,cities_checked);
}

/* Replay diagnostic: antialiased terrain must erase a selection marker just
 * as completely as a fresh image, both on blink updates and on recentering. */
void map_redraw_probe(void)
{
    if(!hd_enabled || !getenv("CIV2_MAP_REDRAW_VERIFY"))return;
    MapView *m=&map_views_block.map_view_recs[0];
    auto saved_blink=blink;auto saved_cursor=map_cursor;
    auto saved_mode=moving_units_mode;auto saved_turn=player_turn_active;
    hd_redraw_begin();
    ON_SCOPE_EXIT(blink=saved_blink;map_cursor=saved_cursor;moving_units_mode=saved_mode;
                  player_turn_active=saved_turn;hd_redraw_end());
    player_turn_active=0;moving_units_mode=0;blink.unit_active=1;blink.unit_phase=0;
    int x=m->center_x,y=m->center_y;
    map_cursor.x[0]=(i16)x;map_cursor.y[0]=(i16)y;
    RECT r;m->tile_radius_rect(x,y,1,(iptr)&r);
    m->set_clip((RECT *)&m->client_rect);m->fill(10);m->draw_all_tiles(human_civ[0]);
    uint32_t *clean=hd_debug_capture_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top);
    int tolerance=hd_gpu_active && !getenv("CIV2_GPU_NO_ATLAS") ? 3 : 0;
    for(int pass=0;pass<3;pass++) {
        blink.unit_phase=1;m->redraw_cursor_tile();
        blink.unit_phase=0;
        if(pass==0)m->redraw_cursor_tile();
        if(pass==1) {
            map_set_cursor_pos(map_wrap_x(x+2),y);
            blink.unit_phase=0;m->redraw_cursor_tile();
        }
        if(pass==2)m->draw_all_tiles(human_civ[0]);
        int matches=hd_debug_compare_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top,clean,tolerance);
        if(!matches)plat_fatal("Map redraw retains selection pixels (pass %d, zoom %d)",pass,m->zoom);
    }
    free(clean);
    /* Movement/revealing terrain repaints a larger radius than cursor blinks.
     * Check the whole dirty rectangle, including its formerly unpainted upper
     * corners, against the full render left by the last pass. */
    for(int radius=1;radius<=3;radius++) {
        m->tile_radius_rect(x,y,radius,(iptr)&r);
        clean=hd_debug_capture_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top);
        for(int repeat=0;repeat<2;repeat++) {
            m->redraw_area(x,y,radius,human_civ[0],0);
            int matches=hd_debug_compare_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top,clean,tolerance);
            if(!matches)plat_fatal("Map radius redraw differs (radius %d, zoom %d)",radius,m->zoom);
        }
        free(clean);
    }
    /* A transport's second shield projects outside its 64x48 sprite. Slide a
     * temporary trireme over the real map without placing it in a tile stack;
     * every pixel must return to the clean background after the animation. */
    auto saved_dummy=dummy_unit;auto saved_combat=combat_anim;
    auto saved_options=game.game_options;
    ON_SCOPE_EXIT(dummy_unit=saved_dummy;combat_anim=saved_combat;game.game_options=saved_options);
    game.game_options|=OPT_FAST_PIECE_SLIDE;
    int unit=unit_make_dummy(UNIT_TRIREME,human_civ[0]);
    m->tile_radius_rect(x,y,2,(iptr)&r);
    clean=hd_debug_capture_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top);
    for(int stacked=0;stacked<=1;stacked++)for(int dir=0;dir<8;dir++) {
        dummy_unit.prev_in_stack=stacked ? 0 : -1;
        if(dir==0) {
            int sx,sy;m->tile_to_screen((iptr)&sx,(iptr)&sy,x,y);
            gfx_draw_unit((iptr)m,unit,5,sx,sy-m->half_tile_height,m->zoom,1);
            m->redraw_area(x,y,0,human_civ[0],0);
            int matches=hd_debug_compare_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top,clean,tolerance);
            if(!matches)plat_fatal("Unit redraw leaves shield pixels (stacked %d, zoom %d)",stacked,m->zoom);
        }
        map_animate_unit_move(unit,x,y,dir,-1,-1);
        int matches=hd_debug_compare_rect((void *)m->bits,r.left,r.top,r.right-r.left,r.bottom-r.top,clean,tolerance);
        if(!matches)plat_fatal("Unit animation leaves pixels (stacked %d, direction %d, zoom %d)",stacked,dir,m->zoom);
    }
    free(clean);
    plat_log("unit animation verified: single/stacked triremes in all 8 directions at zoom %d\n",m->zoom);
    plat_log("map redraw verified: blink, selection move, movement radii 1-3, full redraw at zoom %d\n",m->zoom);
}
