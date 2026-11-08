/* Optional GPU camera. A bounded viewport cache supplies backing refreshes;
 * legacy draw coordinates, picking masks and animation captures stay intact. */
#include <math.h>
#include "game.h"
#include "../platform/hd.h"
EXTERN_C void plat_log(const char *,...);

static struct {
    MapView *view;
    HWND hwnd;
    MapCameraTransform transform;
    double scale,target,cx,cy,anchor_x,anchor_y,world_x,world_y;
    double drag_x,drag_y;
    POINT start;
    DWORD tick;
    int zoom,center_x,center_y,width,height,map_w,map_h,civ;
    int button,moved,double_click,refreshing,animating,dirty,prefetch,detail;
} camera;

static int camera_option=-1;
int map_camera_option(void)
{
    if(camera_option<0)camera_option=GetPrivateProfileIntA("OpenCiv2","GpuMapCamera",0,"CIV.INI")!=0;
    return camera_option;
}
static int enabled(void)
{
    if(!hd_gpu_active || !turn_loop.active || map.width<1 || map.height<1)return 0;
    return map_camera_option();
}
static HWND view_hwnd(MapView *m)
{
    NativeWindow *w=((WndBase *)((i8 *)m+0x48))->native_wnd;
    return w ? w->hwnd : 0;
}
int map_camera_active(void) { return enabled() && camera.view!=0; }
int map_camera_owns(void *view) { return enabled() && camera.view==view; }
int map_camera_surface(void *surface)
{
    return enabled() && (camera.view==surface || map_view_cache_surface(surface));
}
int map_camera_detail_zoom(void *surface,int zoom)
{
    if(!map_camera_surface(surface))return zoom;
    /* Detail depends on the displayed scale, not the deliberately coarser
     * source image or graphics density. Only these three bands change layers. */
    return camera.scale>=.625 ? 0 : camera.scale>=.5 ? -4 : -5;
}
int map_camera_topology_zoom(void *surface,int zoom)
{
    /* Essential geography and health remain visible at low resolution.
     * Decorative terrain layers use their ordinary detail cutoffs. */
    return map_camera_surface(surface) && zoom<0 ? 0 : zoom;
}
int map_camera_backing_zoom(double scale,int width,int height)
{
    /* Re-centering is quantized to diamond-lattice coordinates. Reserve a
     * whole lattice step plus 32 display pixels on either side, so choosing
     * another backing origin cannot force the camera itself to jump. */
    double rx=1+(64*scale+64)/width,ry=1+(32*scale+64)/height;
    int zoom=(int)floor(scale*8/(rx>ry ? rx : ry)+1e-8)-8;
    return zoom<-7 ? -7 : zoom>8 ? 8 : zoom;
}
static double clamp(double v,double lo,double hi) { return v<lo ? lo : v>hi ? hi : v; }
static double horizontal_center(double center,double scale,int width)
{
    if(!(game.rules & RULE_FLAT_WORLD))return center;
    /* World coordinates run from the left tip of the first diamond to the
     * right tip of the last. Raster margins are not extra scrollable land. */
    double extent=map.width+1,half=width/(64*scale);
    return extent<=2*half ? extent*.5 : clamp(center,half,extent-half);
}
static double source_x(MapView *m,double px)
{ return m->origin_x+1+(px-m->client_x-m->margin_x)/(double)m->half_tile_width; }
static double source_y(MapView *m,double py)
{ return m->origin_y+1+(py-m->client_y-m->margin_y)/(double)m->half_tile_height; }

