/* The game, cities: what a city builds (the AI's choice) and what it completes: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* The state city_choose_production (00498e8b) shares between its parts: the
   survey of the city, its civ and the other cities, and the best choices so
   far. */
struct ChooseProdState {
  byte civ5;
  uptr civ;
  iptr landmass2;
  uint policy;
  uint threatened;
  int space_race;
  int food_surplus;
  int ocean_tiles;
  int shield_surplus;
  int net_trade;
  int wonder_in_progress;
  int wonders_here;
  int wonders_owned;
  int unrest_state;
  int nearest_dist;
  int enemy_strength;
  int farmland_count;
  int wonders_building;
  int airport_count;
  int port_count;
  int unit_total;
  int barracks_count;
  int rivals_ahead;
  int same_build_count;
  int foreign_same_build;
  int space_builders;
  int human_space_builders;
  int palace_count;
  int palace_size;
  int flagged_settlers;
  int homeless_settlers;
  int expand_bonus;
  int settle_goals;
  int enemy_offshore;
  int defense_urgent;
  int defense_need;
  int naval_units;
  /* the original's city loop counter: the palace score reads the value the
     survey left in it (stale) */
  iptr city2;
  int best_build_score;
  int best_unit_score;
  int choice_score;
  iptr best_build;
  iptr best_unit;
  iptr choice;
  int best_raw_score;
  /* the main function's arrays */
  i8 *wonder_builders;
  int *units_in_build;
  int *unit_counts;
};

/* Part of city_choose_production (00498e8b): the tiles around the city:
   ocean tiles, enemy strength next to it, farmland, and for an AI city short
   of food a tile to irrigate, irrigated for free where no human sees it. */
static void city_choose_production_scan_tiles(ChooseProdState *s,iptr city)
{
  byte terrain_byte;
  int ret;
  i32 bits;
  uint adjusted;
  byte *tile;
  Tile *map_tile;
  iptr unit;
  int want_irrigation;
  int y2;
  iptr x2;
  uint tmp;
  uptr terrain2;
  uptr civ2;
  iptr irrigate_tile;
  iptr unit_type2;

  irrigate_tile = -1;
  want_irrigation = 0;
  if ((((s->food_surplus == 0) && ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
      ((cities[city].flags & CITYF_AI_SETTLER_NEAR) == 0)) && (city_calc.settlers_supported == 0)) {
    want_irrigation = 1;
  }
  s->enemy_strength = 0;
  s->farmland_count = 0;
  for (unit_type2 = 0; unit_type2 < 0x14; unit_type2++) {
    x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[unit_type2]);
    y2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[unit_type2];
    ret = map_in_bounds(x2,y2);
    if (ret != 0) {
      terrain_byte = tile_terrain(x2,y2);
      terrain2 = (uptr)terrain_byte;
      if (terrain2 == TERRAIN_OCEAN) {
        s->ocean_tiles++;
        ret = city_tile_worked(city,unit_type2);
        if (ret != 0) {
          s->ocean_tiles += 3;
        }
      }
      civ2 = tile_unit_owner(x2,y2);
      if (((-1 < (int)civ2) && (civ2 != s->civ)) &&
         ((unit = tile_first_unit(x2,y2), -1 < unit &&
          ((int)units[unit].owner != s->civ)))) {
        ret = unit_stack_sum(unit,2);
        s->enemy_strength += ret;
      }
      bits = tile_improvements(x2,y2);
      if (((byte)bits & TILE_FARMLAND) == 0xc) {
        s->farmland_count++;
      }
      if (((want_irrigation != 0) && (ret = ai_tile_improve_choice(s->civ,x2,y2), ret == 1)) &&
         ((adjusted = tile_improvements(x2,y2), (adjusted & TILE_IRRIGATION) == 0 &&
          (terrain_types[terrain2].irrigate_to < '\0')))) {
        irrigate_tile = unit_type2;
        ret = tile_resource(x2,y2);
        if ((ret != 0) && (terrain2 == TERRAIN_PLAINS)) {
          want_irrigation = 0;
        }
      }
    }
  }
  if (-1 < irrigate_tile) {
    x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[irrigate_tile]);
    y2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[irrigate_tile];
    map_tile = tile_at(x2,y2);
    if ((map_tile->visible & game.human_players) == 0) {
      tmp = 4;
      tile = (byte *)tile_at(x2,y2);
      if ((*tile & 0x80) == 0) {
        tmp = tmp | 0x10;
      }
      tile_set_improvements(x2,y2,tmp,1,1);
      s->food_surplus++;
    }
  }
}

/* Part of city_choose_production (00498e8b): the civ's other cities: palaces,
   the same build, spaceship parts, barracks, airports, ports and wonders being
   built on the landmass, the units being built; the wonders other civs build. */
static void city_choose_production_survey_cities(ChooseProdState *s,iptr city)
{
  int ret;

  s->wonders_building = 0;
  s->airport_count = 0;
  s->port_count = 0;
  s->unit_total = 0;
  ret = civ_has_wonder(s->civ,WONDER_SUN_TZU);
  if (ret == 0) {
    s->barracks_count = 0;
  }
  else {
    s->barracks_count = 4;
  }
  s->rivals_ahead = 0;
  s->same_build_count = 0;
  s->foreign_same_build = 0;
  s->space_builders = 0;
  s->human_space_builders = 0;
  s->palace_count = 0;
  s->palace_size = 0;
  for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
    if (cities[s->city2].id != 0) {
      if ((int)cities[s->city2].owner == s->civ) {
        ret = city_has_improvement(s->city2,IMP_PALACE);
        if (((ret != 0) || (cities[s->city2].building == -IMP_PALACE)) &&
           (s->palace_count = s->palace_count + 1, s->palace_size < cities[s->city2].size)) {
          s->palace_size = (int)cities[s->city2].size;
        }
        if (s->city2 != city) {
          if ((cities[city].building == cities[s->city2].building) &&
             (s->same_build_count = s->same_build_count + 1,
             cities[city].total_shields < cities[s->city2].total_shields)) {
            s->rivals_ahead++;
          }
          if ((-0x26 < cities[s->city2].building) && (cities[s->city2].building < -0x22)) {
            s->space_builders++;
          }
          ret = tile_landmass(cities[s->city2].x,cities[s->city2].y);
          if (ret == s->landmass2) {
            ret = city_has_improvement(s->city2,IMP_BARRACKS);
            if (ret != 0) {
              s->barracks_count++;
            }
            ret = city_has_improvement(s->city2,IMP_AIRPORT);
            if (ret != 0) {
              s->airport_count++;
            }
            ret = city_has_improvement(s->city2,IMP_PORT_FACILITY);
            if (ret != 0) {
              s->port_count++;
            }
            if (cities[s->city2].building == -IMP_BARRACKS) {
              s->barracks_count++;
            }
            if (cities[s->city2].building < -0x26) {
              s->wonders_building++;
              if (cities[city].total_shields < cities[s->city2].total_shields) {
                *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
                     *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] |
                     (byte)(1 << (s->civ5 & 0x1f));
              }
              ret = (uint)improvements[-(int)cities[s->city2].building].cost * city_calc.shield_rows;
              if (ret - cities[s->city2].shield_box == 0 || ret < cities[s->city2].shield_box)
              {
                *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
                     *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] | 1;
              }
            }
            if (-1 < cities[s->city2].building) {
              s->units_in_build[cities[s->city2].building] = s->units_in_build[cities[s->city2].building] + 1;
            }
          }
        }
      }
      else {
        if (cities[city].building == cities[s->city2].building) {
          s->foreign_same_build++;
        }
        if (cities[s->city2].building < -0x26) {
          *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] =
               *(byte *)&s->wonder_builders[-0x27 - cities[s->city2].building] |
               (byte)(1 << (cities[s->city2].owner & 0x1fU));
        }
        if (((((uint)game.human_players & 1 << (cities[s->city2].owner & 0x1fU)) != 0) &&
            (-0x26 < cities[s->city2].building)) && (cities[s->city2].building < -0x22)) {
          s->human_space_builders++;
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the civ's units: settlers
   flagged or without a home city, units of each type on the landmass.
   unit_type is the original's loop counter, left at 0x3e by the loop that
   clears the counts: the settler test reads it so. */
static void city_choose_production_count_units(ChooseProdState *s,iptr unit_type)
{
  int ret;
  iptr unit;

  s->flagged_settlers = 0;
  s->homeless_settlers = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && ((int)units[unit].owner == s->civ)) {
      s->unit_total++;
      if (unit_types[unit_type].role == ROLE_SETTLE) {
        if ((units[unit].flags & UNITF_AI_COLONIST) != 0) {
          s->flagged_settlers++;
        }
        if (units[unit].home_city == -1) {
          s->homeless_settlers++;
          continue;
        }
      }
      ret = tile_landmass(units[unit].x,units[unit].y);
      if (ret == s->landmass2) {
        s->unit_counts[units[unit].type] = s->unit_counts[units[unit].type] + 1;
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the bonus for settlers when the
   city is far from the civ's other cities or big. */
static void city_choose_production_expand_bonus(ChooseProdState *s,iptr city)
{
  int dist;

  if ((((cities[city].flags & (CITYF_AI_SETTLER_NEAR | CITYF_AI_TERRAFORM_GOAL)) == 0x80000) && (s->enemy_strength == 0)) &&
     ((city_calc.settlers_supported == 0 && ((s->policy != 1 && (2 < cities[city].size)))))) {
    s->choice_score = 9999;
    for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
      if ((((cities[s->city2].id != 0) && (s->city2 != city)) &&
          ((int)cities[s->city2].owner == s->civ)) &&
         (dist = map_distance((int)cities[city].x,(int)cities[city].y,
                                        (int)cities[s->city2].x,(int)cities[s->city2].y),
         dist < s->choice_score)) {
        s->choice_score = dist;
      }
    }
    if (8 < s->choice_score) {
      s->expand_bonus = 1;
    }
    if (((6 < cities[city].size) && (GOV_FUNDAMENTALISM < civs[s->civ].government)) &&
       ((0 < s->food_surplus && (s->policy != 0)))) {
      s->expand_bonus++;
    }
  }
}

/* Part of city_choose_production (00498e8b): the barbarians' choice: keep the
   unit being built or its upgrade. */
static iptr city_choose_production_barbarian(iptr city)
{
  int ret;
  iptr obsolete_by2;
  iptr unit_type;
  iptr unit_type2;

  if (cities[city].building < 1) {
    if (game.advance_known_by[ADV_EXPLOSIVES] != '\0') {
      return 0x13;
    }
    return 5;
  }
  unit_type = (iptr)cities[city].building;
  obsolete_by2 = (iptr)unit_types[unit_type].obsolete_by;
  if (obsolete_by2 < 0) {
    return unit_type;
  }
  if (game.difficulty == DIFF_CHIEFTAIN) {
    return unit_type;
  }
  ret = bits_count8((int)(char)game.advance_known_by[obsolete_by2]);
  if (ret < 2) {
    return unit_type;
  }
  if (((game.human_players & game.advance_known_by[obsolete_by2]) == 0) && (game.difficulty < DIFF_EMPEROR)) {
    return unit_type;
  }
  unit_type2 = 0;
  while( true ) {
    if (0x3d < unit_type2) {
      return unit_type;
    }
    if ((unit_types[unit_type2].prerequisite == obsolete_by2) &&
       (unit_types[unit_type].role == unit_types[unit_type2].role)) break;
    unit_type2++;
  }
  return unit_type2;
}

/* Part of city_choose_production (00498e8b): the defenders the city needs
   (defense_need, defense_urgent) and its naval units. */
static void city_choose_production_defense_need(ChooseProdState *s,iptr city)
{
  int ret;
  uint adjusted;
  int extra_defender;
  iptr unit;
  int under_threat;
  uint defenders_wanted;
  uint tmp;
  int defenders;
  int size_per_defender;

  under_threat = 0;
  s->defense_urgent = 0;
  s->defense_need = 0;
  unit = tile_first_unit((int)cities[city].x,(int)cities[city].y);
  defenders = unit_stack_count_role(unit,ROLE_DEFEND);
  ret = unit_stack_count_role(unit,ROLE_SEA_TRANSPORT);
  s->naval_units = unit_stack_count_role(unit,ROLE_NAVAL_SUPERIORITY);
  s->naval_units = (ret >> 1) + s->naval_units;
  if (defenders < 1) {
    s->defense_urgent = 2;
    s->defense_need = 2;
  }
  else {
    if ((under_threat == 0) &&
       (ret = tile_zoc_unit_adjacent((int)cities[city].x,(int)cities[city].y,s->civ), ret == 0)
       ) {
      size_per_defender = 4;
    }
    else {
      size_per_defender = 3;
    }
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret == 0) {
      if ((s->policy == 5) && (under_threat == 0)) {
        size_per_defender++;
      }
    }
    else {
      size_per_defender--;
    }
    if ((s->threatened == 0) && (*(char *)&civs[s->civ].continent_flags[s->landmass2] == '\0')) {
      extra_defender = 0;
    }
    else {
      extra_defender = 1;
    }
    defenders_wanted = extra_defender + (int)cities[city].size / size_per_defender;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = defenders_wanted;
    if (ret != 0) {
      tmp = 1;
      if (3 < cities[city].size) {
        tmp = 2;
      }
      if (7 < civs[s->civ].num_cities) {
        tmp = 3;
      }
      if (0xb < civs[s->civ].num_cities) {
        tmp = 4;
      }
      adjusted = tmp;
      if ((int)tmp <= (int)defenders_wanted) {
        adjusted = defenders_wanted;
      }
    }
    defenders_wanted = adjusted;
    if ((defenders < (int)defenders_wanted) && (s->defense_need = 1, defenders + 1 < (int)defenders_wanted)) {
      s->defense_urgent = 1;
    }
  }
}

/* Part of city_choose_production (00498e8b): an AI city sells the
   improvements its civ's wonders make redundant. */
static void city_choose_production_sell_redundant(ChooseProdState *s,iptr city)
{
  int ret;

  ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_GRANARY), ret != 0)) {
    city_set_improvement(city,3,0);
    civs[s->civ].gold += (uint)improvements[IMP_GRANARY].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_SETI);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_RESEARCH_LAB), ret != 0)) {
    city_set_improvement(city,0x1a,0);
    civs[s->civ].gold += (uint)improvements[IMP_RESEARCH_LAB].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_WOMENS_SUFFRAGE);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_POLICE_STATION), ret != 0)) {
    city_set_improvement(city,0x21,0);
    civs[s->civ].gold += (uint)improvements[IMP_POLICE_STATION].cost * city_calc.shield_rows;
  }
  ret = civ_has_wonder(s->civ,WONDER_MICHELANGELO);
  if ((ret != 0) && (ret = city_has_improvement(city,IMP_CATHEDRAL), ret != 0)) {
    city_set_improvement(city,0xb,0);
    civs[s->civ].gold += (uint)improvements[IMP_CATHEDRAL].cost * city_calc.shield_rows;
  }
}

