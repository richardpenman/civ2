/* The game, diplomacy and treaties: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00448cc0 auto */
void __cdecl diplo_status_icons_global_init(void)

{
  diplo_status_icons_static_init();
  diplo_status_icons_reg_atexit();
  return;
}

/* 00448cda auto */
void __cdecl diplo_status_icons_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&ceasefire_icon_sprite));
  sprite_init((Sprite *)((i8 *)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite));
  sprite_init((Sprite *)((i8 *)&war_icon_sprite_block.war_icon_sprite));
  sprite_init((Sprite *)((i8 *)&signal_allied_icon_sprite));
  sprite_init((Sprite *)((i8 *)&diplo_popup_portrait_sprite));
  return;
}

/* 00448d1c auto */
void __cdecl diplo_status_icons_reg_atexit(void)

{
  _atexit(diplo_status_icons_static_dtor);
  return;
}

/* 00448d39 auto */
void __cdecl diplo_status_icons_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&diplo_popup_portrait_sprite));
  sprite_destroy((Sprite *)((i8 *)&signal_allied_icon_sprite));
  sprite_destroy((Sprite *)((i8 *)&war_icon_sprite_block.war_icon_sprite));
  sprite_destroy((Sprite *)((i8 *)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite));
  sprite_destroy((Sprite *)((i8 *)&ceasefire_icon_sprite));
  return;
}

/* 00456f8b auto */
int __cdecl diplo_patience_limit(iptr civ2,iptr civ)

{
  int has_wonder;
  i32 limit;
  
  limit = 2;
  if (diplo_eval.cur_attitude < 0x19) {
    limit = 3;
  }
  if (0x3c < diplo_eval.cur_attitude) {
    limit--;
  }
  has_wonder = civ_has_wonder(civ2,WONDER_EIFFEL_TOWER);
  if (has_wonder != 0) {
    limit++;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) {
    limit++;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    limit += 2;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) {
    limit = 2;
  }
  return limit;
}

/* Part of diplo_eval_ai_stance (0045705e): the balance of forces on each continent: fear, aggression and the tribute to demand. */
static void diplo_stance_continents(iptr civ2,iptr civ,int war_count)
{
  int value;
  uint strength;
  iptr continent;
  
  for (continent = 1; continent < 0x40; continent++) {
    if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent]) &&
       (civs[civ].continent_military[continent] <
        civs[civ2].continent_attack[continent])) {
      (*(uint *)&diplo_eval.ai_fear_level) = (*(uint *)&diplo_eval.ai_fear_level) +
                     ((uint)civs[civ2].continent_attack[continent] *
                     ((-(uint)(game.difficulty == DIFF_CHIEFTAIN) & 2) + 2)) /
                     (civs[civ].continent_military[continent] + 1);
    }
    else {
      if ((*(short *)&civs[civ].continent_military[continent] != 0) &&
         (*(short *)&civs[civ2].continent_military[continent] != 0)) {
        diplo_eval.ai_aggressive = 1;
      }
      if (*(char *)&civs[civ2].continent_cities[continent] == '\0') {
        if (*(char *)&civs[civ].continent_cities[continent] == '\0') {
          value = (uint)civs[civ].continent_attack[continent] -
                  (uint)civs[civ2].continent_attack[continent];
          diplo_eval.tribute_demand += ((int)(value + (value >> 0x1f & 3U)) >> 2);
        }
        else {
          diplo_eval.tribute_demand = diplo_eval.tribute_demand +
                         (int)((uint)civs[civ].continent_military[continent] -
                              (uint)civs[civ2].continent_attack[continent]) / 2;
        }
      }
      else {
        if ((civs[civ].continent_cities[continent] < 2) &&
           ((((int)game.turn <= war_count * -0x32 + 200 || (6 < game.power_rank[civ2])) ||
            (6 < game.power_rank[civ])))) {
          strength = (uint)civs[civ].continent_attack[continent];
        }
        else {
          strength = (uint)civs[civ].continent_attack[continent] -
                  (uint)civs[civ2].continent_attack[continent];
        }
        diplo_eval.tribute_demand += strength;
      }
    }
  }
}

/* Part of diplo_eval_ai_stance (0045705e): the other civs: allies, common enemies, the war target and the strongest and weakest enemies. */
static void diplo_stance_other_civs(iptr civ2,iptr civ)
{
  ushort max_strength;
  ushort min_strength;
  int value;
  uint strength;
  int alliance_count;
  iptr civ3;
  
  diplo_eval.weakest_enemy_civ = -1;
  diplo_eval.strongest_enemy = -1;
  diplo_eval.war_target_civ = -1;
  max_strength = 0;
  min_strength = 9999;
  alliance_count = 0;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if ((civ2 != civ3) && (civ != civ3)) {
      if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
        if (game.power_rank[civ] < game.power_rank[civ2]) {
          diplo_eval.peace_inclination--;
        }
        if ((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) &&
            (200 < game.turn)) &&
           ((game.difficulty != DIFF_CHIEFTAIN &&
            ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_CONTACT) != 0)))) {
          if ((*(uint *)&diplo_eval.ai_fear_level) == 0) {
            if (((((byte)(civs[civ3].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) &&
                (value = civ_has_wonder(civ2,WONDER_GREAT_WALL), value == 0)) &&
               (value = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), value == 0)) {
              diplo_eval.ai_aggressive = 1;
            }
          }
          else {
            (*(uint *)&diplo_eval.ai_fear_level) = int_clamp((*(uint *)&diplo_eval.ai_fear_level) - 4,0,99);
          }
        }
      }
      if (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0) {
        alliance_count++;
      }
      value = diplo_is_hostile(civ,civ3);
      if (value != 0) {
        strength = (uint)civs[civ3].military_strength * 4;
        if (strength - civs[civ].military_strength != 0 &&
            civs[civ].military_strength <= strength) {
          diplo_eval.peace_inclination++;
        }
        if (civs[civ].military_strength < civs[civ3].military_strength) {
          diplo_eval.peace_inclination++;
        }
        if (((*(byte *)&civs[civ2].treaties[civ3] & TREATY_CONTACT) != 0) &&
           (((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0)) {
          diplo_eval.war_target_civ = civ3;
        }
        if ((((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0) &&
           (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_FIGHTING >> 8)) != 0)) {
          if (max_strength < civs[civ3].military_strength) {
            max_strength = civs[civ3].military_strength;
            diplo_eval.strongest_enemy = civ3;
          }
          if (civs[civ3].military_strength < min_strength) {
            min_strength = civs[civ3].military_strength;
            diplo_eval.weakest_enemy_civ = civ3;
          }
        }
      }
    }
  }
  if (alliance_count == 0) {
    diplo_clear_treaty_flags(civ,civ2,0x100000);
  }
}

/* Part of diplo_eval_ai_stance (0045705e): scenario overrides of the aggression between particular civs. */
static void diplo_stance_scenario_overrides(iptr civ2,iptr civ)
{
  if (((game.game_flags & GAME_SCENARIO) != 0) && (((int)(short)scenario.flags[0] & 0x8000U) != 0)) {
    if (civ == 6) {
      if (civ2 == 7) {
        if (((byte)civs[6].treaties[7] & TREATY_ALLIANCE) != 0) {
          diplo_eval.ai_aggressive = 1;
        }
      }
      else if (civ2 == 1) {
        diplo_eval.ai_aggressive = 0;
      }
      else if (civ2 == 3) {
        diplo_eval.ai_aggressive = 1;
      }
      else {
        diplo_eval.ai_aggressive = 0;
      }
    }
    if ((civ == 3) && ((civ2 == 6 || (civ2 == 1)))) {
      diplo_eval.ai_aggressive = 1;
    }
  }
}

/* 0045705e auto */
void __cdecl diplo_eval_ai_stance(iptr civ2,iptr civ)

{
  uint *treaty_flags;
  char patience;
  int value;
  iptr civ3;
  iptr city;
  int war_count;
  
  diplo_eval.space_race_threat = ai_spaceship_race_behind(civ);
  war_count = 0;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if (((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0) {
      war_count++;
    }
    if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
        (value = civ_spaceship_launched(civ3), value != 0)) && (DIFF_KING < game.difficulty)) {
      diplo_eval.space_race_threat = 1;
    }
  }
  diplo_eval.ai_outranks_target = (uint)(game.power_rank[civ2] < game.power_rank[civ]);
  diplo_eval.suppressed_aggression = 0;
  if ((diplo_meeting.opening_eval_active != 0) && (diplo_meeting.initial_eval_done == 0)) {
    diplo_eval.last_contact_turn = (int)civs[civ2].last_contact_turn[civ];
  }
  (*(uint *)&diplo_eval.ai_fear_level) = 0;
  diplo_eval.tribute_demand = 0;
  diplo_eval.ai_aggressive = 0;
  diplo_eval.ai_request_mode = 1;
  diplo_stance_continents(civ2,civ,war_count);
  value = civ_has_wonder(civ,WONDER_MARCO_POLO);
  if (value != 0) {
    diplo_eval.tribute_demand += ((int)(diplo_eval.tribute_demand + (diplo_eval.tribute_demand >> 0x1f & 3U)) >> 2);
  }
  diplo_eval.target_is_leader = 0;
  if ((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) &&
      ((1 < civs[civ].num_cities &&
       ((civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] == '\0' && (game.difficulty != DIFF_CHIEFTAIN)))))) &&
     (200 < game.turn)) {
    diplo_eval.target_is_leader = 1;
  }
  (*(uint *)&diplo_eval.cur_attitude) = (uint)civs[civ].attitude[civ2];
  if (((diplo_eval.target_is_leader != 0) ||
      (((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0 &&
       ((uint)civs[civ2].military_strength < (uint)civs[civ].military_strength * 3)))) &&
     (civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) {
    diplo_eval.tribute_demand = int_clamp(diplo_eval.tribute_demand,(uint)game.power_rank[civ] * 2,9999);
    if (diplo_eval.ai_aggressive == 0) {
      diplo_eval.tribute_demand = int_clamp(diplo_eval.tribute_demand,0,
                                        (civs[civ2].gold / 0x32) /
                                        (int)(8 - (uint)game.power_rank[civ]));
    }
    diplo_eval.ai_aggressive = 1;
    (*(uint *)&diplo_eval.ai_fear_level) = 0;
    if ((int)(*(uint *)&diplo_eval.cur_attitude) < 0x4c) {
      (*(uint *)&diplo_eval.cur_attitude) = 0x4b;
    }
  }
  if ((diplo_eval.space_race_threat != 0) && (diplo_eval.ai_outranks_target != 0)) {
    diplo_eval.ai_aggressive = 1;
  }
  if ((civs[civ].units_active[UNIT_NUCLEAR_MISSILE] != '\0') &&
     (treaty_flags = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594), *treaty_flags = *treaty_flags | 0x100,
     civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) {
    diplo_eval.ai_aggressive = 1;
    diplo_eval.tribute_demand = int_clamp(diplo_eval.tribute_demand,100,9999);
  }
  if ((civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] != '\0') &&
     ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) {
    if (civs[civ].units_active[UNIT_NUCLEAR_MISSILE] == '\0') {
      (*(uint *)&diplo_eval.ai_fear_level) = ((int)(uint)civs[civ2].reputation >> 1) + 1;
      if ((*(uint *)&diplo_eval.ai_fear_level) < 5) {
        (*(uint *)&diplo_eval.ai_fear_level) = 4;
      }
    }
    else {
      diplo_eval.tribute_demand = (int)((uint)civs[civ].units_active[UNIT_NUCLEAR_MISSILE] * (diplo_eval.tribute_demand / 2)) /
                     (int)(uint)civs[civ2].units_active[UNIT_NUCLEAR_MISSILE];
    }
  }
  (*(uint *)&diplo_eval.ai_fear_level) = (int)(*(uint *)&diplo_eval.ai_fear_level) / ((civs[civ2].reputation - 1) / 2 + 1);
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    (*(uint *)&diplo_eval.cur_attitude) -= 0x19;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) {
    (*(uint *)&diplo_eval.cur_attitude) += 0x19;
  }
  if (civs[civ2].reputation != '\0') {
    (*(uint *)&diplo_eval.cur_attitude) = (*(uint *)&diplo_eval.cur_attitude) +
                   ((civs[civ2].reputation - 1) -
                   (int)civs[civ].reputation_credit[civ2]) * 5;
  }
  if ((int)(*(uint *)&diplo_eval.cur_attitude) < 0x1a) {
    diplo_eval.tribute_demand = diplo_eval.tribute_demand / 2;
    diplo_eval.ai_aggressive = 0;
  }
  else {
    value = diplo_attitude_level((*(uint *)&diplo_eval.cur_attitude));
    if (value < 4) {
      diplo_eval.tribute_demand = (diplo_eval.tribute_demand * 2) / 3;
    }
    else if (0x4a < (int)(*(uint *)&diplo_eval.cur_attitude)) {
      diplo_eval.tribute_demand = (diplo_eval.tribute_demand * 3) / 2;
      diplo_eval.ai_aggressive = 1;
    }
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0) {
    diplo_eval.ai_aggressive = 1;
  }
  if (((game.rules & RULE_BLOODLUST) != 0) && (civs[civ].science_rate == '\0')) {
    diplo_eval.ai_aggressive = 1;
  }
  if ((*(uint *)&diplo_eval.ai_fear_level) != 0) {
    diplo_eval.ai_aggressive = 0;
    diplo_eval.ai_request_mode = 0;
  }
  value = civ_has_wonder(civ2,WONDER_GREAT_WALL);
  if ((value != 0) || (value = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), value != 0)) {
    diplo_eval.target_is_leader = 0;
    diplo_eval.suppressed_aggression = diplo_eval.ai_aggressive;
    diplo_eval.ai_aggressive = 0;
    (*(uint *)&diplo_eval.cur_attitude) -= 10;
    diplo_clear_treaty_flags(civ,civ2,0x10000);
  }
  if (civs[civ2].reputation != '\0') {
    diplo_eval.tribute_demand += (int)((uint)civs[civ2].reputation * diplo_eval.tribute_demand) / 2;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) {
    if (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) != 0) {
      diplo_eval.tribute_demand = (int)(diplo_eval.tribute_demand + (diplo_eval.tribute_demand >> 0x1f & 3U)) >> 2;
    }
  }
  else {
    diplo_eval.tribute_demand = diplo_eval.tribute_demand << 1;
  }
  diplo_eval.ai_units_near_cities = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((((cities[city].id != 0) && (cities[city].owner == civ2)) &&
        (value = tile_unit_owner((int)cities[city].x,(int)cities[city].y), value < 0)) &&
       ((value = unit_find_nearest((int)cities[city].x,(int)cities[city].y,civ,-1),
        -1 < value && (dialog_text_font_obj_block.unit_nearest_dist < 3)))) {
      diplo_eval.tribute_demand = diplo_eval.tribute_demand << 1;
      diplo_eval.ai_units_near_cities++;
      break;
    }
  }
  value = (game.difficulty + 1) * diplo_eval.tribute_demand;
  value = int_clamp((int)(value + (value >> 0x1f & 0x1fU)) >> 5,0,0x14);
  diplo_eval.tribute_demand = value * 0x32;
  if (((civs[civ2].gold < diplo_eval.tribute_demand) && (diplo_eval.tribute_demand < civs[civ2].gold * 2)) &&
     (0x31 < civs[civ2].gold)) {
    diplo_eval.tribute_demand = (civs[civ2].gold / 0x32) * 0x32;
  }
  if ((diplo_eval.tribute_demand == 0) ||
     ((uint)civs[civ].military_strength * 3 < (uint)civs[civ2].military_strength)) {
    diplo_eval.ai_aggressive = 0;
  }
  if (9 < civs[civ].aggression_counter[civ2]) {
    *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
    diplo_eval.ai_aggressive = 0;
  }
  if (diplo_eval.ai_aggressive == 0) {
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      diplo_eval.ai_request_mode = 0;
    }
  }
  else {
    diplo_eval.ai_request_mode = 2;
  }
  diplo_eval.ai_initial_aggressive = diplo_eval.ai_aggressive;
  diplo_eval.peace_inclination = 0;
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
    diplo_eval.peace_inclination = -2;
  }
  if (((game.rules & RULE_BLOODLUST) != 0) && (civs[civ].science_rate == '\0')) {
    diplo_eval.peace_inclination--;
  }
  if (civs[civ2].reputation != '\0') {
    diplo_eval.peace_inclination -= (civs[civ2].reputation - 1) / 2;
  }
  diplo_stance_other_civs(civ2,civ);
  diplo_eval.peace_inclination -= leaders[civs[civ].tribe].attack;
  if (civs[civ2].military_strength < civs[civ].military_strength) {
    diplo_eval.peace_inclination--;
  }
  value = diplo_is_hostile(civ,civ2);
  if (((((int)(uint)(value != 0) < diplo_eval.peace_inclination) && (diplo_eval.target_is_leader == 0)) &&
      ((civs[civ].treaties[civ2] & (TREATY_ALLIANCE | TREATY_AGGRESSOR)) == 0)) &&
     (diplo_eval.ai_aggressive = 0, (game.turn + civ + civ2 & 0x20U) != 0)) {
    diplo_eval.tribute_demand = 0;
  }
  diplo_eval.captured_city_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if (((cities[city].id != 0) && (cities[city].founder == civ)) &&
       (cities[city].owner == civ2)) {
      diplo_eval.captured_city_count++;
    }
  }
  if ((((((diplo_meeting.initial_eval_done == 0) ||
         (6 < (int)((uint)civs[civ2].reputation -
                   (int)civs[civ].reputation_credit[civ2]))) &&
        ((diplo_eval.ai_declare_war = 0, ((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0 ||
         (4 < diplo_eval.captured_city_count)))) &&
       ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) &&
      (((diplo_eval.ai_aggressive != 0 || ((diplo_eval.space_race_threat != 0 && (DIFF_PRINCE < game.difficulty)))) ||
       ((diplo_eval.suppressed_aggression != 0 &&
        ((6 < (int)((uint)civs[civ2].reputation -
                   (int)civs[civ].reputation_credit[civ2]) &&
         ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0)))))))) &&
     ((civs[civ].military_strength < civs[civ2].military_strength ||
      (((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0 ||
       (6 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]))))))) {
    diplo_eval.ai_declare_war = 1;
  }
  if (diplo_eval.ai_declare_war != 0) {
    diplo_eval.ai_aggressive = 0;
    diplo_eval.ai_request_mode = 0;
    if (0x1d < (int)(*(uint *)&diplo_eval.cur_attitude)) {
      (*(uint *)&diplo_eval.cur_attitude) = 0x1e;
    }
  }
  diplo_stance_scenario_overrides(civ2,civ);
  if (((((diplo_meeting.opening_eval_active != 0) &&
        (patience = civs[civ].diplo_request_count, value = diplo_patience_limit(civ2,civ),
        patience < value)) && (GOV_FUNDAMENTALISM < civs[civ].government)) &&
      ((diplo_eval.ai_aggressive != 0 &&
       ((civs[civ].treaties[civ2] & (TREATY_ALLIANCE | TREATY_CITY_CAPTURED)) == 0)))) &&
     (((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 1) == 0)))) {
    if ((((uint)civs[civ].military_strength * 2 < (uint)civs[civ2].military_strength * 3)
        && (diplo_eval.target_is_leader == 0)) &&
       ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0)) {
      diplo_eval.ai_aggressive = 0;
    }
    else {
      ai_choose_government(civ,1);
    }
  }
  if (((*(uint *)&diplo_eval.ai_fear_level) == 0) && ((int)(*(uint *)&diplo_eval.cur_attitude) < 2)) {
    (*(uint *)&diplo_eval.cur_attitude) = 1;
  }
  if ((((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_WAR >> 8)) == 0) &&
     (0x62 < (int)(*(uint *)&diplo_eval.cur_attitude))) {
    (*(uint *)&diplo_eval.cur_attitude) = 99;
  }
  if ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) != 0) || (diplo_eval.ai_request_mode == 0))
     && (0x49 < (int)(*(uint *)&diplo_eval.cur_attitude))) {
    (*(uint *)&diplo_eval.cur_attitude) = 0x4a;
  }
  if ((((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_WAR >> 8)) != 0) &&
     ((int)(*(uint *)&diplo_eval.cur_attitude) < 0x1b)) {
    (*(uint *)&diplo_eval.cur_attitude) = 0x1a;
  }
  return;
}

/* 00458a3b auto */
void __cdecl diplo_set_attitude_arg(iptr civ2,iptr civ)

{
  iptr level;
  iptr adjective;
  
  text_clear();
  level = diplo_attitude_level(diplo_eval.cur_attitude);
  text_append_string_id(attitude_names[level]);
  if (game_language == 2) {
    text_append((const char *)&str_er);
  }
  text_append_space();
  adjective = civ_get_adjective(civ);
  text_append((const char *)adjective);
  text_set_string_arg(0,(const char *)&textbuf.buf);
  return;
}

/* 00458ab1 auto */
void __cdecl diplo_greet_emissary(iptr civ2,iptr civ)

{
  uint *treaty;
  int offset_x;
  int offset_y;
  iptr str;
  Dialog dlg;
  RECT screen_rect2;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  diplo_meeting.session_active = 1;
  if (((*((char *)&game.game_options + 2)) & (OPT_DIPLOMACY_SCREEN >> 16)) != 0) {
    gfx_get_screen_rect((iptr)((i8 *)&screen_rect2.left));
    offset_x = int_clamp((screen_rect2.right - 0x280) / 3,1,9999);
    offset_y = int_clamp((screen_rect2.bottom - 0x1e0) / 3,1,9999);
    dialog_set_default_offset(-offset_x,-offset_y);
    dlg.set_position(-offset_x,-offset_y);
    diplo_leader_wnd_launch(civ2,civ);
  }
  diplo_set_attitude_arg(civ2,civ);
  str = (iptr)(civ_leader_name(civ));
  text_set_string_arg(1,(const char *)str);
  str = civ_leader_title(civ);
  text_set_string_arg(2,(const char *)str);
  str = (iptr)(civ_name_plural(civ));
  text_set_string_arg(3,(const char *)str);
  text_set_label_arg(5,label_table.string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
  str = (iptr)(civ_name_plural(civ2));
  text_set_string_arg(6,(const char *)str);
  text_clear();
  text_append((const char *)&str_greetings);
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) {
    text_append((const char *)&str_0_3);
  }
  else {
    text_append((const char *)&str_1);
  }
  str = rand_range(0,3);
  text_append_int(str);
  dlg.load_game_text((const char *)&textbuf.buf);
  if (civs[civ].units_active[UNIT_NUCLEAR_MISSILE] != '\0') {
    dlg.load_game_section((const char *)&str_nuclearweapons,0x8000);
  }
  dlg.show_modal(0);
  if ((civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] != '\0') &&
     ((civs[civ2].treaties[civ] & (TREATY_PEACE | TREATY_NUCLEAR_THREAT)) != 0x100)) {
    text_clear();
    text_append((const char *)&str_yournukes);
    text_append_int(2 - (uint)(civs[civ].units_active[UNIT_NUCLEAR_MISSILE] == '\0'));
    popup_game((const char *)&textbuf.buf);
    treaty = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *treaty = *treaty | 0x100;
  }
  return;
}

/* 00458df9 auto */
uint __cdecl diplo_ask_receive_emissary(iptr civ2,iptr civ,iptr x,iptr y)

{
  int ret;
  iptr str;
  iptr option_id;
  i32 option_flags;
  uint accepted;
  uint result;
  Dialog dlg;
  int refused;
  
  refused = 0;
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (autoplay.active == 0) {
    if (diplo_meeting.emissary_asked == 0) {
      diplo_meeting.emissary_asked = 1;
      while (diplo_meeting.emissary_response == 0) {
        if ((civs[civ].units_active[UNIT_NUCLEAR_MISSILE] != '\0') &&
           (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_NUCLEAR_THREAT >> 8)) == 0)) {
          refused = 1;
        }
        diplo_eval_ai_stance(civ2,civ);
        ret = map_in_bounds(x,y);
        if ((ret != 0) && (ret = mapviews_ensure_visible(x,y,civ2), ret == 0)) {
          map_redraw_tile_all_views(x,y);
        }
        diplo_set_attitude_arg(civ2,civ);
        if (refused != 0) {
          str = civ_get_adjective(civ);
          text_set_string_arg(1,(const char *)str);
          popup_game((const char *)&str_emissaryforce);
          break;
        }
        str = civ_leader_title(civ);
        text_set_string_arg(1,(const char *)str);
        str = (iptr)(civ_leader_name(civ));
        text_set_string_arg(2,(const char *)str);
        str = (iptr)(civ_name_plural(civ));
        text_set_string_arg(3,(const char *)str);
        text_set_label_arg(4,label_table.string_ids[0x76 - (uint)(leaders[civs[civ].tribe].female == '\0')]);
        dlg.load_game_text((const char *)&str_emissary);
        if ((((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0) ||
            (ret = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), ret != 0)) ||
           (ret = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), ret != 0)) {
          option_flags = 0;
          option_id = 2;
          str = (iptr)(label_string(label_table.string_ids[0x68]));
          dlg.add_option((const char *)str,option_id,option_flags);
        }
        ret = dlg.show_modal(0);
        diplo_meeting.emissary_response = -ret;
        if (diplo_meeting.emissary_response != -2) break;
        intel_report_show(civ2,civ);
        diplo_meeting.emissary_response = 0;
      }
      if (diplo_meeting.emissary_response != -1) {
        diplo_greet_emissary(civ2,civ);
      }
      accepted = (uint)(diplo_meeting.emissary_response != -1);
      result = accepted;
    }
    else {
      result = (uint)(diplo_meeting.emissary_response != -1);
    }
  }
  else {
    diplo_meeting.emissary_response = -1;
    result = 0;
  }
  return result;
}

/* 0045918e auto */
void __cdecl diplo_session_reset(void)

{
  diplo_meeting.session_active = 0;
  diplo_meeting.emissary_asked = 0;
  diplo_meeting.emissary_response = 0;
  diplo_meeting.initial_eval_done = 0;
  diplo_leader_wnd_close();
  return;
}

/* 004591cb auto */
i32 __cdecl diplo_offer_sell_tech(iptr civ,iptr civ2,int mode)

{
  i32 result;
  int known_count;
  int choice;
  int price;
  
  if (diplo_eval.wanted_tech < 0) {
    result = 0;
  }
  else {
    price = ai_advance_value(civ2,diplo_eval.wanted_tech);
    price = price * 0x14;
    if (0x32 < diplo_eval.cur_attitude) {
      price = (diplo_eval.cur_attitude * price) / 0x32;
    }
    if ((((game.power_rank[civ] == 7) && (4 < civs[civ].num_cities)) && (200 < game.turn)
        ) && (game.difficulty != DIFF_CHIEFTAIN)) {
      price = price << 1;
    }
    known_count = bits_count8((int)(char)game.advance_known_by[diplo_eval.wanted_tech]);
    known_count--;
    if (known_count < 2) {
      known_count = 1;
    }
    known_count = price / known_count;
    price = known_count * 10;
    if (0x5dc < civs[civ].gold) {
      price = (known_count * 0x1e) / 2;
    }
    if (3000 < civs[civ].gold) {
      price = (price * 3) / 2;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      if (civs[civ].num_advances < civs[civ2].num_advances) {
        price = price / 2;
      }
      else {
        price -= ((int)(price + (price >> 0x1f & 3U)) >> 2);
      }
      if (civs[civ].num_advances + 4 < (uint)civs[civ2].num_advances) {
        price = price / 2;
      }
    }
    if (price < 0) {
      price = 30000;
    }
    if (price < 100) {
      price = 100;
    }
    if (civs[civ].gold < price) {
      result = 0;
    }
    else {
      text_set_label_arg(1,advances[diplo_eval.wanted_tech].name);
      text_set_number_arg(0,price);
      if (mode < 2) {
        choice = popup_game((const char *)&str_selltech);
      }
      else {
        choice = popup_game((const char *)&str_selltech2);
      }
      if (choice == 1) {
        civs[civ].gold -= price;
        status_draw_info(1);
        civ_give_advance(civ,diplo_eval.wanted_tech,civ2,0,0);
        diplo_set_attitude_arg(civ,civ2);
        result = 1;
      }
      else {
        result = 1;
      }
    }
  }
  return result;
}

/* Part of diplo_tech_exchange (0045950b): survey the advances: the ones each side could give, the best two the AI could offer, the one it wants most, and a wonder the player could block. */
static void diplo_exchange_survey(iptr civ2,iptr civ,int &offer2_value,int &offer_value,int &civ_tech_count,int &ai_tech_count,iptr &blocking_wonder,iptr &advance2)
{
  int ret;
  iptr advance;
  iptr city;
  int civ_has;
  iptr wonder;
  int wanted_value;
  iptr wonder_group;
  int value;
  
  offer2_value = 0;
  offer_value = 0;
  wanted_value = 0;
  civ_tech_count = 0;
  ai_tech_count = 0;
  blocking_wonder = -1;
  advance2 = -1;
  diplo_eval.offer_tech_2nd = -1;
  diplo_eval.offer_tech_best = -1;
  diplo_eval.wanted_tech = -1;
  for (advance = 0; advance < 100; advance++) {
    if ((advances[advance].preq[0] != -2) || (advances[advance].preq[1] != -2)) {
      ret = civ_has_advance(civ2,advance);
      civ_has = civ_has_advance(civ,advance);
      if ((ret != 0) && (ai_tech_count = ai_tech_count + 1, advance2 < 0)) {
        advance2 = advance;
      }
      if ((civ_has != 0) && (civ_tech_count = civ_tech_count + 1, ret == 0)) {
        for (wonder = 0; wonder < 0x1c; wonder++) {
          if (((improvements[wonder + 0x27].preq == advance) &&
              (game.wonder_city[wonder] == -1)) &&
             (wonder_group = wonder / 7,
             civs[civ].wonders_by_group[wonder_group] <
             civs[civ2].wonders_by_group[wonder_group])) {
            for (city = 0; city < game.num_cities; city++) {
              if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
                 ((int)cities[city].building == -(wonder + 0x27))) {
                blocking_wonder = wonder;
                break;
              }
            }
          }
        }
        value = ai_advance_value(civ2,advance);
        if (wanted_value <= value) {
          diplo_eval.wanted_tech = advance;
          wanted_value = value;
        }
      }
      if ((ret != 0) && (civ_has == 0)) {
        value = ai_advance_value(civ,advance);
        if (value < offer_value) {
          if (offer2_value <= value) {
            diplo_eval.offer_tech_2nd = advance;
            offer2_value = value;
          }
        }
        else {
          if (-1 < diplo_eval.offer_tech_best) {
            diplo_eval.offer_tech_2nd = diplo_eval.offer_tech_best;
            offer2_value = offer_value;
          }
          diplo_eval.offer_tech_best = advance;
          offer_value = value;
        }
      }
    }
  }
}