static void transform(void)
{
    MapView *m=camera.view;
    double base=m->tile_width/64.0,reference=source_x(m,m->client_x+m->client_width*.5);
    MapCameraTransform *c=&camera.transform;
    /* Choose the same copy of the cylindrical world as the backing surface. */
    if(!(game.rules & RULE_FLAT_WORLD) && map.width>0)
        camera.cx+=floor((reference-camera.cx)/map.width+.5)*map.width;
    c->left=m->client_x;c->top=m->client_y;c->width=m->client_width;c->height=m->client_height;
    c->scale=camera.scale/base;
    c->x=c->width*.5-(m->margin_x+(camera.cx-m->origin_x-1)*m->half_tile_width)*c->scale;
    c->y=c->height*.5-(m->margin_y+(camera.cy-m->origin_y-1)*m->half_tile_height)*c->scale;
}
static void remember(void)
{
    MapView *m=camera.view;
    camera.zoom=m->zoom;camera.center_x=m->center_x;camera.center_y=m->center_y;
    camera.width=m->client_width;camera.height=m->client_height;
    camera.map_w=map.width;camera.map_h=map.height;camera.civ=human_civ[0];
    camera.detail=map_camera_detail_zoom(m,m->zoom);
}
static void cancel_drag(void)
{
    camera.button=0;camera.moved=0;
    if(camera.hwnd && GetCapture()==camera.hwnd)ReleaseCapture();
}
void map_camera_cancel(void) { cancel_drag();camera.animating=0;camera.target=camera.scale; }
int map_camera_busy(void) { return camera.refreshing; }
void map_camera_forget(void *view)
{
    if(camera.view!=view)return;
    map_view_cache_clear();
    cancel_drag();memset(&camera,0,sizeof camera);
}
/* The options page persists the setting and redraws after applying all changes. */
void map_camera_set_option(int on)
{
    on=on!=0;
    if(on==map_camera_option())return;
    if(camera.view) {
        MapView *m=camera.view;
        /* Return to the closest fixed zoom, not the camera's coarser backing
         * zoom. Keep the displayed map center when discarding its transform. */
        m->zoom=(i16)clamp(floor(camera.scale*8+.5)-8,-7,8);
        int x=(int)floor(camera.cx),y=(int)clamp(floor(camera.cy),0,map.height-1);
        if(game.rules & RULE_FLAT_WORLD)x=(int)clamp(x,0,map.width-1);
        if((x+y)&1)x+=(game.rules & RULE_FLAT_WORLD) && x==map.width-1 ? -1 : 1;
        m->center_x=(i16)map_wrap_x(x);m->center_y=(i16)y;
        map_camera_forget(m);
    }
    camera_option=on;
}
void map_camera_layout(void *view)
{
    MapView *m=(MapView *)view;
    if(!enabled() || m->map_view_index!=0 || !m->half_tile_width || !m->half_tile_height ||
       m->client_width<=0 || m->client_height<=0)return;
    HWND hwnd=view_hwnd(m);
    if(!hwnd)return;
    if(!camera.refreshing) {
        bool fresh=camera.view!=m || camera.hwnd!=hwnd || camera.zoom!=m->zoom ||
            camera.width!=m->client_width || camera.height!=m->client_height ||
            camera.map_w!=(int)map.width || camera.map_h!=(int)map.height || camera.civ!=human_civ[0];
        if(fresh || camera.center_x!=m->center_x || camera.center_y!=m->center_y) {
            if(fresh)map_view_cache_clear();
            cancel_drag();camera.animating=0;camera.dirty=fresh;
            if(fresh) { camera.scale=(m->zoom+8)/8.0;camera.prefetch=-1; }
            camera.target=camera.scale;
            camera.cx=source_x(m,m->client_x+m->client_width*.5);
            camera.cy=source_y(m,m->client_y+m->client_height*.5);
        }
    }
    camera.view=m;camera.hwnd=hwnd;remember();transform();
}
int map_camera_pick(void *view,int *x,int *y)
{
    if(!enabled() || camera.view!=view)return 1;
    return camera_pick(&camera.transform,x,y);
}
uintptr_t map_camera_get(MapCameraTransform *out)
{
    if(!enabled() || !camera.view || !IsWindow(camera.hwnd) || !IsWindowVisible(camera.hwnd))return 0;
    *out=camera.transform;
    return camera.hwnd;
}
void *map_camera_pixels(void)
{
    return enabled() && camera.view ? (void *)camera.view->bits : 0;
}
int map_camera_labels(MapCameraLabel *labels,int capacity,double *height,int sizes)
{
    if(!enabled() || !camera.view || map_show_hidden_terrain)return 0;
    if(sizes && camera.scale<.625)return 0;
    MapView *m=camera.view;
    const MapCameraTransform &c=camera.transform;
    int count=0,civ=human_civ[0];
    *height=sizes ? 11*camera.scale : clamp(32*camera.scale*3/5,6,48);
    for(int city=0;city<game.num_cities && count<capacity;city++) {
        if(!cities[city].id || !m->tile_in_view(cities[city].x,cities[city].y))continue;
        int visible=tile_visible_to(cities[city].x,cities[city].y,civ);
        if(cities[city].owner!=civ)
            visible=visible && (cities[city].known_to & (1<<civ)) && cities[city].revealed_size[civ];
        int scenario_sizes=sizes && (game.game_flags & GAME_SCENARIO) && (scenario.flags[0] & 8);
        if(!visible && !game.reveal_map && !scenario_sizes)continue;
        if(sizes && (cities[city].flags & CITYF_DISORDER))continue;
        int x,y;m->tile_to_screen((iptr)&x,(iptr)&y,cities[city].x,cities[city].y);
        MapCameraLabel &label=labels[count++];
        label.flags=0;
        if(sizes) {
            int size,index=gfx_city_sprite_index(city,0,&size);
            label.x=c.x+(x-m->client_x)*c.scale+
                road_sprites_block.city_size_box_x_offsets[index]*camera.scale;
            label.y=c.y+(y-m->client_y-m->half_tile_height)*c.scale+
                road_sprites_block.city_label_y_offsets[index]*camera.scale;
            label.color=civ_color_primary(cities[city].owner);
            snprintf(label.text,sizeof label.text,"%d",size);
        } else {
            label.x=c.x+(x-m->client_x+m->half_tile_width)*c.scale;
            label.y=c.y+(y-m->client_y+m->tile_height*.75)*c.scale;
            label.color=civ_color_secondary(cities[city].owner);
            snprintf(label.text,sizeof label.text,"%s",cities[city].name);
        }
    }
    return count;
}

