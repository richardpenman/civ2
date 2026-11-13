/*
 * Large functions whose generated translation contained EBP-relative code
 * (exception-unwind funclets run with the caller's EBP, or a lost stack slot).
 * Bodies follow the generated versions; the funclets are inlined as plain
 * code on the function's own locals, exception bookkeeping is dropped.
 */
#include "game.h"

/* Part of setup_new_game_dialogs (0041ba52): the advanced rules (checkboxes)
   dialog.  Returns the accelerated start check. */
static int setup_rules_dialog(Dialog &dlg,int setup_mode,int *pick_opponents)
{
  int choice;
  int accel_choice;
  int opt_i;

  dlg_checks_clear();
  dlg_check_set(0,(uint)((game.rules & RULE_NORMAL_COMBAT) == 0));
  dlg_check_set(1,game.rules & RULE_FLAT_WORLD);
  dlg_check_set(2,0);
  dlg_check_set(3,(int)prefs.accelerated_start);
  dlg_check_set(4,game.rules & RULE_BLOODLUST);
  dlg_check_set(5,game.rules & RULE_NO_RESTART_ELIMINATED);
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(3,6,9);
  dlg.load_text_section((const char *)&str_game_7,(const char *)str_advanced,0,0,0,0,0,0);
  if (((byte)dlg.style_flags & 4) != 0) {
    for (opt_i = 0; opt_i < (int)dlg.option_count; opt_i = opt_i + 1) {
      dlg.set_option_checked(opt_i,1 << ((byte)opt_i & 0x1f) & dlg_input.check_bits);
    }
  }
  dlg.set_position(
                     ((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3) +
                     1,-(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3
                         ) + 1));
  dlg.show_modal(0);
  snd_play_effect(SND_MENUOK,0,0,0);
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
  dlg_input.help_pressed = (int)dlg.result_extra;
  if (((byte)dlg.style_flags & 4) != 0) {
    dlg_input.check_bits = 0;
    for (opt_i = 0; opt_i < (int)dlg.option_count; opt_i = opt_i + 1) {
      choice = dlg.option_is_checked(opt_i);
      if (choice != 0) {
        dlg_input.check_bits = dlg_input.check_bits | 1 << ((byte)opt_i & 0x1f);
      }
    }
  }
  game.rules = 0x2f;
  choice = dlg_check_get(0);
  if (choice == 0) {
    game.rules = game.rules | RULE_NORMAL_COMBAT;
  }
  choice = dlg_check_get(1);
  if (choice != 0) {
    game.rules = game.rules | RULE_FLAT_WORLD;
  }
  *pick_opponents = dlg_check_get(2);
  accel_choice = dlg_check_get(3);
  choice = dlg_check_get(4);
  if (choice != 0) {
    game.rules = game.rules | RULE_BLOODLUST;
  }
  choice = dlg_check_get(5);
  if (choice != 0) {
    game.rules = game.rules | RULE_NO_RESTART_ELIMINATED;
  }
  if (setup_mode == 0) {
    prefs.game_rules[0] = game.rules;
  }
  else {
    prefs.game_rules[0] = game.rules & ~RULE_FLAT_WORLD | prefs.game_rules[0] & 0x8000;
  }
  return accel_choice;
}

/* Part of setup_new_game_dialogs (0041ba52): the years of the accelerated
   starts (turns 0, 21 and 41) as the text arguments. */
static void setup_accel_year_args(void)
{
  uint year_abs;
  int choice;
  int year_a;
  int year_b;
  int year_c;

  game.first_launch_turn = 0xffff;
  choice = game_turn_to_year(0);
  if (choice < 1) {
    year_abs = game_turn_to_year(0);
    year_a = ~year_abs + 1;
  }
  else {
    year_a = game_turn_to_year(0);
  }
  text_set_number_arg(0,year_a);
  choice = game_turn_to_year(0x15);
  if (choice < 1) {
    year_abs = game_turn_to_year(0x15);
    year_b = ~year_abs + 1;
  }
  else {
    year_b = game_turn_to_year(0x15);
  }
  text_set_number_arg(1,year_b);
  choice = game_turn_to_year(0x29);
  if (choice < 1) {
    year_abs = game_turn_to_year(0x29);
    year_c = ~year_abs + 1;
  }
  else {
    year_c = game_turn_to_year(0x29);
  }
  text_set_number_arg(2,year_c);
}

/* Part of setup_new_game_dialogs (0041ba52): makes the leaders' name label
   ids positive again (a chosen name is marked used by negating it). */
static void setup_restore_leader_labels(void)
{
  int leader_i;
  int title_i;

  for (leader_i = 0; leader_i < 0x15; leader_i = leader_i + 1) {
    if (leaders[leader_i].leader_name < 1) {
      leaders[leader_i].leader_name =
           ~leaders[leader_i].leader_name + 1;
    }
    if (leaders[leader_i].plural < 1) {
      leaders[leader_i].plural =
           ~leaders[leader_i].plural + 1;
    }
    if (leaders[leader_i].adjective < 1) {
      leaders[leader_i].adjective =
           ~leaders[leader_i].adjective + 1;
    }
    for (title_i = 0; title_i < 7; title_i = title_i + 1) {
      if (leaders[leader_i].titles[title_i][0] < 1) {
        leaders[leader_i].titles[title_i][0] =
             ~leaders[leader_i].titles[title_i][0] + 1;
      }
      if (leaders[leader_i].titles[title_i][1] < 1) {
        leaders[leader_i].titles[title_i][1] =
             ~leaders[leader_i].titles[title_i][1] + 1;
      }
    }
  }
}

/* Part of setup_new_game_dialogs (0041ba52): the custom tribe dialogs (leader,
   tribe and adjective names, the government titles, the portrait).  Returns 0
   to go on to the city style, 1 to go back to the leaders (gender) dialog, 2
   to quit (a dialog failed to load). */
static int setup_custom_tribe_dialogs(Dialog &dlg)
{
  int choice;
  int dlg_err;
  int title_i;

  while( true ) {
    while( true ) {
      text_set_string_arg_b(1,(const char *)&civ_names[game.player_civ].tribe_name[0]);
      text_set_string_arg_b(2,(const char *)&civ_names[game.player_civ].adjective[0]);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,7);
      dlg_err = dlg.load_text_section((const char *)&str_game_7,(const char *)str_customtribe,0x17,
                                    &civ_names[game.player_civ].leader_name[0],0,0,0,1);
      if (dlg_err != 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        return 2;
      }
      dlg.set_position(0xfffffc19,
                         -(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)
                                 ) >> 3) + 1));
      choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) return 1;
      crt_strcpy(&civ_names[game.player_civ].leader_name[0],((i8 *)&text_args.strings));
      crt_strcpy(&civ_names[game.player_civ].tribe_name[0],((i8 *)&text_args.string1));
      crt_strcpy(&civ_names[game.player_civ].adjective[0],((i8 *)&text_args.string2));
      if ((int)dlg.result_extra != 1) break;
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,7);
      dlg_err = dlg.load_game_section((const char *)str_customtribe2,1);
      if (dlg_err != 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        return 2;
      }
      dlg_err = 0;
      for (title_i = 0; title_i < 7; title_i = title_i + 1) {
        text_clear();
        text_append_string_id(government_names[title_i]);
        text_append_colon();
        dlg.add_edit_field((const char *)&textbuf.buf,
                           &civ_names[game.player_civ].titles[title_i][0],0x17);
      }
      dlg.set_position(0xfffffc19,
                         -(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)
                                 ) >> 3) + 1));
      choice = dlg.show_modal(0);
      if (choice == 0) {
        for (title_i = 0; title_i < 7; title_i = title_i + 1) {
          crt_strcpy(&civ_names[game.player_civ].titles[title_i][0],
                       ((i8 *)&text_args.strings) + title_i * 0x104);
        }
      }
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    if ((int)dlg.result_extra != 2) break;
    portrait_dlg_show((int)game.player_civ);
  }
  return 0;
}

