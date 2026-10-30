/* The game, the computer players: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00442885 auto */
i32 __cdecl ai_city_touches_landmass(iptr city,int landmass)

{
  short x2;
  short y2;
  int y;
  i32 result;
  iptr x;
  int check;
  uptr dir;
  
  x2 = cities[city].x;
  y2 = cities[city].y;
  y = tile_landmass((int)x2,(int)y2);
  if (y == landmass) {
    result = 1;
  }
  else {
    for (dir = 0; (int)dir < 8; dir++) {
      x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
      y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
      check = map_in_bounds(x,y);
      if ((check != 0) && (check = tile_is_ocean(x,y), check != 0)) {
        x = tile_landmass(x,y);
        y = landmass_borders_body(landmass,x);
        if (y != 0) {
          return 1;
        }
        dir += (2 - (dir & 1));
      }
    }
    result = 0;
  }
  return result;
}

/* Part of ai_advance_value (004bdb2c): each tribe's favorite advances add to
   the value (and some tribes value expansion or free land). */
static void ai_advance_tribe_bonus(iptr civ, iptr advance,
                                              int &free_continent,
                                              uint &expand_bonus, int &value)
{
  switch(civs[civ].tribe) {
  case 0:
    if (((advance == ADV_IRON_WORKING) || (advance == ADV_BRONZE_WORKING)) || (advance == ADV_WARRIOR_CODE)) {
      value += 2;
    }
    if (advance == ADV_MONOTHEISM) {
      value--;
    }
    break;
  case 1:
    if (advance == ADV_CODE_OF_LAWS) {
      value++;
    }
    break;
  case 2:
    if (advance == ADV_BANKING) {
      value++;
    }
    if (advance == ADV_THEOLOGY) {
      value++;
    }
    if (advance == ADV_PHILOSOPHY) {
      value++;
    }
    break;
  case 3:
    if (advance == ADV_MASONRY) {
      value += 2;
    }
    break;
  case 4:
    if (advance == ADV_DEMOCRACY) {
      value += 2;
    }
    if (advance == ADV_COMMUNISM) {
      value--;
    }
    if (advance == ADV_ROCKETRY) {
      value++;
    }
    if (advance == ADV_COMPUTERS) {
      value++;
    }
    if (advance == ADV_LEADERSHIP) {
      value++;
    }
    break;
  case 5:
    if (advance == ADV_POLYTHEISM) {
      value++;
    }
    if (advance == ADV_BRONZE_WORKING) {
      value++;
    }
    if (advance == ADV_ALPHABET) {
      value++;
    }
    if (advance == ADV_MAP_MAKING) {
      value++;
    }
    if (advance == ADV_MONOTHEISM) {
      value--;
    }
    if (advance == ADV_PHILOSOPHY) {
      value += 2;
    }
    break;
  case 6:
    if (advance == ADV_POLYTHEISM) {
      value += 2;
    }
    if (advance == ADV_HORSEBACK_RIDING) {
      value++;
    }
    if (advance == ADV_MYSTICISM) {
      value++;
    }
    if (advance == ADV_CEREMONIAL_BURIAL) {
      value++;
    }
    if (advance == ADV_MONOTHEISM) {
      value--;
    }
    break;
  case 7:
    if (advance == ADV_COMMUNISM) {
      value += 2;
    }
    if (advance == ADV_PHILOSOPHY) {
      value++;
    }
    if (advance == ADV_GUERRILLA_WARFARE) {
      value++;
    }
    break;
  case 8:
    if (advance == ADV_POLYTHEISM) {
      value += 2;
    }
    if (advance == ADV_HORSEBACK_RIDING) {
      value++;
    }
    if (advance == ADV_MYSTICISM) {
      value++;
    }
    if (advance == ADV_CEREMONIAL_BURIAL) {
      value++;
    }
    if (advance == ADV_BRONZE_WORKING) {
      value--;
    }
    if (advance == ADV_IRON_WORKING) {
      value--;
    }
    break;
  case 9:
    if (advance == ADV_LEADERSHIP) {
      value++;
    }
    if (advance == ADV_TACTICS) {
      value++;
    }
    if (advance == ADV_CONSCRIPTION) {
      value++;
    }
    break;
  case 10:
    if (advance == ADV_GUNPOWDER) {
      value -= 2;
    }
    if (advance == ADV_MONOTHEISM) {
      value--;
    }
    break;
  case 0xb:
    if (free_continent != 0) {
      value -= expand_bonus;
    }
    if (advance == ADV_GUNPOWDER) {
      value++;
    }
    break;
  case 0xc:
    if (advance == ADV_MONOTHEISM) {
      value++;
    }
    if (expand_bonus != 0) {
      value++;
    }
    break;
  case 0xf:
    if (advance == ADV_STEEL) {
      value++;
    }
    if (advance == ADV_MINIATURIZATION) {
      value++;
    }
    break;
  case 0x10:
    if (advance == ADV_MAP_MAKING) {
      value++;
    }
    break;
  case 0x11:
    if (1 < expand_bonus) {
      value++;
    }
    if (advance == ADV_MONOTHEISM) {
      value++;
    }
    break;
  case 0x12:
  case 0x13:
    if (advance == ADV_POLYTHEISM) {
      value += 2;
    }
    if (advance == ADV_HORSEBACK_RIDING) {
      value++;
    }
    if (advance == ADV_MYSTICISM) {
      value++;
    }
    if (advance == ADV_CEREMONIAL_BURIAL) {
      value++;
    }
    if (advance == ADV_MONOTHEISM) {
      value--;
    }
    break;
  case 0x14:
    if (advance == ADV_HORSEBACK_RIDING) {
      value += 2;
    }
  }
}

/* Part of ai_advance_value (004bdb2c): for an AI civ: advances leading to the
   units and wonders that answer a human civ's lead count more (once humans
   have advance 0x4c, or the prerequisite of unit 0x1c), and advances that end
   a wonder: less if it is its own, more if a human civ's. */
static void ai_advance_ai_bonus(iptr advance, iptr civ, int &check,
                                           int &city2, int &value)
{
  iptr has_prereq;
  iptr improvement;
  iptr wonder;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    if ((game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) != 0) {
      has_prereq = advance_has_prereq(ADV_FUSION_POWER,advance);
      if (has_prereq != 0) {
        value += 2;
      }
      for (improvement = 0x23; improvement < 0x26; improvement++) {
        if (improvements[improvement].preq == advance) {
          value += 3;
        }
      }
    }
    city2 = value;
    if ((((-1 < unit_types[UNIT_BOMBER].prerequisite) &&
         ((game.human_players & game.advance_known_by[unit_types[UNIT_BOMBER].prerequisite]) != 0)) &&
        (check = advance_has_prereq((int)unit_types[UNIT_BOMBER].prerequisite,advance), check != 0)) &&
       (city2 = value + 2, unit_types[UNIT_BOMBER].prerequisite == advance)) {
      city2 = value + 5;
    }
    value = city2;
    for (wonder = 0; wonder < 0x1c; wonder++) {
      if (wonder_expires[wonder] == advance) {
        city2 = civ_has_wonder(civ,wonder);
        if ((city2 != 0) && (advance != ADV_INDUSTRIALIZATION)) {
          value -= 2;
        }
        city2 = wonder_get_city(wonder);
        if ((-1 < city2) && (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) != 0))
        {
          value += 2;
        }
      }
    }
  }
}

/* Part of ai_advance_value (004bdb2c): advances toward the settling units
   (0x20, 0x21, 0x23, 0x2b) are worth expand_bonus (3 down to 1, doubled with
   flag 4 of the game) where it has a landmass of its own to fill
   (free_continent), else 1 while it has met no human civ. */
static void ai_advance_expansion(iptr civ, iptr advance, int &city2,
                                            int &free_continent,
                                            iptr &continent, iptr &civ2,
                                            uint &expand_bonus, int &value)
{
  bool human_contact;
  bool contested;
  iptr settler_prereq;
  settler_prereq = advance_has_prereq((int)unit_types[UNIT_TRIREME].prerequisite,advance);
  if (settler_prereq == 0) {
    city2 = advance_has_prereq((int)unit_types[UNIT_CARAVEL].prerequisite,advance);
    if ((city2 == 0) &&
       (city2 = advance_has_prereq((int)unit_types[UNIT_FRIGATE].prerequisite,advance), city2 == 0)) {
      city2 = advance_has_prereq((int)unit_types[UNIT_TRANSPORT].prerequisite,advance);
      if (city2 == 0) {
        expand_bonus = 0;
      }
      else {
        expand_bonus = 1;
      }
    }
    else {
      expand_bonus = 2;
    }
  }
  else {
    expand_bonus = 3;
  }
  if ((game.game_flags & GAME_LARGE_MAP) != 0) {
    expand_bonus = expand_bonus << 1;
  }
  if (expand_bonus != 0) {
    free_continent = 0;
    human_contact = false;
    for (continent = 1; continent < 0x3f; continent++) {
      if (*(char *)&civs[civ].continent_cities[continent] != '\0') {
        contested = false;
        for (civ2 = 0; civ2 < 8; civ2++) {
          if ((civ != civ2) &&
             ((*(char *)&civs[civ].continent_cities[continent] != '\0' ||
              (*(short *)&civs[civ].continent_military[continent] != 0)))) {
            contested = true;
            if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
              human_contact = true;
            }
            break;
          }
        }
        if (!contested) {
          free_continent = 1;
          break;
        }
      }
    }
    if (free_continent == 0) {
      if ((!human_contact) || ((game.game_flags & GAME_LARGE_MAP) != 0)) {
        value++;
      }
    }
    else {
      value += expand_bonus;
    }
  }
}

/* 004bdb2c auto */
int __cdecl ai_advance_value(iptr civ,iptr advance)

{
  byte size_limit;
  int check;
  int city2;
  uint size_needed;
  int civilize_mod;
  int free_continent;
  iptr city;
  iptr continent;
  int max_size;
  iptr advance2;
  iptr civ2;
  uint expand_bonus;
  int value;
  
  civilize_mod = (int)leaders[civs[civ].tribe].civilize;
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) && (-1 < civilize_mod)) {
    for (civ2 = 1; civ2 < 8; civ2++) {
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)) &&
         (civs[civ].num_advances < civs[civ2].num_advances)) {
        for (continent = 1; continent < 0x3f; continent++) {
          if ((*(char *)&civs[civ].continent_cities[continent] != '\0') &&
             (*(char *)&civs[civ2].continent_cities[continent] != '\0')) {
            if (-1 < civilize_mod) {
              civilize_mod--;
            }
            if (6 < civs[civ2].reputation) {
              civilize_mod = -1;
            }
          }
        }
      }
    }
  }
  value = advances[advance].civilized_mod * civilize_mod + (int)advances[advance].ai_value;
  ai_advance_expansion(civ, advance, city2, free_continent, continent, civ2, expand_bonus, value);
  if (improvements[IMP_PORT_FACILITY].preq == advance) {
    value = value + ((int)((int)civs[civ].num_ships +
                              ((int)civs[civ].num_ships >> 0x1f & 3U)) >> 2);
  }
  ai_advance_ai_bonus(advance, civ, check, city2, value);
  if ((improvements[IMP_AQUEDUCT].preq == advance) || (improvements[IMP_SEWER_SYSTEM].preq == advance)) {
    max_size = 0;
    for (city = 0; city < game.num_cities; city++) {
      city2 = max_size;
      if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
         (city2 = (int)cities[city].size, cities[city].size <= max_size)) {
        city2 = max_size;
      }
      max_size = city2;
    }
    size_limit = cosmic.sewer_size;
    if (improvements[IMP_AQUEDUCT].preq == advance) {
      size_limit = cosmic.aqueduct_size;
    }
    size_needed = (uint)size_limit;
    if ((int)size_needed <= max_size) {
      value += 2;
    }
  }
  if ((-1 < leaders[civs[civ].tribe].civilize) &&
     (city2 = civ_has_advance(civ,ADV_UNIVERSITY), city2 != 0)) {
    city2 = advance_has_prereq(ADV_COMPUTERS,advance);
    if (city2 != 0) {
      value = value + leaders[civs[civ].tribe].civilize + 1;
    }
    if (advance == ADV_COMPUTERS) {
      value += 2;
    }
  }
  check = civ_has_advance(civ,ADV_INVENTION);
  city2 = value;
  if ((check != 0) && (check = advance_has_prereq(ADV_INDUSTRIALIZATION,advance), check != 0)) {
    check = civ_has_advance(civ,ADV_NAVIGATION);
    city2 = value + 1;
    if (check != 0) {
      city2 = value + 2;
    }
  }
  value = city2;
  ai_advance_tribe_bonus(civ, advance, free_continent, expand_bonus, value);
  if (game.advance_known_by[advance] == '\0') {
    value++;
  }
  for (advance2 = 0; advance2 < 100; advance2++) {
    if (((advances[advance2].preq[0] == advance) || (advances[advance2].preq[1] == advance)) &&
       (city2 = civ_has_advance(civ,advance2), city2 != 0)) {
      value--;
    }
  }
  if (value < 2) {
    value = 1;
  }
  return value;
}

/* 004f03b7 auto */
i32 __cdecl ai_find_road_link_tile(iptr city)

{
  bool road_ok;
  iptr landmass;
  int civ;
  int x;
  int y;
  iptr landmass2;
  iptr distance;
  int dir;
  uint tile_imps;
  byte *tile;
  iptr city2;
  int steps;
  int y2;
  int x2;
  int pass;
  
  landmass = tile_land_landmass((int)cities[city].x,(int)cities[city].y);
  civ = (int)cities[city].owner;
  if (1 < civs[civ].continent_cities[landmass]) {
    for (pass = 0; pass < 2; pass++) {
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if (((cities[city2].id != 0) && (city != city2)) &&
           ((cities[city2].owner == civ ||
            ((pass != 0 &&
             ((*(byte *)&civs[civ].treaties[cities[city2].owner] & (TREATY_PEACE | TREATY_ALLIANCE)) != 0)))))) {
          x = (int)cities[city2].x;
          y = (int)cities[city2].y;
          x2 = (int)cities[city].x;
          y2 = (int)cities[city].y;
          steps = 0;
          landmass2 = tile_land_landmass(x,y);
          if ((landmass == landmass2) &&
             (distance = map_distance(x,y,x2,y2), distance < 0x17)) {
            pathfind.road_planning = 1;
            pathfind.owner_civ = 0xffffffff;
            pathfind.unit_type = 2;
            road_ok = true;
            path_queue.goal_x = x;
            path_queue.goal_y = y;
            while ((dir = map_path_next_dir(x2,y2,99), -1 < dir && (dir != 8))) {
              x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
              y2 += (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
              if ((x2 == x) && (y2 == y)) break;
              dir = tile_city_owner(x2,y2);
              if (dir < 0) {
                tile_imps = tile_improvements(x2,y2);
                if ((tile_imps & TILE_ROAD) == 0) {
                  tile = (byte *)tile_at(x2,y2);
                  if (((*tile & 0x80) == 0) || (dir = civ_has_advance(civ,ADV_BRIDGE_BUILDING), dir != 0))
                  {
                    road_ok = false;
                  }
                }
                else {
                  tile_imps = tile_improvements(x2,y2);
                  if (((tile_imps & TILE_RAILROAD) == 0) && (dir = civ_has_advance(civ,ADV_RAILROAD), dir != 0))
                  {
                    road_ok = false;
                  }
                }
              }
              steps++;
              if ((0x32 < steps) || (!road_ok)) break;
            }
            pathfind.road_planning = 0;
            if (((!road_ok) &&
                ((x = tile_city_or_unit_owner(x2,y2), x < 0 || (civ == x))
                )) && ((x = tile_owner_at_peace(x2,y2,civ), x < 0 ||
                       (civ == x)))) {
              city_turn.tile_job_found = 1;
              city_calc.tile_job_x = x2;
              city_calc.tile_job_y = y2;
              return 1;
            }
          }
        }
      }
    }
  }
  return 0;
}

/* 00531287 auto */
int __cdecl ai_unit_role(iptr unit)

{
  int ai_role;
  
  ai_role = (int)(char)unit_types[units[unit].type].role;
  if ((ai_role == ROLE_SETTLE) && ((units[unit].flags & UNITF_AI_COLONIST) != 0)) {
    ai_role = 0x15;
  }
  return ai_role;
}

/* 005312e4 auto */
iptr __cdecl ai_ship_best_landing_dir(iptr unit)

{
  short x3;
  short y3;
  int civ;
  iptr x2;
  int y2;
  int y;
  iptr x;
  int owner_civ;
  iptr best_dir;
  iptr dir2;
  byte owner_byte;
  iptr dir;
  int best_score;
  int score;
  
  x3 = units[unit].x;
  y3 = units[unit].y;
  civ = (int)units[unit].owner;
  best_score = -1;
  best_dir = -1;
  for (dir = 0; dir < 9; dir++) {
    x2 = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x3);
    y2 = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y3;
    y = map_in_bounds(x2,y2);
    if (((y != 0) && (y = tile_is_ocean(x2,y2), y != 0)) &&
       ((dir == 8 || (y = tile_unit_owner(x2,y2), y < 0)))) {
      score = 0;
      for (dir2 = 0; dir2 < 8; dir2++) {
        x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir2] + x2);
        y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir2] + y2;
        owner_civ = map_in_bounds(x,y);
        if ((((owner_civ != 0) && (owner_civ = tile_is_ocean(x,y), owner_civ == 0)) &&
            (owner_civ = tile_unit_owner(x,y), owner_civ < 0)) &&
           (((owner_civ = tile_owner_or_fertility(x,y), 7 < owner_civ ||
             (owner_byte = (byte)owner_civ, (1 << (owner_byte & 0x1f) & (uint)game.human_players) == 0)) ||
            ((*(byte *)&civs[civ].treaties[owner_civ] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)))) {
          owner_civ = tile_city_owner(x,y);
          if (((-1 < owner_civ) && (owner_civ != civ)) &&
             ((y = tile_unit_owner(x,y), y < 0 &&
              ((y = diplo_is_hostile(civ,owner_civ), y != 0 && (dir == 8)))))) {
            score += 0xc;
          }
          score++;
        }
      }
      if (best_score <= score) {
        best_score = score;
        best_dir = dir;
      }
    }
  }
  return best_dir;
}

/* 00531653 auto */
void __cdecl ai_ship_goto_near_shore(iptr unit,i32 goto_mode,iptr x,int y)

{
  bool found_land;
  int ship_landmass;
  int target_landmass;
  iptr x3;
  int y3;
  int y2;
  iptr x2;
  int check;
  int dest_y;
  iptr dest_x;
  iptr dir;
  iptr offset;
  
  dest_x = x;
  dest_y = y;
  ship_landmass = tile_landmass((int)units[unit].x,(int)units[unit].y);
  target_landmass = tile_landmass(x,y);
  for (offset = 0; y3 = dest_y, x3 = dest_x, offset < 0x14; offset++) {
    x3 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[offset] + x);
    y3 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[offset] + y;
    y2 = map_in_bounds(x3,y3);
    if (((y2 != 0) && (y2 = tile_is_ocean(x3,y3), y2 != 0)) &&
       (y2 = tile_landmass(x3,y3), y2 == ship_landmass)) {
      found_land = false;
      for (dir = 0; dir < 8; dir++) {
        x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x3);
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
        check = map_in_bounds(x2,y2);
        if (((check != 0) && (check = tile_is_ocean(x2,y2), check == 0)) &&
           ((check = tile_landmass(x2,y2), check == target_landmass &&
            (y2 = tile_city_owner(x2,y2), y2 < 0)))) {
          found_land = true;
        }
      }
      if (found_land) break;
    }
  }
  dest_x = x3;
  dest_y = y3;
  unit_set_goto(unit,goto_mode,dest_x,dest_y);
  return;
}

/* Where ai_civ_turn_assess's unit loop goes on after one of its parts:
   NEXT is the line after the call, CONTINUE the next unit. */
enum AiAssessNext {
  AI_ASSESS_NEXT,
  AI_ASSESS_CONTINUE
};

/* The locals of ai_civ_turn_assess that its parts share: references to the
   entry point's own. */
struct AiCivAssess {
  uptr &civ;
  char *&counter;
  iptr &has_para;
  int &city6;
  int &y;
  uptr &owner_civ;
  i32 &imps;
  uptr &unit;
  iptr &city;
  int &attack_count;
  int (&goal_weight)[48];
  uint &is_human;
  iptr &continent;
  uptr &unit_type;
  uint &old_policy;
  int &y2;
  int &scenario_mode;
  int &x2;
  int (&settler_count)[64];
  int &rival_military;
  int &threat_count;
  int &settlers_total;
  int (&settle_goals)[64];
  uptr &civ2;
  int &defend_count;
  int &rival_cities;
  iptr &goal;
  int &land_city_count;
};

/* Part of ai_civ_turn_assess (0053184d): an AI unit (not a trade unit bound
   for a foreign city) takes the nearest worthwhile goal of its role on its
   landmass (air units any in range): weighed by priority and how many units
   the goal has drawn; at the goal it stops, a defender in a fortress may
   stay, else a goto there. */
static int ai_assess_unit_goal(AiCivAssess *o)
{
  uint tile_imps;
  int ai_role;
  int goal_dist;
  int best_dist;
  iptr best_goal;
  uptr &civ = o->civ; int &city6 = o->city6; int &y = o->y;
  uptr &owner_civ = o->owner_civ; i32 &imps = o->imps; uptr &unit = o->unit;
  int (&goal_weight)[48] = o->goal_weight; uint &is_human = o->is_human;
  iptr &continent = o->continent; int &y2 = o->y2; int &x2 = o->x2;
  uptr &civ2 = o->civ2; iptr &goal = o->goal;
  best_dist = 9999;
  best_goal = -1;
  ai_role = ai_unit_role(unit);
  if (((((ai_role != 7) || (units[unit].orders != ORDER_GOTO)) ||
       (y = map_in_bounds((int)units[unit].goto_x,(int)units[unit].goto_y)
       , y == 0)) ||
      ((civ2 = tile_city_owner((int)units[unit].goto_x,
                                   (int)units[unit].goto_y), (int)civ2 < 0 ||
       (civ == civ2)))) &&
     ((unit_types[units[unit].type].role != ROLE_ATTACK ||
      (unit_types[units[unit].type].attack != '\0')))) {
    for (goal = 0; goal < 0x30; goal++) {
      if (((civs[civ].ai_goals[goal].role != -1) &&
          ('\0' < civs[civ].ai_goals[goal].priority)) &&
         (y = map_in_bounds((int)civs[civ].ai_goals[goal].x,
                                (int)civs[civ].ai_goals[goal].y), y != 0)) {
        if (((civs[civ].ai_goals[goal].role == 18) &&
            (ai_role == 2)) &&
           ((unit_types[units[unit].type].flags & UTYPE_SPOT_SUBMARINES) != 0)) {
          ai_role = 0x12;
        }
        if (((civs[civ].ai_goals[goal].role == ai_role) &&
            (y = tile_landmass((int)civs[civ].ai_goals[goal].x,
                                   (int)civs[civ].ai_goals[goal].y), y == continent))
           || (((unit_types[units[unit].type].domain == DOMAIN_AIR &&
                ((char)unit_types[UNIT_BOMBER].attack <=
                 (char)unit_types[units[unit].type].attack)) &&
               ((civs[civ].ai_goals[goal].role == ai_role ||
                (civs[civ].ai_goals[goal].role == ROLE_NAVAL_SUPERIORITY)))))) {
          goal_dist = map_distance_diag(x2,y2,
                                        (int)civs[civ].ai_goals[goal].x,
                                        (int)civs[civ].ai_goals[goal].y);
          if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
            if (unit_types[units[unit].type].range == 1) {
              if (!(goal_dist <=
                    (int)(char)unit_types[units[unit].type].move /
                    (int)(uint)cosmic.road_move_mult - 2 >> 1)) continue;
            }
            else {
              if (!(((*(byte *)&civs[civ].ai_goals[goal].priority & 1) != 0)
                    && ((((int)game.turn >> 1) + goal & 1U) != 0))) continue;
              goal_dist = (int)((((goal_dist <=
                                 (int)(char)unit_types[units[unit].type].move /
                                 (int)(uint)cosmic.road_move_mult) - 1 & 0xfffffffe) + 4
                               ) * goal_dist) /
                         (civs[civ].ai_goals[goal].priority + 1);
            }
          }
          else {
            goal_dist = ((goal_weight[goal] /
                        ((civs[civ].ai_goals[goal].role ==
                              ROLE_ATTACK) + 1)) * goal_dist) /
                       (civs[civ].ai_goals[goal].priority + 1);
          }
          if (((units[unit].orders != ORDER_FORTIFYING) &&
              (units[unit].orders != ORDER_FORTIFIED)) ||
             ((1 < civs[civ].ai_goals[goal].priority &&
              (((goal_dist <=
                 civs[civ].ai_goals[goal].priority * city6 ||
                (goal_weight[goal] /
                 ((civs[civ].ai_goals[goal].role == ROLE_ATTACK)
                      + 1) == city6)) &&
               (y = tile_city_owner(x2,y2), y < 0)))))) {
            if (((is_human != 0) && (goal_weight[goal] == city6)) &&
               (tile_imps = tile_improvements((int)civs[civ].ai_goals[goal].x,
                                           (int)civs[civ].ai_goals[goal].y),
               (tile_imps & TILE_POLLUTION) != 0)) {
              goal_dist = (int)(goal_dist + (goal_dist >> 0x1f & 7U)) >> 3;
            }
            if ((goal_dist < best_dist) &&
               (goal_dist / city6 <
                civs[civ].ai_goals[goal].priority * 4)) {
              best_dist = goal_dist;
              best_goal = goal;
            }
          }
        }
      }
    }
    if (-1 < best_goal) {
      if ((civs[civ].ai_goals[best_goal].x == units[unit].x)
         && (civs[civ].ai_goals[best_goal].y == units[unit].y)
         ) {
        units[unit].orders = ORDER_AI_IDLE;
        units[unit].flags = units[unit].flags | UNITF_GOAL_REACHED;
        return AI_ASSESS_CONTINUE;
      }
      if ((ai_role == 1) &&
         (imps = tile_improvements((int)units[unit].x,(int)units[unit].y),
         ((byte)imps & 0x42) == 0x40)) {
        if ((*(byte *)&civs[civ].ai_goals[best_goal].priority & 1) == 0)
        return AI_ASSESS_CONTINUE;
        civ_add_standing_goal(civ,(int)units[unit].x,(int)units[unit].y,ROLE_DEFEND,2);
      }
      if (((unit_types[units[unit].type].flags & UTYPE_PARADROP) == 0) ||
         (owner_civ = tile_city_owner((int)civs[civ].ai_goals[best_goal].x,
                                   (int)civs[civ].ai_goals[best_goal].y), owner_civ == civ))
      {
        unit_set_goto(unit,0x31,
                           (int)civs[civ].ai_goals[best_goal].x,
                           (int)civs[civ].ai_goals[best_goal].y);
        if ((unit_types[units[unit].type].role == ROLE_SETTLE) &&
           (y = tile_city_owner((int)civs[civ].ai_goals[best_goal].x,
                                    (int)civs[civ].ai_goals[best_goal].y), -1 < y)) {
          civs[civ].ai_goals[best_goal].priority = 0;
        }
        goal_weight[best_goal]++;
      }
    }
  }
  return AI_ASSESS_NEXT;
}

/* Part of ai_civ_turn_assess (0053184d): in a scenario with flag 0x8000,
   civ 3's units of types 10 and 22 between rows 40 and 92 make for the tile
   43,53 until it is theirs (for type 10, until three units hold it);
   between turns 4 and 12 the type 22 ones then head for 76,34, or one in
   three for 67,91. */
static int ai_assess_scenario_unit(AiCivAssess *o)
{
  u16 saved_flags;
  iptr unit2;
  uptr &civ = o->civ; int &y = o->y; uptr &owner_civ = o->owner_civ;
  i32 &imps = o->imps; uptr &unit = o->unit; int &y2 = o->y2;
  int &scenario_mode = o->scenario_mode;
  if (((scenario_mode != 0) &&
      (((units[unit].type == UNIT_ARMOR || (units[unit].type == UNIT_ALPINE_TROOPS)) &&
       (0x27 < y2)))) && (y2 < 0x5d)) {
    saved_flags = units[unit].flags;
    if (((game.turn == 1) || (owner_civ = tile_owner(0x2b,0x35), owner_civ != civ)) ||
       ((units[unit].type == UNIT_ALPINE_TROOPS) &&
        (imps = 2, unit2 = tile_first_unit(0x2b,0x35),
         y = unit_stack_sum(unit2,imps), y < 3))) {
      unit_set_goto(unit,0x31,0x2b,0x35);
      units[unit].flags = units[unit].flags & ~(UNITF_SCRATCH | UNITF_SPY_ON_MISSION);
      return AI_ASSESS_CONTINUE;
    }
    if (((units[unit].type == UNIT_ARMOR) && (3 < game.turn)) && (game.turn < 0xd)) {
      if ((units[unit].flags & (UNITF_SCRATCH | UNITF_SPY_ON_MISSION)) == 0) {
        y = crt_rand_at("ai_civ_turn_assess");
        if (y % 3 == 0) {
          units[unit].flags = units[unit].flags | UNITF_SPY_ON_MISSION;
        }
        else {
          units[unit].flags = units[unit].flags | UNITF_SCRATCH;
        }
      }
      if (((units[unit].flags & UNITF_SCRATCH) == 0) ||
         (owner_civ = tile_owner(0x4c,0x22), owner_civ == civ)) {
        if ((units[unit].flags & UNITF_SPY_ON_MISSION) != 0) {
          owner_civ = tile_owner(0x3e,0x48);
          if (owner_civ == civ) {
            owner_civ = tile_owner(0x43,0x5b);
            if (owner_civ != civ) {
              unit_set_goto(unit,0x31,0x43,0x5b);
            }
          }
          else {
            unit_set_goto(unit,0x31,0x43,0x5b);
          }
        }
      }
      else {
        unit_set_goto(unit,0x31,0x4c,0x22);
      }
    }
    units[unit].flags = saved_flags;
  }
  return AI_ASSESS_NEXT;
}

/* Part of ai_civ_turn_assess (0053184d): now and then an AI unit that is
   obsolete (or a warrior under a government past despotism on a defensive
   landmass), not next to an enemy and not needed where it stands, is
   disbanded (its city gets half its cost back in shields); an obsolete one
   is upgraded in place instead to the unit of its role that replaces it
   when no human sees it and it holds a fortress or a city alone. */
static int ai_assess_unit_upgrade(AiCivAssess *o)
{
  uptr city5;
  uptr &civ = o->civ; int &y = o->y; i32 &imps = o->imps; uptr &unit = o->unit;
  uint &is_human = o->is_human; iptr &continent = o->continent;
  uptr &unit_type = o->unit_type; int &y2 = o->y2; int &x2 = o->x2;
  if (((((int)game.turn & unit & 0xf) == 0) && (is_human == 0)) &&
     ((((-1 < unit_types[units[unit].type].obsolete_by &&
        (y = civ_has_advance(civ,(int)unit_types[units[unit].type].
                                                 obsolete_by), y != 0)) ||
       (((GOV_DESPOTISM < civs[civ].government &&
         (*(char *)&civs[civ].continent_policy[continent] == 4)) &&
        (units[unit].type == UNIT_WARRIORS)))) &&
      ((y = tile_zoc_unit_adjacent(x2,y2,civ), y == 0 &&
       (((y = tile_city_owner(x2,y2), y < 0 ||
         (y = unit_stack_sum(unit,2), 1 < y)) &&
        ((y = tile_is_ocean(x2,y2), y == 0 &&
         ((civs[civ].continent_flags[continent] & 0x10) == 0)))))))))) {
    if ((((unit_types[units[unit].type].obsolete_by < '\0') ||
         (y = civ_has_advance(civ,(int)unit_types[units[unit].type].
                                                  obsolete_by), y == 0)) ||
        ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0))
       || (((int)units[unit].owner == (uint)(byte)(*(i8 *)&human_civ) ||
           ((imps = tile_improvements(x2,y2),
            ((byte)imps & 0x42) != 0x40 &&
            ((y = tile_city_owner(x2,y2), y < 0 ||
             (y = unit_stack_sum(unit,2), y != 1)))))))) {
      city5 = city_at(x2,y2);
      if (-1 < (int)city5) {
        city_turn.calc_cached_id = 0xffffffff;
        city_calc_unit_support(city5);
        cities[city5].shield_box =
             (short)(((char)unit_types[units[unit].type].cost * city_calc.shield_rows) / 2)
             + cities[city5].shield_box;
      }
      unit_destroy_redraw(unit,1);
      return AI_ASSESS_CONTINUE;
    }
    for (unit_type = 0; (int)unit_type < 0x3e; unit_type++) {
      if ((unit_types[units[unit].type].obsolete_by ==
           unit_types[unit_type].prerequisite) &&
         (unit_types[units[unit].type].role == unit_types[unit_type].role)) {
        units[unit].type = (u8)unit_type;
        units[unit].flags = units[unit].flags & ~UNITF_VETERAN;
        break;
      }
    }
  }
  return AI_ASSESS_NEXT;
}