int map_camera_unit_labels(MapCameraLabel *labels,int capacity,double *height)
{
    if(!enabled() || !camera.view || map_show_hidden_terrain || camera.scale<.625)return 0;
    MapView *m=camera.view;
    const MapCameraTransform &c=camera.transform;
    int count=0;
    *height=11*camera.scale;
    for(int i=0;i<=game.num_units && count<capacity;i++) {
        int tx,ty,first=i;
        if(i<game.num_units) {
            if(!units[i].id || units[i].prev_in_stack>=0)continue;
            tx=units[i].x;ty=units[i].y;
        } else {
            /* Combat can retain a sprite at an origin whose unit has moved. */
            if(combat_anim.origin_stack_unit<0)continue;
            tx=combat_anim.origin_x;ty=combat_anim.origin_y;
            first=tile_first_unit(tx,ty);
            if(first>=0)continue; // Its stack was already visited above.
        }
        if(!m->tile_in_view(tx,ty))continue;
        uint flags;
        int unit=map_stack_unit(map_tile_unit(tx,ty,first),&flags);
        if(unit<0)continue;
        if(flags & 2)unit=(int)unit_stack_pick_display_unit(unit);
        if(unit<0)continue;
        int x,y;m->tile_to_screen((iptr)&x,(iptr)&y,tx,ty);
        y-=m->half_tile_height;
        int type=units[unit].type;
        if(type==UNIT_DIPLOMAT && units[unit].owner==0)type=0x3e;
        int ch=gfx_unit_order_char(unit);
        if(ch) {
            /* Match the actual shield rectangle, including backing-grid rounding,
             * while the glyph's size follows only the displayed camera scale. */
            int sx=x+gfx_zoom_scale(road_sprites_block.unit_shield_x_offsets[type],m->zoom);
            int sy=y+gfx_zoom_scale(road_sprites_block.unit_shield_y_offsets[type],m->zoom);
            int top=sy+2*gfx_zoom_scale(2,m->zoom)+gfx_zoom_scale(3,m->zoom);
            int bottom=sy+gfx_zoom_scale(20,m->zoom)-gfx_zoom_scale(3,m->zoom);
            if(crt_toupper(ch)=='M') { top-=gfx_zoom_scale(1,m->zoom);bottom-=gfx_zoom_scale(1,m->zoom); }
            MapCameraLabel &label=labels[count++];
            label.flags=1;
            label.left=c.x+(sx-m->client_x)*c.scale;
            label.right=label.left+gfx_zoom_scale(12,m->zoom)*c.scale;
            label.top=c.y+(top-m->client_y)*c.scale;
            label.bottom=c.y+(bottom-m->client_y)*c.scale;
            label.x=(label.left+label.right)*.5;label.y=(label.top+label.bottom)*.5;
            label.color=units[unit].flags & UNITF_AUTOMATED ? 0x1d : 10;
            label.text[0]=(char)ch;label.text[1]=0;
        }
        if(count<capacity && extra_ui(X_WORK_COUNTER) && unit_types[units[unit].type].role==ROLE_SETTLE &&
           units[unit].counter && (units[unit].owner==human_civ[0] || game.reveal_map)) {
            MapCameraLabel &label=labels[count++];
            label.flags=2;
            label.x=c.x+(x-m->client_x)*c.scale+32*camera.scale;
            label.y=c.y+(y-m->client_y)*c.scale+40*camera.scale;
            label.color=0x72;
            snprintf(label.text,sizeof label.text,"%d",units[unit].counter);
        }
    }
    return count;
}

