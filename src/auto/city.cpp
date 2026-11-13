/* The game, cities: growth, production, happiness, the city window and city view: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004100cf auto */
void __cdecl city_info_popup(int city)

{
  iptr adjective;
  int objective_value;
  uptr commodity;
  iptr i;
  Dialog dialog2;
  Sprite sprite;
  uptr demand;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite.create_solid(0x40,0x30,0);
  text_set_string_arg(0,(const char *)&cities[city].name);
  text_clear();
  adjective = civ_get_adjective(cities[city].owner);
  text_append((const char *)adjective);
  if (((game.game_flags & GAME_SCENARIO) != 0) && ((scenario.flags[0] & 2) != 0)) {
    objective_value = city_objective_value(city);
    if (objective_value != 0) {
      text_append_space();
      text_append_label(0x1b0);
      if (1 < objective_value) {
        text_append((const char *)&str_x_2);
        text_append_int(objective_value);
      }
    }
  }
  text_set_string_arg(1,(const char *)&textbuf.buf);
  for (i = 0; i < 3; i++) {
    demand = (uptr)*(char *)&cities[city].demands[i];
    text_clear();
    if ((int)demand < 0) {
      text_append_lparen();
    }
    if ((int)demand < 1) {
      commodity = ~demand + 1;
    }
    else {
      commodity = demand;
    }
    text_append_string_id(commodity_names[commodity]);
    if ((int)demand < 0) {
      text_append_rparen();
    }
    text_set_string_arg(i + 2,(const char *)&textbuf.buf);
  }
  popup_game_with_city((const char *)&str_cityinfo,city);
  return;
}

/* 00414e00 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 CityWindow::city_window_is_closed()

{
  return ((u32)is_closed);
}

/* 0043cc00 auto */
void __cdecl city_reveal_to_civ(int city,int civ)

{
  if (-1 < civ) {
    cities[city].known_to = cities[city].known_to | (byte)(1 << ((byte)civ & 0x1f));
    *(i8 *)&cities[city].revealed_size[civ] = cities[city].size;
  }
  return;
}

/* 0043cc7e auto */
int __cdecl city_population_units(int city)

{
  int total;
  int i;
  
  total = 0;
  for (i = 1; i <= cities[city].size; i++) {
    total += i;
  }
  if (total < 2) {
    total = 1;
  }
  return total;
}

/* 0043cef9 auto */
uint __cdecl city_objective_value(int city)

{
  uint value;
  iptr wonder;
  
  value = (uint)((cities[city].flags & CITYF_OBJECTIVE) != 0);
  if ((scenario.flags[0] & 4) != 0) {
    for (wonder = 0; wonder < 0x1c; wonder++) {
      if (game.wonder_city[wonder] == city) {
        value++;
      }
    }
  }
  return value;
}

/* 0043cf76 auto */
i32 __cdecl city_at(int x,int y)

{
  int result;
  iptr city;
  
  result = map_in_bounds(x,y);
  if ((result != 0) && (result = tile_city_owner(x,y), -1 < result)) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].x == x)) &&
         (cities[city].y == y)) {
        return city;
      }
    }
  }
  return 0xffffffff;
}

/* 0043d07a auto */
int __cdecl city_find_nearest(int x,int y,int owner_civ,int target_landmass,int civ)

{
  short x2;
  short y2;
  int dist;
  int landmass2;
  iptr nearest;
  iptr city;
  
  nearest = -1;
  advisor_window_block.nearest_city_dist = 9999;
  if ((-1 < target_landmass) && (landmass2 = tile_land_landmass(x,y), landmass2 < 0)) {
    target_landmass = -1;
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      x2 = cities[city].x;
      y2 = cities[city].y;
      if (((((target_landmass < 0) || (landmass2 == target_landmass)) &&
           (((owner_civ < 0 || ((int)cities[city].owner == (owner_civ & 0xffU))) ||
            ((-1 < civ && (cities[city].owner == civ)))))) &&
          ((target_landmass != -2 || (dist = city_has_improvement(city,IMP_PORT_FACILITY), dist != 0)))) &&
         (dist = map_distance_diag(x,y,(int)x2,(int)y2), dist <= advisor_window_block.nearest_city_dist))
      {
        nearest = city;
        advisor_window_block.nearest_city_dist = dist;
      }
    }
  }
  return nearest;
}

/* 0043d20a auto */
i32 __cdecl city_has_improvement(int city,int improvement_id)

{
  int improvement;
  byte bit_mask[4];
  i32 result;
  
  result = 0;
  if (((0 < improvement_id) && (improvement_id < 0x23)) &&
     (bit_index_split(improvement_id,&improvement,(int *)bit_mask),
     (bit_mask[0] & cities[city].improvements[improvement]) != 0)) {
    result = 1;
  }
  return result;
}

/* 0043d289: written by hand in src/game/regargs.cpp */

/* Part of city_calc_commodity_demand (0043d400): the terrain of the city radius:
   the count of each terrain, rivers and roads; the scores cleared. */
static void city_commodity_count_terrain(int &road_tiles,int terrain_counts[11],iptr supply_order[16],int &river_tiles,int y3,int x3,iptr demand_order[16])
{
  byte terrain_byte;
  iptr in_bounds;
  int city2;
  byte *tile;
  uint tile_flags;
  int y2;
  uptr terrain2;
  iptr x2;
  iptr i;

  river_tiles = 0;
  for (i = 0; i < 0xb; i++) {
    terrain_counts[i] = 0;
  }
  for (i = 0; i < 0x10; i++) {
    (((int *)&commodity_demand.by_commodity))[i] = 0;
    (((int *)&commodity_supply.by_commodity))[i] = (((int *)&commodity_demand.by_commodity))[i];
    demand_order[i] = i;
    supply_order[i] = demand_order[i];
  }
  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x3);
    y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y3;
    in_bounds = map_in_bounds(x2,y2);
    if (in_bounds != 0) {
      terrain_byte = tile_terrain(x2,y2);
      terrain2 = (uint)terrain_byte;
      terrain_counts[terrain_byte]++;
      city2 = tile_resource(x2,y2);
      if (city2 != 0) {
        terrain_counts[terrain2] += 3;
      }
      tile = (byte *)tile_at(x2,y2);
      if ((*tile & 0x80) != 0) {
        river_tiles++;
      }
      tile_flags = tile_improvements(x2,y2);
      if ((tile_flags & TILE_ROAD) != 0) {
        road_tiles++;
      }
    }
  }
  terrain_counts[6] += terrain_counts[7];
  terrain_counts[7] = 0;
}

/* Part of city_calc_commodity_demand (0043d400): the supply score of each
   commodity, from the terrain, the advances, the size and the place. */
static void city_commodity_supply_scores(uptr city,iptr civ,int terrain_counts[11],int river_tiles,int size_class,uint landmass2,uint south_dist,uint advance_count,uint center_dx,uint east_dist,uint center_dy)
{
  int city2;
  i32 shift;
  int cloth_base;
  int wool_factor;
  iptr wine_bonus;
  int wine_base;

  commodity_supply.by_commodity[0] = terrain_counts[9] * 3 + terrain_counts[6] * 6 + terrain_counts[3] * 4 + river_tiles * 3;
  if ((int)advance_count < 0x10) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] * 2;
  }
  if ((int)advance_count < 0x18) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] << 1;
  }
  if ((int)advance_count < 0x31) {
    if (cities[city].size < 3) {
      commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] << 1;
    }
  }
  else {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] / 2;
  }
  if (7 < cities[city].size) {
    commodity_supply.by_commodity[0] = commodity_supply.by_commodity[0] / 2;
  }
  wool_factor = terrain_counts[6] + 2;
  if ((int)map.height / 3 < (int)center_dy) {
    wool_factor = terrain_counts[6] + 3;
  }
  commodity_supply.wool[0] = (river_tiles / 2 + terrain_counts[4] * 2 + terrain_counts[2]) * wool_factor;
  commodity_supply.beads = terrain_counts[10] * 8 - center_dy;
  if ('\t' < cities[city].size) {
    commodity_supply.beads = commodity_supply.beads >> 1;
  }
  if (0x20 < (int)advance_count) {
    commodity_supply.beads = commodity_supply.beads >> 1;
  }
  cloth_base = (terrain_counts[1] * 3 + terrain_counts[0]) - river_tiles;
  city2 = int_clamp((int)advance_count / 10,1,2);
  commodity_supply.cloth = cloth_base * city2;
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_supply.cloth = commodity_supply.cloth * 3 >> 1;
  }
  if ((int)advance_count < 8) {
    commodity_supply.cloth = commodity_supply.cloth >> 1;
  }
  if ((int)advance_count < 0x10) {
    commodity_supply.cloth = commodity_supply.cloth >> 1;
  }
  commodity_supply.salt = (terrain_counts[10] * 3 + terrain_counts[0] * 4 + terrain_counts[8] * 2) - (int)advance_count / 6;
  city2 = civ_has_advance(civ,ADV_POTTERY);
  if (city2 == 0) {
    commodity_supply.salt = commodity_supply.salt / 3;
  }
  city2 = city_has_improvement(city,IMP_AQUEDUCT);
  if (city2 != 0) {
    commodity_supply.salt += (commodity_supply.salt >> 1);
  }
  if (((landmass2 & 1) != 0) && ((int)landmass2 < 6)) {
    commodity_supply.salt += (commodity_supply.salt >> 1);
  }
  commodity_supply.coal = (terrain_counts[9] + terrain_counts[1] + terrain_counts[3] + terrain_counts[8] + 1) * terrain_counts[4] *
                 5;
  city2 = size_class / 2;
  if (1 < city2) {
    city2 = 2;
  }
  commodity_supply.coal = int_shift_signed(commodity_supply.coal,city2 - 1);
  if (((landmass2 & 1) != 0) && (1 < (int)landmass2)) {
    commodity_supply.coal += (commodity_supply.coal >> 1);
  }
  if ((int)advance_count < 0x14) {
    commodity_supply.coal = commodity_supply.coal >> 1;
  }
  commodity_supply.copper = terrain_counts[5] * 5 + terrain_counts[4] * 5;
  if ((landmass2 != 0) && ((landmass2 & 1) == 0)) {
    commodity_supply.copper = commodity_supply.copper * 2;
  }
  city2 = (terrain_counts[2] * 5 - terrain_counts[1]) + river_tiles;
  commodity_supply.dye = city2 * 2;
  if ((landmass2 != 0) && ((landmass2 & 3) == 0)) {
    commodity_supply.dye = city2 * 4;
  }
  wine_base = river_tiles * 5 - terrain_counts[2];
  wine_bonus = terrain_counts[1] << 2;
  wine_base = int_clamp(wine_base,0,wine_bonus);
  wine_bonus = int_clamp(wine_bonus,0,wine_base);
  commodity_supply.wine = wine_bonus + (int)(((int)map.width >> 1) - center_dx) / 2 + wine_base;
  if (south_dist != 0) {
    commodity_supply.wine = commodity_supply.wine >> 1;
  }
  shift = int_clamp(size_class - 1,0xfffffffe,1);
  commodity_supply.wine = int_shift_signed(commodity_supply.wine,shift);
  if ((landmass2 - 2 & 3) == 0) {
    commodity_supply.wine += (commodity_supply.wine >> 1);
  }
  if (civs[civ].tribe == 9) {
    commodity_supply.wine = commodity_supply.wine << 1;
  }
  if ('\n' < cities[city].size) {
    commodity_supply.wine = commodity_supply.wine >> 1;
  }
  commodity_supply.silk = (terrain_counts[3] * 2 + terrain_counts[9] + 1) * (terrain_counts[4] + 1);
  if (((commodity_supply.silk != 0) && (commodity_supply.silk = commodity_supply.silk + east_dist * 2, landmass2 != 0)) &&
     ((int)landmass2 % 5 == 0)) {
    commodity_supply.silk = commodity_supply.silk * 2;
  }
  if (civs[civ].tribe == 0xb) {
    commodity_supply.silk = commodity_supply.silk << 1;
  }
  commodity_supply.silver = terrain_counts[5] * 8;
  if (commodity_supply.silver != 0) {
    commodity_supply.silver = commodity_supply.silver + terrain_counts[4] + center_dx;
    city2 = civ_has_advance(civ,ADV_IRON_WORKING);
    if (city2 == 0) {
      commodity_supply.silver = commodity_supply.silver >> 1;
    }
    if (8 < (int)landmass2) {
      commodity_supply.silver += (commodity_supply.silver >> 1);
    }
    if (cities[city].size < 5) {
      commodity_supply.silver = commodity_supply.silver >> 1;
    }
  }
  if (civs[civ].tribe == 10) {
    commodity_supply.silk = commodity_supply.silk << 1;
  }
  commodity_supply.spice = (terrain_counts[9] * 3 + terrain_counts[8] * 2 + terrain_counts[0] * 2) *
                 ((terrain_counts[10] + river_tiles) / 2);
  if (commodity_supply.spice != 0) {
    if ((int)center_dy < 10) {
      commodity_supply.spice = commodity_supply.spice * 2;
    }
    commodity_supply.spice -= center_dy;
    if (landmass_table_block.landmass_recs[landmass2].tiles < 0x1a) {
      commodity_supply.spice += (commodity_supply.spice >> 1);
    }
    if (300 < landmass_table_block.landmass_recs[landmass2].tiles) {
      commodity_supply.spice = commodity_supply.spice >> 1;
    }
    if (landmass2 == 1) {
      commodity_supply.spice = commodity_supply.spice >> 1;
    }
  }
  commodity_supply.gems = (terrain_counts[5] + 1) * (terrain_counts[8] + 1) * (terrain_counts[0] + 1) + terrain_counts[1];
  if (commodity_supply.gems != 0) {
    city2 = int_clamp(size_class,1,4);
    commodity_supply.gems = (city2 * commodity_supply.gems) / 2;
    if (landmass2 == 7) {
      commodity_supply.gems += (commodity_supply.gems >> 1);
    }
  }
  commodity_supply.gold = (terrain_counts[5] + terrain_counts[4] / 2 + 1) * (river_tiles + 2);
  if (2 < terrain_counts[5]) {
    commodity_supply.gold = commodity_supply.gold * 2;
  }
  if (4 < cities[city].size) {
    commodity_supply.gold = commodity_supply.gold << 1;
  }
  if ('\t' < cities[city].size) {
    commodity_supply.gold = commodity_supply.gold << 1;
  }
  commodity_supply.oil = terrain_counts[6] * 8 + terrain_counts[0] * 10 + terrain_counts[8] * 6 + terrain_counts[7] * 0xc;
  if (game.advance_known_by[ADV_COMBUSTION] == '\0') {
    commodity_supply.oil = commodity_supply.oil >> 3;
  }
  if (commodity_supply.oil == 0) {
    commodity_supply.oil = -1;
  }
  else {
    if (landmass2 == 0x11) {
      commodity_supply.oil = commodity_supply.oil * 3;
    }
    else if ((1 < (int)landmass2) && ((landmass2 - 1 & 7) == 0)) {
      commodity_supply.oil += (commodity_supply.oil >> 1);
    }
    city2 = int_clamp(size_class / 2 - 2,1,2);
    commodity_supply.oil = city2 * commodity_supply.oil;
  }
  commodity_supply.uranium = (terrain_counts[6] + terrain_counts[0] + 1) * (terrain_counts[4] + river_tiles + 1) *
                 (terrain_counts[5] + 1);
  city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
  if (city2 == 0) {
    commodity_supply.uranium = 0;
  }
  if (commodity_supply.uranium == 0) {
    commodity_supply.uranium = -1;
  }
  else {
    if ((landmass2 != 0) && ((int)landmass2 % 10 == 0)) {
      commodity_supply.uranium += (commodity_supply.uranium >> 1);
    }
    city2 = size_class;
    if (5 < size_class) {
      city2 = 6;
    }
    commodity_supply.uranium = (commodity_supply.uranium * city2) / 6;
  }
}

/* Part of city_calc_commodity_demand (0043d400): the demand score of each
   commodity. */
static void city_commodity_demand_scores(uptr city,iptr civ,int road_tiles,int terrain_counts[11],int river_tiles,int size_class,uint landmass2,uint advance_count,uint center_dx,uint center_dy)
{
  int city2;
  i32 shift;
  int x4;
  int polar_dist;
  int chunk;
  int remaining_size;
  iptr metal_commodity;
  int weight;

  commodity_demand.by_commodity[0] = (terrain_counts[6] + terrain_counts[7]) * 5 + (int)(center_dy * 3) / 2 + terrain_counts[5] * 2 +
                 terrain_counts[3];
  city2 = ((int)advance_count / 10) * commodity_demand.by_commodity[0];
  commodity_demand.cloth = terrain_counts[4] * 4 + terrain_counts[3] * 4 + ((int)(city2 + (city2 >> 0x1f & 7U)) >> 3)
  ;
  if (cities[city].size < 3) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] * 2;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] / 3;
  }
  city2 = civ_has_advance(civ,ADV_MASS_PRODUCTION);
  if (city2 != 0) {
    commodity_demand.by_commodity[0] = 1;
  }
  if ((int)advance_count < 10) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] << 1;
  }
  if ((int)advance_count < 0x14) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] << 1;
  }
  if (0x2f < (int)advance_count) {
    commodity_demand.by_commodity[0] = commodity_demand.by_commodity[0] / 2;
  }
  if ((int)map.height >> 2 == center_dy || (int)(((int)map.height >> 2) - center_dy) < 0) {
    polar_dist = ~(((int)map.height >> 2) - center_dy) + 1;
  }
  else {
    polar_dist = ((int)map.height >> 2) - center_dy;
  }
  commodity_demand.wool[0] = polar_dist * 2 + terrain_counts[1] * 2 + terrain_counts[3];
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.wool[0] = commodity_demand.wool[0] << 1;
  }
  commodity_demand.beads = center_dy + ((0x15 - terrain_counts[10]) * 3) / 2;
  if (cities[city].size < 4) {
    commodity_demand.beads += (commodity_demand.beads >> 1);
  }
  if (11 < cities[city].size) {
    commodity_demand.beads = commodity_demand.beads / 2;
  }
  if (0x2f < (int)advance_count) {
    commodity_demand.beads = commodity_demand.beads / 2;
  }
  weight = 8;
  for (remaining_size = (int)cities[city].size; (weight != 0 && (0 < remaining_size));
      remaining_size = remaining_size - chunk) {
    chunk = int_clamp(remaining_size,0,5);
    commodity_demand.salt += chunk * weight;
    weight = weight / 2;
  }
  commodity_demand.salt -= (int)advance_count / 2;
  commodity_demand.coal = (center_dy + 10) * size_class + advance_count;
  if (cities[city].size < 5) {
    commodity_demand.coal = 0;
  }
  if (cities[city].size < 8) {
    commodity_demand.coal = commodity_demand.coal >> 1;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = civ_has_advance(civ,ADV_ELECTRICITY);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = city_has_improvement(city,IMP_POWER_PLANT);
  if (city2 != 0) {
    commodity_demand.coal = commodity_demand.coal << 1;
  }
  city2 = city_has_improvement(city,IMP_HYDRO_PLANT);
  if (((city2 != 0) || (city2 = city_has_improvement(city,IMP_NUCLEAR_PLANT), city2 != 0)) ||
     (city2 = city_has_improvement(city,IMP_SOLAR_PLANT), city2 != 0)) {
    commodity_demand.coal = commodity_demand.coal >> 3;
  }
  commodity_demand.copper = (river_tiles + road_tiles + 1) * size_class;
  if (commodity_demand.copper <= commodity_supply.copper) {
    commodity_demand.copper = commodity_demand.copper >> 1;
  }
  city2 = city_has_improvement(city,IMP_MARKETPLACE);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = city_has_improvement(city,IMP_BANK);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = civ_has_advance(civ,ADV_ELECTRICITY);
  if (city2 != 0) {
    commodity_demand.copper += (commodity_demand.copper >> 1);
  }
  city2 = civ_has_advance(civ,ADV_COMPUTERS);
  if (city2 != 0) {
    commodity_demand.copper = (int)(commodity_demand.copper + (commodity_demand.copper >> 0x1f & 3U)) >> 2;
  }
  if (cities[city].size < 5) {
    commodity_demand.copper = commodity_demand.copper >> 1;
  }
  commodity_demand.dye = commodity_supply.cloth + road_tiles;
  city2 = civ_has_advance(civ,ADV_CHEMISTRY);
  if (city2 != 0) {
    commodity_demand.dye = commodity_demand.dye / 2;
  }
  city2 = civ_has_advance(civ,ADV_MASS_PRODUCTION);
  if (city2 != 0) {
    commodity_demand.dye = commodity_demand.dye / 2;
  }
  if ((int)cities[city].x == (int)cities[city].y ||
      (int)cities[city].x - (int)cities[city].y < 0) {
    x4 = ~((int)cities[city].x - (int)cities[city].y) + 1;
  }
  else {
    x4 = (int)cities[city].x - (int)cities[city].y;
  }
  commodity_demand.wine = size_class * 4 + 4 + x4;
  commodity_demand.silk = ((((int)map.height >> 1) - center_dy) * 2 - center_dx) + ((int)map.width >> 1) +
                 terrain_counts[9] * 4 + terrain_counts[1] / 2 + terrain_counts[8] * 2 + terrain_counts[0] * 4;
  shift = int_clamp(size_class - 1,0xffffffff,1);
  commodity_demand.silk = int_shift_signed(commodity_demand.silk,shift);
  if ((landmass2 == 1) && ((city & 2) != 0)) {
    commodity_demand.silk += (commodity_demand.silk >> 1);
  }
  if (cities[city].size < 7) {
    commodity_demand.silk = commodity_demand.silk >> 1;
  }
  commodity_demand.silver = (int)cities[city].size << 3;
  commodity_demand.gold = 0;
  commodity_demand.gems = 0;
  metal_commodity = 10;
  city2 = ((int)cities[city].y + (int)cities[city].x) % 3;
  if (city2 == 1) {
    metal_commodity = 0xc;
    commodity_demand.gems = commodity_demand.silver;
  }
  else if (city2 == 2) {
    metal_commodity = 0xd;
    commodity_demand.gold = commodity_demand.silver;
  }
  city2 = city_has_improvement(city,IMP_CATHEDRAL);
  if ((city2 != 0) || (city2 = civ_has_wonder(civ,WONDER_MICHELANGELO), city2 != 0)) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] * 3 >> 1;
  }
  city2 = city_has_improvement(city,IMP_BANK);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] * 3 >> 1;
  }
  city2 = civ_has_advance(civ,ADV_ECONOMICS);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] >> 1;
  }
  city2 = civ_has_advance(civ,ADV_COMPUTERS);
  if (city2 != 0) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] >> 1;
  }
  if (civs[civ].tribe == 0x11) {
    (((int *)&commodity_demand.by_commodity))[metal_commodity] = (((int *)&commodity_demand.by_commodity))[metal_commodity] << 1;
  }
  city2 = advance_count - 0xc;
  if (city2 < 1) {
    city2 = 0;
  }
  commodity_demand.spice = (int)landmass_table_block.landmass_recs[landmass2].tiles / 10 - city2;
  if (cities[city].size < 4) {
    commodity_demand.spice = commodity_demand.spice / 2;
  }
  if (400 < landmass_table_block.landmass_recs[landmass2].tiles) {
    if (7 < cities[city].size) {
      commodity_demand.spice = commodity_demand.spice << 1;
    }
    if ((-(int)cities[city].x - (int)cities[city].y & 3U) == 0) {
      commodity_demand.spice = 0;
    }
  }
  city2 = civ_has_advance(civ,ADV_REFRIGERATION);
  if (city2 != 0) {
    commodity_demand.spice = commodity_demand.spice >> 1;
  }
  city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (city2 == 0) {
    commodity_demand.oil = -1;
  }
  else {
    commodity_demand.oil = ((int)advance_count / 3) * (size_class + 2);
    if (commodity_demand.oil <= commodity_supply.oil) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
    city2 = city_has_improvement(city,IMP_FACTORY);
    if (city2 == 0) {
      if (cities[city].size < 5) {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
      if (cities[city].size < '\n') {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
      if (cities[city].size < 20) {
        commodity_demand.oil = commodity_demand.oil >> 1;
      }
    }
    else {
      commodity_demand.oil = (commodity_demand.oil * 3) / 2;
    }
    city2 = civ_has_advance(civ,ADV_AUTOMOBILE);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil * 3;
    }
    city2 = city_has_improvement(city,IMP_SUPERHIGHWAYS);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil << 1;
    }
    city2 = city_has_improvement(city,IMP_MASS_TRANSIT);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
    city2 = city_has_improvement(city,IMP_RECYCLING_CENTER);
    if (city2 != 0) {
      commodity_demand.oil = commodity_demand.oil >> 1;
    }
  }
  city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
  if (city2 == 0) {
    commodity_demand.uranium = -1;
  }
  else {
    commodity_demand.uranium = advance_count * advance_count;
    shift = int_clamp(size_class - 3,0xfffffffd,0);
    commodity_demand.uranium = int_shift_signed(commodity_demand.uranium,shift);
    city2 = city_has_improvement(city,IMP_NUCLEAR_PLANT);
    if ((city2 != 0) || (city2 = city_has_improvement(city,IMP_SDI_DEFENSE), city2 != 0)) {
      commodity_demand.uranium = commodity_demand.uranium << 1;
    }
  }
}

