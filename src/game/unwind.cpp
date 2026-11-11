/*
 * Functions whose cleanup runs through MSVC unwind funclets (small `call $L`
 * blocks executing with the parent's EBP) that the generator could not
 * translate.  The funclets are inlined here as plain destructor calls on the
 * parent's own locals; exception-frame bookkeeping (fs:[0], trylevel) is
 * dropped.
 */
#include "game.h"

/* Value an MSVC function returns in EAX after its epilogue restored fs:[0]:
   the previous SEH list head, i.e. the current fs:[0] (we push no frame). */
static inline i32 seh_list_head(void)
{
    i32 r;
    r = 0;      /* no SEH chain outside 32-bit Windows */
    return r;
}

/* 0x0044ca60: destructor - destroys the member at this+0x10, then the base. */
i32 ModalMenuWindow::destruct()
{
    WndHandlers *member;

    /* funclet 0044caab */
    member = (this != NULL) ? &wnd_handlers : NULL;
    member->destruct();
    /* funclet 0044cad3 */
    ((WndBase *)this)->destroy();
    return seh_list_head();
}

/* 0x0044cba0: destructor - resets the vtable, clears this+0x110, destroys
   the member at this+0x48, then the base. */
void SurfaceWindow::destruct()
{
    Wnd *member;

    *(i8 ***)this = (i8 **)((pointer *)(ptr_fun_rest + 8));
    *(i32 *)&on_paint = 0;
    /* funclet 0044cc01 */
    member = (this != NULL) ? &window : NULL;
    member->destruct();
    /* funclet 0044cc29 */
    surface_dtor((DibSurface *)this);
}

/* 0x004f3e70: destructor - destroys the member at this+0x4a4, then the base. */
void CityPicturePopup::destruct()
{
    Dialog *member;

    /* funclet 004f3ebb */
    member = (this != NULL) ? &dialog : NULL;
    member->Dialog::destruct();
    /* funclet 004f3ee5 */
    PictureWindow::destruct();
}

/* 0x005efeb0: destructor - releases the handle at this+0xb0, runs the
   cleanup, destroys the members at this+0xc0 and this+0x44, then the base. */
void DDWindow::destruct()
{
    WndHandlers *member;

    native_wnd = ddwin_destroy_native((i32 *)native_wnd);
    remove_all_controls();
    /* funclet 005eff2b */
    child_controls.destruct();
    /* funclet 005eff3a */
    member = (this != NULL) ? &handlers : NULL;
    member->destruct();
    /* funclet 005eff62 */
    ((DDSurface *)this)->destruct();
}

/* 0x0055c3d3: "PICKGOVT" dialog - lets civ civ2 pick a government
   (all_govs != 0: all of them, including anarchy) and applies the choice. */
void __cdecl civ_select_government_dialog(int civ2, int all_govs)
{
    i32 text_arg;
    int choice2;
    Dialog dlg;
    int gov;

    dlg.Dialog::construct( 0x4000);
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
        /* nothing */
    }
    else if ((game.multiplayer < 3) || (human_civ[0] == civ2)) {
        dlg.load_game_text( (const char *)str_pickgovt);
        for (gov = (all_govs != 0) ? 0 : 1; gov < 7; gov = gov + 1) {
            /* (civ_can_choose_government's last 19 parameters are a decompiler artifact) */
            if ((all_govs != 0) || (civ_can_choose_government(civ2, gov, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) != 0)) {
                text_clear();
                text_append_string_id(government_names[gov]);
                dlg.add_option( (const char *)&textbuf.buf, gov, 0);
            }
        }
        choice2 = dlg.show_modal( 0);
        if (-1 < choice2) {
            civ_set_government(civ2, choice2);
            snd_play_effect(SND_NEWGOVT_20, 1, 0, 0);
            text_arg = (i32)civ_leader_name(civ2);
            text_set_string_arg(0, (const char *)text_arg);
            text_arg = civ_leader_title(civ2);
            text_set_string_arg(1, (const char *)text_arg);
            text_arg = civ_get_adjective(civ2);
            text_set_string_arg(2, (const char *)text_arg);
            text_set_label_arg(3, government_names[choice2]);
            popup_game_with_sprite((const char *)str_newgovt, ((Sprite *)(((i8 *)&government_sprites_block.government_sprites) + choice2 * 0x3c)), 0);
            if (((4 < choice2) && (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0)) &&
                ((game.field_0c & 0x20) == 0)) {
                text_set_label_arg(0, government_names[choice2]);
                popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest), (const char *)str_democrats, (Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite, 0);
                game.field_0c = game.field_0c | 0x20;
            }
        }
    }
    else {
        net_send_message(NM_PICK_GOVT, net.net_slots[net.net_civ_slots[civ2]].id,
                           all_govs, 0, 0, 0, 0, 0, 0, 0);
    }
    /* funclet 0055c679 */
    dlg.Dialog::destruct();
}

/* 0x00493e83: builds a temporary object, stores civ in it, runs it if
   its check succeeds, then destroys it. */