/* Part of city_choose_production (00498e8b): an AI city with an airport
   airlifts a defender in from another (and a human's diplomat may see it). */
static void city_choose_production_airlift_defender(ChooseProdState *s,iptr city)
{
  char leader_trait;
  int ret;
  int count;
  iptr unit;
  int watchers;
  iptr unit3;

  ret = city_has_improvement(city,IMP_AIRPORT);
  if ((((ret != 0) && ((int)game.turn_civ == s->civ)) && ((cities[city].flags & CITYF_AIRLIFTED) == 0)
      ) && ((s->policy != 4 && (s->policy != 5)))) {
    for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
      if (((cities[s->city2].id != 0) && ((int)cities[s->city2].owner == s->civ)) &&
         ((s->city2 != city &&
          (((ret = city_has_improvement(s->city2,IMP_AIRPORT), ret != 0 &&
            ((cities[s->city2].flags & CITYF_AIRLIFTED) == 0)) &&
           (ret = tile_adjacent_nonallied_unit((int)cities[s->city2].x,(int)cities[s->city2].y,s->civ),
           ret == 0)))))) {
        ret = tile_landmass(cities[s->city2].x,cities[s->city2].y);
        if (((ret == s->landmass2) && (1 < s->defense_need)) ||
            ((ret != s->landmass2) && (civs[s->civ].continent_policy[ret] != s->policy))) {
          unit = tile_first_unit((int)cities[s->city2].x,(int)cities[s->city2].y);
          if (((-1 < unit) &&
              ((s->policy != 1 || (count = unit_stack_count_role(unit,ROLE_DEFEND), 1 < count)))) &&
             (leader_trait = *(char *)&civs[s->civ].continent_policy[ret],
             ret = unit_stack_sum(unit,2),
             (int)(3 - (uint)(leader_trait == 4)) <= ret)) {
            while ((-1 < unit && ((int)(char)unit_types[units[unit].type].role != s->policy))) {
              unit = unit_next_in_stack(unit);
            }
            if (-1 < unit) {
              cities[city].flags = cities[city].flags | CITYF_AIRLIFTED;
              cities[s->city2].flags = cities[s->city2].flags | CITYF_AIRLIFTED;
              unit_exhaust_moves(unit);
              unit_relocate(unit,cities[city].x,cities[city].y,1);
              unit3 = unit;
              watchers = 0;
              for (unit = 0; unit < game.num_units; unit++) {
                if ((((units[unit].id != 0) && (unit_types[units[unit].type].role == ROLE_AIR_SUPERIORITY)) &&
                    (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) &&
                   (((byte)(civs[units[unit].owner].treaties[s->civ] >> 8) & (TREATY_WAR >> 8)) != 0)) {
                  ret = map_distance((int)cities[city].x,(int)cities[city].y,
                                             (int)units[unit].x,(int)units[unit].y);
                  count = unit_max_moves(unit);
                  if (count / (int)(uint)cosmic.road_move_mult < ret) {
                    ret = map_distance((int)cities[s->city2].x,(int)cities[s->city2].y,
                                               (int)units[unit].x,(int)units[unit].y);
                    count = unit_max_moves(unit);
                    if (count / (int)(uint)cosmic.road_move_mult < ret) continue;
                  }
                  ret = rand_range(0,(-(uint)((units[unit].flags & UNITF_VETERAN) == 0) & 2) + 3);
                  if (ret == 0) {
                    unit_delete(unit3,1);
                    unit3 = -1;
                    break;
                  }
                  watchers++;
                }
              }
              if (unit3 < 0) {
                text_set_string_arg(0,(const char *)&cities[city].name);
                popup_game((const char *)&str_weshot);
              }
              else if (watchers != 0) {
                text_set_string_arg(0,(const char *)&cities[city].name);
                popup_game((const char *)&str_wesaw);
              }
            }
            break;
          }
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the score of City Walls. */
static uint city_choose_production_walls_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  iptr civ3;

  ret = civ_has_wonder(s->civ,WONDER_GREAT_WALL);
  if (ret == 0) {
    score = 10 - ((int)cities[city].size >> 1);
    for (civ3 = 1; civ3 < 8; civ3++) {
      if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
        if ((civs[s->civ].treaties[civ3] & (TREATY_VENDETTA | TREATY_WAR)) != 0) {
          score -= 2;
        }
        if (*(char *)&civs[civ3].continent_cities[s->landmass2] != '\0') {
          score--;
        }
      }
    }
    if (game.advance_known_by[ADV_ROBOTICS] != '\0') {
      score += 2;
    }
    if (game.advance_known_by[ADV_ADVANCED_FLIGHT] != '\0') {
      score += 2;
    }
    if (game.advance_known_by[ADV_ROCKETRY] != '\0') {
      score += 2;
    }
    score = int_clamp(score,1,10);
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret != 0) {
      score -= 4;
    }
    *defensive = 1;
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a SAM Missile Battery. */
static uint city_choose_production_sam_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  iptr civ3;
  uint tmp;
  uptr civ2;
  int threat_level;

  threat_level = ((uint)civs[game.best_human_rank].units_active[UNIT_STEALTH_BOMBER] * 2 +
              (uint)civs[game.best_human_rank].units_active[UNIT_BOMBER] * 2 +
              (uint)civs[game.best_human_rank].units_active[UNIT_CRUISE_MISSILE] +
             (uint)civs[game.best_human_rank].units_active[UNIT_HELICOPTER]) * 2;
  if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
    threat_level = 0;
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (civ2 != s->civ) {
        tmp = (uint)civs[civ2].units_active[UNIT_CRUISE_MISSILE] +
                   (uint)civs[civ2].units_active[UNIT_BOMBER] +
                   (uint)civs[civ2].units_active[UNIT_HELICOPTER] +
                   (uint)civs[civ2].units_active[UNIT_STEALTH_BOMBER];
        if (((*((byte *)&civs[s->civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) == 0) &&
           ((*(byte *)&civs[civ2].treaties[s->civ] & TREATY_VENDETTA) == 0)) {
          tmp = tmp / 2;
        }
        threat_level += tmp;
      }
    }
  }
  score = 0xc - threat_level;
  score = int_clamp(score,1,0xc);
  if ((cities[city].flags & CITYF_SAM_UNUSED) != 0) {
    score -= 3;
  }
  *defensive = 1;
  for (civ3 = 1; civ3 < 8; civ3++) {
    if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
       ((4 < civs[civ3].reputation ||
        (((byte)(civs[s->civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0)))) {
      *defensive = (uint)(s->threatened != 0);
      break;
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a Coastal Fortress. */
static uint city_choose_production_coastal_fortress_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  iptr civ3;
  uint tmp;
  uptr civ2;
  int threat_level;

  ret = city_adjacent_to_body(city,1);
  if (ret != 0) {
    threat_level = (uint)civs[game.best_human_rank].units_active[UNIT_CRUISER] * 3 +
               (uint)civs[game.best_human_rank].units_active[UNIT_AEGIS_CRUISER] * 3 +
               (uint)civs[game.best_human_rank].units_active[UNIT_BATTLESHIP] * 5 +
               (uint)civs[game.best_human_rank].units_active[UNIT_IRONCLAD] +
               (uint)civs[game.best_human_rank].units_active[UNIT_DESTROYER];
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      threat_level = 0;
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        if (civ2 != s->civ) {
          tmp = ((int)(uint)civs[civ2].units_active[UNIT_IRONCLAD] >> 1) +
                     ((int)(uint)civs[civ2].units_active[UNIT_DESTROYER] >> 1) +
                     (uint)civs[civ2].units_active[UNIT_BATTLESHIP] * 2 +
                     (uint)civs[civ2].units_active[UNIT_CRUISER] +
                     (uint)civs[civ2].units_active[UNIT_AEGIS_CRUISER];
          if (((*((byte *)&civs[s->civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) == 0) &&
             ((*(byte *)&civs[civ2].treaties[s->civ] & TREATY_VENDETTA) == 0)) {
            tmp = tmp / 2;
          }
          threat_level += tmp;
        }
      }
    }
    score = 0xc - threat_level;
    score = int_clamp(score,1,0xc);
    *defensive = 1;
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         ((4 < civs[civ3].reputation ||
          (((byte)(civs[s->civ].treaties[civ3] >> 8) & (TREATY_WAR >> 8)) != 0)))) {
        *defensive = (uint)(s->threatened != 0);
        break;
      }
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the score of a spaceship
   structural, and whether the AI holds back the other builds for it. */
static uint city_choose_production_spaceship_score(ChooseProdState *s,iptr city,uint score,uint *defensive)
{
  int ret;
  uint adjusted;
  int human_launched;
  iptr civ3;
  iptr part_group;
  int own_parts_cost;
  int part;
  int leader_parts_cost;

  ret = civ_spaceship_launched(s->civ);
  if (((ret == 0) && ((game.game_flags & GAME_WON) == 0)) &&
     ((ret = spaceship_choose_part_group(s->civ,0), -1 < ret &&
      (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0 ||
       (civs[s->civ].spaceship_parts[0] < 0x26)))))) {
    score = 4;
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         (ret = civ_spaceship_exists(civ3), ret != 0)) {
        score = 2;
        break;
      }
    }
    if ((s->policy != 0) && ((civs[s->civ].continent_flags[s->landmass2] & 0x10) == 0)) {
      *defensive = s->threatened;
    }
    human_launched = 0;
    if ((game.game_flags & GAME_WON) == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
           (ret = civ_spaceship_launched(civ3), ret != 0)) {
          human_launched = 1;
          break;
        }
      }
    }
    if ((1 < s->space_race) && (game.difficulty != DIFF_CHIEFTAIN)) {
      adjusted = score - 1;
      if (human_launched != 0) {
        adjusted = score - 2;
      }
      score = adjusted;
      if (s->defense_urgent == 0) {
        *defensive = s->threatened;
      }
    }
    leader_parts_cost = 0;
    own_parts_cost = 0;
    for (part = 0; part < 6; part++) {
      part_group = int_clamp((part + 1) / 2,0,2);
      own_parts_cost = own_parts_cost +
                  (int)civs[s->civ].spaceship_parts[part] *
                  (uint)improvements[part_group + 0x23].cost;
      leader_parts_cost = leader_parts_cost +
                 (int)civs[((uint)game.best_human_rank)].spaceship_parts[part] *
                 (uint)improvements[part_group + 0x23].cost;
    }
    if (((s->space_builders == 0) ||
        (((s->space_builders <= s->human_space_builders && (own_parts_cost <= leader_parts_cost)) || (human_launched != 0)))) &&
       ((((cities[city].flags & CITYF_DISORDER) == 0 && (game.difficulty != DIFF_CHIEFTAIN)) &&
        (((int)civs[s->civ].best_shield_surplus >> 1 <= s->shield_surplus || (0x13 < s->shield_surplus)))))) {
      s->choice_score = 999;
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): the base score of an
   improvement, and whether it is defensive. */
static uint city_choose_production_improvement_score(ChooseProdState *s,iptr city,iptr improvement,uint *defensive)
{
  int ret;
  uint tmp;
  uint score;

  score = 999;
  *defensive = 0;
  if (improvement - 1U < 0x23) {
    ret = s->food_surplus >> 1;
    switch(improvement) {
    case IMP_PALACE:
      if ((((s->palace_count == 0) || ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
          ((s->palace_count == 0 ||
           ((s->palace_count == 1 && (s->palace_size * 2 < (int)cities[s->city2].size)))))) &&
         ((s->policy == 4 || (((s->policy == 5 || (s->policy == 0)) || (3 < city_calc.shield_waste)))))) {
        score = 2;
        if ('\t' < cities[s->city2].size) {
          score = 1;
        }
        if (14 < cities[s->city2].size) {
          score--;
        }
        if (3 < city_calc.shield_waste) {
          *defensive = s->threatened;
        }
      }
      break;
    case IMP_BARRACKS:
      score = (leaders[civs[s->civ].tribe].civilize + s->barracks_count + 4) -
                 (int)leaders[civs[s->civ].tribe].attack;
      if (((leaders[civs[s->civ].tribe].attack < '\0') ||
          ('\0' < leaders[civs[s->civ].tribe].civilize)) &&
         ((int)leaders[civs[s->civ].tribe].civilize + (int)leaders[civs[s->civ].tribe].attack != 0
         )) {
        score += s->barracks_count;
      }
      *defensive = 1;
      if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
        if (game.difficulty == DIFF_KING) {
          score = (int)(score * 3) / 2;
        }
        if (DIFF_KING < game.difficulty) {
          score = score << 1;
        }
      }
      break;
    case IMP_GRANARY:
      ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS);
      if (ret == 0) {
        if (city_calc.total_shields < 3) {
          score = 8;
        }
        else {
          score = 4;
        }
        if (city_calc.happy_count <= city_calc.unhappy_count) {
          score += 2;
        }
        if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) &&
           (tmp = (game.difficulty + 4) - (int)cities[s->city2].size, 0 < (int)tmp))
        {
          score += tmp;
        }
      }
      break;
    case IMP_TEMPLE:
      if (s->unrest_state == 1) {
        score = 9;
      }
      if (s->unrest_state == 2) {
        score = 0xfffffffb;
      }
      break;
    case IMP_MARKETPLACE:
      score = int_clamp(10 - (s->net_trade >> 1),1,10);
      if (s->unrest_state == 1) {
        score--;
      }
      if (s->unrest_state == 2) {
        score = 0xfffffffc;
      }
      break;
    case IMP_LIBRARY:
      score = int_clamp(10 - s->net_trade / 3,1,10);
      break;
    case IMP_COURTHOUSE:
      score = 0xe - (city_calc.shield_waste * 2 + city_calc.trade_corruption);
      if (civs[s->civ].government == GOV_DEMOCRACY) {
        if (s->unrest_state == 2) {
          score = 0xffffffff;
        }
      }
      else if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) {
        score -= (int)civs[game.best_human_rank].subversion_count / 2;
      }
      break;
    case IMP_CITY_WALLS:
      score = city_choose_production_walls_score(s,city,score,defensive);
      break;
    case IMP_AQUEDUCT:
      if ((int)((uint)cosmic.aqueduct_size - ret) <= (int)cities[city].size) {
        score = ((cosmic.aqueduct_size + 4) - (int)cities[city].size) - ret;
        score = int_clamp(score,1,0x14);
      }
      break;
    case IMP_BANK:
      score = int_clamp(10 - s->net_trade / 3,1,10);
      if (s->unrest_state == 1) {
        score--;
      }
      if (s->unrest_state == 2) {
        score = 0;
      }
      break;
    case IMP_CATHEDRAL:
      ret = civ_has_wonder(s->civ,WONDER_MICHELANGELO);
      if (ret == 0) {
        if (s->unrest_state == 1) {
          score = 8;
        }
        if (s->unrest_state == 2) {
          score = 0xfffffffd;
        }
      }
      break;
    case IMP_UNIVERSITY:
      if ((civs[s->civ].science_rate != '\0') &&
         ((s->space_race == 0 || (ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT), ret == 0)))) {
        score = int_clamp(10 - (s->net_trade >> 2),2,10);
      }
      break;
    case IMP_MASS_TRANSIT:
      break;
    case IMP_COLOSSEUM:
      if (s->unrest_state == 2) {
        score = 0xfffffffe;
      }
      break;
    case IMP_FACTORY:
    case IMP_HYDRO_PLANT:
      score = int_clamp(0xe - city_calc.total_shields,1,0xe);
      break;
    default:
      score = int_clamp(0xc - ((int)(city_calc.total_shields + (city_calc.total_shields >> 0x1f & 3U)) >>
                                          2),2,0xc);
      break;
    case IMP_SDI_DEFENSE:
      if (s->unrest_state < 2) {
        ret = int_clamp(0xf - cities[city].size,1,0xf);
        score = (ret + 1) / 2;
        if (game.wonder_city[WONDER_MANHATTAN] == -1) {
          score = score << 1;
        }
        if (civs[game.best_human_rank].units_active[UNIT_NUCLEAR_MISSILE] == '\0') {
          score = score << 1;
        }
        if ('\t' < cities[city].size) {
          score--;
        }
        ret = city_has_improvement(city,IMP_PALACE);
        if (ret != 0) {
          score = 0;
        }
      }
      *defensive = 1;
      break;
    case IMP_POWER_PLANT:
      score = int_clamp(0xc - city_calc.total_shields / 5,2,0xe);
      break;
    case IMP_STOCK_EXCHANGE:
      if ((s->unrest_state != 0) || (s->space_race < 2)) {
        score = int_clamp(0xb - (s->net_trade >> 2),2,0xb);
        if (s->unrest_state == 1) {
          score--;
        }
        if (s->unrest_state == 2) {
          score = 0;
        }
      }
      break;
    case IMP_SEWER_SYSTEM:
      if ((s->space_race < 2) &&
         ((int)((uint)cosmic.sewer_size - ret) <= (int)cities[city].size)) {
        score = ((cosmic.sewer_size + 4) - (int)cities[city].size) - ret;
        score = int_clamp(score,1,0x14);
      }
      break;
    case IMP_SUPERMARKET:
      score = (0xe - ((int)cities[city].size >> 1)) + s->farmland_count * -2 + s->space_race * 2 + ret
      ;
      score = int_clamp(score,2,0xe);
      break;
    case IMP_SUPERHIGHWAYS:
      score = (0xf - ((int)cities[city].size >> 1)) + s->space_race * 6;
      score = int_clamp(score,2,0xf);
      break;
    case IMP_RESEARCH_LAB:
      if (((s->space_race == 0) || (ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT), ret == 0)) &&
         (civs[s->civ].science_rate != '\0')) {
        score = int_clamp(0xb - (s->net_trade >> 2),2,10);
      }
      break;
    case IMP_SAM_BATTERY:
      score = city_choose_production_sam_score(s,city,score,defensive);
      break;
    case IMP_COASTAL_FORTRESS:
      score = city_choose_production_coastal_fortress_score(s,city,score,defensive);
      break;
    case IMP_SOLAR_PLANT:
      break;
    case IMP_HARBOR:
      if ((s->unrest_state < 2) &&
         (score = int_clamp(0x10 - s->ocean_tiles,2,0x10), s->food_surplus < 1)) {
        score = (int)score / 2;
      }
      break;
    case IMP_OFFSHORE_PLATFORM:
      if (s->unrest_state < 2) {
        score = 0x10 - s->ocean_tiles;
        score = int_clamp(score,2,0x10);
        if (0 < (int)score) {
          ret = city_has_improvement(city,IMP_FACTORY);
          if (ret != 0) {
            score = (int)score / 2;
          }
          ret = city_has_improvement(city,IMP_MFG_PLANT);
          if (ret != 0) {
            score = (int)score / 2;
          }
        }
        if (((city_calc.total_shields - city_calc.shield_support <= city_calc.total_shields / 3) ||
            (city_calc.total_shields < (int)cities[s->city2].size / 2)) &&
           ((int)cities[s->city2].size / 2 <= (int)(s->ocean_tiles + (s->ocean_tiles >> 0x1f & 3U)) >> 2))
        {
          score -= 2;
        }
      }
      break;
    case IMP_AIRPORT:
      score = (s->airport_count * 4 + 4) - (int)leaders[civs[s->civ].tribe].attack;
      if (*(char *)&civs[s->civ].continent_policy[s->landmass2] == 4) {
        score = (int)score / 2;
      }
      ret = civ_has_advance(s->civ,ADV_ADVANCED_FLIGHT);
      if (ret == 0) {
        score = score << 1;
      }
      if (((*(char *)&civs[s->civ].continent_cities[s->landmass2] == 1) ||
          (((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0 && (s->airport_count == 0))))
         && ((int)(uint)civs[s->civ].continent_cities[s->landmass2] < (int)score)) {
        score = (uint)civs[s->civ].continent_cities[s->landmass2];
      }
      ret = city_has_improvement(city,IMP_PALACE);
      if ((ret != 0) && (-1 < (int)score)) {
        score = 0;
      }
      *defensive = s->threatened;
      break;
    case IMP_POLICE_STATION:
      ret = civ_has_wonder(s->civ,WONDER_WOMENS_SUFFRAGE);
      if (ret == 0) {
        tmp = (*(uint *)&city_calc.aggressive_units);
        if (GOV_REPUBLIC < civs[s->civ].government) {
          tmp = (*(uint *)&city_calc.aggressive_units) << 1;
        }
        if (((GOV_FUNDAMENTALISM < civs[s->civ].government) && (tmp != 0)) &&
           (city_calc.happy_count - city_calc.unhappy_count < 2)) {
          tmp += 2;
        }
        score = 10 - tmp;
      }
      break;
    case IMP_PORT_FACILITY:
      ret = city_adjacent_to_body(city,1);
      if (ret != 0) {
        ret = s->nearest_dist;
        if (0x27 < s->nearest_dist) {
          ret = 0x28;
        }
        score = (0xb - (ret >> 2)) + s->naval_units * -5 +
                   (s->port_count * 2 - (int)leaders[civs[s->civ].tribe].attack) +
                   (int)leaders[civs[s->civ].tribe].civilize;
        score = int_clamp(score,2,0xf);
        if (leaders[civs[s->civ].tribe].attack < '\0') {
          score += s->port_count;
        }
      }
      break;
    case IMP_SS_STRUCTURAL:
      score = city_choose_production_spaceship_score(s,city,score,defensive);
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): an improvement: its score and
   whether it is the best so far. */
static void city_choose_production_try_improvement(ChooseProdState *s,iptr city,iptr improvement)
{
  int ret;
  uint defensive;
  uint score;

  ret = city_can_build_improvement(s->civ,city,improvement);
  if (ret != 0) {
    score = city_choose_production_improvement_score(s,city,improvement,&defensive);
    if ((int)score < 400) {
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
    }
    else {
      score = 0x3fff;
    }
    ret = city_has_improvement(city,IMP_BARRACKS);
    if (ret != 0) {
      score += (int)score / ((s->barracks_count >> 1) + 3);
    }
    ret = wonder_get_city(WONDER_SHAKESPEARE);
    if (ret == city) {
      score += (int)score / 3;
    }
    if ((int)score < s->choice_score) {
      s->choice_score = score;
      s->choice = -improvement;
    }
    if ((int)score < s->best_build_score) {
      s->best_build_score = score;
      s->best_build = -improvement;
    }
  }
}

/* Part of city_choose_production (00498e8b): the bias of each wonder for the
   civ's situation and its leader; 0 to leave the wonder out. */
static int city_choose_production_wonder_bias(ChooseProdState *s,iptr city,iptr wonder,uint score,uint *defensive,uint *out)
{
  int ret;
  uint adjusted;
  iptr civ3;
  iptr unit_type;
  uint tmp;
  uptr civ4;
  int ship_count;

  adjusted = score;
  switch(wonder) {
  case WONDER_PYRAMIDS:
    adjusted = score - (leaders[civs[s->civ].tribe].civilize * 3 + 2);
    break;
  case WONDER_HANGING_GARDENS:
    adjusted = score - ((int)civs[s->civ].num_cities / 3 + 1);
    break;
  case WONDER_COLOSSUS:
    ret = city_has_improvement(city,IMP_PALACE);
    if (ret != 0) {
      score--;
    }
    ret = city_has_improvement(city,IMP_MARKETPLACE);
    if (ret != 0) {
      score--;
    }
    adjusted = score - (int)cities[city].size / 3;
    break;
  case WONDER_LIGHTHOUSE:
    adjusted = score -
            ((int)(civs[s->civ].units_active[UNIT_CARAVEL] + 2 + (uint)civs[s->civ].units_active[UNIT_TRIREME]
                  ) >> 2);
    break;
  case WONDER_GREAT_LIBRARY:
    for (civ4 = 1; adjusted = score, (int)civ4 < 8; civ4++) {
      if (civs[civ4].num_advances < civs[s->civ].num_advances) {
        if ((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) {
          adjusted = score + 1;
        }
      }
      else {
        adjusted = score - 1;
        if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
           (civs[s->civ].num_advances + 5 < (uint)civs[civ4].num_advances)) {
          adjusted = score - 3;
        }
      }
      score = adjusted;
    }
    break;
  case WONDER_ORACLE:
    adjusted = score -
            (((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U)
                   ) >> 2) - (int)game.turn / 0x19);
    break;
  case WONDER_GREAT_WALL:
    adjusted = score + leaders[civs[s->civ].tribe].civilize * -2;
    break;
  case WONDER_SUN_TZU:
    adjusted = score -
            (leaders[civs[s->civ].tribe].attack * 3 + (int)leaders[civs[s->civ].tribe].expand
             + leaders[civs[s->civ].tribe].civilize * -2 + 1);
    break;
  case WONDER_KING_RICHARDS:
    ret = int_clamp((city_calc.total_shields * 2) / 3,0,3);
    adjusted = score - ret;
    break;
  case WONDER_MARCO_POLO:
    for (civ3 = 1; adjusted = score, civ3 < 8; civ3++) {
      if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
        if ((*(byte *)&civs[s->civ].treaties[civ3] & TREATY_ALLIANCE) != 0) {
          score--;
        }
        if ((*(byte *)&civs[s->civ].treaties[civ3] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0) {
          score--;
        }
      }
    }
    break;
  case WONDER_MICHELANGELO:
    adjusted = score -
            ((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U))
            >> 2);
    break;
  case WONDER_COPERNICUS:
    score = (score - ((int)(s->net_trade + (s->net_trade >> 0x1f & 7U)) >> 3)) - 1;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_MAGELLAN:
    ship_count = 0;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if (unit_types[unit_type].domain == DOMAIN_SEA) {
        ship_count += (uint)civs[s->civ].units_active[unit_type];
      }
    }
    adjusted = score - ((int)(ship_count + 3 + (ship_count + 3 >> 0x1f & 3U)) >> 2);
    break;
  case WONDER_SHAKESPEARE:
    adjusted = score - (int)cities[city].size / 5;
    break;
  case WONDER_LEONARDO:
    *defensive = s->threatened;
    adjusted = score - 4;
    break;
  case WONDER_BACH:
    score -= ((int)(uint)civs[s->civ].continent_city_size[s->landmass2] >> 4);
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_NEWTON:
    score -= (int)cities[city].size / 6;
    ret = city_has_improvement(city,IMP_PALACE);
    adjusted = score;
    if (ret != 0) {
      adjusted = score - 1;
    }
    break;
  case WONDER_ADAM_SMITH:
    adjusted = score -
            ((int)((int)civs[s->civ].num_cities + ((int)civs[s->civ].num_cities >> 0x1f & 3U))
            >> 2);
    break;
  case WONDER_DARWIN:
    adjusted = score - 3;
    break;
  case WONDER_STATUE_OF_LIBERTY:
    ret = civ_has_advance(s->civ,ADV_COMMUNISM);
    if (ret == 0) {
      adjusted = score - 2;
    }
    else {
      adjusted = score - 1;
    }
    break;
  case WONDER_EIFFEL_TOWER:
    if (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) {
      adjusted = score - 1;
      if (3 < civs[game.civ_by_rank[7]].reputation) {
        adjusted = score - 2;
      }
      score = adjusted;
      adjusted = score;
      if ((7 < civs[game.civ_by_rank[7]].num_cities) &&
         (adjusted = score - 1, game.power_rank[s->civ] == 6)) {
        adjusted = score - 2;
      }
    }
    break;
  case WONDER_WOMENS_SUFFRAGE:
    adjusted = score - 1;
    if (((-1 < leaders[civs[s->civ].tribe].expand) &&
        (-1 < leaders[civs[s->civ].tribe].civilize)) &&
       (-1 < leaders[civs[s->civ].tribe].attack)) {
      adjusted = score - 2;
    }
    score = adjusted;
    if (0 < civs[s->civ].government_score[GOV_REPUBLIC]) {
      score--;
    }
    if (GOV_FUNDAMENTALISM < civs[s->civ].government) {
      score--;
    }
    adjusted = score;
    if (GOV_REPUBLIC < civs[s->civ].government) {
      adjusted = score - 1;
    }
    break;
  case WONDER_HOOVER_DAM:
    adjusted = score - civs[s->civ].continent_city_size[s->landmass2] / 0x14;
    break;
  case WONDER_MANHATTAN:
    ret = civ_has_advance(s->civ,ADV_ROCKETRY);
    if (ret != 0) {
      tmp = 0;
      for (civ3 = 1; civ3 < 8; civ3++) {
        if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
            (ret = civ_has_advance(civ3,ADV_ROCKETRY), ret != 0)) &&
           (civs[civ3].military_strength < civs[s->civ].military_strength)) {
          tmp = 1;
          break;
        }
      }
      if (tmp == 0) {
        if (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) {
          score--;
          ret = civ_has_advance(game.civ_by_rank[7],ADV_ROCKETRY);
          if (ret == 0) {
            score--;
          }
        }
        adjusted = score +
                (leaders[civs[s->civ].tribe].civilize * 2 -
                (int)leaders[civs[s->civ].tribe].attack);
        break;
      }
    }
    return 0;
  case WONDER_UNITED_NATIONS:
    adjusted = score -
            ((int)leaders[civs[s->civ].tribe].civilize +
            leaders[civs[s->civ].tribe].attack * -2);
    if (game.power_rank[s->civ] == 7) {
      adjusted--;
    }
    break;
  case WONDER_APOLLO:
    adjusted = score -
            (leaders[civs[s->civ].tribe].civilize * 3 +
            leaders[civs[s->civ].tribe].attack * -2);
    break;
  case WONDER_SETI:
    adjusted = score - (leaders[civs[s->civ].tribe].civilize * 2 + 2);
    break;
  case WONDER_CURE_FOR_CANCER:
    adjusted = score - (leaders[civs[s->civ].tribe].attack + 2);
  }
  *out = adjusted;
  return 1;
}