/* Part of city_calc_commodity_demand (0043d400): the three commodities the city
   supplies and demands, the extra ones, and the caravans and trade
   routes kept up to date. */
static void city_commodity_pick(uptr city,iptr civ,iptr supply_order[16],uint advance_count,iptr demand_order[16])
{
  iptr *entry;
  char route_good;
  iptr saved_demand;
  int city2;
  iptr saved_supply;
  iptr unit;
  iptr slot;
  uint special_flags;
  iptr j;
  iptr commodity;
  iptr i;
  uint extra_demand;
  uint extra_supply;

  for (i = 0; i < 0x10; i++) {
    if ((((int *)&commodity_supply.by_commodity))[i] < (((int *)&commodity_demand.by_commodity))[i]) {
      if (0 < (((int *)&commodity_supply.by_commodity))[i]) {
        (((int *)&commodity_supply.by_commodity))[i] = 0;
      }
    }
    else if (0 < (((int *)&commodity_demand.by_commodity))[i]) {
      (((int *)&commodity_demand.by_commodity))[i] = 0;
    }
  }
  int_pairs_bubble_sort(0x10,(i32 *)supply_order,(i32 *)&commodity_supply.by_commodity);
  int_pairs_bubble_sort(0x10,(i32 *)demand_order,(i32 *)&commodity_demand.by_commodity);
  slot = 0xf;
  for (i = 0; i < 3; i++) {
    do {
      entry = supply_order + slot;
      commodity = *entry;
      saved_supply = commodity;
      slot--;
    } while (*(int *)(((i8 *)&commodity_supply.wool) + slot * 4) < 0);
    (*((char *)&commodity + 0)) = (i8)*entry;
    *(i8 *)&cities[city].supplies[i] = (i8)commodity;
    commodity = saved_supply;
  }
  slot = 0xf;
  for (i = 0; i < 3; i++) {
    do {
      entry = demand_order + slot;
      commodity = *entry;
      saved_demand = commodity;
      slot--;
    } while ((((i32 *)&commodity_demand.wool))[slot] < 0);
    (*((char *)&commodity + 0)) = (i8)*entry;
    *(i8 *)&cities[city].demands[i] = (i8)commodity;
    commodity = saved_demand;
  }
  if ((int)advance_count < 0x20) {
    extra_demand = (cities[city].y * 5 + cities[city].x * 3) % 0xe;
    extra_supply = (cities[city].y * 7 + cities[city].x * 0xd) % 0xe;
  }
  else {
    extra_demand = (cities[city].y * 5 + cities[city].x * 3) % 9 + 5;
    extra_supply = (cities[city].y * 7 + cities[city].x * 0xd) % 9 + 5;
  }
  if (extra_supply == extra_demand) {
    extra_supply = 0xfffffffe;
  }
  special_flags = 0;
  for (i = 0; i < 3; i++) {
    if ('\r' < *(char *)&cities[city].demands[i]) {
      special_flags = special_flags | 1;
    }
    if ('\r' < *(char *)&cities[city].supplies[i]) {
      special_flags = special_flags | 2;
    }
    if (((int)*(char *)&cities[city].supplies[i] == extra_demand) ||
       ((int)*(char *)&cities[city].demands[i] == extra_demand)) {
      extra_demand = 0xfffffffe;
    }
    if (((int)*(char *)&cities[city].supplies[i] == extra_supply) ||
       ((int)*(char *)&cities[city].demands[i] == extra_supply)) {
      extra_supply = 0xfffffffe;
    }
  }
  if (((int)extra_demand < 0) && ((special_flags & 1) == 0)) {
    city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
    if ((city2 == 0) || ((extra_demand & 1) == 0)) {
      city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION);
      if ((city2 != 0) && ((extra_demand & 1) == 0)) {
        extra_demand = 0xf;
      }
    }
    else {
      extra_demand = 0xe;
    }
  }
  if (-1 < (int)extra_demand) {
    cities[city].demands[1] = (u8)extra_demand;
  }
  if (((int)extra_supply < 0) && ((special_flags & 2) == 0)) {
    extra_supply = (extra_supply & 1) + 0xe;
    if ((extra_supply == 0xe) && (city2 = civ_has_advance(civ,ADV_INDUSTRIALIZATION), city2 == 0)) {
      extra_supply = 0xffffffff;
    }
    if ((extra_supply == 0xf) && (city2 = civ_has_advance(civ,ADV_NUCLEAR_FISSION), city2 == 0)) {
      extra_supply = 0xffffffff;
    }
  }
  if (-1 < (int)extra_supply) {
    cities[city].supplies[1] = (u8)extra_supply;
  }
  for (i = 0; i < (char)cities[city].trade_routes; i++) {
    route_good = *(char *)&cities[city].route_goods[i];
    if (-1 < route_good) {
      for (j = 0; j < 3; j++) {
        if (*(char *)&cities[city].supplies[j] == route_good) {
          *(char *)&cities[city].supplies[j] =
               -*(char *)&cities[city].supplies[j];
        }
      }
      city2 = (int)cities[city].route_partner[i];
      commodity = -1;
      for (j = 0; j < (char)cities[city2].trade_routes; j++) {
        if ((int)cities[city2].route_partner[j] == city) {
          commodity = (iptr)*(char *)&cities[city2].route_goods[j];
        }
      }
      if (-1 < commodity) {
        for (j = 0; j < 3; j++) {
          if (*(char *)&cities[city].demands[j] == commodity) {
            *(char *)&cities[city].demands[j] =
                 -*(char *)&cities[city].demands[j];
          }
        }
      }
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if ((((units[unit].id != 0) && ((byte)units[unit].home_city == city)) &&
        (unit_types[units[unit].type].role == ROLE_TRADE)) && (-1 < (char)units[unit].counter)
       ) {
      for (i = 0; i < 3; i++) {
        if (cities[city].supplies[i] == units[unit].counter) {
          *(char *)&cities[city].supplies[i] =
               -*(char *)&cities[city].supplies[i];
        }
      }
    }
  }
}

/* 0043d400 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_calc_commodity_demand(uptr city)

{
  iptr civ;
  int road_tiles;
  int terrain_counts[11];
  iptr supply_order[16];
  int river_tiles;
  int size_class;
  int west_dist;
  uint landmass2;
  int north_dist;
  int y3;
  uint south_dist;
  int x3;
  uint advance_count;
  uint center_dx;
  uint east_dist;
  iptr demand_order[16];
  uint center_dy;
  /* B1 (docs/BUGS.md): the original never zeroes the road count; its stack
     held 0 there in 687 of 713 calls probed, a stale address in the rest.
     0 in every mode. */
  road_tiles = 0;  /* stale stack the original reads (FRAME_PRESETS) */
  
  if ((((cities[city].flags & CITYF_TRADE_DIRTY) != 0) || (game.reveal_map != '\0')) ||
     (((int)game.turn + city & 0xf) == 0)) {
    cities[city].flags = cities[city].flags & ~CITYF_TRADE_DIRTY;
    x3 = (int)cities[city].x;
    y3 = (int)cities[city].y;
    civ = (iptr)cities[city].owner;
    landmass2 = tile_landmass(x3,y3);
    size_class = (cities[city].size + 2) / 5;
    advance_count = (uint)civs[cities[city].owner].num_advances;
    center_dy = y3 - ((int)map.height >> 1);
    center_dx = x3 - ((int)map.width >> 1);
    west_dist = 0;
    east_dist = 0;
    south_dist = 0;
    north_dist = 0;
    if ((int)center_dy < 0) {
      north_dist = ~center_dy + 1;
    }
    if (0 < (int)center_dy) {
      south_dist = center_dy;
    }
    if (0 < (int)center_dx) {
      east_dist = center_dx;
    }
    if ((int)center_dx < 0) {
      west_dist = ~center_dx + 1;
    }
    if ((int)center_dy < 1) {
      center_dy = ~center_dy + 1;
    }
    if ((int)center_dx < 1) {
      center_dx = ~center_dx + 1;
    }
    city_commodity_count_terrain(road_tiles,terrain_counts,supply_order,river_tiles,y3,x3,demand_order);
    city_commodity_supply_scores(city,civ,terrain_counts,river_tiles,size_class,landmass2,south_dist,advance_count,center_dx,east_dist,center_dy);
    city_commodity_demand_scores(city,civ,road_tiles,terrain_counts,river_tiles,size_class,landmass2,advance_count,center_dx,center_dy);
    city_commodity_pick(city,civ,supply_order,advance_count,demand_order);
  }
  return;
}

/* 0043f493 auto */
void __cdecl city_assign_default_name(int city)

{
  char civ;
  int result;
  size_t len;
  int name_label;
  uint skip_count;
  int attempt;
  i8 section_name[0x100];  /* BF-054 (Peter Johnson's ledger): the original's
                             24, less than a civilization's name */
  
  attempt = 0;
  civ = cities[city].owner;
  str_clear((char *)section_name);
  if (leaders[civs[civ].tribe].plural < 1) {
    name_label = ~(int)leaders[civs[civ].tribe].plural + 1;
  }
  else {
    name_label = (int)leaders[civs[civ].tribe].plural;
  }
  str_append_max((char *)section_name,label_string(name_label),sizeof section_name);
  do {
    attempt++;
    if (2 < attempt) break;
    leaders[civs[civ].tribe].city_name_index = leaders[civs[civ].tribe].city_name_index + 1
    ;
    skip_count = (uint)leaders[civs[civ].tribe].city_name_index;
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    result = text_open_section((const char *)&str_city,(const char *)section_name);
    if (result != 0) {
      __chdir(((i8 *)&game_dirs.home));
      result = text_open_section((const char *)&str_city_2,(const char *)section_name);
      if (result != 0) break;
    }
    __chdir(((i8 *)&game_dirs.home));
    for (; 0 < (int)skip_count; skip_count--) {
      text_read_line(1);
      len = _strlen(((i8 *)&textbuf.buf));
      if ((len == 0) || (result = __strnicmp(((i8 *)&textbuf.buf),str_stop,5), result == 0)) break;
    }
    if (0 < (int)skip_count) {
      result = text_open_section(0,(const char *)&str_extra);
      if (result != 0) {
        leaders[civs[civ].tribe].city_name_index = '\0';
        continue;
      }
      for (; 0 < (int)skip_count; skip_count--) {
        text_read_line(1);
        len = _strlen(((i8 *)&textbuf.buf));
        if ((len == 0) || (result = __strnicmp(((i8 *)&textbuf.buf),str_stop_2,5), result == 0))
        break;
      }
    }
    if ((int)skip_count < 1) {
      _memset(cities[city].name,0,0x10);
      _strncpy(cities[city].name,((i8 *)&textbuf.buf),0xf);
      break;
    }
    leaders[civs[civ].tribe].city_name_index = '\0';
  } while( true );
  text_close_file();
  return;
}

/* 0043f7a7 auto */
void __cdecl city_refresh_area_tiles(int city)

{
  char civ;
  short x2;
  short y2;
  iptr x;
  int y;
  int in_bounds;
  uint fertility;
  iptr notify;
  iptr tile_idx;
  
  x2 = cities[city].x;
  y2 = cities[city].y;
  civ = cities[city].owner;
  net_batch_begin();
  for (tile_idx = 0; tile_idx < 0x2d; tile_idx++) {
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx] + (int)y2;
    in_bounds = map_in_bounds(x,y);
    if (in_bounds != 0) {
      notify = 1;
      fertility = tile_get_fertility(x,y);
      tile_set_fertility(x,y,fertility & 7,notify);
      if (tile_idx < 0x15) {
        tile_set_radius_civ(x,y,civ,1);
      }
    }
  }
  net_batch_flush();
  return;
}

/* 0043f8b0 auto */
uptr __cdecl city_create(int x,int y,int civ)

{
  char *build_count;
  char is_host;
  byte terrain;
  iptr start_ticks;
  iptr now_ticks;
  iptr can_build;
  iptr x2;
  int y2;
  iptr in_bounds;
  int result;
  i32 start_size;
  byte *tile;
  Tile *map_tile;
  iptr other_civ;
  uptr city;
  iptr unit_type;
  iptr tile_idx;
  int best_score;
  int score;
  
  net.city_create_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    for (city = 0; ((int)city < (int)game.num_cities && (cities[city].id != 0));
        city = city + 1) {
    }
    if ((int)game.num_cities == city) {
      if (0xfe < game.num_cities) {
        if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           (game.multiplayer < 3)) {
          popup_debug_text((const char *)&str_toomanycities);
        }
        net.city_create_busy = 0;
        return 0xffffffff;
      }
      game.num_cities++;
    }
    civs[civ].num_cities++;
    civs[civ].last_city_turn = game.turn;
    tile_set_improvements(x,y,2,1,1);
    tile_set_improvements(x,y,0x40,0,1);
    cities[city].x = (i16)x;
    cities[city].y = (i16)y;
    cities[city].owner = (byte)civ;
    cities[city].id = next_ids.city;
    next_ids.city++;
    cities[city].founder = (byte)civ;
    cities[city].turns_captured = '\0';
    cities[city].flags = 0x20000;
    cities[city].size = 1;
    if (game.wonder_city[WONDER_APOLLO] < 0) {
      cities[city].known_to = '\0';
      for (other_civ = 0; other_civ < 8; other_civ++) {
        *(i8 *)&cities[city].revealed_size[other_civ] = 0;
      }
    }
    else {
      cities[city].known_to = 0xff;
      for (other_civ = 0; other_civ < 8; other_civ++) {
        *(i8 *)&cities[city].revealed_size[other_civ] = 1;
      }
      tile_set_visible(x,y,0xff,1,1);
      for (other_civ = 1; other_civ < 8; other_civ++) {
        tile_update_known(x,y,other_civ);
      }
    }
    cities[city].specialists = 0;
    cities[city].building = UNIT_SETTLERS;
    best_score = 0;
    for (unit_type = 2; unit_type < 0x3e; unit_type++) {
      if ((unit_types[unit_type].role == ROLE_DEFEND) &&
         (can_build = civ_can_build_unit(civ,city,unit_type), can_build != 0)) {
        score = ((int)(char)unit_types[unit_type].defense << 3) /
                  (int)(char)unit_types[unit_type].cost;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          score++;
        }
        if (best_score <= score) {
          best_score = score;
          cities[city].building = (i8)unit_type;
        }
      }
    }
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_count = *build_count + 1;
    cities[city].food_box = 0;
    cities[city].shield_box = 0;
    cities[city].base_trade = 0;
    cities[city].name[0] = '\0';
    city_assign_default_name(city);
    cities[city].worked_tiles = 0;
    _memset(cities[city].improvements,0,5);
    if (civs[civ].num_cities == 1) {
      city_set_improvement(city,1,1);
      civs[civ].capital_x = (i16)x;
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
        net_batch_begin();
        for (tile_idx = 0; tile_idx < 0x14; tile_idx++) {
          x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + x);
          y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx] + y;
          in_bounds = map_in_bounds(x2,y2);
          if ((((in_bounds != 0) &&
               (tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),1,1), 0x27 < game.turn)) &&
              (tile_idx < 8)) &&
             ((map_tile = tile_at(x2,y2), (map_tile->visible & game.human_players) == 0 &&
              (terrain = tile_terrain(x2,y2), terrain_types[terrain].irrigate_to == -2)))) {
            tile_set_improvements(x2,y2,4,1,1);
          }
        }
        net_batch_flush();
        if ((0x28 < game.turn) &&
           (map_tile = tile_at(x,y), (map_tile->visible & game.human_players) == 0)) {
          start_size = int_clamp((game.turn - 0x14) / 0x14,2,10);
          cities[city].size = (i8)start_size;
          city_set_improvement(city,4,1);
          city_set_improvement(city,5,1);
          city_set_improvement(city,6,1);
        }
        if ((0x28 < game.turn) && (cities[city].building != UNIT_SETTLERS)) {
          unit_create((int)cities[city].building,civ,x,y);
        }
      }
    }
    cities[city].trade_routes = '\0';
    _memset(cities[city].route_partner,0,3);
    _memset(cities[city].route_goods,0,3);
    for (tile_idx = 0; tile_idx < 9; tile_idx++) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[tile_idx] + x);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[tile_idx] + y;
      result = map_in_bounds(x2,y2);
      if (result != 0) {
        result = tile_is_ocean(x2,y2);
        if (result != 0) {
          result = tile_landmass(x2,y2);
          if ((1 < landmass_table_block.landmass_recs[result].water_body_size) &&
             ((result < 0x3f || ((cities[city].flags & CITYF_COASTAL) != 0)))) {
            cities[city].flags = cities[city].flags | CITYF_OCEAN_ACCESS;
          }
          cities[city].flags = cities[city].flags | CITYF_COASTAL;
        }
        tile = (byte *)tile_at(x2,y2);
        if (((*tile & 0x80) != 0) || (is_host = tile_terrain(x2,y2), is_host == 5)) {
          cities[city].flags = cities[city].flags | CITYF_HYDRO_POSSIBLE;
        }
      }
    }
    city_refresh_area_tiles(city);
    city_calc_commodity_demand(city);
    cities[city].science = 0;
    cities[city].tax = 0;
    cities[city].trade = 0;
    cities[city].total_food = '\0';
    cities[city].total_shields = '\0';
    cities[city].happy = '\0';
    cities[city].unhappy = '\0';
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.city_create_busy = 0;
  }
  else {
    net.city_create_busy = 0;
    net.nm_city_created_result = 0xfffffffe;
    net_send_message(NM_CREATE_CITY,0,x,y,civ,0,0,0,0,0);
    start_ticks = timer_ticks();
    while ((net.nm_city_created_result == 0xfffffffe && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_city_created_result == 0xfffffffe) {
      dbg_log((const char *)&str_create_city_connection_to_server_timed_o);
      popup_game_with_sprite(str_serverconnecttime,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (city = net.nm_city_created_result, net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return city;
}

/* 00440325 auto */
void __cdecl city_remove_trade_route(int city,int route_index)

{
  iptr slot;
  
  for (slot = route_index; slot < (char)cities[city].trade_routes - 1; slot++)
  {
    cities[city].route_partner[slot] =
         cities[city].route_partner[slot + 1];
    *(i8 *)&cities[city].route_goods[slot] =
         *(i8 *)&cities[city].route_goods[slot + 1];
  }
  cities[city].trade_routes += 0xff;
  cities[city].flags = cities[city].flags | CITYF_TRADE_DIRTY;
  return;
}

/* 004403ec auto */
void __cdecl city_set_trade_route_slot(int city,int slot,int partner,int commodity)

{
  i32 goods;
  
  cities[city].route_partner[slot] = (short)partner;
  goods = int_clamp(commodity,0xffffffff,0xf);
  *(char *)&cities[city].route_goods[slot] = (char)goods;
  cities[city].flags = cities[city].flags | CITYF_TRADE_DIRTY;
  return;
}

/* 00440453 auto */
void __cdecl city_add_trade_route(uptr city,uptr city2,int commodity)

{
  u8 route_count;
  iptr city3;
  int has_improvement;
  iptr weakest_slot;
  int min_value;
  int road_bonus;
  iptr slot;
  int route_value;
  
  for (slot = 0; slot < (char)cities[city].trade_routes; slot++) {
    if (((int)cities[city].route_partner[slot] == city2) &&
       (*(char *)&cities[city].route_goods[slot] == commodity)) {
      return;
    }
  }
  if ((char)cities[city].trade_routes < 3) {
    route_count = cities[city].trade_routes;
    cities[city].trade_routes++;
    city_set_trade_route_slot(city,(int)(char)route_count,city2,commodity);
    city_calc_commodity_demand(city);
    city_calc_commodity_demand(city2);
  }
  else {
    min_value = 9999;
    weakest_slot = -1;
    for (slot = 0; slot < (char)cities[city].trade_routes; slot++) {
      if ((*(char *)&cities[city].route_goods[slot] < '\0') && (commodity < 0)) {
        return;
      }
      city3 = (iptr)cities[city].route_partner[slot];
      route_value = (int)cities[city3].base_trade;
      road_bonus = map_road_connection(cities[city].owner,(int)cities[city].x,
                                    (int)cities[city].y,(int)cities[city3].x,(int)cities[city3].y
                                   );
      has_improvement = city_has_improvement(city,IMP_AIRPORT);
      if (((has_improvement != 0) && (has_improvement = city_has_improvement(city3,IMP_AIRPORT), has_improvement != 0)) && (road_bonus < 2))
      {
        road_bonus = 1;
      }
      if (road_bonus != 0) {
        route_value += (road_bonus * route_value >> 1);
      }
      if (cities[city].owner == cities[city3].owner) {
        route_value = route_value >> 1;
      }
      if (route_value < min_value) {
        min_value = route_value;
        weakest_slot = slot;
      }
    }
    /* BF-026 (Peter Johnson's ledger): with every route worth 9999 or more
       (and a partner whose trade is as much) no slot was picked; the
       original wrote the route at an unset slot */
    if ((-1 < weakest_slot) && (min_value <= cities[city2].base_trade)) {
      city_set_trade_route_slot(city,weakest_slot,city2,commodity);
      city_calc_commodity_demand(city);
      city_calc_commodity_demand(city2);
    }
  }
  return;
}

/* 004413d1 auto */
void __cdecl city_delete(uptr city,i32 from_network)

{
  char is_host;
  int saved_refresh_lock;
  int x2;
  int y2;
  iptr civ;
  uptr city4;
  int y;
  iptr x;
  int result;
  uint fertility;
  iptr notify;
  iptr unit;
  uint nearby_civs;
  iptr city3;
  byte owner_civ;
  iptr city2;
  i8 new_home;
  
  if ((-1 < (int)city) && (cities[city].id != 0)) {
    net.city_delete_busy = 1;
    if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
      saved_refresh_lock = city_wnd.refresh_locked;
      city_wnd.refresh_locked = 1;
      x2 = (int)cities[city].x;
      y2 = (int)cities[city].y;
      civ = (iptr)cities[city].owner;
      civs[civ].num_cities--;
      do {
        unit = (iptr)game.num_units;
        while( true ) {
          while( true ) {
            do {
              unit--;
              if (unit < 0) {
                cities[city].id = 0;
                if ((int)game.num_cities - 1U == city) {
                  game.num_cities--;
                }
                for (city2 = 0; city2 < game.num_cities; city2++) {
                  if (cities[city2].id != 0) {
                    unit = (iptr)(char)cities[city2].trade_routes;
                    while (unit = unit - 1, -1 < unit) {
                      if ((int)cities[city2].route_partner[unit] == city) {
                        city_remove_trade_route(city2,unit);
                      }
                    }
                  }
                }
                for (city2 = 0; city2 < 0x1c; city2++) {
                  if ((int)game.wonder_city[city2] == city) {
                    game.wonder_city[city2] = -2;
                  }
                }
                nearby_civs = 0;
                net_batch_begin();
                tile_set_improvements(x2,y2,2,0,1);
                for (city2 = 0; city2 < 0x2d; city2++) {
                  x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[city2] + x2);
                  y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[city2] + y2;
                  result = map_in_bounds(x,y);
                  if ((result != 0) && (result = tile_is_ocean(x,y), result == 0)) {
                    notify = 1;
                    fertility = tile_get_fertility(x,y);
                    tile_set_fertility(x,y,fertility | 8,notify);
                    if (city2 < 0x15) {
                      result = tile_radius_civ(x,y);
                      if (result == civ) {
                        tile_set_radius_civ(x,y,0,1);
                      }
                      y = tile_city_or_unit_owner(x,y);
                      if ((0 < y) && (y != civ)) {
                        owner_civ = (byte)y;
                        nearby_civs = nearby_civs | 1 << (owner_civ & 0x1f);
                      }
                    }
                  }
                }
                net_batch_flush();
                if (nearby_civs != 0) {
                  for (unit = tile_first_unit(x2,y2); -1 < unit;
                      unit = unit_next_in_stack(unit)) {
                    units[unit].visibility = units[unit].visibility | (byte)nearby_civs;
                  }
                }
                for (city3 = 0; city3 < game.num_cities; city3++) {
                  if (cities[city3].id != 0) {
                    city_refresh_area_tiles(city3);
                  }
                }
                ((CityWindow *)((i8 *)&city_wnd.window))->close_if_city(city);
                city_wnd.refresh_locked = saved_refresh_lock;
                if (saved_refresh_lock == 0) {
                  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
                }
                if (((char)from_network == '\0') && (2 < game.multiplayer)) {
                  net_send_state_diff(0xff,2,0,0,0);
                  net_send_message(NM_CITYWIN_DELETED,0xff,city,0,0,0,0,0,0,0);
                  net_send_message(NM_CITYWIN_REDRAW,0xff,0,0,0,0,0,0,0,0);
                  XD_FlushSendBuffer(5000);
                }
                net.city_delete_busy = 0;
                return;
              }
            } while ((units[unit].id == 0) || ((byte)units[unit].home_city != city));
            if (units[unit].type != UNIT_PARTISANS) break;
            units[unit].home_city = -1;
          }
          if (((((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) ||
              (city4 = city_at((int)units[unit].x,units[unit].y),
              (int)city4 < 0)) || (city == city4)) break;
          if ((unit_types[units[unit].type].role != ROLE_DEFEND) ||
             (y = unit_stack_count_role(unit,ROLE_DEFEND), y != 1)) {
            cities[city4].flags = cities[city4].flags | CITYF_DEFENDER_LOST;
            break;
          }
          new_home = (i8)city4;
          units[unit].home_city = new_home;
        }
        unit_destroy_redraw(unit,1);
      } while( true );
    }
    net.city_delete_busy = 0;
    net.city_deleted_ack = -2;
    net_send_message(NM_DELETE_CITY,0,city,0,0,0,0,0,0,0);
    saved_refresh_lock = timer_ticks();
    while ((net.city_deleted_ack == -2 && (x2 = timer_ticks(), x2 - saved_refresh_lock < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.city_deleted_ack == -2) {
      dbg_log((const char *)&str_delete_city_connection_to_server_timed_o);
      popup_game_with_sprite(str_serverconnecttime_2,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 00441a79 auto */
i32 __cdecl city_needed_size_improvement(int city)

{
  int has_building;
  
  if (((int)(uint)cosmic.aqueduct_size <= (int)cities[city].size) &&
     (has_building = city_has_improvement(city,IMP_AQUEDUCT), has_building == 0)) {
    return 9;
  }
  if (((int)(uint)cosmic.sewer_size <= (int)cities[city].size) &&
     (has_building = city_has_improvement(city,IMP_SEWER_SYSTEM), has_building == 0)) {
    return 0x17;
  }
  return 0;
}

/* 00441b11 auto */
void __cdecl city_set_production(int city,int production)

{
  char *build_counter;
  char old_building;
  byte civ2;
  int civ;
  i32 new_shields;
  char *civ_name;
  int bonus_mult;
  iptr improvement2;
  int old_count;
  iptr improvement;
  iptr old_wonder;
  Dialog dialog2;
  int new_count;
  iptr wonder;
  int civ3;
  int all_humans_have;
  iptr city3;
  iptr wonder_group;
  iptr city2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  old_building = cities[city].building;
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  if ((-1 < cities[city].building) && (cities[city].building < '>')) {
    build_counter = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_counter = *build_counter - 1;
  }
  if (0x62 < production) {
    production = city_choose_production(city,0,0);
  }
  if (production < 99) {
    if (((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) && (turn_flags.civ_turn_in_progress == 0)) {
      new_shields = city_shields_after_change(city,production);
      cities[city].shield_box = (i16)new_shields;
    }
    cities[city].building = (i8)production;
  }
  if ((cities[city].building != old_building) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0)
     ) {
    if ((cities[city].building < -0x26) || (old_building < -0x26)) {
      old_count = 0;
      new_count = 0;
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if (((cities[city2].id != 0) && (cities[city].owner == cities[city2].owner)) &&
           (city2 != city)) {
          if (cities[city2].building == old_building) {
            old_count++;
          }
          if (cities[city].building == cities[city2].building) {
            new_count++;
          }
        }
      }
    }
    if (cities[city].building < -0x26) {
      if (-0x27 < old_building) {
        cities[city].shield_box = cities[city].shield_box / 2;
      }
      if (cities[city].building < 1) {
        improvement = ~(int)cities[city].building + 1;
      }
      else {
        improvement = (iptr)cities[city].building;
      }
      wonder = improvement - 0x27;
      if (old_building < -0x26) {
        old_wonder = -0x27 - old_building;
      }
      else {
        old_wonder = 0;
      }
      civ_name = civ_name_plural(civ);
      text_set_string_arg(1,civ_name);
      if (new_count == 0) {
        if ((old_building < -0x26) && ((old_count == 0 || (game.wonder_city[old_wonder] != -1)))) {
          if (old_count != 0) {
            for (city3 = 0; city3 < game.num_cities; city3++) {
              if (((cities[city3].id != 0) && (cities[city3].owner == civ)) &&
                 (cities[city3].building == old_building)) {
                cities[city3].building = cities[city].building;
              }
            }
          }
          if (autoplay.active == 0) {
            if (old_building < 1) {
              improvement2 = ~(int)old_building + 1;
            }
            else {
              improvement2 = (iptr)old_building;
            }
            text_set_label_arg(2,improvements[improvement2].name);
            text_set_label_arg(3,improvements[improvement].name);
            dialog2.load_game_text((const char *)&str_switchwonder);
            dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)),0,0);
            dialog2.set_image_zoom(8);
            if (2 < game.multiplayer) {
              net_send_text_args(0x13,0xff,4,0,improvement,8);
            }
            dialog2.show_modal(0);
          }
        }
        else if ((((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] & 1) == 0) &&
                ((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] =
                      (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] | 1, autoplay.active == 0)) {
          text_set_label_arg(2,improvements[improvement].name);
          dialog2.load_game_text((const char *)&str_startwonder);
          dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)),0,0);
          dialog2.set_image_zoom(8);
          if (2 < game.multiplayer) {
            net_send_text_args(0x12,0xff,3,0,improvement,8);
          }
          dialog2.show_modal(0);
        }
      }
      if ((cities[city].shield_box == 0) &&
         (wonder_group = (improvement - 0x27) / 7, *(char *)&civs[civ].wonders_by_group[wonder_group] == '\0'
         )) {
        all_humans_have = 1;
        for (civ3 = 1; civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if ((all_humans_have == 0) || (*(char *)&civs[civ3].wonders_by_group[wonder_group] == '\0')) {
              all_humans_have = 0;
            }
            else {
              all_humans_have = 1;
            }
          }
        }
        if (all_humans_have != 0) {
          bonus_mult = (uint)game.difficulty + (uint)civs[civ].wonders_by_group[wonder_group] * -2;
          if (bonus_mult < 1) {
            bonus_mult = 0;
          }
          cities[city].shield_box =
               (short)bonus_mult * (ushort)improvements[improvement].cost + cities[city].shield_box;
        }
      }
    }
    else if ((old_building < -0x26) && (old_count == 0)) {
      if (old_building < 1) {
        improvement = ~(int)old_building + 1;
      }
      else {
        improvement = (iptr)old_building;
      }
      wonder = improvement - 0x27;
      if (((game.wonder_city[improvement - 0x27] != city) &&
          (bonus_mult = civ_has_wonder(civ,improvement - 0x27), bonus_mult == 0)) &&
         ((((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] & 2) == 0 &&
          ((((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] = (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] | 2,
          autoplay.active == 0)))) {
        civ_name = civ_name_plural(civ);
        text_set_string_arg(1,civ_name);
        text_set_label_arg(2,improvements[improvement].name);
        dialog2.load_game_text((const char *)&str_abandonwonder);
        dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)),0,0);
        dialog2.set_image_zoom(8);
        if (2 < game.multiplayer) {
          net_send_text_args(0x14,0xff,3,0,improvement,8);
        }
        dialog2.show_modal(0);
      }
    }
  }
  if ((-1 < cities[city].building) && (cities[city].building < '>')) {
    build_counter = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_counter = *build_counter + 1;
  }
  return;
}

/* 0044263f auto */
i32 __cdecl city_adjacent_to_body(int city,int landmass)

{
  iptr x;
  int y;
  int ok;
  iptr dir;
  
  if (landmass < 0x3f) {
    for (dir = 0; dir < 8; dir++) {
      x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir]);
      y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
      ok = map_in_bounds(x,y);
      if (((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) &&
         (y = tile_landmass(x,y), y == landmass)) {
        return 1;
      }
    }
  }
  return 0;
}

