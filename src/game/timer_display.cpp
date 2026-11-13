/*
 * Turn timer (multiplayer "timed turns"): counts down the remaining time and
 * redraws the progress indicator.
 */
#include "game.h"


/* 0x0055b0fb */
void __cdecl mp_turn_timer_tick(void)
{
    int percent, color, elapsed;
    RECT rect;

    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0)
        return;
    (*(uint *)&setup_picwnd_block.g_timer_blink) ^= 1;
    if ((*(uint *)&setup_picwnd_block.g_timer_blink) != 0) {
        if (mp_timer.seconds_left != 0) {
            /* Seconds since the last tick, rounded (ticks are in 1/60 s). */
            elapsed = (int)((double)(timer_ticks() - mp_timer.last_tick) / 60.0 + 0.5);
            mp_timer.seconds_left -= elapsed;
            if (mp_timer.seconds_left < 0)
                mp_timer.seconds_left = 0;
            mp_timer.last_tick = timer_ticks();
        }
        if (mp_timer.seconds_left == 0) {
            city_window_close();
            intel_report_end_modal();
            throneroom_close();
            pedia_end_modal();
            player_turn_active = 0;
            status_draw_timer_bar(100, 0xffffffff);
            return;
        }
        ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
        rect.left = (*(int *)((u8 *)&map_views_block.main_view_frame_left));
        rect.top = (*(int *)((u8 *)&map_views_block.main_view_frame_top));
        rect.right = (*(int *)((u8 *)&map_views_block.main_view_frame_right));
        rect.bottom = (*(int *)((u8 *)&map_views_block.main_view_client_top));
        ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect( &rect);
    }
    percent = (mp_timer.seconds_left * 100000) / civ_saved_cursor_x_block.mp_turn_time_limit_ms;
    if (percent < 0x21)
        color = 0x6a;
    else if (percent < 0x42)
        color = 0x7a;
    else
        color = 0x2a;
    if ((color == 0x6a || mp_timer.seconds_left < 0x1e) &((uint *)&setup_picwnd_block.g_timer_blink) != 0)
        color = -1;
    status_draw_timer_bar(percent, color);
    if (mp_timer.handle != 0)
        game_timer_destroy(mp_timer.handle);
    mp_timer.handle = game_timer_create(((TimerFn)mp_turn_timer_tick), 500, 1);
}