/* Part of diplo_tech_exchange (0045950b): compose the exchange offer in the dialog. */
static void diplo_exchange_compose_offer(iptr civ2,int mode,iptr advance2,Dialog &dlg)
{
  int ret;
  iptr str;
  i8 label_buf[80];
  
  text_set_label_arg(1,advances[diplo_eval.wanted_tech].name);
  str = civ_get_adjective(civ2);
  text_set_string_arg(2,(const char *)str);
  text_set_label_arg(3,advances[diplo_eval.offer_tech_best].name);
  if (diplo_eval.offer_tech_2nd < 0) {
    if (-1 < advance2) {
      text_set_label_arg(4,advances[advance2].name);
    }
  }
  else {
    text_set_label_arg(4,advances[diplo_eval.offer_tech_2nd].name);
  }
  text_clear();
  text_append((const char *)&str_exchange);
  if (((mode == 0) && (diplo_eval.ai_aggressive == 0)) &&
     (ret = diplo_attitude_level(diplo_eval.cur_attitude), ret < 5)) {
    text_append((const char *)&str_1_2);
  }
  else {
    text_append((const char *)&str_0_4);
  }
  dlg.init_defaults();
  dlg.load_game_text((const char *)&textbuf.buf);
  if ((-1 < diplo_eval.offer_tech_2nd) && (diplo_eval.offer_tech_2nd != diplo_eval.offer_tech_best)) {
    str_clear((char *)label_buf);
    str_append_label((iptr)label_buf,0x6b);
    text_expand_vars((iptr)label_buf,(iptr)&textbuf.buf);
    dlg.add_option((const char *)&textbuf.buf,2,0);
  }
}

/* 0045950b auto */
int __cdecl
diplo_tech_exchange(iptr civ2,iptr civ,i32 x,i32 y,int mode,int eval_only
            )

{
  byte ai_advances;
  char civilize_bonus;
  uint treaty;
  bool early_game;
  bool allied;
  int ret;
  iptr str;
  int ai_tech_count;
  int offer2_value;
  int offer_value;
  iptr advance2;
  Dialog dlg;
  int civ_tech_count;
  iptr blocking_wonder;
  int attitude_cost;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
    allied = false;
  }
  else {
    allied = true;
  }
  do {
    diplo_exchange_survey(civ2,civ,offer2_value,offer_value,civ_tech_count,ai_tech_count,blocking_wonder,advance2);
    if (eval_only != 0) {
      mode = 0;
      return mode;
    }
    diplo_set_attitude_arg(civ2,civ);
    str = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)str);
    if (!allied) {
      if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) ||
          (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) != 0)) ||
         (diplo_eval.captured_city_count != 0)) {
        if (mode == 0) {
          ret = rand_range(0,2);
          if (ret == 0) {
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_noexchangemedium);
          }
          else {
            diplo_leader_play_reaction(4);
            popup_game((const char *)&str_noexchangemad);
          }
        }
        mode = 0;
        return mode;
      }
      if (((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_FIGHTING >> 8)) != 0) ||
          ((diplo_eval.ai_aggressive != 0 && (5 < civ_tech_count)))) ||
         (ret = diplo_attitude_level(diplo_eval.cur_attitude), 5 < ret)) {
        if (mode == 0) {
          ret = rand_range(0,2);
          if (ret == 0) {
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_noexchangemedium_2);
          }
          else {
            diplo_leader_play_reaction(4);
            popup_game((const char *)&str_noexchangemad_2);
          }
        }
        mode = 0;
        return mode;
      }
    }
    if ((-1 < blocking_wonder) && (DIFF_KING < game.difficulty)) {
      if (mode == 0) {
        text_set_label_arg(1,improvements[blocking_wonder + 0x27].name);
        popup_game((const char *)&str_noexchangewonder);
        mode = 0;
      }
      else {
        mode = 0;
      }
      return mode;
    }
    if (ai_tech_count < civ_tech_count) {
      if (mode == 1) {
        mode = 0;
        return mode;
      }
      treaty = civs[civ].treaties[civ2];
      ret = diplo_attitude_level(diplo_eval.cur_attitude);
      if (((int)(5 - (uint)((treaty & TREATY_PEACE) == 0)) < ret) && (!allied)) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret == 0) {
          if (mode == 0) {
            str = civ_get_adjective(civ2);
            text_set_string_arg(1,(const char *)str);
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_noexchangemedium_3);
            mode = 0;
          }
          else {
            mode = 0;
          }
        }
        return mode;
      }
    }
    if ((((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) && (!allied)) &&
         ((treaty = civs[civ].treaties[civ2], early_game = game.turn < 0xc9,
          ret = diplo_attitude_level(diplo_eval.cur_attitude),
          (int)((early_game + 3) - (uint)((treaty & TREATY_PEACE) == 0)) < ret &&
          (unit_types[UNIT_ARMOR].prerequisite != diplo_eval.offer_tech_best)))) &&
        (unit_types[UNIT_CAVALRY].prerequisite != diplo_eval.offer_tech_best)) &&
       (((unit_types[UNIT_ARTILLERY].prerequisite != diplo_eval.offer_tech_best &&
         (unit_types[UNIT_HOWITZER].prerequisite != diplo_eval.offer_tech_best)) &&
        ((unit_types[UNIT_RIFLEMEN].prerequisite != diplo_eval.offer_tech_best &&
         ((unit_types[UNIT_MUSKETEERS].prerequisite != diplo_eval.offer_tech_best && (diplo_eval.offer_tech_best != 0x22)))))))) {
      if (mode != 1) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret != 0) {
          return mode;
        }
        if (mode == 0) {
          popup_game((const char *)&str_noexchangenow);
        }
      }
      mode = 0;
      return mode;
    }
    if ((((diplo_eval.offer_tech_best < 0) && (-1 < diplo_eval.wanted_tech)) &&
        ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0)) &&
       ((ai_advances = civs[civ2].num_advances, civilize_bonus = leaders[civs[civ].tribe].civilize,
        ret = int_clamp(civs[civ].num_advances / 0xc,0,5),
        (((ai_advances + 6) - (int)civilize_bonus) - ret) < (int)(uint)civs[civ].num_advances &&
        ((civs[civ].diplo_request_count < 2 || (mode == 3)))))) {
      text_set_label_arg(1,advances[diplo_eval.wanted_tech].name);
      diplo_leader_play_reaction(3);
      if (mode < 2) {
        popup_game((const char *)&str_exchangegift);
      }
      else {
        popup_game((const char *)&str_exchangegift2);
      }
      civ_give_advance(civ2,diplo_eval.wanted_tech,civ,0,0);
      diplo_eval.wanted_tech = -1;
      if (mode < 2) {
        civs[civ].diplo_request_count += 2;
        mode = 0;
      }
      else {
        mode = 1;
      }
      return mode;
    }
    if ((diplo_eval.wanted_tech < 0) || (diplo_eval.offer_tech_best < 0)) {
      if (mode != 1) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret != 0) {
          return mode;
        }
        if (mode == 0) {
          popup_game((const char *)&str_noexchangenow_2);
        }
      }
      mode = 0;
      return mode;
    }
    if (1 < mode) {
      mode = 0;
      return mode;
    }
    ret = diplo_ask_receive_emissary(civ2,civ,x,y);
    if (ret == 0) {
      mode = 0;
      return mode;
    }
    diplo_exchange_compose_offer(civ2,mode,advance2,dlg);
    ret = dlg.show_modal(0);
    if (ret == 0) {
      mode = 0;
      return mode;
    }
    attitude_cost = offer_value * 2;
    if (ret == 2) {
      ret = civ_has_advance(civ,diplo_eval.offer_tech_2nd);
      if ((((ret != 0) || (ret = civ_has_wonder(civ,WONDER_MARCO_POLO), ret != 0)) ||
          ((game.power_rank[civ2] == 7 && (4 < civs[civ2].num_cities)))) ||
         ((offer2_value < offer_value &&
          ((ai_tech_count < civ_tech_count || (ret = diplo_attitude_level(diplo_eval.cur_attitude), 4 < ret)))))) {
        diplo_leader_play_reaction(3);
        popup_game((const char *)&str_exchangepetty);
        civs[civ].diplo_request_count++;
        mode = 0;
        return mode;
      }
      int_swap((iptr)&diplo_eval.offer_tech_best,(iptr)&diplo_eval.offer_tech_2nd);
      attitude_cost = offer2_value;
    }
    civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
    civ_steal_advance(civ2,civ);
    diplo_eval.offer_tech_best = diplo_eval.offer_tech_2nd;
    diplo_eval.offer_tech_2nd = -1;
    civ_adjust_attitude(civ,civ2,-attitude_cost);
    civs[civ].flags = civs[civ].flags | CIVF_TRADED_TECH;
    civs[civ2].flags = civs[civ2].flags | CIVF_TRADED_TECH;
    diplo_eval.ai_aggressive = 0;
  } while (mode != 0);
  mode = 1;
  return mode;
}

/* 0045a535 auto */
void __cdecl diplo_form_alliance(iptr civ2,iptr civ)

{
  iptr adjective;
  
  civ_adjust_attitude(civ,civ2,0xffffffe7);
  diplo_set_treaty_flags(civ,civ2,8);
  diplo_eval_ai_stance(civ2,civ);
  civs[civ].diplo_request_count = '\0';
  civs[civ].flags = civs[civ].flags | CIVF_HAS_ALLIED;
  diplo_eval.ai_declare_war = 0;
  civs[civ2].last_contact_turn[civ] = game.turn;
  civs[civ2].flags = civs[civ2].flags | CIVF_HAS_ALLIED;
  diplo_set_attitude_arg(civ2,civ);
  adjective = civ_get_adjective(civ);
  text_set_string_arg(1,(const char *)adjective);
  adjective = civ_get_adjective(civ2);
  text_set_string_arg(2,(const char *)adjective);
  diplo_leader_play_reaction(2);
  popup_game_with_sprite((const char *)&str_alliance,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
  return;
}

/* 0045a6ab auto */
void __cdecl diplo_sign_peace_treaty(iptr civ2,iptr civ)

{
  iptr str;
  i32 attitude;
  i32 max_attitude;
  
  diplo_set_treaty_flags(civ,civ2,0x4004);
  max_attitude = 0x32;
  attitude = 0;
  str = civ_get_attitude(civ,civ2);
  attitude = int_clamp(str,attitude,max_attitude);
  civ_set_attitude(civ,civ2,attitude);
  diplo_eval_ai_stance(civ2,civ);
  civs[civ].diplo_request_count = '\0';
  civs[civ2].last_contact_turn[civ] = game.turn;
  diplo_set_attitude_arg(civ2,civ);
  str = civ_get_adjective(civ);
  text_set_string_arg(1,(const char *)str);
  str = civ_get_adjective(civ2);
  text_set_string_arg(2,(const char *)str);
  diplo_leader_play_reaction(2);
  popup_game_with_sprite((const char *)&str_treaty,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
  return;
}

/* 0045a7a8 auto */
void __cdecl diplo_agree_ceasefire(iptr civ2,iptr civ)

{
  uint *treaty;
  iptr str;
  i32 attitude;
  i32 max_attitude;
  i32 other;
  
  diplo_set_treaty_flags(civ,civ2,0x4002);
  diplo_clear_treaty_flags(civ,civ2,0x40000);
  max_attitude = 0x32;
  attitude = 0;
  str = civ_get_attitude(civ,civ2);
  attitude = int_clamp(str,attitude,max_attitude);
  civ_set_attitude(civ,civ2,attitude);
  diplo_eval_ai_stance(civ2,civ);
  civs[civ2].last_contact_turn[civ] = game.turn;
  diplo_set_attitude_arg(civ2,civ);
  str = civ_get_adjective(civ);
  text_set_string_arg(1,(const char *)str);
  str = civ_get_adjective(civ2);
  text_set_string_arg(2,(const char *)str);
  diplo_leader_play_reaction(2);
  popup_game_with_sprite((const char *)&str_ceasefire,(iptr)&ceasefire_icon_sprite,0);
  for (other = 1; other < 8; other++) {
    treaty = (uint *)(other * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
    *treaty = *treaty & 0xfffff7ff;
  }
  return;
}

/* 0045a8e3 auto */
void __cdecl diplo_allies_join_war(iptr civ,iptr civ2)

{
  uint *treaty;
  iptr name2;
  int civ3;
  
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0) {
    for (civ3 = 1; civ3 < 8; civ3++) {
      if ((((civ3 != civ) && (civ3 != civ2)) &&
          ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) != 0)) &&
         (((civs[civ3].treaties[civ2] & (TREATY_ALLIANCE | TREATY_WAR)) == 0 &&
          ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_CONTACT) != 0)))) {
        name2 = (iptr)(civ_name_plural(civ));
        text_set_string_arg(1,(const char *)name2);
        name2 = (iptr)(civ_name_plural(civ3));
        text_set_string_arg(2,(const char *)name2);
        name2 = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(3,(const char *)name2);
        if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
            if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
              if (human_civ[0] == civ) {
                popup_game_with_sprite((const char *)&str_allyhelps,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                  ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                  0xffff7c80)),0);
              }
              else if ((human_civ[0] != civ) && (2 < game.multiplayer)) {
                net_send_text_args(0x39,net.net_slots[net.net_civ_slots[civ]].id,4,0,0,0);
              }
            }
            diplo_set_treaty_flags(civ3,civ2,0x2401);
            treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
            *treaty = *treaty | 0x80800;
          }
        }
        else {
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
            if (human_civ[0] == civ2) {
              popup_game_with_sprite((const char *)&str_activateally,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                0xffff7c80)),0);
            }
            else if ((human_civ[0] != civ2) && (2 < game.multiplayer)) {
              net_send_text_args(0x38,net.net_slots[net.net_civ_slots[civ2]].id,4,0,0,0);
            }
          }
          civ_adjust_attitude(civ3,civ2,100);
          diplo_set_treaty_flags(civ3,civ2,0x2401);
          treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
          *treaty = *treaty | 0x80800;
          treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
          *treaty = *treaty | 0x80800;
          civs[civ2].last_contact_turn[civ3] = game.turn;
        }
      }
    }
  }
  return;
}

/* 0045ac71 auto */
void __cdecl diplo_declare_war(iptr civ,iptr civ3,iptr civ2)

{
  char *counter;
  uint *treaty;
  int has_wonder;
  
  if (-1 < civ2) {
    counter = (char *)(civ + (iptr)((u8 *)&civs + 72) + civ2 * 0x594);
    *counter = *counter + 1;
  }
  if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ3] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) {
      if (-1 < civ2) {
        civ_adjust_attitude(civ2,civ,0xfffffffb);
      }
    }
    else {
      if (((*(byte *)&civs[civ].treaties[civ3] & TREATY_PEACE) != 0) &&
         ((*(byte *)&civs[civ].treaties[civ3] & TREATY_CEASEFIRE) != 0)) {
        if ((game.power_rank[civ] == 7) && ((4 < civs[civ].num_cities && (civ2 < 0))))
        {
          civs[civ].reputation++;
        }
        if ((game.difficulty != DIFF_CHIEFTAIN) && (has_wonder = civ_has_wonder(civ,WONDER_EIFFEL_TOWER), has_wonder == 0)) {
          civs[civ].reputation++;
        }
        if ((civ2 < 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
          counter = (char *)(civ3 + (iptr)((u8 *)&civs + 72) + civ * 0x594);
          *counter = *counter - 1;
        }
      }
      if (((game.power_rank[civ] == 7) && (4 < civs[civ].num_cities)) && (civ2 < 0)) {
        civs[civ].reputation++;
      }
      if ((game.difficulty != DIFF_CHIEFTAIN) && (has_wonder = civ_has_wonder(civ,WONDER_EIFFEL_TOWER), has_wonder == 0)) {
        civs[civ].reputation++;
      }
      if ((civ2 < 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        counter = (char *)(civ3 + (iptr)((u8 *)&civs + 72) + civ * 0x594);
        *counter = *counter - 1;
      }
      if (-1 < civ2) {
        civ_adjust_attitude(civ2,civ,0xfffffff1);
      }
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      treaty = (uint *)(civ3 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
      *treaty = *treaty | 0x10;
    }
    diplo_set_treaty_flags(civ,civ3,0x2000);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ3 * 4);
      *treaty = *treaty | 0x80800;
      civs[civ].last_contact_turn[civ3] = game.turn;
      diplo_allies_join_war(civ3,civ);
    }
  }
  else {
    if ((game.difficulty != DIFF_CHIEFTAIN) && (has_wonder = civ_has_wonder(civ,WONDER_EIFFEL_TOWER), has_wonder == 0)) {
      civs[civ].reputation++;
    }
    if ((civ2 < 0) &&
       (civs[civ].reputation = civs[civ].reputation + 1,
       (1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      counter = (char *)(civ3 + (iptr)((u8 *)&civs + 72) + civ * 0x594);
      *counter = *counter - 1;
    }
    if (-1 < civ2) {
      civ_adjust_attitude(civ2,civ,0xffffffe7);
    }
    diplo_cancel_alliance(civ,civ3);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      treaty = (uint *)(civ3 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
      *treaty = *treaty | 0x10;
    }
  }
  return;
}

/* 0045b0d6 auto */
void __cdecl diplo_ally_demand_help(iptr civ,iptr civ2)

{
  int payment;
  iptr name2;
  int gold_units;
  iptr civ3;
  int choice;
  int threat;
  iptr continent;
  byte human_byte;
  
  civ3 = human_civ[0];
  human_byte = (byte)human_civ[0];
  if ((((1 << (human_byte & 0x1f) & (uint)game.human_players) != 0) &&
      ((*(byte *)&civs[civ].treaties[human_civ[0]] & TREATY_ALLIANCE) != 0)) &&
     ((civs[human_civ[0]].treaties[civ2] & (TREATY_ALLIANCE | TREATY_WAR)) == 0)) {
    payment = diplo_ask_receive_emissary(human_civ[0],civ,0xffffffff,0xffffffff);
    if (payment == 0) {
      civ_adjust_attitude(civ,civ3,100);
    }
    else {
      name2 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)name2);
      choice = popup_game_with_sprite((const char *)&str_demandhelp,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                   0xffff7c80)),0);
      if (choice != 1) {
        threat = 0;
        for (continent = 1; continent < 0x3f; continent++) {
          if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent]) &&
             (civs[civ].continent_military[continent] <
              civs[civ2].continent_attack[continent])) {
            threat = threat +
                       ((uint)civs[civ2].continent_attack[continent] *
                       ((-(uint)(game.difficulty == DIFF_CHIEFTAIN) & 2) + 2)) /
                       (civs[civ].continent_military[continent] + 1);
          }
        }
        if (threat != 0) {
          gold_units = int_clamp(threat,0,civs[civ].gold / 0x32);
          payment = gold_units * 0x32;
          if (payment != 0) {
            text_set_number_arg(0,payment);
            choice = popup_game_with_sprite((const char *)&str_helpbonus,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                         ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                         0xffff7c80)),0);
            if (choice == 1) {
              civs[civ].gold += gold_units * -0x32;
              civs[civ3].gold += payment;
              status_draw_info(1);
            }
          }
        }
      }
      if (choice == 1) {
        diplo_declare_war(civ3,civ2,civ);
      }
      else {
        popup_game_with_sprite((const char *)&str_didnthelp,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
        if (diplo_eval.cur_attitude < 0x33) {
          civ_adjust_attitude(civ,civ3,100);
        }
        else {
          diplo_cancel_alliance(civ3,civ);
        }
      }
    }
    diplo_session_reset();
  }
  return;
}

/* 0045b472 auto */
int __cdecl diplo_gold_attitude_value(int gold)

{
  int chunk;
  i32 step;
  i32 value;
  int divisor;
  
  value = 0;
  divisor = 10;
  step = 0x32;
  while (0 < gold) {
    chunk = int_clamp(gold,0,step);
    value += chunk / divisor;
    divisor += 5;
    gold -= step;
    step = 100;
  }
  return value;
}

/* Part of diplo_ai_answer_proposal (0045b4da): is the AI willing to ally (sets the bits 1, 2 and 4 of reasons for the war target it has in mind). */
static bool diplo_alliance_willing(iptr civ2,iptr civ,uint &reasons)
{
  bool agree;
  int city;
  iptr other;
  iptr target;
  int ally_count;
  int best_rank;
  uint score;
  uint at_war;
  
  ally_count = 0;
  for (other = 1; other < 8; other++) {
    if ((*(byte *)&civs[civ2].treaties[other] & TREATY_ALLIANCE) != 0) {
      ally_count++;
    }
  }
  score = (uint)(diplo_eval.war_target_civ == -1);
  if (game.difficulty < DIFF_PRINCE) {
    score--;
  }
  if ((game.power_rank[civ] < 5) && (game.power_rank[civ2] < 5)) {
    if ((DIFF_WARLORD < game.difficulty) && (ally_count == 0)) {
      score--;
    }
    score--;
  }
  if (game.power_rank[civ2] < game.power_rank[civ]) {
    if (5 < game.power_rank[civ]) {
      score++;
    }
    if (6 < game.power_rank[civ]) {
      score++;
    }
    if ((game.power_rank[civ2] < 6) &&
       (civs[civ].military_strength < civs[civ2].military_strength)) {
      score--;
    }
  }
  if (diplo_eval.target_is_leader == 0) {
    if (2 < (int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ])) {
      score--;
    }
  }
  else {
    score++;
  }
  if (diplo_eval.ai_fear_level != 0) {
    score--;
  }
  agree = (int)score < diplo_eval.peace_inclination;
  city = diplo_attitude_level(diplo_eval.cur_attitude);
  agree = game.difficulty == DIFF_CHIEFTAIN || city < 4 && agree;
  if ((game.power_rank[civ2] == 7) && (ally_count != 0)) {
    agree = false;
  }
  if ((int)((-(uint)(game.difficulty == DIFF_CHIEFTAIN) & 2) + 2) <
      (int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
     ) {
    agree = false;
  }
  if ((game.power_rank[civ2] < 4) &&
     ((int)((uint)game.power_rank[civ2] * 0x14) < (int)game.turn)) {
    agree = true;
  }
  if (((0 < diplo_eval.war_target_civ) && (city = diplo_is_hostile(civ2,diplo_eval.war_target_civ), city == 0))
     && ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_CONTACT) != 0)) {
    if ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_PEACE) == 0) {
      score = (uint)civs[civ].military_strength << 1;
    }
    else {
      score = ((uint)civs[civ].military_strength * 3) / 2;
    }
    if ((((byte)(civs[civ].treaties[diplo_eval.war_target_civ] >> 8) & (TREATY_FIGHTING >> 8)) != 0) &&
       (score <= civs[diplo_eval.war_target_civ].military_strength)) {
      agree = true;
      reasons = 2;
    }
  }
  if (((diplo_eval.war_target_civ < 1) || (reasons == 0)) &&
     ((game.power_rank[civ2] < 7 &&
      ((((game.power_rank[civ] < 7 && (99 < game.turn)) &&
        (ally_count < (int)(7 - (uint)game.power_rank[civ2]))) &&
       (city = diplo_attitude_level(diplo_eval.cur_attitude), city < 5)))))) {
    best_rank = 8;
    target = -1;
    for (other = 1; other < 8; other++) {
      if ((((civ != other) && (civ2 != other)) &&
          (((game.power_rank[civ] <= game.power_rank[other] &&
            (game.power_rank[civ2] <= game.power_rank[other])) &&
           ((((*(byte *)&civs[civ2].treaties[other] & TREATY_CONTACT) != 0 ||
             ((*(byte *)&civs[civ].treaties[other] & TREATY_CONTACT) != 0)) &&
            (((byte)(civs[civ2].treaties[other] >> 8) & (TREATY_WAR >> 8)) == 0)))))) &&
         (((*(byte *)&civs[civ].treaties[other] & TREATY_ALLIANCE) == 0 &&
          (at_war = (uint)((*(byte *)&civs[civ2].treaties[other] & TREATY_CONTACT) != 0),
          (int)(game.power_rank[other] - at_war) < best_rank)))) {
        target = other;
        best_rank = game.power_rank[other] - at_war;
      }
    }
    if ((0 < diplo_eval.war_target_civ) && (game.power_rank[diplo_eval.war_target_civ] != 7)) {
      target = diplo_eval.war_target_civ;
    }
    if (0 < target) {
      diplo_eval.war_target_civ = target;
      agree = true;
      reasons = reasons | 4;
    }
  }
  if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) ||
     (4 < (int)((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2]))) {
    agree = false;
  }
  if ((-1 < diplo_eval.war_target_civ) &&
     ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_ALLIANCE) != 0)) {
    agree = true;
    reasons = reasons | 1;
  }
  if (0x4b < diplo_eval.cur_attitude) {
    agree = false;
  }
  if ((4 < civs[civ].diplo_request_count) && (game.difficulty != DIFF_CHIEFTAIN)) {
    agree = false;
  }
  return agree;
}

/* Part of diplo_ai_answer_proposal (0045b4da): the player proposes an alliance (returns the result: 1 when the AI ends the meeting). */
static uint diplo_answer_alliance(iptr civ2,iptr civ)
{
  uint *treaty;
  bool agree;
  bool paid;
  iptr str;
  int city;
  int gold_offer;
  uint reasons;
  
  paid = false;
  reasons = 0;
  agree = diplo_alliance_willing(civ2,civ,reasons);
  diplo_set_attitude_arg(civ2,civ);
  str = civ_get_adjective(civ2);
  text_set_string_arg(1,(const char *)str);
  str = civ_get_adjective(civ);
  text_set_string_arg(2,(const char *)str);
  if (agree) {
    agree = true;
    if (((((1 < civs[civ].diplo_request_count) || (0x19 < diplo_eval.cur_attitude)) ||
         (civs[civ2].reputation != '\0')) || (diplo_eval.ai_declare_war != 0)) &&
       ((((reasons & 3) == 0 || (diplo_eval.ai_declare_war != 0)) && (0 < diplo_eval.offer_tech_best)))) {
      text_set_label_arg(3,advances[diplo_eval.offer_tech_best].name);
      text_clear();
      diplo_leader_play_reaction(3);
      text_append((const char *)&str_perhapssecret);
      str = rand_range(0,1);
      text_append_int(str);
      city = popup_game((const char *)&textbuf.buf);
      if (city == 0) {
        agree = false;
      }
      else {
        paid = true;
        civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
        city = ai_advance_value(civ,diplo_eval.offer_tech_best);
        civ_adjust_attitude(civ,civ2,city * -2);
        diplo_set_attitude_arg(civ2,civ);
        diplo_eval.offer_tech_best = diplo_eval.offer_tech_2nd;
        diplo_eval.offer_tech_2nd = -1;
      }
    }
    if ((((1 < civs[civ].diplo_request_count) || (0x19 < diplo_eval.cur_attitude)) ||
        ((civs[civ2].reputation != '\0' || (diplo_eval.ai_declare_war != 0)))) &&
       (((((reasons & 2) == 0 || (diplo_eval.ai_declare_war != 0)) && (!paid)) && (agree)))) {
      city = (int)((civs[civ2].gold / 0x32) * (uint)civs[civ].military_strength) /
               (int)((uint)civs[civ].military_strength + (uint)civs[civ2].military_strength
                    + 1) + (uint)civs[civ2].reputation * 2;
      gold_offer = city * 0x32;
      if (civs[civ].military_strength < civs[civ2].military_strength) {
        gold_offer = city * 100;
      }
      for (; civs[civ2].gold < gold_offer; gold_offer -= 0x32) {
      }
      if (0 < gold_offer) {
        text_set_number_arg(0,gold_offer);
        text_clear();
        diplo_leader_play_reaction(3);
        text_append((const char *)&str_perhapsthrowin);
        str = rand_range(0,1);
        text_append_int(str);
        diplo_leader_play_reaction(3);
        city = popup_game((const char *)&textbuf.buf);
        if (city == 0) {
          agree = false;
        }
        else {
          civs[civ2].gold -= gold_offer;
          civs[civ].gold += gold_offer;
          paid = true;
          city = diplo_gold_attitude_value(gold_offer);
          civ_adjust_attitude(civ,civ2,-city);
          status_redraw(1);
          diplo_set_attitude_arg(civ2,civ);
        }
      }
    }
    if (agree) {
      if (0 < diplo_eval.war_target_civ) {
        str = (iptr)(civ_name_plural(diplo_eval.war_target_civ));
        text_set_string_arg(4,(const char *)str);
        diplo_leader_play_reaction(3);
        city = popup_game((const char *)&str_perhapssolidarity);
        if (city == 0) {
          if (((reasons != 0) || (game.power_rank[civ2] == 7)) ||
             ((2 < (int)((uint)civs[civ2].reputation -
                        (int)civs[civ].reputation_credit[civ2]) ||
              (diplo_eval.ai_declare_war != 0)))) {
            civs[civ].diplo_request_count++;
            diplo_leader_play_reaction(3);
            if (!paid) {
              popup_game((const char *)&str_perhapsbye);
            }
            else {
              popup_game((const char *)&str_perhapsthanksanyway);
            }
            return (uint)!paid;
          }
        }
        else {
          diplo_declare_war(civ2,diplo_eval.war_target_civ,civ);
          if ((reasons & 4) != 0) {
            diplo_set_treaty_flags(civ,diplo_eval.war_target_civ,0x2001);
            diplo_set_treaty_flags(civ2,diplo_eval.war_target_civ,1);
          }
          treaty = (uint *)(diplo_eval.war_target_civ * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
          *treaty = *treaty | 0x80800;
          diplo_eval.war_target_civ = -1;
        }
      }
      diplo_form_alliance(civ2,civ);
    }
    else {
      diplo_leader_play_reaction(3);
      popup_game((const char *)&str_alliancenopatience_2);
    }
  }
  else if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) &&
           ((int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]) < 3)) &&
          (diplo_eval.captured_city_count == 0)) {
    if ((civs[civ2].num_cities < 4) ||
       ((game.power_rank[civ2] != 7 &&
        (((int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) < 3 ||
         (game.power_rank[civ2] < 5)))))) {
      city = diplo_attitude_level(diplo_eval.cur_attitude);
      if (city < 4) {
        if (game.power_rank[civ2] < game.power_rank[civ]) {
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_alliancenosmall);
        }
        else if (civs[civ].diplo_request_count < 3) {
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_alliancenothanks);
        }
        else {
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_alliancenopatience);
        }
      }
      else {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_alliancenodislike);
      }
    }
    else {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_alliancenowinning);
    }
  }
  else {
    if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) &&
        (2 < (int)((uint)civs[civ2].reputation -
                  (int)civs[civ].reputation_credit[civ2]))) &&
       ((diplo_eval.ai_fear_level != 0 || (city = diplo_attitude_level((uint)(diplo_eval.cur_attitude < 2)), city != 0)
        ))) {
      diplo_leader_play_reaction(3);
      popup_game((const char *)&str_nobetrayweak);
    }
    else {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_alliancenobetray);
    }
    civs[civ].diplo_request_count++;
  }
  return 0;
}