void __cdecl portrait_dlg_show(i32 civ)
{
    int created;
    PortraitDialog portrait_dlg;

    portrait_dlg.construct();
    portrait_dlg.civ = civ;
    created = portrait_dlg.create();
    if (created != 0) {
        portrait_dlg.run();
    }
    /* funclet 00493eeb: lea ecx,[ebp-0x1f4] */
    portrait_dlg.destruct();
}

/* 0x005e0863: builds a window with "OK"/"Cancel" buttons on the stack and
   tears it all down again; returns 0. */
i32 __cdecl FUN_005e0863(void)
{
    int btn_y;
    int btn_x;
    i8 ok_button[60];
    i8 font2[8];
    i8 palette[1076];
    SurfaceWindow surf_wnd2;
    i8 cancel_button[60];
    i8 edit[72];
    tagRECT btn_rect;

    ((GamePalette *)palette)->init();
    surf_wnd2.SurfaceWindow::construct();
    ((GameFont *)font2)->init_empty();
    button_ctor((Button *)ok_button);
    button_ctor((Button *)cancel_button);
    edit_ctor((EditControl *)edit);
    ((GameFont *)font2)->set( 0, 0x11);
    ui_set_button_font((GameFont *)font2);
    ui_set_default_font((GameFont *)font2);
    SetRect(&btn_rect, 0, 0, 0x3c, 0x19);
    btn_y = ((DibSurface *)(&surf_wnd2))->get_height();
    btn_y = btn_y + -0x23;
    btn_x = ((DibSurface *)(&surf_wnd2))->get_width();
    OffsetRect(&btn_rect, btn_x + -0x46, btn_y);
    ((Button *)ok_button)->create( (i32)(u8 *)&surf_wnd2.window, 0x65, (i32)&btn_rect,
                       (const char *)((i32 *)&str_ok_3));
    ((Button *)ok_button)->set_callback( dlg_radio_choice_button_cb);
    OffsetRect(&btn_rect, 0, -0x23);
    ((Button *)cancel_button)->create( (i32)(u8 *)&surf_wnd2.window, 0x66, (i32)&btn_rect,
                       (const char *)str_cancel_3);
    ((Button *)cancel_button)->set_callback( dlg_radio_choice_button_cb);
    /* funclets 005e0a31..005e0a6a: destroy the locals in reverse order */
    edit_dtor((EditControl *)edit);
    ctrl_dtor((Button *)cancel_button);
    ctrl_dtor((Button *)ok_button);
    ((GameFont *)font2)->destroy();
    surf_wnd2.SurfaceWindow::destruct();
    ((GamePalette *)palette)->destroy();
    return 0;
}

/* 0x004d53ab: loads picture 0x1f1 and cuts it into six button images
   (two rows of three; the second row's caption depends on the language). */
void SpaceshipWindow::make_button_sprites()
{
    int loaded;
    i32 label_text;
    i8 surface[72];
    tagRECT rect;

    surface_ctor((DibSurface *)surface);
    gfx_set_text_color_index(0x8b);
    loaded = ((DibSurface *)surface)->load_gif_resource( 0x1f1, 10, 0xec, (iptr)this + 0x1f8);
    if (loaded != 0) {
        text_clear();
        label_text = (i32)label_string(label_table.string_ids[0xfe]);
        text_append((const char *)label_text);
        SetRect(&rect, 0, 0, 0x34, 0x24);
        OffsetRect(&rect, 1, 1);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        close_btn_disabled_sprite.grab_from_surface( (i32)surface, 9, 1, 1, 0x34, 0x24);
        OffsetRect(&rect, 0x35, 0);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        close_btn_pressed_sprite.grab_from_surface( (i32)surface, 9, 0x36, 1, 0x34, 0x24);
        OffsetRect(&rect, 0x35, 0);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        close_btn_normal_sprite.grab_from_surface( (i32)surface, 9, 0x6b, 1, 0x34, 0x24);
        gfx_set_text_color_index(0xc6);
        text_clear();
        if (game_language == 0) {
            label_text = (i32)label_string(label_table.string_ids[0xd5]);
            text_append((const char *)label_text);
        }
        else if (game_language == 1) {
            text_append((const char *)str_lancer);
        }
        else if (game_language == 2) {
            text_append((const char *)str_starten);
        }
        SetRect(&rect, 0, 0, 0x43, 0x24);
        OffsetRect(&rect, 1, 0x26);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        launch_btn_disabled_sprite.grab_from_surface( (i32)surface, 9, 1, 0x26, 0x43, 0x24);
        OffsetRect(&rect, 0x44, 0);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        launch_btn_pressed_sprite.grab_from_surface( (i32)surface, 9, 0x45, 0x26, 0x43, 0x24);
        OffsetRect(&rect, 0x44, 0);
        ((DibSurface *)surface)->draw_text_in_rect( (iptr)this + 0x8d4, (const char *)((i8 *)&textbuf.buf), &rect, 0);
        launch_btn_normal_sprite.grab_from_surface( (i32)surface, 9, 0x89, 0x26, 0x43, 0x24);
    }
    /* funclet 004d56ea: lea ecx,[ebp-0x64] */
    surface_dtor((DibSurface *)surface);
}