/* Part of ai_civ_turn_assess (0053184d): the policy for one landmass: 0
   attack (enemies there it can beat, or barbarian cities), 1 defend (strong
   enemies or a threat), else 5 expand or, crowded (by units, or by cities for
   its fertile land and the leader's expansion; never before advance 0x2e), 4;
   where it has no units or cities, unless only units of civs at peace are
   there, 0; a new policy resets the production of its cities there. */
static void ai_assess_continent_policy(AiCivAssess *o)
{
  int density;
  int base_density;
  uint crowded;
  uptr &civ = o->civ; int &city6 = o->city6;
  int &attack_count = o->attack_count; iptr &continent = o->continent;
  uint &old_policy = o->old_policy; int &rival_military = o->rival_military;
  int &threat_count = o->threat_count; int &defend_count = o->defend_count;
  int &rival_cities = o->rival_cities;
  if (civs[0].continent_cities[continent] != '\0') {
    attack_count++;
  }
  base_density = 6;
  if (((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) &&
      (4 < civs[game.civ_by_rank[7]].num_cities)) && (0x96 < game.turn)) {
    base_density = 7;
  }
  if ((game.game_flags & GAME_LARGE_MAP) != 0) {
    base_density++;
  }
  city6 = ((uint)civs[civ].continent_military[continent] + rival_military) * 2;
  crowded = (uint)(city6 - landmass_table_block.landmass_recs[continent].tiles != 0 &&
                    landmass_table_block.landmass_recs[continent].tiles <= city6);
  if (*(char *)&civs[civ].continent_flags[continent] == '\0') {
    density = leaders[civs[civ].tribe].expand + base_density + 1;
  }
  else {
    density = base_density;
  }
  if ((int)landmass_table_block.landmass_recs[continent].fertile_tiles <
      (int)(((uint)civs[civ].continent_cities[continent] + rival_cities) * density + 2))
  {
    crowded = 1;
  }
  city6 = civ_has_advance(civ,ADV_MAP_MAKING);
  if (city6 == 0) {
    crowded = 0;
  }
  if (crowded == 0) {
    *(i8 *)&civs[civ].continent_policy[continent] = 5;
  }
  else {
    *(i8 *)&civs[civ].continent_policy[continent] = 4;
  }
  if (attack_count != 0) {
    *(i8 *)&civs[civ].continent_policy[continent] = 0;
  }
  if ((defend_count != 0) || (threat_count != 0)) {
    *(i8 *)&civs[civ].continent_policy[continent] = 1;
  }
  if (defend_count != 0) {
    civs[civ].continent_flags[continent] =
         civs[civ].continent_flags[continent] | 0x10;
  }
  if (((*(short *)&civs[civ].continent_military[continent] == 0) &&
      (*(char *)&civs[civ].continent_cities[continent] == '\0')) &&
     ((civs[civ].continent_flags[continent] & 0xc) != 8)) {
    *(i8 *)&civs[civ].continent_policy[continent] = 0;
  }
  if ((civs[civ].continent_policy[continent] != old_policy) &&
     ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
    civ_reset_city_production(civ,continent);
  }
}

/* Part of ai_civ_turn_assess (0053184d): the civs with units on one landmass
   (with real strength there or cities): enemies it can beat count in
   attack_count, stronger ones in defend_count, human civs with a ceasefire or
   peace whose attack there beats its forces in threat_count; their units and
   cities summed (an ally's twice); flags for the cities (1, at peace 2) and
   units (4, at peace 8) of civs not allied. */
static void ai_assess_continent_rivals(AiCivAssess *o)
{
  uptr &civ = o->civ; int &city6 = o->city6;
  int &attack_count = o->attack_count; iptr &continent = o->continent;
  int &rival_military = o->rival_military; int &threat_count = o->threat_count;
  uptr &civ2 = o->civ2; int &defend_count = o->defend_count;
  int &rival_cities = o->rival_cities;
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    if ((*(short *)&civs[civ2].continent_military[continent] != 0) &&
       (((uint)((int)(uint)civs[civ].continent_attack[continent] >> 2) <
         (uint)civs[civ2].continent_attack[continent] ||
        (*(char *)&civs[civ2].continent_cities[continent] != '\0')))) {
      if (civ != civ2) {
        city6 = diplo_is_hostile(civ,civ2);
        if ((city6 != 0) || ((*(byte *)&civs[civ].treaties[civ2] & TREATY_HOSTILE_INTENT) != 0))
        {
          if ((civs[civ2].continent_military[continent] <
               civs[civ].continent_attack[continent]) ||
             ((civs[civ2].continent_attack[continent] <
               civs[civ].continent_military[continent] ||
              (*(char *)&civs[civ].continent_cities[continent] == '\0')))) {
            attack_count++;
          }
          else {
            defend_count++;
          }
        }
        if ((((*(short *)&civs[civ2].continent_military[continent] != 0) &&
             ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
            ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0)) &&
           (((int)(uint)civs[civ].continent_attack[continent] >> 1) +
            (uint)civs[civ].continent_military[continent] <
            (uint)civs[civ2].continent_attack[continent])) {
          threat_count++;
        }
      }
      rival_military += (uint)civs[civ2].continent_military[continent];
      rival_cities += (uint)civs[civ2].continent_cities[continent];
      if (civ != civ2) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          if (*(char *)&civs[civ2].continent_cities[continent] != '\0') {
            if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
              civs[civ].continent_flags[continent] =
                   civs[civ].continent_flags[continent] | 1;
            }
            else {
              civs[civ].continent_flags[continent] =
                   civs[civ].continent_flags[continent] | 2;
            }
          }
          if (*(short *)&civs[civ2].continent_military[continent] != 0) {
            if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) {
              civs[civ].continent_flags[continent] =
                   civs[civ].continent_flags[continent] | 4;
            }
            else {
              civs[civ].continent_flags[continent] =
                   civs[civ].continent_flags[continent] | 8;
            }
          }
        }
        else {
          rival_military += (uint)civs[civ2].continent_military[continent];
          rival_cities += (uint)civs[civ2].continent_cities[continent];
        }
      }
    }
  }
}

/* Part of ai_civ_turn_assess (0053184d): the cities: its own counted (number,
   size, what they build, per landmass), an undefended one a defense goal; a
   foreign one it knows of (or any, late enough or of an AI civ) gets an
   attack goal two turns in three if hostile (or with treaty flag 0x20 and no
   alliance), and defense and attack goals if undefended and without a treaty.
   */
static void ai_assess_count_cities(AiCivAssess *o)
{
  uptr &civ = o->civ; char *&counter = o->counter; int &city6 = o->city6;
  iptr &city = o->city; iptr &continent = o->continent; int &y2 = o->y2;
  int &x2 = o->x2; int &land_city_count = o->land_city_count;
  land_city_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    if (cities[city].id != 0) {
      x2 = (int)cities[city].x;
      y2 = (int)cities[city].y;
      city6 = map_in_bounds(x2,y2);
      if (city6 != 0) {
        if ((int)cities[city].owner == civ) {
          civs[civ].num_cities++;
          civs[civ].total_city_size =
               (short)cities[city].size + civs[civ].total_city_size;
          if (-1 < cities[city].building) {
            counter = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
            *counter = *counter + 1;
          }
          continent = tile_land_landmass(x2,y2);
          if (-1 < continent) {
            *(char *)&civs[civ].continent_city_size[continent] =
                 *(char *)&civs[civ].continent_city_size[continent] + cities[city].size;
            counter = (char *)(continent + (iptr)((u8 *)&civs + 658) + civ * 0x594);
            *counter = *counter + 1;
            if ((0 < continent) && (continent < 0x3f)) {
              land_city_count++;
            }
          }
          city6 = tile_unit_owner(x2,y2);
          if (city6 < 0) {
            civ_add_ai_goal(civ,x2,y2,ROLE_DEFEND,5);
          }
        }
        else if ((((200 < (int)((uint)game.difficulty * (int)game.turn)) ||
                  ((1 << ((byte)civ & 0x1f) & (int)(char)cities[city].known_to) != 0)) ||
                 ((int)cities[city].owner == (civ & 0xff))) ||
                (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) == 0)) {
          city6 = diplo_is_hostile(civ,(int)cities[city].owner);
          if (((city6 != 0) ||
              (((*(byte *)&civs[civ].treaties[cities[city].owner] & TREATY_HOSTILE_INTENT) != 0 &&
               ((*(byte *)&civs[civ].treaties[cities[city].owner] & TREATY_ALLIANCE) == 0))))
             && ((game.turn + city) % 3 != 0)) {
            city6 = city_has_improvement(city,IMP_CITY_WALLS);
            civ_add_ai_goal(civ,x2,y2,ROLE_ATTACK,(-(uint)(city6 == 0) & 2) + 2);
          }
          city6 = tile_unit_owner(x2,y2);
          if ((city6 < 0) &&
             ((*(byte *)&civs[civ].treaties[cities[city].owner] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) {
            civ_add_ai_goal(civ,x2,y2,ROLE_DEFEND,2);
            civ_add_ai_goal(civ,x2,y2,ROLE_ATTACK,3);
          }
        }
      }
    }
  }
}

/* Part of ai_civ_turn_assess (0053184d): the units: its own counted (per
   type, military per landmass, ships, attack and strength; trade units per
   landmass, marking their home city), an AI unit's stale orders cleared (or
   sentried next to enemies, put to sleep at sea), its settlers weighed
   against the settle goals of their landmass (flag 0x200 for the ones
   needed); with hit points rules, a badly hurt foreign unit without a treaty
   that it sees is a goal. */
static void ai_assess_count_units(AiCivAssess *o)
{
  short *mil_counter;
  byte city2;
  int settler_divisor;
  int settler_quota;
  uptr &civ = o->civ; char *&counter = o->counter; int &city6 = o->city6;
  int &y = o->y; uptr &unit = o->unit; iptr &continent = o->continent;
  uptr &unit_type = o->unit_type; int &y2 = o->y2; int &x2 = o->x2;
  int (&settler_count)[64] = o->settler_count;
  int &settlers_total = o->settlers_total;
  int (&settle_goals)[64] = o->settle_goals;
  for (unit = 0; (int)unit < (int)game.num_units; unit++) {
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    if (units[unit].id != 0) {
      if ((int)units[unit].owner == civ) {
        unit_type = (uptr)units[unit].type;
        counter = (char *)(unit_type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *counter = *counter + 1;
        if ((char)unit_types[unit_type].attack < 'c') {
          if ((units[unit].orders == ORDER_FORTIFYING) || (units[unit].orders == ORDER_FORTIFIED)) {
            units[unit].flags = units[unit].flags | UNITF_AI_GARRISON;
          }
          x2 = (int)units[unit].x;
          y2 = (int)units[unit].y;
          city6 = map_in_bounds(x2,y2);
          if (city6 == 0) {
            continent = -1;
            unit_set_sleep(unit);
          }
          else {
            continent = tile_land_landmass(x2,y2);
            if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
              if ((units[unit].orders == ORDER_SLEEP) &&
                 ((unit_types[unit_type].domain != DOMAIN_LAND ||
                  (city6 = tile_is_ocean(x2,y2), city6 == 0)))) {
                units[unit].orders = ORDER_NONE;
              }
              if (15 < units[unit].orders) {
                units[unit].orders = ORDER_NONE;
              }
              if ((units[unit].orders == ORDER_NONE) &&
                 (city6 = tile_zoc_unit_adjacent(x2,y2,civ), city6 != 0)) {
                units[unit].orders = ORDER_AI_IDLE;
              }
            }
            city6 = tile_is_ocean(x2,y2);
            if ((city6 != 0) && (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
              unit_set_sleep(unit);
            }
          }
          civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
          if (((char)unit_types[unit_type].role < 5) &&
             (civs[civ].num_military_units = civs[civ].num_military_units + 1,
             -1 < continent)) {
            mil_counter = (short *)(continent * 2 + (iptr)((u8 *)&civs + 402) + civ * 0x594);
            *mil_counter = *mil_counter + 1;
          }
          if (((unit_types[units[unit].type].role == ROLE_TRADE) &&
              (units[unit].home_city != -1)) &&
             (cities[(byte)units[unit].home_city].flags =
                   cities[(byte)units[unit].home_city].flags | CITYF_HAD_CARAVAN, -1 < continent)) {
            counter = (char *)(continent + (iptr)((u8 *)&civs + 786) + civ * 0x594);
            *counter = *counter + 1;
          }
          if ((unit_types[unit_type].role == ROLE_SETTLE) && (units[unit].home_city != -1)) {
            settlers_total++;
            if (continent < 0) continue;
            settler_count[continent]++;
            if (*(char *)&civs[civ].continent_cities[continent] != '\0') {
              if (civs[civ].government < GOV_MONARCHY) {
                settler_divisor = 4;
              }
              else if (civs[civ].government < GOV_REPUBLIC) {
                settler_divisor = 3;
              }
              else {
                settler_divisor = 2;
              }
              settler_quota = (int)(((settler_divisor >> 1) - (int)leaders[civs[civ].tribe].expand) - 1 +
                             (uint)civs[civ].continent_cities[continent]) / settler_divisor;
              if (unit_type == UNIT_ENGINEERS) {
                settler_quota++;
              }
              city6 = settle_goals[continent];
              if (settler_quota <= settle_goals[continent]) {
                city6 = settler_quota;
              }
              settler_quota = city6;
              if (settler_count[continent] <= city6) {
                units[unit].flags = units[unit].flags | UNITF_AI_COLONIST;
              }
            }
            if (units[unit].home_city != -1) {
              city2 = units[unit].home_city;
              if ((((cities[city2].flags & CITYF_AI_SETTLER_NEAR) != 0) && ((units[unit].flags & UNITF_AI_COLONIST) != 0))
                 && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
                settler_count[continent]--;
                units[unit].flags = units[unit].flags & ~UNITF_AI_COLONIST;
              }
              if (((units[unit].flags & UNITF_AI_COLONIST) != 0) &&
                 (city6 = map_distance_diag(x2,y2,(int)cities[city2].x,
                                             (int)cities[city2].y), city6 < 5)) {
                cities[city2].flags = cities[city2].flags | CITYF_AI_SETTLER_NEAR;
              }
            }
          }
          if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
            civs[civ].num_ships++;
          }
          civs[civ].total_attack =
               (short)(char)unit_types[unit_type].attack + civs[civ].total_attack;
          civs[civ].military_strength =
               (short)(char)unit_types[unit_type].attack +
               (short)(char)unit_types[unit_type].defense + civs[civ].military_strength;
          if (-1 < continent) {
            *(short *)&civs[civ].continent_attack[continent] =
                 *(short *)&civs[civ].continent_attack[continent] +
                 (short)(char)unit_types[unit_type].attack;
          }
        }
      }
      else if ((((game.rules & RULE_NORMAL_COMBAT) != 0) &&
               ((*(byte *)&civs[civ].treaties[units[unit].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) &&
              (((1 << ((byte)civ & 0x1f) & (uint)units[unit].visibility) != 0 ||
               ((int)units[unit].owner == (civ & 0xff))))) {
        city6 = unit_max_hp(unit);
        y = unit_hp_remaining(unit);
        if (y <= city6 / 2) {
          y = 1;
          city6 = tile_is_ocean((int)units[unit].x,(int)units[unit].y);
          civ_add_ai_goal(civ,(int)units[unit].x,(int)units[unit].y,
                             (city6 == 0) - 1 & 2,y);
        }
      }
    }
  }
}

/* Part of ai_civ_turn_assess (0053184d): its settlers (role 5): flags reset;
   for a human civ all marked 0x200; homeless ones on land counted per
   landmass and marked, a city of its own near them flagged 0x40. */
static void ai_assess_count_settlers(AiCivAssess *o)
{
  uptr &civ = o->civ; int &city6 = o->city6; uptr &unit = o->unit;
  int (&settler_count)[64] = o->settler_count;
  int &settlers_total = o->settlers_total;
  for (unit = 0; (int)unit < (int)game.num_units; unit++) {
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    if (((units[unit].id != 0) && ((int)units[unit].owner == civ)) &&
       (units[unit].flags = units[unit].flags & ~(UNITF_GOAL_REACHED | UNITF_AI_GARRISON | UNITF_AI_COLONIST),
       unit_types[units[unit].type].role == ROLE_SETTLE)) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        units[unit].flags = units[unit].flags | UNITF_AI_COLONIST;
      }
      if (units[unit].home_city == -1) {
        settlers_total++;
        city6 = tile_is_ocean((int)units[unit].x,(int)units[unit].y);
        if (city6 == 0) {
          units[unit].flags = units[unit].flags | UNITF_AI_COLONIST;
          city6 = tile_landmass((int)units[unit].x,(int)units[unit].y);
          settler_count[city6]++;
          city6 = city_find_nearest((int)units[unit].x,(int)units[unit].y,-1,0xffffffff,
                                     -1);
          if ((advisor_window_block.nearest_city_dist < 4) && ((int)cities[city6].owner == civ)) {
            cities[city6].flags = cities[city6].flags | CITYF_AI_SETTLER_NEAR;
          }
        }
      }
    }
  }
}

/* Part of ai_civ_turn_assess (0053184d): an AI civ with paratroopers in play:
   for each undefended city of a civ it is at war with, a city of its own
   within drop range of it and without paratroopers gets one (with moves left)
   moved in from another of its cities out of range. */
static void ai_assess_paratroopers(AiCivAssess *o)
{
  iptr occupant;
  int dist;
  int x3;
  int done;
  int y3;
  iptr city4;
  iptr city3;
  uptr &civ = o->civ; iptr &has_para = o->has_para; int &city6 = o->city6;
  int &y = o->y; uptr &unit = o->unit; iptr &city = o->city; int &y2 = o->y2;
  int &x2 = o->x2;
  if (((has_para != 0) && (civs[civ].units_active[UNIT_PARATROOPERS] != '\0')) &&
     ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && ((int)cities[city].owner != civ)) &&
         (((byte)(civs[civ].treaties[cities[city].owner] >> 8) & (TREATY_WAR >> 8)) != 0)) {
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        occupant = tile_unit_owner(x2,y2);
        if (occupant < 0) {
          for (city3 = 0; city3 < game.num_cities; city3++) {
            if ((cities[city3].id != 0) && ((int)cities[city3].owner == civ)) {
              city6 = (int)cities[city3].x;
              y3 = (int)cities[city3].y;
              y = map_distance(x2,y2,city6,y3);
              if (y <= (int)(uint)cosmic.paradrop_range) {
                done = 0;
                for (unit = tile_first_unit(city6,y3); -1 < (int)unit;
                    unit = unit_next_in_stack(unit)) {
                  if ((unit_types[units[unit].type].flags & UTYPE_PARADROP) != 0) {
                    done = 1;
                    break;
                  }
                }
                if (done != 0) break;
                for (city4 = 0; city4 < game.num_cities; city4++) {
                  if (2 < game.multiplayer) {
                    net.process_messages(1,0);
                  }
                  if ((cities[city4].id != 0) && ((int)cities[city4].owner == civ)) {
                    x3 = (int)cities[city4].x;
                    y = (int)cities[city4].y;
                    dist = map_distance(x2,y2,x3,y);
                    if (((int)(uint)cosmic.paradrop_range < dist) &&
                       (dist = map_distance(city6,y3,x3,y),
                       dist <= (int)(uint)cosmic.paradrop_range)) {
                      for (unit = tile_first_unit(x3,y); -1 < (int)unit;
                          unit = unit_next_in_stack(unit)) {
                        if (((unit_types[units[unit].type].flags & UTYPE_PARADROP) != 0) &&
                           (y = unit_moves_left(unit), y != 0)) {
                          unit_relocate(unit,city6,y3,1);
                          units[unit].visibility = '\0';
                          unit_exhaust_moves(unit);
                          done = 1;
                          break;
                        }
                      }
                      if (done != 0) break;
                    }
                  }
                }
                if (done != 0) break;
              }
            }
          }
        }
      }
    }
  }
}

/* Part of ai_civ_turn_assess (0053184d): its wonders counted per group, its
   counts per landmass and unit type and its totals cleared, and its goals of
   role 21 counted per landmass (settle_goals). */
static void ai_assess_reset_counts(AiCivAssess *o)
{
  int wonder;
  iptr group;
  uptr &civ = o->civ; char *&counter = o->counter; int &city6 = o->city6;
  iptr &continent = o->continent; uptr &unit_type = o->unit_type;
  int (&settler_count)[64] = o->settler_count;
  int (&settle_goals)[64] = o->settle_goals; iptr &goal = o->goal;
  for (group = 0; group < 4; group++) {
    *(i8 *)&civs[civ].wonders_by_group[group] = 0;
  }
  for (wonder = 0; wonder < 0x1c; wonder++) {
    city6 = civ_has_wonder(civ,wonder);
    if (city6 != 0) {
      group = wonder / 7;
      counter = (char *)(group + (iptr)((u8 *)&civs + 23) + civ * 0x594);
      *counter = *counter + 1;
    }
  }
  for (continent = 0; continent < 0x40; continent++) {
    *(i16 *)&civs[civ].continent_military[continent] = 0;
    *(i16 *)&civs[civ].continent_attack[continent] = 0;
    *(i8 *)&civs[civ].continent_city_size[continent] = 0;
    *(i8 *)&civs[civ].continent_trade_units[continent] = 0;
    *(i8 *)&civs[civ].continent_flags[continent] = 0;
    settler_count[continent] = 0;
    settle_goals[continent] = 0;
  }
  civ_unit_counts_block.civ_unit_totals[civ] = 0;
  civs[civ].num_military_units = 0;
  civs[civ].num_cities = 0;
  civs[civ].num_ships = 0;
  civs[civ].total_city_size = 0;
  civs[civ].military_strength = 0;
  civs[civ].total_attack = 0;
  for (unit_type = 0; (int)unit_type < 0x3e; unit_type++) {
    *(i8 *)&civs[civ].units_active[unit_type] = 0;
    *(i8 *)&civs[civ].units_in_production[unit_type] = 0;
  }
  for (goal = 0; goal < 0x30; goal++) {
    if ((civs[civ].ai_goals[goal].role == 21) &&
       ('\0' < civs[civ].ai_goals[goal].priority)) {
      continent = tile_land_landmass((int)civs[civ].ai_goals[goal].x,
                                     (int)civs[civ].ai_goals[goal].y);
      if (-1 < continent) {
        settle_goals[continent]++;
      }
    }
  }
}

/* 0053184d auto */
void __cdecl ai_civ_turn_assess(uptr civ)

{
  char *counter;
  iptr has_para;
  int city6;
  int y;
  uptr owner_civ;
  i32 imps;
  uptr unit;
  iptr city;
  int attack_count;
  int goal_weight[48];
  uint is_human;
  iptr continent;
  uptr unit_type;
  uint old_policy;
  int y2;
  int scenario_mode;
  int x2;
  int settler_count[64];
  int rival_military;
  int threat_count;
  int settlers_total;
  int settle_goals[64];
  uptr civ2;
  int defend_count;
  iptr goal_idx;
  int rival_cities;
  iptr goal;
  int land_city_count;
  AiCivAssess st = {civ, counter, has_para, city6, y, owner_civ, imps, unit,
    city, attack_count, goal_weight, is_human, continent, unit_type,
    old_policy, y2, scenario_mode, x2, settler_count, rival_military,
    threat_count, settlers_total, settle_goals, civ2, defend_count,
    rival_cities, goal, land_city_count};
  
  settlers_total = 0;
  scenario_mode = 0;
  has_para = civ_has_advance(civ,(int)unit_types[UNIT_PARATROOPERS].prerequisite);
  ai_assess_paratroopers(&st);
  ai_assess_reset_counts(&st);
  ai_assess_count_settlers(&st);
  ai_assess_count_units(&st);
  for (continent = 0; continent < 0x40; continent++) {
    *(i8 *)&civs[civ].continent_cities[continent] = 0;
  }
  ai_assess_count_cities(&st);
  civs[civ].continent_policy[0] = 5;
  civs[civ].continent_policy[0x3f] = 5;
  civs[civ].flags = civs[civ].flags & ~CIVF_AT_WAR;
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    if ((civ != civ2) &&
       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)) {
      civs[civ].flags = civs[civ].flags | CIVF_AT_WAR;
    }
  }
  for (continent = 1; continent < 0x3f; continent++) {
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    threat_count = 0;
    defend_count = 0;
    attack_count = 0;
    rival_military = 0;
    rival_cities = 0;
    old_policy = (uint)civs[civ].continent_policy[continent];
    ai_assess_continent_rivals(&st);
    ai_assess_continent_policy(&st);
  }
  if ((civs[civ].continent_cities[0x3f] != '\0') &&
     (land_city_count <= (int)(uint)civs[civ].continent_cities[0x3f])) {
    civs[civ].continent_policy[0x3f] = 4;
  }
  if (civ != 0) {
    is_human = (uint)game.human_players & 1 << ((byte)civ & 0x1f);
    if (is_human == 0) {
      if ((settlers_total == 0) && (civs[civ].num_cities == 0)) {
        civ_check_destroyed(civ,0);
      }
      if (((int)game.turn + civ & 7) == 0) {
        ai_choose_government(civ,0);
      }
    }
    city6 = int_clamp((int)civs[civ].num_military_units >> 3,3,99);
    for (goal_idx = 0; goal_idx < 0x30; goal_idx++) {
      goal_weight[goal_idx] = city6;
      if (civs[civ].ai_goals[goal_idx].role == ROLE_ATTACK) {
        goal_weight[goal_idx] = goal_weight[goal_idx] << 1;
      }
    }
    scenario_mode = 0;
    if (((((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) && (civ == 3)
        ) && (is_human == 0)) {
      scenario_mode = 1;
    }
    for (unit = (uptr)game.num_units - 1; -1 < (int)unit; unit--) {
      if (2 < game.multiplayer) {
        net.process_messages(1,0);
      }
      if (((units[unit].id != 0) && ((int)units[unit].owner == civ)) &&
         ((is_human == 0 || ((units[unit].flags & UNITF_AUTOMATED) != 0)))) {
        if ((char)unit_types[units[unit].type].attack < 'c') {
          if ((units[unit].orders != ORDER_AI_IDLE) &&
             ((((15 < units[unit].orders || (units[unit].orders == ORDER_GOTO)) ||
               (units[unit].ai_task = '?', units[unit].orders < 4)) &&
              (units[unit].orders != ORDER_SLEEP)))) {
            x2 = (int)units[unit].x;
            y2 = (int)units[unit].y;
            y = map_in_bounds(x2,y2);
            if ((y != 0) &&
               (continent = tile_landmass(x2,y2), units[unit].ai_task != '7')) {
              if (ai_assess_scenario_unit(&st) == AI_ASSESS_CONTINUE) continue;
              if (ai_assess_unit_upgrade(&st) == AI_ASSESS_CONTINUE) continue;
              if (ai_assess_unit_goal(&st) == AI_ASSESS_CONTINUE) continue;
            }
          }
        }
        else {
          units[unit].ai_task = '!';
        }
      }
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && ((int)cities[city].owner == civ)) {
      if ((cities[city].flags & CITYF_AI_FERRY_PORT_FRESH) == 0) {
        cities[city].flags = cities[city].flags & ~CITYF_AI_FERRY_PORT;
      }
      cities[city].flags = cities[city].flags & ~CITYF_AI_FERRY_PORT_FRESH;
    }
  }
  civ_refresh_ai_goals(civ);
  return;
}

/* Where ai_barbarian_unit_move goes on after one of its parts: NEXT is the
   line after the call, the others its labels or its return value. */
enum AiBarbNext {
  AI_BARB_NEXT,
  AI_BARB_FINALIZE_DIR,
  AI_BARB_CLEAR_GOTO_DONE,
  AI_BARB_RETURN_1
};

/* The locals of ai_barbarian_unit_move that its parts share: references to the
   entry point's own. */
struct AiBarbMove {
  Tile *&map_tile;
  iptr &unit;
  iptr &civ2;
  int &x2;
  iptr &y;
  int &y3;
  iptr &x;
  int &y2;
  int &check;
  iptr &civ;
  int &value;
  i32 &misc;
  uptr &unit_type;
  int &dy_toward;
  uint &x3;
  iptr &city;
  iptr &best_dir;
  iptr &dir;
  int &best_score;
};

/* Part of ai_barbarian_unit_move (005351aa): a barbarian next to an
   undefended city: the ransom (half its owner's gold, at least 50 and 25 per
   size of the nearest city; for the top civ late in the game with over 4
   cities, its gold in whole hundreds, or all of it plus one when under 1000);
   a human civ that can pay is asked, and if it pays the barbarians around the
   city are removed. */
static int ai_barb_demand_ransom(AiBarbMove *o)
{
  short y5;
  short x4;
  int ransom;
  iptr line;
  byte civ_byte;
  iptr &unit = o->unit; int &x2 = o->x2; int &y3 = o->y3; iptr &x = o->x;
  int &y2 = o->y2; iptr &civ = o->civ; int &value = o->value;
  iptr &city = o->city;
  value = civs[civ].gold / 2;
  if (value < 0x33) {
    value = 0x32;
  }
  ransom = cities[city].size * 0x19;
  if (ransom <= value) {
    ransom = value;
  }
  if ((((game.power_rank[civ] == 7) && (200 < game.year)) &&
      (4 < civs[civ].num_cities)) &&
     (ransom = (civs[civ].gold / 100) * 100, ransom < 1000)) {
    ransom = civs[civ].gold + 1;
  }
  text_set_number_arg(0,ransom);
  if (((game.multiplayer < 3) &&
      (civ_byte = (byte)civ,
      (1 << (civ_byte & 0x1f) & (uint)game.human_players) != 0)) &&
     (ransom <= civs[civ].gold)) {
    city = city_at(x,y2);
    text_set_string_arg(1,(const char *)&cities[city].name);
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    value = text_open_section((const char *)&str_city_6,(const char *)&str_barbarians_3);
    if (value != 0) {
      __chdir(((i8 *)&game_dirs.home));
      value = text_open_section((const char *)&str_city_7,(const char *)&str_barbarians_4);
      if (value != 0) return AI_BARB_CLEAR_GOTO_DONE;
    }
    __chdir(((i8 *)&game_dirs.home));
    text_read_line(1);
    value = _atoi(((i8 *)&textbuf.buf));
    if (0 < value) {
      y5 = cities[city].y;
      x4 = cities[city].x;
      for (line = 0; line <= (y5 * 3 + x4 * 5) % value;
          line = line + 1) {
        text_read_line(1);
      }
      text_set_string_arg(0,(const char *)&textbuf.buf);
      mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,civ);
      if ((autoplay.active == 0) &&
         (value = dlg_game_txt_with_unit((const char *)&str_ransomcity,unit), value != 0)) {
        civs[civ].gold -= ransom;
        status_draw_info(1);
        for (line = 0; line < 0x14; line++) {
          x = map_wrap_x((int)cities[city].x +
                              (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[line]);
          x2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[line];
          y3 = map_in_bounds(x,x2);
          if (((y3 != 0) &&
              (y3 = tile_first_unit(x,x2), -1 < y3)) &&
             (units[y3].owner == '\0')) {
            unit_stack_delete_all(y3,1);
            map_redraw_tile_all_views(x,x2);
          }
        }
        return AI_BARB_RETURN_1;
      }
    }
  }
  return AI_BARB_NEXT;
}

/* Part of ai_barbarian_unit_move (005351aa): one of the eight move directions
   of a barbarian: a foreign city next to it is taken (or ransomed); otherwise
   scored by ownership, improvements, a little chance, units there (its own
   only from the sea, others to attack), its last direction and the way to its
   target city; a leader by moves and distance from cities. The best is kept
   in best_dir. */
static int ai_barb_score_dir(AiBarbMove *o)
{
  u8 move_pts;
  byte terrain;
  iptr unit2;
  uint tile_imps;
  int step_dy;
  int step_dx;
  int score;
  iptr &unit = o->unit; iptr &civ2 = o->civ2; int &x2 = o->x2; iptr &y = o->y;
  int &y3 = o->y3; iptr &x = o->x; int &y2 = o->y2; int &check = o->check;
  iptr &civ = o->civ; int &value = o->value; uptr &unit_type = o->unit_type;
  int &dy_toward = o->dy_toward; uint &x3 = o->x3;
  iptr &best_dir = o->best_dir; iptr &dir = o->dir;
  int &best_score = o->best_score;
  x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
  y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
  check = map_in_bounds(x,y2);
  if (check != 0) {
    terrain = tile_terrain(x,y2);
    if ((unit_types[unit_type].domain == DOMAIN_SEA) == (terrain == TERRAIN_OCEAN)) {
      check = tile_unit_owner(x,y2);
      unit2 = tile_first_unit(x,y2);
      score = 0;
      if ((char)unit_types[unit_type].role < 5) {
        civ = tile_city_owner(x,y2);
        if (!((civ < 0) || (civ2 == civ))) {
          value = tile_is_ocean(x2,y);
          if (value != 0) return AI_BARB_NEXT;
          value = tile_unit_owner(x,y2);
          if (value < 0) {
            switch (ai_barb_demand_ransom(o)) {
            case AI_BARB_CLEAR_GOTO_DONE: return AI_BARB_CLEAR_GOTO_DONE;
            case AI_BARB_RETURN_1: return AI_BARB_RETURN_1;
            default: break;
            }
          }
          if ((unit_type != UNIT_PARTISANS) || (value = tile_unit_owner(x,y2), value < 0)) {
            best_dir = dir;
            return AI_BARB_FINALIZE_DIR;
          }
          if ((game.turn + unit & 7U) == 0) return AI_BARB_NEXT;
          value = unit_stack_sum(unit,2);
          if ((value < 2) && (units[unit].orders = ORDER_FORTIFYING, best_score < 99)) {
            best_score = 99;
            best_dir = 8;
          }
        }
        value = tile_owner(x,y2);
        if (civ2 != value) {
          score = 4;
        }
        tile_imps = tile_improvements(x,y2);
        if ((tile_imps & TILE_FORTRESS) == 0) {
          tile_imps = tile_improvements(x,y2);
          if ((tile_imps & TILE_FARMLAND) != 0) {
            score += 6;
          }
        }
        else {
          score += 8;
        }
        value = rand_range(0,5);
        score += value;
        if (-1 < unit2) {
          if (check == civ2) {
            check = tile_is_ocean(x2,y);
            if (check == 0) return AI_BARB_NEXT;
            score -= 0x14;
          }
          else {
            check = tile_is_ocean(x2,y);
            if (check != 0) return AI_BARB_NEXT;
            if (unit_type == UNIT_PARTISANS) {
              if ((char)unit_types[units[unit2].type].role < 5) return AI_BARB_NEXT;
              score += 99;
            }
            else if (unit_types[units[unit2].type].role != ROLE_DIPLOMACY) {
              score += 99;
            }
          }
        }
        if ((((y3 == 0) || (unit_types[unit_type].role != ROLE_ATTACK)) || (-1 < unit2)) ||
           ((y2 = tile_in_enemy_zoc(x,y2,civ2), y2 == 0 ||
            ((unit_types[units[unit].type].flags & UTYPE_IGNORE_ZOC) != 0)))) {
          if ((char)units[unit].last_dir == dir) {
            score += 6;
          }
          if (x3 != -3) {
            if ((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] < 1) {
              if ((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] < '\0') {
                step_dx = -1;
              }
              else {
                step_dx = 0;
              }
            }
            else {
              step_dx = 1;
            }
            if (x3 == step_dx) {
              score += 2;
            }
          }
          if (dy_toward != -3) {
            if ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] < 1) {
              if ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] < '\0') {
                step_dy = -1;
              }
              else {
                step_dy = 0;
              }
            }
            else {
              step_dy = 1;
            }
            if (dy_toward == step_dy) {
              score += 2;
            }
          }
        }
        else {
          return AI_BARB_NEXT;
        }
      }
      else {
        if (-1 < unit2) {
          if (check == civ2) {
            score = 99;
          }
          else {
            score = -99;
          }
        }
        city_find_nearest(x,y2,-1,0xffffffff,-1);
        move_pts = terrain_types[terrain].move_cost;
        y2 = rand_range(0,3);
        score = score + (char)move_pts + y2 + advisor_window_block.nearest_city_dist * 4;
      }
      if (best_score <= score) {
        best_score = score;
        best_dir = dir;
      }
    }
  }
  return AI_BARB_NEXT;
}

