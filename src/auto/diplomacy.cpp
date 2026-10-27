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
  if (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x19) {
    limit = 3;
  }
  if (0x3c < tax_rate_icon_sprites_block.diplo_cur_attitude) {
    limit--;
  }
  has_wonder = civ_has_wonder(civ2,0x14);
  if (has_wonder != 0) {
    limit++;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & 4) != 0) {
    limit++;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
    limit += 2;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) {
    limit = 2;
  }
  return limit;
}

/* 0045705e auto */
void __cdecl diplo_eval_ai_stance(iptr civ2,iptr civ)

{
  uint *treaty_flags;
  char patience;
  ushort max_strength;
  ushort min_strength;
  int value;
  uint strength;
  int alliance_count;
  iptr civ3;
  iptr city;
  iptr continent;
  int war_count;
  
  tax_rate_icon_sprites_block.diplo_space_race_threat = ai_spaceship_race_behind(civ);
  war_count = 0;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if (((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) != 0) {
      war_count++;
    }
    if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
        (value = civ_spaceship_launched(civ3), value != 0)) && (3 < game.difficulty)) {
      tax_rate_icon_sprites_block.diplo_space_race_threat = 1;
    }
  }
  tax_rate_icon_sprites_block.diplo_ai_outranks_target = (uint)(game.power_rank[civ2] < game.power_rank[civ]);
  tax_rate_icon_sprites_block.diplo_suppressed_aggression = 0;
  if ((diplo_opening_eval_active != 0) && (diplo_initial_eval_done == 0)) {
    tax_rate_icon_sprites_block.diplo_last_treaty_turn = (int)civs[civ2].last_treaty_turn[civ];
  }
  (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = 0;
  tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
  tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
  tax_rate_icon_sprites_block.diplo_ai_request_mode = 1;
  for (continent = 1; continent < 0x40; continent++) {
    if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent]) &&
       (civs[civ].continent_military[continent] <
        civs[civ2].continent_attack[continent])) {
      (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) +
                     ((uint)civs[civ2].continent_attack[continent] *
                     ((-(uint)(game.difficulty == '\0') & 2) + 2)) /
                     (civs[civ].continent_military[continent] + 1);
    }
    else {
      if ((*(short *)&civs[civ].continent_military[continent] != 0) &&
         (*(short *)&civs[civ2].continent_military[continent] != 0)) {
        tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
      }
      if (*(char *)&civs[civ2].continent_cities[continent] == '\0') {
        if (*(char *)&civs[civ].continent_cities[continent] == '\0') {
          value = (uint)civs[civ].continent_attack[continent] -
                  (uint)civs[civ2].continent_attack[continent];
          tax_rate_icon_sprites_block.diplo_tribute_demand += ((int)(value + (value >> 0x1f & 3U)) >> 2);
        }
        else {
          tax_rate_icon_sprites_block.diplo_tribute_demand = tax_rate_icon_sprites_block.diplo_tribute_demand +
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
        tax_rate_icon_sprites_block.diplo_tribute_demand += strength;
      }
    }
  }
  value = civ_has_wonder(civ,9);
  if (value != 0) {
    tax_rate_icon_sprites_block.diplo_tribute_demand += ((int)(tax_rate_icon_sprites_block.diplo_tribute_demand + (tax_rate_icon_sprites_block.diplo_tribute_demand >> 0x1f & 3U)) >> 2);
  }
  tax_rate_icon_sprites_block.diplo_target_is_leader = 0;
  if ((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) &&
      ((1 < civs[civ].num_cities &&
       ((civs[civ2].units_active[0x2d] == '\0' && (game.difficulty != '\0')))))) &&
     (200 < game.turn)) {
    tax_rate_icon_sprites_block.diplo_target_is_leader = 1;
  }
  (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = (uint)civs[civ].attitude[civ2];
  if (((tax_rate_icon_sprites_block.diplo_target_is_leader != 0) ||
      (((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0 &&
       ((uint)civs[civ2].military_strength < (uint)civs[civ].military_strength * 3)))) &&
     (civs[civ2].units_active[0x2d] == '\0')) {
    tax_rate_icon_sprites_block.diplo_tribute_demand = int_clamp(tax_rate_icon_sprites_block.diplo_tribute_demand,(uint)game.power_rank[civ] * 2,9999);
    if (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = int_clamp(tax_rate_icon_sprites_block.diplo_tribute_demand,0,
                                        (civs[civ2].gold / 0x32) /
                                        (int)(8 - (uint)game.power_rank[civ]));
    }
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
    (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = 0;
    if ((int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) < 0x4c) {
      (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 0x4b;
    }
  }
  if ((tax_rate_icon_sprites_block.diplo_space_race_threat != 0) && (tax_rate_icon_sprites_block.diplo_ai_outranks_target != 0)) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
  }
  if ((civs[civ].units_active[0x2d] != '\0') &&
     (treaty_flags = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594), *treaty_flags = *treaty_flags | 0x100,
     civs[civ2].units_active[0x2d] == '\0')) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
    tax_rate_icon_sprites_block.diplo_tribute_demand = int_clamp(tax_rate_icon_sprites_block.diplo_tribute_demand,100,9999);
  }
  if ((civs[civ2].units_active[0x2d] != '\0') &&
     ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0)) {
    if (civs[civ].units_active[0x2d] == '\0') {
      (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = ((int)(uint)civs[civ2].reputation >> 1) + 1;
      if ((*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) < 5) {
        (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = 4;
      }
    }
    else {
      tax_rate_icon_sprites_block.diplo_tribute_demand = (int)((uint)civs[civ].units_active[0x2d] * (tax_rate_icon_sprites_block.diplo_tribute_demand / 2)) /
                     (int)(uint)civs[civ2].units_active[0x2d];
    }
  }
  (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = (int)(*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) / ((civs[civ2].reputation - 1) / 2 + 1);
  if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
    (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) -= 0x19;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) {
    (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) += 0x19;
  }
  if (civs[civ2].reputation != '\0') {
    (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) +
                   ((civs[civ2].reputation - 1) -
                   (int)civs[civ].reputation_credit[civ2]) * 5;
  }
  if ((int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) < 0x1a) {
    tax_rate_icon_sprites_block.diplo_tribute_demand = tax_rate_icon_sprites_block.diplo_tribute_demand / 2;
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
  }
  else {
    value = diplo_attitude_level((*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude));
    if (value < 4) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = (tax_rate_icon_sprites_block.diplo_tribute_demand * 2) / 3;
    }
    else if (0x4a < (int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude)) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = (tax_rate_icon_sprites_block.diplo_tribute_demand * 3) / 2;
      tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
    }
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 8) != 0) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
  }
  if (((game.rules & 0x80) != 0) && (civs[civ].science_rate == '\0')) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
  }
  if ((*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) != 0) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
    tax_rate_icon_sprites_block.diplo_ai_request_mode = 0;
  }
  value = civ_has_wonder(civ2,6);
  if ((value != 0) || (value = civ_has_wonder(civ2,0x18), value != 0)) {
    tax_rate_icon_sprites_block.diplo_target_is_leader = 0;
    tax_rate_icon_sprites_block.diplo_suppressed_aggression = tax_rate_icon_sprites_block.diplo_ai_aggressive;
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
    (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) -= 10;
    diplo_clear_treaty_flags(civ,civ2,0x10000);
  }
  if (civs[civ2].reputation != '\0') {
    tax_rate_icon_sprites_block.diplo_tribute_demand += (int)((uint)civs[civ2].reputation * tax_rate_icon_sprites_block.diplo_tribute_demand) / 2;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) {
    if (((byte)(civs[civ].treaties[civ2] >> 16) & 1) != 0) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = (int)(tax_rate_icon_sprites_block.diplo_tribute_demand + (tax_rate_icon_sprites_block.diplo_tribute_demand >> 0x1f & 3U)) >> 2;
    }
  }
  else {
    tax_rate_icon_sprites_block.diplo_tribute_demand = tax_rate_icon_sprites_block.diplo_tribute_demand << 1;
  }
  tax_rate_icon_sprites_block.diplo_ai_units_near_cities = 0;
  city = 0;
  do {
    if (game.num_cities <= city) {
compute_tribute_demand:
      value = (game.difficulty + 1) * tax_rate_icon_sprites_block.diplo_tribute_demand;
      value = int_clamp((int)(value + (value >> 0x1f & 0x1fU)) >> 5,0,0x14);
      tax_rate_icon_sprites_block.diplo_tribute_demand = value * 0x32;
      if (((civs[civ2].gold < tax_rate_icon_sprites_block.diplo_tribute_demand) && (tax_rate_icon_sprites_block.diplo_tribute_demand < civs[civ2].gold * 2)) &&
         (0x31 < civs[civ2].gold)) {
        tax_rate_icon_sprites_block.diplo_tribute_demand = (civs[civ2].gold / 0x32) * 0x32;
      }
      if ((tax_rate_icon_sprites_block.diplo_tribute_demand == 0) ||
         ((uint)civs[civ].military_strength * 3 < (uint)civs[civ2].military_strength)) {
        tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
      }
      if (9 < civs[civ].aggression_counter[civ2]) {
        *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
        tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
      }
      if (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
          tax_rate_icon_sprites_block.diplo_ai_request_mode = 0;
        }
      }
      else {
        tax_rate_icon_sprites_block.diplo_ai_request_mode = 2;
      }
      tax_rate_icon_sprites_block.diplo_ai_initial_aggressive = tax_rate_icon_sprites_block.diplo_ai_aggressive;
      tax_rate_icon_sprites_block.diplo_peace_inclination = 0;
      if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
        tax_rate_icon_sprites_block.diplo_peace_inclination = -2;
      }
      if (((game.rules & 0x80) != 0) && (civs[civ].science_rate == '\0')) {
        tax_rate_icon_sprites_block.diplo_peace_inclination--;
      }
      if (civs[civ2].reputation != '\0') {
        tax_rate_icon_sprites_block.diplo_peace_inclination -= (civs[civ2].reputation - 1) / 2;
      }
      tax_rate_icon_sprites_block.diplo_weakest_enemy_civ = -1;
      tax_rate_icon_sprites_block.diplo_strongest_enemy = -1;
      tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
      max_strength = 0;
      min_strength = 9999;
      alliance_count = 0;
      for (civ3 = 1; civ3 < 8; civ3++) {
        if ((civ2 != civ3) && (civ != civ3)) {
          if ((*(byte *)&civs[civ].treaties[civ3] & 8) != 0) {
            if (game.power_rank[civ] < game.power_rank[civ2]) {
              tax_rate_icon_sprites_block.diplo_peace_inclination--;
            }
            if ((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) &&
                (200 < game.turn)) &&
               ((game.difficulty != '\0' &&
                ((*(byte *)&civs[civ3].treaties[civ2] & 1) != 0)))) {
              if ((*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) == 0) {
                if (((((byte)(civs[civ3].treaties[civ2] >> 8) & 0x20) != 0) &&
                    (value = civ_has_wonder(civ2,6), value == 0)) &&
                   (value = civ_has_wonder(civ2,0x18), value == 0)) {
                  tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
                }
              }
              else {
                (*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) = int_clamp((*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) - 4,0,99);
              }
            }
          }
          if (((byte)(civs[civ].treaties[civ3] >> 8) & 8) != 0) {
            alliance_count++;
          }
          value = diplo_is_hostile(civ,civ3);
          if (value != 0) {
            strength = (uint)civs[civ3].military_strength * 4;
            if (strength - civs[civ].military_strength != 0 &&
                civs[civ].military_strength <= strength) {
              tax_rate_icon_sprites_block.diplo_peace_inclination++;
            }
            if (civs[civ].military_strength < civs[civ3].military_strength) {
              tax_rate_icon_sprites_block.diplo_peace_inclination++;
            }
            if (((*(byte *)&civs[civ2].treaties[civ3] & 1) != 0) &&
               (((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) == 0)) {
              tax_rate_icon_sprites_block.diplo_war_target_civ = civ3;
            }
            if ((((byte)(civs[civ].treaties[civ3] >> 8) & 0x20) != 0) &&
               (((byte)(civs[civ].treaties[civ3] >> 8) & 2) != 0)) {
              if (max_strength < civs[civ3].military_strength) {
                max_strength = civs[civ3].military_strength;
                tax_rate_icon_sprites_block.diplo_strongest_enemy = civ3;
              }
              if (civs[civ3].military_strength < min_strength) {
                min_strength = civs[civ3].military_strength;
                tax_rate_icon_sprites_block.diplo_weakest_enemy_civ = civ3;
              }
            }
          }
        }
      }
      if (alliance_count == 0) {
        diplo_clear_treaty_flags(civ,civ2,0x100000);
      }
      tax_rate_icon_sprites_block.diplo_peace_inclination -= leaders[civs[civ].tribe].attack;
      if (civs[civ2].military_strength < civs[civ].military_strength) {
        tax_rate_icon_sprites_block.diplo_peace_inclination--;
      }
      value = diplo_is_hostile(civ,civ2);
      if (((((int)(uint)(value != 0) < tax_rate_icon_sprites_block.diplo_peace_inclination) && (tax_rate_icon_sprites_block.diplo_target_is_leader == 0)) &&
          ((civs[civ].treaties[civ2] & 0x808) == 0)) &&
         (tax_rate_icon_sprites_block.diplo_ai_aggressive = 0, (game.turn + civ + civ2 & 0x20U) != 0)) {
        tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
      }
      tax_rate_icon_sprites_block.diplo_captured_city_count = 0;
      for (city = 0; city < game.num_cities; city++) {
        if (((cities[city].id != 0) && (cities[city].founder == civ)) &&
           (cities[city].owner == civ2)) {
          tax_rate_icon_sprites_block.diplo_captured_city_count++;
        }
      }
      if ((((((diplo_initial_eval_done == 0) ||
             (6 < (int)((uint)civs[civ2].reputation -
                       (int)civs[civ].reputation_credit[civ2]))) &&
            ((tax_rate_icon_sprites_block.diplo_ai_declare_war = 0, ((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0 ||
             (4 < tax_rate_icon_sprites_block.diplo_captured_city_count)))) &&
           ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0)) &&
          (((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0 || ((tax_rate_icon_sprites_block.diplo_space_race_threat != 0 && (2 < game.difficulty)))) ||
           ((tax_rate_icon_sprites_block.diplo_suppressed_aggression != 0 &&
            ((6 < (int)((uint)civs[civ2].reputation -
                       (int)civs[civ].reputation_credit[civ2]) &&
             ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0)))))))) &&
         ((civs[civ].military_strength < civs[civ2].military_strength ||
          (((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0 ||
           (6 < (int)((uint)civs[civ2].reputation -
                     (int)civs[civ].reputation_credit[civ2]))))))) {
        tax_rate_icon_sprites_block.diplo_ai_declare_war = 1;
      }
      if (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0) {
        tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
        tax_rate_icon_sprites_block.diplo_ai_request_mode = 0;
        if (0x1d < (int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude)) {
          (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 0x1e;
        }
      }
      if (((game.game_flags & 0x80) != 0) && (((int)(short)prefs_game_rules_block.scenario_flags[0] & 0x8000U) != 0)) {
        if (civ == 6) {
          if (civ2 == 7) {
            if (((byte)civs[6].treaties[7] & 8) != 0) {
              tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
            }
          }
          else if (civ2 == 1) {
            tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
          }
          else if (civ2 == 3) {
            tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
          }
          else {
            tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
          }
        }
        if ((civ == 3) && ((civ2 == 6 || (civ2 == 1)))) {
          tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
        }
      }
      if (((((diplo_opening_eval_active != 0) &&
            (patience = civs[civ].diplo_request_count, value = diplo_patience_limit(civ2,civ),
            patience < value)) && (4 < civs[civ].government)) &&
          ((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0 &&
           ((civs[civ].treaties[civ2] & 0x400008) == 0)))) &&
         (((game.game_flags & 0x80) == 0 || ((prefs_game_rules_block.scenario_flags[0] & 1) == 0)))) {
        if ((((uint)civs[civ].military_strength * 2 < (uint)civs[civ2].military_strength * 3)
            && (tax_rate_icon_sprites_block.diplo_target_is_leader == 0)) &&
           ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0)) {
          tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
        }
        else {
          ai_choose_government(civ,1);
        }
      }
      if (((*(uint *)&tax_rate_icon_sprites_block.diplo_ai_fear_level) == 0) && ((int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) < 2)) {
        (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 1;
      }
      if ((((byte)(civs[civ2].treaties[civ] >> 8) & 0x20) == 0) &&
         (0x62 < (int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude))) {
        (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 99;
      }
      if ((((*(byte *)&civs[civ2].treaties[civ] & 8) != 0) || (tax_rate_icon_sprites_block.diplo_ai_request_mode == 0))
         && (0x49 < (int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude))) {
        (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 0x4a;
      }
      if ((((byte)(civs[civ2].treaties[civ] >> 8) & 0x20) != 0) &&
         ((int)(*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) < 0x1b)) {
        (*(uint *)&tax_rate_icon_sprites_block.diplo_cur_attitude) = 0x1a;
      }
      return;
    }
    if ((((cities[city].id != 0) && (cities[city].owner == civ2)) &&
        (value = tile_unit_owner((int)cities[city].x,(int)cities[city].y), value < 0)) &&
       ((value = unit_find_nearest((int)cities[city].x,(int)cities[city].y,civ,-1),
        -1 < value && (dialog_text_font_obj_block.unit_nearest_dist < 3)))) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = tax_rate_icon_sprites_block.diplo_tribute_demand << 1;
      tax_rate_icon_sprites_block.diplo_ai_units_near_cities++;
      goto compute_tribute_demand;
    }
    city++;
  } while( true );
}

/* 00458a3b auto */
void __cdecl diplo_set_attitude_arg(iptr civ2,iptr civ)