/* Part of diplo_ai_answer_proposal (0045b4da): the player proposes a peace treaty (returns the result: 1 when the AI ends the meeting). */
static uint diplo_answer_peace(iptr civ2,iptr civ)
{
  bool paid;
  byte peace_flags;
  iptr str;
  int city;
  int gold_offer;
  uint score;
  
  paid = false;
  peace_flags = diplo_eval.target_is_leader == 0 &&
          (diplo_eval.cur_attitude < 0x33 &&
          (int)((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2]) <=
          (int)((-(uint)(game.difficulty == DIFF_CHIEFTAIN) & 2) + 4));
  if (((0 < diplo_eval.war_target_civ) && (city = diplo_is_hostile(civ2,diplo_eval.war_target_civ), city == 0))
     && ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_CONTACT) != 0)) {
    if ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_PEACE) == 0) {
      score = (uint)civs[civ].military_strength << 1;
    }
    else {
      score = ((uint)civs[civ].military_strength * 3) / 2;
    }
    if ((((byte)(civs[civ].treaties[diplo_eval.war_target_civ] >> 8) & (TREATY_FIGHTING >> 8)) != 0) &&
       (score <= civs[diplo_eval.war_target_civ].military_strength)) {
      peace_flags = 3;
    }
  }
  if ((0 < diplo_eval.war_target_civ) &&
     ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_ALLIANCE) != 0)) {
    peace_flags = peace_flags | 4;
  }
  if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) &&
      (2 - leaders[civs[civ].tribe].attack <
       (int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2]
            ))) ||
     (6 < (int)((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2]))) {
    peace_flags = 0;
  }
  if (0x4b < diplo_eval.cur_attitude) {
    peace_flags = 0;
  }
  city = civ_has_wonder(civ2,WONDER_GREAT_WALL);
  if ((city != 0) || (city = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), city != 0)) {
    peace_flags = peace_flags | 8;
  }
  if (peace_flags == 0) {
    if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) &&
        ((int)((uint)civs[civ2].reputation -
              (int)civs[civ].reputation_credit[civ2]) < 5)) && (diplo_eval.captured_city_count < 2)) {
      if ((diplo_eval.target_is_leader == 0) && ((uint)game.power_rank[civ2] <= game.power_rank[civ] + 2))
      {
        if ((diplo_eval.cur_attitude < 0x33) &&
           ((game.power_rank[civ] <= game.power_rank[civ2] || (diplo_eval.cur_attitude < 0x1a)))) {
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_peacenopatience);
        }
        else {
          diplo_leader_play_reaction(4);
          popup_game((const char *)&str_peacenodislike);
        }
      }
      else {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_peacenowinning);
      }
    }
    else {
      if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) &&
          (4 < (int)((uint)civs[civ2].reputation -
                    (int)civs[civ].reputation_credit[civ2]))) &&
         ((diplo_eval.ai_fear_level != 0 ||
          (city = diplo_attitude_level((uint)(diplo_eval.cur_attitude < 2)), city != 0)))) {
        diplo_leader_play_reaction(3);
        popup_game((const char *)&str_nobetrayweak_2);
      }
      else {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_peacenobetray_2);
      }
      civs[civ].diplo_request_count++;
    }
  }
  else {
    if ((((1 < civs[civ].diplo_request_count) || (0x32 < diplo_eval.cur_attitude)) ||
        (1 < (int)((uint)civs[civ2].reputation -
                  (int)civs[civ].reputation_credit[civ2]))) &&
       (((peace_flags & 6) == 0 && (0 < diplo_eval.offer_tech_best)))) {
      text_set_label_arg(3,advances[diplo_eval.offer_tech_best].name);
      diplo_leader_play_reaction(3);
      text_clear();
      text_append((const char *)&str_perhapssecret_2);
      str = rand_range(0,1);
      text_append_int(str);
      city = popup_game((const char *)&textbuf.buf);
      if (city == 0) {
        peace_flags = peace_flags & 8;
      }
      else {
        paid = true;
        civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
        city = ai_advance_value(civ,diplo_eval.offer_tech_best);
        civ_adjust_attitude(civ,civ2,city * -2);
        diplo_set_attitude_arg(civ2,civ);
        diplo_eval.offer_tech_best = diplo_eval.offer_tech_2nd;
        diplo_eval.offer_tech_2nd = -1;
      }
    }
    if (((((1 < civs[civ].diplo_request_count) || (0x32 < diplo_eval.cur_attitude)) ||
         (1 < (int)((uint)civs[civ2].reputation -
                   (int)civs[civ].reputation_credit[civ2]))) &&
        (((peace_flags & 4) == 0 && (!paid)))) && (peace_flags != 0)) {
      city = (int)((civs[civ2].gold / 0x32) * (uint)civs[civ].military_strength) /
               (int)((uint)civs[civ].military_strength + (uint)civs[civ2].military_strength
                    + 1) + (uint)civs[civ2].reputation;
      gold_offer = city * 0x32;
      if (civs[civ].military_strength < civs[civ2].military_strength) {
        gold_offer = city * 100;
      }
      for (; civs[civ2].gold < gold_offer; gold_offer -= 0x32) {
      }
      if (0 < gold_offer) {
        text_set_number_arg(0,gold_offer);
        diplo_leader_play_reaction(3);
        text_clear();
        text_append((const char *)&str_perhapsthrowin_2);
        str = rand_range(0,1);
        text_append_int(str);
        city = popup_game((const char *)&textbuf.buf);
        if (city == 0) {
          peace_flags = peace_flags & 8;
        }
        else {
          civs[civ2].gold -= gold_offer;
          civs[civ].gold += gold_offer;
          paid = true;
          city = diplo_gold_attitude_value(gold_offer);
          civ_adjust_attitude(civ,civ2,-city);
          diplo_set_attitude_arg(civ2,civ);
          status_redraw(1);
        }
      }
    }
    if (peace_flags == 0) {
      diplo_leader_play_reaction(3);
      popup_game((const char *)&str_peacenopatience_2);
    }
    else {
      if (0 < diplo_eval.war_target_civ) {
        str = (iptr)(civ_name_plural(diplo_eval.war_target_civ));
        text_set_string_arg(4,(const char *)str);
        diplo_leader_play_reaction(3);
        city = popup_game((const char *)&str_perhapssolidarity_2);
        if (city == 0) {
          if ((((peace_flags & 6) != 0) || (game.power_rank[civ2] == 7)) ||
             (3 < (int)((uint)civs[civ2].reputation -
                       (int)civs[civ].reputation_credit[civ2]))) {
            civs[civ].diplo_request_count++;
            diplo_leader_play_reaction(3);
            if (!paid) {
              popup_game((const char *)&str_perhapsbye_2);
            }
            else {
              popup_game((const char *)&str_perhapsthanksanyway_2);
            }
            return (uint)!paid;
          }
        }
        else {
          diplo_declare_war(civ2,diplo_eval.war_target_civ,civ);
          diplo_eval.war_target_civ = -1;
        }
      }
      diplo_sign_peace_treaty(civ2,civ);
    }
  }
  return 0;
}

/* Part of diplo_ai_answer_proposal (0045b4da): the player asks an ally for help (3), demands tribute (4) or the withdrawal of troops (5) (returns the result: 1 when the AI ends the meeting). */
static uint diplo_answer_demand(iptr civ2,iptr civ,int proposal)
{
  char patience;
  bool paid;
  i32 limit;
  iptr str;
  iptr has_wonder;
  iptr has_wonder2;
  int city;
  uint rnd;
  uint rnd_sign;
  int units_moved;
  i32 attitude_delta;
  iptr unit;
  int y2;
  int x2;
  uint result;
  int tribute;
  
  result = 0;
  patience = civs[civ].diplo_request_count;
  if (proposal < 5) {
    limit = diplo_patience_limit(civ2,civ);
    civs[civ].diplo_request_count = (i8)limit;
  }
  else {
    civs[civ].diplo_request_count++;
  }
  diplo_set_attitude_arg(civ2,civ);
  if ((diplo_eval.ai_declare_war != 0) || ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_HOSTILE_INTENT | TREATY_BROKEN)) != 0))
  {
    diplo_set_attitude_arg(civ2,civ);
    text_clear();
    text_append((const char *)&str_provoke);
    str = rand_range(0,2);
    text_append_int(str);
    diplo_eval.ai_declare_war = 0;
    diplo_leader_play_reaction(4);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    civs[civ2].last_contact_turn[civ] = game.turn;
    popup_game_with_sprite((const char *)&textbuf.buf,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    return 1;
  }
  if ((proposal == 3) && (game.power_rank[civ2] < game.power_rank[civ])) {
    diplo_eval.ai_fear_level = (uint)game.power_rank[civ] - (uint)game.power_rank[civ2];
  }
  tribute = int_clamp(diplo_eval.ai_fear_level,0,civs[civ].gold / 0x32);
  tribute = tribute * 0x32;
  if (game.turn - diplo_eval.last_contact_turn < 8) {
    tribute = 0;
    diplo_eval.wanted_tech = -1;
  }
  if (game.turn - diplo_eval.last_contact_turn < 0x10) {
    tribute = tribute / 2;
  }
  if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) == 0) {
    tribute = tribute / 2;
  }
  has_wonder = civ_has_wonder(civ,WONDER_GREAT_WALL);
  if ((has_wonder != 0) || (has_wonder2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), has_wonder2 != 0)) {
    tribute = 0;
  }
  if (((((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) && (-1 < diplo_eval.wanted_tech))
      && ((uint)civs[civ2].num_advances + (uint)game.difficulty <
          (uint)civs[civ].num_advances)) &&
     ((civs[civ].military_strength < civs[civ2].military_strength ||
      (game.power_rank[civ2] < game.power_rank[civ])))) {
    tribute = 0;
    diplo_eval.ai_fear_level = 1;
  }
  units_moved = 0;
  city = civ_has_wonder(civ2,WONDER_GREAT_WALL);
  if ((city == 0) && (city = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), city == 0)) {
    paid = false;
  }
  else {
    paid = true;
  }
  if (6 < (int)((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2])) {
    paid = false;
  }
  if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) &&
     ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) {
    if (!(((diplo_eval.ai_initial_aggressive != 0) || (0x4a < diplo_eval.cur_attitude)) &&
          ((diplo_eval.ai_fear_level == 0 && (!paid)))) ||
        (rnd = crt_rand_at("diplo_ai_answer_proposal"), rnd_sign = (int)rnd >> 0x1f,
         !(((int)(uint)civs[civ].government <
            (int)((((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) - rnd_sign) + 5)) &&
           ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_CHANGED_THIS_TURN >> 8)) == 0 &&
            (0x19 < diplo_eval.cur_attitude)))))) {
      for (unit = 0; unit < game.num_units; unit++) {
        if (((units[unit].id != 0) && (units[unit].owner == civ)) &&
           ((char)unit_types[units[unit].type].role < 6)) {
          x2 = (int)units[unit].x;
          y2 = (int)units[unit].y;
          city = tile_is_ocean(x2,y2);
          if (((city == 0) &&
              (city = city_find_nearest(x2,y2,civ2,0xffffffff,-1), -1 < city))
             && ((advisor_window_block.nearest_city_dist < 3 &&
                 (city = city_find_nearest(x2,y2,civ,0xffffffff,-1), -1 < city)
                 ))) {
            unit_relocate(unit,(int)cities[city].x,(int)cities[city].y,1);
            units[unit].visibility = '\0';
            units[unit].orders = ORDER_NONE;
            map_redraw_tile_all_views(x2,y2);
            units_moved++;
          }
        }
      }
    }
  }
  if (proposal == 5) {
    civ_adjust_attitude(civ,civ2,2);
  }
  else {
    attitude_delta = 10;
    if (proposal == 3) {
      if (game.power_rank[civ2] < game.power_rank[civ]) {
        attitude_delta = 5;
      }
      if (((civs[civ2].num_advances < civs[civ].num_advances) &&
          (game.power_rank[civ2] < game.power_rank[civ])) || (game.power_rank[civ2] < 4)
         ) {
        attitude_delta = 0;
      }
    }
    civ_adjust_attitude(civ,civ2,attitude_delta);
  }
  if (units_moved != 0) {
    diplo_leader_play_reaction(3);
    popup_game_with_sprite((const char *)&str_apologize,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
    if (proposal == 5) {
      return 0;
    }
  }
  if (((tribute == 0) && ((diplo_eval.wanted_tech < 0 || (diplo_eval.ai_fear_level == 0)))) || (proposal == 5)) {
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
      if (((diplo_eval.ai_initial_aggressive == 0) && (diplo_eval.cur_attitude < 0x4c)) ||
         ((units_moved != 0 ||
          (((((diplo_eval.ai_fear_level != 0 || (paid)) ||
             (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_CHANGED_THIS_TURN >> 8)) != 0)) ||
            (diplo_eval.cur_attitude < 0x1a)) && ((city = crt_rand_at("diplo_ai_answer_proposal"), city % 3 != 0 || (proposal == 5))))))))
      {
        if (units_moved == 0) {
          diplo_leader_play_reaction(3);
          if (proposal == 5) {
            popup_game((const char *)&str_noviolators);
          }
          else {
            popup_game((const char *)&str_feeble);
          }
        }
      }
      else {
        result = 1;
        diplo_set_attitude_arg(civ2,civ);
        text_clear();
        text_append((const char *)&str_provoke_2);
        str = rand_range(0,2);
        text_append_int(str);
        diplo_eval.ai_declare_war = 0;
        diplo_leader_play_reaction(4);
        diplo_set_treaty_flags(civ,civ2,0x2000);
        popup_game_with_sprite((const char *)&textbuf.buf,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
        civs[civ2].last_contact_turn[civ] = game.turn;
      }
    }
    else {
      city = diplo_tech_exchange(civ2,civ,x2,y2,(patience < 2) + 2,0);
      if (city == 0) {
        if (((civs[civ2].num_advances < civs[civ].num_advances) &&
            (game.power_rank[civ2] < game.power_rank[civ])) ||
           (game.power_rank[civ2] < 4)) {
          diplo_leader_play_reaction(3);
          popup_game_with_sprite((const char *)&str_sympathy,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                            ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 & 0xffff7c80
                                            )),0);
        }
        else if (((diplo_eval.ai_initial_aggressive == 0) && (diplo_eval.cur_attitude < 0x33)) && (diplo_eval.target_is_leader == 0)) {
          if (units_moved == 0) {
            diplo_leader_play_reaction(3);
            popup_game_with_sprite((const char *)&str_feebleally,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                              0xffff7c80)),0);
          }
        }
        else {
          diplo_leader_play_reaction(4);
          popup_game_with_sprite((const char *)&str_tauntally,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                            ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 & 0xffff7c80
                                            )),0);
          diplo_cancel_alliance(civ2,civ);
          result = 1;
        }
      }
    }
  }
  else {
    if ((diplo_eval.wanted_tech < 0) ||
       ((tribute != 0 && (civs[civ].num_advances <= civs[civ2].num_advances)))) {
      text_set_number_arg(0,tribute);
      diplo_leader_play_reaction(2);
      if (proposal == 3) {
        popup_game_with_sprite((const char *)&str_givemoreally,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                           ,0);
      }
      else {
        popup_game((const char *)&str_givemore);
      }
      civs[civ2].gold += tribute;
      civs[civ].gold -= tribute;
      status_redraw(1);
    }
    else {
      text_set_label_arg(1,advances[diplo_eval.wanted_tech].name);
      diplo_leader_play_reaction(2);
      popup_game((const char *)&str_givemoreciv);
      civ_give_advance(civ2,diplo_eval.wanted_tech,civ,0,0);
      diplo_eval.wanted_tech = -1;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
      if (diplo_eval.had_ceasefire == 0) {
        diplo_set_treaty_flags(civ,civ2,0x40000);
      }
      diplo_set_treaty_flags(civ,civ2,2);
    }
  }
  return result;
}

/* 0045b4da auto */
i32 __cdecl diplo_ai_answer_proposal(iptr civ2,iptr civ,int proposal)

{
  uint result;
  
  result = 0;
  blink.unit_active = 0;
  switch(proposal) {
  case 1:
    result = diplo_answer_alliance(civ2,civ);
    break;
  case 2:
    result = diplo_answer_peace(civ2,civ);
    break;
  case 3:
  case 4:
  case 5:
    result = diplo_answer_demand(civ2,civ,proposal);
    break;
  case 6:
    diplo_clear_treaty_flags(civ,civ2,8);
    civ_adjust_attitude(civ,civ2,0x32);
    diplo_eval_ai_stance(civ2,civ);
    diplo_set_attitude_arg(civ2,civ);
    diplo_leader_play_reaction(4);
    popup_game_with_sprite((const char *)&str_cancelally,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    civs[civ2].reputation++;
    diplo_cancel_alliance(civ2,civ);
  }
  return result;
}

/* Part of diplo_favor_menu (0045dd7f): the AI agrees to share maps: each civ learns what the other has seen. */
static void diplo_favor_share_maps(iptr civ2,iptr civ)
{
  byte visible_bits;
  iptr civ_name;
  int civ3;
  uint civ_mask;
  byte *known;
  uint civ2_mask;
  i32 arg_a;
  i32 arg_b;
  iptr target_civ;
  iptr unit;
  byte civ2_mask_byte;
  byte civ_mask_byte;
  iptr city;
  uint y2;
  uint x2;
  int i;
  
  diplo_leader_play_reaction(2);
  popup_game((const char *)&str_mapyes);
  civs[civ].diplo_request_count += 2;
  civ_mask = 1 << ((byte)civ & 0x1f);
  civ2_mask = 1 << ((byte)civ2 & 0x1f);
  y2 = 0;
  x2 = 0;
  net_batch_begin();
  for (i = 0; i < map.area; i++) {
    civ3 = (int)(tile_at(x2,y2));
    if ((civ_mask & ((Tile *)civ3)->visible) == 0) {
      if ((((civ2_mask & ((Tile *)civ3)->visible) != 0) &&
          (tile_set_visible(x2,y2,civ_mask,1,1), civ != 0)) &&
         (civ2 != 0)) {
        arg_b = 1;
        arg_a = 1;
        target_civ = civ;
        civ3 = (int)(tile_at(x2,y2));
        visible_bits = ((Tile *)civ3)->improvements;
        known = known_at(x2,y2,civ2);
        known_set(x2,y2,(uint)(visible_bits & *known),target_civ,arg_a,arg_b);
      }
    }
    else {
      tile_set_visible(x2,y2,civ2_mask,1,1);
      if ((civ != 0) && (civ2 != 0)) {
        arg_b = 1;
        arg_a = 1;
        civ_name = civ2;
        civ3 = (int)(tile_at(x2,y2));
        visible_bits = ((Tile *)civ3)->improvements;
        known = known_at(x2,y2,civ);
        known_set(x2,y2,(uint)(visible_bits & *known),civ_name,arg_a,arg_b);
      }
    }
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  net_batch_flush();
  for (unit = 0; civ_mask_byte = (byte)civ_mask, unit < game.num_units;
      unit = unit + 1) {
    if (units[unit].id != 0) {
      if (((civ_mask & units[unit].visibility) == 0) &&
         (units[unit].owner != civ)) {
        if (((civ2_mask & units[unit].visibility) != 0) ||
           (units[unit].owner == civ2)) {
          units[unit].visibility = units[unit].visibility | civ_mask_byte;
        }
      }
      else {
        civ2_mask_byte = (byte)civ2_mask;
        units[unit].visibility = units[unit].visibility | civ2_mask_byte;
      }
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      if (((civ_mask & (int)(char)cities[city].known_to) == 0) &&
         (cities[city].owner != civ)) {
        if (((civ2_mask & (int)(char)cities[city].known_to) != 0) ||
           (cities[city].owner == civ2)) {
          cities[city].known_to = cities[city].known_to | civ_mask_byte;
        }
      }
      else {
        city_reveal_to_civ(city,civ2);
      }
    }
  }
  net_send_state_diff(0xff,2,0,0,0);
  map_redraw_all_views(human_civ[0],1);
}

/* Part of diplo_favor_menu (0045dd7f): the price the AI asks for going to war with civ3. */
static int diplo_favor_war_price(iptr civ2,iptr civ,int civ3)
{
  bool no_embassy;
  int i;
  int price;
  
  price = int_clamp((int)(((((uint)civs[civ2].reputation + civs[civ].gold / 100
                                        ) - (uint)civs[civ3].reputation) -
                                      (int)civs[civ].reputation_credit[civ2]) *
                                     ((int)(uint)civs[civ3].military_strength >> 1)) / 0x32,2,500
                               );
  price = price * 0x32;
  if ((*(byte *)&civs[civ].treaties[civ3] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) {
    price -= price / 3;
  }
  if (civs[civ].gold < civs[civ2].gold) {
    price += price / 3;
  }
  if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
    if (7000 < price) {
      price = 7000;
    }
    price = int_clamp(price * 3,500,25000);
  }
  if ((game.power_rank[civ3] == 7) && (4 < civs[civ3].num_cities)) {
    price = price / 2;
  }
  if (((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
    price += price / 3;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    no_embassy = true;
    for (i = 1; i < 8; i++) {
      if (((byte)(civs[civ].treaties[i] >> 8) & (TREATY_WAR >> 8)) != 0) {
        no_embassy = false;
      }
    }
    if (no_embassy) {
      price -= ((int)(price + (price >> 0x1f & 3U)) >> 2);
    }
    if (((byte)(civs[civ3].treaties[civ2] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) != 0) {
      price -= ((int)(price + (price >> 0x1f & 3U)) >> 2);
    }
    if (game.power_rank[civ2] < game.power_rank[civ]) {
      price -= price / 3;
    }
  }
  if (((byte)(civs[civ3].treaties[civ2] >> 16) & (TREATY_NUKED >> 16)) != 0) {
    if (((byte)(civs[civ2].treaties[civ3] >> 16) & (TREATY_NUKED >> 16)) == 0) {
      price = price / 2;
    }
    if (game.power_rank[civ2] < game.power_rank[civ3]) {
      price = price / 2;
    }
  }
  price = int_clamp(price / 100,1,500);
  price = price * 0x32;
  return price;
}

/* Part of diplo_favor_menu (0045dd7f): how far the AI distrusts the player asking it to go to war with civ3. */
static int diplo_favor_war_distrust(iptr civ2,iptr civ,int civ3)
{
  int distrust;
  
  distrust = ((uint)civs[civ2].reputation -
              (int)civs[civ].reputation_credit[civ2]) -
              (int)leaders[civs[civ].tribe].attack;
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
    distrust += 2;
  }
  if (((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
    distrust++;
  }
  if (game.power_rank[civ3] < game.power_rank[civ2]) {
    distrust++;
  }
  if (game.power_rank[civ] < game.power_rank[civ2]) {
    distrust++;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    distrust = distrust / 2;
  }
  return distrust;
}

/* Part of diplo_favor_menu (0045dd7f): the deal is made: the AI declares war on civ3, which may turn on the player. */
static void diplo_favor_mercenary_war(iptr civ2,iptr civ,int civ3,int &civ4)
{
  uint *treaty;
  iptr civ_name;
  uint civ_mask;
  uint civ2_mask;
  
  status_redraw(1);
  diplo_set_treaty_flags(civ,civ3,0x2401);
  treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ3 * 4);
  *treaty = *treaty | 0x80800;
  treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
  *treaty = *treaty | 0x100000;
  net_send_state_diff(0xff,2,0,0,0);
  civ_name = (iptr)(civ_name_plural(civ));
  text_set_string_arg(1,(const char *)civ_name);
  civ_name = (iptr)(civ_name_plural(civ3));
  text_set_string_arg(2,(const char *)civ_name);
  popup_game((const char *)&str_mercdeclare);
  if (2 < game.multiplayer) {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
       (human_civ[0] != civ)) {
      net_send_text_args(0x62,net.net_slots[net.net_civ_slots[civ]].id,
                         3,0,0,0);
    }
    (*((char *)&civ4 + 0)) = (byte)civ3;
    if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
       (human_civ[0] != civ3)) {
      net_send_text_args(0x62,net.net_slots[net.net_civ_slots[civ3]].id,3,
                         0,0,0);
    }
  }
  civ_name = civ_get_adjective(civ3);
  text_set_string_arg(1,(const char *)civ_name);
  if ((*(byte *)&civs[civ2].treaties[civ3] & TREATY_ALLIANCE) == 0) {
    if ((*(byte *)&civs[civ2].treaties[civ3] & TREATY_PEACE) != 0) {
      civ_mask = crt_rand_at("diplo_favor_menu");
      civ2_mask = (int)civ_mask >> 0x1f;
      if (((civ_mask ^ civ2_mask) - civ2_mask & 1 ^ civ2_mask) == civ2_mask) {
        civ_adjust_attitude(civ3,civ2,0x19);
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_mercbetray);
        diplo_set_treaty_flags(civ2,civ3,0x2000);
        treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
        *treaty = *treaty | 0x10;
      }
    }
  }
  else {
    civ_mask = crt_rand_at("diplo_favor_menu");
    civ2_mask = (int)civ_mask >> 0x1f;
    if (((civ_mask ^ civ2_mask) - civ2_mask & 1 ^ civ2_mask) == civ2_mask) {
      civ_adjust_attitude(civ3,civ2,0x19);
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_mercbetrayally);
      diplo_cancel_alliance(civ2,civ3);
    }
  }
  net_send_state_diff(0xff,2,0,0,0);
}

/* 0045dd7f auto */
void __cdecl diplo_favor_menu(iptr civ2,iptr civ)

{
  char patience;
  iptr menu_choice;
  iptr exchanged;
  iptr patience_limit;
  iptr civ_name;
  int civ3;
  i32 arg_a;
  int civ4;
  int choice;
  int distrust;
  Dialog dlg;
  int price;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  while( true ) {
    diplo_set_attitude_arg(civ2,civ);
    menu_choice = popup_game((const char *)&str_favormenu);
    if (menu_choice == 0) {
      civs[civ].diplo_request_count--;
      return;
    }
    if (menu_choice == 1) {
      exchanged = diplo_tech_exchange(civ2,civ,0xffffffff,0xffffffff,0,0);
      if ((exchanged != 0) &&
         (patience = civs[civ].diplo_request_count, patience_limit = diplo_patience_limit(civ2,civ),
         patience + 1 < patience_limit)) {
        civs[civ].diplo_request_count++;
        continue;
      }
      net_send_state_diff(0xff,2,0,0,0);
    }
    if (menu_choice != 2) {
      if (menu_choice == 3) {
        if ((((((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) == 0) &&
              (0 < diplo_eval.cur_attitude)) && (civ3 = civ_has_wonder(civ2,WONDER_MARCO_POLO), civ3 == 0)) &&
            (civ3 = civ_has_wonder(civ2,WONDER_MAGELLAN), civ3 == 0)) ||
           (((0x19 < diplo_eval.cur_attitude || (civ3 = civ_has_advance(civ,ADV_MAP_MAKING), civ3 == 0)) ||
            (civ3 = civ_has_advance(civ2,ADV_MAP_MAKING), civ3 == 0)))) {
          civ_name = civ_get_adjective(civ2);
          text_set_string_arg(1,(const char *)civ_name);
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_mapno);
          return;
        }
        diplo_favor_share_maps(civ2,civ);
      }
      return;
    }
    dlg.load_game_text((const char *)&str_mustattack);
    for (civ4 = 1; civ4 < 8; civ4++) {
      if ((((1 << ((byte)civ4 & 0x1f) & (uint)game.civs_alive) != 0) && (civ4 != civ))
         && (civ4 != civ2)) {
        arg_a = 0;
        civ3 = civ4;
        civ_name = (iptr)(civ_name_plural(civ4));
        dlg.add_option((const char *)civ_name,civ3,arg_a);
      }
    }
    civ3 = dlg.show_modal(0);
    if (civ3 != 0) break;
  }
  civ_name = (iptr)(civ_name_plural(civ3));
  text_set_string_arg(1,(const char *)civ_name);
  if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_CONTACT) == 0) {
    diplo_leader_play_reaction(3);
    popup_game((const char *)&str_nocontact);
  }
  else if (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
    price = diplo_favor_war_price(civ2,civ,civ3);
    text_set_number_arg(0,price);
    distrust = diplo_favor_war_distrust(civ2,civ,civ3);
    if ((distrust < 7) ||
       ((((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) == 0 &&
         ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0)) &&
        ((((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0 &&
         (game.power_rank[civ2] <= game.power_rank[civ3])))))) {
      if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) == 0) {
        if ((((civs[civ].gold < 1000) || (civs[civ].gold <= civs[civ2].gold)) ||
            (diplo_eval.offer_tech_best < 0)) || (diplo_eval.offer_tech_2nd < 0)) {
          choice = popup_game((const char *)&str_mercenary);
        }
        else {
          price = -1;
          text_set_label_arg(2,advances[diplo_eval.offer_tech_best].name);
          text_set_label_arg(3,advances[diplo_eval.offer_tech_2nd].name);
          choice = popup_game((const char *)&str_cybercop);
        }
      }
      else {
        diplo_leader_play_reaction(4);
        choice = popup_game((const char *)&str_eternalallies);
      }
      if (choice != 0) {
        if (price < 0) {
          civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
          civ_give_advance(civ,diplo_eval.offer_tech_2nd,civ2,0,0);
          diplo_eval.offer_tech_best = -1;
          diplo_eval.offer_tech_2nd = -1;
        }
        else {
          if (civs[civ2].gold < price) {
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_unfortunate);
            return;
          }
          civs[civ2].gold -= price;
          civs[civ].gold += price;
        }
        diplo_favor_mercenary_war(civ2,civ,civ3,civ4);
      }
    }
    else {
      diplo_leader_play_reaction(3);
      popup_game((const char *)&str_hellnowewontgo);
    }
  }
  else {
    diplo_leader_play_reaction(3);
    popup_game((const char *)&str_atwar);
  }
  return;
}

/* 0045f0b1 auto */
void __cdecl diplo_gift_menu(iptr civ2,iptr civ)