/* Part of setup_new_game_dialogs (0041ba52): picks the enemy civs at random,
   then, if asked, lets the player choose each one's tribe.  Returns 1 when a
   choice is cancelled (back to the city style dialog).  leader_i is the
   caller's local (its upper bytes carry over from the tribe list).  It calls
   rand() as crt_rand_at("setup_new_game_dialogs"): the rand trace names the
   original function, as the original's does. */
static int setup_choose_opponents(Dialog &dlg,int pick_opponents,int &leader_i)
{
  int choice;
  int title_i;
  int opp_number;
  int leader_pick;

  for (title_i = 0; title_i < (int)(uint)game.num_enemies; title_i = title_i + 1) {
    choice = crt_rand_at("setup_new_game_dialogs");
    (*((char *)&leader_i + 0)) = (byte)(choice >> 0x1f);
    (*((char *)&leader_i + 0)) =
         (((byte)choice ^ (byte)leader_i) - (byte)leader_i & 7 ^ (byte)leader_i) -
         (byte)leader_i;
    if ((1 << ((byte)leader_i & 0x1f) & (uint)game.civs_alive) == 0) {
      game.civs_alive = game.civs_alive | (byte)(1 << ((byte)leader_i & 0x1f));
    }
    else {
      title_i = title_i + -1;
    }
  }
  if (pick_opponents != 0) {
    opp_number = 0;
    leader_i = 1;
    do {
      if (7 < leader_i) break;
      if (((1 << ((byte)leader_i & 0x1f) & (uint)game.human_players) == 0) &&
         ((1 << ((byte)leader_i & 0x1f) & (uint)game.civs_alive) != 0)) {
        opp_number = opp_number + 1;
        text_set_number_arg(0,opp_number);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,9);
        dlg.load_game_section((const char *)str_opponent,1);
        dlg.option_list->id = 0x15;
        for (leader_pick = 0; leader_pick < 0x15; leader_pick = leader_pick + 1) {
          if (leaders[leader_pick].color == leader_i) {
            text_clear();
            text_append_string_id((int)leaders[leader_pick].plural);
            text_append_space();
            text_append_lparen();
            text_append_string_id((int)leaders[leader_pick].leader_name);
            text_append_rparen();
            dlg.add_option((const char *)&textbuf.buf,leader_pick,0);
          }
        }
        dlg.set_position(0xfffffc19,
                           -(((int)(caption_font_block.screen_height + -0x1e0 +
                                   (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        leader_pick = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (leader_pick < 0) return 1;
        if (leader_pick < 0x15) {
          (((i8 *)&setup_chosen_leaders))[leader_i] = (i8)leader_pick;
        }
      }
      leader_i = leader_i + 1;
    } while( true );
  }
  return 0;
}

/* 0x0041ba52: new-game setup dialogs (difficulty, enemies, barbarity, ...).
 * Unwind funclet 0x0041d3f2 destroys the dialog object at EBP-0x320 (dlg). */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl setup_new_game_dialogs(int setup_mode)

{
  byte rand_sign;
  int choice;
  i32 label;
  int next_step;
  char *label_text;
  int leader_arg;
  i32 option_arg;
  int leader_i;
  int style_i;
  i8 female_flag;
  char barb_level;
  i8 custom_name[260];
  i32 result;
  int enemies_choice;
  int accel_choice;
  int pick_opponents;
  int title_i;
  int tribe_choice;
  int difficulty_choice;
  int dlg_err;
  i32 unused_10;
  
  result = 1;
  pick_opponents = 0;
  accel_choice = 0;
  Dialog dlg(0x2000);
  leaders_fixup_names();
  game.field_0e = 0;
  for (;;) {
    do {
      do {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(3,6,3);
        dlg.load_game_section((const char *)str_difficulty_2,1);
        dlg.set_default_choice((int)prefs.difficulty);
        dlg.set_position(
                           ((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3)
                           + 1,-(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U))
                                 >> 3) + 1));
        difficulty_choice = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (difficulty_choice < 0) return result;
        game.difficulty = (byte)difficulty_choice;
        prefs.difficulty = (short)difficulty_choice;
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(1,6,4);
        dlg.load_game_section((const char *)str_enemies,1);
        dlg.set_default_choice((int)prefs.enemies_choice);
        dlg.set_position(
                           -(((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3
                             ) + 1),-(((int)(caption_font_block.screen_height + -0x1e0 +
                                            (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        enemies_choice = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      } while (enemies_choice < 0);
      if (dlg.result_extra == 0) {
        prefs.enemies_choice = (short)enemies_choice;
        game.num_enemies = 6 - (char)enemies_choice;
      }
      else {
        choice = _rand();
        game.num_enemies = (char)(choice % 5) + 2;
      }
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(3,6,5);
      dlg.load_game_section((const char *)str_barbarity,1);
      dlg.set_default_choice((int)prefs.barbarity);
      dlg.set_position(
                         ((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3) +
                         1,-(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3
                             ) + 1));
      choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    } while (choice < 0);
    if (dlg.result_extra == 0) {
      barb_level = (char)choice;
      game.barbarian_activity = barb_level;
      prefs.barbarity = (short)choice;
    }
    else if (game.difficulty < DIFF_PRINCE) {
      choice = _rand();
      rand_sign = (byte)(choice >> 0x1f);
      game.barbarian_activity = ((((byte)choice ^ rand_sign) - rand_sign & 1 ^ rand_sign) - rand_sign) + game.difficulty;
    }
    else if (game.difficulty < DIFF_DEITY) {
      choice = _rand();
      game.barbarian_activity = (char)(choice % 3) + '\x01';
    }
    else {
      choice = _rand();
      rand_sign = (byte)(choice >> 0x1f);
      game.barbarian_activity = ((((byte)choice ^ rand_sign) - rand_sign & 1 ^ rand_sign) - rand_sign) + '\x02';
    }
    do {
      game.rules = 0x3f;
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(1,6,9);
      dlg.load_game_section((const char *)str_rules_2,1);
      dlg.set_default_choice((int)prefs.rules_choice);
      dlg.set_position(
                         -(((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3)
                          + 1),-(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U))
                                 >> 3) + 1));
      choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) return result;
      prefs.rules_choice = (short)choice;
      if (choice == 0) break;  /* the standard rules: no accelerated start */
      game.rules = prefs.game_rules[0];
      if (setup_mode == 1) {
        game.rules = prefs.game_rules[0] & 0x7fff;
      }
      else if (setup_mode == 2) {
        game.rules = prefs.game_rules[0] | 0x8000;
      }
      accel_choice = setup_rules_dialog(dlg,setup_mode,&pick_opponents);
      if (accel_choice == 0) break;
      game.first_launch_turn = 0xffff;
      setup_accel_year_args();
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(1,6,9);
      dlg.load_game_section((const char *)str_accelerated,1);
      dlg.set_default_choice((int)prefs.accelerated_start);
      dlg.set_position(
                         -(((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3)
                          + 1),-(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U))
                                 >> 3) + 1));
      accel_choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    } while (accel_choice < 0);
    if (choice != 0) {
      prefs.accelerated_start = (short)accel_choice;
    }
    do {
      setup_restore_leader_labels();
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,6);
      dlg.load_game_section((const char *)str_gender,1);
      dlg.set_default_choice((int)prefs.gender);
      dlg.set_position(0xfffffc19,
                         -(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3)
                          + 1));
      choice = dlg.show_modal(0);
      /* BF-113 (Peter Johnson's ledger): a choice past the two genders (a row
         added to GAME.TXT's @GENDER) is no choice (the original indexed the
         leader's names and titles with it) */
      if (1 < choice) {
        choice = -1;
      }
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) break;  /* back to the difficulty dialog */
      prefs.gender = (short)choice;
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,7);
      dlg_err = dlg.load_text((const char *)&str_game,(const char *)str_tribe,1);
      if (dlg_err != 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        return result;
      }
      dlg.set_columns(3);
      for (leader_i = 0; leader_i < 0x15; leader_i = leader_i + 1) {
        option_arg = 0;
        leader_arg = leader_i;
        label_text = label_string((int)leaders[leader_i].plural);
        dlg.add_option((const char *)label_text,leader_arg,option_arg);
      }
      dlg.set_default_choice((int)prefs.tribe);
      dlg.set_position(0xfffffc19,
                         -(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3)
                          + 1));
      tribe_choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (-1 < tribe_choice) {
        game.player_civ = leaders[tribe_choice].color;
        prefs.tribe = (short)tribe_choice;
        civs[game.player_civ].tribe = prefs.tribe;
        civs[game.player_civ].leader_portrait = (i8)tribe_choice;
        if (choice != 0) {
          civs[game.player_civ].leader_portrait =
               civs[game.player_civ].leader_portrait + '\x15';
        }
        human_civ[0] = (int)game.player_civ;
        game.human_players = (byte)(1 << (game.player_civ & 0x1f));
        female_flag = (i8)choice;
        leaders[civs[game.player_civ].tribe].female = female_flag;
        leaders[civs[game.player_civ].tribe].leader_name
             = leaders[civs[game.player_civ].tribe].names[(uint)(&leaders[0].female)
                            [civs[game.player_civ].tribe * 0x30]];
        label_text = label_string((int)leaders[civs[game.player_civ].tribe].leader_name);
        crt_strcpy(&civ_names[game.player_civ].leader_name[0],label_text);
        leaders[civs[game.player_civ].tribe].leader_name =
             -leaders[civs[game.player_civ].tribe].leader_name;
        if (dlg.result_extra == 0) {
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,7);
          dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name,0x17,
                             &civ_names[game.player_civ].leader_name[0],0,0,0,1);
          dlg.set_position(0xfffffc19,
                             -(((int)(caption_font_block.screen_height + -0x1e0 + (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >>
                               3) + 1));
          dlg_err = dlg.run_modal(custom_name,0);
          snd_play_effect(SND_MENUOK,0,0,0);
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
          if (dlg_err != 0) continue;
          crt_strcpy(&civ_names[game.player_civ].leader_name[0],custom_name);
        }
        else {
          label_text = label_string((int)leaders[civs[game.player_civ].tribe].plural);
          crt_strcpy(&civ_names[game.player_civ].tribe_name[0],label_text);
          label_text = label_string((int)leaders[civs[game.player_civ].tribe].adjective);
          crt_strcpy(&civ_names[game.player_civ].adjective[0],label_text);
          leaders[civs[game.player_civ].tribe].plural =
               -leaders[civs[game.player_civ].tribe].plural;
          leaders[civs[game.player_civ].tribe].adjective =
               -leaders[civs[game.player_civ].tribe].adjective;
          for (title_i = 0; title_i < 7; title_i = title_i + 1) {
            label_text = label_string((int)leaders[civs[game.player_civ].tribe].titles[title_i][choice]);
            crt_strcpy(&civ_names[game.player_civ].titles[title_i][0],label_text);
            leaders[civs[game.player_civ].tribe].titles[title_i][choice] =
                 -leaders[civs[game.player_civ].tribe].titles[title_i][choice]
            ;
          }
          next_step = setup_custom_tribe_dialogs(dlg);
          if (next_step == 1) continue;  /* back to the leader's gender */
          if (next_step == 2) return result;
        }
        for (;;) {  /* the city style, again when the opponents dialog goes back */
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(1,6,8);
          dlg.load_game_section((const char *)str_customcity,0x20001);
          for (style_i = 0; style_i < 4; style_i = style_i + 1) {
            label_text = label_string(label_table.string_ids[0xf6 + style_i]);
            dlg.add_sprite((Sprite *)(style_i * 0x1e0 + (iptr)(((u8 *)&gfx_sheet_surface_block.city_walled_sprites + 360))),style_i,(const char *)label_text);
          }
          dlg.set_default_choice(
                             (int)leaders[civs[game.player_civ].tribe].city_style);
          dlg.set_position(
                             -(((int)(caption_font_block.screen_width + -0x280 + (caption_font_block.screen_width + -0x280 >> 0x1f & 7U)) >> 3
                               ) + 1),-(((int)(caption_font_block.screen_height + -0x1e0 +
                                              (caption_font_block.screen_height + -0x1e0 >> 0x1f & 7U)) >> 3) + 1));
          choice = dlg.show_modal(0);
          snd_play_effect(SND_MENUOK,0,0,0);
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
          if (choice < 0) break;  /* back to the leader's gender */
          /* BF-102 (Peter Johnson's ledger): a choice past the four styles
             (a row added to GAME.TXT's @CUSTOMCITY) is asked again */
          if (3 < (choice & 0xffff)) continue;
          civ_names[game.player_civ].city_style = (short)choice;
          game.civs_alive = game.human_players | 1;
          next_step = setup_choose_opponents(dlg,pick_opponents,leader_i);
          if (next_step != 0) continue;
          prefs_save_civ2dat();
          rules_load_advances(1);
          rules_load_units(1);
          rules_load_improvements(1);
          snd_menu_end();
          map_generate(setup_mode);
          game_init_new();
          if (accel_choice != 0) {
            game_accelerated_start(accel_choice + -1);
          }
          civ_intro_popup(game.player_civ);
          result = 0;
          return result;
        }
      }
    } while( true );
  }
  label = result;
  return label;
}