{
  iptr level;
  iptr adjective;
  
  text_clear();
  level = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude);
  text_append_string_id(attitude_names[level]);
  if (game_language == 2) {
    text_append((const char *)&str_er);
  }
  text_append_space();
  adjective = civ_get_adjective(civ);
  text_append((const char *)adjective);
  text_set_string_arg(0,(const char *)&map_views_block.text_buffer);
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
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
  diplo_session_active = 1;
  if (((*((char *)&game.game_options + 2)) & 4) != 0) {
    gfx_get_screen_rect((iptr)((i8 *)&screen_rect2.left));
    offset_x = int_clamp((screen_rect2.right - 0x280) / 3,1,9999);
    offset_y = int_clamp((screen_rect2.bottom - 0x1e0) / 3,1,9999);
    dialog_set_default_offset(-offset_x,-offset_y);
    dialog_set_position(&dlg,-offset_x,-offset_y);
    diplo_leader_wnd_launch(civ2,civ);
  }
  diplo_set_attitude_arg(civ2,civ);
  str = (iptr)(civ_leader_name(civ));
  text_set_string_arg(1,(const char *)str);
  str = civ_leader_title(civ);
  text_set_string_arg(2,(const char *)str);
  str = (iptr)(civ_name_plural(civ));
  text_set_string_arg(3,(const char *)str);
  text_set_label_arg(5,label_string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
  str = (iptr)(civ_name_plural(civ2));
  text_set_string_arg(6,(const char *)str);
  text_clear();
  text_append((const char *)&str_greetings);
  if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) {
    text_append((const char *)&str_0_3);
  }
  else {
    text_append((const char *)&str_1);
  }
  str = rand_range(0,3);
  text_append_int(str);
  dialog_load_game_text(&dlg,(const char *)&map_views_block.text_buffer);
  if (civs[civ].units_active[0x2d] != '\0') {
    dlg_load_game_section(&dlg,(const char *)&str_nuclearweapons,0x8000);
  }
  dlg_show_modal(&dlg,0);
  if ((civs[civ2].units_active[0x2d] != '\0') &&
     ((civs[civ2].treaties[civ] & 0x104) != 0x100)) {
    text_clear();
    text_append((const char *)&str_yournukes);
    text_append_int(2 - (uint)(civs[civ].units_active[0x2d] == '\0'));
    popup_game((const char *)&map_views_block.text_buffer);
    treaty = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *treaty = *treaty | 0x100;
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00458dd5 */ { dialog_dtor(&dlg); }
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
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  refused = 0;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
  if (civ_saved_cursor_x_block.autoplay_active == 0) {
    if (diplo_emissary_asked == 0) {
      diplo_emissary_asked = 1;
      while (diplo_emissary_response == 0) {
        if ((civs[civ].units_active[0x2d] != '\0') &&
           (((byte)(civs[civ].treaties[civ2] >> 8) & 1) == 0)) {
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
        text_set_label_arg(4,label_string_ids[0x76 - (uint)(leaders[civs[civ].tribe].female == '\0')]);
        dialog_load_game_text(&dlg,(const char *)&str_emissary);
        if ((((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0) ||
            (ret = civ_has_wonder(human_civ[0],0x18), ret != 0)) ||
           (ret = civ_has_wonder(human_civ[0],9), ret != 0)) {
          option_flags = 0;
          option_id = 2;
          str = (iptr)(label_string(label_string_ids[0x68]));
          dialog_add_option(&dlg,(const char *)str,option_id,option_flags);
        }
        ret = dlg_show_modal(&dlg,0);
        diplo_emissary_response = -ret;
        if (diplo_emissary_response != -2) break;
        intel_report_show(civ2,civ);
        diplo_emissary_response = 0;
      }
      if (diplo_emissary_response != -1) {
        diplo_greet_emissary(civ2,civ);
      }
      accepted = (uint)(diplo_emissary_response != -1);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 00459169 */ { dialog_dtor(&dlg); }
      result = accepted;
    }
    else {
      result = (uint)(diplo_emissary_response != -1);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 00459169 */ { dialog_dtor(&dlg); }
    }
  }
  else {
    diplo_emissary_response = -1;
    seh_state = 0xffffffff;
    /* inlined unwind funclet 00459169 */ { dialog_dtor(&dlg); }
    result = 0;
  }
  return result;
}

/* 0045918e auto */
void __cdecl diplo_session_reset(void)

{
  diplo_session_active = 0;
  diplo_emissary_asked = 0;
  diplo_emissary_response = 0;
  diplo_initial_eval_done = 0;
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
  
  if (tax_rate_icon_sprites_block.diplo_wanted_tech < 0) {
    result = 0;
  }
  else {
    price = ai_advance_value(civ2,tax_rate_icon_sprites_block.diplo_wanted_tech);
    price = price * 0x14;
    if (0x32 < tax_rate_icon_sprites_block.diplo_cur_attitude) {
      price = (tax_rate_icon_sprites_block.diplo_cur_attitude * price) / 0x32;
    }
    if ((((game.power_rank[civ] == 7) && (4 < civs[civ].num_cities)) && (200 < game.turn)
        ) && (game.difficulty != '\0')) {
      price = price << 1;
    }
    known_count = bits_count8((int)(char)game.advance_known_by[tax_rate_icon_sprites_block.diplo_wanted_tech]);
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
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
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
      text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_wanted_tech].name);
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
        civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_wanted_tech,civ2,0,0);
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
  iptr advance;
  iptr city;
  iptr advance2;
  Dialog dlg;
  int civ_has;
  int civ_tech_count;
  iptr blocking_wonder;
  iptr wonder;
  i8 label_buf[80];
  int wanted_value;
  int attitude_cost;
  iptr wonder_group;
  int value;
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
  if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
    allied = false;
  }
  else {
    allied = true;
  }
  do {
    offer2_value = 0;
    offer_value = 0;
    wanted_value = 0;
    civ_tech_count = 0;
    ai_tech_count = 0;
    blocking_wonder = -1;
    advance2 = -1;
    tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
    tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
    tax_rate_icon_sprites_block.diplo_wanted_tech = -1;
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
            tax_rate_icon_sprites_block.diplo_wanted_tech = advance;
            wanted_value = value;
          }
        }
        if ((ret != 0) && (civ_has == 0)) {
          value = ai_advance_value(civ,advance);
          if (value < offer_value) {
            if (offer2_value <= value) {
              tax_rate_icon_sprites_block.diplo_offer_tech_2nd = advance;
              offer2_value = value;
            }
          }
          else {
            if (-1 < tax_rate_icon_sprites_block.diplo_offer_tech_best) {
              tax_rate_icon_sprites_block.diplo_offer_tech_2nd = tax_rate_icon_sprites_block.diplo_offer_tech_best;
              offer2_value = offer_value;
            }
            tax_rate_icon_sprites_block.diplo_offer_tech_best = advance;
            offer_value = value;
          }
        }
      }
    }
    if (eval_only != 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    diplo_set_attitude_arg(civ2,civ);
    str = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)str);
    if (!allied) {
      if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) ||
          (((byte)(civs[civ2].treaties[civ] >> 8) & 0x10) != 0)) ||
         (tax_rate_icon_sprites_block.diplo_captured_city_count != 0)) {
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
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
        goto done;
      }
      if (((((byte)(civs[civ].treaties[civ2] >> 8) & 2) != 0) ||
          ((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0 && (5 < civ_tech_count)))) ||
         (ret = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), 5 < ret)) {
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
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
        goto done;
      }
    }
    if ((-1 < blocking_wonder) && (3 < game.difficulty)) {
      if (mode == 0) {
        text_set_label_arg(1,improvements[blocking_wonder + 0x27].name);
        popup_game((const char *)&str_noexchangewonder);
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
      }
      else {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
      }
      goto done;
    }
    if (ai_tech_count < civ_tech_count) {
      if (mode == 1) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
        goto done;
      }
      treaty = civs[civ].treaties[civ2];
      ret = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude);
      if (((int)(5 - (uint)((treaty & 4) == 0)) < ret) && (!allied)) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret == 0) {
          if (mode == 0) {
            str = civ_get_adjective(civ2);
            text_set_string_arg(1,(const char *)str);
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_noexchangemedium_3);
            seh_state = 0xffffffff;
            /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
            mode = 0;
          }
          else {
            seh_state = 0xffffffff;
            /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
            mode = 0;
          }
        }
        else {
          seh_state = 0xffffffff;
          /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        }
        goto done;
      }
    }
    if ((((((game.power_rank[civ2] == 7) && (4 < civs[civ2].num_cities)) && (!allied)) &&
         ((treaty = civs[civ].treaties[civ2], early_game = game.turn < 0xc9,
          ret = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude),
          (int)((early_game + 3) - (uint)((treaty & 4) == 0)) < ret &&
          (unit_types[0x16].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best)))) &&
        (unit_types[0x15].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best)) &&
       (((unit_types[0x19].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best &&
         (unit_types[0x1a].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best)) &&
        ((unit_types[0xb].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best &&
         ((unit_types[7].prerequisite != tax_rate_icon_sprites_block.diplo_offer_tech_best && (tax_rate_icon_sprites_block.diplo_offer_tech_best != 0x22)))))))) {
      if (mode != 1) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret != 0) {
          seh_state = 0xffffffff;
          /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
          goto done;
        }
        if (mode == 0) {
          popup_game((const char *)&str_noexchangenow);
        }
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    if ((((tax_rate_icon_sprites_block.diplo_offer_tech_best < 0) && (-1 < tax_rate_icon_sprites_block.diplo_wanted_tech)) &&
        ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0)) &&
       ((ai_advances = civs[civ2].num_advances, civilize_bonus = leaders[civs[civ].tribe].civilize,
        ret = int_clamp(civs[civ].num_advances / 0xc,0,5),
        (((ai_advances + 6) - (int)civilize_bonus) - ret) < (int)(uint)civs[civ].num_advances &&
        ((civs[civ].diplo_request_count < 2 || (mode == 3)))))) {
      text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_wanted_tech].name);
      diplo_leader_play_reaction(3);
      if (mode < 2) {
        popup_game((const char *)&str_exchangegift);
      }
      else {
        popup_game((const char *)&str_exchangegift2);
      }
      civ_give_advance(civ2,tax_rate_icon_sprites_block.diplo_wanted_tech,civ,0,0);
      tax_rate_icon_sprites_block.diplo_wanted_tech = -1;
      if (mode < 2) {
        civs[civ].diplo_request_count += 2;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
      }
      else {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 1;
      }
      goto done;
    }
    if ((tax_rate_icon_sprites_block.diplo_wanted_tech < 0) || (tax_rate_icon_sprites_block.diplo_offer_tech_best < 0)) {
      if (mode != 1) {
        ret = diplo_offer_sell_tech(civ2,civ,mode);
        if (ret != 0) {
          seh_state = 0xffffffff;
          /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
          goto done;
        }
        if (mode == 0) {
          popup_game((const char *)&str_noexchangenow_2);
        }
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    if (1 < mode) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    ret = diplo_ask_receive_emissary(civ2,civ,x,y);
    if (ret == 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_wanted_tech].name);
    str = civ_get_adjective(civ2);
    text_set_string_arg(2,(const char *)str);
    text_set_label_arg(3,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
    if (tax_rate_icon_sprites_block.diplo_offer_tech_2nd < 0) {
      if (-1 < advance2) {
        text_set_label_arg(4,advances[advance2].name);
      }
    }
    else {
      text_set_label_arg(4,advances[tax_rate_icon_sprites_block.diplo_offer_tech_2nd].name);
    }
    text_clear();
    text_append((const char *)&str_exchange);
    if (((mode == 0) && (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0)) &&
       (ret = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), ret < 5)) {
      text_append((const char *)&str_1_2);
    }
    else {
      text_append((const char *)&str_0_4);
    }
    dialog_init_defaults(&dlg);
    dialog_load_game_text(&dlg,(const char *)&map_views_block.text_buffer);
    if ((-1 < tax_rate_icon_sprites_block.diplo_offer_tech_2nd) && (tax_rate_icon_sprites_block.diplo_offer_tech_2nd != tax_rate_icon_sprites_block.diplo_offer_tech_best)) {
      str_clear((char *)label_buf);
      str_append_label((iptr)label_buf,0x6b);
      text_expand_vars((iptr)label_buf,(iptr)&map_views_block.text_buffer);
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,2,0);
    }
    ret = dlg_show_modal(&dlg,0);
    if (ret == 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
      mode = 0;
      goto done;
    }
    attitude_cost = offer_value * 2;
    if (ret == 2) {
      ret = civ_has_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_2nd);
      if ((((ret != 0) || (ret = civ_has_wonder(civ,9), ret != 0)) ||
          ((game.power_rank[civ2] == 7 && (4 < civs[civ2].num_cities)))) ||
         ((offer2_value < offer_value &&
          ((ai_tech_count < civ_tech_count || (ret = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), 4 < ret)))))) {
        diplo_leader_play_reaction(3);
        popup_game((const char *)&str_exchangepetty);
        civs[civ].diplo_request_count++;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
        mode = 0;
        goto done;
      }
      int_swap((iptr)&tax_rate_icon_sprites_block.diplo_offer_tech_best,(iptr)&tax_rate_icon_sprites_block.diplo_offer_tech_2nd);
      attitude_cost = offer2_value;
    }
    civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
    civ_steal_advance(civ2,civ);
    tax_rate_icon_sprites_block.diplo_offer_tech_best = tax_rate_icon_sprites_block.diplo_offer_tech_2nd;
    tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
    civ_adjust_attitude(civ,civ2,-attitude_cost);
    civs[civ].flags = civs[civ].flags | 0x80;
    civs[civ2].flags = civs[civ2].flags | 0x80;
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
  } while (mode != 0);
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0045a510 */ { dialog_dtor(&dlg); }
  mode = 1;
done:
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
  civs[civ].flags = civs[civ].flags | 0x100;
  tax_rate_icon_sprites_block.diplo_ai_declare_war = 0;
  civs[civ2].last_treaty_turn[civ] = game.turn;
  civs[civ2].flags = civs[civ2].flags | 0x100;
  diplo_set_attitude_arg(civ2,civ);
  adjective = civ_get_adjective(civ);
  text_set_string_arg(1,(const char *)adjective);
  adjective = civ_get_adjective(civ2);
  text_set_string_arg(2,(const char *)adjective);
  diplo_leader_play_reaction(2);
  popup_game_with_sprite((const char *)&str_alliance,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
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
  civs[civ2].last_treaty_turn[civ] = game.turn;
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
  civs[civ2].last_treaty_turn[civ] = game.turn;
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
  
  if ((*(byte *)&civs[civ].treaties[civ2] & 1) != 0) {
    for (civ3 = 1; civ3 < 8; civ3++) {
      if ((((civ3 != civ) && (civ3 != civ2)) &&
          ((*(byte *)&civs[civ].treaties[civ3] & 8) != 0)) &&
         (((civs[civ3].treaties[civ2] & 0x2008) == 0 &&
          ((*(byte *)&civs[civ3].treaties[civ2] & 1) != 0)))) {
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
                                                  ((game.advance_known_by[0xf] != '\0') - 1 &
                                                  0xffff7c80)),0);
              }
              else if ((human_civ[0] != civ) && (2 < game.multiplayer)) {
                net_send_text_args(0x39,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,4,0,0,0);
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
                                                ((game.advance_known_by[0xf] != '\0') - 1 &
                                                0xffff7c80)),0);
            }
            else if ((human_civ[0] != civ2) && (2 < game.multiplayer)) {
              net_send_text_args(0x38,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,4,0,0,0);
            }
          }
          civ_adjust_attitude(civ3,civ2,100);
          diplo_set_treaty_flags(civ3,civ2,0x2401);
          treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
          *treaty = *treaty | 0x80800;
          treaty = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
          *treaty = *treaty | 0x80800;
          civs[civ2].last_treaty_turn[civ3] = game.turn;
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
  if ((*(byte *)&civs[civ].treaties[civ3] & 8) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ3] & 6) == 0) {
      if (-1 < civ2) {
        civ_adjust_attitude(civ2,civ,0xfffffffb);
      }
    }
    else {
      if (((*(byte *)&civs[civ].treaties[civ3] & 4) != 0) &&
         ((*(byte *)&civs[civ].treaties[civ3] & 2) != 0)) {
        if ((game.power_rank[civ] == 7) && ((4 < civs[civ].num_cities && (civ2 < 0))))
        {
          civs[civ].reputation++;
        }
        if ((game.difficulty != '\0') && (has_wonder = civ_has_wonder(civ,0x14), has_wonder == 0)) {
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
      if ((game.difficulty != '\0') && (has_wonder = civ_has_wonder(civ,0x14), has_wonder == 0)) {
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
      civs[civ].last_treaty_turn[civ3] = game.turn;
      diplo_allies_join_war(civ3,civ);
    }
  }
  else {
    if ((game.difficulty != '\0') && (has_wonder = civ_has_wonder(civ,0x14), has_wonder == 0)) {
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
      ((*(byte *)&civs[civ].treaties[human_civ[0]] & 8) != 0)) &&
     ((civs[human_civ[0]].treaties[civ2] & 0x2008) == 0)) {
    payment = diplo_ask_receive_emissary(human_civ[0],civ,0xffffffff,0xffffffff);
    if (payment == 0) {
      civ_adjust_attitude(civ,civ3,100);
    }
    else {
      name2 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)name2);
      choice = popup_game_with_sprite((const char *)&str_demandhelp,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[0xf] != '\0') - 1 &
                                                   0xffff7c80)),0);
      if (choice != 1) {
        threat = 0;
        for (continent = 1; continent < 0x3f; continent++) {
          if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent]) &&
             (civs[civ].continent_military[continent] <
              civs[civ2].continent_attack[continent])) {
            threat = threat +
                       ((uint)civs[civ2].continent_attack[continent] *
                       ((-(uint)(game.difficulty == '\0') & 2) + 2)) /
                       (civs[civ].continent_military[continent] + 1);
          }
        }
        if (threat != 0) {
          gold_units = int_clamp(threat,0,civs[civ].gold / 0x32);
          payment = gold_units * 0x32;
          if (payment != 0) {
            text_set_number_arg(0,payment);
            choice = popup_game_with_sprite((const char *)&str_helpbonus,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                         ((game.advance_known_by[0xf] != '\0') - 1 &
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
        popup_game_with_sprite((const char *)&str_didnthelp,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
        if (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x33) {
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

/* 0045b4da auto */
i32 __cdecl diplo_ai_answer_proposal(iptr civ2,iptr civ,int proposal)

{
  uint *treaty;
  char patience;
  bool agree;
  bool paid;
  byte peace_flags;
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
  iptr other;
  int gold_offer;
  int y2;
  iptr target;
  int ally_count;
  int x2;
  uint result;
  uint reasons;
  int best_rank;
  uint score;
  int tribute;
  uint at_war;
  
  result = 0;
  paid = false;
  reasons = 0;
  unit_blink_active = 0;
  switch(proposal) {
  case 1:
    ally_count = 0;
    for (other = 1; other < 8; other++) {
      if ((*(byte *)&civs[civ2].treaties[other] & 8) != 0) {
        ally_count++;
      }
    }
    score = (uint)(tax_rate_icon_sprites_block.diplo_war_target_civ == -1);
    if (game.difficulty < 2) {
      score--;
    }
    if ((game.power_rank[civ] < 5) && (game.power_rank[civ2] < 5)) {
      if ((1 < game.difficulty) && (ally_count == 0)) {
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
    if (tax_rate_icon_sprites_block.diplo_target_is_leader == 0) {
      if (2 < (int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ])) {
        score--;
      }
    }
    else {
      score++;
    }
    if (tax_rate_icon_sprites_block.diplo_ai_fear_level != 0) {
      score--;
    }
    agree = (int)score < tax_rate_icon_sprites_block.diplo_peace_inclination;
    city = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude);
    agree = game.difficulty == '\0' || city < 4 && agree;
    if ((game.power_rank[civ2] == 7) && (ally_count != 0)) {
      agree = false;
    }
    if ((int)((-(uint)(game.difficulty == '\0') & 2) + 2) <
        (int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
       ) {
      agree = false;
    }
    if ((game.power_rank[civ2] < 4) &&
       ((int)((uint)game.power_rank[civ2] * 0x14) < (int)game.turn)) {
      agree = true;
    }
    if (((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) && (city = diplo_is_hostile(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ), city == 0))
       && ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 1) != 0)) {
      if ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 4) == 0) {
        score = (uint)civs[civ].military_strength << 1;
      }
      else {
        score = ((uint)civs[civ].military_strength * 3) / 2;
      }
      if ((((byte)(civs[civ].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] >> 8) & 2) != 0) &&
         (score <= civs[tax_rate_icon_sprites_block.diplo_war_target_civ].military_strength)) {
        agree = true;
        reasons = 2;
      }
    }
    if (((tax_rate_icon_sprites_block.diplo_war_target_civ < 1) || (reasons == 0)) &&
       ((game.power_rank[civ2] < 7 &&
        ((((game.power_rank[civ] < 7 && (99 < game.turn)) &&
          (ally_count < (int)(7 - (uint)game.power_rank[civ2]))) &&
         (city = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), city < 5)))))) {
      best_rank = 8;
      target = -1;
      for (other = 1; other < 8; other++) {
        if ((((civ != other) && (civ2 != other)) &&
            (((game.power_rank[civ] <= game.power_rank[other] &&
              (game.power_rank[civ2] <= game.power_rank[other])) &&
             ((((*(byte *)&civs[civ2].treaties[other] & 1) != 0 ||
               ((*(byte *)&civs[civ].treaties[other] & 1) != 0)) &&
              (((byte)(civs[civ2].treaties[other] >> 8) & 0x20) == 0)))))) &&
           (((*(byte *)&civs[civ].treaties[other] & 8) == 0 &&
            (at_war = (uint)((*(byte *)&civs[civ2].treaties[other] & 1) != 0),
            (int)(game.power_rank[other] - at_war) < best_rank)))) {
          target = other;
          best_rank = game.power_rank[other] - at_war;
        }
      }
      if ((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) && (game.power_rank[tax_rate_icon_sprites_block.diplo_war_target_civ] != 7)) {
        target = tax_rate_icon_sprites_block.diplo_war_target_civ;
      }
      if (0 < target) {
        tax_rate_icon_sprites_block.diplo_war_target_civ = target;
        agree = true;
        reasons = reasons | 4;
      }
    }
    if (((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) ||
       (4 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]))) {
      agree = false;
    }
    if ((-1 < tax_rate_icon_sprites_block.diplo_war_target_civ) &&
       ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 8) != 0)) {
      agree = true;
      reasons = reasons | 1;
    }
    if (0x4b < tax_rate_icon_sprites_block.diplo_cur_attitude) {
      agree = false;
    }
    if ((4 < civs[civ].diplo_request_count) && (game.difficulty != '\0')) {
      agree = false;
    }
    diplo_set_attitude_arg(civ2,civ);
    str = civ_get_adjective(civ2);
    text_set_string_arg(1,(const char *)str);
    str = civ_get_adjective(civ);
    text_set_string_arg(2,(const char *)str);
    if (agree) {
      agree = true;
      if (((((1 < civs[civ].diplo_request_count) || (0x19 < tax_rate_icon_sprites_block.diplo_cur_attitude)) ||
           (civs[civ2].reputation != '\0')) || (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0)) &&
         ((((reasons & 3) == 0 || (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0)) && (0 < tax_rate_icon_sprites_block.diplo_offer_tech_best)))) {
        text_set_label_arg(3,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
        text_clear();
        diplo_leader_play_reaction(3);
        text_append((const char *)&str_perhapssecret);
        str = rand_range(0,1);
        text_append_int(str);
        city = popup_game((const char *)&map_views_block.text_buffer);
        if (city == 0) {
          agree = false;
        }
        else {
          paid = true;
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
          city = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
          civ_adjust_attitude(civ,civ2,city * -2);
          diplo_set_attitude_arg(civ2,civ);
          tax_rate_icon_sprites_block.diplo_offer_tech_best = tax_rate_icon_sprites_block.diplo_offer_tech_2nd;
          tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
        }
      }
      if ((((1 < civs[civ].diplo_request_count) || (0x19 < tax_rate_icon_sprites_block.diplo_cur_attitude)) ||
          ((civs[civ2].reputation != '\0' || (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0)))) &&
         (((((reasons & 2) == 0 || (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0)) && (!paid)) && (agree)))) {
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
          city = popup_game((const char *)&map_views_block.text_buffer);
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
        if (0 < tax_rate_icon_sprites_block.diplo_war_target_civ) {
          str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_war_target_civ));
          text_set_string_arg(4,(const char *)str);
          diplo_leader_play_reaction(3);
          city = popup_game((const char *)&str_perhapssolidarity);
          if (city == 0) {
            if (((reasons != 0) || (game.power_rank[civ2] == 7)) ||
               ((2 < (int)((uint)civs[civ2].reputation -
                          (int)civs[civ].reputation_credit[civ2]) ||
                (tax_rate_icon_sprites_block.diplo_ai_declare_war != 0)))) {
              civs[civ].diplo_request_count++;
              diplo_leader_play_reaction(3);
              if (!paid) {
                popup_game((const char *)&str_perhapsbye);
              }
              else {
                popup_game((const char *)&str_perhapsthanksanyway);
              }
              result = (uint)!paid;
              return result;
            }
          }
          else {
            diplo_declare_war(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ,civ);
            if ((reasons & 4) != 0) {
              diplo_set_treaty_flags(civ,tax_rate_icon_sprites_block.diplo_war_target_civ,0x2001);
              diplo_set_treaty_flags(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ,1);
            }
            treaty = (uint *)(tax_rate_icon_sprites_block.diplo_war_target_civ * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
            *treaty = *treaty | 0x80800;
            tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
          }
        }
        diplo_form_alliance(civ2,civ);
      }
      else {
        diplo_leader_play_reaction(3);
        popup_game((const char *)&str_alliancenopatience_2);
      }
    }
    else if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) &&
             ((int)((uint)civs[civ2].reputation -
                   (int)civs[civ].reputation_credit[civ2]) < 3)) &&
            (tax_rate_icon_sprites_block.diplo_captured_city_count == 0)) {
      if ((civs[civ2].num_cities < 4) ||
         ((game.power_rank[civ2] != 7 &&
          (((int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) < 3 ||
           (game.power_rank[civ2] < 5)))))) {
        city = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude);
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
      if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) &&
          (2 < (int)((uint)civs[civ2].reputation -
                    (int)civs[civ].reputation_credit[civ2]))) &&
         ((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0 || (city = diplo_attitude_level((uint)(tax_rate_icon_sprites_block.diplo_cur_attitude < 2)), city != 0)
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
    break;
  case 2:
    peace_flags = tax_rate_icon_sprites_block.diplo_target_is_leader == 0 &&
            (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x33 &&
            (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]) <=
            (int)((-(uint)(game.difficulty == '\0') & 2) + 4));
    if (((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) && (city = diplo_is_hostile(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ), city == 0))
       && ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 1) != 0)) {
      if ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 4) == 0) {
        score = (uint)civs[civ].military_strength << 1;
      }
      else {
        score = ((uint)civs[civ].military_strength * 3) / 2;
      }
      if ((((byte)(civs[civ].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] >> 8) & 2) != 0) &&
         (score <= civs[tax_rate_icon_sprites_block.diplo_war_target_civ].military_strength)) {
        peace_flags = 3;
      }
    }
    if ((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) &&
       ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 8) != 0)) {
      peace_flags = peace_flags | 4;
    }
    if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) &&
        (2 - leaders[civs[civ].tribe].attack <
         (int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2]
              ))) ||
       (6 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2]))) {
      peace_flags = 0;
    }
    if (0x4b < tax_rate_icon_sprites_block.diplo_cur_attitude) {
      peace_flags = 0;
    }
    city = civ_has_wonder(civ2,6);
    if ((city != 0) || (city = civ_has_wonder(civ2,0x18), city != 0)) {
      peace_flags = peace_flags | 8;
    }
    if (peace_flags == 0) {
      if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) &&
          ((int)((uint)civs[civ2].reputation -
                (int)civs[civ].reputation_credit[civ2]) < 5)) && (tax_rate_icon_sprites_block.diplo_captured_city_count < 2)) {
        if ((tax_rate_icon_sprites_block.diplo_target_is_leader == 0) && ((uint)game.power_rank[civ2] <= game.power_rank[civ] + 2))
        {
          if ((tax_rate_icon_sprites_block.diplo_cur_attitude < 0x33) &&
             ((game.power_rank[civ] <= game.power_rank[civ2] || (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x1a)))) {
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
        if ((((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) &&
            (4 < (int)((uint)civs[civ2].reputation -
                      (int)civs[civ].reputation_credit[civ2]))) &&
           ((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0 ||
            (city = diplo_attitude_level((uint)(tax_rate_icon_sprites_block.diplo_cur_attitude < 2)), city != 0)))) {
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
      if ((((1 < civs[civ].diplo_request_count) || (0x32 < tax_rate_icon_sprites_block.diplo_cur_attitude)) ||
          (1 < (int)((uint)civs[civ2].reputation -
                    (int)civs[civ].reputation_credit[civ2]))) &&
         (((peace_flags & 6) == 0 && (0 < tax_rate_icon_sprites_block.diplo_offer_tech_best)))) {
        text_set_label_arg(3,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
        diplo_leader_play_reaction(3);
        text_clear();
        text_append((const char *)&str_perhapssecret_2);
        str = rand_range(0,1);
        text_append_int(str);
        city = popup_game((const char *)&map_views_block.text_buffer);
        if (city == 0) {
          peace_flags = peace_flags & 8;
        }
        else {
          paid = true;
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
          city = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
          civ_adjust_attitude(civ,civ2,city * -2);
          diplo_set_attitude_arg(civ2,civ);
          tax_rate_icon_sprites_block.diplo_offer_tech_best = tax_rate_icon_sprites_block.diplo_offer_tech_2nd;
          tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
        }
      }
      if (((((1 < civs[civ].diplo_request_count) || (0x32 < tax_rate_icon_sprites_block.diplo_cur_attitude)) ||
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
          city = popup_game((const char *)&map_views_block.text_buffer);
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
        if (0 < tax_rate_icon_sprites_block.diplo_war_target_civ) {
          str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_war_target_civ));
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
              result = (uint)!paid;
              return result;
            }
          }
          else {
            diplo_declare_war(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ,civ);
            tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
          }
        }
        diplo_sign_peace_treaty(civ2,civ);
      }
    }
    break;
  case 3:
  case 4:
  case 5:
    patience = civs[civ].diplo_request_count;
    if (proposal < 5) {
      limit = diplo_patience_limit(civ2,civ);
      civs[civ].diplo_request_count = (i8)limit;
    }
    else {
      civs[civ].diplo_request_count++;
    }
    diplo_set_attitude_arg(civ2,civ);
    if ((tax_rate_icon_sprites_block.diplo_ai_declare_war != 0) || ((*(byte *)&civs[civ].treaties[civ2] & 0x60) != 0))
    {
      diplo_set_attitude_arg(civ2,civ);
      text_clear();
      text_append((const char *)&str_provoke);
      str = rand_range(0,2);
      text_append_int(str);
      tax_rate_icon_sprites_block.diplo_ai_declare_war = 0;
      diplo_leader_play_reaction(4);
      diplo_set_treaty_flags(civ,civ2,0x2000);
      civs[civ2].last_treaty_turn[civ] = game.turn;
      popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
      return 1;
    }
    if ((proposal == 3) && (game.power_rank[civ2] < game.power_rank[civ])) {
      tax_rate_icon_sprites_block.diplo_ai_fear_level = (uint)game.power_rank[civ] - (uint)game.power_rank[civ2];
    }
    tribute = int_clamp(tax_rate_icon_sprites_block.diplo_ai_fear_level,0,civs[civ].gold / 0x32);
    tribute = tribute * 0x32;
    if (game.turn - tax_rate_icon_sprites_block.diplo_last_treaty_turn < 8) {
      tribute = 0;
      tax_rate_icon_sprites_block.diplo_wanted_tech = -1;
    }
    if (game.turn - tax_rate_icon_sprites_block.diplo_last_treaty_turn < 0x10) {
      tribute = tribute / 2;
    }
    if ((*(byte *)&civs[civ2].treaties[civ] & 4) == 0) {
      tribute = tribute / 2;
    }
    has_wonder = civ_has_wonder(civ,6);
    if ((has_wonder != 0) || (has_wonder2 = civ_has_wonder(civ,0x18), has_wonder2 != 0)) {
      tribute = 0;
    }
    if (((((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) && (-1 < tax_rate_icon_sprites_block.diplo_wanted_tech))
        && ((uint)civs[civ2].num_advances + (uint)game.difficulty <
            (uint)civs[civ].num_advances)) &&
       ((civs[civ].military_strength < civs[civ2].military_strength ||
        (game.power_rank[civ2] < game.power_rank[civ])))) {
      tribute = 0;
      tax_rate_icon_sprites_block.diplo_ai_fear_level = 1;
    }
    units_moved = 0;
    city = civ_has_wonder(civ2,6);
    if ((city == 0) && (city = civ_has_wonder(civ2,0x18), city == 0)) {
      paid = false;
    }
    else {
      paid = true;
    }
    if (6 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2])) {
      paid = false;
    }
    if (((*(byte *)&civs[civ].treaties[civ2] & 4) != 0) &&
       ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0)) {
      if (((tax_rate_icon_sprites_block.diplo_ai_initial_aggressive != 0) || (0x4a < tax_rate_icon_sprites_block.diplo_cur_attitude)) && ((tax_rate_icon_sprites_block.diplo_ai_fear_level == 0 && (!paid)))) {
        rnd = _rand();
        rnd_sign = (int)rnd >> 0x1f;
        if (((int)(uint)civs[civ].government <
             (int)((((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) - rnd_sign) + 5)) &&
           ((((byte)(civs[civ].treaties[civ2] >> 8) & 0x40) == 0 &&
            (0x19 < tax_rate_icon_sprites_block.diplo_cur_attitude)))) goto apply_attitude_change;
      }
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
            units[unit].orders = -1;
            map_redraw_tile_all_views(x2,y2);
            units_moved++;
          }
        }
      }
    }