/* Part of city_choose_production (00498e8b): the civs (a bit each) building
   the wonder the city builds, from wonder_builders.  B21: for a unit or an
   improvement the original reads that byte all the same, from its other
   locals below the array; none of them is a wonder's builders (every mode:
   ours would read past the array; docs/BUGS.md). */
static uint city_choose_production_rivals(ChooseProdState *s,iptr city)
{
  if (cities[city].building > -0x27) {
    return 0;
  }
  return (uint)*(byte *)&s->wonder_builders[-0x27 - cities[city].building];
}

/* Part of city_choose_production (00498e8b): a wonder: its score and whether
   it is the best so far. */
static void city_choose_production_try_wonder(ChooseProdState *s,iptr city,iptr wonder)
{
  int ret;
  uint adjusted;
  iptr improvement;
  int wonder_cost;
  iptr preq2;
  int expired;
  uint defensive;
  iptr wonder_group;
  uint score;
  int raw_score;

  improvement = wonder + 0x27;
  ret = city_can_build_improvement(s->civ,city,improvement);
  if (ret != 0) {
    defensive = 0;
    if (s->policy == 4) {
      defensive = s->threatened;
    }
    ret = (s->wonders_owned + s->wonders_here + 8) - civs[s->civ].gold / 200;
    if (ret < 2) {
      ret = 1;
    }
    wonder_group = wonder / 7;
    score = ret + (uint)civs[s->civ].wonders_by_group[wonder_group];
    if ((byte)(((i8 *)&map_views_block.human_wonders_by_group_min))[wonder_group] < civs[s->civ].wonders_by_group[wonder_group]) {
      score += 2;
    }
    if (civs[s->civ].wonders_by_group[wonder_group] <
        civs[((uint)game.worst_human_rank)].wonders_by_group[wonder_group]) {
      score--;
    }
    if ((((s->wonder_in_progress == 0) || ((int)cities[city].building == -improvement)) ||
        ((((game.human_players & *(byte *)&s->wonder_builders[-0x27 - cities[city].building]) ==
           0 || ((*(short *)((int)&game + cities[city].building * -2 + 0xb0) != -1 ||
                 ((int)cities[city].shield_box <
                  (int)((uint)improvements[-(int)cities[city].building].cost * city_calc.shield_rows)
                  / 2)))) &&
         ((*(byte *)&s->wonder_builders[-0x27 - cities[city].building] & 1) == 0)))) &&
       (((DIFF_WARLORD < game.difficulty || (preq2 = (iptr)improvements[improvement].preq, preq2 < 0))
        || ((game.human_players & game.advance_known_by[preq2]) != 0)))) {
      if (wonder != WONDER_MANHATTAN) {
        if (game.difficulty < DIFF_PRINCE) {
          for (s->city2 = 0; s->city2 < game.num_cities; s->city2++) {
            if (((cities[s->city2].id != 0) &&
                (((uint)game.human_players & 1 << (cities[s->city2].owner & 0x1fU)) != 0)) &&
               ((int)cities[s->city2].building == -improvement)) {
              score = 999;
            }
          }
          if (score == 999) return;
        }
        ret = civ_has_advance(s->civ,ADV_ROCKETRY);
        if ((ret != 0) && ((game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0))
        return;
      }
      if (city_choose_production_wonder_bias(s,city,wonder,score,&defensive,&adjusted) == 0) return;
      score = adjusted;
      expired = wonder_is_expired(wonder);
      if (expired != 0) {
        score = score * 3 + (uint)game.difficulty * 0x32;
      }
      score += s->wonders_building * 5;
      if (((s->wonders_building == 0) || (s->wonder_in_progress != 0)) &&
         (*(char *)&civs[s->civ].wonders_by_group[wonder_group] == '\0')) {
        score -= 2;
      }
      if (s->policy == 1) {
        score = score + s->enemy_strength + 1;
      }
      else {
        score += s->enemy_strength / 2;
      }
      if ((int)cities[city].building == -improvement) {
        if (s->foreign_same_build != 0) {
          score = (int)score / 2;
        }
        score = (s->rivals_ahead + 1) * score + s->rivals_ahead;
      }
      else if ((1 << (s->civ5 & 0x1f) &
               (uint)city_choose_production_rivals(s,city)) != 0) {
        score = score * 2 + 1;
      }
      raw_score = score;
      wonder_cost = (uint)improvements[improvement].cost * city_calc.shield_rows + 1;
      if (((s->wonder_in_progress != 0) && (s->defense_urgent == 0)) && (expired == 0)) {
        if (s->wonders_building == 0) {
          if (s->threatened == defensive) {
            score = (int)score / 2;
          }
          else {
            defensive = s->threatened;
          }
        }
        if ((int)(wonder_cost * 3 + (wonder_cost * 3 >> 0x1f & 3U)) >> 2 <=
            (int)cities[city].shield_box) {
          defensive = s->threatened;
        }
        if (((s->threatened != defensive) && (wonder_cost / 2 <= (int)cities[city].shield_box)) &&
           (s->wonders_building < 3)) {
          defensive = s->threatened;
        }
      }
      if (s->wonder_in_progress == 0) {
        if ((int)civs[s->civ].best_shield_surplus >> 1 < s->shield_surplus) {
          if ((((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) &&
              (civs[s->civ].best_shield_surplus != s->shield_surplus)) && (s->shield_surplus < 6)) {
            score = score << 1;
          }
        }
        else {
          score = score << 1;
        }
      }
      if (civs[s->civ].best_shield_surplus == s->shield_surplus) {
        score -= (int)score / 3;
      }
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
      ret = city_has_improvement(city,IMP_BARRACKS);
      if (ret != 0) {
        score += (int)score / ((s->barracks_count >> 1) + 3);
      }
      ret = city_has_improvement(city,IMP_PALACE);
      if ((ret != 0) &&
         ((score = score - (int)score / 3, s->wonders_here == 0 ||
          (*(char *)&civs[s->civ].wonders_by_group[wonder_group] == '\0')))) {
        score = (int)score / 2;
      }
      if ((cities[city].flags & CITYF_WONDER_BUILT) == 0) {
        if ((s->wonder_in_progress != 0) && (expired == 0)) {
          score = (((wonder_cost - cities[city].shield_box) *
                      ((int)(score * 3 + ((int)(score * 3) >> 0x1f & 3U)) >> 2)) /
                      wonder_cost + ((int)(score + ((int)score >> 0x1f & 3U)) >> 2)) / 3;
        }
      }
      else {
        score = score * 3;
      }
      if (s->choice < -0x26) {
        if (raw_score < s->best_raw_score) {
          s->best_raw_score = raw_score;
          s->choice = -improvement;
          s->best_build = -improvement;
          if ((int)score < s->choice_score) {
            s->choice_score = score;
          }
          if ((int)score < s->best_build_score) {
            s->best_build_score = score;
          }
        }
      }
      else {
        if ((int)score < s->choice_score) {
          s->best_raw_score = raw_score;
          s->choice_score = score;
          s->choice = -improvement;
        }
        if ((int)score < s->best_build_score) {
          s->best_build_score = score;
          s->best_build = -improvement;
        }
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the score of a diplomat or spy
   (to steal the spaceship advances when behind in the race); 0 to leave it
   out. */
static int city_choose_production_diplomat_score(ChooseProdState *s,iptr city,iptr unit_type,uint *score,uint *defensive)
{
  int ret;
  uint adjusted;
  iptr improvement;
  int race_behind;
  uptr civ2;
  uptr civ4;

  race_behind = ai_spaceship_race_behind(s->civ);
  if (race_behind != 0) {
    ret = civ_has_advance(s->civ,ADV_SPACE_FLIGHT);
    if ((((ret != 0) &&
         (ret = civ_has_advance(s->civ,improvements[IMP_SS_COMPONENT].preq), ret != 0)) &&
        (ret = civ_has_advance(s->civ,improvements[IMP_SS_MODULE].preq), ret != 0)) &&
       (ret = civ_has_advance(s->civ,ADV_FUSION_POWER), ret != 0)) {
      race_behind = 0;
    }
    if ((s->defense_urgent != 0) || ((cities[city].flags & CITYF_DISORDER) != 0)) {
      race_behind = 0;
    }
    if (game.difficulty < DIFF_PRINCE) {
      race_behind = 0;
    }
  }
  if (!(((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
       ((cities[city].flags & CITYF_AUTOBUILD) == 0))) return 0;
  if ((s->units_in_build[unit_type] <= race_behind) &&
     (s->unit_counts[unit_type] < (int)(uint)civs[s->civ].continent_cities[s->landmass2]))
  {
    *score = 0xb;
    civ4 = 0xffffffff;
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (((civ2 != s->civ) &&
          (((*(byte *)&civs[s->civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0 ||
           (((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0 &&
              ((game.power_rank[s->civ] <= game.power_rank[civ2] || (race_behind != 0))))
             && ((unit_type == UNIT_SPY ||
                 (((7 < (int)((uint)civs[civ2].num_advances -
                             (uint)civs[s->civ].num_advances) || (race_behind != 0)) ||
                  (game.power_rank[civ2] == 7)))))) &&
            ((((*(byte *)&civs[s->civ].treaties[civ2] & TREATY_ALLIANCE) != 0 ||
              (race_behind != 0)) ||
             ((civs[civ2].government < GOV_REPUBLIC ||
              ((civs[civ2].continent_attack[s->landmass2] <=
                civs[s->civ].continent_military[s->landmass2] ||
               (*(char *)&civs[s->civ].continent_cities[s->landmass2] == '\0')))))))))))) &&
         (*(char *)&civs[civ2].continent_cities[s->landmass2] != '\0')) {
        civ4 = civ2;
      }
    }
    if ((int)civ4 < 0) {
      civ4 = (uint)game.best_human_rank;
      if ((s->policy == 4) && (game.difficulty != DIFF_CHIEFTAIN)) {
        *defensive = s->threatened;
        *score = 8;
        ret = civs[s->civ].num_cities + 3;
        if ((int)((uint)civs[s->civ].units_active[unit_type] +
                 (uint)civs[s->civ].units_in_production[unit_type]) <
            (int)(ret + (ret >> 0x1f & 3U)) >> 2) {
          if (civs[game.best_human_rank].gold < civs[s->civ].gold) {
            *score = 7;
          }
          if (civs[game.best_human_rank].gold * 2 < civs[s->civ].gold) {
            (*score)--;
          }
          if (civs[s->civ].num_advances < civs[game.best_human_rank].num_advances) {
            if (s->unit_counts[unit_type] + s->units_in_build[unit_type] == 0) {
              *score -= 3;
            }
            *score = *score -
                       ((uint)civs[game.best_human_rank].num_advances -
                       (uint)civs[s->civ].num_advances);
          }
          *score = int_clamp(*score,civs[s->civ].units_active[unit_type] + 1,10);
        }
      }
      if (race_behind != 0) {
        *score = (int)*score / 2;
      }
      if (s->unit_counts[unit_type] != 0) {
        *score = 999;
      }
    }
    else {
      *score = int_clamp(10 - ((uint)civs[civ4].num_advances -
                                         (uint)civs[s->civ].num_advances),2,10);
      if ((((*(byte *)&civs[s->civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0) &&
          (*score = *score + 1, s->unit_counts[unit_type] != 0)) && (race_behind == 0)) {
        *score = 999;
      }
      if (civs[civ4].military_strength <= civs[s->civ].military_strength) {
        *defensive = s->threatened;
      }
    }
    if (civs[s->civ].government == GOV_COMMUNISM) {
      (*score)--;
    }
    if ((race_behind != 0) && ((int)*score < 0x14)) {
      for (improvement = 0x23; improvement < 0x26; improvement++) {
        ret = civ_has_advance(s->civ,improvements[improvement].preq);
        if ((ret == 0) &&
           (ret = civ_has_advance(civ4,improvements[improvement].preq),
           ret != 0)) {
          *defensive = s->threatened;
          adjusted = s->units_in_build[unit_type] * 2 + 1;
          if ((int)*score <= (int)adjusted) {
            adjusted = *score;
          }
          *score = adjusted;
          if ((((s->units_in_build[unit_type] == 0) &&
               (*(char *)&civs[s->civ].units_active[unit_type] == '\0')) &&
              (*score = adjusted - 1, -0x26 < s->choice)) && (s->choice < -0x22)) {
            s->choice = unit_type;
          }
        }
      }
    }
  }
  return 1;
}

/* Part of city_choose_production (00498e8b): a diplomat or a caravan: its
   score and whether it is the best so far. */
static void city_choose_production_try_special_unit(ChooseProdState *s,iptr city,iptr unit_type)
{
  int ret;
  uint defensive;
  uint score;

  if ((5 < (char)unit_types[unit_type].role) &&
     (ret = civ_can_build_unit(s->civ,city,unit_type), ret != 0)) {
    score = 999;
    defensive = 0;
    if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
      if (city_choose_production_diplomat_score(s,city,unit_type,&score,&defensive) == 0) return;
    }
    else if (unit_types[unit_type].role == ROLE_TRADE) {
      if ((cities[city].flags & CITYF_HAD_CARAVAN) != 0) return;
      score = 10;
      if ((char)cities[city].trade_routes < 3) {
        score = 10 - ((5 - leaders[civs[s->civ].tribe].expand) * city_calc.total_trade) / 10;
      }
      ret = score + (char)cities[city].trade_routes * 2;
      if (ret < 3) {
        ret = 2;
      }
      score = ret + (uint)civs[s->civ].continent_trade_units[s->landmass2];
      if (!(((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
           ((cities[city].flags & CITYF_AUTOBUILD) == 0))) return;
    }
    if ((int)score < 400) {
      score = (int)(((-(uint)(s->threatened == defensive) & 0xfffffff6) + 0x14) * score * 3) /
                 (leaders[civs[s->civ].tribe].civilize + 3);
    }
    else {
      score = 0x3fff;
    }
    ret = city_has_improvement(city,IMP_BARRACKS);
    if (ret != 0) {
      score += ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
    }
    if ((int)score < s->choice_score) {
      s->choice_score = score;
      s->choice = unit_type;
    }
    if ((int)score < s->best_build_score) {
      s->best_build_score = score;
      s->best_build = unit_type;
    }
  }
}

/* Part of city_choose_production (00498e8b): whether a military or settler
   unit is one to consider (not obsolete, not too many). */
static int city_choose_production_unit_allowed(ChooseProdState *s,iptr city,iptr unit_type,uint role2)
{
  int ret;

  if (((((((int)role2 < 6) &&
         (ret = civ_can_build_unit(s->civ,city,unit_type), ret != 0)) &&
        (unit_type != UNIT_CARRIER)) &&
       ((role2 != 5 ||
        (((city_calc.settler_food_cost <= s->food_surplus ||
          (((city_calc.settlers_supported == 0 &&
            ((uint)civs[s->civ].units_active[UNIT_SETTLERS] + (uint)civs[s->civ].units_active[UNIT_ENGINEERS] ==
             s->flagged_settlers)) && (s->units_in_build[1] + s->units_in_build[0] < 2)))) &&
         (1 < cities[city].size)))))) &&
      (((unit_type != UNIT_WARRIORS || (civs[s->civ].government < GOV_MONARCHY)) &&
       (((char)unit_types[unit_type].attack < 'c' ||
        ((civs[s->civ].units_active[unit_type] < 4 &&
         (civs[s->civ].units_in_production[unit_type] < 2)))))))) &&
     ((role2 != 3 ||
      (((uint)civs[s->civ].units_active[UNIT_FIGHTER] + (uint)civs[s->civ].units_in_production[UNIT_FIGHTER] +
        (uint)civs[s->civ].units_active[UNIT_STEALTH_FIGHTER] + (uint)civs[s->civ].units_in_production[UNIT_STEALTH_FIGHTER] <
        (uint)civs[game.best_human_rank].units_active[UNIT_BOMBER] +
        (uint)civs[game.best_human_rank].units_active[UNIT_HELICOPTER] +
        (uint)civs[game.best_human_rank].units_active[UNIT_STEALTH_BOMBER] + 2 &&
       (((*(byte *)&civs[s->civ].treaties[((uint)game.best_human_rank)] & TREATY_PEACE) == 0 ||
        ((uint)civs[s->civ].units_active[unit_type] +
         (uint)civs[s->civ].units_in_production[unit_type] == 0)))))))) {
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      if (unit_type == UNIT_WARRIORS) {
        ret = civ_has_advance(s->civ,unit_types[UNIT_PHALANX].prerequisite);
        if (ret != 0) return 0;
      }
      else if (unit_type == UNIT_ARCHERS) {
        ret = civ_has_advance(s->civ,unit_types[UNIT_LEGION].prerequisite);
        if (ret != 0) return 0;
      }
      else if (unit_type == UNIT_HORSEMEN) {
        ret = civ_has_advance(s->civ,unit_types[UNIT_CHARIOT].prerequisite);
        if (ret != 0) return 0;
        ret = civ_has_advance(s->civ,unit_types[UNIT_ELEPHANT].prerequisite);
        if (ret != 0) return 0;
      }
    }
    if (unit_type == UNIT_CHARIOT) {
      ret = civ_has_advance(s->civ,unit_types[UNIT_KNIGHTS].prerequisite);
      if (ret != 0) return 0;
      ret = civ_has_advance(s->civ,unit_types[UNIT_CRUSADERS].prerequisite);
      if (ret != 0) return 0;
    }
    else if (unit_type == UNIT_ELEPHANT) {
      ret = civ_has_advance(s->civ,unit_types[UNIT_KNIGHTS].prerequisite);
      if (ret != 0) return 0;
    }
    return 1;
  }
  return 0;
}

/* Part of city_choose_production (00498e8b): the base score of a unit from the
   units of its kind there are and being built; 0 to leave it out. */
static int city_choose_production_unit_base_score(ChooseProdState *s,iptr unit_type,uint *out)
{
  int ret;
  int explorer_cap;
  uint tmp;
  uint score;

  if (unit_types[unit_type].domain == DOMAIN_LAND) {
    score = s->units_in_build[unit_type] * 2 + s->unit_counts[unit_type];
    if ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0) {
      score = (int)score / 2;
      if (((unit_type == UNIT_MARINES) || (unit_type == UNIT_ALPINE_TROOPS)) || (unit_type == UNIT_PARTISANS)) {
        score += 2;
      }
    }
    if (unit_type == UNIT_ARCHERS) {
      score = score + s->units_in_build[5] * 2 + s->unit_counts[5];
    }
    if (leaders[civs[s->civ].tribe].attack < '\0') {
      if (unit_type == UNIT_KNIGHTS) {
        score = score + s->units_in_build[0x12] * 2 + s->unit_counts[0x12];
      }
    }
    else if (unit_type == UNIT_CRUSADERS) {
      score = score + s->units_in_build[0x13] * 2 + s->unit_counts[0x13];
    }
    if ((unit_types[unit_type].role == ROLE_ATTACK) && (unit_types[unit_type].attack == '\0')) {
      if ((s->policy == 0) || (s->policy == 1)) return 0;
      if (s->policy == 4) {
        explorer_cap = 1;
      }
      else {
        explorer_cap = (landmass_table_block.landmass_recs[s->landmass2].tiles + 199) / 200;
      }
      if (!((s->units_in_build[unit_type] * 2 + s->unit_counts[unit_type] <= explorer_cap) &&
           (civs[s->civ].units_active[unit_type] < 2))) return 0;
    }
    if (s->policy == 4) {
      tmp = 1 - (int)leaders[civs[s->civ].tribe].expand;
      if (unit_types[unit_type].role != ROLE_SETTLE) {
        tmp = tmp * 2;
      }
      if (s->enemy_offshore != 0) {
        tmp = (int)tmp / 2;
      }
      score += tmp;
      if ((s->enemy_offshore != 0) && (unit_types[unit_type].role == ROLE_ATTACK)) {
        tmp = (int)leaders[civs[s->civ].tribe].attack + 1;
        ret = civ_has_advance(s->civ,unit_types[UNIT_TRANSPORT].prerequisite);
        if (ret != 0) {
          tmp = tmp << 1;
        }
        score = int_clamp(score - tmp,0,99);
      }
    }
  }
  else {
    if (!((ushort)(((civs[s->civ].continent_cities[s->landmass2] < 2) - 1 & 4) + 4) <=
           civs[s->civ].continent_military[s->landmass2])) return 0;
    score = (int)(((uint)civs[s->civ].units_active[unit_type] +
                     (uint)civs[s->civ].units_in_production[unit_type] * 2) *
                    (uint)civs[s->civ].continent_military[s->landmass2]) /
               (s->unit_total + 1);
    if (((unit_types[unit_type].flags & UTYPE_SPOT_SUBMARINES) != 0) &&
       (score = score - civs[game.best_human_rank].units_active[UNIT_SUBMARINE], (int)score < 1))
    {
      score = 0;
    }
  }
  *out = score;
  return 1;
}

/* Part of city_choose_production (00498e8b): the score of settlers. */
static uint city_choose_production_settler_score(ChooseProdState *s,iptr city,iptr unit_type,uint score)
{
  char leader_trait;
  iptr saved_type;
  int ret;
  int settler_base;
  int support_penalty;
  iptr unit_type2;
  int threat_level;

  ret = city_has_improvement(city,IMP_GRANARY);
  if ((ret == 0) && (ret = civ_has_wonder(s->civ,WONDER_PYRAMIDS), ret == 0)) {
    settler_base = 6;
  }
  else {
    settler_base = 4;
  }
  saved_type = unit_type;
  if (s->unit_counts[unit_type] < ((civs[s->civ].government + 1) / 2)) {
    leader_trait = leaders[civs[s->civ].tribe].civilize;
  }
  else {
    leader_trait = leaders[civs[s->civ].tribe].expand;
  }
  score = ((settler_base - leader_trait) - s->expand_bonus) * score;
  if (city_calc.settlers_supported != 0) {
    support_penalty = 8;
    if (cities[city].size < 4) {
      support_penalty = 0x10;
    }
    for (unit_type2 = 0; unit_type2 < city_calc.settlers_supported; unit_type2++) {
      score += (unit_type2 + 1) * support_penalty;
    }
  }
  if (cities[city].size < 2) {
    score = score << 1;
  }
  if ((s->unit_counts[unit_type] == 0) && (city_calc.settler_food_cost == s->food_surplus)) {
    score -= (int)score / 2;
  }
  if (s->defense_need < 2) {
    if (s->units_in_build[unit_type] == 0) {
      if (civs[s->civ].government < GOV_REPUBLIC) {
        if (GOV_DESPOTISM < civs[s->civ].government) {
          ret = int_clamp((int)(uint)civs[s->civ].continent_cities[s->landmass2] >> 1,1,3);
          if (s->unit_counts[unit_type] < ret) {
            score = (int)score / 2;
          }
          if (s->unit_counts[unit_type] <
              (civs[s->civ].continent_cities[s->landmass2] / 5)) {
            score = (int)score / 2;
          }
        }
      }
      else {
        ret = int_clamp(civs[s->civ].continent_cities[s->landmass2]
                                   - 1,1,6);
        if (s->unit_counts[saved_type] < ret) {
          score = (int)score / 2;
        }
        if (s->unit_counts[unit_type] <
            (int)(uint)civs[s->civ].continent_cities[s->landmass2] >> 2) {
          score = (int)score / 2;
        }
      }
    }
    if (((s->unit_counts[unit_type] == 0) && (s->units_in_build[unit_type] == 0)) &&
       (score = (int)score / 2, s->policy != 4)) {
      threat_level = leaders[civs[s->civ].tribe].expand + 3;
      if (s->policy == 1) {
        threat_level = leaders[civs[s->civ].tribe].expand + 2;
      }
      if (s->policy == 5) {
        threat_level++;
      }
      if ((int)(uint)civs[s->civ].continent_cities[s->landmass2] < threat_level) {
        score = 0;
      }
    }
  }
  return score;
}

/* Part of city_choose_production (00498e8b): a military or settler unit: its
   score and whether it is the best so far. */
static void city_choose_production_try_military_unit(ChooseProdState *s,iptr city,iptr unit_type)
{
  int ret;
  uint adjusted;
  int count;
  int combat_value;
  int support_penalty;
  uint role2;
  int hp_factor;
  uint score;

  role2 = (uint)(char)unit_types[unit_type].role;
  if (city_choose_production_unit_allowed(s,city,unit_type,role2) != 0) {
    if (city_choose_production_unit_base_score(s,unit_type,&score) == 0) return;
    if ((unit_types[unit_type].flags & UTYPE_PARADROP) != 0) {
      role2 = 0;
    }
    if ((s->defense_need != 0) &&
       (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0 || (citypref.no_defend == 0))))
    {
      if (role2 == ROLE_DEFEND) {
        score = (uint)(2 / (longlong)s->defense_need);
        if ((s->defense_need == 2) && (s->unrest_state < 2)) {
          score = -1;
        }
      }
      else if (s->defense_need == 2) return;
    }
    adjusted = score;
    if (((role2 != ROLE_DEFEND) && (role2 != ROLE_SETTLE)) &&
       (adjusted = score << 1, s->policy != role2)) {
      adjusted = score << 2;
    }
    score = adjusted;
    if (((unit_type != UNIT_PARTISANS) ||
        ((s->defense_need == 0 &&
         ((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0)))) &&
       ((unit_types[unit_type].domain != DOMAIN_SEA ||
        ((((ret = city_adjacent_sea_info(city,0,0), ret != 0x3f &&
           ((cities[city].flags & CITYF_COASTAL) != 0)) && (ret != 0)) &&
         ((count = landmass_count_bordering(ret), 1 < count ||
          (((role2 != ROLE_SEA_TRANSPORT && (0x2f < landmass_table_block.landmass_recs[ret].water_body_size)) &&
           (ret < 0x3f)))))))))) {
      if (role2 == ROLE_SEA_TRANSPORT) {
        if (s->policy == 4) {
          score = (int)(score * 3) / 2;
        }
        else {
          if ((s->policy == 1) ||
             (*(char *)&civs[s->civ].units_active[unit_type] != '\0'))
          return;
          if (unit_type != UNIT_TRANSPORT) {
            score = score * 2 << ((byte)s->threatened & 0x1f);
          }
        }
      }
      if (role2 == ROLE_SETTLE) {
        score = city_choose_production_settler_score(s,city,unit_type,score);
      }
      else {
        score = score << 4;
      }
      score = ((char)unit_types[unit_type].cost + 2) * score;
      if ((unit_types[unit_type].attack == '\0') &&
         (unit_types[unit_type].defense == '\0')) {
        score = 0;
      }
      else {
        if ((game.rules & RULE_NORMAL_COMBAT) == 0) {
          hp_factor = 2;
        }
        else {
          hp_factor = int_clamp((int)(char)unit_types[unit_type].hit_points / 10
                                        + (int)(char)unit_types[unit_type].firepower,2,4)
          ;
        }
        combat_value = ((int)(char)unit_types[unit_type].attack +
                    (int)(char)unit_types[unit_type].defense) * hp_factor;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          combat_value++;
        }
        if (unit_type == UNIT_PARTISANS) {
          combat_value++;
        }
        score = (int)score /
                   ((((int)(char)unit_types[unit_type].move /
                      (int)(uint)cosmic.road_move_mult + 1) * combat_value) / 2);
      }
      if (role2 == ROLE_NAVAL_SUPERIORITY) {
        if ((game.game_flags & GAME_LARGE_MAP) != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
        if ((s->policy == 4) && (0 < (int)score)) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
        if (s->threatened == 0) {
          score = score << 1;
        }
        else {
          score = score << 2;
        }
        ret = city_has_improvement(city,IMP_PORT_FACILITY);
        if (ret != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
      }
      if (unit_types[unit_type].domain == DOMAIN_AIR) {
        score = score << 1;
        ret = city_has_improvement(city,IMP_AIRPORT);
        if (ret != 0) {
          score -= ((int)(score + ((int)score >> 0x1f & 3U)) >> 2);
        }
      }
      if (role2 == ROLE_DEFEND) {
        score -= (s->settle_goals + 1 >> 1);
      }
      if (0x77 < s->unit_total) {
        score = score << 2;
      }
      if ((s->defense_need == 0) && ((int)role2 < 6)) {
        support_penalty = 0;
        if ((city_calc.shield_support != 0) && (city_calc.total_shields != 0)) {
          support_penalty = (int)((3 - s->threatened) * (city_calc.shield_support + 1) * score) /
                      city_calc.total_shields;
        }
        if (role2 == ROLE_SETTLE) {
          if ((((uint)civs[s->civ].units_active[unit_type] - s->homeless_settlers) +
               s->units_in_build[unit_type] == 0) &&
             (3 - leaders[civs[s->civ].tribe].expand < (int)cities[city].size)) {
            support_penalty = 0;
            if (city_calc.total_shields <=
                (int)((uint)(-1 < leaders[civs[s->civ].tribe].expand) + city_calc.shield_support * 2)
               ) {
              score -= (int)score / 3;
            }
          }
          else {
            support_penalty = support_penalty / 2;
          }
        }
        else if (((s->threatened == 0) && (city_calc.total_shields <= city_calc.support_unit_count)) ||
                ((civs[s->civ].flags & CIVF_AT_WAR) == 0)) {
          score = score << 1;
        }
        score += support_penalty;
      }
      if (((role2 == ROLE_DEFEND) && ((1 << (s->civ5 & 0x1f) & (uint)game.human_players) != 0))
         && (citypref.no_defend != 0)) {
        score = 999;
      }
      if (((int)score < s->choice_score) ||
         (((s->choice < 0 && ((cities[city].flags & CITYF_DISORDER) == 0)) &&
          ((1 < s->defense_need ||
           (((s->defense_urgent != 0 && (s->unrest_state < 2)) &&
            ((civs[s->civ].continent_flags[s->landmass2] & 0x10) != 0)))))))) {
        s->choice_score = score;
        s->choice = unit_type;
      }
      if ((int)score < s->best_unit_score) {
        s->best_unit_score = score;
        s->best_unit = unit_type;
      }
    }
  }
}

/* Part of city_choose_production (00498e8b): the final choice: an AI city
   keeps its wonder or palace, a human's city follows its governor's
   preference. */
static iptr city_choose_production_final_choice(ChooseProdState *s,iptr city)
{
  int ret;
  i32 bits;
  iptr unit2;
  iptr cur_improvement;
  iptr wonder;

  if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) ||
     ((cities[city].flags & CITYF_AUTOBUILD) == 0)) {
    if (((1 << (s->civ5 & 0x1f) & (uint)game.human_players) == 0) && (cities[city].building == -IMP_PALACE))
    {
      s->choice = -1;
    }
    else if ((s->choice < -0x26) && (ret = city_has_improvement(city,IMP_TEMPLE), ret == 0)) {
      s->choice = -4;
    }
    else if ((cities[city].building < -0x26) &&
            (((s->same_build_count == 0 && (-0x27 < s->choice)) && ((cities[city].flags & CITYF_DISORDER_2ND_TURN) == 0)))) {
      if (cities[city].building < 1) {
        cur_improvement = ~(int)cities[city].building + 1;
      }
      else {
        cur_improvement = (iptr)cities[city].building;
      }
      wonder = cur_improvement - 0x27;
      if (((s->defense_need < 2) && (ret = wonder_is_expired(wonder), ret == 0)) &&
         ((game.wonder_city[wonder] == -1 &&
          ((s->defense_urgent == 0 || ((cities[city].flags & CITYF_DEFENDER_LOST) == 0)))))) {
        bits = 1;
        unit2 = tile_first_unit((int)cities[city].x,(int)cities[city].y);
        ret = unit_stack_count_role(unit2,bits);
        if (1 < ret) {
          s->choice = (iptr)cities[city].building;
        }
      }
    }
  }
  else if ((cities[city].flags & CITYF_AUTO_UNITS_ONLY) == 0) {
    if ((cities[city].flags & CITYF_AUTO_IMPROVEMENTS_ONLY) == 0) {
      if (500 < s->choice_score) {
        s->choice = 99;
      }
    }
    else if ((0x62 < s->best_build) || (s->choice = s->best_build, 500 < s->best_build_score)) {
      s->choice = 99;
    }
  }
  else if ((0x62 < s->best_unit) || (s->choice = s->best_unit, 500 < s->best_unit_score)) {
    s->choice = 99;
  }
  return s->choice;
}

/* 00498e8b auto */
/* WARNING: Type propagation algorithm not settling */

int __cdecl city_choose_production(int city,i32 *out_best_unit,i32 *out_best_build)

{
  iptr behind;
  iptr city_landmass;
  int ret;
  int unit_counts[62];
  iptr improvement;
  int units_in_build[62];
  int lost_cities;
  iptr unit_type;
  i8 wonder_builders[28];
  iptr wonder;
  uptr civ2;
  iptr unit_type2;
  int pref_chosen;
  ChooseProdState st;

  st.wonder_builders = wonder_builders;
  st.units_in_build = units_in_build;
  st.unit_counts = unit_counts;
  st.expand_bonus = 0;
  if (out_best_unit != 0) {
    *out_best_unit = 999;
  }
  if (out_best_build != 0) {
    *out_best_build = 999;
  }
  city_recalc(city,0);
  st.civ5 = cities[city].owner;
  st.civ = (uptr)(char)st.civ5;
  st.landmass2 = tile_landmass(cities[city].x,cities[city].y);
  st.policy = (uint)civs[st.civ].continent_policy[st.landmass2];
  if ((st.policy == 0) || (st.policy == 1)) {
    st.threatened = 1;
  }
  else {
    st.threatened = 0;
  }
  if (((game.game_flags & GAME_AI_GANG_UP) != 0) && (st.policy == 4)) {
    st.threatened = 1;
  }
  behind = ai_spaceship_race_behind(st.civ);
  st.space_race = spaceship_race_active();
  st.space_race = behind + st.space_race;
  lost_cities = 0;
  for (st.city2 = 0; st.city2 < game.num_cities; st.city2++) {
    if ((((cities[st.city2].id != 0) && ((int)cities[st.city2].founder == st.civ)) &&
        ((int)cities[st.city2].owner != st.civ)) &&
       ((city_landmass = tile_landmass(cities[st.city2].x,cities[st.city2].y), city_landmass == st.landmass2
        && (((uint)game.human_players & 1 << (cities[st.city2].owner & 0x1fU)) != 0)))) {
      lost_cities++;
    }
  }
  if (leaders[civs[st.civ].tribe].attack + 1 < lost_cities) {
    st.threatened = 1;
  }
  st.food_surplus = (city_calc.yield_totals[0] - (int)cities[city].size * (uint)cosmic.food_per_citizen) -
              city_calc.settlers_supported * city_calc.settler_food_cost;
  st.ocean_tiles = 0;
  st.shield_surplus = city_calc.total_shields * 2 - city_calc.shield_support;
  st.net_trade = city_calc.total_trade - city_calc.trade_corruption;
  if ((cities[city].building < -0x26) && (0x27 < cities[city].shield_box)) {
    st.wonder_in_progress = 1;
  }
  else {
    st.wonder_in_progress = 0;
  }
  st.wonders_here = 0;
  st.wonders_owned = 0;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    st.wonder_builders[wonder] = 0;
    ret = wonder_get_city(wonder);
    if (ret == city) {
      st.wonders_here++;
    }
    ret = civ_has_wonder(st.civ,wonder);
    if (ret != 0) {
      st.wonders_owned++;
    }
  }
  if (((cities[city].flags & CITYF_DISORDER) == 0) && (((byte)(cities[city].flags >> 8) & ((CITYF_ON_EDGE | CITYF_AI_SPECIALIST_HELD) >> 8)) != 0xc0)) {
    if (city_calc.unhappy_count == city_calc.happy_count) {
      st.unrest_state = 1;
    }
    else {
      st.unrest_state = 0;
    }
  }
  else {
    st.unrest_state = 2;
  }
  city_find_nearest(cities[city].x,cities[city].y,st.civ,0xfffffffe,-1);
  st.nearest_dist = advisor_window_block.nearest_city_dist;
  city_choose_production_scan_tiles(&st,city);
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    st.units_in_build[unit_type] = 0;
    st.unit_counts[unit_type] = 0;
  }
  city_choose_production_survey_cities(&st,city);
  city_choose_production_count_units(&st,unit_type);
  city_choose_production_expand_bonus(&st,city);
  st.settle_goals = 0;
  for (unit_type2 = 0; unit_type2 < 0x10; unit_type2++) {
    if ((civs[st.civ].ai_standing_goals[unit_type2].role == ROLE_DEFEND) &&
       (ret = tile_landmass(civs[st.civ].ai_standing_goals[unit_type2].x,
                              civs[st.civ].ai_standing_goals[unit_type2].y),
       ret == st.landmass2)) {
      st.settle_goals++;
    }
  }
  if (st.threatened != 0) {
    st.settle_goals += (st.settle_goals >> 1);
  }
  if ((((st.unit_total == 0) && ((1 << (st.civ5 & 0x1f) & (uint)game.human_players) == 0)) &&
      (civs[st.civ].government < GOV_MONARCHY)) && ((out_best_unit == 0 && (out_best_build == 0)))) {
    for (unit_type = 0x3d; -1 < unit_type; unit_type--) {
      if (((unit_types[unit_type].role == ROLE_DEFEND) && ((unit_types[unit_type].flags & UTYPE_PARADROP) == 0)) &&
         (ret = civ_can_build_unit(st.civ,city,unit_type), ret != 0)) {
        return unit_type;
      }
    }
  }
  if (st.civ == 0) {
    return city_choose_production_barbarian(city);
  }
  st.enemy_offshore = 0;
  if (st.policy == 4) {
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (((*((byte *)&civs[st.civ].treaties[civ2] + 1) & (TREATY_WAR >> 8)) != 0) &&
         (*(char *)&civs[civ2].continent_cities[st.landmass2] == '\0')) {
        st.enemy_offshore = 1;
      }
    }
  }
  city_choose_production_defense_need(&st,city);
  if ((1 << (st.civ5 & 0x1f) & (uint)game.human_players) == 0) {
    city_choose_production_sell_redundant(&st,city);
    city_choose_production_airlift_defender(&st,city);
  }
  st.best_build_score = 999;
  st.best_unit_score = 999;
  st.choice_score = 999;
  st.best_build = 999;
  st.best_unit = 999;
  st.choice = 999;
  for (improvement = 1; improvement < 0x27; improvement++) {
    city_choose_production_try_improvement(&st,city,improvement);
  }
  pref_chosen = 0;
  if (((1 << (st.civ5 & 0x1f) & (uint)game.human_players) != 0) && (citypref.count != 0)) {
    for (unit_type2 = 0; unit_type2 < citypref.count; unit_type2++) {
      improvement = (((i32 *)&map_views_block.citypref_improvements))[unit_type2];
      ret = city_has_improvement(city,improvement);
      if ((ret == 0) && (ret = city_can_build_improvement(st.civ,city,improvement), ret != 0)) {
        st.best_build = -improvement;
        st.choice = -improvement;
        pref_chosen = 1;
        break;
      }
    }
  }
  /* an improvement from the human player's CITYPREF.TXT list skips the wonders and special units */
  if (pref_chosen == 0) {
    if ((st.wonder_in_progress != 0) ||
       (((((civs[st.civ].continent_flags[st.landmass2] & 0x10) == 0 &&
          (2 < cities[city].size)) && (((byte)(cities[city].flags >> 8) & ((CITYF_ON_EDGE | CITYF_AI_SPECIALIST_HELD) >> 8)) != 0xc0))
        && ((4 - leaders[civs[st.civ].tribe].civilize <=
             (int)(uint)civs[st.civ].continent_cities[st.landmass2] ||
            ((*(char *)&civs[st.civ].continent_flags[st.landmass2] == '\0' &&
             (4 - leaders[civs[st.civ].tribe].civilize <= (int)civs[st.civ].num_cities)))))))) {
      for (wonder = 0; wonder < 0x1c; wonder++) {
        city_choose_production_try_wonder(&st,city,wonder);
      }
    }
    if (civs[st.civ].num_military_units < 0x200) {
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        city_choose_production_try_special_unit(&st,city,unit_type);
      }
    }
  }
  if (civs[st.civ].num_military_units < 0x200) {
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      city_choose_production_try_military_unit(&st,city,unit_type);
    }
  }
  else if (st.choice == 999) {
    st.choice = -0x26;
  }
  if (out_best_build != 0) {
    *out_best_build = st.best_build;
  }
  if (out_best_unit != 0) {
    *out_best_unit = st.best_unit;
  }
  return city_choose_production_final_choice(&st,city);
}

/* Part of city_process_production (004ec3fe): the announcement of a completed
   improvement or wonder: sound, message or dialog (human), and the next
   build. */
static void city_process_production_announce(uptr city,uptr civ,iptr wonder,iptr improvement,int acquired)
{
  iptr str;
  i8 *built_sprite;
  i8 *notify_sprite;
  int announce;
  uptr city3;
  iptr sound;

  if ((human_civ[0] == civ) || (0x22 < improvement)) {
    announce = 1;
  }
  else {
    announce = 0;
  }
  if (autoplay.active != 0) {
    announce = 0;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    cities[city].known_to = 0xff;
  }
  text_set_label_arg(1,improvements[improvement].name);
  str = civ_get_adjective(civ);
  text_set_string_arg(2,(const char *)str);
  city3 = 0xbb;
  if (wonder == WONDER_CURE_FOR_CANCER) {
    city3 = 0xb8;
  }
  if ((((wonder == WONDER_MARCO_POLO) || (wonder == WONDER_SUN_TZU)) || (wonder == WONDER_KING_RICHARDS)) || (wonder == WONDER_LEONARDO)) {
    city3 = 0xba;
  }
  if ((wonder == WONDER_DARWIN) || (wonder == WONDER_MAGELLAN)) {
    city3 = 0xba;
  }
  if (wonder == WONDER_WOMENS_SUFFRAGE) {
    city3 = 0xb9;
  }
  text_set_label_arg(3,label_table.string_ids[city3]);
  if (announce != 0) {
    if (wonder < 0) {
      if ((improvement < 0x23) || (0x25 < improvement)) {
        switch(improvement) {
        case IMP_BARRACKS:
          sound = 5;
          break;
        default:
          sound = 2;
          break;
        case IMP_AQUEDUCT:
          sound = 4;
          break;
        case IMP_BANK:
          sound = 0x2f;
          break;
        case IMP_CATHEDRAL:
          sound = 0xb;
          break;
        case IMP_UNIVERSITY:
          sound = 0x37;
          break;
        case IMP_STOCK_EXCHANGE:
          sound = 0x45;
        }
        snd_play_effect(sound,1,0,0);
      }
      else {
        snd_play_effect(SND_BLDSPCSH,1,0,0);
      }
    }
    else {
      snd_play_effect(SND_NEWONDER,1,0,0);
    }
  }
  if ((human_civ[0] == civ) && (autoplay.active == 0)) {
    if ((2 < game.multiplayer) && (0x22 < improvement)) {
      text_set_string_arg(0,(const char *)&cities[city].name);
      net_send_text_args(0x47,0xff,4,0,wonder,improvement);
    }
    if ((game.message_options & MSGOFF_IMPROVEMENT_BUILT) == 0) {
      if (wonder < 0) {
        notify_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
      }
      else {
        notify_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
      }
      city_notify_dialog_ex(str_built_4,city,1,(Sprite *)notify_sprite);
    }
  }
  else if (announce != 0) {
    text_set_string_arg(0,(const char *)&cities[city].name);
    if ((autoplay.active == 0) && (2 < game.multiplayer)) {
      net_send_text_args(0x47,0xff,4,0,wonder,improvement);
    }
    Dialog built_dialog(0x4000);
    built_dialog.load_game_text((const char *)&str_built2);
    if (wonder < 0) {
      built_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
    }
    else {
      built_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
    }
    built_dialog.add_sprite((Sprite *)built_sprite,0,0)
    ;
    built_dialog.set_image_zoom(8);
    built_dialog.show_modal(0);
  }
  if ((((announce != 0) && (-1 < wonder)) && (human_civ[0] == civ)) &&
     ((autoplay.active == 0 && (((*((char *)&game.game_options + 2)) & (OPT_WONDER_MOVIES >> 16)) != 0)))) {
    wonder_show_movie(wonder);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    city_calc.completed_improvement = improvement;
  }
  if (acquired == 0) {
    cities[city].shield_box = 0;
  }
  if (((cities[city].flags & CITYF_AUTOBUILD) != 0) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
    city_set_production(city,99);
  }
}

/* Part of city_process_production (004ec3fe): a completed improvement or
   wonder: build it, announce it, and its effects (spaceship part, palace,
   wonders with an effect when built). */
static void city_process_production_complete_improvement(uptr city,uptr civ,int cost2,iptr wonder,iptr improvement,int acquired)
{
  int result;
  iptr str;
  i32 value;
  uptr city2;
  uptr city3;
  iptr part;
  uptr civ2;

  if (-1 < improvement) {
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
    }
    if (wonder == WONDER_MANHATTAN) {
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        value = int_clamp((civs[civ2].reputation + 1) / 2,0,6);
        civs[civ2].reputation = (u8)value;
      }
      if (autoplay.active == 0) {
        for (civ2 = 1; (int)civ2 < 8; civ2++) {
          if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) &&
              ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
             (((1 << ((byte)civ2 & 0x1f) & (int)(char)cities[city].known_to) != 0 ||
              ((int)cities[city].owner == (civ2 & 0xff))))) {
            if (human_civ[0] == civ2) {
              snd_play_effect(SND_LARGEXPL,1,0,0);
              city_notify_dialog(str_manhattan,city,(Sprite *)&caption_close_button_sprite_block.city_notify_sprite);
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x46,net.net_slots[net.net_civ_slots[civ2]].id,1,0,
                                 city,0);
            }
          }
        }
      }
    }
    if (acquired == 0) {
      city_set_improvement(city,improvement,1);
      cities[city].shield_box -= (short)cost2;
    }
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       (improvement < 0x23)) {
      if (improvement == IMP_PALACE) {
        cities[city].building--;
      }
      city_set_production(city,99);
    }
    else {
      city_process_production_announce(city,civ,wonder,improvement,acquired);
    }
    if ((0x22 < improvement) && (improvement < 0x26)) {
      part = improvement - 0x23;
      part = spaceship_add_part(civ,part);
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         ((result = civ_spaceship_launched(civ), result == 0 && (part < 0)))) {
        text_set_label_arg(1,improvements[improvement].name);
        city_notify_dialog(str_badspace_2,city,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)));
      }
    }
    if (improvement == IMP_PALACE) {
      for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
        if ((cities[city2].id != 0) && ((int)cities[city2].owner == civ)) {
          city_set_improvement(city2,1,0);
        }
      }
      city_set_improvement(city,1,1);
      civs[civ].capital_x = cities[city].x;
      if (autoplay.active == 0) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        str = civ_adjective(civ);
        text_set_string_arg(1,(const char *)str);
        if ((autoplay.active == 0) && (2 < game.multiplayer)) {
          net_send_text_args(0x48,0xff,2,0,0,0);
        }
        popup_game((const char *)&str_movecapital);
      }
    }
    if (wonder == WONDER_DARWIN) {
      civ_research_complete(civ,0);
      civ_research_complete(civ,0);
    }
    if (wonder == WONDER_LEONARDO) {
      civ_leonardo_upgrade(civ);
    }
    if (wonder == WONDER_EIFFEL_TOWER) {
      civ_apply_eiffel_tower(civ);
    }
    if (wonder == WONDER_APOLLO) {
      wonder_apollo_reveal_map();
    }
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_improvements(1);
    }
    if (-1 < wonder) {
      city3 = city;
      for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
        if ((((cities[city2].id != 0) && (city2 != city3)) &&
            (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) == 0)) &&
           ((int)cities[city2].building == -improvement)) {
          city_set_production(city2,99);
        }
      }
      city_recalc(city3,0);
    }
  }
}