/* 0044272d auto */
int __cdecl city_adjacent_sea_info(int city,int *out_max_size,uptr *out_dir)

{
  short x2;
  short y2;
  iptr x;
  int y;
  int ok;
  int body_size;
  int landmass2;
  uptr dir;
  int max_size;
  
  max_size = 0;
  landmass2 = 0;
  x2 = cities[city].x;
  y2 = cities[city].y;
  for (dir = 0; (int)dir < 8; dir++) {
    x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
    ok = map_in_bounds(x,y);
    if ((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) {
      landmass2 = tile_landmass(x,y);
      body_size = (int)landmass_table_block.landmass_recs[landmass2].water_body_size;
      if (0x3e < landmass2) {
        body_size = 1;
      }
      if (max_size <= body_size) {
        max_size = body_size;
      }
      dir += (2 - (dir & 1));
    }
  }
  if (out_max_size != (int *)0x0) {
    *out_max_size = max_size;
  }
  if (out_dir != (uptr *)0x0) {
    *out_dir = dir;
  }
  return landmass2;
}

/* 004429af auto */
i32 __cdecl cities_share_ocean(int city,int city2)

{
  iptr x;
  int y;
  int ok;
  int last_landmass;
  iptr dir;
  
  last_landmass = -1;
  dir = 0;
  while( true ) {
    if (7 < dir) {
      return 0;
    }
    x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir]);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    ok = map_in_bounds(x,y);
    if ((((ok != 0) && (ok = tile_is_ocean(x,y), ok != 0)) &&
        (y = tile_landmass(x,y), y != last_landmass)) &&
       (ok = city_adjacent_to_body(city2,y), last_landmass = y, ok != 0)) break;
    dir++;
  }
  return 1;
}

/* 00447e53 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_down_arrow_sprite_global_init(void)

{
  city_down_arrow_sprite_static_init();
  city_down_arrow_sprite_reg_atexit();
  return;
}

/* 00447e6d auto */
void __cdecl city_down_arrow_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite));
  return;
}

/* 00447e87 auto */
void __cdecl city_down_arrow_sprite_reg_atexit(void)

{
  _atexit(city_down_arrow_sprite_static_dtor);
  return;
}

/* 00447ea4 auto */
void __cdecl city_down_arrow_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&city_down_arrow_sprite_block.city_down_arrow_sprite));
  return;
}

/* 0044816e auto */
void __cdecl city_sprites_global_init(void)

{
  city_sprites_static_init();
  city_sprites_reg_atexit();
  return;
}

/* 00448188 auto */
void __cdecl city_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&gfx_sheet_surface_block.city_sprites),0x3c,0x30,sprite_init,sprite_destroy);
  return;
}

/* 004481b0 auto */
void __cdecl city_sprites_reg_atexit(void)

{
  _atexit(city_sprites_static_dtor);
  return;
}

/* 004481cd auto */
void __cdecl city_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&gfx_sheet_surface_block.city_sprites),0x3c,0x30,sprite_destroy);
  return;
}

/* 00448376 auto */
void __cdecl citizen_sprites_global_init(void)

{
  citizen_sprites_static_init();
  citizen_sprites_reg_atexit();
  return;
}

/* 00448390 auto */
void __cdecl citizen_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&road_sprites_block.citizen_sprites),0x3c,0x2c,sprite_init,sprite_destroy);
  return;
}

/* 004483b8 auto */
void __cdecl citizen_sprites_reg_atexit(void)

{
  _atexit(citizen_sprites_static_dtor);
  return;
}

/* 004483d5 auto */
void __cdecl citizen_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&road_sprites_block.citizen_sprites),0x3c,0x2c,sprite_destroy);
  return;
}

/* 004484fc auto */
void __cdecl tax_rate_icon_sprites_global_init(void)

{
  tax_rate_icon_sprites_static_init();
  tax_rate_icon_sprites_reg_atexit();
  return;
}

/* 00448516 auto */
void __cdecl tax_rate_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 0044853e auto */
void __cdecl tax_rate_icon_sprites_reg_atexit(void)

{
  _atexit(tax_rate_icon_sprites_static_dtor);
  return;
}

/* 0044855b auto */
void __cdecl tax_rate_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 00448600 auto */
void __cdecl lux_tax_sci_icon_sprites_global_init(void)

{
  lux_tax_sci_icon_sprites_static_init();
  lux_tax_sci_icon_sprites_reg_atexit();
  return;
}

/* 0044861a auto */
void __cdecl lux_tax_sci_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 00448642 auto */
void __cdecl lux_tax_sci_icon_sprites_reg_atexit(void)

{
  _atexit(lux_tax_sci_icon_sprites_static_dtor);
  return;
}

/* 0044865f auto */
void __cdecl lux_tax_sci_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tax_rate_icon_sprites_block.lux_tax_sci_icon_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 004486ed auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unhappy_suppressed_icon_sprite_global_init(void)

{
  unhappy_suppressed_icon_sprite_static_init();
  unhappy_suppressed_icon_sprite_reg_atexit();
  return;
}

/* 00448707 auto */
void __cdecl unhappy_suppressed_icon_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite));
  return;
}

/* 00448721 auto */
void __cdecl unhappy_suppressed_icon_sprite_reg_atexit(void)

{
  _atexit(unhappy_suppressed_icon_sprite_static_dtor);
  return;
}

/* 0044873e auto */
void __cdecl unhappy_suppressed_icon_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite));
  return;
}

/* 00448949 auto */
void __cdecl improvement_icon_sprites_global_init(void)

{
  improvement_icon_sprites_static_init();
  improvement_icon_sprites_reg_atexit();
  return;
}

/* 00448963 auto */
void __cdecl improvement_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites),0x3c,0x43,sprite_init,sprite_destroy);
  return;
}

/* 0044898b auto */
void __cdecl improvement_icon_sprites_reg_atexit(void)

{
  _atexit(improvement_icon_sprites_static_dtor);
  return;
}

/* 004489a8 auto */
void __cdecl improvement_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites),0x3c,0x43,sprite_destroy);
  return;
}

/* 0044ac3e auto */
void __cdecl city_sprite_scan_flag_offset(int style,int row,int col,int sheet_x,int sheet_y)

{
  int pixel;
  int dy;
  int dx;
  
  peace_treaty_icon_sprite_block.city_flag_x_offsets[col + style * 8 + row * 2] = 0;
  peace_treaty_icon_sprite_block.city_flag_y_offsets[col + style * 8 + row * 2] = 0;
  for (dx = 0; dx < 0x40; dx++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(dx + sheet_x - 1 + 1,sheet_y - 1);
    if (pixel == 4) {
      peace_treaty_icon_sprite_block.city_flag_x_offsets[col + style * 8 + row * 2] = dx - 1;
    }
  }
  for (dy = 0; dy < 0x30; dy++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(sheet_x - 1,dy + sheet_y - 1 + 1);
    if (pixel == 4) {
      peace_treaty_icon_sprite_block.city_flag_y_offsets[col + style * 8 + row * 2] = dy - 0xf;
    }
  }
  return;
}

/* 0044ad47 auto */
void __cdecl city_sprite_scan_size_box_offset(int style,int row,int col,int sheet_x,int sheet_y)

{
  int pixel;
  int dy;
  int dx;
  
  road_sprites_block.city_size_box_x_offsets[row * 2 + style * 8 + col] = 0;
  road_sprites_block.city_label_y_offsets[row * 2 + style * 8 + col] = 0;
  for (dx = 0; dx < 0x40; dx++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(dx + sheet_x - 1 + 1,sheet_y - 1);
    if (pixel == 3) {
      road_sprites_block.city_size_box_x_offsets[row * 2 + style * 8 + col] = dx;
    }
  }
  for (dy = 0; dy < 0x30; dy++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(sheet_x - 1,dy + sheet_y - 1 + 1);
    if (pixel == 3) {
      road_sprites_block.city_label_y_offsets[row * 2 + style * 8 + col] = dy;
    }
  }
  return;
}

/* 00453da0 auto */
i32 __cdecl wonder_is_expired(int wonder)

{
  char expiry_advance;
  int has_advance;
  int civ;
  i32 expired;
  
  expired = 0;
  expiry_advance = wonder_expires[wonder];
  if (expiry_advance < 0) {
    expired = 0;
  }
  else {
    for (civ = 1; civ < 8; civ++) {
      has_advance = civ_has_advance(civ,expiry_advance);
      if (has_advance != 0) {
        expired = 1;
      }
    }
  }
  return expired;
}

/* 00453e18 auto */
int __cdecl wonder_get_city(int wonder)

{
  int result;
  
  result = wonder_is_expired(wonder);
  if (result == 0) {
    result = (int)game.wonder_city[wonder];
  }
  else {
    result = -1;
  }
  return result;
}

/* 00453edf auto */
int __cdecl wonder_get_owner(int wonder)

{
  iptr city;
  int civ;
  
  city = wonder_get_city(wonder);
  if (city < 0) {
    civ = -1;
  }
  else {
    civ = (int)cities[city].owner;
  }
  return civ;
}

/* 00490560 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 CityWindow::get_city()

{
  return ((u32)city_index);
}

/* 00498d40 auto */
void __cdecl city_load_citypref(void)

{
  int result;
  char *line;
  char *_Str1;
  char *_Str2;
  iptr found;
  iptr improvement;
  
  citypref.count = 0;
  result = text_open_section((const char *)&str_citypref_txt,(const char *)&str_nodefend);
  if (result == 0) {
    citypref.no_defend = 1;
  }
  text_close_file();
  result = text_open_section((const char *)&str_citypref_txt_2,(const char *)&str_autobuild);
  if (result == 0) {
    do {
      line = text_read_line(1);
      if (line == 0) break;
      text_next_token();
      found = -1;
      for (improvement = 1; improvement < 0x27; improvement++) {
        _Str2 = ((i8 *)&text_parse.token_buf);
        _Str1 = label_string(improvements[improvement].name);
        result = __strcmpi(_Str1,_Str2);
        if (result == 0) {
          found = improvement;
          break;
        }
      }
      if (0 < found) {
        (((i32 *)&map_views_block.citypref_improvements))[citypref.count] = found;
        citypref.count++;
      }
    } while (citypref.count < 0x20 && 0 < found);
    text_close_file();
  }
  return;
}

/* 004c03ae auto */
i32 __cdecl city_can_build_improvement(int civ,int city,int improvement)

{
  int ok;
  i32 result;
  
  result = 0;
  if (improvement < 0x27) {
    ok = civ_has_advance(civ,improvements[improvement].preq);
    if (ok != 0) {
      if (city < 0) {
        result = 1;
      }
      else {
        ok = city_has_improvement(city,improvement);
        if (((((ok == 0) &&
              (((improvement != IMP_HARBOR && (improvement != IMP_OFFSHORE_PLATFORM)) || ((cities[city].flags & CITYF_COASTAL) != 0))))
             && (((improvement != IMP_PORT_FACILITY && (improvement != IMP_COASTAL_FORTRESS)) ||
                 (((cities[city].flags & CITYF_OCEAN_ACCESS) != 0 && ((cities[city].flags & CITYF_COASTAL) != 0))
                 )))) &&
            ((((improvement != IMP_GRANARY || (ok = civ_has_wonder(civ,WONDER_PYRAMIDS), ok == 0)) &&
              ((improvement != IMP_CATHEDRAL || (ok = civ_has_wonder(civ,WONDER_MICHELANGELO), ok == 0)))) &&
             ((improvement != IMP_COURTHOUSE || (ok = city_has_improvement(city,IMP_PALACE), ok == 0)))))) &&
           (((((improvement != IMP_BANK ||
               ((ok = city_has_improvement(city,IMP_MARKETPLACE), ok != 0 &&
                (ok = civ_has_advance(civ,improvements[IMP_MARKETPLACE].preq), ok != 0)))) &&
              ((improvement != IMP_STOCK_EXCHANGE ||
               ((ok = city_has_improvement(city,IMP_BANK), ok != 0 &&
                (ok = civ_has_advance(civ,improvements[IMP_BANK].preq), ok != 0)))))) &&
             (((improvement != IMP_UNIVERSITY ||
               ((ok = city_has_improvement(city,IMP_LIBRARY), ok != 0 &&
                (ok = civ_has_advance(civ,improvements[IMP_LIBRARY].preq), ok != 0)))) &&
              (((improvement != IMP_RESEARCH_LAB ||
                (((ok = civ_has_wonder(civ,WONDER_SETI), ok == 0 &&
                  (ok = city_has_improvement(city,IMP_UNIVERSITY), ok != 0)) &&
                 (ok = civ_has_advance(civ,improvements[IMP_UNIVERSITY].preq), ok != 0)))) &&
               ((improvement != IMP_SEWER_SYSTEM || (ok = city_has_improvement(city,IMP_AQUEDUCT), ok != 0)))))))) &&
            ((((improvement != IMP_POLICE_STATION || (ok = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE), ok == 0)) &&
              ((((improvement != IMP_MFG_PLANT ||
                 ((ok = city_has_improvement(city,IMP_FACTORY), ok != 0 &&
                  (ok = civ_has_advance(civ,improvements[IMP_FACTORY].preq), ok != 0))))
                && ((improvement != IMP_HYDRO_PLANT || ((cities[city].flags & CITYF_HYDRO_POSSIBLE) != 0)))) &&
               (((((improvement != IMP_POWER_PLANT && (improvement != IMP_HYDRO_PLANT)) && (improvement != IMP_SOLAR_PLANT)) && (improvement != IMP_NUCLEAR_PLANT)
                 ) || (((ok = city_has_improvement(city,IMP_FACTORY), ok != 0 ||
                        (ok = city_has_improvement(city,IMP_MFG_PLANT), ok != 0)) &&
                       ((improvement == IMP_SOLAR_PLANT ||
                        ((((ok = civ_has_wonder(civ,WONDER_HOOVER_DAM), ok == 0 &&
                           (ok = city_has_improvement(city,IMP_HYDRO_PLANT), ok == 0)) &&
                          ((ok = city_has_improvement(city,IMP_NUCLEAR_PLANT), ok == 0 ||
                           (improvement == IMP_HYDRO_PLANT)))) &&
                         (ok = city_has_improvement(city,IMP_SOLAR_PLANT), ok == 0)))))))))))) &&
             (((improvement < 0x23 || (0x25 < improvement)) ||
              ((((game.rules & RULE_BLOODLUST) == 0 && (game.wonder_city[WONDER_APOLLO] != -1)) &&
               (((0 < game.year || ((game.game_flags & GAME_SCENARIO) != 0)) &&
                (ok = civ_spaceship_launched(civ), ok == 0)))))))))))) {
          result = 1;
        }
      }
    }
  }
  else {
    result = civ_can_build_wonder(civ,improvement - 0x27);
  }
  return result;
}