{
  char *unit_count;
  char advance;
  iptr menu_choice;
  iptr str;
  iptr attitude_value;
  int city;
  iptr unit2;
  iptr level;
  iptr unit;
  int has_advance;
  uint rnd;
  uint rnd_sign;
  iptr city3;
  int choice;
  iptr city2;
  int advance_idx;
  int city_count;
  int advance2;
  int attitude_cost;
  Dialog dlg;
  int gold_amount;
  int i;
  int prev_amount;
  iptr city4;
  int value;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  diplo_set_attitude_arg(civ2,civ);
  menu_choice = popup_game((const char *)&str_giftmenu);
  if (menu_choice == 0) {
    civs[civ].diplo_request_count--;
  }
  else if (menu_choice == 1) {
    diplo_tech_exchange(civ2,civ,0xffffffff,0xffffffff,0,1);
    if (diplo_eval.offer_tech_best < 0) {
      str = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)str);
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_knowno);
    }
    else {
      text_set_label_arg(1,advances[diplo_eval.offer_tech_best].name);
      if (diplo_eval.offer_tech_2nd < 0) {
        choice = popup_game((const char *)&str_techgift);
      }
      else {
        text_set_label_arg(2,advances[diplo_eval.offer_tech_2nd].name);
        choice = popup_game((const char *)&str_techgift2);
      }
      if (choice == 0) {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_wasting);
      }
      else {
        if (choice == 1) {
          value = ai_advance_value(civ,diplo_eval.offer_tech_best);
          value = value << 2;
          civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
        }
        else {
          value = ai_advance_value(civ,diplo_eval.offer_tech_best);
          value = value * 2;
          civ_give_advance(civ,diplo_eval.offer_tech_2nd,civ2,0,0);
        }
        civ_adjust_attitude(civ,civ2,-value);
        diplo_set_attitude_arg(civ2,civ);
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(2);
        popup_game((const char *)&str_accept);
        civs[civ].diplo_request_count -= ((char)diplo_eval.gifts_this_meeting + 2);
        diplo_eval.gifts_this_meeting++;
      }
    }
  }
  else if (menu_choice == 2) {
    dlg.load_game_text((const char *)&str_moneygift);
    value = civs[civ2].gold / 0x32;
    prev_amount = 0;
    for (i = 1; i < 5; i++) {
      gold_amount = ((int)(i * value + (i * value >> 0x1f & 3U)) >> 2) * 0x32;
      if ((0 < gold_amount) && (prev_amount != gold_amount)) {
        prev_amount = gold_amount;
        text_clear();
        text_append_label(0x72);
        text_append_space();
        text_append_gold(gold_amount);
        dlg.add_option((const char *)&textbuf.buf,gold_amount,0);
      }
    }
    gold_amount = dlg.show_modal(0);
    if (gold_amount == 0) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_wasting_2);
    }
    else {
      civs[civ2].gold -= gold_amount;
      civs[civ].gold += gold_amount;
      status_redraw(1);
      attitude_value = diplo_gold_attitude_value(gold_amount);
      civ_adjust_attitude(civ,civ2,-((attitude_value * 3) / 2));
      diplo_set_attitude_arg(civ2,civ);
      str = civ_get_adjective(civ);
      text_set_string_arg(1,(const char *)str);
      diplo_leader_play_reaction(2);
      popup_game((const char *)&str_accept_2);
      civs[civ].diplo_request_count -= ((char)diplo_eval.gifts_this_meeting + 2);
      diplo_eval.gifts_this_meeting++;
    }
  }
  else {
    if (menu_choice == 3) {
      city_count = 0;
      dlg.load_game_section((const char *)&str_militarysource,1);
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if ((cities[city2].id != 0) && (cities[city2].owner == civ2)) {
          dlg.add_option((const char *)&cities[city2].name,city2,0);
          city_count++;
        }
      }
      if (city_count == 0) {
        popup_game((const char *)&str_militarynone);
        civs[civ].diplo_request_count--;
        return;
      }
      city = dlg.show_modal(0);
      if (city < 0) {
        civs[civ].diplo_request_count--;
        return;
      }
      unit2 = tile_first_unit((int)cities[city].x,(int)cities[city].y);
      if (unit2 < 0) {
        popup_game((const char *)&str_militarynone_2);
        civs[civ].diplo_request_count--;
        return;
      }
      city4 = city_find_nearest((int)cities[city].x,(int)cities[city].y,civ,0xffffffff,
                                    -1);
      if ((((city4 < 0) || (level = diplo_attitude_level(diplo_eval.cur_attitude), 4 < level)) ||
          (civs[civ2].num_advances < civs[civ].num_advances)) ||
         ((game.power_rank[civ2] < game.power_rank[civ] ||
          (civs[civ2].military_strength < civs[civ].military_strength)))) {
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_militaryno);
        return;
      }
      text_clear();
      text_append_label(0x77);
      text_append_space();
      text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
      unit = unit_stack_select_dialog(unit2,(const char *)&textbuf.buf,1);
      if (unit < 0) {
        civs[civ].diplo_request_count--;
        return;
      }
      if ((-1 < unit_types[units[unit].type].obsolete_by) &&
         (has_advance = civ_has_advance(civ,(int)unit_types[units[unit].type].obsolete_by),
         has_advance != 0)) {
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_militaryno_2);
        return;
      }
      unit_stack_unlink(unit,1);
      city3 = city4;
      unit_count = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
      *unit_count = *unit_count - 1;
      units[unit].owner = (i8)civ;
      units[unit].visibility = '\0';
      unit_count = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
      *unit_count = *unit_count + 1;
      unit_stack_link(unit,(int)cities[city4].x,(int)cities[city4].y,1);
      units[unit].home_city = -1;
      attitude_cost = (char)unit_types[units[unit].type].cost * 3;
      has_advance = civ_has_advance(civ,(int)unit_types[units[unit].type].prerequisite);
      if (has_advance != 0) {
        attitude_cost = attitude_cost / 2;
      }
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city3);
      map_redraw_tile_all_views((int)cities[city3].x,(int)cities[city3].y);
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
      map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
      civ_adjust_attitude(civ,civ2,-attitude_cost);
      str = civ_get_adjective(civ);
      text_set_string_arg(1,(const char *)str);
      diplo_leader_play_reaction(2);
      popup_game((const char *)&str_accept_3);
      city = civ_has_advance(civ,(int)unit_types[units[unit].type].prerequisite);
      if (city == 0) {
        advance = unit_types[units[unit].type].prerequisite;
        advance2 = -1;
        for (advance_idx = 0; advance_idx < 100; advance_idx++) {
          city = civ_has_advance(civ,advance_idx);
          if ((city == 0) && (city = advance_has_prereq((int)advance,advance_idx), city != 0)) {
            advance2 = advance_idx;
            break;
          }
        }
        if (0 < advance2) {
          rnd = _rand();
          rnd_sign = (int)rnd >> 0x1f;
          if (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign) {
            str = civ_get_adjective(civ);
            text_set_string_arg(1,(const char *)str);
            popup_game((const char *)&str_breakthrough);
            civ_give_advance(civ,advance2,civ2,0,0);
          }
        }
      }
    }
  }
  return;
}

/* 0045fd67 auto */
void __cdecl diplo_ai_senate_hawks_notice(iptr civ,iptr civ2)

{
  iptr name2;
  int has_wonder;
  
  if ((GOV_FUNDAMENTALISM < civs[civ].government) &&
     ((((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 1) == 0)) && (autoplay.active == 0)))) {
    name2 = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)name2);
    has_wonder = civ_has_wonder(civ,WONDER_UNITED_NATIONS);
    if (has_wonder == 0) {
      popup_game((const char *)&str_continuehawks);
    }
    else {
      popup_game((const char *)&str_continueun);
    }
  }
  return;
}

/* 0045fe19 auto */
i32 __cdecl diplo_proposal_menu(iptr civ2,iptr civ)

{
  bool at_peace;
  bool allied;
  int err;
  i32 choice;
  Dialog dlg;
  
  choice = 0;
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  diplo_set_attitude_arg(civ2,civ);
  dlg.load_game_text((const char *)&str_diplomacy);
  err = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_diplomacymenu);
  if (err == 0) {
    text_read_line(1);
    dlg.add_option((const char *)&textbuf.buf,0,0);
    allied = false;
    at_peace = false;
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) {
        at_peace = true;
      }
    }
    else {
      allied = true;
    }
    text_read_line(1);
    if (at_peace) {
      dlg.add_option((const char *)&textbuf.buf,1,0);
    }
    text_read_line(1);
    if ((!at_peace) && (!allied)) {
      dlg.add_option((const char *)&textbuf.buf,2,0);
    }
    text_read_line(1);
    if (allied) {
      dlg.add_option((const char *)&textbuf.buf,3,0);
    }
    text_read_line(1);
    if ((!allied) && (civs[civ2].government < GOV_REPUBLIC)) {
      dlg.add_option((const char *)&textbuf.buf,4,0);
    }
    text_read_line(1);
    if ((at_peace) && (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_CHANGED_THIS_TURN >> 8)) == 0)) {
      dlg.add_option((const char *)&textbuf.buf,5,0);
    }
    text_read_line(1);
    if (allied) {
      dlg.add_option((const char *)&textbuf.buf,6,0);
    }
    text_read_line(1);
    dlg.add_option((const char *)&textbuf.buf,7,0);
    text_read_line(1);
    dlg.add_option((const char *)&textbuf.buf,8,0);
    choice = dlg.show_modal(0);
  }
  text_close_file();
  return choice;
}

/* The parts below call rand() as crt_rand_at("diplo_ai_meet_player"): the
   rand trace names the original function, as the original's does. */
/* Part of diplo_ai_meet_player (00460129): the AI refuses to listen to a player who asked once too often (true: the meeting ends). */
static bool diplo_meet_refuses_audience(iptr civ2,iptr civ)
{
  char patience;
  int advance2;
  
  if (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_WAR_THIS_TURN >> 16)) != 0) {
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    popup_game((const char *)&str_outahere);
    return true;
  }
  patience = civs[civ].diplo_request_count;
  advance2 = diplo_patience_limit(civ2,civ);
  if ((advance2 <= patience) && (((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_MET_THIS_TURN >> 16)) != 0)) {
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_outahere_2);
    }
    else {
      diplo_leader_play_reaction(3);
      popup_game_with_sprite((const char *)&str_outahereally,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                        );
    }
    return true;
  }
  return false;
}

/* Part of diplo_ai_meet_player (00460129): the AI asks the player to cancel a treaty with / declare war on its war target. */
static void diplo_meet_ask_war_on_target(iptr civ2,iptr civ,i32 x,i32 y,bool &propose,bool &declared_war,bool &paid)
{
  int advance2;
  iptr str;
  int variant;
  iptr stance_idx;
  uint rnd;
  uint rnd_sign;
  
  if ((0 < diplo_eval.war_target_civ) &&
     (((diplo_eval.ai_request_mode != 0 || ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0)) &&
      (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)))) {
    stance_idx = 2;
    if ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_PEACE) != 0) {
      stance_idx = 1;
    }
    if ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & TREATY_ALLIANCE) != 0) {
      stance_idx = 0;
    }
    str = (iptr)(civ_name_plural(diplo_eval.war_target_civ));
    text_set_string_arg(1,(const char *)str);
    if (diplo_eval.ai_aggressive == 0) {
      variant = 0x6d;
    }
    else {
      variant = 0x6c;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      variant = 0x6e;
    }
    text_set_label_arg(2,label_table.string_ids[variant]);
    text_clear();
    text_append((const char *)&str_canceltreaty);
    text_append_int(stance_idx);
    advance2 = popup_game((const char *)&textbuf.buf);
    if (advance2 == ADV_ADVANCED_FLIGHT) {
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
        civ_adjust_attitude(civ,civ2,5);
      }
      else {
        if (diplo_eval.ai_request_mode != 0) {
          civ_adjust_attitude(civ,civ2,0x14);
        }
        propose = true;
      }
      civs[civ].diplo_request_count++;
    }
    else {
      diplo_declare_war(civ2,diplo_eval.war_target_civ,civ);
      declared_war = true;
      diplo_eval.ai_aggressive = 0;
      diplo_eval.strongest_enemy = -1;
      diplo_eval.war_target_civ = -1;
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
        rnd = crt_rand_at("diplo_ai_meet_player");
        rnd_sign = (int)rnd >> 0x1f;
        if ((((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) != rnd_sign) &&
           (diplo_eval.tribute_demand = 0, (*(uint *)&diplo_eval.had_ceasefire) != 0)) {
          diplo_set_treaty_flags(civ,civ2,2);
        }
      }
      else {
        diplo_eval.tribute_demand = 0;
        paid = true;
      }
    }
    diplo_set_attitude_arg(civ2,civ);
  }
}

/* Part of diplo_ai_meet_player (00460129): an allied AI eases its tribute and tech demands. */
static void diplo_meet_ally_eases_demands(iptr civ2,iptr civ,bool paid,bool &propose)
{
  if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) != 0) {
    if ((4 < (int)((uint)civs[civ2].reputation -
                  (int)civs[civ].reputation_credit[civ2])) && (!paid)) {
      propose = true;
    }
    if (-1 < diplo_eval.offer_tech_best) {
      if (game.power_rank[civ] < game.power_rank[civ2]) {
        if ((civs[civ].num_advances < civs[civ2].num_advances) ||
           (civs[civ2].gold < civs[civ].gold)) {
          diplo_eval.tribute_demand = 0;
        }
      }
      else if ((civs[civ].num_advances < civs[civ2].num_advances) &&
              (civs[civ2].gold < civs[civ].gold)) {
        diplo_eval.tribute_demand = 0;
      }
    }
    if ((diplo_eval.space_race_threat != 0) && ((*(int *)&diplo_eval.ai_outranks_target) != 0)) {
      propose = true;
      diplo_eval.tribute_demand = 0;
    }
    if (((((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) && (civ == 6)
        ) && (civ2 == 7)) {
      diplo_eval.tribute_demand = 0;
      diplo_eval.offer_tech_best = -1;
      propose = true;
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): does the AI lack a basic military advance the player has (the tech gap). */
static void diplo_meet_tech_gap(iptr civ2,iptr civ,bool &tech_gap)
{
  int advance2;
  
  advance2 = civ_has_advance(civ2,ADV_MOBILE_WARFARE);
  if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ2,ADV_ADVANCED_FLIGHT), advance2 == ADV_ADVANCED_FLIGHT)) {
    advance2 = civ_has_advance(civ2,ADV_CONSCRIPTION);
    if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ2,ADV_TACTICS), advance2 == ADV_ADVANCED_FLIGHT)) {
      advance2 = civ_has_advance(civ2,ADV_GUNPOWDER);
      if ((advance2 != ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ,ADV_GUNPOWDER), advance2 == ADV_ADVANCED_FLIGHT)) {
        tech_gap = true;
      }
    }
    else {
      advance2 = civ_has_advance(civ,ADV_CONSCRIPTION);
      if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ,ADV_TACTICS), advance2 == ADV_ADVANCED_FLIGHT)) {
        tech_gap = true;
      }
    }
  }
  else {
    advance2 = civ_has_advance(civ,ADV_MOBILE_WARFARE);
    if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ,ADV_ADVANCED_FLIGHT), advance2 == ADV_ADVANCED_FLIGHT)) {
      tech_gap = true;
    }
  }
  if ((civs[civ2].num_advances <= civs[civ].num_advances) &&
     (advance2 = civ_has_advance(civ,ADV_CONSCRIPTION), advance2 != ADV_ADVANCED_FLIGHT)) {
    tech_gap = false;
  }
}

/* Part of diplo_ai_meet_player (00460129): the AI demands tribute in gold. */
static void diplo_meet_demand_tribute(iptr civ2,iptr civ,i32 x,i32 y,bool propose,bool tech_gap,bool &paid)
{
  int advance2;
  iptr str;
  int variant;
  int choice;
  
  if (((0 < diplo_eval.tribute_demand) &&
      (((diplo_eval.ai_request_mode == 2 || (propose)) && (diplo_eval.tribute_demand <= civs[civ2].gold)))) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)) {
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      advance2 = civs[civ2].gold / 2;
      if (advance2 < 0x65) {
        advance2 = 100;
      }
      if (advance2 <= diplo_eval.tribute_demand) {
        diplo_eval.tribute_demand = advance2;
      }
    }
    str = (iptr)(civ_leader_name(civ));
    text_set_string_arg(1,(const char *)str);
    str = civ_leader_title(civ);
    text_set_string_arg(2,(const char *)str);
    str = civ_get_adjective(civ);
    text_set_string_arg(3,(const char *)str);
    text_set_label_arg(4,label_table.string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
    text_set_number_arg(0,diplo_eval.tribute_demand);
    text_clear();
    diplo_leader_play_reaction(4);
    text_append((const char *)&str_tribute);
    variant = rand_range(0,3);
    if ((variant == 3) && (civs[civ].military_strength < civs[civ2].military_strength)) {
      variant = 2;
    }
    if ((variant == 2) && (game.first_discoverer[ADV_INVENTION] == '\0')) {
      variant = 1;
    }
    if (tech_gap) {
      variant = 8;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      variant = 9;
    }
    text_append_int(variant);
    if (variant == 9) {
      choice = popup_game_with_sprite((const char *)&textbuf.buf,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                   0xffff7c80)),0);
    }
    else {
      choice = popup_game((const char *)&textbuf.buf);
    }
    if (choice == 1) {
      civs[civ2].gold -= diplo_eval.tribute_demand;
      civs[civ].gold += diplo_eval.tribute_demand;
      diplo_eval.ai_aggressive = 0;
      paid = true;
      status_redraw(1);
      if (civs[civ].diplo_request_count != '\0') {
        civs[civ].diplo_request_count--;
      }
      advance2 = diplo_gold_attitude_value(diplo_eval.tribute_demand);
      civ_adjust_attitude(civ,civ2,-advance2);
      diplo_set_attitude_arg(civ2,civ);
      if ((*(uint *)&diplo_eval.had_ceasefire) != 0) {
        diplo_set_treaty_flags(civ,civ2,2);
      }
    }
    diplo_eval.tribute_demand = 999;
  }
}

/* Part of diplo_ai_meet_player (00460129): the AI demands an advance. */
static void diplo_meet_demand_tech(iptr civ2,iptr civ,i32 x,i32 y,bool propose,bool tech_gap,bool &paid)
{
  int advance2;
  iptr str;
  int variant;
  int choice;
  
  if ((((diplo_eval.ai_request_mode != 0) || (propose)) && (-1 < diplo_eval.offer_tech_best)) &&
     ((diplo_eval.tribute_demand != 999 &&
      (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)))) {
    text_set_label_arg(1,advances[diplo_eval.offer_tech_best].name);
    str = (iptr)(civ_leader_name(civ));
    text_set_string_arg(3,(const char *)str);
    str = civ_leader_title(civ);
    text_set_string_arg(2,(const char *)str);
    text_set_label_arg(4,label_table.string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
    text_clear();
    text_append((const char *)&str_takeciv);
    variant = rand_range(0,2);
    if ((tech_gap) && (variant < 2)) {
      variant = 8;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      variant = 9;
    }
    text_append_int(variant);
    if (variant == 9) {
      choice = popup_game_with_sprite((const char *)&textbuf.buf,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                   0xffff7c80)),0);
    }
    else {
      choice = popup_game((const char *)&textbuf.buf);
    }
    if (choice == 1) {
      civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
      advance2 = ai_advance_value(civ,diplo_eval.offer_tech_best);
      civ_adjust_attitude(civ,civ2,-((advance2 * 3) / 2));
      advance2 = diplo_eval.offer_tech_2nd;
      diplo_eval.offer_tech_best = diplo_eval.offer_tech_2nd;
      diplo_eval.offer_tech_2nd = -1;
      if (((diplo_eval.ai_units_near_cities == 0) || (diplo_eval.ai_aggressive == 0)) || (advance2 < 0)) {
        paid = true;
      }
      else {
        text_set_label_arg(1,advances[advance2].name);
        diplo_leader_play_reaction(4);
        advance2 = popup_game((const char *)&str_overabarrel);
        if (advance2 == ADV_ALPHABET) {
          civ_give_advance(civ,diplo_eval.offer_tech_best,civ2,0,0);
          advance2 = ai_advance_value(civ,diplo_eval.offer_tech_best);
          civ_adjust_attitude(civ,civ2,-((advance2 * 3) / 2));
          diplo_eval.offer_tech_best = -1;
          paid = true;
        }
      }
      if (paid) {
        diplo_eval.ai_aggressive = 0;
        if ((*(uint *)&diplo_eval.had_ceasefire) != 0) {
          diplo_set_treaty_flags(civ,civ2,2);
        }
        if (civs[civ].diplo_request_count != '\0') {
          civs[civ].diplo_request_count--;
        }
      }
    }
    else if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
      civ_adjust_attitude(civ,civ2,
                         (civs[civ2].reputation + 2) -
                         (int)civs[civ].reputation_credit[civ2]);
      civs[civ].diplo_request_count++;
    }
    diplo_set_attitude_arg(civ2,civ);
    diplo_eval.tribute_demand = 999;
  }
}

/* Part of diplo_ai_meet_player (00460129): an unpaid ally dismisses the player and cancels the alliance (true: the meeting ends). */
static bool diplo_meet_ally_dismissed(iptr civ2,iptr civ,i32 x,i32 y,bool paid)
{
  if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) &&
     ((((diplo_eval.ai_aggressive != 0 || (0x31 < diplo_eval.cur_attitude)) || ((diplo_eval.space_race_threat != 0 && ((*(int *)&diplo_eval.ai_outranks_target) != 0)))
       ) && (!paid)))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    if (diplo_eval.tribute_demand == 999) {
      popup_game_with_sprite((const char *)&str_begone0,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
    else {
      popup_game_with_sprite((const char *)&str_begone1,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
    diplo_cancel_alliance(civ,civ2);
    civs[civ2].last_contact_turn[civ] = game.turn;
    if ((diplo_eval.space_race_threat != 0) && (DIFF_KING < game.difficulty)) {
      diplo_eval.ai_declare_war = 1;
    }
    return true;
  }
  return false;
}

/* Part of diplo_ai_meet_player (00460129): the AI may turn aggressive at first contact. */
static void diplo_meet_turn_aggressive(iptr civ2,iptr civ,bool no_contact,bool paid)
{
  int advance2;
  uint rnd;
  uint rnd_sign;
  
  if ((((((no_contact) && (!paid)) && (diplo_eval.tribute_demand == 999)) &&
       ((diplo_eval.ai_aggressive == 0 &&
        (advance2 = diplo_attitude_level(diplo_eval.cur_attitude),
        (int)((-(uint)(diplo_eval.ai_initial_aggressive == 0) & 2) + 2) < advance2)))) &&
      (advance2 = civ_has_wonder(civ2,WONDER_GREAT_WALL), advance2 == ADV_ADVANCED_FLIGHT)) &&
     (advance2 = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT)) {
    rnd = crt_rand_at("diplo_ai_meet_player");
    rnd_sign = (int)rnd >> 0x1f;
    if ((int)(uint)civs[civ].government <
        (int)(6 - (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) - rnd_sign))) {
      diplo_eval.ai_aggressive = 1;
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): an aggressive AI provokes, rejects or threatens the player (true: the meeting ends). */
static bool diplo_meet_threaten_war(iptr civ2,iptr civ,i32 x,i32 y,int player_initiated)
{
  int advance2;
  iptr str;
  
  if (((diplo_eval.ai_aggressive != 0) && (0x32 < diplo_eval.tribute_demand)) &&
     (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0 ||
      (((advance2 = diplo_is_hostile(civ,civ2), advance2 == ADV_ADVANCED_FLIGHT && (0 < diplo_eval.last_contact_turn)) &&
       (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) == 0)))))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    text_clear();
    diplo_leader_play_reaction(4);
    text_append((const char *)&str_provoke_3);
    str = rand_range(0,2);
    text_append_int(str);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    advance2 = civ_get_attitude(civ,civ2);
    if (advance2 < 0x4b) {
      civ_set_attitude(civ,civ2,0x4b);
    }
    popup_game_with_sprite((const char *)&textbuf.buf,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_contact_turn[civ] = game.turn;
    return true;
  }
  if ((diplo_eval.ai_aggressive != 0) && (diplo_eval.tribute_demand == 999)) {
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    advance2 = civ_get_attitude(civ,civ2);
    if (advance2 < 0x4b) {
      civ_set_attitude(civ,civ2,0x4b);
    }
    popup_game_with_sprite((const char *)&str_reject0,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_contact_turn[civ] = game.turn;
    return true;
  }
  if ((diplo_eval.ai_aggressive != 0) &&
     (((diplo_eval.last_contact_turn < 0 || (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) != 0)) ||
      (player_initiated != 0)))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_meeting.emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    advance2 = civ_get_attitude(civ,civ2);
    if (advance2 < 0x4b) {
      civ_set_attitude(civ,civ2,0x4b);
    }
    popup_game_with_sprite((const char *)&str_rid0,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_contact_turn[civ] = game.turn;
    return true;
  }
  return false;
}

/* Part of diplo_ai_meet_player (00460129): an AI at peace proposes an alliance, perhaps against a common enemy. */
static void diplo_meet_propose_alliance(iptr civ2,iptr civ,i32 x,i32 y,bool declared_war,bool &propose,bool &tech_gap,bool &treaty_made)
{
  uint *treaty;
  byte best_rank;
  int advance2;
  iptr str;
  iptr other;
  int ally_count;
  
  if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) &&
      ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) && (diplo_eval.target_is_leader == 0)) {
    ally_count = 0;
    for (other = 1; other < 8; other++) {
      if ((*(byte *)&civs[civ2].treaties[other] & TREATY_ALLIANCE) != 0) {
        ally_count++;
      }
    }
    if ((game.power_rank[civ2] < 6) && (game.power_rank[civ] < 6)) {
      propose = true;
    }
    else {
      propose = false;
    }
    if (((propose) && (diplo_eval.war_target_civ < 1)) && (200 < game.turn)) {
      best_rank = 8;
      for (other = 1; other < 8; other++) {
        if (((((civ != other) && (civ2 != other)) &&
             ((game.power_rank[civ] <= game.power_rank[other] &&
              ((game.power_rank[civ2] <= game.power_rank[other] &&
               (game.power_rank[other] <= best_rank)))))) &&
            ((*(byte *)&civs[civ2].treaties[other] & TREATY_CONTACT) != 0)) &&
           ((((byte)(civs[civ2].treaties[other] >> 8) & (TREATY_WAR >> 8)) == 0 &&
            ((civs[civ].treaties[other] & (TREATY_ALLIANCE | TREATY_WAR)) == 0)))) {
          diplo_eval.war_target_civ = other;
          best_rank = game.power_rank[other];
        }
      }
    }
    if ((diplo_eval.war_target_civ < 1) ||
       ((*(byte *)&civs[civ2].treaties[diplo_eval.war_target_civ] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) {
      tech_gap = false;
    }
    else {
      tech_gap = true;
    }
    if (((((((diplo_eval.ai_fear_level != 0) || (propose)) || (tech_gap)) && ((1 < diplo_eval.peace_inclination || (propose)))) &&
         ((ally_count == 0 && ((diplo_eval.cur_attitude < 0x19 || ((diplo_eval.cur_attitude < 0x32 && (tech_gap)))))))) &&
        ((tech_gap || ((declared_war || (propose)))))) &&
       (((((int)((uint)civs[civ2].reputation -
                (int)civs[civ].reputation_credit[civ2]) < 4 &&
          (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) == 0 && (diplo_eval.captured_city_count < 2))))
         && ((game.power_rank[civ2] < 6 || (game.power_rank[civ] < 5)))) &&
        ((advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT &&
         (advance2 = popup_game_with_sprite((const char *)&str_proposealliance,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                     ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                     0xffff7c80)),0), advance2 == ADV_ADVANCED_FLIGHT)))))) {
      propose = true;
      if (-1 < diplo_eval.war_target_civ) {
        propose = false;
        str = (iptr)(civ_name_plural(diplo_eval.war_target_civ));
        text_set_string_arg(4,(const char *)str);
        diplo_leader_play_reaction(3);
        advance2 = popup_game((const char *)&str_perhapssolidarity_3);
        if (advance2 == ADV_ALPHABET) {
          diplo_declare_war(civ2,diplo_eval.war_target_civ,civ);
          treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + diplo_eval.war_target_civ * 4);
          *treaty = *treaty | 0x80800;
          diplo_eval.war_target_civ = -1;
          propose = true;
        }
        else {
          civ_adjust_attitude(civ,civ2,10);
          diplo_set_attitude_arg(civ2,civ);
          if (((diplo_eval.ai_fear_level == 0) ||
              (2 < (int)((uint)civs[civ2].reputation -
                        (int)civs[civ].reputation_credit[civ2]))) ||
             (0x19 < diplo_eval.cur_attitude)) {
            propose = false;
            diplo_leader_play_reaction(4);
            popup_game((const char *)&str_perhapsdidntprove);
          }
        }
      }
      if (propose) {
        diplo_form_alliance(civ2,civ);
        diplo_set_attitude_arg(civ2,civ);
      }
      treaty_made = true;
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): the AI at war offers or grants a ceasefire, maybe for a concession (true: the war goes on and the meeting ends). */
static bool diplo_meet_offer_ceasefire(iptr civ2,iptr civ,i32 x,i32 y,bool paid,uint hostile,bool &senate_blocked,bool &conceded,bool &treaty_made)
{
  int advance2;
  iptr str;
  int choice;
  int roll;
  iptr advance;
  int distrust;
  int variant;
  
  if ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    if (paid) {
      choice = popup_game_with_sprite((const char *)&str_grantcease,(iptr)&ceasefire_icon_sprite,0);
    }
    else if ((diplo_eval.suppressed_aggression == 0) && (hostile == 0)) {
      str = civ_get_adjective(civ);
      text_set_string_arg(1,(const char *)str);
      choice = popup_game_with_sprite((const char *)&str_proposecease,(iptr)&ceasefire_icon_sprite,0);
    }
    else {
      choice = popup_game_with_sprite((const char *)&str_wallcease,(iptr)&ceasefire_icon_sprite,0);
    }
    if (choice != 0) {
      advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
      if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT)) {
        advance2 = diplo_senate_intervenes(civ2,civ);
        if (advance2 == ADV_ADVANCED_FLIGHT) {
          diplo_ai_senate_hawks_notice(civ2,civ);
        }
        else {
          popup_game_with_sprite((const char *)&str_overrulecease,(iptr)&ceasefire_icon_sprite,0);
          choice = 0;
          senate_blocked = true;
        }
      }
      else {
        str = (iptr)(civ_name_plural(civ));
        text_set_string_arg(1,(const char *)str);
        advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
        if (advance2 == ADV_ADVANCED_FLIGHT) {
          popup_game_with_sprite((const char *)&str_unovercease,(iptr)&ceasefire_icon_sprite,0);
        }
        else {
          popup_game_with_sprite((const char *)&str_wallovercease,(iptr)&ceasefire_icon_sprite,0);
        }
        choice = 0;
        senate_blocked = true;
      }
    }
    distrust = ((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2]) + 1;
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
      distrust = distrust * 2;
    }
    if (choice != 0) {
      if (distrust == 1 || distrust - 1 < 0) {
        roll = 0;
      }
      else {
        roll = crt_rand_at("diplo_ai_meet_player");
        roll = roll % distrust;
      }
      if (roll == 0) {
        if (((choice != 0) && (3 < diplo_eval.ai_fear_level)) && (-1 < diplo_eval.wanted_tech)) {
          text_set_label_arg(1,advances[diplo_eval.wanted_tech].name);
          diplo_leader_play_reaction(4);
          advance2 = popup_game_with_sprite((const char *)&str_giveciv,(iptr)&ceasefire_icon_sprite,0);
          if (advance2 != ADV_ADVANCED_FLIGHT) {
            choice = 0;
            civ_give_advance(civ2,diplo_eval.wanted_tech,civ,0,0);
            conceded = true;
          }
        }
        if (choice != 0) {
          variant = int_clamp(diplo_eval.ai_fear_level * 2 - 4,0,civs[civ].gold / 0x32);
          variant = variant * 0x32;
          advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
          if ((advance2 != ADV_ADVANCED_FLIGHT) || (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 != ADV_ADVANCED_FLIGHT)) {
            variant = variant / 2;
          }
          if (variant != 0) {
            text_set_number_arg(0,variant);
            diplo_leader_play_reaction(4);
            advance2 = popup_game_with_sprite((const char *)&str_givecash,(iptr)&ceasefire_icon_sprite,0);
            if (advance2 != ADV_ADVANCED_FLIGHT) {
              civs[civ].gold -= variant;
              civs[civ2].gold += variant;
              choice = 0;
              status_redraw(1);
              conceded = true;
            }
          }
        }
        if (((choice != 0) && (diplo_eval.ai_fear_level != 0)) &&
           ((civs[civ].num_cities < 2 &&
            ((advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL), advance2 == ADV_ADVANCED_FLIGHT &&
             (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT)))))) {
          text_set_number_arg_v(0,civs[civ].gold);
          diplo_leader_play_reaction(4);
          advance2 = popup_game_with_sprite((const char *)&str_grovel,(iptr)&ceasefire_icon_sprite,0);
          if (advance2 != ADV_ADVANCED_FLIGHT) {
            choice = 0;
            civs[civ2].gold += civs[civ].gold;
            civs[civ].gold = 0;
            status_redraw(1);
            conceded = true;
            for (advance = 0; advance < 100; advance++) {
              advance2 = civ_has_advance(civ,advance);
              if (((advance2 != ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_advance(civ2,advance), advance2 == ADV_ADVANCED_FLIGHT)) &&
                 ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) {
                civ_give_advance(civ2,advance,civ,0,0);
              }
            }
          }
        }
        if (choice == 0) {
          diplo_eval.last_contact_turn = (int)game.turn;
        }
      }
    }
    if (choice != 0) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_continuewar);
      civs[civ2].last_contact_turn[civ] = game.turn;
      return true;
    }
    treaty_made = true;
    diplo_agree_ceasefire(civ2,civ);
    diplo_set_attitude_arg(civ2,civ);
    if (6 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2])) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_worthless);
    }
  }
  return false;
}

