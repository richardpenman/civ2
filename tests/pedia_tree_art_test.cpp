// SOURCE src/auto/pedia.cpp pedia_draw_tree_icon
// SOURCE src/auto/pedia.cpp PediaWindow::draw_advance_tree
// SOURCE src/auto/pedia.cpp PediaWindow::draw_list_icon
// Exercise all four tiers of the actual tree, including missing artwork and
// original graphics. HD selection must not change links or connector geometry.
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using iptr=intptr_t;
using i8=char;
using u32=unsigned;
using uint=unsigned;
using i32=int;
struct RECT { int left,top,right,bottom; };
using tagRECT=RECT;
static void SetRect(RECT *r,int l,int t,int rr,int b) { *r={l,t,rr,b}; }
static void IntersectRect(RECT *r,const RECT *a,const RECT *b)
{
    RECT v={std::max(a->left,b->left),std::max(a->top,b->top),
            std::min(a->right,b->right),std::min(a->bottom,b->bottom)};
    *r=v.left<v.right && v.top<v.bottom ? v : RECT{};
}
static int rect_width(const RECT *r) { return r->right-r->left; }
static int rect_height(const RECT *r) { return r->bottom-r->top; }
struct DibSurface { int clip_left=0,clip_top=0,clip_right=800,clip_bottom=600; };
static int hd_enabled=1,fallbacks;
static std::vector<int> requested,painted;
static std::vector<int> last_art;
static bool hd_advance(DibSurface *,int id,int x,int y,int w,int h)
{
    requested.push_back(id);last_art={id,x,y,w,h};
    assert((w==36 && h==20) || (w==54 && h==30));
    if(!hd_enabled || id==81)return false;
    painted.push_back(id);return true;
}
struct Sprite {
    int w=36,h=20;
    char padding[52];
    int get_width() { return w; }
    int get_height() { return h; }
    int width() { return w; }
    int height() { return h; }
    iptr draw(iptr bounds,iptr target,int x,int y) {
        ++fallbacks;
        SetRect((RECT *)bounds,x,y,x+w,y+h);
        IntersectRect((RECT *)bounds,(RECT *)bounds,(RECT *)&((DibSurface *)target)->clip_left);
        return bounds;
    }
};
static_assert(sizeof(Sprite)==0x3c,"legacy sprite stride");
static struct { Sprite advance_category_icon_sprites[20]; } government_sprites_block;
static struct { Sprite shield_icon_sprite; } war_icon_sprite_block;
static struct { int margin_x=10; } panel_layout;
static struct { int list_bg=1,list_sel_bg=2; } dlg_style;
static struct { int name,preq[2];char epoch,category; } advances[100];
static struct { char buf[20]; } textbuf;
static void text_clear() {}
static void text_append_string_id(int) {}
struct GameFont {
    int text_width(const char *) { return 50; }
    int line_spacing() { return 24; }
};
static std::vector<std::vector<int>> links,connectors,stubs;
struct PediaWindow: DibSurface {
    int content_left=10,content_top=20,content_right=790,content_bottom=580;
    GameFont font_size21;
    int resize_border=3,pedia_advance_ids[100]={};
    int category=1,list_top_index=9,list_items[100]={};
    void get_clip(RECT *r) { memcpy(r,&clip_left,sizeof *r); }
    void set_clip(RECT *r) { memcpy(&clip_left,r,sizeof *r); }
    void clear_links() { links.clear(); }
    void update_controls() {}
    void fill_background(RECT *,int) {}
    void fill_rect(iptr,int) {}
    void wnd_blit_surface_rect(iptr) {}
    void add_link(int id,int x,int y,int kind,int l,int t,int r,int b) {
        assert(kind==0);links.push_back({id,x,y,l,t,r,b});
    }
    void pedia_draw_tree_connector(int x,int y,int xx,int yy) { connectors.push_back({x,y,xx,yy}); }
    void draw_tree_stub(int x,int y) { stubs.push_back({x,y}); }
    void draw_advance_tree();
    void draw_list_icon(int,uint,int,int,int,int,int,Sprite *&,tagRECT &,int &);
};
static struct { PediaWindow window; } pedia_wnd;
#include "production.inc"

int main()
{
    const std::vector<int> ids={42,9,68,23,5,17,81,41,6,37,62,8,72,83,13};
    for(auto &a:advances)a.preq[0]=a.preq[1]=-1;
    for(unsigned n=0;n<ids.size();n++) {
        auto &a=advances[ids[n]];a.category=n%5;a.epoch=n%4;
        if(n<7) { a.preq[0]=ids[2*n+1];a.preq[1]=ids[2*n+2]; }
    }
    advances[ids.back()].preq[0]=1; // A continuation beyond the visible four tiers.
    auto &view=pedia_wnd.window;view.pedia_advance_ids[view.resize_border]=ids[0];
    view.draw_advance_tree();
    assert(requested==ids && fallbacks==1 && painted.size()==14);
    for(unsigned i=0;i<ids.size();i++)assert(links[i][0]==ids[i]);
    auto hd_links=links,hd_connectors=connectors,hd_stubs=stubs;
    assert(connectors.size()==14 && stubs.size()==1);
    hd_enabled=0;requested.clear();painted.clear();connectors.clear();stubs.clear();fallbacks=0;
    view.draw_advance_tree();
    assert(requested==ids && painted.empty() && fallbacks==15);
    assert(links==hd_links && connectors==hd_connectors && stubs==hd_stubs);
    // Clipped HD slots return the same bounds used by the tree's connectors.
    hd_enabled=1;Sprite icon;int bounds[4];
    pedia_draw_tree_icon(&icon,42,bounds,&view,-10,590);
    assert(bounds[0]==0 && bounds[1]==590 && bounds[2]==26 && bounds[3]==600);
    // The scrolled right column uses its mapped ID, not its row or list index.
    view.list_items[20]=42;Sprite *sprite=&icon;RECT row;int text_offset;
    for(hd_enabled=0;hd_enabled<2;hd_enabled++) {
        requested.clear();painted.clear();fallbacks=0;row={400,80,790,114};
        view.draw_list_icon(3,1,9,2,400,80,34,sprite,row,text_offset);
        assert(last_art==std::vector<int>({42,410,81,54,30}));
        assert(fallbacks==!hd_enabled && painted.size()==(unsigned)hd_enabled);
        assert(text_offset==74 && row.left==474);
    }
    hd_enabled=1;view.list_items[20]=81;fallbacks=0;row={400,80,790,114};
    view.draw_list_icon(3,0,9,2,400,80,34,sprite,row,text_offset);
    assert(fallbacks==1); // Advance without individual art uses the category sprite.
    view.category=2;requested.clear();fallbacks=0;row={400,80,790,114};
    view.draw_list_icon(3,0,9,2,400,80,34,sprite,row,text_offset);
    assert(requested.empty() && fallbacks==1); // Improvement list remains unchanged.
    puts("Civilopedia art: four tree tiers, scrolled list IDs, fallback, links, connectors and clipping passed.");
}