/* 004c66ba auto */
void __cdecl city_incite_transfer(int city,int civ,i32 reason)

{
  char *unit_counter;
  byte civ3;
  short x2;
  short y2;
  int civ2;
  iptr x;
  int y;
  int ok;
  iptr unit;
  iptr tile_index;
  
  civ3 = cities[city].owner;
  civ2 = (int)(char)civ3;
  x2 = cities[city].x;
  y2 = cities[city].y;
  if (-1 < civ) {
    net_batch_begin();
    for (tile_index = 0; tile_index < 0x15; tile_index++) {
      x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index] + (int)x2);
      y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index] + (int)y2;
      ok = map_in_bounds(x,y);
      if (ok != 0) {
        tile_set_visible(x,y,1 << ((byte)civ & 0x1f),1,1);
        tile_update_known(x,y,civ);
      }
    }
    net_batch_flush();
    if ((autoplay.active == 0) &&
       (((((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
            ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) ||
           ((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_EMBASSY) != 0)) ||
          ((y = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), y != 0 ||
           (y = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), y != 0)))) ||
         (((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ2] & TREATY_EMBASSY) != 0 ||
          ((y = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), y != 0 ||
           (y = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), y != 0)))))) ||
        (game.reveal_map != '\0')))) {
      if (((game.reveal_map != '\0') ||
          ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) != 0)) ||
         ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))) {
        mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,cities[city].owner)
        ;
      }
      x = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(0,(const char *)x);
      text_set_string_arg(1,(const char *)&cities[city].name);
      x = civ_get_adjective(civ);
      text_set_string_arg(2,(const char *)x);
      popup_game((const char *)&str_civilwar);
      if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer)) {
        net_send_text_args(0x5e,net.net_slots[net.net_civ_slots[civ2]].id,3,0,0,
                           0);
      }
    }
    unit = (iptr)game.num_units;
    while (unit = unit - 1, -1 < unit) {
      if (((units[unit].id != 0) && (units[unit].owner == civ2)) &&
         ((y = map_distance_diag((int)units[unit].x,(int)units[unit].y,(int)x2,
                                      (int)y2), y < 2 &&
          ((y == 0 ||
           (ok = tile_city_owner(units[unit].x,units[unit].y), ok < 0))))))
      {
        unit_counter = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *unit_counter = *unit_counter - 1;
        unit_counter = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *unit_counter = *unit_counter + 1;
        units[unit].owner = (byte)civ;
        units[unit].home_city = (i8)city;
        units[unit].moves_used = '\0';
        if ((units[unit].orders != ORDER_FORTIFYING) && (units[unit].orders != ORDER_FORTIFIED)) {
          units[unit].orders = ORDER_NONE;
        }
        tile_set_owner(units[unit].x,units[unit].y,civ,1);
        if (y != 0) {
          map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
        }
      }
    }
    city_capture_aftermath(city,civ,reason);
  }
  return;
}

/* 004e7270 auto */
int __cdecl city_acquire_wonder(int civ,int city,int wonder,int cost,int impr)

{
  char is_host;
  int start_time;
  int now;
  int acquired;
  
  acquired = 0;
  net.city_acquire_wonder_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    if (game.wonder_city[wonder] == -1) {
      acquired = 1;
      game.wonder_city[wonder] = (i16)city;
      cities[city].flags = cities[city].flags | CITYF_WONDER_BUILT;
      city_set_improvement(city,impr,1);
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) || (0x22 < impr)) {
        cities[city].shield_box = 0;
      }
    }
    if ((2 < game.multiplayer) && (acquired != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.city_acquire_wonder_busy = 0;
  }
  else {
    net.city_acquire_wonder_busy = 0;
    net.nm_wonder_acquired_result = -2;
    net_send_message(NM_ACQUIRE_WONDER,0,civ,city,wonder,cost,impr,0,0,0);
    start_time = timer_ticks();
    while ((net.nm_wonder_acquired_result == -2 && (now = timer_ticks(), now - start_time < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_wonder_acquired_result == -2) {
      dbg_log((const char *)&str_acquire_wonder_connection_to_server_time);
      acquired = 0;
    }
    else {
      while ((net.msgq_priority_count != 0 || (acquired = net.nm_wonder_acquired_result, net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return acquired;
}

/* 004e7458 auto */
i32 __cdecl city_production_category(int city,int build_item)

{
  i32 category;
  
  if (build_item < 0) {
    if (build_item < -0x22) {
      category = 2;
    }
    else {
      category = 1;
    }
  }
  else {
    category = 0;
  }
  return category;
}

/* 004e7492 auto */
void __cdecl city_save_build_state(int city)

{
  city_calc.saved_build_category = city_production_category(city,(int)cities[city].building);
  city_calc.saved_shield_box = (int)cities[city].shield_box;
  return;
}

/* 004e74df auto */
uint __cdecl city_shields_after_change(int city,int build_item)

{
  int category;
  uint shields;
  
  shields = (*(uint *)&city_calc.saved_shield_box);
  if ((game.difficulty != DIFF_CHIEFTAIN) &&
     (category = city_production_category(city,build_item), shields = (*(uint *)&city_calc.saved_shield_box), category != city_calc.saved_build_category)) {
    shields = (*(uint *)&city_calc.saved_shield_box) - ((uint)cosmic.change_penalty * ((*(uint *)&city_calc.saved_shield_box) & 0xffff)) / 100;
  }
  return shields;
}

/* 004e7549 auto */
void __cdecl city_set_specialist_type(int city,int slot,int type)

{
  byte shift;
  
  if (slot < 0x10) {
    shift = (char)slot * 2;
    cities[city].specialists = cities[city].specialists & ~(3 << (shift & 0x1f));
    cities[city].specialists = cities[city].specialists | type << (shift & 0x1f);
  }
  return;
}

/* 004e75a6 auto */
uint __cdecl city_specialist_type(int city,int slot)

{
  uint type;
  
  if (slot < 0x10) {
    type = (int)cities[city].specialists >> ((char)slot * 2 & 0x1fU) & 3;
  }
  else {
    type = 1;
  }
  return type;
}

/* 004e75ea auto */
int __cdecl city_count_specialists(int city,int type)

{
  int slot_type;
  i32 count;
  int slot;
  
  count = 0;
  for (slot = 0; slot < 0x10; slot++) {
    slot_type = city_specialist_type(city,slot);
    if (slot_type == type) {
      count++;
    }
  }
  return count;
}

/* 004e7641 auto */
void __cdecl city_calc_tile_flags(int city)

{
  byte civ2;
  int civ;
  iptr x;
  int y;
  byte *tile_flags;
  int result;
  uint tile_improv;
  iptr unit;
  iptr tile_index;
  
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  tile_index = 0;
  do {
    if (0x18 < tile_index) {
      return;
    }
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index] + (int)cities[city].x);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
    tile_flags = (byte *)(((i8 *)&city_calc.tile_flags) + tile_index);
    *tile_flags = 0;
    result = map_in_bounds(x,y);
    if ((result == 0) || (0x14 < tile_index)) {
      *tile_flags = *tile_flags | 1;
    }
    else {
      result = tile_visible_to(x,y,civ);
      if (result == 0) {
        *tile_flags = *tile_flags | 1;
      }
      else {
        if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (city_turn.processing != 0)) &&
           (tile_improv = tile_improvements(x,y), (tile_improv & TILE_POLLUTION) != 0)) {
          game.warming_pollution++;
        }
        unit = tile_first_unit(x,y);
        while (!(((unit < 0) || (units[unit].owner == civ)) ||
                ((result = tile_is_ocean(x,y), result != 0 ||
                 ((*(byte *)&civs[civ].treaties[units[unit].owner] & TREATY_ALLIANCE) != 0))))) {
          if (unit_types[units[unit].type].attack != '\0') {
            *tile_flags = *tile_flags | 4;
            if (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) {
              *tile_flags = *tile_flags | 0x20;
            }
            break;
          }
          unit = unit_next_in_stack(unit);
        }
        y = tile_city_owner(x,y);
        if (-1 < y) {
          *tile_flags = *tile_flags | 8;
        }
      }
    }
    tile_index++;
  } while( true );
}

/* 004e78ce auto */
uint __cdecl city_tile_worked(int city,int tile_index)

{
  return (uint)((cities[city].worked_tiles & 1 << ((byte)tile_index & 0x1f)) != 0);
}

/* 004e790c auto */
void __cdecl city_set_tile_worked(int city,int tile_index,int worked)

{
  uint mask;
  
  mask = 1 << ((byte)tile_index & 0x1f);
  if (worked == 0) {
    cities[city].worked_tiles = cities[city].worked_tiles & ~mask;
  }
  else {
    cities[city].worked_tiles = cities[city].worked_tiles | mask;
  }
  return;
}

/* 004e7967 auto */
void __cdecl city_calc_capital_link(int city)

{
  short x2;
  short y2;
  int x;
  int y;
  int x3;
  int y3;
  int dist;
  iptr city2;
  iptr near_tile;
  iptr tile_index;
  
  x = (int)cities[city].x;
  y = (int)cities[city].y;
  x3 = city_has_improvement(city,IMP_PALACE);
  if (x3 == 0) {
    city_calc.capital_distance = 0x20;
  }
  else {
    city_calc.capital_distance = 0;
  }
  city_calc.capital_nearest = -1;
  for (city2 = 0; city2 < game.num_cities; city2++) {
    if ((cities[city2].id != 0) && (city2 != city)) {
      x3 = map_distance_diag((int)cities[city].x,(int)cities[city].y,
                                 (int)cities[city2].x,(int)cities[city2].y);
      if ((cities[city2].owner == cities[city].owner) &&
         (y3 = city_has_improvement(city2,IMP_PALACE), y3 != 0)) {
        if ((x3 < city_calc.capital_distance) || (city_calc.capital_nearest < 0)) {
          city_calc.capital_nearest = city2;
        }
        if (x3 < city_calc.capital_distance) {
          city_calc.capital_distance = x3;
        }
      }
      if (x3 < 6) {
        for (tile_index = 0; tile_index < 0x15; tile_index++) {
          x3 = city_tile_worked(city2,tile_index);
          if ((x3 != 0) || (tile_index == 0x14)) {
            x3 = (int)cities[city2].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index];
            y3 = (int)cities[city2].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
            dist = map_distance_diag(x,y,x3,y3);
            if (dist < 3) {
              for (near_tile = 0; near_tile < 0x14; near_tile++) {
                if (((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[near_tile] == x3 - x) &&
                   ((int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[near_tile] == y3 - y)) {
                  (((i8 *)&city_calc.tile_flags))[near_tile] = (((i8 *)&city_calc.tile_flags))[near_tile] | 0x10;
                }
              }
            }
          }
        }
      }
    }
  }
  city_calc.capital_link = (uint)(city_calc.capital_distance == 0);
  if ((city_calc.capital_link != 0) &&
     (x3 = civ_has_advance(cities[city].owner,ADV_RAILROAD), x3 != 0)) {
    city_calc.capital_link++;
  }
  if ((-1 < city_calc.capital_nearest) && (city != city_calc.capital_nearest)) {
    x3 = city_has_improvement(city,IMP_AIRPORT);
    if ((x3 != 0) && (x3 = city_has_improvement(city_calc.capital_nearest,IMP_AIRPORT), x3 != 0)) {
      city_calc.capital_link = 1;
    }
    x3 = tile_landmass(x,y);
    x2 = cities[city_calc.capital_nearest].x;
    y2 = cities[city_calc.capital_nearest].y;
    y3 = tile_landmass(x2,y2);
    if (y3 == x3) {
      city_calc.capital_link = map_road_connection(cities[city].owner,x,y,(int)x2,(int)y2
                                       );
    }
  }
  return;
}

/* 004e7d7f auto */
i32 __cdecl city_count_unit_support(int city,int unit,i32 government)

{
  city_calc.support_unit_count++;
  switch(government) {
  case GOV_ANARCHY:
  case GOV_DESPOTISM:
    if (cities[city].size < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_MONARCHY:
    if ((int)(uint)cosmic.monarchy_free_support < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_COMMUNISM:
    if ((int)(uint)cosmic.communism_free_support < city_calc.support_unit_count) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  case GOV_FUNDAMENTALISM:
    if (((unit_types[units[unit].type].flags & UTYPE_FUNDAMENTALISM_FREE) == 0) &&
       ((int)(uint)cosmic.fundamentalism_free_support < city_calc.support_unit_count)) {
      city_calc.shield_support++;
      return city_calc.shield_support;
    }
    break;
  default:
    city_calc.shield_support++;
    return city_calc.shield_support;
  }
  return 0;
}

/* 004e7eb1 auto */
void __cdecl city_calc_food_box_rows(int city,int civ)

{
  byte food_cost;
  int adjusted;
  uint settler_food;
  int penalty;
  
  food_cost = cosmic.settler_food_low;
  if (GOV_MONARCHY < civs[civ].government) {
    food_cost = cosmic.settler_food_high;
  }
  settler_food = (uint)food_cost;
  (*(uint *)&city_calc.settler_food_cost) = settler_food;
  (*(uint *)&city_calc.food_box_rows) = (uint)cosmic.food_box_rows;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    (*(uint *)&city_calc.food_box_rows) = -(uint)game.difficulty + 0xd;
    if (game.difficulty < DIFF_KING) {
      (*(uint *)&city_calc.food_box_rows) = -(uint)game.difficulty + 0xe;
    }
    if (game.difficulty == DIFF_CHIEFTAIN) {
      (*(uint *)&city_calc.food_box_rows)++;
    }
    if (((200 < game.turn) && (DIFF_WARLORD < game.difficulty)) &&
       (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0)) {
      penalty = 2;
      if (((game.wonder_city[WONDER_MANHATTAN] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_ROCKETRY]) != 0)) &&
         ((penalty = 1, (game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0 ||
          ((game.human_players & game.advance_known_by[ADV_NUCLEAR_FISSION]) == 0)))) {
        penalty = 0;
      }
      adjusted = penalty;
      if (((game.wonder_city[WONDER_APOLLO] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_SPACE_FLIGHT]) != 0)) &&
         (adjusted = penalty - 1, (game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) == 0)) {
        adjusted = penalty - 2;
      }
      penalty = adjusted;
      adjusted = int_clamp(penalty,0,2);
      (*(uint *)&city_calc.food_box_rows) -= adjusted;
    }
    if ((cosmic.food_box_rows != '\n') &&
       ((*(uint *)&city_calc.food_box_rows) = (int)(cosmic.food_box_rows * (*(uint *)&city_calc.food_box_rows)) / 10, ((*(uint *)&city_calc.food_box_rows) & 1) != 0)) {
      (*(uint *)&city_calc.food_box_rows)++;
    }
  }
  return;
}

/* 004e80b1 auto */
void __cdecl city_calc_unit_support(uptr city)

{
  byte civ2;
  byte government;
  iptr civ;
  int result;
  int check;
  i32 tile_improv;
  iptr unit;
  iptr city2;
  int penalty;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  government = civs[civ].government;
  city_calc_food_box_rows(city,civ);
  (*(uint *)&city_calc.shield_rows) = (uint)cosmic.shield_box_rows;
  if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
    (*(uint *)&city_calc.shield_rows) = 0xd - game.difficulty;
    if (game.difficulty == DIFF_CHIEFTAIN) {
      (*(uint *)&city_calc.shield_rows) = 0xe;
    }
    if (game.difficulty < DIFF_KING) {
      (*(uint *)&city_calc.shield_rows)++;
    }
    if (((199 < game.turn) && (DIFF_WARLORD < game.difficulty)) &&
       (((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0)) {
      penalty = 2;
      if (((game.wonder_city[WONDER_MANHATTAN] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_ROCKETRY]) != 0)) &&
         ((penalty = 1, (game.human_players & game.advance_known_by[ADV_ROCKETRY]) == 0 ||
          ((game.human_players & game.advance_known_by[ADV_NUCLEAR_FISSION]) == 0)))) {
        penalty = 0;
      }
      result = penalty;
      if (((game.wonder_city[WONDER_APOLLO] != -1) &&
          ((~(uint)game.human_players & (int)(char)game.advance_known_by[ADV_SPACE_FLIGHT]) != 0)) &&
         (result = penalty - 1, (game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) == 0)) {
        result = penalty - 2;
      }
      penalty = result;
      result = int_clamp(penalty,0,2);
      (*(uint *)&city_calc.shield_rows) -= result;
    }
    if ((cosmic.shield_box_rows != '\n') &&
       ((*(uint *)&city_calc.shield_rows) = (int)(cosmic.shield_box_rows * (*(uint *)&city_calc.shield_rows)) / 10, ((*(uint *)&city_calc.shield_rows) & 1) != 0))
    {
      (*(uint *)&city_calc.shield_rows)++;
    }
  }
  city_calc.shield_support = 0;
  city_calc.support_unit_count = 0;
  city_calc.settlers_supported = 0;
  city_calc.aggressive_units = 0;
  city_calc.home_attack_units = 0;
  unit = 0;
  do {
    if (game.num_units <= unit) {
      return;
    }
    if ((((units[unit].id != 0) && (units[unit].owner == civ)) &&
        ((byte)units[unit].home_city == city)) &&
       (units[unit].flags = units[unit].flags & ~(UNITF_AGGRESSIVE | UNITF_COSTS_SHIELD),
       (char)unit_types[units[unit].type].role < 6)) {
      result = city_count_unit_support(city,unit,(uint)government);
      if (result != 0) {
        units[unit].flags = units[unit].flags | UNITF_COSTS_SHIELD;
      }
      result = city_calc.aggressive_units;
      if (unit_types[units[unit].type].attack != '\0') {
        if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
          if (unit_types[units[unit].type].role != ROLE_AIR_SUPERIORITY) {
            city_calc.aggressive_units++;
          }
          if ((units[unit].x == cities[city].x) || (units[unit].y == cities[city].y))
          {
            city_calc.home_attack_units++;
          }
        }
        else if ((units[unit].x == cities[city].x) &&
                (units[unit].y == cities[city].y)) {
          city_calc.home_attack_units++;
        }
        else {
          check = tile_city_owner(units[unit].x,units[unit].y);
          if (check < 0) {
            city_calc.aggressive_units++;
            tile_improv = tile_improvements(units[unit].x,units[unit].y);
            if (((byte)tile_improv & TILE_AIRBASE) == 0x40) {
              for (city2 = 0; city2 < game.num_cities; city2++) {
                if (((cities[city2].id != 0) && (cities[city2].owner == civ)) &&
                   (check = map_distance((int)units[unit].x,(int)units[unit].y,
                                               (int)cities[city2].x,(int)cities[city2].y),
                   check < 4)) {
                  city_calc.aggressive_units--;
                  break;
                }
              }
            }
          }
        }
      }
      if (city_calc.aggressive_units != result) {
        units[unit].flags = units[unit].flags | UNITF_AGGRESSIVE;
      }
      if (unit_types[units[unit].type].role == ROLE_SETTLE) {
        city_calc.settlers_supported++;
      }
    }
    unit++;
  } while( true );
}

/* 004e868f auto */
int __cdecl city_tile_yield(int city,int tile_index,int yield_type)

{
  byte terrain_byte;
  iptr civ;
  iptr x;
  int y;
  int result;
  uint tile_improv;
  byte *tile;
  uptr terrain;
  int yield_value;
  byte improv_bits;
  
  civ = (iptr)cities[city].owner;
  x = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index]);
  y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
  result = map_in_bounds(x,y);
  if (result == 0) {
    yield_value = 0;
  }
  else {
    if (city_turn.yield_tile_worked != 0) {
      tile_set_radius_civ(x,y,civ,1);
      result = tile_city_or_unit_owner(x,y);
      if (result < 0) {
        tile_set_owner(x,y,civ,1);
      }
    }
    terrain_byte = tile_terrain(x,y);
    terrain = (uptr)terrain_byte;
    result = tile_resource(x,y);
    yield_value = (int)*(char *)(yield_type + (iptr)((u8 *)&terrain_types + 10) + (result * 0xb + terrain) * 0x18);
    if ((game.multiplayer != '\0') && (mp_doubles.production != 0)) {
      yield_value = yield_value << 1;
    }
    tile_improv = tile_improvements(x,y);
    improv_bits = (byte)tile_improv;
    if (terrain == TERRAIN_OCEAN) {
      if ((yield_type == 0) && (result = city_has_improvement(city,IMP_HARBOR), result != 0)) {
        yield_value++;
      }
      if ((yield_type == 1) && (result = city_has_improvement(city,IMP_OFFSHORE_PLATFORM), result != 0)) {
        yield_value++;
      }
      if ((yield_type == 2) && (result = wonder_get_city(WONDER_COLOSSUS), result == city)) {
        yield_value++;
      }
    }
    else if (yield_type == 0) {
      if ((tile_improv & (TILE_CITY | TILE_IRRIGATION)) == 0) {
        city_note_terraform_job(city,civ,0,x,y);
      }
      else {
        yield_value += (char)terrain_types[terrain].irrigate_bonus;
        result = city_has_improvement(city,IMP_SUPERMARKET);
        if (result != 0) {
          if ((tile_improv & (TILE_CITY | TILE_MINE)) == 0) {
            city_note_terraform_job(city,civ,0,x,y);
          }
          else {
            yield_value += (yield_value >> 1);
          }
        }
      }
    }
    else if (yield_type == 1) {
      if (((improv_bits & 0xc) == 8) ||
         (((tile_improv & TILE_CITY) != 0 && (terrain_types[terrain].irrigate_bonus == '\0')))) {
        yield_value += (char)terrain_types[terrain].mine_bonus;
      }
      else if ((tile_improv & TILE_FARMLAND) == 0) {
        city_note_terraform_job(city,civ,1,x,y);
      }
      if ((terrain == TERRAIN_GRASSLAND) && (result = tile_grassland_has_shield(x,y), result == 0)) {
        yield_value = 0;
      }
      if ((tile_index == 0x14) && (yield_value == 0)) {
        yield_value = 1;
      }
    }
    else if (yield_type == 2) {
      tile = (byte *)tile_at(x,y);
      if ((*tile & 0x80) != 0) {
        yield_value++;
      }
      if ((tile_improv & (TILE_CITY | TILE_ROAD)) == 0) {
        if ((terrain < 3) || (yield_value != 0)) {
          city_note_road_job(city,civ,x,y);
        }
      }
      else if ((terrain < 3) || (yield_value != 0)) {
        yield_value++;
      }
      if ((yield_value != 0) && (result = wonder_get_city(WONDER_COLOSSUS), result == city)) {
        yield_value++;
      }
    }
    if ((yield_type == 1) && (result = wonder_get_city(WONDER_KING_RICHARDS), result == city)) {
      yield_value++;
    }
    if (yield_type == 1) {
      if (((tile_improv & TILE_RAILROAD) == 0) &&
         (((tile_improv & TILE_CITY) == 0 || (result = civ_has_advance(civ,ADV_RAILROAD), result == 0)))) {
        if ((((tile_improv & (TILE_CITY | TILE_RAILROAD)) == 0) && (result = civ_has_advance(civ,ADV_RAILROAD), result != 0)) &&
           (terrain != TERRAIN_OCEAN)) {
          city_note_road_job(city,civ,x,y);
        }
      }
      else {
        yield_value += (yield_value >> 1);
      }
    }
    if ((2 < yield_value) && ((cities[city].flags & CITYF_CELEBRATING) == 0)) {
      if (city_turn.processing != 0) {
        civs[civ].government_score[GOV_DESPOTISM]--;
      }
      if (civs[civ].government < GOV_MONARCHY) {
        yield_value--;
      }
    }
    if ((yield_value != 0) && (yield_type == 2)) {
      if (city_turn.processing != 0) {
        civs[civ].government_score[GOV_REPUBLIC]++;
        civs[civ].government_score[GOV_DEMOCRACY]++;
      }
      if ((byte)((-((cities[city].flags & CITYF_CELEBRATING) == 0) & 3U) + 2) <= civs[civ].government) {
        yield_value++;
      }
    }
    if (((yield_type == 2) && ((tile_improv & (TILE_CITY | TILE_ROAD)) != 0)) &&
       (result = city_has_improvement(city,IMP_SUPERHIGHWAYS), result != 0)) {
      yield_value += yield_value / 2;
    }
    if ((tile_improv & TILE_POLLUTION) != 0) {
      yield_value = yield_value + 1 >> 1;
      city_calc.tile_job_priority++;
      city_turn.tile_job_found = 1;
      city_calc.tile_job_x = x;
      city_calc.tile_job_y = y;
    }
    if (yield_value < 1) {
      yield_value = 0;
    }
  }
  return yield_value;
}