/* Part of diplo_ai_meet_player (00460129): the AI proposes a peace treaty, maybe for cash. */
static void diplo_meet_offer_peace(iptr civ2,iptr civ,i32 x,i32 y,bool paid,bool no_contact,bool senate_blocked,bool &conceded,bool &treaty_made)
{
  int advance2;
  uint rnd;
  iptr str;
  uint rnd_sign;
  int roll2;
  int choice;
  int distrust;
  int variant;
  int threshold;
  
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
    threshold = 3;
    if ((uint)civs[civ2].military_strength * 2 < (uint)civs[civ].military_strength) {
      threshold = 2;
    }
    if (((uint)civs[civ2].military_strength * 3) / 2 < (uint)civs[civ].military_strength) {
      threshold--;
    }
    if (civs[civ].military_strength < civs[civ2].military_strength) {
      threshold++;
    }
    if ((uint)civs[civ].military_strength * 2 < (uint)civs[civ2].military_strength) {
      threshold++;
    }
    if ((((diplo_eval.suppressed_aggression != 0) ||
         (((paid || (no_contact)) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) == 0)))
         ) || ((((diplo_eval.ai_fear_level != 0 ||
                 (advance2 = diplo_attitude_level(diplo_eval.cur_attitude), advance2 <= threshold)) &&
                (diplo_eval.target_is_leader == 0)) &&
               ((int)(((uint)civs[civ2].reputation -
                      (int)civs[civ].reputation_credit[civ2]) + diplo_eval.captured_city_count) <
                diplo_eval.ai_fear_level + 4)))) &&
       (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)) {
      str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)str);
      choice = popup_game_with_sprite((const char *)&str_proposepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
      if (choice != 0) {
        advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
        if (((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT)) ||
            ((senate_blocked) &&
             (rnd = crt_rand_at("diplo_ai_meet_player"), rnd_sign = (int)rnd >> 0x1f,
              ((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign))) {
          advance2 = diplo_senate_intervenes(civ2,civ);
          if ((advance2 == ADV_ADVANCED_FLIGHT) ||
              ((senate_blocked) &&
               (rnd = crt_rand_at("diplo_ai_meet_player"), rnd_sign = (int)rnd >> 0x1f,
                ((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign))) {
            if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_CHANGED_THIS_TURN >> 8)) == 0) {
              diplo_ai_senate_hawks_notice(civ2,civ);
            }
          }
          else {
            popup_game_with_sprite((const char *)&str_overrulepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            choice = 0;
          }
        }
        else {
          advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
          if (advance2 == ADV_ADVANCED_FLIGHT) {
            popup_game_with_sprite((const char *)&str_unoverpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
          else {
            popup_game_with_sprite((const char *)&str_walloverpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
          choice = 0;
        }
      }
      if (choice != 0) {
        distrust = ((uint)civs[civ2].reputation -
                   (int)civs[civ].reputation_credit[civ2]) + 1;
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
          distrust = distrust * 2;
        }
        if (distrust == 1 || distrust - 1 < 0) {
          roll2 = 0;
        }
        else {
          roll2 = crt_rand_at("diplo_ai_meet_player");
          roll2 = roll2 % distrust;
        }
        if (roll2 == 0) {
          variant = int_clamp(diplo_eval.ai_fear_level * 2 - 4,0,civs[civ].gold / 0x32);
          variant = variant * 0x32;
          advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
          if ((advance2 != ADV_ADVANCED_FLIGHT) || (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 != ADV_ADVANCED_FLIGHT)) {
            variant = variant / 2;
          }
          if (variant != 0) {
            text_set_number_arg(0,variant);
            advance2 = popup_game_with_sprite((const char *)&str_cashforpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            if (advance2 != ADV_ADVANCED_FLIGHT) {
              civs[civ].gold -= variant;
              civs[civ2].gold += variant;
              choice = 0;
              conceded = true;
              status_redraw(1);
            }
          }
        }
      }
      treaty_made = true;
      if (choice == 0) {
        diplo_sign_peace_treaty(civ2,civ);
        diplo_set_attitude_arg(civ2,civ);
      }
      else {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_nopeace);
      }
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): a player with a bad reputation provokes a war declaration. */
static void diplo_meet_reputation_war(iptr civ2,iptr civ)
{
  iptr other;
  int civ_ally_count;
  
  if (6 < (int)((uint)civs[civ2].reputation -
               (int)civs[civ].reputation_credit[civ2])) {
    civ_ally_count = 0;
    for (other = 1; other < 8; other++) {
      if ((*(byte *)&civs[civ].treaties[other] & TREATY_ALLIANCE) != 0) {
        civ_ally_count++;
      }
    }
    if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) &&
       (((civ_ally_count != 0 || (diplo_eval.target_is_leader != 0)) ||
        (civs[civ2].military_strength < civs[civ].military_strength)))) {
      diplo_eval.ai_declare_war = 1;
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): when the player did not listen, the senate forces a peace treaty or ceasefire. */
static void diplo_meet_senate_forces_treaty(iptr civ2,iptr civ)
{
  int advance2;
  iptr str;
  
  if (diplo_eval.ai_declare_war == 0) {
    advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
    if ((advance2 == ADV_ADVANCED_FLIGHT) && (advance2 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT)) {
      advance2 = diplo_senate_intervenes(civ2,civ);
      if (advance2 == ADV_ADVANCED_FLIGHT) {
        diplo_ai_senate_hawks_notice(civ2,civ);
      }
      else {
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) &&
           (diplo_eval.ai_fear_level != 0)) {
          popup_game_with_sprite((const char *)&str_senatepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          diplo_set_treaty_flags(civ,civ2,4);
          civs[civ2].last_contact_turn[civ] = game.turn + 0x10;
        }
        if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) {
          popup_game_with_sprite((const char *)&str_senatecease,(iptr)&ceasefire_icon_sprite,0);
          diplo_set_treaty_flags(civ,civ2,2);
          civs[civ2].last_contact_turn[civ] = game.turn;
        }
      }
    }
    else {
      str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)str);
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
        if (diplo_eval.ai_fear_level != 0) {
          advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
          if (advance2 == ADV_ADVANCED_FLIGHT) {
            popup_game_with_sprite((const char *)&str_unoverpeace_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
          else {
            popup_game_with_sprite((const char *)&str_walloverpeace_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
          diplo_set_treaty_flags(civ,civ2,4);
          civs[civ2].last_contact_turn[civ] = game.turn + 0x10;
        }
        if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) {
          advance2 = civ_has_wonder(civ,WONDER_GREAT_WALL);
          if (advance2 == ADV_ADVANCED_FLIGHT) {
            popup_game_with_sprite((const char *)&str_unovercease_2,(iptr)&ceasefire_icon_sprite,0);
          }
          else {
            popup_game_with_sprite((const char *)&str_wallovercease_2,(iptr)&ceasefire_icon_sprite,0);
          }
          diplo_set_treaty_flags(civ,civ2,2);
          civs[civ2].last_contact_turn[civ] = game.turn;
        }
      }
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): after a treaty, the AI denounces the civ with the worst reputation with it. */
static void diplo_meet_denounce_notorious(iptr civ2,iptr civ,int player_initiated)
{
  char *counter;
  char patience;
  char rel;
  iptr str;
  iptr other;
  iptr civ3;
  
  if (((civs[civ].treaties[civ2] & (TREATY_VENDETTA | TREATY_AGGRESSOR)) == 0) || (player_initiated == 0))
  {
    civ3 = -1;
    patience = -1;
    for (other = 1; other < 8; other++) {
      if ((((civ != other) && (civ2 != other)) &&
          (((byte)(civs[civ].treaties[other] >> 8) & (TREATY_WAR >> 8)) == 0)) &&
         (rel = civs[civ2].reputation_credit[other], rel <= patience)) {
        civ3 = other;
        patience = rel;
      }
    }
    if (-1 < civ3) {
      str = (iptr)(civ_name_plural(civ3));
      text_set_string_arg(1,(const char *)str);
      counter = (char *)(civ3 + (iptr)((u8 *)&civs + 72) + civ2 * 0x594);
      *counter = *counter + 1;
      text_clear();
      text_append((const char *)&str_notorious);
      if (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
        if (civs[civ2].military_strength < civs[civ].military_strength) {
          diplo_leader_play_reaction(3);
          text_append((const char *)&str_big);
        }
        else {
          diplo_leader_play_reaction(3);
          text_append((const char *)&str_small);
        }
      }
      else {
        diplo_leader_play_reaction(4);
        text_append((const char *)&str_hate);
      }
      popup_game((const char *)&textbuf.buf);
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): the greeting (welcome, howdy, or the attitude complaint). */
static void diplo_meet_greet(iptr civ2,iptr civ,int player_initiated,bool paid)
{
  int advance2;
  iptr str;
  int variant;
  
  if ((diplo_eval.tribute_demand != 999) || (paid)) {
    if (player_initiated == 0) {
      variant = 0x71;
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) {
        if (civs[civ].military_strength < civs[civ2].military_strength) {
          variant = 0x70;
        }
        advance2 = diplo_attitude_level(diplo_eval.cur_attitude);
        if ((advance2 < 4) &&
           ((int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]) < 3)) {
          variant = 0x70;
        }
        if (diplo_eval.ai_fear_level != 0) {
          variant = 0x6f;
        }
      }
      text_set_label_arg(1,label_table.string_ids[variant]);
      str = civ_get_adjective(civ2);
      text_set_string_arg(2,(const char *)str);
      str = civ_leader_title(civ2);
      text_set_string_arg(3,(const char *)str);
      str = (iptr)(civ_leader_name(civ2));
      text_set_string_arg(4,(const char *)str);
      diplo_leader_play_reaction(2);
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
          popup_game((const char *)&str_welcome);
        }
        else {
          popup_game((const char *)&str_welcomepeace);
        }
      }
      else {
        popup_game((const char *)&str_welcomeally);
      }
    }
    else {
      str = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(2,(const char *)str);
      if (civs[civ].diplo_request_count < 2) {
        diplo_leader_play_reaction(2);
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
            popup_game((const char *)&str_howdy);
          }
          else {
            popup_game((const char *)&str_howdypeace);
          }
        }
        else {
          popup_game((const char *)&str_howdyally);
        }
      }
      else {
        diplo_leader_play_reaction(2);
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_doody);
        }
        else {
          diplo_leader_play_reaction(2);
          popup_game((const char *)&str_doodyally);
        }
      }
    }
  }
  else {
    diplo_leader_play_reaction(4);
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
      if ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) {
        popup_game((const char *)&str_attitude);
      }
      else {
        popup_game((const char *)&str_attitudepeace);
      }
    }
    else {
      popup_game((const char *)&str_attitudeally);
    }
  }
}

/* Part of diplo_ai_meet_player (00460129): an ally pleads for help against its strongest enemy or brags about its weakest. */
static void diplo_meet_ally_war_talk(iptr civ2,iptr civ)
{
  iptr str;
  iptr continent;
  int threat_count;
  
  if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) != 0) {
    threat_count = 0;
    if (0 < diplo_eval.strongest_enemy) {
      for (continent = 1; continent < 0x3f; continent++) {
        if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent])
           && (civs[civ].continent_military[continent] <
               civs[diplo_eval.strongest_enemy].continent_attack[continent])) {
          threat_count++;
        }
      }
    }
    if (threat_count == 0) {
      if (0 < diplo_eval.weakest_enemy_civ) {
        threat_count = 0;
        for (continent = 1; continent < 0x3f; continent++) {
          if (((1 < civs[diplo_eval.weakest_enemy_civ].num_cities) <
               civs[diplo_eval.weakest_enemy_civ].continent_cities[continent]) &&
             (civs[diplo_eval.weakest_enemy_civ].continent_military[continent] <
              civs[civ].continent_attack[continent])) {
            threat_count++;
          }
        }
        if (threat_count != 0) {
          str = (iptr)(civ_name_plural(diplo_eval.weakest_enemy_civ));
          text_set_string_arg(1,(const char *)str);
          diplo_leader_play_reaction(3);
          popup_game((const char *)&str_allybrag);
        }
      }
    }
    else {
      str = (iptr)(civ_name_plural(diplo_eval.strongest_enemy));
      text_set_string_arg(1,(const char *)str);
      diplo_leader_play_reaction(2);
      popup_game((const char *)&str_allyplea);
    }
  }
}

/* 00460129 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
diplo_ai_meet_player(iptr civ2,iptr civ,i32 x,i32 y,int player_initiated)

{
  uint *treaty;
  char patience;
  bool propose;
  bool tech_gap;
  bool paid;
  bool declared_war;
  bool senate_blocked;
  bool treaty_made;
  bool ended;
  bool conceded;
  bool emissary_sent;
  iptr allowed;
  i32 limit;
  iptr patience_limit;
  iptr str;
  int advance2;
  bool no_contact;
  uint hostile;
  
  declared_war = false;
  ended = false;
  paid = false;
  propose = false;
  treaty_made = false;
  conceded = false;
  hostile = 0;
  tech_gap = false;
  senate_blocked = false;
  emissary_sent = false;
  diplo_meeting.ai_meeting_active = 1;
  if ((scenario_events_loaded != 0) &&
     (allowed = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_negotiation(civ2,civ),
     allowed == 0)) {
    diplo_meeting.ai_meeting_active = 0;
    return;
  }
  diplo_eval.attitude_target = civ2;
  diplo_eval.attitude_civ = civ;
  diplo_eval.gifts_this_meeting = 0;
  if (player_initiated == 0) {
    civs[civ].diplo_request_count = '\0';
  }
  diplo_meeting.initial_eval_done = 0;
  diplo_meeting.opening_eval_active = 1;
  diplo_eval_ai_stance(civ2,civ);
  diplo_meeting.initial_eval_done = 1;
  diplo_meeting.opening_eval_active = 0;
  (*(uint *)&diplo_eval.had_ceasefire) = (uint)((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) != 0);
  no_contact = (*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0;
  if (player_initiated == 0) {
    civs[civ2].last_contact_turn[civ] = game.turn;
    diplo_clear_treaty_flags(civ,civ2,0x40002);
  }
  else {
    if (civs[civ].diplo_request_count < '\0') {
      civs[civ].diplo_request_count = '\0';
    }
    if (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0) {
      limit = diplo_patience_limit(civ2,civ);
      civs[civ].diplo_request_count = (i8)limit;
    }
    if (((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_INCIDENT >> 16)) == 0) {
      if ((((DIFF_KING < game.difficulty) && (advance2 = civ_spaceship_launched(civ), advance2 != ADV_ADVANCED_FLIGHT)) &&
          (advance2 = civ_spaceship_launched(civ2), advance2 != ADV_ADVANCED_FLIGHT)) &&
         ((civs[civ2].spaceship_arrival_year < civs[civ].spaceship_arrival_year &&
          (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)))) {
        diplo_set_treaty_flags(civ,civ2,0x200000);
      }
    }
    else {
      civs[civ].diplo_request_count = '\0';
      diplo_clear_treaty_flags(civ,civ2,0x200000);
    }
  }
  (*(uint *)&diplo_meeting.emissary_response) = (uint)(player_initiated != 0);
  if (player_initiated != 0) {
    if (diplo_meet_refuses_audience(civ2,civ)) {
      ended = true;
      goto end_meeting;
    }
    diplo_ask_receive_emissary(civ2,civ,x,y);
  }
  diplo_set_treaty_flags(civ2,civ,0x800000);
  emissary_sent = true;
  hostile = (uint)(((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_CITY_CAPTURED >> 16)) != 0);
  diplo_clear_treaty_flags(civ,civ2,0x400000);
  if (((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) {
    if (((civ == 3) && (civ2 == 6)) || ((civ == 6 && (civ2 == 3)))) {
      diplo_eval.war_target_civ = 0;
      diplo_eval.offer_tech_best = -1;
      diplo_eval.tribute_demand = 0;
      hostile = 0;
    }
    if (((civ == 3) && (civ2 == 1)) || ((civ == 1 && (civ2 == 3)))) {
      diplo_eval.offer_tech_best = -1;
      diplo_eval.war_target_civ = 0;
      hostile = 0;
    }
  }
  diplo_tech_exchange(civ2,civ,x,y,1,0);
  if ((((diplo_eval.ai_aggressive == 0) && (player_initiated == 0)) &&
      ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0)) && (2 < diplo_eval.ai_fear_level))
  goto end_meeting;
  diplo_set_attitude_arg(civ2,civ);
  if (((0 < diplo_eval.war_target_civ) && (diplo_eval.war_target_civ < 8)) &&
     ((1 << ((byte)diplo_eval.war_target_civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    diplo_eval.war_target_civ = -1;
  }
  if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) && (diplo_eval.space_race_threat != 0)) &&
     ((*(int *)&diplo_eval.ai_outranks_target) != 0)) {
    diplo_eval.war_target_civ = -1;
  }
  diplo_meet_ask_war_on_target(civ2,civ,x,y,propose,declared_war,paid);
  diplo_meet_ally_eases_demands(civ2,civ,paid,propose);
  diplo_meet_tech_gap(civ2,civ,tech_gap);
  diplo_meet_demand_tribute(civ2,civ,x,y,propose,tech_gap,paid);
  if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) &&
     (advance2 = diplo_attitude_level(diplo_eval.cur_attitude), 3 < advance2)) {
    propose = true;
  }
  diplo_meet_demand_tech(civ2,civ,x,y,propose,tech_gap,paid);
  if (diplo_meet_ally_dismissed(civ2,civ,x,y,paid)) goto end_meeting;
  if ((hostile == 0) || (diplo_eval.ai_aggressive == 0)) {
    hostile = 0;
  }
  else {
    hostile = 1;
  }
  if (hostile != 0) {
    diplo_eval.ai_aggressive = 0;
  }
  diplo_meet_turn_aggressive(civ2,civ,no_contact,paid);
  if (diplo_meet_threaten_war(civ2,civ,x,y,player_initiated)) goto end_meeting;
  if (diplo_eval.ai_aggressive != 0) goto end_meeting;
  if (((diplo_eval.suppressed_aggression != 0) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != ADV_ADVANCED_FLIGHT)) {
    diplo_leader_play_reaction(4);
    advance2 = civ_has_wonder(civ2,WONDER_GREAT_WALL);
    if (advance2 == ADV_ADVANCED_FLIGHT) {
      popup_game((const char *)&str_accursedun);
    }
    else {
      popup_game((const char *)&str_accursedwall);
    }
  }
  diplo_meet_propose_alliance(civ2,civ,x,y,declared_war,propose,tech_gap,treaty_made);
  if (diplo_meet_offer_ceasefire(civ2,civ,x,y,paid,hostile,senate_blocked,conceded,treaty_made)) goto end_meeting;
  diplo_meet_offer_peace(civ2,civ,x,y,paid,no_contact,senate_blocked,conceded,treaty_made);
  if (((diplo_meeting.emissary_asked != 0) || (player_initiated != 0)) ||
     (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0 &&
      (((diplo_eval.ai_fear_level != 0 || (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0))
       || ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0)))))) {
    diplo_meet_reputation_war(civ2,civ);
    if (no_contact) {
      diplo_ask_receive_emissary(civ2,civ,x,y);
    }
    if (((diplo_meeting.emissary_asked == 0) || ((*(uint *)&diplo_meeting.emissary_response) == 0xffffffff)) && (player_initiated == 0)) {
      diplo_meet_senate_forces_treaty(civ2,civ);
    }
    else {
      if (diplo_meeting.emissary_asked == 0) {
        diplo_set_attitude_arg(civ2,civ);
      }
      if (treaty_made) {
        diplo_meet_denounce_notorious(civ2,civ,player_initiated);
      }
      else {
        diplo_meet_greet(civ2,civ,player_initiated,paid);
      }
      if (((*(byte *)&civs[civ].treaties[civ2] & (TREATY_PEACE | TREATY_ALLIANCE)) == 0) &&
         (diplo_count_border_intruders(civ,civ2), border_intruders.intruder_count != 0)) {
        text_set_string_arg(1,(const char *)&cities[border_intruders.intruder_city].name);
        str = civ_get_adjective(civ);
        text_set_string_arg(2,(const char *)str);
        if (border_intruders.intruder_city < 2) {
          popup_game((const char *)&str_pleasecity);
        }
        else {
          popup_game((const char *)&str_pleasecities);
        }
      }
      diplo_meet_ally_war_talk(civ2,civ);
      do {
        if (civs[civ].diplo_request_count < '\0') {
          civs[civ].diplo_request_count = '\0';
        }
        advance2 = diplo_proposal_menu(civ2,civ);
        if (advance2 == ADV_ADVANCED_FLIGHT) {
          ended = true;
          goto end_meeting;
        }
        if (advance2 == ADV_BRIDGE_BUILDING) {
          diplo_favor_menu(civ2,civ);
        }
        else if (advance2 == ADV_BRONZE_WORKING) {
          diplo_gift_menu(civ2,civ);
        }
        else {
          advance2 = diplo_ai_answer_proposal(civ2,civ,advance2);
          if (advance2 != ADV_ADVANCED_FLIGHT) goto end_meeting;
        }
        civs[civ].diplo_request_count++;
        patience = civs[civ].diplo_request_count;
        advance2 = diplo_patience_limit(civ2,civ);
      } while (patience < advance2);
      diplo_set_attitude_arg(civ2,civ);
      diplo_leader_play_reaction(3);
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
        popup_game((const char *)&str_patience);
      }
      else {
        popup_game((const char *)&str_patienceally);
      }
    }
  }
end_meeting:
  diplo_clear_treaty_flags(civ,civ2,0x10000);
  if ((diplo_eval.ai_declare_war == 0) || ((civs[civ2].treaties[civ] & (TREATY_ALLIANCE | TREATY_WAR)) != 0))
  {
    treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
    *treaty = *treaty & 0xffffffdf;
  }
  else {
    treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
    *treaty = *treaty | 0x20;
  }
  if (!ended) {
    civs[civ].diplo_request_count++;
  }
  if ((conceded) || (hostile != 0)) {
    while (patience_limit = diplo_patience_limit(civ2,civ),
          (int)civs[civ].diplo_request_count < (int)(hostile + patience_limit)) {
      civs[civ].diplo_request_count++;
    }
  }
  diplo_session_reset();
  if ((((*(uint *)&diplo_eval.had_ceasefire) != 0) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) == 0)) &&
     ((civs[civ2].treaties[civ] & (TREATY_PEACE | TREATY_ALLIANCE | TREATY_WAR)) == 0)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    popup_game_with_sprite((const char *)&str_ceaseexpire,(iptr)&ceasefire_icon_sprite,0);
  }
  if ((((game.difficulty == DIFF_CHIEFTAIN) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0)) &&
      (((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) == 0 &&
       ((advance2 = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), advance2 == ADV_ADVANCED_FLIGHT &&
        (advance2 = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), advance2 == ADV_ADVANCED_FLIGHT)))))) &&
     (advance2 = civ_has_advance(civ,ADV_WRITING), advance2 != ADV_ADVANCED_FLIGHT)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    dlg_game_txt_with_unit_flags((const char *)&str_ambassadors,0x2e,0);
    treaty = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *treaty = *treaty | 0x80;
    intel_report_show(civ2,civ);
  }
  if (((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (emissary_sent)) &&
     (((diplo_meeting.emissary_asked != 0 || (player_initiated != 0)) &&
      (((((*(uint *)&diplo_meeting.emissary_response) != 0xffffffff && (4 < civs[civ].num_cities)) &&
        (game.power_rank[civ2] + 1 < (uint)game.power_rank[civ])) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)))))) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)str);
    popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_schism);
  }
  diplo_meeting.ai_meeting_active = 0;
  return;
}

/* 00467750 auto */
void __cdecl diplo_clear_treaty_flags(iptr civ,iptr civ2,uint flags)

{
  uint *treaty;
  
  if ((flags & 4) != 0) {
    diplo_clear_treaty_flags(civ,civ2,8);
  }
  if ((flags & 0x2000) != 0) {
    diplo_clear_treaty_flags(civ,civ2,0x1800);
  }
  if ((flags & 1) != 0) {
    diplo_clear_treaty_flags(civ,civ2,0x2000);
  }
  treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
  *treaty = *treaty & ~flags;
  treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
  *treaty = *treaty & ~flags;
  return;
}

/* 00467825 auto */
void __cdecl diplo_set_treaty_flags(iptr civ,iptr civ2,uint flags)

{
  uint *treaty;
  
  if ((flags & 8) != 0) {
    diplo_set_treaty_flags(civ,civ2,4);
  }
  if ((flags & 0xe) != 0) {
    diplo_clear_treaty_flags(civ,civ2,0x2a60);
  }
  if ((flags & 0x2000) != 0) {
    diplo_clear_treaty_flags(civ,civ2,0xe);
    flags = flags | 0x200000;
  }
  treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
  *treaty = *treaty | flags;
  treaty = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
  *treaty = *treaty | flags;
  return;
}

/* 004679ab auto */
i32 __cdecl diplo_attitude_level(int attitude)

{
  i32 level;
  
  if (attitude < 0) {
    level = 0;
  }
  else if (attitude < 0xb) {
    level = 1;
  }
  else if (attitude < 0x1a) {
    level = 2;
  }
  else if (attitude < 0x27) {
    level = 3;
  }
  else if (attitude < 0x3e) {
    level = 4;
  }
  else if (attitude < 0x4b) {
    level = 5;
  }
  else if (attitude < 0x5a) {
    level = 6;
  }
  else if (attitude < 100) {
    level = 7;
  }
  else {
    level = 8;
  }
  return level;
}

/* 00467af0 auto */
uint __cdecl diplo_is_hostile(iptr civ,iptr civ2)

{
  uint result;
  int attitude;
  
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
      if (((byte)*(i32 *)&civs[civ].treaties[civ2] & (TREATY_CONTACT | TREATY_PEACE)) == 1) {
        attitude = civ_get_attitude(civ,civ2);
        result = (uint)(0x31 < attitude);
      }
      else {
        result = 0;
      }
    }
    else {
      result = 0;
    }
  }
  else {
    result = 1;
  }
  return result;
}

/* 00467baf auto */
void __cdecl diplo_expel_units(int civ2,int civ)

