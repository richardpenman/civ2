/* The game, units: combat (strengths, the best defender, attacks resolved, nuclear attacks): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0057e2c3 auto */
int __cdecl unit_attack_strength(int unit)

{
  int strength;
  
  strength = (char)unit_types[units[unit].type].attack * 8;
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    strength += (strength >> 1);
  }
  if ((units[unit].flags & UNITF_PARADROPPED) != 0) {
    strength += (strength >> 1);
  }
  return strength;
}

/* 0057e33a auto */
int __cdecl unit_defense_strength(int unit,int recalc_tile,int unit2)

{
  u8 base_defense;
  byte unit_type;
  int x;
  int y;
  byte *tile;
  i32 tile_impr;
  int strength;
  
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  if (recalc_tile != 0) {
    unit_type = tile_terrain(x,y);
    sprite_picker_sprite_block.unit_defense_terrain = (uptr)unit_type;
    sprite_picker_sprite_block.combat_defender_city = city_at(x,y);
  }
  base_defense = unit_types[units[unit].type].defense;
  tile = (byte *)tile_at(x,y);
  strength = ((uint)(*tile >> 7) + (int)(char)terrain_types[sprite_picker_sprite_block.unit_defense_terrain].defense) *
            (char)base_defense * 4;
  sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
  if ((units[unit].orders == ORDER_FORTIFIED) && (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
    sprite_picker_sprite_block.combat_defense_bonus_halves = 3;
  }
  tile_impr = tile_improvements(x,y);
  if (((byte)tile_impr & TILE_AIRBASE) == 0x40) {
    if (unit2 < 0) {
      sprite_picker_sprite_block.combat_defense_bonus_halves = 4;
    }
    else if ((unit_types[units[unit2].type].domain != DOMAIN_AIR) &&
            ((unit_types[units[unit2].type].flags & UTYPE_IGNORE_WALLS) == 0)) {
      sprite_picker_sprite_block.combat_defense_bonus_halves = 4;
    }
  }
  if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
    if ((((unit_types[units[unit].type].domain == DOMAIN_AIR) && (-1 < unit2)) &&
        ((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) != 0)) &&
       ((unit_type = units[unit2].type, unit_types[unit_type].domain == DOMAIN_AIR &&
        ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)))) {
      if ((unit_types[unit_type].flags & UTYPE_ATTACK_AIR) == 0) {
        sprite_picker_sprite_block.combat_defense_bonus_halves = sprite_picker_sprite_block.combat_defense_bonus_halves << 2;
      }
      else {
        sprite_picker_sprite_block.combat_defense_bonus_halves = sprite_picker_sprite_block.combat_defense_bonus_halves << 1;
      }
    }
    else {
      x = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_CITY_WALLS);
      if (((x != 0) || (x = civ_has_wonder(units[unit].owner,WONDER_GREAT_WALL), x != 0)) &&
         (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
        if (unit2 < 0) {
          sprite_picker_sprite_block.combat_defense_bonus_halves = 6;
        }
        else if ((unit_types[units[unit2].type].domain == DOMAIN_LAND) &&
                ((unit_types[units[unit2].type].flags & UTYPE_IGNORE_WALLS) == 0)) {
          sprite_picker_sprite_block.combat_defense_bonus_halves = 6;
        }
      }
      if (unit_types[units[unit].type].domain != DOMAIN_LAND) {
        sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
      }
    }
  }
  else if (unit_types[units[unit].type].domain != DOMAIN_LAND) {
    sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
  }
  if (sprite_picker_sprite_block.combat_defense_bonus_halves != 2) {
    strength = strength * sprite_picker_sprite_block.combat_defense_bonus_halves >> 1;
  }
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    strength += (strength >> 1);
  }
  return strength;
}

/* 0057e6e2 auto */
int __cdecl unit_stack_best_defender(int unit,int unit2)

{
  short x;
  short y;
  byte terrain;
  int hp_left;
  int max_hp;
  int strength;
  iptr best_unit;
  int best_strength;
  
  best_strength = 0;
  best_unit = unit;
  if (-1 < unit) {
    x = units[unit].x;
    y = units[unit].y;
    terrain = tile_terrain(x,y);
    sprite_picker_sprite_block.unit_defense_terrain = (uptr)terrain;
    sprite_picker_sprite_block.combat_defender_city = city_at((int)x,y);
    for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit))
    {
      if ((sprite_picker_sprite_block.unit_defense_terrain != 10) || (unit_types[units[unit].type].domain != DOMAIN_LAND)) {
        strength = unit_defense_strength(unit,0,unit2);
        if ((game.rules & RULE_NORMAL_COMBAT) != 0) {
          hp_left = unit_hp_remaining(unit);
          max_hp = unit_max_hp(unit);
          strength = (hp_left * strength) / max_hp;
        }
        if ((unit_types[units[unit].type].flags & UTYPE_X2_VS_HORSE) != 0) {
          strength++;
        }
        if ((unit_types[units[unit].type].flags & UTYPE_X2_VS_AIR) != 0) {
          if (unit2 == -1) {
            strength++;
          }
          else if (unit_types[units[unit2].type].domain == DOMAIN_AIR) {
            if ((unit_types[units[unit2].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
              strength = strength * 3;
            }
            else {
              strength = strength * 5;
            }
          }
        }
        if (((((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) != 0) && (unit2 != -1)) &&
            (unit_types[units[unit2].type].domain == DOMAIN_AIR)) &&
           ((unit_types[units[unit2].type].flags & UTYPE_ATTACK_AIR) != 0)) {
          strength = strength << 1;
        }
        if ((unit_types[units[unit].type].domain == DOMAIN_SEA) && (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) {
          if (unit2 == -1) {
            strength = strength / 2;
          }
          else if ((unit_types[units[unit2].type].domain == DOMAIN_AIR) &&
                  (hp_left = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SAM_BATTERY), hp_left == 0)) {
            strength = strength << 1;
          }
          else {
            strength = strength / 2;
          }
        }
        if (best_strength <= strength) {
          best_strength = strength;
          best_unit = unit;
        }
      }
    }
  }
  return best_unit;
}

/* 0057e9f9 auto */
void __cdecl unit_kill_in_combat(int unit,int unit2,int x,int y)

{
  char *lost_count_ptr;
  
  if (*(char *)&civs[units[unit].owner].units_lost[units[unit].type] != -1) {
    lost_count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 278) + units[unit].owner * 0x594);
    *lost_count_ptr = *lost_count_ptr + 1;
  }
  if ((units[unit].owner == human_civ[0]) || (game.multiplayer < 3)) {
    casualty_log_add(unit,unit2,game.year,x,y);
  }
  else if ((2 < game.multiplayer) &&
          (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) {
    net_send_message(NM_DEATH_STACK,net.net_slots[net.net_civ_slots[units[unit].owner]].id,unit,
                       unit2,game.year,x,y,0,0,0);
  }
  sprite_picker_sprite_block.combat_kill_count++;
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_unit_killed((uint)units[unit].type,(int)units[unit2].owner,
               (int)units[unit].owner);
  }
  unit_delete(unit,1);
  return;
}

