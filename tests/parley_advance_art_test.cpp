// SOURCE src/auto/network.cpp parley_draw_advance_list
// Actual trade-list painter with real window layout, replacing drawing only.
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
using uint=unsigned;
using byte=unsigned char;
using ushort=unsigned short;
#include "civ2classes.h"
static_assert(sizeof(void *)==4,"legacy window layout");
static ParleyWindow parley_wnd_block;
static Control hotspot;
static GameFont font_size14,font_size21;
static struct { Sprite advance_category_icon_sprites[20]; } government_sprites_block;
static struct { int name;char category,epoch; } advances[100];
static struct { int margin_x=10,margin_y=2; } panel_layout;
static struct { int list_bg=1,list_sel_bg=2,list_text=3,list_shadow=3,sel_text_color=4,sel_text_shadow_color=4; } dlg_style;
static struct { char buf[80]; } textbuf;
static int hd_enabled,fallbacks;
static std::vector<std::vector<int>> icons,labels;
static int wnd_get_active_child() { return (int)&parley_wnd_block+0x48; }
static int rect_width(const RECT *r) { return r->right-r->left; }
static int rect_height(const RECT *r) { return r->bottom-r->top; }
static BOOL SetRect(RECT *r,int l,int t,int rr,int b) { *r={l,t,rr,b};return 1; }
void DibSurface::get_clip(RECT *r) { memcpy(r,&clip_left,sizeof *r); }
void DibSurface::set_clip(RECT *r) { memcpy(&clip_left,r,sizeof *r); }
void DibSurface::fill_rect(iptr,i32) {}
void DibSurface::draw_text(iptr,const char *s,iptr x,int y,uint) { labels.push_back({atoi(s),(int)x,y}); }
void ParleyWindow::update_buttons() {}
void Control::show_and_redraw() {}
u32 GameFont::font_cached_height() { return this==&font_size14 ? 14 : 21; }
i32 GameFont::text_width(const char *) { return 24; }
i32 Sprite::get_width() { return 36; }
i32 Sprite::get_height() { return 20; }
iptr Sprite::draw(iptr out,iptr,int,int) { ++fallbacks;return out; }
static void gfx_set_text_color_index(int) {}
static void text_clear() { textbuf.buf[0]=0; }
static void text_append_label(int) {}
static const char *label_string(int id) { static char s[32];snprintf(s,sizeof s,"%d",id);return s; }
static void text_append(const char *s) { strcpy(textbuf.buf,s); }
static bool hd_advance(DibSurface *s,int id,int x,int y,int w,int h) {
    assert(s==(DibSurface *)&parley_wnd_block && w==36 && h==20);
    assert(x>=(int)s->clip_left && x+w<=(int)s->clip_right && y>=(int)s->clip_top && y+h<=(int)s->clip_bottom);
    icons.push_back({id,x,y});return hd_enabled && id!=81;
}
#include "production.inc"
int main()
{
    const int ids[]={0,1,81,42,7};
    for(int id:ids)advances[id].name=id;
    auto &v=parley_wnd_block;
    for(int pane=0;pane<2;pane++)for(int large=0;large<2;large++)for(int scroll=0;scroll<=2;scroll++) {
        v.large_layout=large;v.list_counts[pane]=5;v.list_rows[pane]=2;v.list_first_row[pane]=scroll;
        v.list_hotspots[pane]=(u32)&hotspot;
        RECT clip={0,0,800,600},r={10+pane*400,30,310+pane*400,120};
        memcpy(&v.list_rects[pane*4],&r,sizeof r);v.set_clip(&clip);
        for(int i=0;i<5;i++)v.list_items[pane*0x801+i]=ids[i];
        std::vector<std::vector<int>> before_icons,before_labels;
        for(hd_enabled=0;hd_enabled<=1;hd_enabled++) {
            icons.clear();labels.clear();fallbacks=0;parley_draw_advance_list(pane);
            assert(icons.size()==3 && labels.size()==3);
            for(int row=0;row<3;row++)assert(icons[row][0]==ids[scroll+row] && labels[row][0]==ids[scroll+row]);
            assert(fallbacks==(hd_enabled ? 1 : 3)); // Each visible set includes missing-art advance 81.
            if(!hd_enabled) { before_icons=icons;before_labels=labels; }
            else { assert(icons==before_icons && labels==before_labels); }
            RECT after;v.get_clip(&after);assert(!memcmp(&clip,&after,sizeof clip));
        }
    }
    puts("Technology trade lists: both panes/layouts, scrolling, per-advance IDs, fallback and clip restoration passed.");
}
