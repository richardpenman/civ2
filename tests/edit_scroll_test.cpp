// SOURCE src/platform/controls.c edit_origin
// SOURCE src/platform/controls.c edit_format_rect
// SOURCE src/platform/controls.c edit_layout
// SOURCE src/platform/controls.c edit_page_lines
// SOURCE src/platform/controls.c edit_update_scroll
// SOURCE src/platform/controls.c edit_scroll_to
// SOURCE src/platform/controls.c edit_scroll_command
// SOURCE src/platform/controls.c edit_wheel
// SOURCE src/platform/controls.c edit_pos_from_point
// SOURCE src/platform/controls.c edit_scroll
// SOURCE src/platform/controls.c edit_move_vertical
// Exercise the real wrapped-line layout, viewport, hit testing and navigation.
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
enum { ES_MULTILINE=4, ES_AUTOHSCROLL=0x80, WS_HSCROLL=0x100000,
       WS_VSCROLL=0x200000, WS_EX_CLIENTEDGE=0x200, SM_CXBORDER=5,
       EN_VSCROLL=0x602, TRUE=1, EDIT_MAX_LINES=1024 };
enum { SB_LINEUP, SB_LINEDOWN, SB_PAGEUP, SB_PAGEDOWN, SB_THUMBPOSITION,
       SB_THUMBTRACK, SB_TOP, SB_BOTTOM, SB_ENDSCROLL };
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
    RECT client={0,0,160,48};
    Scroll vscroll;
    Edit edit;
    Window *hwnd=this;
};
static Edit *edit_of(Window *w) { return &w->edit; }
static FontFace *font_face_of(int) { return &face; }
static int text_height(FontFace *f) { return f->height; }
static int text_width(FontFace *f,const char *,int n) { return f->width*n; }
static int GetSystemMetrics(int) { return 1; }
static void GetClientRect(Window *w,RECT *r) { *r={0,0,w->client.right-w->client.left,w->client.bottom-w->client.top}; }
static int paints,invalidates,notifications;
static void wnd_nc_paint(Window *) { paints++; }
static void InvalidateRect(Window *,const RECT *,int) { invalidates++; }
static void notify(Window *,int code) { assert(code==EN_VSCROLL);notifications++; }
#include "production.inc"

static void text(Window &w,std::string &s)
{
    w.edit.text=&s[0];w.edit.len=(int)s.size();
}

int main()
{
    Window w;
    std::string description;
    for(int i=0;i<40;i++) {
        if(i)description+="\r\n";
        description+="line "+std::to_string(i)+" example";
    }
    text(w,description);
    int starts[EDIT_MAX_LINES],lens[EDIT_MAX_LINES];
    assert(edit_layout(&w,starts,lens)==40);
    assert(edit_page_lines(&w)==3);
    edit_update_scroll(&w,40);
    assert(w.vscroll.max==39 && w.vscroll.page==3 && w.vscroll.pos==0);
    assert(edit_scroll_command(&w,SB_LINEUP,0)==0);
    assert(edit_scroll_command(&w,SB_LINEDOWN,0)==1);
    assert(edit_scroll_command(&w,SB_PAGEDOWN,0)==2);
    assert(w.edit.top_line==3 && w.vscroll.pos==3);
    edit_scroll_command(&w,SB_THUMBTRACK,20);
    assert(w.edit.top_line==20);
    edit_scroll_command(&w,SB_THUMBPOSITION,25);
    assert(w.edit.top_line==25);
    edit_scroll_command(&w,SB_BOTTOM,0);
    assert(w.edit.top_line==37);
    assert(edit_scroll_command(&w,SB_LINEDOWN,0)==0);
    int count=notifications;
    edit_scroll_command(&w,SB_ENDSCROLL,0);
    assert(notifications==count);
    edit_scroll_command(&w,SB_TOP,0);
    edit_wheel(&w,-60);assert(w.edit.top_line==0);
    edit_wheel(&w,-60);assert(w.edit.top_line==3 && w.edit.wheel_delta==0);
    edit_wheel(&w,120);assert(w.edit.top_line==0);
    edit_wheel(&w,-12000);assert(w.edit.top_line==37);
    edit_wheel(&w,12000);assert(w.edit.top_line==0);

    // Clicking after scrolling uses the visible line, not the start of text.
    edit_scroll_to(&w,10);
    assert(edit_pos_from_point(&w,0,0)==starts[10]);
    assert(edit_pos_from_point(&w,16,20)==starts[11]+2);
    assert(edit_pos_from_point(&w,1000,47)==starts[12]+lens[12]);
    w.edit.sel_end=starts[11]+3;
    edit_move_vertical(&w,1,0);edit_scroll(&w);
    assert(w.edit.sel_end==starts[12]+3 && w.edit.top_line==10);
    edit_move_vertical(&w,3,1);edit_scroll(&w);
    assert(w.edit.sel_end==starts[15]+3 && w.edit.top_line==13);
    w.edit.sel_end=w.edit.len;edit_scroll(&w);
    assert(w.edit.top_line==37);
    w.edit.sel_end=0;edit_scroll(&w);assert(w.edit.top_line==0);

    // Page extent uses the format rectangle; reflow and resize clamp the end.
    w.edit.border=1;assert(edit_page_lines(&w)==2);
    w.edit.border=0;
    edit_scroll_to(&w,37);
    w.client.bottom=160;edit_update_scroll(&w,40);
    assert(w.edit.top_line==30 && w.vscroll.page==10);
    std::string short_text="short";text(w,short_text);
    edit_update_scroll(&w,edit_layout(&w,starts,lens));
    assert(w.edit.top_line==0 && w.vscroll.pos==0);
    assert(edit_scroll_command(&w,SB_PAGEDOWN,0)==0);

    std::string wrapped="one two three four five six seven eight nine ten";
    text(w,wrapped);w.client.right=80;w.client.bottom=32;
    int lines=edit_layout(&w,starts,lens);assert(lines>2);
    edit_scroll_command(&w,SB_BOTTOM,0);assert(w.edit.top_line==lines-2);
    w.client.right=800;
    edit_update_scroll(&w,edit_layout(&w,starts,lens));
    assert(w.edit.top_line==0);
    assert(description.find("line 39 example")!=std::string::npos);

    // Existing single-line caret scrolling/hit testing stays independent.
    w.style=0;w.client.right=80;w.edit.first=0;w.edit.sel_end=20;
    edit_scroll(&w);assert(w.edit.first>0);
    assert(edit_pos_from_point(&w,0,100)==w.edit.first);
    assert(paints && invalidates && notifications);
    puts("Multiline edit scrolling, wrapped text, hit testing and keyboard navigation passed");
}