/* 0057eb94 auto */
void __cdecl unit_kill_stack_in_combat(int unit,int unit2,i32 x,i32 y)

{
  short next_unit;
  
  sprite_picker_sprite_block.combat_kill_count = 0;
  unit = unit_stack_first(unit);
  while (-1 < unit) {
    next_unit = units[unit].next_in_stack;
    unit_kill_in_combat(unit,unit2,x,y);
    unit = (iptr)next_unit;
  }
  return;
}

/* 0057ebfd auto */
void __cdecl unit_promote_veteran(int unit)

{
  if (((((units[unit].flags & UNITF_VETERAN) == 0) &&
       ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) &&
      (units[unit].flags = units[unit].flags | UNITF_VETERAN,
      ((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) && (autoplay.active == 0)
     ) {
    text_set_label_arg(0,unit_types[units[unit].type].name);
    if (units[unit].owner == human_civ[0]) {
      dlg_game_txt_with_unit((const char *)&str_promoted_2,unit);
    }
    else if (2 < game.multiplayer) {
      net_send_text_args(0x20,net.net_slots[net.net_civ_slots[units[unit].owner]].id,1,0,
                         unit,0);
    }
  }
  return;
}

/* 0057f9e3 auto */
i32 __cdecl unit_nuclear_attack(int civ,int x,int y,int check_sdi)

{
  uint *treaty_ptr;
  char dy;
  iptr target_civ;
  iptr x2;
  iptr has_sdi;
  int unit;
  iptr city;
  byte target_civ_byte;
  iptr dir;
  
  target_civ = tile_city_or_unit_owner(x,y);
  x2 = (iptr)(civ_name_plural(civ));
  text_set_string_arg(0,(const char *)x2);
  target_civ_byte = (byte)target_civ;
  if (((game.multiplayer < 3) || ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) == 0)) ||
     (human_civ[0] == target_civ)) {
    if ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) != 0) {
      popup_game_with_improvement((const char *)&str_useweapons_2,IMP_MANHATTAN,8);
    }
  }
  else {
    net_send_text_args(0x29,net.net_slots[net.net_civ_slots[target_civ]].id,1,0,0,0);
  }
  if (check_sdi != 0) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner != civ)) &&
         ((has_sdi = city_has_improvement(city,IMP_SDI_DEFENSE), has_sdi != 0 &&
          (unit = map_distance((int)cities[city].x,(int)cities[city].y,x,
                                      y), unit < 4)))) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        if (((game.multiplayer < 3) ||
            (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) == 0)) ||
           (cities[city].owner == human_civ[0])) {
          if (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0) {
            popup_game_with_improvement((const char *)&str_sdi_2,IMP_SDI_DEFENSE,8);
          }
        }
        else {
          net_send_text_args(0x2a,net.net_slots[net.net_civ_slots[cities[city].owner]].id,1,0,
                             0,0);
        }
        if (((game.multiplayer < 3) ||
            ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) ||
           (human_civ[0] == civ)) {
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
            popup_game_with_improvement((const char *)&str_sdi_3,IMP_SDI_DEFENSE,8);
          }
        }
        else {
          net_send_text_args(0x2a,net.net_slots[net.net_civ_slots[civ]].id,1,
                             0,0,0);
        }
        return 0;
      }
    }
  }
  if (((2 < game.multiplayer) && ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) != 0)) &&
     (human_civ[0] != target_civ)) {
    net_send_message(NM_SHOW_NUKE,net.net_slots[net.net_civ_slots[target_civ]].id,x,
                       y,0,0,0,0,0,0);
  }
  map_animate_nuke(x,y);
  for (dir = 0; dir < 9; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    unit = map_in_bounds(x2,dy + y);
    if ((unit != 0) && (unit = tile_first_unit(x2,dy + y), -1 < unit)) {
      if (units[unit].owner != civ) {
        treaty_ptr = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + units[unit].owner * 0x594);
        *treaty_ptr = *treaty_ptr | 0x110;
        treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + units[unit].owner * 4);
        *treaty_ptr = *treaty_ptr | 0x20000;
        civ_adjust_attitude(units[unit].owner,civ,100);
      }
      unit_stack_delete_all(unit,1);
    }
  }
  nuclear_blast(x,y);
  return 1;
}

/* Part of unit_resolve_attack (00580341): a defending city's Coastal Defense,
   SAM battery and SDI, or fighters scrambling against bombers. */
static void unit_resolve_attack_city_defenses(iptr unit,iptr unit2,int execute,int &defense2,
                                                         iptr &improvement,int &sdi_bonus,int &scramble_msg)
{
  iptr has_coastal;
  int unit3;
  if ((((0 < (int)sprite_picker_sprite_block.combat_defender_city) && (unit_types[units[unit].type].domain == DOMAIN_SEA)) &&
      (unit_types[units[unit2].type].domain != DOMAIN_SEA)) &&
     ((has_coastal = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_COASTAL_FORTRESS), has_coastal != 0 &&
      (defense2 = defense2 << 1, execute != 0)))) {
    improvement = 0x1c;
    cities[sprite_picker_sprite_block.combat_defender_city].flags = cities[sprite_picker_sprite_block.combat_defender_city].flags | CITYF_COASTAL_FORT_USED;
  }
  if ((0 < (int)sprite_picker_sprite_block.combat_defender_city) && (unit_types[units[unit].type].domain == DOMAIN_AIR)) {
    if (((unit_types[units[unit2].type].flags & UTYPE_ATTACK_AIR) == 0) ||
       ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) {
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SDI_DEFENSE);
      if (((unit3 != 0) && ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) &&
         ((char)unit_types[units[unit].type].attack < 'c')) {
        defense2 = defense2 << 1;
        sdi_bonus = 1;
        if (execute != 0) {
          improvement = 0x11;
        }
      }
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SAM_BATTERY);
      if ((unit3 != 0) && (defense2 = defense2 << 1, execute != 0)) {
        improvement = 0x1b;
      }
    }
    else if (execute != 0) {
      scramble_msg = 1;
    }
  }
}

/* Part of unit_resolve_attack (00580341): the odds of barbarian attacks, and of
   attacks on barbarians (difficulty, Great Wall). */
static void unit_resolve_attack_barbarian_odds(iptr unit2,uptr civ,uptr civ2,byte civ5,
                                                          int &attack_power,int &defense2,
                                                          bool &defender_reroll)
{
  int unit3;
  if (civ == 0) {
    if ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) {
      attack_power = attack_power / 2;
    }
    else {
      attack_power = (game.difficulty + 1) * attack_power;
      attack_power = (int)(attack_power + (attack_power >> 0x1f & 3U)) >> 2;
    }
    if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
      if (civs[civ2].num_cities < 2) {
        attack_power = 0;
      }
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_PALACE);
      defender_reroll = unit3 != 0;
      if (defender_reroll) {
        attack_power = attack_power >> 1;
      }
      if (civs[civ2].num_cities < 8) {
        defender_reroll = true;
      }
    }
    unit3 = civ_has_wonder(civ2,WONDER_GREAT_WALL);
    if (unit3 != 0) {
      attack_power = attack_power / 2;
    }
  }
  if (civ2 == 0) {
    if (((units[unit2].type == UNIT_ARCHERS) ||
        ((units[unit2].type == UNIT_LEGION && ((game.human_players & game.advance_known_by[ADV_IRON_WORKING]) == 0)
         ))) && (defense2 = defense2 / 2, defense2 < 2)) {
      defense2 = 1;
    }
    unit3 = civ_has_wonder(civ,WONDER_GREAT_WALL);
    if (unit3 != 0) {
      attack_power = attack_power << 1;
    }
  }
}