/* Part of city_process_production (004ec3fe): an AI diplomat: created next
   to the foreign city it is likely to reach first. */
static void city_process_production_ai_diplomat(uptr city,uptr civ,iptr &unit,iptr unit_type)
{
  int result;
  int landmass2;
  int roll;
  uptr target_city;
  uptr city2;
  int tie_count;
  iptr unit2;
  int best_dist;

  target_city = 0xffffffff;
  best_dist = 0x7fff;
  tie_count = 0;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if (((cities[city2].id != 0) && ((int)cities[city2].owner != civ)) &&
       ((*(byte *)&civs[civ].treaties[cities[city2].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) {
      unit = unit_find_nearest((int)cities[city2].x,(int)cities[city2].y,civ
                                    ,-1);
      if (dialog_text_font_obj_block.unit_nearest_dist == 3 || dialog_text_font_obj_block.unit_nearest_dist - 3 < 0) {
        dialog_text_font_obj_block.unit_nearest_dist = ~(dialog_text_font_obj_block.unit_nearest_dist - 3U) + 1;
      }
      else {
        dialog_text_font_obj_block.unit_nearest_dist -= 3;
      }
      if (-1 < unit) {
        if (((cities[city2].flags & CITYF_TECH_STOLEN) != 0) && (dialog_text_font_obj_block.unit_nearest_dist < 4)) {
          dialog_text_font_obj_block.unit_nearest_dist = 3;
        }
        if (dialog_text_font_obj_block.unit_nearest_dist <= best_dist) {
          if (best_dist == dialog_text_font_obj_block.unit_nearest_dist) {
            if (tie_count + 1 == 1 || tie_count < 0) {
              roll = 0;
              tie_count++;
            }
            else {
              tie_count += 2;
              roll = crt_rand_at("city_process_production");
              roll = roll % tie_count;
            }
            if (roll != 0) continue;
          }
          result = tile_is_ocean((int)units[unit].x,(int)units[unit].y);
          if (result == 0) {
            result = tile_landmass(cities[city2].x,cities[city2].y);
            landmass2 = tile_landmass(units[unit].x,units[unit].y);
            if (result == landmass2) {
              if (best_dist < dialog_text_font_obj_block.unit_nearest_dist) {
                tie_count = 1;
                best_dist = dialog_text_font_obj_block.unit_nearest_dist;
              }
              target_city = city2;
              unit2 = unit;
            }
          }
        }
      }
    }
  }
  if ((best_dist < 4) && (result = crt_rand_at("city_process_production"), result % 10 <= (int)(uint)game.difficulty)) {
    city2 = target_city;
    unit = unit2;
    unit = unit_create(unit_type,civ,units[unit2].x,
                                  units[unit2].y);
  }
  else {
    unit = unit_create(unit_type,civ,cities[city].x,
                                  cities[city].y);
  }
  if (civs[civ].government == GOV_COMMUNISM) {
    units[unit].flags = units[unit].flags | UNITF_VETERAN;
  }
}

/* Part of city_process_production (004ec3fe): a human's caravan: the dialog to
   choose its commodity. */
static void city_process_production_caravan_dialog(uptr city,uptr civ,iptr unit)
{
  int result;
  iptr str;
  iptr option_id;
  i32 value;
  iptr option_index;
  char goods_amount;
  iptr choice;
  iptr unit_type2;

  Dialog caravan_dialog(0x4000);
  mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,
                     cities[city].owner);
  do {
    text_set_string_arg(0,(const char *)&cities[city].name);
    caravan_dialog.load_game_text((const char *)&str_caravanbuilt);
    for (unit_type2 = 0; unit_type2 < 3; unit_type2++) {
      if (-1 < *(char *)&cities[city].supplies[unit_type2]) {
        value = 0;
        option_index = unit_type2;
        str = (iptr)label_string(commodity_names
                                   [*(char *)&cities[city].supplies[unit_type2]]);
        caravan_dialog.add_option((const char *)str,option_index,value)
        ;
      }
    }
    value = 0;
    option_id = 3;
    str = (iptr)label_string(label_table.string_ids[0xc0]);
    caravan_dialog.add_option((const char *)str,option_id,value);
    choice = caravan_dialog.show_modal(0);
    if (caravan_dialog.result_extra == 0) {
      if (choice < 3) {
        text_set_label_arg(0,commodity_names
                             [*(char *)&cities[city].supplies[choice]]);
      }
      else {
        str = (iptr)label_string(label_table.string_ids[0xc0]);
        text_set_string_arg(0,(const char *)str);
      }
      result = popup_game((const char *)&str_caraconfirm);
      if (result != 0) {
        caravan_dialog.result_extra = 1;
      }
    }
    else if (choice < 3) {
      trade_commodity_cities_dialog(civ,*(char *)&cities[city].supplies[choice]);
    }
    else {
      popup_game((const char *)&str_nofoodreport);
    }
  } while (caravan_dialog.result_extra != 0);
  if (dlg_input.help_pressed != 0) {
    city_calc.zoom_requested = 1;
  }
  if (choice < 3) {
    if (*(char *)&cities[city].supplies[choice] < 1) {
      units[unit].counter = ~cities[city].supplies[choice] + 1;
    }
    else {
      units[unit].counter = cities[city].supplies[choice];
    }
    if (*(char *)&cities[city].supplies[choice] < 1) {
      goods_amount = ~cities[city].supplies[choice] + 1;
    }
    else {
      goods_amount = *(char *)&cities[city].supplies[choice];
    }
    *(char *)&cities[city].supplies[choice] = -goods_amount;
  }
  else {
    units[unit].counter = 0xff;
  }
}

/* Part of city_process_production (004ec3fe): a completed unit: create it
   (settlers shrink the city), place an AI diplomat, a human's caravan
   dialog or notice.  1 when city_process_production returns at once (the
   city is gone or the unit was refused), 0 to go on. */
static int city_process_production_unit(uptr city,uptr civ)
{
  byte cost_byte;
  int result;
  iptr str;
  iptr unit;
  int civ3;
  iptr unit_type;
  int cost2;
  int y2;
  int x2;
  uptr city3;
  iptr unit_type2;

  unit_type = (iptr)cities[city].building;
  if ((-1 < unit_types[unit_type].obsolete_by) &&
     (result = civ_has_advance(civ,unit_types[unit_type].obsolete_by), result != 0)) {
    for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
      if ((unit_types[unit_type2].prerequisite == unit_types[unit_type].obsolete_by) &&
         (unit_types[unit_type2].role == unit_types[unit_type].role)) {
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
          text_set_label_arg(1,unit_types[unit_type].name);
          text_set_label_arg(2,unit_types[unit_type2].name);
          city_notify_dialog(str_upgraded,city,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type2 * 0x3c)));
        }
        unit_type = unit_type2;
        str = unit_type;
        (*((char *)&unit_type + 0)) = (i8)unit_type2;
        cities[city].building = (i8)unit_type;
        unit_type = str;
        break;
      }
    }
  }
  cost2 = (char)unit_types[unit_type].cost * city_calc.shield_rows;
  if (cost2 <= cities[city].shield_box) {
    cities[city].flags = cities[city].flags & ~CITYF_WONDER_BUILT;
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
    }
    if (((unit_types[unit_type].role == ROLE_SETTLE) && (cities[city].size == 1)) &&
       (game.difficulty == DIFF_CHIEFTAIN)) return 1;
    cities[city].shield_box -= (short)cost2;
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
       (unit_types[unit_type].role != ROLE_DIPLOMACY)) {
      unit = unit_create(unit_type,civ,cities[city].x,
                                    cities[city].y);
      if (unit < 0) return 0;
      if ((unit_types[unit_type].role == ROLE_DIPLOMACY) && (civs[civ].government == GOV_COMMUNISM)) {
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      else if ((unit_types[unit_type].domain == DOMAIN_LAND) || ((game.rules & RULE_NORMAL_COMBAT) == 0)) {
        result = city_has_improvement(city,IMP_BARRACKS);
        if ((((result != 0) || (result = civ_has_wonder(civ,WONDER_SUN_TZU), result != 0)) &&
            ((char)unit_types[unit_type].role < 6)) &&
           ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      else if (unit_types[unit_type].domain == DOMAIN_SEA) {
        result = city_has_improvement(city,IMP_PORT_FACILITY);
        if (((result != 0) || (result = civ_has_wonder(civ,WONDER_LIGHTHOUSE), result != 0)) &&
           ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      else if (((unit_types[unit_type].domain == DOMAIN_AIR) &&
               (result = city_has_improvement(city,IMP_AIRPORT), result != 0)) &&
              ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) {
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      if (unit_types[unit_type].role == ROLE_SEA_TRANSPORT) {
        units[unit].counter = 0xff - ((byte)city & 0x3f);
      }
    }
    if (unit_types[unit_type].role == ROLE_SETTLE) {
      if ((cities[city].size == 1) && (civs[civ].num_cities == 1)) return 1;
      if ((cities[city].size == 1) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        text_set_label_arg(1,unit_types[unit_type].name);
        if ((autoplay.active != 0) || (result = popup_game((const char *)&str_ghosttown), result == 0)) {
          unit_delete(unit,1);
          return 1;
        }
      }
      cities[city].size--;
      if (cities[city].size < 1) {
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        text_set_string_arg(0,(const char *)&cities[city].name);
        text_set_label_arg(1,unit_types[unit_type].name);
        text_set_label_arg(3,label_table.string_ids[0xbb]);
        city_delete(city,0);
        result = civ_check_destroyed(civ,0);
        if (((result == 0) &&
            (unit_create(unit_type,civ,x2,y2),
            (game.message_options & MSGOFF_UNIT_BUILT) == 0)) && (city_calc.notify_enabled != 0)) {
          popup_game((const char *)&str_built_2);
        }
        city_turn.window_shown = 0;
        city_calc.notify_enabled = 0;
        map_redraw_tile_all_views(x2,y2);
        return 1;
      }
    }
    if (city_turn.window_shown != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_supported_units(1);
      ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      if (unit_types[unit_type].role == ROLE_TRADE) {
        unit_pick_caravan_commodity(city,unit);
      }
      if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
        city_process_production_ai_diplomat(city,civ,unit,unit_type);
      }
      if (('b' < (char)unit_types[unit_type].attack) &&
         (*(char *)&civs[civ].units_active[unit_type] == 1)) {
        for (civ3 = 1; civ3 < 8; civ3++) {
          civs[civ3].last_contact_turn[civ] = 0xffff;
        }
      }
      city_set_production(city,99);
    }
    else {
      if ((unit_types[unit_type].role == ROLE_SETTLE) || (5 < (char)unit_types[unit_type].role)) {
        text_set_label_arg(1,unit_types[unit_type].name);
        if (unit_types[unit_type].role == ROLE_TRADE) {
          city_process_production_caravan_dialog(city,civ,unit);
        }
        else {
          if (unit_types[unit_type].role == ROLE_TRADE) {
            result = crt_rand_at("city_process_production");
            cost_byte = (byte)(result >> 0x1f);
            units[unit].counter = (((byte)result ^ cost_byte) - cost_byte & 0xf ^ cost_byte) - cost_byte;
          }
          if ((game.message_options & MSGOFF_UNIT_BUILT) == 0) {
            city3 = 0xbb;
            if (unit_types[unit_type].role == ROLE_DIPLOMACY) {
              city3 = 0xb6;
            }
            if (unit_types[unit_type].role == ROLE_TRADE) {
              city3 = 0xbc;
            }
            text_set_label_arg(3,label_table.string_ids[city3]);
            city_notify_dialog(str_built_3,city,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)));
          }
        }
      }
      cities[city].shield_box = 0;
      if ((cities[city].flags & CITYF_AUTOBUILD) != 0) {
        city_set_production(city,99);
      }
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
      }
    }
  }
  return 0;
}