apply_attitude_change:
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
    if (((tribute == 0) && ((tax_rate_icon_sprites_block.diplo_wanted_tech < 0 || (tax_rate_icon_sprites_block.diplo_ai_fear_level == 0)))) || (proposal == 5)) {
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        if (((tax_rate_icon_sprites_block.diplo_ai_initial_aggressive == 0) && (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x4c)) ||
           ((units_moved != 0 ||
            (((((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0 || (paid)) ||
               (((byte)(civs[civ].treaties[civ2] >> 8) & 0x40) != 0)) ||
              (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x1a)) && ((city = _rand(), city % 3 != 0 || (proposal == 5))))))))
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
          tax_rate_icon_sprites_block.diplo_ai_declare_war = 0;
          diplo_leader_play_reaction(4);
          diplo_set_treaty_flags(civ,civ2,0x2000);
          popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
          civs[civ2].last_treaty_turn[civ] = game.turn;
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
                                              ((game.advance_known_by[0xf] != '\0') - 1 & 0xffff7c80
                                              )),0);
          }
          else if (((tax_rate_icon_sprites_block.diplo_ai_initial_aggressive == 0) && (tax_rate_icon_sprites_block.diplo_cur_attitude < 0x33)) && (tax_rate_icon_sprites_block.diplo_target_is_leader == 0)) {
            if (units_moved == 0) {
              diplo_leader_play_reaction(3);
              popup_game_with_sprite((const char *)&str_feebleally,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                ((game.advance_known_by[0xf] != '\0') - 1 &
                                                0xffff7c80)),0);
            }
          }
          else {
            diplo_leader_play_reaction(4);
            popup_game_with_sprite((const char *)&str_tauntally,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[0xf] != '\0') - 1 & 0xffff7c80
                                              )),0);
            diplo_cancel_alliance(civ2,civ);
            result = 1;
          }
        }
      }
    }
    else {
      if ((tax_rate_icon_sprites_block.diplo_wanted_tech < 0) ||
         ((tribute != 0 && (civs[civ].num_advances <= civs[civ2].num_advances)))) {
        text_set_number_arg(0,tribute);
        diplo_leader_play_reaction(2);
        if (proposal == 3) {
          popup_game_with_sprite((const char *)&str_givemoreally,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
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
        text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_wanted_tech].name);
        diplo_leader_play_reaction(2);
        popup_game((const char *)&str_givemoreciv);
        civ_give_advance(civ2,tax_rate_icon_sprites_block.diplo_wanted_tech,civ,0,0);
        tax_rate_icon_sprites_block.diplo_wanted_tech = -1;
      }
      if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
        if (tax_rate_icon_sprites_block.diplo_had_ceasefire == 0) {
          diplo_set_treaty_flags(civ,civ2,0x40000);
        }
        diplo_set_treaty_flags(civ,civ2,2);
      }
    }
    break;
  case 6:
    diplo_clear_treaty_flags(civ,civ2,8);
    civ_adjust_attitude(civ,civ2,0x32);
    diplo_eval_ai_stance(civ2,civ);
    diplo_set_attitude_arg(civ2,civ);
    diplo_leader_play_reaction(4);
    popup_game_with_sprite((const char *)&str_cancelally,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    civs[civ2].reputation++;
    diplo_cancel_alliance(civ2,civ);
  }
  return result;
}

/* 0045dd7f auto */
void __cdecl diplo_favor_menu(iptr civ2,iptr civ)

{
  uint *treaty;
  char patience;
  byte visible_bits;
  bool no_embassy;
  iptr menu_choice;
  iptr exchanged;
  iptr patience_limit;
  iptr civ_name;
  int civ3;
  uint civ_mask;
  byte *known;
  uint civ2_mask;
  i32 arg_a;
  i32 arg_b;
  iptr target_civ;
  iptr unit;
  int civ4;
  int choice;
  byte civ2_mask_byte;
  byte civ_mask_byte;
  iptr city;
  int distrust;
  uint y2;
  uint x2;
  Dialog dlg;
  int i;
  int price;
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
favor_menu_loop:
  do {
    diplo_set_attitude_arg(civ2,civ);
    menu_choice = popup_game((const char *)&str_favormenu);
    if (menu_choice == 0) {
      civs[civ].diplo_request_count--;
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
      goto done;
    }
    if (menu_choice == 1) {
      exchanged = diplo_tech_exchange(civ2,civ,0xffffffff,0xffffffff,0,0);
      if ((exchanged != 0) &&
         (patience = civs[civ].diplo_request_count, patience_limit = diplo_patience_limit(civ2,civ),
         patience + 1 < patience_limit)) {
        civs[civ].diplo_request_count++;
        goto favor_menu_loop;
      }
      net_send_state_diff(0xff,2,0,0,0);
    }
    if (menu_choice != 2) goto check_choice_3;
    dialog_load_game_text(&dlg,(const char *)&str_mustattack);
    for (civ4 = 1; civ4 < 8; civ4++) {
      if ((((1 << ((byte)civ4 & 0x1f) & (uint)game.civs_alive) != 0) && (civ4 != civ))
         && (civ4 != civ2)) {
        arg_a = 0;
        civ3 = civ4;
        civ_name = (iptr)(civ_name_plural(civ4));
        dialog_add_option(&dlg,(const char *)civ_name,civ3,arg_a);
      }
    }
    civ3 = dlg_show_modal(&dlg,0);
  } while (civ3 == 0);
  civ_name = (iptr)(civ_name_plural(civ3));
  text_set_string_arg(1,(const char *)civ_name);
  if ((*(byte *)&civs[civ].treaties[civ3] & 1) == 0) {
    diplo_leader_play_reaction(3);
    popup_game((const char *)&str_nocontact);
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
  }
  else if (((byte)(civs[civ].treaties[civ3] >> 8) & 0x20) == 0) {
    price = int_clamp((int)(((((uint)civs[civ2].reputation + civs[civ].gold / 100
                                          ) - (uint)civs[civ3].reputation) -
                                        (int)civs[civ].reputation_credit[civ2]) *
                                       ((int)(uint)civs[civ3].military_strength >> 1)) / 0x32,2,500
                                 );
    price = price * 0x32;
    if ((*(byte *)&civs[civ].treaties[civ3] & 6) == 0) {
      price -= price / 3;
    }
    if (civs[civ].gold < civs[civ2].gold) {
      price += price / 3;
    }
    if ((*(byte *)&civs[civ].treaties[civ3] & 8) != 0) {
      if (7000 < price) {
        price = 7000;
      }
      price = int_clamp(price * 3,500,25000);
    }
    if ((game.power_rank[civ3] == 7) && (4 < civs[civ3].num_cities)) {
      price = price / 2;
    }
    if (((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) == 0) {
      price += price / 3;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      no_embassy = true;
      for (i = 1; i < 8; i++) {
        if (((byte)(civs[civ].treaties[i] >> 8) & 0x20) != 0) {
          no_embassy = false;
        }
      }
      if (no_embassy) {
        price -= ((int)(price + (price >> 0x1f & 3U)) >> 2);
      }
      if (((byte)(civs[civ3].treaties[civ2] >> 8) & 0x10) != 0) {
        price -= ((int)(price + (price >> 0x1f & 3U)) >> 2);
      }
      if (game.power_rank[civ2] < game.power_rank[civ]) {
        price -= price / 3;
      }
    }
    if (((byte)(civs[civ3].treaties[civ2] >> 16) & 2) != 0) {
      if (((byte)(civs[civ2].treaties[civ3] >> 16) & 2) == 0) {
        price = price / 2;
      }
      if (game.power_rank[civ2] < game.power_rank[civ3]) {
        price = price / 2;
      }
    }
    price = int_clamp(price / 100,1,500);
    price = price * 0x32;
    text_set_number_arg(0,price);
    distrust = ((uint)civs[civ2].reputation -
                (int)civs[civ].reputation_credit[civ2]) -
                (int)leaders[civs[civ].tribe].attack;
    if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
      distrust += 2;
    }
    if (((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) == 0) {
      distrust++;
    }
    if (game.power_rank[civ3] < game.power_rank[civ2]) {
      distrust++;
    }
    if (game.power_rank[civ] < game.power_rank[civ2]) {
      distrust++;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      distrust = distrust / 2;
    }
    if ((distrust < 7) ||
       ((((*(byte *)&civs[civ].treaties[civ3] & 8) == 0 &&
         ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0)) &&
        ((((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) != 0 &&
         (game.power_rank[civ2] <= game.power_rank[civ3])))))) {
      if ((*(byte *)&civs[civ].treaties[civ3] & 8) == 0) {
        if ((((civs[civ].gold < 1000) || (civs[civ].gold <= civs[civ2].gold)) ||
            (tax_rate_icon_sprites_block.diplo_offer_tech_best < 0)) || (tax_rate_icon_sprites_block.diplo_offer_tech_2nd < 0)) {
          choice = popup_game((const char *)&str_mercenary);
        }
        else {
          price = -1;
          text_set_label_arg(2,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
          text_set_label_arg(3,advances[tax_rate_icon_sprites_block.diplo_offer_tech_2nd].name);
          choice = popup_game((const char *)&str_cybercop);
        }
      }
      else {
        diplo_leader_play_reaction(4);
        choice = popup_game((const char *)&str_eternalallies);
      }
      if (choice == 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
      }
      else {
        if (price < 0) {
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_2nd,civ2,0,0);
          tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
          tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
        }
        else {
          if (civs[civ2].gold < price) {
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_unfortunate);
            seh_state = 0xffffffff;
            /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
            goto done;
          }
          civs[civ2].gold -= price;
          civs[civ].gold += price;
        }
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
            net_send_text_args(0x62,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                               3,0,0,0);
          }
          (*((char *)&civ4 + 0)) = (byte)civ3;
          if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
             (human_civ[0] != civ3)) {
            net_send_text_args(0x62,net_session_block.net_slots[net_session_block.net_civ_slots[civ3]].id,3,
                               0,0,0);
          }
        }
        civ_name = civ_get_adjective(civ3);
        text_set_string_arg(1,(const char *)civ_name);
        if ((*(byte *)&civs[civ2].treaties[civ3] & 8) == 0) {
          if ((*(byte *)&civs[civ2].treaties[civ3] & 4) != 0) {
            civ_mask = _rand();
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
          civ_mask = _rand();
          civ2_mask = (int)civ_mask >> 0x1f;
          if (((civ_mask ^ civ2_mask) - civ2_mask & 1 ^ civ2_mask) == civ2_mask) {
            civ_adjust_attitude(civ3,civ2,0x19);
            diplo_leader_play_reaction(4);
            popup_game((const char *)&str_mercbetrayally);
            diplo_cancel_alliance(civ2,civ3);
          }
        }
        net_send_state_diff(0xff,2,0,0,0);
check_choice_3:
        if (menu_choice == 3) {
          if ((((((*(byte *)&civs[civ2].treaties[civ] & 8) == 0) &&
                (0 < tax_rate_icon_sprites_block.diplo_cur_attitude)) && (civ3 = civ_has_wonder(civ2,9), civ3 == 0)) &&
              (civ3 = civ_has_wonder(civ2,0xc), civ3 == 0)) ||
             (((0x19 < tax_rate_icon_sprites_block.diplo_cur_attitude || (civ3 = civ_has_advance(civ,0x2e), civ3 == 0)) ||
              (civ3 = civ_has_advance(civ2,0x2e), civ3 == 0)))) {
            civ_name = civ_get_adjective(civ2);
            text_set_string_arg(1,(const char *)civ_name);
            diplo_leader_play_reaction(3);
            popup_game((const char *)&str_mapno);
            seh_state = 0xffffffff;
            /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
            goto done;
          }
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
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
      }
    }
    else {
      diplo_leader_play_reaction(3);
      popup_game((const char *)&str_hellnowewontgo);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
    }
  }
  else {
    diplo_leader_play_reaction(3);
    popup_game((const char *)&str_atwar);
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0045f08d */ { dialog_dtor(&dlg); }
  }
done:
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
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
  diplo_set_attitude_arg(civ2,civ);
  menu_choice = popup_game((const char *)&str_giftmenu);
  if (menu_choice == 0) {
    civs[civ].diplo_request_count--;
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
  }
  else if (menu_choice == 1) {
    diplo_tech_exchange(civ2,civ,0xffffffff,0xffffffff,0,1);
    if (tax_rate_icon_sprites_block.diplo_offer_tech_best < 0) {
      str = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)str);
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_knowno);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
    }
    else {
      text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
      if (tax_rate_icon_sprites_block.diplo_offer_tech_2nd < 0) {
        choice = popup_game((const char *)&str_techgift);
      }
      else {
        text_set_label_arg(2,advances[tax_rate_icon_sprites_block.diplo_offer_tech_2nd].name);
        choice = popup_game((const char *)&str_techgift2);
      }
      if (choice == 0) {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_wasting);
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
      }
      else {
        if (choice == 1) {
          value = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
          value = value << 2;
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
        }
        else {
          value = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
          value = value * 2;
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_2nd,civ2,0,0);
        }
        civ_adjust_attitude(civ,civ2,-value);
        diplo_set_attitude_arg(civ2,civ);
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(2);
        popup_game((const char *)&str_accept);
        civs[civ].diplo_request_count -= ((char)tax_rate_icon_sprites_block.diplo_gifts_this_meeting + 2);
        tax_rate_icon_sprites_block.diplo_gifts_this_meeting++;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
      }
    }
  }
  else if (menu_choice == 2) {
    dialog_load_game_text(&dlg,(const char *)&str_moneygift);
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
        dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,gold_amount,0);
      }
    }
    gold_amount = dlg_show_modal(&dlg,0);
    if (gold_amount == 0) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_wasting_2);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
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
      civs[civ].diplo_request_count -= ((char)tax_rate_icon_sprites_block.diplo_gifts_this_meeting + 2);
      tax_rate_icon_sprites_block.diplo_gifts_this_meeting++;
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
    }
  }
  else {
    if (menu_choice == 3) {
      city_count = 0;
      dlg_load_game_section(&dlg,(const char *)&str_militarysource,1);
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if ((cities[city2].id != 0) && (cities[city2].owner == civ2)) {
          dialog_add_option
                    (&dlg,(const char *)&cities[city2].name,city2,0);
          city_count++;
        }
      }
      if (city_count == 0) {
        popup_game((const char *)&str_militarynone);
        civs[civ].diplo_request_count--;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
      }
      city = dlg_show_modal(&dlg,0);
      if (city < 0) {
        civs[civ].diplo_request_count--;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
      }
      unit2 = tile_first_unit((int)cities[city].x,(int)cities[city].y);
      if (unit2 < 0) {
        popup_game((const char *)&str_militarynone_2);
        civs[civ].diplo_request_count--;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
      }
      city4 = city_find_nearest((int)cities[city].x,(int)cities[city].y,civ,0xffffffff,
                                    -1);
      if ((((city4 < 0) || (level = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), 4 < level)) ||
          (civs[civ2].num_advances < civs[civ].num_advances)) ||
         ((game.power_rank[civ2] < game.power_rank[civ] ||
          (civs[civ2].military_strength < civs[civ].military_strength)))) {
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_militaryno);
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
      }
      text_clear();
      text_append_label(0x77);
      text_append_space();
      text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
      unit = unit_stack_select_dialog(unit2,(const char *)&map_views_block.text_buffer,1);
      if (unit < 0) {
        civs[civ].diplo_request_count--;
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
      }
      if ((-1 < unit_types[units[unit].type].obsolete_by) &&
         (has_advance = civ_has_advance(civ,(int)unit_types[units[unit].type].obsolete_by),
         has_advance != 0)) {
        str = civ_get_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_militaryno_2);
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
        goto done;
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
      city_window_refresh_if_city((CityWindow *)((i8 *)&city_window_block.city_window),city3);
      map_redraw_tile_all_views((int)cities[city3].x,(int)cities[city3].y);
      city_window_refresh_if_city((CityWindow *)((i8 *)&city_window_block.city_window),city);
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
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0045fd43 */ { dialog_dtor(&dlg); }
  }
done:
  return;
}

/* 0045fd67 auto */
void __cdecl diplo_ai_senate_hawks_notice(iptr civ,iptr civ2)

