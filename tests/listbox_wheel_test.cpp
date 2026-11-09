// SOURCE src/platform/controls.c lb_page
// SOURCE src/platform/controls.c lb_update_scroll
// SOURCE src/platform/controls.c lb_wheel
#include <cassert>
#include <cstdio>
enum { WS_VSCROLL=0x200000, LBS_DISABLENOSCROLL=0x1000, SB_VERT=1, FALSE=0, TRUE=1 };
struct RECT { int left, top, right, bottom; };
struct ListBox {
    int n=40, cur=2, top=0, item_h=16, vscroll=1, wheel_delta=0;
};
struct Window {
    Window *hwnd=this;
    int style=WS_VSCROLL, height=160;
    ListBox list;
    struct { int min=0, max=0, page=0, pos=0; } vscroll;
};
static ListBox *lb_of(Window *w) { return &w->list; }
static void GetClientRect(Window *w, RECT *r) { *r={0,0,200,w->height}; }
static void ShowScrollBar(Window *w, int, int show) {
    if (show) w->style |= WS_VSCROLL; else w->style &= ~WS_VSCROLL;
}
static void SetScrollPos(Window *w, int, int pos, int) { w->vscroll.pos=pos; }
static int repaints;
static void InvalidateRect(Window *, const RECT *, int) { repaints++; }
#include "production.inc"
int main()
{
    Window w;
    lb_update_scroll(&w);
    assert(w.vscroll.max==39 && w.vscroll.page==10);
    lb_wheel(&w,-60);assert(w.list.top==0 && !repaints);
    lb_wheel(&w,-60);assert(w.list.top==3 && w.vscroll.pos==3 && repaints==1);
    lb_wheel(&w,-240);assert(w.list.top==9 && w.vscroll.pos==9);
    lb_wheel(&w,120);assert(w.list.top==6 && w.list.cur==2);
    lb_wheel(&w,-12000);assert(w.list.top==30 && w.vscroll.pos==30);
    int old=repaints;
    lb_wheel(&w,-120);assert(w.list.top==30 && repaints==old);
    lb_wheel(&w,12000);assert(w.list.top==0);
    old=repaints;lb_wheel(&w,120);assert(w.list.top==0 && repaints==old);
    lb_wheel(&w,-60);lb_wheel(&w,60);
    assert(w.list.top==0 && w.list.wheel_delta==0);
    Window other;lb_wheel(&w,-60);lb_wheel(&other,-60);
    assert(w.list.top==0 && other.list.top==0); // Remainders belong to each list.
    lb_wheel(&w,-60);assert(w.list.top==3 && other.list.top==0);
    w.list.n=3;lb_wheel(&w,-120);assert(w.list.top==0 && !(w.style & WS_VSCROLL));
    w.list.n=0;lb_wheel(&w,120);lb_wheel(&w,-120);assert(w.list.top==0);
    w.list.n=40;w.height=8;lb_wheel(&w,-12000);
    assert(w.list.top==39 && w.vscroll.page==1 && w.list.cur==2);
    puts("List wheel: partial notches, limits, scrollbar position, selection and independent lists passed.");
}