/* The state render_ship shares with its parts: a spaceship structure row
   (the 88 rows the original builds on its stack) and the twelve GIF surfaces. */
struct SpaceshipWindow::ShipPartRow { u8 kind; u8 count; u8 pad[2]; i32 v[6]; };
struct SpaceshipWindow::ShipSurfaces {   /* the surfaces (render_ship's locals) */
  i8 *surf_part;
  i8 *surf_1b9;
  i8 *surf_1e0;
  i8 *surf_1c7;
  i8 *surf_1ea;
  i8 *surf_1d6;
  i8 *surf_1eb;
  i8 *surf_1ba;
  i8 *surf_1e1;
  i8 *surf_1c8;
  i8 *surf_1e9;
  i8 *surf_1d7;
};

/* Part of SpaceshipWindow::render_ship (004d17bf): constructs the surfaces. */
void SpaceshipWindow::render_ship_construct_surfaces(ShipSurfaces &sf,int &unwind_state)
{
  surface_ctor((DibSurface *)sf.surf_part);
  surface_ctor((DibSurface *)sf.surf_1b9);
  surface_ctor((DibSurface *)sf.surf_1e0);
  surface_ctor((DibSurface *)sf.surf_1c7);
  surface_ctor((DibSurface *)sf.surf_1ea);
  surface_ctor((DibSurface *)sf.surf_1d6);
  surface_ctor((DibSurface *)sf.surf_1eb);
  surface_ctor((DibSurface *)sf.surf_1ba);
  surface_ctor((DibSurface *)sf.surf_1e1);
  surface_ctor((DibSurface *)sf.surf_1c8);
  surface_ctor((DibSurface *)sf.surf_1e9);
  surface_ctor((DibSurface *)sf.surf_1d7);
}

