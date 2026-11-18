// SOURCE src/platform/controls.c edit_origin
// SOURCE src/platform/controls.c edit_format_rect
// SOURCE src/platform/controls.c lb_class
// SOURCE src/platform/controls.c edit_soft_breaks
// SOURCE src/platform/controls.c edit_soft_breaks_utf8
// SOURCE src/platform/controls.c edit_layout
// SOURCE src/platform/controls.c edit_pos_from_point
// SOURCE src/platform/controls.c edit_page_lines
// SOURCE src/platform/controls.c edit_update_scroll
// SOURCE src/platform/controls.c edit_scroll_to
// SOURCE src/platform/controls.c edit_move_vertical
// SOURCE src/auto/text.cpp str_copy_max
// SOURCE src/auto/text.cpp str_append_max
// Text in UTF-8 (a translation's "@@codepage utf-8": Chinese; src/include/utf8.h):
// characters decoded whole, texts cut to fit without cutting a character, the
// break rules next to wide characters and closing marks, words for the game's
// dialogs, and the edit control's wrapping and hit testing by character.  With
// plat_text_utf8 0 every byte is a character, as before.
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "utf8.h"
int plat_text_utf8 = 1;
enum { ES_MULTILINE=4, ES_AUTOHSCROLL=0x80, WS_HSCROLL=0x100000,
       WS_VSCROLL=0x200000, WS_EX_CLIENTEDGE=0x200, SM_CXBORDER=5,
       EN_VSCROLL=0x602, TRUE=1, EDIT_MAX_LINES=1024 };
enum { LB_AL, LB_BA, LB_BB, LB_B2, LB_BK, LB_CL, LB_CM, LB_CP, LB_CR, LB_EX, LB_GL, LB_HY, LB_IN,
       LB_IS, LB_LF, LB_NL, LB_NU, LB_OP, LB_PO, LB_PR, LB_QU, LB_SP, LB_SY };     // (controls.c's)
struct RECT { int left,top,right,bottom; };
struct FontFace { int width=8,height=16; } face;
struct Edit {
    char *text=nullptr;
    int len=0,first=0,top_line=0,wheel_delta=0,sel_start=0,sel_end=0;
    int font=0,border=0,left_margin=0,right_margin=0;
};
struct Scroll { int min=0,max=0,pos=0,page=0; };
struct Window {
    int style=ES_MULTILINE|WS_VSCROLL,exstyle=0;
    RECT client={0,0,96,64};
    Scroll vscroll;
    Edit edit;
    Window *hwnd=this;
};
static Edit *edit_of(Window *w) { return &w->edit; }
static FontFace *font_face_of(int) { return &face; }
static int text_height(FontFace *f) { return f->height; }
// as text.c measures: by character, a wide one twice as wide, a byte of no character none
static int text_width(FontFace *f,const char *s,int n)
{
    int w=0;
    for(int i=0;i<n;) {
        int len; unsigned c=u8_decode(s+i,n-i,&len);
        if(c!=U8_BAD) w+=u8_wide(c)?2*f->width:f->width;
        i+=len;
    }
    return w;
}
static int text_ave_width(FontFace *f) { return f->width; }
static int GetSystemMetrics(int) { return 1; }
static void GetClientRect(Window *w,RECT *r) { *r={0,0,w->client.right-w->client.left,w->client.bottom-w->client.top}; }
static void wnd_nc_paint(Window *) {}
static void InvalidateRect(Window *,const RECT *,int) {}
static void notify(Window *,int) {}
#define __cdecl
#define _strlen strlen
#define crt_memcpy memcpy
#include "production.inc"

static void text(Window &w,std::string &s) { w.edit.text=&s[0];w.edit.len=(int)s.size(); }