{
  iptr name2;
  int has_wonder;
  
  if ((4 < civs[civ].government) &&
     ((((game.game_flags & 0x80) == 0 || ((prefs_game_rules_block.scenario_flags[0] & 1) == 0)) && (civ_saved_cursor_x_block.autoplay_active == 0)))) {
    name2 = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(1,(const char *)name2);
    has_wonder = civ_has_wonder(civ,0x18);
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
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  choice = 0;
  dialog_ctor(&dlg,0x4000);
  seh_state = 0;
  diplo_set_attitude_arg(civ2,civ);
  dialog_load_game_text(&dlg,(const char *)&str_diplomacy);
  err = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_diplomacymenu);
  if (err == 0) {
    text_read_line(1);
    dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
    allied = false;
    at_peace = false;
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
      if ((*(byte *)&civs[civ].treaties[civ2] & 4) != 0) {
        at_peace = true;
      }
    }
    else {
      allied = true;
    }
    text_read_line(1);
    if (at_peace) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,1,0);
    }
    text_read_line(1);
    if ((!at_peace) && (!allied)) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,2,0);
    }
    text_read_line(1);
    if (allied) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,3,0);
    }
    text_read_line(1);
    if ((!allied) && (civs[civ2].government < 5)) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,4,0);
    }
    text_read_line(1);
    if ((at_peace) && (((byte)(civs[civ].treaties[civ2] >> 8) & 0x40) == 0)) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,5,0);
    }
    text_read_line(1);
    if (allied) {
      dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,6,0);
    }
    text_read_line(1);
    dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,7,0);
    text_read_line(1);
    dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,8,0);
    choice = dlg_show_modal(&dlg,0);
  }
  text_close_file();
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00460104 */ { dialog_dtor(&dlg); }
  return choice;
}