{
  int x;
  int y;
  iptr city3;
  iptr landmass;
  int ret;
  iptr unit;
  iptr city2;
  iptr city;
  int best_dist;
  
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == civ2)) {
      x = (int)units[unit].x;
      y = (int)units[unit].y;
      city3 = city_find_nearest(x,y,civ2,0xffffffff,civ);
      if ((-1 < city3) && (cities[city3].owner == civ)) {
        landmass = tile_landmass(x,y);
        ret = tile_is_ocean(x,y);
        if ((ret != 0) ||
           ((*(char *)&civs[civ].continent_cities[landmass] != '\0' &&
            (unit_types[units[unit].type].role != ROLE_TRADE)))) {
          if (ret == 0) {
            city2 = city_find_nearest(x,y,civ2,0xffffffff,-1);
          }
          else {
            best_dist = 9999;
            city2 = -1;
            for (city = 0; city < game.num_cities; city++) {
              if ((((cities[city].id != 0) && (cities[city].owner == civ2)) &&
                  (ret = city_adjacent_to_body(city,landmass), ret != 0)) &&
                 (ret = map_distance_diag(x,y,(int)cities[city].x,
                                             (int)cities[city].y), ret < best_dist)) {
                city2 = city;
                best_dist = ret;
              }
            }
          }
          if ((-1 < city2) && (units[unit].owner == cities[city2].owner)) {
            unit_lift_off_map(unit,0);
            unit_stack_relocate_all(unit,(int)cities[city2].x,(int)cities[city2].y,1);
            if ((moving_units_mode == 1) && (game.active_unit == unit)) {
              map_cursor.x[0] = cities[city2].x;
              map_cursor.y[0] = cities[city2].y;
            }
            units[unit].orders = ORDER_NONE;
          }
        }
      }
    }
  }
  return;
}

/* 00467ef2 auto */
void __cdecl diplo_cancel_alliance(iptr civ,iptr civ2)

{
  iptr name2;
  int has_wonder;
  
  diplo_clear_treaty_flags(civ,civ2,8);
  diplo_expel_units(civ,civ2);
  diplo_expel_units(civ2,civ);
  map_redraw_all_views(human_civ[0],1);
  text_clear();
  text_append((const char *)&str_cancelalliance);
  if (((human_civ[0] == civ) || (human_civ[0] == civ2)) && (autoplay.active == 0)) {
    if (human_civ[0] == civ) {
      name2 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)name2);
      name2 = civ_get_adjective(civ2);
      text_set_string_arg(2,(const char *)name2);
    }
    else {
      name2 = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)name2);
      name2 = civ_get_adjective(civ);
      text_set_string_arg(2,(const char *)name2);
    }
    text_append((const char *)&str_0_5);
    popup_game_with_sprite((const char *)&textbuf.buf,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
  }
  else {
    if ((((((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) == 0) &&
          (has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), has_wonder == 0)) &&
         ((has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0 &&
          (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) == 0 &&
           (has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), has_wonder == 0)))))) &&
        (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0)) && (game.reveal_map == '\0')) {
      return;
    }
    if (autoplay.active == 0) {
      name2 = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)name2);
      name2 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(2,(const char *)name2);
      text_append((const char *)&str_1_3);
      popup_game_with_sprite((const char *)&textbuf.buf,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
  }
  return;
}

/* 00493e50 auto */
void __cdecl diplo_greeting_sound_done_cb(void)

{
  if (diplo_leader.wnd_open != '\0') {
    snd_play_effect(diplo_leader.sound_ids[0],1,1,0);
  }
  return;
}

/* 00493e83: written by hand in src/game/unwind.cpp */

/* 00493f0f auto */
void __cdecl diplo_leader_wnd_launch(iptr civ2,iptr civ)

{
  DiploLeaderWindow *self;
  int ok;
  iptr parent_wnd2;
  iptr tribe_idx;
  DiploLeaderWindow *view;
  int i;
  
  diplo_leader.wnd_open = 0;
  diplo_leader.greeting_sound_id = ((uint)map.seed + civ & 7) + 0x53;
  if (civs[civ].tribe < 1) {
    tribe_idx = ~(int)civs[civ].tribe + 1;
  }
  else {
    tribe_idx = (iptr)civs[civ].tribe;
  }
  diplo_leader.sound_ids[0] = (uint)(byte)(((i8 *)&tribe_leader_sound_set))[tribe_idx] * 4 + 0x70;
  for (i = 0; i < 3; i++) {
    diplo_leader.reaction_sound_ids[i] = diplo_leader.sound_ids[0] + i + 1;
  }
  snd_cd_stop();
  if (diplo_leader.window != 0) {
    if (diplo_leader.window != 0) {
      ((DiploLeaderWindow *)diplo_leader.window)->scalar_destruct(1);
    }
    diplo_leader.window = 0;
  }
  self = (DiploLeaderWindow *)(operator_new(0x108c));
  if (self == (DiploLeaderWindow *)0x0) {
    view = (DiploLeaderWindow *)0x0;
  }
  else {
    view = self->construct();
  }
  diplo_leader.window = view;
  if (((view != (DiploLeaderWindow *)0x0) && (((*((char *)&game.game_options + 2)) & (OPT_DIPLOMACY_SCREEN >> 16)) != 0)) &&
     (ok = view->init(civ2,civ), ok != 0)) {
    diplo_leader.wnd_open = 1;
    if ((iptr)diplo_leader.window == -200) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)diplo_leader.window + 0x110;
    }
    dlg_set_parent_window(parent_wnd2);
    dlg_set_palette((iptr)diplo_leader.window + 0x224);
    diplo_leader.window->show();
  }
  return;
}

/* 00494148 auto */
void __cdecl diplo_leader_wnd_close(void)

{
  if (diplo_leader.wnd_open != '\0') {
    diplo_leader.wnd_open = '\0';
    if (diplo_leader.window != 0) {
      diplo_leader.window->teardown();
      if (diplo_leader.window != 0) {
        ((DiploLeaderWindow *)diplo_leader.window)->scalar_destruct(1);
      }
      diplo_leader.window = 0;
    }
    diplo_leader.greeting_sound_id = 0;
    ui_delay_ticks(0x1e);
    snd_play_effect(-diplo_leader.sound_ids[0],0,0,0);
    snd_sync_music_option();
  }
  return;
}

/* 004941ee auto */
void __cdecl diplo_leader_play_reaction(int reaction)

{
  if ((((diplo_leader.video_active != '\0') && (diplo_leader.window != 0)) &&
      ((reaction == 2 || ((reaction == 3 || (reaction == 4)))))) &&
     ((leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader.window->leader_tribe][1][0] <
       diplo_leader.window->video_cur_frame &&
      (diplo_leader.window->video_cur_frame <
       leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader.window->leader_tribe][1][1])))) {
    diplo_leader.window->pending_flag_1088 = (i8)reaction;
    diplo_leader_video_end_cb();
  }
  return;
}

/* 004942a3 auto */
DiploLeaderWindow *DiploLeaderWindow::construct()

{
  i32 eh_frame;
  i8 *eh_prev;
  
  this->ModalMenuWindow::construct();
  ((SurfaceWindow *)leader_surface_window)->SurfaceWindow::construct();
  surface_ctor((DibSurface *)&frame_right);
  ((GamePalette *)leader_palette)->init();
  ((DllHandle *)leader_dll)->init();
  ((AviPlayer *)leader_video_wnd)->construct();
  gfx_get_screen_rect((iptr)&control_list);
  pending_flag_1088 = '\0';
  return this;
}

/* 004943c9 auto */
void DiploLeaderWindow::destruct()

{
  i8 *eh_prev;
  int eh_state;
  
  eh_state = eh_state & 0xff;
  diplo_leader.video_active = 0;
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0049444f */ { ((AviPlayer *)(((int)this) + 0x65c))->destruct(); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0049445e */ { ((DllHandle *)(((int)this) + 0x658))->free(); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0049446d */ { ((GamePalette *)(((int)this) + 0x224))->destroy(); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0049447c */ { surface_dtor((DibSurface *)(((int)this) + 0x1dc)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0049448b */ { ((SurfaceWindow *)(((int)this) + 200))->SurfaceWindow::destruct(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0049449a */ { ModalMenuWindow::destruct(); }
  return;
}

/* 004944bb auto */
i32 DiploLeaderWindow::init(i32 viewer_civ,iptr leader_civ)

{
  int ok;
  i32 result;
  i8 saved_rgb2[96];
  i8 dll_path[128];
  
  diplo_leader.video_active = 0;
  ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb((iptr)saved_rgb2,10,0x20);
  ((GamePalette *)leader_palette)->set_rgb(10,0x20,(iptr)saved_rgb2);
  this->viewer_civ = (u8)viewer_civ;
  this->leader_civ = (u8)leader_civ;
  leader_tribe = (u8)civs[this->leader_civ].tribe;
  str_clear((char *)dll_path);
  str_append((char *)dll_path,(const char *)&str_civ2_mk_dll);
  ok = file_resolve_path((const char *)dll_path);
  if (ok == 0) {
    result = 0;
  }
  else {
    ((DllHandle *)leader_dll)->load((const char *)dll_path);
    result = civ_city_style_era((uint)this->leader_civ);
    style_era = (u8)result;
    ok = init_tile();
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_init_tile_failed,str_notice_3,0x40);
      result = 0;
    }
    else {
      ok = init_background();
      if (ok == 0) {
        MessageBoxA((HWND)0x0,str_init_background_failed,str_notice_4,0x40);
        result = 0;
      }
      else {
        ok = init_main_art();
        if (ok == 0) {
          MessageBoxA((HWND)0x0,str_init_main_art_failed,str_notice_5,0x40);
          result = 0;
        }
        else {
          ok = init_military();
          if (ok == 0) {
            MessageBoxA((HWND)0x0,str_init_military_failed,str_notice_6,0x40);
            result = 0;
          }
          else {
            ok = init_science();
            if (ok == 0) {
              MessageBoxA((HWND)0x0,str_init_science_failed,str_notice_7,0x40);
              result = 0;
            }
            else {
              ok = init_portrait();
              if (ok == 0) {
                MessageBoxA((HWND)0x0,str_init_portrait_failed,str_notice_8,0x40);
                result = 0;
              }
              else {
                if (((*((char *)&game.game_options + 2)) & (OPT_ANIMATED_HERALDS >> 16)) != 0) {
                  diplo_leader.video_active = init_video();
                }
                result = 1;
              }
            }
          }
        }
      }
    }
  }
  return result;
}

/* 00494704 auto */
void DiploLeaderWindow::show()

{
  handle_show_raise();
  wnd_flush_paint();
  set_palette((iptr)(leader_palette));
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)(leader_palette));
  if (diplo_leader.video_active != '\0') {
    ((SurfaceWindow *)leader_video_wnd)->SurfaceWindow::show();
  }
  ((SurfaceWindow *)leader_surface_window)->SurfaceWindow::show();
  wnd_flush_paint();
  if (diplo_leader.video_active != '\0') {
    ((AviPlayer *)leader_video_wnd)->play_range(
               leader_anim_frame_starts_block.leader_anim_frames[(uint)leader_tribe][0][0],
               leader_anim_frame_starts_block.leader_anim_frames[(uint)leader_tribe][0][1]);
  }
  ((DllHandle *)leader_dll)->dll_handle_free();
  diplo_leader.wnd_busy = 0;
  snd_play_effect(diplo_leader.greeting_sound_id,1,0,(SoundDoneFn)diplo_greeting_sound_done_cb);
  return;
}

/* 004947f0 auto */
void DiploLeaderWindow::teardown()

{
  ((WndBase *)((u8 *)this + 0x110))->hide();
  wnd_flush_paint();
  set_palette((iptr)&game_palette_block.game_palette);
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
  hide();
  wnd_flush_paint();
  destroy_native();
  ((DllHandle *)&leader_dll)->dll_handle_free();
  dlg_set_parent_window(0);
  dlg_set_palette(0);
  ((DibSurface *)leader_surface_window)->wnd_destroy_with_surface();
  destroy_native();
  dialog_set_default_offset(0,0);
  return;
}

/* 0049488e auto */
uint DiploLeaderWindow::init_tile()

{
  int loaded;
  
  ((DibSurface *)&bg_tile_surface)->DibSurface::create(0x40,0x40);
  loaded = ((DibSurface *)&bg_tile_surface)->load_gif_resource(299,10,0x20,(iptr)&game_palette_block.game_palette);
  return (uint)(loaded != 0);
}

/* 004948e6 auto */
i32 DiploLeaderWindow::init_background()

{
  create_with_handlers((const char *)&str_empty_102,0x800,0,0,screen_width_value,screen_height_value + 5,(iptr)&game_palette_block.game_palette,
             (iptr)&main_frame_wnd_block.main_frame_wnd);
  wnd_set_bg_tile((DibSurface *)&bg_tile_surface);
  return 1;
}

/* 00494949 auto */
uint DiploLeaderWindow::init_main_art()

{
  int loaded;
  iptr color_index;
  i32 *red_ptr;
  i32 *green_ptr;
  i32 *blue_ptr;
  i8 rgb_buf[120];
  int wnd_y;
  int wnd_x;
  i32 green;
  GamePalette palette;
  i32 red;
  i32 blue;
  i8 *eh_prev;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  wnd_x = (int)(screen_width_value - 0x280) / 3;
  wnd_y = (int)(screen_height_value - 0x1e0) / 3;
  ((DibSurface *)leader_surface)->wnd_create_with_surface((const char *)&str_empty_103,0x800,wnd_x,wnd_y,0x280,0x1e0,
             (iptr)(leader_palette),(iptr)this);
  loaded = ((DibSurface *)leader_surface)->load_gif_resource(civs[leader_civ].government + 200,10,0xec,
                     (iptr)&palette);
  if (loaded != 0) {
    blue_ptr = &blue;
    green_ptr = &green;
    red_ptr = &red;
    color_index = civ_color_primary((uint)leader_civ);
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_entry(color_index,(iptr)red_ptr,(iptr)green_ptr,(iptr)blue_ptr);
    palette.set_entry(0x91,red,green,blue);
    palette.get_rgb((iptr)rgb_buf,0x6a,0x28);
    ((GamePalette *)leader_palette)->set_rgb(0x6a,0x28,(iptr)rgb_buf)
    ;
    ((DibSurface *)leader_surface)->set_palette((iptr)(leader_palette));
  }
  return (uint)(loaded != 0);
}

/* 00494b5f auto */
i32 DiploLeaderWindow::init_military()

{
  int loaded;
  i8 draw_ctx[16];
  i32 fail_result;
  i32 result;
  int icon_count;
  int step_y;
  int draw_y;
  int i;
  int draw_x;
  Sprite sprite;
  DibSurface surface;
  GamePalette palette;
  i8 *eh_prev;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  icon_count = game.power_rank[leader_civ] - 1;
  if (icon_count == 0) {
    result = 1;
  }
  else {
    loaded = surface.load_gif_resource(0xd7,10,0xec,(iptr)&palette);
    if (loaded == 0) {
      fail_result = 0;
      result = fail_result;
    }
    else {
      draw_x = (uint)style_era * 0x6d + 1;
      draw_y = 0x9c;
      sprite.grab_from_surface((iptr)&surface,9,draw_x,0x9c,0x6c,0x1e);
      draw_x = 0x2d;
      draw_y = 0x82;
      step_y = 0x1f;
      for (i = 0; i < icon_count; i++) {
        sprite.draw((iptr)draw_ctx,(iptr)leader_surface,draw_x,draw_y);
        draw_y += step_y;
      }
      result = 1;
    }
  }
  return result;
}

/* Part of DiploLeaderWindow::init_science (00494e2a): rank the leader civ's advances by the depth of their prerequisites, the deepest six in top_techs. */
void DiploLeaderWindow::science_rank_top_techs(int (&top_techs)[13],int &tech_count)
{
  int value;
  int swap_depth;
  int swap_tech;
  iptr advance;
  int word2;
  int j;
  int depth;
  
  for (advance = 0; advance < 100; advance++) {
    value = civ_has_advance((uint)leader_civ,advance);
    if (value != 0) {
      tech_count++;
      if (map_views_block.tech_prereq_depth_cache[advance] < 1) {
        tech_prereq_walk_count = 0;
        advance_count_prereq_depth(advance);
        tech_prereq_walk_count++;
        map_views_block.tech_prereq_depth_cache[advance] = tech_prereq_walk_count;
        depth = tech_prereq_walk_count;
      }
      else {
        depth = map_views_block.tech_prereq_depth_cache[advance];
      }
      if (top_techs[1] < depth) {
        top_techs[0] = advance;
        top_techs[1] = depth;
        for (j = 1; j < 6; j++) {
          swap_tech = top_techs[j * 2];
          swap_depth = top_techs[j * 2 + 1];
          word2 = j - 1;
          while ((-1 < word2 && (swap_depth < top_techs[word2 * 2 + 1]))) {
            top_techs[word2 * 2 + 2] = top_techs[word2 * 2];
            top_techs[word2 * 2 + 3] = top_techs[word2 * 2 + 1];
            word2--;
          }
          top_techs[word2 * 2 + 2] = swap_tech;
          top_techs[word2 * 2 + 3] = swap_depth;
        }
      }
    }
  }
}

/* Part of DiploLeaderWindow::init_science (00494e2a): draw one advance: its category icon and its name on one or two lines. words, aCStack_641 and line2_buf are the caller's buffers (line2_buf is read before it is written, with what the previous advance left). */
void DiploLeaderWindow::science_draw_tech(iptr advance,GameFont &font2,Sprite (&category_sprites)[5],i8 (&draw_ctx)[16],i32 draw_x,int draw_y,tagRECT &text_rect,int text_width,int (&top_techs)[13],i8 (&text_colors)[4],CHAR (&words)[252],CHAR (&aCStack_641)[53],CHAR (&line2_buf)[51])
{
  int value;
  iptr label;
  int rect_height2;
  int truncated;
  int word2;
  int adv_category;
  tagRECT draw_rect;
  int j;
  char char_pos;
  
  adv_category = (int)(char)advances[advance].category;
  (category_sprites + adv_category)->draw((iptr)draw_ctx,(iptr)leader_surface,draw_x,draw_y);
  for (j = 0; j < 5; j++) {
    str_clear((words + j * 0x32));
  }
  label = (iptr)(label_string(advances[advance].name));
  str_append((char *)words,(const char *)label);
  CharUpperA(words);
  word2 = 0;
  do {
    char_pos = '\0';
    while ((words[(int)char_pos + word2 * 0x32] != '\0' &&
           (words[(int)char_pos + word2 * 0x32] != ' '))) {
      char_pos++;
    }
    if (words[(int)char_pos + word2 * 0x32] != '\0') {
      words[(int)char_pos + word2 * 0x32] = '\0';
      str_append((words + (word2 * 5 + 5) * 10),
                         (const char *)(words + (int)char_pos + word2 * 0x32 + 1));
    }
    word2++;
  } while (char_pos != '\0');
  str_clear((aCStack_641 + 1));
  for (j = 0; (j < 5 && (words[j * 0x32] != '\0'));
      j = j + 1) {
    str_append((aCStack_641 + 1),(const char *)(words + j * 0x32));
    str_append((aCStack_641 + 1),(const char *)&str_empty_104);
    value = font2.text_width((const char *)(aCStack_641 + 1));
    if (text_width < value) break;
  }
  if ((words[j * 0x32] == '\0') || (word2 == 2)) {
    if (word2 == 2) {
      truncated = 0;
      value = lstrlenA(aCStack_641 + 1);
      char_pos = (char)value;
      while ((char_pos != '\0' &&
             (value = font2.text_width((const char *)(aCStack_641 + 1)),
             text_width < value))) {
        aCStack_641[char_pos + 1] = '\0';
        char_pos--;
        truncated = 1;
      }
      if (truncated != 0) {
        aCStack_641[char_pos] = '\0';
        str_append((aCStack_641 + 1),(const char *)&str_empty_105);
      }
    }
    CopyRect(&draw_rect,&text_rect);
    if (top_techs[0xc] != 0) {
      gfx_set_text_color_index(10);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)(aCStack_641 + 1),
                 &draw_rect,0);
      OffsetRect(&draw_rect,-1,-1);
      gfx_set_text_color_index(text_colors[style_era]);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)(aCStack_641 + 1),
                 &draw_rect,0);
    }
  }
  else {
    str_clear((aCStack_641 + 1));
    if (j == 0) {
      str_append((aCStack_641 + 1),(const char *)words);
      word2 = 1;
      value = lstrlenA(line2_buf);
      truncated = 0;
      char_pos = (char)value;
      while ((char_pos != '\0' &&
             (value = font2.text_width((const char *)(aCStack_641 + 1)),
             text_width < value))) {
        aCStack_641[char_pos + 1] = '\0';
        char_pos--;
        truncated = 1;
      }
      if (truncated != 0) {
        aCStack_641[char_pos] = '\0';
        str_append((aCStack_641 + 1),(const char *)&str_empty_106);
      }
    }
    else {
      for (word2 = 0; word2 < j; word2++) {
        str_append((aCStack_641 + 1),(const char *)(words + word2 * 0x32));
      }
    }
    if (top_techs[0xc] != 0) {
      CopyRect(&draw_rect,&text_rect);
      rect_height2 = text_rect.bottom - text_rect.top;
      value = font2.font_cached_height();
      OffsetRect(&draw_rect,0,((rect_height2 >> 1) - value) + 1);
      gfx_set_text_color_index(10);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)(aCStack_641 + 1),
                 &draw_rect,1);
      OffsetRect(&draw_rect,-1,-1);
      gfx_set_text_color_index(text_colors[style_era]);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)(aCStack_641 + 1),
                 &draw_rect,1);
    }
    str_clear((char *)line2_buf);
    for (; word2 < 5; word2++) {
      str_append((char *)line2_buf,(const char *)(words + word2 * 0x32));
    }
    value = lstrlenA(line2_buf);
    truncated = 0;
    char_pos = (char)value;
    while ((char_pos != '\0' &&
           (value = font2.text_width((const char *)line2_buf),
           text_width < value))) {
      line2_buf[char_pos] = '\0';
      char_pos--;
      truncated = 1;
    }
    if (truncated != 0) {
      line2_buf[char_pos - 1] = '\0';
      str_append((char *)line2_buf,(const char *)&str_empty_107);
    }
    if (top_techs[0xc] != 0) {
      value = font2.font_cached_height();
      OffsetRect(&draw_rect,0,value - 3);
      gfx_set_text_color_index(10);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)line2_buf,
                 &draw_rect,1);
      OffsetRect(&draw_rect,-1,-1);
      gfx_set_text_color_index(text_colors[style_era]);
      ((DibSurface *)leader_surface)->draw_text_in_rect((iptr)&font2,(const char *)line2_buf,
                 &draw_rect,1);
    }
  }
}

/* 00494e2a auto */
i32 DiploLeaderWindow::init_science()

{
  int value;
  CHAR aCStack_641 [53];
  i8 draw_ctx[16];
  i32 fail_result;
  i32 result;
  CHAR words[252];
  iptr advance;
  int tech_count;
  int slot;
  int step_y;
  int draw_y;
  GameFont font2;
  i32 draw_x;
  CHAR line2_buf[51];
  Sprite category_sprites[5];
  DibSurface surface;
  int top_techs[13];
  GamePalette palette;
  int text_width;
  tagRECT text_rect;
  i8 text_colors[4];
  i8 *eh_prev;
  
  font2.init(0,0xb);
  ON_SCOPE_EXIT(font2.destroy());
  crt_vector_ctor_iterator(category_sprites,0x3c,5,sprite_init,sprite_destroy);
  ON_SCOPE_EXIT(crt_vector_dtor_iterator(category_sprites,0x3c,5,sprite_destroy));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  text_colors[0] = 0x13;
  text_colors[1] = 0x62;
  text_colors[2] = 0x13;
  text_colors[3] = 0x25;
  top_techs[0] = -1;
  top_techs[1] = -1;
  top_techs[2] = 0xffffffff;
  top_techs[3] = 0xffffffff;
  top_techs[4] = 0xffffffff;
  top_techs[5] = 0xffffffff;
  top_techs[6] = 0xffffffff;
  top_techs[7] = 0xffffffff;
  top_techs[8] = 0xffffffff;
  top_techs[9] = 0xffffffff;
  top_techs[10] = 0xffffffff;
  top_techs[0xb] = 0xffffffff;
  tech_count = 0;
  /* BF-024 (Peter Johnson's ledger): science_draw_tech takes the length of
     the line it trims from line2_buf, which the first advance reads before
     any is drawn; unset, its length can put the NUL past aCStack_641 (53). */
  line2_buf[0] = '\0';
  if (((((*(byte *)&civs[human_civ[0]].treaties[((uint)leader_civ)] & TREATY_EMBASSY) == 0) &&
       (value = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), value == 0)) &&
      (value = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), value == 0)) && (game.difficulty != DIFF_CHIEFTAIN)) {
    top_techs[0xc] = 0;
  }
  else {
    top_techs[0xc] = 1;
  }
  science_rank_top_techs(top_techs,tech_count);
  if (6 < tech_count) {
    tech_count = 6;
  }
  if (tech_count == 0) {
    result = 1;
  }
  else {
    value = surface.load_gif_resource(0xd7,10,0xec,(iptr)&palette);
    if (value == 0) {
      fail_result = 0;
      result = fail_result;
    }
    else {
      surface.set_palette((iptr)(leader_palette));
      draw_x = (uint)style_era * 0x6d + 1;
      draw_y = 1;
      (category_sprites + 3)->grab_from_surface((iptr)&surface,9,draw_x,1,0x6c,0x1e);
      draw_y += 0x1f;
      category_sprites->grab_from_surface((iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      (category_sprites + 1)->grab_from_surface((iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      (category_sprites + 2)->grab_from_surface((iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      (category_sprites + 4)->grab_from_surface((iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      step_y = 0x1f;
      draw_x = 0x1e5;
      draw_y = 0x82;
      SetRect(&text_rect,0,0,0x3a,0x14);
      OffsetRect(&text_rect,0x213,0x89);
      text_width = text_rect.right - text_rect.left;
      for (slot = 5; -1 < slot; slot--) {
        advance = top_techs[slot * 2];
        if (-1 < advance) {
          science_draw_tech(advance,font2,category_sprites,draw_ctx,draw_x,draw_y,text_rect,text_width,top_techs,
                            text_colors,words,aCStack_641,line2_buf);
          OffsetRect(&text_rect,0,step_y);
          draw_y += step_y;
        }
      }
      result = 1;
    }
  }
  return result;
}

/* 00495c35 auto */
i32 DiploLeaderWindow::init_portrait()

{
  int loaded;
  i8 draw_ctx[16];
  i32 result;
  i8 rgb_buf[192];
  Sprite sprite;
  DibSurface surface;
  GamePalette palette;
  i8 *eh_prev;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  loaded = surface.load_gif_resource(civs[leader_civ].leader_portrait + 0xdc,10,0xec,
                     (iptr)&palette);
  if (loaded == 0) {
    result = 0;
  }
  else {
    palette.get_rgb((iptr)rgb_buf,0x2a,0x40);
    ((GamePalette *)leader_palette)->set_rgb(0x2a,0x40,(iptr)rgb_buf)
    ;
    surface.set_palette((iptr)(leader_palette));
    sprite.grab_from_surface((iptr)&surface,9,1,1,0xe1,0x113);
    sprite.draw((iptr)draw_ctx,(iptr)leader_surface,0xd0,0x39);
    result = 1;
  }
  return result;
}

/* 00495e0c auto */
i8 DiploLeaderWindow::init_video()

{
  i8 result;
  int found;
  i8 file_stem[20];
  i8 rgb_buf[300];
  i8 video_path[128];
  GamePalette palette;
  int open_err;
  i8 *eh_prev;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  leader_tribe = (u8)civs[leader_civ].tribe;
  str_clear((char *)video_path);
  str_append((char *)video_path,(const char *)&str_civ2_kings);
  str_clear((char *)file_stem);
  str_append((char *)file_stem,(const char *)&str_hrld);
  str_append((char *)file_stem,(const char *)(((i8 * *)&leader_video_tribe_codes))[civs[leader_civ].tribe]);
  file_stem[7] = 0;
  str_append((char *)video_path,(const char *)file_stem);
  str_append((char *)video_path,(const char *)&str_avi);
  found = file_resolve_path((const char *)video_path);
  if (found == 0) {
    result = 0;
  }
  else {
    ((AviPlayer *)video_player)->create_child((iptr)&str_empty_108,0x200,
               leader_video_x_table_block.leader_video_pos[(uint)leader_tribe][0],
               leader_video_x_table_block.leader_video_pos[(uint)leader_tribe][1],(iptr)leader_surface);
    open_err = ((AviPlayer *)video_player)->open((const char *)video_path);
    if (open_err == 0) {
      ((AviPlayer *)video_player)->set_end_callback(diplo_leader_video_end_cb);
      ((AviPlayer *)video_player)->set_transparent(1);
      palette.load_ctab_res(leader_tribe + 1000);
      palette.get_rgb((iptr)rgb_buf,0x92,100);
      ((GamePalette *)leader_palette)->set_rgb(0x92,100,(iptr)rgb_buf);
      ((DibSurface *)video_player)->avi_player_set_palette((iptr)(leader_palette));
      ((WndBase *)video_window)->set_palette((iptr)(leader_palette));
      ((AviPlayer *)video_player)->show_frame(0);
      result = 1;
    }
    else {
      if (open_err == -0x7ffbfeac) {
        popup_game((const char *)&str_vfwnotregistered_4);
      }
      result = 0;
    }
  }
  return result;
}

/* 00496125 auto */
void __cdecl diplo_leader_video_end_cb(void)

{
  uptr reaction;
  DiploLeaderWindow *wnd;
  
  wnd = diplo_leader.window;
  if (diplo_leader.video_active != '\0') {
    if ((char)diplo_leader.window->pending_flag_1088 == '\0') {
      ((AviPlayer *)((iptr)diplo_leader.window + 0x65c))->loop_range(
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader.window->leader_tribe][1][0],
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader.window->leader_tribe][1][1]);
    }
    else {
      reaction = (uptr)diplo_leader.window->pending_flag_1088;
      diplo_leader.window->pending_flag_1088 = 0;
      ((AviPlayer *)((iptr)wnd + 0x65c))->play_range(
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)wnd->leader_tribe][reaction][0]
                 ,leader_anim_frame_starts_block.leader_anim_frames[(uint)wnd->leader_tribe][reaction][1]);
      snd_play_effect((((i32 *)&diplo_leader.sound_ids))[reaction],1,0,0);
    }
  }
  return;
}

/* 00497bf0 auto */
void *DiploLeaderWindow::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004b75fb auto */
void __cdecl diplo_wnd_escape_close(i32 unused)

{
  i32 wnd2;
  
  wnd2 = wnd_get_active_child();
  if (wnd2 == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 -= 0x48;
  }
  ((Wnd *)(wnd2 + 0x48))->activate();
  diplo_wnd_close_active();
  return;
}

/* 004b76d5 auto */
i32 __cdecl diplo_wnd_close_active(void)

{
  int child;
  iptr civ_name;
  ParleyWindow *wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if ((wnd2 == &parley_wnd_block) && (parley_wnd_block.closed == 0)) {
    if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
      operator_delete((void *)parley_wnd_block.received_offer);
      parley_wnd_block.received_offer = 0;
    }
    if ((((int)parley_wnd_block.other_civ == -1) || (net.nm_parley_cancel_count != 0)) || (net.nm_parley_accept_offer_count != 0)) {
      if ((((int)parley_wnd_block.other_civ != -1) && (net.nm_parley_cancel_count != 0)) && (net.nm_parley_accept_offer_count == 0)) {
        net.nm_parley_cancel_count = 0;
        civ_name = (iptr)(civ_name_plural((int)parley_wnd_block.other_civ));
        text_set_string_arg(0,(const char *)civ_name);
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleycancel_3,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else {
      net_send_message(NM_PARLEYREQUEST_CANCEL,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0
                         ,0,0,0,0,0,0,0);
    }
    diplo_meeting.mp_pending = 0;
  }
  wnd2->closed = 1;
  wnd2->other_civ = 0xffffffff;
  ((WndBase *)((i8 *)wnd2 + 0x48))->hide();
  main_frame_wnd_block.main_frame_wnd.focus_and_raise();
  ui_flush_input();
  return 1;
}

/* 004c59f0 auto */
void __cdecl diplo_espionage_incident(iptr civ,iptr civ2)

{
  uint *contact_flags;
  iptr civ_name;
  int choice;
  uint roll;
  uint sign;
  
  if (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_WAR >> 8)) == 0) {
    if ((*(byte *)&civs[civ2].treaties[civ] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0) {
      civ_adjust_attitude(civ2,civ,0x14);
    }
    else {
      civ_name = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_name);
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)civ_name);
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        if (autoplay.active != 0) {
          return;
        }
        if (civs[civ].government == GOV_FUNDAMENTALISM) {
          return;
        }
        if ((game.multiplayer < 3) || (human_civ[0] == civ2)) {
          if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) == 0) {
            choice = popup_game_with_sprite((const char *)&str_pretext_2,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
            if (choice == 1) {
              diplo_set_treaty_flags(civ,civ2,0x2000);
              civs[civ2].last_contact_turn[civ] = game.turn;
            }
          }
          else {
            choice = popup_game_with_sprite((const char *)&str_pretextallied_2,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                      ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                                      0xffff7c80)),0);
            if (choice == 1) {
              diplo_clear_treaty_flags(civ,civ2,8);
              civs[civ2].last_contact_turn[civ] = game.turn;
            }
          }
        }
        else {
          net_send_state_diff(0xff,2,0,0,0);
          net_send_message(NM_PRETEXT,net.net_slots[net.net_civ_slots[civ2]].id,
                             civ,civ2,0,0,0,0,0,0);
        }
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          if (civs[civ].government == GOV_FUNDAMENTALISM) {
            contact_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
            *contact_flags = *contact_flags | 0x10;
            civ_adjust_attitude(civ2,civ,10);
            if (autoplay.active == 0) {
              civ_name = civ_get_adjective(civ);
              text_set_string_arg(0,(const char *)civ_name);
              popup_game_with_sprite((const char *)&str_incidentterror,(iptr)&government_sprites_block.government_sprite_fundamentalism,0);
            }
          }
          else {
            if (autoplay.active == 0) {
              popup_game_with_sprite((const char *)&str_incidentwar,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
            }
            diplo_declare_war(civ,civ2,-1);
          }
        }
        else {
          if ((spy_last_mission == 2) && (civs[civ].num_advances < civs[civ2].num_advances)) {
            if (autoplay.active != 0) {
              return;
            }
            diplo_eval_ai_stance(civ,civ2);
            diplo_set_attitude_arg(civ,civ2);
            choice = _rand();
            civ_adjust_attitude(civ2,civ,choice % 0xf + 5);
            popup_game_with_sprite((const char *)&str_wimpout,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 & 0xffff7c80
                                              )),0);
            return;
          }
          if (autoplay.active == 0) {
            popup_game_with_sprite((const char *)&str_incidentallied,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 & 0xffff7c80
                                              )),0);
          }
          diplo_declare_war(civ,civ2,-1);
        }
      }
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        if (civs[civ].government != GOV_DEMOCRACY) {
          roll = _rand();
          sign = (int)roll >> 0x1f;
          if (((roll ^ sign) - sign & 1 ^ sign) == sign) {
            return;
          }
          if (civs[civ].government != GOV_REPUBLIC) {
            return;
          }
        }
        if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 1) == 0)) {
          if (autoplay.active == 0) {
            civ_name = civ_get_adjective(civ);
            text_set_string_arg(0,(const char *)civ_name);
            popup_game_with_sprite((const char *)&str_senatescandal,(iptr)(((i8 *)&government_sprites_block.government_sprites) + (uint)civs[civ].government * 0x3c
                                              ),0);
          }
          civ_change_government(civ,0);
        }
      }
    }
  }
  return;
}

