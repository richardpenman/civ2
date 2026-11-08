// SOURCE src/game/hd_windows.cpp art
// SOURCE src/game/hd_windows.cpp hd_advance
// SOURCE src/game/hd_windows.cpp dialog_draw_advance_sprite
// SOURCE src/game/hd_windows.cpp Dialog::add_advance
// SOURCE src/auto/window.cpp popup_text_with_advance
// Actual dialog adapters: distinct advances can share a category sprite;
// mixed pictures, callback data, zoom, offsets and classic fallback must survive.
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
using iptr=intptr_t;
using i32=int;
using u32=unsigned;
#define _fclose fclose
#include "scoped.h"
static_assert(sizeof(void *)==4,"the game callback carries a 32-bit sprite pointer");
struct RECT { int left,top,right,bottom; };
struct DibSurface { iptr bits=123;int clip_left=10,clip_top=20,clip_right=400,clip_bottom=300; };
static DibSurface surface;
static int hd_enabled=1,scale_num=8,fallbacks,destructs;
static std::string loaded_file,loaded_section;
static int loaded_flags;
static std::vector<int> drawn;
static int hd_find(const char *key) {
    int id=-1;assert(sscanf(key,"advance/%d",&id)==1);
    return id==81 ? 0 : id+100;
}
static void hd_size(int,int *w,int *h) { *w=72;*h=48; }
static void hd_draw(void *bits,int image,int x,int y,int w,int h,int l,int t,int r,int b,int tint) {
    assert((iptr)bits==123 && tint==-1);drawn={image,x,y,w,h,l,t,r,b};
}
static int gfx_scale_length(int v,int,int) { return v*scale_num/8; }
struct Sprite {
    int offset_x=2,offset_y=4;
    char padding[52];
    int width() { return 36; }
    int height() { return 20; }
    iptr draw(iptr bounds,iptr,int,int) { ++fallbacks;return bounds; }
};
static_assert(sizeof(Sprite)==60,"sprite stride");
static struct { Sprite advance_category_icon_sprites[20]; } government_sprites_block;
static struct { signed char category,epoch; } advances[100];
struct DialogSprite { int id,field_04;const char *label;Sprite *sprite; };
using DrawFn=void (*)(i32,iptr,i32,i32,i32,i32);
struct Dialog {
    std::vector<DialogSprite> entries;
    DrawFn callback=nullptr;
    int sprite_zoom=0,result_extra=17;
    void construct(int flags) { assert(flags==0x4000); }
    void destruct() { ++destructs; }
    void load_text(const char *file,const char *section,int flags) {
        loaded_file=file;loaded_section=section;loaded_flags=flags;
    }
    DialogSprite *add_sprite(iptr sprite,iptr id,const char *label) {
        entries.push_back({(int)id,0,label,(Sprite *)sprite});return &entries.back();
    }
    void set_sprite_draw_fn(DrawFn fn) { callback=fn; }
    void set_image_zoom(int zoom) { sprite_zoom=zoom; }
    void add_advance(int,iptr=0);
    void show_modal(int) {
        scale_num=sprite_zoom+8;
        for(const auto &e:entries) {
            if(callback)callback((i32)e.sprite,(iptr)&surface,e.id,e.field_04,40,50);
            else { RECT r;e.sprite->draw((iptr)&r,(iptr)&surface,40,50); }
        }
    }
};
static struct { u32 help_pressed; } dlg_input;
#include "production.inc"
int main()
{
    advances[0]={3,2};advances[1]=advances[0];advances[81]={4,3};
    for(int zoom: {0,4,8}) {
        Dialog d;d.set_image_zoom(zoom);d.add_advance(0,8);
        assert(d.entries[0].id==8 && d.entries[0].field_04==1 && !d.entries[0].label);
        assert(d.entries[0].sprite==government_sprites_block.advance_category_icon_sprites+14);
        drawn.clear();fallbacks=0;d.show_modal(0);
        int scale=zoom+8,w=30*scale/8,h=20*scale/8;
        assert(drawn==std::vector<int>({100,40+2*scale/8+(36*scale/8-w)/2,50+4*scale/8,w,h,10,20,400,300}));
        assert(!fallbacks);
        d.add_advance(1);d.show_modal(0);assert(drawn[0]==101); // Same generic sprite, different art.
        Sprite other;d.add_sprite((iptr)&other,1,"Wonder");fallbacks=0;d.show_modal(0);
        assert(fallbacks==1 && drawn[0]==101); // Ordinary entry IDs must not become advances.
        d.add_advance(81);fallbacks=0;d.show_modal(0);assert(fallbacks==2); // Missing individual art.
    }
    hd_enabled=0;Dialog classic;classic.add_advance(0,8);
    assert(!classic.callback && classic.entries[0].field_04==0 && classic.entries[0].id==8);
    drawn.clear();fallbacks=0;classic.show_modal(0);assert(drawn.empty() && fallbacks==1);
    for(hd_enabled=0;hd_enabled<2;hd_enabled++) {
        destructs=fallbacks=0;drawn.clear();
        popup_text_with_advance("GAME","GOLDENAGE",7,0,8);
        assert(loaded_file=="GAME" && loaded_section=="GOLDENAGE" && loaded_flags==7);
        assert(destructs==1 && dlg_input.help_pressed==17);
        assert(fallbacks==!hd_enabled && drawn.empty()==!hd_enabled);
    }
    puts("Advance dialogs: IDs, mixed sprites, zoom/offsets, clipping, missing art, classic mode and popup results passed.");
}