/* Part of unit_resolve_attack (00580341): attacking breaks a treaty: who is told
   (treaty and ally warnings), the senate, the victim's message. */
static void unit_resolve_attack_break_treaty(iptr unit,uptr civ,uptr civ2,byte civ4,byte civ5,
                                                        int &attack_power,int &show_treaty_msg,
                                                        int &ally_warning)
{
  int unit3;
  iptr name_str;
  int senate_roll;
  uptr civ3;
  if (((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) ||
     (((*(byte *)&civs[civ2].treaties[civ] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0 &&
      ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0 &&
        ((*(byte *)&civs[civ].treaties[civ2] & TREATY_BROKEN) == 0)) ||
       ((*(byte *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 33) + civ * 4) & 0x20) != 0)))))) {
    if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) ||
       ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) != 0)) {
      if (game.multiplayer < 3) {
        if (((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0) &&
           ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
          if ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_ALLIANCE) == 0) {
            if ((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_ALLIANCE) == 0) {
              if (game.reveal_map != '\0') {
                show_treaty_msg = 1;
              }
            }
            else {
              ally_warning = 2;
            }
          }
          else {
            ally_warning = 1;
          }
        }
        else {
          show_treaty_msg = 1;
        }
      }
      else {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if (civ == civ3) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            else if (civ2 == civ3) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            else if (game.reveal_map != '\0') {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            if (*(int *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) != 0) {
              show_treaty_msg = 1;
            }
          }
        }
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if ((civ2 == civ3) ||
               ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_ALLIANCE) == 0)) {
              if ((civ != civ3) &&
                 ((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) != 0)) {
                *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 2;
                ally_warning = 2;
              }
            }
            else {
              *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 1;
              ally_warning = 1;
            }
          }
        }
      }
    }
  }
  else {
    if (GOV_FUNDAMENTALISM < civs[civ].government) {
      senate_roll = crt_rand_at("unit_resolve_attack");
      senate_roll = senate_roll % 100;
      if ((civs[civ].government == GOV_REPUBLIC) ||
         (unit3 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), unit3 != 0)) {
        senate_roll -= 0x32;
      }
      if ((-1 < (int)(senate_roll + (uint)civs[civ2].reputation * -10)) &&
         ((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0)) {
        ai_choose_government(civ,1);
      }
    }
    if (((*(byte *)&civs[civ2].treaties[civ] & TREATY_CEASEFIRE) == 0) ||
       ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) != 0)) {
      if ((autoplay.active == 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
        name_str = civ_get_adjective(civ);
        text_set_string_arg(0,(const char *)name_str);
        if (human_civ[0] == civ2) {
          dlg_game_txt_with_unit((const char *)&str_sneak_2,unit);
        }
        else if (2 < game.multiplayer) {
          net_send_text_args(0x2e,net.net_slots[net.net_civ_slots[civ2]].id,1,0
                             ,unit,0);
        }
      }
    }
    else if ((autoplay.active == 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
      name_str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)name_str);
      if (human_civ[0] == civ2) {
        dlg_game_txt_with_unit((const char *)&str_breakcease_3,unit);
      }
      else if (2 < game.multiplayer) {
        net_send_text_args(0x2d,net.net_slots[net.net_civ_slots[civ2]].id,1,0,
                           unit,0);
      }
    }
    attack_power = attack_power << 1;
    civs[civ2].last_contact_turn[civ] = game.turn;
  }
}

/* Part of unit_resolve_attack (00580341): which human players see the combat
   (civ_sees). Returns whether any does. */