/* Part of city_process_production (004ec3fe): the notice that an AI city will
   soon complete a wonder. */
static void city_process_production_almost_wonder(uptr city,uptr civ)
{
  iptr str;
  iptr improvement2;
  int cost2;

  if (cities[city].building < -0x26) {
    if (cities[city].building < 1) {
      improvement2 = ~(int)cities[city].building + 1;
    }
    else {
      improvement2 = (iptr)cities[city].building;
    }
    cost2 = (uint)improvements[improvement2].cost * city_calc.shield_rows;
    if (((cost2 <= (cities[city].shield_box + city_calc.total_shields) - city_calc.shield_support) &&
        (game.wonder_city[improvement2 - 0x27] == -1)) && (autoplay.active == 0)) {
      str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(1,(const char *)str);
      text_set_label_arg(2,improvements[improvement2].name);
      if ((autoplay.active == 0) && (2 < game.multiplayer)) {
        net_send_text_args(0x4a,0xff,3,0,improvement2,0);
      }
      Dialog wonder_dialog(0x4000);
      wonder_dialog.load_game_text((const char *)&str_almostwonder);
      wonder_dialog.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c)),0,0);
      wonder_dialog.set_image_zoom(8);
      wonder_dialog.show_modal(0);
    }
  }
}

/* Part of city_process_production (004ec3fe): an AI city buys part of its
   production. */