/* Part of ai_barbarian_unit_move (005351aa): a barbarian leader (or other
   non-military unit): stays with a lone escort on land; at sea near a city,
   or on land far from cities, it is removed; with a defender near it follows
   that unit. */
static int ai_barb_leader(AiBarbMove *o)
{
  char here_terrain;
  iptr &unit = o->unit; iptr &civ2 = o->civ2; int &x2 = o->x2; iptr &y = o->y;
  int &y2 = o->y2; uptr &unit_type = o->unit_type;
  iptr &best_dir = o->best_dir;
  here_terrain = tile_terrain(x2,y);
  y2 = unit_stack_sum(unit,2);
  if (((1 < y2) && (here_terrain != TERRAIN_OCEAN)) &&
     (y2 = unit_stack_count_role(unit,(int)(char)unit_types[unit_type].role), y2 < 2)) {
    best_dir = 8;
    return AI_BARB_FINALIZE_DIR;
  }
  if (here_terrain == TERRAIN_OCEAN) {
    if (advisor_window_block.nearest_city_dist < 3) {
      unit_destroy_redraw(unit,1);
      return AI_BARB_RETURN_1;
    }
  }
  else {
    y2 = unit_find_nearest(x2,y,civ2,unit);
    if ((dialog_text_font_obj_block.unit_nearest_dist < 4) && ((char)unit_types[units[y2].type].role < 2)) {
      unit_set_goto(unit,0x58,(int)units[y2].x,(int)units[y2].y);
      return AI_BARB_CLEAR_GOTO_DONE;
    }
    if ((int)(((int)game.turn & 7U) + 4) <= advisor_window_block.nearest_city_dist) {
      unit_destroy_redraw(unit,1);
      return AI_BARB_RETURN_1;
    }
  }
  return AI_BARB_NEXT;
}

/* Part of ai_barbarian_unit_move (005351aa): the way to the nearest city when
   it is a human civ's on the same landmass (x3 and dy_toward the signs of the
   way, -3 for none), unless it is a barbarian city or its civ has a single
   city or under 100 gold; with a way, the unit's last direction is the first
   step of a goto there. */
static void ai_barb_head_for_city(AiBarbMove *o)
{
  uint abs_dx;
  iptr &unit = o->unit; iptr &civ2 = o->civ2; int &x2 = o->x2; iptr &y = o->y;
  int &y2 = o->y2; int &check = o->check; i32 &misc = o->misc;
  int &dy_toward = o->dy_toward; uint &x3 = o->x3; iptr &city = o->city;
  dy_toward = -3;
  x3 = -3;
  if (-1 < city) {
    y2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
    check = tile_landmass(x2,y);
    if ((y2 == check) &&
       (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) {
      x3 = cities[city].x - x2;
      if ((game.rules & RULE_FLAT_WORLD) == 0) {
        abs_dx = x3;
        if ((int)x3 < 1) {
          abs_dx = ~x3 + 1;
        }
        if ((int)map.width >> 1 <= (int)abs_dx) {
          x3 = -x3;
        }
      }
      if ((int)x3 < 1) {
        if ((int)x3 < 0) {
          x3 = -1;
        }
        else {
          x3 = 0;
        }
      }
      else {
        x3 = 1;
      }
      if (cities[city].y == -3 || cities[city].y + 3 < 0) {
        if (cities[city].y + 3 < 0) {
          dy_toward = -1;
        }
        else {
          dy_toward = 0;
        }
      }
      else {
        dy_toward = 1;
      }
    }
  }
  if (-1 < city) {
    if (cities[city].owner == civ2) {
      dy_toward = -3;
      x3 = -3;
    }
    if (civs[cities[city].owner].num_cities < 2) {
      dy_toward = -3;
      x3 = -3;
    }
    if (civs[cities[city].owner].gold < 100) {
      dy_toward = -3;
      x3 = -3;
    }
  }
  if (x3 != -3) {
    units[unit].goto_x = cities[city].x;
    units[unit].goto_y = cities[city].y;
    misc = unit_goto_next_dir(unit);
    units[unit].last_dir = (u8)misc;
  }
}

/* Part of ai_barbarian_unit_move (005351aa): a barbarian ship without a goto:
   toward the shore of the richest reachable city (treasury share by
   distance), on the sea it is on; with none it is removed. */
static int ai_barb_ship_target(AiBarbMove *o)
{
  iptr city2;
  int sea_body;
  iptr &unit = o->unit; int &x2 = o->x2; iptr &y = o->y; int &y3 = o->y3;
  int &y2 = o->y2; i32 &misc = o->misc; iptr &city = o->city;
  int &best_score = o->best_score;
  if (units[unit].orders != ORDER_GOTO) {
    best_score = 0;
    city2 = -1;
    y3 = tile_is_ocean(x2,y);
    if (y3 == 0) {
      sea_body = -1;
    }
    else {
      sea_body = tile_landmass(x2,y);
    }
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner != '\0')) {
        if (-1 < sea_body) {
          misc = tile_landmass((int)cities[city].x,(int)cities[city].y);
          y3 = landmass_borders_body(misc,sea_body);
          if (y3 == 0) continue;
        }
        y3 = city_treasury_share(city);
        y2 = map_distance_diag(x2,y,(int)cities[city].x,(int)cities[city].y);
        y3 = (y3 + 0x32) / (y2 + 1);
        if (best_score < y3) {
          city2 = city;
          best_score = y3;
        }
      }
    }
    if (city2 < 0) {
      unit_destroy_redraw(unit,1);
      return AI_BARB_RETURN_1;
    }
    ai_ship_goto_near_shore(unit,0x70,(int)cities[city2].x,(int)cities[city2].y);
  }
  return AI_BARB_NEXT;
}

/* Part of ai_barbarian_unit_move (005351aa): a barbarian ship next to land:
   lands its units on a free tile on the landmass of the nearest city (a small
   party next to a city just disbands), telling the human owner of that city
   who sees it; or attacks a unit at sea next to it. */
static int ai_barb_ship_land(AiBarbMove *o)
{
  iptr valid;
  int civ3;
  Tile *&map_tile = o->map_tile; iptr &unit = o->unit; iptr &civ2 = o->civ2;
  int &x2 = o->x2; iptr &y = o->y; int &y3 = o->y3; iptr &x = o->x;
  int &y2 = o->y2; int &check = o->check; iptr &city = o->city;
  iptr &best_dir = o->best_dir; iptr &dir = o->dir;
  for (dir = 0; dir < 8; dir++) {
    x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
    y3 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    valid = map_in_bounds(x,y3);
    if (valid != 0) {
      if (((-1 < city) && (advisor_window_block.nearest_city_dist < 9)) &&
         ((y2 = tile_is_ocean(x,y3), y2 == 0 &&
          (((y2 = tile_city_or_unit_owner(x,y3), y2 < 0 && (1 < y3)) &&
           (y3 < map.height - 2)))))) {
        y2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
        check = tile_landmass(x,y3);
        if (y2 == check) {
          y2 = unit_stack_sum(unit,2);
          if (((y2 < 3) && (y2 = unit_stack_count_role(unit,ROLE_ATTACK), y2 == 0)) &&
             ((y2 = unit_stack_count_role(unit,ROLE_DEFEND), y2 == 0 && (advisor_window_block.nearest_city_dist < 3)))) {
            unit_destroy_redraw(unit,1);
          }
          else {
            for (civ3 = 1; civ3 < 8; civ3++) {
              if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0) &&
                  ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0)) &&
                 ((y2 = tile_visible_to(x,y3,civ3), y2 != 0 &&
                  ((cities[city].owner == civ3 && (autoplay.active == 0)))))) {
                city = city_find_nearest(x,y3,-1,0xffffffff,-1);
                text_set_string_arg(0,(const char *)&cities[city].name);
                if (human_civ[0] == civ3) {
                  if (game.multiplayer == '\0') {
                    unit_stack_set_seen_by(unit,civ3);
                    mapviews_ensure_visible(x,y3,civ2);
                    map_redraw_tile_all_views(x,y3);
                  }
                  dlg_game_txt_with_unit((const char *)&str_barbariansland_2,unit);
                }
                else if (2 < game.multiplayer) {
                  net_send_text_args(0x4e,net.net_slots[net.net_civ_slots[civ3]].id,1,0,
                                     unit,0);
                }
              }
            }
            map_tile = tile_at(x,y3);
            units[unit].visibility = map_tile->visible | units[unit].visibility;
            units[unit].ai_task = 'U';
            unit_stack_wake_by_role(unit,0xffff,0);
            unit_exhaust_moves(unit);
          }
          return AI_BARB_CLEAR_GOTO_DONE;
        }
      }
      y2 = tile_is_ocean(x,y3);
      if ((y2 != 0) && (y3 = tile_unit_owner(x,y3), 0 < y3)) {
        best_dir = dir;
        return AI_BARB_FINALIZE_DIR;
      }
    }
  }
  return AI_BARB_NEXT;
}

/* 005351aa auto */
i32 __cdecl ai_barbarian_unit_move(void)

{
  Tile *map_tile;
  short y4;
  iptr unit;
  iptr civ2;
  int x2;
  iptr y;
  iptr in_bounds;
  int y3;
  iptr transport_count;
  iptr x;
  int y2;
  int check;
  iptr civ;
  int value;
  i32 misc;
  uptr unit_type;
  bool at_city;
  int dy_toward;
  uint x3;
  iptr city;
  iptr best_dir;
  iptr dir;
  int best_score;
  AiBarbMove st = {map_tile, unit, civ2, x2, y, y3, x, y2, check, civ, value,
    misc, unit_type, dy_toward, x3, city, best_dir, dir, best_score};
  
  best_dir = 8;
  unit = (iptr)game.active_unit;
  civ2 = (iptr)units[unit].owner;
  if (units[unit].orders != ORDER_GOTO) {
    units[unit].orders = ORDER_NONE;
  }
  x2 = (int)units[unit].x;
  y4 = units[unit].y;
  y = (iptr)y4;
  if (((y < 2) || (map.height - 2 <= y)) ||
     (in_bounds = map_in_bounds(x2,y), in_bounds == 0)) {
    unit_delete_with_cargo(unit,1);
    return 1;
  }
  y3 = tile_in_enemy_zoc(x2,y,civ2);
  unit_type = (uptr)units[unit].type;
  city = city_find_nearest(x2,y,-1,0xffffffff,-1);
  if (unit_types[unit_type].hold != '\0') {
    y3 = unit_stack_sum(unit,2);
    transport_count = unit_stack_count_role(unit,ROLE_DIPLOMACY);
    if (y3 - transport_count < 2) {
      unit_destroy_redraw(unit,1);
      return 1;
    }
    units[unit].counter++;
    if (30 < (char)units[unit].counter) {
      unit_destroy_redraw(unit,1);
      return 1;
    }
    switch (ai_barb_ship_land(&st)) {
    case AI_BARB_CLEAR_GOTO_DONE: goto clear_goto_done;
    case AI_BARB_FINALIZE_DIR: goto finalize_dir;
    default: break;
    }
    if (ai_barb_ship_target(&st) == AI_BARB_RETURN_1) return 1;
    goto clear_goto_done;
  }
  if (((advisor_window_block.nearest_city_dist == 0) ||
      (misc = tile_improvements(x2,y), ((byte)misc & 0x42) == 0x40)) &&
     (y2 = unit_stack_sum(unit,2), check = unit_stack_count_role(unit,ROLE_DIPLOMACY),
      !((((1 < y2 - check) || (y3 != 0)) && (unit_types[unit_type].role != ROLE_DEFEND)) ||
        ((at_city = advisor_window_block.nearest_city_dist == 0, y2 = unit_stack_sum(unit,2),
         (int)(at_city + 2 + (uint)(y3 == 0)) <= y2 &&
         ((char)unit_types[unit_type].defense <= (char)unit_types[unit_type].attack)))))) {
    units[unit].orders = ORDER_FORTIFYING;
    best_dir = 8;
  }
  else {
    if (((-1 < city) && ((game.turn + unit & 3U) == 0)) &&
       ((y2 = tile_is_ocean(x2,y), y2 == 0 || (unit_types[unit_type].hold != '\0')))) {
      y2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
      check = tile_landmass(x2,y);
      if ((y2 != check) || (8 < advisor_window_block.nearest_city_dist)) {
        unit_destroy_redraw(unit,1);
        return 1;
      }
    }
    ai_barb_head_for_city(&st);
    if (4 < (char)unit_types[unit_type].role) {
      switch (ai_barb_leader(&st)) {
      case AI_BARB_CLEAR_GOTO_DONE: goto clear_goto_done;
      case AI_BARB_FINALIZE_DIR: goto finalize_dir;
      case AI_BARB_RETURN_1: return 1;
      default: break;
      }
    }
    best_score = -999;
    for (dir = 0; dir < 8; dir++) {
      switch (ai_barb_score_dir(&st)) {
      case AI_BARB_CLEAR_GOTO_DONE: goto clear_goto_done;
      case AI_BARB_FINALIZE_DIR: goto finalize_dir;
      case AI_BARB_RETURN_1: return 1;
      default: break;
      }
    }
    y3 = tile_city_owner(x2,y);
    if (((y3 < 0) || (y3 = unit_stack_sum(unit,2), 1 < y3)) || (0x62 < best_score)) {
      if ((((best_score < 99) &&
           (y3 = unit_moves_left(unit), y3 <= (int)(uint)cosmic.road_move_mult)) &&
          (-1 < city)) &&
         (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) {
        map_tile = tile_at(x2,y);
        if (((map_tile->improvements & (TILE_IRRIGATION | TILE_MINE | TILE_FORTRESS)) != 0) ||
           ((unit_type == UNIT_PARTISANS && ((map_tile->improvements & TILE_ROAD) != 0)))) {
          unit_pillage(unit,0);
          unit_exhaust_moves(unit);
          goto clear_goto_done;
        }
      }
    }
    else {
      best_dir = 8;
    }
  }
finalize_dir:
  if (best_dir == 8) {
    units[unit].last_dir = 0xff;
    unit_exhaust_moves(unit);
    if (((unit_types[unit_type].role == ROLE_DEFEND) ||
        (misc = tile_improvements(x2,y), ((byte)misc & 0x42) == 0x40)) &&
       (units[unit].orders = ORDER_FORTIFYING, (units[unit].flags & UNITF_AI_GARRISON) != 0)) {
      units[unit].orders = ORDER_FORTIFIED;
    }
  }
  else {
    units[unit].orders = ORDER_AI_GOTO;
    misc = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[best_dir] + x2);
    units[unit].goto_x = (i16)misc;
    units[unit].goto_y = (((i8 *)&dir_dy_table_block.dir_dy_table))[best_dir] + y4;
  }
clear_goto_done:
  if (((units[unit].orders == ORDER_GOTO) && (units[unit].goto_x == x2)) &&
     (units[unit].goto_y == y)) {
    units[unit].orders = ORDER_NONE;
  }
  return 0;
}

/* 005369f3 auto */
void __cdecl ai_transports_raid_empty_city(iptr city)

{
  char civ2;
  short x;
  short y;
  int civ;
  int max_moves;
  iptr unit;
  
  x = cities[city].x;
  y = cities[city].y;
  civ2 = cities[city].owner;
  for (unit = 0; unit < game.num_units; unit++) {
    if (units[unit].id != 0) {
      civ = (int)units[unit].owner;
      if ((((civ != 0) && ((1 << (units[unit].owner & 0x1fU) & (uint)game.human_players) == 0)
           ) && (civ2 != civ)) &&
         ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0 &&
           (unit_types[units[unit].type].hold != '\0')) &&
          ((civ = unit_stack_sum(unit,2), 1 < civ &&
           (civ = tile_is_ocean((int)units[unit].x,(int)units[unit].y), civ != 0)))))) {
        civ = map_distance_diag((int)x,(int)y,(int)units[unit].x,
                                   (int)units[unit].y);
        max_moves = unit_max_moves(unit);
        if (civ < max_moves) {
          units[unit].orders = ORDER_GOTO;
          units[unit].goto_x = x;
          units[unit].goto_y = y;
        }
      }
    }
  }
  return;
}

/* 00536bc9 auto */
int __cdecl ai_continent_defense_quota(iptr civ,iptr continent)

{
  uint batch;
  i32 quota;
  i32 remaining;
  int weight;
  
  quota = 0;
  remaining = (uint)civs[civ].continent_cities[continent];
  weight = 3;
  do {
    batch = remaining;
    if (4 < remaining) {
      batch = 5;
    }
    remaining -= batch;
    quota += weight * batch;
    weight--;
  } while ((remaining != 0) && (0 < weight));
  return quota;
}

/* 00536c4c auto */
i32 __cdecl ai_nuke_pick_target(iptr unit)

{
  uint strength;
  uint *treaty;
  bool in_range;
  iptr civ;
  iptr civ2;
  int y;
  iptr x;
  int owner_civ;
  iptr city2;
  iptr city;
  int unit2;
  uptr dir;
  int best_score;
  iptr city3;
  int score;
  
  civ = (iptr)units[unit].owner;
  best_score = -1;
  city2 = -1;
  city = 0;
  do {
    if (game.num_cities <= city) {
      y = 5 - civs[civ].num_cities;
      if (y < 1) {
        y = 0;
      }
      if ((10 - y <= best_score) &&
         (1 < civs[civ].units_active[units[unit].type])) {
        for (dir = 0; (int)dir < 8; dir++) {
          x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)cities[city2].x);
          y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)cities[city2].y;
          owner_civ = map_in_bounds(x,y);
          if (((owner_civ != 0) && (owner_civ = tile_city_owner(x,y), owner_civ < 0)) &&
             (owner_civ = tile_unit_owner(x,y), owner_civ < 0)) {
            unit_relocate(unit,x,y,1);
            units[unit].visibility = '\0';
            if (((uint)game.human_players & 1 << (cities[city2].owner & 0x1fU)) != 0) {
              if (((byte)(civs[civ].treaties[cities[city2].owner] >> 16) & (TREATY_NUKED >> 16)) == 0) {
                treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + cities[city2].owner * 4);
                *treaty = *treaty | 0x20000;
                treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + cities[city2].owner * 4);
                *treaty = *treaty & 0xffffffef;
                diplo_set_treaty_flags(civ,(int)cities[city2].owner,0x10000);
              }
              else {
                civs[cities[city2].owner].last_contact_turn[civ] =
                     civs[cities[city2].owner].last_contact_turn[civ] - 2;
              }
            }
            return dir ^ 4;
          }
        }
      }
      return 8;
    }
    if ((((((cities[city].id != 0) && (civ2 = (iptr)cities[city].owner, civ2 != civ)) &&
          (y = city_has_improvement(city,IMP_SDI_DEFENSE), y == 0)) &&
         (4 < cities[city].size)) &&
        (((strength = (uint)civs[civ2].military_strength * 2,
          strength + (uint)civs[civ].military_strength * -3 != 0 &&
          (uint)civs[civ].military_strength * 3 <= strength ||
          ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0)) ||
         ((civs[civ2].units_active[UNIT_NUCLEAR_MISSILE] == '\0' &&
          (strength = (uint)civs[civ2].military_strength * 2,
          strength - civs[civ].military_strength != 0 && civs[civ].military_strength <= strength))))))
       && ((civs[civ].treaties[civ2] & (TREATY_PEACE | TREATY_NUCLEAR_THREAT)) == 0x100)) {
      score = 0;
      for (dir = 0; (int)dir < 9; dir++) {
        x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)cities[city].x);
        y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)cities[city].y;
        owner_civ = map_in_bounds(x,y);
        if ((owner_civ != 0) && (owner_civ = tile_unit_owner(x,y), 0 < owner_civ)) {
          if (owner_civ == civ) {
            score -= 2;
          }
          else if ((civs[civ].treaties[owner_civ] & (TREATY_PEACE | TREATY_NUCLEAR_THREAT)) == 0x100) {
            for (unit2 = tile_first_unit(x,y); -1 < unit2;
                unit2 = unit_next_in_stack(unit2)) {
              score++;
            }
          }
          else {
            score -= 99;
          }
        }
      }
      score += ((int)cities[city].size >> 1);
      if (best_score < score) {
        in_range = false;
        for (city3 = 0; city3 < game.num_cities; city3++) {
          if ((cities[city3].id != 0) && (cities[city3].owner == civ)) {
            y = map_distance((int)cities[city3].x,(int)cities[city3].y,
                                       (int)cities[city].x,(int)cities[city].y);
            owner_civ = unit_max_moves(unit);
            if (y <= owner_civ / (int)(uint)cosmic.road_move_mult) {
              in_range = true;
              break;
            }
          }
        }
        if (in_range) {
          city2 = city;
          best_score = score;
        }
      }
    }
    city++;
  } while( true );
}

/* ai_air_unit_orders' parts return the orders it returns (a direction, -1
   orders set, -2 move on its goto) or AI_AIR_NEXT to go on. */
enum AiAirNext {
  AI_AIR_NEXT = 0x7fffffff
};

/* The locals of ai_air_unit_orders that its parts share: references to the
   entry point's own. */
struct AiAirOrders {
  iptr &unit;
  iptr &unit_type;
  iptr &x;
  iptr &y;
  int &mode;
  iptr &out_flag;
  int &may_go_home;
  iptr &landmass;
  int &set_flag;
  byte &civ2;
  char &small_val;
  u8 &stat_a;
  u8 &stat_b;
  int &civ;
  int &value;
  int &y2;
  iptr &x2;
  iptr &unit2;
  iptr &unit3;
  int &best_threat;
  int &home_dist;
  iptr &city;
  uptr &dir;
  int &range_left;
  int &best_score;
  int &score;
};

/* Part of ai_air_unit_orders (00537331): an attacker with a range (role 0):
   with a goto, on its first turn out (counter 0) it goes on with it; else a
   goto (in a scenario on turn 1, for its objective civ, to 45,53) to the best
   foreign city in reach of a civ without a treaty: big, weakly defended, on
   its landmass, with its own units around. */
static i32 ai_air_raid_city(AiAirOrders *o)
{
  iptr &unit_type = o->unit_type; int &set_flag = o->set_flag; iptr &x = o->x;
  int &mode = o->mode; iptr &landmass = o->landmass;
  iptr &out_flag = o->out_flag; iptr &y = o->y; char &small_val = o->small_val;
  int &civ = o->civ; int &value = o->value; int &y2 = o->y2; iptr &x2 = o->x2;
  iptr &unit2 = o->unit2; iptr &unit3 = o->unit3; iptr &city = o->city;
  uptr &dir = o->dir; int &best_score = o->best_score; int &score = o->score;
  if (((mode == 0) && (unit_types[units[unit2].type].range != '\0')) &&
     ((char)unit_types[unit_type].attack < 'c')) {
    if (units[unit2].orders == ORDER_GOTO) {
      if ((units[unit2].counter == '\0') && (set_flag != 0)) {
        *(i32 *)out_flag = 1;
        if (*(int *)out_flag != 0) {
          return 0xfffffffe;
        }
        return 0xffffffff;
      }
    }
    else {
      unit3 = -1;
      best_score = -9999;
      if ((((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) &&
         ((game.turn == 1 &&
          ((scenario.objective_civ == civ && (value = tile_unit_owner(0x2d,0x35), -1 < value)))))) {
        unit_set_goto(unit2,0x42,0x2d,0x35);
        if (set_flag != 0) {
          *(i32 *)out_flag = 1;
        }
        if (*(int *)out_flag != 0) {
          return 0xfffffffe;
        }
        return 0xffffffff;
      }
      for (city = 0; city < game.num_cities; city++) {
        if (((cities[city].id != 0) && (cities[city].owner != civ)) &&
           ((*(byte *)&civs[civ].treaties[cities[city].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) {
          value = map_distance(*(i32 *)x,*(i32 *)y,
                                      (int)cities[city].x,(int)cities[city].y);
          y2 = unit_moves_left(unit2);
          if ((value <= y2 / (int)(uint)cosmic.road_move_mult) &&
             (value = tile_first_unit((int)cities[city].x,(int)cities[city].y),
             -1 < value)) {
            small_val = cities[city].size;
            value = unit_stack_sum(value,2);
            score = small_val * 10 + value * -0x19;
            value = tile_landmass((int)cities[city].x,(int)cities[city].y);
            if (value == landmass) {
              score += 0x28;
            }
            for (dir = 0; (int)dir < 8; dir++) {
              x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir]);
              value = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
              y2 = map_in_bounds(x2,value);
              if ((y2 != 0) && (value = tile_unit_owner(x2,value), value == civ)) {
                score += 0x3c;
              }
            }
            if (best_score <= score) {
              best_score = score;
              unit3 = city;
            }
          }
        }
      }
      if (-1 < unit3) {
        unit_set_goto(unit2,0x42,(int)cities[unit3].x,(int)cities[unit3].y);
        if (set_flag != 0) {
          *(i32 *)out_flag = 1;
        }
        if (*(int *)out_flag != 0) {
          return 0xfffffffe;
        }
        return 0xffffffff;
      }
    }
  }
  return AI_AIR_NEXT;
}

/* Part of ai_air_unit_orders (00537331): an air defender with a target: goes
   for it when it can get back to a city of its own afterwards (or cannot
   reach home anyway); else it moves to the city of its own in range nearest
   the target (or a city of a human civ near it, flag 0x10000, it is at war
   with) to wait, or goes home, or stays (direction 8). */
static i32 ai_air_strike_target(AiAirOrders *o)
{
  int x3;
  int y3;
  int city_dist;
  iptr city2;
  iptr nearest_city;
  int &may_go_home = o->may_go_home; iptr &x = o->x; iptr &y = o->y;
  int &civ = o->civ; int &value = o->value; int &y2 = o->y2; iptr &x2 = o->x2;
  iptr &unit2 = o->unit2; iptr &unit3 = o->unit3;
  int &best_threat = o->best_threat; int &home_dist = o->home_dist;
  iptr &city = o->city; uptr &dir = o->dir; int &range_left = o->range_left;
  int &best_score = o->best_score;
  if (-1 < unit3) {
    if ((best_score <= range_left) || (range_left < home_dist)) {
      if (((home_dist <= range_left - best_score) && (best_score == 1)) || (range_left < home_dist)) {
        unit_set_goto(unit2,0x41,(int)units[unit3].x,(int)units[unit3].y);
        return 0xffffffff;
      }
      best_threat = 9999;
      for (dir = 0; (int)dir < 8; dir++) {
        x2 = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)units[unit3].x);
        value = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)units[unit3].y;
        y2 = map_in_bounds(x2,value);
        if ((((y2 != 0) && (y2 = tile_city_owner(x2,value), y2 < 0)) &&
            ((y2 = tile_unit_owner(x2,value), y2 < 0 || (civ == y2)))) &&
           (y2 = map_distance(*(i32 *)x,*(i32 *)y,x2,
                                        value), y2 <= best_score - 1)) {
          advisor_window_block.nearest_city_dist = 9999;
          nearest_city = -1;
          for (city = 0; city < game.num_cities; city++) {
            if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
               (y2 = map_distance(x2,value,(int)cities[city].x,
                                            (int)cities[city].y), y2 < advisor_window_block.nearest_city_dist)) {
              nearest_city = city;
              advisor_window_block.nearest_city_dist = y2;
            }
          }
          if (nearest_city < 0) {
            advisor_window_block.nearest_city_dist = 0;
          }
          if (advisor_window_block.nearest_city_dist < best_threat) {
            best_threat = advisor_window_block.nearest_city_dist;
          }
        }
      }
      if (best_score + best_threat <= range_left) {
        unit_set_goto(unit2,0x41,(int)units[unit3].x,(int)units[unit3].y);
        return 0xffffffff;
      }
    }
    x3 = (int)units[unit3].x;
    y3 = (int)units[unit3].y;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) &&
          (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) &&
         ((cities[city].flags & CITYF_AIRLIFTED) != 0)) {
        city_dist = map_distance(x3,y3,(int)cities[city].x,
                                      (int)cities[city].y);
        if ((*(byte *)&civs[civ].treaties[cities[city].owner] & TREATY_VENDETTA) == 0) {
          if (((byte)(civs[civ].treaties[cities[city].owner] >> 8) & (TREATY_WAR >> 8)) == 0) {
            city_dist = city_dist << 1;
          }
          if ((*(byte *)&civs[civ].treaties[cities[city].owner] & TREATY_PEACE) != 0) continue;
        }
        if (city_dist < best_score) {
          best_score = city_dist;
          x3 = (int)cities[city].x;
          y3 = (int)cities[city].y;
        }
      }
    }
    best_score = 999;
    city2 = -1;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
         ((value = map_distance(*(i32 *)x,*(i32 *)y,
                                       (int)cities[city].x,(int)cities[city].y),
          value <= range_left &&
          (value = map_distance(x3,y3,(int)cities[city].x,
                                       (int)cities[city].y), value < best_score)))) {
        city2 = city;
        best_score = value;
      }
    }
    if (-1 < city2) {
      unit_set_goto(unit2,0x61,(int)cities[city2].x,(int)cities[city2].y);
      return 0xffffffff;
    }
    if (may_go_home != 0) {
      unit_go_home(unit2);
      return 0xffffffff;
    }
    units[unit2].ai_task = 'h';
    return 8;
  }
  return AI_AIR_NEXT;
}