static bool unit_resolve_attack_viewers(iptr unit,iptr unit2,uptr civ,uptr civ2,int *civ_sees)
{
  bool any_viewer;
  uptr civ3;
  any_viewer = false;
  for (civ3 = 1; (int)civ3 < 8; civ3++) {
    civ_sees[civ3] = 0;
    if ((((2 < game.multiplayer) || (human_civ[0] == civ3)) &&
        ((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0)) {
      if (game.multiplayer < 3) {
        if (((((civ == civ3) || (civ2 == civ3)) ||
             ((game.reveal_map != '\0' ||
              ((((1 << ((byte)civ3 & 0x1f) & (uint)units[unit].visibility) != 0 ||
                ((int)units[unit].owner == (civ3 & 0xff))) ||
               ((1 << ((byte)civ3 & 0x1f) & (uint)units[unit2].visibility) != 0)))))) ||
            ((int)units[unit2].owner == (civ3 & 0xff))) ||
           (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
          civ_sees[civ3] = 1;
          any_viewer = true;
        }
      }
      else if ((civ == civ3) || (civ2 == civ3)) {
        civ_sees[civ3] = 1;
        any_viewer = true;
      }
    }
  }
  return any_viewer;
}

/* Part of unit_resolve_attack (00580341): the messages before the combat
   (missile attack, sneak attack from the sea, city defenses, scrambled
   fighters, attack from a ship). */
static void unit_resolve_attack_announce(iptr unit,iptr unit2,uptr civ,uptr civ2,byte civ5,iptr x,
                                                    int y,int x2,int y2,iptr improvement,int sdi_bonus,
                                                    int scramble_msg,int shore_bombard,int *civ_sees)
{
  iptr name_str;
  int city2;
  uptr civ3;
  if (((((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) &&
       ((char)unit_types[units[unit].type].attack < 'c')) &&
      ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) &&
     ((city2 = city_find_nearest(x,y,-1,0xffffffff,-1), -1 < city2 &&
      (text_set_string_arg(0,(const char *)&cities[city2].name), autoplay.active == 0)))) {
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          dlg_game_txt_with_unit((const char *)&str_missileattack_2,unit);
        }
        else {
          net_send_text_args(0x2f,net.net_slots[net.net_civ_slots[civ3]].id
                             ,1,0,unit,0);
        }
      }
    }
  }
  if (shore_bombard != 0) {
    name_str = civ_get_adjective(civ);
    text_set_string_arg(0,(const char *)name_str);
    text_set_label_arg(1,unit_types[units[unit].type].name);
    name_str = civ_get_adjective(civ2);
    text_set_string_arg(2,(const char *)name_str);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_pearlharbor_2,unit);
          }
          else {
            net_send_text_args(0x30,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit,0)
            ;
          }
        }
      }
    }
  }
  if (improvement != IMP_NOTHING) {
    text_set_label_arg(1,improvements[improvement].name);
    text_set_string_arg(2,(const char *)&cities[sprite_picker_sprite_block.combat_defender_city].name);
    if ((sdi_bonus == 0) || (improvement == IMP_SDI_DEFENSE)) {
      if (autoplay.active == 0) {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if (civ_sees[civ3] != 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_improvement((const char *)&str_battery_2,improvement,8);
            }
            else {
              net_send_text_args(0x32,net.net_slots[net.net_civ_slots[civ3]].id,3,0,improvement
                                 ,0);
            }
          }
        }
      }
    }
    else {
      text_set_label_arg(3,improvements[IMP_SDI_DEFENSE].name);
      if (autoplay.active == 0) {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if (civ_sees[civ3] != 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_improvement((const char *)&str_battery2_2,improvement,8);
            }
            else {
              net_send_text_args(0x31,net.net_slots[net.net_civ_slots[civ3]].id,4,0,improvement
                                 ,0);
            }
          }
        }
      }
    }
  }
  if (scramble_msg != 0) {
    name_str = civ_get_adjective(civ2);
    text_set_string_arg(0,(const char *)name_str);
    text_set_label_arg(1,unit_types[units[unit2].type].name);
    text_set_string_arg(2,(const char *)&cities[sprite_picker_sprite_block.combat_defender_city].name);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_scramble_2,unit2);
          }
          else {
            net_send_text_args(0x33,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit2,0)
            ;
          }
        }
      }
    }
  }
  if ((unit_types[units[unit].type].domain == DOMAIN_LAND) &&
     (city2 = tile_is_ocean(x2,y2), city2 != 0)) {
    name_str = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)name_str);
    text_set_label_arg(2,unit_types[units[unit].type].name);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_amphibmotize_2,unit);
          }
          else {
            net_send_text_args(0x34,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit,0)
            ;
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the attacker's sound. Returns the kind of
   explosion shown (0: the default). */
static int unit_resolve_attack_sound(iptr unit,iptr unit2)
{
  byte unit_type;
  int explosion_kind;
  explosion_kind = 0;
  unit_type = units[unit].type;
  prefs_main_menu_choice_block.snd_last_effect_id = -1;
  prefs_main_menu_choice_block.snd_last_effect_arg = -1;
  if (unit_type == UNIT_TEST_1) {
    snd_play_effect(SND_EXTRA1,1,0,0);
  }
  else if (unit_type == UNIT_TEST_2) {
    snd_play_effect(SND_EXTRA2,1,0,0);
  }
  else if (unit_type == UNIT_TEST_3) {
    snd_play_effect(SND_EXTRA3,1,0,0);
  }
  else if (unit_type == UNIT_TEST_4) {
    snd_play_effect(SND_EXTRA4,1,0,0);
  }
  else if (unit_type == UNIT_TEST_5) {
    snd_play_effect(SND_EXTRA5,1,0,0);
  }
  else if (unit_type == UNIT_TEST_6) {
    snd_play_effect(SND_EXTRA6,1,0,0);
  }
  else if (unit_type == UNIT_TEST_7) {
    snd_play_effect(SND_EXTRA7,1,0,0);
  }
  else if (unit_type == UNIT_TEST_8) {
    snd_play_effect(SND_EXTRA8,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_LAND) {
    snd_play_effect(SND_CUSTOM1,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_SHIP) {
    snd_play_effect(SND_CUSTOM2,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_AIR) {
    snd_play_effect(SND_CUSTOM3,1,0,0);
  }
  else if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
    if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
      if (unit_types[units[unit2].type].domain == DOMAIN_AIR) {
        if (units[unit].type < 0x1e) {
          snd_play_effect(SND_AIRCOMBT,0,0,0);
        }
        else {
          snd_play_effect(SND_JETCOMBT,0,0,0);
        }
      }
      else if (unit_types[units[unit].type].range == '\0') {
        snd_play_effect(SND_HELISHOT,1,0,0);
      }
      else if (units[unit].type < 0x1e) {
        snd_play_effect(SND_DIVEBOMB,1,0,0);
      }
      else {
        snd_play_effect(SND_JETBOMB,1,0,0);
      }
    }
    else if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
      if ((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) == 0) {
        explosion_kind = 6;
        if (((unit_type == UNIT_BATTLESHIP) || (unit_type == UNIT_CRUISER)) || ((unit_type == UNIT_AEGIS_CRUISER || (unit_type == UNIT_DESTROYER)))) {
          explosion_kind = 0x2e;
        }
      }
      else {
        snd_play_effect(SND_TORPEDOS,1,0,0);
      }
    }
    else if (unit_type == UNIT_ELEPHANT) {
      snd_play_effect(SND_ELEPHANT,1,0,0);
    }
    else if ((((unit_type == UNIT_HORSEMEN) || (unit_type == UNIT_CHARIOT)) || (unit_type == UNIT_KNIGHTS)) || (unit_type == UNIT_CRUSADERS)) {
      snd_play_effect(SND_SWRDHORS,1,0,0);
    }
    else if ((unit_type == UNIT_DRAGOONS) || (unit_type == UNIT_CAVALRY)) {
      snd_play_effect(SND_CAVALRY,1,0,0);
    }
    else if (((unit_type == UNIT_MUSKETEERS) || (unit_type == UNIT_RIFLEMEN)) || ((unit_type == UNIT_ALPINE_TROOPS || (unit_type == UNIT_PARTISANS)))) {
      snd_play_effect(SND_INFANTRY,1,0,0);
    }
    else if ((((unit_type == UNIT_FANATICS) || (unit_type == UNIT_PARATROOPERS)) || (unit_type == UNIT_MARINES)) || (unit_type == UNIT_MECH_INF)) {
      snd_play_effect(SND_MCHNGUNS,1,0,0);
    }
    else if ((unit_type < 0x16) || (0x1a < unit_type)) {
      snd_play_effect(SND_SWORDFGT,1,0,0);
    }
    else if (unit_type == UNIT_CATAPULT) {
      snd_play_effect(SND_CATAPULT,1,0,0);
    }
    else {
      explosion_kind = 0x28;
      if (0x17 < unit_type) {
        snd_play_effect(SND_FIRE,1,0,0);
        ui_delay_ticks(0x14);
      }
    }
  }
  else if ((char)unit_types[units[unit].type].attack < 'c') {
    snd_play_effect(SND_MISSILE,1,0,0);
  }
  return explosion_kind;
}

/* Part of unit_resolve_attack (00580341): a nuclear missile strikes instead. */
static void unit_resolve_attack_nuclear(iptr unit,uptr civ,uptr civ2,byte civ4,iptr x,int y,int x2,
                                                   int y2,int *civ_sees)
{
  int unit3;
  uptr civ3;
  combat_anim.defender = 0xffffffff;
  unit3 = unit_nuclear_attack(civ,x,y,1);
  if (unit3 == 0) {
    unit_delete(unit,1);
    map_redraw_tile_all_views(x2,y2);
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id
                             ,x2,y2,0,0,0,0,0,0);
        }
      }
    }
  }
  else {
    *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
  }
  if (((unit3 != 0) && ((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0)) &&
     (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) {
    ai_respond_to_attack(civ,x,y);
  }
}

/* Part of unit_resolve_attack (00580341): a combat round's hit: the victim's
   damage, and an explosion shown at (ex,ey) when its health bar drops a step. */
static void unit_resolve_attack_hit(iptr victim,int firepower,iptr ex,int ey,iptr x,int y,int x2,
                                               int y2,bool any_viewer,int anim_step,int explosion_kind,
                                               int *civ_sees,bool &explosion_shown)
{
  byte unit_type;
  int city2;
  uptr civ3;

  unit_type = units[victim].damage;
  units[victim].damage += (char)firepower;
  if (((any_viewer) && (anim_step != 0)) &&
     ((int)((longlong)(ulonglong)units[victim].damage / (longlong)anim_step) !=
      (int)(uint)unit_type / anim_step)) {
    explosion_shown = true;
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_animate_explosion(ex,ey,explosion_kind);
          city2 = unit_hp_remaining(victim);
          if (city2 != 0) {
            map_redraw_combat_tiles(x,y,x2,y2);
          }
        }
        else {
          net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,ex,ey,
                             explosion_kind,0,0,0,0,0);
          city2 = unit_hp_remaining(victim);
          if (city2 != 0) {
            net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,x,
                               y,x2,y2,0,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the combat rounds. Returns whether the
   attacker won. */
static uint unit_resolve_attack_fight(iptr unit,iptr unit2,iptr x,int y,int x2,int y2,
                                                 int attack_power,int defense2,int att_firepower,
                                                 int def_firepower,bool defender_reroll,bool any_viewer,
                                                 int explosion_kind,int *civ_sees,bool &explosion_shown)
{
  int city2;
  int def_roll2;
  int att_roll2;
  int def_roll;
  int att_roll;
  sbyte small_hp;
  uint attacker_won;
  int anim_step;
  uptr civ3;
  if (((char)unit_types[units[unit2].type].hit_points < 30) &&
     ((char)unit_types[units[unit].type].hit_points < 30)) {
    small_hp = 1;
  }
  else {
    small_hp = 0;
  }
  anim_step = 10 >> small_hp;
  if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) {
    anim_step = 0;
    explosion_shown = false;
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_animate_explosion(x,y,explosion_kind);
        }
        else {
          net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id
                             ,x,y,explosion_kind,0,0,0,0,0);
        }
      }
    }
  }
  for (;;) {
    city2 = unit_hp_remaining(unit);
    if ((city2 == 0) || (city2 = unit_hp_remaining(unit2), city2 == 0)) {
      attacker_won = unit_hp_remaining(unit);
      break;
    }
    if (attack_power == 1 || attack_power - 1 < 0) {
      att_roll = 0;
    }
    else {
      att_roll = crt_rand_at("unit_resolve_attack");
      att_roll = att_roll % attack_power;
    }
    if (defense2 == 1 || defense2 - 1 < 0) {
      def_roll = 0;
    }
    else {
      def_roll = crt_rand_at("unit_resolve_attack");
      def_roll = def_roll % defense2;
    }
    attacker_won = (uint)(def_roll < att_roll);
    if ((defender_reroll) && (attacker_won != 0)) {
      if (attack_power == 1 || attack_power - 1 < 0) {
        att_roll2 = 0;
      }
      else {
        att_roll2 = crt_rand_at("unit_resolve_attack");
        att_roll2 = att_roll2 % attack_power;
      }
      if (defense2 == 1 || defense2 - 1 < 0) {
        def_roll2 = 0;
      }
      else {
        def_roll2 = crt_rand_at("unit_resolve_attack");
        def_roll2 = def_roll2 % defense2;
      }
      if (att_roll2 < def_roll2) {
        attacker_won = 0;
      }
    }
    if (attacker_won != 0) {
      unit_resolve_attack_hit(unit2,att_firepower,x,y,x,y,x2,y2,any_viewer,anim_step,explosion_kind,civ_sees,
                              explosion_shown);
    }
    else {
      unit_resolve_attack_hit(unit,def_firepower,x2,y2,x,y,x2,y2,any_viewer,anim_step,explosion_kind,civ_sees,
                              explosion_shown);
    }
    if ((game.rules & RULE_NORMAL_COMBAT) == 0) break;
  }
  return attacker_won;
}