/* 004e8c8c auto */
void __cdecl
city_note_terraform_job(int city,int civ,int job_type,i32 x,i32 y)

{
  byte terrain_byte;
  int has_source;
  uint terrain;
  uint govt_level;
  
  if (((city_turn.yield_tile_worked != 0) && (city_turn.processing != 0)) && (city_turn.tile_job_found == 0)) {
    terrain_byte = tile_terrain(x,y);
    terrain = (uint)terrain_byte;
    if (((*(char *)(job_type + (iptr)((u8 *)&terrain_types + 14) + terrain * 0x18) != -1) &&
        (*(char *)(job_type + (iptr)((u8 *)&terrain_types + 20) + terrain * 0x18) != '\0')) &&
       ((job_type != 0 || (has_source = tile_has_irrigation_source(x,y), has_source != 0)))) {
      govt_level = (uint)civs[civ].government;
      if (civs[civ].government < GOV_MONARCHY) {
        govt_level = 1;
      }
      if ((int)*(char *)(job_type + (iptr)((u8 *)&terrain_types + 20) + terrain * 0x18) <= (int)govt_level) {
        city_calc.tile_job_priority++;
        city_turn.tile_job_found = 1;
        city_calc.tile_job_x = x;
        city_calc.tile_job_y = y;
      }
    }
  }
  return;
}

/* 004e8db5 auto */
void __cdecl city_note_road_job(int city,int civ,int x,int y)

{
  byte *tile;
  int has_adv;
  
  if ((((city_turn.yield_tile_worked != 0) && (city_turn.processing != 0)) && (city_turn.tile_job_found == 0)) &&
     ((tile = (byte *)tile_at(x,y), (*tile & 0x80) == 0 ||
      (has_adv = civ_has_advance(civ,ADV_BRIDGE_BUILDING), has_adv != 0)))) {
    city_turn.tile_job_found = 1;
    city_calc.tile_job_x = x;
    city_calc.tile_job_y = y;
  }
  return;
}

/* 004e8e4d auto */
void __cdecl city_calc_tile_yields(int city,int tile_index,int worked)

{
  i32 yield_value;
  iptr yield_type;
  
  city_turn.yield_tile_worked = worked;
  for (yield_type = 0; yield_type < 3; yield_type++) {
    yield_value = city_tile_yield(city,tile_index,yield_type);
    (((i32 *)&city_calc.tile_yields))[yield_type] = yield_value;
    if (city_turn.yield_tile_worked != 0) {
      (((int *)&city_calc.yield_totals))[yield_type] = (((int *)&city_calc.yield_totals))[yield_type] + (((i32 *)&city_calc.tile_yields))[yield_type];
    }
  }
  city_turn.yield_tile_worked = 0;
  return;
}

/* 004e8ecf auto */
uint __cdecl city_release_blocked_tiles(int city)

{
  uint status;
  i32 enemy_flags;
  int tile_index;
  
  enemy_flags = 0;
  for (tile_index = 0; tile_index < 0x14; tile_index++) {
    status = city_tile_status_flags(city,tile_index);
    enemy_flags = enemy_flags | status & 0x20;
    if (status != 0) {
      city_set_tile_worked(city,tile_index,0);
    }
  }
  return enemy_flags;
}

/* 004e8f42 auto */
void __cdecl city_assign_workers(int city)

{
  byte civ;
  char size;
  byte food_per_citizen;
  int score;
  int shield_score;
  int trade_score;
  iptr tile;
  uint worked_mask;
  int best_score;
  int best_food;
  iptr tile_index;
  uint specialists2;
  iptr best_tile;
  int tile_taken[21];
  int kept;
  
  worked_mask = 0;
  kept = 0;
  city_calc.workers_overflow = 0;
  civ = cities[city].owner;
  (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
  specialists2 = cities[city].worked_tiles >> 0x1a;
  if ((1 << (civ & 0x1f) & (uint)game.human_players) == 0) {
    if ((cities[city].flags & CITYF_DISORDER) == 0) {
      if ((city_turn.worker_realloc != 0) && (specialists2 != 0)) {
        if ((cities[city].flags & CITYF_ON_EDGE) == 0) {
          specialists2--;
        }
        else {
          cities[city].flags = cities[city].flags | CITYF_AI_SPECIALIST_HELD;
        }
      }
    }
    else {
      cities[city].worked_tiles = 0;
    }
  }
  if (specialists2 == 0) {
    cities[city].flags = cities[city].flags & ~CITYF_AI_SPECIALIST_HELD;
  }
  score = city_release_blocked_tiles(city);
  for (tile_index = 0; tile_index < 3; tile_index++) {
    (((int *)&city_calc.yield_totals))[tile_index] = 0;
  }
  for (tile_index = 0; tile_index < 0x14; tile_index++) {
    shield_score = city_tile_status_flags(city,tile_index);
    tile_taken[tile_index] = shield_score;
  }
  tile_taken[20] = 0;
  if (((((1 << (civ & 0x1f) & (uint)game.human_players) != 0) || ((game.turn + city & 3U) != 0)
       ) || ((cities[city].flags & CITYF_DISORDER) != 0)) || (score != 0)) {
    for (tile_index = 0; tile_index < 0x15; tile_index++) {
      score = city_tile_worked(city,tile_index);
      if (score != 0) {
        (*(uint *)&city_calc.unassigned_citizens)--;
      }
    }
    if ((int)(*(uint *)&city_calc.unassigned_citizens) < 0) {
      city_calc.workers_overflow = 1;
      (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
    }
    else {
      (*(uint *)&city_calc.unassigned_citizens) = (int)cities[city].size + 1;
      for (tile_index = 0; tile_index < 0x15; tile_index++) {
        if (tile_index == 0) {
          tile = 0x14;
        }
        else {
          tile = tile_index - 1;
        }
        score = city_tile_worked(city,tile);
        if (score != 0) {
          city_calc_tile_yields(city,tile,1);
          tile_taken[tile] = 1;
          worked_mask = worked_mask | 1 << ((byte)tile & 0x1f);
          (*(uint *)&city_calc.unassigned_citizens)--;
          if ((*(uint *)&city_calc.unassigned_citizens) == 0) break;
        }
      }
    }
    if (((*(uint *)&city_calc.unassigned_citizens) == specialists2) || ((*(uint *)&city_calc.unassigned_citizens) == 0)) {
      kept = 1;
    }
  }
  /* the citizens kept on their tiles: nothing more to place */
  if (kept == 0) {
    if ((0 < (int)(*(uint *)&city_calc.unassigned_citizens)) && (tile_taken[20] == 0)) {
      worked_mask = worked_mask | 0x100000;
      city_calc_tile_yields(city,0x14,1);
      tile_taken[20] = 1;
      (*(uint *)&city_calc.unassigned_citizens)--;
    }
    best_tile = 0;
    food_per_citizen = cosmic.food_per_citizen;
    if (cosmic.food_per_citizen < 2) {
      food_per_citizen = 1;
    }
    while (((city_calc.yield_totals[0] + (int)(*(uint *)&city_calc.unassigned_citizens) / (int)(uint)food_per_citizen <
             (int)((int)cities[city].size * (uint)cosmic.food_per_citizen +
                  city_calc.settlers_supported * city_calc.settler_food_cost) || (cities[city].size < 3)) &&
           (((*(uint *)&city_calc.unassigned_citizens) != specialists2 && -1 < (int)((*(uint *)&city_calc.unassigned_citizens) - specialists2) && (-1 < best_tile))))) {
      best_tile = -1;
      best_score = 0;
      best_food = 0;
      for (tile_index = 0; tile_index < 0x14; tile_index++) {
        if (tile_taken[tile_index] == 0) {
          score = city_tile_yield(city,tile_index,0);
          shield_score = city_tile_yield(city,tile_index,1);
          if ((((*(uint *)&city_calc.unassigned_citizens) != 1) || (city_calc.total_shields != 0)) || (shield_score * 2 != 0)) {
            trade_score = city_tile_yield(city,tile_index,2);
            trade_score = shield_score * 2 + trade_score;
            if ((best_food < score) || ((score == best_food && (best_score < trade_score)))) {
              best_tile = tile_index;
              best_score = trade_score;
              best_food = score;
            }
          }
        }
      }
      if (-1 < best_tile) {
        tile_taken[best_tile] = 1;
        city_calc_tile_yields(city,best_tile,1);
        worked_mask = worked_mask | 1 << ((byte)best_tile & 0x1f);
        (*(uint *)&city_calc.unassigned_citizens)--;
      }
    }
    if ((((1 << (civ & 0x1f) & (uint)game.human_players) == 0) && ((cities[city].flags & CITYF_DISORDER) != 0)
        ) && (city_turn.worker_realloc != 0)) {
      cities[city].flags = cities[city].flags | CITYF_AI_SPECIALIST_HELD;
      specialists2++;
    }
    for (; (*(uint *)&city_calc.unassigned_citizens) != specialists2 && -1 < (int)((*(uint *)&city_calc.unassigned_citizens) - specialists2);
        (*(uint *)&city_calc.unassigned_citizens) = (*(uint *)&city_calc.unassigned_citizens) - 1) {
      best_tile = -1;
      best_score = 0;
      for (tile_index = 0; tile_index < 0x14; tile_index++) {
        if (tile_taken[tile_index] == 0) {
          city_calc_tile_yields(city,tile_index,0);
          score = int_clamp((city_calc.yield_totals[0] -
                                     (int)cities[city].size * (uint)cosmic.food_per_citizen) -
                                     city_calc.settlers_supported * city_calc.settler_food_cost,1,99);
          score = (int)(0x10 / (longlong)score) * city_calc.tile_yields[0];
          size = cities[city].size;
          shield_score = int_clamp(city_calc.total_shields - city_calc.shield_support,1,99);
          shield_score = ((size * 3) / shield_score) * city_calc.tile_shields;
          size = cities[city].size;
          trade_score = int_clamp(city_calc.total_trade,1,99);
          score = score + shield_score + ((size * 2) / trade_score) * city_calc.tile_trade;
          if (best_score < score) {
            best_tile = tile_index;
            best_score = score;
          }
        }
      }
      if (best_tile < 0) break;
      tile_taken[best_tile] = 1;
      city_calc_tile_yields(city,best_tile,1);
      worked_mask = worked_mask | 1 << ((byte)best_tile & 0x1f);
    }
  }
  cities[city].worked_tiles = (*(uint *)&city_calc.unassigned_citizens) * 0x4000000 + worked_mask;
  if (city_calc.yield_totals[0] <
      (int)((int)cities[city].size * (uint)cosmic.food_per_citizen + city_calc.settlers_supported * city_calc.settler_food_cost)
     ) {
    if (cities[city].food_box == 0) {
      cities[city].food_box--;
    }
  }
  else if (cities[city].food_box < 0) {
    cities[city].food_box = 0;
  }
  return;
}

/* 004e9719 auto */
void __cdecl city_adjust_specialists(int city,int delta)

{
  if (delta < 0) {
    for (; delta != 0; delta++) {
      if (0x3ffffff < cities[city].worked_tiles) {
        cities[city].worked_tiles += 0xfc000000;
      }
    }
  }
  else {
    for (; delta != 0; delta--) {
      cities[city].worked_tiles += 0x4000000;
    }
  }
  return;
}

/* 004e97ae auto */
void __cdecl city_fill_specialists(int city)

{
  int type;
  int slot;
  
  city_calc.specialist_count = city_calc.unassigned_citizens;
  for (slot = 0; slot < city_calc.specialist_count; slot++) {
    type = city_specialist_type(city,slot);
    if (type == 0) {
      city_set_specialist_type(city,slot,1);
    }
  }
  for (slot = city_calc.specialist_count; slot < 0x10; slot++) {
    city_set_specialist_type(city,slot,0);
  }
  return;
}

/* 004e9849 auto */
int __cdecl city_corruption_divisor(int city,int government)

{
  i32 divisor;
  
  divisor = 4;
  if (GOV_ANARCHY < government) {
    divisor = 5;
  }
  if (GOV_DESPOTISM < government) {
    divisor++;
  }
  if (GOV_MONARCHY < government) {
    divisor++;
  }
  if (GOV_FUNDAMENTALISM < government) {
    divisor++;
  }
  return divisor;
}

/* 004e989a auto */
int __cdecl city_calc_corruption(int city,int amount,int clamp_distance,int update_scores)

{
  byte civ2;
  iptr civ;
  int divisor;
  uint palace_dist;
  bool is_communism;
  int govt;
  int communism_corruption;
  uint govt_level;
  int adj_distance;
  int distance;
  int base_corruption;
  int corruption;
  
  if (amount < 1) {
    corruption = 0;
  }
  else {
    civ2 = cities[city].owner;
    civ = (iptr)(char)civ2;
    govt_level = (uint)civs[civ].government;
    is_communism = govt_level == 3;
    if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
      govt_level++;
    }
    if (((city_turn.processing != 0) && (update_scores != 0)) || (!is_communism)) {
      distance = city_calc.capital_distance;
      if ((clamp_distance != 0) && (0x10 < city_calc.capital_distance)) {
        distance = 0x10;
      }
      adj_distance = distance;
      if (((govt_level < 2) && (distance != 0)) &&
         ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
        adj_distance = int_clamp((uint)game.difficulty + distance,0,0x20);
      }
      divisor = city_corruption_divisor(city,govt_level);
      base_corruption = (adj_distance * amount * 3) / (divisor * 0x14);
    }
    if (((city_turn.processing != 0) && (update_scores != 0)) || (is_communism)) {
      palace_dist = (uint)cosmic.communism_palace_dist;
      divisor = city_corruption_divisor(city,govt_level);
      communism_corruption = (int)(palace_dist * amount * 3) / (divisor * 0x14);
    }
    if (is_communism) {
      corruption = communism_corruption;
    }
    else {
      corruption = base_corruption;
    }
    corruption = int_clamp(corruption,0,amount);
    divisor = city_has_improvement(city,IMP_COURTHOUSE);
    if ((divisor != 0) || (divisor = city_has_improvement(city,IMP_PALACE), divisor != 0)) {
      corruption = corruption >> 1;
    }
    if ((city_turn.processing != 0) && (update_scores != 0)) {
      for (govt = 1; govt < GOV_DEMOCRACY; govt++) {
        govt_level = govt;
        if (govt == GOV_COMMUNISM) {
          civs[civ].government_score[GOV_COMMUNISM] -= (short)communism_corruption;
        }
        else if (govt != GOV_FUNDAMENTALISM) {
          if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
            govt_level = govt + 1;
          }
          adj_distance = distance;
          if ((((int)govt_level < 2) && (distance != 0)) &&
             ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
            adj_distance = int_clamp((uint)game.difficulty + distance,0,0x20);
          }
          divisor = city_corruption_divisor(city,govt_level);
          civs[civ].government_score[govt] =
               civs[civ].government_score[govt] -
               (short)((adj_distance * amount * 3) / (divisor * 0x14));
        }
      }
    }
  }
  return corruption;
}

/* 004e9c14 auto */
void __cdecl city_calc_shields_pollution(int city)

{
  iptr civ;
  int result;
  uint power_bonus;
  uint factory_bonus;
  
  factory_bonus = 0;
  power_bonus = 0;
  civ = (iptr)cities[city].owner;
  city_calc.pollution_divisor = 1;
  result = city_has_improvement(city,IMP_FACTORY);
  if (result != 0) {
    factory_bonus = 2;
  }
  result = city_has_improvement(city,IMP_MFG_PLANT);
  if (result != 0) {
    factory_bonus += 2;
  }
  result = city_has_improvement(city,IMP_POWER_PLANT);
  if (result != 0) {
    power_bonus = 2;
  }
  result = city_has_improvement(city,IMP_HYDRO_PLANT);
  if ((result != 0) || (result = city_has_improvement(city,IMP_NUCLEAR_PLANT), result != 0) ||
     (result = civ_has_wonder(cities[city].owner,WONDER_HOOVER_DAM), result != 0)) {
    power_bonus = 2;
    city_calc.pollution_divisor = 2;
  }
  result = city_has_improvement(city,IMP_RECYCLING_CENTER);
  if (result != 0) {
    city_calc.pollution_divisor = 3;
  }
  result = city_has_improvement(city,IMP_SOLAR_PLANT);
  if (result != 0) {
    power_bonus = 2;
    city_calc.pollution_divisor = 3;
  }
  if (factory_bonus <= power_bonus) {
    power_bonus = factory_bonus;
  }
  city_calc.total_shields = city_calc.total_shields +
                 ((int)(city_calc.total_shields * factory_bonus) >> 2) + ((int)(city_calc.total_shields * power_bonus) >> 2);
  city_calc.pop_pollution_factor = 0;
  result = city_has_improvement(city,IMP_MASS_TRANSIT);
  if (result == 0) {
    result = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_AUTOMOBILE);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_MASS_PRODUCTION);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    result = civ_has_advance(civ,ADV_PLASTICS);
    if (result != 0) {
      city_calc.pop_pollution_factor++;
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = civ_has_advance(civ,ADV_SANITATION);
      if (result == 0) {
        city_calc.pop_pollution_factor++;
      }
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = civ_has_advance(civ,ADV_ENVIRONMENTALISM);
      if (result != 0) {
        city_calc.pop_pollution_factor--;
      }
    }
    if (city_calc.pop_pollution_factor != 0) {
      result = city_has_improvement(city,IMP_SOLAR_PLANT);
      if (result != 0) {
        city_calc.pop_pollution_factor--;
      }
    }
  }
  city_calc.pollution = city_calc.total_shields / city_calc.pollution_divisor - 0x14;
  result = city_has_improvement(city,IMP_SOLAR_PLANT);
  if (result != 0) {
    city_calc.pollution = 0;
  }
  city_calc.pollution += (cities[city].size * city_calc.pop_pollution_factor >> 2);
  result = city_calc_corruption(city,city_calc.total_shields - city_calc.shield_support,1,1);
  result = (((3 - (*(int *)&city_calc.capital_link)) * result) / 3) / (((int)(uint)civs[civ].government >> 1) + 1);
  city_calc.shield_waste = city_calc.total_shields - (city_calc.shield_support + 1);
  if (result <= city_calc.shield_waste) {
    city_calc.shield_waste = result;
  }
  if (city_calc.shield_waste < 1) {
    city_calc.shield_waste = 0;
  }
  if ((((civ == 0) || (civs[civ].government == GOV_DEMOCRACY)) || (civs[civ].government == GOV_FUNDAMENTALISM))
     || ((cities[city].building == -IMP_PALACE && (city_calc.capital_nearest < 0)))) {
    city_calc.shield_waste = 0;
  }
  city_calc.total_shields -= city_calc.shield_waste;
  return;
}

/* 004ea031 auto */
void __cdecl city_happy_clamp_stage(int city,int stage)

{
  int working;
  
  (*(i8 *)&city_calc.happy_count) = int_clamp((*(i8 *)&city_calc.happy_count),0,(int)cities[city].size);
  for (; ((*(i8 *)&city_calc.angry_count) != 0 && ((*(i8 *)&city_calc.unhappy_count) < (*(i8 *)&city_calc.angry_count))); (*(i8 *)&city_calc.unhappy_count)++) {
    (*(i8 *)&city_calc.angry_count)--;
  }
  (*(i8 *)&city_calc.unhappy_count) = int_clamp((*(i8 *)&city_calc.unhappy_count),0,(int)cities[city].size);
  while( true ) {
    working = int_clamp(cities[city].size - city_calc.unassigned_citizens,0,99);
    if ((*(i8 *)&city_calc.unhappy_count) + (*(i8 *)&city_calc.happy_count) <= working) break;
    if ((*(i8 *)&city_calc.angry_count) == 0) {
      (*(i8 *)&city_calc.happy_count)--;
      (*(i8 *)&city_calc.happy_count) = int_clamp((*(i8 *)&city_calc.happy_count),0,(int)cities[city].size);
    }
    else {
      (*(i8 *)&city_calc.angry_count)--;
    }
    (*(i8 *)&city_calc.unhappy_count)--;
    (*(i8 *)&city_calc.unhappy_count) = int_clamp((*(i8 *)&city_calc.unhappy_count),0,(int)cities[city].size);
  }
  for (; ((*(i8 *)&city_calc.angry_count) != 0 && ((*(i8 *)&city_calc.unhappy_count) + (*(i8 *)&city_calc.happy_count) < cities[city].size - city_calc.unassigned_citizens));
      (*(i8 *)&city_calc.unhappy_count) = (*(i8 *)&city_calc.unhappy_count) + 1) {
    (*(i8 *)&city_calc.angry_count)--;
  }
  if (-1 < stage) {
    (((i8 *)&city_calc.happy_by_stage))[stage] = *(i8 *)&city_calc.happy_count;
    (((i8 *)&city_calc.unhappy_by_stage))[stage] = *(i8 *)&city_calc.unhappy_count;
    (((i8 *)&city_calc.angry_by_stage))[stage] = *(i8 *)&city_calc.angry_count;
  }
  return;
}

/* 004ea1f6 auto */
void __cdecl city_split_trade(int city,int trade,int mode,int extra_tax)