/* Part of ai_air_unit_orders (00537331): an air defender (role 3) picks the
   nearest enemy unit it sees without a treaty, alone, and that it can beat
   (its defense against its attack), nearer for a human civ's, a ranged unit
   or a lone one: unit3, at distance best_score. */
static void ai_air_pick_target(AiAirOrders *o)
{
  u8 att_fp;
  ushort own_flags;
  byte terrain;
  int threat_score;
  int defense_power;
  iptr &y = o->y; iptr &x = o->x; iptr &unit_type = o->unit_type;
  iptr &unit = o->unit; byte &civ2 = o->civ2; u8 &stat_a = o->stat_a;
  u8 &stat_b = o->stat_b; int &civ = o->civ; int &value = o->value;
  int &y2 = o->y2; iptr &unit2 = o->unit2; iptr &unit3 = o->unit3;
  int &best_threat = o->best_threat; int &best_score = o->best_score;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((((units[unit].id != 0) && (units[unit].owner != civ)) &&
        ((*(byte *)&civs[civ].treaties[units[unit].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) &&
       (((1 << (civ2 & 0x1f) & (uint)units[unit].visibility) != 0 ||
        ((int)units[unit].owner == (uint)civ2)))) {
      if (unit_types[units[unit].type].domain != DOMAIN_AIR) {
        if (unit_types[units[unit].type].hold == '\0') {
          value = unit_stack_sum(unit,2);
        }
        else {
          value = unit_stack_sum(unit,5);
        }
        if (1 < value) continue;
        own_flags = units[unit2].flags;
        stat_a = unit_types[unit_type].attack;
        stat_b = unit_types[unit_type].hit_points;
        att_fp = unit_types[unit_type].firepower;
        defense_power = (int)(char)unit_types[units[unit].type].hit_points *
                   (int)(char)unit_types[units[unit].type].defense *
                   (int)(char)unit_types[units[unit].type].firepower *
                   (3 - (uint)((units[unit].flags & UNITF_VETERAN) == 0));
        if ((unit_types[units[unit].type].hold != '\0') &&
           (value = unit_stack_sum(unit,2), value == 1)) {
          defense_power = defense_power * 2;
        }
        terrain = tile_terrain((int)units[unit].x,(int)units[unit].y);
        if ((defense_power * (char)terrain_types[terrain].defense >> 1) * 3 >
            (int)((3 - (uint)((own_flags & UNITF_VETERAN) == 0)) * (int)(char)stat_a * (int)(char)stat_b *
                  (int)(char)att_fp * 2)) continue;
      }
      value = map_distance(*(i32 *)x,*(i32 *)y,
                                  (int)units[unit].x,(int)units[unit].y);
      threat_score = value * 2;
      if (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) {
        threat_score = threat_score >> 1;
      }
      if (unit_types[units[unit].type].range != '\0') {
        threat_score--;
      }
      y2 = unit_stack_sum(unit,2);
      if (y2 == 1) {
        threat_score--;
      }
      if (threat_score <= best_threat) {
        best_threat = threat_score;
        unit3 = unit;
        best_score = value;
      }
    }
  }
}

/* Part of ai_air_unit_orders (00537331): a non-nuclear unit with flag 0x1000,
   which moves anywhere within its moves at once: picks the ground unit of a
   civ at war in reach worth most (its cost, more alone in a city near its own
   units, at sea or in a fortress, by its cargo) and appears next to it,
   returning the direction to attack it; without one, it moves to a city of
   its own in reach with fewer such units, or stays (direction 8). */
static i32 ai_air_jump_attack(AiAirOrders *o)
{
  short x4;
  short y4;
  bool at_airbase;
  int check;
  int target_dist;
  i32 tile_imps;
  uint move_mult;
  iptr &y = o->y; iptr &unit = o->unit; iptr &unit_type = o->unit_type;
  iptr &x = o->x; char &small_val = o->small_val; int &civ = o->civ;
  int &value = o->value; int &y2 = o->y2; iptr &x2 = o->x2;
  iptr &unit2 = o->unit2; iptr &unit3 = o->unit3; iptr &city = o->city;
  uptr &dir = o->dir; int &best_score = o->best_score; int &score = o->score;
  if (((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) &&
     ((char)unit_types[unit_type].attack < 'c')) {
    best_score = (int)(char)unit_types[units[unit].type].cost << 2;
    unit3 = -1;
    unit = 0;
    do {
      if (game.num_units <= unit) {
        if (-1 < unit3) {
          x4 = units[unit3].x;
          y4 = units[unit3].y;
          for (dir = 0; (int)dir < 8; dir++) {
            x2 = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x4);
            value = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y4;
            y2 = map_in_bounds(x2,value);
            if ((y2 != 0) && (y2 = tile_city_or_unit_owner(x2,value), y2 < 0)) {
              unit_relocate(unit2,x2,value,1);
              units[unit2].visibility = '\0';
              *(iptr *)x = x2;
              *(int *)y = value;
              return dir ^ 4;
            }
          }
        }
        value = unit_stack_sum(unit2,8);
        if (1 < value) {
          for (city = 0; city < game.num_cities; city++) {
            if ((cities[city].id != 0) && (cities[city].owner == civ)) {
              x2 = tile_first_unit((int)cities[city].x,(int)cities[city].y);
              y2 = unit_stack_sum(x2,8);
              if (y2 < value) {
                y2 = map_distance(*(i32 *)x,*(i32 *)y,
                                            (int)cities[city].x,(int)cities[city].y);
                check = unit_max_moves(unit2);
                if (y2 <= check / (int)(uint)cosmic.road_move_mult) {
                  unit_relocate(unit2,(int)cities[city].x,(int)cities[city].y,1);
                  units[unit2].visibility = '\0';
                  *(int *)x = (int)cities[city].x;
                  *(int *)y = (int)cities[city].y;
                  return 8;
                }
              }
            }
          }
        }
        return 8;
      }
      if ((((units[unit].id != 0) && (units[unit].owner != civ)) &&
          (((byte)(civs[civ].treaties[units[unit].owner] >> 8) & (TREATY_WAR >> 8)) != 0)) &&
         (unit_types[units[unit].type].domain != DOMAIN_AIR)) {
        value = (int)units[unit].x;
        y2 = (int)units[unit].y;
        check = map_in_bounds(value,y2);
        if (check != 0) {
          check = unit_max_moves(unit2);
          move_mult = (uint)cosmic.road_move_mult;
          target_dist = map_distance(*(i32 *)x,*(i32 *)y,value,y2);
          if (target_dist <= check / (int)move_mult) {
            score = (int)(char)unit_types[units[unit].type].cost;
            tile_imps = tile_improvements(value,y2);
            if ((((byte)tile_imps & TILE_AIRBASE) == 0x40) ||
               (check = tile_airbase_owner(value,y2), check != 0)) {
              at_airbase = true;
            }
            else {
              at_airbase = false;
            }
            if ((at_airbase) || (check = tile_city_owner(value,y2), -1 < check)) {
              if (!at_airbase) {
                check = unit_stack_sum(unit,2);
                if (check == 1) {
                  score = score << 1;
                }
                for (dir = 0; (int)dir < 8; dir++) {
                  x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + value);
                  small_val = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
                  check = map_in_bounds(x2,small_val + y2);
                  if ((check != 0) &&
                     (check = tile_unit_owner(x2,small_val + y2), check == civ)) {
                    score = score << 1;
                    break;
                  }
                }
              }
            }
            else {
              score = unit_stack_sum(unit,0);
            }
            check = tile_is_ocean(value,y2);
            if (check != 0) {
              score = score << 1;
            }
            tile_imps = tile_improvements(value,y2);
            if (((byte)tile_imps & TILE_AIRBASE) == 0x40) {
              score = score << 1;
            }
            score += (char)unit_types[units[unit].type].hold;
            if (best_score <= score) {
              unit3 = unit;
              best_score = score;
            }
          }
        }
      }
      unit++;
    } while( true );
  }
  return AI_AIR_NEXT;
}

/* 00537331 auto */
i32 __cdecl
ai_air_unit_orders(iptr unit,iptr unit_type,iptr x,iptr y,int mode,iptr out_flag,
            int may_go_home,iptr landmass,int set_flag)

{
  byte civ2;
  char small_val;
  u8 stat_a;
  u8 stat_b;
  int civ;
  int value;
  int y2;
  iptr x2;
  iptr unit2;
  iptr unit3;
  int best_threat;
  int home_dist;
  iptr city;
  uptr dir;
  int range_left;
  int best_score;
  int score;
  i32 next;  /* what a part returns */
  AiAirOrders st = {unit, unit_type, x, y, mode, out_flag, may_go_home,
    landmass, set_flag, civ2, small_val, stat_a, stat_b, civ, value, y2, x2,
    unit2, unit3, best_threat, home_dist, city, dir, range_left, best_score,
    score};
  
  unit2 = unit;
  civ2 = units[unit].owner;
  civ = (int)(char)civ2;
  if (unit_types[units[unit].type].range == '\0') {
    range_left = unit_max_moves(unit);
    range_left = range_left * 2;
  }
  else {
    stat_a = unit_types[units[unit].type].range;
    stat_b = units[unit].counter;
    value = unit_max_moves(unit);
    range_left = unit_moves_left(unit);
    range_left = ((int)(char)stat_a - ((char)stat_b + 1)) * value + range_left;
  }
  range_left = range_left / (int)(uint)cosmic.road_move_mult;
  home_dist = 9999;
  for (city = 0; city < game.num_cities; city++) {
    if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
       (value = map_distance(*(i32 *)x,*(i32 *)y,
                                    (int)cities[city].x,(int)cities[city].y),
       value < home_dist)) {
      home_dist = value;
    }
  }
  if (unit_types[units[unit].type].range != '\0') {
    if (unit_types[units[unit].type].range == 1) {
      if (units[unit].orders != ORDER_GOTO) {
        value = unit_max_moves(unit);
        y2 = unit_moves_left(unit);
        if (((y2 <= value >> 1) && (range_left < home_dist + 1)) && (home_dist <= range_left)) {
          units[unit].ai_task = 'H';
          unit_go_home(unit);
          return 0xffffffff;
        }
      }
    }
    else if (((int)(char)unit_types[units[unit].type].range / 2 <=
              (int)(char)units[unit].counter) && (home_dist <= range_left)) {
      units[unit].ai_task = 'H';
      unit_go_home(unit);
      return 0xffffffff;
    }
  }
  next = ai_air_jump_attack(&st);
  if (next != AI_AIR_NEXT) return next;
  if (mode == 3) {
    best_score = 9999;
    best_threat = 9999;
    unit3 = -1;
    ai_air_pick_target(&st);
    next = ai_air_strike_target(&st);
    if (next != AI_AIR_NEXT) return next;
    if (may_go_home != 0) {
      unit_go_home(unit2);
      return 0xffffffff;
    }
  }
  next = ai_air_raid_city(&st);
  if (next != AI_AIR_NEXT) return next;
  return 0xffffff9d;
}

/* Where ai_unit_choose_orders goes on after one of its parts: NEXT is the
   line after the call, the others its labels (finish_orders, apply_dir...),
   a break or continue of its transport loop, or its return value. */
enum AiOrdersNext {
  AI_ORDERS_NEXT,
  AI_ORDERS_FINISH_ORDERS,
  AI_ORDERS_APPLY_DIR,
  AI_ORDERS_PLAN_MOVE,
  AI_ORDERS_GUARD_CHECK,
  AI_ORDERS_SCENARIO_GUARD_CHECK,
  AI_ORDERS_BREAK,
  AI_ORDERS_CONTINUE,
  AI_ORDERS_RETURN_0,
  AI_ORDERS_RETURN_1
};

/* The locals of ai_unit_choose_orders that its parts share: references to the
   entry point's own. */
struct AiUnitOrders {
  Tile *&map_tile;
  u8 &stat;
  ushort &military;
  bool &forced_attack;
  bool &stay_home;
  byte &tile_byte;
  uptr &civ;
  i32 &result;
  iptr &landmass;
  int &city7;
  uptr &city3;
  iptr &x;
  iptr &x5;
  int &y;
  byte &owner_byte;
  uint &terrain;
  byte *&tile;
  int &check;
  int &near_dist;
  uptr &unit_type;
  uint &policy;
  bool &is_settler;
  bool &settler_type;
  bool &cond;
  i32 &arg_hi;
  iptr &unit2;
  uint &x4;
  int &goal_match;
  iptr &unit;
  uptr &city6;
  int &in_enemy_zoc;
  int &civ4;
  int &unit_domain;
  uptr &city;
  uptr &needed_defenders;
  int &is_attack;
  int &near_landmass;
  int &max_score;
  int &fort_need;
  int &ally_territory;
  int &y3;
  uint &ai_role;
  int &needs_units;
  uint &damage_level;
  int &x2;
  uptr &unit4;
  uptr &best_dir;
  uint &flag;
  uint &on_fortress;
  int &attack_bonus;
  uint &room_aboard;
  int &has_orders;
  u32 &type_flags;
  int &defender_count;
  iptr &unit5;
  int &retried;
  int &hold_pos;
  uptr &city4;
  uptr &terrain2;
  uptr &dir2;
  uptr &civ2;
  uint &y2;
  int &cand_score;
  int &explore;
  int &attack_cost;
  uptr &dir;
  int &own_landmass;
  iptr &unit3;
  int &will_attack;
  int &is_guard;
  uptr &city5;
  uptr &city2;
  int &own_dist;
  int &best_score;
  int &freed_count;
  uint &target_policy;
  int &no_moves;
  int &here_landmass;
  int &score;
  uint &tile_flags;
  uint &tasks_mask;
};

/* Part of ai_unit_choose_orders (00538a29): a trade unit (role 7): from the
   sea, the coast of a landmass with cities; else a goto to the best trade
   route city on its landmass, or home. */
static int ai_orders_caravan(AiUnitOrders *o)
{
  byte &tile_byte = o->tile_byte; uptr &civ = o->civ; i32 &result = o->result;
  iptr &landmass = o->landmass; int &city7 = o->city7; uptr &city3 = o->city3;
  iptr &x = o->x; i32 &arg_hi = o->arg_hi; iptr &unit = o->unit;
  uptr &city6 = o->city6; uptr &city = o->city;
  int &near_landmass = o->near_landmass; int &y3 = o->y3; int &x2 = o->x2;
  uptr &unit4 = o->unit4; uptr &best_dir = o->best_dir; uint &flag = o->flag;
  uptr &city4 = o->city4; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  int &cand_score = o->cand_score; uptr &dir = o->dir; uptr &city5 = o->city5;
  uptr &city2 = o->city2; int &best_score = o->best_score;
  int &score = o->score;
  if (units[unit].orders == ORDER_GOTO) return AI_ORDERS_FINISH_ORDERS;
  city7 = tile_is_ocean(x2,y3);
  if (city7 != 0) {
    city6 = 0xffffffff;
    best_score = 0;
    for (dir = 0; (int)dir < 8; dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
      city7 = map_in_bounds(x,y2);
      if (((city7 != 0) && (city7 = tile_is_ocean(x,y2), city7 == 0)) &&
         (city7 = tile_landmass(x,y2),
         *(char *)&civs[civ].continent_flags[city7] != '\0')) {
        score = 0;
        for (civ2 = 1; (int)civ2 < 8; civ2++) {
          if (civ2 == civ) {
            city7 = (civs[civ].continent_city_size[landmass] + 1) >> 1;
          }
          else {
            city7 = (uint)civs[civ].continent_city_size[landmass] * 2;
          }
          score += city7;
        }
        if (best_score < score) {
          best_score = score;
          city6 = dir;
        }
      }
    }
    if (-1 < (int)city6) {
      units[unit].ai_task = 't';
      best_dir = city6;
      city3 = best_dir;
      return AI_ORDERS_APPLY_DIR;
    }
  }
  best_score = 0;
  city6 = 0xffffffff;
  if (units[unit].home_city == -1) {
    city5 = 0xffffffff;
  }
  else {
    city5 = (uptr)(byte)units[unit].home_city;
  }
  if ((int)city5 < 0) {
    city5 = city2;
  }
  if ((int)city5 < 0) {
    unit_destroy_redraw(unit,1);
    return AI_ORDERS_RETURN_1;
  }
  for (city = 0; (int)city < (int)game.num_cities; city++) {
    if ((cities[city].id != 0) &&
       (city7 = tile_landmass((int)cities[city].x,(int)cities[city].y),
       city7 == landmass)) {
      flag = 1;
      for (dir = 0; (int)dir < (int)(char)cities[city5].trade_routes;
          dir = dir + 1) {
        if ((int)cities[city5].route_partner[dir] == city) {
          flag = 0;
        }
      }
      city7 = map_distance_diag((int)cities[city].x,(int)cities[city].y,
                                  (int)cities[city5].x,(int)cities[city5].y);
      cand_score = (((int)cities[city5].base_trade + (int)cities[city].base_trade) *
                 city7) / 0x18;
      if ((int)cities[city].owner == civ) {
        cand_score = cand_score / 2;
      }
      else {
        arg_hi = 100;
        result = 0;
        x = civ_get_attitude(civ,(int)cities[city].owner);
        unit4 = int_clamp(x,result,arg_hi);
        cand_score = (int)((200 - unit4) * cand_score) / 100;
      }
      if (flag == 0) {
        cand_score = cand_score / 2;
      }
      cand_score = ((5 - (char)cities[city].trade_routes) * cand_score) / 5;
      if ((((-1 < (int)city4) && (city2 != city4)) && (landmass == near_landmass)) &&
         ((cities[city].owner != cities[city4].owner &&
          (cand_score = cand_score / 3,
          ((uint)game.human_players & 1 << (cities[city4].owner & 0x1fU)) != 0)))) {
        cand_score = cand_score / 2;
      }
      if (best_score < cand_score) {
        best_score = cand_score;
        city6 = city;
      }
    }
  }
  if (-1 < (int)city6) {
    unit_set_goto(unit,99,(int)cities[city6].x,(int)cities[city6].y);
    return AI_ORDERS_FINISH_ORDERS;
  }
  if (units[unit].home_city != -1) {
    tile_byte = units[unit].home_city;
    city7 = tile_landmass((int)cities[tile_byte].x,(int)cities[tile_byte].y);
    if (city7 == landmass) {
      unit_set_goto(unit,99,(int)cities[tile_byte].x,(int)cities[tile_byte].y);
      return AI_ORDERS_FINISH_ORDERS;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a diplomat (role 6): keeps its
   goto to a foreign city it may enter; else, on land, a goto to the best city
   to visit (its own, near, or a foreign one weighed by treaty, attitude and
   the tech race), disbanding when there is none. */
static int ai_orders_diplomat(AiUnitOrders *o)
{
  int race_behind;
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  iptr &x = o->x; int &y = o->y; uint &terrain = o->terrain;
  iptr &unit = o->unit; uptr &city6 = o->city6; uptr &city = o->city;
  int &y3 = o->y3; int &x2 = o->x2; uint &flag = o->flag;
  int &has_orders = o->has_orders; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  int &cand_score = o->cand_score; uptr &dir = o->dir;
  int &best_score = o->best_score; int &score = o->score;
  if ((((units[unit].orders == ORDER_GOTO) &&
       (city7 = city_at((int)units[unit].goto_x,
                                    (int)units[unit].goto_y), -1 < city7)) &&
      ((int)cities[city7].owner != civ)) &&
     ((*(byte *)&civs[civ].treaties[cities[city7].owner] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) {
    has_orders = 1;
  }
  else {
    city7 = tile_is_ocean(x2,y3);
    if (city7 == 0) {
      race_behind = ai_spaceship_race_behind(civ);
      best_score = -999;
      city6 = 0xffffffff;
      for (city = 0; (int)city < (int)game.num_cities; city = city + 1
          ) {
        if (cities[city].id != 0) {
          city7 = tile_landmass((int)cities[city].x,(int)cities[city].y);
          y = tile_is_ocean(x2,y3);
          if (y == 0) {
            if (city7 != landmass) continue;
          }
          else {
            flag = 0;
            for (dir = 0; (int)dir < 8; dir++) {
              x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
              y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
              y = map_in_bounds(x,y2);
              if (((y != 0) && (y = tile_is_ocean(x,y2), y == 0)
                  ) && (y = tile_landmass(x,y2), y == city7)) {
                flag = 1;
                break;
              }
            }
            if (flag == 0) continue;
          }
          civ2 = (uptr)cities[city].owner;
          cand_score = map_distance_diag(x2,y3,(int)cities[city].x,
                                        (int)cities[city].y);
          if (((uint)(byte)units[unit].home_city % 3 != (int)city % 3) &&
             (terrain = tile_improvements(x2,y3),
             (int)(((terrain & 0x10) >> 4) + 3) < cand_score)) {
            cand_score += 8;
          }
          if (civ2 == civ) {
            score = 99 - cand_score;
          }
          else {
            score = 100;
            if ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0) {
              if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
                  ((cities[city].flags & CITYF_TECH_STOLEN) != 0)) ||
                 ((city7 = ai_spaceship_race_behind(civ), city7 == 0 &&
                  ((game.power_rank[civ2] < game.power_rank[civ] ||
                   ((((units[unit].type != UNIT_SPY &&
                      ((int)((uint)civs[civ2].num_advances -
                            (uint)civs[civ].num_advances) < 10)) &&
                     (((GOV_FUNDAMENTALISM < civs[civ].government || (game.power_rank[civ2] < 7))
                      || (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) !=
                           0 && (civs[civ2].num_advances <
                                 civs[civ].num_advances)))))) ||
                    (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0 &&
                     (((GOV_FUNDAMENTALISM < civs[civ2].government &&
                       (civs[civ].continent_military[landmass] <
                        civs[civ2].continent_attack[landmass])) &&
                      (*(char *)&civs[civ].continent_cities[landmass] != '\0')))))))))))
                 ) continue;
              score = score / 2;
            }
            if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) == 0) {
              city7 = civ_get_attitude(civ,civ2);
              score += city7;
            }
            else {
              score += 100;
            }
            score = score / (cand_score + 1);
            if ((cities[city].flags & CITYF_TECH_STOLEN) != 0) {
              score = score / 2;
              if (civs[civ].num_advances + 6 < (uint)civs[civ2].num_advances) {
                score = score / 2;
              }
              if (race_behind != 0) {
                score = 1;
              }
            }
            score += 100;
          }
          if (best_score < score) {
            best_score = score;
            city6 = city;
          }
        }
      }
      if ((int)city6 < 0) {
        if (units[unit].orders != ORDER_GOTO) {
          unit_destroy_redraw(unit,1);
          return AI_ORDERS_RETURN_1;
        }
        return AI_ORDERS_FINISH_ORDERS;
      }
      has_orders = 1;
      if ((units[unit].orders != ORDER_GOTO) || ((int)cities[city6].owner != civ)) {
        unit_set_goto(unit,0x53,(int)cities[city6].x,
                           (int)cities[city6].y);
      }
    }
  }
  return AI_ORDERS_PLAN_MOVE;
}

/* Part of ai_unit_choose_orders (00538a29): a paratrooper (unit flag 0x100):
   in its own city, unmoved, drops next to an undefended city of a civ it is
   at war with in range (on the best tile around it); else it holds. Away from
   home, back to the nearest city of its own or fortified. */