/* 0055bbc0 auto */
int __cdecl diplo_count_border_intruders(iptr civ,iptr intruder_civ)

{
  iptr x;
  int y;
  int check;
  uint improvements2;
  iptr unit;
  int intruders;
  iptr city;
  iptr tile;
  
  border_intruders.intruder_city = -1;
  diplo_border_intruded_cities = 0;
  border_intruders.intruder_units = 0;
  border_intruders.intruder_count = 0;
  border_intruders.repeat_offenses = 0;
  if ((*(byte *)&civs[civ].treaties[intruder_civ] & TREATY_ALLIANCE) == 0) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        intruders = 0;
        for (tile = 0; tile < 0x14; tile++) {
          x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile]);
          y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile];
          check = map_in_bounds(x,y);
          if ((((check == 0) || (check = tile_is_ocean(x,y), check != 0)) ||
              (check = tile_city_owner(x,y), -1 < check)) ||
             (check = tile_unit_owner(x,y), check != intruder_civ)) continue;
          improvements2 = tile_improvements(x,y);
          for (unit = tile_first_unit(x,y); -1 < unit;
              unit = unit_next_in_stack(unit)) {
            if (((char)unit_types[units[unit].type].role < 5) &&
               ((units[unit].flags & UNITF_BORDER_INTRUDER) == 0)) {
              if (city != border_intruders.intruder_city) {
                diplo_border_intruded_cities++;
              }
              border_intruders.intruder_city = city;
              if ((units[unit].flags & UNITF_BORDER_WARNED) == 0) {
                units[unit].flags = units[unit].flags | UNITF_BORDER_WARNED;
              }
              else {
                border_intruders.repeat_offenses++;
              }
              units[unit].flags = units[unit].flags | UNITF_BORDER_INTRUDER;
              y = border_intruders.intruder_count;
              intruders++;
              border_intruders.intruder_units++;
              border_intruders.intruder_count++;
              if ((improvements2 & TILE_ROAD) != 0) {
                border_intruders.intruder_count = y + 2;
              }
              if ((improvements2 & TILE_RAILROAD) != 0) {
                border_intruders.intruder_count++;
              }
              if ((improvements2 & TILE_MINE) != 0) {
                border_intruders.intruder_count++;
              }
              if ((improvements2 & TILE_IRRIGATION) != 0) {
                border_intruders.intruder_count++;
              }
              if ((improvements2 & TILE_FORTRESS) != 0) {
                border_intruders.intruder_count += 2;
              }
            }
          }
          /* only the first tile with a unit of intruder_civ is examined */
          if (!bugfix_state()) {
            break;  /* B4: the other tiles are not examined (docs/BUGS.md) */
          }
        }
        if (3 < intruders) {
          border_intruders.repeat_offenses++;
        }
      }
    }
  }
  else {
    border_intruders.intruder_count = 0;
  }
  return border_intruders.intruder_count;
}

/* 0055bef9 auto */
i32 __cdecl diplo_senate_intervenes(iptr civ,iptr civ2)

{
  i32 result;
  int threshold;
  int bonus;
  
  if (civs[civ].government < GOV_REPUBLIC) {
    result = 0;
  }
  else if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 1) == 0)) {
    bonus = 0;
    if (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) != 0) {
      bonus = 0x19;
    }
    threshold = civ_has_wonder(civ,WONDER_UNITED_NATIONS);
    if (threshold != 0) {
      bonus = 0x32;
    }
    threshold = int_clamp(bonus + (uint)civs[civ2].reputation * 0xf,0,0x4b);
    if ((int)(uint)civs[civ].diplo_random < threshold) {
      result = 0;
    }
    else if (civs[civ].government < GOV_DEMOCRACY) {
      if ((civs[civ].flags & CIVF_SENATE_ACTIVE) == 0) {
        result = 0;
      }
      else {
        result = 1;
      }
    }
    else {
      result = 1;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 0055cbd5 auto */
i32 __cdecl diplo_ai_feels_secure(iptr civ,iptr civ2)

{
  byte treaty_flags;
  i32 result;
  int check;
  int their_strength;
  iptr civ3;
  int our_attack;
  iptr continent;
  iptr other_civ;
  int security;
  int shared_cities;
  
  treaty_flags = *(byte *)&civs[civ].treaties[civ2];
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
          check = diplo_is_hostile(civ,civ3);
          if (((check != 0) && (civs[civ2].military_strength < civs[civ3].military_strength))
             && ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_ALLIANCE) == 0)) {
            return 0;
          }
          check = diplo_is_hostile(civ2,civ3);
          if (((check != 0) && (civs[civ].military_strength < civs[civ3].military_strength))
             && ((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) == 0)) {
            return 0;
          }
        }
      }
      if (civs[civ].units_active[UNIT_NUCLEAR_MISSILE] < civs[civ2].units_active[UNIT_NUCLEAR_MISSILE]) {
        result = 0;
      }
      else {
        security = 0;
        for (other_civ = 1; other_civ < 8; other_civ++) {
          if ((other_civ != civ) && (other_civ != civ2)) {
            check = diplo_is_hostile(civ,other_civ);
            if (check != 0) {
              security++;
            }
            check = security;
            if (((((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) &&
                 ((*(byte *)&civs[civ2].treaties[other_civ] & TREATY_ALLIANCE) != 0)) &&
                ((1 << ((byte)other_civ & 0x1f) & (uint)game.human_players) != 0)) &&
               ((check = security + 1, game.power_rank[civ] < game.power_rank[other_civ] &&
                (game.power_rank[civ2] < game.power_rank[other_civ])))) {
              check = security + 2;
            }
            security = check;
            if ((((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) &&
                (((byte)(civs[civ2].treaties[other_civ] >> 8) & (TREATY_WAR >> 8)) != 0)) &&
               ((1 << ((byte)other_civ & 0x1f) & (uint)game.human_players) != 0)) {
              security--;
            }
            if (((game.power_rank[other_civ] == 7) && (199 < game.turn)) &&
               (((byte)(civs[civ2].treaties[other_civ] >> 8) & (TREATY_WAR >> 8)) != 0)) {
              security--;
            }
          }
        }
        check = diplo_is_hostile(civ,civ2);
        if (check == 0) {
          security++;
        }
        if ((treaty_flags & 8) != 0) {
          security++;
        }
        shared_cities = 0;
        our_attack = 0;
        their_strength = 1;
        for (continent = 1; continent < 0x3f; continent++) {
          if ((*(char *)&civs[civ].continent_cities[continent] != '\0') &&
             ((uint)civs[civ].continent_attack[continent] +
              (uint)civs[civ].continent_military[continent] <
              (uint)civs[civ2].continent_attack[continent])) {
            return 0;
          }
          if ((*(short *)&civs[civ].continent_attack[continent] != 0) &&
             (*(short *)&civs[civ2].continent_military[continent] != 0)) {
            our_attack += (uint)civs[civ].continent_attack[continent];
            their_strength = their_strength +
                       ((int)(uint)civs[civ2].continent_attack[continent] >> 1) +
                       (uint)civs[civ2].continent_military[continent];
            shared_cities += (uint)civs[civ].continent_cities[continent];
          }
        }
        if (((our_attack << 2) / their_strength < (security - leaders[civs[civ].tribe].attack) + 4) &&
           (shared_cities != 0)) {
          result = 0;
        }
        else {
          result = 1;
        }
      }
    }
    else {
      result = 1;
    }
  }
  else {
    result = 1;
  }
  return result;
}

/* 0055d1e2 auto */
i32 __cdecl diplo_ai_swap_advances(uptr civ,uptr civ2)

{
  bool leader_far_ahead;
  i32 result;
  int check;
  int adv_value;
  iptr advance3;
  iptr advance2;
  iptr advance;
  int best_value2;
  int best_value;
  uint found_mask;
  
  best_value = 0;
  best_value2 = 0;
  found_mask = 0;
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x20) == 0)) {
    if (((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) == 0) ||
        (((game.difficulty == DIFF_CHIEFTAIN || (civs[game.civ_by_rank[7]].num_cities < 5)) ||
         (game.turn < 0xc9)))) ||
       ((civs[game.civ_by_rank[7]].num_advances <= civs[civ].num_advances ||
        (civs[game.civ_by_rank[7]].num_advances <= civs[civ2].num_advances)))) {
      leader_far_ahead = false;
    }
    else {
      leader_far_ahead = true;
    }
    for (advance = 0; advance < 100; advance++) {
      if ((advances[advance].preq[0] != -2) || (advances[advance].preq[1] != -2)) {
        check = civ_has_advance(civ,advance);
        if ((check == 0) && (check = civ_has_advance(civ2,advance), check != 0)) {
          check = _rand();
          adv_value = ai_advance_value(civ,advance);
          adv_value = check % 3 + adv_value;
          if (((found_mask & 1) == 0) || (best_value < adv_value)) {
            advance2 = advance;
            found_mask = found_mask | 1;
            best_value = adv_value;
          }
        }
        else {
          check = civ_has_advance(civ2,advance);
          if ((check == 0) && (check = civ_has_advance(civ,advance), check != 0)) {
            check = _rand();
            adv_value = ai_advance_value(civ2,advance);
            adv_value = check % 3 + adv_value;
            if (((found_mask & 2) == 0) || (best_value2 < adv_value)) {
              advance3 = advance;
              found_mask = found_mask | 2;
              best_value2 = adv_value;
            }
          }
        }
      }
    }
    if (found_mask == 3) {
      civ_give_advance(civ,advance2,civ2,0,0);
      civ_give_advance(civ2,advance3,civ,0,0);
      result = 1;
    }
    else {
      if (found_mask == 1) {
        if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) &&
           ((((int)((uint)civs[civ].num_advances + (6 - (uint)game.difficulty) * 2) <
              (int)(uint)civs[civ2].num_advances || (leader_far_ahead)) &&
            (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_TRIBUTE >> 16)) == 0)))) {
          diplo_set_treaty_flags(civ,civ2,0x40000);
          civ_give_advance(civ,advance2,civ2,0,0);
          return 1;
        }
      }
      else if ((((found_mask == 2) && ((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) != 0)
                ) && (((int)((uint)civs[civ2].num_advances + (6 - (uint)game.difficulty) * 2) <
                       (int)(uint)civs[civ].num_advances || (leader_far_ahead)))) &&
              (((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_TRIBUTE >> 16)) == 0)) {
        diplo_set_treaty_flags(civ,civ2,0x40000);
        civ_give_advance(civ2,advance3,civ,0,0);
        return 1;
      }
      result = 0;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 0055d685 auto */
i32 __cdecl diplo_ally_join_war(iptr civ,iptr civ2,iptr civ3)

{
  uint *flags_ptr;
  i32 result;
  int roll;
  iptr civ_name;
  
  if ((civs[civ].treaties[civ2] & (TREATY_ALLIANCE | TREATY_WAR)) == 0) {
    if (((byte)(civs[civ3].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) {
      if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) &&
         ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_VENDETTA) != 0)) {
        flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
        *flags_ptr = *flags_ptr | 0x20;
        flags_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
        *flags_ptr = *flags_ptr | 0x20;
      }
      result = 0;
    }
    else {
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0) {
          if ((int)civs[civ2].last_contact_turn[civ] - (int)game.turn < 6) {
            return 0;
          }
          if ((game.power_rank[civ2] < 7) && (roll = _rand(), roll % 3 != 0)) {
            return 0;
          }
        }
        civs[civ2].last_contact_turn[civ] = game.turn;
        civs[civ2].last_contact_turn[civ3] = game.turn;
      }
      civ_name = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_name);
      civ_name = (iptr)(civ_name_plural(civ3));
      text_set_string_arg(1,(const char *)civ_name);
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(2,(const char *)civ_name);
      popup_game_with_sprite((const char *)&str_joinwar,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
      diplo_set_treaty_flags(civ,civ2,0x2000);
      result = 1;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* Part of diplo_civs_meet (0055d8d8): contact with a human player: a remote one is told to meet, a local one meets the AI (true: diplo_civs_meet returns). */
static bool diplo_civs_meet_human(uptr civ,uptr civ2,iptr x,iptr y)
{
  if ((((2 < game.multiplayer) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
      && (human_civ[0] != civ)) &&
     ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0 ||
       (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) != 0)) ||
      ((civs[civ].last_contact_turn[civ2] < 0 ||
       (0xf < (int)game.turn - (int)civs[civ].last_contact_turn[civ2])))))) {
    net_send_message(NM_DO_AIPARLEY,net.net_slots[net.net_civ_slots[civ]].id,
                       civ,civ2,x,y,0,0,0,0);
    return true;
  }
  if ((((2 < game.multiplayer) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
      && (human_civ[0] != civ2)) &&
     ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0 ||
       (((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_INCIDENT >> 16)) != 0)) ||
      ((civs[civ2].last_contact_turn[civ] < 0 ||
       (0xf < (int)game.turn - (int)civs[civ2].last_contact_turn[civ])))))) {
    net_send_message(NM_DO_AIPARLEY,net.net_slots[net.net_civ_slots[civ2]].id,
                       civ2,civ,x,y,0,0,0,0);
    return true;
  }
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
      (human_civ[0] == civ)) &&
     (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0 ||
      (((((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) != 0 ||
        (civs[civ].last_contact_turn[civ2] < 0)) ||
       (0xf < (int)game.turn - (int)civs[civ].last_contact_turn[civ2])))))) {
    if ((2 < game.multiplayer) && (human_civ[0] != civ)) {
      return true;
    }
    diplo_ai_meet_player(civ,civ2,x,y,0);
  }
  if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
      (human_civ[0] == civ2)) &&
     (((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0 ||
      (((((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_INCIDENT >> 16)) != 0 ||
        (civs[civ2].last_contact_turn[civ] < 0)) ||
       (0xf < (int)game.turn - (int)civs[civ2].last_contact_turn[civ])))))) {
    if ((2 < game.multiplayer) && (human_civ[0] != civ2)) {
      return true;
    }
    diplo_ai_meet_player(civ2,civ,x,y,0);
  }
  return false;
}

/* Part of diplo_civs_meet (0055d8d8): two human players in a network game: ask the other for a parley. */
static void diplo_civs_meet_parley(uptr civ,uptr civ2)
{
  int city;
  iptr civ_name;
  
  advisor_window_block.mp_meeting_reply = -1;
  diplo_meeting.mp_pending = 1;
  if (bugfix_ui()) {
    net.nm_parley_cancel_count = 0;  /* B6: a cancel left from an earlier audience does not end this one (docs/BUGS.md) */
  }
  net_send_text_args(0x3d,net.net_slots[net.net_civ_slots[civ2]].id,0,0,
                     civ,0);
  civ_name = (iptr)(civ_leader_name(civ2));
  text_set_string_arg(1,(const char *)civ_name);
  civ_name = (iptr)(civ_name_plural(civ2));
  text_set_string_arg(2,(const char *)civ_name);
  dlg_style.pending_idle_callback = mp_meeting_wait_idle;
  civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
  city = popup_game_sprite_flags((const char *)&str_parleywaiting_2,0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (net.host_handoff_stage == '\0') {
    if (net.nm_parley_cancel_count == 0) {
      if (city == -1) {
        net_send_message(NM_PARLEYREQUEST_CANCEL,net.net_slots[net.net_civ_slots[civ2]].id,0,
                           0,0,0,0,0,0,0);
        parley_wnd_block.other_civ = 0xffffffff;
        diplo_meeting.mp_pending = 0;
      }
      else if (advisor_window_block.mp_meeting_reply < 1) {
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleygoaway_2,(iptr)&popup_info_icon_sprite,0);
        diplo_meeting.mp_pending = 0;
      }
      else if (advisor_window_block.mp_meeting_reply == 1) {
        advisor_window_block.mp_meeting_reply = -1;
        parley_wnd_block.other_civ = civ2;
        dlg_style.pending_idle_callback = mp_meeting_wait_idle;
        civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(0,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_parleyok_2,(iptr)&popup_info_icon_sprite,0);
        if (net.host_handoff_stage == '\0') {
          if (net.nm_parley_cancel_count == 0) {
            parley_wnd_block.open(civ2,3);
          }
          else {
            parley_wnd_block.other_civ = 0xffffffff;
            net.nm_parley_cancel_count = 0;
            civ_name = (iptr)(civ_name_plural(civ2));
            text_set_string_arg(0,(const char *)civ_name);
            dlg_style.pending_idle_callback = turn_popup_timeout_idle;
            popup_game_with_sprite((const char *)&str_parleycancel_6,(iptr)&popup_info_icon_sprite,0);
            diplo_meeting.mp_pending = 0;
          }
        }
        else {
          parley_wnd_block.other_civ = 0xffffffff;
          dlg_style.pending_idle_callback = turn_popup_timeout_idle;
          popup_game_with_sprite((const char *)&str_parleybusy_4,(iptr)&popup_info_icon_sprite,0);
          diplo_meeting.mp_pending = 0;
        }
      }
      else {
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleybusy_5,(iptr)&popup_info_icon_sprite,0);
        diplo_meeting.mp_pending = 0;
      }
    }
    else {
      net.nm_parley_cancel_count = 0;
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(0,(const char *)civ_name);
      dlg_style.pending_idle_callback = turn_popup_timeout_idle;
      popup_game_with_sprite((const char *)&str_parleycancel_5,(iptr)&popup_info_icon_sprite,0);
      parley_wnd_block.other_civ = 0xffffffff;
      diplo_meeting.mp_pending = 0;
    }
  }
  else {
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    popup_game_with_sprite((const char *)&str_parleybusy_3,(iptr)&popup_info_icon_sprite,0);
    diplo_meeting.mp_pending = 0;
  }
}

/* Part of diplo_civs_meet (0055d8d8): two AIs make peace. */
static void diplo_civs_meet_peace(uptr civ,uptr civ2)
{
  int city;
  iptr civ_name;
  
  if ((autoplay.active == 0) && (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0))
  {
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(2,(const char *)civ_name);
    city = civ_has_wonder(civ2,WONDER_UNITED_NATIONS);
    if (city == 0) {
      city = civ_has_wonder(civ2,WONDER_GREAT_WALL);
      if (city != 0) {
        popup_game_with_sprite((const char *)&str_wallforce,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
      }
    }
    else {
      popup_game_with_sprite((const char *)&str_unforce,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
    }
  }
  if ((autoplay.active == 0) && (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0))
  {
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(2,(const char *)civ_name);
    city = civ_has_wonder(civ,WONDER_UNITED_NATIONS);
    if (city == 0) {
      city = civ_has_wonder(civ,WONDER_GREAT_WALL);
      if (city != 0) {
        popup_game_with_sprite((const char *)&str_wallforce_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
      }
    }
    else {
      popup_game_with_sprite((const char *)&str_unforce_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
    }
  }
  if ((autoplay.active == 0) &&
     ((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_ALLIANCE) != 0)) {
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_allymakespeace,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                       ,0);
  }
  else if ((autoplay.active == 0) &&
          ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_ALLIANCE) != 0)) {
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_allymakespeace_2,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                       ,0);
  }
  else if ((autoplay.active == 0) &&
          (((((((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0 ||
               (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)) ||
              (city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0)) ||
             (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
              (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)))) ||
            (city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0)) ||
           (game.reveal_map != '\0')))) {
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_signpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
  }
  diplo_set_treaty_flags(civ,civ2,4);
}

/* Part of diplo_civs_meet (0055d8d8): two AIs at peace form an alliance against a third civ, or end theirs. */
static void diplo_civs_meet_alliance(uptr civ,uptr civ2,iptr x,iptr y)
{
  bool leader_gap;
  bool target_leader;
  int city;
  iptr civ_name;
  uint joined_war2;
  bool first_or_allied;
  bool was_allied;
  uptr civ3;
  uptr civ4;
  uint joined_war;
  int allied_count;
  
  was_allied = (*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0;
  leader_gap = false;
  first_or_allied = was_allied;
  if ((game.power_rank[civ] == 7) && (3 < game.power_rank[civ2])) {
    first_or_allied = false;
    leader_gap = true;
  }
  if ((game.power_rank[civ2] == 7) && (3 < game.power_rank[civ])) {
    first_or_allied = false;
    leader_gap = true;
  }
  allied_count = 0;
  civ3 = 0xffffffff;
  target_leader = false;
  for (civ4 = 1; (int)civ4 < 8; civ4++) {
    if ((civ4 != civ) && (civ4 != civ2)) {
      if (((*(byte *)&civs[civ].treaties[civ4] & TREATY_ALLIANCE) == 0) &&
         ((*(byte *)&civs[civ2].treaties[civ4] & TREATY_ALLIANCE) == 0)) {
        if (((((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) == 0) ||
             (game.power_rank[civ4] != 7)) || (game.difficulty == DIFF_CHIEFTAIN)) ||
           ((civs[civ4].num_cities < 5 || (game.turn < 0xc9)))) {
          target_leader = false;
        }
        else {
          target_leader = true;
        }
        if (target_leader) {
          civ3 = civ4;
          break;
        }
        if (game.difficulty != DIFF_CHIEFTAIN) {
          if (first_or_allied) {
            if (((*(byte *)&civs[civ].treaties[civ4] & TREATY_PEACE) == 0) &&
               ((*(byte *)&civs[civ2].treaties[civ4] & TREATY_PEACE) == 0)) {
              civ3 = civ4;
              break;
            }
          }
          else if (((!leader_gap) && (city = diplo_is_hostile(civ,civ4), city != 0)) &&
                  ((game.power_rank[civ] <= game.power_rank[civ4] &&
                   ((city = diplo_is_hostile(civ2,civ4), city != 0 &&
                    (game.power_rank[civ2] <= game.power_rank[civ4])))))) {
            civ3 = civ4;
            break;
          }
        }
      }
      else {
        allied_count++;
      }
    }
  }
  if ((((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) &&
     ((((civ == 6 && (civ2 == 7)) || ((civ == 7 && (civ2 == 6)))) &&
      (1 < game.turn)))) {
    civ3 = 0;
  }
  if (((int)civ3 < 1) ||
     ((((!target_leader || ((int)(uint)civs[game.player_civ].reputation < allied_count)) &&
       (allied_count != 0)) && (!first_or_allied)))) {
    if (((was_allied) && (autoplay.active == 0)) &&
       (((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0 ||
        (((city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0 ||
          (city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0)) ||
         (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
          (((city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0 ||
            (city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0)) ||
           (game.reveal_map != '\0')))))))))) {
      civ_name = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_name);
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)civ_name);
      popup_game_with_sprite((const char *)&str_cancelallied_2,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                        ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 & 0xffff7c80
                                        )),0);
    }
    diplo_clear_treaty_flags(civ,civ2,8);
  }
  else {
    if (!was_allied) {
      joined_war = 0;
      if (((((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0) ||
           (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)) ||
          ((city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0 ||
           (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
            (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)))))) ||
         (((city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0 ||
           (game.reveal_map != '\0')) ||
          (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0 &&
           (((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_CONTACT) != 0 ||
            ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_CONTACT) != 0)))))))) {
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(0,(const char *)civ_name);
        civ_name = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(1,(const char *)civ_name);
        if ((autoplay.active == 0) && (target_leader)) {
          city = city_find_nearest(x,y,civ,0xffffffff,civ2);
          if (-1 < city) {
            text_set_string_arg(2,(const char *)&cities[city].name);
          }
          civ_name = civ_get_adjective(civ3);
          text_set_string_arg(3,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_signnato,(iptr)&diplo_popup_portrait_sprite,0);
        }
        else if (autoplay.active == 0) {
          civ_name = (iptr)(civ_name_plural(civ3));
          text_set_string_arg(2,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_signallied,(iptr)&signal_allied_icon_sprite,0);
        }
        joined_war = diplo_ally_join_war(civ,civ3,civ2);
        joined_war2 = diplo_ally_join_war(civ2,civ3,civ);
        joined_war = joined_war | joined_war2;
      }
      if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
          (city = civ_spaceship_launched(civ3), city == 0)) && (joined_war == 0)) {
        if (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
          diplo_set_treaty_flags(civ,civ3,0x10000);
        }
        if (((byte)(civs[civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) == 0) {
          diplo_set_treaty_flags(civ2,civ3,0x10000);
        }
      }
    }
    diplo_set_treaty_flags(civ,civ2,8);
  }
}

/* Part of diplo_civs_meet (0055d8d8): two AIs go to war. */
static void diplo_civs_meet_war(uptr civ,uptr civ2)
{
  uint *flags_ptr;
  int city;
  iptr civ_name;
  
  if ((autoplay.active == 0) &&
     ((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_ALLIANCE) != 0)) {
    city = diplo_ai_feels_secure(civ,civ2);
    if (city == 0) {
      diplo_ally_demand_help(civ,civ2);
    }
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_allymakeswar,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                      );
  }
  else if ((autoplay.active == 0) &&
          ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_ALLIANCE) != 0)) {
    city = diplo_ai_feels_secure(civ2,civ);
    if (city == 0) {
      diplo_ally_demand_help(civ2,civ);
    }
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_allymakeswar_2,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                      );
  }
  else if ((autoplay.active == 0) &&
          (((((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0 ||
             (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)) ||
            (city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0)) ||
           ((((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0 ||
             (city = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), city != 0)) ||
            ((city = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), city != 0 ||
             (game.reveal_map != '\0')))))))) {
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) ||
       ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0)) {
      popup_game_with_sprite((const char *)&str_declarewar,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    }
    else {
      popup_game_with_sprite((const char *)&str_cancelpeace_3,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    }
  }
  diplo_clear_treaty_flags(civ,civ2,4);
  diplo_set_treaty_flags(civ,civ2,0x2000);
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
    diplo_clear_treaty_flags(civ,civ2,0x10);
    flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
    *flags_ptr = *flags_ptr | 0x800;
  }
}