static void coverage(void)
{
    MapView *m=camera.view;
    MapCameraTransform *c=&camera.transform;
    camera.cx=horizontal_center(camera.cx,camera.scale,camera.width);
    const int saved_click=map_click_in_progress;
    ON_SCOPE_EXIT(camera.refreshing=0;map_click_in_progress=saved_click);
    int zoom=map_camera_backing_zoom(camera.scale,camera.width,camera.height);
    /* Don't oscillate between adjacent source levels around a wheel
     * threshold. Larger changes still upgrade during the animation so a
     * long zoom-in doesn't end with a sudden jump from a very coarse source. */
    if(zoom==m->zoom+1)zoom=m->zoom;
    transform();
    if(m->zoom!=zoom || camera.detail!=map_camera_detail_zoom(m,m->zoom) ||
       c->x>0 || c->y>0 || c->x+c->width*c->scale<c->width ||
       c->y+c->height*c->scale<c->height) {
        int x=(int)floor(camera.cx),y=(int)floor(camera.cy);
        y=(int)clamp(y,0,map.height-1);if((x+y)&1)x++;
        if(game.rules & RULE_FLAT_WORLD)x=(int)clamp(x,0,map.width-1);
        x=map_wrap_x(x);
        int keep=map_click_in_progress;
        DWORD started=timeGetTime();
        camera.refreshing=1;map_click_in_progress=1;
        m->zoom=(i16)zoom;
        /* The legacy center is not the exact viewport midpoint: row parity
         * and ceil(tile counts) shift it by up to two lattice coordinates.
         * Choose a valid backing center around the desired camera center. */
        double best=1e30;int best_x=x,best_y=y;
        for(int yy=y-2;yy<=y+2;yy++)for(int xx=x-2;xx<=x+2;xx++) {
            if(((xx+yy)&1) || yy<0 || yy>=(int)map.height ||
               ((game.rules & RULE_FLAT_WORLD) && (xx<0 || xx>=(int)map.width)))continue;
            m->center_x=(i16)map_wrap_x(xx);m->center_y=(i16)yy;m->update_layout();
            double ex=source_x(m,m->client_x+m->client_width*.5)-camera.cx;
            if(!(game.rules & RULE_FLAT_WORLD))ex-=floor(ex/map.width+.5)*map.width;
            double ey=source_y(m,m->client_y+m->client_height*.5)-camera.cy;
            double error=ex*ex+ey*ey;
            if(error<best) { best=error;best_x=m->center_x;best_y=m->center_y; }
        }
        m->center_x=(i16)best_x;m->center_y=(i16)best_y;
        m->update_layout();
        if(map_view_cache_draw(m,game.reveal_map ? -1 : human_civ[0])) {
            /* A camera refresh changes map pixels and the minimap footprint,
             * not window layout/captions/buttons. Keep the regular backing
             * surface current for picking, blinks and animation captures. */
            m->set_clip((RECT *)&m->client_rect);
            if(extra_ui(X_SUPPRESS_POPUPS))extras_draw_map_messages((iptr)m,0);
            m->wnd_blit_surface_rect((iptr)&m->client_rect);
            minimap_redraw();
        } else m->redraw(human_civ[0],1);
        map_click_in_progress=keep;
        remember();transform();
        if(getenv("CIV2_CAMERA_TRACE"))plat_log("camera refresh: %u ms base=%d\n",
            (unsigned)(timeGetTime()-started),m->zoom);
    }
    /* Presentation still samples the live map crop. Respect its coverage and
     * the original layout's flat-map/pole/small-world bounds. */
    double nx=clamp(c->x,c->width*(1-c->scale),0),ny=clamp(c->y,c->height*(1-c->scale),0);
    if(getenv("CIV2_CAMERA_TRACE") && (fabs(c->x-nx)>.001 || fabs(c->y-ny)>.001))
        plat_log("camera clamp: %.6f,%.6f display pixels\n",c->x-nx,c->y-ny);
    camera.cx+=(c->x-nx)/(32*camera.scale);
    camera.cy+=(c->y-ny)/(16*camera.scale);
    c->x=nx;c->y=ny;
    camera.refreshing=0;
}
void map_camera_tick(void)
{
    if(!enabled() || !camera.view || camera.refreshing || net.map_redraw_busy)return;
    if(!IsWindow(camera.hwnd)) { map_camera_forget(camera.view);return; }
    ON_SCOPE_EXIT(if(IsWindowEnabled(camera.hwnd))
        map_view_cache_idle(camera.view,camera.animating ?
            map_camera_backing_zoom(camera.target,camera.width,camera.height) :
            camera.view->zoom+(camera.button ? 0 : camera.prefetch),camera.animating));
    if(camera.button && (GetCapture()!=camera.hwnd || !IsWindowEnabled(camera.hwnd)))cancel_drag();
    if(camera.dirty) { coverage();camera.dirty=0; }
    if(!camera.animating || !IsWindowEnabled(camera.hwnd))return;
    DWORD now=timeGetTime(),elapsed=now-camera.tick;
    if(!elapsed)return;
    camera.tick=now;
    /* Time-based easing also runs on idle frames, independent of wheel rate. */
    double a=1-exp(-clamp(elapsed,0,1000)/35.0);
    camera.scale+=a*(camera.target-camera.scale);
    if(fabs(camera.target-camera.scale)<.0001) {
        camera.scale=camera.target;camera.animating=0;
    }
    camera.cx=camera.world_x-(camera.anchor_x-camera.width*.5)/(32*camera.scale);
    camera.cy=camera.world_y-(camera.anchor_y-camera.height*.5)/(16*camera.scale);
    coverage();
}
int map_camera_zoom_step(void *view,int step)
{
    if(!enabled() || camera.view!=view)return 0;
    camera.anchor_x=camera.width*.5;camera.anchor_y=camera.height*.5;
    camera.world_x=camera.cx;camera.world_y=camera.cy;
    camera.target=step ? clamp((step>0 ? floor(camera.target*8+1e-6)+1 :
                               ceil(camera.target*8-1e-6)-1)/8,.125,2) : 1;
    camera.prefetch=camera.target>camera.scale ? 2 : -1;
    camera.tick=timeGetTime();camera.animating=1;return 1;
}
int map_camera_input(void *message)
{
    MSG *m=(MSG *)message;
    if(!enabled() || !camera.view || camera.refreshing)return 0;
    if(m->message==WM_KILLFOCUS || m->message==WM_CAPTURECHANGED || m->message==WM_CANCELMODE) {
        if(m->hwnd==camera.hwnd)cancel_drag();return 0;
    }
    if(m->message!=WM_MOUSEWHEEL && m->message!=WM_LBUTTONDOWN && m->message!=WM_LBUTTONUP &&
       m->message!=WM_MBUTTONDOWN && m->message!=WM_MBUTTONUP && m->message!=WM_MOUSEMOVE &&
       m->message!=WM_LBUTTONDBLCLK && m->message!=WM_MBUTTONDBLCLK)return 0;
    if(getenv("CIV2_CAMERA_TRACE"))plat_log("camera input %x hwnd=%x map=%x enabled=%d busy=%d city=%d drop=%d\n",
        m->message,(unsigned)m->hwnd,(unsigned)camera.hwnd,IsWindowEnabled(camera.hwnd),net.ui_input_busy,
        city_turn.processing,ui_paradrop_cursor_active());
    if(camera.button && GetCapture()!=camera.hwnd)cancel_drag();
    if(!IsWindowEnabled(camera.hwnd) || city_turn.processing || net.ui_input_busy ||
       ui_paradrop_cursor_active()) { cancel_drag();return 0; }
    /* Messages may queue while a source refresh runs. The live cursor can
     * already be many events ahead of this down/move/up; use the event's
     * coordinates, especially for the drag origin and zoom anchor. */
    POINT pt={(short)LOWORD(m->lParam),(short)HIWORD(m->lParam)};
    HWND under=m->hwnd;
    if(m->message==WM_MOUSEWHEEL) {
        under=plat_window_at_point(pt.x,pt.y);
        ScreenToClient(camera.hwnd,&pt);
    } else if(m->hwnd!=camera.hwnd)return 0;
    MapCameraTransform *c=&camera.transform;
    int inside=under==camera.hwnd && pt.x>=c->left && pt.y>=c->top &&
        pt.x<c->left+c->width && pt.y<c->top+c->height;
    if(getenv("CIV2_CAMERA_TRACE"))plat_log("camera under=%x point=%d,%d viewport=%d,%d,%d,%d inside=%d\n",
        (unsigned)under,pt.x,pt.y,c->left,c->top,c->width,c->height,inside);
    if(m->message==WM_MOUSEWHEEL && inside) {
        int delta=(short)HIWORD(m->wParam);
        if(!delta)return 1;
        camera.anchor_x=pt.x-c->left;camera.anchor_y=pt.y-c->top;
        camera.world_x=camera.cx+(camera.anchor_x-camera.width*.5)/(32*camera.scale);
        camera.world_y=camera.cy+(camera.anchor_y-camera.height*.5)/(16*camera.scale);
        camera.target=clamp(camera.target*pow(1.15,clamp(delta/120.0,-20,20)),.125,2);
        camera.prefetch=delta>0 ? 2 : -1;
        camera.tick=timeGetTime();camera.animating=1;
        return 1;
    }
    if(m->message==WM_MBUTTONDOWN && inside && (m->wParam & MK_CONTROL))
        return map_camera_zoom_step(camera.view,0);
    if((m->message==WM_LBUTTONDOWN || m->message==WM_MBUTTONDOWN ||
        m->message==WM_LBUTTONDBLCLK || m->message==WM_MBUTTONDBLCLK) && inside &&
       !(m->wParam & (MK_SHIFT|MK_CONTROL))) {
        /* A quick repeat drag can begin with DBLCLK. Defer its legacy action
         * until release, just like a single click, so it cannot open a city
         * while the user is trying to pan again. */
        camera.animating=0;camera.target=camera.scale;
        mapview_cancel_drag();
        camera.button=(m->message==WM_LBUTTONDOWN || m->message==WM_LBUTTONDBLCLK) ? 1 : 2;
        camera.double_click=m->message==WM_LBUTTONDBLCLK || m->message==WM_MBUTTONDBLCLK;
        camera.moved=0;
        camera.start=pt;camera.drag_x=camera.cx;camera.drag_y=camera.cy;
        SetCapture(camera.hwnd);return 1;
    }
    if(m->message==WM_MOUSEMOVE && camera.button) {
        if(!(m->wParam & (camera.button==1 ? MK_LBUTTON : MK_MBUTTON))) { cancel_drag();return 1; }
        int dx=pt.x-camera.start.x,dy=pt.y-camera.start.y;
        if(dx*dx+dy*dy>=36)camera.moved=1;
        if(camera.moved) {
            camera.cx=camera.drag_x-dx/(32*camera.scale);
            camera.cy=camera.drag_y-dy/(16*camera.scale);
            camera.dirty=1;
        }
        return 1;
    }
    if((m->message==WM_LBUTTONUP && camera.button==1) ||
       (m->message==WM_MBUTTONUP && camera.button==2)) {
        int click=!camera.moved && inside,button=camera.button,double_click=camera.double_click;
        cancel_drag();
        if(click && button==1) {
            /* The legacy window records button-down before dispatching a
             * click. Deliver the pair only after we know this wasn't a pan. */
            LPARAM pos=MAKELPARAM(pt.x,pt.y);
            SendMessageA(camera.hwnd,double_click ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN,m->wParam|MK_LBUTTON,pos);
            SendMessageA(camera.hwnd,WM_LBUTTONUP,m->wParam,pos);
        }
        return 1;
    }
    return 0;
}