{
  byte civ2;
  iptr civ;
  int result;
  int result2;
  uint science_rate2;
  int science_bonus;
  int orig_trade;
  uint bonus_level;
  
  if ((mode == 1) && (cities[city].building == -IMP_CAPITALIZATION)) {
    orig_trade = trade;
    trade = (int)cities[city].trade;
  }
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  science_rate2 = (uint)civs[civ].science_rate;
  if ((civs[civ].government == GOV_FUNDAMENTALISM) && (cosmic.fund_max_science < science_rate2)) {
    science_rate2 = (uint)cosmic.fund_max_science;
  }
  city_calc.lux_total = int_clamp((int)(trade * (10 - ((uint)civs[civ].science_rate +
                                                          (uint)civs[civ].tax_rate)) + 4) / 10,0,
                                    trade);
  city_calc.science_total = int_clamp((int)(trade * science_rate2 + 4) / 10,0,trade - city_calc.lux_total);
  city_calc.tax_total = trade - (city_calc.science_total + city_calc.lux_total);
  if (city_turn.processing != 0) {
    civs[civ].government_score[GOV_FUNDAMENTALISM] += (short)(city_calc.lux_total / 2);
    result = (leaders[civs[civ].tribe].civilize + 3) * city_calc.science_total;
    civs[civ].government_score[GOV_FUNDAMENTALISM] =
         civs[civ].government_score[GOV_FUNDAMENTALISM] - (short)((int)(result + (result >> 0x1f & 3U)) >> 2);
  }
  if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (civs[civ].government == GOV_FUNDAMENTALISM))
  {
    city_calc.science_total += city_calc.lux_total;
    city_calc.lux_total = 0;
  }
  result = city_count_specialists(city,1);
  city_calc.lux_total += result * 2;
  result = city_count_specialists(city,2);
  city_calc.tax_total += result * 3;
  result = city_count_specialists(city,3);
  city_calc.science_total += result * 3;
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.science_total -= (int)((uint)cosmic.fund_science_loss * city_calc.science_total) / 100;
  }
  city_calc.building_content = 0;
  result = city_has_improvement(city,IMP_TEMPLE);
  if (result != 0) {
    result = civ_has_advance(civ,ADV_MYSTICISM);
    if (result != 0) {
      city_calc.building_content++;
    }
    result = civ_has_advance(civ,ADV_CEREMONIAL_BURIAL);
    if (result != 0) {
      city_calc.building_content++;
    }
    result = civ_has_wonder(civ,WONDER_ORACLE);
    if (result != 0) {
      city_calc.building_content = city_calc.building_content << 1;
    }
  }
  result = city_has_improvement(city,IMP_COLOSSEUM);
  if (result != 0) {
    city_calc.building_content += 3;
    result = civ_has_advance(civ,ADV_ELECTRONICS);
    if (result != 0) {
      city_calc.building_content++;
    }
  }
  result = civ_has_advance(civ,ADV_MONOTHEISM);
  if ((result != 0) &&
     ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
      (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
    result = civ_has_advance(civ,ADV_COMMUNISM);
    result2 = civ_has_advance(civ,ADV_THEOLOGY);
    city_calc.building_content = city_calc.building_content + (uint)(result == 0) + (3 - (uint)(result2 == 0));
  }
  result = civ_has_wonder(civ,WONDER_BACH);
  if (result != 0) {
    city_calc.building_content += 2;
  }
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.tax_total += city_calc.building_content;
  }
  result = city_has_improvement(city,IMP_MARKETPLACE);
  bonus_level = (uint)(result != 0);
  result = city_has_improvement(city,IMP_BANK);
  if (result != 0) {
    bonus_level++;
  }
  result = city_has_improvement(city,IMP_STOCK_EXCHANGE);
  if (result != 0) {
    bonus_level++;
  }
  city_calc.lux_total += ((int)(city_calc.lux_total * bonus_level) >> 1);
  city_calc.tax_total += ((int)(city_calc.tax_total * bonus_level) >> 1);
  if (city_turn.processing != 0) {
    civs[civ].government_score[GOV_FUNDAMENTALISM] =
         civs[civ].government_score[GOV_FUNDAMENTALISM] + (short)((int)((bonus_level + 2) * city_calc.building_content) >> 1);
  }
  result = city_has_improvement(city,IMP_LIBRARY);
  bonus_level = (uint)(result != 0);
  result = city_has_improvement(city,IMP_UNIVERSITY);
  if (result != 0) {
    bonus_level++;
  }
  result = city_has_improvement(city,IMP_RESEARCH_LAB);
  if ((result != 0) || (result = civ_has_wonder(civ,WONDER_SETI), result != 0)) {
    bonus_level++;
  }
  science_bonus = city_calc.science_total * bonus_level;
  result = wonder_get_city(WONDER_NEWTON);
  if (result != city) {
    science_bonus = science_bonus >> 1;
  }
  city_calc.science_total += science_bonus;
  result = wonder_get_city(WONDER_COPERNICUS);
  if (result == city) {
    city_calc.science_total = city_calc.science_total << 1;
  }
  if (cities[city].building == -IMP_CAPITALIZATION) {
    if (mode == 0) {
      if (city_calc.shield_support < city_calc.total_shields) {
        city_calc.tax_total += (city_calc.total_shields - city_calc.shield_support);
      }
    }
    else {
      if (mode == 1) {
        city_split_trade(city,orig_trade,2,cities[city].tax - city_calc.tax_total);
        return;
      }
      if (mode == 2) {
        city_calc.tax_total += extra_tax;
      }
    }
  }
  cities[city].science = (i16)city_calc.science_total;
  cities[city].tax = (i16)city_calc.tax_total;
  cities[city].trade = (i16)trade;
  return;
}

/* 004ea8e4 auto */
int __cdecl city_calc_trade_happiness(uptr city)

{
  byte civ2;
  char goods;
  iptr civ;
  iptr city2;
  int net_trade;
  int result;
  int result2;
  uptr wonder_city;
  iptr unit;
  int content_limit;
  uint factor;
  int route_trade;
  int road_bonus;
  int unhappy_mult;
  iptr route;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  city_calc.trade_corruption = city_calc_corruption(city,city_calc.total_trade,0,0);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.trade_corruption = 0;
  }
  if (civs[civ].government == GOV_DEMOCRACY) {
    city_calc.trade_corruption = 0;
  }
  cities[city].base_trade = (short)city_calc.total_trade - (short)city_calc.trade_corruption;
  city_calc_commodity_demand(city);
  for (route = 0; route < (char)cities[city].trade_routes; route++) {
    city2 = (iptr)cities[city].route_partner[route];
    route_trade = (int)cities[city].base_trade + (int)cities[city2].base_trade + 4 >> 3;
    goods = *(char *)&cities[city].route_goods[route];
    road_bonus = map_road_connection(cities[city].owner,(int)cities[city].x,
                                  (int)cities[city].y,(int)cities[city2].x,(int)cities[city2].y);
    net_trade = city_has_improvement(city,IMP_AIRPORT);
    if (((net_trade != 0) && (net_trade = city_has_improvement(city2,IMP_AIRPORT), net_trade != 0)) && (road_bonus < 2)) {
      road_bonus = 1;
    }
    net_trade = city_has_improvement(city,IMP_SUPERHIGHWAYS);
    if (net_trade != 0) {
      road_bonus++;
    }
    if (road_bonus != 0) {
      route_trade += (road_bonus * route_trade >> 1);
    }
    if (goods < '\0') {
      route_trade = 0;
    }
    if (cities[city2].owner == civ) {
      route_trade = route_trade >> 1;
    }
    city_calc.route_trade_values[route] = route_trade;
    city_calc.total_trade += route_trade;
  }
  city_calc.trade_corruption = city_calc_corruption(city,city_calc.total_trade,0,1);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.trade_corruption = 0;
  }
  if (civs[civ].government == GOV_DEMOCRACY) {
    city_calc.trade_corruption = 0;
  }
  net_trade = city_calc.total_trade - city_calc.trade_corruption;
  city_split_trade(city,net_trade,0,0);
  (*(i8 *)&city_calc.happy_count) = 0;
  if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
    (*(i8 *)&city_calc.unhappy_count) = (cities[city].size - 1) - (cosmic.unhappy_size - 5);
  }
  else {
    factor = (uint)cosmic.riot_factor + (uint)game.difficulty * -2;
    if ((game.game_flags & GAME_LARGE_MAP) != 0) {
      factor += 2;
    }
    result = (int)((((int)(uint)civs[civ].government >> 1) + 2) * factor) / 2;
    if (result < 2) {
      result = 1;
    }
    content_limit = (uint)cosmic.unhappy_size - (uint)game.difficulty;
    if (((content_limit < 3) && (result2 = city_has_improvement(city,IMP_PALACE), result2 != 0)) &&
       ((city_calc.aggressive_units == 0 && (city_calc.home_attack_units == 0)))) {
      content_limit = 2;
    }
    (*(i8 *)&city_calc.unhappy_count) = (cities[city].size - 1) - (content_limit - 2);
    if (civs[civ].government != GOV_COMMUNISM) {
      (*(i8 *)&city_calc.unhappy_count) = (*(i8 *)&city_calc.unhappy_count) +
                     ((civs[civ].num_cities - result) + (int)city % result) / result;
    }
  }
  city_calc.angry_count = 0;
  if (cities[city].size < (*(i8 *)&city_calc.unhappy_count)) {
    city_calc.angry_count = (*(i8 *)&city_calc.unhappy_count) - cities[city].size;
    (*(i8 *)&city_calc.unhappy_count) = (int)cities[city].size;
  }
  city_happy_clamp_stage(city,0);
  (*(i8 *)&city_calc.happy_count) = city_calc.lux_total >> 1;
  city_happy_clamp_stage(city,1);
  result = city_has_improvement(city,IMP_COLOSSEUM);
  if (result != 0) {
    (*(i8 *)&city_calc.unhappy_count) -= 3;
    result = civ_has_advance(civ,ADV_ELECTRONICS);
    if (result != 0) {
      (*(i8 *)&city_calc.unhappy_count)--;
    }
  }
  result = civ_has_advance(civ,ADV_MONOTHEISM);
  if ((result != 0) &&
     ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
      (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
    result = civ_has_advance(civ,ADV_COMMUNISM);
    result2 = civ_has_advance(civ,ADV_THEOLOGY);
    (*(i8 *)&city_calc.unhappy_count) -= ((uint)(result == 0) + (3 - (uint)(result2 == 0)));
  }
  result = city_has_improvement(city,IMP_TEMPLE);
  if (result != 0) {
    result = civ_has_advance(civ,ADV_MYSTICISM);
    factor = (uint)(result != 0);
    result = civ_has_advance(civ,ADV_CEREMONIAL_BURIAL);
    if (result != 0) {
      factor++;
    }
    result = civ_has_wonder(civ,WONDER_ORACLE);
    if (result != 0) {
      factor = factor << 1;
    }
    (*(i8 *)&city_calc.unhappy_count) -= factor;
  }
  result = city_has_improvement(city,IMP_COURTHOUSE);
  if (((result != 0) || (result = city_has_improvement(city,IMP_PALACE), result != 0)) &&
     (civs[civ].government == GOV_DEMOCRACY)) {
    (*(i8 *)&city_calc.happy_count)++;
  }
  city_happy_clamp_stage(city,2);
  if (civs[civ].government == GOV_FUNDAMENTALISM) {
    city_calc.angry_count = 0;
    (*(i8 *)&city_calc.unhappy_count) = 0;
  }
  else if (civs[civ].government < GOV_REPUBLIC) {
    city_calc.martial_law_units = 0;
    for (unit = tile_first_unit((int)cities[city].x,(int)cities[city].y);
        result = city_calc.martial_law_units, -1 < unit; unit = unit_next_in_stack(unit)) {
      if ((unit_types[units[unit].type].attack != '\0') &&
         (city_calc.martial_law_units = city_calc.martial_law_units + 1, civs[civ].government == GOV_COMMUNISM)) {
        city_calc.martial_law_units = result + 2;
      }
    }
    factor = 3;
    if (civs[civ].government == GOV_COMMUNISM) {
      factor = 6;
    }
    if ((int)factor <= (int)city_calc.martial_law_units) {
      city_calc.martial_law_units = factor;
    }
    result = int_clamp(city_calc.martial_law_units,0,(*(i8 *)&city_calc.unhappy_count));
    (*(i8 *)&city_calc.unhappy_count) -= result;
  }
  else {
    result = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((result == 0) && (result = city_has_improvement(city,IMP_POLICE_STATION), result == 0)) {
      unhappy_mult = 1;
    }
    else {
      unhappy_mult = 0;
    }
    if (civs[civ].government == GOV_DEMOCRACY) {
      unhappy_mult++;
    }
    if (unhappy_mult != 0) {
      factor = city_calc.aggressive_units;
      if ((city_calc.aggressive_units != 0) && (civs[civ].government == GOV_REPUBLIC)) {
        factor = city_calc.aggressive_units - 1;
      }
      (*(i8 *)&city_calc.unhappy_count) += unhappy_mult * factor;
    }
  }
  city_happy_clamp_stage(city,3);
  result = civ_has_wonder(civ,WONDER_HANGING_GARDENS);
  if (result != 0) {
    (*(i8 *)&city_calc.happy_count)++;
    wonder_city = wonder_get_city(WONDER_HANGING_GARDENS);
    if (wonder_city == city) {
      (*(i8 *)&city_calc.happy_count) += 2;
    }
  }
  result = civ_has_wonder(civ,WONDER_CURE_FOR_CANCER);
  if (result != 0) {
    (*(i8 *)&city_calc.happy_count)++;
  }
  wonder_city = wonder_get_city(WONDER_SHAKESPEARE);
  if (wonder_city == city) {
    (*(i8 *)&city_calc.unhappy_count) = 0;
  }
  result = civ_has_wonder(civ,WONDER_BACH);
  if (result != 0) {
    (*(i8 *)&city_calc.unhappy_count) -= 2;
  }
  city_happy_clamp_stage(city,4);
  cities[city].science = (i16)city_calc.science_total;
  cities[city].tax = (i16)city_calc.tax_total;
  cities[city].trade = (i16)net_trade;
  cities[city].total_food = (u8)(*(i8 *)&city_calc.yield_totals);
  cities[city].total_shields = (u8)(*(i8 *)&city_calc.total_shields);
  cities[city].happy = (u8)(*(i8 *)&city_calc.happy_count);
  cities[city].unhappy = (u8)(*(i8 *)&city_calc.unhappy_count);
  return (*(i8 *)&city_calc.happy_count) - (*(i8 *)&city_calc.unhappy_count);
}

/* 004eb327 auto */
void __cdecl city_count_food_routes(int city)

{
  iptr city2;
  iptr route;
  
  city_calc.food_routes_out = 0;
  city_calc.food_routes_in = 0;
  city_calc.food_route_partner = -1;
  for (route = 0; route < (char)cities[city].trade_routes; route++) {
    if (*(char *)&cities[city].route_goods[route] < '\0') {
      city_calc.food_routes_out++;
      city_calc.food_route_partner = (int)cities[city].route_partner[route];
    }
  }
  city_calc.yield_totals[0] -= city_calc.food_routes_out;
  for (city2 = 0; city2 < game.num_cities; city2++) {
    if (cities[city2].id != 0) {
      for (route = 0; route < (char)cities[city2].trade_routes; route++) {
        if ((cities[city2].route_partner[route] == city) &&
           (*(char *)&cities[city2].route_goods[route] < '\0')) {
          city_calc.yield_totals[0]++;
          city_calc.food_routes_in++;
        }
      }
    }
  }
  return;
}

/* 004eb4a1 auto */
void __cdecl city_calc_all_stats(int city)

{
  city_assign_workers(city);
  city_count_food_routes(city);
  city_fill_specialists(city);
  city_calc_shields_pollution(city);
  city_calc_trade_happiness(city);
  return;
}

/* 004eb4ed: written by hand in src/game/retvals.cpp */

/* 004eb571 auto */
int __cdecl city_notify_dialog_ex(const char *msg_key,int city,int force,Sprite *sprite)

{
  int result;
  iptr label;
  iptr option_id;
  i32 option_flags;
  Dialog dialog2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (autoplay.active == 0) {
    if ((city_calc.notify_enabled == 0) && (force == 0)) {
      result = 0;
    }
    else {
      text_set_string_arg_b(0,(const char *)&cities[city].name);
      text_clear();
      text_append(msg_key);
      result = _strcmp((char *)msg_key,str_built);
      if ((result == 0) && ((cities[city].flags & CITYF_AUTOBUILD) != 0)) {
        text_append((const char *)&str_3);
      }
      dialog2.load_game_text((const char *)&textbuf.buf);
      mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,cities[city].owner);
      if (city_calc.zoom_requested == 0) {
        option_flags = 0;
        option_id = 1;
        label = (iptr)label_string(label_table.string_ids[0x20]);
        dialog2.add_option((const char *)label,option_id,option_flags);
        option_flags = 0;
        option_id = 0;
        label = (iptr)label_string(label_table.string_ids[0x21]);
        dialog2.add_option((const char *)label,option_id,option_flags);
        if ((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0) {
          dialog2.set_default_choice(0);
        }
        else {
          dialog2.set_default_choice(1);
        }
      }
      if (sprite != 0) {
        dialog2.add_sprite(sprite,0,0);
        dialog2.set_image_zoom(8);
      }
      result = dialog2.show_modal(0);
      if (0 < result) {
        city_calc.zoom_requested = 1;
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004ebbde auto */
i32 __cdecl city_process_food(uptr city)

{
  char *goods_ptr;
  byte civ2;
  iptr civ;
  iptr box_limit;
  iptr improvement;
  iptr has_granary;
  int route_count;
  iptr unit2;
  iptr unit;
  int y2;
  int x2;
  /* "FAMINE0": the digit (msg_key[6]) picks the message; one array, as the
     original's two locals that the copy fills together */
  i8 msg_key[8];
  int route;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  city_calc_food_box_rows(city,civ);
  if (cities[city].food_box < 0) {
    unit2 = -1;
    for (unit = 0; unit < game.num_units; unit++) {
      if (((units[unit].id != 0) && (unit_types[units[unit].type].role == ROLE_SETTLE)) &&
         ((byte)units[unit].home_city == city)) {
        unit2 = unit;
        break;
      }
    }
    crt_strcpy(msg_key,str_famine0);
    /* B30: the famine's square, redrawn at the end; the original sets it only
       when the city shrinks (every mode: ours would redraw another stale square;
       docs/BUGS.md) */
    x2 = (int)cities[city].x;
    y2 = (int)cities[city].y;
    if (unit2 < 0) {
      msg_key[6] = 0x31;
    }
    else {
      text_set_label_arg(1,unit_types[units[unit2].type].name);
    }
    if (-1 < city_calc.food_route_partner) {
      msg_key[6] = 0x32;
      text_set_string_arg(1,(const char *)&cities[city_calc.food_route_partner].name);
    }
    if (autoplay.active == 0) {
      city_notify_dialog_ex(msg_key,city,0,0);
    }
    if (city_calc.food_routes_out < 1) {
      if (unit2 < 0) {
        cities[city].size--;
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        if (cities[city].size < 1) {
          city_delete(city,0);
          civ_check_destroyed(civ,0);
          return 1;
        }
      }
      else if (((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) ||
              ((game.human_players & cities[city].known_to) != 0)) {
        unit_delete_with_cargo(unit2,1);
      }
    }
    else {
      route_count = (int)(char)cities[city].trade_routes;
      while (route = route_count - 1, -1 < route) {
        goods_ptr = (char *)&cities[city].route_goods[route];
        route_count = route;
        if ((*goods_ptr < '\0') &&
           (cities[city].route_partner[route] == city_calc.food_route_partner)) {
          city_remove_trade_route(city,route);
          route_count = route;
        }
      }
    }
    cities[city].food_box = 0;
    map_redraw_area_all_views(x2,y2,0,human_civ[0],1);
    if (city_turn.window_shown != 0) {
      city_turn.window_dirty = 1;
    }
  }
  else {
    box_limit = (cities[city].size + 1) * city_calc.food_box_rows;
    if (bugfix_state() && (0x7e < cities[city].size)) {
      /* B16: a city stops at 127, the most its size (a signed byte) holds;
         the original grows on to -128 (docs/BUGS.md) */
      if (box_limit < cities[city].food_box) {
        cities[city].food_box = (short)box_limit;
      }
    }
    else if (box_limit - cities[city].food_box == 0 || box_limit < cities[city].food_box) {
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_food_box(1);
      }
      improvement = city_needed_size_improvement(city);
      cities[city].size++;
      has_granary = city_has_improvement(city,IMP_GRANARY);
      if ((has_granary == 0) && (route_count = civ_has_wonder(civ,WONDER_PYRAMIDS), route_count == 0)) {
        cities[city].food_box = 0;
      }
      else {
        cities[city].food_box =
             (cities[city].size + 1) * (short)((int)(uint)cosmic.food_box_rows >> 1);
      }
      if (improvement != IMP_NOTHING) {
        cities[city].size--;
        text_set_label_arg(1,improvements[improvement].name);
        if (((((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) ||
             (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) == 0)) || ((game.shown_event_flags & HINT_AQUEDUCT) != 0)) || (improvement != IMP_AQUEDUCT)) {
          if (((int)cities[city].building != -improvement) && ((game.message_options & MSGOFF_GROWTH_HALTED) == 0)) {
            city_notify_dialog(str_furthergrowth,city,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)));
          }
        }
        else {
          text_set_number_arg_u8(0,cosmic.aqueduct_size);
          city_notify_dialog(str_aqueduct,city,(((Sprite *)((u8 *)&caption_close_button_sprite_block.notify_sprites + 420))));
          game.shown_event_flags = game.shown_event_flags | HINT_AQUEDUCT;
        }
      }
      if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) &&
         (cities[city].size == 2)) {
        city_set_production(city,99);
      }
      if (city_turn.window_shown != 0) {
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_food_box(1);
        ((CityWindow *)((i8 *)&city_wnd.window))->draw_citizens(1,0);
        city_turn.window_dirty = 1;
      }
    }
    map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
  }
  return 0;
}

/* 004eef23 auto */
void __cdecl city_disband_unsupported_units(uptr city)

{
  byte civ2;
  byte government;
  bool within_support;
  bool near_city;
  iptr civ;
  int dist;
  i32 tile_improv;
  iptr city3;
  iptr cur_unit;
  iptr unit;
  iptr city2;
  int max_dist;
  iptr unit2;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  government = civs[civ].government;
  while( true ) {
    city_calc.support_unit_count = 0;
    city_calc.shield_support = 0;
    unit = 0;
    do {
      cur_unit = unit;
      if (game.num_units <= unit) {
        return;
      }
      if (((((units[unit].id != 0) && (units[unit].owner == civ)) &&
           ((byte)units[unit].home_city == city)) &&
          ((char)unit_types[units[unit].type].role < 6)) &&
         ((city_count_unit_support(city,unit,(uint)government), city_calc.total_shields < city_calc.shield_support ||
          ((((cities[city].flags & CITYF_DISORDER) != 0 && ((game.turn + unit & 7U) == 0)) &&
           (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0 && (GOV_FUNDAMENTALISM < government)))))))) {
        within_support = city_calc.shield_support <= city_calc.total_shields;
        max_dist = -1;
        unit2 = -1;
        for (unit = 0; unit < game.num_units; unit++) {
          if (((units[unit].id != 0) && ((byte)units[unit].home_city == city)) &&
             ((char)unit_types[units[unit].type].role < 6)) {
            if (within_support) {
              if ((4 < (char)unit_types[units[unit].type].role) ||
                 (dist = tile_city_owner(units[unit].x,units[unit].y), -1 < dist))
              continue;
              tile_improv = tile_improvements(units[unit].x,units[unit].y);
              if (((byte)tile_improv & TILE_AIRBASE) == 0x40) {
                near_city = false;
                for (city2 = 0; city2 < game.num_cities; city2++) {
                  if (((cities[city2].id != 0) && (cities[city2].owner == cities[city].owner)
                      ) && (dist = map_distance((int)units[unit].x,(int)units[unit].y,
                                                       (int)cities[city2].x,(int)cities[city2].y
                                                      ), dist < 4)) {
                    near_city = true;
                    break;
                  }
                }
                if (near_city) continue;
              }
            }
            dist = map_distance_diag((int)units[unit].x,(int)units[unit].y,
                                       (int)cities[city].x,(int)cities[city].y);
            if (max_dist < dist) {
              unit2 = unit;
              max_dist = dist;
            }
          }
        }
        if (city_calc.total_shields < city_calc.shield_support) {
          if (-1 < max_dist) {
            if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
              text_set_label_arg(1,unit_types[units[unit2].type].name);
              city_notify_dialog(str_support,city,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit2].type * 0x3c)));
            }
            unit_destroy_redraw(unit2,1);
            if (city_turn.window_shown != 0) {
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_supported_units(1);
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
            }
            break;
          }
        }
        else if (((game.game_flags & GAME_AI_GANG_UP) == 0) ||
                (civs[game.best_human_rank].num_advances <= civs[civ].num_advances)) {
          if ((0 < max_dist) && (unit_types[units[unit2].type].domain == DOMAIN_LAND)) {
            city3 = city_at((int)units[unit2].x,units[unit2].y);
            if (-1 < city3) {
              cities[city3].shield_box =
                   (short)(((char)unit_types[units[unit2].type].cost * city_calc.shield_rows) / 2) +
                   cities[city3].shield_box;
            }
            unit_delete(unit2,1);
            cities[city].flags = cities[city].flags & ~(CITYF_DISORDER | CITYF_DISORDER_2ND_TURN | CITYF_DISORDER_LONG);
            break;
          }
        }
        else {
          civs[civ].government_score[GOV_DEMOCRACY] = -999;
          civs[civ].government_score[GOV_REPUBLIC] = -999;
        }
      }
      unit = cur_unit + 1;
    } while( true );
  }
}

/* 004ef578 auto */
void __cdecl city_process_disorder(int city)