int main()
{
    // decoding: whole characters, broken sequences one byte of no character
    int len;
    assert(u8_decode("\xe4\xb8\xad",3,&len)==0x4e2d && len==3);
    assert(u8_decode("\xe4\xb8",2,&len)==U8_BAD && len==1);         // cut short
    assert(u8_decode("\xb8\xad",2,&len)==U8_BAD && len==1);         // a continuation byte
    assert(u8_decode("\xc0\x80",2,&len)==U8_BAD);                   // overlong
    assert(u8_decode("\xed\xa0\x80",3,&len)==U8_BAD);               // a surrogate
    assert(u8_decode("\xf0\xa0\x80\x80",4,&len)==0x20000 && len==4);

    // cutting to fit: never a character in two
    const char *zh="\xe4\xb8\xad\xe6\x96\x87";                       // 中文
    assert(u8_clip(zh,6)==6 && u8_clip(zh,5)==3 && u8_clip(zh,4)==3 && u8_clip(zh,3)==3 && u8_clip(zh,2)==0);
    char buf[8];
    str_copy_max(buf,"Ab\xe4\xb8\xad\xe6\x96\x87",6);              // room for 5 bytes: Ab中
    assert(!strcmp(buf,"Ab\xe4\xb8\xad"));
    str_copy_max(buf,"Ab\xe4\xb8\xad",5);                           // room for 4: Ab
    assert(!strcmp(buf,"Ab"));
    strcpy(buf,"x");
    str_append_max(buf,zh,6);                                        // x + 中 (5 bytes left: not 文)
    assert(!strcmp(buf,"x\xe4\xb8\xad"));

    // the caret: characters back and forward
    assert(u8_prev(zh,6)==3 && u8_prev(zh,3)==0 && u8_len_at(zh,6,3)==3 && u8_len_at("ab",2,0)==1);

    // break rules: next to a wide character, not before a closing mark nor after an opening one
    assert(u8_break_between(0x4e2d,0x6587));                          // 中|文
    assert(u8_break_between('a',0x6587) && u8_break_between(0x4e2d,'a'));
    assert(!u8_break_between('a','b'));
    assert(!u8_break_between(0x4e2d,0xff0c));                         // not before ，
    assert(!u8_break_between(0x4e2d,0x3002) && !u8_break_between(0x300c,0x4e2d));   // 。 「
    assert(!u8_break_between(0x8a71,'_'));                            // the game's "_._._."

    // the game's dialogs' words (Dialog::draw_wrapped_text): one wide character
    // (with its closing marks), or up to a space
    const char *s="\xe4\xb8\xad\xe6\x96\x87\xef\xbc\x8c""OK go";       // 中文，OK go
    const char *e=u8_word_end(s);
    assert(e==s+3);
    e=u8_word_end(s+3);                                               // 文， together
    assert(e==s+9);
    e=u8_word_end(s+9);                                               // "OK"
    assert(e==s+11 && *e==' ');
    plat_text_utf8=0;                                                 // (a byte a character: to the space)
    assert(u8_word_end(s)==s+11 && u8_clip(zh,4)==4 && u8_prev(zh,6)==5);
    plat_text_utf8=1;

    // the edit control: wrapped between Chinese characters (12 wide ones: 24 columns
    // of 8 pixels in 96 - two margins of nothing: 6 a line), never inside one, a
    // closing mark kept with the character before it
    Window w;
    std::string zhs;
    const char *chars[]={"\xe4\xb8\x80","\xe4\xba\x8c","\xe4\xb8\x89","\xe5\x9b\x9b","\xe4\xba\x94",
                         "\xe5\x85\xad","\xe4\xb8\x83","\xe5\x85\xab","\xe4\xb9\x9d","\xe5\x8d\x81",
                         "\xe7\x99\xbe","\xe5\x8d\x83"};                  // 一 .. 十 百 千
    for(const char *c:chars) zhs+=c;
    text(w,zhs);
    int starts[EDIT_MAX_LINES],lens[EDIT_MAX_LINES];
    int n=edit_layout(&w,starts,lens);
    assert(n==2 && starts[0]==0 && lens[0]==18 && starts[1]==18 && lens[1]==18);
    std::string punct;
    for(int i=0;i<5;i++) punct+=chars[i];
    punct+="\xe3\x80\x82";                                           // 一二三四五。 (6 wide: fits)
    punct+=chars[5];
    punct+="\xef\xbc\x8c";                                           // 六， : 六 goes down with its ，
    text(w,punct);
    n=edit_layout(&w,starts,lens);
    assert(n==2 && lens[0]==18 && starts[1]==18);
    for(int l=0;l<n;l++)
        assert((punct[starts[l]]&0xc0)!=0x80);                       // lines start on a character

    // hit testing: the caret lands between characters
    text(w,zhs);
    w.edit.top_line=0;
    for(int x=0;x<96;x++) {
        int p=edit_pos_from_point(&w,x,0);
        assert(p%3==0);
    }
    assert(edit_pos_from_point(&w,17,0)==3 && edit_pos_from_point(&w,15,0)==3 && edit_pos_from_point(&w,7,0)==0);
    // up and down keep to characters
    w.edit.sel_end=21;                                                // second line, after its first character
    edit_move_vertical(&w,-1,0);
    assert(w.edit.sel_end==3);
    // an escaped character (U8_ESC + UTF-8: a language's own name in a
    // single-byte interface) is one character in any code page
    for (int utf8 = 0; utf8 < 2; utf8++) {
        plat_text_utf8 = utf8;
        const char *el = "E\x1a\xce\xbb\x1a\xce\xbbx";        // "Eλλx"
        assert(u8_escaped_len(el + 1, -1) == 3 && u8_escaped_len(el, -1) == 0);
        assert(u8_len_at(el, -1, 1) == 3 && u8_len_at(el, 8, 4) == 3 && u8_len_at(el, -1, 7) == 1);
        assert(u8_prev(el, 8) == 7 && u8_prev(el, 7) == 4 && u8_prev(el, 4) == 1 && u8_prev(el, 1) == 0);
        assert(u8_clip(el, 3) == 1 && u8_clip(el, 2) == 1 && u8_clip(el, 4) == 4 && u8_clip(el, 6) == 4);
        assert(u8_escaped_len("\x1a", -1) == 0 && u8_escaped_len("\x1a\xce", 2) == 0 &&
               u8_escaped_len("\x1a\xe7\xb9\x81", -1) == 4);  // cut short; 繁
    }
    plat_text_utf8 = 1;
    printf("UTF-8 text: decoding, cutting to fit, break rules, dialog words, edit wrapping and caret passed\n");
    return 0;
}