static void city_process_production_ai_buy(uptr city,uptr civ,int net_shields)
{
  byte cost_byte;
  int result;
  uint base_cost;
  int civ3;
  iptr landmass3;
  int cost2;
  int refund;
  uint policy;
  int gold_spent;
  iptr improvement;
  iptr group;

  if (((cities[city].flags & CITYF_DEFENDER_LOST) != 0) ||
     (result = tile_unit_owner((int)cities[city].x,(int)cities[city].y), result < 0)) {
    city_set_production(city,99);
  }
  gold_spent = 0;
  if (cities[city].building < '\0') {
    if (cities[city].building < 1) {
      improvement = ~(int)cities[city].building + 1;
    }
    else {
      improvement = (iptr)cities[city].building;
    }
  }
  else {
    improvement = 0;
  }
  if (improvement == IMP_NOTHING) {
    cost_byte = unit_types[cities[city].building].cost;
  }
  else {
    cost_byte = improvements[improvement].cost;
  }
  base_cost = (uint)cost_byte;
  cost2 = base_cost * city_calc.shield_rows;
  landmass3 = tile_land_landmass(cities[city].x,cities[city].y);
  if (landmass3 < 0) {
    policy = 0;
  }
  else {
    policy = (uint)civs[civ].continent_policy[landmass3];
  }
  if (((((policy == 0) || (policy == 1)) || (policy == 4)) &&
      ((net_shields != 0 && (-1 < cities[city].building)))) &&
     ((int)(char)unit_types[cities[city].building].role == policy)) {
    gold_spent = civs[civ].gold >> 6;
  }
  if (0x26 < improvement) {
    group = (improvement - 0x27) / 7;
    if ((*(char *)&civs[civ].wonders_by_group[group] == '\0') ||
       (civs[civ].wonders_by_group[group] < (byte)(((i8 *)&map_views_block.human_wonders_by_group_min))[group])) {
      gold_spent = civs[civ].gold >> 6;
    }
    else if ((byte)(((i8 *)&map_views_block.human_wonders_by_group_max))[group] + 1 <
             (uint)civs[civ].wonders_by_group[group]) {
      gold_spent = civs[civ].gold >> 8;
    }
    else {
      gold_spent = civs[civ].gold >> 7;
    }
    if (0x9c3 < civs[civ].gold) {
      gold_spent = gold_spent << 1;
    }
  }
  if ((0x22 < improvement) && (improvement < 0x27)) {
    for (civ3 = 1; civ3 < 8; civ3++) {
      if (((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
         (result = civ_spaceship_exists(civ3), result != 0)) {
        gold_spent = civs[civ].gold >> 7;
        break;
      }
    }
    result = ai_spaceship_race_behind(civ);
    if (result != 0) {
      gold_spent = gold_spent << 1;
    }
  }
  if ((((cities[city].flags & CITYF_DISORDER) != 0) && (improvement != IMP_NOTHING)) && (cities[city].shield_box != 0)
     ) {
    gold_spent = civs[civ].gold >> 3;
  }
  result = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
  if (((result < 0) || ((cities[city].flags & CITYF_DEFENDER_LOST) != 0)) &&
     ((-1 < cities[city].building && (cities[city].shield_box != 0)))) {
    gold_spent = civs[civ].gold >> 3;
  }
  if ((improvement == IMP_PALACE) && (cities[city].shield_box != 0)) {
    gold_spent = civs[civ].gold >> 3;
  }
  if (((cities[city].building == UNIT_NUCLEAR_MISSILE) && (civs[civ].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) &&
     (cities[city].shield_box != 0)) {
    gold_spent = civs[civ].gold >> 4;
  }
  if (2000 < civs[civ].gold) {
    gold_spent += (civs[civ].gold >> 9);
  }
  gold_spent = int_clamp(0,gold_spent,cost2 - cities[city].shield_box);
  cities[city].shield_box += (short)gold_spent;
  refund = ((int)((uint)game.difficulty * gold_spent) / 10) /
             (int)(8 - (uint)game.power_rank[game.best_human_rank]);
  if (civs[game.worst_human_rank].gold < civs[civ].gold) {
    if (999 < civs[civ].gold) {
      refund = refund / 2;
    }
    if (1999 < civs[civ].gold) {
      refund = 0;
    }
  }
  civs[civ].gold -= (gold_spent - refund);
  city_process_production_almost_wonder(city,civ);
}

/* 004ec3fe auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_process_production(uptr city)

{
  iptr unit_owner;
  int result;
  i8 *done_sprite;
  uptr civ;
  iptr improvement2;
  uptr city2;
  iptr landmass3;
  int cost2;
  iptr part;
  iptr wonder;
  int net_shields;
  iptr improvement;
  int acquired;
  
  improvement = -1;
  acquired = 0;
  city_calc.completed_improvement = -1;
  civ = (uptr)cities[city].owner;
  net_shields = city_calc.total_shields - city_calc.shield_support;
  if ((cities[city].flags & CITYF_DISORDER) != 0) {
    net_shields = 0;
  }
  if (net_shields < 1) {
    net_shields = 0;
  }
  if (cities[city].building != -IMP_CAPITALIZATION) {
    cities[city].shield_box += (short)net_shields;
  }
  if (((civ == 0) &&
      (landmass3 = tile_landmass(cities[city].x,cities[city].y),
      0x10 < civs[civ].continent_military[landmass3])) &&
     (unit_owner = tile_unit_owner((int)cities[city].x,(int)cities[city].y), -1 < unit_owner)) {
    cities[city].shield_box = 0;
  }
  if (cities[city].building < '\0') {
    if (cities[city].building < 1) {
      improvement = ~(int)cities[city].building + 1;
    }
    else {
      improvement = (iptr)cities[city].building;
    }
    wonder = -1;
    cost2 = (uint)improvements[improvement].cost * city_calc.shield_rows;
    if (0x26 < improvement) {
      wonder = improvement - 0x27;
    }
    if ((autoplay.active == 0) &&
       ((((-1 < wonder && (game.wonder_city[wonder] != -1)) ||
         ((wonder < 0 && (result = city_has_improvement(city,improvement), result != 0)))) &&
        (text_set_label_arg(1,improvements[improvement].name), (game.message_options & MSGOFF_INVALID_BUILD) == 0)))) {
      if (wonder < 0) {
        done_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
      }
      else {
        done_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c;
      }
      city_notify_dialog(str_badbuild,city,(Sprite *)done_sprite);
    }
    if (((improvement < 0x23) || (0x25 < improvement)) ||
       (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
        ((result = civ_spaceship_launched(civ), result == 0 &&
         (result = civ_spaceship_part_maxed(civ,improvement), result == 0)))))) {
      if ((cost2 <= cities[city].shield_box) && (improvement != IMP_CAPITALIZATION)) {
        cities[city].flags = cities[city].flags & ~CITYF_WONDER_BUILT;
        acquired = 0;
        if (wonder < 0) {
          result = city_has_improvement(city,improvement);
          if (result != 0) {
            improvement = -1;
          }
        }
        else {
          if ((game.wonder_city[wonder] == -1) &&
             ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
            for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
              if ((((cities[city2].id != 0) &&
                   (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) != 0)) &&
                  (cities[city2].building == cities[city].building)) &&
                 (result = (uint)improvements[improvement].cost * (uint)cosmic.shield_box_rows,
                 result - cities[city2].shield_box == 0 || result < cities[city2].shield_box))
              return;
            }
          }
          if (((game.multiplayer < 3) && (game.wonder_city[wonder] == -1)) ||
             ((2 < game.multiplayer &&
              (acquired = city_acquire_wonder(civ,city,wonder,cost2,improvement),
              acquired != 0)))) {
            if (game.multiplayer < 3) {
              game.wonder_city[wonder] = (i16)city;
              cities[city].flags = cities[city].flags | CITYF_WONDER_BUILT;
            }
          }
          else {
            wonder = -1;
            improvement = -1;
            if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
              city_set_production(city,99);
            }
          }
        }
        if (((improvement < 0x23) || (0x25 < improvement)) ||
           ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
          city_process_production_complete_improvement(city,civ,cost2,wonder,improvement,acquired);
        }
        else {
          result = civ_spaceship_launched(civ);
          if (result == 0) {
            part = spaceship_choose_part_group(civ,improvement - 0x23);
            if (part < 0) {
              city_set_production(city,99);
            }
            else {
              improvement2 = part + 0x23;
              cities[city].building = -(char)improvement2;
              cost2 = (uint)improvements[improvement2].cost * city_calc.shield_rows;
              improvement = improvement2;
              if (cost2 <= cities[city].shield_box) {
                city_process_production_complete_improvement(city,civ,cost2,wonder,improvement,acquired);
              }
            }
          }
          else {
            city_set_production(city,99);
          }
        }
      }
    }
    else {
      text_set_label_arg(1,improvements[improvement].name);
      city_notify_dialog(str_badspace,city,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)));
    }
  }
  else {
    if (city_process_production_unit(city,civ) != 0) return;
  }
  if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
       ((cities[city].flags & CITYF_AUTOBUILD) != 0)) &&
      ((cities[city].shield_box == 0 || ((cities[city].building < '\0' && (improvement < 0))))))
     && (city_set_production(city,99), cities[city].building == 'c')) {
    cities[city].flags = cities[city].flags & ~(CITYF_AUTOBUILD | CITYF_AUTO_UNITS_ONLY | CITYF_AUTO_IMPROVEMENTS_ONLY);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    city_process_production_ai_buy(city,civ,net_shields);
  }
  cities[city].flags = cities[city].flags & ~CITYF_DEFENDER_LOST;
  return;
}