{
  byte civ2;
  iptr civ;
  int happy_margin;
  iptr title;
  
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  if (civ != 0) {
    happy_margin = city_calc.happy_count - city_calc.unhappy_count;
    if (happy_margin < 0) {
      if (((human_civ[0] == civ) && (autoplay.active == 0)) && ((game.message_options & MSGOFF_DISORDER) == 0))
      {
        snd_play_effect(SND_CIVDISOR,1,0,0);
        popup_city_picture_zoom((const char *)&str_disorder,city,0x48,0,0);
      }
      if ((cities[city].flags & CITYF_DISORDER) == 0) {
        cities[city].flags = cities[city].flags | (CITYF_DISORDER | CITYF_ON_EDGE);
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
      }
      else {
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        if ((cities[city].flags & CITYF_DISORDER_2ND_TURN) != 0) {
          cities[city].flags = cities[city].flags | CITYF_DISORDER_LONG;
        }
        cities[city].flags = cities[city].flags | CITYF_DISORDER_2ND_TURN;
        if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) &&
            (civs[civ].government == GOV_DEMOCRACY)) &&
           (((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 0x10) == 0)))) {
          city_notify_dialog(str_revolt,city,(Sprite *)&government_sprites_block.government_sprite_democracy);
          civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
          civ_change_government(civ,0);
          civs[civ].flags = civs[civ].flags | CIVF_REVOLUTION;
        }
      }
    }
    else {
      if (((city_calc.happy_count == city_calc.unhappy_count) && (0 < city_calc.total_shields - city_calc.shield_support)) &&
         (-1 < city_calc.food_surplus)) {
        cities[city].flags = cities[city].flags | CITYF_ON_EDGE;
      }
      else {
        cities[city].flags = cities[city].flags & ~CITYF_ON_EDGE;
      }
      if ((-1 < happy_margin) && ((cities[city].flags & CITYF_DISORDER) != 0)) {
        cities[city].flags = cities[city].flags & ~(CITYF_DISORDER | CITYF_DISORDER_2ND_TURN | CITYF_DISORDER_LONG);
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) {
          city_set_production(city,99);
        }
        else if ((game.message_options & MSGOFF_ORDER_RESTORED) == 0) {
          city_notify_dialog_ex(str_restored,city,0,0);
        }
        map_redraw_area_all_views((int)cities[city].x,(int)cities[city].y,0,human_civ[0],1);
      }
      if (civs[civ].government != GOV_ANARCHY) {
        civs[civ].gold += city_calc.tax_total;
      }
    }
    if ((((city_calc.unhappy_count == 0) && (2 < cities[city].size)) &&
        (cities[city].size + 1 >> 1 <= city_calc.happy_count)) && (civs[civ].government != GOV_ANARCHY)) {
      if ((cities[city].flags & CITYF_CELEBRATING) == 0) {
        if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
          title = civ_leader_title(civ);
          text_set_string_arg_b(1,(const char *)title);
          if ((game.message_options & MSGOFF_CELEBRATION) == 0) {
            if (city_calc.notify_enabled != 0) {
              music_play_track(3,0);
            }
            popup_city_picture_zoom((const char *)&str_weloveking,city,0x4a,0,0);
          }
        }
        cities[city].flags = cities[city].flags | CITYF_CELEBRATING;
      }
      else if (((GOV_FUNDAMENTALISM < civs[civ].government) &&
               ((int)((int)cities[city].size * (uint)cosmic.food_per_citizen +
                     city_calc.settlers_supported * city_calc.settler_food_cost) < city_calc.yield_totals[0])) &&
              (happy_margin = city_needed_size_improvement(city), happy_margin == 0) &&
              (!bugfix_state() || (cities[city].size < 0x7f))) {  /* B16 */
        cities[city].size++;
      }
    }
    else if ((cities[city].flags & CITYF_CELEBRATING) != 0) {
      if ((((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (civs[civ].government != GOV_ANARCHY)) && ((cities[city].flags & CITYF_DISORDER) == 0)) {
        title = civ_leader_title(civ);
        text_set_string_arg_b(1,(const char *)title);
        if ((game.message_options & MSGOFF_CELEBRATION) == 0) {
          city_notify_dialog_ex(str_wedontloveking,city,0,0);
        }
      }
      cities[city].flags = cities[city].flags & ~CITYF_CELEBRATING;
    }
  }
  return;
}

/* 004efbc6 auto */
void __cdecl city_add_science(int city)

{
  byte civ2;
  uptr civ;
  int result;
  iptr improvement;
  
  civ2 = cities[city].owner;
  civ = (uptr)(char)civ2;
  if ((((-1 < civs[civ].researching) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0))
      && (game.difficulty == DIFF_CHIEFTAIN)) &&
     ((civs[civ].researching != 0x59 && (game.first_discoverer[civs[civ].researching] != '\0')))
     ) {
    city_calc.science_total = city_calc.science_total << 1;
  }
  if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) && (DIFF_WARLORD < game.difficulty)) {
    result = ai_spaceship_race_behind(civ);
    if (result != 0) {
      for (improvement = 0x23; improvement < 0x26; improvement++) {
        result = civ_has_advance(civ,improvements[improvement].preq);
        if (result == 0) {
          city_calc.science_total = city_calc.science_total << 1;
          break;
        }
      }
    }
  }
  if (civs[civ].government != GOV_ANARCHY) {
    civ_add_research_points(civ,city_calc.science_total);
  }
  return;
}

/* 004efd44: written by hand in src/game/retvals.cpp */

/* 004f0221 auto */
void __cdecl city_pay_improvement_upkeep(int city)

{
  iptr civ;
  int upkeep;
  iptr improvement;
  
  civ = (iptr)cities[city].owner;
  if (civs[civ].government != GOV_ANARCHY) {
    for (improvement = 1; improvement < 0x27; improvement++) {
      upkeep = city_has_improvement(city,improvement);
      if (upkeep != 0) {
        upkeep = civ_improvement_upkeep(civ,improvement);
        civs[civ].gold -= upkeep;
        if (civs[civ].gold < 0) {
          civs[civ].gold = 0;
          city_set_improvement(city,improvement,0);
          if (autoplay.active == 0) {
            text_set_label_arg(1,improvements[improvement].name);
            text_set_number_arg(0,(uint)improvements[improvement].cost * city_calc.shield_rows);
            city_notify_dialog(str_inhock,city,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)));
          }
          civs[civ].gold += (uint)improvements[improvement].cost * city_calc.shield_rows;
        }
      }
    }
  }
  return;
}

/* 004f080d auto */
void __cdecl city_add_terraform_goals(int city)

{
  byte civ2;
  bool added_goal;
  int civ;
  iptr x;
  int y;
  int in_bounds;
  uint improvements2;
  iptr tile_idx;
  
  added_goal = false;
  civ2 = cities[city].owner;
  civ = (int)(char)civ2;
  for (tile_idx = 0; tile_idx < 0x14; tile_idx++) {
    x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + (int)cities[city].x);
    y = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx];
    in_bounds = map_in_bounds(x,y);
    if ((in_bounds != 0) && (improvements2 = tile_improvements(x,y), (improvements2 & TILE_POLLUTION) != 0)) {
      civ_add_ai_goal(civ,x,y,0x15,6);
      cities[city].flags = cities[city].flags | CITYF_AI_TERRAFORM_GOAL;
      added_goal = true;
    }
  }
  if (!added_goal) {
    y = civ_has_advance(civ,ADV_RAILROAD);
    if ((y != 0) &&
       (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0 || (city_turn.tile_job_found == 0)))) {
      if (city_calc.tile_job_priority < 4) {
        city_calc.tile_job_priority = 3;
      }
      ai_find_road_link_tile(city);
      if (city_turn.tile_job_found == 0) {
        return;
      }
    }
    city_calc.tile_job_priority += 2;
    if (city_turn.tile_job_found == 0) {
      city_calc.tile_job_priority = 2;
      ai_find_road_link_tile(city);
    }
    if (city_turn.tile_job_found == 0) {
      cities[city].flags = cities[city].flags & ~CITYF_AI_TERRAFORM_GOAL;
    }
    else {
      if (2 < city_calc.tile_job_priority) {
        if (4 < cities[city].size) {
          city_calc.tile_job_priority++;
        }
        y = city_has_improvement(city,IMP_PALACE);
        if (y != 0) {
          city_calc.tile_job_priority++;
        }
      }
      if (cities[city].size < 4) {
        city_calc.tile_job_priority--;
      }
      civ_add_ai_goal(civ,city_calc.tile_job_x,city_calc.tile_job_y,0x15,city_calc.tile_job_priority);
      cities[city].flags = cities[city].flags | CITYF_AI_TERRAFORM_GOAL;
    }
  }
  return;
}

/* 004f0a9c auto */
int __cdecl city_process_turn(uptr city)

{
  byte civ2;
  char civilize_level;
  short prev_food;
  iptr civ;
  uptr window_city;
  int result;
  i32 clamped;
  int gov_idx;
  uint free_support;
  short unrest_factor;
  int has_police;
  
  city_wnd.refresh_locked = 1;
  civ2 = cities[city].owner;
  civ = (iptr)(char)civ2;
  if (((cities[city].turns_captured - 1 ^ (int)game.turn & 0x3fU) & 0x3f) == 0) {
    cities[city].founder = civ2;
  }
  if ((human_civ[0] == civ) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
    city_calc.notify_enabled = 1;
  }
  else {
    city_calc.notify_enabled = 0;
  }
  if (autoplay.active != 0) {
    city_calc.notify_enabled = 0;
  }
  city_calc.zoom_requested = 0;
  cities[city].flags = cities[city].flags & ~(CITYF_IMPROVEMENT_SOLD | CITYF_AI_SETTLER_NEAR | CITYF_INVESTIGATED);
  window_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  (*(uint *)&city_turn.window_shown) = (uint)(window_city == city);
  city_turn.window_dirty = 0;
  prev_food = cities[city].food_box;
  result = city_process_food(city);
  if (result == 0) {
    city_recalc(city,1);
    if (((*(uint *)&city_turn.window_shown) != 0) && (city_turn.window_dirty != 0)) {
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    if ((city_calc.workers_overflow != 0) && (autoplay.active == 0)) {
      city_notify_dialog_ex(str_decrease,city,0,0);
    }
    city_calc.food_surplus = (city_calc.yield_totals[0] - (int)cities[city].size * (uint)cosmic.food_per_citizen) -
                   city_calc.settlers_supported * city_calc.settler_food_cost;
    cities[city].food_box += (short)city_calc.food_surplus;
    if ((((0 < prev_food) && (cities[city].food_box < prev_food)) &&
        ((int)cities[city].food_box + city_calc.food_surplus * 3 < 0)) &&
       (((game.message_options & MSGOFF_FOOD_LOW) == 0 && (autoplay.active == 0)))) {
      city_notify_dialog_ex(str_foodshortage,city,0,0);
    }
    city_process_production(city);
    city_disband_unsupported_units(city);
    city_turn.worker_realloc = 1;
    city_turn.processing = 1;
    city_turn.tile_job_found = 0;
    city_calc.tile_job_priority = 0;
    city_recalc(city,1);
    city_turn.worker_realloc = 0;
    if (city_calc.total_shields < city_calc.shield_support) {
      civs[civ].government_score[GOV_ANARCHY] =
           civs[civ].government_score[GOV_ANARCHY] + ((short)city_calc.shield_support - (short)city_calc.total_shields) * 5;
    }
    clamped = int_clamp(city_calc.support_unit_count - (uint)cosmic.monarchy_free_support,0,
                               (int)cities[city].size);
    civs[civ].government_score[GOV_ANARCHY] = (short)clamped + civs[civ].government_score[GOV_ANARCHY];
    for (gov_idx = 1; gov_idx < 7; gov_idx++) {
      free_support = 0;
      switch(gov_idx) {
      case 1:
        free_support = (uint)cities[city].size;
        break;
      case 2:
        free_support = (uint)cosmic.monarchy_free_support;
        break;
      case 3:
        free_support = (uint)cosmic.communism_free_support;
        break;
      case 4:
        free_support = (uint)cosmic.fundamentalism_free_support;
      }
      if ((int)free_support < city_calc.support_unit_count) {
        civs[civ].government_score[gov_idx] =
             civs[civ].government_score[gov_idx] -
             ((short)city_calc.support_unit_count - (short)free_support);
      }
    }
    result = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((result == 0) && (result = city_has_improvement(city,IMP_POLICE_STATION), result == 0)) {
      has_police = 0;
    }
    else {
      has_police = 1;
    }
    civilize_level = leaders[civs[civ].tribe].civilize;
    unrest_factor = -(short)civilize_level + 7;
    result = city_has_improvement(city,IMP_MARKETPLACE);
    if (result != 0) {
      unrest_factor = -(short)civilize_level + 5;
    }
    result = city_has_improvement(city,IMP_BANK);
    if (result != 0) {
      unrest_factor--;
    }
    result = city_has_improvement(city,IMP_TEMPLE);
    if ((result != 0) &&
       ((result = city_has_improvement(city,IMP_CATHEDRAL), result != 0 ||
        (result = civ_has_wonder(civ,WONDER_MICHELANGELO), result != 0)))) {
      unrest_factor--;
    }
    window_city = wonder_get_city(WONDER_SHAKESPEARE);
    if (window_city != city) {
      result = leaders[civs[civ].tribe].attack + city_calc.aggressive_units;
      prev_food = civs[civ].government_score[GOV_DEMOCRACY];
      clamped = int_clamp((2 - has_police) * (result + 1),0,99);
      civs[civ].government_score[GOV_DEMOCRACY] = prev_food - (short)clamped * unrest_factor;
      prev_food = civs[civ].government_score[GOV_REPUBLIC];
      clamped = int_clamp(result * (1 - has_police),0,99);
      civs[civ].government_score[GOV_REPUBLIC] = prev_food - (short)clamped * unrest_factor;
    }
    city_process_disorder(city);
    city_add_science(city);
    if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
      city_process_pollution(city);
      city_pay_improvement_upkeep(city);
    }
    city_add_terraform_goals(city);
    result = city_calc.total_shields * 2 - city_calc.shield_support;
    if (civs[civ].best_shield_surplus <= result) {
      civs[civ].best_shield_surplus = (i16)result;
    }
    city_wnd.refresh_locked = 0;
    city_turn.calc_cached_id = 0xffffffff;
    if ((*(uint *)&city_turn.window_shown) != 0) {
      city_save_build_state(city);
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    if (city_calc.zoom_requested != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    }
    city_turn.processing = 0;
    result = city_calc.happy_count - city_calc.unhappy_count;
  }
  else {
    result = -999;
  }
  return result;
}

/* 004f1220 auto */
void __cdecl wonder_apollo_reveal_map(void)

{
  Tile *tile;
  i32 arg_zero;
  i32 arg_one;
  iptr known_civ;
  uint y2;
  iptr civ_idx;
  iptr city;
  uint x2;
  int tile_idx;
  
  if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
    y2 = 0;
    x2 = 0;
    net_batch_begin();
    for (tile_idx = 0; tile_idx < map.area; tile_idx++) {
      tile_set_visible(x2,y2,0xff,1,1);
      for (civ_idx = 1; civ_idx < 8; civ_idx++) {
        arg_one = 1;
        arg_zero = 0;
        known_civ = civ_idx;
        tile = tile_at(x2,y2);
        known_set(x2,y2,(u8)tile->improvements,
                  known_civ,arg_zero,arg_one);
      }
      x2 += 2;
      if ((int)map.width <= (int)x2) {
        y2++;
        x2 = y2 & 1;
      }
    }
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        cities[city].known_to = 0xff;
        for (civ_idx = 0; civ_idx < 8; civ_idx++) {
          *(i8 *)&cities[city].revealed_size[civ_idx] = cities[city].size;
        }
        tile_set_visible(cities[city].x,cities[city].y,0xff,1,1);
      }
    }
    net_batch_flush();
    if (2 < game.multiplayer) {
      net.tile_sync_enabled = 1;
      net_send_state_diff(0xff,2,0,0,0);
      net_send_message(NM_DRAW_MAP,0xff,0,0,0,0,0,0,0,0);
      if (autoplay.active == 0) {
        net_send_text_args(0x49,0xff,0,0,0,0);
      }
    }
    map_redraw_all_views(human_civ[0],1);
    if (autoplay.active == 0) {
      popup_game((const char *)&str_astronauts);
    }
  }
  else {
    net_send_message(NM_DO_APOLLO,0,0,0,0,0,0,0,0,0);
  }
  return;
}

/* 004f3d30 auto */
uint __cdecl city_tile_status_flags(int city,int tile_idx)

{
  return (uint)(byte)(((i8 *)&city_calc.tile_flags))[tile_idx];
}

/* 004f3d60 auto */
CityPicturePopup *CityPicturePopup::construct()

{
  
  PictureWindow::construct();
  dialog.Dialog::construct(0x4000);
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 3);
  return this;
}

/* 004f3e20 auto */
void *CityPicturePopup::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004f3e70: written by hand in src/game/unwind.cpp */

/* 004f3f30 auto */
void __cdecl city_notify_dialog(const char *message,int city,Sprite *extra)

{
  city_notify_dialog_ex(message,city,0,extra);
  return;
}

/* 00579dbb auto */
int __cdecl city_treasury_share(int city)

{
  char civ;
  int share;
  
  civ = cities[city].owner;
  if (civs[civ].gold < (int)(32000 / (longlong)(int)cities[city].size)) {
    share = ((int)cities[city].size * civs[civ].gold) / (civs[civ].total_city_size + 1);
  }
  else {
    share = (civs[civ].gold / (civs[civ].total_city_size + 1)) * (int)cities[city].size;
  }
  if (share < 0) {
    share = 32000;
  }
  return share;
}

/* 0057a7e9 auto */
void __cdecl city_transfer_for_civil_war(int city,int civ2,int civ)

{
  civs[civ2].num_cities--;
  civs[civ].num_cities++;
  civs[civ2].total_city_size -= (short)cities[city].size;
  civs[civ].total_city_size = (short)cities[city].size + civs[civ].total_city_size;
  cities[city].owner = (i8)civ;
  tile_set_owner(cities[city].x,cities[city].y,civ,1);
  return;
}

/* Part of city_capture_aftermath (0057b5df): the capital taken: the loser may
   buy its palace into its best other city (1000 gold), else loses its
   spaceship. */
static void city_capture_aftermath_palace_escape(uptr city,int x2,int y,iptr city4,iptr civ2)
{
  char size;
  int improvement2;
  int value;
  iptr x;
  iptr unit;
  uptr city3;
  int save_palace;
  uptr city2;
  int best_score;
  int score;

  best_score = -999;
  city3 = 0xffffffff;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if ((((cities[city2].id != 0) && (cities[city2].owner == civ2)) &&
        (city2 != city)) &&
       (((int)cities[city].size / 2 <= (int)cities[city2].size &&
        (7 < cities[city2].size)))) {
      size = cities[city2].size;
      score = map_distance_diag((int)cities[city2].x,(int)cities[city2].y,x2,y);
      score = size * 3 - score;
      unit = tile_first_unit((int)cities[city2].x,(int)cities[city2].y);
      improvement2 = unit_stack_sum(unit,2);
      score += improvement2 * 4;
      improvement2 = city_has_improvement(city2,IMP_SAM_BATTERY);
      if (improvement2 != IMP_NOTHING) {
        score = (score * 3) / 2;
      }
      improvement2 = city_has_improvement(city2,IMP_CITY_WALLS);
      if ((improvement2 != IMP_NOTHING) || (improvement2 = civ_has_wonder(civ2,WONDER_GREAT_WALL), improvement2 != IMP_NOTHING)) {
        score = score << 1;
      }
      improvement2 = city_has_improvement(city2,IMP_SDI_DEFENSE);
      if (improvement2 != IMP_NOTHING) {
        score = score * 3;
      }
      improvement2 = tile_landmass(cities[city2].x,cities[city2].y);
      value = tile_landmass(x2,y);
      if (improvement2 != value) {
        score = score / 2;
      }
      if (best_score < score) {
        best_score = score;
        city3 = city2;
      }
    }
  }
  save_palace = 0;
  if (((999 < civs[civ2].gold) && (0 < (int)city3)) && (city4 < 0)) {
    city2 = city3;
    text_set_string_arg(1,(const char *)&cities[city3].name);
    x = civ_get_adjective(civ2);
    text_set_string_arg(2,(const char *)x);
    text_set_number_arg(0,1000);
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
        (human_civ[0] != civ2)) || (autoplay.active != 0)) {
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
          (game.multiplayer < 3)) || (autoplay.active != 0)) {
        if ((DIFF_PRINCE < game.difficulty) &&
           (((improvement2 = civ_spaceship_exists(civ2), improvement2 != IMP_NOTHING ||
             (0xb < civs[civ2].num_cities)) || (1999 < civs[civ2].gold)))) {
          save_palace = 1;
        }
      }
      else {
        sprite_picker_sprite_block.net_can_escape_reply = -1;
        net_send_text_args(0x17,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,1,human_civ[0],0);
        improvement2 = timer_ticks();
        while ((value = timer_ticks(), value - improvement2 < 0xe10 && (sprite_picker_sprite_block.net_can_escape_reply == -1))) {
          net.process_messages(1,0);
        }
        if (sprite_picker_sprite_block.net_can_escape_reply == -1) {
          save_palace = 0;
          improvement2 = civ_spaceship_exists(civ2);
          if (((improvement2 != IMP_NOTHING) || (0xb < civs[civ2].num_cities)) || (1999 < civs[civ2].gold)
             ) {
            save_palace = 1;
          }
        }
      }
    }
    else {
      save_palace = popup_game_with_sprite(str_canescape_2,(Sprite *)&popup_info_icon_sprite,0);
    }
  }
  if (save_palace == 1) {
    civs[civ2].gold -= 1000;
    city_set_improvement(city2,1,1);
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) ||
        (game.multiplayer < 3)) || (human_civ[0] == civ2)) {
      popup_game_with_city((const char *)&str_escape_2,city2);
    }
    else {
      net_send_text_args(0x18,net.net_slots[net.net_civ_slots[civ2]].id,3,0
                         ,city2,0);
    }
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
      city_set_production(city3,99);
    }
  }
  else {
    civ_spaceship_lost_notify(civ2);
    if (-1 < city4) {
      cities[city4].building = -IMP_PALACE;
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the wonder the city was building is
   abandoned (the dialog). */
static void city_capture_aftermath_abandon_wonder(uptr city,Dialog &dialog2,iptr civ2)
{
  int improvement2;
  iptr x;
  iptr improvement3;
  int same_build_count;
  uptr city2;

  if (cities[city].building < 1) {
    improvement3 = ~(int)cities[city].building + 1;
  }
  else {
    improvement3 = (iptr)cities[city].building;
  }
  same_build_count = 0;
  for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
    if (((cities[city2].id != 0) && (city2 != city)) &&
       ((cities[city2].owner == civ2 &&
        (cities[city2].building == cities[city].building)))) {
      same_build_count++;
    }
  }
  x = (iptr)(civ_name_plural(civ2));
  text_set_string_arg(1,(const char *)x);
  text_set_label_arg(2,improvements[improvement3].name);
  text_set_number_arg(0,same_build_count);
  if (same_build_count == 0) {
    dialog2.load_game_text((const char *)&str_abandonwonder_3);
    if (2 < game.multiplayer) {
      net_send_text_args(0x14,0xff,3,0,improvement3,8);
    }
  }
  else {
    text_clear();
    text_append((const char *)&str_stillwonder);
    x = int_clamp(same_build_count,1,2);
    text_append_int(x);
    dialog2.load_game_text((const char *)&textbuf.buf);
    improvement2 = int_clamp(same_build_count,1,2);
    if (improvement2 == IMP_PALACE) {
      if (game.multiplayer < 3) {
        net_send_text_args(0x4c,0xff,3,0,improvement3,0);
      }
      else {
        net_send_text_args(0x4b,0xff,3,0,improvement3,0);
      }
    }
  }
  dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement3 * 0x3c)),0,0);
  dialog2.set_image_zoom(8);
  dialog2.show_modal(0);
}

/* Part of city_capture_aftermath (0057b5df): the wonders of the city change hands
   (the dialogs). */