/* Part of unit_resolve_attack (00580341): the end of the combat shown: its sound,
   the loser's explosion. */
static void unit_resolve_attack_show_result(iptr unit,iptr unit2,iptr x,int y,int x2,int y2,
                                                       uint attacker_won,bool any_viewer,
                                                       bool explosion_shown,int explosion_kind,int *civ_sees)
{
  uptr civ3;
  if (any_viewer) {
    prefs_main_menu_choice_block.snd_last_effect_id = -1;
    if ((((attacker_won == 0) || (unit_types[units[unit2].type].domain != DOMAIN_AIR)) ||
        (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) &&
       (((attacker_won != 0 || (unit_types[units[unit].type].domain != DOMAIN_AIR)) ||
        ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)))) {
      if (explosion_kind != 0) {
        snd_play_effect(SND_LARGEXPL,1,0,0);
      }
    }
    else if (attacker_won == 0) {
      if (units[unit].type < 0x1e) {
        snd_play_effect(SND_DIVCRASH,1,0,0);
      }
      else {
        snd_play_effect(SND_JETCRASH,1,0,0);
      }
    }
    else if (units[unit2].type < 0x1e) {
      snd_play_effect(SND_DIVCRASH,1,0,0);
    }
    else {
      snd_play_effect(SND_JETCRASH,1,0,0);
    }
    if ((2 < game.multiplayer) && (prefs_main_menu_choice_block.snd_last_effect_id != -1)) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_SOUND_PLAY,net.net_slots[net.net_civ_slots[civ3]].id,
                             prefs_main_menu_choice_block.snd_last_effect_id,prefs_main_menu_choice_block.snd_last_effect_arg,0,0,0,0,0,0);
        }
      }
    }
  }
  net_send_state_diff(0xff,2,0,0,0);
  if (any_viewer) {
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_redraw_combat_tiles(x,y,x2,y2);
          if (attacker_won == 0) {
            if (!explosion_shown) {
              map_animate_explosion(x2,y2,0);
            }
          }
          else if (!explosion_shown) {
            map_animate_explosion(x,y,0);
          }
        }
        else {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
          if (attacker_won == 0) {
            if (!explosion_shown) {
              net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,x2,y2,0,
                                 0,0,0,0,0);
            }
          }
          else if (!explosion_shown) {
            net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id
                               ,x,y,0,0,0,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the kill counted, and the winner may
   become a veteran. */
static void unit_resolve_attack_veterans(iptr unit,iptr unit2,uptr civ,uptr civ2,uint attacker_won,
                                                    int attack_power,int defense2)
{
  char *kill_count_ptr;
  int city2;
  int def_vet_roll;
  int att_vet_roll;
  if (attacker_won == 0) {
    kill_count_ptr = (char *)(civ2 + (iptr)((u8 *)&civs + 80) + civ * 0x594);
    *kill_count_ptr = *kill_count_ptr + 1;
    if (defense2 + attack_power == 1 || defense2 + attack_power - 1 < 0) {
      def_vet_roll = 0;
    }
    else {
      def_vet_roll = crt_rand_at("unit_resolve_attack");
      def_vet_roll = def_vet_roll % (defense2 + attack_power);
    }
    if ((def_vet_roll <= attack_power) || (city2 = civ_has_wonder(civ2,WONDER_SUN_TZU), city2 != 0)) {
      unit_promote_veteran(unit2);
    }
  }
  else {
    *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
    if (defense2 + attack_power == 1 || defense2 + attack_power - 1 < 0) {
      att_vet_roll = 0;
    }
    else {
      att_vet_roll = crt_rand_at("unit_resolve_attack");
      att_vet_roll = att_vet_roll % (defense2 + attack_power);
    }
    if ((att_vet_roll <= defense2) || (city2 = civ_has_wonder(civ,WONDER_SUN_TZU), city2 != 0)) {
      unit_promote_veteran(unit);
    }
  }
}

/* Part of unit_resolve_attack (00580341): the loser (and its stack) is killed; a
   city that lost its defender shrinks or is destroyed. */
static void unit_resolve_attack_remove_loser(iptr unit,iptr unit2,uptr civ,uptr civ2,byte civ5,
                                                        iptr x,int y,int x2,int y2,iptr target_x,
                                                        int target_y,uint attacker_won,uptr &winner_civ,
                                                        uptr &loser_civ,bool &barbarian_ransom,
                                                        int &show_treaty_msg)
{
  iptr name_str;
  int city2;
  i32 tile_impr;
  uptr city;
  int view_civ2;
  uptr civ3;
  if (attacker_won == 0) {
    winner_civ = civ2;
    unit_lift_off_map(unit,1);
    unit_kill_stack_in_combat(unit,unit2,target_x,target_y);
    loser_civ = civ;
  }
  else {
    if ((civ2 == 0) && (4 < (char)unit_types[units[unit2].type].role)) {
      barbarian_ransom = true;
    }
    winner_civ = civ;
    if ((((int)sprite_picker_sprite_block.combat_defender_city < 0) &&
        (tile_impr = tile_improvements(x,y), ((byte)tile_impr & TILE_AIRBASE) != 0x40)) &&
       (city2 = tile_airbase_owner(x,y), city2 < 0)) {
      unit_kill_stack_in_combat(unit2,unit,target_x,target_y);
    }
    else {
      unit_kill_in_combat(unit2,unit,target_x,target_y);
    }
    city = sprite_picker_sprite_block.combat_defender_city;
    loser_civ = civ2;
    if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
      cities[sprite_picker_sprite_block.combat_defender_city].flags = cities[sprite_picker_sprite_block.combat_defender_city].flags | CITYF_DEFENDER_LOST;
      city2 = tile_is_ocean(x2,y2);
      if (((city2 == 0) && (city2 = city_has_improvement(city,IMP_CITY_WALLS), city2 == 0)) &&
         ((city2 = civ_has_wonder(civ2,WONDER_GREAT_WALL), city2 == 0 &&
          ((game.difficulty != DIFF_CHIEFTAIN || ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0))))))
      {
        cities[city].size--;
        if (cities[city].size == '\0') {
          city_delete(city,0);
          for (view_civ2 = 1; view_civ2 < 8; view_civ2++) {
            tile_update_known(x,y,view_civ2);
          }
          map_redraw_neighbors_all_views(x,y);
          city2 = civ_check_destroyed(civ2,civ);
          if (city2 != 0) {
            show_treaty_msg = 0;
            for (civ3 = 1; (int)civ3 < 8; civ3++) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 0;
            }
          }
        }
        else {
          city_reveal_to_civ(city,civ);
          city2 = tile_unit_owner(x,y);
          if ((city2 < 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
            name_str = tile_landmass(x,y);
            civ_reset_city_production(civ2,name_str);
          }
        }
      }
      city2 = tile_unit_owner(x,y);
      if ((city2 < 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
        ai_transports_raid_empty_city(city);
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): several units killed at once. */
static void unit_resolve_attack_multiple_kills(uptr winner_civ,uptr loser_civ)
{
  if ((autoplay.active == 0) && (1 < sprite_picker_sprite_block.combat_kill_count)) {
    text_set_number_arg(0,sprite_picker_sprite_block.combat_kill_count);
    if (human_civ[0] == loser_civ) {
      popup_game((const char *)&str_multiplelose_2);
    }
    else if (human_civ[0] == winner_civ) {
      popup_game((const char *)&str_multiplewin_2);
    }
    if (((game.multiplayer < 3) || ((1 << ((byte)loser_civ & 0x1f) & (uint)game.human_players) == 0))
       || (human_civ[0] == loser_civ)) {
      if (((2 < game.multiplayer) &&
          ((1 << ((byte)winner_civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (human_civ[0] != winner_civ)) {
        net_send_text_args(0x36,net.net_slots[net.net_civ_slots[winner_civ]].id,0,1
                           ,0,0);
      }
    }
    else {
      net_send_text_args(0x35,net.net_slots[net.net_civ_slots[loser_civ]].id,0,1,0
                         ,0);
    }
  }
}

/* Part of unit_resolve_attack (00580341): the ransom for a barbarian leader. */
static void unit_resolve_attack_ransom(uptr civ,int *civ_sees)
{
  uint ransom;
  uptr civ3;
  ransom = ((uint)game.barbarian_activity * 100) / 2;
  civs[civ].gold += ransom;
  text_set_number_arg(0,ransom);
  if ((human_civ[0] == civ) && (autoplay.active == 0)) {
    dlg_game_txt_with_unit_flags((const char *)&str_ransom_2,0x3e,0);
    status_draw_info(1);
  }
  else if ((2 < game.multiplayer) && (autoplay.active == 0)) {
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if ((civ_sees[civ3] != 0) && (human_civ[0] != civ3)) {
        net_send_text_args(0x37,net.net_slots[net.net_civ_slots[civ3]].id,1
                           ,0,0x3e,0);
        net_send_message(NM_INFO_STATUS,net.net_slots[net.net_civ_slots[civ3]].id,0
                           ,0,0,0,0,0,0,0);
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the treaty broken and the allies at war
   told. */
static void unit_resolve_attack_treaty_messages(uptr civ,uptr civ2,byte civ5,int show_treaty_msg,
                                                           int ally_warning)
{
  iptr name_str;
  uptr civ3;
  if ((autoplay.active == 0) && ((show_treaty_msg != 0 || (ally_warning != 0)))) {
    name_str = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(0,(const char *)name_str);
    name_str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)name_str);
    if (game.multiplayer < 3) {
      if (show_treaty_msg == 0) {
        if (ally_warning == 1) {
          popup_game((const char *)&str_allyunderattack_2);
          diplo_ally_demand_help(civ2,civ);
        }
        else {
          popup_game((const char *)&str_allyattacking_2);
        }
      }
      else {
        popup_game((const char *)&str_cancelpeace_4);
      }
    }
    else if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (*(int *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) != 0) {
          if (human_civ[0] == civ3) {
            popup_game((const char *)&str_cancelpeace_5);
          }
          else {
            net_send_text_args(0x3a,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
          }
        }
        if ((*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) != 0) && (autoplay.active == 0)) {
          if (human_civ[0] == civ3) {
            if (*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) == 1) {
              popup_game((const char *)&str_allyunderattack_3);
              diplo_ally_demand_help(civ2,civ);
            }
            else {
              popup_game((const char *)&str_allyattacking_3);
            }
          }
          else if (*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) == 1) {
            net_send_text_args(0x3b,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
            if ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) {
              net_send_message(NM_ASK_ALLY,net.net_slots[net.net_civ_slots[civ3]].id,civ2,civ,0
                                 ,0,0,0,0,0);
            }
          }
          else {
            net_send_text_args(0x3c,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the viewers are shown the two units
   (the attacker drawn on its tile, the defender on top of its stack). */
static void unit_resolve_attack_show_start(iptr unit,iptr unit2,uptr civ,uptr civ2,iptr x,int y,
                                                      int x2,int y2,int *civ_sees)
{
  int city2;
  uptr msg_civ;
  int view;
  uptr civ3;
    unit_set_seen_by(unit,civ2);
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          city2 = mapviews_ensure_visible(x,y,civ);
          if (city2 == 0) {
            map_redraw_tile_all_views(x2,y2);
          }
        }
        else {
          msg_civ = civ2;
          if (civ == civ3) {
            msg_civ = civ;
          }
          net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,msg_civ,0,0,0,0,0);
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id,
                             x2,y2,0,0,0,0,0,0);
        }
      }
    }
    unit_lift_off_map(unit,1);
    for (view = 0; view < 8; view++) {
      if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
        map_views_block.map_view_recs[view].draw_unit_at(unit2);
        map_views_block.map_view_recs[view].blit_tile(x,y);
      }
    }
    if ((unit_types[units[unit].type].flags & (UTYPE_SUBMARINE | UTYPE_DESTROYED_AFTER_ATTACK)) != 0) {
      ui_delay_ticks(10);
    }
}

/* Part of unit_resolve_attack (00580341): the attack animated (and relayed to the
   other human players). */
static void unit_resolve_attack_animate(iptr unit,iptr dir,iptr unit2,iptr x,int y,int x2,int y2,
                                                   int *civ_sees)
{
  uptr civ3;
    combat_anim.defender = unit2;
    combat_anim.defender_x = x;
    combat_anim.defender_y = y;
    if (2 < game.multiplayer) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_SET_SLIDE_SOUND,net.net_slots[net.net_civ_slots[civ3]].id,
                             prefs_main_menu_choice_block.snd_last_effect_id,prefs_main_menu_choice_block.snd_last_effect_arg,0,0,0,0,0,0);
          net_send_message(NM_SLIDE_PIECE,net.net_slots[net.net_civ_slots[civ3]].id,
                             unit,x2,y2,dir,combat_anim.defender,0,0,0);
        }
      }
    }
    map_animate_unit_move(unit,x2,y2,dir,0xffffffff,0xffffffff);
    unit_stack_relocate_all(unit,x2,y2,0);
    map_redraw_combat_tiles(x,y,x2,y2);
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
        }
      }
    }
}