/* The spaceship structure rows render_ship draws (kind, count needed, GIF
   resource, position, size, source x), as the original wrote them field by
   field into its frame. */
static const SpaceshipWindow::ShipPartRow ship_part_rows[88] = {
  {2,  6, {0, 0}, {0x1cf, 0xb2, 0xaa, 0x2b, 0x33, 0x204}},
  {2,  6, {0, 0}, {0x1d0, 0x189, 0x68, 0x3d, 0x28, 0x230}},
  {0,  7, {0, 0}, {0x197, 0x1df, 0x13c, 0x3a, 0x22, 0x44}},
  {2,  2, {0, 0}, {0x1c9, 0x1de, 0x144, 0x19, 0x36, 0x92}},
  {2,  2, {0, 0}, {0x1ca, 0x1fa, 0x139, 0x26, 0x27, 0xac}},
  {0,  9, {0, 0}, {0x199, 0x1e2, 0x145, 0x42, 0x37, 0x7f}},
  {1,  2, {0, 0}, {0x1bb, 0x1df, 0x13f, 0x4d, 0x4e, 0x69}},
  {0,  0, {0, 0}, {0x190, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  1, {0, 0}, {0x191, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  2, {0, 0}, {0x192, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  3, {0, 0}, {0x193, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  4, {0, 0}, {0x194, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  6, {0, 0}, {0x196, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0,  8, {0, 0}, {0x198, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0, 10, {0, 0}, {0x19a, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0, 12, {0, 0}, {0x19c, 0x138, 0x9d, 0xfd, 0xf0, 0x0}},
  {0, 14, {0, 0}, {0x19e, 0x137, 0x9d, 0xfe, 0xf0, 0x0}},
  {0, 16, {0, 0}, {0x1a0, 0x137, 0x9d, 0xfe, 0xf0, 0x0}},
  {0, 18, {0, 0}, {0x1a2, 0x137, 0x9d, 0xfe, 0xf0, 0x0}},
  {0, 20, {0, 0}, {0x1a5, 0x137, 0x9d, 0xfe, 0xf0, 0x0}},
  {0, 22, {0, 0}, {0x1a8, 0x137, 0x9d, 0x115, 0xf0, 0x0}},
  {0, 23, {0, 0}, {0x1a9, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 24, {0, 0}, {0x1aa, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 25, {0, 0}, {0x1ab, 0xba, 0x72, 0x192, 0x11b, 0x0}},
  {0, 26, {0, 0}, {0x1ac, 0xba, 0x72, 0x192, 0x11b, 0x0}},
  {0, 27, {0, 0}, {0x1ad, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 28, {0, 0}, {0x1ae, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 30, {0, 0}, {0x1b1, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 32, {0, 0}, {0x1b4, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 36, {0, 0}, {0x1b7, 0xbb, 0x72, 0x191, 0x11b, 0x0}},
  {0, 38, {0, 0}, {0x1b8, 0xba, 0x72, 0x192, 0x11b, 0x0}},
  {1,  6, {0, 0}, {0x1c1, 0x1ba, 0x75, 0x21, 0x2b, 0x1ce}},
  {5,  4, {0, 0}, {0x1f0, 0x18e, 0xa5, 0x30, 0x16, 0x190}},
  {3,  3, {0, 0}, {0x1d9, 0x17d, 0xa4, 0x3e, 0x35, 0xd2}},
  {4,  4, {0, 0}, {0x1e7, 0x1c5, 0xa9, 0x30, 0x24, 0x168}},
  {4,  3, {0, 0}, {0x1e5, 0x1e0, 0xbd, 0x32, 0x24, 0x103}},
  {5,  4, {0, 0}, {0x1f1, 0x1ad, 0xb4, 0x47, 0x26, 0x1c1}},
  {4,  1, {0, 0}, {0x1e1, 0x1fc, 0xd2, 0x2b, 0x20, 0x30}},
  {4,  2, {0, 0}, {0x1e3, 0x211, 0xe1, 0x37, 0x2a, 0x96}},
  {5,  2, {0, 0}, {0x1ec, 0x1af, 0xc7, 0x4a, 0x24, 0x97}},
  {3,  2, {0, 0}, {0x1d7, 0x1af, 0xce, 0x45, 0x3a, 0x4a}},
  {5,  2, {0, 0}, {0x1ed, 0x1ca, 0xe9, 0x50, 0x2f, 0xe2}},
  {1,  6, {0, 0}, {0x1c0, 0xb2, 0xc8, 0x3e, 0x28, 0x18f}},
  {5,  3, {0, 0}, {0x1ee, 0x12c, 0xc5, 0x12, 0x18, 0x133}},
  {3,  4, {0, 0}, {0x1d8, 0x136, 0xbc, 0x41, 0x38, 0x90}},
  {5,  1, {0, 0}, {0x1ea, 0x153, 0xe8, 0x25, 0x29, 0x1}},
  {5,  3, {0, 0}, {0x1ef, 0x12c, 0xd9, 0x49, 0x3c, 0x146}},
  {3,  1, {0, 0}, {0x1d6, 0x164, 0xe9, 0x48, 0x40, 0x1}},
  {5,  1, {0, 0}, {0x1eb, 0x15e, 0xfb, 0x6f, 0x45, 0x27}},
  {4,  4, {0, 0}, {0x1e6, 0x109, 0xed, 0x31, 0x2c, 0x136}},
  {4,  3, {0, 0}, {0x1e4, 0x11f, 0x107, 0x34, 0x2e, 0xce}},
  {4,  1, {0, 0}, {0x1e0, 0x137, 0x122, 0x2e, 0x28, 0x1}},
  {4,  2, {0, 0}, {0x1e2, 0x148, 0x136, 0x39, 0x35, 0x5c}},
  {2,  8, {0, 0}, {0x1d4, 0x20e, 0xf6, 0x51, 0x36, 0x334}},
  {0, 33, {0, 0}, {0x1b6, 0x24b, 0x114, 0x15, 0x19, 0x2d1}},
  {1,  8, {0, 0}, {0x1c5, 0x246, 0x111, 0x2f, 0x2b, 0x285}},
  {0, 31, {0, 0}, {0x1b3, 0x210, 0xfd, 0x30, 0x26, 0x282}},
  {2,  7, {0, 0}, {0x1d2, 0x1f7, 0x102, 0x4d, 0x3a, 0x2ab}},
  {0, 29, {0, 0}, {0x1b0, 0x22d, 0x123, 0x19, 0x19, 0x243}},
  {1,  7, {0, 0}, {0x1c3, 0x228, 0x11d, 0x2f, 0x2e, 0x222}},
  {2,  5, {0, 0}, {0x1ce, 0x1f1, 0xfd, 0x4e, 0x39, 0x1b5}},
  {0, 21, {0, 0}, {0x1a7, 0x228, 0x11d, 0x18, 0x1a, 0x20c}},
  {1,  5, {0, 0}, {0x1bf, 0x224, 0x118, 0x31, 0x2e, 0x15d}},
  {0, 19, {0, 0}, {0x1a4, 0x1f0, 0x107, 0x34, 0x35, 0x1b9}},
  {2,  1, {0, 0}, {0x1c8, 0x1da, 0x10b, 0x4b, 0x41, 0x46}},
  {0, 11, {0, 0}, {0x19b, 0x1cf, 0x108, 0x42, 0x3d, 0xc2}},
  {2,  3, {0, 0}, {0x1cb, 0x1ce, 0x109, 0x4d, 0x40, 0xd3}},
  {2,  1, {0, 0}, {0x1c7, 0x1bc, 0x11b, 0x44, 0x44, 0x1}},
  {0,  5, {0, 0}, {0x195, 0x1e4, 0x12f, 0x42, 0x31, 0x1}},
  {0, 13, {0, 0}, {0x19d, 0x1fb, 0x12b, 0x20, 0x1f, 0x105}},
  {1,  1, {0, 0}, {0x1ba, 0x204, 0x129, 0x2f, 0x33, 0x39}},
  {1,  3, {0, 0}, {0x1bc, 0x1f9, 0x125, 0x37, 0x35, 0xb7}},
  {1,  1, {0, 0}, {0x1b9, 0x1df, 0x13e, 0x33, 0x34, 0x1}},
  {0, 19, {0, 0}, {0x1a3, 0x1be, 0x11f, 0x2c, 0x40, 0x18c}},
  {2,  5, {0, 0}, {0x1cd, 0x1ac, 0x117, 0x42, 0x46, 0x172}},
  {0, 21, {0, 0}, {0x1a6, 0x1d1, 0x142, 0x1d, 0x1d, 0x1ee}},
  {1,  5, {0, 0}, {0x1be, 0x1cd, 0x13c, 0x34, 0x33, 0x128}},
  {2,  7, {0, 0}, {0x1d1, 0x19d, 0x12c, 0x3c, 0x4b, 0x26e}},
  {0, 31, {0, 0}, {0x1b2, 0x19e, 0x12b, 0x24, 0x41, 0x25d}},
  {0, 29, {0, 0}, {0x1af, 0x1bd, 0x15c, 0x1d, 0x1d, 0x225}},
  {1,  7, {0, 0}, {0x1c2, 0x1b9, 0x157, 0x31, 0x34, 0x1f0}},
  {2,  8, {0, 0}, {0x1d3, 0x188, 0x129, 0x3a, 0x4e, 0x2f9}},
  {0, 33, {0, 0}, {0x1b5, 0x1a5, 0x15c, 0x1c, 0x1e, 0x2b3}},
  {1,  8, {0, 0}, {0x1c4, 0x1a0, 0x156, 0x32, 0x35, 0x252}},
  {0, 15, {0, 0}, {0x19f, 0x1d1, 0x107, 0x43, 0x2a, 0x126}},
  {2,  4, {0, 0}, {0x1cc, 0x1cf, 0x101, 0x50, 0x32, 0x121}},
  {0, 17, {0, 0}, {0x1a1, 0x201, 0x11a, 0x21, 0x1a, 0x16a}},
  {1,  4, {0, 0}, {0x1bd, 0x1fb, 0x116, 0x38, 0x2d, 0xef}},
};

/* Part of SpaceshipWindow::render_ship (004d17bf): the spaceship structure rows
   (kind, count needed, GIF resource, position, size, source x). */
void SpaceshipWindow::render_ship_init_rows(ShipPartRow *part_rows)
{
  int row;
  int i;

  /* field by field, as the original: the rows' padding keeps what the stack held */
  for (row = 0; row < 88; row++) {
    part_rows[row].kind = ship_part_rows[row].kind;
    part_rows[row].count = ship_part_rows[row].count;
    for (i = 0; i < 6; i++) {
      part_rows[row].v[i] = ship_part_rows[row].v[i];
    }
  }
}

/* Part of SpaceshipWindow::render_ship (004d17bf): destroys the surfaces (the
   original's unwind funclets, inlined at each exit). */
void SpaceshipWindow::render_ship_free_surfaces(ShipSurfaces &sf,int &unwind_state)
{
  (*((char *)&unwind_state + 0)) = 10;
  /* inlined unwind funclet 004d49d5 */ { surface_dtor((DibSurface *)sf.surf_1d7); }
  (*((char *)&unwind_state + 0)) = 9;
  /* inlined unwind funclet 004d49e1 */ { surface_dtor((DibSurface *)sf.surf_1e9); }
  (*((char *)&unwind_state + 0)) = 8;
  /* inlined unwind funclet 004d49ed */ { surface_dtor((DibSurface *)sf.surf_1c8); }
  (*((char *)&unwind_state + 0)) = 7;
  /* inlined unwind funclet 004d49f9 */ { surface_dtor((DibSurface *)sf.surf_1e1); }
  (*((char *)&unwind_state + 0)) = 6;
  /* inlined unwind funclet 004d4a05 */ { surface_dtor((DibSurface *)sf.surf_1ba); }
  (*((char *)&unwind_state + 0)) = 5;
  /* inlined unwind funclet 004d4a11 */ { surface_dtor((DibSurface *)sf.surf_1eb); }
  (*((char *)&unwind_state + 0)) = 4;
  /* inlined unwind funclet 004d4a1d */ { surface_dtor((DibSurface *)sf.surf_1d6); }
  (*((char *)&unwind_state + 0)) = 3;
  /* inlined unwind funclet 004d4a29 */ { surface_dtor((DibSurface *)sf.surf_1ea); }
  (*((char *)&unwind_state + 0)) = 2;
  /* inlined unwind funclet 004d4a35 */ { surface_dtor((DibSurface *)sf.surf_1c7); }
  (*((char *)&unwind_state + 0)) = 1;
  /* inlined unwind funclet 004d4a3e */ { surface_dtor((DibSurface *)sf.surf_1e0); }
  unwind_state = unwind_state & 0xffffff00;
  /* inlined unwind funclet 004d4a4a */ { surface_dtor((DibSurface *)sf.surf_1b9); }
  unwind_state = 0xffffffff;
  /* inlined unwind funclet 004d4a56 */ { surface_dtor((DibSurface *)sf.surf_part); }
}

/* Part of SpaceshipWindow::render_ship (004d17bf): loads the part GIFs; 0 when
   one fails. */
int SpaceshipWindow::render_ship_load_surfaces(ShipSurfaces &sf)
{
  int load_ok;

  load_ok = ((DibSurface *)sf.surf_1b9)->load_gif_resource(0x1b9,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1e0)->load_gif_resource(0x1e0,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1c7)->load_gif_resource(0x1c7,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1ea)->load_gif_resource(0x1ea,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1d6)->load_gif_resource(0x1d6,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1ba)->load_gif_resource(0x1ba,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1e1)->load_gif_resource(0x1e1,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1c8)->load_gif_resource(0x1c8,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1e9)->load_gif_resource(0x1e9,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1d7)->load_gif_resource(0x1d7,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_1eb)->load_gif_resource(0x1eb,10,0xec,(GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    return 0;
  }
  return 1;
}

/* Part of SpaceshipWindow::render_ship (004d17bf): draws one part of the ship
   onto sprite (when the civ has built it: its full or its empty picture). */
void SpaceshipWindow::render_ship_draw_part(ShipSurfaces &sf,ShipPartRow *row,i8 *sprite)
{
  byte part_count;
  int load_ok;

  if ((int)(uint)row->count <=
      editor_text_edit_block.spaceship_wnd_part_count_list[(uint)row->kind]) {
    if (row->kind == 1) {
      part_count = row->count;
      load_ok = spaceship_part_count(ship_civ,1);
      if (load_ok < (int)(uint)part_count) {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1ba,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
      else {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1b9,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
    }
    else if (row->kind == 4) {
      part_count = row->count;
      load_ok = spaceship_part_count(ship_civ,4);
      if (load_ok < (int)(uint)part_count) {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1e1,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
      else {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1e0,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
    }
    else if (row->kind == 2) {
      part_count = row->count;
      load_ok = spaceship_part_count(ship_civ,2);
      if (load_ok < (int)(uint)part_count) {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1c8,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
      else {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1c7,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
    }
    else if (row->kind == 5) {
      part_count = row->count;
      load_ok = spaceship_part_count(ship_civ,5);
      if (load_ok < (int)(uint)part_count) {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1e9,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
      else {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1ea,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
    }
    else if (row->kind == 3) {
      part_count = row->count;
      load_ok = spaceship_part_count(ship_civ,3);
      if (load_ok < (int)(uint)part_count) {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1d7,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
      else {
        ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1d6,9,
                     row->v[5],1,
                     row->v[3],
                     row->v[4]);
      }
    }
    else {
      ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_1eb,9,
                   row->v[5],1,
                   row->v[3],
                   row->v[4]);
    }
    ship_image_sprite.draw((RECT *)sprite,&ship_view_surface,
                 row->v[1] + -0xaf,
                 row->v[2] + -99);
  }
}

/* 0x004d17bf: unwind funclet 0x004d4a11 destroys the control at EBP-0x12c (surf_1eb). */
i32 SpaceshipWindow::render_ship()
{
  int load_ok;
  i8 sprite_final[16];
  i8 sprite_part[16];
  i8 sprite_struct[16];
  i8 sprite_core[16];
  i8 surf_1ea[72];
  i8 surf_1d6[72];
  ShipPartRow part_rows[88];  /* the spaceship structure rows */
  i8 surf_part[72];
  int row_i;
  i8 surf_1b9[72];
  i8 surf_1c8[72];
  i8 surf_1e1[72];
  i8 surf_1e0[72];
  int struct_row;
  i8 surf_1ba[72];
  i8 surf_1eb[72];
  i8 surf_1e9[72];
  i8 surf_1d7[72];
  i8 surf_1c7[72];
  int unwind_state;
  ShipSurfaces sf = { surf_part,surf_1b9,surf_1e0,surf_1c7,surf_1ea,surf_1d6,surf_1eb,surf_1ba,
                      surf_1e1,surf_1c8,surf_1e9,surf_1d7 };

  render_ship_construct_surfaces(sf,unwind_state);
  render_ship_init_rows(part_rows);
  if (render_ship_load_surfaces(sf) == 0) {
    render_ship_free_surfaces(sf,unwind_state);
    return 0;
  }
  win_flush_pending_msgs();
  for (row_i = 0; row_i < 7; row_i = row_i + 1) {
    win_flush_pending_msgs();
    render_ship_draw_part(sf,&part_rows[row_i],sprite_core);
  }
  struct_row = 0;
  for (row_i = 7; row_i < 0x1f; row_i = row_i + 1) {
    if ((int)(uint)part_rows[row_i].count <=
        editor_text_edit_block.spaceship_wnd_part_count_list[(uint)part_rows[row_i].kind]) {
      struct_row = row_i;
    }
  }
  win_flush_pending_msgs();
  load_ok = ((DibSurface *)sf.surf_part)->load_gif_resource(part_rows[struct_row].v[0],10,0xec,
                       (GamePalette *)((iptr)this + 0x1f8));
  if (load_ok == 0) {
    render_ship_free_surfaces(sf,unwind_state);
    return 0;
  }
  win_flush_pending_msgs();
  ship_image_sprite.grab_from_surface((DibSurface *)sf.surf_part,9,
               part_rows[struct_row].v[1],part_rows[struct_row].v[2],
               part_rows[struct_row].v[3],part_rows[struct_row].v[4]);
  ship_image_sprite.draw((RECT *)sprite_struct,&ship_view_surface,
               part_rows[struct_row].v[1] + -0xaf,
               part_rows[struct_row].v[2] + -99);
  for (row_i = 0x1f; row_i < 0x58; row_i = row_i + 1) {
    win_flush_pending_msgs();
    win_flush_pending_msgs();
    render_ship_draw_part(sf,&part_rows[row_i],sprite_part);
  }
  ship_image_sprite.grab_from_surface((DibSurface *)((iptr)this + 0x8f4),9,0,0,0x1c9,
               0x130);
  win_flush_pending_msgs();
  if ((int)launch_state == 3) {
    surface_wnd.fill(0);
  }
  ship_image_sprite.draw((RECT *)sprite_final,&surface_wnd,0xaf,
               99);
  spaceship_wnd_draw_title(ship_civ);
  render_ship_free_surfaces(sf,unwind_state);
  return 1;
}

/* Part of net_host_turn_loop (005aa0e5): logs the server and the clients, centers the map
   on one of the host's units (or cities) and finds the civ a resumed turn starts
   with. */
static void net_host_begin(void)
{
  byte resume_start;
  i32 civ_name;
  int result;
  int civ;
  int unit_idx;
  int found_owned;

  result = human_civ[0];
  civ_name = (i32)(civ_name_plural(human_civ[0]));
  dbg_logf_str_int((const char *)str_server_s_d_3,(const char *)civ_name,result);
  for (civ = 1; civ < 8; civ = civ + 1) {
    if (((human_civ[0] != civ) && ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0))
       && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      result = civ;
      civ_name = (i32)(civ_name_plural(civ));
      dbg_logf_str_int((const char *)str_client_s_d_3,(const char *)civ_name,result);
    }
  }
  net_reset_error_counts();
  map_rebuild_unit_flags();
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  ui_redraw_all_maps(1);
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  found_owned = 0;
  for (unit_idx = 0; unit_idx < game.num_units; unit_idx = unit_idx + 1) {
    if ((units[unit_idx].id != 0) &&
       (units[unit_idx].owner == human_civ[0])) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit_idx].x,
                         (int)units[unit_idx].y);
      found_owned = found_owned + 1;
      break;
    }
  }
  if (found_owned == 0) {
    for (unit_idx = 0; unit_idx < game.num_cities; unit_idx = unit_idx + 1) {
      if ((cities[unit_idx].id != 0) &&
         (cities[unit_idx].owner == human_civ[0])) {
        ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)cities[unit_idx].x,
                           (int)cities[unit_idx].y);
        break;
      }
    }
  }
  resume_start = (*(byte *)&turn_loop.resume_civ);
  if ((*(byte *)&turn_loop.resume_civ) != 0) {
    while ((1 << ((*(byte *)&turn_loop.resume_civ) & 0x1f) & (uint)game.civs_alive) == 0) {
      (*(byte *)&turn_loop.resume_civ) = (*(byte *)&turn_loop.resume_civ) + 1;
      if ((*(byte *)&turn_loop.resume_civ) == 8) {
        (*(byte *)&turn_loop.resume_civ) = 0;
      }
      if ((*(byte *)&turn_loop.resume_civ) == resume_start) {
        game_over_reason = 1;
        break;
      }
    }
  }
}

/* Part of net_host_turn_loop (005aa0e5): waits until every human player has sent the
   current phase done (or only one player is left).  civ is the caller's: the
   value the search leaves is read after the second wait. */
static void net_host_wait_phase_done(int &civ)
{
  int done;

  done = 0;
  while (done == 0) {
    net.process_messages(1,0);
    if (net.net_player_count[0] < 2) {
      done = 1;
      break;
    }
    done = 1;
    for (civ = 1; civ < 8; civ = civ + 1) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        done = 0;
        break;
      }
    }
  }
}

/* Part of net_host_turn_loop (005aa0e5): the AI civs move their units. */
static void net_host_ai_units_phase(void)
{
  int civ;

  for (civ = 0; (turn_loop.active != '\0' && (civ < 8)); civ = civ + 1) {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
      game.turn_civ = (byte)civ;
      net.mp_turn_civ = civ;
      menu_update_states();
      status_draw_civ_color(civ);
      if (((((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) && ((*(byte *)&turn_loop.resume_civ) != 0)) &&
         (turn_resume_new_game != 0)) {
        ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
        map_redraw_all_views(human_civ[0],1);
        wnd_flush_paint();
      }
      ai_process_civ_units();
    }
  }
}

/* Part of net_host_turn_loop (005aa0e5): the host's turn: the intro, the turn timer, the
   caption, the casualties or "our turn to move" popup, then the host plays its
   turn (or autoplay moves its units). */
static void net_host_human_turn(Dialog &dlg)
{
  SHORT esc_state;
  i32 civ_name;
  i32 rect_left2;
  i32 rect_top2;
  i32 rect_right2;
  i32 rect_bottom2;
  int new_casualties;

  if ((civ_view.intro_shown[human_civ[0]] == 0) && (autoplay.active == 0)) {
    civ_intro_popup(human_civ[0]);
    civ_view.intro_shown[human_civ[0]] = 1;
  }
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
    map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  }
  net.mp_turn_civ = human_civ[0];
  menu_update_states();
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  rect_left2 = map_views_block.main_view_frame_left;
  rect_top2 = map_views_block.main_view_frame_top;
  rect_right2 = map_views_block.main_view_frame_right;
  rect_bottom2 = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((RECT *)&rect_left2);
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  map_redraw_all_views(human_civ[0],1);
  status_draw_civ_color(human_civ[0]);
  new_casualties = net.casualty_counts[human_civ[0]] -
             net.casualty_reported_counts[human_civ[0]];
  if (autoplay.active == 0) {
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    civ_name = civ_leader_title(human_civ[0]);
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (i32)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)civ_name);
    if (net.casualty_reported_counts[human_civ[0]] < net.casualty_counts[human_civ[0]]) {
      net.casualty_reported_counts[human_civ[0]] =
           net.casualty_counts[human_civ[0]];
      text_set_number_arg(0,new_casualties);
      if (new_casualties == 1) {
        dlg.load_text_noflags((const char *)&str_game_7,(const char *)str_casualty_4);
      }
      else {
        dlg.load_text_noflags((const char *)&str_game_7,(const char *)str_casualties_4);
      }
      dlg.add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
      dlg.set_image_zoom(0);
      input_flush_events();
      snd_play_effect(SND_NEWONDER,0,0,0);
      dlg.show_modal(0);
      if (dlg.result_extra != 0) {
        casualty_timeline_show(human_civ[0]);
      }
    }
    else {
      dlg.load_text_noflags((const char *)&str_game_7,(const char *)str_ourturntomove_3);
      dlg.add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
      dlg.set_image_zoom(0);
      input_flush_events();
      snd_play_effect(SND_NEWONDER,0,0,0);
      dlg.show_modal(0);
    }
  }
  if (autoplay.active == 0) {
    game_turn_start_popups();
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    game_player_move_loop();
  }
  else {
    map_views_block.saved_human_players = (uint)game.human_players;
    game.human_players = 0;
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    ai_process_civ_units();
    game.human_players = (*(byte *)&map_views_block.saved_human_players);
    esc_state = GetAsyncKeyState(0x1b);
    if (((int)esc_state & 0x8001U) != 0) {
      autoplay.turns_left = 1;
    }
    autoplay.turns_left = autoplay.turns_left + -1;
    if (autoplay.turns_left == 0) {
      autoplay.active = 0;
    }
  }
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    mp_turn_timer_stop(1);
  }
}

/* Part of net_host_turn_loop (005aa0e5): the end of the turn: civ's units have no moves
   left, the cities' flag 0x400000 is cleared. */
static void net_host_exhaust_moves(int civ)
{
  int city_idx;
  int unit_idx;

  for (unit_idx = 0; unit_idx < game.num_units; unit_idx = unit_idx + 1) {
    if ((units[unit_idx].id != 0) &&
       (units[unit_idx].owner == civ)) {
      unit_exhaust_moves(unit_idx);
    }
  }
  city_idx = (int)game.num_cities;
  while (city_idx = city_idx + -1, -1 < city_idx) {
    if (cities[city_idx].id != 0) {
      cities[city_idx].flags =
           cities[city_idx].flags & ~CITYF_INVESTIGATED;
    }
  }
}

/* 0x005aa0e5: unwind funclet 0x005ab07f destroys the object at EBP-0x320 (dlg). */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_host_turn_loop(void)

{
  int result;
  int civ;
  i32 unused_pad;
  
  Dialog dlg(0x4000);
  net_host_begin();
  do {
    game.human_players = net.pending_human_players | game.human_players;
    net.pending_human_players = 0;
    _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots),-1,0xc0);
    units_reset_for_turn(0xfffffffd);
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(60000);
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
    game_calc_human_wonder_range();
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      game_advance_turn(0xfffffffd);
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(60000);
    }
    result = game_check_retirement();
    if ((result != 0) || (game_over_reason != 0)) {
      turn_loop.active = '\0';
      break;
    }
    if ((scenario_events_loaded != 0) && (((*(byte *)&turn_loop.resume_civ) == 0 || (turn_resume_new_game != 0)))) {
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn_interval((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_random_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_received_tech();
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    game.human_players = (net.pending_human_players | game.human_players) & (byte)civ_saved_cursor_x_block.net_claimed_civs_mask;
    net.pending_human_players = 0;
    net.tile_sync_enabled = 0;
    for (civ = 0; (turn_loop.active != '\0' && (civ < 8)); civ = civ + 1) {
      game.turn_civ = (byte)civ;
      if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
        civ_process_turn(civ);
        net_send_state_diff(0xff,2,0,0,0);
      }
    }
    net.tile_sync_enabled = 1;
    XD_FlushSendBuffer(60000);
    net_reset_msg_counts();
    for (civ = 0; civ < 8; civ = civ + 1) {
      dialog_text_font_obj_block.net_civ_phase_done[civ] = 0;
    }
    net_send_message(NM_PRODUCTION_BEGIN,0xff,0,0,0,0,0,0,0,0);
    human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
    game.player_civ = (byte)human_civ[0];
    game.turn_civ = (byte)human_civ[0];
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      civ_check_throne_room_upgrade();
    }
    civ_process_turn(human_civ[0]);
    net_send_state_diff(0xff,2,0,0,0);
    dialog_text_font_obj_block.net_civ_phase_done[human_civ[0]] = dialog_text_font_obj_block.net_civ_phase_done[human_civ[0]] + 1;
    dlg_style.pending_idle_callback = net_host_wait_production_idle;
    popup_game_with_sprite((const char *)str_waitproduction,(Sprite *)&popup_info_icon_sprite,0);
    net_host_wait_phase_done(civ);
    net.nm_production_complete_count = 0;
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(60000);
    net_send_message(NM_AI_MOVEMENT,0xff,0,0,0,0,0,0,0,0);
    human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
    game.player_civ = (byte)human_civ[0];
    net.tile_sync_enabled = 0;
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      units_reset_for_turn(0xffffffff);
    }
    net_host_ai_units_phase();
    for (civ = 0; civ < 8; civ = civ + 1) {
      dialog_text_font_obj_block.net_civ_phase_done[civ] = 0;
      net.civ_new_player_flags[civ] = 0;
    }
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_HUMAN_MOVEMENT,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    game.turn_civ = (byte)human_civ[0];
    net.tile_sync_enabled = 1;
    if (turn_loop.active == '\0') {
      net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
      break;
    }
    net_host_human_turn(dlg);
    status_redraw(1);
    net_send_state_diff(0xff,2,0,0,0);
    saved_turn.civ = 0;
    (*(byte *)&turn_loop.resume_civ) = 0;
    saved_turn.resume_new_game = 0;
    turn_resume_new_game = 0;
    game.field_06 = game.field_06 & 0xfffe;
    dialog_text_font_obj_block.net_civ_phase_done[human_civ[0]] = dialog_text_font_obj_block.net_civ_phase_done[human_civ[0]] + 1;
    XD_FlushSendBuffer(60000);
    dlg_style.pending_idle_callback = net_host_wait_human_moves_idle;
    popup_game_with_sprite((const char *)str_waithumanmoves,(Sprite *)&popup_info_icon_sprite,0);
    net_host_wait_phase_done(civ);
    net.nm_human_move_complete_count = 0;
    net_check_lost_connections();
    if ((net.player_has_quit == '\0') || (net.mp_turn_civ == human_civ[0])) {
      net_host_exhaust_moves(civ);
    }
    ui_pump_messages();
    if (turn_loop.active == '\0') break;
    if ((turn_loop.active != '\0') && (game_over_reason != 0)) {
      turn_loop.active = '\0';
      break;
    }
  } while (turn_loop.active != '\0');
  mp_turn_timer_stop(0);
  net_host_quit_handoff();
  if (game_over_reason != 0) {
    game_handle_game_over();
  }
  return;
}

/* 0x0055f5a3: pick a starting/setup choice for civ civ2 by scoring options
 * 1..n with civ_can_choose_government and taking the best score (ties go to the later one).
 * Ghidra lost the stack frame and passed garbage register/stack values to
 * civ_can_choose_government; the real call is civ_can_choose_government(civ, i). */
void __cdecl ai_choose_government(int civ2, int limit_choices)
{
    int count;          /* EBP-4 */
    int best_score;     /* EBP-8 */
    int diff;           /* EBP-0xc */
    int i;              /* EBP-0x10 */
    int best;           /* EBP-0x14 */

    if ((game.game_flags & GAME_SCENARIO) != 0 && ((short)scenario.flags[0] & 0x10) != 0 &&
        civs[civ2].government >= GOV_DESPOTISM)
        return;
    count = 6;
    if (limit_choices != 0) {
        count--;
        if (_rand() % 3 != 0)
            count--;
    }
    if ((game.game_flags & GAME_AI_GANG_UP) != 0) {
        diff = (uint)civs[game.player_civ].num_advances -
               (uint)civs[civ2].num_advances;
        if (game.power_rank[civ2] < 6) {
            if (diff > 6)
                civs[civ2].government_score[GOV_DEMOCRACY] = -2;
            if (diff > 8)
                civs[civ2].government_score[GOV_REPUBLIC] = -1;
        }
    }
    if (civs[civ2].government_score[GOV_ANARCHY] > 0) {
        count = 3;
        if (civs[civ2].government <= GOV_REPUBLIC)
            count = 1;
    }
    best_score = -999;
    best = 1;
    for (i = 1; i <= count; i++) {
        /* (its other 19 parameters are a decompiler artifact, unused) */
        if (civ_can_choose_government(civ2, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == 0)
            continue;
        if (civs[civ2].government_score[i] >= best_score) {
            best_score = civs[civ2].government_score[i];
            best = i;
        }
    }
    civ_change_government(civ2, best);
}