/* 00460129 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
diplo_ai_meet_player(iptr civ2,iptr civ,i32 x,i32 y,int player_initiated)

{
  char *counter;
  uint *treaty;
  char patience;
  byte best_rank;
  char rel;
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
  uint rnd;
  iptr patience_limit;
  iptr str;
  int advance2;
  uint rnd_sign;
  bool no_contact;
  int roll2;
  int roll;
  int choice;
  iptr other;
  iptr advance;
  iptr continent;
  int distrust;
  int civ_ally_count;
  int ally_count;
  iptr stance_idx;
  uint hostile;
  int threat_count;
  int variant;
  int threshold;
  iptr civ3;
  
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
  diplo_ai_meeting_active_i32 = 1;
  if ((scenario_events_loaded != 0) &&
     (allowed = events_check_negotiation((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events),civ2,civ),
     allowed == 0)) {
    diplo_ai_meeting_active_i32 = 0;
    return;
  }
  tax_rate_icon_sprites_block.diplo_attitude_target = civ2;
  tax_rate_icon_sprites_block.diplo_attitude_civ = civ;
  tax_rate_icon_sprites_block.diplo_gifts_this_meeting = 0;
  if (player_initiated == 0) {
    civs[civ].diplo_request_count = '\0';
  }
  diplo_initial_eval_done = 0;
  diplo_opening_eval_active = 1;
  diplo_eval_ai_stance(civ2,civ);
  diplo_initial_eval_done = 1;
  diplo_opening_eval_active = 0;
  (*(uint *)&tax_rate_icon_sprites_block.diplo_had_ceasefire) = (uint)((*(byte *)&civs[civ].treaties[civ2] & 2) != 0);
  no_contact = (*(byte *)&civs[civ].treaties[civ2] & 1) == 0;
  if (player_initiated == 0) {
    civs[civ2].last_treaty_turn[civ] = game.turn;
    diplo_clear_treaty_flags(civ,civ2,0x40002);
  }
  else {
    if (civs[civ].diplo_request_count < '\0') {
      civs[civ].diplo_request_count = '\0';
    }
    if (((byte)(civs[civ2].treaties[civ] >> 8) & 8) != 0) {
      limit = diplo_patience_limit(civ2,civ);
      civs[civ].diplo_request_count = (i8)limit;
    }
    if (((byte)(civs[civ2].treaties[civ] >> 16) & 1) == 0) {
      if ((((3 < game.difficulty) && (advance2 = civ_spaceship_launched(civ), advance2 != 0)) &&
          (advance2 = civ_spaceship_launched(civ2), advance2 != 0)) &&
         ((civs[civ2].spaceship_arrival_year < civs[civ].spaceship_arrival_year &&
          (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0)))) {
        diplo_set_treaty_flags(civ,civ2,0x200000);
      }
    }
    else {
      civs[civ].diplo_request_count = '\0';
      diplo_clear_treaty_flags(civ,civ2,0x200000);
    }
  }
  (*(uint *)&diplo_emissary_response) = (uint)(player_initiated != 0);
  if (player_initiated != 0) {
    if (((byte)(civs[civ].treaties[civ2] >> 16) & 0x20) != 0) {
      if (diplo_emissary_asked == 0) {
        diplo_set_attitude_arg(civ2,civ);
      }
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_outahere);
      ended = true;
      goto end_meeting;
    }
    patience = civs[civ].diplo_request_count;
    advance2 = diplo_patience_limit(civ2,civ);
    if ((advance2 <= patience) && (((byte)(civs[civ2].treaties[civ] >> 16) & 0x80) != 0)) {
      if (diplo_emissary_asked == 0) {
        diplo_set_attitude_arg(civ2,civ);
      }
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        diplo_leader_play_reaction(4);
        popup_game((const char *)&str_outahere_2);
      }
      else {
        diplo_leader_play_reaction(3);
        popup_game_with_sprite((const char *)&str_outahereally,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
      }
      ended = true;
      goto end_meeting;
    }
    diplo_ask_receive_emissary(civ2,civ,x,y);
  }
  diplo_set_treaty_flags(civ2,civ,0x800000);
  emissary_sent = true;
  hostile = (uint)(((byte)(civs[civ].treaties[civ2] >> 16) & 0x40) != 0);
  diplo_clear_treaty_flags(civ,civ2,0x400000);
  if (((game.game_flags & 0x80) != 0) && (((int)(*(short *)&prefs_game_rules_block.scenario_flags) & 0x8000U) != 0)) {
    if (((civ == 3) && (civ2 == 6)) || ((civ == 6 && (civ2 == 3)))) {
      tax_rate_icon_sprites_block.diplo_war_target_civ = 0;
      tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
      tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
      hostile = 0;
    }
    if (((civ == 3) && (civ2 == 1)) || ((civ == 1 && (civ2 == 3)))) {
      tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
      tax_rate_icon_sprites_block.diplo_war_target_civ = 0;
      hostile = 0;
    }
  }
  diplo_tech_exchange(civ2,civ,x,y,1,0);
  if ((((tax_rate_icon_sprites_block.diplo_ai_aggressive == 0) && (player_initiated == 0)) &&
      ((*(byte *)&civs[civ].treaties[civ2] & 6) != 0)) && (2 < tax_rate_icon_sprites_block.diplo_ai_fear_level))
  goto end_meeting;
  diplo_set_attitude_arg(civ2,civ);
  if (((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) && (tax_rate_icon_sprites_block.diplo_war_target_civ < 8)) &&
     ((1 << ((byte)tax_rate_icon_sprites_block.diplo_war_target_civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
  }
  if ((((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) && (tax_rate_icon_sprites_block.diplo_space_race_threat != 0)) &&
     ((*(int *)&tax_rate_icon_sprites_block.diplo_ai_outranks_target) != 0)) {
    tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
  }
  if ((0 < tax_rate_icon_sprites_block.diplo_war_target_civ) &&
     (((tax_rate_icon_sprites_block.diplo_ai_request_mode != 0 || ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0)) &&
      (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)))) {
    stance_idx = 2;
    if ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 4) != 0) {
      stance_idx = 1;
    }
    if ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 8) != 0) {
      stance_idx = 0;
    }
    str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_war_target_civ));
    text_set_string_arg(1,(const char *)str);
    if (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0) {
      variant = 0x6d;
    }
    else {
      variant = 0x6c;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      variant = 0x6e;
    }
    text_set_label_arg(2,label_string_ids[variant]);
    text_clear();
    text_append((const char *)&str_canceltreaty);
    text_append_int(stance_idx);
    advance2 = popup_game((const char *)&map_views_block.text_buffer);
    if (advance2 == 0) {
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        civ_adjust_attitude(civ,civ2,5);
      }
      else {
        if (tax_rate_icon_sprites_block.diplo_ai_request_mode != 0) {
          civ_adjust_attitude(civ,civ2,0x14);
        }
        propose = true;
      }
      civs[civ].diplo_request_count++;
    }
    else {
      diplo_declare_war(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ,civ);
      declared_war = true;
      tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
      tax_rate_icon_sprites_block.diplo_strongest_enemy = -1;
      tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        rnd = _rand();
        rnd_sign = (int)rnd >> 0x1f;
        if ((((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) != rnd_sign) &&
           (tax_rate_icon_sprites_block.diplo_tribute_demand = 0, (*(uint *)&tax_rate_icon_sprites_block.diplo_had_ceasefire) != 0)) {
          diplo_set_treaty_flags(civ,civ2,2);
        }
      }
      else {
        tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
        paid = true;
      }
    }
    diplo_set_attitude_arg(civ2,civ);
  }
  if ((*(byte *)&civs[civ2].treaties[civ] & 8) != 0) {
    if ((4 < (int)((uint)civs[civ2].reputation -
                  (int)civs[civ].reputation_credit[civ2])) && (!paid)) {
      propose = true;
    }
    if (-1 < tax_rate_icon_sprites_block.diplo_offer_tech_best) {
      if (game.power_rank[civ] < game.power_rank[civ2]) {
        if ((civs[civ].num_advances < civs[civ2].num_advances) ||
           (civs[civ2].gold < civs[civ].gold)) {
          tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
        }
      }
      else if ((civs[civ].num_advances < civs[civ2].num_advances) &&
              (civs[civ2].gold < civs[civ].gold)) {
        tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
      }
    }
    if ((tax_rate_icon_sprites_block.diplo_space_race_threat != 0) && ((*(int *)&tax_rate_icon_sprites_block.diplo_ai_outranks_target) != 0)) {
      propose = true;
      tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
    }
    if (((((game.game_flags & 0x80) != 0) && (((int)(*(short *)&prefs_game_rules_block.scenario_flags) & 0x8000U) != 0)) && (civ == 6)
        ) && (civ2 == 7)) {
      tax_rate_icon_sprites_block.diplo_tribute_demand = 0;
      tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
      propose = true;
    }
  }
  advance2 = civ_has_advance(civ2,0x35);
  if ((advance2 == 0) && (advance2 = civ_has_advance(civ2,0), advance2 == 0)) {
    advance2 = civ_has_advance(civ2,0x11);
    if ((advance2 == 0) && (advance2 = civ_has_advance(civ2,0x51), advance2 == 0)) {
      advance2 = civ_has_advance(civ2,0x23);
      if ((advance2 != 0) && (advance2 = civ_has_advance(civ,0x23), advance2 == 0)) {
        tech_gap = true;
      }
    }
    else {
      advance2 = civ_has_advance(civ,0x11);
      if ((advance2 == 0) && (advance2 = civ_has_advance(civ,0x51), advance2 == 0)) {
        tech_gap = true;
      }
    }
  }
  else {
    advance2 = civ_has_advance(civ,0x35);
    if ((advance2 == 0) && (advance2 = civ_has_advance(civ,0), advance2 == 0)) {
      tech_gap = true;
    }
  }
  if ((civs[civ2].num_advances <= civs[civ].num_advances) &&
     (advance2 = civ_has_advance(civ,0x11), advance2 != 0)) {
    tech_gap = false;
  }
  if (((0 < tax_rate_icon_sprites_block.diplo_tribute_demand) &&
      (((tax_rate_icon_sprites_block.diplo_ai_request_mode == 2 || (propose)) && (tax_rate_icon_sprites_block.diplo_tribute_demand <= civs[civ2].gold)))) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)) {
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      advance2 = civs[civ2].gold / 2;
      if (advance2 < 0x65) {
        advance2 = 100;
      }
      if (advance2 <= tax_rate_icon_sprites_block.diplo_tribute_demand) {
        tax_rate_icon_sprites_block.diplo_tribute_demand = advance2;
      }
    }
    str = (iptr)(civ_leader_name(civ));
    text_set_string_arg(1,(const char *)str);
    str = civ_leader_title(civ);
    text_set_string_arg(2,(const char *)str);
    str = civ_get_adjective(civ);
    text_set_string_arg(3,(const char *)str);
    text_set_label_arg(4,label_string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
    text_set_number_arg(0,tax_rate_icon_sprites_block.diplo_tribute_demand);
    text_clear();
    diplo_leader_play_reaction(4);
    text_append((const char *)&str_tribute);
    variant = rand_range(0,3);
    if ((variant == 3) && (civs[civ].military_strength < civs[civ2].military_strength)) {
      variant = 2;
    }
    if ((variant == 2) && (game.first_discoverer[0x26] == '\0')) {
      variant = 1;
    }
    if (tech_gap) {
      variant = 8;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      variant = 9;
    }
    text_append_int(variant);
    if (variant == 9) {
      choice = popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[0xf] != '\0') - 1 &
                                                   0xffff7c80)),0);
    }
    else {
      choice = popup_game((const char *)&map_views_block.text_buffer);
    }
    if (choice == 1) {
      civs[civ2].gold -= tax_rate_icon_sprites_block.diplo_tribute_demand;
      civs[civ].gold += tax_rate_icon_sprites_block.diplo_tribute_demand;
      tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
      paid = true;
      status_redraw(1);
      if (civs[civ].diplo_request_count != '\0') {
        civs[civ].diplo_request_count--;
      }
      advance2 = diplo_gold_attitude_value(tax_rate_icon_sprites_block.diplo_tribute_demand);
      civ_adjust_attitude(civ,civ2,-advance2);
      diplo_set_attitude_arg(civ2,civ);
      if ((*(uint *)&tax_rate_icon_sprites_block.diplo_had_ceasefire) != 0) {
        diplo_set_treaty_flags(civ,civ2,2);
      }
    }
    tax_rate_icon_sprites_block.diplo_tribute_demand = 999;
  }
  if (((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) &&
     (advance2 = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), 3 < advance2)) {
    propose = true;
  }
  if ((((tax_rate_icon_sprites_block.diplo_ai_request_mode != 0) || (propose)) && (-1 < tax_rate_icon_sprites_block.diplo_offer_tech_best)) &&
     ((tax_rate_icon_sprites_block.diplo_tribute_demand != 999 &&
      (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)))) {
    text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_offer_tech_best].name);
    str = (iptr)(civ_leader_name(civ));
    text_set_string_arg(3,(const char *)str);
    str = civ_leader_title(civ);
    text_set_string_arg(2,(const char *)str);
    text_set_label_arg(4,label_string_ids[0x6a - (uint)(leaders[civs[civ].tribe].female == '\0')]);
    text_clear();
    text_append((const char *)&str_takeciv);
    variant = rand_range(0,2);
    if ((tech_gap) && (variant < 2)) {
      variant = 8;
    }
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      variant = 9;
    }
    text_append_int(variant);
    if (variant == 9) {
      choice = popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                   ((game.advance_known_by[0xf] != '\0') - 1 &
                                                   0xffff7c80)),0);
    }
    else {
      choice = popup_game((const char *)&map_views_block.text_buffer);
    }
    if (choice == 1) {
      civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
      advance2 = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
      civ_adjust_attitude(civ,civ2,-((advance2 * 3) / 2));
      advance2 = tax_rate_icon_sprites_block.diplo_offer_tech_2nd;
      tax_rate_icon_sprites_block.diplo_offer_tech_best = tax_rate_icon_sprites_block.diplo_offer_tech_2nd;
      tax_rate_icon_sprites_block.diplo_offer_tech_2nd = -1;
      if (((tax_rate_icon_sprites_block.diplo_ai_units_near_cities == 0) || (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0)) || (advance2 < 0)) {
        paid = true;
      }
      else {
        text_set_label_arg(1,advances[advance2].name);
        diplo_leader_play_reaction(4);
        advance2 = popup_game((const char *)&str_overabarrel);
        if (advance2 == 1) {
          civ_give_advance(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best,civ2,0,0);
          advance2 = ai_advance_value(civ,tax_rate_icon_sprites_block.diplo_offer_tech_best);
          civ_adjust_attitude(civ,civ2,-((advance2 * 3) / 2));
          tax_rate_icon_sprites_block.diplo_offer_tech_best = -1;
          paid = true;
        }
      }
      if (paid) {
        tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
        if ((*(uint *)&tax_rate_icon_sprites_block.diplo_had_ceasefire) != 0) {
          diplo_set_treaty_flags(civ,civ2,2);
        }
        if (civs[civ].diplo_request_count != '\0') {
          civs[civ].diplo_request_count--;
        }
      }
    }
    else if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
      civ_adjust_attitude(civ,civ2,
                         (civs[civ2].reputation + 2) -
                         (int)civs[civ].reputation_credit[civ2]);
      civs[civ].diplo_request_count++;
    }
    diplo_set_attitude_arg(civ2,civ);
    tax_rate_icon_sprites_block.diplo_tribute_demand = 999;
  }
  if (((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) &&
     ((((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0 || (0x31 < tax_rate_icon_sprites_block.diplo_cur_attitude)) || ((tax_rate_icon_sprites_block.diplo_space_race_threat != 0 && ((*(int *)&tax_rate_icon_sprites_block.diplo_ai_outranks_target) != 0)))
       ) && (!paid)))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    if (tax_rate_icon_sprites_block.diplo_tribute_demand == 999) {
      popup_game_with_sprite((const char *)&str_begone0,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
    else {
      popup_game_with_sprite((const char *)&str_begone1,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
    diplo_cancel_alliance(civ,civ2);
    civs[civ2].last_treaty_turn[civ] = game.turn;
    if ((tax_rate_icon_sprites_block.diplo_space_race_threat != 0) && (3 < game.difficulty)) {
      tax_rate_icon_sprites_block.diplo_ai_declare_war = 1;
    }
    goto end_meeting;
  }
  if ((hostile == 0) || (tax_rate_icon_sprites_block.diplo_ai_aggressive == 0)) {
    hostile = 0;
  }
  else {
    hostile = 1;
  }
  if (hostile != 0) {
    tax_rate_icon_sprites_block.diplo_ai_aggressive = 0;
  }
  if ((((((no_contact) && (!paid)) && (tax_rate_icon_sprites_block.diplo_tribute_demand == 999)) &&
       ((tax_rate_icon_sprites_block.diplo_ai_aggressive == 0 &&
        (advance2 = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude),
        (int)((-(uint)(tax_rate_icon_sprites_block.diplo_ai_initial_aggressive == 0) & 2) + 2) < advance2)))) &&
      (advance2 = civ_has_wonder(civ2,6), advance2 == 0)) &&
     (advance2 = civ_has_wonder(civ2,0x18), advance2 == 0)) {
    rnd = _rand();
    rnd_sign = (int)rnd >> 0x1f;
    if ((int)(uint)civs[civ].government <
        (int)(6 - (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) - rnd_sign))) {
      tax_rate_icon_sprites_block.diplo_ai_aggressive = 1;
    }
  }
  if (((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0) && (0x32 < tax_rate_icon_sprites_block.diplo_tribute_demand)) &&
     (((*(byte *)&civs[civ].treaties[civ2] & 4) != 0 ||
      (((advance2 = diplo_is_hostile(civ,civ2), advance2 == 0 && (0 < tax_rate_icon_sprites_block.diplo_last_treaty_turn)) &&
       (((byte)(civs[civ].treaties[civ2] >> 16) & 1) == 0)))))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_emissary_asked == 0) {
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
    popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_treaty_turn[civ] = game.turn;
    goto end_meeting;
  }
  if ((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0) && (tax_rate_icon_sprites_block.diplo_tribute_demand == 999)) {
    if (diplo_emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    advance2 = civ_get_attitude(civ,civ2);
    if (advance2 < 0x4b) {
      civ_set_attitude(civ,civ2,0x4b);
    }
    popup_game_with_sprite((const char *)&str_reject0,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_treaty_turn[civ] = game.turn;
    goto end_meeting;
  }
  if ((tax_rate_icon_sprites_block.diplo_ai_aggressive != 0) &&
     (((tax_rate_icon_sprites_block.diplo_last_treaty_turn < 0 || (((byte)(civs[civ].treaties[civ2] >> 16) & 1) != 0)) ||
      (player_initiated != 0)))) {
    diplo_ask_receive_emissary(civ2,civ,x,y);
    if (diplo_emissary_asked == 0) {
      diplo_set_attitude_arg(civ2,civ);
    }
    diplo_leader_play_reaction(4);
    diplo_set_treaty_flags(civ,civ2,0x2000);
    advance2 = civ_get_attitude(civ,civ2);
    if (advance2 < 0x4b) {
      civ_set_attitude(civ,civ2,0x4b);
    }
    popup_game_with_sprite((const char *)&str_rid0,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    civs[civ2].last_treaty_turn[civ] = game.turn;
    goto end_meeting;
  }
  if (tax_rate_icon_sprites_block.diplo_ai_aggressive != 0) goto end_meeting;
  if (((tax_rate_icon_sprites_block.diplo_suppressed_aggression != 0) && ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0)) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)) {
    diplo_leader_play_reaction(4);
    advance2 = civ_has_wonder(civ2,6);
    if (advance2 == 0) {
      popup_game((const char *)&str_accursedun);
    }
    else {
      popup_game((const char *)&str_accursedwall);
    }
  }
  if ((((*(byte *)&civs[civ].treaties[civ2] & 4) != 0) &&
      ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0)) && (tax_rate_icon_sprites_block.diplo_target_is_leader == 0)) {
    ally_count = 0;
    for (other = 1; other < 8; other++) {
      if ((*(byte *)&civs[civ2].treaties[other] & 8) != 0) {
        ally_count++;
      }
    }
    if ((game.power_rank[civ2] < 6) && (game.power_rank[civ] < 6)) {
      propose = true;
    }
    else {
      propose = false;
    }
    if (((propose) && (tax_rate_icon_sprites_block.diplo_war_target_civ < 1)) && (200 < game.turn)) {
      best_rank = 8;
      for (other = 1; other < 8; other++) {
        if (((((civ != other) && (civ2 != other)) &&
             ((game.power_rank[civ] <= game.power_rank[other] &&
              ((game.power_rank[civ2] <= game.power_rank[other] &&
               (game.power_rank[other] <= best_rank)))))) &&
            ((*(byte *)&civs[civ2].treaties[other] & 1) != 0)) &&
           ((((byte)(civs[civ2].treaties[other] >> 8) & 0x20) == 0 &&
            ((civs[civ].treaties[other] & 0x2008) == 0)))) {
          tax_rate_icon_sprites_block.diplo_war_target_civ = other;
          best_rank = game.power_rank[other];
        }
      }
    }
    if ((tax_rate_icon_sprites_block.diplo_war_target_civ < 1) ||
       ((*(byte *)&civs[civ2].treaties[tax_rate_icon_sprites_block.diplo_war_target_civ] & 0xe) == 0)) {
      tech_gap = false;
    }
    else {
      tech_gap = true;
    }
    if (((((((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0) || (propose)) || (tech_gap)) && ((1 < tax_rate_icon_sprites_block.diplo_peace_inclination || (propose)))) &&
         ((ally_count == 0 && ((tax_rate_icon_sprites_block.diplo_cur_attitude < 0x19 || ((tax_rate_icon_sprites_block.diplo_cur_attitude < 0x32 && (tech_gap)))))))) &&
        ((tech_gap || ((declared_war || (propose)))))) &&
       (((((int)((uint)civs[civ2].reputation -
                (int)civs[civ].reputation_credit[civ2]) < 4 &&
          (((*(byte *)&civs[civ].treaties[civ2] & 2) == 0 && (tax_rate_icon_sprites_block.diplo_captured_city_count < 2))))
         && ((game.power_rank[civ2] < 6 || (game.power_rank[civ] < 5)))) &&
        ((advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0 &&
         (advance2 = popup_game_with_sprite((const char *)&str_proposealliance,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                     ((game.advance_known_by[0xf] != '\0') - 1 &
                                                     0xffff7c80)),0), advance2 == 0)))))) {
      propose = true;
      if (-1 < tax_rate_icon_sprites_block.diplo_war_target_civ) {
        propose = false;
        str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_war_target_civ));
        text_set_string_arg(4,(const char *)str);
        diplo_leader_play_reaction(3);
        advance2 = popup_game((const char *)&str_perhapssolidarity_3);
        if (advance2 == 1) {
          diplo_declare_war(civ2,tax_rate_icon_sprites_block.diplo_war_target_civ,civ);
          treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + tax_rate_icon_sprites_block.diplo_war_target_civ * 4);
          *treaty = *treaty | 0x80800;
          tax_rate_icon_sprites_block.diplo_war_target_civ = -1;
          propose = true;
        }
        else {
          civ_adjust_attitude(civ,civ2,10);
          diplo_set_attitude_arg(civ2,civ);
          if (((tax_rate_icon_sprites_block.diplo_ai_fear_level == 0) ||
              (2 < (int)((uint)civs[civ2].reputation -
                        (int)civs[civ].reputation_credit[civ2]))) ||
             (0x19 < tax_rate_icon_sprites_block.diplo_cur_attitude)) {
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
  if ((((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) &&
     (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    if (paid) {
      choice = popup_game_with_sprite((const char *)&str_grantcease,(iptr)&ceasefire_icon_sprite,0);
    }
    else if ((tax_rate_icon_sprites_block.diplo_suppressed_aggression == 0) && (hostile == 0)) {
      str = civ_get_adjective(civ);
      text_set_string_arg(1,(const char *)str);
      choice = popup_game_with_sprite((const char *)&str_proposecease,(iptr)&ceasefire_icon_sprite,0);
    }
    else {
      choice = popup_game_with_sprite((const char *)&str_wallcease,(iptr)&ceasefire_icon_sprite,0);
    }
    if (choice != 0) {
      advance2 = civ_has_wonder(civ,6);
      if ((advance2 == 0) && (advance2 = civ_has_wonder(civ,0x18), advance2 == 0)) {
        advance2 = diplo_senate_intervenes(civ2,civ);
        if (advance2 == 0) {
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
        advance2 = civ_has_wonder(civ,6);
        if (advance2 == 0) {
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
    if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
      distrust = distrust * 2;
    }
    if (choice != 0) {
      if (distrust == 1 || distrust - 1 < 0) {
        roll = 0;
      }
      else {
        roll = _rand();
        roll = roll % distrust;
      }
      if (roll == 0) {
        if (((choice != 0) && (3 < tax_rate_icon_sprites_block.diplo_ai_fear_level)) && (-1 < tax_rate_icon_sprites_block.diplo_wanted_tech)) {
          text_set_label_arg(1,advances[tax_rate_icon_sprites_block.diplo_wanted_tech].name);
          diplo_leader_play_reaction(4);
          advance2 = popup_game_with_sprite((const char *)&str_giveciv,(iptr)&ceasefire_icon_sprite,0);
          if (advance2 != 0) {
            choice = 0;
            civ_give_advance(civ2,tax_rate_icon_sprites_block.diplo_wanted_tech,civ,0,0);
            conceded = true;
          }
        }
        if (choice != 0) {
          variant = int_clamp(tax_rate_icon_sprites_block.diplo_ai_fear_level * 2 - 4,0,civs[civ].gold / 0x32);
          variant = variant * 0x32;
          advance2 = civ_has_wonder(civ,6);
          if ((advance2 != 0) || (advance2 = civ_has_wonder(civ,0x18), advance2 != 0)) {
            variant = variant / 2;
          }
          if (variant != 0) {
            text_set_number_arg(0,variant);
            diplo_leader_play_reaction(4);
            advance2 = popup_game_with_sprite((const char *)&str_givecash,(iptr)&ceasefire_icon_sprite,0);
            if (advance2 != 0) {
              civs[civ].gold -= variant;
              civs[civ2].gold += variant;
              choice = 0;
              status_redraw(1);
              conceded = true;
            }
          }
        }
        if (((choice != 0) && (tax_rate_icon_sprites_block.diplo_ai_fear_level != 0)) &&
           ((civs[civ].num_cities < 2 &&
            ((advance2 = civ_has_wonder(civ,6), advance2 == 0 &&
             (advance2 = civ_has_wonder(civ,0x18), advance2 == 0)))))) {
          text_set_number_arg_v(0,civs[civ].gold);
          diplo_leader_play_reaction(4);
          advance2 = popup_game_with_sprite((const char *)&str_grovel,(iptr)&ceasefire_icon_sprite,0);
          if (advance2 != 0) {
            choice = 0;
            civs[civ2].gold += civs[civ].gold;
            civs[civ].gold = 0;
            status_redraw(1);
            conceded = true;
            for (advance = 0; advance < 100; advance++) {
              advance2 = civ_has_advance(civ,advance);
              if (((advance2 != 0) && (advance2 = civ_has_advance(civ2,advance), advance2 == 0)) &&
                 ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) {
                civ_give_advance(civ2,advance,civ,0,0);
              }
            }
          }
        }
        if (choice == 0) {
          tax_rate_icon_sprites_block.diplo_last_treaty_turn = (int)game.turn;
        }
      }
    }
    if (choice != 0) {
      diplo_leader_play_reaction(4);
      popup_game((const char *)&str_continuewar);
      civs[civ2].last_treaty_turn[civ] = game.turn;
      goto end_meeting;
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
  if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
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
    if ((((tax_rate_icon_sprites_block.diplo_suppressed_aggression != 0) ||
         (((paid || (no_contact)) && ((*(byte *)&civs[civ].treaties[civ2] & 2) == 0)))
         ) || ((((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0 ||
                 (advance2 = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude), advance2 <= threshold)) &&
                (tax_rate_icon_sprites_block.diplo_target_is_leader == 0)) &&
               ((int)(((uint)civs[civ2].reputation -
                      (int)civs[civ].reputation_credit[civ2]) + tax_rate_icon_sprites_block.diplo_captured_city_count) <
                tax_rate_icon_sprites_block.diplo_ai_fear_level + 4)))) &&
       (advance2 = diplo_ask_receive_emissary(civ2,civ,x,y), advance2 != 0)) {
      str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)str);
      choice = popup_game_with_sprite((const char *)&str_proposepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
      if (choice != 0) {
        advance2 = civ_has_wonder(civ,6);
        if ((advance2 == 0) && (advance2 = civ_has_wonder(civ,0x18), advance2 == 0)) {
check_senate:
          advance2 = diplo_senate_intervenes(civ2,civ);
          if (advance2 == 0) {
senate_hawks_check:
            if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x40) == 0) {
              diplo_ai_senate_hawks_notice(civ2,civ);
            }
          }
          else {
            if (senate_blocked) {
              rnd = _rand();
              rnd_sign = (int)rnd >> 0x1f;
              if (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign) goto senate_hawks_check;
            }
            popup_game_with_sprite((const char *)&str_overrulepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            choice = 0;
          }
        }
        else {
          if (senate_blocked) {
            rnd = _rand();
            rnd_sign = (int)rnd >> 0x1f;
            if (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign) goto check_senate;
          }
          advance2 = civ_has_wonder(civ,6);
          if (advance2 == 0) {
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
        if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
          distrust = distrust * 2;
        }
        if (distrust == 1 || distrust - 1 < 0) {
          roll2 = 0;
        }
        else {
          roll2 = _rand();
          roll2 = roll2 % distrust;
        }
        if (roll2 == 0) {
          variant = int_clamp(tax_rate_icon_sprites_block.diplo_ai_fear_level * 2 - 4,0,civs[civ].gold / 0x32);
          variant = variant * 0x32;
          advance2 = civ_has_wonder(civ,6);
          if ((advance2 != 0) || (advance2 = civ_has_wonder(civ,0x18), advance2 != 0)) {
            variant = variant / 2;
          }
          if (variant != 0) {
            text_set_number_arg(0,variant);
            advance2 = popup_game_with_sprite((const char *)&str_cashforpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            if (advance2 != 0) {
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
  if (((diplo_emissary_asked != 0) || (player_initiated != 0)) ||
     (((*(byte *)&civs[civ].treaties[civ2] & 4) == 0 &&
      (((tax_rate_icon_sprites_block.diplo_ai_fear_level != 0 || (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0))
       || ((*(byte *)&civs[civ].treaties[civ2] & 1) == 0)))))) {
    if (6 < (int)((uint)civs[civ2].reputation -
                 (int)civs[civ].reputation_credit[civ2])) {
      civ_ally_count = 0;
      for (other = 1; other < 8; other++) {
        if ((*(byte *)&civs[civ].treaties[other] & 8) != 0) {
          civ_ally_count++;
        }
      }
      if (((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) &&
         (((civ_ally_count != 0 || (tax_rate_icon_sprites_block.diplo_target_is_leader != 0)) ||
          (civs[civ2].military_strength < civs[civ].military_strength)))) {
        tax_rate_icon_sprites_block.diplo_ai_declare_war = 1;
      }
    }
    if (no_contact) {
      diplo_ask_receive_emissary(civ2,civ,x,y);
    }
    if (((diplo_emissary_asked == 0) || ((*(uint *)&diplo_emissary_response) == 0xffffffff)) && (player_initiated == 0)) {
      if (tax_rate_icon_sprites_block.diplo_ai_declare_war == 0) {
        advance2 = civ_has_wonder(civ,6);
        if ((advance2 == 0) && (advance2 = civ_has_wonder(civ,0x18), advance2 == 0)) {
          advance2 = diplo_senate_intervenes(civ2,civ);
          if (advance2 == 0) {
            diplo_ai_senate_hawks_notice(civ2,civ);
          }
          else {
            str = civ_get_adjective(civ);
            text_set_string_arg(1,(const char *)str);
            if (((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) &&
               (tax_rate_icon_sprites_block.diplo_ai_fear_level != 0)) {
              popup_game_with_sprite((const char *)&str_senatepeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
              diplo_set_treaty_flags(civ,civ2,4);
              civs[civ2].last_treaty_turn[civ] = game.turn + 0x10;
            }
            if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) {
              popup_game_with_sprite((const char *)&str_senatecease,(iptr)&ceasefire_icon_sprite,0);
              diplo_set_treaty_flags(civ,civ2,2);
              civs[civ2].last_treaty_turn[civ] = game.turn;
            }
          }
        }
        else {
          str = (iptr)(civ_name_plural(civ));
          text_set_string_arg(1,(const char *)str);
          if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
            if (tax_rate_icon_sprites_block.diplo_ai_fear_level != 0) {
              advance2 = civ_has_wonder(civ,6);
              if (advance2 == 0) {
                popup_game_with_sprite((const char *)&str_unoverpeace_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
              }
              else {
                popup_game_with_sprite((const char *)&str_walloverpeace_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
              }
              diplo_set_treaty_flags(civ,civ2,4);
              civs[civ2].last_treaty_turn[civ] = game.turn + 0x10;
            }
            if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) {
              advance2 = civ_has_wonder(civ,6);
              if (advance2 == 0) {
                popup_game_with_sprite((const char *)&str_unovercease_2,(iptr)&ceasefire_icon_sprite,0);
              }
              else {
                popup_game_with_sprite((const char *)&str_wallovercease_2,(iptr)&ceasefire_icon_sprite,0);
              }
              diplo_set_treaty_flags(civ,civ2,2);
              civs[civ2].last_treaty_turn[civ] = game.turn;
            }
          }
        }
      }
    }
    else {
      if (diplo_emissary_asked == 0) {
        diplo_set_attitude_arg(civ2,civ);
      }
      if (treaty_made) {
        if (((civs[civ].treaties[civ2] & 0x810) == 0) || (player_initiated == 0))
        {
          civ3 = -1;
          patience = -1;
          for (other = 1; other < 8; other++) {
            if ((((civ != other) && (civ2 != other)) &&
                (((byte)(civs[civ].treaties[other] >> 8) & 0x20) == 0)) &&
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
            if (((byte)(civs[civ].treaties[civ3] >> 8) & 0x20) == 0) {
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
            popup_game((const char *)&map_views_block.text_buffer);
          }
        }
      }
      else if ((tax_rate_icon_sprites_block.diplo_tribute_demand != 999) || (paid)) {
        if (player_initiated == 0) {
          variant = 0x71;
          if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) {
            if (civs[civ].military_strength < civs[civ2].military_strength) {
              variant = 0x70;
            }
            advance2 = diplo_attitude_level(tax_rate_icon_sprites_block.diplo_cur_attitude);
            if ((advance2 < 4) &&
               ((int)((uint)civs[civ2].reputation -
                     (int)civs[civ].reputation_credit[civ2]) < 3)) {
              variant = 0x70;
            }
            if (tax_rate_icon_sprites_block.diplo_ai_fear_level != 0) {
              variant = 0x6f;
            }
          }
          text_set_label_arg(1,label_string_ids[variant]);
          str = civ_get_adjective(civ2);
          text_set_string_arg(2,(const char *)str);
          str = civ_leader_title(civ2);
          text_set_string_arg(3,(const char *)str);
          str = (iptr)(civ_leader_name(civ2));
          text_set_string_arg(4,(const char *)str);
          diplo_leader_play_reaction(2);
          if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
            if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
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
            if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
              if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
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
            if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
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
        if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & 6) == 0) {
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
      if (((*(byte *)&civs[civ].treaties[civ2] & 0xc) == 0) &&
         (diplo_count_border_intruders(civ,civ2), setup_picwnd_block.diplo_border_intruder_count != 0)) {
        text_set_string_arg(1,(const char *)&cities[setup_picwnd_block.border_intruder_city].name);
        str = civ_get_adjective(civ);
        text_set_string_arg(2,(const char *)str);
        if (setup_picwnd_block.border_intruder_city < 2) {
          popup_game((const char *)&str_pleasecity);
        }
        else {
          popup_game((const char *)&str_pleasecities);
        }
      }
      if ((*(byte *)&civs[civ2].treaties[civ] & 8) != 0) {
        threat_count = 0;
        if (0 < tax_rate_icon_sprites_block.diplo_strongest_enemy) {
          for (continent = 1; continent < 0x3f; continent++) {
            if (((1 < civs[civ].num_cities) < civs[civ].continent_cities[continent])
               && (civs[civ].continent_military[continent] <
                   civs[tax_rate_icon_sprites_block.diplo_strongest_enemy].continent_attack[continent])) {
              threat_count++;
            }
          }
        }
        if (threat_count == 0) {
          if (0 < tax_rate_icon_sprites_block.diplo_weakest_enemy_civ) {
            threat_count = 0;
            for (continent = 1; continent < 0x3f; continent++) {
              if (((1 < civs[tax_rate_icon_sprites_block.diplo_weakest_enemy_civ].num_cities) <
                   civs[tax_rate_icon_sprites_block.diplo_weakest_enemy_civ].continent_cities[continent]) &&
                 (civs[tax_rate_icon_sprites_block.diplo_weakest_enemy_civ].continent_military[continent] <
                  civs[civ].continent_attack[continent])) {
                threat_count++;
              }
            }
            if (threat_count != 0) {
              str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_weakest_enemy_civ));
              text_set_string_arg(1,(const char *)str);
              diplo_leader_play_reaction(3);
              popup_game((const char *)&str_allybrag);
            }
          }
        }
        else {
          str = (iptr)(civ_name_plural(tax_rate_icon_sprites_block.diplo_strongest_enemy));
          text_set_string_arg(1,(const char *)str);
          diplo_leader_play_reaction(2);
          popup_game((const char *)&str_allyplea);
        }
      }
      do {
        if (civs[civ].diplo_request_count < '\0') {
          civs[civ].diplo_request_count = '\0';
        }
        advance2 = diplo_proposal_menu(civ2,civ);
        if (advance2 == 0) {
          ended = true;
          goto end_meeting;
        }
        if (advance2 == 7) {
          diplo_favor_menu(civ2,civ);
        }
        else if (advance2 == 8) {
          diplo_gift_menu(civ2,civ);
        }
        else {
          advance2 = diplo_ai_answer_proposal(civ2,civ,advance2);
          if (advance2 != 0) goto end_meeting;
        }
        civs[civ].diplo_request_count++;
        patience = civs[civ].diplo_request_count;
        advance2 = diplo_patience_limit(civ2,civ);
      } while (patience < advance2);
      diplo_set_attitude_arg(civ2,civ);
      diplo_leader_play_reaction(3);
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        popup_game((const char *)&str_patience);
      }
      else {
        popup_game((const char *)&str_patienceally);
      }
    }
  }
end_meeting:
  diplo_clear_treaty_flags(civ,civ2,0x10000);
  if ((tax_rate_icon_sprites_block.diplo_ai_declare_war == 0) || ((civs[civ2].treaties[civ] & 0x2008) != 0))
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
  if ((((*(uint *)&tax_rate_icon_sprites_block.diplo_had_ceasefire) != 0) && ((*(byte *)&civs[civ].treaties[civ2] & 2) == 0)) &&
     ((civs[civ2].treaties[civ] & 0x200c) == 0)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    popup_game_with_sprite((const char *)&str_ceaseexpire,(iptr)&ceasefire_icon_sprite,0);
  }
  if ((((game.difficulty == '\0') &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0)) &&
      (((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) == 0 &&
       ((advance2 = civ_has_wonder(human_civ[0],0x18), advance2 == 0 &&
        (advance2 = civ_has_wonder(human_civ[0],9), advance2 == 0)))))) &&
     (advance2 = civ_has_advance(civ,0x58), advance2 != 0)) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)str);
    dlg_game_txt_with_unit_flags((const char *)&str_ambassadors,0x2e,0);
    treaty = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *treaty = *treaty | 0x80;
    intel_report_show(civ2,civ);
  }
  if (((((*((char *)&game.game_options + 1)) & 1) != 0) && (emissary_sent)) &&
     (((diplo_emissary_asked != 0 || (player_initiated != 0)) &&
      (((((*(uint *)&diplo_emissary_response) != 0xffffffff && (4 < civs[civ].num_cities)) &&
        (game.power_rank[civ2] + 1 < (uint)game.power_rank[civ])) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0)))))) {
    str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)str);
    popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_schism);
  }
  diplo_ai_meeting_active_i32 = 0;
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
  
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
      if (((byte)*(i32 *)&civs[civ].treaties[civ2] & 5) == 1) {
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
            (unit_types[units[unit].type].role != 7)))) {
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
              map_cursor_x[0] = cities[city2].x;
              map_cursor_y[0] = cities[city2].y;
            }
            units[unit].orders = -1;
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
  if (((human_civ[0] == civ) || (human_civ[0] == civ2)) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
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
    popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
  }
  else {
    if ((((((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) == 0) &&
          (has_wonder = civ_has_wonder(human_civ[0],0x18), has_wonder == 0)) &&
         ((has_wonder = civ_has_wonder(human_civ[0],9), has_wonder == 0 &&
          (((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) == 0 &&
           (has_wonder = civ_has_wonder(human_civ[0],0x18), has_wonder == 0)))))) &&
        (has_wonder = civ_has_wonder(human_civ[0],9), has_wonder == 0)) && (game.reveal_map == '\0')) {
      return;
    }
    if (civ_saved_cursor_x_block.autoplay_active == 0) {
      name2 = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)name2);
      name2 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(2,(const char *)name2);
      text_append((const char *)&str_1_3);
      popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    }
  }
  return;
}

/* 00493e50 auto */
void __cdecl diplo_greeting_sound_done_cb(void)

{
  if (diplo_leader_wnd_open != '\0') {
    snd_play_effect(diplo_leader_sound_ids[0],1,1,0);
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
  i32 seh_handler;
  i8 *seh_prev;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  diplo_leader_wnd_open = 0;
  diplo_greeting_sound_id = ((uint)map.seed + civ & 7) + 0x53;
  if (civs[civ].tribe < 1) {
    tribe_idx = ~(int)civs[civ].tribe + 1;
  }
  else {
    tribe_idx = (iptr)civs[civ].tribe;
  }
  diplo_leader_sound_ids[0] = (uint)(byte)(((i8 *)&tribe_leader_sound_set))[tribe_idx] * 4 + 0x70;
  for (i = 0; i < 3; i++) {
    diplo_leader_reaction_sound_ids[i] = diplo_leader_sound_ids[0] + i + 1;
  }
  snd_cd_stop();
  if (diplo_leader_window != 0) {
    if (diplo_leader_window != 0) {
      diplo_leader_wnd_scalar_dtor((void *)diplo_leader_window,1);
    }
    diplo_leader_window = 0;
  }
  self = (DiploLeaderWindow *)(operator_new(0x108c));
  seh_state = 0;
  if (self == (DiploLeaderWindow *)0x0) {
    view = (DiploLeaderWindow *)0x0;
  }
  else {
    view = diplo_leader_wnd_ctor(self);
  }
  seh_state = 0xffffffff;
  diplo_leader_window = view;
  if (((view != (DiploLeaderWindow *)0x0) && (((*((char *)&game.game_options + 2)) & 4) != 0)) &&
     (ok = diplo_leader_wnd_init(view,civ2,civ), ok != 0)) {
    diplo_leader_wnd_open = 1;
    if ((iptr)diplo_leader_window == -200) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)diplo_leader_window + 0x110;
    }
    dlg_set_parent_window(parent_wnd2);
    dlg_set_palette((iptr)diplo_leader_window + 0x224);
    diplo_leader_wnd_show(diplo_leader_window);
  }
  return;
}