/* Part of unit_resolve_attack (00580341): after a won attack: an aircraft's moves
   used, a missile spent, an AI's attack on an emptied city answered. */
static void unit_resolve_attack_after_win(uptr civ,byte civ4,iptr x,int y,int x2,int y2,
                                                     uint attacker_won,i32 &result)
{
  int unit3;
  int city2;
    unit3 = (int)game.moving_unit;
    if ((unit_types[units[unit3].type].domain == DOMAIN_AIR) &&
       (unit_types[units[unit3].type].range != 1)) {
      unit_exhaust_moves(unit3);
    }
    if ((((attacker_won == 0) || ((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0)) ||
        ((int)sprite_picker_sprite_block.combat_defender_city < 0)) || (city2 = tile_first_unit(x,y), -1 < city2)) {
      if ((attacker_won != 0) && ((unit_types[units[unit3].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) {
        unit_delete(unit3,1);
        result = 0;
      }
    }
    else {
      if ((unit_types[units[unit3].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
        if (!(((unit_types[units[unit3].type].domain == DOMAIN_AIR) &&
              (unit_types[units[unit3].type].range != '\0')) &&
             (units[unit3].counter = units[unit3].counter + 1,
             (char)unit_types[units[unit3].type].range <= (char)units[unit3].counter))) {
          ai_respond_to_attack(civ,x,y);
          result = 0;
        }
      }
      else {
        unit_delete(unit3,1);
        map_redraw_neighbors_all_views(x2,y2);
        ai_respond_to_attack(civ,x,y);
        result = 0;
      }
    }
}

/* 00580341 auto */
i32 __cdecl unit_resolve_attack(int unit,int dir,int execute)

{
  byte civ4;
  byte civ5;
  bool any_viewer;
  bool explosion_shown;
  bool barbarian_ransom;
  uptr civ;
  int x2;
  int y2;
  int y;
  iptr x;
  uptr civ2;
  iptr max_hp;
  int unit3;
  bool defender_reroll;
  int max_moves;
  uint attacker_won;
  iptr improvement;
  int ally_warning;
  int explosion_kind;
  int attack_power;
  uint moves_avail;
  int att_firepower;
  int defense2;
  uptr loser_civ;
  int civ_sees[8];
  i32 result;
  int def_firepower;
  int show_treaty_msg;
  int scramble_msg;
  int sdi_bonus;
  uptr winner_civ;
  uptr civ3;
  int target_y;
  int shore_bombard;
  iptr unit2;
  iptr target_x;
  
  scramble_msg = 0;
  improvement = 0;
  defender_reroll = false;
  barbarian_ransom = false;
  show_treaty_msg = 0;
  ally_warning = 0;
  shore_bombard = 0;
  sdi_bonus = 0;
  explosion_kind = 0;
  result = 1;
  for (civ3 = 1; (int)civ3 < 8; civ3++) {
    *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 0;
    *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 0;
  }
  civ4 = units[unit].owner;
  civ = (uptr)(char)civ4;
  x2 = (int)units[unit].x;
  y2 = (int)units[unit].y;
  attack_power = unit_attack_strength(unit);
  att_firepower = (int)(char)unit_types[units[unit].type].firepower;
  y = unit_moves_left(unit);
  if (y < (int)(uint)cosmic.road_move_mult) {
    moves_avail = unit_moves_left(unit);
  }
  else {
    moves_avail = (uint)cosmic.road_move_mult;
  }
  if (execute != 0) {
    attack_power = (int)(attack_power * moves_avail) / (int)(uint)cosmic.road_move_mult;
  }
  x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
  y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y2;
  target_y = y;
  target_x = x;
  unit2 = tile_first_unit(x,y);
  if (unit2 < 0) {
    return 999;
  }
  unit2 = unit_stack_best_defender(unit2,unit);
  civ5 = units[unit2].owner;
  civ2 = (uptr)(char)civ5;
  defense2 = unit_defense_strength(unit2,0,unit);
  def_firepower = (int)(char)unit_types[units[unit2].type].firepower;
  if (((unit_types[units[unit].type].role == ROLE_AIR_SUPERIORITY) &&
      (unit_types[units[unit2].type].domain == DOMAIN_AIR)) &&
     (unit_types[units[unit2].type].range == '\0')) {
    defense2 -= (defense2 >> 1);
    def_firepower = 1;
  }
  if ((unit_types[units[unit2].type].flags & UTYPE_X2_VS_HORSE) != 0) {
    max_moves = unit_max_moves(unit);
    if (mp_doubles.moves != 0) {
      max_moves -= (int)(char)unit_types[units[unit].type].move / 2;
    }
    if ((((uint)cosmic.road_move_mult * 2 == max_moves) &&
        (unit_types[units[unit].type].domain == DOMAIN_LAND)) &&
       (max_hp = unit_max_hp(unit), max_hp == 10)) {
      defense2 += (defense2 >> 1);
    }
  }
  if (((unit_types[units[unit2].type].flags & UTYPE_X2_VS_AIR) != 0) &&
     (unit_types[units[unit].type].domain == DOMAIN_AIR)) {
    if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
      defense2 = defense2 * 3;
    }
    else {
      defense2 = defense2 * 5;
    }
  }
  unit_resolve_attack_city_defenses(unit,unit2,execute,defense2,improvement,sdi_bonus,scramble_msg);
  if ((units[unit].type == UNIT_PARTISANS) && (unit_types[units[unit2].type].attack == '\0')) {
    attack_power = attack_power << 3;
  }
  if ((unit_types[units[unit].type].domain == DOMAIN_SEA) &&
     (unit_types[units[unit2].type].domain == DOMAIN_LAND)) {
    att_firepower = 1;
    def_firepower = 1;
  }
  if ((unit_types[units[unit2].type].flags & UTYPE_SUBMARINE) != 0) {
    def_firepower = 1;
  }
  if (((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0) &&
     ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
    civ_clear_standing_goals_near(civ2,0x12,x,y,3);
    civ_add_standing_goal(civ2,x,y,0x12,2);
  }
  if (((unit_types[units[unit2].type].domain == DOMAIN_SEA) &&
      (unit3 = tile_is_ocean(x,y), unit3 == 0)) &&
     (unit_types[units[unit].type].domain != DOMAIN_SEA)) {
    def_firepower = 1;
    if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
      shore_bombard = 1;
      att_firepower = att_firepower << 1;
    }
    else {
      shore_bombard = 1;
      att_firepower = att_firepower << 1;
    }
  }
  unit_resolve_attack_barbarian_odds(unit2,civ,civ2,civ5,attack_power,defense2,defender_reroll);
  if (execute == 0) {
    if ((game.rules & RULE_NORMAL_COMBAT) != 0) {
      x2 = unit_hp_remaining(unit);
      attack_power = (x2 + att_firepower * 8) * attack_power;
      x2 = unit_hp_remaining(unit2);
      defense2 = (x2 + def_firepower * 8) * defense2;
    }
    for (; 3999 < attack_power; attack_power = attack_power >> 1) {
      defense2 = defense2 >> 1;
    }
    x2 = (attack_power << 3) / (defense2 + 1);
    if (x2 < 0x401) {
      return x2;
    }
    return 0x400;
  }
  if ((unit_sound_editor_window_block.unit_move_flags & 2) != 0) {
    units[unit].moves_used += cosmic.road_move_mult;
  }
  if (((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0) &&
     (unit3 = diplo_confirm_attack(civ,civ2,0xd), unit3 != 0)) {
    return 1;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    return 1;
  }
  if ((civ != 0) && (civ2 != 0)) {
    game.peace_turns = 0;
  }
  diplo_set_treaty_flags(civ,civ2,0x200);
  unit_resolve_attack_break_treaty(unit,civ,civ2,civ4,civ5,attack_power,show_treaty_msg,ally_warning);
  unit3 = diplo_force_war(civ,civ2);
  if (civ != 0) {
    if ((game.difficulty < DIFF_PRINCE) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
      attack_power = attack_power >> 1;
    }
    if ((game.difficulty == DIFF_CHIEFTAIN) && ((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0)) {
      attack_power = attack_power << 1;
    }
  }
  any_viewer = unit_resolve_attack_viewers(unit,unit2,civ,civ2,civ_sees);
  if (any_viewer) {
    unit_resolve_attack_show_start(unit,unit2,civ,civ2,x,y,x2,y2,civ_sees);
    unit_resolve_attack_announce(unit,unit2,civ,civ2,civ5,x,y,x2,y2,improvement,sdi_bonus,scramble_msg,
                                 shore_bombard,civ_sees);
    explosion_kind = unit_resolve_attack_sound(unit,unit2);
    unit_resolve_attack_animate(unit,dir,unit2,x,y,x2,y2,civ_sees);
  }
  if ('b' < (char)unit_types[units[unit].type].attack) {
    unit_resolve_attack_nuclear(unit,civ,civ2,civ4,x,y,x2,y2,civ_sees);
    return 0;
  }
  explosion_shown = false;
  if ((att_firepower == 0) && (def_firepower == 0)) {
    attacker_won = 0;
  }
  else {
    attacker_won = unit_resolve_attack_fight(unit,unit2,x,y,x2,y2,attack_power,defense2,att_firepower,
                                             def_firepower,defender_reroll,any_viewer,explosion_kind,
                                             civ_sees,explosion_shown);
  }
  if (attacker_won == 0) {
    turn_flags.active_unit_lost_in_combat = 1;
  }
  unit_resolve_attack_show_result(unit,unit2,x,y,x2,y2,attacker_won,any_viewer,explosion_shown,
                                  explosion_kind,civ_sees);
  combat_anim.defender = 0xffffffff;
  sprite_picker_sprite_block.combat_kill_count = 0;
  unit_resolve_attack_veterans(unit,unit2,civ,civ2,attacker_won,attack_power,defense2);
  unit_resolve_attack_remove_loser(unit,unit2,civ,civ2,civ5,x,y,x2,y2,target_x,target_y,attacker_won,
                                   winner_civ,loser_civ,barbarian_ransom,show_treaty_msg);
  net_send_state_diff(0xff,2,0,0,0);
  if (any_viewer) {
    if (2 < game.multiplayer) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
        }
      }
    }
    map_redraw_combat_tiles(x,y,x2,y2);
  }
  unit_resolve_attack_multiple_kills(winner_civ,loser_civ);
  if (barbarian_ransom) {
    unit_resolve_attack_ransom(civ,civ_sees);
  }
  if (unit3 != 0) {
    diplo_allies_join_war(civ2,civ);
  }
  unit_resolve_attack_treaty_messages(civ,civ2,civ5,show_treaty_msg,ally_warning);
  advisor_refresh_if_open(2);
  if (2 < game.multiplayer) {
    net_send_message(NM_REPORTS_UPDATE,0xff,2,0,0,0,0,0,0,0);
  }
  if (attacker_won == 0) {
    result = 0;
  }
  else {
    unit_resolve_attack_after_win(civ,civ4,x,y,x2,y2,attacker_won,result);
  }
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
        net_send_message(NM_ABORT_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,0,0
                           ,0,0,0,0,0,0);
        net_send_message(NM_DRAW_AREA,net.net_slots[net.net_civ_slots[civ3]].id,
                           x2,y2,0,0,0,0,0,0);
      }
    }
  }
  return result;
}