static int ai_orders_paratrooper(AiUnitOrders *o)
{
  int y4;
  int x6;
  byte &tile_byte = o->tile_byte; uptr &civ = o->civ; i32 &result = o->result;
  iptr &landmass = o->landmass; int &city7 = o->city7; iptr &x = o->x;
  iptr &unit = o->unit; uptr &city6 = o->city6; uptr &city = o->city;
  int &y3 = o->y3; int &x2 = o->x2; uptr &best_dir = o->best_dir;
  uint &on_fortress = o->on_fortress; int &hold_pos = o->hold_pos;
  uptr &city4 = o->city4; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  uptr &dir = o->dir; int &own_landmass = o->own_landmass;
  uptr &city2 = o->city2; int &own_dist = o->own_dist;
  int &best_score = o->best_score; int &score = o->score;
  if (own_dist == 0) {
    if ((units[unit].moves_used == '\0') && (units[unit].orders != ORDER_GOTO)) {
      best_score = 0;
      for (city = 0; (int)city < (int)game.num_cities; city = city + 1
          ) {
        if ((((cities[city].id != 0) && ((int)cities[city].owner != civ)) &&
            (((byte)(civs[civ].treaties[cities[city].owner] >> 8) & (TREATY_WAR >> 8)) !=
             0)) && (city7 = map_distance(x2,y3,
                                                 (int)cities[city].x,
                                                 (int)cities[city].y),
                    city7 <= (int)(uint)cosmic.paradrop_range)) {
          city7 = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
          if ((city7 < 0) && (best_score < cities[city].size + 999)) {
            best_score = cities[city].size + 999;
            x6 = (int)cities[city].x;
            y4 = (int)cities[city].y;
          }
          if (0 < best_score) {
            best_score = 0;
            city6 = 0xffffffff;
            for (dir = 9; -1 < (int)dir; dir--) {
              x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x6);
              y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y4;
              city7 = map_in_bounds(x,y2);
              if ((city7 != 0) &&
                 (city7 = map_distance(x2,y3,x,y2),
                 city7 <= (int)(uint)cosmic.paradrop_range)) {
                civ2 = tile_unit_owner(x,y2);
                if (civ2 == civ) {
                  city6 = 0xffffffff;
                  break;
                }
                if ((int)civ2 < 0) {
                  score = 0;
                  if (dir == 8) {
                    score = 500;
                  }
                  result = tile_improvements(x,y2);
                  if (((byte)result & 0x42) == 0x40) {
                    score += 0x32;
                  }
                  tile_byte = tile_terrain(x,y2);
                  score += (char)terrain_types[tile_byte].defense * 3;
                  city7 = map_distance(x2,y3,x,y2);
                  score += city7;
                  if (best_score < score) {
                    city6 = dir;
                    best_score = score;
                  }
                }
              }
            }
            if (-1 < (int)city6) {
              x6 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[city6] + x6);
              unit_paradrop(unit,x6,
                                 y4 + (((i8 *)&dir_dy_table_block.dir_dy_table))[city6]);
              return AI_ORDERS_RETURN_0;
            }
          }
        }
      }
    }
    if (units[unit].orders != ORDER_GOTO) {
      hold_pos = 1;
      return AI_ORDERS_PLAN_MOVE;
    }
  }
  else {
    if (((city2 == city4) && (landmass == own_landmass)) &&
       (city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 == 0)) {
      unit_set_goto(unit,99,(int)cities[city].x,(int)cities[city].y);
      return AI_ORDERS_FINISH_ORDERS;
    }
    if ((on_fortress != 0) && (city7 = unit_stack_sum(unit,2), city7 == 1)) {
      units[unit].ai_task = 'F';
      best_dir = 8;
      return AI_ORDERS_FINISH_ORDERS;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): land units weigh the tiles around
   for a city site: a settler founds a city where it stands when the site is
   good enough (its civ's start on turn 1, or near a human civ ahead in tech)
   or heads for the best tile, others post it as a goal; attackers and
   defenders take a hut next to them. */
static int ai_orders_city_site(AiUnitOrders *o)
{
  int tile_count;
  bool &forced_attack = o->forced_attack; uptr &civ = o->civ;
  i32 &result = o->result; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &city3 = o->city3; iptr &x = o->x; int &y = o->y;
  int &near_dist = o->near_dist; bool &is_settler = o->is_settler;
  bool &settler_type = o->settler_type; i32 &arg_hi = o->arg_hi;
  iptr &unit = o->unit; int &in_enemy_zoc = o->in_enemy_zoc;
  uptr &city = o->city; int &near_landmass = o->near_landmass; int &y3 = o->y3;
  uint &ai_role = o->ai_role; int &x2 = o->x2; uptr &best_dir = o->best_dir;
  uptr &city4 = o->city4; uint &y2 = o->y2; uptr &dir = o->dir;
  int &best_score = o->best_score; int &score = o->score;
  is_settler = ai_role == 5;
  settler_type = unit_types[units[unit].type].role == ROLE_SETTLE;
  best_score = tile_owner_or_fertility(x2,y3);
  if (((near_dist < 4) && (landmass == near_landmass)) && (7 < best_score)) {
    best_score = 8;
  }
  city7 = tile_is_ocean(x2,y3);
  if (city7 != 0) {
    best_score = 0;
  }
  if ((((is_settler) &&
       (civ_clear_standing_goals_near(civ,ROLE_SETTLE,x2,y3,0), civs[civ].num_cities == 0)) &&
      (game.turn == 1)) &&
     (((start_positions.x[civs[civ].tribe] == x2 && (start_positions.y[civs[civ].tribe] == y3))
      && (7 < best_score)))) {
    unit_found_city(unit,-1,0);
    return AI_ORDERS_RETURN_1;
  }
  city = city4;
  if (((best_score != 0) && (((int)ai_role < 2 || (settler_type)))) && (!forced_attack)) {
    best_dir = 0xffffffff;
    if (is_settler) {
      tile_count = 0x14;
    }
    else {
      tile_count = 8;
    }
    for (dir = 0; (int)dir < tile_count; dir++) {
      x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[dir] + x2);
      y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[dir] + y3;
      city7 = map_in_bounds(x,y2);
      if (((city7 != 0) && (city7 = tile_is_ocean(x,y2), city7 == 0)) &&
         (city7 = tile_landmass(x,y2), city7 == landmass)) {
        score = tile_owner_or_fertility(x,y2);
        if (best_score < score) {
          best_dir = dir;
          best_score = score;
        }
        city7 = tile_has_hut(x,y2);
        if (((city7 != 0) && ((int)ai_role < 2)) && ((int)dir < 8)) {
          best_dir = dir;
          units[unit].ai_task = 'U';
          city3 = best_dir;
          return AI_ORDERS_APPLY_DIR;
        }
      }
    }
    if (((7 < best_score) && (in_enemy_zoc == 0)) &&
       (((0xf - near_dist <= best_score ||
         (((is_settler && ((int)best_dir < 0)) &&
          (*(char *)&civs[civ].continent_cities[landmass] == '\0')))) ||
        (((is_settler && ((int)best_dir < 0)) &&
         (units[unit].aux_timer = units[unit].aux_timer + 1,
         4 < (char)units[unit].aux_timer)))))) {
      if (is_settler) {
        if ((int)best_dir < 0) {
          unit_found_city(unit,-1,0);
          return AI_ORDERS_RETURN_1;
        }
        units[unit].ai_task = '2';
        city3 = best_dir;
        if ((int)best_dir < 8) return AI_ORDERS_APPLY_DIR;
        city7 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[best_dir] + y3;
        result = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[best_dir] + x2);
        unit_set_goto(unit,0x32,result,city7);
      }
      else if ((-1 < (int)best_dir) && ((int)best_dir < 8)) {
        y = 2;
        arg_hi = 5;
        city7 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[best_dir] + y3;
        result = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[best_dir] + x2);
        civ_add_standing_goal(civ,result,city7,arg_hi,y);
      }
    }
  }
  if (((((((is_settler) && (game.difficulty != DIFF_CHIEFTAIN)) && (-1 < (int)city4)) &&
        ((city = city4,
         ((uint)game.human_players & 1 << (cities[city4].owner & 0x1fU)) != 0 &&
         (in_enemy_zoc == 0)))) && (1 < near_dist)) &&
      ((civs[civ].num_advances < civs[cities[city4].owner].num_advances &&
       (score = tile_get_fertility(x2,y3), 8 < score)))) &&
     (0xe - near_dist <= score)) {
    unit_found_city(unit,-1,0);
    return AI_ORDERS_RETURN_1;
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a unit in its own city: how many
   defenders the city needs (by size, walls and danger); a defender stays as
   guard while the city has too few or it is the best one, else it may leave;
   others guard a city with too few units, posting a defense goal. */
static void ai_orders_city_guard(AiUnitOrders *o)
{
  uptr quota;
  int role_count;
  int divisor;
  uptr &civ = o->civ; int &city7 = o->city7; uint &policy = o->policy;
  bool &settler_type = o->settler_type; iptr &unit = o->unit;
  uptr &needed_defenders = o->needed_defenders; int &y3 = o->y3;
  uint &ai_role = o->ai_role; int &x2 = o->x2; uptr &unit4 = o->unit4;
  int &defender_count = o->defender_count; int &is_guard = o->is_guard;
  uptr &city2 = o->city2; int &own_dist = o->own_dist;
  if (own_dist == 0) {
    role_count = unit_stack_count_role(unit,ROLE_DEFEND);
    if (((ai_role == 1) && (1 < role_count)) || (ai_role == 4)) {
      city7 = tile_zoc_unit_adjacent(x2,y3,civ);
      if (city7 == 0) {
        divisor = 4;
      }
      else {
        divisor = 3;
      }
      city7 = city_has_improvement(city2,IMP_PALACE);
      if (city7 == 0) {
        if (policy == 5) {
          divisor++;
        }
      }
      else {
        divisor--;
      }
      quota = (int)cities[city2].size / divisor + 1;
      city7 = city_has_improvement(city2,IMP_PALACE);
      needed_defenders = quota;
      if (city7 != 0) {
        unit4 = 1;
        if (3 < cities[city2].size) {
          unit4 = 2;
        }
        if (7 < civs[civ].num_cities) {
          unit4 = 3;
        }
        if (0xb < civs[civ].num_cities) {
          unit4 = 4;
        }
        needed_defenders = unit4;
        if ((int)unit4 <= (int)quota) {
          needed_defenders = quota;
        }
      }
    }
    if (ai_role == 1) {
      if (role_count < 2) {
        is_guard = 1;
      }
      else {
        defender_count = unit_stack_best_defender(unit,-1);
        if (unit == defender_count) {
          is_guard = 1;
        }
        else {
          if (role_count < (int)needed_defenders) {
            civ_add_ai_goal(civ,x2,y3,ROLE_DEFEND,3);
          }
          if ((int)needed_defenders < role_count) {
            unit4 = (uptr)units[unit].orders;
            units[unit].orders = ORDER_FORTIFIED;
            city7 = unit_stack_best_defender(unit,-1);
            if ((city7 == unit) &&
               (city7 = unit_stack_sum(unit,4), city7 <= (int)needed_defenders)) {
              units[unit].orders = (i8)unit4;
              is_guard = 1;
            }
            else {
              units[unit].orders = (i8)unit4;
              if (((game.turn + unit & 7U) == 0) &&
                 (((units[unit].orders == ORDER_SLEEP || (units[unit].orders == ORDER_FORTIFYING)
                   ) || (units[unit].orders == ORDER_FORTIFIED)))) {
                units[unit].orders = ORDER_NONE;
              }
            }
          }
          else {
            is_guard = 1;
          }
        }
      }
    }
    else {
      city7 = unit_stack_sum(unit,2);
      if ((city7 < 2) &&
         ((civ_add_ai_goal(civ,x2,y3,ROLE_DEFEND,5), !settler_type ||
          (city7 = tile_zoc_unit_adjacent(x2,y3,civ), city7 != 0)))) {
        is_guard = 1;
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a ship with no goto yet: to a
   city of its own on another landmass whose policy wants what it carries,
   else toward the shore of unsettled fertile land (a random spot or the one
   it was exploring for), else of unsettled land nearby. */
static void ai_orders_ship_goal_land(AiUnitOrders *o)
{
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  iptr &x = o->x; int &y = o->y; uint &x4 = o->x4; iptr &unit = o->unit;
  uptr &city = o->city; int &y3 = o->y3; int &x2 = o->x2; uint &flag = o->flag;
  uptr &dir2 = o->dir2; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  int &cand_score = o->cand_score; uptr &dir = o->dir; uptr &city2 = o->city2;
  int &own_dist = o->own_dist; int &best_score = o->best_score;
  int &here_landmass = o->here_landmass; uint &tasks_mask = o->tasks_mask;
  if (units[unit].orders != ORDER_GOTO) {
    best_score = 999;
    here_landmass = tile_land_landmass(x2,y3);
    for (city = 0; (int)city < (int)game.num_cities;
        city = city + 1) {
      if (((cities[city].id != 0) &&
          (civ2 = (uptr)cities[city].owner, civ2 == civ)) &&
         ((cand_score = map_distance_diag(x2,y3,
                                         (int)cities[city].x,
                                         (int)cities[city].y), 7 < cand_score
          && ((city7 = tile_landmass((int)cities[city].x,
                                      (int)cities[city].y),
              here_landmass != city7 &&
              (*(char *)&civs[civ].continent_policy[city7] != 4)))))) {
        if (own_dist == 0) {
          y = cities_share_ocean(city2,city);
        }
        else {
          y = city_adjacent_to_body(city,landmass);
        }
        if (y != 0) {
          if ((tasks_mask &
              1 << (civs[civ].continent_policy[city7] & 0x1f)) == 0) {
            cand_score = cand_score << 1;
          }
          if (cand_score < best_score) {
            best_score = cand_score;
            unit_set_goto(unit,0x35,(int)cities[city].x,
                               (int)cities[city].y);
          }
        }
      }
    }
  }
  if ((units[unit].orders != ORDER_GOTO) &&
     (unit_types[units[unit].type].hold != '\0')) {
    if (units[unit].ai_task == 'V') {
      x4 = (uint)units[unit].goto_x;
      y2 = (uint)units[unit].goto_y;
    }
    else {
      y2 = rand_range(0,map.height - 1);
      x4 = rand_range(0,map.width - 1);
      if ((x4 & 1) != 0) {
        x4--;
      }
      if ((y2 & 1) != 0) {
        x4++;
      }
    }
    city7 = tile_land_landmass(x4,y2);
    here_landmass = tile_land_landmass(x2,y3);
    if ((((0 < city7) && (city7 < 0x3f)) &&
        (0x10 < landmass_table_block.landmass_recs[city7].fertile_tiles)) &&
       ((here_landmass != city7 &&
        (*(char *)&civs[civ].continent_cities[city7] == '\0')))) {
      flag = 1;
      if (own_dist == 0) {
        city7 = ai_city_touches_landmass(city2,city7);
        if (city7 == 0) {
          flag = 0;
        }
      }
      else {
        city7 = landmass_borders_body(city7,landmass);
        if (city7 == 0) {
          flag = 0;
        }
      }
      if (flag != 0) {
        ai_ship_goto_near_shore(unit,0x56,x4,y2);
      }
    }
  }
  if (units[unit].orders != ORDER_GOTO) {
    for (dir = 2; (int)dir < 0x18; dir++) {
      dir2 = rand_range(0,7);
      x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir2] * dir +
                          x2);
      y2 = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir2] * dir + y3;
      city7 = map_in_bounds(x,y2);
      if (((city7 != 0) && (1 < (int)y2)) &&
         (((int)y2 <= map.height - 3 &&
          ((city7 = tile_is_ocean(x,y2), city7 == 0 &&
           (city7 = tile_landmass(x,y2),
           *(char *)&civs[civ].continent_cities[city7] == '\0')))))) {
        if (own_dist == 0) {
          city7 = ai_city_touches_landmass(city2,city7);
        }
        else {
          city7 = landmass_borders_body(city7,landmass);
        }
        if (city7 != 0) {
          ai_ship_goto_near_shore(unit,0x36,x,y2);
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a ship carrying a trade unit,
   with no goto yet: to the city overseas worth most for a trade route from
   the trade unit's home (by trade, treaty and distance). */
static void ai_orders_ship_goal_trade(AiUnitOrders *o)
{
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  iptr &x = o->x; iptr &unit2 = o->unit2; iptr &unit = o->unit;
  uptr &city = o->city; int &y3 = o->y3; int &x2 = o->x2;
  iptr &unit5 = o->unit5; uptr &civ2 = o->civ2;
  int &cand_score = o->cand_score; uptr &city5 = o->city5;
  uptr &city2 = o->city2; int &own_dist = o->own_dist;
  int &best_score = o->best_score; int &here_landmass = o->here_landmass;
  uint &tasks_mask = o->tasks_mask;
  if (((units[unit2].orders != ORDER_GOTO) && ((tasks_mask & 0x80) != 0)) &&
     (unit_types[units[unit2].type].hold != '\0')) {
    best_score = -999;
    here_landmass = tile_land_landmass(x2,y3);
    city5 = 0xffffffff;
    unit5 = unit2;
    for (unit = unit_stack_first(unit2); x = unit5, -1 < unit;
        unit = unit_next_in_stack(unit)) {
      if ((unit_types[units[unit].type].role == ROLE_TRADE) &&
         (units[unit].home_city != -1)) {
        city5 = (uptr)(byte)units[unit].home_city;
      }
    }
    unit = unit5;
    if (((int)city5 < 0) &&
       (city5 = city_find_nearest(x2,y3,civ,0xffffffff,-1),
       (int)city5 < 0)) {
      city5 = 0;
    }
    for (city = 0; (int)city < (int)game.num_cities;
        city = city + 1) {
      if ((cities[city].id != 0) &&
         (city7 = tile_landmass((int)cities[city].x,(int)cities[city].y
                                ), here_landmass != city7)) {
        if (own_dist == 0) {
          if ((int)cities[city].owner == civ) {
            city7 = cities_share_ocean(city2,city);
          }
          else {
            city7 = ai_city_touches_landmass(city2,city7);
          }
        }
        else if ((int)cities[city].owner == civ) {
          city7 = city_adjacent_to_body(city,landmass);
        }
        else {
          city7 = landmass_borders_body(city7,landmass);
        }
        if (city7 != 0) {
          cand_score = map_distance_diag((int)cities[city5].x,
                                        (int)cities[city5].y,
                                        (int)cities[city].x,
                                        (int)cities[city].y);
          if ((int)cities[city].owner == civ) {
            cand_score = cand_score / 2;
          }
          if (((byte)(civs[civ].treaties[cities[city].owner] >> 8) &
              (TREATY_WAR >> 8)) != 0) {
            cand_score -= cand_score / 3;
          }
          if ((*(byte *)&civs[civ].treaties[cities[city].owner] &
              TREATY_ALLIANCE) != 0) {
            cand_score += cand_score / 3;
          }
          if (((((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)
                ) != 0) && (game.power_rank[cities[city].owner] == 7)) &&
             (4 < civs[civ2].num_cities)) {
            cand_score = cand_score / 2;
          }
          cand_score = cities[city].base_trade * cand_score;
          city7 = map_distance_diag(x2,y3,(int)cities[city].x,
                                      (int)cities[city].y);
          cand_score = cand_score / ((city7 >> 3) + 1);
          if (best_score < cand_score) {
            best_score = cand_score;
            unit_set_goto(x,0x54,(int)cities[city].x,
                               (int)cities[city].y);
          }
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a ship carrying attackers or
   defenders, with no goto yet: to the nearest city of its own on a defensive
   landmass (more so if undefended), else to an undefended city of a civ at
   war (more so one a human ally fights), else to the nearest enemy city on
   another landmass (by treaty and the war of its ally). */
static void ai_orders_ship_goal_war(AiUnitOrders *o)
{
  uptr ally_civ;
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  iptr &unit2 = o->unit2; int &civ4 = o->civ4; uptr &city = o->city;
  int &y3 = o->y3; int &x2 = o->x2; uptr &civ2 = o->civ2;
  int &cand_score = o->cand_score; uptr &city2 = o->city2;
  int &own_dist = o->own_dist; int &best_score = o->best_score;
  int &here_landmass = o->here_landmass; uint &tasks_mask = o->tasks_mask;
  if ((units[unit2].orders != ORDER_GOTO) &&
     (((tasks_mask & 3) != 0 && (unit_types[units[unit2].type].hold != '\0')))) {
    best_score = 0x18;
    for (city = 0; (int)city < (int)game.num_cities;
        city = city + 1) {
      if (((cities[city].id != 0) && ((int)cities[city].owner == civ))
         && (city7 = tile_landmass((int)cities[city].x,
                                    (int)cities[city].y),
            *(char *)&civs[civ].continent_policy[city7] == 1)) {
        if (own_dist == 0) {
          city7 = cities_share_ocean(city2,city);
        }
        else {
          city7 = city_adjacent_to_body(city,landmass);
        }
        if (city7 != 0) {
          cand_score = map_distance_diag(x2,y3,(int)cities[city].x
                                        ,(int)cities[city].y);
          city7 = tile_unit_owner((int)cities[city].x,
                                   (int)cities[city].y);
          if (city7 < 0) {
            cand_score = cand_score / 2;
          }
          else {
            city7 = civ_ai_goal_get_priority(civ,(int)cities[city].x,
                                        (int)cities[city].y,ROLE_DEFEND);
            if (city7 != 0) {
              cand_score -= cand_score / 3;
            }
          }
          if (cand_score < best_score) {
            best_score = cand_score;
            unit_set_goto(unit2,0x41,(int)cities[city].x,
                               (int)cities[city].y);
          }
        }
      }
    }
  }
  if ((units[unit2].orders != ORDER_GOTO) && ((tasks_mask & 3) != 0)) {
    best_score = 999;
    for (city = 0; (int)city < (int)game.num_cities;
        city = city + 1) {
      if (((cities[city].id != 0) &&
          (civ2 = (uptr)cities[city].owner, civ2 != civ)) &&
         ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0 &&
          (city7 = tile_unit_owner((int)cities[city].x,
                                    (int)cities[city].y), city7 < 0)))) {
        tile_landmass((int)cities[city].x,(int)cities[city].y);
        if (own_dist == 0) {
          city7 = cities_share_ocean(city2,city);
        }
        else {
          city7 = city_adjacent_to_body(city,landmass);
        }
        if (city7 != 0) {
          cand_score = map_distance_diag(x2,y3,(int)cities[city].x
                                        ,(int)cities[city].y);
          for (civ4 = 1; civ4 < 8; civ4++) {
            if ((((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0)
                && ((*(byte *)&civs[civ].treaties[civ4] & TREATY_ALLIANCE) != 0
                   )) && (((byte)(civs[civ4].treaties[civ2] >> 8)
                          & (TREATY_WAR >> 8)) != 0)) {
              cand_score = cand_score / 2;
              break;
            }
          }
          if ((cities[city].flags & CITYF_COASTAL_FORT_USED) != 0) {
            cand_score = cand_score << 1;
          }
          if (cand_score < best_score) {
            best_score = cand_score;
            units[unit2].counter = (byte)city & 7;
            unit_set_goto(unit2,0x49,(int)cities[city].x,
                               (int)cities[city].y);
          }
        }
      }
    }
  }
  if (units[unit2].orders != ORDER_GOTO) {
    ally_civ = 0xffffffff;
    for (civ2 = 0; (int)civ2 < 8; civ2++) {
      if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) &&
         (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0 ||
          ((int)ally_civ < 0)))) {
        ally_civ = civ2;
      }
    }
    if (((tasks_mask & 1) != 0) || ((tasks_mask & 0x20) == 0)) {
      best_score = 999;
      here_landmass = tile_land_landmass(x2,y3);
      for (city = (int)(char)units[unit2].counter & 7;
          (int)city < (int)game.num_cities; city += 8) {
        if ((((((cities[city].id != 0) &&
               (civ2 = (uptr)cities[city].owner, civ2 != civ)) &&
              ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) &&
             (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0 ||
              ((*(byte *)&civs[civ].treaties[civ2] & TREATY_HOSTILE_INTENT) != 0)))
             ) && ((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f))
                    == 0 || (game.civ_by_rank[7] == civ)))) &&
           (city7 = tile_landmass((int)cities[city].x,
                                   (int)cities[city].y), here_landmass != city7))
        {
          if (own_dist == 0) {
            city7 = ai_city_touches_landmass(city2,city7);
          }
          else {
            city7 = landmass_borders_body(city7,landmass);
          }
          if (city7 != 0) {
            cand_score = map_distance_diag(x2,y3,
                                          (int)cities[city].x,
                                          (int)cities[city].y);
            if ((0 < (int)ally_civ) &&
               (((byte)(civs[ally_civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) !=
                0)) {
              cand_score = cand_score / 2;
            }
            if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) != 0) {
              cand_score = cand_score / 2;
            }
            if ((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0)
               && (city7 = tile_unit_owner((int)cities[city].x,
                                            (int)cities[city].y), city7 < 0)
               ) {
              cand_score = cand_score / 2;
            }
            if ((cities[city].flags & CITYF_COASTAL_FORT_USED) != 0) {
              cand_score = cand_score << 1;
            }
            if (cand_score < best_score) {
              best_score = cand_score;
              unit_set_goto(unit2,0x34,(int)cities[city].x,
                                 (int)cities[city].y);
            }
          }
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a transport that may unload: for
   each land tile next to it, whether its cargo is wanted there (a landmass
   short of defense or unexplored, a trade route worth starting, a hostile
   city, damaged units): it lands the cargo (or first moves to a better
   landing spot); a slow transport on a long trip posts a goal halfway for an
   escort. */
static int ai_orders_transport_unload(AiUnitOrders *o)
{
  bool ally_near;
  bool trade_drop;
  i32 explore_flag;
  ushort &military = o->military; uptr &civ = o->civ; i32 &result = o->result;
  int &city7 = o->city7; uptr &city3 = o->city3; iptr &x = o->x;
  iptr &x5 = o->x5; int &y = o->y; int &near_dist = o->near_dist;
  uint &policy = o->policy; bool &cond = o->cond; iptr &unit2 = o->unit2;
  uint &x4 = o->x4; int &goal_match = o->goal_match;
  int &near_landmass = o->near_landmass; int &y3 = o->y3;
  int &needs_units = o->needs_units; uint &damage_level = o->damage_level;
  int &x2 = o->x2; uptr &unit4 = o->unit4; uptr &best_dir = o->best_dir;
  uint &room_aboard = o->room_aboard; uptr &city4 = o->city4;
  uptr &dir2 = o->dir2; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  uptr &dir = o->dir; uptr &city5 = o->city5; uptr &city2 = o->city2;
  int &own_dist = o->own_dist; uint &tasks_mask = o->tasks_mask;
  cond = tasks_mask == 0x80;
  for (dir = 0; (int)dir < 8; dir++) {
    x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
    city7 = map_in_bounds(x,y2);
    if (((city7 != 0) && (city7 = tile_is_ocean(x,y2), city7 == 0)
        ) && ((civ2 = tile_unit_owner(x,y2), (int)civ2 < 0 ||
              (civ2 == civ)))) {
      if (civ2 == civ) {
        result = 2;
        x5 = tile_first_unit(x,y2);
        city7 = unit_stack_sum(x5,result);
        if (1 < city7) continue;
      }
      ally_near = false;
      if (((-1 < (int)city4) && ((int)cities[city4].owner != civ)) &&
         (((*(byte *)&civs[civ].treaties[cities[city4].owner] & 4
           ) != 0 && (near_dist < 4)))) {
        ally_near = true;
      }
      city7 = tile_landmass(x,y2);
      city3 = city4;
      needs_units = 0;
      trade_drop = false;
      explore_flag = 0;
      if ((tasks_mask & 0x80) == 0) {
        if (ally_near) continue;
      }
      else if (city7 == near_landmass) {
        city5 = 0xffffffff;
        for (unit4 = unit_stack_first(unit2); -1 < (int)unit4;
            unit4 = unit_next_in_stack(unit4)) {
          if (unit_types[units[unit4].type].role == ROLE_TRADE) {
            if (units[unit4].home_city == -1) {
              city5 = 0xffffffff;
            }
            else {
              city5 = (uptr)(byte)units[unit4].home_city;
            }
          }
        }
        if ((int)city5 < 0) {
          city5 = city_find_nearest(x,y2,civ,0xffffffff,-1);
        }
        if ((int)city5 < 1) {
          city5 = 0;
        }
        trade_drop = units[unit2].ai_task != '3';
        y = tile_landmass((int)cities[city5].x,(int)cities[city5].y);
        if ((y == near_landmass) &&
           (y = map_distance_diag((int)cities[city3].x,
                                        (int)cities[city3].y,
                                        (int)cities[city5].x,
                                        (int)cities[city5].y), y < 0x10)
           ) {
          trade_drop = false;
        }
        if ((int)cities[city3].owner == civ) {
          if (cities[city3].size < 8) {
            trade_drop = false;
          }
          y = map_distance_diag((int)cities[city3].x,
                                      (int)cities[city3].y,
                                      (int)cities[city5].x,
                                      (int)cities[city5].y);
          if (y < 0x10) {
            trade_drop = false;
          }
        }
        for (dir2 = 0;
            (int)dir2 < (int)(char)cities[city5].trade_routes;
            dir2 = dir2 + 1) {
          if ((int)cities[city5].route_partner[dir2] ==
              city3) {
            trade_drop = false;
          }
        }
      }
      military = civs[civ].continent_military[city7];
      y = ai_continent_defense_quota(civ,city7);
      if (((int)(uint)military < y) &&
         (*(char *)&civs[civ].continent_policy[city7] != 4)) {
        needs_units = 1;
      }
      if ((*(char *)&civs[civ].continent_cities[city7] == '\0') &&
         (civs[civ].continent_military[city7] < 8)) {
        needs_units = 1;
      }
      if ((1 << (civs[civ].continent_policy[city7] & 0x1f) & tasks_mask
          & 3) != 0) {
        needs_units = 1;
      }
      if (((((tasks_mask & 1) != 0) && (-1 < (int)city4)) &&
          (city2 != city4)) &&
         (((10 < own_dist && (near_dist < 4)) &&
          ((city7 == near_landmass &&
           ((((uint)game.human_players & 1 << (cities[city4].owner & 0x1fU))
             != 0 && (((byte)(civs[civ].treaties[cities[city4].owner] >> 8) & (TREATY_WAR >> 8)) != 0))))))))
      {
        needs_units = 1;
      }
      if (((units[unit2].orders == ORDER_GOTO) &&
          (y = tile_landmass((int)units[unit2].goto_x,
                                  (int)units[unit2].goto_y), y == city7))
         && (units[unit2].ai_task != '3')) {
        needs_units = 1;
        goal_match = 1;
      }
      if (((tasks_mask & 0x20) == 0) &&
         (*(char *)&civs[civ].continent_flags[city7] == '\0')) {
        if ((*(short *)&civs[civ].continent_military[city7] == 0) &&
           (0x31 < landmass_table_block.landmass_recs[city7].tiles)) {
          civs[civ].continent_flags[city7] =
               civs[civ].continent_flags[city7] | 0x80;
          explore_flag = 1;
        }
        else {
          needs_units = 0;
        }
      }
      if ((room_aboard != 0) &&
         ((civs[civ].continent_flags[city7] & 5) != 0)) {
        needs_units = 0;
      }
      if (1 < (int)damage_level) {
        needs_units = 1;
      }
      if ((((room_aboard == 0) &&
           (y = tile_city_owner(x,y2), -1 < y)) &&
          (civ2 != civ)) &&
         (y = diplo_is_hostile(civ,civ2), y != 0)) {
        needs_units = 1;
      }
      if (cond) {
        needs_units = 0;
      }
      if (goal_match != 0) {
        if (cond) {
          if (!trade_drop) {
            units[unit2].counter = (byte)game.turn & 7;
          }
        }
        else if (needs_units == 0) {
          units[unit2].counter = (byte)game.turn & 7;
        }
      }
      if (((needs_units != 0) && (4 < landmass_table_block.landmass_recs[city7].fertile_tiles))
         && ((1 < (int)y2 && ((int)y2 < map.height - 2)))) {
        y = unit_moves_left(unit2);
        if ((((int)(uint)cosmic.road_move_mult <= y) &&
            ((near_dist != 0 || (policy != 1)))) &&
           ((best_dir = ai_ship_best_landing_dir(unit2), -1 < (int)best_dir &&
            (best_dir != 8)))) {
          units[unit2].ai_task = 'b';
          city3 = best_dir;
          return AI_ORDERS_APPLY_DIR;
        }
        if (*(char *)&civs[civ].continent_policy[city7] == '\0') {
          civ_add_ai_goal(civ,x2,y3,ROLE_NAVAL_SUPERIORITY,5);
        }
        units[unit2].ai_task = 'U';
        unit_exhaust_moves(unit2);
        units[unit2].orders = ORDER_NONE;
        if (!trade_drop) {
          tasks_mask = tasks_mask & 0xffffff7f;
        }
        unit_stack_wake_by_role(unit2,tasks_mask,explore_flag);
        return AI_ORDERS_FINISH_ORDERS;
      }
      if (((trade_drop) && (1 < (int)y2)) && ((int)y2 < map.height - 2)
         ) {
        if (*(char *)&civs[civ].continent_policy[city7] == '\0') {
          civ_add_ai_goal(civ,x2,y3,ROLE_NAVAL_SUPERIORITY,5);
        }
        units[unit2].ai_task = 'u';
        unit_exhaust_moves(unit2);
        tasks_mask = 0x80;
        if (!ally_near) {
          tasks_mask = 0x82;
        }
        unit_stack_wake_by_role(unit2,tasks_mask,explore_flag);
        units[unit2].orders = ORDER_NONE;
        return AI_ORDERS_FINISH_ORDERS;
      }
      if ((game.turn & 3U) == 0) {
        units[unit2].orders = ORDER_NONE;
      }
    }
  }
  city7 = unit_moves_left(unit2);
  if ((city7 <= (int)(uint)cosmic.road_move_mult) &&
     (units[unit2].orders == ORDER_GOTO)) {
    x4 = units[unit2].goto_x + x2 >> 1;
    y2 = units[unit2].goto_y + y3 >> 1;
    if ((x4 & 1) != 0) {
      x4--;
    }
    if ((y2 & 1) != 0) {
      x4++;
    }
    civ_add_ai_goal(civ,x4,y2,ROLE_NAVAL_SUPERIORITY,3);
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a transport with too little
   aboard: picks the port city of its own to gather units at (one whose
   landmass needs units, near, with no other transport) and posts goals there
   for the units it needs; or, in its own city, gathers units there and stays.
   */
static int ai_orders_transport_port(AiUnitOrders *o)
{
  iptr dest_landmass;
  int cargo_target;
  int trade_target;
  ushort &military = o->military; uptr &civ = o->civ; i32 &result = o->result;
  iptr &landmass = o->landmass; int &city7 = o->city7; uptr &city3 = o->city3;
  iptr &x = o->x; int &near_dist = o->near_dist; uint &policy = o->policy;
  iptr &unit2 = o->unit2; uptr &city6 = o->city6; uptr &city = o->city;
  int &y3 = o->y3; int &needs_units = o->needs_units; int &x2 = o->x2;
  uptr &unit4 = o->unit4; uptr &best_dir = o->best_dir; uint &flag = o->flag;
  uint &room_aboard = o->room_aboard; int &cand_score = o->cand_score;
  iptr &unit3 = o->unit3; uptr &city2 = o->city2; int &own_dist = o->own_dist;
  int &best_score = o->best_score; int &freed_count = o->freed_count;
  uint &tasks_mask = o->tasks_mask;
  uint &target_policy = o->target_policy;
  if ((near_dist == 0) || (unit4 != 0)) {
    cargo_target = (int)(char)unit_types[units[unit2].type].hold;
    if (2 < cargo_target) {
      cargo_target = 3;
    }
  }
  else {
    cargo_target = 1;
  }
  if ((freed_count < cargo_target) || (room_aboard != 0)) {
    flag = room_aboard;
    best_score = 999;
    trade_target = 0;
    for (city = 0; (int)city < (int)game.num_cities;
        city = city + 1) {
      if ((((cities[city].id != 0) &&
           ((int)cities[city].owner == civ)) &&
          ((cities[city].flags & CITYF_COASTAL) != 0)) &&
         (((cities[city].flags & CITYF_AI_FERRY_PORT) == 0 ||
          (((char)units[unit2].counter < '\0' &&
           ((int)(char)units[unit2].counter == -1 - (city & 0x3f))))))) {
        dest_landmass = tile_landmass((int)cities[city].x,(int)cities[city].y
                              );
        if (own_dist == 0) {
          city7 = cities_share_ocean(city2,city);
        }
        else {
          city7 = city_adjacent_to_body(city,landmass);
        }
        if (city7 != 0) {
          needs_units = 0;
          military = civs[civ].continent_military[dest_landmass];
          city7 = ai_continent_defense_quota(civ,dest_landmass);
          if (((int)(uint)military < city7) &&
             (*(char *)&civs[civ].continent_policy[dest_landmass] != 4)) {
            if (*(char *)&civs[civ].continent_trade_units[dest_landmass] == '\0')
            continue;
            needs_units = 1;
          }
          cand_score = map_distance_diag(x2,y3,
                                        (int)cities[city].x,
                                        (int)cities[city].y);
          unit3 = tile_first_unit((int)cities[city].x,
                                        (int)cities[city].y);
          if ((-1 < unit3) &&
             (city7 = unit_stack_count_role(unit3,ROLE_SEA_TRANSPORT),
             (int)(uint)(cand_score == 0) < city7)) {
            cand_score += 0x10;
          }
          if ((*(char *)&civs[civ].continent_policy[dest_landmass] == 4) ||
             (*(char *)&civs[civ].continent_flags[dest_landmass] == '\0')) {
            cand_score = cand_score -
                       ((int)(uint)*(ushort *)
                                    (dest_landmass * 2 + (iptr)((u8 *)&civs + 530) + civ * 0x594) >> 2
                       );
          }
          else {
            cand_score += 0x10;
          }
          if (cand_score < best_score) {
            best_score = cand_score;
            city6 = city;
            trade_target = needs_units;
            target_policy = (uint)civs[civ].continent_policy[dest_landmass];
            unit_set_goto(unit2,0x33,(int)cities[city].x,
                               (int)cities[city].y);
          }
        }
      }
    }
    if (((near_dist == 0) &&
        ((((int)(char)units[unit2].counter ^ (int)game.turn - 1U & 3) & 3) != 0)
        ) && (((tasks_mask & 0x20) == 0 ||
              ((tasks_mask == 0x20 ||
               (freed_count < (char)unit_types[units[unit2].type].hold)))))) {
      best_score = 1000;
    }
    if (target_policy == 4) {
      cand_score = 4;
    }
    else {
      cand_score = 2;
    }
    if (best_score < cand_score * 3) {
      units[unit2].counter = 0xff - ((byte)city6 & 0x3f);
      cities[city6].flags = cities[city6].flags | (CITYF_AI_FERRY_PORT | CITYF_AI_FERRY_PORT_FRESH);
      if (trade_target == 0) {
        result = 5;
        x = tile_first_unit((int)units[unit2].goto_x,
                                    (int)units[unit2].goto_y);
        city7 = unit_stack_count_role(x,result);
        if (city7 == 0) {
          if (target_policy != 5) {
            civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                               (int)units[unit2].goto_y,ROLE_SETTLE,cand_score);
          }
          if (target_policy != 5) {
            civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                               (int)units[unit2].goto_y,0x15,cand_score - 1);
          }
        }
        if (target_policy != 1) {
          civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                             (int)units[unit2].goto_y,ROLE_DEFEND,cand_score);
        }
        if (target_policy != 0) {
          civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                             (int)units[unit2].goto_y,ROLE_ATTACK,cand_score);
        }
        if ((target_policy == 4) || (target_policy == 5)) {
          civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                             (int)units[unit2].goto_y,ROLE_DIPLOMACY,cand_score);
        }
      }
      civ_add_ai_goal(civ,(int)units[unit2].goto_x,(int)units[unit2].goto_y
                         ,ROLE_TRADE,2);
    }
    else if (((own_dist == 0) &&
             (city7 = unit_stack_count_role(unit2,ROLE_SEA_TRANSPORT), city7 < 2)) &&
            (((city7 = rand_range(0,3), city7 != 0 ||
              (best_score == 1000)) &&
             ((civs[civ].continent_flags[landmass] & 0x10) == 0)))) {
      military = civs[civ].continent_military[landmass];
      city7 = ai_continent_defense_quota(civ,landmass);
      city3 = city2;
      if ((city7 <= (int)(uint)military) || (policy == 4)) {
        needs_units = 1;
      }
      if (*(char *)&civs[civ].continent_trade_units[landmass] != '\0') {
        trade_target = 1;
      }
      if ((needs_units != 0) || (trade_target != 0)) {
        cities[city2].flags = cities[city2].flags | (CITYF_AI_FERRY_PORT | CITYF_AI_FERRY_PORT_FRESH);
        if (needs_units != 0) {
          result = 5;
          x = tile_first_unit((int)cities[city2].x,
                                      (int)cities[city2].y);
          near_dist = unit_stack_count_role(x,result);
          if ((near_dist == 0) && (policy != 5)) {
            civ_add_ai_goal(civ,(int)cities[city3].x,(int)cities[city3].y
                               ,ROLE_SETTLE,cand_score);
            civ_add_ai_goal(civ,(int)cities[city3].x,(int)cities[city3].y
                               ,0x15,cand_score - 1);
          }
          if (policy != 1) {
            civ_add_ai_goal(civ,(int)cities[city3].x,(int)cities[city3].y
                               ,ROLE_DEFEND,cand_score);
          }
          if (policy != 0) {
            civ_add_ai_goal(civ,(int)cities[city3].x,(int)cities[city3].y
                               ,ROLE_ATTACK,cand_score);
          }
          if ((policy == 4) || (policy == 5)) {
            civ_add_ai_goal(civ,(int)units[unit2].goto_x,
                               (int)units[unit2].goto_y,ROLE_DIPLOMACY,cand_score);
          }
        }
        civ_add_ai_goal(civ,(int)cities[city3].x,(int)cities[city3].y,ROLE_TRADE,2
                          );
        best_dir = 8;
        city3 = best_dir;
        return AI_ORDERS_APPLY_DIR;
      }
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a settler or defender (with
   Construction) out of its city on land, in the area of its own city: whether
   the tile is a good fortress site (a pass between sea or fortresses, or a
   road near home): fort_need 1 or 2, posted as a goal. */
static void ai_orders_fortress_site(AiUnitOrders *o)
{
  int sea_south;
  int sea_north;
  int home_dist;
  int sea_east;
  int sea_west;
  int ocean_count;
  uptr &civ = o->civ; i32 &result = o->result; int &city7 = o->city7;
  iptr &x = o->x; uint &terrain = o->terrain;
  bool &settler_type = o->settler_type; int &fort_need = o->fort_need;
  int &y3 = o->y3; uint &ai_role = o->ai_role; int &x2 = o->x2;
  uint &on_fortress = o->on_fortress; uptr &city4 = o->city4; uint &y2 = o->y2;
  uptr &dir = o->dir; uptr &city2 = o->city2; int &own_dist = o->own_dist;
  fort_need = 0;
  if (((settler_type) || (ai_role == 1)) &&
     (city7 = civ_has_advance(civ,ADV_CONSTRUCTION), city7 != 0)) {
    home_dist = own_dist;
    if (-1 < (int)city2) {
      home_dist = map_distance(x2,y3,(int)cities[city2].x,
                                    (int)cities[city2].y);
    }
    if (((own_dist != 0) && (city2 == city4)) &&
       ((on_fortress == 0 &&
        ((city7 = tile_airbase_owner(x2,y3), city7 < 0 &&
         (city7 = tile_is_ocean(x2,y3), city7 == 0)))))) {
      ocean_count = 0;
      sea_south = 0;
      sea_north = 0;
      sea_east = 0;
      sea_west = 0;
      for (dir = 0; (int)dir < 8; dir++) {
        x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
        city7 = map_in_bounds(x,y2);
        if (city7 != 0) {
          result = tile_improvements(x,y2);
          if ((((byte)result & 0x42) == 0x40) ||
             (city7 = tile_city_owner(x,y2), -1 < city7))
          return;
          city7 = tile_is_ocean(x,y2);
          if (city7 != 0) {
            ocean_count++;
            if (3 < ocean_count) return;
            if ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] < '\0') {
              sea_north = 1;
            }
            if ('\0' < (((i8 *)&dir_dy_table_block.dir_dy_table))[dir]) {
              sea_south = 1;
            }
            if ((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] < '\0') {
              sea_west = 1;
            }
            if ('\0' < (((i8 *)&dir_dx_table_block.dir_dx_table))[dir]) {
              sea_east = 1;
            }
          }
          city7 = civ_ai_goal_get_priority(civ,x,y2,ROLE_DEFEND);
          if (city7 != 0) return;
        }
      }
      terrain = tile_improvements(x2,y3);
      if (((((terrain & 0x10) != 0) && (1 < own_dist)) && (own_dist < 4)) &&
         (city2 == city4)) {
        fort_need = 1;
      }
      for (dir = 0xfffffffe; (int)dir < 3; dir++) {
        x = map_wrap_x(dir + x2);
        city7 = map_in_bounds(x,y3 - 2);
        if (((city7 != 0) && (sea_north == 0)) &&
           ((city7 = tile_is_ocean(x,y3 - 2), city7 != 0 ||
            (result = tile_improvements(x,y3 - 2),
            ((byte)result & 0x42) == 0x40)))) {
          sea_north = 2;
        }
        city7 = map_in_bounds(x,y3 + 2);
        if (((city7 != 0) && (sea_south == 0)) &&
           ((city7 = tile_is_ocean(x,y3 + 2), city7 != 0 ||
            (result = tile_improvements(x,y3 + 2),
            ((byte)result & 0x42) == 0x40)))) {
          sea_south = 2;
        }
      }
      for (dir = 0xfffffffe; (int)dir < 3; dir++) {
        y2 = dir + y3;
        x = map_wrap_x(x2 - 2);
        city7 = map_in_bounds(x,y2);
        if (((city7 != 0) && (sea_west == 0)) &&
           ((city7 = tile_is_ocean(x,y2), city7 != 0 ||
            (result = tile_improvements(x,y2),
            ((byte)result & 0x42) == 0x40)))) {
          sea_west = 2;
        }
        x = map_wrap_x(x2 + 2);
        city7 = map_in_bounds(x,y2);
        if (((city7 != 0) && (sea_east == 0)) &&
           ((city7 = tile_is_ocean(x,y2), city7 != 0 ||
            (result = tile_improvements(x,y2),
            ((byte)result & 0x42) == 0x40)))) {
          sea_east = 2;
        }
      }
      if ((1 < sea_north + sea_south) && ((sea_west == 0 || (sea_east == 0)))) {
        fort_need = 2;
      }
      if ((1 < sea_east + sea_west) && ((sea_north == 0 || (sea_south == 0)))) {
        fort_need = 2;
      }
      if (fort_need != 0) {
        if (settler_type) {
          civ_add_standing_goal(civ,x2,y3,ROLE_DEFEND,2);
        }
        else {
          civ_add_standing_goal(civ,x2,y3,0x15,
                             (uint)(home_dist < 4) + fort_need);
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a terraforming unit (role 0x15)
   on land with no goto: back near its home city if far; else builds a road,
   irrigation, a mine, a railroad or clears pollution where it stands near
   home, or goes to the first tile of the home city that needs one. */
static int ai_orders_terraformer(AiUnitOrders *o)
{
  Tile *&map_tile = o->map_tile; byte &tile_byte = o->tile_byte;
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &city3 = o->city3; iptr &x = o->x; uint &terrain = o->terrain;
  byte *&tile = o->tile; iptr &unit = o->unit;
  int &in_enemy_zoc = o->in_enemy_zoc; uptr &city = o->city; int &y3 = o->y3;
  int &x2 = o->x2; uptr &terrain2 = o->terrain2; uint &y2 = o->y2;
  uptr &dir = o->dir; uptr &city2 = o->city2; int &own_dist = o->own_dist;
  uint &tile_flags = o->tile_flags;
  if (units[unit].home_city == -1) {
    city = city2;
  }
  else {
    city = (uptr)(byte)units[unit].home_city;
  }
  if ((-1 < (int)city) &&
     (city7 = tile_landmass((int)cities[city].x,(int)cities[city].y),
     city7 != landmass)) {
    city = city2;
  }
  if ((-1 < (int)city) &&
     (city7 = map_distance_diag(x2,y3,(int)cities[city].x,
                                  (int)cities[city].y), 4 < city7)) {
    unit_set_goto(unit,99,(int)cities[city].x,
                       (int)cities[city].y);
    return AI_ORDERS_FINISH_ORDERS;
  }
  terrain = tile_improvements(x2,y3);
  if (((terrain & 0x10) == 0) &&
     ((tile = (byte *)tile_at(x2,y3), (*tile & 0x80) == 0 ||
      (city7 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), city7 != 0)))) {
    units[unit].ai_task = 'r';
    units[unit].orders = ORDER_BUILD_ROAD;
    return AI_ORDERS_FINISH_ORDERS;
  }
  if ((-1 < (int)city2) && (in_enemy_zoc != 0)) {
    unit_set_goto(unit,99,(int)cities[city2].x,(int)cities[city2].y)
    ;
    return AI_ORDERS_FINISH_ORDERS;
  }
  if (own_dist < 3) {
    map_tile = tile_at(x2,y3);
    tile_byte = map_tile->improvements;
    tile_flags = (uint)tile_byte;
    if ((tile_byte & 0xe) == 0) {
      if ((terrain_types[terrain2].mine_ai_govt != '\0') &&
         ((char)terrain_types[terrain2].mine_ai_govt <
          (char)terrain_types[terrain2].irrigate_ai_govt)) {
        units[unit].orders = ORDER_MINE;
        return AI_ORDERS_FINISH_ORDERS;
      }
      if (terrain_types[terrain2].irrigate_ai_govt != '\0') {
        units[unit].orders = ORDER_IRRIGATE;
        return AI_ORDERS_FINISH_ORDERS;
      }
    }
    if ((tile_byte & 0x80) != 0) {
      units[unit].orders = ORDER_CLEAN_POLLUTION;
      return AI_ORDERS_FINISH_ORDERS;
    }
    terrain = tile_improvements(x2,y3);
    if (((terrain & 0x20) == 0) &&
       (city7 = civ_has_advance(civ,ADV_RAILROAD), city7 != 0)) {
      units[unit].ai_task = 'r';
      units[unit].orders = ORDER_BUILD_ROAD;
      return AI_ORDERS_FINISH_ORDERS;
    }
    city3 = city2;
    for (dir = 0; (int)dir < 0x14; dir++) {
      x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[dir] +
                          (int)cities[city3].x);
      y2 = (int)cities[city3].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[dir];
      city7 = map_in_bounds(x,y2);
      if (city7 != 0) {
        tile_byte = tile_terrain(x,y2);
        map_tile = tile_at(x,y2);
        tile_flags = (uint)map_tile->improvements;
        if ((map_tile->improvements & 0xe) == 0) {
          if ((terrain_types[tile_byte].mine_ai_govt != '\0') &&
             ((char)terrain_types[tile_byte].mine_ai_govt <
              (char)terrain_types[tile_byte].irrigate_ai_govt)) {
            unit_set_goto(unit,0x6d,x,y2);
            return AI_ORDERS_FINISH_ORDERS;
          }
          if (terrain_types[tile_byte].irrigate_ai_govt != '\0') {
            unit_set_goto(unit,0x69,x,y2);
            return AI_ORDERS_FINISH_ORDERS;
          }
        }
        tile = (byte *)tile_at(x,y2);
        if ((((*tile & 0x80) == 0) ||
            (city7 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), city7 != 0)) &&
           ((terrain = tile_improvements(x,y2), (terrain & 0x10) == 0 ||
            ((terrain = tile_improvements(x,y2), (terrain & 0x20) == 0 &&
             (city7 = civ_has_advance(civ,ADV_RAILROAD), city7 != 0)))))) {
          unit_set_goto(unit,0x72,x,y2);
          return AI_ORDERS_FINISH_ORDERS;
        }
      }
    }
  }
  else {
    terrain = tile_improvements(x2,y3);
    if (((terrain & 0x20) == 0) &&
       (city7 = civ_has_advance(civ,ADV_RAILROAD), city7 != 0)) {
      units[unit].ai_task = 'r';
      units[unit].orders = ORDER_BUILD_ROAD;
      return AI_ORDERS_FINISH_ORDERS;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a settler on land outside a city:
   posts a defense goal when alone, clears pollution, builds a fortress at a
   good site, irrigates or mines worthwhile tiles near its city, builds a road
   or railroad, or moves to a tile near home worth improving; keeps a goto to
   settle while its landmass has room. */
static int ai_orders_settler_work(AiUnitOrders *o)
{
  uptr here_imps;
  uptr goto_imps;
  uint work_flags;
  Tile *&map_tile = o->map_tile; byte &tile_byte = o->tile_byte;
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &city3 = o->city3; iptr &x = o->x; byte &owner_byte = o->owner_byte;
  uint &terrain = o->terrain; byte *&tile = o->tile; uint &policy = o->policy;
  bool &is_settler = o->is_settler; iptr &unit = o->unit;
  int &fort_need = o->fort_need; int &y3 = o->y3; int &x2 = o->x2;
  uptr &unit4 = o->unit4; uptr &best_dir = o->best_dir; uint &flag = o->flag;
  uptr &terrain2 = o->terrain2; uptr &dir2 = o->dir2; uint &y2 = o->y2;
  uptr &dir = o->dir; iptr &unit3 = o->unit3; uptr &city2 = o->city2;
  int &own_dist = o->own_dist; uint &tile_flags = o->tile_flags;
  city7 = unit_stack_sum(unit,2);
  flag = (uint)(1 < city7);
  if (flag == 0) {
    for (dir = 0; (int)dir < 8; dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
      city7 = map_in_bounds(x,y2);
      if ((city7 != 0) && (city7 = tile_unit_owner(x,y2), -1 < city7))
      {
        flag = 1;
        break;
      }
    }
  }
  if (flag == 0) {
    civ_add_ai_goal(civ,x2,y3,ROLE_DEFEND,2);
  }
  civ_clear_standing_goals_near(civ,0x15,x2,y3,0);
  here_imps = tile_improvements(x2,y3);
  if ((here_imps & TILE_POLLUTION) != 0) {
    units[unit].orders = ORDER_CLEAN_POLLUTION;
    units[unit].ai_task = 'p';
    return AI_ORDERS_FINISH_ORDERS;
  }
  if (((((1 << (owner_byte & 0x1f) & (uint)game.human_players) == 0) ||
       (units[unit].orders != ORDER_GOTO)) ||
      (city7 = map_in_bounds((int)units[unit].goto_x,
                              (int)units[unit].goto_y), city7 == 0)) ||
     (goto_imps = tile_improvements((int)units[unit].goto_x,
                                 (int)units[unit].goto_y), (goto_imps & TILE_POLLUTION) == 0
     )) {
    if (((fort_need == 2) && ((1 << (owner_byte & 0x1f) & (uint)game.human_players) == 0))
       && ((city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 == 0 ||
           (city7 = unit_stack_sum(unit,2), 1 < city7)))) {
      units[unit].orders = ORDER_BUILD_FORTRESS;
      units[unit].ai_task = 'X';
      return AI_ORDERS_FINISH_ORDERS;
    }
    work_flags = 0;
    if ((is_settler) && ((units[unit].flags & UNITF_GOAL_REACHED) != 0)) {
      work_flags = 1;
    }
    else {
      city7 = civ_ai_goal_get_priority(civ,x2,y3,0x15);
      if (city7 != 0) {
        work_flags = 1;
      }
    }
    if ((((2 < own_dist) || ((cities[city2].flags & CITYF_AI_SETTLER_NEAR) == 0)) || (!is_settler)) ||
       (units[unit].orders != ORDER_GOTO)) {
      if ((((own_dist != 0) && (own_dist < 3)) &&
          ((((int)(char)terrain_types[terrain2].irrigate_turns / 5) * 3 - 3 <=
            (int)cities[city2].size ||
           (city7 = tile_resource(x2,y3), city7 != 0)))) &&
         ((city7 = ai_tile_improve_choice(civ,x2,y3), city7 != 0 ||
          (policy == 4)))) {
        work_flags = work_flags | 2;
      }
      if (work_flags != 0) {
        if ((work_flags & 2) != 0) {
          city7 = ai_tile_improve_choice(civ,x2,y3);
          if (city7 == 1) {
            units[unit].orders = ORDER_IRRIGATE;
            units[unit].ai_task = 'I';
            return AI_ORDERS_FINISH_ORDERS;
          }
          if (city7 == 2) {
            units[unit].orders = ORDER_MINE;
            units[unit].ai_task = 'M';
            return AI_ORDERS_FINISH_ORDERS;
          }
        }
        map_tile = tile_at(x2,y3);
        tile_byte = map_tile->improvements;
        tile_flags = (uint)tile_byte;
        if (((tile_byte & 0x10) == 0) || ((tile_byte & 0x20) != 0)) {
          if (((tile_byte & 0x10) == 0) &&
             ((tile = (byte *)tile_at(x2,y3), (*tile & 0x80) == 0
              || (city7 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), city7 != 0)))) {
            work_flags = work_flags & 1;
            if (((tile_flags & 0xc) != 0) &&
               ((tile = (byte *)tile_at(x2,y3),
                (*tile & 0x80) != 0 || ((int)terrain2 < 3)))) {
              work_flags = work_flags | 2;
            }
            if (work_flags != 0) {
              units[unit].orders = ORDER_BUILD_ROAD;
              units[unit].ai_task = 'r';
              return AI_ORDERS_FINISH_ORDERS;
            }
          }
        }
        else {
          city7 = civ_has_advance(civ,ADV_RAILROAD);
          if (city7 != 0) {
            work_flags = work_flags & 1;
            city7 = tile_resource(x2,y3);
            if (city7 != 0) {
              work_flags = work_flags | 2;
            }
            if ((tile_flags & 8) != 0) {
              work_flags = work_flags | 2;
            }
            if (1 < (char)terrain_types[terrain2].shields) {
              work_flags = work_flags | 2;
            }
            if (work_flags != 0) {
              units[unit].orders = ORDER_BUILD_ROAD;
              units[unit].ai_task = 'R';
              return AI_ORDERS_FINISH_ORDERS;
            }
          }
        }
      }
      if (((fort_need != 0) &&
          ((1 << (owner_byte & 0x1f) & (uint)game.human_players) == 0)) &&
         ((city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 == 0 ||
          (city7 = unit_stack_sum(unit,2), 1 < city7)))) {
        units[unit].orders = ORDER_BUILD_FORTRESS;
        units[unit].ai_task = 'X';
        return AI_ORDERS_FINISH_ORDERS;
      }
      city3 = city2;
      if (((units[unit].orders == ORDER_GOTO) &&
          ((units[unit].flags & UNITF_AI_COLONIST) == 0)) &&
         (city7 = (leaders[civs[civ].tribe].expand + 1) *
                   (int)landmass_table_block.landmass_recs[landmass].fertile_tiles,
         terrain = (uint)civs[civ].continent_cities[landmass],
         city7 + terrain * -0x10 != 0 && (int)(terrain * 0x10) <= city7))
      return AI_ORDERS_FINISH_ORDERS;
      if (own_dist < 2) {
        for (dir = 0; (int)dir < 8; dir++) {
          dir2 = (int)game.turn + dir & 7;
          x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir2] + x2);
          y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir2] + y3;
          city7 = map_in_bounds(x,y2);
          if (((city7 != 0) &&
              (city7 = tile_city_owner(x,y2), city7 < 0)) &&
             (((unit3 = tile_first_unit(x,y2), unit3 < 0 ||
               (((int)units[unit3].owner == civ &&
                (city7 = unit_stack_count_role(unit3,ROLE_SETTLE), city7 == 0)))) &&
              (city7 = ai_tile_improve_choice(civ,x,y2), city7 != 0)))) {
            tile_byte = tile_terrain(x,y2);
            unit4 = (uptr)tile_byte;
            if (((((int)(char)terrain_types[unit4].irrigate_turns / 5) * 3 - 3
                  <= (int)cities[city3].size) ||
                (city7 = tile_resource(x,y2), city7 != 0)) &&
               (city7 = map_distance_diag(x2,y3,x,y2),
               city7 < 3)) {
              units[unit].ai_task = 's';
              best_dir = dir2;
              city3 = best_dir;
              return AI_ORDERS_APPLY_DIR;
            }
          }
        }
      }
      terrain = tile_improvements(x2,y3);
      if ((((terrain & 0x10) == 0) &&
          ((tile = (byte *)tile_at(x2,y3), (*tile & 0x80) == 0 ||
           (city7 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), city7 != 0)))) &&
         ((own_dist < 3 && ((int)terrain2 < 3)))) {
        units[unit].ai_task = 'r';
        units[unit].orders = ORDER_BUILD_ROAD;
        return AI_ORDERS_FINISH_ORDERS;
      }
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): exploring, a move direction
   scores the unseen tiles four steps on and around it (land, or sea for a
   ship), less those with units; a settler adds their food. */
static void ai_orders_move_explore_bonus(AiUnitOrders *o)
{
  iptr x3;
  uptr &civ = o->civ; int &city7 = o->city7; iptr &x5 = o->x5; int &y = o->y;
  byte &owner_byte = o->owner_byte; int &check = o->check;
  bool &settler_type = o->settler_type; int &unit_domain = o->unit_domain;
  int &y3 = o->y3; int &x2 = o->x2; uptr &dir2 = o->dir2; uptr &dir = o->dir;
  int &score = o->score;
  x5 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] * 4 + x2);
  city7 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] * 4 + y3;
  y = map_in_bounds(x5,city7);
  if ((y != 0) &&
     (y = tile_visible_to(x5,city7,civ), y == 0)) {
    y = tile_is_ocean(x5,city7);
    if (y == 0) {
      score += 8;
    }
    else {
      score += 2;
    }
  }
  for (dir2 = 0; (int)dir2 < 8; dir2++) {
    x3 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir2] + x5);
    y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir2] + city7;
    check = map_in_bounds(x3,y);
    if (check != 0) {
      check = tile_visible_to(x3,y,civ);
      if ((check == 0) &&
         ((check = tile_is_ocean(x3,y), check == 0 ||
          (unit_domain == 2)))) {
        score += 2;
      }
      check = tile_unit_owner(x3,y);
      if (-1 < check) {
        score -= 2;
      }
      if (settler_type) {
        owner_byte = tile_terrain(x3,y);
        score += (char)terrain_types[owner_byte].food;
        if ((game.multiplayer != '\0') && (mp_doubles.production != 0)) {
          owner_byte = tile_terrain(x3,y);
          score += (char)terrain_types[owner_byte].food;
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): the move direction holds a
   foreign unit: whether attacking its best defender is worth it (odds by
   cost, its city, the roles, guard duty, the war) and the score; a weak one
   may be bribed instead; at peace, a lone foreign diplomat near our cities is
   a target. */
static int ai_orders_move_attack_score(AiUnitOrders *o)
{
  int attack_value;
  u8 &stat = o->stat; uptr &civ = o->civ; i32 &result = o->result;
  iptr &landmass = o->landmass; int &city7 = o->city7; iptr &x = o->x;
  int &y = o->y; byte &owner_byte = o->owner_byte; uint &terrain = o->terrain;
  uptr &unit_type = o->unit_type; uint &policy = o->policy;
  bool &settler_type = o->settler_type; iptr &unit = o->unit;
  int &civ4 = o->civ4; int &unit_domain = o->unit_domain;
  int &is_attack = o->is_attack; int &max_score = o->max_score;
  int &y3 = o->y3; uint &ai_role = o->ai_role; int &x2 = o->x2;
  uptr &unit4 = o->unit4; uptr &best_dir = o->best_dir;
  int &attack_bonus = o->attack_bonus; uptr &city4 = o->city4;
  uptr &civ2 = o->civ2; uint &y2 = o->y2; int &attack_cost = o->attack_cost;
  uptr &dir = o->dir; iptr &unit3 = o->unit3;
  int &will_attack = o->will_attack; int &is_guard = o->is_guard;
  uptr &city2 = o->city2; int &best_score = o->best_score;
  int &score = o->score;
  is_attack = 1;
  attack_bonus = 0;
  if (((settler_type) || (unit_types[units[unit].type].role == ROLE_TRADE)) ||
     ((((unit_domain == 2 && (unit_types[units[unit].type].hold != '\0')) &&
       (((city7 = tile_is_ocean(x,y2), city7 == 0 &&
         ((char)unit_types[units[unit].type].attack < 3)) ||
        ((city7 = unit_stack_sum(unit,2), 1 < city7 &&
         (((char)unit_types[units[unit].type].attack <=
           (char)unit_types[units[unit3].type].defense ||
          ((char)unit_types[units[unit3].type].attack <=
           (char)unit_types[units[unit].type].defense)))))))) ||
      ((city7 = tile_is_ocean(x2,y3), city7 != 0 &&
       ((unit_domain == 0 && ((unit_types[units[unit].type].flags & UTYPE_AMPHIBIOUS) == 0))))
      )))) return AI_ORDERS_CONTINUE;
  if (((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) &&
     (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
      ((ai_role != 6) &&
       (owner_byte = civs[civ2].reputation, city7 = civ_get_attitude(civ,civ2),
        y = rand_range(0,0x13), 0x32 < (int)((uint)owner_byte * 5 + city7 + y - 10))))) {
    unit3 = unit_stack_best_defender(unit3,unit);
    if (!(((((unit_types[units[unit3].type].domain != DOMAIN_AIR) ||
           (unit_types[units[unit3].type].range == '\0')) ||
          ((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) != 0)) ||
         (((city7 = tile_city_owner(x,y2), -1 < city7 ||
           (city7 = tile_airbase_owner(x,y2), -1 < city7)) ||
          (city7 = unit_stack_sum(unit3,9), city7 != 0)))) &&
        (((unit_types[units[unit3].type].role != ROLE_TRADE ||
          (city2 != city4)) ||
         (((civs[civ].government < civs[civ2].government ||
           ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0)) &&
          (city7 = rand_range(0,(uint)civs[civ2].government -
                                         (uint)civs[civ].government),
          city7 != 0))))))) return AI_ORDERS_CONTINUE;
    attack_value = unit_resolve_attack(unit,dir,0);
    city7 = unit_stack_sum(unit3,0);
    attack_value = ((city7 + 1) * attack_value) /
               (int)(char)unit_types[units[unit].type].cost;
    city7 = tile_city_owner(x,y2);
    if (-1 < city7) {
      attack_value = attack_value * 3;
    }
    if ((ai_role == 2) && (unit_types[units[unit3].type].role == ROLE_SEA_TRANSPORT))
    {
      attack_value = attack_value << 1;
    }
    if (is_guard == 0) {
      if (ai_role == 0) {
        if (policy == 0) {
          attack_value = attack_value << 1;
        }
        stat = unit_types[units[unit].type].attack;
        city7 = unit_stack_sum(unit,3);
        if ((char)stat * 2 < city7) {
          attack_value = attack_value << 1;
          attack_bonus = 1;
        }
        city7 = unit_stack_count_role(unit3,ROLE_ATTACK);
        if (city7 != 0) {
          attack_value = attack_value << 1;
          attack_bonus = 1;
        }
        city7 = civ_ai_goal_get_priority(civ,x2,y3,ROLE_ATTACK);
        if (city7 != 0) {
          attack_value = attack_value << 1;
        }
      }
      if ((game.rules & RULE_NORMAL_COMBAT) != 0) {
        city7 = unit_max_hp(unit3);
        y = unit_hp_remaining(unit3);
        if (y <= city7 / 3) {
          attack_bonus = 1;
          attack_value = attack_value << 1;
        }
      }
      if ((unit_types[units[unit3].type].flags & UTYPE_SUBMARINE) != 0) {
        attack_value = attack_value * 3;
      }
    }
    else {
      attack_value = attack_value / 2;
      if ((char)unit_types[units[unit3].type].attack <=
          (char)unit_types[units[unit].type].defense) {
        attack_value = 0;
      }
    }
    if ((unit_type == UNIT_PARTISANS) &&
       (4 < (char)unit_types[units[unit3].type].role)) {
      attack_bonus = 2;
    }
    unit4 = (uptr)(char)unit_types[units[unit3].type].role;
    if ((civ2 == 0) && (unit4 == 7)) {
      if ((game.human_players & 1) == 0) {
        attack_value = attack_value / 2;
      }
    }
    else if ((civ2 == 0) && (4 < (int)unit4)) {
      attack_value += 0xc;
    }
    else if (unit4 == 6) {
      attack_value = attack_value << 1;
    }
    if ((int)((-(uint)(civ2 == 0) & 6) + 6) <= attack_value) {
      score += attack_value * 4;
    }
    else {
      score = -999;
      if ((((((policy == 0) && (ai_role == 0)) && (unit_domain == 0)) &&
           (((units[unit3].flags & UNITF_VETERAN) != 0 ||
            (units[unit3].orders == ORDER_FORTIFIED)))) &&
          ((3 < (char)terrain_types[terrain].defense ||
           (result = tile_improvements(x,y2),
           ((byte)result & 0x42) == 0x40)))) &&
         (((city7 = unit_stack_sum(unit3,2), city7 < 2 &&
           (city7 = tile_city_owner(x,y2), city7 < 0)) &&
          (city7 = rand_range(0,3), city7 == 0)))) {
        unit_bribe(unit3,civ);
      }
      return AI_ORDERS_CONTINUE;
    }
  }
  else {
    if (((((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) ||
         (unit_types[units[unit3].type].role != ROLE_DIPLOMACY)) ||
        (city7 = unit_stack_sum(unit3,2), city7 != 1)) ||
       ((unit_types[units[unit].type].attack == '\0' ||
        (city7 = city_find_nearest(x,y2,-1,0xffffffff,-1),
        (int)cities[city7].owner != civ)))) {
      if (((ai_role == 6) &&
           ((policy == 0) ||
            ((policy == 5 ||
             ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0))))) ||
          ((policy == 0) && (ai_role == 0))) {
        if (!(((ai_role == 6) || (city2 == city4)) && (DIFF_PRINCE < game.difficulty))) {
          if (units[unit3].orders != ORDER_FORTIFIED) return AI_ORDERS_CONTINUE;
          unit4 = 0;
          for (civ4 = 1; civ4 < 8; civ4++) {
            if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0)
               && (city7 = diplo_is_hostile(civ,civ4), city7 != 0)) {
              unit4 = 1;
            }
          }
          if (unit4 == 0) return AI_ORDERS_CONTINUE;
        }
        if (unit_domain == 0) {
          unit4 = 0;
          if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
            for (civ4 = 1; civ4 < 8; civ4++) {
              if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) !=
                   0) && (city7 = diplo_is_hostile(civ2,civ4),
                         city7 == 0)) {
                unit4 = 1;
              }
            }
          }
          if (unit4 == 0) {
            unit4 = 1;
            for (civ4 = 1; civ4 < 8; civ4++) {
              if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) !=
                   0) && (*(char *)&civs[civ4].continent_cities[landmass] !=
                          '\0')) {
                unit4 = 0;
                break;
              }
            }
            if ((((unit4 != 0) &&
                 (city7 = unit_stack_sum(unit3,2), city7 < 2)) &&
                (city7 = tile_city_owner(x,y2), city7 < 0)) &&
               ((city7 = rand_range(0,7), city7 == 0 ||
                ((-1 < (int)city4 &&
                 (((uint)game.human_players &
                  1 << (cities[city4].owner & 0x1fU)) != 0)))))) {
              unit_bribe(unit3,civ);
            }
          }
        }
      }
    }
    else {
      score = 0x3c;
      if (best_score < 0x3c) {
        max_score = 0x3c;
        best_score = 0x3c;
        best_dir = dir;
        will_attack = 1;
        attack_cost = 2;
      }
    }
    return AI_ORDERS_CONTINUE;
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): the base score of a move
   direction: random for a settler; on land out of sight by terrain (move cost
   for attackers, else defense); otherwise defense, or for diplomats and trade
   units moves and the way to a city near; next to an enemy, by the units of
   the stack there per role. */