/* 00494148 auto */
void __cdecl diplo_leader_wnd_close(void)

{
  if (diplo_leader_wnd_open != '\0') {
    diplo_leader_wnd_open = '\0';
    if (diplo_leader_window != 0) {
      diplo_leader_wnd_teardown(diplo_leader_window);
      if (diplo_leader_window != 0) {
        diplo_leader_wnd_scalar_dtor((void *)diplo_leader_window,1);
      }
      diplo_leader_window = 0;
    }
    diplo_greeting_sound_id = 0;
    ui_delay_ticks(0x1e);
    snd_play_effect(-diplo_leader_sound_ids[0],0,0,0);
    snd_sync_music_option();
  }
  return;
}

/* 004941ee auto */
void __cdecl diplo_leader_play_reaction(int reaction)

{
  if ((((diplo_leader_video_active != '\0') && (diplo_leader_window != 0)) &&
      ((reaction == 2 || ((reaction == 3 || (reaction == 4)))))) &&
     ((leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader_window->leader_tribe][1][0] <
       diplo_leader_window->video_cur_frame &&
      (diplo_leader_window->video_cur_frame <
       leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader_window->leader_tribe][1][1])))) {
    diplo_leader_window->pending_flag_1088 = (i8)reaction;
    diplo_leader_video_end_cb();
  }
  return;
}

/* 004942a3 auto */
DiploLeaderWindow * __thiscall diplo_leader_wnd_ctor(DiploLeaderWindow *self)

{
  i32 eh_frame;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  wnd_ctor(self);
  eh_state = 0;
  surfwnd_ctor((SurfaceWindow *)self->leader_surface_window);
  (*((char *)&eh_state + 0)) = 1;
  surface_ctor((DibSurface *)&self->frame_right);
  (*((char *)&eh_state + 0)) = 2;
  palette_init((GamePalette *)self->leader_palette);
  (*((char *)&eh_state + 0)) = 3;
  dll_handle_init((DllHandle *)self->leader_dll);
  (*((char *)&eh_state + 0)) = 4;
  mmwnd_ctor((AviPlayer *)self->leader_video_wnd);
  eh_state = (eh_state & 0xffffff00 | (byte)(5));
  gfx_get_screen_rect((iptr)&self->control_list);
  self->pending_flag_1088 = '\0';
  return self;
}

/* 004943c9 auto */
void __thiscall diplo_leader_wnd_dtor(void *self)