/* 0055d8d8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diplo_civs_meet(uptr civ,uptr civ2,iptr x,iptr y)

{
  int city;
  bool first_or_allied;
  
  if (civ == 0) {
    return;
  }
  if (civ2 == 0) {
    return;
  }
  if (game.turn == 0) {
    return;
  }
  if ((scenario_events_loaded != 0) &&
     (city = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_negotiation(civ,civ2),
     city == 0)) {
    return;
  }
  if (((game.multiplayer == '\0') || ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)
      ) || ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) {
    if (diplo_civs_meet_human(civ,civ2,x,y)) {
      return;
    }
  }
  else if (game.multiplayer < 3) {
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ)) &&
       (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0 ||
        (((((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_INCIDENT >> 16)) != 0 ||
          (civs[civ].last_contact_turn[civ2] < 0)) ||
         (0xf < (int)game.turn - (int)civs[civ].last_contact_turn[civ2])))))) {
      diplo_ai_meet_player(civ,civ2,x,y,0);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ2)) &&
       ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0 ||
         ((((byte)(civs[civ2].treaties[civ] >> 16) & (TREATY_INCIDENT >> 16)) != 0 ||
          (civs[civ2].last_contact_turn[civ] < 0)))) ||
        (0xf < (int)game.turn - (int)civs[civ2].last_contact_turn[civ])))) {
      diplo_ai_meet_player(civ2,civ,x,y,0);
    }
  }
  else if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
          (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0 ||
           ((civs[civ].last_contact_turn[civ2] < 0 ||
            (0xf < (int)game.turn - (int)civs[civ].last_contact_turn[civ2]))))))
  {
    diplo_civs_meet_parley(civ,civ2);
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0) {
    diplo_set_treaty_flags(civ,civ2,0x4000);
  }
  first_or_allied = (*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0;
  diplo_set_treaty_flags(civ,civ2,0x401);
  if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) &&
      ((((int)game.turn + civ2 + civ & 3) == 0 || (first_or_allied)))) &&
     ((((game.game_flags & GAME_SCENARIO) == 0 || (((int)(*(short *)&scenario.flags) & 0x8000U) == 0)) ||
      ((((civ != 3 || (civ2 != 1)) && ((civ != 1 || (civ2 != 3)))) || (4 < game.turn))
      )))) {
    city = diplo_ai_feels_secure(civ,civ2);
    if (((((city == 0) || (city = civ_has_wonder(civ2,WONDER_GREAT_WALL), city != 0)) ||
         (city = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), city != 0)) ||
        ((GOV_FUNDAMENTALISM < civs[civ].government &&
         (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) &&
       (((city = diplo_ai_feels_secure(civ2,civ), city == 0 ||
         ((city = civ_has_wonder(civ,WONDER_GREAT_WALL), city != 0 ||
          (city = civ_has_wonder(civ,WONDER_UNITED_NATIONS), city != 0)))) ||
        ((GOV_FUNDAMENTALISM < civs[civ2].government &&
         (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))))) {
      diplo_ai_swap_advances(civ,civ2);
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
        diplo_civs_meet_peace(civ,civ2);
      }
      else {
        diplo_civs_meet_alliance(civ,civ2,x,y);
      }
    }
    else if ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) || (first_or_allied)) {
      diplo_civs_meet_war(civ,civ2);
    }
  }
  return;
}

/* 0055f5a3: written by hand in src/game/bigframes.cpp */

/* 0055f7d1 auto */
void __cdecl diplo_ai_military_aid(iptr civ)

{
  char *unit_count;
  u8 unit_attack;
  u8 unit_defense;
  iptr x2;
  iptr y2;
  iptr in_bounds;
  int x;
  int y;
  iptr tile_owner2;
  int city2;
  iptr civ_name;
  iptr unit2;
  iptr unit;
  iptr civ3;
  iptr city;
  int war_count;
  iptr civ2;
  int best_score;
  
  war_count = 0;
  for (civ2 = 1; civ2 < 8; civ2++) {
    if ((civ2 != civ) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)) {
      war_count++;
    }
  }
  civ2 = 1;
  do {
    if (7 < civ2) {
      return;
    }
    if (((civ2 != civ) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0))
       && ((civs[civ2].military_strength <= civs[civ].military_strength || (war_count == 0)))
       ) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((((civ3 != civ) && (civ3 != civ2)) &&
             (((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0)) &&
            ((((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_FIGHTING >> 8)) != 0 &&
             (civs[civ2].military_strength <= civs[civ3].military_strength)))) &&
           (game.power_rank[civ2] <= game.power_rank[civ3])) {
          unit2 = -1;
          best_score = 0;
          for (unit = 0; unit < game.num_units; unit++) {
            if (((units[unit].id != 0) && (units[unit].owner == civ)) &&
               (((char)unit_types[units[unit].type].role < 2 &&
                (unit_types[units[unit].type].domain == DOMAIN_LAND)))) {
              x2 = (iptr)units[unit].x;
              y2 = (iptr)units[unit].y;
              in_bounds = map_in_bounds(x2,y2);
              if ((((in_bounds == 0) && (x = tile_city_owner(x2,y2), -1 < x)) &&
                  (x = unit_stack_sum(unit,2), 1 < x)) &&
                 ((x = tile_landmass(x2,y2),
                  *(char *)&civs[civ].continent_policy[x] == 4 ||
                  ((*(char *)&civs[civ].continent_policy[x] == 5 && (war_count == 0))))))
              {
                unit_attack = unit_types[units[unit].type].attack;
                unit_defense = unit_types[units[unit].type].defense;
                x = unit_max_hp(unit);
                x = ((int)(char)unit_defense + (char)unit_attack * 2) * x;
                if (best_score < x) {
                  unit2 = unit;
                  best_score = x;
                }
              }
            }
          }
          if (-1 < unit2) {
            for (city = 0; city < game.num_cities; city++) {
              if ((cities[city].id != 0) && (cities[city].owner == civ2)) {
                x = (int)cities[city].x;
                y = (int)cities[city].y;
                tile_owner2 = tile_unit_owner(x,y);
                if (((-1 < tile_owner2) &&
                    ((((cities[city].flags & CITYF_COASTAL) != 0 &&
                      (city2 = tile_landmass(x,y),
                      *(char *)&civs[civ2].continent_policy[city2] == 1)) &&
                     (city2 = tile_zoc_unit_adjacent(x,y,civ2), city2 == 0)))) &&
                   (city2 = city_at((int)units[unit2].x,(int)units[unit2].y),
                   -1 < city2)) {
                  unit_stack_unlink(unit2,1);
                  unit_count = (char *)(units[unit2].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
                  *unit_count = *unit_count - 1;
                  units[unit2].owner = (byte)civ2;
                  units[unit2].home_city = (i8)city;
                  units[unit2].visibility = '\0';
                  unit_count = (char *)(units[unit2].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
                  *unit_count = *unit_count + 1;
                  unit_stack_link(unit2,x,y,1);
                  ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
                  map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
                  ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city2);
                  map_redraw_tile_all_views((int)cities[city2].x,(int)cities[city2].y);
                  if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
                     (autoplay.active == 0)) {
                    civ_name = (iptr)(civ_name_plural(civ));
                    text_set_string_arg(0,(const char *)civ_name);
                    text_set_label_arg(1,unit_types[units[unit2].type].name);
                    civ_name = (iptr)(civ_name_plural(civ2));
                    text_set_string_arg(2,(const char *)civ_name);
                    if (human_civ[0] == civ3) {
                      dlg_game_txt_with_unit((const char *)&str_militaryaid1_2,unit2);
                    }
                    else if (2 < game.multiplayer) {
                      net_send_text_args(0x51,net.net_slots[net.net_civ_slots[civ3]].id,3,0,
                                         unit2,0);
                    }
                  }
                  if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
                     (autoplay.active == 0)) {
                    civ_name = (iptr)(civ_name_plural(civ));
                    text_set_string_arg(0,(const char *)civ_name);
                    text_set_label_arg(1,unit_types[units[unit2].type].name);
                    text_set_string_arg(2,(const char *)&cities[city].name);
                    civ_name = (iptr)(civ_name_plural(civ3));
                    text_set_string_arg(3,(const char *)civ_name);
                    if (human_civ[0] == civ2) {
                      dlg_game_txt_with_unit((const char *)&str_militaryaid2_2,unit2);
                    }
                    else if (2 < game.multiplayer) {
                      net_send_text_args(0x52,net.net_slots[net.net_civ_slots[civ2]].id,4,0,
                                         unit2,0);
                    }
                  }
                  if (game.multiplayer < 3) {
                    return;
                  }
                  net_send_state_diff(0xff,2,0,0,0);
                  XD_FlushSendBuffer(5000);
                  return;
                }
              }
            }
          }
        }
      }
    }
    civ2++;
  } while( true );
}

/* Part of diplo_ai_review_relations (00560d95): units of a civ at peace intrude again: it withdraws them or the AI declares war. */
static void diplo_review_violators(iptr civ,uptr civ2,iptr city,iptr x,int y)
{
  iptr civ_name;
  uint rand_val;
  uint sign;
  iptr unit;
  int choice;
  
  diplo_eval_ai_stance(civ2,civ);
  diplo_set_attitude_arg(civ2,civ);
  if (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) == 0) &&
     ((int)cities[city].owner != (civ2 & 0xff))) {
    city_reveal_to_civ(city,civ2);
    tile_set_visible(x,y,1 << ((byte)civ2 & 0x1f),1,1);
    map_redraw_tile_all_views(x,y);
  }
  text_set_string_arg(1,(const char *)&cities[city].name);
  if (diplo_border_intruded_cities < 2) {
    choice = popup_game_with_sprite((const char *)&str_violator,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
  }
  else {
    choice = popup_game_with_sprite((const char *)&str_violators,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
  }
  if (choice == 0) {
    for (unit = 0; unit < game.num_units; unit++) {
      if ((((units[unit].id != 0) && ((units[unit].flags & UNITF_BORDER_INTRUDER) != 0)) &&
          (units[unit].flags = units[unit].flags & ~UNITF_BORDER_INTRUDER,
          (int)units[unit].owner == civ2)) &&
         (y = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ2,
                                     0xffffffff,-1), -1 < y)) {
        unit_relocate(unit,(int)cities[y].x,(int)cities[y].y,1);
        units[unit].visibility = '\0';
        units[unit].orders = ORDER_NONE;
        if (unit_types[units[unit].type].range != '\0') {
          units[unit].counter = '\0';
        }
      }
    }
    map_redraw_all_views(human_civ[0],1);
    text_set_number_arg(0,border_intruders.intruder_units);
    text_clear();
    text_append((const char *)&str_withdrawn);
    if (border_intruders.intruder_units == 1) {
      text_append((const char *)&str_1_6);
    }
    popup_game_with_sprite((const char *)&textbuf.buf,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
  }
  else {
    if ((civs[civ2].government == GOV_DEMOCRACY) ||
        ((rand_val = crt_rand_at("diplo_ai_review_relations"), sign = (int)rand_val >> 0x1f,
          ((rand_val ^ sign) - sign & 1 ^ sign) != sign) &&
         (civs[civ2].government == GOV_REPUBLIC))) {
      if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 1) == 0)) {
        civ_name = civ_get_adjective(civ2);
        text_set_string_arg(0,(const char *)civ_name);
        popup_game((const char *)&str_senatescandal_2);
        civ_change_government(civ2,0);
      }
    }
    civ_adjust_attitude(civ,civ2,0x32);
    diplo_declare_war(civ2,civ,-1);
    civ_name = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)civ_name);
    popup_game_with_sprite((const char *)&str_violate,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
  }
  diplo_session_reset();
}

/* Part of diplo_ai_review_relations (00560d95): units inside the borders of the AI: it complains, admires the city, or reproaches a civ at peace (true: the intruding units keep their flags). */
static bool diplo_review_border_incident(iptr civ,uptr civ2,int &attitude_delta)
{
  iptr x;
  int y;
  iptr civ_name;
  uint rand_val;
  int value;
  uint sign;
  iptr city;
  int roll;
  int choice;
  
  diplo_count_border_intruders(civ,civ2);
  city = border_intruders.intruder_city;
  if ((diplo_border_intruded_cities != 0) && ((civs[civ].treaties[civ2] & (TREATY_ALLIANCE | TREATY_WAR)) == 0))
  {
    x = (iptr)cities[border_intruders.intruder_city].x;
    y = (int)cities[border_intruders.intruder_city].y;
    if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
      if (setup_picwnd_block.diplo_recent_peace_table[civ2] != 0) return true;
      attitude_delta = border_intruders.intruder_count;
      rand_val = crt_rand_at("diplo_ai_review_relations");
      sign = (int)rand_val >> 0x1f;
      choice = ((rand_val ^ sign) - sign & 1 ^ sign) - sign;
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) {
        border_intruders.repeat_offenses++;
      }
      if (2 < (int)((uint)civs[civ2].reputation + border_intruders.repeat_offenses)) {
        value = (uint)civs[civ2].reputation + border_intruders.repeat_offenses;
        if (value == 1 || value - 1 < 0) {
          roll = 0;
        }
        else {
          roll = crt_rand_at("diplo_ai_review_relations");
          roll = roll % (int)((uint)civs[civ2].reputation + border_intruders.repeat_offenses);
        }
        if (1 < roll) {
          choice = 0;
          diplo_set_treaty_flags(civ,civ2,0x20);
        }
      }
      if ((game.turn + civ & 3U) == 0) {
        value = diplo_ask_receive_emissary(civ2,civ,x,y);
        if (value != 0) {
          if (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) == 0) &&
             ((int)cities[city].owner != (civ2 & 0xff))) {
            city_reveal_to_civ(city,civ2);
            map_redraw_tile_all_views(x,y);
          }
          text_set_string_arg(1,(const char *)&cities[city].name);
          if (choice == 0) {
            popup_game_with_sprite((const char *)&str_nearcity,(iptr)&ceasefire_icon_sprite,0);
          }
          else {
            popup_game_with_sprite((const char *)&str_admirecity,(iptr)&ceasefire_icon_sprite,0);
          }
        }
        diplo_session_reset();
      }
    }
    else {
      if (setup_picwnd_block.diplo_recent_peace_table[civ2] != 0) {
        text_set_string_arg(1,(const char *)&cities[border_intruders.intruder_city].name);
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(2,(const char *)civ_name);
        civ_name = civ_get_adjective(civ);
        text_set_string_arg(3,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_terms,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        return true;
      }
      if ((civs[civ2].last_contact_turn[civ] < game.turn) &&
         ((int)game.turn - (int)civs[civ2].last_contact_turn[civ] < 3))
      return true;
      if (border_intruders.repeat_offenses == 0) {
        diplo_eval_ai_stance(civ2,civ);
        diplo_set_attitude_arg(civ2,civ);
        if (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) == 0) &&
           ((int)cities[city].owner != (civ2 & 0xff))) {
          city_reveal_to_civ(city,civ2);
          tile_set_visible(x,y,1 << ((byte)civ2 & 0x1f),1,1);
          map_redraw_tile_all_views(x,y);
        }
        attitude_delta = border_intruders.intruder_count - 1;
        text_set_string_arg(1,(const char *)&cities[city].name);
        if (diplo_border_intruded_cities < 2) {
          popup_game_with_sprite((const char *)&str_intruder,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        else {
          popup_game_with_sprite((const char *)&str_intruders,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        return true;
      }
      diplo_review_violators(civ,civ2,city,x,y);
    }
  }
  return false;
}

/* Part of diplo_ai_review_relations (00560d95): the change of attitude: relations with the other civs, ranks, strengths and wonders. */
static int diplo_review_attitude_delta(iptr civ,uptr civ2,int attitude_delta)
{
  int y;
  iptr hostile;
  int value;
  iptr civ3;
  int rank_diff;
  int ahead_count;
  int rank_adjust;
  int hostile_count;
  int war_score;
  
  hostile_count = 0;
  ahead_count = 0;
  war_score = 0;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if (civs[civ2].num_advances < civs[civ3].num_advances) {
      ahead_count++;
    }
    hostile = diplo_is_hostile(civ,civ3);
    if (hostile != 0) {
      hostile_count++;
    }
    y = war_score;
    if ((((byte)(civs[civ3].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) &&
       (y = war_score + 1, civs[civ2].military_strength < civs[civ3].military_strength)) {
      y = war_score + 2;
    }
    war_score = y;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    if (hostile_count == 0) {
      if (civs[civ].gold < civs[civ2].gold) {
        attitude_delta++;
      }
      if ((civs[civ].num_advances < civs[civ2].num_advances) && (game.difficulty != DIFF_CHIEFTAIN)) {
        attitude_delta++;
      }
      if (civs[civ2].num_advances < civs[civ].num_advances) {
        attitude_delta--;
      }
      if (civs[civ].military_strength < civs[civ2].military_strength) {
        attitude_delta++;
      }
      if (civs[civ2].military_strength < civs[civ].military_strength) {
        attitude_delta--;
      }
    }
    else if ((leaders[civs[civ].tribe].attack < 1) || (1 < hostile_count)) {
      y = (hostile_count - leaders[civs[civ].tribe].attack) - 1;
      if (y < 2) {
        y = 1;
      }
      attitude_delta -= y;
    }
    if (civs[civ].num_advances + 8 < (uint)civs[civ2].num_advances) {
      attitude_delta++;
    }
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) {
    attitude_delta -= 2;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) {
    attitude_delta--;
  }
  if ((game.power_rank[civ2] < 7) && (200 < game.turn)) {
    rank_adjust = 7 - (uint)game.power_rank[civ2];
    y = spaceship_race_active();
    if (y != 0) {
      rank_adjust = 1;
    }
    if (game.turn < 400) {
      rank_adjust = rank_adjust + 1 >> 1;
    }
    attitude_delta -= rank_adjust;
  }
  if ((((game.power_rank[civ2] == 7) && (3 < civs[civ2].num_cities)) && (200 < game.turn))
     && (game.difficulty != DIFF_CHIEFTAIN)) {
    rank_adjust = game.difficulty / 3 + 1;
    if (((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2]
              ) <= war_score) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0))
    {
      rank_adjust = rank_adjust >> 1;
    }
    attitude_delta += rank_adjust;
  }
  y = attitude_delta;
  if (((civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] != '\0') &&
      (y = attitude_delta - 1, civs[civ].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) && (0 < y)) {
    y = attitude_delta - 2;
  }
  attitude_delta = y;
  y = leaders[civs[civ].tribe].attack * 3 + leaders[civs[civ].tribe].expand * 2;
  if (y < -1) {
    y = -2;
  }
  rank_diff = (uint)game.power_rank[civ2] - (uint)game.power_rank[civ];
  if (rank_diff < 0) {
    rank_diff = rank_diff / 2;
  }
  else if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
    rank_diff = rank_diff / 2;
  }
  attitude_delta = attitude_delta + y + rank_diff;
  if (((game.game_flags & GAME_AI_GANG_UP) == 0) ||
     (((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
       <= war_score && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0)))) {
    if ((uint)civs[civ].military_strength * 4 < (uint)civs[civ2].military_strength) {
      attitude_delta--;
    }
    if ((uint)civs[civ].military_strength * 2 < (uint)civs[civ2].military_strength) {
      attitude_delta--;
    }
    if (((uint)civs[civ].military_strength * 3) / 2 < (uint)civs[civ2].military_strength) {
      attitude_delta--;
    }
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
    if (civs[civ2].military_strength < civs[civ].military_strength) {
      attitude_delta++;
    }
    if ((uint)civs[civ2].military_strength * 2 < (uint)civs[civ].military_strength) {
      attitude_delta++;
    }
  }
  y = civ_has_wonder(civ2,WONDER_GREAT_WALL);
  if ((y != 0) || (y = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), y != 0)) {
    if (attitude_delta < 1) {
      attitude_delta--;
    }
    else {
      attitude_delta = attitude_delta >> 1;
    }
  }
  if (ahead_count == 0) {
    attitude_delta++;
  }
  if (civs[civ].num_advances < civs[civ2].num_advances) {
    attitude_delta += (1 - leaders[civs[civ].tribe].civilize);
  }
  if ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0) && (attitude_delta < 1)) {
    attitude_delta = 0;
  }
  y = civ_has_wonder(civ,WONDER_EIFFEL_TOWER);
  if (y != 0) {
    attitude_delta++;
  }
  value = civ_has_wonder(civ2,WONDER_EIFFEL_TOWER);
  y = attitude_delta;
  if (value != 0) {
    if (0 < attitude_delta) {
      attitude_delta = attitude_delta / 2;
    }
    y = attitude_delta - 1;
    if (-1 < attitude_delta - 1) {
      y = attitude_delta - 2;
    }
  }
  attitude_delta = y;
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) == 0) {
    attitude_delta = 0;
  }
  return attitude_delta;
}

/* 00560d95 auto */
void __cdecl diplo_ai_review_relations(iptr civ,uptr civ2)

{
  int attitude_delta;
  iptr unit;
  if (bugfix_state()) {
    attitude_delta = 0;  /* B3: the change starts from stale stack (docs/BUGS.md) */
  }
  else {
    attitude_delta = 0x6715d7a0;  /* stale stack the original reads (FRAME_PRESETS) */
  }
  
  if (!diplo_review_border_incident(civ,civ2,attitude_delta)) {
    for (unit = 0; unit < game.num_units; unit++) {
      if (units[unit].id != 0) {
        units[unit].flags = units[unit].flags & ~UNITF_BORDER_INTRUDER;
      }
    }
  }
  attitude_delta = diplo_review_attitude_delta(civ,civ2,attitude_delta);
  civ_adjust_attitude(civ,civ2,attitude_delta);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}

/* 00562021 auto */
void __cdecl diplo_ai_seek_war_ally(uptr civ2,uptr civ)

{
  uint double_strength;
  uint *flags_ptr;
  int value;
  int check;
  iptr civ_name;
  iptr advance2;
  uptr civ3;
  iptr advance;
  i8 msg_key[80];
  int best_value;
  int adv_value;
  
  if ((((int)game.turn & 0x1fU) == civ2 << 2) &&
     ((int)((uint)civs[civ].reputation - (int)civs[civ2].reputation_credit[civ]) <
      6)) {
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (((civ3 != civ2) && (civ3 != civ)) &&
         ((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0)) {
        if (((((byte)(civs[civ2].treaties[civ3] >> 8) & (TREATY_FIGHTING >> 8)) != 0) &&
            ((civs[civ2].treaties[civ] & (TREATY_ALLIANCE | TREATY_WAR)) == 0)) &&
           ((value = diplo_is_hostile(civ,civ3), value == 0 &&
            (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0 &&
             ((*(byte *)&civs[civ].treaties[civ3] & TREATY_CONTACT) != 0)))))) {
          double_strength = (uint)civs[civ2].military_strength * 2;
          if (double_strength - civs[civ3].military_strength == 0 ||
              double_strength < civs[civ3].military_strength) {
            value = diplo_ask_receive_emissary(civ,civ2,0xffffffff,0xffffffff);
            if (value == 0) {
              value = (int)civs[civ].last_contact_turn[civ2];
              if (value <= game.turn - 0xf) {
                value = game.turn - 0xf;
              }
              civs[civ].last_contact_turn[civ2] = (short)value;
            }
            else {
              value = int_clamp(civs[civ2].gold / 100,1,10);
              advance = -1;
              best_value = -99;
              for (advance2 = 0; advance2 < 100; advance2++) {
                if (((((advances[advance2].preq[0] != -2) || (advances[advance2].preq[1] != -2)) &&
                     (check = civ_has_advance(civ,advance2), check == 0)) &&
                    ((check = civ_has_advance(civ2,advance2), check != 0 ||
                     (((check = civ_has_advance(civ3,advance2), check != 0 &&
                       ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) == 0)) &&
                      ((check = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), check == 0 &&
                       (check = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), check == 0)))))))) &&
                   (adv_value = ai_advance_value(civ,advance2), best_value < adv_value)) {
                  advance = advance2;
                  best_value = adv_value;
                }
              }
              civ_name = (iptr)(civ_name_plural(civ3));
              text_set_string_arg(1,(const char *)civ_name);
              text_set_number_arg(0,value * 0x32);
              crt_strcpy(msg_key,str_helpme);
              if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
                str_append((char *)msg_key,(const char *)&str_instead);
              }
              if (-1 < advance) {
                str_append((char *)msg_key,(const char *)&str_2_4);
                text_set_label_arg(2,advances[advance].name);
              }
              check = popup_game_with_sprite((const char *)msg_key,(iptr)&signal_allied_icon_sprite,0);
              if (check == 0) {
                value = (int)civs[civ].last_contact_turn[civ2];
                if (value <= game.turn - 0xe) {
                  value = game.turn - 0xe;
                }
                civs[civ].last_contact_turn[civ2] = (short)value;
              }
              else {
                if (-1 < advance) {
                  check = civ_has_advance(civ2,advance);
                  if (check == 0) {
                    civ_give_advance(civ2,advance,civ3,0,0);
                  }
                  civ_give_advance(civ,advance,civ2,0,0);
                }
                diplo_set_treaty_flags(civ2,civ,0xc);
                diplo_declare_war(civ,civ3,civ2);
                diplo_set_treaty_flags(civ,civ3,0x2000);
                flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ3 * 4);
                *flags_ptr = *flags_ptr | 0x80800;
                civs[civ].last_contact_turn[civ3] = game.turn;
                civs[civ].last_contact_turn[civ2] = game.turn + 0x10;
                civs[civ2].gold += value * -0x32;
                civs[civ].gold += value * 0x32;
                status_redraw(1);
              }
            }
            diplo_session_reset();
            return;
          }
          if ((civs[civ2].military_strength < civs[civ3].military_strength) &&
             ((game.power_rank[civ] < 6 || (game.power_rank[civ2] < 6)))) {
            value = diplo_ask_receive_emissary(civ,civ2,0xffffffff,0xffffffff);
            if (value == 0) {
              value = (int)civs[civ].last_contact_turn[civ2];
              if (value <= game.turn - 0xf) {
                value = game.turn - 0xf;
              }
              civs[civ].last_contact_turn[civ2] = (short)value;
            }
            else {
              civ_name = (iptr)(civ_name_plural(civ3));
              text_set_string_arg(1,(const char *)civ_name);
              text_set_label_arg(2,label_table.string_ids[(((civs[civ2].treaties[civ] & TREATY_ALLIANCE) == 0 ) + 0x73)]);
              crt_strcpy(msg_key,str_crusade);
              if ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
                crt_strcpy(msg_key,str_jihad);
              }
              value = popup_game_with_sprite((const char *)msg_key,(iptr)&signal_allied_icon_sprite,0);
              if (value == 0) {
                value = (int)civs[civ].last_contact_turn[civ2];
                if (value <= game.turn - 0xe) {
                  value = game.turn - 0xe;
                }
                civs[civ].last_contact_turn[civ2] = (short)value;
              }
              else {
                diplo_set_treaty_flags(civ2,civ,0xc);
                diplo_declare_war(civ,civ3,civ2);
                civs[civ].last_contact_turn[civ2] = game.turn + 0x10;
                diplo_set_treaty_flags(civ,civ3,0x2000);
                flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ3 * 4);
                *flags_ptr = *flags_ptr | 0x80800;
                civs[civ].last_contact_turn[civ3] = game.turn;
              }
            }
            diplo_session_reset();
            return;
          }
        }
        flags_ptr = (uint *)(civ3 * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
        *flags_ptr = *flags_ptr & 0xfffffdff;
        if (2 < game.multiplayer) {
          net_send_state_diff(0xff,2,0,0,0);
          XD_FlushSendBuffer(5000);
        }
      }
    }
  }
  return;
}

/* 00579c40 auto */
i32 __cdecl diplo_force_war(iptr civ,iptr civ2)

{
  uint *flags_ptr;
  i32 result;
  
  result = 0;
  diplo_clear_treaty_flags(civ,civ2,0x60);
  if (((*(byte *)&civs[civ2].treaties[civ] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0) &&
     (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) {
    diplo_declare_war(civ,civ2,-1);
  }
  diplo_clear_treaty_flags(civ,civ2,0xc);
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0) {
    diplo_set_treaty_flags(civ,civ2,0x2000);
    civ_set_attitude(civ,civ2,100);
    civ_set_attitude(civ2,civ,100);
    if ((((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) == 0) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) == 0)) {
      flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
      *flags_ptr = *flags_ptr | 0x1000;
      result = 1;
    }
  }
  return result;
}

/* 00579ed0 auto */
i32 __cdecl diplo_confirm_attack(iptr civ,iptr civ2,uint treaty_mask)

{
  i32 result;
  iptr civ_name;
  int check;
  int choice;
  
  if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) {
    if ((treaty_mask & civs[civ].treaties[civ2]) == 0) {
      result = 0;
    }
    else if ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) == 0) {
      result = 0;
    }
    else {
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)civ_name);
      if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CEASEFIRE) == 0) {
            choice = popup_game_with_sprite((const char *)&str_annoy,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
          }
          else if (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_TRIBUTE >> 16)) == 0) {
            choice = popup_game_with_sprite((const char *)&str_annoycease,(iptr)&ceasefire_icon_sprite,0);
          }
          else {
            choice = popup_game_with_sprite((const char *)&str_annoyvassal,(iptr)&ceasefire_icon_sprite,0);
          }
        }
        else {
          choice = popup_game_with_sprite((const char *)&str_annoypeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        if (choice == 1) {
          check = diplo_senate_intervenes(civ,civ2);
          if (check == 0) {
            if (GOV_FUNDAMENTALISM < civs[civ].government) {
              check = civ_has_wonder(civ2,WONDER_UNITED_NATIONS);
              if (check == 0) {
                if ((((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_SNEAK_ATTACK >> 8)) == 0) &&
                   (civs[civ2].reputation == '\0')) {
                  popup_game_with_sprite((const char *)&str_allowhawks,(iptr)(((i8 *)&government_sprites_block.government_sprites) +
                                                    (uint)civs[civ].government * 0x3c),0);
                }
                else {
                  civ_name = civ_get_adjective(civ2);
                  text_set_string_arg(0,(const char *)civ_name);
                  popup_game_with_sprite((const char *)&str_allowaggressor,(iptr)(((i8 *)&government_sprites_block.government_sprites) +
                                                    (uint)civs[civ].government * 0x3c),0);
                }
              }
              else {
                popup_game_with_sprite((const char *)&str_allowun,(iptr)(((i8 *)&government_sprites_block.government_sprites) +
                                                  (uint)civs[civ].government * 0x3c),0);
              }
            }
            result = 0;
          }
          else {
            popup_game_with_sprite((const char *)&str_overrule,(iptr)(((i8 *)&government_sprites_block.government_sprites) + (uint)civs[civ].government * 0x3c
                                              ),0);
            result = 1;
          }
        }
        else {
          result = 1;
        }
      }
      else {
        popup_game_with_sprite((const char *)&str_annoyallied,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
        result = 1;
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}