static int ai_orders_move_base_score(AiUnitOrders *o)
{
  uptr road_imps;
  u8 &stat = o->stat; uptr &civ = o->civ; i32 &result = o->result;
  int &city7 = o->city7; iptr &x = o->x; int &y = o->y;
  uint &terrain = o->terrain; int &near_dist = o->near_dist;
  bool &settler_type = o->settler_type; iptr &unit = o->unit;
  uint &ai_role = o->ai_role; uptr &unit4 = o->unit4; uptr &city4 = o->city4;
  uptr &terrain2 = o->terrain2; uptr &civ2 = o->civ2; uint &y2 = o->y2;
  int &cand_score = o->cand_score; iptr &unit3 = o->unit3;
  uptr &city2 = o->city2; int &score = o->score;
  score = 0;
  if (settler_type) {
    if ((terrain2 == TERRAIN_OCEAN) &&
       (city7 = tile_zoc_unit_adjacent(x,y2,civ), city7 != 0))
    return AI_ORDERS_CONTINUE;
    score = rand_range(0,4);
  }
  else if ((units[unit].visibility == '\0') && (terrain2 != TERRAIN_OCEAN)) {
    if (ai_role == 0) {
      score = rand_range(0,2);
      score += (char)terrain_types[terrain].move_cost * -2;
    }
    else {
      score = rand_range(0,2);
      score -= (char)terrain_types[terrain].defense;
    }
  }
  else {
    score = rand_range(0,4);
    if ((unit3 < 0) || (civ2 != civ)) {
      if ((int)ai_role < 6) {
        score += (char)terrain_types[terrain].defense * 4;
      }
      else {
        score = score +
                   (6 - ((char)terrain_types[terrain].move_cost - 1) *
                        (uint)cosmic.road_move_mult);
        if ((near_dist < 4) && (city2 != city4)) {
          cand_score = map_distance(x,y2,(int)cities[city4].x,
                                        (int)cities[city4].y);
          if (cand_score == 1) {
            if ((int)(uint)cosmic.road_move_mult <
                (int)(char)terrain_types[terrain].move_cost) {
              score -= 4;
            }
            else {
              score += 8;
            }
          }
          else if (((cand_score == 2) &&
                   ((int)(char)terrain_types[terrain].move_cost <=
                    (int)(uint)cosmic.road_move_mult)) &&
                  (road_imps = tile_improvements(x,y2), (road_imps & TILE_ROAD) != 0))
          {
            score += 4;
          }
        }
      }
    }
    else {
      result = tile_improvements(x,y2);
      if ((((byte)result & 0x43) == 1) &&
         (city7 = tile_adjacent_nonallied_unit(x,y2,civ), city7 != 0)) {
        switch(ai_role) {
        case 0:
          unit4 = unit_stack_count_role(unit3,ROLE_ATTACK);
          if (((int)unit4 < 1) &&
             (city7 = unit_stack_count_role(unit3,ROLE_DEFEND), city7 != 0)) {
            score += 8;
          }
          else {
            score += unit4 * -8;
          }
          break;
        case 1:
          unit4 = unit_stack_count_role(unit3,ROLE_DEFEND);
          if ((int)unit4 < 1) {
            score += 8;
          }
          else {
            score += unit4 * -4;
          }
          break;
        case 2:
          city7 = unit_stack_count_role(unit3,ROLE_SEA_TRANSPORT);
          if (city7 != 0) {
            score += 8;
          }
          city7 = unit_stack_count_role(unit3,ROLE_NAVAL_SUPERIORITY);
          score += city7 * -4;
          break;
        case 4:
          city7 = unit_stack_count_role(unit3,ROLE_NAVAL_SUPERIORITY);
          if (city7 != 0) {
            score += 8;
          }
          city7 = unit_stack_count_role(unit3,ROLE_SEA_TRANSPORT);
          score += city7 * -8;
          break;
        case 5:
        case 0x15:
          score -= 4;
          break;
        case 6:
          city7 = unit_stack_count_role(unit3,ROLE_ATTACK);
          y = unit_stack_count_role(unit3,ROLE_DEFEND);
          if (city7 + y != 0) {
            score += 8;
          }
        }
      }
      else {
        switch(ai_role) {
        case 0:
          city7 = unit_stack_sum(unit3,0);
          y = unit_stack_sum(unit3,2);
          score += (city7 * 4) / (y + 1);
          break;
        case 1:
          city7 = unit_stack_sum(unit3,3);
          y = unit_stack_sum(unit3,0);
          score += (city7 * 2) / (y + 1);
          break;
        case 5:
          stat = terrain_types[terrain].defense;
          city7 = unit_stack_sum(unit3,0);
          score += ((char)stat + city7) * 2;
          break;
        case 6:
          city7 = unit_stack_count_role(unit3,ROLE_ATTACK);
          y = unit_stack_count_role(unit3,ROLE_DEFEND);
          if (city7 + y != 0) {
            score += 8;
          }
        }
      }
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): one of the eight move directions:
   whether the unit may go there (zones of control, sea, stacking) and its
   score (base, last direction, closeness to a foreign city, treaties, huts
   and cities to take, attacks, exploring); the best one so far kept in
   best_dir. */
static void ai_orders_move_score_dir(AiUnitOrders *o)
{
  uint is_ocean;
  uptr civ3;
  uptr &civ = o->civ; i32 &result = o->result; int &city7 = o->city7;
  uptr &city3 = o->city3; iptr &x = o->x; int &y = o->y;
  byte &owner_byte = o->owner_byte; uint &terrain = o->terrain;
  int &check = o->check; int &near_dist = o->near_dist;
  i32 &arg_hi = o->arg_hi; iptr &unit = o->unit;
  int &in_enemy_zoc = o->in_enemy_zoc; int &unit_domain = o->unit_domain;
  int &is_attack = o->is_attack; int &max_score = o->max_score;
  int &y3 = o->y3; int &x2 = o->x2; uptr &unit4 = o->unit4;
  uptr &best_dir = o->best_dir; uint &flag = o->flag;
  int &attack_bonus = o->attack_bonus; int &has_orders = o->has_orders;
  u32 &type_flags = o->type_flags; int &hold_pos = o->hold_pos;
  uptr &city4 = o->city4; uptr &terrain2 = o->terrain2; uptr &civ2 = o->civ2;
  uint &y2 = o->y2; int &cand_score = o->cand_score; int &explore = o->explore;
  int &attack_cost = o->attack_cost; uptr &dir = o->dir;
  iptr &unit3 = o->unit3; int &will_attack = o->will_attack;
  uptr &city2 = o->city2; int &own_dist = o->own_dist;
  int &best_score = o->best_score; int &score = o->score;
  uint &tile_flags = o->tile_flags;
  x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
  y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
  city7 = map_in_bounds(x,y2);
  if (city7 != 0) {
    civ2 = tile_owner(x,y2);
    owner_byte = tile_terrain(x,y2);
    terrain = (uint)owner_byte;
    is_ocean = (uint)(terrain == TERRAIN_OCEAN);
    if ((is_ocean == 0) || (unit_domain != 0)) {
      unit3 = tile_first_unit(x,y2);
      while (((-1 < unit3 && (-1 < units[unit3].next_in_stack)) &&
             (4 < (char)unit_types[units[unit3].type].role))) {
        unit3 = unit_next_in_stack(unit3);
      }
      if (((((((in_enemy_zoc == 0) || (unit_domain != 0)) ||
             ((-1 < unit3 ||
              ((city7 = tile_in_enemy_zoc(x,y2,civ), city7 == 0 ||
               ((type_flags & 2) != 0)))))) || (is_ocean != 0)) &&
           ((((is_ocean == 0 || (unit_domain != 0)) || (unit3 < 0)) || (civ2 == civ))
           )) && (((unit_domain != 2 || (is_ocean != 0)) ||
                  (((-1 < unit3 && ((civ2 != civ && (terrain2 == TERRAIN_OCEAN)))) &&
                   ((type_flags & 8) == 0)))))) &&
         ((((unit3 < 0 || (civ2 != civ)) ||
           (city7 = unit_stack_sum(unit3,2), city7 < 2)) ||
          ((city7 = tile_city_owner(x,y2), -1 < city7 ||
           (result = tile_improvements(x,y2), ((byte)result & 0x42) == 0x40))))))
      {
        if (ai_orders_move_base_score(o) == AI_ORDERS_CONTINUE) return;
        if (unit_domain == 1) {
          score = rand_range(0,2);
        }
        if (-1 < (char)units[unit].last_dir) {
          if ((int)(char)units[unit].last_dir == dir ||
              (int)((int)(char)units[unit].last_dir - dir) < 0) {
            unit4 = ~((int)(char)units[unit].last_dir - dir) + 1;
          }
          else {
            unit4 = (int)(char)units[unit].last_dir - dir;
          }
          if (4 < (int)unit4) {
            unit4 = 8 - unit4;
          }
          score += unit4 * unit4 * -2;
        }
        is_attack = 0;
        flag = 0;
        if (((unit3 < 0) &&
            (city7 = tile_is_ocean(x,y2), city3 = city4, city7 == 0)) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          if ((((near_dist < 4) && ((int)cities[city4].owner != civ)) &&
              (((*(byte *)&civs[civ].treaties[cities[city4].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0
               && ((*(byte *)&civs[civ].treaties[cities[city4].owner] & TREATY_ALLIANCE) ==
                   0)))) &&
             (((*(byte *)&civs[civ].treaties[cities[city4].owner] & TREATY_PEACE) != 0
              || (city7 = civ_get_attitude(civ,(int)cities[city4].owner),
                 city7 < 0x32)))) {
            cand_score = map_distance_diag((int)cities[city3].x,(int)cities[city3].y,x
                                          ,y2);
            if ((near_dist < cand_score) && (near_dist < 3)) {
              score += 5;
              flag = 1;
            }
            if (cand_score < near_dist) return;
            if (cand_score == near_dist) {
              score -= 5;
            }
          }
          civ3 = tile_radius_civ(x,y2);
          if ((((0 < (int)civ3) && (civ3 != civ)) &&
              ((*(byte *)&civs[civ].treaties[civ3] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0)) &&
             ((*(byte *)&civs[civ].treaties[civ3] & TREATY_ALLIANCE) == 0)) {
            if ((((*(byte *)&civs[civ].treaties[civ3] & TREATY_PEACE) != 0) &&
                ((int)((uint)civs[civ3].reputation -
                      (int)civs[civ].reputation_credit[civ]) < 7)) &&
               (flag == 0)) return;
            if ((((*(byte *)&civs[civ].treaties[civ3] & TREATY_PEACE) != 0) &&
                ((int)((uint)civs[civ3].reputation -
                      (int)civs[civ].reputation_credit[civ]) < 7)) ||
               (city7 = civ_get_attitude(civ,civ3), city7 < 0x32)) {
              score -= 5;
              tile_flags = tile_improvements(x,y2);
              if ((tile_flags & 0x1c) != 0) {
                score -= 3;
              }
            }
          }
        }
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
          score -= 6;
        }
        if (unit3 < 0) {
          city7 = tile_city_owner(x,y2);
          if ((-1 < city7) && (civ2 != civ)) {
            if (((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0) &&
               (((civs[civ2].reputation < 7 ||
                 ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) == 0)) ||
                ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0))))
            return;
            score = 999;
            has_orders = 0;
            hold_pos = 0;
          }
          city7 = tile_has_hut(x,y2);
          if (city7 != 0) {
            score += 0x14;
          }
        }
        else {
          if (civ2 != civ) {
            if (ai_orders_move_attack_score(o) == AI_ORDERS_CONTINUE) return;
          }
          else {
            score -= (char)unit_types[units[unit].type].defense;
          }
        }
        if (((own_dist != 0) && (unit_domain == 2)) && ((unit3 < 0 || (civ2 == civ)))
           ) {
          city7 = tile_landmass(x,y2);
          y = city_adjacent_sea_info(city2,0,0);
          if (city7 == y) {
            score += 10;
          }
        }
        if ((((unit_domain != 1) || (city7 = tile_airbase_owner(x,y2), city7 < 0)
             ) || (civ2 == civ)) || (-1 < unit3)) {
          if (explore != 0) {
            ai_orders_move_explore_bonus(o);
          }
          if ((y2 == 0) || ((int)map.height - 1U == y2)) {
            score = score / 3;
          }
          city7 = score;
          if (((is_attack != 0) &&
              (y = unit_moves_left(unit),
              y < (int)(uint)cosmic.road_move_mult)) &&
             (y = tile_city_owner(x,y2), y < 0)) {
            y = unit_stack_sum(unit,0);
            arg_hi = 99;
            result = 1;
            x = unit_stack_sum(unit,3);
            check = int_clamp(x,result,arg_hi);
            score = (y / check) * score;
          }
          if (best_score < score) {
            best_score = score;
            best_dir = dir;
            will_attack = is_attack;
            attack_cost = attack_bonus;
          }
          if (max_score < city7) {
            max_score = city7;
          }
        }
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): the move by score: whether to
   explore, a defender with attack to spare counting as an attacker, then the
   best of the eight directions; no move when an attack lacks the moves, when
   the best is not the most promising, or when holding (a unit with a goto
   keeps it). */
static int ai_orders_plan_move(AiUnitOrders *o)
{
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &city3 = o->city3; int &near_dist = o->near_dist;
  uptr &unit_type = o->unit_type; uint &policy = o->policy;
  iptr &unit = o->unit; int &max_score = o->max_score; int &y3 = o->y3;
  uint &ai_role = o->ai_role; int &x2 = o->x2; uptr &best_dir = o->best_dir;
  int &has_orders = o->has_orders; int &hold_pos = o->hold_pos;
  int &explore = o->explore; int &attack_cost = o->attack_cost;
  uptr &dir = o->dir; int &will_attack = o->will_attack;
  int &own_dist = o->own_dist; int &best_score = o->best_score;
  best_score = -999;
  max_score = -999;
  best_dir = 8;
  explore = 0;
  will_attack = 0;
  if (((ai_role == 4) && (own_dist != 0)) &&
     ((city7 = unit_stack_sum(unit,2), 1 < city7 &&
      (units[unit].orders == ORDER_GOTO)))) {
    has_orders = 1;
  }
  city7 = tile_zoc_unit_adjacent(x2,y3,civ);
  if (city7 == 0) {
    if (has_orders == 0) {
      explore = 1;
    }
    else {
      explore = 0;
    }
  }
  else {
    units[unit].last_dir = 0xff;
  }
  if (((game.game_flags & GAME_SCENARIO) != 0) && ((scenario.flags[0] & 8) != 0)) {
    explore = 0;
  }
  if (ai_role == 1) {
    if (((char)unit_types[unit_type].defense < (char)unit_types[unit_type].attack) &&
       ((civs[civ].continent_flags[landmass] & 0x10) == 0)) {
      ai_role = 0;
    }
    if (((char)unit_types[unit_type].defense <= (char)unit_types[unit_type].attack) && (policy == 0)
       ) {
      ai_role = 0;
    }
  }
  for (dir = 0; (int)dir < 8; dir++) {
    ai_orders_move_score_dir(o);
  }
  if ((will_attack != 0) &&
     (city7 = (uint)cosmic.road_move_mult - attack_cost, near_dist = unit_moves_left(unit),
     near_dist < city7)) {
    best_dir = 8;
  }
  if (max_score != best_score) {
    best_dir = 8;
  }
  if (best_dir == 8) {
    will_attack = 0;
  }
  if ((has_orders != 0) && (will_attack == 0)) return AI_ORDERS_FINISH_ORDERS;
  if ((hold_pos != 0) && (will_attack == 0)) {
    best_dir = 8;
  }
  units[unit].ai_task = '9';
  city3 = best_dir;
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a ship: its land units: sentried
   ones beyond the defenders its city needs are woken for the trip, defenders
   kept up to that number and fortified, the rest woken; tasks_mask the roles
   of the woken ones. */
static void ai_orders_transport_cargo(AiUnitOrders *o)
{
  uint &policy = o->policy; iptr &unit2 = o->unit2; iptr &unit = o->unit;
  uptr &needed_defenders = o->needed_defenders;
  int &defender_count = o->defender_count; iptr &unit5 = o->unit5;
  int &own_dist = o->own_dist; int &freed_count = o->freed_count;
  uint &tasks_mask = o->tasks_mask;
  tasks_mask = 0;
  freed_count = 0;
  defender_count = 0;
  unit5 = unit;
  for (unit = unit_stack_first(unit); unit2 = unit5, -1 < unit;
      unit = unit_next_in_stack(unit)) {
    if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
      if (units[unit].orders == ORDER_FORTIFIED) {
        if (((own_dist == 0) && (defender_count = defender_count + 1, (int)needed_defenders < defender_count)
            ) && ((policy == 4 || (freed_count == 0)))) {
          units[unit].orders = ORDER_NONE;
          tasks_mask = tasks_mask | 1 << (unit_types[units[unit].type].role & 0x1f);
          freed_count++;
        }
      }
      else if ((unit_types[units[unit].type].role == ROLE_DEFEND) &&
              (defender_count < (int)needed_defenders)) {
        defender_count++;
        units[unit].orders = ORDER_FORTIFYING;
      }
      else {
        units[unit].orders = ORDER_NONE;
        tasks_mask = tasks_mask | 1 << (unit_types[units[unit].type].role & 0x1f);
        freed_count++;
      }
    }
  }
  unit = unit5;
}

/* Part of ai_unit_choose_orders (00538a29): a transport (role 4): whether
   there is room aboard, then (with moves) the port to gather units at; in its
   city, a settler aboard gets off; it unloads where its cargo is wanted
   unless still bound for a good port. */
static int ai_orders_transport(AiUnitOrders *o)
{
  iptr &landmass = o->landmass; int &city7 = o->city7;
  int &near_dist = o->near_dist; iptr &unit2 = o->unit2;
  uptr &unit4 = o->unit4; uint &flag = o->flag;
  uint &room_aboard = o->room_aboard; int &retried = o->retried;
  int &own_dist = o->own_dist; int &freed_count = o->freed_count;
  int &no_moves = o->no_moves; uint &tasks_mask = o->tasks_mask;
  if ((near_dist == 0) && ((char)units[unit2].counter < '\0')) {
    units[unit2].counter = (byte)game.turn & 7;
  }
  if (((tasks_mask & 0xffffff7f) == 0x20) &&
     (freed_count < (char)unit_types[units[unit2].type].hold)) {
    room_aboard = 1;
  }
  else {
    room_aboard = 0;
  }
  if ((tasks_mask == 0x20) && (units[unit2].orders != ORDER_GOTO)) {
    unit4 = 1;
  }
  else {
    unit4 = 0;
  }
  flag = (uint)(no_moves == 0);
  if (no_moves == 0) {
    if (ai_orders_transport_port(o) == AI_ORDERS_APPLY_DIR) return AI_ORDERS_APPLY_DIR;
  }
  if (own_dist == 0) {
    for (unit4 = unit_stack_first(unit2); -1 < (int)unit4;
        unit4 = unit_next_in_stack(unit4)) {
      if (unit_types[units[unit4].type].role == ROLE_SETTLE) {
        unit_relocate_in_place(unit4,1);
        break;
      }
    }
  }
  if (((((flag != 0) && (units[unit2].ai_task == 'A')) &&
       (units[unit2].orders == ORDER_GOTO)) &&
      ((near_dist != 0 &&
       (city7 = city_at((int)units[unit2].goto_x,
                                    (int)units[unit2].goto_y), -1 < city7)))) &&
     (((cities[city7].flags & CITYF_COASTAL) != 0 &&
      ((city7 = city_adjacent_to_body(city7,landmass), city7 != 0 &&
       ((game.turn & 0xfU) != 0)))))) {
    flag = 0;
  }
  if (((flag != 0) || (retried != 0)) || (no_moves != 0)) {
    switch (ai_orders_transport_unload(o)) {
    case AI_ORDERS_APPLY_DIR: return AI_ORDERS_APPLY_DIR;
    case AI_ORDERS_FINISH_ORDERS: return AI_ORDERS_FINISH_ORDERS;
    default: break;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a slow land attacker in the
   area of a human civ it is at war with pillages its tile: a fortress, mine
   or irrigation near the city, or also a road on a landmass without cities
   of its own (or for a partisan). */
static int ai_orders_pillage(AiUnitOrders *o)
{
  Tile *&map_tile = o->map_tile; bool &forced_attack = o->forced_attack;
  byte &tile_byte = o->tile_byte; uptr &civ = o->civ;
  iptr &landmass = o->landmass; int &city7 = o->city7; uptr &city3 = o->city3;
  int &near_dist = o->near_dist; uptr &unit_type = o->unit_type;
  iptr &unit = o->unit; int &y3 = o->y3; int &x2 = o->x2;
  uptr &unit4 = o->unit4; uptr &city4 = o->city4;
  uint &tile_flags = o->tile_flags;
  city3 = city4;
  city7 = unit_max_moves(unit);
  if (((((city7 <= (int)(uint)cosmic.road_move_mult) && (-1 < (int)city3)) &&
       (((uint)game.human_players & 1 << (cities[city3].owner & 0x1fU)) != 0)) &&
      ((((byte)(civs[civ].treaties[cities[city3].owner] >> 8) & (TREATY_WAR >> 8)) != 0
       && (unit_types[units[unit].type].domain == DOMAIN_LAND)))) &&
     (unit_types[units[unit].type].attack != '\0')) {
    map_tile = tile_at(x2,y3);
    tile_byte = map_tile->improvements;
    tile_flags = (uint)tile_byte;
    unit4 = 0;
    if (((tile_byte & 0x4c) == 0) || (3 < near_dist)) {
      if (((tile_byte & 0x5c) != 0) &&
         (((*(char *)&civs[civ].continent_cities[landmass] == '\0' && (near_dist < 8)) ||
          (unit_type == UNIT_PARTISANS)))) {
        unit4 = 1;
      }
    }
    else {
      unit4 = 1;
    }
    if ((unit4 != 0) && (!forced_attack)) {
      unit_pillage(unit,0);
      return AI_ORDERS_FINISH_ORDERS;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a partisan: goes after a visible
   foreign non-military unit near on its landmass, of a civ not at peace; next
   to a human civ's city it is not at peace with, moves to a road or
   improvement no farther from it, unless a city is next to it. */
static int ai_orders_partisan(AiUnitOrders *o)
{
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &city3 = o->city3; iptr &x = o->x; byte &owner_byte = o->owner_byte;
  uint &terrain = o->terrain; uptr &unit_type = o->unit_type;
  iptr &unit = o->unit; uptr &city6 = o->city6; int &y3 = o->y3;
  int &x2 = o->x2; uptr &best_dir = o->best_dir;
  int &has_orders = o->has_orders; iptr &unit5 = o->unit5;
  uptr &civ2 = o->civ2; uint &y2 = o->y2; int &cand_score = o->cand_score;
  uptr &dir = o->dir;
  if (unit_type == UNIT_PARTISANS) {
    unit5 = unit;
    for (unit = 0; unit < game.num_units; unit++) {
      if (((((units[unit].id != 0) &&
            (4 < (char)unit_types[units[unit].type].role)) &&
           ((civ2 = (uptr)units[unit].owner, civ2 != civ &&
            (((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0 &&
             (city7 = map_distance(x2,y3,(int)units[unit].x,
                                          (int)units[unit].y), city7 < 4))))))
          && (((1 << (owner_byte & 0x1f) & (uint)units[unit].visibility) != 0 ||
              ((int)units[unit].owner == (uint)owner_byte)))) &&
         (city7 = tile_landmass((int)units[unit].x,(int)units[unit].y),
         city7 == landmass)) {
        unit_set_goto(unit5,0x50,(int)units[unit].x,
                           (int)units[unit].y);
        has_orders = 1;
        break;
      }
    }
    unit = unit5;
    if (has_orders != 0) return AI_ORDERS_BREAK;
  }
  if ((((unit_type == UNIT_PARTISANS) && (-1 < (int)city3)) &&
      (((uint)game.human_players & 1 << (cities[city3].owner & 0x1fU)) != 0)) &&
     ((*(byte *)&civs[civ].treaties[cities[city3].owner] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)) {
    cand_score = map_distance_diag(x2,y3,(int)cities[city3].x,
                                  (int)cities[city3].y);
    city6 = 0xffffffff;
    for (dir = 0; (int)dir < 8; dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
      city7 = map_in_bounds(x,y2);
      if ((city7 != 0) && (city7 = tile_unit_owner(x,y2), city7 < 0)) {
        city7 = tile_city_owner(x,y2);
        if (-1 < city7) {
          city6 = 0xffffffff;
          break;
        }
        city7 = map_distance_diag(x,y2,(int)cities[city3].x,
                                    (int)cities[city3].y);
        if ((city7 <= cand_score) &&
           (terrain = tile_improvements(x,y2), (terrain & 0x5c) != 0)) {
          city6 = dir;
        }
      }
    }
    if (-1 < (int)city6) {
      units[unit].ai_task = 'p';
      best_dir = city6;
      city3 = best_dir;
      return AI_ORDERS_APPLY_DIR;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a unit with a goto keeps it
   (orders finished) unless an enemy is next to it on the way, or the goto
   ends at a foreign unit or city it may not attack (a transport with company
   retries once without the goto). */
static int ai_orders_keep_goto(AiUnitOrders *o)
{
  bool &forced_attack = o->forced_attack; uptr &civ = o->civ;
  int &city7 = o->city7; iptr &unit = o->unit; int &y3 = o->y3;
  uint &ai_role = o->ai_role; int &x2 = o->x2; uint &flag = o->flag;
  int &has_orders = o->has_orders; int &retried = o->retried;
  uptr &civ2 = o->civ2;
  if ((units[unit].orders == ORDER_GOTO) &&
     (((ai_role != 2) ||
       (city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 == 0)) && (!forced_attack))) {
    flag = 1;
    city7 = map_distance(x2,y3,(int)units[unit].goto_x,
                                (int)units[unit].goto_y);
    if (1 < city7) {
      if (((int)ai_role < 5) &&
         (city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 != 0)) {
        has_orders = 1;
        flag = 0;
      }
    }
    else {
      civ2 = tile_city_or_unit_owner
                           ((int)units[unit].goto_x,(int)units[unit].goto_y);
      if (!(((int)civ2 < 0) || (civ2 == civ))) {
        if ((ai_role != 4) ||
           ((city7 = unit_stack_sum(unit,2), city7 < 2 || (retried != 0)))) {
          if (((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0) &&
             ((ai_role != 7 ||
              (city7 = tile_city_owner((int)units[unit].goto_x,(int)units[unit].goto_y),
              city7 < 0)))) {
            flag = 0;
            units[unit].orders = ORDER_NONE;
          }
        }
        else {
          units[unit].orders = ORDER_NONE;
          retried = 1;
          return AI_ORDERS_CONTINUE;
        }
      }
    }
    if (flag != 0) return AI_ORDERS_FINISH_ORDERS;
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a defender out of its city:
   fortifies (holds, or stays put when better at defense) when alone near
   home, the last defender facing an enemy, or at a fortress or fortress site;
   not in the area of a civ at peace with a good name. */
static int ai_orders_defender_fortify(AiUnitOrders *o)
{
  bool &forced_attack = o->forced_attack; uptr &civ = o->civ;
  int &city7 = o->city7; uptr &city3 = o->city3; uint &terrain = o->terrain;
  int &near_dist = o->near_dist; uptr &unit_type = o->unit_type;
  iptr &unit = o->unit; int &in_enemy_zoc = o->in_enemy_zoc;
  int &fort_need = o->fort_need; int &y3 = o->y3; uint &ai_role = o->ai_role;
  int &x2 = o->x2; uptr &best_dir = o->best_dir; uint &flag = o->flag;
  uint &on_fortress = o->on_fortress; int &has_orders = o->has_orders;
  int &hold_pos = o->hold_pos; uptr &city4 = o->city4;
  uptr &terrain2 = o->terrain2; uptr &civ2 = o->civ2; uptr &city2 = o->city2;
  int &own_dist = o->own_dist;
  if ((((ai_role == 1) && (terrain2 != TERRAIN_OCEAN)) && (has_orders == 0)) && (!forced_attack)) {
    flag = 0;
    city7 = unit_stack_sum(unit,2);
    if ((city7 < 2) && (own_dist < 4)) {
      flag = flag | 1;
    }
    if (((in_enemy_zoc != 0) && (city7 = unit_stack_count_role(unit,ROLE_DEFEND), city7 == 1)) &&
       ((((int)city4 < 0 ||
         (((city4 == city2 || (4 < near_dist)) ||
          (((byte)(civs[civ].treaties[cities[city4].owner] >> 8) & (TREATY_WAR >> 8)) != 0)))) ||
        (6 < (int)((uint)civs[cities[city4].owner].reputation -
                  (int)civs[civ].reputation_credit[cities[city4].owner]))))) {
      flag = flag | 2;
    }
    if ((fort_need != 0) || (on_fortress != 0)) {
      city7 = unit_stack_count_role(unit,ROLE_DEFEND);
      if (city7 == 1) {
        flag = flag | 4;
      }
      if ((flag != 0) || ((units[unit].flags & UNITF_AI_GARRISON) != 0)) {
        flag = flag | 8;
      }
    }
    terrain = flag;
    if (((char)unit_types[unit_type].defense <= (char)unit_types[unit_type].attack) &&
       (terrain = flag & 0xfffffffe,
       (char)unit_types[unit_type].defense < (char)unit_types[unit_type].attack)) {
      terrain = flag & 0xfffffff4;
    }
    flag = terrain;
    civ2 = tile_radius_civ(x2,y3);
    if ((((0 < (int)civ2) && (civ2 != civ)) &&
        ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0)) &&
       ((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
        < 7)) {
      flag = 0;
    }
    if (flag != 0) {
      if ((flag & 0xc) != 0) {
        civ_clear_standing_goals_near(civ,ROLE_DEFEND,x2,y3,1);
      }
      units[unit].ai_task = 'F';
      if ((char)unit_types[unit_type].attack < (char)unit_types[unit_type].defense) {
        best_dir = 8;
        city3 = best_dir;
        return AI_ORDERS_APPLY_DIR;
      }
      hold_pos = 1;
      return AI_ORDERS_BREAK;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): an unmoved partisan counts its
   turns out of enemy zones of control (down when in one); after ten, or when
   the nearest city on its landmass is not its own, it leaves its fortress
   (on_fortress 0). */
static void ai_orders_partisan_fortress(AiUnitOrders *o)
{
  uptr &civ = o->civ; iptr &landmass = o->landmass; int &city7 = o->city7;
  uptr &unit_type = o->unit_type; iptr &unit = o->unit; uptr &city6 = o->city6;
  int &in_enemy_zoc = o->in_enemy_zoc; uptr &city = o->city; int &y3 = o->y3;
  int &x2 = o->x2; uint &on_fortress = o->on_fortress; uptr &city4 = o->city4;
  int &cand_score = o->cand_score; uptr &city2 = o->city2;
  int &best_score = o->best_score;
  if ((unit_type == UNIT_PARTISANS) && (units[unit].moves_used == '\0')) {
    if (in_enemy_zoc == 0) {
      if ((char)units[unit].counter < 15) {
        units[unit].counter++;
      }
    }
    else if (units[unit].counter != '\0') {
      units[unit].counter += 0xff;
    }
    if ('\t' < (char)units[unit].counter) {
      on_fortress = 0;
    }
    if (on_fortress != 0) {
      best_score = 9999;
      city6 = 0xffffffff;
      for (city = 0; (int)city < (int)game.num_cities; city++) {
        if ((((cities[city].id != 0) && (city != city4)) &&
            (city7 = tile_landmass((int)cities[city].x,(int)cities[city].y),
            city7 == landmass)) &&
           ((cand_score = map_distance_diag(x2,y3,(int)cities[city].x,
                                           (int)cities[city].y), cand_score < best_score ||
            ((cand_score == best_score && ((int)cities[city].owner == civ)))))) {
          city6 = city;
          best_score = cand_score;
        }
      }
      if ((city2 != city4) &&
         (((int)city6 < 0 || ((int)cities[city6].owner != civ)))) {
        on_fortress = 0;
      }
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): in the area of its own city, next
   to a unit of a civ at peace with it (the zone of control check left that
   civ in adjacent_foreign_civ): its attitude toward that civ changes, and
   for a human civ, above difficulty 2, with attitude over 60 and a lower
   power rank than that civ's, one time in four it sets treaty flag 0x20. */
static void ai_orders_trespassers(AiUnitOrders *o)
{
  uint *treaty;
  iptr attitude;
  iptr roll;
  uptr &civ = o->civ; int &in_enemy_zoc = o->in_enemy_zoc;
  uptr &city4 = o->city4; uptr &city2 = o->city2;
  if (((in_enemy_zoc != 0) && (dialog_text_font_obj_block.adjacent_foreign_civ != 0)) &&
     ((city2 == city4 && ((*(byte *)&civs[civ].treaties[dialog_text_font_obj_block.adjacent_foreign_civ] & TREATY_PEACE) != 0))
     )) {
    civ_adjust_attitude(civ,dialog_text_font_obj_block.adjacent_foreign_civ,1);
    if ((((1 << ((byte)dialog_text_font_obj_block.adjacent_foreign_civ & 0x1f) & (uint)game.human_players) != 0) &&
        (DIFF_PRINCE < game.difficulty)) &&
       ((attitude = civ_get_attitude(civ,dialog_text_font_obj_block.adjacent_foreign_civ), 0x3c < attitude &&
        ((game.power_rank[civ] < game.power_rank[dialog_text_font_obj_block.adjacent_foreign_civ] &&
         (roll = rand_range(0,3), roll == 0)))))) {
      treaty = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + dialog_text_font_obj_block.adjacent_foreign_civ * 4);
      *treaty = *treaty | 0x20;
    }
  }
}

/* Part of ai_unit_choose_orders (00538a29): a badly damaged unit (task D): an
   attacker or defender holds where it defends well (a fortress or good
   terrain) unless in danger far from home; else it heads home, or holds in a
   fortress; when home is no help it goes on with the usual orders. */
static int ai_orders_badly_damaged(AiUnitOrders *o)
{
  int &city7 = o->city7; uptr &city3 = o->city3; iptr &unit = o->unit;
  int &in_enemy_zoc = o->in_enemy_zoc; int &unit_domain = o->unit_domain;
  int &ally_territory = o->ally_territory; uint &ai_role = o->ai_role;
  uptr &best_dir = o->best_dir; uint &on_fortress = o->on_fortress;
  uptr &terrain2 = o->terrain2; int &own_dist = o->own_dist;
  units[unit].ai_task = 'D';
  if (((unit_domain == 0) && ((int)ai_role < 2)) &&
     (((((units[unit].flags & UNITF_AI_GARRISON) != 0 ||
        (2 < (char)terrain_types[terrain2].defense)) &&
       ((((ai_role == 1 ||
          (city7 = unit_max_moves(unit), city7 <= (int)(uint)cosmic.road_move_mult))
         || (in_enemy_zoc == 0)) || (8 < own_dist)))) && (ally_territory == 0)))) {
    best_dir = 8;
    city3 = best_dir;
  }
  else {
    if (((unit_domain != 0) || (on_fortress == 0)) || (ally_territory != 0)) {
      if (((ai_role != 4) || (city7 = unit_stack_sum(unit,2), city7 < 2)) &&
         ((unit_go_home(unit), units[unit].orders == ORDER_GOTO ||
          (city7 = unit_moves_left(unit), city7 == 0)))) return AI_ORDERS_FINISH_ORDERS;
      return AI_ORDERS_SCENARIO_GUARD_CHECK;
    }
    best_dir = 8;
    city3 = best_dir;
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): a damaged unit that may go on:
   with all its moves, outside friendly territory, a land unit (out of enemy
   zones) or a lone or slightly damaged ship (no human watching) stays to heal
   (task d); a damaged warship or lone transport heads home. */
static int ai_orders_damaged(AiUnitOrders *o)
{
  uptr &civ = o->civ; int &city7 = o->city7; uptr &city3 = o->city3;
  uint &policy = o->policy; bool &cond = o->cond; iptr &unit = o->unit;
  int &in_enemy_zoc = o->in_enemy_zoc; int &unit_domain = o->unit_domain;
  int &ally_territory = o->ally_territory; int &y3 = o->y3;
  uint &ai_role = o->ai_role; uint &damage_level = o->damage_level;
  int &x2 = o->x2; uptr &best_dir = o->best_dir;
  int &has_orders = o->has_orders; int &own_dist = o->own_dist;
  units[unit].flags = units[unit].flags & ~UNITF_AI_RETREATING;
  if (((((damage_level != 0) && (units[unit].moves_used == '\0')) && (ally_territory == 0)) &&
      ((unit_domain == 0 ||
       (((unit_domain == 2 && (city7 = unit_stack_sum(unit,2), city7 < 2)) &&
        ((damage_level == 1 || ((int)(uint)units[unit].damage >> 1 <= own_dist / 2)))))))) &&
     (((unit_domain != 2 || ((game.human_players & units[unit].visibility) == 0)) &&
      ((damage_level == 3 ||
       (((int)(char)unit_types[units[unit].type].role != policy || (unit_domain == 2)))))))) {
    if (unit_domain == 0) {
      cond = in_enemy_zoc == 0;
    }
    else {
      city7 = tile_adjacent_nonallied_unit(x2,y3,civ);
      if (city7 == 0) {
        cond = true;
      }
      else {
        cond = false;
      }
    }
    if (cond) {
      units[unit].ai_task = 'd';
      best_dir = 8;
      city3 = best_dir;
      return AI_ORDERS_APPLY_DIR;
    }
  }
  if (((damage_level == 2) && (unit_domain == 2)) &&
     ((ai_role == 2 ||
      ((ai_role == 4 && (city7 = unit_stack_sum(unit,2), city7 < 2)))))) {
    unit_go_home(unit);
    city7 = unit_moves_left(unit);
    if (city7 == 0) return AI_ORDERS_FINISH_ORDERS;
    if (units[unit].orders == ORDER_GOTO) {
      has_orders = 1;
      return AI_ORDERS_PLAN_MOVE;
    }
  }
  return AI_ORDERS_NEXT;
}

/* Part of ai_unit_choose_orders (00538a29): the orders set: a unit left
   without orders (or fortified, sentried) uses up its moves (an air unit one
   move) and fortifies or sentries if a land unit (not a settler); a unit at
   its goto's end stops there (a warship with a transport keeps its moves). */
static i32 ai_orders_finish(AiUnitOrders *o)
{
  bool &stay_home = o->stay_home; int &near_dist = o->near_dist;
  iptr &unit = o->unit; int &unit_domain = o->unit_domain;
  if ((((units[unit].orders == ORDER_NONE) || (units[unit].orders == ORDER_AI_IDLE)) ||
      (units[unit].orders == ORDER_FORTIFYING)) || (units[unit].orders == ORDER_FORTIFIED)) {
    if (units[unit].orders == ORDER_AI_IDLE) {
      units[unit].ai_task = '0';
    }
    if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
      units[unit].orders = ORDER_NONE;
      units[unit].moves_used += cosmic.road_move_mult;
      return 1;
    }
    unit_exhaust_moves(unit);
    if ((unit_domain == 0) && (unit_types[units[unit].type].role != ROLE_SETTLE)) {
      units[unit].orders = ORDER_FORTIFYING;
      if ((units[unit].flags & UNITF_AI_GARRISON) != 0) {
        units[unit].orders = ORDER_FORTIFIED;
      }
      if (stay_home) {
        units[unit].orders = ORDER_NONE;
      }
    }
    else {
      units[unit].orders = ORDER_NONE;
    }
  }
  if (((units[unit].orders == ORDER_GOTO) && (units[unit].goto_x == units[unit].x)) &&
     (units[unit].y == units[unit].goto_y)) {
    units[unit].flags = units[unit].flags | UNITF_GOAL_REACHED;
    units[unit].orders = ORDER_NONE;
    if (units[unit].moves_used != '\0') {
      if (unit_types[units[unit].type].role == ROLE_NAVAL_SUPERIORITY) {
        near_dist = unit_stack_count_role(unit,ROLE_SEA_TRANSPORT);
        if (near_dist != 0) {
          unit_exhaust_moves(unit);
        }
      }
      else {
        unit_exhaust_moves(unit);
      }
    }
  }
  return 0;
}

/* 00538a29 auto */
i32 __cdecl ai_unit_choose_orders(void)

{
  Tile *map_tile;
  u8 stat;
  ushort military;
  bool forced_attack;
  bool stay_home;
  byte tile_byte;
  uptr civ;
  i32 result;
  iptr landmass;
  int city7;
  uptr city3;
  iptr x;
  iptr x5;
  int y;
  byte owner_byte;
  uint terrain;
  byte *tile;
  int check;
  int near_dist;
  uptr unit_type;
  uint policy;
  bool is_settler;
  bool settler_type;
  bool cond;
  i32 arg_hi;
  iptr unit2;
  uint x4;
  int goal_match;
  iptr unit;
  uptr city6;
  int in_enemy_zoc;
  int civ4;
  int unit_domain;
  uptr city;
  uptr needed_defenders;
  int is_attack;
  int near_landmass;
  int max_score;
  int fort_need;
  int ally_territory;
  int y3;
  uint ai_role;
  int needs_units;
  uint damage_level;
  int x2;
  uptr unit4;
  uptr best_dir;
  uint flag;
  uint on_fortress;
  int goto_city;
  int attack_bonus;
  uint room_aboard;
  int has_orders;
  u32 type_flags;
  int defender_count;
  iptr unit5;
  int retried;
  int hold_pos;
  uptr city4;
  uptr terrain2;
  uptr dir2;
  uptr civ2;
  uint y2;
  int cand_score;
  int explore;
  int attack_cost;
  uptr dir;
  int own_landmass;
  iptr unit3;
  int will_attack;
  int is_guard;
  uptr city5;
  uptr city2;
  int own_dist;
  int best_score;
  int freed_count;
  uint target_policy;
  int no_moves;
  int here_landmass;
  int score;
  uint tile_flags;
  uint tasks_mask;
  int next;  /* where a part says to go on */
  AiUnitOrders st = {map_tile, stat, military, forced_attack, stay_home,
    tile_byte, civ, result, landmass, city7, city3, x, x5, y, owner_byte,
    terrain, tile, check, near_dist, unit_type, policy, is_settler,
    settler_type, cond, arg_hi, unit2, x4, goal_match, unit, city6,
    in_enemy_zoc, civ4, unit_domain, city, needed_defenders, is_attack,
    near_landmass, max_score, fort_need, ally_territory, y3, ai_role,
    needs_units, damage_level, x2, unit4, best_dir, flag, on_fortress,
    attack_bonus, room_aboard, has_orders, type_flags, defender_count, unit5,
    retried, hold_pos, city4, terrain2, dir2, civ2, y2, cand_score, explore,
    attack_cost, dir, own_landmass, unit3, will_attack, is_guard, city5, city2,
    own_dist, best_score, freed_count, target_policy, no_moves, here_landmass,
    score, tile_flags, tasks_mask};
  defender_count = 0x108; needed_defenders = 0x64a8c19c; cand_score = 0x3bd; best_score = 0; goal_match = 4;  /* stale stack the original reads (FRAME_PRESETS) */
  needs_units = 0x21f238; target_policy = 0x21f2dc;  /* stale stack the original reads (FRAME_PRESETS; CIV2_STACKPROBE=538a29:dc:4 and 538a29:2c:4, civilwar) */
  if (bugfix_state()) {
    goal_match = 0;  /* B13: a ship's cargo flag starts set from stale stack (docs/BUGS.md) */
    needed_defenders = 0;  /* B19: away from its own city a ship keeps every defender aboard fortified (docs/BUGS.md) */
    needs_units = 0;  /* B24: a ship with no port to go to makes its own city a ferry port for stale stack (docs/BUGS.md) */
  }
  
  near_landmass = -1;
  own_landmass = -1;
  settler_type = false;
  is_settler = false;
  retried = 0;
  hold_pos = 0;
  stay_home = false;
  has_orders = 0;
  forced_attack = false;
  no_moves = 0;
  is_guard = 0;
  goto_city = 0;
  unit = (iptr)game.active_unit;
  owner_byte = units[unit].owner;
  civ = (uptr)(char)owner_byte;
  if (civ == 0) {
    result = ai_barbarian_unit_move();
    return result;
  }
  if ((3 < units[unit].orders) && (units[unit].orders < '\n')) goto finish_orders;
  x2 = (int)units[unit].x;
  y3 = (int)units[unit].y;
  unit_type = (uptr)units[unit].type;
  best_dir = 8;
  near_dist = map_in_bounds(x2,y3);
  if (near_dist == 0) {
    unit_set_sleep(unit);
    goto finish_orders;
  }
  in_enemy_zoc = tile_in_enemy_zoc(x2,y3,civ);
  city4 = city_find_nearest(x2,y3,-1,0xffffffff,-1);
  near_dist = advisor_window_block.nearest_city_dist;
  if (-1 < (int)city4) {
    near_landmass = tile_land_landmass((int)cities[city4].x,(int)cities[city4].y);
  }
  city2 = city_find_nearest(x2,y3,civ,0xffffffff,-1);
  own_dist = advisor_window_block.nearest_city_dist;
  if (-1 < (int)city2) {
    own_landmass = tile_land_landmass((int)cities[city2].x,(int)cities[city2].y);
  }
  tile_byte = tile_terrain(x2,y3);
  terrain2 = (uptr)tile_byte;
  landmass = tile_landmass(x2,y3);
  result = tile_improvements(x2,y3);
  on_fortress = (uint)(((byte)result & 0x42) == 0x40);
  if (((8 < own_dist) || (landmass != own_landmass)) &&
     ((1 << (owner_byte & 0x1f) & (uint)game.human_players) == 0)) {
    units[unit].flags = units[unit].flags & ~UNITF_AI_COLONIST;
  }
  ai_role = ai_unit_role(unit);
  type_flags = unit_types[units[unit].type].flags;
  unit_domain = (int)(char)unit_types[units[unit].type].domain;
  policy = (uint)civs[civ].continent_policy[landmass];
  ai_orders_trespassers(&st);
  city3 = city4;
  if (((((near_dist == 1) && (city2 != city4)) && (unit_domain == DOMAIN_LAND)) &&
      (((char)unit_types[units[unit].type].role < 6 &&
       (city7 = tile_unit_owner((int)cities[city4].x,(int)cities[city4].y), city7 < 0)))) &&
     ((*(byte *)&civs[civ].treaties[cities[city3].owner] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) {
    units[unit].orders = ORDER_NONE;
    in_enemy_zoc = 1;
    dialog_text_font_obj_block.adjacent_foreign_civ = (iptr)cities[city3].owner;
    forced_attack = true;
  }
  damage_level = (uint)(units[unit].damage != '\0');
  if ((int)(char)unit_types[unit_type].hit_points >> 2 < (int)(uint)units[unit].damage) {
    damage_level = 2;
  }
  if ((int)(char)unit_types[unit_type].hit_points >> 1 < (int)(uint)units[unit].damage) {
    damage_level = 3;
  }
  if ((unit_types[units[unit].type].hold != '\0') &&
     (city7 = unit_moves_left(unit), city7 == 0)) {
    no_moves = 1;
    goto load_transport_loop;
  }
  if (damage_level != 0) {
    ally_territory = 0;
    civ2 = tile_radius_civ(x2,y3);
    if ((((0 < (int)civ2) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0)) &&
        ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0)) &&
       ((int)((uint)civs[civ2].reputation - (int)civs[civ].reputation_credit[civ2])
        < 6)) {
      ally_territory = 1;
    }
    if (near_dist == 0) {
      hold_pos = 1;
      is_guard = 1;
      goto guard_check;
    }
    if (near_dist == 1) {
      if (city4 == city2) {
        unit_set_goto(unit,100,(int)cities[city4].x,(int)cities[city4].y);
        goto finish_orders;
      }
      if (forced_attack) {
        damage_level = 0;
      }
    }
  }
  if ((((((damage_level == 0) || (units[unit].moves_used != '\0')) ||
        ((ally_territory != 0 || ((unit_domain != DOMAIN_AIR || (unit_types[units[unit].type].range != '\0')))))
        ) || (city7 = tile_adjacent_nonallied_unit(x2,y3,civ), city7 != 0)) ||
      ((own_dist < 0xd || (units[unit].visibility != '\0')))) ||
     (((damage_level != 2 && ((units[unit].flags & UNITF_AI_RETREATING) == 0)) &&
      ((((damage_level != 3 || (own_dist == 0)) || (own_dist < 0x18)) || (near_dist < 9)))))) {
    switch (ai_orders_damaged(&st)) {
    case AI_ORDERS_APPLY_DIR: goto apply_dir;
    case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
    case AI_ORDERS_PLAN_MOVE: goto plan_move;
    default: break;
    }
    if (damage_level == 3) {
      switch (ai_orders_badly_damaged(&st)) {
      case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
      case AI_ORDERS_SCENARIO_GUARD_CHECK: goto scenario_guard_check;
      default: break;
      }
    }
    else {
scenario_guard_check:
      if (((((game.game_flags & GAME_SCENARIO) == 0) || (((int)(short)scenario.flags[0] & 0x8000U) == 0)) ||
          (civ != 1)) ||
         (((((*((char *)&civs[1].treaties[3] + 1)) & (TREATY_WAR >> 8)) != 0 || (5 < game.turn)) ||
          (0x4f < units[unit].x)))) {
        if (((unit_domain == DOMAIN_LAND) && (own_dist == 0)) &&
           (((units[unit].flags & UNITF_AI_GARRISON) == 0 &&
            (((cities[city2].flags & CITYF_AI_FERRY_PORT) != 0 && (policy != ai_role)))))) {
          stay_home = true;
        }
        else {
          if (unit_domain == DOMAIN_AIR) {
            unit4 = ai_air_unit_orders(unit,unit_type,(iptr)&x2,(iptr)&y3,ai_role,
                                          (iptr)&has_orders,own_dist,landmass,in_enemy_zoc);
            if (unit4 == 0xfffffffe) goto plan_move;
            if (unit4 == 0xffffffff) goto finish_orders;
            city3 = unit4;
            if (-1 < (int)unit4) goto apply_dir;
          }
          if ('b' < (char)unit_types[unit_type].attack) {
            city3 = ai_nuke_pick_target(unit);
            x2 = (int)units[unit].x;
            y3 = (int)units[unit].y;
            goto apply_dir;
          }
          if (ai_role == 6) {
            switch (ai_orders_diplomat(&st)) {
            case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
            case AI_ORDERS_PLAN_MOVE: goto plan_move;
            case AI_ORDERS_RETURN_1: return 1;
            default: break;
            }
          }
          if (ai_role == 7) {
            switch (ai_orders_caravan(&st)) {
            case AI_ORDERS_APPLY_DIR: goto apply_dir;
            case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
            case AI_ORDERS_RETURN_1: return 1;
            default: break;
            }
          }
          switch (ai_orders_city_site(&st)) {
          case AI_ORDERS_APPLY_DIR: goto apply_dir;
          case AI_ORDERS_RETURN_1: return 1;
          default: break;
          }
          if ((unit_types[units[unit].type].flags & UTYPE_PARADROP) != 0) {
            switch (ai_orders_paratrooper(&st)) {
            case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
            case AI_ORDERS_PLAN_MOVE: goto plan_move;
            case AI_ORDERS_RETURN_0: return 0;
            default: break;
            }
          }
        }
        ai_orders_partisan_fortress(&st);
        if ((((on_fortress == 0) || (unit4 = tile_radius_civ(x2,y3), (int)unit4 < 1))
            || (unit4 == civ)) ||
           ((((*(byte *)&civs[civ].treaties[unit4] & TREATY_PEACE) != 0 &&
             ((int)((uint)civs[unit4].reputation -
                   (int)civs[civ].reputation_credit[unit4]) < 7)) &&
            (((*(byte *)&civs[civ].treaties[unit4] & TREATY_ALLIANCE) != 0 || (2 < own_dist)))))) {
guard_check:
          ai_orders_city_guard(&st);
          if (is_guard == 0) {
            if (stay_home) {
              hold_pos = 1;
            }
            if (hold_pos == 0) {
load_transport_loop:
              for (;;) {
                if (unit_domain == DOMAIN_SEA) {
                  ai_orders_transport_cargo(&st);
                  if (ai_role == 2) {
                    civ_clear_standing_goals_near(civ,ROLE_NAVAL_SUPERIORITY,x2,y3,0);
                    civ_clear_standing_goals_near(civ,0x12,x2,y3,0);
                  }
                  if (ai_role == 4) {
                    switch (ai_orders_transport(&st)) {
                    case AI_ORDERS_APPLY_DIR: goto apply_dir;
                    case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
                    default: break;
                    }
                  }
                  if (no_moves != 0) {
                    return 1;
                  }
                  if ((ai_role == 4) || (in_enemy_zoc == 0)) {
                    ai_orders_ship_goal_war(&st);
                    ai_orders_ship_goal_trade(&st);
                    ai_orders_ship_goal_land(&st);
                  }
                }
                if (ai_orders_pillage(&st) == AI_ORDERS_FINISH_ORDERS) goto finish_orders;
                next = ai_orders_partisan(&st);
                if (next == AI_ORDERS_APPLY_DIR) goto apply_dir;
                if (next == AI_ORDERS_BREAK) break;
                ai_orders_fortress_site(&st);
                if (((ai_role == 5) && (units[unit].orders == ORDER_GOTO)) &&
                   (city7 = tile_city_owner((int)units[unit].goto_x,
                                             (int)units[unit].goto_y), -1 < city7)) {
                  goto_city = 1;
                }
                if (((settler_type) && (!forced_attack)) &&
                   ((goto_city == 0 &&
                    ((city7 = tile_is_ocean(x2,y3), city7 == 0 &&
                     (city7 = tile_city_owner(x2,y3), city7 < 0)))))) {
                  switch (ai_orders_settler_work(&st)) {
                  case AI_ORDERS_APPLY_DIR: goto apply_dir;
                  case AI_ORDERS_FINISH_ORDERS: goto finish_orders;
                  default: break;
                  }
                }
                if (((ai_role == 0x15) && (units[unit].orders != ORDER_GOTO)) &&
                   ((!forced_attack &&
                    (((goto_city == 0 && (own_dist != 0)) &&
                     (city7 = tile_is_ocean(x2,y3), city7 == 0)))))) {
                  if (ai_orders_terraformer(&st) == AI_ORDERS_FINISH_ORDERS) goto finish_orders;
                }
                next = ai_orders_keep_goto(&st);
                if (next == AI_ORDERS_CONTINUE) continue;
                if (next == AI_ORDERS_FINISH_ORDERS) goto finish_orders;
                next = ai_orders_defender_fortify(&st);
                if (next == AI_ORDERS_APPLY_DIR) goto apply_dir;
                if (next == AI_ORDERS_BREAK) break;
                if (((is_settler) &&
                    (landmass_table_block.landmass_recs[landmass].fertile_tiles >> 3 <
                     (short)(ushort)civs[civ].continent_cities[landmass])) &&
                   (units[unit].home_city != -1)) {
                  if ((own_dist == 0) && (cities[city2].size < '\n')) {
                    cities[city2].size++;
                    unit_destroy_redraw(unit,1);
                    ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city2);
                    map_redraw_area_all_views(x2,y3,1,1,1);
                    return 1;
                  }
                  if (((-1 < (int)city2) && (landmass == own_landmass)) && (cities[city2].size < '\n')) {
                    unit_set_goto(unit,0x42,(int)cities[city2].x,(int)cities[city2].y);
                    goto finish_orders;
                  }
                }
                break;
              }
            }
          }
          else {
            unit4 = (uptr)(char)unit_types[units[unit].type].defense;
            city7 = city_has_improvement(city2,IMP_CITY_WALLS);
            if (city7 == 0) {
              if ((units[unit].flags & UNITF_AI_GARRISON) != 0) {
                unit4 = (int)(unit4 * 3) / 2;
              }
            }
            else {
              unit4 = unit4 << 1;
            }
            if ((int)(char)unit_types[units[unit].type].attack < (int)unit4) {
              units[unit].ai_task = 'F';
              units[unit].home_city = (i8)city2;
              stay_home = false;
              best_dir = 8;
              city3 = best_dir;
              goto apply_dir;
            }
            hold_pos = 1;
          }
        }
        else {
          if (ai_role != 1) {
            if (ai_role == 0) {
              if (in_enemy_zoc != 0) {
                hold_pos = 1;
              }
              city7 = unit_stack_sum(unit,2);
              if (((city7 == 1) && (-1 < unit_types[units[unit].type].obsolete_by)) &&
                 (game.advance_known_by[unit_types[units[unit].type].obsolete_by] != '\0')) {
                hold_pos = 1;
              }
              if (hold_pos != 0) goto plan_move;
            }
            goto guard_check;
          }
          city7 = unit_stack_count_role(unit,ROLE_DEFEND);
          if ((1 < city7) && (units[unit].orders == ORDER_GOTO)) goto guard_check;
          units[unit].ai_task = 'F';
          hold_pos = 1;
        }
plan_move:
        if (ai_orders_plan_move(&st) == AI_ORDERS_FINISH_ORDERS) goto finish_orders;
      }
      else {
        best_dir = 8;
        city3 = best_dir;
      }
    }
  }
  else {
    units[unit].flags = units[unit].flags | UNITF_AI_RETREATING;
    units[unit].ai_task = 'd';
    best_dir = 8;
    city3 = best_dir;
  }
apply_dir:
  best_dir = city3;
  if (best_dir == 8) {
    units[unit].last_dir = 0xff;
    units[unit].orders = ORDER_NONE;
  }
  else {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[best_dir] + x2);
    y3 += (((i8 *)&dir_dy_table_block.dir_dy_table))[best_dir];
    near_dist = map_in_bounds(x2,y3);
    if (near_dist == 0) {
      units[unit].orders = ORDER_NONE;
      unit_exhaust_moves(unit);
    }
    else {
      units[unit].orders = ORDER_AI_GOTO;
      units[unit].goto_x = (i16)x2;
      units[unit].goto_y = (i16)y3;
    }
  }
finish_orders:
  return ai_orders_finish(&st);
}

/* 00543cd6 auto */
void __cdecl ai_process_civ_units(void)

{
  bool naval_first_turn;
  i16 prev_num_units;
  int civ;
  int result;
  iptr unit;
  int order_steps;
  int pass;
  
  civ = (int)game.turn_civ;
  naval_first_turn = false;
  if (((((game.game_flags & GAME_SCENARIO) != 0) && (((int)(*(short *)&scenario.flags) & 0x8000U) != 0)) && (game.turn == 1)
      ) && (scenario.objective_civ == civ)) {
    naval_first_turn = true;
  }
  pass = 0;
  do {
    if (1 < pass) {
      return;
    }
    unit = (iptr)game.num_units;
    while (unit = unit - 1, -1 < unit) {
      if (2 < game.multiplayer) {
        if (net.host_handoff_stage != '\0') {
          return;
        }
        if (net.player_has_quit != '\0') {
          return;
        }
      }
      if (((units[unit].id != 0) && (units[unit].owner == civ)) &&
         ((order_steps = 0, !naval_first_turn ||
          ((pass != 0 || (unit_types[units[unit].type].domain == DOMAIN_AIR)))))) {
        while (result = unit_is_awake_with_moves(unit), result != 0) {
          if (2 < game.multiplayer) {
            net_send_state_diff(0xff,2,0,0,0);
            XD_FlushSendBuffer(5000);
            net.process_messages(1,0);
          }
          prev_num_units = game.num_units;
          game.active_unit = (i16)unit;
          unit_continue_active_orders();
          if (game.num_units != prev_num_units) break;
          if ((units[unit].orders != ORDER_GOTO) &&
             (unit_types[units[unit].type].domain != DOMAIN_AIR)) {
            order_steps++;
          }
          order_steps++;
          if (0x14 < order_steps) {
            unit_exhaust_moves(unit);
            break;
          }
          if ((((pass == 0) && (civ != 0)) && (unit_types[units[unit].type].hold != '\0'))
             && (((result = unit_moves_left(unit), result == 0 &&
                  (result = tile_is_ocean((int)units[unit].x,(int)units[unit].y), result != 0)
                  ) && ((result = unit_stack_sum(unit,2), 1 < result &&
                        (((game.human_players & units[unit].visibility) != 0 ||
                         (result = tile_adjacent_nonallied_civ((int)units[unit].x,(int)units[unit].y,
                                                     civ), result != 0)))))))) {
            unit_continue_active_orders();
          }
        }
        mapviews_redraw_cursor();
      }
    }
    pass++;
  } while( true );
}

/* 0057febc auto */
void __cdecl ai_respond_to_attack(iptr civ,iptr x,iptr y)

{
  int civ2;
  int target_landmass;
  int result;
  iptr unit;
  
  unit = 0;
  while( true ) {
    if (game.num_units <= unit) {
      civ2 = tile_owner(x,y);
      target_landmass = tile_landmass(x,y);
      for (unit = 0; unit < game.num_units; unit++) {
        if (((units[unit].id != 0) &&
            ((((units[unit].owner == civ ||
               (((units[unit].owner != civ2 &&
                 ((*(byte *)&civs[civ].treaties[units[unit].owner] & TREATY_ALLIANCE) != 0)) &&
                (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) == 0)))) &&
              (((unit_types[units[unit].type].domain == DOMAIN_LAND &&
                (unit_types[units[unit].type].attack != '\0')) &&
               (result = unit_moves_left(unit), result != 0)))) &&
             ((unit_types[units[unit].type].role != ROLE_DEFEND ||
              (result = tile_city_owner((int)units[unit].x,(int)units[unit].y), result < 0))))
            )) && ((result = map_distance(x,y,(int)units[unit].x,
                                               (int)units[unit].y), result < 7 &&
                   (result = tile_landmass((int)units[unit].x,(int)units[unit].y),
                   result == target_landmass)))) {
          pathfind.owner_civ = (int)units[unit].owner;
          pathfind.unit_type = (iptr)units[unit].type;
          path_queue.goal_x = x;
          path_queue.goal_y = y;
          result = unit_moves_left(unit);
          result = map_path_next_dir((int)units[unit].x,(int)units[unit].y,result * 2);
          if ((0 < result) && (result != 8)) {
            units[unit].orders = ORDER_GOTO;
            units[unit].goto_x = (i16)x;
            units[unit].goto_y = (i16)y;
            units[unit].ai_task = 'K';
          }
        }
      }
      return;
    }
    if (((((units[unit].id != 0) && (units[unit].owner == civ)) &&
         ((unit_types[units[unit].type].flags & UTYPE_PARADROP) != 0)) &&
        (civ2 = tile_city_owner((int)units[unit].x,(int)units[unit].y), -1 < civ2)) &&
       (((units[unit].moves_used == '\0' || ((units[unit].flags & UNITF_AI_GARRISON) != 0)) &&
        (((units[unit].flags & UNITF_PARADROPPED) == 0 &&
         (civ2 = map_distance((int)units[unit].x,(int)units[unit].y,x,y),
         civ2 <= (int)(uint)cosmic.paradrop_range)))))) break;
    unit++;
  }
  unit_paradrop(unit,x,y);
  return;
}

/* 00598d45 auto */
i32 __cdecl ai_spaceship_race_behind(iptr civ)

{
  iptr race_active;
  iptr launched;
  int result;
  int own_progress;
  int part;
  int rival_progress;
  int civ2;
  
  race_active = spaceship_race_active();
  if ((((race_active != 0) && (launched = civ_spaceship_launched(civ), launched == 0)) &&
      ((game.game_flags & GAME_WON) == 0)) &&
     ((game.difficulty != DIFF_CHIEFTAIN && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0))))
  {
    for (civ2 = 1; civ2 < 8; civ2++) {
      if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
         (result = civ_spaceship_launched(civ2), result != 0)) {
        return 1;
      }
    }
    own_progress = 0;
    for (part = 0; part < 6; part++) {
      result = int_clamp((part + 1) / 2,0,2);
      own_progress = own_progress +
                 (int)civs[civ].spaceship_parts[part] *
                 (uint)improvements[result + 0x23].cost;
    }
    for (civ2 = 1; civ2 < 8; civ2++) {
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (result = civ_has_advance(civ2,ADV_SPACE_FLIGHT), result != 0)) && (civ2 != civ)) {
        rival_progress = 0;
        for (part = 0; part < 6; part++) {
          result = int_clamp((part + 1) / 2,0,2);
          rival_progress = rival_progress + (int)civs[civ2].spaceship_parts[part] *
                              (uint)improvements[result + 0x23].cost;
        }
        if (own_progress <= rival_progress) {
          return 1;
        }
      }
    }
  }
  return 0;
}

/* 005b68f6 auto */
i32 __cdecl ai_tile_improve_choice(iptr civ,iptr x,i32 y)

{
  byte terrain_byte;
  int result;
  uint improvements_mask;
  uint terrain;
  iptr level;
  
  result = tile_city_owner(x,y);
  if (((result < 0) && (improvements_mask = tile_improvements(x,y), (improvements_mask & TILE_MINE) == 0)) &&
     (((improvements_mask & TILE_IRRIGATION) == 0 || (result = civ_has_advance(civ,ADV_REFRIGERATION), result != 0)))) {
    terrain_byte = tile_terrain(x,y);
    terrain = (uint)terrain_byte;
    for (level = 0; level < 2; level++) {
      if (level == 0) {
        result = tile_has_irrigation_source(x,y);
        if (result == 0) {
          return 0;
        }
      }
      else if ((improvements_mask & TILE_IRRIGATION) != 0) {
        return 0;
      }
      if (((terrain_types[terrain].improve_to[level] != -1) &&
          ((char)terrain_types[terrain].improve_ai_govt[level] != '\0')) &&
         ((int)(char)terrain_types[terrain].improve_ai_govt[level] <= (int)(uint)civs[civ].government))
      {
        return level + 1;
      }
    }
  }
  return 0;
}