{
  i32 eh_handler;
  i8 *eh_prev;
  int eh_state;
  
  eh_state = eh_state & 0xff;
  diplo_leader_video_active = 0;
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0049444f */ { avi_player_dtor((AviPlayer *)(*(int *)&self + 0x65c)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0049445e */ { dll_free((DllHandle *)(*(int *)&self + 0x658)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0049446d */ { palette_destroy((GamePalette *)(*(int *)&self + 0x224)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0049447c */ { surface_dtor((DibSurface *)(*(int *)&self + 0x1dc)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0049448b */ { surfwnd_dtor((SurfaceWindow *)(*(int *)&self + 200)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0049449a */ { wnd_dtor(*(WndBase **)&self); }
  return;
}

/* 004944bb auto */
i32 __thiscall diplo_leader_wnd_init(DiploLeaderWindow *self,i32 viewer_civ,iptr leader_civ)

{
  int ok;
  i32 result;
  i8 saved_rgb2[96];
  i8 dll_path[128];
  
  diplo_leader_video_active = 0;
  palette_get_rgb((GamePalette *)((i8 *)&game_palette_block.game_palette),(iptr)saved_rgb2,10,0x20);
  palette_set_rgb((GamePalette *)self->leader_palette,10,0x20,(iptr)saved_rgb2);
  self->viewer_civ = (u8)viewer_civ;
  self->leader_civ = (u8)leader_civ;
  self->leader_tribe = (u8)civs[self->leader_civ].tribe;
  str_clear((char *)dll_path);
  str_append((char *)dll_path,(const char *)&str_civ2_mk_dll);
  ok = file_resolve_path((const char *)dll_path);
  if (ok == 0) {
    result = 0;
  }
  else {
    dll_handle_load((DllHandle *)self->leader_dll,(const char *)dll_path);
    result = civ_city_style_era((uint)self->leader_civ);
    self->style_era = (u8)result;
    ok = diplo_leader_init_tile(self);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_init_tile_failed,str_notice_3,0x40);
      result = 0;
    }
    else {
      ok = diplo_leader_init_background(self);
      if (ok == 0) {
        MessageBoxA((HWND)0x0,str_init_background_failed,str_notice_4,0x40);
        result = 0;
      }
      else {
        ok = diplo_leader_init_main_art(self);
        if (ok == 0) {
          MessageBoxA((HWND)0x0,str_init_main_art_failed,str_notice_5,0x40);
          result = 0;
        }
        else {
          ok = diplo_leader_init_military(self);
          if (ok == 0) {
            MessageBoxA((HWND)0x0,str_init_military_failed,str_notice_6,0x40);
            result = 0;
          }
          else {
            ok = diplo_leader_init_science(self);
            if (ok == 0) {
              MessageBoxA((HWND)0x0,str_init_science_failed,str_notice_7,0x40);
              result = 0;
            }
            else {
              ok = diplo_leader_init_portrait(self);
              if (ok == 0) {
                MessageBoxA((HWND)0x0,str_init_portrait_failed,str_notice_8,0x40);
                result = 0;
              }
              else {
                if (((*((char *)&game.game_options + 2)) & 0x20) != 0) {
                  diplo_leader_video_active = diplo_leader_init_video(self);
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
void __thiscall diplo_leader_wnd_show(DiploLeaderWindow *self)

{
  wnd_handle_show_raise(self);
  wnd_flush_paint();
  wnd_set_palette(self,(iptr)(self->leader_palette));
  wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)(self->leader_palette));
  if (diplo_leader_video_active != '\0') {
    window_show((SurfaceWindow *)self->leader_video_wnd);
  }
  window_show((SurfaceWindow *)self->leader_surface_window);
  wnd_flush_paint();
  if (diplo_leader_video_active != '\0') {
    avi_player_play_range
              ((AviPlayer *)self->leader_video_wnd,
               leader_anim_frame_starts_block.leader_anim_frames[(uint)self->leader_tribe][0][0],
               leader_anim_frame_starts_block.leader_anim_frames[(uint)self->leader_tribe][0][1]);
  }
  dll_handle_free((DllHandle *)self->leader_dll);
  diplo_leader_wnd_busy = 0;
  snd_play_effect(diplo_greeting_sound_id,1,0,(SoundDoneFn)diplo_greeting_sound_done_cb);
  return;
}

/* 004947f0 auto */
void __thiscall diplo_leader_wnd_teardown(DiploLeaderWindow *self)

{
  wnd_hide((WndBase *)((u8 *)self + 0x110));
  wnd_flush_paint();
  wnd_set_palette(self,(iptr)&game_palette_block.game_palette);
  wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)&game_palette_block.game_palette);
  wnd_hide(self);
  wnd_flush_paint();
  wnd_destroy_native(self);
  dll_handle_free((DllHandle *)&self->leader_dll);
  dlg_set_parent_window(0);
  dlg_set_palette(0);
  wnd_destroy_with_surface((DibSurface *)self->leader_surface_window);
  wnd_destroy_native(self);
  dialog_set_default_offset(0,0);
  return;
}

/* 0049488e auto */
uint __thiscall diplo_leader_init_tile(DiploLeaderWindow *self)

{
  int loaded;
  
  surface_create((DibSurface *)&self->bg_tile_surface,0x40,0x40);
  loaded = surface_load_gif_resource((DibSurface *)&self->bg_tile_surface,299,10,0x20,(iptr)&game_palette_block.game_palette);
  return (uint)(loaded != 0);
}

/* 004948e6 auto */
i32 __thiscall diplo_leader_init_background(DiploLeaderWindow *self)

{
  wnd_create_with_handlers
            (self,(const char *)&str_empty_102,0x800,0,0,self->screen_width_value,self->screen_height_value + 5,(iptr)&game_palette_block.game_palette,
             (iptr)&main_frame_wnd_block.main_frame_wnd);
  wnd_set_bg_tile(self,(DibSurface *)&self->bg_tile_surface);
  return 1;
}

/* 00494949 auto */
uint __thiscall diplo_leader_init_main_art(DiploLeaderWindow *self)

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
  i32 eh_handler;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  palette_init(&palette);
  eh_state = 0;
  wnd_x = (int)(self->screen_width_value - 0x280) / 3;
  wnd_y = (int)(self->screen_height_value - 0x1e0) / 3;
  wnd_create_with_surface
            ((DibSurface *)self->leader_surface,(const char *)&str_empty_103,0x800,wnd_x,wnd_y,0x280,0x1e0,
             (iptr)(self->leader_palette),(iptr)self);
  loaded = surface_load_gif_resource
                    ((DibSurface *)self->leader_surface,civs[self->leader_civ].government + 200,10,0xec,
                     (iptr)&palette);
  if (loaded != 0) {
    blue_ptr = &blue;
    green_ptr = &green;
    red_ptr = &red;
    color_index = civ_color_primary((uint)self->leader_civ);
    palette_get_entry
              ((GamePalette *)((i8 *)&game_palette_block.game_palette),color_index,(iptr)red_ptr,(iptr)green_ptr,(iptr)blue_ptr);
    palette_set_entry(&palette,0x91,red,green,blue);
    palette_get_rgb(&palette,(iptr)rgb_buf,0x6a,0x28);
    palette_set_rgb((GamePalette *)self->leader_palette,0x6a,0x28,(iptr)rgb_buf)
    ;
    surface_set_palette((DibSurface *)self->leader_surface,(iptr)(self->leader_palette));
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00494b3a */ { palette_destroy(&palette); }
  }
  else {
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00494b3a */ { palette_destroy(&palette); }
  }
  return (uint)(loaded != 0);
}

/* 00494b5f auto */
i32 __thiscall diplo_leader_init_military(DiploLeaderWindow *self)

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
  i32 eh_handler;
  i8 *eh_prev;
  int eh_state;
  
  eh_state = 0xffffffff;
  sprite_init(&sprite);
  eh_state = 0;
  palette_init(&palette);
  (*((char *)&eh_state + 0)) = 1;
  surface_ctor(&surface);
  (*((char *)&eh_state + 0)) = 2;
  icon_count = game.power_rank[self->leader_civ] - 1;
  if (icon_count == 0) {
    result = 1;
    (*((char *)&eh_state + 0)) = 1;
    /* inlined unwind funclet 00494d71 */ { surface_dtor(&surface); }
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 00494d7d */ { palette_destroy(&palette); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00494d89 */ { sprite_destroy(&sprite); }
  }
  else {
    loaded = surface_load_gif_resource(&surface,0xd7,10,0xec,(iptr)&palette);
    if (loaded == 0) {
      fail_result = 0;
      (*((char *)&eh_state + 0)) = 1;
      /* inlined unwind funclet 00494d71 */ { surface_dtor(&surface); }
      eh_state = eh_state & 0xffffff00;
      /* inlined unwind funclet 00494d7d */ { palette_destroy(&palette); }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00494d89 */ { sprite_destroy(&sprite); }
      result = fail_result;
    }
    else {
      draw_x = (uint)self->style_era * 0x6d + 1;
      draw_y = 0x9c;
      sprite_grab_from_surface(&sprite,(iptr)&surface,9,draw_x,0x9c,0x6c,0x1e);
      draw_x = 0x2d;
      draw_y = 0x82;
      step_y = 0x1f;
      for (i = 0; i < icon_count; i++) {
        sprite_draw
                  (&sprite,(iptr)draw_ctx,(iptr)self->leader_surface,draw_x,draw_y);
        draw_y += step_y;
      }
      (*((char *)&eh_state + 0)) = 1;
      /* inlined unwind funclet 00494d71 */ { surface_dtor(&surface); }
      eh_state = eh_state & 0xffffff00;
      /* inlined unwind funclet 00494d7d */ { palette_destroy(&palette); }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00494d89 */ { sprite_destroy(&sprite); }
      result = 1;
    }
  }
  return result;
}

/* 00494e2a auto */
i32 __thiscall diplo_leader_init_science(DiploLeaderWindow *self)

{
  int value;
  iptr label;
  int rect_height2;
  CHAR aCStack_641 [53];
  i8 draw_ctx[16];
  i32 fail_result;
  i32 result;
  int swap_depth;
  int swap_tech;
  int truncated;
  CHAR words[252];
  iptr advance;
  int tech_count;
  int slot;
  int step_y;
  int word2;
  int draw_y;
  int adv_category;
  tagRECT draw_rect;
  int j;
  GameFont font2;
  i32 draw_x;
  CHAR line2_buf[51];
  char char_pos;
  int depth;
  Sprite category_sprites[5];
  DibSurface surface;
  int top_techs[13];
  GamePalette palette;
  int text_width;
  tagRECT text_rect;
  i8 text_colors[4];
  i32 eh_handler;
  i8 *eh_prev;
  int eh_state;
  
  eh_state = 0xffffffff;
  font_init(&font2,0,0xb);
  eh_state = 0;
  crt_vector_ctor_iterator(category_sprites,0x3c,5,sprite_init,sprite_destroy);
  (*((char *)&eh_state + 0)) = 1;
  palette_init(&palette);
  (*((char *)&eh_state + 0)) = 2;
  surface_ctor(&surface);
  eh_state = (eh_state & 0xffffff00 | (byte)(3));
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
  if (((((*(byte *)&civs[human_civ[0]].treaties[((uint)self->leader_civ)] & 0x80) == 0) &&
       (value = civ_has_wonder(human_civ[0],0x18), value == 0)) &&
      (value = civ_has_wonder(human_civ[0],9), value == 0)) && (game.difficulty != '\0')) {
    top_techs[0xc] = 0;
  }
  else {
    top_techs[0xc] = 1;
  }
  for (advance = 0; advance < 100; advance++) {
    value = civ_has_advance((uint)self->leader_civ,advance);
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
  if (6 < tech_count) {
    tech_count = 6;
  }
  if (tech_count == 0) {
    result = 1;
    (*((char *)&eh_state + 0)) = 2;
    /* inlined unwind funclet 00495be2 */ { surface_dtor(&surface); }
    (*((char *)&eh_state + 0)) = 1;
    /* inlined unwind funclet 00495bee */ { palette_destroy(&palette); }
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 00495bfa */ { crt_vector_dtor_iterator(category_sprites,0x3c,5,sprite_destroy); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00495c10 */ { font_destroy(&font2); }
  }
  else {
    value = surface_load_gif_resource(&surface,0xd7,10,0xec,(iptr)&palette);
    if (value == 0) {
      fail_result = 0;
      (*((char *)&eh_state + 0)) = 2;
      /* inlined unwind funclet 00495be2 */ { surface_dtor(&surface); }
      (*((char *)&eh_state + 0)) = 1;
      /* inlined unwind funclet 00495bee */ { palette_destroy(&palette); }
      eh_state = eh_state & 0xffffff00;
      /* inlined unwind funclet 00495bfa */ { crt_vector_dtor_iterator(category_sprites,0x3c,5,sprite_destroy); }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00495c10 */ { font_destroy(&font2); }
      result = fail_result;
    }
    else {
      surface_set_palette(&surface,(iptr)(self->leader_palette));
      draw_x = (uint)self->style_era * 0x6d + 1;
      draw_y = 1;
      sprite_grab_from_surface(category_sprites + 3,(iptr)&surface,9,draw_x,1,0x6c,0x1e);
      draw_y += 0x1f;
      sprite_grab_from_surface(category_sprites,(iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      sprite_grab_from_surface(category_sprites + 1,(iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      sprite_grab_from_surface(category_sprites + 2,(iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      draw_y += 0x1f;
      sprite_grab_from_surface((category_sprites + 4),(iptr)&surface,9,draw_x,draw_y,0x6c,0x1e);
      step_y = 0x1f;
      draw_x = 0x1e5;
      draw_y = 0x82;
      SetRect(&text_rect,0,0,0x3a,0x14);
      OffsetRect(&text_rect,0x213,0x89);
      text_width = text_rect.right - text_rect.left;
      for (slot = 5; -1 < slot; slot--) {
        advance = top_techs[slot * 2];
        if (-1 < advance) {
          adv_category = (int)(char)advances[advance].category;
          sprite_draw
                    (category_sprites + adv_category,(iptr)draw_ctx,(iptr)self->leader_surface,draw_x,draw_y);
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
            value = font_text_width(&font2,(const char *)(aCStack_641 + 1));
            if (text_width < value) break;
          }
          if ((words[j * 0x32] == '\0') || (word2 == 2)) {
            if (word2 == 2) {
              truncated = 0;
              value = lstrlenA(aCStack_641 + 1);
              char_pos = (char)value;
              while ((char_pos != '\0' &&
                     (value = font_text_width(&font2,(const char *)(aCStack_641 + 1)),
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
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)(aCStack_641 + 1),
                         &draw_rect,0);
              OffsetRect(&draw_rect,-1,-1);
              gfx_set_text_color_index(text_colors[self->style_era]);
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)(aCStack_641 + 1),
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
                     (value = font_text_width(&font2,(const char *)(aCStack_641 + 1)),
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
              value = font_cached_height(&font2);
              OffsetRect(&draw_rect,0,((rect_height2 >> 1) - value) + 1);
              gfx_set_text_color_index(10);
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)(aCStack_641 + 1),
                         &draw_rect,1);
              OffsetRect(&draw_rect,-1,-1);
              gfx_set_text_color_index(text_colors[self->style_era]);
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)(aCStack_641 + 1),
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
                   (value = font_text_width(&font2,(const char *)line2_buf),
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
              value = font_cached_height(&font2);
              OffsetRect(&draw_rect,0,value - 3);
              gfx_set_text_color_index(10);
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)line2_buf,
                         &draw_rect,1);
              OffsetRect(&draw_rect,-1,-1);
              gfx_set_text_color_index(text_colors[self->style_era]);
              surface_draw_text_in_rect
                        ((DibSurface *)self->leader_surface,(iptr)&font2,(const char *)line2_buf,
                         &draw_rect,1);
            }
          }
          OffsetRect(&text_rect,0,step_y);
          draw_y += step_y;
        }
      }
      (*((char *)&eh_state + 0)) = 2;
      /* inlined unwind funclet 00495be2 */ { surface_dtor(&surface); }
      (*((char *)&eh_state + 0)) = 1;
      /* inlined unwind funclet 00495bee */ { palette_destroy(&palette); }
      eh_state = eh_state & 0xffffff00;
      /* inlined unwind funclet 00495bfa */ { crt_vector_dtor_iterator(category_sprites,0x3c,5,sprite_destroy); }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00495c10 */ { font_destroy(&font2); }
      result = 1;
    }
  }
  return result;
}

/* 00495c35 auto */
i32 __thiscall diplo_leader_init_portrait(DiploLeaderWindow *self)

{
  int loaded;
  i8 draw_ctx[16];
  i32 result;
  i8 rgb_buf[192];
  Sprite sprite;
  DibSurface surface;
  GamePalette palette;
  i32 eh_handler;
  i8 *eh_prev;
  int eh_state;
  
  eh_state = 0xffffffff;
  sprite_init(&sprite);
  eh_state = 0;
  palette_init(&palette);
  (*((char *)&eh_state + 0)) = 1;
  surface_ctor(&surface);
  (*((char *)&eh_state + 0)) = 2;
  loaded = surface_load_gif_resource
                    (&surface,civs[self->leader_civ].leader_portrait + 0xdc,10,0xec,
                     (iptr)&palette);
  if (loaded == 0) {
    result = 0;
    (*((char *)&eh_state + 0)) = 1;
    /* inlined unwind funclet 00495dcf */ { surface_dtor(&surface); }
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 00495ddb */ { palette_destroy(&palette); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00495de7 */ { sprite_destroy(&sprite); }
  }
  else {
    palette_get_rgb(&palette,(iptr)rgb_buf,0x2a,0x40);
    palette_set_rgb((GamePalette *)self->leader_palette,0x2a,0x40,(iptr)rgb_buf)
    ;
    surface_set_palette(&surface,(iptr)(self->leader_palette));
    sprite_grab_from_surface(&sprite,(iptr)&surface,9,1,1,0xe1,0x113);
    sprite_draw(&sprite,(iptr)draw_ctx,(iptr)self->leader_surface,0xd0,0x39);
    (*((char *)&eh_state + 0)) = 1;
    /* inlined unwind funclet 00495dcf */ { surface_dtor(&surface); }
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 00495ddb */ { palette_destroy(&palette); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00495de7 */ { sprite_destroy(&sprite); }
    result = 1;
  }
  return result;
}

/* 00495e0c auto */
i8 __thiscall diplo_leader_init_video(DiploLeaderWindow *self)

{
  i8 result;
  int found;
  i8 file_stem[20];
  i8 rgb_buf[300];
  i8 video_path[128];
  GamePalette palette;
  int open_err;
  i32 eh_handler;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  palette_init(&palette);
  eh_state = 0;
  self->leader_tribe = (u8)civs[self->leader_civ].tribe;
  str_clear((char *)video_path);
  str_append((char *)video_path,(const char *)&str_civ2_kings);
  str_clear((char *)file_stem);
  str_append((char *)file_stem,(const char *)&str_hrld);
  str_append((char *)file_stem,(const char *)(((i8 * *)&leader_video_tribe_codes))[civs[self->leader_civ].tribe]);
  file_stem[7] = 0;
  str_append((char *)video_path,(const char *)file_stem);
  str_append((char *)video_path,(const char *)&str_avi);
  found = file_resolve_path((const char *)video_path);
  if (found == 0) {
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00496100 */ { palette_destroy(&palette); }
    result = 0;
  }
  else {
    avi_player_create_child
              ((AviPlayer *)self->video_player,(iptr)&str_empty_108,0x200,
               leader_video_x_table_block.leader_video_pos[(uint)self->leader_tribe][0],
               leader_video_x_table_block.leader_video_pos[(uint)self->leader_tribe][1],(iptr)self->leader_surface);
    open_err = avi_player_open((AviPlayer *)self->video_player,(const char *)video_path);
    if (open_err == 0) {
      avi_player_set_end_callback((AviPlayer *)self->video_player,diplo_leader_video_end_cb);
      avi_player_set_transparent((AviPlayer *)self->video_player,1);
      palette_load_ctab_res(&palette,self->leader_tribe + 1000);
      palette_get_rgb(&palette,(iptr)rgb_buf,0x92,100);
      palette_set_rgb
                ((GamePalette *)self->leader_palette,0x92,100,(iptr)rgb_buf);
      avi_player_set_palette
                ((DibSurface *)self->video_player,(iptr)(self->leader_palette));
      wnd_set_palette
                ((WndBase *)self->video_window,(iptr)(self->leader_palette));
      avi_player_show_frame(self->video_player,0);
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00496100 */ { palette_destroy(&palette); }
      result = 1;
    }
    else {
      if (open_err == -0x7ffbfeac) {
        popup_game((const char *)&str_vfwnotregistered_4);
      }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00496100 */ { palette_destroy(&palette); }
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
  
  wnd = diplo_leader_window;
  if (diplo_leader_video_active != '\0') {
    if ((char)diplo_leader_window->pending_flag_1088 == '\0') {
      avi_player_loop_range
                ((AviPlayer *)((iptr)diplo_leader_window + 0x65c),
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader_window->leader_tribe][1][0],
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)diplo_leader_window->leader_tribe][1][1]);
    }
    else {
      reaction = (uptr)diplo_leader_window->pending_flag_1088;
      diplo_leader_window->pending_flag_1088 = 0;
      avi_player_play_range
                ((AviPlayer *)((iptr)wnd + 0x65c),
                 leader_anim_frame_starts_block.leader_anim_frames[(uint)wnd->leader_tribe][reaction][0]
                 ,leader_anim_frame_starts_block.leader_anim_frames[(uint)wnd->leader_tribe][reaction][1]);
      snd_play_effect((((i32 *)&diplo_leader_sound_ids))[reaction],1,0,0);
    }
  }
  return;
}

/* 00497bf0 auto */
void * __thiscall diplo_leader_wnd_scalar_dtor(void *self,uint flags)

{
  diplo_leader_wnd_dtor(self);
  if ((flags & 1) != 0) {
    operator_delete(self);
  }
  return self;
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
  wnd_activate((Wnd *)(wnd2 + 0x48));
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
    if ((((int)parley_wnd_block.other_civ == -1) || (net_session_block.nm_parley_cancel_count != 0)) || (net_session_block.nm_parley_accept_offer_count != 0)) {
      if ((((int)parley_wnd_block.other_civ != -1) && (net_session_block.nm_parley_cancel_count != 0)) && (net_session_block.nm_parley_accept_offer_count == 0)) {
        net_session_block.nm_parley_cancel_count = 0;
        civ_name = (iptr)(civ_name_plural((int)parley_wnd_block.other_civ));
        text_set_string_arg(0,(const char *)civ_name);
        dlg_pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleycancel_3,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else {
      net_send_message(0x81,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0
                         ,0,0,0,0,0,0,0);
    }
    mp_meeting_pending = 0;
  }
  wnd2->closed = 1;
  wnd2->other_civ = 0xffffffff;
  wnd_hide((WndBase *)((i8 *)wnd2 + 0x48));
  wnd_focus_and_raise(&main_frame_wnd_block.main_frame_wnd);
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
  
  if (((byte)(civs[civ2].treaties[civ] >> 8) & 0x20) == 0) {
    if ((*(byte *)&civs[civ2].treaties[civ] & 0xe) == 0) {
      civ_adjust_attitude(civ2,civ,0x14);
    }
    else {
      civ_name = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_name);
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)civ_name);
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        if (civ_saved_cursor_x_block.autoplay_active != 0) {
          return;
        }
        if (civs[civ].government == 4) {
          return;
        }
        if ((game.multiplayer < 3) || (human_civ[0] == civ2)) {
          if ((*(byte *)&civs[civ2].treaties[civ] & 8) == 0) {
            choice = popup_game_with_sprite((const char *)&str_pretext_2,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
            if (choice == 1) {
              diplo_set_treaty_flags(civ,civ2,0x2000);
              civs[civ2].last_treaty_turn[civ] = game.turn;
            }
          }
          else {
            choice = popup_game_with_sprite((const char *)&str_pretextallied_2,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                      ((game.advance_known_by[0xf] != '\0') - 1 &
                                                      0xffff7c80)),0);
            if (choice == 1) {
              diplo_clear_treaty_flags(civ,civ2,8);
              civs[civ2].last_treaty_turn[civ] = game.turn;
            }
          }
        }
        else {
          net_send_state_diff(0xff,2,0,0,0);
          net_send_message(0xa4,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,
                             civ,civ2,0,0,0,0,0,0);
        }
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) {
        if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
          if (civs[civ].government == 4) {
            contact_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
            *contact_flags = *contact_flags | 0x10;
            civ_adjust_attitude(civ2,civ,10);
            if (civ_saved_cursor_x_block.autoplay_active == 0) {
              civ_name = civ_get_adjective(civ);
              text_set_string_arg(0,(const char *)civ_name);
              popup_game_with_sprite((const char *)&str_incidentterror,(iptr)&government_sprites_block.government_sprite_fundamentalism,0);
            }
          }
          else {
            if (civ_saved_cursor_x_block.autoplay_active == 0) {
              popup_game_with_sprite((const char *)&str_incidentwar,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
            }
            diplo_declare_war(civ,civ2,-1);
          }
        }
        else {
          if ((spy_last_mission == 2) && (civs[civ].num_advances < civs[civ2].num_advances)) {
            if (civ_saved_cursor_x_block.autoplay_active != 0) {
              return;
            }
            diplo_eval_ai_stance(civ,civ2);
            diplo_set_attitude_arg(civ,civ2);
            choice = _rand();
            civ_adjust_attitude(civ2,civ,choice % 0xf + 5);
            popup_game_with_sprite((const char *)&str_wimpout,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[0xf] != '\0') - 1 & 0xffff7c80
                                              )),0);
            return;
          }
          if (civ_saved_cursor_x_block.autoplay_active == 0) {
            popup_game_with_sprite((const char *)&str_incidentallied,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[0xf] != '\0') - 1 & 0xffff7c80
                                              )),0);
          }
          diplo_declare_war(civ,civ2,-1);
        }
      }
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        if (civs[civ].government != 6) {
          roll = _rand();
          sign = (int)roll >> 0x1f;
          if (((roll ^ sign) - sign & 1 ^ sign) == sign) {
            return;
          }
          if (civs[civ].government != 5) {
            return;
          }
        }
        if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 1) == 0)) {
          if (civ_saved_cursor_x_block.autoplay_active == 0) {
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
  
  setup_picwnd_block.border_intruder_city = -1;
  diplo_border_intruded_cities = 0;
  setup_picwnd_block.diplo_border_intruder_units = 0;
  setup_picwnd_block.diplo_border_intruder_count = 0;
  setup_picwnd_block.diplo_border_repeat_offenses = 0;
  if ((*(byte *)&civs[civ].treaties[intruder_civ] & 8) == 0) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        intruders = 0;
        tile = 0;
tile_loop:
        if (tile < 0x14) {
          x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile]);
          y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile];
          check = map_in_bounds(x,y);
          if ((((check == 0) || (check = tile_is_ocean(x,y), check != 0)) ||
              (check = tile_city_owner(x,y), -1 < check)) ||
             (check = tile_unit_owner(x,y), check != intruder_civ)) goto next_tile;
          improvements2 = tile_improvements(x,y);
          for (unit = tile_first_unit(x,y); -1 < unit;
              unit = unit_next_in_stack(unit)) {
            if (((char)unit_types[units[unit].type].role < 5) &&
               ((units[unit].flags & 4) == 0)) {
              if (city != setup_picwnd_block.border_intruder_city) {
                diplo_border_intruded_cities++;
              }
              setup_picwnd_block.border_intruder_city = city;
              if ((units[unit].flags & 0x20) == 0) {
                units[unit].flags = units[unit].flags | 0x20;
              }
              else {
                setup_picwnd_block.diplo_border_repeat_offenses++;
              }
              units[unit].flags = units[unit].flags | 4;
              y = setup_picwnd_block.diplo_border_intruder_count;
              intruders++;
              setup_picwnd_block.diplo_border_intruder_units++;
              setup_picwnd_block.diplo_border_intruder_count++;
              if ((improvements2 & 0x10) != 0) {
                setup_picwnd_block.diplo_border_intruder_count = y + 2;
              }
              if ((improvements2 & 0x20) != 0) {
                setup_picwnd_block.diplo_border_intruder_count++;
              }
              if ((improvements2 & 8) != 0) {
                setup_picwnd_block.diplo_border_intruder_count++;
              }
              if ((improvements2 & 4) != 0) {
                setup_picwnd_block.diplo_border_intruder_count++;
              }
              if ((improvements2 & 0x40) != 0) {
                setup_picwnd_block.diplo_border_intruder_count += 2;
              }
            }
          }
        }
        if (3 < intruders) {
          setup_picwnd_block.diplo_border_repeat_offenses++;
        }
      }
    }
  }
  else {
    setup_picwnd_block.diplo_border_intruder_count = 0;
  }
  return setup_picwnd_block.diplo_border_intruder_count;
next_tile:
  tile++;
  goto tile_loop;
}

/* 0055bef9 auto */
i32 __cdecl diplo_senate_intervenes(iptr civ,iptr civ2)

{
  i32 result;
  int threshold;
  int bonus;
  
  if (civs[civ].government < 5) {
    result = 0;
  }
  else if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 1) == 0)) {
    bonus = 0;
    if (((byte)(civs[civ2].treaties[civ] >> 8) & 0x10) != 0) {
      bonus = 0x19;
    }
    threshold = civ_has_wonder(civ,0x18);
    if (threshold != 0) {
      bonus = 0x32;
    }
    threshold = int_clamp(bonus + (uint)civs[civ2].reputation * 0xf,0,0x4b);
    if ((int)(uint)civs[civ].diplo_random < threshold) {
      result = 0;
    }
    else if (civs[civ].government < 6) {
      if ((civs[civ].flags & 4) == 0) {
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
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 8) == 0) {
    if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
          check = diplo_is_hostile(civ,civ3);
          if (((check != 0) && (civs[civ2].military_strength < civs[civ3].military_strength))
             && ((*(byte *)&civs[civ3].treaties[civ2] & 8) == 0)) {
            return 0;
          }
          check = diplo_is_hostile(civ2,civ3);
          if (((check != 0) && (civs[civ].military_strength < civs[civ3].military_strength))
             && ((*(byte *)&civs[civ3].treaties[civ] & 8) == 0)) {
            return 0;
          }
        }
      }
      if (civs[civ].units_active[0x2d] < civs[civ2].units_active[0x2d]) {
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
            if (((((*(byte *)&civs[civ].treaties[other_civ] & 8) != 0) &&
                 ((*(byte *)&civs[civ2].treaties[other_civ] & 8) != 0)) &&
                ((1 << ((byte)other_civ & 0x1f) & (uint)game.human_players) != 0)) &&
               ((check = security + 1, game.power_rank[civ] < game.power_rank[other_civ] &&
                (game.power_rank[civ2] < game.power_rank[other_civ])))) {
              check = security + 2;
            }
            security = check;
            if ((((*(byte *)&civs[civ].treaties[other_civ] & 8) != 0) &&
                (((byte)(civs[civ2].treaties[other_civ] >> 8) & 0x20) != 0)) &&
               ((1 << ((byte)other_civ & 0x1f) & (uint)game.human_players) != 0)) {
              security--;
            }
            if (((game.power_rank[other_civ] == 7) && (199 < game.turn)) &&
               (((byte)(civs[civ2].treaties[other_civ] >> 8) & 0x20) != 0)) {
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
  if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 0x20) == 0)) {
    if (((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) == 0) ||
        (((game.difficulty == '\0' || (civs[game.civ_by_rank[7]].num_cities < 5)) ||
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
        if (((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) &&
           ((((int)((uint)civs[civ].num_advances + (6 - (uint)game.difficulty) * 2) <
              (int)(uint)civs[civ2].num_advances || (leader_far_ahead)) &&
            (((byte)(civs[civ].treaties[civ2] >> 16) & 4) == 0)))) {
          diplo_set_treaty_flags(civ,civ2,0x40000);
          civ_give_advance(civ,advance2,civ2,0,0);
          return 1;
        }
      }
      else if ((((found_mask == 2) && ((*(byte *)&civs[civ2].treaties[civ] & 8) != 0)
                ) && (((int)((uint)civs[civ2].num_advances + (6 - (uint)game.difficulty) * 2) <
                       (int)(uint)civs[civ].num_advances || (leader_far_ahead)))) &&
              (((byte)(civs[civ2].treaties[civ] >> 16) & 4) == 0)) {
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
  
  if ((civs[civ].treaties[civ2] & 0x2008) == 0) {
    if (((byte)(civs[civ3].treaties[civ2] >> 8) & 0x20) == 0) {
      if (((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) &&
         ((*(byte *)&civs[civ3].treaties[civ2] & 0x10) != 0)) {
        flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
        *flags_ptr = *flags_ptr | 0x20;
        flags_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ3 * 0x594);
        *flags_ptr = *flags_ptr | 0x20;
      }
      result = 0;
    }
    else {
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0) {
          if ((int)civs[civ2].last_treaty_turn[civ] - (int)game.turn < 6) {
            return 0;
          }
          if ((game.power_rank[civ2] < 7) && (roll = _rand(), roll % 3 != 0)) {
            return 0;
          }
        }
        civs[civ2].last_treaty_turn[civ] = game.turn;
        civs[civ2].last_treaty_turn[civ3] = game.turn;
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

/* 0055d8d8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diplo_civs_meet(uptr civ,uptr civ2,iptr x,iptr y)

{
  uint *flags_ptr;
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
     (city = events_check_negotiation((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events),civ,civ2),
     city == 0)) {
    return;
  }
  if (((game.multiplayer == '\0') || ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)
      ) || ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) {
    if ((((2 < game.multiplayer) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
        && (human_civ[0] != civ)) &&
       ((((*(byte *)&civs[civ].treaties[civ2] & 1) == 0 ||
         (((byte)(civs[civ].treaties[civ2] >> 16) & 1) != 0)) ||
        ((civs[civ].last_treaty_turn[civ2] < 0 ||
         (0xf < (int)game.turn - (int)civs[civ].last_treaty_turn[civ2])))))) {
      net_send_message(0x99,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                         civ,civ2,x,y,0,0,0,0);
      return;
    }
    if ((((2 < game.multiplayer) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
        && (human_civ[0] != civ2)) &&
       ((((*(byte *)&civs[civ2].treaties[civ] & 1) == 0 ||
         (((byte)(civs[civ2].treaties[civ] >> 16) & 1) != 0)) ||
        ((civs[civ2].last_treaty_turn[civ] < 0 ||
         (0xf < (int)game.turn - (int)civs[civ2].last_treaty_turn[civ])))))) {
      net_send_message(0x99,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,
                         civ2,civ,x,y,0,0,0,0);
      return;
    }
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ)) &&
       (((*(byte *)&civs[civ].treaties[civ2] & 1) == 0 ||
        (((((byte)(civs[civ].treaties[civ2] >> 16) & 1) != 0 ||
          (civs[civ].last_treaty_turn[civ2] < 0)) ||
         (0xf < (int)game.turn - (int)civs[civ].last_treaty_turn[civ2])))))) {
      if ((2 < game.multiplayer) && (human_civ[0] != civ)) {
        return;
      }
      diplo_ai_meet_player(civ,civ2,x,y,0);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ2)) &&
       (((*(byte *)&civs[civ2].treaties[civ] & 1) == 0 ||
        (((((byte)(civs[civ2].treaties[civ] >> 16) & 1) != 0 ||
          (civs[civ2].last_treaty_turn[civ] < 0)) ||
         (0xf < (int)game.turn - (int)civs[civ2].last_treaty_turn[civ])))))) {
      if ((2 < game.multiplayer) && (human_civ[0] != civ2)) {
        return;
      }
      diplo_ai_meet_player(civ2,civ,x,y,0);
    }
  }
  else if (game.multiplayer < 3) {
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ)) &&
       (((*(byte *)&civs[civ].treaties[civ2] & 1) == 0 ||
        (((((byte)(civs[civ].treaties[civ2] >> 16) & 1) != 0 ||
          (civs[civ].last_treaty_turn[civ2] < 0)) ||
         (0xf < (int)game.turn - (int)civs[civ].last_treaty_turn[civ2])))))) {
      diplo_ai_meet_player(civ,civ2,x,y,0);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (human_civ[0] == civ2)) &&
       ((((*(byte *)&civs[civ2].treaties[civ] & 1) == 0 ||
         ((((byte)(civs[civ2].treaties[civ] >> 16) & 1) != 0 ||
          (civs[civ2].last_treaty_turn[civ] < 0)))) ||
        (0xf < (int)game.turn - (int)civs[civ2].last_treaty_turn[civ])))) {
      diplo_ai_meet_player(civ2,civ,x,y,0);
    }
  }
  else if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
          (((*(byte *)&civs[civ].treaties[civ2] & 1) == 0 ||
           ((civs[civ].last_treaty_turn[civ2] < 0 ||
            (0xf < (int)game.turn - (int)civs[civ].last_treaty_turn[civ2]))))))
  {
    advisor_window_block.mp_meeting_reply = -1;
    mp_meeting_pending = 1;
    net_send_text_args(0x3d,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,0,0,
                       civ,0);
    civ_name = (iptr)(civ_leader_name(civ2));
    text_set_string_arg(1,(const char *)civ_name);
    civ_name = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(2,(const char *)civ_name);
    dlg_pending_idle_callback = mp_meeting_wait_idle;
    civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
    city = popup_game_sprite_flags((const char *)&str_parleywaiting_2,0x2000001,(iptr)&popup_info_icon_sprite,0);
    if (net_session_block.net_host_handoff_stage == '\0') {
      if (net_session_block.nm_parley_cancel_count == 0) {
        if (city == -1) {
          net_send_message(0x81,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,0,
                             0,0,0,0,0,0,0);
          parley_wnd_block.other_civ = 0xffffffff;
          mp_meeting_pending = 0;
        }
        else if (advisor_window_block.mp_meeting_reply < 1) {
          dlg_pending_idle_callback = turn_popup_timeout_idle;
          popup_game_with_sprite((const char *)&str_parleygoaway_2,(iptr)&popup_info_icon_sprite,0);
          mp_meeting_pending = 0;
        }
        else if (advisor_window_block.mp_meeting_reply == 1) {
          advisor_window_block.mp_meeting_reply = -1;
          parley_wnd_block.other_civ = civ2;
          dlg_pending_idle_callback = mp_meeting_wait_idle;
          civ_rating_title_font_block.mp_meeting_wait_start_i32 = timer_ticks();
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_parleyok_2,(iptr)&popup_info_icon_sprite,0);
          if (net_session_block.net_host_handoff_stage == '\0') {
            if (net_session_block.nm_parley_cancel_count == 0) {
              parley_wnd_open(&parley_wnd_block,civ2,3);
            }
            else {
              parley_wnd_block.other_civ = 0xffffffff;
              net_session_block.nm_parley_cancel_count = 0;
              civ_name = (iptr)(civ_name_plural(civ2));
              text_set_string_arg(0,(const char *)civ_name);
              dlg_pending_idle_callback = turn_popup_timeout_idle;
              popup_game_with_sprite((const char *)&str_parleycancel_6,(iptr)&popup_info_icon_sprite,0);
              mp_meeting_pending = 0;
            }
          }
          else {
            parley_wnd_block.other_civ = 0xffffffff;
            dlg_pending_idle_callback = turn_popup_timeout_idle;
            popup_game_with_sprite((const char *)&str_parleybusy_4,(iptr)&popup_info_icon_sprite,0);
            mp_meeting_pending = 0;
          }
        }
        else {
          dlg_pending_idle_callback = turn_popup_timeout_idle;
          popup_game_with_sprite((const char *)&str_parleybusy_5,(iptr)&popup_info_icon_sprite,0);
          mp_meeting_pending = 0;
        }
      }
      else {
        net_session_block.nm_parley_cancel_count = 0;
        civ_name = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(0,(const char *)civ_name);
        dlg_pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleycancel_5,(iptr)&popup_info_icon_sprite,0);
        parley_wnd_block.other_civ = 0xffffffff;
        mp_meeting_pending = 0;
      }
    }
    else {
      dlg_pending_idle_callback = turn_popup_timeout_idle;
      popup_game_with_sprite((const char *)&str_parleybusy_3,(iptr)&popup_info_icon_sprite,0);
      mp_meeting_pending = 0;
    }
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & 1) == 0) {
    diplo_set_treaty_flags(civ,civ2,0x4000);
  }
  first_or_allied = (*(byte *)&civs[civ].treaties[civ2] & 1) == 0;
  diplo_set_treaty_flags(civ,civ2,0x401);
  if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) &&
      ((((int)game.turn + civ2 + civ & 3) == 0 || (first_or_allied)))) &&
     ((((game.game_flags & 0x80) == 0 || (((int)(*(short *)&prefs_game_rules_block.scenario_flags) & 0x8000U) == 0)) ||
      ((((civ != 3 || (civ2 != 1)) && ((civ != 1 || (civ2 != 3)))) || (4 < game.turn))
      )))) {
    city = diplo_ai_feels_secure(civ,civ2);
    if (((((city == 0) || (city = civ_has_wonder(civ2,6), city != 0)) ||
         (city = civ_has_wonder(civ2,0x18), city != 0)) ||
        ((4 < civs[civ].government &&
         (((byte)(civs[civ].treaties[civ2] >> 8) & 8) == 0)))) &&
       (((city = diplo_ai_feels_secure(civ2,civ), city == 0 ||
         ((city = civ_has_wonder(civ,6), city != 0 ||
          (city = civ_has_wonder(civ,0x18), city != 0)))) ||
        ((4 < civs[civ2].government &&
         (((byte)(civs[civ2].treaties[civ] >> 8) & 8) == 0)))))) {
      diplo_ai_swap_advances(civ,civ2);
      if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
        if ((civ_saved_cursor_x_block.autoplay_active == 0) && (((byte)(civs[civ].treaties[civ2] >> 8) & 8) != 0))
        {
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(1,(const char *)civ_name);
          civ_name = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(2,(const char *)civ_name);
          city = civ_has_wonder(civ2,0x18);
          if (city == 0) {
            city = civ_has_wonder(civ2,6);
            if (city != 0) {
              popup_game_with_sprite((const char *)&str_wallforce,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            }
          }
          else {
            popup_game_with_sprite((const char *)&str_unforce,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
        }
        if ((civ_saved_cursor_x_block.autoplay_active == 0) && (((byte)(civs[civ2].treaties[civ] >> 8) & 8) != 0))
        {
          civ_name = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(1,(const char *)civ_name);
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(2,(const char *)civ_name);
          city = civ_has_wonder(civ,0x18);
          if (city == 0) {
            city = civ_has_wonder(civ,6);
            if (city != 0) {
              popup_game_with_sprite((const char *)&str_wallforce_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
            }
          }
          else {
            popup_game_with_sprite((const char *)&str_unforce_2,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
          }
        }
        if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
           ((*(byte *)&civs[human_civ[0]].treaties[civ] & 8) != 0)) {
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)civ_name);
          civ_name = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(1,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_allymakespeace,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                             ,0);
        }
        else if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
                ((*(byte *)&civs[human_civ[0]].treaties[civ2] & 8) != 0)) {
          civ_name = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(0,(const char *)civ_name);
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(1,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_allymakespeace_2,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                             ,0);
        }
        else if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
                (((((((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0 ||
                     (city = civ_has_wonder(human_civ[0],0x18), city != 0)) ||
                    (city = civ_has_wonder(human_civ[0],9), city != 0)) ||
                   (((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) != 0 ||
                    (city = civ_has_wonder(human_civ[0],0x18), city != 0)))) ||
                  (city = civ_has_wonder(human_civ[0],9), city != 0)) ||
                 (game.reveal_map != '\0')))) {
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)civ_name);
          civ_name = (iptr)(civ_name_plural(civ2));
          text_set_string_arg(1,(const char *)civ_name);
          popup_game_with_sprite((const char *)&str_signpeace,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        diplo_set_treaty_flags(civ,civ2,4);
      }
      else {
        was_allied = (*(byte *)&civs[civ].treaties[civ2] & 8) != 0;
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
            if (((*(byte *)&civs[civ].treaties[civ4] & 8) == 0) &&
               ((*(byte *)&civs[civ2].treaties[civ4] & 8) == 0)) {
              if (((((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) == 0) ||
                   (game.power_rank[civ4] != 7)) || (game.difficulty == '\0')) ||
                 ((civs[civ4].num_cities < 5 || (game.turn < 0xc9)))) {
                target_leader = false;
              }
              else {
                target_leader = true;
              }
              if (target_leader) goto found_third_civ;
              if (game.difficulty != '\0') {
                if (first_or_allied) {
                  if (((*(byte *)&civs[civ].treaties[civ4] & 4) == 0) &&
                     ((*(byte *)&civs[civ2].treaties[civ4] & 4) == 0)) {
found_third_civ:
                    civ3 = civ4;
                    break;
                  }
                }
                else if (((!leader_gap) && (city = diplo_is_hostile(civ,civ4), city != 0)) &&
                        ((game.power_rank[civ] <= game.power_rank[civ4] &&
                         ((city = diplo_is_hostile(civ2,civ4), city != 0 &&
                          (game.power_rank[civ2] <= game.power_rank[civ4]))))))
                goto found_third_civ;
              }
            }
            else {
              allied_count++;
            }
          }
        }
        if ((((game.game_flags & 0x80) != 0) && (((int)(*(short *)&prefs_game_rules_block.scenario_flags) & 0x8000U) != 0)) &&
           ((((civ == 6 && (civ2 == 7)) || ((civ == 7 && (civ2 == 6)))) &&
            (1 < game.turn)))) {
          civ3 = 0;
        }
        if (((int)civ3 < 1) ||
           ((((!target_leader || ((int)(uint)civs[game.player_civ].reputation < allied_count)) &&
             (allied_count != 0)) && (!first_or_allied)))) {
          if (((was_allied) && (civ_saved_cursor_x_block.autoplay_active == 0)) &&
             (((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0 ||
              (((city = civ_has_wonder(human_civ[0],0x18), city != 0 ||
                (city = civ_has_wonder(human_civ[0],9), city != 0)) ||
               (((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) != 0 ||
                (((city = civ_has_wonder(human_civ[0],0x18), city != 0 ||
                  (city = civ_has_wonder(human_civ[0],9), city != 0)) ||
                 (game.reveal_map != '\0')))))))))) {
            civ_name = (iptr)(civ_name_plural(civ));
            text_set_string_arg(0,(const char *)civ_name);
            civ_name = (iptr)(civ_name_plural(civ2));
            text_set_string_arg(1,(const char *)civ_name);
            popup_game_with_sprite((const char *)&str_cancelallied_2,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[0xf] != '\0') - 1 & 0xffff7c80
                                              )),0);
          }
          diplo_clear_treaty_flags(civ,civ2,8);
        }
        else {
          if (!was_allied) {
            joined_war = 0;
            if (((((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0) ||
                 (city = civ_has_wonder(human_civ[0],0x18), city != 0)) ||
                ((city = civ_has_wonder(human_civ[0],9), city != 0 ||
                 (((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) != 0 ||
                  (city = civ_has_wonder(human_civ[0],0x18), city != 0)))))) ||
               (((city = civ_has_wonder(human_civ[0],9), city != 0 ||
                 (game.reveal_map != '\0')) ||
                (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0 &&
                 (((*(byte *)&civs[human_civ[0]].treaties[civ] & 1) != 0 ||
                  ((*(byte *)&civs[human_civ[0]].treaties[civ2] & 1) != 0)))))))) {
              civ_name = (iptr)(civ_name_plural(civ));
              text_set_string_arg(0,(const char *)civ_name);
              civ_name = (iptr)(civ_name_plural(civ2));
              text_set_string_arg(1,(const char *)civ_name);
              if ((civ_saved_cursor_x_block.autoplay_active == 0) && (target_leader)) {
                city = city_find_nearest(x,y,civ,0xffffffff,civ2);
                if (-1 < city) {
                  text_set_string_arg(2,(const char *)&cities[city].name);
                }
                civ_name = civ_get_adjective(civ3);
                text_set_string_arg(3,(const char *)civ_name);
                popup_game_with_sprite((const char *)&str_signnato,(iptr)&diplo_popup_portrait_sprite,0);
              }
              else if (civ_saved_cursor_x_block.autoplay_active == 0) {
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
              if (((byte)(civs[civ].treaties[civ3] >> 8) & 0x20) == 0) {
                diplo_set_treaty_flags(civ,civ3,0x10000);
              }
              if (((byte)(civs[civ].treaties[civ3] >> 8) & 0x20) == 0) {
                diplo_set_treaty_flags(civ2,civ3,0x10000);
              }
            }
          }
          diplo_set_treaty_flags(civ,civ2,8);
        }
      }
    }
    else if ((((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0) || (first_or_allied)) {
      if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
         ((*(byte *)&civs[human_civ[0]].treaties[civ] & 8) != 0)) {
        city = diplo_ai_feels_secure(civ,civ2);
        if (city == 0) {
          diplo_ally_demand_help(civ,civ2);
        }
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(0,(const char *)civ_name);
        civ_name = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(1,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_allymakeswar,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
      }
      else if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
              ((*(byte *)&civs[human_civ[0]].treaties[civ2] & 8) != 0)) {
        city = diplo_ai_feels_secure(civ2,civ);
        if (city == 0) {
          diplo_ally_demand_help(civ2,civ);
        }
        civ_name = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(0,(const char *)civ_name);
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(1,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_allymakeswar_2,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
                          );
      }
      else if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
              (((((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0 ||
                 (city = civ_has_wonder(human_civ[0],0x18), city != 0)) ||
                (city = civ_has_wonder(human_civ[0],9), city != 0)) ||
               ((((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) != 0 ||
                 (city = civ_has_wonder(human_civ[0],0x18), city != 0)) ||
                ((city = civ_has_wonder(human_civ[0],9), city != 0 ||
                 (game.reveal_map != '\0')))))))) {
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(0,(const char *)civ_name);
        civ_name = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(1,(const char *)civ_name);
        if (((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) ||
           ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0)) {
          popup_game_with_sprite((const char *)&str_declarewar,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
        }
        else {
          popup_game_with_sprite((const char *)&str_cancelpeace_3,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
        }
      }
      diplo_clear_treaty_flags(civ,civ2,4);
      diplo_set_treaty_flags(civ,civ2,0x2000);
      if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
        diplo_clear_treaty_flags(civ,civ2,0x10);
        flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
        *flags_ptr = *flags_ptr | 0x800;
      }
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
       (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0)) {
      war_count++;
    }
  }
  civ2 = 1;
  do {
    if (7 < civ2) {
      return;
    }
    if (((civ2 != civ) && ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0))
       && ((civs[civ2].military_strength <= civs[civ].military_strength || (war_count == 0)))
       ) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((((civ3 != civ) && (civ3 != civ2)) &&
             (((byte)(civs[civ2].treaties[civ3] >> 8) & 0x20) != 0)) &&
            ((((byte)(civs[civ2].treaties[civ3] >> 8) & 2) != 0 &&
             (civs[civ2].military_strength <= civs[civ3].military_strength)))) &&
           (game.power_rank[civ2] <= game.power_rank[civ3])) {
          unit2 = -1;
          best_score = 0;
          for (unit = 0; unit < game.num_units; unit++) {
            if (((units[unit].id != 0) && (units[unit].owner == civ)) &&
               (((char)unit_types[units[unit].type].role < 2 &&
                (unit_types[units[unit].type].domain == '\0')))) {
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
                    ((((cities[city].flags & 0x80) != 0 &&
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
                  city_window_refresh_if_city((CityWindow *)((i8 *)&city_window_block.city_window),city);
                  map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
                  city_window_refresh_if_city((CityWindow *)((i8 *)&city_window_block.city_window),city2);
                  map_redraw_tile_all_views((int)cities[city2].x,(int)cities[city2].y);
                  if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
                     (civ_saved_cursor_x_block.autoplay_active == 0)) {
                    civ_name = (iptr)(civ_name_plural(civ));
                    text_set_string_arg(0,(const char *)civ_name);
                    text_set_label_arg(1,unit_types[units[unit2].type].name);
                    civ_name = (iptr)(civ_name_plural(civ2));
                    text_set_string_arg(2,(const char *)civ_name);
                    if (human_civ[0] == civ3) {
                      dlg_game_txt_with_unit((const char *)&str_militaryaid1_2,unit2);
                    }
                    else if (2 < game.multiplayer) {
                      net_send_text_args(0x51,net_session_block.net_slots[net_session_block.net_civ_slots[civ3]].id,3,0,
                                         unit2,0);
                    }
                  }
                  if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
                     (civ_saved_cursor_x_block.autoplay_active == 0)) {
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
                      net_send_text_args(0x52,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,4,0,
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

/* 00560d95 auto */
void __cdecl diplo_ai_review_relations(iptr civ,uptr civ2)

{
  iptr x;
  int y;
  iptr civ_name;
  uint rand_val;
  iptr hostile;
  int value;
  uint sign;
  iptr city;
  int roll;
  int attitude_delta;
  iptr unit;
  int choice;
  iptr civ3;
  int rank_diff;
  int ahead_count;
  int rank_adjust;
  int hostile_count;
  int war_score;
  attitude_delta = 0x6715d7a0;  /* Captured compatibility preset; see docs/stack-compatibility.md. */
  
  diplo_count_border_intruders(civ,civ2);
  city = setup_picwnd_block.border_intruder_city;
  if ((diplo_border_intruded_cities != 0) && ((civs[civ].treaties[civ2] & 0x2008) == 0))
  {
    x = (iptr)cities[setup_picwnd_block.border_intruder_city].x;
    y = (int)cities[setup_picwnd_block.border_intruder_city].y;
    if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
      if (setup_picwnd_block.diplo_recent_peace_table[civ2] != 0) goto count_relations;
      attitude_delta = setup_picwnd_block.diplo_border_intruder_count;
      rand_val = _rand();
      sign = (int)rand_val >> 0x1f;
      choice = ((rand_val ^ sign) - sign & 1 ^ sign) - sign;
      if ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0) {
        setup_picwnd_block.diplo_border_repeat_offenses++;
      }
      if (2 < (int)((uint)civs[civ2].reputation + setup_picwnd_block.diplo_border_repeat_offenses)) {
        value = (uint)civs[civ2].reputation + setup_picwnd_block.diplo_border_repeat_offenses;
        if (value == 1 || value - 1 < 0) {
          roll = 0;
        }
        else {
          roll = _rand();
          roll = roll % (int)((uint)civs[civ2].reputation + setup_picwnd_block.diplo_border_repeat_offenses);
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
        text_set_string_arg(1,(const char *)&cities[setup_picwnd_block.border_intruder_city].name);
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(2,(const char *)civ_name);
        civ_name = civ_get_adjective(civ);
        text_set_string_arg(3,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_terms,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        goto count_relations;
      }
      if ((civs[civ2].last_treaty_turn[civ] < game.turn) &&
         ((int)game.turn - (int)civs[civ2].last_treaty_turn[civ] < 3))
      goto count_relations;
      if (setup_picwnd_block.diplo_border_repeat_offenses == 0) {
        diplo_eval_ai_stance(civ2,civ);
        diplo_set_attitude_arg(civ2,civ);
        if (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) == 0) &&
           ((int)cities[city].owner != (civ2 & 0xff))) {
          city_reveal_to_civ(city,civ2);
          tile_set_visible(x,y,1 << ((byte)civ2 & 0x1f),1,1);
          map_redraw_tile_all_views(x,y);
        }
        attitude_delta = setup_picwnd_block.diplo_border_intruder_count - 1;
        text_set_string_arg(1,(const char *)&cities[city].name);
        if (diplo_border_intruded_cities < 2) {
          popup_game_with_sprite((const char *)&str_intruder,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        else {
          popup_game_with_sprite((const char *)&str_intruders,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
        }
        goto count_relations;
      }
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
          if ((((units[unit].id != 0) && ((units[unit].flags & 4) != 0)) &&
              (units[unit].flags = units[unit].flags & 0xfffb,
              (int)units[unit].owner == civ2)) &&
             (y = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ2,
                                         0xffffffff,-1), -1 < y)) {
            unit_relocate(unit,(int)cities[y].x,(int)cities[y].y,1);
            units[unit].visibility = '\0';
            units[unit].orders = -1;
            if (unit_types[units[unit].type].range != '\0') {
              units[unit].counter = '\0';
            }
          }
        }
        map_redraw_all_views(human_civ[0],1);
        text_set_number_arg(0,setup_picwnd_block.diplo_border_intruder_units);
        text_clear();
        text_append((const char *)&str_withdrawn);
        if (setup_picwnd_block.diplo_border_intruder_units == 1) {
          text_append((const char *)&str_1_6);
        }
        popup_game_with_sprite((const char *)&map_views_block.text_buffer,(iptr)&peace_treaty_icon_sprite_block.peace_treaty_icon_sprite,0);
      }
      else {
        if (civs[civ2].government == 6) {
senate_scandal:
          if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 1) == 0)) {
            civ_name = civ_get_adjective(civ2);
            text_set_string_arg(0,(const char *)civ_name);
            popup_game((const char *)&str_senatescandal_2);
            civ_change_government(civ2,0);
          }
        }
        else {
          rand_val = _rand();
          sign = (int)rand_val >> 0x1f;
          if ((((rand_val ^ sign) - sign & 1 ^ sign) != sign) &&
             (civs[civ2].government == 5)) goto senate_scandal;
        }
        civ_adjust_attitude(civ,civ2,0x32);
        diplo_declare_war(civ2,civ,-1);
        civ_name = (iptr)(civ_name_plural(civ));
        text_set_string_arg(1,(const char *)civ_name);
        popup_game_with_sprite((const char *)&str_violate,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
      }
      diplo_session_reset();
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if (units[unit].id != 0) {
      units[unit].flags = units[unit].flags & 0xfffb;
    }
  }
count_relations:
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
    if ((((byte)(civs[civ3].treaties[civ2] >> 8) & 0x20) != 0) &&
       (y = war_score + 1, civs[civ2].military_strength < civs[civ3].military_strength)) {
      y = war_score + 2;
    }
    war_score = y;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0) {
    if (hostile_count == 0) {
      if (civs[civ].gold < civs[civ2].gold) {
        attitude_delta++;
      }
      if ((civs[civ].num_advances < civs[civ2].num_advances) && (game.difficulty != '\0')) {
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
  if ((*(byte *)&civs[civ].treaties[civ2] & 4) != 0) {
    attitude_delta -= 2;
  }
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0) {
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
     && (game.difficulty != '\0')) {
    rank_adjust = game.difficulty / 3 + 1;
    if (((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2]
              ) <= war_score) && ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0))
    {
      rank_adjust = rank_adjust >> 1;
    }
    attitude_delta += rank_adjust;
  }
  y = attitude_delta;
  if (((civs[civ2].units_active[0x2d] != '\0') &&
      (y = attitude_delta - 1, civs[civ].units_active[0x2d] == '\0')) && (0 < y)) {
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
  else if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
    rank_diff = rank_diff / 2;
  }
  attitude_delta = attitude_delta + y + rank_diff;
  if (((game.game_flags & 1) == 0) ||
     (((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
       <= war_score && ((*(byte *)&civs[civ].treaties[civ2] & 0x10) == 0)))) {
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
  if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
    if (civs[civ2].military_strength < civs[civ].military_strength) {
      attitude_delta++;
    }
    if ((uint)civs[civ2].military_strength * 2 < (uint)civs[civ].military_strength) {
      attitude_delta++;
    }
  }
  y = civ_has_wonder(civ2,6);
  if ((y != 0) || (y = civ_has_wonder(civ2,0x18), y != 0)) {
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
  if ((((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0) && (attitude_delta < 1)) {
    attitude_delta = 0;
  }
  y = civ_has_wonder(civ,0x14);
  if (y != 0) {
    attitude_delta++;
  }
  value = civ_has_wonder(civ2,0x14);
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
  if ((*(byte *)&civs[civ].treaties[civ2] & 1) == 0) {
    attitude_delta = 0;
  }
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
        if (((((byte)(civs[civ2].treaties[civ3] >> 8) & 2) != 0) &&
            ((civs[civ2].treaties[civ] & 0x2008) == 0)) &&
           ((value = diplo_is_hostile(civ,civ3), value == 0 &&
            (((*(byte *)&civs[civ].treaties[civ2] & 1) != 0 &&
             ((*(byte *)&civs[civ].treaties[civ3] & 1) != 0)))))) {
          double_strength = (uint)civs[civ2].military_strength * 2;
          if (double_strength - civs[civ3].military_strength == 0 ||
              double_strength < civs[civ3].military_strength) {
            value = diplo_ask_receive_emissary(civ,civ2,0xffffffff,0xffffffff);
            if (value == 0) {
              value = (int)civs[civ].last_treaty_turn[civ2];
              if (value <= game.turn - 0xf) {
                value = game.turn - 0xf;
              }
              civs[civ].last_treaty_turn[civ2] = (short)value;
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
                       ((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) == 0)) &&
                      ((check = civ_has_wonder(human_civ[0],0x18), check == 0 &&
                       (check = civ_has_wonder(human_civ[0],9), check == 0)))))))) &&
                   (adv_value = ai_advance_value(civ,advance2), best_value < adv_value)) {
                  advance = advance2;
                  best_value = adv_value;
                }
              }
              civ_name = (iptr)(civ_name_plural(civ3));
              text_set_string_arg(1,(const char *)civ_name);
              text_set_number_arg(0,value * 0x32);
              crt_strcpy(msg_key,str_helpme);
              if ((*(byte *)&civs[civ].treaties[civ3] & 8) != 0) {
                str_append((char *)msg_key,(const char *)&str_instead);
              }
              if (-1 < advance) {
                str_append((char *)msg_key,(const char *)&str_2_4);
                text_set_label_arg(2,advances[advance].name);
              }
              check = popup_game_with_sprite((const char *)msg_key,(iptr)&signal_allied_icon_sprite,0);
              if (check == 0) {
                value = (int)civs[civ].last_treaty_turn[civ2];
                if (value <= game.turn - 0xe) {
                  value = game.turn - 0xe;
                }
                civs[civ].last_treaty_turn[civ2] = (short)value;
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
                civs[civ].last_treaty_turn[civ3] = game.turn;
                civs[civ].last_treaty_turn[civ2] = game.turn + 0x10;
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
              value = (int)civs[civ].last_treaty_turn[civ2];
              if (value <= game.turn - 0xf) {
                value = game.turn - 0xf;
              }
              civs[civ].last_treaty_turn[civ2] = (short)value;
            }
            else {
              civ_name = (iptr)(civ_name_plural(civ3));
              text_set_string_arg(1,(const char *)civ_name);
              text_set_label_arg(2,label_string_ids[(((civs[civ2].treaties[civ] & 8) == 0 ) + 0x73)]);
              crt_strcpy(msg_key,str_crusade);
              if ((*(byte *)&civs[civ].treaties[civ3] & 8) != 0) {
                crt_strcpy(msg_key,str_jihad);
              }
              value = popup_game_with_sprite((const char *)msg_key,(iptr)&signal_allied_icon_sprite,0);
              if (value == 0) {
                value = (int)civs[civ].last_treaty_turn[civ2];
                if (value <= game.turn - 0xe) {
                  value = game.turn - 0xe;
                }
                civs[civ].last_treaty_turn[civ2] = (short)value;
              }
              else {
                diplo_set_treaty_flags(civ2,civ,0xc);
                diplo_declare_war(civ,civ3,civ2);
                civs[civ].last_treaty_turn[civ2] = game.turn + 0x10;
                diplo_set_treaty_flags(civ,civ3,0x2000);
                flags_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ3 * 4);
                *flags_ptr = *flags_ptr | 0x80800;
                civs[civ].last_treaty_turn[civ3] = game.turn;
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
  if (((*(byte *)&civs[civ2].treaties[civ] & 6) != 0) &&
     (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) {
    diplo_declare_war(civ,civ2,-1);
  }
  diplo_clear_treaty_flags(civ,civ2,0xc);
  if ((*(byte *)&civs[civ].treaties[civ2] & 1) != 0) {
    diplo_set_treaty_flags(civ,civ2,0x2000);
    civ_set_attitude(civ,civ2,100);
    civ_set_attitude(civ2,civ,100);
    if ((((byte)(civs[civ2].treaties[civ] >> 8) & 0x10) == 0) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & 0x10) == 0)) {
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
  
  if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) == 0) {
    if ((treaty_mask & civs[civ].treaties[civ2]) == 0) {
      result = 0;
    }
    else if ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) == 0) {
      result = 0;
    }
    else {
      civ_name = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)civ_name);
      if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
        if ((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & 2) == 0) {
            choice = popup_game_with_sprite((const char *)&str_annoy,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
          }
          else if (((byte)(civs[civ].treaties[civ2] >> 16) & 4) == 0) {
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
            if (4 < civs[civ].government) {
              check = civ_has_wonder(civ2,0x18);
              if (check == 0) {
                if ((((byte)(civs[civ2].treaties[civ] >> 8) & 0x10) == 0) &&
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
        popup_game_with_sprite((const char *)&str_annoyallied,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0
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