/* Replay diagnostics use the real game's mask-based picker, without orders. */
void map_camera_boundary_test(int flat)
{
    if(!getenv("CIV2_CAMERA_BOUNDARY_TEST"))return;
    MapView *m=&map_views_block.map_view_recs[0];
    if(flat)game.rules|=RULE_FLAT_WORLD;else game.rules&=~RULE_FLAT_WORLD;
    map_camera_forget(m);
    m->redraw(human_civ[0],1);
    plat_log("camera boundary mode: flat=%d map=%d,%d\n",flat,map.width,map.height);
}
void map_camera_probe(int x,int y)
{
    map_camera_tick();
    MapView *m=(MapView *)&map_views_block.map_views;
    POINT p={x,y};ScreenToClient(view_hwnd(m),&p);
    int tx=-1,ty=-1,sx=p.x,sy=p.y;
    int ok=map_camera_pick(m,&sx,&sy);
    int result=m->screen_to_tile((iptr)&tx,(iptr)&ty,p.x,p.y);
    plat_log("camera probe %d %d: result=%d tile=%d,%d source=%d,%d scale=%.6f base=%d mapped=%d cursor=%d,%d\n",
             x,y,result,tx,ty,sx,sy,camera.scale,m->zoom,ok,(int)map_cursor.x[0],(int)map_cursor.y[0]);
    plat_log("camera world: %.9f,%.9f\n",
        source_x(m,camera_source_x(&camera.transform,p.x)),
        source_y(m,camera_source_y(&camera.transform,p.y)));
}