static void city_capture_aftermath_wonder_notices(uptr city,iptr civ,Dialog &dialog2,iptr civ2)
{
  int improvement2;
  uptr wonder_city;
  iptr wonder;

  for (wonder = 0; wonder < 0x1c; wonder++) {
    wonder_city = wonder_get_city(wonder);
    if (wonder_city == city) {
      improvement2 = wonder + 0x27;
      text_set_label_arg(1,improvements[improvement2].name);
      text_clear();
      if (((wonder < 7) || (wonder == WONDER_STATUE_OF_LIBERTY)) || ((wonder == WONDER_EIFFEL_TOWER || (0x16 < wonder)))) {
        text_append_label(0xf3);
        text_append_space();
      }
      text_set_string_arg(2,(const char *)&textbuf.buf);
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
          (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
        net_send_text_args(0x1a,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,0,improvement2,0);
      }
      if ((human_civ[0] == civ) && (autoplay.active == 0)) {
        dialog2.load_game_text((const char *)&str_capturewonder_2);
        dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c)),0,0);
        dialog2.set_image_zoom(8);
        dialog2.show_modal(0);
      }
      else if ((human_civ[0] == civ2) && (autoplay.active == 0)) {
        dialog2.load_game_text((const char *)&str_lostwonder_2);
        dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c)),0,0);
        dialog2.set_image_zoom(8);
        dialog2.show_modal(0);
      }
      if (wonder == WONDER_EIFFEL_TOWER) {
        civ_apply_eiffel_tower(civ);
      }
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the city shrinks (an AI may keep it
   whole), city = -1 when it is destroyed. */
static void city_capture_aftermath_shrink(uptr &city,iptr civ,int x2,int y,iptr civ2)
{
  int improvement2;
  uint rnd;
  uint sign;
  int civ4;
  int human_on_continent;

  human_on_continent = 0;
  if (civ == 0) {
    improvement2 = tile_landmass(x2,y);
    for (civ4 = 1; civ4 < 8; civ4++) {
      if (((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) &&
         (*(char *)&civs[civ4].continent_cities[improvement2] != '\0')) {
        human_on_continent = 1;
      }
    }
  }
  if ((cities[city].size < 2) &&
     (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 &&
        ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) ||
       ((civ == 0 && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) &&
      ((((civ != 0 || (human_on_continent != 0)) &&
        ((game.human_players & cities[city].known_to) == 0)) &&
       (game.power_rank[civ] != 7)))))) {
    if (game.power_rank[game.worst_human_rank] < game.power_rank[civ]) {
      rnd = crt_rand_at("city_capture_aftermath");
      sign = (int)rnd >> 0x1f;
      if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
        cities[city].size--;
      }
    }
  }
  else {
    cities[city].size--;
  }
  if (cities[city].size == '\0') {
    city_delete(city,0);
    city = 0xffffffff;
  }
}

/* Part of city_capture_aftermath (0057b5df): the new owner takes the city radius
   and the city. */
static void city_capture_aftermath_take_tiles(uptr city,iptr civ,int x2,int y,iptr civ2)
{
  int improvement2;
  iptr x;
  iptr y2;
  iptr improvement;

  civs[civ].last_city_turn = game.turn;
  civ_clear_standing_goals_near(civ,ROLE_SETTLE,x2,y,4);
  net_batch_begin();
  for (improvement = 0; improvement < 0x15; improvement++) {
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[improvement] + x2);
    y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[improvement] + y;
    improvement2 = map_in_bounds(x,y2);
    if ((improvement2 != IMP_NOTHING) && (tile_set_radius_civ(x,y2,civ,1), civ == 0)) {
      tile_set_visible(x,y2,1,1,1);
    }
  }
  net_batch_flush();
  cities[city].owner = (byte)civ;
  city_reveal_to_civ(city,civ2);
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_city_taken((const char*)&cities[city].name,civ,civ2);
  }
}

/* Part of city_capture_aftermath (0057b5df): the news of the capture: plunder,
   messages to the humans and the multiplayer peers. */
static void city_capture_aftermath_announce(uptr city,iptr civ,int x2,int y,iptr civ2,iptr plunder)
{
  int improvement2;
  iptr x;
  int civ3;

  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) || (autoplay.active != 0)) {
    if (autoplay.active == 0) {
      for (civ3 = 1; civ3 < 8; civ3++) {
        if (((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0) &&
           ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0)) {
          if (human_civ[0] == civ3) {
            map_redraw_tile_all_views(x2,y);
          }
          else if (2 < game.multiplayer) {
            net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id,x2,y,0,0,0,0,0,0);
          }
          if (((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) == 0) &&
             ((((byte)(civs[civ].treaties[civ3] >> 16) & (TREATY_MERCENARY >> 16)) == 0 ||
              (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) {
            if (((*(byte *)&civs[civ3].treaties[civ2] & TREATY_ALLIANCE) == 0) &&
               ((((byte)(civs[civ2].treaties[civ3] >> 16) & (TREATY_MERCENARY >> 16)) == 0 ||
                (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) {
              if (((((*(byte *)&civs[civ3].treaties[civ] & TREATY_EMBASSY) != 0) ||
                   (improvement2 = civ_has_wonder(civ3,WONDER_UNITED_NATIONS), improvement2 != IMP_NOTHING)) ||
                  (improvement2 = civ_has_wonder(civ3,WONDER_MARCO_POLO), improvement2 != IMP_NOTHING)) ||
                 ((((*(byte *)&civs[civ3].treaties[civ2] & TREATY_EMBASSY) != 0 ||
                   (improvement2 = civ_has_wonder(civ3,WONDER_UNITED_NATIONS), improvement2 != IMP_NOTHING)) ||
                  (improvement2 = civ_has_wonder(civ3,WONDER_MARCO_POLO), improvement2 != IMP_NOTHING)))) {
                x = (iptr)(civ_name_plural(civ));
                text_set_string_arg(1,(const char *)x);
                x = civ_get_adjective(civ2);
                text_set_string_arg(2,(const char *)x);
                if (human_civ[0] == civ3) {
                  popup_city_picture_zoom((const char *)&str_citycapture2_2,city,0x46,1,civ);
                }
                else if (2 < game.multiplayer) {
                  net_send_text_args(0x1e,net.net_slots[net.net_civ_slots[civ3]].id,4,1,
                                     civ,city);
                }
              }
            }
            else {
              x = (iptr)(civ_name_plural(civ));
              text_set_string_arg(1,(const char *)x);
              x = (iptr)(civ_name_plural(civ2));
              text_set_string_arg(2,(const char *)x);
              if (human_civ[0] == civ3) {
                popup_city_picture_zoom((const char *)&str_cityloseally_2,city,0x46,1,civ);
              }
              else if (2 < game.multiplayer) {
                net_send_text_args(0x1d,net.net_slots[net.net_civ_slots[civ3]].id,4,1,civ
                                   ,city);
              }
            }
          }
          else {
            x = (iptr)(civ_name_plural(civ));
            text_set_string_arg(1,(const char *)x);
            x = civ_get_adjective(civ2);
            text_set_string_arg(2,(const char *)x);
            if (human_civ[0] == civ3) {
              popup_city_picture_zoom((const char *)&str_citywinally_2,city,0x46,1,civ);
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x1c,net.net_slots[net.net_civ_slots[civ3]].id,5,1,civ,
                                 city);
            }
          }
        }
      }
    }
  }
  else {
    x = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)x);
    text_set_number_arg(0,plunder);
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      mapviews_ensure_visible(x2,y,civ);
      map_redraw_tile_all_views(x2,y);
      popup_city_picture_zoom((const char *)&str_citycapture_2,city,0x46,1,civ);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer))
       && (human_civ[0] != civ2)) {
      net_send_message(NM_CITYWIN_DELETED,net.net_slots[net.net_civ_slots[civ2]].id,
                         city,0,0,0,0,0,0,0);
      net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0
                         ,0,0,0,0,0);
      net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ2]].id,x2
                         ,y,civ,0,0,0,0,0);
      net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ2]].id,x2
                         ,y,0,0,0,0,0,0);
      net_send_text_args(0x1b,net.net_slots[net.net_civ_slots[civ2]].id,4,1,
                         civ,city);
    }
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer))
       && (human_civ[0] != civ)) {
      net_send_message(NM_CITYWIN_DELETED,net.net_slots[net.net_civ_slots[civ2]].id,
                         city,0,0,0,0,0,0,0);
      net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0
                         ,0,0,0,0,0);
      net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ]].id,x2,
                         y,civ,0,0,0,0,0);
      net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ]].id,x2,
                         y,0,0,0,0,0,0);
      net_send_text_args(0x1b,net.net_slots[net.net_civ_slots[civ]].id,4,1,
                         civ,city);
    }
  }
}

/* Part of city_capture_aftermath (0057b5df): the units the city supported:
   rehomed or disbanded. */
static void city_capture_aftermath_rehome_units(uptr city,iptr civ,int x2,int y,iptr civ2)
{
  char *build_count;
  int improvement2;
  Tile *map_tile;
  iptr x;
  iptr improvement4;
  iptr unit;
  iptr unit_type;
  iptr y2;
  iptr improvement;
  uptr city2;
  int best_score;
  int score;

  for (unit = (iptr)game.num_units - 1; -1 < unit; unit--) {
    if (((units[unit].id != 0) && (units[unit].owner == civ2)) &&
       ((byte)units[unit].home_city == city)) {
      if (units[unit].type != UNIT_PARTISANS) {
        if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
            (city2 = city_at((int)units[unit].x,units[unit].y),
            -1 < (int)city2)) && (city2 != city)) {
          if ((unit_types[units[unit].type].role == ROLE_DEFEND) &&
             (improvement2 = unit_stack_count_role(unit,ROLE_DEFEND), improvement2 == IMP_PALACE)) {
            units[unit].home_city = (i8)city2;
            continue;
          }
          cities[city2].flags = cities[city2].flags | CITYF_DEFENDER_LOST;
        }
        unit_destroy_redraw(unit,1);
        /* rescan from the last unit */
        unit = (iptr)game.num_units;
        continue;
      }
      units[unit].home_city = -1;
    }
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
     (((GOV_FUNDAMENTALISM < civs[civ].government || (GOV_FUNDAMENTALISM < civs[civ2].government)) &&
      (game.difficulty != DIFF_CHIEFTAIN)))) {
    cities[city].flags = cities[city].flags | CITYF_DISORDER;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    cities[city].building = UNIT_SETTLERS;
    if (civ == 0) {
      cities[city].building = UNIT_LEGION;
      unit = tile_first_unit(x2,y);
      if (-1 < unit) {
        cities[city].building = units[unit].type;
      }
    }
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ * 0x594);
    *build_count = *build_count + 1;
    city_set_production(city,99);
  }
  else {
    map_redraw_neighbors_all_views((int)cities[city].x,cities[city].y);
    if ((cities[city].building < '\0') ||
       (improvement2 = civ_can_build_unit(civ,city,cities[city].building), improvement2 != IMP_NOTHING)) {
      if (cities[city].building < '\0') {
        if (cities[city].building < 1) {
          improvement4 = ~(int)cities[city].building + 1;
        }
        else {
          improvement4 = (iptr)cities[city].building;
        }
        improvement2 = city_can_build_improvement(civ,city,improvement4);
      }
    }
    /* improvement2 == 0: the current build can't be built */
    if (improvement2 == IMP_NOTHING) {
      cities[city].building = UNIT_SETTLERS;
      best_score = 0;
      for (unit_type = 0x3e; -1 < unit_type; unit_type--) {
        if ((unit_types[unit_type].role == ROLE_DEFEND) &&
           (improvement2 = civ_can_build_unit(civ,city,unit_type), improvement2 != IMP_NOTHING)) {
          score = ((int)(char)unit_types[unit_type].defense << 3) /
                     (int)(char)unit_types[unit_type].cost;
          if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
            score++;
          }
          if (best_score < score) {
            best_score = score;
            cities[city].building = (i8)unit_type;
          }
          break;
        }
      }
    }
    net_batch_begin();
    for (improvement = 0; improvement < 8; improvement++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[improvement] + x2);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[improvement] + y;
      improvement2 = map_in_bounds(x,y2);
      if ((improvement2 != IMP_NOTHING) &&
         (map_tile = tile_at(x,y2),
         ((uint)map_tile->visible & 1 << ((byte)civ & 0x1f)) == 0)) {
        tile_set_visible(x,y2,1 << ((byte)civ & 0x1f),1,1);
        map_redraw_tile_all_views(x,y2);
      }
    }
    net_batch_flush();
    city_turn.processing = 1;
    ((CityWindow *)((i8 *)&city_wnd.window))->begin_notice();
    ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    ((CityWindow *)((i8 *)&city_wnd.window))->end_notice();
    city_turn.processing = 0;
  }
}

/* Part of city_capture_aftermath (0057b5df): partisans rise for the loser. */
static void city_capture_aftermath_partisans(uptr city,iptr civ,int capture_mode,int x2,int y,iptr civ2,i8 city_name[80])
{
  byte terrain;
  int improvement2;
  int value;
  iptr x;
  uint rnd;
  i32 tile_bits;
  int veteran_roll;
  int rank_diff;
  int gov_diff;
  iptr unit;
  int partisan_count;
  int partisans_made;
  iptr y3;
  iptr x3;
  int partisan_idx;
  iptr y2;
  iptr improvement;
  int best_score;
  int score;

  if ((uint)civs[civ2].government == (uint)civs[civ].government ||
      (int)((uint)civs[civ2].government - (uint)civs[civ].government) < 0) {
    gov_diff = ~((uint)civs[civ2].government - (uint)civs[civ].government) + 1;
  }
  else {
    gov_diff = (uint)civs[civ2].government - (uint)civs[civ].government;
  }
  if ((uint)game.power_rank[civ2] == (uint)game.power_rank[civ] ||
      (int)((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) < 0) {
    rank_diff = ~((uint)game.power_rank[civ2] - (uint)game.power_rank[civ]) + 1;
  }
  else {
    rank_diff = (uint)game.power_rank[civ2] - (uint)game.power_rank[civ];
  }
  improvement2 = cities[city].size + 5;
  partisan_count = (((int)(improvement2 + (improvement2 >> 0x1f & 7U)) >> 3) * (gov_diff + rank_diff + 1)) / 2;
  if (capture_mode != 0) {
    partisan_count = partisan_count / 2;
  }
  improvement2 = civ_has_advance(civ2,ADV_CONSCRIPTION);
  if (improvement2 == IMP_NOTHING) {
    partisan_count--;
  }
  improvement2 = civ_has_advance(civ2,ADV_GUERRILLA_WARFARE);
  if (improvement2 == IMP_NOTHING) {
    improvement2 = civ_has_advance(civ2,ADV_COMMUNISM);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = 0;
    }
    improvement2 = civ_has_advance(civ2,ADV_GUNPOWDER);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = 0;
    }
  }
  else {
    improvement2 = civ_has_advance(civ,ADV_GUERRILLA_WARFARE);
    if (improvement2 == IMP_NOTHING) {
      partisan_count = partisan_count << 1;
    }
    else {
      partisan_count++;
    }
  }
  improvement2 = (((civs[civ2].treaties[civ] & TREATY_VENDETTA) == 0) - 1 & 2) +
           (civs[civ].reputation - 4);
  if (improvement2 < 1) {
    improvement2 = 0;
  }
  partisans_made = 0;
  for (partisan_idx = 0; partisan_idx < partisan_count; partisan_idx++) {
    best_score = 0;
    for (improvement = 0; improvement < 0x14; improvement++) {
      x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[improvement] + x2);
      y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[improvement] + y;
      value = map_in_bounds(x,y2);
      if (((value != 0) && (value = tile_is_ocean(x,y2), value == 0)) &&
         (value = tile_city_or_unit_owner(x,y2), value < 0)) {
        terrain = tile_terrain(x,y2);
        score = (char)terrain_types[terrain].defense * 2;
        rnd = tile_improvements(x,y2);
        if ((rnd & TILE_ROAD) != 0) {
          score++;
        }
        rnd = tile_improvements(x,y2);
        if ((rnd & TILE_RAILROAD) != 0) {
          score++;
        }
        tile_bits = tile_improvements(x,y2);
        if (((byte)tile_bits & TILE_AIRBASE) == 0x40) {
          score = score << 1;
        }
        if (best_score < score) {
          best_score = score;
          y3 = y2;
          x3 = x;
        }
      }
    }
    if ((best_score != 0) &&
       (unit = unit_create(UNIT_PARTISANS,civ2,x3,y3), -1 < unit)) {
      if (partisans_made == 0) {
        if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
          snd_play_effect(SND_INFANTRY,1,0,0);
        }
        if (((2 < game.multiplayer) &&
            ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
           (human_civ[0] != civ2)) {
          net_send_message(NM_SOUND_PLAY,net.net_slots[net.net_civ_slots[civ2]].id
                             ,0x22,1,0,0,0,0,0,0);
        }
      }
      partisans_made++;
      units[unit].orders = ORDER_FORTIFIED;
      if (improvement2 != IMP_NOTHING) {
        if (improvement2 < 1) {
          veteran_roll = 0;
        }
        else {
          veteran_roll = crt_rand_at("city_capture_aftermath");
          veteran_roll = veteran_roll % (improvement2 + 1);
        }
        if (veteran_roll != 0) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
      }
      unit_set_seen_by(unit,civ);
      map_redraw_tile_all_views(x3,y3);
      if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
        ui_delay_ticks(1);
      }
    }
  }
  if (((partisans_made != 0) &&
      (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 ||
       ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) && (autoplay.active == 0))
  {
    x = civ_get_adjective(civ2);
    text_set_string_arg(0,(const char *)x);
    text_set_string_arg(1,(const char *)city_name);
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      dlg_game_txt_with_unit((const char *)&str_partisans_2,unit);
    }
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
      net_send_text_args(0x1f,net.net_slots[net.net_civ_slots[civ2]].id,2,0
                         ,unit,0);
    }
  }
}

/* 0057b5df auto */
void __cdecl city_capture_aftermath(uptr city,int civ,int capture_mode)

{
  uint *attitude_flags;
  char *build_count;
  bool is_weaker;
  byte terrain;
  int x2;
  int y;
  iptr loser_launched;
  iptr winner_launched;
  int improvement2;
  uptr wonder_city;
  iptr city4;
  Dialog dialog2;
  uint founded_count;
  iptr civ2;
  uint was_founder;
  iptr plunder;
  iptr improvement;
  i8 city_name[80];
  uptr city2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  civ2 = (iptr)cities[city].owner;
  x2 = (int)cities[city].x;
  y = (int)cities[city].y;
  if (capture_mode < 2) {
    diplo_force_war(civ,civ2);
  }
  was_founder = (uint)(cities[city].founder == civ);
  crt_strcpy(city_name,(const char *)(&cities[city].name));
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
     (((loser_launched = civ_spaceship_launched(civ2), loser_launched == 0 &&
       (winner_launched = civ_spaceship_launched(civ), winner_launched == 0)) || (game.difficulty < DIFF_PRINCE)))) {
    diplo_set_treaty_flags(civ,civ2,0x10000);
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
        (civs[civ].government != GOV_DEMOCRACY)) || (game.power_rank[civ] != 7)) {
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         (game.power_rank[civ2] < game.power_rank[civ])) {
        founded_count = 0;
        for (city2 = 0; (int)city2 < (int)game.num_cities; city2++) {
          if (((cities[city2].id != 0) && (city2 != city)) &&
             ((cities[city2].founder == civ2 && (cities[city2].owner == civ)))) {
            founded_count++;
          }
        }
        if ((1 < (int)founded_count) && ((founded_count & 1) == 0)) {
          attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
          *attitude_flags = *attitude_flags | 0x400000;
        }
      }
    }
    else {
      attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
      *attitude_flags = *attitude_flags | 0x400000;
    }
  }
  diplo_clear_treaty_flags(civ,civ2,0x800);
  if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
      (14 < cities[city].size)) && (was_founder == 0)) {
    attitude_flags = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
    *attitude_flags = *attitude_flags | 0x10;
  }
  city4 = -1;
  if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) &&
      (improvement2 = city_has_improvement(city,IMP_PALACE), improvement2 != IMP_NOTHING)) && (4 < civs[civ2].num_cities)) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      if (game.power_rank[game.worst_human_rank] < game.power_rank[civ2]) {
        is_weaker = true;
      }
      else {
        is_weaker = false;
      }
    }
    else {
      is_weaker = game.power_rank[civ] < game.power_rank[civ2];
    }
    if ((is_weaker) && (improvement2 = civ_civil_war(civ2), improvement2 == IMP_NOTHING)) {
      cities[city].owner = -1;
      city4 = civ_find_central_city(civ2);
      cities[city].owner = (byte)civ2;
    }
  }
  plunder = city_treasury_share(city);
  improvement2 = city_has_improvement(city,IMP_PALACE);
  if (improvement2 != IMP_NOTHING) {
    city_capture_aftermath_palace_escape(city,x2,y,city4,civ2);
  }
  if ((cities[city].building < -0x26) && (autoplay.active == 0)) {
    city_capture_aftermath_abandon_wonder(city,dialog2,civ2);
  }
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
      ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) && (autoplay.active == 0)) {
    city_capture_aftermath_wonder_notices(city,civ,dialog2,civ2);
  }
  plunder = int_clamp(plunder,0,civs[civ2].gold);
  civs[civ2].gold -= plunder;
  if (civ != 0) {
    civs[civ].gold += plunder;
  }
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    status_draw_info(1);
  }
  cities[city].shield_box = 0;
  cities[city].flags = cities[city].flags & 0xffffffc4;
  if (-1 < cities[city].building) {
    build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + civ2 * 0x594);
    *build_count = *build_count - 1;
  }
  if (was_founder == 0) {
    city_set_improvement(city,1,0);
    city_set_improvement(city,4,0);
    city_set_improvement(city,0xb,0);
    city_set_improvement(city,7,0);
  }
  if ((capture_mode == 0) && (was_founder == 0)) {
    improvement2 = _rand();
    terrain = (byte)(improvement2 >> 0x1f);
    for (improvement = 0; improvement < 5; improvement++) {
      cities[city].improvements[improvement] =
           cities[city].improvements[improvement] &
           (byte)(0xaa >> ((((byte)improvement2 ^ terrain) - terrain & 1 ^ terrain) - terrain & 0x1f));
    }
  }
  if (((capture_mode == 0) && (was_founder == 0)) || (1 < cities[city].size)) {
    city_capture_aftermath_shrink(city,civ,x2,y,civ2);
  }
  if (-1 < (int)city) {
    city_capture_aftermath_take_tiles(city,civ,x2,y,civ2);
  }
  tile_set_owner(x2,y,civ,1);
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    snd_play_effect(SND_BARRACKS,0,0,0);
  }
  text_set_string_arg(0,(const char *)city_name);
  text_set_label_arg(3,label_table.string_ids[175 - (uint)(was_founder == 0)]);
  text_set_label_arg(4,label_table.string_ids[0xb1 - (uint)(was_founder == 0)]);
  tile_update_known(x2,y,civ);
  tile_update_known(x2,y,civ2);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_INFO_STATUS,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  city_capture_aftermath_announce(city,civ,x2,y,civ2,plunder);
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x20) == 0)) {
    civ_steal_advance(civ,civ2);
  }
  if (-1 < (int)city) {
    city_capture_aftermath_rehome_units(city,civ,x2,y,civ2);
  }
  improvement2 = civ_check_destroyed(civ2,civ);
  if ((((improvement2 == IMP_NOTHING) && (-1 < (int)city)) && (was_founder == 0)) &&
     (((civs[civ2].government == GOV_COMMUNISM || (civs[civ2].government == GOV_DEMOCRACY)) ||
      ((game.first_discoverer[ADV_GUERRILLA_WARFARE] != '\0' ||
       (improvement2 = civ_has_advance(civ2,ADV_COMMUNISM), improvement2 != IMP_NOTHING)))))) {
    city_capture_aftermath_partisans(city,civ,capture_mode,x2,y,civ2,city_name);
  }
  if (-1 < (int)city) {
    cities[city].founder = (byte)civ2;
    cities[city].turns_captured = (u8)game.turn;
    wonder_city = wonder_get_city(WONDER_LEONARDO);
    if (wonder_city == city) {
      civ_leonardo_upgrade(civ);
    }
  }
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_INFO_STATUS,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}
