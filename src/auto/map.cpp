/* The game, the world map: terrain, generation, visibility: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004087c0 auto */
i32 __cdecl map_in_bounds(iptr x,iptr y)

{
  i32 result;
  
  if ((((y < 0) || (map.height <= y)) || (x < 0)) || (map.width <= x)) {
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 00408830 auto */
void __cdecl map_fill_tile_byte(iptr ptr,i32 value)

{
  i32 i;
  
  for (i = 0; i < map.area; i++) {
    *(i8 *)ptr = (i8)value;
    ptr += 6;
  }
  return;
}

/* 00408903 auto */
void __cdecl map_copy_tile_byte(iptr dst,iptr src)

{
  i32 i;
  
  for (i = 0; i < map.area; i++) {
    *(i8 *)dst = *(i8 *)src;
    src += 6;
    dst += 6;
  }
  return;
}

/* 0040897f auto */
void __cdecl mapgen_calc_fertility(void)

{
  byte terrain_byte;
  iptr in_bounds;
  int tmp;
  Tile *map_tile;
  byte *tile;
  iptr terrain;
  uptr nbr_terrain;
  int terrain_score[22];
  int landmass2;
  uint y3;
  uint x3;
  uptr terrain2;
  int y2;
  iptr x2;
  iptr tile_idx;
  int total_score;
  int tile_score;
  
  for (landmass2 = 0; landmass2 < 0x40; landmass2++) {
    landmass_table_block.landmass_recs[landmass2].fertile_tiles = 0;
  }
  for (terrain2 = 0; (int)terrain2 < 0x16; terrain2++) {
    terrain = (int)terrain2 % 0xb;
    terrain_score[terrain2] =
         (char)terrain_types[terrain2].food * 3 + (int)(char)terrain_types[terrain2].trade;
    if (terrain != TERRAIN_GRASSLAND) {
      terrain_score[terrain2] += (char)terrain_types[terrain2].shields * 2;
    }
    if (terrain_types[terrain].mine_to == -2) {
      if (terrain == TERRAIN_HILLS) {
        terrain_score[terrain2] += 3;
      }
      else {
        terrain_score[terrain2]++;
      }
    }
    else if (terrain_types[terrain].irrigate_to == -2) {
      terrain_score[terrain2] += 2;
    }
  }
  for (y3 = 0; (int)y3 < (int)map.height; y3++) {
    for (x3 = (uint)((y3 & 1) != 0); (int)x3 < (int)map.width;
        x3 = x3 + 2) {
      terrain_byte = tile_terrain(x3,y3);
      terrain2 = (uptr)terrain_byte;
      if ((terrain2 == TERRAIN_GRASSLAND) || (terrain2 == TERRAIN_PLAINS)) {
        total_score = 0;
        for (tile_idx = 0; tile_idx < 0x15; tile_idx++) {
          tile_score = 0;
          x2 = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_idx] + x3);
          y2 = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_idx] + y3;
          in_bounds = map_in_bounds(x2,y2);
          if (in_bounds != 0) {
            terrain_byte = tile_terrain(x2,y2);
            nbr_terrain = (uptr)terrain_byte;
            if ((nbr_terrain == 2) && (tmp = tile_grassland_has_shield(x2,y2), tmp != 0)) {
              tile_score += 2;
            }
            tmp = tile_resource(x2,y2);
            if ((tmp != 0) && (tile_idx != 0x14)) {
              nbr_terrain += 0xb;
            }
            tile_score += terrain_score[nbr_terrain];
            tile = (byte *)tile_at(x2,y2);
            if ((*tile & 0x80) != 0) {
              tile_score++;
            }
            if (((tile_idx < 8) || (tmp = tile_score, tile_idx == 0x14)) &&
               (tmp = tile_score * 2, tile_idx == 0x14)) {
              tile_score = tile_score * 4;
              tile = (byte *)tile_at(x2,y2);
              tmp = tile_score;
              if ((*tile & 0x80) != 0) {
                tmp = tile_score + tile_score / 2;
              }
            }
            tile_score = tmp;
            total_score += tile_score;
          }
        }
        if ((terrain2 != TERRAIN_PLAINS) && (tmp = tile_grassland_has_shield(x3,y3), tmp == 0)) {
          total_score -= 0x10;
        }
        tmp = int_clamp(total_score - 0x78 >> 3,1,0xf);
        tile_score = (tmp >> 1) + 8;
        landmass2 = tile_landmass(x3,y3);
        if (-1 < landmass2) {
          landmass_table_block.landmass_recs[landmass2].fertile_tiles =
               landmass_table_block.landmass_recs[landmass2].fertile_tiles + 1;
        }
        map_tile = tile_at(x3,y3);
        map_tile->owner = (char)tile_score - 0x10;
      }
      else {
        map_tile = tile_at(x3,y3);
        map_tile->owner = 0xf0;
      }
    }
  }
  return;
}

/* The parts below call rand() as crt_rand_at("map_generate"): the
   rand trace names the original function, as the original's does. */
/* Part of map_generate (00408d33): the start positions cleared, a new seed,
   the map allocated and filled with ocean, then landmasses added until the
   land target for the chosen land mass and form, and islets up to 64
   landmasses. */
static void mapgen_place_landmasses(void)
{
  uint rnd;
  int tmp;
  uptr sign;
  iptr civ;
  int landmass_count;
  uint i;
  int coin;
  int extra_masses;
  int area_units;
  int land_target;

  for (civ = 0; civ < 0x15; civ++) {
    start_positions.x[civ] = -1;
    start_positions.y[civ] = -1;
  }
  map_new_seed();
  if ((game.rules & RULE_FLAT_WORLD) != 0) {
    mapgen.climate = mapgen.climate << 1;
    mapgen.temperature = mapgen.temperature << 1;
  }
  map.flat = game.rules & RULE_FLAT_WORLD;
  map_alloc();
  mapgen_bounds.land_placed = 0;
  map_fill_tile_byte((iptr)map_data.tiles,10);
  map_fill_tile_byte((iptr)&map_data.tiles->improvements,0);
  mapgen_bounds.min_y = 0;
  mapgen_bounds.max_y = map.height - 1;
  mapgen_bounds.min_x = 3;
  mapgen_bounds.max_x = map.width - 3;
  rnd = crt_rand_at("map_generate");
  sign = (int)rnd >> 0x1f;
  coin = ((rnd ^ sign) - sign & 1 ^ sign) - sign;
  if (coin == 0) {
    mapgen_bounds.max_y = map.height - 5;
  }
  else {
    mapgen_bounds.min_y = 4;
  }
  tmp = mapgen.land_form;
  if (mapgen.land_form < 1) {
    tmp = 0;
  }
  area_units = ((int)map.height * (int)map.width >> 1) / 400;
  land_target = (area_units * ((mapgen.land_mass * 8 + 8 + tmp * 8) * 5 + 0x50) * 8) / 10;
  do {
    mapgen_add_landmass(0);
  } while (mapgen_bounds.land_placed < land_target);
  map_assign_landmass_ids();
  landmass_count = 0;
  for (i = 0; (int)i < 0x40; i++) {
    if (landmass_table_block.landmass_recs[i].tiles != 0) {
      landmass_count++;
    }
  }
  extra_masses = 0x40 - (landmass_count + 1);
  if (0 < extra_masses) {
    for (i = 0; (int)i < extra_masses; i++) {
      mapgen_add_landmass(1);
    }
  }
}

/* Part of map_generate (00408d33): land touching only diagonally (the
   corner patterns 6 and 9 of a 2x2 block) is joined by filling the block. */
static void mapgen_fill_diagonal_gaps(void)
{
  int tmp;
  uint y2;
  uint x2;
  uint new_terrain;

  for (y2 = 1; (int)y2 < map.height - 2; y2++) {
    for (x2 = y2 & 1; (int)x2 < map.width - 2; x2 += 2) {
      new_terrain = 0;
      tmp = (iptr)tile_at(x2,y2);
      if (((Tile *)tmp)->improvements != '\0') {
        new_terrain = new_terrain | 1;
      }
      tmp = (iptr)tile_at(x2 + 1,y2 + 1);
      if (((Tile *)tmp)->improvements != '\0') {
        new_terrain = new_terrain | 2;
      }
      tmp = (iptr)tile_at(x2 + 1,y2 - 1);
      if (((Tile *)tmp)->improvements != '\0') {
        new_terrain = new_terrain | 4;
      }
      tmp = (iptr)tile_at(x2 + 2,y2);
      if (((Tile *)tmp)->improvements != '\0') {
        new_terrain = new_terrain | 8;
      }
      if ((new_terrain == 6) || (new_terrain == 9)) {
        tmp = (iptr)tile_at(x2 + 1,y2 + 1);
        ((Tile *)tmp)->improvements = 1;
        tmp = (iptr)tile_at(x2 + 1,y2 - 1);
        ((Tile *)tmp)->improvements = 1;
        tmp = (iptr)tile_at(x2 + 2,y2);
        ((Tile *)tmp)->improvements = 1;
        if (((int)x2 < 2) || ((int)y2 < 2)) {
          if ((int)y2 < 2) {
            if (1 < (int)x2) {
              x2 -= 2;
            }
          }
          else {
            y2--;
            x2++;
          }
        }
        else {
          x2--;
          y2--;
        }
      }
    }
  }
}

/* Part of map_generate (00408d33): each tile's first terrain from its land
   kind (0 ocean, 1 by latitude and temperature: glacier, tundra, desert or
   plains, 2 hills, else mountains). */
static void mapgen_assign_base_terrain(void)
{
  uint rnd;
  int tmp;
  uptr sign2;
  uint sign3;
  int equator;
  uint land_type;
  uint y2;
  uint x2;
  int temp_bias;
  Tile *tile;
  int lat_band;
  uint new_terrain;
  uptr idx;
  int lat;

  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    lat_band = (int)map.height / 0xc;
  }
  else {
    lat_band = (int)((int)map.height + ((int)map.height >> 0x1f & 7U)) >> 3;
  }
  if (lat_band < 3) {
    lat_band = 2;
  }
  tile = map_data.tiles;
  y2 = 0;
  x2 = 0;
  for (idx = 0; (int)idx < (int)map.area; idx++) {
    land_type = (uint)tile->improvements;
    if (land_type == 0) {
      new_terrain = 10;
    }
    else if (land_type == 1) {
      temp_bias = mapgen.temperature * -3;
      if ((game.rules & RULE_FLAT_WORLD) != 0) {
        temp_bias = mapgen.temperature * -6;
      }
      equator = ((int)map.height >> 1) + 8;
      rnd = crt_rand_at("map_generate");
      sign2 = (int)rnd >> 0x1f;
      tmp = y2 + (((rnd ^ sign2) - sign2 & 0xf ^ sign2) - sign2) + 1;
      if (equator == tmp || equator - tmp < 0) {
        tmp = (int)map.height;
        rnd = crt_rand_at("map_generate");
        sign3 = (int)rnd >> 0x1f;
        lat = ~(((tmp >> 1) + 8) -
                    (y2 + (((rnd ^ sign3) - sign3 & 0xf ^ sign3) - sign3) + 1)) + 1;
      }
      else {
        tmp = (int)map.height;
        rnd = crt_rand_at("map_generate");
        sign3 = (int)rnd >> 0x1f;
        lat = ((tmp >> 1) + 8) -
                   (y2 + (((rnd ^ sign3) - sign3 & 0xf ^ sign3) - sign3) + 1);
      }
      lat += temp_bias;
      if (lat < 1) {
        lat = 0;
      }
      lat = lat / lat_band;
      switch(lat) {
      case 0:
        new_terrain = 0;
        break;
      case 1:
      case 2:
      case 3:
        new_terrain = 1;
        break;
      case 4:
        rnd = crt_rand_at("map_generate");
        sign3 = (int)rnd >> 0x1f;
        if (((rnd ^ sign3) - sign3 & 1 ^ sign3) == sign3) {
          new_terrain = 1;
        }
        else {
          new_terrain = 6;
        }
        break;
      case 5:
      case 6:
        new_terrain = 6;
        break;
      default:
        new_terrain = 7;
      }
    }
    else if (land_type == 2) {
      new_terrain = 4;
    }
    else {
      new_terrain = 5;
    }
    tile->terrain = (u8)new_terrain;
    tile++;
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
    ui_pump_messages_net();
  }
}

/* Part of map_generate (00408d33): rainfall: each row swept east and
   west, the rain picked up over ocean turning dry land wetter. */
static void mapgen_apply_rainfall(void)
{
  u8 terrain_val;
  int tmp;
  int lat_diff3;
  int lat_diff2;
  int lat_diff1;
  uint y2;
  uint x2;
  int rain;
  Tile *tile;
  Tile *tile_rev;
  int lat;

  tile = map_data.tiles;
  tile_rev = map_data.tiles + map.area - 1;
  for (y2 = 0; (int)y2 < (int)map.height; y2++) {
    if ((int)map.height >> 1 == y2 || (int)(((int)map.height >> 1) - y2) < 0) {
      lat = ~(((int)map.height >> 1) - y2) + 1;
    }
    else {
      lat = ((int)map.height >> 1) - y2;
    }
    if ((int)map.height >> 2 == lat || ((int)map.height >> 2) - lat < 0) {
      lat_diff1 = ~(((int)map.height >> 2) - lat) + 1;
    }
    else {
      lat_diff1 = ((int)map.height >> 2) - lat;
    }
    if (mapgen.climate * 4 + 4 + lat_diff1 < 1) {
      rain = 0;
    }
    else {
      if ((int)map.height >> 2 == lat || ((int)map.height >> 2) - lat < 0) {
        lat_diff2 = ~(((int)map.height >> 2) - lat) + 1;
      }
      else {
        lat_diff2 = ((int)map.height >> 2) - lat;
      }
      rain = crt_rand_at("map_generate");
      rain = rain % (mapgen.climate * 4 + lat_diff2 + 5);
    }
    for (x2 = (uint)((y2 & 1) != 0); (int)x2 < (int)map.width;
        x2 = x2 + 2) {
      terrain_val = tile->terrain;
      if (terrain_val == '\n') {
        if ((int)map.height >> 2 == lat || ((int)map.height >> 2) - lat < 0) {
          lat_diff3 = ~(((int)map.height >> 2) - lat) + 1;
        }
        else {
          lat_diff3 = ((int)map.height >> 2) - lat;
        }
        if (rain < mapgen.climate * 4 + 4 + lat_diff3) {
          rain++;
        }
      }
      else if (0 < rain) {
        if (mapgen.climate * -2 != -6 && -1 < mapgen.climate * -2 + 6) {
          tmp = crt_rand_at("map_generate");
          rain -= tmp % (mapgen.climate * -2 + 7);
        }
        switch(terrain_val) {
        case '\0':
          tile->terrain = 1;
          break;
        case 1:
          tile->terrain = 2;
          break;
        case 4:
          tile->terrain = 3;
          break;
        case 5:
          rain -= 3;
          break;
        case 6:
          tile->terrain = 3;
        }
      }
      tile++;
    }
    rain = 0;
    for (x2 = (int)map.width - ((y2 & 1) + 1); -1 < (int)x2;
        x2 = x2 - 2) {
      terrain_val = tile_rev->terrain;
      if (terrain_val == '\n') {
        if (rain < (lat >> 1) + mapgen.climate + 1) {
          rain++;
        }
      }
      else if (0 < rain) {
        tmp = -(mapgen.climate * 2 + 2);
        if (tmp != -6 && -1 < tmp + 6) {
          tmp = crt_rand_at("map_generate");
          rain -= tmp % (7 - (mapgen.climate * 2 + 2));
        }
        switch(terrain_val) {
        case '\0':
          tile_rev->terrain = 1;
          break;
        case 1:
          tile_rev->terrain = 2;
          break;
        case 2:
          if (lat < (map.height * 3) / 10) {
            if (lat < 10) {
              tile_rev->terrain = '\t';
            }
            else {
              tile_rev->terrain = 8;
            }
          }
          else {
            tile_rev->terrain = 3;
          }
          rain -= 2;
          break;
        case 4:
          tile_rev->terrain = 3;
          break;
        case 5:
          rain -= 3;
          break;
        case 6:
          tile_rev->terrain = 1;
          break;
        case 8:
          tile_rev->terrain = 3;
        }
      }
      tile_rev--;
    }
    ui_pump_messages_net();
  }
}

/* Part of map_generate (00408d33): the age of the world: random walks
   turning terrain along a cycle (mountains raise lakes). */
static void mapgen_age_terrain(void)
{
  uint rnd;
  int tmp;
  uint sign3;
  int rand_col;
  uint y2;
  uint x2;
  Tile *tile;
  uint i;
  int lat;

  for (i = 0; (int)i < (mapgen.age * 5 + 10) * 0xa0; i++) {
    if ((i & 1) == 0) {
      if (map.height == 1 || map.height - 1 < 0) {
        y2 = 0;
      }
      else {
        tmp = crt_rand_at("map_generate");
        y2 = tmp % (int)map.height;
      }
      if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
        rand_col = 0;
      }
      else {
        rand_col = crt_rand_at("map_generate");
        rand_col = rand_col % ((int)map.width / 2);
      }
      x2 = (y2 & 1) + rand_col * 2;
    }
    else {
      rnd = crt_rand_at("map_generate");
      sign3 = (int)rnd >> 0x1f;
      tmp = ((rnd ^ sign3) - sign3 & 7 ^ sign3) - sign3;
      x2 = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[tmp] + x2);
      y2 += (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[tmp];
    }
    if ((int)map.height >> 1 == y2 || (int)(((int)map.height >> 1) - y2) < 0) {
      lat = ~(((int)map.height >> 1) - y2) + 1;
    }
    else {
      lat = ((int)map.height >> 1) - y2;
    }
    tmp = map_in_bounds(x2,y2);
    if (tmp != 0) {
      tile = tile_at(x2,y2);
      switch(tile->terrain) {
      case '\0':
        tile->terrain = 1;
        break;
      case 1:
        tile->terrain = 4;
        break;
      case 2:
        tile->terrain = 3;
        break;
      case 3:
        if (lat < (map.height * 3) / 10) {
          tile->terrain = '\t';
        }
        else {
          tile->terrain = 1;
        }
        break;
      case 4:
        tile->terrain = 5;
        break;
      case 5:
        mapgen_try_place_lake(x2,y2);
        break;
      case 6:
        tile->terrain = 4;
        break;
      case 7:
        tile->terrain = 5;
        break;
      case 8:
        tile->terrain = 2;
        break;
      case '\t':
        tile->terrain = 8;
      }
    }
  }
}

/* Part of map_generate (00408d33): random walks giving land tiles the
   terrain most of their neighbours have, biased by climate and
   temperature. */
static void mapgen_smooth_terrain(void)
{
  byte terrain_byte;
  iptr x;
  uint rnd;
  int tmp;
  uint sign3;
  int rand_col2;
  uptr last_terrain;
  int terrain_votes[11];
  uint y2;
  uint x2;
  Tile *tile;
  uint i;
  uptr terr;
  int y3;
  uptr idx;
  uint is_hilly;
  int best_votes;

  for (i = 0; (int)i < (3 - (mapgen.age + 2)) * 800; i++) {
    if ((i & 1) == 0) {
      if (map.height == 1 || map.height - 1 < 0) {
        y2 = 0;
      }
      else {
        tmp = crt_rand_at("map_generate");
        y2 = tmp % (int)map.height;
      }
      if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
        rand_col2 = 0;
      }
      else {
        rand_col2 = crt_rand_at("map_generate");
        rand_col2 = rand_col2 % ((int)map.width / 2);
      }
      x2 = (y2 & 1) + rand_col2 * 2;
      last_terrain = 0xffffffff;
    }
    else {
      rnd = crt_rand_at("map_generate");
      sign3 = (int)rnd >> 0x1f;
      tmp = ((rnd ^ sign3) - sign3 & 7 ^ sign3) - sign3;
      x2 = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[tmp] + x2);
      y2 += (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[tmp];
    }
    tmp = map_in_bounds(x2,y2);
    if ((tmp != 0) && (tmp = tile_is_ocean(x2,y2), tmp == 0)) {
      terrain_byte = tile_terrain(x2,y2);
      terr = (uptr)terrain_byte;
      if (((int)last_terrain < 0) || (last_terrain != terr)) {
        is_hilly = (uint)(2 < terr);
        tile = tile_at(x2,y2);
        for (terr = 0; (int)terr < 0xb; terr++) {
          terrain_votes[terr] = 0;
        }
        if (mapgen.climate == -1) {
          terrain_votes[0]++;
          terrain_votes[1]++;
          terrain_votes[8]--;
        }
        else if (mapgen.climate == 1) {
          terrain_votes[2]++;
          terrain_votes[8]++;
          if (-1 < mapgen.temperature) {
            terrain_votes[9]++;
          }
        }
        if (mapgen.temperature == -1) {
          terrain_votes[6]++;
          terrain_votes[3]++;
          terrain_votes[0]--;
        }
        else if (mapgen.temperature == 1) {
          terrain_votes[1]++;
          terrain_votes[6]--;
          terrain_votes[7]--;
          if (mapgen.climate == -1) {
            terrain_votes[2]--;
            terrain_votes[9]--;
          }
        }
        for (idx = 0; (int)idx < 8; idx++) {
          x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[idx] + x2);
          y3 = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[idx] + y2;
          tmp = map_in_bounds(x,y3);
          if (tmp != 0) {
            terrain_byte = tile_terrain(x,y3);
            terr = (uptr)terrain_byte;
            if (terr != 10) {
              if (((2 < terr) && (is_hilly != 0)) && (terrain_votes[terr] != 0)) {
                terrain_votes[terr]++;
              }
              if ((idx & 1) == 0) {
                terrain_votes[terr]++;
              }
              terrain_votes[terr]++;
            }
          }
        }
        best_votes = 0;
        for (terr = 0; (int)terr < 0xb; terr++) {
          if (best_votes < terrain_votes[terr]) {
            best_votes = terrain_votes[terr];
            tile->terrain = (u8)terr;
            last_terrain = terr;
          }
        }
      }
    }
  }
}

/* Part of map_generate (00408d33): on a round world the top and bottom
   rows glacier, with random tundra on a new map. */
static void mapgen_place_polar_caps(int keep_map)
{
  i8 *tile_byte;
  int rand_x4;
  int rand_x3;
  int rand_x2;
  int rand_x1;
  uint x2;
  uint i;

  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    for (x2 = 0; (int)x2 < (int)map.width; x2 += 2) {
      tile_byte = (i8 *)tile_at(x2,0);
      *tile_byte = 7;
      tile_byte = (i8 *)tile_at(x2 + 1,map.height - 1);
      *tile_byte = 7;
    }
    if (keep_map == 0) {
      for (i = 0; (int)i < (int)map.width >> 3; i++) {
        if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
          rand_x1 = 0;
        }
        else {
          rand_x1 = crt_rand_at("map_generate");
          rand_x1 = rand_x1 % ((int)map.width / 2);
        }
        tile_byte = (i8 *)tile_at(rand_x1 * 2,0);
        *tile_byte = 6;
        if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
          rand_x2 = 0;
        }
        else {
          rand_x2 = crt_rand_at("map_generate");
          rand_x2 = rand_x2 % ((int)map.width / 2);
        }
        tile_byte = (i8 *)tile_at(rand_x2 * 2 + 1,1);
        *tile_byte = 6;
        if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
          rand_x3 = 0;
        }
        else {
          rand_x3 = crt_rand_at("map_generate");
          rand_x3 = rand_x3 % ((int)map.width / 2);
        }
        tile_byte = (i8 *)tile_at(rand_x3 * 2 + 1,map.height - 1);
        *tile_byte = 6;
        if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
          rand_x4 = 0;
        }
        else {
          rand_x4 = crt_rand_at("map_generate");
          rand_x4 = rand_x4 % ((int)map.width / 2);
        }
        tile_byte = (i8 *)tile_at(rand_x4 * 2,map.height - 2);
        *tile_byte = 6;
      }
    }
  }
}

/* Part of map_generate (00408d33): ocean resources with no land within
   the city radius are hidden (terrain bit 0x40). */
static void mapgen_hide_far_ocean_resources(void)
{
  iptr x;
  int tmp;
  uint y2;
  uint x2;
  Tile *tile;
  uint i;
  int y3;
  uptr idx;
  int has_land;

  if (map_layout_wide == 0) {
    tile = map_data.tiles;
    y2 = 0;
    x2 = 0;
    for (i = 0; (int)i < (int)map.area; i++) {
      if ((tile->terrain == '\n') && (tmp = tile_resource(x2,y2), tmp != 0)) {
        has_land = 0;
        for (idx = 0; (has_land == 0 && ((int)idx < 0x14)); idx++) {
          x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[idx] + x2);
          y3 = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[idx] + y2;
          tmp = map_in_bounds(x,y3);
          if ((tmp != 0) && (tmp = tile_is_ocean(x,y3), tmp == 0)) {
            has_land = 1;
          }
        }
        if (has_land == 0) {
          tile->terrain = tile->terrain | TERRAIN_NO_RESOURCE;
        }
      }
      tile++;
      x2 += 2;
      if ((int)map.width <= (int)x2) {
        y2++;
        x2 = y2 & 1;
      }
      ui_pump_messages_net();
    }
  }
}

/* 00408d33 auto */
void __cdecl map_generate(int keep_map)

{
  iptr x;
  iptr north_ocean;
  i8 *tile_byte;
  iptr south_ocean;
  Dialog dialog2;
  uint x2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  dialog2.Dialog::reset(8);
  dialog2.set_position(
             -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
             -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
  x = (iptr)label_string(label_table.string_ids[2]);
  dialog2.set_title((const char *)x);
  dialog2.set_width(0x100);
  dialog2.add_line((const char *)&str_empty_6,0xffffffff,0xffffffff,0xffffffff,0);
  text_clear();
  text_append((const char *)&str_empty_7);
  text_append_label(8);
  dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
  dialog2.add_line((const char *)&str_empty_8,0xffffffff,0xffffffff,0xffffffff,0);
  dialog2.show_modal(0);
  ((WndBase *)(dialog2.surf_wnd + 0x48))->set_cursor(0x200);
  main_frame_wnd_block.main_frame_wnd.set_cursor(0x200);
  ui_flush_input();
  if (keep_map == 0) {
    mapgen_place_landmasses();
    mapgen_fill_diagonal_gaps();
    ui_pump_messages_net();
    mapgen_assign_base_terrain();
    mapgen_apply_rainfall();
    mapgen_age_terrain();
    ui_pump_messages_net();
    mapgen_smooth_terrain();
    ui_pump_messages_net();
    mapgen_place_rivers();
    ui_pump_messages_net();
  }
  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    for (x2 = 0; (int)x2 < (int)map.width; x2 += 2) {
      north_ocean = tile_is_ocean(x2 + 1,1);
      if (north_ocean == 0) {
        tile_byte = (i8 *)tile_at(x2,0);
        *tile_byte = 7;
      }
      south_ocean = tile_is_ocean(x2,map.height - 2);
      if (south_ocean == 0) {
        tile_byte = (i8 *)tile_at(x2 + 1,map.height - 1);
        *tile_byte = 7;
      }
    }
  }
  map_assign_landmass_ids();
  map_build_coarse_connectivity();
  mapgen_calc_fertility();
  mapgen_place_polar_caps(keep_map);
  map_fill_tile_byte((iptr)&map_data.tiles->improvements,0);
  mapgen_hide_far_ocean_resources();
  crt_memcpy(&map_handles.offmap_tile,map_data.tiles,6);
  main_frame_wnd_block.main_frame_wnd.set_cursor(0x201);
  ui_flush_input();
  return;
}

/* 0040a572 auto */
void __cdecl mapgen_add_landmass(int islet)

{
  Tile *tile;
  int rnd;
  int roll;
  int y2;
  int x2;
  u8 *owner_ptr;
  u8 *count_ptr;
  int i;
  
  map_fill_tile_byte((iptr)&map_data.tiles->owner,0);
  do {
    if (map.width == 0x11 || map.width - 0x11 < 0) {
      x2 = 0;
    }
    else {
      x2 = _rand();
      x2 = x2 % (map.width - 0x10);
    }
    x2 += 8;
    if (map.height == 9 || map.height - 9 < 0) {
      y2 = 0;
    }
    else {
      y2 = _rand();
      y2 = y2 % (map.height - 8);
    }
    y2 += 4;
    if (islet == 0) {
      if (mapgen.land_form == -1 || mapgen.land_form + 1 < 0) {
        roll = 0;
      }
      else {
        roll = _rand();
        roll = roll % (mapgen.land_form + 2);
      }
      if (roll != 0) break;
    }
    tile = tile_at(x2,y2);
  } while (tile->improvements != '\0');
  if (islet == 0) {
    if (mapgen.land_form < 1) {
      mapgen_land_walk(x2,y2);
    }
    else {
      mapgen_land_walk_wide(x2,y2);
    }
  }
  else {
    rnd = _rand();
    mapgen_islet_walk(x2,y2);
    if (6 < rnd % 10) {
      mapgen_islet_walk(x2,y2);
    }
    if (8 < rnd % 10) {
      mapgen_islet_walk(x2,y2);
    }
  }
  owner_ptr = &map_data.tiles->owner;
  count_ptr = &map_data.tiles->improvements;
  for (i = 0; i < map.area; i++) {
    if (*owner_ptr != '\0') {
      *count_ptr = *count_ptr + 1;
      mapgen_bounds.land_placed++;
    }
    owner_ptr += 6;
    count_ptr += 6;
  }
  ui_pump_messages_net();
  return;
}

/* 0040a763 auto */
void __cdecl mapgen_land_walk(int x,int y)

{
  int dir_idx;
  uint rnd;
  Tile *tile;
  iptr in_bounds;
  uint sign;
  int steps;
  
  rnd = _rand();
  sign = (int)rnd >> 0x1f;
  steps = ((rnd ^ sign) - sign & 0x3f ^ sign) - sign;
  tile = tile_at(x,y);
  if (tile->improvements != '\0') {
    steps = steps >> 1;
  }
  steps++;
  while( true ) {
    if (steps == 0) break;
    in_bounds = mapgen_in_bounds(x,y);
    if (in_bounds == 0) break;
    mapgen_mark_tile_cluster(x,y);
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    dir_idx = (((rnd ^ sign) - sign & 3 ^ sign) - sign) * 2;
    x += (((i8 *)&dir_dx_table_block.mapgen_cardinal_dx_table))[dir_idx];
    y += (((i8 *)&dir_dy_table_block.mapgen_cardinal_dy_table))[dir_idx];
    steps--;
  }
  ui_pump_messages_net();
  return;
}

/* 0040a824 auto */
i32 __cdecl mapgen_in_bounds(int x,int y)

{
  i32 result;
  
  if (((game.rules & RULE_FLAT_WORLD) == 0) || ((mapgen_bounds.min_x <= x && (x <= mapgen_bounds.max_x)))) {
    if ((y < mapgen_bounds.min_y) || (mapgen_bounds.max_y < y)) {
      result = 0;
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

/* 0040a892 auto */
void __cdecl mapgen_mark_tile_cluster(int x,int y)

{
  mapgen_mark_tile(x,y);
  mapgen_mark_tile(x + 1,y - 1);
  mapgen_mark_tile(x + 1,y + 1);
  ui_pump_messages_net();
  return;
}

/* 0040a8db auto */
void __cdecl mapgen_mark_tile(iptr raw_x,iptr y)

{
  iptr x;
  iptr in_bounds;
  Tile *tile;
  
  x = map_wrap_x(raw_x);
  in_bounds = mapgen_in_bounds(x,y);
  if (in_bounds != 0) {
    tile = tile_at(x,y);
    tile->owner = 1;
  }
  return;
}

/* 0040a92f auto */
void __cdecl mapgen_land_walk_wide(int x,int y)

{
  int dir_idx;
  Tile *tile;
  iptr in_bounds;
  uint rnd;
  uint sign;
  int steps;
  
  steps = _rand();
  steps = steps % 0x30;
  tile = tile_at(x,y);
  if (tile->improvements != '\0') {
    steps = steps >> 1;
  }
  steps++;
  while( true ) {
    if (steps == 0) break;
    in_bounds = mapgen_in_bounds(x,y);
    if (in_bounds == 0) break;
    mapgen_mark_tile_cluster(x,y);
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    if (((rnd ^ sign) - sign & 3 ^ sign) == sign) {
      mapgen_mark_tile_cluster(x + 2,y);
    }
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    if (((rnd ^ sign) - sign & 3 ^ sign) == sign) {
      mapgen_mark_tile_cluster(x - 2,y);
    }
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    if (((rnd ^ sign) - sign & 3 ^ sign) == sign) {
      mapgen_mark_tile_cluster(x,y + 2);
    }
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    if (((rnd ^ sign) - sign & 3 ^ sign) == sign) {
      mapgen_mark_tile_cluster(x,y - 2);
    }
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    dir_idx = (((rnd ^ sign) - sign & 3 ^ sign) - sign) * 2;
    x += (((i8 *)&dir_dx_table_block.mapgen_cardinal_dx_table))[dir_idx];
    y += (((i8 *)&dir_dy_table_block.mapgen_cardinal_dy_table))[dir_idx];
    steps--;
  }
  ui_pump_messages_net();
  return;
}

/* 0040aaa4 auto */
void __cdecl mapgen_islet_walk(int x,int y)

{
  uint rnd;
  iptr in_bounds;
  uint sign;
  iptr dir_idx;
  int steps;
  
  rnd = _rand();
  sign = (int)rnd >> 0x1f;
  steps = (((rnd ^ sign) - sign & 0xf ^ sign) - sign) + 1;
  while( true ) {
    if (steps == 0) break;
    in_bounds = mapgen_in_bounds(x,y);
    if (in_bounds == 0) break;
    mapgen_mark_tile(x,y);
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    dir_idx = (((rnd ^ sign) - sign & 3 ^ sign) - sign) * 2;
    x += (((i8 *)&dir_dx_table_block.dir_dx_table))[dir_idx];
    y += (((i8 *)&dir_dy_table_block.dir_dy_table))[dir_idx];
    steps--;
  }
  ui_pump_messages_net();
  return;
}

/* 0040ab41 auto */
i32 __cdecl mapgen_try_place_lake(int x,int y)

{
  int check;
  i32 result;
  i8 *tile;
  
  check = map_in_bounds(x,y);
  if (check == 0) {
    result = 0;
  }
  else if ((((x < 2) || (y < 2)) || (map.width - 2 <= x)) ||
          (map.height - 2 <= y)) {
    result = 0;
  }
  else {
    check = tile_is_ocean(x - 2,y);
    if (check == 0) {
      check = tile_is_ocean(x + 2,y);
      if (check == 0) {
        check = tile_is_ocean(x,y - 2);
        if (check == 0) {
          check = tile_is_ocean(x,y + 2);
          if (check == 0) {
            tile = (i8 *)tile_at(x,y);
            *tile = 10;
            result = 1;
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
        result = 0;
      }
    }
    else {
      result = 0;
    }
  }
  return result;
}

/* 0040ac5a auto */
void __cdecl mapgen_place_rivers(void)

{
  byte terrain;
  char dy;
  bool reached_ocean;
  uint start_y2;
  iptr start_x2;
  int y;
  Tile *map_tile;
  byte *tile;
  uint rnd;
  iptr x;
  iptr nb_in_bounds;
  iptr nb_is_ocean;
  int in_bounds;
  uint sign;
  int half_x;
  uptr dir;
  int rivers_placed;
  int attempts;
  uint river_len;
  uint y2;
  iptr x2;
  iptr i;
  int equator_dist;
  
  rivers_placed = 0;
  attempts = 0;
  do {
    map_copy_tile_byte((iptr)&map_data.tiles->owner,(iptr)map_data.tiles);
    attempts++;
    river_len = 0;
    do {
      if (map.height == 1 || map.height - 1 < 0) {
        y2 = 0;
      }
      else {
        y = _rand();
        y2 = y % (int)map.height;
      }
      start_y2 = y2;
      if ((int)map.width / 2 == 1 || (int)map.width / 2 - 1 < 0) {
        half_x = 0;
      }
      else {
        half_x = _rand();
        half_x = half_x % ((int)map.width / 2);
      }
      x2 = half_x * 2;
      if ((y2 & 1) != 0) {
        x2++;
      }
      start_x2 = x2;
      tile = (byte *)tile_at(x2,y2);
      terrain = *tile;
    } while ((terrain == TERRAIN_MOUNTAINS) || (terrain == TERRAIN_OCEAN));
    rnd = _rand();
    sign = (int)rnd >> 0x1f;
    dir = ((rnd ^ sign) - sign & 3 ^ sign) - sign;
    do {
      switch(terrain) {
      case TERRAIN_DESERT:
        terrain = 1;
        y = _rand();
        if ((1 < y % 10) || (3 < (int)river_len)) {
          terrain = 2;
        }
        break;
      case TERRAIN_PLAINS:
        y = _rand();
        if (3 < y % 10) {
          terrain = 2;
        }
        break;
      case TERRAIN_FOREST:
        y = _rand();
        if (2 < y % 10) {
          terrain = 2;
        }
        break;
      case TERRAIN_HILLS:
      case TERRAIN_MOUNTAINS:
        terrain = 2;
        break;
      case TERRAIN_TUNDRA:
        terrain = 1;
        break;
      case TERRAIN_GLACIER:
        terrain = 6;
        break;
      case TERRAIN_SWAMP:
        y = _rand();
        if (3 < y % 10) {
          terrain = 9;
        }
      }
      terrain = terrain | 0x80;
      tile = (byte *)tile_at(x2,y2);
      *tile = terrain;
      river_len++;
      reached_ocean = false;
      for (i = 0; (!reached_ocean && (i < 4)); i++) {
        x = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[i] + x2);
        dy = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[i];
        nb_in_bounds = map_in_bounds(x,(int)dy + y2);
        if ((nb_in_bounds != 0) && (nb_is_ocean = tile_is_ocean(x,(int)dy + y2), nb_is_ocean != 0)) {
          reached_ocean = true;
        }
      }
      rnd = _rand();
      sign = (int)rnd >> 0x1f;
      dir = (dir + (((rnd ^ sign) - sign & 1 ^ sign) - sign)) - (river_len & 1) &
                 3;
      x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x2);
      y2 += (int)(((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir];
      y = map_in_bounds(x2,y2);
      if (y != 0) {
        map_tile = tile_at(x2,y2);
        terrain = map_tile->owner;
      }
    } while (((!reached_ocean) && (y = map_in_bounds(x2,y2), y != 0)) &&
            ((terrain & 0x80) == 0));
    if (((reached_ocean) || ((terrain & 0x80) != 0)) && (5 - attempts / 800 <= (int)river_len)) {
      rivers_placed++;
      for (i = 0; i < 0x14; i++) {
        x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + start_x2);
        y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + start_y2;
        in_bounds = map_in_bounds(x,y);
        if (in_bounds != 0) {
          if ((int)map.height >> 1 == y || ((int)map.height >> 1) - y < 0) {
            equator_dist = ~(((int)map.height >> 1) - y) + 1;
          }
          else {
            equator_dist = ((int)map.height >> 1) - y;
          }
          tile = (byte *)tile_at(x,y);
          if (((*tile & 0xf) == 3) && (equator_dist < (map.height * 3) / 10)) {
            _rand();
          }
        }
      }
    }
    else {
      map_copy_tile_byte((iptr)map_data.tiles,(iptr)&map_data.tiles->owner);
    }
    ui_pump_messages_net();
  } while ((attempts < 0x400) && (rivers_placed < mapgen.land_mass * 2 + mapgen.climate * 2 + 0xc));
  return;
}

/* 0040bcb0 auto */
uint __cdecl tile_grassland_has_shield(int x,int y)

{
  return (uint)((-(y + x >> 1) - x & 2U) == 0);
}

/* 00410d98 auto */
uint __cdecl map_set_cursor_pos(iptr x,iptr y)

{
  int in_bounds;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds != 0) {
    blink.unit_phase = 0;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw_cursor_tile();
    map_cursor.x[0] = (i16)x;
    map_cursor.y[0] = (i16)y;
    status_redraw(1);
    blink.unit_phase = 1;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw_cursor_tile();
  }
  return (uint)(in_bounds != 0);
}

/* 00410f77 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl map_window_click(i32 screen_x,i32 screen_y,int alt_click)

{
  iptr flush_pending;
  iptr child_wnd;
  int result;
  iptr x2;
  int paradrop_active;
  iptr unit;
  iptr city;
  uptr dir;
  iptr y2;
  MapView *view;
  uint goto_mode;
  
  if ((game.multiplayer == '\0') || (net.ui_input_busy == 0)) {
    flush_pending = XD_InFlushSendBuffer();
    if (((((flush_pending == 0) &&
          (((((net.unit_create_busy == 0 && (net.unit_delete_busy == 0)) && (net.city_create_busy == 0)) &&
            ((net.city_delete_busy == 0 && (net.lock_map_busy == 0)))) && (net.unlock_map_busy == 0)))) &&
         (((net.unit_stack_unlink_busy == 0 && (net.unit_stack_link_busy == 0)) && (net.unit_stack_bottom_busy == 0)))) &&
        ((((net.unit_relocate_busy == 0 && (net.city_acquire_wonder_busy == 0)) && (net.civ_eliminate_busy == 0)) &&
         ((net.civ_create_busy == 0 && (net.civ_check_destroyed_busy == 0)))))) &&
       (((net.unit_gather_cargo_busy == 0 && ((net.unit_lift_off_map_busy == 0 && (net.unit_delete_cargo_busy == 0)))) && (net.unit_destroy_redraw_busy == 0))
       )) {
      net.ui_input_busy = 1;
      if (city_turn.processing == 0) {
        child_wnd = wnd_get_active_child();
        if (child_wnd == 0) {
          view = (MapView *)0x0;
        }
        else {
          view = (MapView *)(child_wnd - 0x48);
        }
        goto_mode = (uint)(view->cursor_id_uptr == 0x1fe);
        paradrop_active = ui_paradrop_cursor_active();
        mapview_cancel_drag();
        if (alt_click == 0) {
          mapviews_reset_cursor();
        }
        result = view->screen_to_tile((iptr)&x2,(iptr)&y2,screen_x,screen_y);
        if (result == 0) {
          map_click_in_progress = 1;
          result = map_in_bounds(x2,y2);
          if (result != 0) {
            map_click.tile_x = x2;
            map_click.tile_y = y2;
            if ((extra_rule(X_MASS_MOVE) && (alt_click != 0)) && (GetKeyState(VK_SHIFT) < 0) &&
               (extras_mass_move(x2,y2) != 0)) {
              /* X36: Shift+right click, the active unit's like there */
              map_click_in_progress = 0;
              net.ui_input_busy = 0;
              return;
            }
            if ((paradrop_active != 0) && (alt_click != 0)) {
              blink.unit_phase = 0;
              view->redraw_cursor_tile();
              view->center_on(x2,y2);
              map_click_in_progress = 0;
              net.ui_input_busy = 0;
              return;
            }
            if ((moving_units_mode == 0) || (alt_click != 0)) {
              if (view->map_view_index == 0) {
                ui_redraw_all_maps(0);
                map_set_cursor_pos(x2,y2);
              }
              if (alt_click == 0) {
                city = city_at(x2,y2);
                if (-1 < city) {
                  if (((cities[city].owner == (*(i8 *)&human_civ)) || (game.reveal_map != '\0')) ||
                     ((cities[city].flags & CITYF_INVESTIGATED) != 0)) {
                    ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
                  }
                  else {
                    result = civ_has_advance((*(i8 *)&human_civ),ADV_TRADE);
                    if (((result != 0) &&
                        (result = tile_visible_to(x2,y2,(*(i8 *)&human_civ)), result != 0)) &&
                       ((((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to)
                          != 0 || ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))) ||
                        (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))))) {
                      city_info_popup(city);
                    }
                  }
                  map_click_in_progress = 0;
                  net.ui_input_busy = 0;
                  return;
                }
                ui_pick_unit_from_stack();
              }
            }
            else {
              if (goto_mode != 0) {
                unit = (iptr)game.active_unit;
                if (-1 < unit) {
                  units[unit].goto_x = (i16)x2;
                  units[unit].goto_y = (i16)y2;
                  units[unit].orders = ORDER_GOTO;
                  if (bugfix_state()) {
                    units[unit].aux_timer = '\0';  /* B9: the long move count starts again (docs/BUGS.md) */
                  }
                  unit_goto_step(unit);
                }
                map_click_in_progress = 0;
                net.ui_input_busy = 0;
                return;
              }
              if (paradrop_active != 0) {
                unit_paradrop((int)game.active_unit,x2,y2);
                map_click_in_progress = 0;
                net.ui_input_busy = 0;
                return;
              }
              if ((499 < (int)view->cursor_id_uptr) && ((int)view->cursor_id_uptr < 0x1fc)) {
                dir = view->cursor_id_uptr - 0x1f5;
                if ((int)dir < 0) {
                  dir = 7;
                }
                unit_move_dir((int)game.active_unit,dir,3);
                map_click_in_progress = 0;
                net.ui_input_busy = 0;
                return;
              }
              city = city_at(x2,y2);
              if (-1 < city) {
                if (((cities[city].owner == (*(i8 *)&human_civ)) || (game.reveal_map != '\0')) ||
                   ((cities[city].flags & CITYF_INVESTIGATED) != 0)) {
                  ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
                }
                else {
                  result = civ_has_advance((*(i8 *)&human_civ),ADV_TRADE);
                  if (((result != 0) &&
                      (result = tile_visible_to(x2,y2,(*(i8 *)&human_civ)), result != 0)) &&
                     ((((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to)
                        != 0 || ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))) ||
                      (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))))) {
                    city_info_popup(city);
                  }
                }
                map_click_in_progress = 0;
                net.ui_input_busy = 0;
                return;
              }
              unit = tile_first_unit(x2,y2);
              if ((-1 < unit) && (units[unit].owner == (*(i8 *)&human_civ))) {
                ui_redraw_all_maps(0);
                map_set_cursor_pos(x2,y2);
                ui_pick_unit_from_stack();
              }
              blink.unit_phase = 0;
              mapviews_redraw_cursor();
            }
            view->center_on(x2,y2);
          }
          map_click_in_progress = 0;
          net.ui_input_busy = 0;
        }
        else {
          net.ui_input_busy = 0;
        }
      }
      else {
        city_window_modal_notice();
        net.ui_input_busy = 0;
      }
    }
    else {
      dbg_log((const char *)&str_map_3_map_window_click_blocked_by_xd_flu);
      net.blocked_input_kind = 1;
      net.blocked_input_arg1 = screen_x;
      net.blocked_input_arg2 = screen_y;
      net.blocked_input_arg3 = alt_click;
      net.blocked_input_window = wnd_get_active_child();
      if (net.blocked_input_window == 0) {
        net.blocked_input_window = 0;
      }
      else {
        net.blocked_input_window -= 0x48;
      }
    }
  }
  return;
}

/* 00411705 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl map_window_double_click(i32 screen_x,i32 screen_y)

{
  bool was_busy;
  iptr x;
  iptr y;
  byte terrain;
  int result;
  uint resource;
  
  was_busy = false;
  if ((game.multiplayer != '\0') && (net.ui_input_busy != 0)) {
    was_busy = true;
  }
  result = XD_InFlushSendBuffer();
  if (((((((result == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
        (((net.city_create_busy == 0 && (net.city_delete_busy == 0)) &&
         ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))))) &&
       ((net.unit_stack_link_busy == 0 && (((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0))))
       )) && (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
              (((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))) &&
               (net.unit_delete_cargo_busy == 0)))))) && (net.unit_destroy_redraw_busy == 0)) {
    if (!was_busy) {
      net.ui_input_busy = 1;
    }
    if (city_turn.processing == 0) {
      wnd_get_active_child();
      y = map_click.tile_y;
      x = map_click.tile_x;
      result = map_in_bounds(map_click.tile_x,map_click.tile_y);
      if (((result != 0) && (result = tile_city_or_unit_owner(x,y), result < 0)) &&
         (result = tile_visible_to(x,y,human_civ[0]), result != 0)) {
        terrain = tile_terrain(x,y);
        if (terrain == TERRAIN_GRASSLAND) {
          result = tile_grassland_has_shield(x,y);
          resource = (uint)(result != 0);
        }
        else {
          resource = tile_resource(x,y);
        }
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_terrain_modal(resource * 0xb + (uint)terrain);
      }
      if (!was_busy) {
        net.ui_input_busy = 0;
      }
    }
    else {
      city_window_modal_notice();
      if (!was_busy) {
        net.ui_input_busy = 0;
      }
    }
  }
  else {
    dbg_log((const char *)&str_map_3_map_double_click_blocked_by_xd_flu);
    net.blocked_input_kind = 2;
    net.blocked_input_arg1 = screen_x;
    net.blocked_input_arg2 = screen_y;
    net.blocked_input_window = wnd_get_active_child();
    if (net.blocked_input_window == 0) {
      net.blocked_input_window = 0;
    }
    else {
      net.blocked_input_window -= 0x48;
    }
  }
  return;
}

/* 00411a13 auto */
void __cdecl map_cursor_step_dir(iptr dir)

{
  char dy;
  i32 new_x;
  int moved;
  int cursor_y;
  
  new_x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)map_cursor.x[0]);
  dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
  cursor_y = (int)map_cursor.y[0];
  moved = map_set_cursor_pos(new_x,dy + cursor_y);
  if (moved != 0) {
    ((MapView *)((i8 *)&map_views_block.map_views))->scroll_if_near_edge(new_x,dy + cursor_y);
  }
  return;
}

/* 00411a85 auto */
void __cdecl map_ascii_view_mode(int key)

{
  iptr city;
  int has_advance;
  
  if ((key == 0xd) || (key == 0x20)) {
    if (((game.transient_flags & TRANSIENT_END_OF_TURN) == 0) || (key != 0xd)) {
      city = city_at((int)map_cursor.x[0],(int)map_cursor.y[0]);
      if (-1 < city) {
        if (((cities[city].owner == human_civ[0]) || (game.reveal_map != '\0')) ||
           ((cities[city].flags & CITYF_INVESTIGATED) != 0)) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
        }
        else {
          has_advance = civ_has_advance(human_civ[0],ADV_TRADE);
          if ((has_advance != 0) || (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
            city_info_popup(city);
          }
        }
      }
    }
    else {
      player_turn_active = 0;
    }
  }
  else if (key == 0x43) {
    ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)map_cursor.x[0],(int)map_cursor.y[0]);
  }
  return;
}

/* 00411bf5 auto */
void __cdecl map_ascii_unit_cmd(int key,int raw_key)

{
  iptr unit;
  iptr city;
  int has_advance;
  
  unit = (iptr)game.active_unit;
  if (((game.multiplayer < 3) || (units[unit].owner == human_civ[0])) || (key == 0xd)) {
    switch(key) {
    case 0xd:
      city = city_at((int)map_cursor.x[0],(int)map_cursor.y[0]);
      if (-1 < city) {
        if (((cities[city].owner == human_civ[0]) || (game.reveal_map != '\0')) ||
           ((cities[city].flags & CITYF_INVESTIGATED) != 0)) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
        }
        else {
          has_advance = civ_has_advance(human_civ[0],ADV_TRADE);
          if ((has_advance != 0) || (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
            city_info_popup(city);
          }
        }
      }
      break;
    case 0x20:
      cmd_unit_skip_turn();
      break;
    case 0x42:
      unit_cmd_build_city();
      break;
    case 0x43:
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y);
      break;
    case 0x45:
      unit_cmd_terraform(ORDER_BUILD_AIRBASE);
      break;
    case 0x46:
      if (unit_types[units[unit].type].role == ROLE_SETTLE) {
        unit_cmd_terraform(ORDER_BUILD_FORTRESS);
      }
      else {
        unit_cmd_fortify();
      }
      break;
    case 0x47:
      unit_cmd_goto_city();
      break;
    case 0x48:
      unit_cmd_home_city();
      break;
    case 0x49:
      unit_cmd_terraform(ORDER_IRRIGATE);
      break;
    case 0x4b:
      unit_cmd_automate();
      break;
    case 0x4c:
      unit_cmd_airlift();
      break;
    case 0x4d:
      unit_cmd_terraform(ORDER_MINE);
      break;
    case 0x4f:
      unit_cmd_terraform(ORDER_TRANSFORM);
      break;
    case 0x50:
      if ((raw_key == 0x70) && ((unit_types[units[unit].type].flags & UTYPE_PARADROP) != 0)) {
        unit_cmd_paradrop();
      }
      else if (unit_types[units[unit].type].role == ROLE_SETTLE) {
        unit_cmd_terraform(ORDER_CLEAN_POLLUTION);
      }
      else if (raw_key == 0x50) {
        unit_cmd_pillage();
      }
      break;
    case 0x51:
      if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) != 0) && (game.multiplayer == '\0')) {
        cheat_throneroom_upgrade();
      }
      break;
    case 0x52:
      unit_cmd_terraform(ORDER_BUILD_ROAD);
      break;
    case 0x53:
      unit_cmd_sleep();
      break;
    case 0x55:
      unit_cmd_unload();
      break;
    case 0x57:
      unit_cmd_wait();
    }
  }
  return;
}

/* 00411f91 auto */
void __cdecl map_window_ascii(uint key)

{
  bool unhandled;
  uint raw_key;
  int result;
  uint upper;
  int city_key;
  
  raw_key = key;
  if (game_session_active == 0) {
    return;
  }
  if ((game.multiplayer != '\0') && (net.ui_input_busy != 0)) {
    return;
  }
  if (net.blocked_input_kind == 4) {
    dbg_log((const char *)&str_map_3_map_ascii_blocked_by_xd_flushsendb);
    net.blocked_input_char_pending = 3;
    net.blocked_input_char = (char)key;
    return;
  }
  net.ui_input_busy = 1;
  if (city_turn.processing != 0) {
    if ((char)key == '\r') {
      net.ui_input_busy = 0;
      return;
    }
    if ((char)key == 27) {
      net.ui_input_busy = 0;
      return;
    }
    if ((char)key == '\n') {
      net.ui_input_busy = 0;
      return;
    }
    city_window_modal_notice();
    net.ui_input_busy = 0;
    return;
  }
  result = ((CityWindow *)((i8 *)&city_wnd.window))->city_window_is_closed();
  city_key = 0;
  if ((result == 0) && (((byte)game.game_options & OPT_ENTER_CLOSES_CITY) != 0)) {
    city_key = 1;
    switch(key & 0xff) {
    case 0xd:
    case 0x45:
    case 0x58:
    case 0x65:
    case 0x78:
      city_window_exit_clicked(0);
      break;
    default:
      city_key = 0;  /* not a city window key */
      break;
    case 0x41:
    case 0x43:
    case 0x61:
    case 99:
      city_button_change(0);
      break;
    case 0x42:
    case 0x62:
      city_button_buy(0);
      break;
    case 0x48:
    case 0x68:
      city_button_happy(0);
      break;
    case 0x49:
    case 0x69:
      city_button_info(0);
      break;
    case 0x4d:
    case 0x6d:
      city_button_map(0);
      break;
    case 0x52:
    case 0x72:
      city_button_rename(0);
      break;
    case 0x56:
    case 0x76:
      city_button_view(0);
    }
  }
  if (city_key == 0) {
    mapview_cancel_drag();
    result = ui_paradrop_cursor_active();
    if ((result == 0) || (mapviews_reset_cursor(), (char)key != 'p')) {
      result = _isalpha(key & 0xff);
      if (result != 0) {
        upper = crt_toupper(key & 0xff);
        key = upper & 0xff;
      }
      unhandled = false;
      switch(raw_key & 0xff) {
      case 0x43:
        menu_find_city();
        break;
      case 0x44:
        unit_cmd_disband();
        break;
      default:
        unhandled = true;
        break;
      case 0x48:
        if (game.multiplayer != 1) {
          throneroom_show(human_civ[0]);
        }
        break;
      case 0x52:
        menu_revolution();
        break;
      case 0x53:
        if (game.multiplayer != 2) {
          game_save_dialog(0);
        }
        break;
      case 0x54:
        taxrate_dialog_show(human_civ[0]);
        break;
      case 0x58:
        map_views_block.mapview_zoom[0] = -3;
        ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
        break;
      case 0x5a:
        map_views_block.mapview_zoom[0] = 0;
        map_views_block.map_view_center_x[0] = map_cursor.x[0];
        map_views_block.mapview_center_y[0] = map_cursor.y[0];
        ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
      }
      if (unhandled) {
        switch(key & 0xff) {
        case 0x41:
          ui_pick_unit_from_stack();
          break;
        case 0x54:
          menu_show_hidden_terrain();
          break;
        case 0x56:
          if (moving_units_mode == 1) {
            ui_redraw_all_maps(0);
          }
          else {
            ui_focus_active_unit(1);
          }
          break;
        case 0x58:
          if (-7 < map_views_block.mapview_zoom[0]) {
            map_views_block.mapview_zoom[0]--;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
          break;
        case 0x5a:
          if (map_views_block.mapview_zoom[0] < 8) {
            map_views_block.mapview_zoom[0]++;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
        }
        if (((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) != 0) && (game.multiplayer == '\0')) &&
           ((key & 0xff) == 0x59)) {
          game.reveal_map = game.reveal_map == '\0';
          map_redraw_all_views(human_civ[0],1);
        }
        if (moving_units_mode == 0) {
          thunk_map_ascii_view_mode(key & 0xff);
        }
        else {
          map_ascii_unit_cmd(key & 0xff,raw_key & 0xff);
        }
      }
    }
  }
  net.ui_input_busy = 0;
  return;
}

/* Part of map_window_key (004125c6): the commands below 0xd3: close the
   city window, the advisors and reports of the World/Advisors menus. */
static void map_key_report_command(int cmd)
{
  int result;

  if (cmd == 0xd2) {
    result = ((CityWindow *)((i8 *)&city_wnd.window))->city_window_is_closed();
    if (result == 0) {
      city_window_close();
    }
  }
  else {
    switch(cmd) {
    case 0xb0:
      city_status_show(human_civ[0]);
      break;
    case 0xb1:
      defense_minister_show(human_civ[0]);
      break;
    case 0xb2:
      foreign_minister_show(human_civ[0]);
      break;
    case 0xb3:
      attitude_advisor_show(human_civ[0]);
      break;
    case 0xb4:
      trade_advisor_show(human_civ[0]);
      break;
    case 0xb5:
      science_advisor_show(human_civ[0]);
      break;
    case 0xb6:
      wonders_report_show(human_civ[0]);
      break;
    case 0xb7:
      top_cities_show(human_civ[0]);
      break;
    case 0xb8:
      civ_score_window_show(human_civ[0]);
      if (game.reveal_map != '\0') {
        civ_rating_window_show(human_civ[0]);
        powergraph_show();
      }
      break;
    case 0xba:
      demographics_show(human_civ[0]);
      break;
    case 0xbb:
      if ((game.rules & RULE_BLOODLUST) == 0) {
        spaceship_select_view_dialog(human_civ[0]);
      }
    }
  }
}

/* Part of map_window_key (004125c6): the commands 0xd3..0x354: the Game
   menu (options, save/load, quit, zoom, chat, multiplayer) and the high
   council. */
static void map_key_game_command(int cmd)
{
  SHORT shift_state;
  int result;

  if (cmd == 0x354) {
    if ((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
       (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) == '\0')) {
      council_show(human_civ[0],0);
    }
    else {
      council_show(human_civ[0],1);
    }
  }
  else {
    switch(cmd) {
    case 0x243:
      if (2 < game.multiplayer) {
        chat_wnd_block.open(0,4);
      }
      break;
    case 0x244:
      casualty_timeline_show(human_civ[0]);
      break;
    case 0x245:
      menu_message_options();
      break;
    case 0x246:
      if (game.multiplayer == '\0') {
        editor_toggle_scenario_flag();
      }
      break;
    case 0x247:
      game.game_options = game.game_options ^ OPT_MAP_GRID;
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_MAP_GRID,(game.game_options & OPT_MAP_GRID) >> 5);
      map_redraw_all_views(human_civ[0],1);
      prefs.game_options = game.game_options;
      prefs_save_civ2dat();
      break;
    case 0x248:
      if ((game.multiplayer == 1) &&
         (result = popup_game_with_sprite((const char *)&str_reallyquit,(iptr)&popup_info_icon_sprite,0), result != 0)) {
        snd_cd_stop();
        map_display_disable();
      }
      break;
    case 0x24a:
      if ((mp_options.open_game != 0) && (game.multiplayer == 1)) {
        game_join_dialog();
      }
      break;
    case 0x24b:
      if ((game.multiplayer == '\0') || (mp_cheat_menu_allowed[0] != 0)) {
        cheat_toggle_mode();
      }
      break;
    case 0x24c:
      if (game.multiplayer == '\0') {
        menu_load_game();
      }
      break;
    case 0x24e:
      cmd_end_player_turn();
      break;
    case 0x24f:
      menu_game_options();
      break;
    case 0x250:
      menu_graphic_options();
      break;
    case 0x251:
      game_quit_or_retire(0);
      break;
    case 0x252:
      game_quit_or_retire(1);
      break;
    case 0x253:
      if (game.multiplayer != 2) {
        game_save_dialog(0);
      }
      break;
    case 0x254:
      if (((game.multiplayer != '\0') && (game.multiplayer != 2)) && (net.host_flag != '\0'))
      {
        mp_cmd_change_timer();
      }
      break;
    case 599:
      civ_change_password_dialog(human_civ[0]);
      break;
    case 600:
      map_views_block.mapview_zoom[0] = 0xfff9;
      ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
      break;
    case 0x259:
      if (game.multiplayer != '\0') {
        menu_multiplayer_options();
      }
      break;
    case 0x25a:
      map_views_block.mapview_zoom[0] = 8;
      ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    }
  }
}

/* Part of map_window_key (004125c6): the Cheat menu and the scenario
   editors (single player, cheat mode on). */
static void map_key_cheat_command(int cmd)
{

  if (cmd < 0x332) {
    if (cmd == 0x331) {
      tech_editor_run();
    }
    else {
      switch(cmd) {
      case 0x1b0:
        cheat_create_unit();
        break;
      case 0x1b1:
        cheat_reveal_map();
        break;
      case 0x1b2:
        cheat_set_human_player();
        break;
      case 0x1b3:
        cheat_set_game_year();
        break;
      case 0x1b4:
        cheat_kill_civ();
        break;
      case 0x1b5:
        cheat_tech_advance();
        break;
      case 0x1b6:
        cheat_force_government();
        break;
      case 0x1b7:
        cheat_terrain_saved_bits = 0xffffffff;
        cheat_change_terrain();
        break;
      case 0x1b8:
        cheat_change_money();
      }
    }
  }
  else {
    switch(cmd) {
    case 0x332:
      city_editor_run();
      break;
    case 0x333:
      cosmic_editor_run();
      break;
    case 0x334:
      impr_editor_run();
      break;
    case 0x335:
      terrain_editor_open();
      break;
    case 0x336:
      tribe_editor_run();
      break;
    case 0x337:
      unit_editor_run();
      break;
    case 0x338:
      events_editor_open();
      break;
    case 0x343:
      cheat_edit_city();
      break;
    case 0x344:
      cheat_destroy_units_at_cursor();
      break;
    case 0x34b:
      cheat_edit_king();
      break;
    case 0x350:
      cheat_scenario_params();
      break;
    case 0x353:
      cheat_save_as_scenario();
      break;
    case 0x355:
      cheat_edit_unit();
      break;
    case 0x3b5:
      cheat_edit_technologies();
      break;
    case 0x3b7:
      cheat_change_terrain();
    }
  }
}

/* 004125c6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl map_window_key(int cmd)

{
  int result;
  uptr move_dir;
  
  if (game_session_active == 0) {
    return;
  }
  if (((game.multiplayer != '\0') && (city_turn.processing != 0)) &&
     ((cmd == 0xd2 || (cmd == 0xd0)))) {
    result = ((CityWindow *)((i8 *)&city_wnd.window))->city_window_is_closed();
    if (result == 0) {
      city_window_exit_clicked(0);
      return;
    }
    return;
  }
  if ((game.multiplayer != '\0') && (net.ui_input_busy != 0)) {
    return;
  }
  result = XD_InFlushSendBuffer();
  if (((((((result != 0) || (net.unit_create_busy != 0)) || (net.unit_delete_busy != 0)) ||
        (((net.city_create_busy != 0 || (net.city_delete_busy != 0)) ||
         ((net.lock_map_busy != 0 || ((net.unlock_map_busy != 0 || (net.unit_stack_unlink_busy != 0)))))))) ||
       (net.unit_stack_link_busy != 0)) ||
      ((((net.unit_stack_bottom_busy != 0 || (net.unit_relocate_busy != 0)) || (net.city_acquire_wonder_busy != 0)) ||
       (((net.civ_eliminate_busy != 0 || (net.civ_create_busy != 0)) ||
        (((net.civ_check_destroyed_busy != 0 || ((net.unit_gather_cargo_busy != 0 || (net.unit_lift_off_map_busy != 0)))) || (net.unit_delete_cargo_busy != 0)
         ))))))) || (net.unit_destroy_redraw_busy != 0)) {
    dbg_log((const char *)&str_map_3_map_key_blocked_by_xd_flushsendbuf);
    net.blocked_input_kind = 4;
    net.blocked_input_arg1 = cmd;
    net.blocked_input_char_pending = 0;
    return;
  }
  net.ui_input_busy = 1;
  move_dir = 0xffffffff;
  if (city_turn.processing != 0) {
    if ((cmd != 0xd2) && (cmd != 0xd0)) {
      city_window_modal_notice();
      net.ui_input_busy = 0;
      return;
    }
    result = ((CityWindow *)((i8 *)&city_wnd.window))->city_window_is_closed();
    if (result != 0) {
      net.ui_input_busy = 0;
      return;
    }
    city_window_exit_clicked(0);
    net.ui_input_busy = 0;
    return;
  }
  if (cmd == 0x100) {
    net.ui_input_busy = 0;
    return;
  }
  if (cmd == 0) {
    net.ui_input_busy = 0;
    return;
  }
  mapview_cancel_drag();
  mapviews_reset_cursor();
  result = ((CityWindow *)((i8 *)&city_wnd.window))->city_window_is_closed();
  if ((result == 0) && ((game.game_options & OPT_ENTER_CLOSES_CITY) != 0)) {
    switch(cmd) {
    case 0xa2:
    case 0xa6:
    case 0xc1:
    case 0xc3:
      city_button_next_city(0);
      net.ui_input_busy = 0;
      return;
    case 0xa4:
    case 0xa8:
    case 0xc0:
    case 0xc2:
      city_button_prev_city(0);
      net.ui_input_busy = 0;
      return;
    }
  }
  if (cmd < 0xd3) {
    map_key_report_command(cmd);
  }
  else if (cmd < 0x355) {
    map_key_game_command(cmd);
  }
  if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
    map_key_cheat_command(cmd);
  }
  switch(cmd) {
  case 0xa1:
  case 199:
    move_dir = 4;
    break;
  case 0xa2:
  case 0xc1:
    move_dir = 3;
    break;
  case 0xa3:
  case 0xc6:
    move_dir = 2;
    break;
  case 0xa4:
  case 0xc2:
    move_dir = 5;
    break;
  case 0xa6:
  case 0xc3:
    move_dir = 1;
    break;
  case 0xa7:
  case 0xc4:
    move_dir = 6;
    break;
  case 0xa8:
  case 0xc0:
    move_dir = 7;
    break;
  case 0xa9:
  case 0xc5:
    move_dir = 0;
  }
  if ((((-1 < game.active_unit) && (game.active_unit < game.num_units)) &&
      (units[game.active_unit].id != 0)) && (-1 < (int)move_dir)) {
    if (moving_units_mode == 1) {
      unit_move_dir((int)game.active_unit,move_dir,3);
    }
    else {
      map_cursor_step_dir(move_dir);
    }
  }
  net.ui_input_busy = 0;
  return;
}

/* 0041dd0e auto */
i32 __cdecl map_load_dialog(void)

{
  iptr filter_str;
  iptr title;
  int ok;
  char *path_buf;
  i8 *default_ext;
  i32 dlg_flag;
  i32 dlg_arg;
  iptr civ;
  char filename[256];
  iptr filter;
  FILE *file;
  i32 result;
  Tile *tile;
  int has_starts;
  iptr i;
  
  result = 1;
  file = (FILE *)0x0;
  crt_strcpy(filename,((i8 *)&map_load_wildcard_str));
  filter_str = (iptr)save_file_filter(0,0,1);
  dlg_arg = 0;
  dlg_flag = 1;
  default_ext = ((i8 *)&map_load_default_ext_str);
  path_buf = filename;
  filter = filter_str;
  title = (iptr)label_string(label_table.string_ids[0x103]);
  ok = ui_file_name_dialog((iptr)&main_frame_wnd_block.main_frame_wnd,(const char *)title,(iptr)path_buf,filter_str,(iptr)default_ext,dlg_flag,dlg_arg);
  if (ok != 0) {
    file = _fopen(filename,((i8 *)&map_load_open_mode_str));
    if (file == (FILE *)0x0) {
      popup_game((const char *)&str_failedtoload);
    }
    else {
      ok = map_read((iptr)file,1);
      if (ok == 0) {
        tile = map_data.tiles;
        for (i = 0; i < map.area; i++) {
          tile->improvements = '\0';
          tile->radius = '\0';
          tile->visible = '\0';
          tile->landmass = '\0';
          tile->owner = 0xf0;
          tile++;
        }
        for (civ = 1; civ < 8; civ++) {
          _memset(map_data.known[civ],0,(int)map.area);
        }
        if (map.seed < 2) {
          map_new_seed();
        }
        else {
          ok = popup_game((const char *)&str_useseed);
          if (ok == 0) {
            map_new_seed();
          }
          snd_play_effect(SND_MENUOK,0,0,0);
        }
        has_starts = 0;
        for (i = 0; i < 0x15; i++) {
          if (-1 < start_positions.x[i]) {
            has_starts = 1;
            break;
          }
        }
        if ((has_starts != 0) && (ok = popup_game((const char *)&str_usestartloc), ok == 0)) {
          snd_play_effect(SND_MENUOK,0,0,0);
          for (i = 0; i < 0x15; i++) {
            start_positions.x[i] = -1;
            start_positions.y[i] = -1;
          }
        }
        if (map.flat == 0) {
          game.rules = game.rules & ~RULE_FLAT_WORLD;
        }
        else {
          game.rules = game.rules | RULE_FLAT_WORLD;
        }
        result = 0;
      }
      else {
        popup_game((const char *)&str_failedtoload_2);
      }
    }
  }
  return result;
}

/* 004272d0 auto */
void __cdecl tile_reveal_to_civ(iptr x,iptr y,int civ)

{
  int city;
  iptr unit;
  
  if (-1 < civ) {
    net_batch_begin();
    tile_set_visible(x,y,1 << ((byte)civ & 0x1f) & 0xff,1,1);
    tile_update_known(x,y,civ);
    city = city_at(x,y);
    if (city < 0) {
      unit = tile_first_unit(x,y);
      unit_stack_set_seen_by(unit,civ);
    }
    else {
      city_reveal_to_civ(city,civ);
    }
    net_batch_flush();
  }
  return;
}

/* 0042a6f8 auto */
void __cdecl tiles_dll_global_init(void)

{
  tiles_dll_static_init();
  tiles_dll_reg_atexit();
  return;
}

/* 0042a712 auto */
void __cdecl tiles_dll_static_init(void)

{
  ((DllHandle *)((i8 *)&tiles_dll_block.tiles_dll))->resource_dll_load((iptr)&str_tiles_dll);
  return;
}

/* 0042a731 auto */
void __cdecl tiles_dll_reg_atexit(void)

{
  _atexit(tiles_dll_static_dtor);
  return;
}

/* 0042a74e auto */
void __cdecl tiles_dll_static_dtor(void)

{
  ((DllHandle *)((i8 *)&tiles_dll_block.tiles_dll))->free();
  return;
}

/* 0043c3f0 auto */
DllHandle *DllHandle::resource_dll_load(iptr dll_name)

{
  u32 hmodule;
  
  hmodule = res_dll_load_library((LPCSTR)dll_name);
  this->hmodule = hmodule;
  return this;
}

/* 00447320 auto */
void __cdecl terrain_base_sprites_global_init(void)

{
  terrain_base_sprites_static_init();
  terrain_base_sprites_reg_atexit();
  return;
}

/* 0044733a auto */
void __cdecl terrain_base_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&terrain_base_sprites_block.terrain_base_sprites),0x3c,0xb,sprite_init,sprite_destroy);
  return;
}

/* 00447362 auto */
void __cdecl terrain_base_sprites_reg_atexit(void)

{
  _atexit(terrain_base_sprites_atexit_dtor);
  return;
}

/* 0044737f auto */
void __cdecl terrain_base_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&terrain_base_sprites_block.terrain_base_sprites),0x3c,0xb,sprite_destroy);
  return;
}

/* 004473a2 auto */
void __cdecl terrain_dither_sprites_global_init(void)

{
  terrain_dither_sprites_static_init();
  terrain_dither_sprites_reg_atexit();
  return;
}

/* 004473bc auto */
void __cdecl terrain_dither_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&map_cursor_sprite_block.terrain_dither_sprites),0x3c,0x34,sprite_init,sprite_destroy);
  return;
}

/* 004473e4 auto */
void __cdecl terrain_dither_sprites_reg_atexit(void)

{
  _atexit(terrain_dither_sprites_atexit_dtor);
  return;
}

/* 00447401 auto */
void __cdecl terrain_dither_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&map_cursor_sprite_block.terrain_dither_sprites),0x3c,0x34,sprite_destroy);
  return;
}

/* 00447424 auto */
void __cdecl river_sprites_global_init(void)

{
  river_sprites_static_init();
  river_sprites_reg_atexit();
  return;
}

/* 0044743e auto */
void __cdecl river_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&icon_misc_sprites_block.river_sprites),0x3c,0x10,sprite_init,sprite_destroy);
  return;
}

/* 00447466 auto */
void __cdecl river_sprites_reg_atexit(void)

{
  _atexit(river_sprites_atexit_dtor);
  return;
}

/* 00447483 auto */
void __cdecl river_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&icon_misc_sprites_block.river_sprites),0x3c,0x10,sprite_destroy);
  return;
}

/* 004474a6 auto */
void __cdecl forest_sprites_global_init(void)

{
  forest_sprites_static_init();
  forest_sprites_reg_atexit();
  return;
}

/* 004474c0 auto */
void __cdecl forest_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&forest_sprites),0x3c,0x10,sprite_init,sprite_destroy);
  return;
}

/* 004474e8 auto */
void __cdecl forest_sprites_reg_atexit(void)

{
  _atexit(forest_sprites_atexit_dtor);
  return;
}

/* 00447505 auto */
void __cdecl forest_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&forest_sprites),0x3c,0x10,sprite_destroy);
  return;
}

/* 00447528 auto */
void __cdecl mountain_sprites_global_init(void)

{
  mountain_sprites_static_init();
  mountain_sprites_reg_atexit();
  return;
}

/* 00447542 auto */
void __cdecl mountain_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&mountain_sprites),0x3c,0x10,sprite_init,sprite_destroy);
  return;
}

/* 0044756a auto */
void __cdecl mountain_sprites_reg_atexit(void)

{
  _atexit(mountain_sprites_atexit_dtor);
  return;
}

/* 00447587 auto */
void __cdecl mountain_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&mountain_sprites),0x3c,0x10,sprite_destroy);
  return;
}

/* 004475aa auto */
void __cdecl hill_sprites_global_init(void)

{
  hill_sprites_static_init();
  hill_sprites_reg_atexit();
  return;
}

/* 004475c4 auto */
void __cdecl hill_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&hill_sprites_block.hill_sprites),0x3c,0x10,sprite_init,sprite_destroy);
  return;
}

/* 004475ec auto */
void __cdecl hill_sprites_reg_atexit(void)

{
  _atexit(hill_sprites_atexit_dtor);
  return;
}

/* 00447609 auto */
void __cdecl hill_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&hill_sprites_block.hill_sprites),0x3c,0x10,sprite_destroy);
  return;
}

/* 0044762c auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl hut_sprite_global_init(void)

{
  hut_sprite_static_init();
  hut_sprite_reg_atexit();
  return;
}

/* 00447646 auto */
void __cdecl hut_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&hut_sprite));
  return;
}

/* 00447660 auto */
void __cdecl hut_sprite_reg_atexit(void)

{
  _atexit(hut_sprite_atexit_dtor);
  return;
}

/* 0044767d auto */
void __cdecl hut_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&hut_sprite));
  return;
}

/* 00447697 auto */
void __cdecl unused_sprite_pair_global_init(void)

{
  unused_sprite_pair_static_init();
  unused_sprite_pair_reg_atexit();
  return;
}

/* 004476b1 auto */
void __cdecl unused_sprite_pair_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&war_icon_sprite_block.unused_sprite_pair),0x3c,2,sprite_init,sprite_destroy);
  return;
}

/* 004476d9 auto */
void __cdecl unused_sprite_pair_reg_atexit(void)

{
  _atexit(unused_sprite_pair_atexit_dtor);
  return;
}

/* 004476f6 auto */
void __cdecl unused_sprite_pair_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&war_icon_sprite_block.unused_sprite_pair),0x3c,2,sprite_destroy);
  return;
}

/* 00447719 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl tile_pick_mask_sprite_global_init(void)

{
  tile_pick_mask_sprite_static_init();
  tile_pick_mask_sprite_reg_atexit();
  return;
}

/* 00447733 auto */
void __cdecl tile_pick_mask_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite));
  return;
}

/* 0044774d auto */
void __cdecl tile_pick_mask_sprite_reg_atexit(void)

{
  _atexit(tile_pick_mask_sprite_atexit_dtor);
  return;
}

/* 0044776a auto */
void __cdecl tile_pick_mask_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite));
  return;
}

/* 00447784 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl tile_direction_mask_sprite_global_init(void)

{
  tile_direction_mask_sprite_static_init();
  tile_direction_mask_sprite_reg_atexit();
  return;
}

/* 0044779e auto */
void __cdecl tile_direction_mask_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&tile_direction_mask_sprite));
  return;
}

/* 004477b8 auto */
void __cdecl tile_direction_mask_sprite_reg_atexit(void)

{
  _atexit(tile_direction_mask_sprite_atexit_dtor);
  return;
}

/* 004477d5 auto */
void __cdecl tile_direction_mask_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&tile_direction_mask_sprite));
  return;
}

/* 004477ef auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unexplored_tile_sprite_global_init(void)

{
  unexplored_tile_sprite_static_init();
  unexplored_tile_sprite_reg_atexit();
  return;
}

/* 00447809 auto */
void __cdecl unexplored_tile_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&unexplored_tile_sprite));
  return;
}

/* 00447823 auto */
void __cdecl unexplored_tile_sprite_reg_atexit(void)

{
  _atexit(unexplored_tile_sprite_atexit_dtor);
  return;
}

/* 00447840 auto */
void __cdecl unexplored_tile_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&unexplored_tile_sprite));
  return;
}

/* 0044785a auto */
void __cdecl coast_quarter_sprites_global_init(void)

{
  coast_quarter_sprites_static_init();
  coast_quarter_sprites_reg_atexit();
  return;
}

/* 00447874 auto */
void __cdecl coast_quarter_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites),0x3c,0x20,sprite_init,sprite_destroy);
  return;
}

/* 0044789c auto */
void __cdecl coast_quarter_sprites_reg_atexit(void)

{
  _atexit(coast_quarter_sprites_atexit_dtor);
  return;
}

/* 004478b9 auto */
void __cdecl coast_quarter_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites),0x3c,0x20,sprite_destroy);
  return;
}

/* 004478dc auto */
void __cdecl river_mouth_sprites_global_init(void)

{
  river_mouth_sprites_static_init();
  river_mouth_sprites_reg_atexit();
  return;
}

/* 004478f6 auto */
void __cdecl river_mouth_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites),0x3c,4,sprite_init,sprite_destroy);
  return;
}

/* 0044791e auto */
void __cdecl river_mouth_sprites_reg_atexit(void)

{
  _atexit(river_mouth_sprites_atexit_dtor);
  return;
}

/* 0044793b auto */
void __cdecl river_mouth_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites),0x3c,4,sprite_destroy);
  return;
}

/* 0044795e auto */
void __cdecl road_sprites_global_init(void)

{
  road_sprites_static_init();
  road_sprites_reg_atexit();
  return;
}

/* 00447978 auto */
void __cdecl road_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&road_sprites_block.road_sprites),0x3c,0x12,sprite_init,sprite_destroy);
  return;
}

/* 004479a0 auto */
void __cdecl road_sprites_reg_atexit(void)

{
  _atexit(road_sprites_atexit_dtor);
  return;
}

/* 004479bd auto */
void __cdecl road_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&road_sprites_block.road_sprites),0x3c,0x12,sprite_destroy);
  return;
}

/* 004479e0 auto */
void __cdecl land_improvement_sprites_global_init(void)

{
  land_improvement_sprites_static_init();
  land_improvement_sprites_reg_atexit();
  return;
}

/* 004479fa auto */
void __cdecl land_improvement_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&land_improvement_sprites_block.land_improvement_sprites),0x3c,3,sprite_init,sprite_destroy);
  return;
}

/* 00447a22 auto */
void __cdecl land_improvement_sprites_reg_atexit(void)

{
  _atexit(land_improvement_sprites_atexit_dtor);
  return;
}

/* 00447a3f auto */
void __cdecl land_improvement_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&land_improvement_sprites_block.land_improvement_sprites),0x3c,3,sprite_destroy);
  return;
}

/* 00447a62 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl pollution_sprite_global_init(void)

{
  pollution_sprite_static_init();
  pollution_sprite_reg_atexit();
  return;
}

/* 00447a7c auto */
void __cdecl pollution_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite));
  return;
}

/* 00447a96 auto */
void __cdecl pollution_sprite_reg_atexit(void)

{
  _atexit(pollution_sprite_atexit_dtor);
  return;
}

/* 00447ab3 auto */
void __cdecl pollution_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite));
  return;
}

/* 00447acd auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl grassland_shield_sprite_global_init(void)

{
  grassland_shield_sprite_static_init();
  grassland_shield_sprite_reg_atexit();
  return;
}

/* 00447ae7 auto */
void __cdecl grassland_shield_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&grassland_shield_sprite));
  return;
}

/* 00447b01 auto */
void __cdecl grassland_shield_sprite_reg_atexit(void)

{
  _atexit(grassland_shield_sprite_atexit_dtor);
  return;
}

/* 00447b1e auto */
void __cdecl grassland_shield_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&grassland_shield_sprite));
  return;
}

/* 00447b38 auto */
void __cdecl terrain_resource_sprites_global_init(void)

{
  terrain_resource_sprites_static_init();
  terrain_resource_sprites_reg_atexit();
  return;
}

/* 00447b52 auto */
void __cdecl terrain_resource_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites),0x3c,0x16,sprite_init,sprite_destroy);
  return;
}

/* 00447b7a auto */
void __cdecl terrain_resource_sprites_reg_atexit(void)

{
  _atexit(terrain_resource_sprites_atexit_dtor);
  return;
}

/* 00447b97 auto */
void __cdecl terrain_resource_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites),0x3c,0x16,sprite_destroy);
  return;
}

/* 00447bba auto */
void __cdecl airbase_sprites_global_init(void)

{
  airbase_sprites_static_init();
  airbase_sprites_reg_atexit();
  return;
}

/* 00447bd4 auto */
void __cdecl airbase_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&airbase_sprites_block.airbase_sprites),0x3c,2,sprite_init,sprite_destroy);
  return;
}

/* 00447bfc auto */
void __cdecl airbase_sprites_reg_atexit(void)

{
  _atexit(airbase_sprites_atexit_dtor);
  return;
}

/* 00447c19 auto */
void __cdecl airbase_sprites_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&airbase_sprites_block.airbase_sprites),0x3c,2,sprite_destroy);
  return;
}

/* 00448098 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl map_cursor_sprite_global_init(void)

{
  map_cursor_sprite_static_init();
  map_cursor_sprite_reg_atexit();
  return;
}

/* 004480b2 auto */
void __cdecl map_cursor_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite));
  return;
}

/* 004480cc auto */
void __cdecl map_cursor_sprite_reg_atexit(void)

{
  _atexit(map_cursor_sprite_static_dtor);
  return;
}

/* 004480e9 auto */
void __cdecl map_cursor_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite));
  return;
}

/* 0044847a auto */
void __cdecl resource_shortage_icon_sprites_global_init(void)

{
  resource_shortage_icon_sprites_static_init();
  resource_shortage_icon_sprites_reg_atexit();
  return;
}

/* 00448494 auto */
void __cdecl resource_shortage_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites),0x3c,6,sprite_init,sprite_destroy);
  return;
}

/* 004484bc auto */
void __cdecl resource_shortage_icon_sprites_reg_atexit(void)

{
  _atexit(resource_shortage_icon_sprites_static_dtor);
  return;
}

/* 004484d9 auto */
void __cdecl resource_shortage_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites),0x3c,6,sprite_destroy);
  return;
}

/* 00448acf auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl map_grid_sprite_global_init(void)

{
  map_grid_sprite_static_init();
  map_grid_sprite_reg_atexit();
  return;
}

/* 00448ae9 auto */
void __cdecl map_grid_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&map_grid_sprite));
  return;
}

/* 00448b03 auto */
void __cdecl map_grid_sprite_reg_atexit(void)

{
  _atexit(map_grid_sprite_static_dtor);
  return;
}

/* 00448b20 auto */
void __cdecl map_grid_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&map_grid_sprite));
  return;
}

/* 00479d20 auto */
void __cdecl map_views_global_init(void)

{
  map_views_static_init();
  map_views_reg_atexit();
  return;
}

/* 00479d3a auto */
void __cdecl map_views_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&map_views_block.map_views),0x3f0,8,mapview_ctor,mapview_dtor);
  return;
}

/* 00479d65 auto */
void __cdecl map_views_reg_atexit(void)

{
  _atexit(map_views_atexit_dtor);
  return;
}

/* 00479d82 auto */
void __cdecl map_views_atexit_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&map_views_block.map_views),0x3f0,8,mapview_dtor);
  return;
}

/* 0047a747 auto */
int __cdecl tile_calc_coast_masks(iptr base_x,int base_y)

{
  char dy;
  iptr in_bounds;
  iptr x;
  iptr nb_in_bounds;
  uptr quarter;
  int quarter_a;
  uptr quarter_b;
  char terrain2;
  uptr dir;
  int land_count;
  
  land_count = 0;
  for (dir = 0; (int)dir < 4; dir++) {
    (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[dir] = 0;
  }
  in_bounds = map_in_bounds(base_x,base_y);
  if (in_bounds == 0) {
    land_count = 0;
  }
  else {
    for (dir = 0; (int)dir < 8; dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + base_x);
      dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
      nb_in_bounds = map_in_bounds(x,dy + base_y);
      if (nb_in_bounds == 0) {
        terrain2 = 7;
      }
      else {
        terrain2 = tile_terrain(x,dy + base_y);
      }
      if (terrain2 != TERRAIN_OCEAN) {
        land_count++;
        if ((dir & 1) == 0) {
          quarter_a = (int)dir >> 1;
          quarter_b = quarter_a + 1U & 3;
          (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter_a] = (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter_a] | 4;
          (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter_b] = (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter_b] | 1;
        }
        else {
          quarter = (dir + 1 & 6) >> 1;
          (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter] = (((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter] | 2;
        }
      }
    }
  }
  return land_count;
}

/* 0047c2f2 auto */
i32 __cdecl map_x_in_span(int x,int left,int width)

{
  i32 result;
  
  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    if (x < left) {
      x += map.width;
    }
    if (left + width <= x) {
      x -= map.width;
    }
  }
  if (x < left) {
    result = 0;
  }
  else if ((left + width) - x < 1) {
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 0047c37f auto */
i32 __cdecl
map_tile_in_box(i32 x,int y,iptr left,iptr top,i32 width,int height
            )

{
  int in_span;
  i32 result;
  
  in_span = map_x_in_span(x,left,width);
  if (in_span == 0) {
    result = 0;
  }
  else if (y < top) {
    result = 0;
  }
  else if (y < height + top) {
    result = 1;
  }
  else {
    result = 0;
  }
  return result;
}

/* 0047ce1e auto */
void __cdecl
map_redraw_area_all_views(i32 x,i32 y,i32 radius,iptr civ,
            i32 full_redraw)

{
  int view_idx;
  
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if ((view_idx == 0) || (map_views_block.map_view_recs[view_idx].open != 0)) {
      map_views_block.map_view_recs[view_idx].redraw_area(x,y,radius,civ
                 ,full_redraw);
    }
  }
  return;
}

/* 0047cea6 auto */
void __cdecl map_redraw_tile_all_views(i32 x,i32 y)

{
  int view_idx;
  
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if ((view_idx == 0) || (map_views_block.map_view_recs[view_idx].open != 0)) {
      map_views_block.map_view_recs[view_idx].redraw_tile(x,y);
    }
  }
  return;
}

/* 0047cf22 auto */
void __cdecl map_redraw_neighbors_all_views(i32 x,iptr y)

{
  int view_idx;
  
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if ((view_idx == 0) || (map_views_block.map_view_recs[view_idx].open != 0)) {
      map_views_block.map_view_recs[view_idx].redraw_tile_neighbors(x,y);
    }
  }
  return;
}

/* 0047cf9e auto */
void __cdecl map_redraw_all_views(iptr civ,i32 full_redraw)

{
  int view_idx;
  
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if ((view_idx == 0) || (map_views_block.map_view_recs[view_idx].open != 0)) {
      map_views_block.map_view_recs[view_idx].redraw(civ,full_redraw);
    }
  }
  return;
}

/* 00484d3b auto */
void __cdecl map_display_disable(void)

{
  turn_loop.active = 0;
  return;
}

/* 004868fb auto */
void __cdecl map_apply_global_warming(uint severity)

{
  char dy;
  byte terrain;
  iptr x;
  int result;
  byte *known;
  int civ;
  int tile_idx;
  uint y2;
  uint x2;
  int ocean_count;
  iptr dir;
  
  if (autoplay.active == 0) {
    if (2 < game.multiplayer) {
      net_send_text_args(3,0xff,0,0,0,0);
    }
    popup_game_with_sprite((const char *)&str_globalwarming,(iptr)&popup_info_icon_sprite,0);
  }
  y2 = 0;
  x2 = 0;
  net_batch_begin();
  for (tile_idx = 0; tile_idx < map.area; tile_idx++) {
    terrain = tile_terrain(x2,y2);
    if (terrain < 4) {
      ocean_count = 0;
      for (dir = 0; dir < 0x14; dir++) {
        x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
        dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
        result = map_in_bounds(x,(int)dy + y2);
        if ((result != 0) && (result = tile_is_ocean(x,(int)dy + y2), result != 0)) {
          ocean_count++;
        }
      }
      if (ocean_count < (int)(7 - severity)) {
        if ((x2 * 3 + y2 * -3 & 7) == severity) {
          result = tile_owner(x2,y2);
          if (result < 0) {
            tile_set_owner(x2,y2,0,1);
          }
          if (terrain < 2) {
            tile_set_terrain(x2,y2,0,1);
          }
          else {
            tile_set_terrain(x2,y2,1,1);
          }
          result = tile_visible_to(x2,y2,human_civ[0]);
          if (result != 0) {
            map_redraw_tile_all_views(x2,y2);
          }
        }
      }
      else {
        result = tile_owner(x2,y2);
        if (result < 0) {
          tile_set_owner(x2,y2,0,1);
        }
        if (terrain == TERRAIN_FOREST) {
          tile_set_terrain(x2,y2,9,1);
        }
        else {
          tile_set_terrain(x2,y2,8,1);
        }
        tile_set_improvements(x2,y2,0xc,0,1);
        for (civ = 1; civ < 8; civ++) {
          result = tile_visible_to(x2,y2,civ);
          if (((result != 0) &&
              (known = known_at(x2,y2,civ), (*known & 0xc) != 0)) &&
             (tile_update_known(x2,y2,civ), human_civ[0] == civ)) {
            map_redraw_tile_all_views(x2,y2);
          }
        }
      }
    }
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  net_batch_flush();
  return;
}

/* 00487007 auto */
void __cdecl map_rebuild_unit_flags(void)

{
  iptr unit;
  int tile_idx;
  uint y2;
  uint x2;
  
  net.tile_sync_enabled = 0;
  y2 = 0;
  x2 = 0;
  for (tile_idx = 0; tile_idx < map.area; tile_idx++) {
    tile_set_improvements(x2,y2,1,0,1);
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if (units[unit].id != 0) {
      tile_set_improvements((int)units[unit].x,(int)units[unit].y,1,1,1);
    }
  }
  net.tile_sync_enabled = 1;
  net_send_state_diff(0xff,2,0,0,1);
  return;
}

/* 00488a45 auto */
i32 __cdecl map_road_connection(iptr civ,int x,int y,int x2,int y2)

{
  bool blocked;
  bool all_railroad;
  iptr landmass_a;
  iptr landmass_b;
  i32 result;
  int dir;
  uint improvements2;
  int steps;
  int y3;
  int x3;
  
  x3 = x;
  y3 = y;
  landmass_a = tile_land_landmass(x,y);
  landmass_b = tile_land_landmass(x2,y2);
  steps = 0;
  all_railroad = true;
  if (landmass_b == landmass_a) {
    dir = map_distance(x2,y2,x,y);
    if (dir < 0x17) {
      pathfind.road_planning = 1;
      pathfind.owner_civ = 0xffffffff;
      path_queue.goal_x = x2;
      path_queue.goal_y = y2;
      pathfind.unit_type = 2;
      pathfind.roads_only = 1;
      blocked = true;
      while( true ) {
        dir = map_path_next_dir(x3,y3,99);
        if ((dir < 0) || (dir == 8)) break;
        x3 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x3);
        y3 += (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
        dir = map_in_bounds(x3,y3);
        if (dir == 0) break;
        if ((x3 == x2) && (y3 == y2)) {
          blocked = false;
          break;
        }
        dir = tile_city_owner(x3,y3);
        if (civ == dir) {
          blocked = false;
          break;
        }
        if ((((-1 < dir) && ((*(byte *)&civs[civ].treaties[dir] & TREATY_ALLIANCE) == 0)) ||
            ((dir = tile_city_or_unit_owner(x3,y3), 0 < dir &&
             ((civ != dir && ((*(byte *)&civs[civ].treaties[dir] & TREATY_ALLIANCE) == 0))))
            )) || (improvements2 = tile_improvements(x3,y3), (improvements2 & TILE_ROAD) == 0))
        break;
        improvements2 = tile_improvements(x3,y3);
        if ((improvements2 & TILE_RAILROAD) == 0) {
          all_railroad = false;
        }
        steps++;
        if (0x32 < steps) break;
      }
      pathfind.road_planning = 0;
      pathfind.roads_only = 0;
      if (blocked) {
        result = 0;
      }
      else if (all_railroad) {
        result = 2;
      }
      else {
        result = 1;
      }
    }
    else {
      result = 0;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004abea0 auto */
iptr __cdecl map_dir_from_delta(int dx,int dy)

{
  int sign_dy;
  int sign_dx;
  iptr result_dir;
  iptr dir;
  
  result_dir = 8;
  if (dx < 1) {
    if (dx < 0) {
      dx = -1;
    }
    else {
      dx = 0;
    }
  }
  else {
    dx = 1;
  }
  if (dy < 1) {
    if (dy < 0) {
      dy = -1;
    }
    else {
      dy = 0;
    }
  }
  else {
    dy = 1;
  }
  for (dir = 0; dir < 8; dir++) {
    if ((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] < 1) {
      if ((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] < '\0') {
        sign_dx = -1;
      }
      else {
        sign_dx = 0;
      }
    }
    else {
      sign_dx = 1;
    }
    if (dx == sign_dx) {
      if ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] < 1) {
        if ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] < '\0') {
          sign_dy = -1;
        }
        else {
          sign_dy = 0;
        }
      }
      else {
        sign_dy = 1;
      }
      if (dy == sign_dy) {
        result_dir = dir;
      }
    }
  }
  return result_dir;
}

/* Part of map_path_next_dir (004abfe5): the flood fill of the cost grid from
   the goal outwards (breadth first through the ring queue), up to max_cost.
   civ: the caller's, read stale by the direction pick. */
static void map_path_flood(int x,int y,int max_cost,bool uses_terrain_cost,bool start_on_land,
                                      bool is_naval,int &civ)
{
  int along_river;
  byte cur_tile_flags;
  bool has_road;
  bool has_rail;
  byte terrain;
  int y4;
  int check;
  iptr unit;
  uint improvements2;
  byte *tile;
  int nb_x;
  int y3;
  iptr x3;
  int cost;
  int y2;
  i32 impr_bits;
  iptr x2;
  int abs_dy;
  int dist_y;
  int dist_x;
  iptr dir;
  int new_cost;

  path_queue.flood_goal_x = path_queue.goal_x;
  path_queue.flood_goal_y = path_queue.goal_y;
  _memset(((i8 *)&dialog_text_font_obj_block.path_cost_grid),0,0x2400);
  path_queue.queue_head = 0;
  path_queue.queue_x[0] = path_queue.goal_x;
  path_queue.queue_y[0] = path_queue.goal_y;
  path_queue.queue_tail = 1;
  path_grid_set_cost(path_queue.goal_x,path_queue.goal_y,1);
  path_queue.result_cost = max_cost;
  do {
    x3 = (((i32 *)&path_queue.queue_x))[path_queue.queue_head];
    y4 = (((i32 *)&path_queue.queue_y))[path_queue.queue_head];
    path_queue.queue_head++;
    if (0x8ff < path_queue.queue_head) {
      path_queue.queue_head = 0;
    }
    y3 = map_wrap_x(x3);
    cost = path_grid_get_cost(x3,y4);
    nb_x = path_queue.result_cost;
    if ((cost <= path_queue.result_cost) && ((y3 != x || (nb_x = cost, y4 != y))))
    {
      improvements2 = tile_improvements(y3,y4);
      if (((improvements2 & TILE_ROAD) == 0) && (nb_x = tile_city_owner(y3,y4), nb_x < 0)) {
        has_road = false;
      }
      else {
        has_road = true;
      }
      tile = (byte *)tile_at(y3,y4);
      cur_tile_flags = *tile;
      improvements2 = tile_improvements(y3,y4);
      if (((improvements2 & TILE_RAILROAD) == 0) && (nb_x = tile_city_owner(y3,y4), nb_x < 0)) {
        has_rail = false;
      }
      else {
        has_rail = true;
      }
      for (dir = 0; nb_x = path_queue.result_cost, dir < 8; dir++) {
        nb_x = (((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x3;
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y4;
        x2 = map_wrap_x(nb_x);
        if (nb_x - path_queue.goal_x < 1) {
          dist_x = ~(nb_x - path_queue.goal_x) + 1;
        }
        else {
          dist_x = nb_x - path_queue.goal_x;
        }
        if (y2 - path_queue.goal_y < 1) {
          dist_y = ~(y2 - path_queue.goal_y) + 1;
        }
        else {
          dist_y = y2 - path_queue.goal_y;
        }
        if (((int)(dist_y + dist_x & 0xfffffffeU) < 0x30) &&
           (check = map_in_bounds(x2,y2), check != 0)) {
          terrain = tile_terrain(x2,y2);
          if (((((terrain == TERRAIN_OCEAN) == is_naval) || ((x == x2 && (y2 == y)))) ||
              ((path_queue.flood_goal_x == x2 && (y2 == path_queue.flood_goal_y)))) &&
             ((!start_on_land ||
              ((check = tile_landmass(x2,y2), check != 0x3f &&
               (landmass_table_block.landmass_recs[check].water_body_size != 1)))))) {
            new_cost = cost;
            if (-1 < pathfind.owner_civ) {
              civ = tile_city_or_unit_owner(x2,y2);
              if ((-1 < civ) && (pathfind.owner_civ != civ)) continue;
              if ((pathfind.owner_civ == civ) &&
                 ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 &&
                   (impr_bits = tile_improvements(x2,y2), ((byte)impr_bits & (TILE_UNIT | TILE_CITY | TILE_FORTRESS)) == 1)) &&
                  (check = tile_adjacent_nonallied_unit(x2,y2,civ), check != 0)))) {
                unit = tile_first_unit(x2,y2);
                check = unit_stack_count_role(unit,(int)(char)unit_types[pathfind.unit_type].role);
                new_cost = cost + check * 0x10;
              }
              check = tile_owner_at_peace(x2,y2,pathfind.owner_civ);
              if ((-1 < check) && ((char)unit_types[pathfind.unit_type].role < 5)) {
                new_cost += (uint)cosmic.road_move_mult * 4;
              }
            }
            if ((pathfind.road_planning != 0) ||
               ((has_rail && ((improvements2 = tile_improvements(x2,y2), (improvements2 & TILE_RAILROAD) != 0 ||
                           (check = tile_city_owner(x2,y2), -1 < check)))))) {
              new_cost++;
            }
            else if ((unit_types[pathfind.unit_type].flags & UTYPE_ALPINE) == 0) {
              if ((has_road) &&
                 ((improvements2 = tile_improvements(x2,y2), (improvements2 & TILE_ROAD) != 0 ||
                  (check = tile_city_owner(x2,y2), -1 < check)))) {
                new_cost += 4;
              }
              else {
                if (pathfind.roads_only != 0) continue;
                along_river = 0;
                if ((((cur_tile_flags & 0x80) != 0) &&
                    (tile = (byte *)tile_at(x2,y2), (*tile & 0x80) != 0)) &&
                   (check = map_dx_wrapped(y3,x2), check == 1)) {
                  if (y4 == y2 || y4 - y2 < 0) {
                    abs_dy = ~(y4 - y2) + 1;
                  }
                  else {
                    abs_dy = y4 - y2;
                  }
                  if (abs_dy == 1) {
                    new_cost += 4;
                    along_river = 1;  /* a diagonal step along a river */
                  }
                }
                if (along_river == 0) {
                  if (uses_terrain_cost) {
                    new_cost = new_cost +
                               (int)(char)terrain_types[terrain].move_cost *
                               (uint)cosmic.road_move_mult * 4;
                  }
                  else {
                    new_cost += (uint)cosmic.road_move_mult * 4;
                  }
                }
              }
            }
            else {
              new_cost += 4;
            }
            if (((unit_types[pathfind.unit_type].flags & UTYPE_COAST_BOUND) != 0) &&
               (tile = (byte *)tile_at(x2,y2), (*tile & 0x40) != 0)) {
              new_cost += (uint)cosmic.road_move_mult * 4;
            }
            check = path_grid_get_cost(nb_x,y2);
            if ((check == 0) || (new_cost < check)) {
              path_grid_set_cost(nb_x,y2,new_cost);
              (((i32 *)&path_queue.queue_x))[path_queue.queue_tail] = nb_x;
              (((i32 *)&path_queue.queue_y))[path_queue.queue_tail] = y2;
              path_queue.queue_tail++;
              if (0x8ff < path_queue.queue_tail) {
                path_queue.queue_tail = 0;
              }
              if (path_queue.queue_head == path_queue.queue_tail) return;  /* the queue is full */
            }
          }
        }
      }
    }
    path_queue.result_cost = nb_x;
  } while ((path_queue.queue_head != path_queue.queue_tail) && (path_queue.queue_head != 0));
}

/* Part of map_path_next_dir (004abfe5): the direction from (x,y) to the
   neighbour of lowest cost to reach (ties: the nearer to the goal).
   civ: the caller's, possibly left by the flood fill (read stale when not
   set here). */
static void map_path_pick_dir(int x,int y,int rel_x,bool uses_terrain_cost,bool is_naval,int &civ,
                                         iptr &best_dir)
{
  int along_river;
  byte cur_tile_flags;
  bool start_on_land;
  bool has_road;
  byte terrain;
  int y4;
  uint improvements2;
  byte *tile;
  int nb_x;
  int y3;
  iptr x3;
  int cost;
  int y2;
  i32 impr_bits;
  iptr x2;
  int abs_dy2;
  int dist_y;
  int dist_x;
  int step_cost;
  int nb_rel_x;
  int best_dist;
  iptr dir;
  int best_cost;

  best_cost = 19999;
  best_dist = 19999;
  improvements2 = tile_improvements(x,y);
  if (((improvements2 & TILE_ROAD) == 0) && (y4 = tile_city_owner(x,y), y4 < 0)) {
    start_on_land = false;
  }
  else {
    start_on_land = true;
  }
  tile = (byte *)tile_at(x,y);
  cur_tile_flags = *tile;
  improvements2 = tile_improvements(x,y);
  if (((improvements2 & TILE_RAILROAD) == 0) && (y4 = tile_city_owner(x,y), y4 < 0)) {
    has_road = false;
  }
  else {
    has_road = true;
  }
  y4 = path_grid_get_cost(rel_x,y);
  for (dir = 0; dir < 8; dir++) {
    nb_x = (((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x;
    y3 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    x3 = map_wrap_x(nb_x);
    if (nb_x - path_queue.goal_x < 1) {
      dist_x = ~(nb_x - path_queue.goal_x) + 1;
    }
    else {
      dist_x = nb_x - path_queue.goal_x;
    }
    if (y3 - path_queue.goal_y < 1) {
      dist_y = ~(y3 - path_queue.goal_y) + 1;
    }
    else {
      dist_y = y3 - path_queue.goal_y;
    }
    if (((int)(dist_y + dist_x & 0xfffffffeU) < 0x30) &&
       (cost = map_in_bounds(x3,y3), cost != 0)) {
      terrain = tile_terrain(x3,y3);
      if ((((terrain == TERRAIN_OCEAN) == is_naval) || ((x == x3 && (y3 == y)))) ||
         ((path_queue.flood_goal_x == x3 && (y3 == path_queue.flood_goal_y)))) {
        nb_rel_x = nb_x - path_queue.grid_center_x;
        if ((game.rules & RULE_FLAT_WORLD) == 0) {
          if (nb_rel_x < 0) {
            nb_rel_x += map.width;
          }
          if ((int)map.width >> 1 <= nb_rel_x) {
            nb_rel_x -= map.width;
          }
        }
        cost = path_grid_get_cost(nb_rel_x + path_queue.grid_center_x,y3);
        if (((((cost == 0) || (pathfind.owner_civ < 0)) ||
             (civ = tile_city_or_unit_owner(x3,y3), civ < 0)) ||
            (pathfind.owner_civ == civ)) && ((cost != 0 && (cost < y4)))) {
          if (pathfind.road_planning == 0) {
            if ((has_road) &&
               ((improvements2 = tile_improvements(x3,y3), (improvements2 & TILE_RAILROAD) != 0 ||
                (y2 = tile_city_owner(x3,y3), -1 < y2)))) {
              step_cost = 1;
            }
            else if ((unit_types[pathfind.unit_type].flags & UTYPE_ALPINE) == 0) {
              if ((start_on_land) &&
                 ((improvements2 = tile_improvements(x3,y3), (improvements2 & TILE_ROAD) != 0 ||
                  (y2 = tile_city_owner(x3,y3), -1 < y2)))) {
                step_cost = 4;
              }
              else {
                if (pathfind.roads_only != 0) continue;
                along_river = 0;
                if ((((cur_tile_flags & 0x80) != 0) &&
                    (tile = (byte *)tile_at(x3,y3), (*tile & 0x80) != 0)) &&
                   (y2 = map_dx_wrapped(x,x3), y2 == 1)) {
                  if (y == y3 || y - y3 < 0) {
                    abs_dy2 = ~(y - y3) + 1;
                  }
                  else {
                    abs_dy2 = y - y3;
                  }
                  if (abs_dy2 == 1) {
                    step_cost = 4;
                    along_river = 1;  /* a diagonal step along a river */
                  }
                }
                if (along_river == 0) {
                  if (uses_terrain_cost) {
                    step_cost = (int)(char)terrain_types[terrain].move_cost *
                               (uint)cosmic.road_move_mult * 4;
                  }
                  else {
                    step_cost = (uint)cosmic.road_move_mult << 2;
                  }
                }
              }
            }
            else {
              step_cost = 4;
            }
            if (((unit_types[pathfind.unit_type].flags & UTYPE_COAST_BOUND) != 0) &&
               (tile = (byte *)tile_at(x3,y3), (*tile & 0x40) != 0)) {
              step_cost += (uint)cosmic.road_move_mult * 4;
            }
            if (((pathfind.owner_civ == civ) &&
                ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
               ((impr_bits = tile_improvements(x3,y3), ((byte)impr_bits & (TILE_UNIT | TILE_CITY | TILE_FORTRESS)) == 1 &&
                (y2 = tile_adjacent_nonallied_unit(x3,y3,civ), y2 != 0)))) {
              x2 = tile_first_unit(x3,y3);
              y2 = unit_stack_count_role(x2,(int)(char)unit_types[pathfind.unit_type].role);
              step_cost += y2 * 8;
            }
            y2 = tile_owner_at_peace(x3,y3,pathfind.owner_civ);
            if ((-1 < y2) && ((char)unit_types[pathfind.unit_type].role < 5)) {
              step_cost += (uint)cosmic.road_move_mult * 4;
            }
          }
          else {
            step_cost = 1;
          }
          cost += step_cost;
          if (cost <= best_cost) {
            nb_x = map_distance_diag(path_queue.goal_x,path_queue.goal_y,nb_x,y3);
            if (cost < best_cost) {
              best_dir = dir;
              best_dist = nb_x;
              best_cost = cost;
            }
            else if (nb_x < best_dist) {
              best_dir = dir;
              best_dist = nb_x;
            }
          }
        }
      }
    }
  }
}

/* Part of map_path_next_dir (004abfe5): the move debug view: the cost grid drawn
   over the map until the debug popup is closed (1/2 zoom, 3/4 recentre). */
static void map_path_debug_show(int x,int y,int debug_mode)
{
  int col;
  int row;

  map_views_block.map_view_center_x[0] = (i16)x;
  map_views_block.mapview_center_y[0] = (i16)y;
  while( true ) {
    while( true ) {
      while( true ) {
        while( true ) {
          ((MapView *)((i8 *)&map_views_block.map_views))->redraw(-1,1);
          for (row = 0; row < 0x30; row++) {
            for (col = 0; col < 0x30; col++) {
              if (*(int *)(((i8 *)&dialog_text_font_obj_block.path_cost_grid) + col * 4 + row * 0xc0) != 0) {
                mapview_draw_tile_number(col - 0x18 + row - 0x18 + path_queue.grid_center_x,
                                   ((col - 0x18) - (row - 0x18)) + path_queue.grid_center_y,
                                   *(i32 *)(((i8 *)&dialog_text_font_obj_block.path_cost_grid) + col * 4 + row * 0xc0)
                                   ,0x29);
              }
            }
          }
          debug_mode = popup_debug_text((const char *)&str_movedebug);
          if (debug_mode != 1) break;
          map_views_block.mapview_zoom[0]--;
        }
        if (debug_mode != 2) break;
        map_views_block.mapview_zoom[0]++;
      }
      if (debug_mode != 3) break;
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on(x,y);
    }
    if (debug_mode != 4) break;
    ((MapView *)((i8 *)&map_views_block.map_views))->center_on(path_queue.goal_x,path_queue.goal_y);
  }
}

/* 004abfe5 auto */
iptr __cdecl map_path_next_dir(int x,int y,int max_cost)

{
  bool uses_terrain_cost;
  bool start_on_land;
  int y4;
  int debug_mode;
  bool is_naval;
  int civ;
  iptr best_dir;
  int rel_x;
  civ = 0;  /* stale stack the original reads (FRAME_PRESETS) */
  
  debug_mode = pathfind.debug_pending;
  path_queue.grid_center_x = path_queue.goal_x;
  path_queue.grid_center_y = path_queue.goal_y;
  is_naval = unit_types[pathfind.unit_type].domain == DOMAIN_SEA;
  uses_terrain_cost = (int)(uint)cosmic.road_move_mult < (int)(char)unit_types[pathfind.unit_type].move;
  if ((is_naval) && (y4 = tile_is_ocean(x,y), y4 == 0)) {
    start_on_land = true;
  }
  else {
    start_on_land = false;
  }
  path_queue.result_cost = 0;
  rel_x = x - path_queue.grid_center_x;
  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    if (rel_x < 0) {
      rel_x += map.width;
    }
    if ((int)map.width >> 1 <= rel_x) {
      rel_x -= map.width;
    }
  }
  rel_x += path_queue.grid_center_x;
  if (((path_queue.goal_x != path_queue.flood_goal_x) || (path_queue.goal_y != path_queue.flood_goal_y)) ||
     (y4 = path_grid_get_cost(rel_x,y), y4 == 0)) {
    map_path_flood(x,y,max_cost,uses_terrain_cost,start_on_land,is_naval,civ);
  }
  best_dir = -1;
  if (path_queue.result_cost < max_cost) {
    map_path_pick_dir(x,y,rel_x,uses_terrain_cost,is_naval,civ,best_dir);
  }
  if (best_dir < 0) {
    path_queue.goal_x = path_queue.flood_goal_x;
    path_queue.goal_y = path_queue.flood_goal_y;
  }
  if (debug_mode != 0) {
    map_path_debug_show(x,y,debug_mode);
  }
  return best_dir;
}

/* 004ad0d1 auto */
int __cdecl
map_path_length(int x,int y,int goal_x,int goal_y,int by_sea,i32 max_cost)

{
  i32 saved_owner;
  int result;
  int dy;
  int dx;
  
  saved_owner = pathfind.owner_civ;
  if (x == goal_x || x - goal_x < 0) {
    dx = ~(x - goal_x) + 1;
  }
  else {
    dx = x - goal_x;
  }
  if (((game.rules & RULE_FLAT_WORLD) == 0) && ((int)map.width >> 1 < dx)) {
    dx = map.width - dx;
  }
  if (y == goal_y || y - goal_y < 0) {
    dy = ~(y - goal_y) + 1;
  }
  else {
    dy = y - goal_y;
  }
  result = dy + dx >> 1;
  if (result == 0) {
    result = 0;
  }
  else if (result < 0x17) {
    if (by_sea == 0) {
      pathfind.unit_type = 2;
    }
    else {
      pathfind.unit_type = 0x21;
    }
    path_queue.goal_x = goal_x;
    path_queue.goal_y = goal_y;
    pathfind.owner_civ = 0xffffffff;
    result = map_path_next_dir(x,y,max_cost);
    if (-1 < result) {
      result = path_queue.result_cost;
    }
  }
  else {
    result = -1;
  }
  pathfind.owner_civ = saved_owner;
  return result;
}

/* 004ad784 auto */
i32 __cdecl map_diag_tile_by_ocean(int base_x,int base_y,iptr out_x,iptr out_y,int want_ocean)

{
  iptr x;
  int y;
  int check;
  int i;
  
  i = 0;
  while( true ) {
    if (1 < i) {
      return 0;
    }
    x = map_wrap_x(base_x + i);
    y = base_y + i;
    check = map_in_bounds(x,y);
    if ((check != 0) && (check = tile_is_ocean(x,y), check == want_ocean)) break;
    i++;
  }
  *(iptr *)out_x = x;
  *(int *)out_y = y;
  return 1;
}

/* 004aee90 auto */
u8 * __cdecl map_coarse0_at(int x,int y)

{
  return map_data.coarse[0] + map.quarter_w * y + x;
}

/* 004aeec0 auto */
u8 * __cdecl map_coarse1_at(int x,int y)

{
  return map_data.coarse[1] + map.quarter_w * y + x;
}

/* 004aeef0 auto */
u8 * __cdecl map_coarse2_at(int x,int y)

{
  return map_data.coarse[2] + map.quarter_w * y + x;
}

/* 004b3110 auto */
void __cdecl landmass_set_border_bit(int landmass,iptr bit)

{
  iptr byte_idx;
  byte bit_mask[4];
  
  bit_index_split(bit,(iptr)&byte_idx,(iptr)bit_mask);
  landmass_table_block.landmass_recs[landmass].border_bits[byte_idx] = landmass_table_block.landmass_recs[landmass].border_bits[byte_idx] | bit_mask[0]
  ;
  return;
}

/* 004b315c auto */
void __cdecl landmass_compute_borders(void)

{
  int y;
  i32 landmass;
  iptr x;
  int check;
  i32 ocean_id;
  int landmass_idx;
  uint y2;
  uint x2;
  uptr dir;
  
  for (landmass_idx = 0; landmass_idx < 0x40; landmass_idx++) {
    for (dir = 0; (int)dir < 8; dir++) {
      landmass_table_block.landmass_recs[landmass_idx].border_bits[dir] = 0;
    }
  }
  for (y2 = 0; (int)y2 < (int)map.height; y2++) {
    for (x2 = (uint)((y2 & 1) != 0); (int)x2 < (int)map.width;
        x2 = x2 + 2) {
      y = tile_is_ocean(x2,y2);
      if (y == 0) {
        landmass = tile_landmass(x2,y2);
        for (dir = 0; (int)dir < 8; dir++) {
          x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
          y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y2;
          check = map_in_bounds(x,y);
          if ((check != 0) && (check = tile_is_ocean(x,y), check != 0)) {
            ocean_id = tile_landmass(x,y);
            landmass_set_border_bit(landmass,ocean_id);
            dir += (2 - (dir & 1));
          }
        }
      }
    }
    ui_pump_messages_net();
  }
  return;
}

/* 004b32fe auto */
uint __cdecl map_assign_landmass_ids(void)

{
  int *count_ptr;
  char dy;
  bool run_active;
  iptr counts_handle;
  i32 *_Dst;
  ushort *_Dst_00;
  int check;
  uptr dir;
  uint nb_id;
  uint col_x;
  uint cur_id;
  int i;
  iptr ids_handle;
  uint overflow_mask;
  uint y2;
  u8 *tile_landmass2;
  uint x2;
  int merged_count;
  ushort *id_ptr;
  int j;
  uint row_y;
  int ocean_pass;
  uint next_id2;
  int idx;
  uint merged_id;
  
  overflow_mask = 0;
  ids_handle = 0;
  for (i = 0; i < 0x40; i++) {
    landmass_table_block.landmass_recs[i].tiles = 0;
    landmass_table_block.landmass_recs[i].water_body_size = 0;
  }
  counts_handle = mem_handle_alloc_tracked(0x20000);
  _Dst = (i32 *)mem_lock_handle(counts_handle);
  if (_Dst == (void *)0x0) {
    error_report(0xfffffff7,8,0,1,0);
  }
  else {
    ids_handle = mem_handle_alloc_tracked((int)map.area << 2);
    _Dst_00 = (ushort *)mem_lock_handle(ids_handle);
    if (_Dst_00 == (ushort *)0x0) {
      error_report(0xfffffff7,8,0,2,0);
    }
    else {
      map_fill_tile_byte((iptr)&map_data.tiles->landmass,0);
      for (ocean_pass = 1; -1 < ocean_pass; ocean_pass--) {
        _memset(_Dst,0,0x20000);
        _memset(_Dst_00,0,(int)map.area << 2);
        next_id2 = 0;
        cur_id = 0;
        run_active = false;
        for (y2 = 0; (int)y2 < (int)map.height; y2++) {
          if ((game.rules & RULE_FLAT_WORLD) != 0) {
            run_active = false;
            cur_id = 0;
          }
          for (x2 = (uint)((y2 & 1) != 0); (int)x2 < (int)map.width;
              x2 = x2 + 2) {
            check = tile_is_ocean(x2,y2);
            if (check == ocean_pass) {
              if (y2 != 0) {
                for (idx = 0; idx < 3; idx++) {
                  dir = idx - 2U & 7;
                  nb_id = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
                  dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
                  check = map_in_bounds(nb_id,(int)dy + y2);
                  if ((check != 0) &&
                     (nb_id = (uint)*(ushort *)
                                     (((int)map.width & 0xfffffffeU) * ((int)dy + y2) +
                                      (nb_id & 0xfffffffe) + (int)_Dst_00), nb_id != 0)) {
                    merged_id = nb_id;
                    if ((cur_id != 0) && (nb_id != cur_id)) {
                      if ((int)cur_id <= (int)nb_id) {
                        merged_id = cur_id;
                      }
                      if ((int)nb_id <= (int)cur_id) {
                        nb_id = cur_id;
                      }
                      count_ptr = &_Dst[merged_id];
                      *count_ptr = *count_ptr + _Dst[nb_id];
                      _Dst[nb_id] = 0;
                      id_ptr = _Dst_00;
                      for (row_y = 0; (int)row_y <= (int)y2; row_y++) {
                        for (col_x = (uint)((row_y & 1) != 0); (int)col_x < (int)map.width;
                            col_x = col_x + 2) {
                          if ((uint)*id_ptr == (nb_id & 0xffff)) {
                            *id_ptr = (ushort)merged_id;
                          }
                          id_ptr++;
                        }
                      }
                    }
                    cur_id = merged_id;
                  }
                }
              }
              if (cur_id == 0) {
                if (!run_active) {
                  next_id2 = 0;
                  do {
                    next_id2++;
                    if (0x7ffe < (int)next_id2) break;
                  } while (_Dst[next_id2] != 0);
                }
                cur_id = next_id2;
              }
              *(short *)(((int)map.width & 0xfffffffeU) * y2 + (x2 & 0xfffffffe) +
                        (int)_Dst_00) = (short)cur_id;
              count_ptr = &_Dst[cur_id];
              *count_ptr = *count_ptr + 1;
              run_active = true;
            }
            else {
              run_active = false;
              cur_id = 0;
            }
          }
          ui_pump_messages_net();
        }
        if (ocean_pass != 0) {
          merged_count = 0;
          for (i = 0; i < 0x3f; i++) {
            if (_Dst[i] < 9) {
              _Dst[0x3f] =
                   _Dst[0x3f] + _Dst[i];
              _Dst[i] = 0xffffffff;
              merged_count++;
            }
          }
          if (merged_count != 0) {
            id_ptr = _Dst_00;
            for (idx = 0; idx < map.area; idx++) {
              if (_Dst[(uint)*id_ptr] < 0) {
                *id_ptr = 0x3f;
              }
              id_ptr++;
            }
            for (i = 0; i < 0x3f; i++) {
              if (_Dst[i] < 0) {
                _Dst[i] = 0;
              }
            }
          }
        }
        tile_landmass2 = &map_data.tiles->landmass;
        id_ptr = _Dst_00;
        for (idx = 0; idx < map.area; idx++) {
          if (*id_ptr != 0) {
            if (0x3f < *id_ptr) {
              if (_Dst[(uint)*id_ptr] < 1) {
                if (_Dst[(uint)*id_ptr] < 1) {
                  *id_ptr = ~(ushort)_Dst[(uint)*id_ptr] + 1;
                }
                else {
                  *id_ptr = (ushort)_Dst[(uint)*id_ptr];
                }
              }
              else {
                next_id2 = 0;
                do {
                  next_id2++;
                } while (_Dst[next_id2] != 0);
                if (((int)next_id2 < 0x40) &&
                   ((ocean_pass == 0 || (0x10 < _Dst[(uint)*id_ptr])))) {
                  _Dst[next_id2] =
                       _Dst[(uint)*id_ptr];
                  _Dst[(uint)*id_ptr] = -next_id2;
                  *id_ptr = (ushort)next_id2;
                }
                else {
                  overflow_mask = overflow_mask | 1 << ((byte)ocean_pass & 0x1f);
                  *id_ptr = 0x3f;
                }
              }
            }
            *tile_landmass2 = (u8)*id_ptr;
          }
          id_ptr++;
          tile_landmass2 += 6;
          ui_pump_messages_net();
        }
        for (j = 0; j < 0x40; j++) {
          if (ocean_pass == 0) {
            landmass_table_block.landmass_recs[j].tiles =
                 (short)_Dst[j];
          }
          else {
            landmass_table_block.landmass_recs[j].water_body_size =
                 (short)_Dst[j];
          }
        }
      }
      landmass_compute_borders();
    }
  }
  mem_unlock_handle(ids_handle);
  mem_free_handle(ids_handle);
  mem_unlock_handle(counts_handle);
  mem_free_handle(counts_handle);
  return overflow_mask;
}

/* 00519200 auto */
void __cdecl terrain_editor_load_rules(void)

{
  char *_Source;
  size_t _Count;
  iptr i;
  iptr terrain2;
  
  for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
    _Count = 0x28;
    _Source = label_string(terrain_types[terrain2].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[terrain2],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[terrain2][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[0] = (int)(char)terrain_types[terrain2].move_cost;
    editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[1] = (int)(char)terrain_types[terrain2].defense;
    for (i = 0; i < 3; i++) {
      editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[2 + i] =
           (int)(char)terrain_types[terrain2].yields[i];
    }
    if (terrain2 < 0xb) {
      for (i = 0; i < 2; i++) {
        editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[5 + i * 4] =
             (int)terrain_types[terrain2].improve_to[i];
        editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[6 + i * 4] =
             (int)(char)terrain_types[terrain2].improve_bonus[i];
        editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[7 + i * 4] =
             (int)(char)terrain_types[terrain2].improve_turns[i];
        editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[8 + i * 4] =
             (int)(char)terrain_types[terrain2].improve_ai_govt[i];
      }
      editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[13] = (int)terrain_types[terrain2].transform_to;
    }
  }
  return;
}

/* 005193ed auto */
void __cdecl terrain_editor_store_rules(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr i;
  iptr terrain2;
  
  for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
    _Count = 0xf;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[terrain2];
    _Dest = label_string(terrain_types[terrain2].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    terrain_types[terrain2].move_cost = (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[0];
    terrain_types[terrain2].defense = (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[1];
    for (i = 0; i < 3; i++) {
      terrain_types[terrain2].yields[i] =
           (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[2 + i];
    }
    if (terrain2 < 0xb) {
      for (i = 0; i < 2; i++) {
        terrain_types[terrain2].improve_to[i] =
             (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[5 + i * 4];
        terrain_types[terrain2].improve_bonus[i] =
             (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[6 + i * 4];
        terrain_types[terrain2].improve_turns[i] =
             (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[7 + i * 4];
        terrain_types[terrain2].improve_ai_govt[i] =
             (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[8 + i * 4];
      }
      terrain_types[terrain2].transform_to = (i8)editor_text_dlg_wnd_block.editor_item_recs[terrain2].values[13];
    }
  }
  return;
}

/* 005195c4 auto */
int __cdecl terrain_editor_terrain_to_sel(int terrain)

{
  if (0x17 < terrain) {
    terrain--;
  }
  if (0xc < terrain) {
    terrain--;
  }
  return terrain;
}

/* 005195f1 auto */
int __cdecl terrain_editor_sel_to_terrain(int sel)

{
  if (0xc < sel) {
    sel++;
  }
  if (0x17 < sel) {
    sel++;
  }
  return sel;
}

/* 0051961e auto */
void __cdecl terrain_editor_load_controls(void)

{
  int ctrl_id;
  int field;
  char text[8];
  int sel;
  
  for (field = 1; field < 0xf; field++) {
    if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->get_id();
      _sprintf(text,((i8 *)&terrain_editor_ctrl_kinds_block.terrain_editor_int_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->set_text(
                 (const char *)text);
    }
    else if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->get_id();
      sel = *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58);
      if (field == 0) {
        sel = terrain_editor_terrain_to_sel(sel);
      }
      if ((field == 6) || (field == 10)) {
        sel += 2;
      }
      if (field == 0xe) {
        sel++;
      }
      editor_active.wnd->combos[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot].set_cur_sel(sel);
    }
  }
  return;
}

/* 005197af auto */
int __cdecl terrain_editor_store_controls(void)

{
  int ctrl_id;
  i32 clamped;
  int field;
  int err_count;
  char text[8];
  int value;
  
  err_count = 0;
  for (field = 1; field < 0xf; field++) {
    if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->Control::get_text(
                 (char *)text);
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->get_id();
      ctrl_id -= 0xca;
      value = _atoi(text);
      clamped = int_clamp(value,*(i32 *)(((i8 *)&terrain_editor_ctrl_kinds_block.terrain_editor_field_min_table) + ctrl_id * 4),
                                 *(i32 *)(((i8 *)&terrain_editor_ctrl_kinds_block.terrain_editor_field_max_table) + ctrl_id * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[ctrl_id] = clamped;
      if (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[ctrl_id] != value) {
        err_count++;
      }
    }
    else if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].kind == 0xc) {
      value = editor_active.wnd->combos[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot].get_cur_sel();
      if (field == 0) {
        value = terrain_editor_sel_to_terrain(value);
      }
      if ((field == 6) || (field == 10)) {
        value -= 2;
      }
      if (field == 0xe) {
        value--;
      }
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[field].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + ctrl_id * 4) = value;
    }
  }
  return err_count;
}

/* 005199a9 auto */
void __cdecl terrain_editor_redraw(void)

{
  editor_active.wnd->terrain_editor_draw();
  return;
}

/* 005199c4 auto */
void __cdecl terrain_editor_append_rule_ref(iptr terrain,int with_comma)

{
  if (terrain == -1) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_ref_no_strs) + ((with_comma != 0) - 1 & 8));
  }
  else if (terrain == -2) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_ref_yes_strs) + ((with_comma != 0) - 1 & 8));
  }
  else if (terrain < 0xb) {
    crt_strcat(((i8 *)&textbuf.buf),(const char *)(terrain_types + terrain));
    if (with_comma != 0) {
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_ref_comma_str));
    }
  }
  else {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_ref_err_strs) + ((with_comma != 0) - 1 & 8));
  }
  return;
}

/* 00519ab0 auto */
i32 __cdecl terrain_editor_write_section(FILE *file)

{
  char *name2;
  size_t len;
  size_t name_len;
  iptr terrain2;
  iptr i;
  
  for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
    text_clear();
    text_append_string_id(terrain_types[terrain2].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_name_sep));
    name2 = label_string(terrain_types[terrain2].name);
    len = _strlen(name2);
    if (len < 0xb) {
      name2 = label_string(terrain_types[terrain2].name);
      name_len = _strlen(name2);
    }
    else {
      name_len = 0xb;
    }
    text_append_spaces(0xb - name_len);
    text_append_int((int)(char)terrain_types[terrain2].move_cost);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_move_sep));
    text_append_int((int)(char)terrain_types[terrain2].defense);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_defense_sep));
    text_append_spaces(2);
    for (i = 0; i < 3; i++) {
      text_append_int((int)(char)terrain_types[terrain2].yields[i]);
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_yield_sep));
    }
    if (terrain2 < 0xb) {
      for (i = 0; i < 2; i++) {
        text_append_spaces(3);
        terrain_editor_append_rule_ref((int)terrain_types[terrain2].improve_to[i],1);
        text_append_int_2d((int)(char)terrain_types[terrain2].improve_bonus[i]);
        crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_bonus_sep));
        text_append_int_2d((int)(char)terrain_types[terrain2].improve_turns[i]);
        crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_turns_sep));
        if (i == 1) {
          text_append_space();
        }
        text_append_int_2d((int)(char)terrain_types[terrain2].improve_ai_govt[i]);
        crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_editor_ctrl_kinds_block.terrain_txt_ai_gov_sep));
      }
      text_append_spaces(2);
      terrain_editor_append_rule_ref((int)terrain_types[terrain2].transform_to,1);
      crt_strcat(((i8 *)&textbuf.buf),str_empty_161);
      terrain_editor_append_rule_ref(terrain2,0);
    }
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&terrain_txt_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 00519d67 auto */
void __cdecl terrain_editor_on_ok(void)

{
  iptr errors;
  HWND hWnd;
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg[32];
  
  errors = terrain_editor_store_controls();
  if (errors == 0) {
    terrain_editor_store_rules();
    rules_edit_set_section((iptr)&str_terrain_2,(RulesSectionWriterFn)terrain_editor_write_section);
    ok = rules_write_sections();
    if (ok == 0) {
      _sprintf(msg,str_error_updating_rules_s_4,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error_4;
      lpText = msg;
      wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    terrain_editor_load_controls();
    terrain_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_12);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 00519e74 auto */
void __cdecl terrain_editor_on_rename(void)

{
  size_t len;
  iptr label;
  iptr parent_wnd2;
  char old_name[15];
  char new_name[260];
  iptr sel4;
  int terrain2;
  iptr sel3;
  iptr sel2;
  iptr sel1;
  int result;
  
  terrain2 = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[terrain2],0xf);
  old_name[14] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_terrname,0xe,(iptr)old_name,(iptr)new_name);
    dlg_set_parent_window(0);
    if (result == -1) break;
    len = _strlen(new_name);
  } while (len == 0);
  if (-1 < result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[terrain2],new_name);
    sel1 = editor_active.wnd->combos[0].get_cur_sel();
    sel2 = editor_active.wnd->combos[1].get_cur_sel();
    sel3 = editor_active.wnd->combos[3].get_cur_sel();
    sel4 = editor_active.wnd->combos[5].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    editor_active.wnd->combos[1].clear();
    editor_active.wnd->combos[3].clear();
    editor_active.wnd->combos[5].clear();
    label = (iptr)label_string(label_table.string_ids[0x1ef]);
    editor_active.wnd->combos[1].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1ef]);
    editor_active.wnd->combos[3].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    editor_active.wnd->combos[1].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    editor_active.wnd->combos[3].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    editor_active.wnd->combos[5].add_string((const char *)label);
    for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
      if ((terrain2 != 0xd) && (terrain2 != 0x18)) {
        editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[terrain2]);
      }
    }
    for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
      editor_active.wnd->combos[1].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[terrain2]);
      editor_active.wnd->combos[3].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[terrain2]);
      editor_active.wnd->combos[5].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[terrain2]);
    }
    editor_active.wnd->combos[0].set_cur_sel(sel1);
    editor_active.wnd->combos[1].set_cur_sel(sel2);
    editor_active.wnd->combos[3].set_cur_sel(sel3);
    editor_active.wnd->combos[5].set_cur_sel(sel4);
    terrain_editor_redraw();
  }
  return;
}

/* 0051a1e5 auto */
void __cdecl terrain_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_5,(const char *)&str_terrain_3);
  dlg_set_parent_window(0);
  return;
}

/* 0051a244 auto */
void __cdecl terrain_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 0051a26c auto */
void __cdecl terrain_editor_on_edit_icon(void)

{
  int choice;
  i32 title;
  iptr parent_wnd3;
  iptr parent_wnd2;
  iptr parent_wnd1;
  i8 *sprites;
  i32 sheet_id;
  SpriteSheetSaveFn save_fn;
  
  sprites = (i8 *)0x0;
  save_fn = sprite_sheet_save_terrain1;
  sheet_id = 7;
  if (editor_active.wnd->item == 3) {
    if (editor_active.wnd == 0) {
      parent_wnd1 = 0;
    }
    else {
      parent_wnd1 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd1);
    while( true ) {
      dlg_input.help_pressed = 0;
      choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_forest_2,1);
      if (dlg_input.help_pressed == 0) break;
      popup_text((const char *)&str_help_6,(const char *)&str_helpforest);
    }
    /* BF-117 (Peter Johnson's ledger): a choice past the 16 sprites (a row
       added to a scenario's DEBUG.TXT) is no choice */
    if ((choice < -1) || (16 < choice)) {
      choice = -1;
    }
    if (choice != -1) {
      if (choice == 0) {
        sprites = ((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + editor_active.wnd->item * 0x3c;
      }
      else {
        sprites = ((i8 *)&forest_sprites) + (choice * 4 - 4) * 0xf;
      }
    }
    if (0 < choice) {
      save_fn = sprite_sheet_save_terrain2;
      sheet_id = 8;
    }
    dlg_set_parent_window(0);
  }
  else if (editor_active.wnd->item == 4) {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    while( true ) {
      dlg_input.help_pressed = 0;
      choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_hills_2,1);
      if (dlg_input.help_pressed == 0) break;
      popup_text((const char *)&str_help_7,(const char *)&str_helphills);
    }
    /* BF-117 */
    if ((choice < -1) || (16 < choice)) {
      choice = -1;
    }
    if (choice != -1) {
      if (choice == 0) {
        sprites = ((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + editor_active.wnd->item * 0x3c;
      }
      else {
        sprites = ((i8 *)&hill_sprites_block.hill_sprites) + (choice * 4 - 4) * 0xf;
      }
    }
    if (0 < choice) {
      save_fn = sprite_sheet_save_terrain2;
      sheet_id = 8;
    }
    dlg_set_parent_window(0);
  }
  else if (editor_active.wnd->item == 5) {
    if (editor_active.wnd == 0) {
      parent_wnd3 = 0;
    }
    else {
      parent_wnd3 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd3);
    while( true ) {
      dlg_input.help_pressed = 0;
      choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_mountains_2,1);
      if (dlg_input.help_pressed == 0) break;
      popup_text((const char *)&str_help_8,(const char *)&str_helpmountains);
    }
    /* BF-117 */
    if ((choice < -1) || (16 < choice)) {
      choice = -1;
    }
    if (choice != -1) {
      if (choice == 0) {
        sprites = ((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + editor_active.wnd->item * 0x3c;
      }
      else {
        sprites = ((i8 *)&mountain_sprites) + (choice * 4 - 4) * 0xf;
      }
    }
    if (0 < choice) {
      save_fn = sprite_sheet_save_terrain2;
      sheet_id = 8;
    }
    dlg_set_parent_window(0);
  }
  else if (editor_active.wnd->item < 0xb) {
    sprites = ((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + editor_active.wnd->item * 0x3c;
  }
  else if (editor_active.wnd->item < 0x16) {
    sprites = ((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + (editor_active.wnd->item % 0xb) * 0x78;
  }
  else {
    sprites = ((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + (editor_active.wnd->item % 0xb) * 0x78;
  }
  if (sprites != (i8 *)0x0) {
    title = (iptr)label_string(label_table.string_ids[0x1f6]);
    sprite_editor_run((iptr)sprites,(const char *)title,sheet_id,save_fn);
  }
  terrain_editor_redraw();
  return;
}

/* 0051a678 auto */
void __cdecl terrain_editor_on_edit_river(void)

{
  int choice;
  i32 title;
  i32 sheet_id;
  SpriteSheetSaveFn save_fn;
  iptr parent_wnd2;
  i8 *sprites;
  
  sprites = (i8 *)0x0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  while( true ) {
    dlg_input.help_pressed = 0;
    choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_river,1);
    if (dlg_input.help_pressed == 0) break;
    popup_text((const char *)&str_help_9,(const char *)&str_helpriver);
  }
  /* BF-072 (Peter Johnson's ledger): a choice past the 16 rivers and 4 mouths (a row
     added to a scenario's DEBUG.TXT) is no choice */
  if ((choice < -1) || (19 < choice)) {
    choice = -1;
  }
  if (choice != -1) {
    if (choice < 0x10) {
      sprites = ((i8 *)&icon_misc_sprites_block.river_sprites) + choice * 0x3c;
    }
    else {
      sprites = ((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + (choice * 4 - 0x40) * 0xf;
    }
  }
  dlg_set_parent_window(0);
  if (sprites != (i8 *)0x0) {
    save_fn = sprite_sheet_save_terrain2;
    sheet_id = 8;
    title = (iptr)label_string(label_table.string_ids[0x1f6]);
    sprite_editor_run((iptr)sprites,(const char *)title,sheet_id,save_fn);
  }
  terrain_editor_redraw();
  return;
}

/* 0051a797 auto */
void __cdecl terrain_editor_on_edit_coast(void)

{
  uint choice;
  i32 title;
  uint sign;
  i32 sheet_id;
  SpriteSheetSaveFn save_fn;
  iptr parent_wnd2;
  i8 *sprites;
  
  sprites = (i8 *)0x0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  while( true ) {
    dlg_input.help_pressed = 0;
    choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_coast,1);
    if (dlg_input.help_pressed == 0) break;
    popup_text((const char *)&str_help_10,(const char *)&str_helpcoast);
  }
  /* BF-118 (Peter Johnson's ledger): a choice past the 32 coast quarters (and the sprite after them, as the original) (a row
     added to a scenario's DEBUG.TXT) is no choice */
  if (((int)choice < -1) || (32 < (int)choice)) {
    choice = 0xffffffff;
  }
  if (choice != 0xffffffff) {
    sign = (int)choice >> 0x1f;
    sprites = ((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) +
               (((choice ^ sign) - sign & 3 ^ sign) - sign) * 0x3c +
               ((int)(choice + (sign & 3)) >> 2) * 0xf0;
  }
  dlg_set_parent_window(0);
  if (sprites != (i8 *)0x0) {
    save_fn = sprite_sheet_save_terrain2;
    sheet_id = 9;
    title = (iptr)label_string(label_table.string_ids[0x1f6]);
    sprite_editor_run((iptr)sprites,(const char *)title,sheet_id,save_fn);
  }
  terrain_editor_redraw();
  return;
}

/* 0051a8b7 auto */
void __cdecl terrain_editor_on_edit_misc(void)

{
  int choice;
  i32 title;
  i32 sheet_id;
  SpriteSheetSaveFn save_fn;
  iptr parent_wnd2;
  i8 *sprites;
  
  sprites = (i8 *)0x0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  while( true ) {
    dlg_input.help_pressed = 0;
    choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_terrmisc,1);
    if (dlg_input.help_pressed == 0) break;
    popup_text((const char *)&str_help_11,(const char *)&str_helpterrmisc);
  }
  /* BF-116 (Peter Johnson's ledger): a choice past the 24 sprites the section lists (a row
     added to a scenario's DEBUG.TXT) is no choice */
  if ((choice < -1) || (23 < choice)) {
    choice = -1;
  }
  if (choice != -1) {
    if (choice < 3) {
      sprites = ((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + choice * 0x3c;
    }
    else if (choice == 0xc) {
      sprites = ((i8 *)&pollution_sprite_block.pollution_sprite);
    }
    else if (choice == 0xd) {
      sprites = ((i8 *)&grassland_shield_sprite);
    }
    else if (choice == 0xe) {
      sprites = ((i8 *)&hut_sprite);
    }
    else if (2 < choice % 0xc) {
      sprites = ((i8 *)&road_sprites_block.road_sprites) + ((choice % 0xc) * 4 - 0xc) * 0xf + (choice / 0xc) * 0x21c;
    }
  }
  dlg_set_parent_window(0);
  if (sprites != (i8 *)0x0) {
    save_fn = sprite_sheet_save_terrain1;
    sheet_id = 10;
    title = (iptr)label_string(label_table.string_ids[0x1f6]);
    sprite_editor_run((iptr)sprites,(const char *)title,sheet_id,save_fn);
  }
  terrain_editor_redraw();
  return;
}

/* 0051aa4e auto */
void __cdecl terrain_editor_update_visibility(void)

{
  int sel;
  int i;
  
  if (editor_active.wnd->item < 0xb) {
    ((Control *)&editor_active.wnd->combos[1])->show_and_redraw();
    sel = editor_active.wnd->combos[1].get_cur_sel();
    if (sel == 0) {
      ((Control *)&editor_active.wnd->edits[5])->show_and_redraw();
      ((Control *)&editor_active.wnd->edits[6])->show_and_redraw();
      ((Control *)&editor_active.wnd->combos[2])->show_and_redraw();
    }
    else if (sel == 1) {
      ((Control *)&editor_active.wnd->edits[5])->hide();
      ((Control *)&editor_active.wnd->edits[6])->hide();
      ((Control *)&editor_active.wnd->combos[2])->hide();
    }
    else {
      ((Control *)&editor_active.wnd->edits[5])->hide();
      ((Control *)&editor_active.wnd->edits[6])->show_and_redraw();
      ((Control *)&editor_active.wnd->combos[2])->show_and_redraw();
    }
    ((Control *)&editor_active.wnd->combos[3])->show_and_redraw();
    sel = editor_active.wnd->combos[3].get_cur_sel();
    if (sel == 0) {
      ((Control *)&editor_active.wnd->edits[7])->show_and_redraw();
      ((Control *)&editor_active.wnd->edits[8])->show_and_redraw();
      ((Control *)&editor_active.wnd->combos[4])->show_and_redraw();
    }
    else if (sel == 1) {
      ((Control *)&editor_active.wnd->edits[7])->hide();
      ((Control *)&editor_active.wnd->edits[8])->hide();
      ((Control *)&editor_active.wnd->combos[4])->hide();
    }
    else {
      ((Control *)&editor_active.wnd->edits[7])->hide();
      ((Control *)&editor_active.wnd->edits[8])->show_and_redraw();
      ((Control *)&editor_active.wnd->combos[4])->show_and_redraw();
    }
    ((Control *)&editor_active.wnd->combos[5])->show_and_redraw();
  }
  else {
    for (i = 5; i < 9; i++) {
      ((Control *)&editor_active.wnd->edits[i])->hide();
    }
    for (i = 1; i < 6; i++) {
      ((Control *)&editor_active.wnd->combos[i])->hide();
    }
  }
  return;
}

/* 0051acdc auto */
void __cdecl terrain_editor_on_combo(int ctrl_id)

{
  int errors;
  HWND hWnd;
  i32 terrain;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    errors = terrain_editor_store_controls();
    if (errors == 0) {
      terrain = editor_active.wnd->combos[0].get_cur_sel();
      terrain = terrain_editor_sel_to_terrain(terrain);
      editor_active.wnd->item = terrain;
      terrain_editor_load_controls();
      terrain_editor_update_visibility();
      terrain_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      terrain_editor_load_controls();
      terrain_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_13);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  else if ((ctrl_id == 0xcf) || (ctrl_id == 0xd3)) {
    terrain_editor_update_visibility();
    terrain_editor_redraw();
  }
  return;
}

/* 0051adfd auto */
void EditorWindow::terrain_editor_create_combo(int combo_idx)

{
  int left;
  int top;
  iptr label;
  u8 *parent2;
  iptr terrain2;
  i8 rect[16];
  
  left = terrain_editor_combo_pos_x_block.terrain_editor_combo_pos[combo_idx].x + client_x;
  top = terrain_editor_combo_pos_x_block.terrain_editor_combo_pos[combo_idx].y + client_y;
  if (combo_idx == 0) {
    rect_set_xywh((iptr)rect,left - 0x1e,top,0xa0,font_height_uptr << 3);
  }
  else if ((combo_idx == 2) || (combo_idx == 4)) {
    rect_set_xywh((iptr)rect,left - 0x1e,top,0x82,font_height_uptr << 3);
  }
  else {
    rect_set_xywh((iptr)rect,left,top,100,font_height_uptr << 3);
  }
  left = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->create((iptr)parent2,left,
             (iptr)rect);
  ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->set_callback(terrain_editor_on_combo);
  switch(combo_idx) {
  case 0:
    for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
      if ((terrain2 != 0xd) && (terrain2 != 0x18)) {
        label = (iptr)label_string(terrain_types[terrain2].name);
        ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
      }
    }
    break;
  case 1:
  case 3:
    label = (iptr)label_string(label_table.string_ids[0x1ef]);
    ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
      label = (iptr)label_string(terrain_types[terrain2].name);
      ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 2:
  case 4:
    label = (iptr)label_string(label_table.string_ids[0x1f2]);
    ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    for (terrain2 = 1; terrain2 < 7; terrain2++) {
      label = (iptr)label_string(government_names[terrain2]);
      ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 5:
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
      label = (iptr)label_string(terrain_types[terrain2].name);
      ((ComboBox *)(text_font + combo_idx * 0x38 + 0x28))->add_string((const char *)label);
    }
  }
  return;
}

/* 0051b1c2 auto */
void EditorWindow::terrain_editor_create_edit(int field_idx)

{
  int ctrl_id;
  u8 *parent2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&terrain_editor_field_pos_x_block.terrain_editor_field_pos_x))[field_idx * 2] + client_x,
                     (((i32 *)&terrain_editor_field_pos_x_block.terrain_editor_field_pos_y))[field_idx * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + field_idx * 0x48 - 0x2c))->create((iptr)parent2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_162);
  ((EditControl *)(numeric_edit_controls_area + field_idx * 0x48 - 0x2c))->set_max_len(3);
  ((EditControl *)(numeric_edit_controls_area + field_idx * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 0051b2b6 auto */
void EditorWindow::terrain_editor_draw()

{
  byte key_color;
  int height;
  i32 width;
  uint color;
  i32 frame_style;
  i8 sprite_rect[16];
  i8 base_rect[16];
  Sprite *sprite;
  int label_y;
  int label_x;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  if ((int)item_uptr < 0xb) {
    sprite = (Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c);
  }
  else if ((int)item_uptr < 0x16) {
    sprite = (Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + ((int)item_uptr % 0xb) * 0x78);
  }
  else {
    sprite = (Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + ((int)item_uptr % 0xb) * 0x78);
  }
  key_color = sprite->get_key_color();
  color = (uint)key_color;
  height = sprite->height();
  height = height * 2;
  width = sprite->width();
  gfx_fill_rect_xywh((iptr)this,icon_x,icon_y,width,height,color);
  height = sprite->height();
  sprite->draw_keyed((iptr)base_rect,(iptr)this,0,icon_x,icon_y + height / 2);
  frame_style = 6;
  height = sprite->height();
  height = height * 2;
  width = sprite->width();
  gfx_draw_tiled_frame((iptr)this,icon_x,icon_y,width,height,frame_style);
  sprite = (Sprite *)0x0;
  if (item_uptr == 3) {
    sprite = (Sprite *)((i8 *)&forest_sprites);
  }
  else if (item_uptr == 4) {
    sprite = (Sprite *)((i8 *)&hill_sprites_block.hill_sprites);
  }
  else if (item_uptr == 5) {
    sprite = (Sprite *)((i8 *)&mountain_sprites);
  }
  if (sprite != (Sprite *)0x0) {
    height = sprite->height();
    sprite->draw((iptr)sprite_rect,(iptr)this,icon_x,icon_y + height / 2);
  }
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  gfx_draw_tiled_frame((iptr)this,client_x + 0x1c,client_y + 0xc2,0x1e6,0x21,6);
  gfx_draw_tiled_frame((iptr)this,client_x + 0x1c,client_y + 0xf9,0x1e6,0x21,6);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_field_pos_x[0] + client_x + 0x18;
  label_y = ((terrain_editor_field_pos_x_block.terrain_editor_field_pos_y[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1d0);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_defense_field_x + client_x + 0x18;
  label_y = ((terrain_editor_field_pos_x_block.terrain_editor_defense_field_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1d1);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0x32);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_food_field_x + client_x + 0x18;
  label_y = terrain_editor_field_pos_x_block.terrain_editor_food_field_y + client_y + font_height_uptr * -2 - 2;
  text_clear();
  text_append_label(0x1d2);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_shield_field_x + client_x + 0x18;
  label_y = terrain_editor_field_pos_x_block.terrain_editor_shield_field_y + client_y + font_height_uptr * -2;
  text_clear();
  text_append_label(0x1d3);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_trade_field_x + client_x + 0x18;
  label_y = terrain_editor_field_pos_x_block.terrain_editor_trade_field_y + client_y + font_height_uptr * -2;
  text_clear();
  text_append_label(0x1d4);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_food_field_x + client_x + 0x18;
  label_y = ((terrain_editor_field_pos_x_block.terrain_editor_food_field_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1d5);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_shield_field_x + client_x + 0x18;
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_trade_field_x + client_x + 0x18;
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_irrigate_bonus_x + client_x + 0x18;
  label_y = ((terrain_editor_field_pos_x_block.terrain_editor_irrigate_bonus_y + client_y) - font_height_uptr) - 10;
  text_clear();
  text_append_label(0x1d9);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_irrigate_turns_x + client_x + 0x18;
  label_y = ((terrain_editor_field_pos_x_block.terrain_editor_irrigate_turns_y + client_y) - font_height_uptr) - 10;
  text_clear();
  text_append_label(0x1da);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_combo_pos_x_block.terrain_editor_irrig_combo_x + client_x + 0x32;
  label_y = ((terrain_editor_combo_pos_x_block.terrain_editor_irrig_combo_y + client_y) - font_height_uptr) - 10;
  text_clear();
  text_append_label(0x1d8);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_combo_pos_x_block.terrain_editor_irrig_ai_combo_x + client_x + 0x32;
  label_y = ((terrain_editor_combo_pos_x_block.terrain_editor_irrig_ai_combo_y + client_y) - font_height_uptr) - 10;
  text_clear();
  text_append_label(0x1db);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_combo_pos_x_block.terrain_editor_transform_combo_x + client_x + 0x32;
  label_y = ((terrain_editor_combo_pos_x_block.terrain_editor_transform_combo_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1dc);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = terrain_editor_combo_pos_x_block.terrain_editor_irrig_combo_x + client_x;
  label_y = terrain_editor_combo_pos_x_block.terrain_editor_irrig_combo_y + client_y + 4;
  text_clear();
  text_append_label(0x1d6);
  height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->text_width((const char *)&textbuf.buf);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,-10 - height);
  label_x = terrain_editor_combo_pos_x_block.terrain_editor_mine_combo_x + client_x;
  label_y = terrain_editor_combo_pos_x_block.terrain_editor_mine_combo_y + client_y + 4;
  text_clear();
  text_append_label(0x1d7);
  height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->text_width((const char *)&textbuf.buf);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,-10 - height);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_irrigate_bonus_x + client_x + 0x30;
  label_y = terrain_editor_field_pos_x_block.terrain_editor_irrigate_bonus_y + client_y + 4;
  text_clear();
  text_append_label(0x1d2);
  height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->text_width((const char *)&textbuf.buf);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,height + 10);
  label_x = terrain_editor_field_pos_x_block.terrain_editor_mine_bonus_x + client_x + 0x30;
  label_y = terrain_editor_field_pos_x_block.terrain_editor_mine_bonus_y + client_y + 4;
  text_clear();
  text_append_label(0x1dd);
  height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->text_width((const char *)&textbuf.buf);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,height + 10);
  wnd_blit_surface();
  return;
}

/* 0051bba1 auto */
void EditorWindow::terrain_editor_run()

{
  DibSurface *this_00;
  uptr line_height;
  i32 title_text;
  int btn_x;
  int btn_y;
  iptr btn_label;
  i32 wnd_style;
  i32 wnd_x;
  i32 wnd_y;
  u32 wnd_width;
  u32 wnd_height;
  i32 create_arg7;
  i32 create_arg8;
  i32 create_arg9;
  u8 *misc_parent;
  u8 *coast_parent;
  u8 *river_parent;
  u8 *icon_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  DibSurface *new_surface;
  int ctrl_idx;
  i8 btn_rect[16];
  GamePalette palette;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    new_surface = (DibSurface *)0x0;
  }
  else {
    new_surface = surface_ctor(this_00);
  }
  editor_ui.bg_surface = new_surface;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorpt_gif_3,10,0xc0,(iptr)&palette);
  line_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = line_height;
  create_arg9 = 0;
  create_arg8 = 0;
  create_arg7 = 0;
  wnd_height = wnd_height_u;
  wnd_width = wnd_width_u;
  wnd_y = 0;
  wnd_x = 0;
  wnd_style = 0xd;
  title_text = (iptr)label_string(label_table.string_ids[0x1ca]);
  this->FrameWindow::create((const char *)title_text,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,create_arg7,create_arg8,create_arg9);
  for (ctrl_idx = 0; ctrl_idx < 0xf; ctrl_idx++) {
    if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[ctrl_idx].kind == 9) {
      terrain_editor_create_edit(terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[ctrl_idx].slot);
    }
    else if (terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[ctrl_idx].kind == 0xc) {
      terrain_editor_create_combo(terrain_editor_ctrl_kinds_block.terrain_editor_ctrl_rows[ctrl_idx].slot);
    }
  }
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_width - 10;
  button_width_u = (int)(btn_x + (btn_x >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  btn_x = client_x + 2;
  rect_set_xywh((iptr)btn_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)ok_parent,0x65,(iptr)btn_rect,(const char *)btn_label);
  ok_button.set_callback((CtrlNotifyFn)terrain_editor_on_ok);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)btn_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((iptr)rename_parent,0x66,(iptr)btn_rect,(const char *)btn_label);
  rename_button.set_callback((CtrlNotifyFn)terrain_editor_on_rename);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)btn_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)help_parent,0x66,(iptr)btn_rect,(const char *)btn_label);
  help_button.set_callback((CtrlNotifyFn)terrain_editor_on_help);
  rect_set_xywh((iptr)btn_rect,btn_x + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)cancel_parent,0x66,(iptr)btn_rect,(const char *)btn_label);
  cancel_button.set_callback((CtrlNotifyFn)terrain_editor_on_cancel);
  cancel_button.set_cancel_button();
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->width();
  button_width_u = btn_x + 0xd;
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  wnd_height = client_x;
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->height();
  rect_set_xywh((iptr)btn_rect,wnd_height + 0x19,btn_x * 2 + client_y + 0x28,
                     button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    icon_parent = (u8 *)0x0;
  }
  else {
    icon_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((iptr)icon_parent,0x65,(iptr)btn_rect,(const char *)btn_label);
  icon_button.set_callback((CtrlNotifyFn)terrain_editor_on_edit_icon);
  btn_y = (client_width + client_x - 0x19) - button_width_u;
  btn_x = client_y + 0x28;
  rect_set_xywh((iptr)btn_rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    river_parent = (u8 *)0x0;
  }
  else {
    river_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x23d]);
  misc_button.create((iptr)river_parent,0x65,(iptr)btn_rect,(const char *)btn_label);
  misc_button.set_callback((CtrlNotifyFn)terrain_editor_on_edit_river);
  btn_x = btn_x + button_height_u + 2;
  rect_set_xywh((iptr)btn_rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    coast_parent = (u8 *)0x0;
  }
  else {
    coast_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x23e]);
  coast_button.create((iptr)coast_parent,0x65,(iptr)btn_rect,(const char *)btn_label);
  coast_button.set_callback((CtrlNotifyFn)terrain_editor_on_edit_coast);
  rect_set_xywh((iptr)btn_rect,btn_y,btn_x + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    misc_parent = (u8 *)0x0;
  }
  else {
    misc_parent = (u8 *)&window;
  }
  btn_label = (iptr)label_string(label_table.string_ids[0x23c]);
  terrain_misc_button.create((iptr)misc_parent,0x65,(iptr)btn_rect,(const char *)btn_label);
  terrain_misc_button.set_callback((CtrlNotifyFn)terrain_editor_on_edit_misc);
  window.set_keyboard_nav(0);
  terrain_editor_load_rules();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  terrain_editor_load_controls();
  terrain_editor_update_visibility();
  combo_count_u = 6;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(terrain_editor_redraw);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if (editor_ui.bg_surface != (DibSurface *)0x0) {
    editor_ui.bg_surface->surface_delete(1);
  }
  editor_ui.bg_surface = (DibSurface *)0x0;
  combo_count_u = 0;
  return;
}

/* 0051c635 auto */
void __cdecl terrain_editor_open(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.terrain_editor_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 0055a980 auto */
void __cdecl map_build_coarse_connectivity(void)

{
  int in_bounds;
  i32 nbr_y;
  iptr dir;
  i32 nbr_x;
  int pass;
  int path_len;
  iptr cx;
  i32 y2;
  uint want_ocean;
  i32 x2;
  int tile_y;
  int cy;
  int tile_x;
  int landmass2;
  int adj_y;
  iptr adj_x;
  int nbr_landmass;
  u8 *grid;
  
  pathfind.road_planning = 1;
  for (pass = 0; pass < 2; pass++) {
    if (pass == 0) {
      grid = map_data.coarse[0];
    }
    else {
      grid = map_data.coarse[1];
    }
    want_ocean = (uint)(pass != 0);
    _memset(grid,0,(int)map.quarter_w * (int)map.quarter_h);
    for (cx = 0; cx < map.quarter_w; cx++) {
      for (cy = 0; cy < map.quarter_h; cy++) {
        tile_x = cx * 4 + 1;
        tile_y = cy * 4 + 1;
        landmass2 = map_coarse_cell_landmass(tile_x,tile_y,(iptr)&x2,(iptr)&y2,want_ocean);
        if (-1 < landmass2) {
          for (dir = 0; dir < 4; dir++) {
            adj_x = map_wrap_x((((i8 *)&dir_dy_table_block.coarse_dir_dx_table))[dir] * 4 + tile_x);
            adj_y = (((i8 *)&dir_dy_table_block.coarse_dir_dy_table))[dir] * 4 + tile_y;
            in_bounds = map_in_bounds(adj_x,adj_y);
            if ((((in_bounds != 0) &&
                 (nbr_landmass = map_coarse_cell_landmass(adj_x,adj_y,(iptr)&nbr_x,(iptr)&nbr_y,want_ocean)
                 , nbr_landmass == landmass2)) &&
                (path_len = map_path_length(x2,y2,nbr_x,nbr_y,want_ocean,5),
                0 < path_len)) && (path_len < 5)) {
              grid[map.quarter_w * cy + cx] =
                   grid[map.quarter_w * cy + cx] |
                   (byte)(1 << ((byte)dir & 0x1f));
              adj_x = coarse_wrap_x((((i8 *)&dir_dy_table_block.coarse_dir_dx_table))[dir] + cx);
              adj_y = (((i8 *)&dir_dy_table_block.coarse_dir_dy_table))[dir] + cy;
              if (((-1 < adj_x) && (-1 < adj_y)) &&
                 ((adj_x < map.quarter_w && (adj_y < map.quarter_h)))) {
                grid[map.quarter_w * adj_y + adj_x] =
                     grid[map.quarter_w * adj_y + adj_x] |
                     (byte)(1 << ((char)dir - 4U & 7));
              }
            }
          }
        }
      }
      ui_pump_messages_net();
    }
  }
  pathfind.road_planning = 0;
  return;
}

/* 0055ac37 auto */
i32 __cdecl map_coarse_cell_landmass(iptr base_x,int base_y,iptr out_x,iptr out_y,int want_ocean)

{
  iptr x;
  int y;
  int ok;
  i32 landmass;
  int offset;
  
  offset = 0;
  while( true ) {
    if (1 < offset) {
      return 0xffffffff;
    }
    x = map_wrap_x(base_x + offset);
    y = base_y + offset;
    ok = map_in_bounds(x,y);
    if ((ok != 0) && (ok = tile_is_ocean(x,y), ok == want_ocean)) break;
    offset++;
  }
  landmass = tile_landmass(x,y);
  *(iptr *)out_x = x;
  *(int *)out_y = y;
  return landmass;
}

/* 0056b8a0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl map_label_font_global_init(void)

{
  map_label_font_static_init();
  map_label_font_reg_atexit();
  return;
}

/* 0056b8ba auto */
void __cdecl map_label_font_static_init(void)

{
  ((GameFont *)((i8 *)&status_window_block.map_label_font))->init_empty();
  return;
}

/* 0056b8d4 auto */
void __cdecl map_label_font_reg_atexit(void)

{
  _atexit(map_label_font_static_dtor);
  return;
}

/* 0056b8f1 auto */
void __cdecl map_label_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&status_window_block.map_label_font))->destroy();
  return;
}

/* 0056c705 auto */
void __cdecl
map_animate_unit_move(iptr unit,int from_x,int from_y,iptr dir,int sound_id,iptr sound_arg)

{
  char dy;
  int tmp;
  i32 dim;
  int step_x[8];
  int to_y;
  i32 save_h[8];
  int dx;
  i32 to_x;
  int rect_bottom2[8];
  int rect_top2[8];
  int frame_delay;
  RECT dirty_rect;
  i32 save_w[8];
  int rect_right2[8];
  int to_sy;
  int rect_left2[8];
  int cur_sy[8];
  int from_sy;
  int cur_sx[8];
  char view_active[8];
  DWORD now;
  DibSurface saved_bg[8];
  int frame2;
  DWORD last_time;
  int num_frames;
  int to_sx;
  int step[8];
  iptr view;
  int any_visible;
  int saved_origin_y[8];
  int from_sx;
  int sound_played;
  i8 rect[16];
  int step_y[8];
  int saved_origin_x[8];
  
  any_visible = 0;
  crt_vector_ctor_iterator(saved_bg,0x48,8,surface_ctor,surface_dtor);
  ON_SCOPE_EXIT(crt_vector_dtor_iterator(&saved_bg,0x48,8,surface_dtor));
  sound_played = 0;
  net.map_redraw_busy = 1;
  net.move_anim_sound = 0xffffffff;
  net.move_anim_sound_arg = 0xffffffff;
  dx = (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir];
  dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
  to_x = map_wrap_x(dx + from_x);
  to_y = dy + from_y;
  map_redraw_area_all_views(from_x,from_y,0,human_civ[0],0);
  map_redraw_area_all_views(to_x,to_y,0,human_civ[0],0);
  combat_anim.origin_stack_unit = -1;
  for (view = 0; view < 8; view++) {
    view_active[view] = '\0';
    if (((view == 0) || (map_views_block.map_view_recs[view].open != 0)) &&
       ((tmp = map_views_block.map_view_recs[view].tile_in_view(from_x,from_y),
        tmp != 0 ||
        (tmp = map_views_block.map_view_recs[view].tile_in_view(to_x,to_y
                           ), tmp != 0)))) {
      view_active[view] = 1;
      saved_origin_x[view] = (((i32 *)&map_views_block.mapview_origin_x))[view * 0xfc];
      saved_origin_y[view] = (((i32 *)&map_views_block.mapview_origin_y))[view * 0xfc];
      map_views_block.map_view_recs[view].tile_to_screen((iptr)&from_sx,
                 (iptr)&from_sy,from_x,from_y);
      map_views_block.map_view_recs[view].tile_to_screen((iptr)&to_sx,
                 (iptr)&to_sy,to_x,to_y);
      from_sy -= (((i32 *)&map_views_block.mapview_half_tile_height))[view * 0xfc];
      to_sy -= (((i32 *)&map_views_block.mapview_half_tile_height))[view * 0xfc];
      tmp = to_sx;
      if (from_sx <= to_sx) {
        tmp = from_sx;
      }
      rect_left2[view] = tmp;
      tmp = to_sy;
      if (from_sy <= to_sy) {
        tmp = from_sy;
      }
      rect_top2[view] = tmp;
      tmp = to_sx;
      if (to_sx <= from_sx) {
        tmp = from_sx;
      }
      rect_right2[view] = map_views_block.map_view_recs[view].tile_width + tmp - 1;
      tmp = to_sy;
      if (to_sy <= from_sy) {
        tmp = from_sy;
      }
      rect_bottom2[view] = map_views_block.map_view_recs[view].unit_sprite_height + tmp - 1;
      cur_sx[view] = from_sx;
      cur_sy[view] = from_sy;
      rect_set((iptr)rect,rect_left2[view],rect_top2[view],
                         rect_right2[view] + 1,rect_bottom2[view] + 1);
      dim = rect_width((RECT *)rect);
      save_w[view] = dim;
      dim = rect_height((RECT *)rect);
      save_h[view] = dim;
      if (any_visible == 0) {
        any_visible = 1;
        frame_delay = 0x10;
        tmp = map_views_block.map_view_recs[view].zoom_scale_round(2);
        step[view] = tmp;
        tmp = step[view];
        if (tmp < 2) {
          tmp = 1;
        }
        step[view] = tmp;
        if (step[view] == 3) {
          step[view] = 4;
        }
        num_frames = map_views_block.map_view_recs[view].tile_height / step[view] + 1;
        frame_delay = (frame_delay << 4) / num_frames;
        if ((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
           (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) {
          step[view] = step[view] << 1;
          num_frames = map_views_block.map_view_recs[view].tile_height / step[view] + 1;
        }
      }
      else {
        step[view] = map_views_block.map_view_recs[view].tile_height / num_frames;
      }
      (saved_bg + view)->DibSurface::create(save_w[view],save_h[view]);
      gfx_copy_rect_clipped((iptr)&map_views_block.map_view_recs[view],(iptr)(saved_bg + view),
                         rect_left2[view],rect_top2[view],
                         (((i32 *)&map_views_block.mapview_client_origin_x))[view * 0xfc],(((i32 *)&map_views_block.mapview_client_origin_y))[view * 0xfc],0,0,0,0,
                         save_w[view],save_h[view]);
      step_x[view] = step[view] * dx;
      step_y[view] = (step[view] * (int)dy) / 2;
    }
  }
  if (any_visible == 0) {
    net.map_redraw_busy = 0;
  }
  else {
    last_time = timeGetTime();
    for (frame2 = 0; frame2 < num_frames; frame2++) {
      for (view = 0; view < 8; view++) {
        if (view_active[view] != '\0') {
          tmp = map_views_block.map_view_recs[view].tile_in_view(from_x,from_y);
          if (((tmp == 0) || ((((i32 *)&map_views_block.mapview_origin_x))[view * 0xfc] != saved_origin_x[view])) ||
             ((((i32 *)&map_views_block.mapview_origin_y))[view * 0xfc] != saved_origin_y[view])) {
            view_active[view] = '\0';
          }
          else {
            if ((sound_played == 0) && (-1 < sound_id)) {
              sound_played = 1;
              snd_play_effect(sound_id,sound_arg,0,0);
            }
            gfx_draw_unit((iptr)&map_views_block.map_view_recs[view],unit,5,
                               cur_sx[view],cur_sy[view],
                               (int)(((i16 *)&map_views_block.mapview_zoom))[view * 0x1f8],1);
            rect_set_xywh((iptr)&dirty_rect,cur_sx[view] + step[view] * -2,
                               cur_sy[view] + step[view] * -2,
                               step[view] * 4 + map_views_block.map_view_recs[view].tile_width
                               ,step[view] * 4 +
                                map_views_block.map_view_recs[view].unit_sprite_height);
            tmp = rect_left2[view];
            if (rect_left2[view] <= dirty_rect.left) {
              tmp = dirty_rect.left;
            }
            dirty_rect.left = tmp;
            tmp = rect_top2[view];
            if (rect_top2[view] <= dirty_rect.top) {
              tmp = dirty_rect.top;
            }
            dirty_rect.top = tmp;
            tmp = rect_right2[view] + 1;
            if (dirty_rect.right <= rect_right2[view] + 1) {
              tmp = dirty_rect.right;
            }
            dirty_rect.right = tmp;
            tmp = rect_bottom2[view] + 1;
            if (dirty_rect.bottom <= rect_bottom2[view] + 1) {
              tmp = dirty_rect.bottom;
            }
            dirty_rect.bottom = tmp;
            map_views_block.map_view_recs[view].wnd_blit_surface_rect((iptr)&dirty_rect);
            gfx_copy_rect_clipped((iptr)(saved_bg + view),(iptr)&map_views_block.map_view_recs[view]
                               ,cur_sx[view] - rect_left2[view],
                               cur_sy[view] - rect_top2[view],0,0,
                               cur_sx[view],cur_sy[view],
                               (((i32 *)&map_views_block.mapview_client_origin_x))[view * 0xfc],(((i32 *)&map_views_block.mapview_client_origin_y))[view * 0xfc],
                               map_views_block.map_view_recs[view].tile_width,
                               map_views_block.map_view_recs[view].unit_sprite_height);
            cur_sx[view] += step_x[view];
            cur_sy[view] += step_y[view];
          }
        }
      }
      if ((((*((char *)&game.game_options + 1)) & (OPT_FAST_PIECE_SLIDE >> 8)) == 0) && (frame2 < num_frames - 1)) {
        do {
          win_flush_pending_msgs();
          now = timeGetTime();
          if (2 < game.multiplayer) {
            net.process_messages(1,0);
          }
        } while ((int)(now - last_time) < frame_delay);
        last_time = now;
      }
    }
    net.map_redraw_busy = 0;
  }
  return;
}

/* 0056e180 auto */
i32 MapView::zoom_scale_round(i32 value)

{
  i32 result;
  
  result = gfx_zoom_scale_round(value,(int)(short)zoom_u16);
  return result;
}

/* 0057ed3f auto */
i32 __cdecl map_animate_explosion(i32 x,i32 y,int sound_id)

{
  int in_view;
  iptr scaled;
  i8 clip_rect[16];
  i32 result;
  iptr sprite_h[8];
  iptr sprite_w[8];
  DWORD last_time;
  i32 screen_y[8];
  int screen_x[8];
  int any_visible;
  DWORD now;
  int view_active[8];
  DibSurface saved_bg[8];
  int frame2;
  int saved_origin_y[8];
  iptr view;
  i8 view_rects[128];
  int saved_origin_x[8];
  
  crt_vector_ctor_iterator(saved_bg,0x48,8,surface_ctor,surface_dtor);
  ON_SCOPE_EXIT(crt_vector_dtor_iterator(&saved_bg,0x48,8,surface_dtor));
  any_visible = 0;
  net.map_redraw_busy = 1;
  net.nm_abort_kaboom_count = 0;
  for (view = 0; view < 8; view++) {
    view_active[view] = 0;
    if (((view == 0) || (map_views_block.map_view_recs[view].open != 0)) &&
       (in_view = map_views_block.map_view_recs[view].tile_in_view(x,y),
       in_view != 0)) {
      view_active[view] = 1;
      any_visible = 1;
      saved_origin_x[view] = (((i32 *)&map_views_block.mapview_origin_x))[view * 0xfc];
      saved_origin_y[view] = (((i32 *)&map_views_block.mapview_origin_y))[view * 0xfc];
      scaled = map_views_block.map_view_recs[view].zoom_scale(0x20);
      sprite_w[view] = scaled;
      scaled = map_views_block.map_view_recs[view].zoom_scale(0x20);
      sprite_h[view] = scaled;
      map_views_block.map_view_recs[view].tile_to_screen((iptr)(screen_x + view)
                 ,(iptr)(screen_y + view),x,y);
      screen_x[view] += map_views_block.map_view_recs[view].quarter_tile_width;
      rect_set_xywh((iptr)(view_rects + view * 0x10),screen_x[view],
                         screen_y[view],sprite_w[view],sprite_h[view]);
      (saved_bg + view)->DibSurface::create(map_views_block.map_view_recs[view].tile_width,
                 map_views_block.map_view_recs[view].tile_height);
      gfx_copy_rect_clipped((iptr)&map_views_block.map_view_recs[view],(iptr)(saved_bg + view),
                         screen_x[view],screen_y[view],
                         (((i32 *)&map_views_block.mapview_client_origin_x))[view * 0xfc],(((i32 *)&map_views_block.mapview_client_origin_y))[view * 0xfc],0,0,0,0,
                         sprite_w[view],sprite_h[view]);
    }
  }
  if (any_visible == 0) {
    net.map_redraw_busy = 0;
    result = 0;
  }
  else {
    ON_SCOPE_EXIT(
      gfx_reset_zoom_scale();
      net.map_redraw_busy = 0);
    if (sound_id != 0) {
      snd_play_effect(sound_id,1,0,0);
    }
    last_time = timeGetTime();
    for (frame2 = 0; frame2 < 8; frame2++) {
      for (view = 0; view < 8; view++) {
        if (view_active[view] != 0) {
          in_view = map_views_block.map_view_recs[view].tile_in_view(x,y);
          if (in_view == 0) {
            view_active[view] = 0;
          }
          else {
            if (((((i32 *)&map_views_block.mapview_origin_x))[view * 0xfc] != saved_origin_x[view]) ||
               ((((i32 *)&map_views_block.mapview_origin_y))[view * 0xfc] != saved_origin_y[view])) {
              saved_origin_x[view] = (((i32 *)&map_views_block.mapview_origin_x))[view * 0xfc];
              saved_origin_y[view] = (((i32 *)&map_views_block.mapview_origin_y))[view * 0xfc];
              scaled = map_views_block.map_view_recs[view].zoom_scale(0x20);
              sprite_w[view] = scaled;
              scaled = map_views_block.map_view_recs[view].zoom_scale(0x20);
              sprite_h[view] = scaled;
              map_views_block.map_view_recs[view].tile_to_screen(
                         (iptr)(screen_x + view),(iptr)(screen_y + view),x,
                         y);
              screen_x[view] =
                   screen_x[view] + map_views_block.map_view_recs[view].quarter_tile_width;
              rect_set_xywh((iptr)(view_rects + view * 0x10),screen_x[view],
                                 screen_y[view],sprite_w[view],sprite_h[view]);
              (saved_bg + view)->DibSurface::create(map_views_block.map_view_recs[view].tile_width,
                         map_views_block.map_view_recs[view].tile_height);
              gfx_copy_rect_clipped((iptr)&map_views_block.map_view_recs[view],
                                 (iptr)(saved_bg + view),screen_x[view],
                                 screen_y[view],(((i32 *)&map_views_block.mapview_client_origin_x))[view * 0xfc],
                                 (((i32 *)&map_views_block.mapview_client_origin_y))[view * 0xfc],0,0,0,0,sprite_w[view],
                                 sprite_h[view]);
            }
            gfx_set_zoom_scale((int)(((i16 *)&map_views_block.mapview_zoom))[view * 0x1f8]);
            ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + frame2 * 0x3c))->draw((iptr)clip_rect,
                       (iptr)&map_views_block.map_view_recs[view],screen_x[view],
                       screen_y[view]);
            map_views_block.map_view_recs[view].wnd_blit_surface_rect(
                       (iptr)(view_rects + view * 0x10));
          }
        }
      }
      do {
        win_flush_pending_msgs();
        now = timeGetTime();
        if ((2 < game.multiplayer) &&
           (net.process_messages(1,0), net.nm_abort_kaboom_count != 0))
        return 1;
      } while ((int)(now - last_time) < 0x40);
      last_time = now;
      for (view = 0; view < 8; view++) {
        if (view_active[view] != 0) {
          gfx_copy_rect_clipped((iptr)(saved_bg + view),(iptr)&map_views_block.map_view_recs[view],0
                             ,0,0,0,screen_x[view],screen_y[view],
                             (((i32 *)&map_views_block.mapview_client_origin_x))[view * 0xfc],(((i32 *)&map_views_block.mapview_client_origin_y))[view * 0xfc],
                             sprite_w[view],sprite_h[view]);
        }
      }
    }
    ui_delay_ticks(10);
    result = 1;
  }
  return result;
}

/* 0057f657 auto */
void __cdecl map_animate_nuke(iptr x,iptr y)

{
  int ok;
  i8 clip_rect[16];
  DibSurface *new_surface;
  DibSurface *alloc;
  int screen_x;
  int anim_h;
  int anim_y;
  DWORD last_time;
  int anim_w;
  int frame2;
  int anim_x;
  DWORD now;
  int screen_y;
  DibSurface *saved_bg;
  RECT dirty_rect;
  
  ok = map_in_bounds(x,y);
  if ((ok != 0) && (ok = tile_visible_to(x,y,human_civ[0]), ok != 0)) {
    net.map_redraw_busy = 1;
    alloc = (DibSurface *)operator_new(0x48);
    if (alloc == (DibSurface *)0x0) {
      new_surface = (DibSurface *)0x0;
    }
    else {
      new_surface = surface_ctor(alloc);
    }
    saved_bg = new_surface;
    mapviews_ensure_visible(x,y,human_civ[0]);
    ui_flush_input();
    snd_play_effect(SND_NUKEXPLO,0,0,0);
    if (((byte)game.game_options & OPT_SOUND) != 0) {
      win_flush_pending_msgs();
      last_time = timeGetTime();
      do {
        win_flush_pending_msgs();
        if (2 < game.multiplayer) {
          net.process_messages(1,0);
        }
        now = timeGetTime();
      } while ((int)(now - last_time) < 0x157c);
    }
    anim_w = ((MapView *)((i8 *)&map_views_block.map_views))->zoom_scale(0x5b);
    anim_h = ((MapView *)((i8 *)&map_views_block.map_views))->zoom_scale(0x48);
    saved_bg->DibSurface::create(anim_w,anim_h);
    ((MapView *)((i8 *)&map_views_block.map_views))->tile_to_screen((iptr)&screen_x,(iptr)&screen_y,x,y);
    anim_x = (map_views_block.mapview_half_tile_width + screen_x) - (anim_w >> 1);
    anim_y = (map_views_block.mapview_half_tile_height[0] + screen_y) - (anim_h >> 1);
    gfx_copy_rect_clipped((iptr)&map_views_block.map_views,(iptr)saved_bg,anim_x,anim_y,map_views_block.mapview_client_origin_x[0],map_views_block.mapview_client_origin_y[0],0,0,0,0,
                       anim_w,anim_h);
    rect_set_xywh((iptr)&dirty_rect,anim_x,anim_y,anim_w,anim_h);
    if (dirty_rect.left < 0) {
      dirty_rect.left = 0;
    }
    if (dirty_rect.top < 0) {
      dirty_rect.top = 0;
    }
    ok = rect_width(&dirty_rect);
    if ((0 < ok) && (ok = rect_height(&dirty_rect), 0 < ok)) {
      last_time = timeGetTime();
      ((MapView *)((i8 *)&map_views_block.map_views))->apply_zoom();
      for (frame2 = 0; frame2 < 0xb; frame2++) {
        ((Sprite *)(((i8 *)&hill_sprites_block.nuke_anim_sprites) + frame2 * 0x3c))->draw((iptr)clip_rect,(iptr)&map_views_block.map_views,
                   anim_x,anim_y);
        ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&dirty_rect);
        do {
          win_flush_pending_msgs();
          if (2 < game.multiplayer) {
            net.process_messages(1,0);
          }
          now = timeGetTime();
        } while ((int)(now - last_time) < 100);
        last_time = now;
        gfx_copy_rect_clipped((iptr)saved_bg,(iptr)&map_views_block.map_views,0,0,0,0,anim_x,anim_y,map_views_block.mapview_client_origin_x[0],
                           map_views_block.mapview_client_origin_y[0],anim_w,anim_h);
      }
      gfx_reset_zoom_scale();
      ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&dirty_rect);
    }
    if (saved_bg != (DibSurface *)0x0) {
      saved_bg->surface_delete(1);
    }
    net.map_redraw_busy = 0;
  }
  return;
}

/* 005802fd auto */
void __cdecl
map_redraw_combat_tiles(i32 att_x,i32 att_y,i32 def_x,i32 def_y)

{
  map_redraw_area_all_views(att_x,att_y,0,human_civ[0],1);
  map_redraw_area_all_views(def_x,def_y,0,human_civ[0],1);
  return;
}

/* 0058c56c auto */
i32 __cdecl tile_has_irrigation_source(int x2,int y2)

{
  iptr x;
  int y;
  int ok;
  byte *tile;
  uint improvements2;
  iptr dir;
  
  dir = 0;
  do {
    if (4 < dir) {
      return 0;
    }
    x = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x2);
    y = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y2;
    ok = map_in_bounds(x,y);
    if (ok != 0) {
      ok = tile_is_ocean(x,y);
      if (ok != 0) {
        return 1;
      }
      tile = (byte *)tile_at(x,y);
      if ((*tile & 0x80) != 0) {
        return 1;
      }
      improvements2 = tile_improvements(x,y);
      if ((improvements2 & TILE_IRRIGATION) != 0) {
        return 1;
      }
    }
    dir++;
  } while( true );
}

/* 0058fda9 auto */
void __cdecl map_reveal_adjacent_units(int x,int y,iptr civ)

{
  int is_ocean;
  iptr x2;
  int y2;
  int ok;
  iptr dir;
  
  is_ocean = tile_is_ocean(x,y);
  net_batch_begin();
  for (dir = 0; dir < 8; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    ok = map_in_bounds(x2,y2);
    if ((((ok != 0) && (ok = tile_is_ocean(x2,y2), ok == is_ocean)) &&
        (ok = tile_unit_owner(x2,y2), -1 < ok)) &&
       (tile_reveal_to_civ(x2,y2,civ), human_civ[0] == civ)) {
      map_redraw_area_all_views(x2,y2,0,civ,1);
      net_send_message(NM_DRAW_AREA,0xff,x2,y2,0,0,0,0,0,0);
    }
  }
  net_batch_flush();
  return;
}

/* 005ae052 auto */
iptr __cdecl map_wrap_x(iptr x)

{
  if ((game.rules & RULE_FLAT_WORLD) == 0) {
    if (x < 0) {
      x = map.width + x;
    }
    else if (map.width <= x) {
      x -= map.width;
    }
  }
  return x;
}

/* 005ae10e auto */
int __cdecl map_dx_wrapped(int x1,int x2)

{
  int dx;
  
  if (x1 == x2 || x1 - x2 < 0) {
    dx = ~(x1 - x2) + 1;
  }
  else {
    dx = x1 - x2;
  }
  if (((game.rules & RULE_FLAT_WORLD) == 0) && ((int)map.width >> 1 < dx)) {
    dx = map.width - dx;
  }
  return dx;
}

/* 005ae1b0 auto */
int __cdecl map_distance(int x1,int y1,int x2,int y2)

{
  i32 dy;
  int dx;
  
  if (x1 == x2 || x1 - x2 < 0) {
    dx = ~(x1 - x2) + 1;
  }
  else {
    dx = x1 - x2;
  }
  if (((game.rules & RULE_FLAT_WORLD) == 0) && ((int)map.width >> 1 < dx)) {
    dx = map.width - dx;
  }
  if (y1 == y2 || y1 - y2 < 0) {
    dy = ~(y1 - y2) + 1;
  }
  else {
    dy = y1 - y2;
  }
  return dy + dx >> 1;
}

/* 005ae31d auto */
i32 __cdecl map_distance_diag(i32 x1,int y1,i32 x2,int y2)

{
  i32 dist;
  i32 dy;
  
  dist = map_dx_wrapped(x1,x2);
  if (y1 == y2 || y1 - y2 < 0) {
    dy = ~(y1 - y2) + 1;
  }
  else {
    dy = y1 - y2;
  }
  dist = dist_from_deltas(dist,dy);
  return dist;
}

/* 005b2e69 auto */
i32 __cdecl tile_first_unit(int x,int y)

{
  int owner;
  i32 result;
  iptr unit;
  iptr unit2;
  
  unit2 = -1;
  if ((unit_move_anim_active == 0) || (owner = tile_unit_owner(x,y), -1 < owner)) {
    for (unit = 0; (unit2 < 0 && (unit < game.num_units)); unit++) {
      if ((units[unit].id != 0) &&
         ((units[unit].x == x && (units[unit].y == y)))) {
        unit2 = unit;
      }
    }
    if (-1 < unit2) {
      unit_stack_validate(unit2);
    }
    result = unit_stack_first(unit2);
  }
  else {
    result = 0xffffffff;
  }
  return result;
}

/* 005b49cf auto */
uint __cdecl tile_adjacent_nonallied_civ(iptr x,int y,iptr civ)

{
  int city_owner;
  iptr x2;
  int y2;
  int ok;
  int nbr_civ;
  int is_ocean;
  int nbr_ocean;
  iptr dir;
  
  is_ocean = tile_is_ocean(x,y);
  city_owner = tile_city_owner(x,y);
  dialog_text_font_obj_block.adjacent_foreign_civ = -1;
  for (dir = 0; (dialog_text_font_obj_block.adjacent_foreign_civ < 0 && (dir < 8)); dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    ok = map_in_bounds(x2,y2);
    if (ok != 0) {
      if (city_owner < 0) {
        nbr_ocean = tile_is_ocean(x2,y2);
      }
      else {
        nbr_ocean = is_ocean;
      }
      nbr_civ = tile_city_owner(x2,y2);
      if (nbr_civ < 0) {
        nbr_civ = tile_unit_owner(x2,y2);
      }
      else {
        is_ocean = nbr_ocean;
      }
      if ((((-1 < nbr_civ) && (nbr_civ != civ)) && (nbr_ocean == is_ocean)) &&
         ((*(byte *)&civs[civ].treaties[nbr_civ] & TREATY_ALLIANCE) == 0)) {
        dialog_text_font_obj_block.adjacent_foreign_civ = nbr_civ;
      }
    }
  }
  return (uint)(-1 < dialog_text_font_obj_block.adjacent_foreign_civ);
}

/* 005b4b66 auto */
uint __cdecl tile_adjacent_nonallied_unit(int x2,int y,iptr civ)

{
  char dy;
  iptr x;
  int owner;
  iptr dir;
  
  dialog_text_font_obj_block.adjacent_foreign_civ = -1;
  for (dir = 0; (dialog_text_font_obj_block.adjacent_foreign_civ < 0 && (dir < 8)); dir++) {
    x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
    dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    owner = map_in_bounds(x,dy + y);
    if ((owner != 0) &&
       (((owner = tile_unit_owner(x,dy + y), -1 < owner && (civ != owner)) &&
        ((*(byte *)&civs[civ].treaties[owner] & TREATY_ALLIANCE) == 0)))) {
      dialog_text_font_obj_block.adjacent_foreign_civ = owner;
    }
  }
  return (uint)(-1 < dialog_text_font_obj_block.adjacent_foreign_civ);
}

/* 005b4c63 auto */
uint __cdecl tile_zoc_unit_adjacent(int x,int y,iptr civ)

{
  int is_ocean;
  iptr x2;
  int y2;
  int owner;
  iptr dir;
  
  dialog_text_font_obj_block.adjacent_foreign_civ = -1;
  is_ocean = tile_is_ocean(x,y);
  for (dir = 0; (dialog_text_font_obj_block.adjacent_foreign_civ < 0 && (dir < 8)); dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    owner = map_in_bounds(x2,y2);
    if (((owner != 0) &&
        (((owner = tile_unit_owner(x2,y2), -1 < owner && (owner != civ)) &&
         (y2 = tile_is_ocean(x2,y2), y2 == is_ocean)))) &&
       ((*(byte *)&civs[civ].treaties[owner] & TREATY_ALLIANCE) == 0)) {
      dialog_text_font_obj_block.adjacent_foreign_civ = owner;
    }
  }
  return (uint)(-1 < dialog_text_font_obj_block.adjacent_foreign_civ);
}

/* 005b4d8c auto */
i32 __cdecl tile_in_enemy_zoc(iptr x,i32 y,iptr civ)

{
  int city_owner;
  i32 result;
  
  result = 0;
  dialog_text_font_obj_block.adjacent_foreign_civ = -1;
  city_owner = tile_city_owner(x,y);
  if (city_owner < 0) {
    result = tile_zoc_unit_adjacent(x,y,civ);
  }
  return result;
}

/* 005b7fe0 auto */
void __cdecl map_alloc(void)

{
  void *handle;
  u8 *buf;
  iptr civ;
  Tile *tile;
  int i;
  uint alloc_size;
  
  map.quarter_w = (i16)(map.width + 3 >> 2);
  map.quarter_h = (i16)(map.height + 3 >> 2);
  map.area = (map.width / 2) * map.height;
  alloc_size = map.area * 6;
  if ((alloc_size & 3) != 0) {
    alloc_size = (4 - (alloc_size & 3)) + alloc_size;
  }
  map_handles.tiles = (void *)mem_handle_alloc_tracked(alloc_size);
  if (map_handles.tiles == (void *)0x0) {
    error_report(0xfffffff7,5,0,(int)map.width,(int)map.height);
  }
  map_data.tiles = (Tile *)mem_lock_handle((iptr)map_handles.tiles);
  if (map_data.tiles == (Tile *)0x0) {
    error_report(0xfffffff6,5,0,(int)map.width,(int)map.height);
  }
  tile = map_data.tiles;
  for (i = 0; i < map.area; i++) {
    tile->terrain = '\n';
    tile->improvements = '\0';
    tile->radius = '\0';
    tile->landmass = '\0';
    tile->visible = '\0';
    tile->owner = '\0';
    tile++;
  }
  alloc_size = (uint)map.area;
  if ((alloc_size & 3) != 0) {
    alloc_size = (4 - (alloc_size & 3)) + alloc_size;
  }
  for (civ = 1; civ < 8; civ++) {
    handle = (void *)mem_handle_alloc_tracked(alloc_size);
    map_data.known_handles[civ] = handle;
    if (map_data.known_handles[civ] == (void *)0x0) {
      error_report(0xfffffff7,5,0,0xea,civ);
    }
    buf = (u8 *)mem_lock_handle((iptr)map_data.known_handles[civ]);
    map_data.known[civ] = buf;
    if (map_data.known[civ] == (u8 *)0x0) {
      error_report(0xfffffff6,5,0,0xea,civ);
    }
    _memset(map_data.known[civ],0,(int)map.area);
  }
  alloc_size = (int)map.quarter_w * (int)map.quarter_h;
  if ((alloc_size & 3) != 0) {
    alloc_size = (4 - (alloc_size & 3)) + alloc_size;
  }
  map_handles.coarse[0] = (void *)mem_handle_alloc_tracked(alloc_size);
  if (map_handles.coarse[0] == (void *)0x0) {
    error_report(0xfffffff7,5,0,(int)map.quarter_w,0);
  }
  map_data.coarse[0] = (u8 *)mem_lock_handle((iptr)map_handles.coarse[0]);
  if (map_data.coarse[0] == (u8 *)0x0) {
    error_report(0xfffffff6,5,0,(int)map.quarter_w,0);
  }
  map_handles.coarse[1] = (void *)mem_handle_alloc_tracked(alloc_size);
  if (map_handles.coarse[1] == (void *)0x0) {
    error_report(0xfffffff7,5,0,(int)map.quarter_w,1);
  }
  map_data.coarse[1] = (u8 *)mem_lock_handle((iptr)map_handles.coarse[1]);
  if (map_data.coarse[1] == (u8 *)0x0) {
    error_report(0xfffffff6,5,0,(int)map.quarter_w,1);
  }
  map_handles.coarse[2] = (void *)mem_handle_alloc_tracked(alloc_size);
  if (map_handles.coarse[2] == (void *)0x0) {
    error_report(0xfffffff7,5,0,(int)map.quarter_w,2);
  }
  map_data.coarse[2] = (u8 *)mem_lock_handle((iptr)map_handles.coarse[2]);
  if (map_data.coarse[2] == (u8 *)0x0) {
    error_report(0xfffffff6,5,0,(int)map.quarter_w,2);
  }
  map_handles.coarse[3] = (void *)mem_handle_alloc_tracked(alloc_size);
  if (map_handles.coarse[3] == (void *)0x0) {
    error_report(0xfffffff7,5,0,(int)map.quarter_w,3);
  }
  map_data.coarse[3] = (u8 *)mem_lock_handle((iptr)map_handles.coarse[3]);
  if (map_data.coarse[3] == (u8 *)0x0) {
    error_report(0xfffffff6,5,0,(int)map.quarter_w,3);
  }
  map_data.allocated = 1;
  return;
}

/* 005b8416 auto */
void __cdecl map_free(void)

{
  u8 *buf;
  void *handle;
  iptr civ;
  
  if (map_data.allocated != '\0') {
    if (map_data.coarse[3] != (u8 *)0x0) {
      map_data.coarse[3] = (u8 *)mem_unlock_handle((iptr)map_handles.coarse[3]);
    }
    if (map_handles.coarse[3] != (void *)0x0) {
      map_handles.coarse[3] = (void *)mem_free_handle((iptr)map_handles.coarse[3]);
    }
    if (map_data.coarse[2] != (u8 *)0x0) {
      map_data.coarse[2] = (u8 *)mem_unlock_handle((iptr)map_handles.coarse[2]);
    }
    if (map_handles.coarse[2] != (void *)0x0) {
      map_handles.coarse[2] = (void *)mem_free_handle((iptr)map_handles.coarse[2]);
    }
    if (map_data.coarse[1] != (u8 *)0x0) {
      map_data.coarse[1] = (u8 *)mem_unlock_handle((iptr)map_handles.coarse[1]);
    }
    if (map_handles.coarse[1] != (void *)0x0) {
      map_handles.coarse[1] = (void *)mem_free_handle((iptr)map_handles.coarse[1]);
    }
    if (map_data.coarse[0] != (u8 *)0x0) {
      map_data.coarse[0] = (u8 *)mem_unlock_handle((iptr)map_handles.coarse[0]);
    }
    if (map_handles.coarse[0] != (void *)0x0) {
      map_handles.coarse[0] = (void *)mem_free_handle((iptr)map_handles.coarse[0]);
    }
    if (map_data.tiles != (Tile *)0x0) {
      map_data.tiles = (Tile *)mem_unlock_handle((iptr)map_handles.tiles);
    }
    if (map_handles.tiles != (void *)0x0) {
      map_handles.tiles = (void *)mem_free_handle((iptr)map_handles.tiles);
    }
    for (civ = 1; civ < 8; civ++) {
      if (map_data.known[civ] != (u8 *)0x0) {
        buf = (u8 *)mem_unlock_handle((iptr)map_data.known_handles[civ]);
        map_data.known[civ] = buf;
      }
      if (map_data.known_handles[civ] != (void *)0x0) {
        handle = (void *)mem_free_handle((iptr)map_data.known_handles[civ]);
        map_data.known_handles[civ] = handle;
      }
    }
    map_data.allocated = '\0';
  }
  return;
}

/* 005b85fe auto */
void __cdecl map_new_seed(void)

{
  uint ticks;
  
  ticks = timer_ticks();
  map.seed = (ushort)ticks & 0x7fff;
  if ((ticks & 0x7fff) == 0) {
    map.seed++;
  }
  return;
}

/* 005b8635 auto */
i32 __cdecl map_write(iptr file,int is_scenario)

{
  size_t written;
  iptr civ;
  i32 result;
  
  result = 1;
  written = _fwrite(&map,0xe,1,(FILE *)file);
  if (written != 0) {
    if (is_scenario == 0) {
      for (civ = 1; civ < 8; civ++) {
        written = _fwrite(map_data.known[civ],(int)map.area,1,(FILE *)file);
        if (written == 0) {
          return 1;
        }
      }
    }
    else {
      written = _fwrite(start_positions.x,0x2a,1,(FILE *)file);
      if (written == 0) {
        return 1;
      }
      written = _fwrite(start_positions.y,0x2a,1,(FILE *)file);
      if (written == 0) {
        return 1;
      }
    }
    written = _fwrite(map_data.tiles,map.area * 6,1,(FILE *)file);
    if (written != 0) {
      result = 0;
    }
  }
  return result;
}

/* BF-094 (Peter Johnson's ledger): a map header the game can lay out: an
   even width (doubled x) of at least 2, a height of at least 1, an area that
   fits map.area (i16). */
static int map_header_valid(const MapHeader *header)
{
  return ((1 < header->width) && ((header->width & 1) == 0) && (0 < header->height) &&
          ((header->width / 2) * header->height < 0x8000));
}

/* BF-236, BF-287, BF-288 (Peter Johnson's ledger): squares as the game writes
   them: terrain 0..10, region (landmass) 0..63, the civ on it 0..7 or none
   (0xf). */
static int map_tiles_valid(void)
{
  int i;
  Tile *tile;

  tile = map_data.tiles;
  for (i = 0; i < map.area; i++) {
    if ((10 < (tile->terrain & 0xf)) || (0x3f < tile->landmass) ||
       ((7 < (tile->owner >> 4)) && ((tile->owner >> 4) != 0xf))) {
      return 0;
    }
    tile++;
  }
  return 1;
}

/* 005b8783 auto */
i32 __cdecl map_read(iptr file,int is_scenario)

{
  size_t nread;
  iptr civ;
  i32 result;
  i8 header[36];
  
  result = 1;
  nread = _fread(header,0xe,1,(FILE *)file);
  if (nread != 0) {
    if (is_scenario == 0) {
      for (civ = 0; civ < 0x15; civ++) {
        start_positions.x[civ] = -1;
        start_positions.y[civ] = -1;
      }
    }
    else {
      nread = _fread(start_positions.x,0x2a,1,(FILE *)file);
      if (nread == 0) {
        return 1;
      }
      nread = _fread(start_positions.y,0x2a,1,(FILE *)file);
      if (nread == 0) {
        return 1;
      }
    }
    if (map_header_valid((MapHeader *)header) == 0) {
      return 1;
    }
    map_free();
    crt_memcpy(&map,header,0xe);
    map_alloc();
    if (is_scenario == 0) {
      for (civ = 1; civ < 8; civ++) {
        nread = _fread(map_data.known[civ],(int)map.area,1,(FILE *)file);
        if (nread == 0) {
          return 1;
        }
      }
    }
    nread = _fread(map_data.tiles,map.area * 6,1,(FILE *)file);
    if ((nread != 0) && (map_tiles_valid() != 0)) {
      result = 0;
    }
  }
  return result;
}

/* 005b8931 auto */
Tile * __cdecl tile_at(iptr x,iptr y)

{
  int in_bounds;
  Tile *tile;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds == 0) {
    tile = &map_handles.offmap_tile;
  }
  else {
    tile = (Tile *)(&map_data.tiles->terrain +
                     ((int)map.width & 0xfffffffeU) * y * 3 + (x & 0xfffffffeU) * 3);
  }
  return tile;
}

/* 005b898b auto */
u8 * __cdecl known_at(int x,int y,iptr civ)

{
  return map_data.known[civ] + (x >> 1) + ((int)map.width >> 1) * y;
}

/* 005b89bb auto */
byte __cdecl tile_terrain(iptr x,iptr y)

{
  byte *tile;
  
  tile = (byte *)tile_at(x,y);
  return *tile & 0xf;
}

/* 005b89e4 auto */
uint __cdecl tile_is_ocean(i32 x,i32 y)

{
  char terrain;
  
  terrain = tile_terrain(x,y);
  return (uint)(terrain == TERRAIN_OCEAN);
}

/* 005b8a1d auto */
int __cdecl tile_owner(iptr x,iptr y)

{
  iptr in_bounds;
  Tile *tile;
  i32 owner2;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds == 0) {
    owner2 = -1;
  }
  else {
    tile = tile_at(x,y);
    owner2 = (int)(uint)tile->owner >> 4;
    if (owner2 == 0xf) {
      owner2 = -1;
    }
  }
  return owner2;
}

/* 005b8a81 auto */
uint __cdecl tile_landmass(iptr x,iptr y)

{
  Tile *tile;
  
  tile = tile_at(x,y);
  return (uint)tile->landmass;
}

/* 005b8aa8 auto */
i32 __cdecl tile_land_landmass(iptr x,iptr y)

{
  int is_ocean;
  i32 landmass2;
  
  landmass2 = 0xffffffff;
  is_ocean = tile_is_ocean(x,y);
  if (is_ocean == 0) {
    landmass2 = tile_landmass(x,y);
  }
  return landmass2;
}

/* 005b8af0 auto */
int __cdecl tile_radius_civ(iptr x,iptr y)

{
  Tile *tile;
  
  tile = tile_at(x,y);
  return (int)(uint)tile->radius >> 5;
}

/* 005b8b1a auto */
void __cdecl tile_update_known(iptr x,iptr y,iptr civ)

{
  Tile *tile;
  i32 known_arg5;
  i32 known_arg6;
  
  if (civ != 0) {
    known_arg6 = 1;
    known_arg5 = 0;
    tile = tile_at(x,y);
    known_set(x,y,CONCAT31((int3)((uint)tile >> 8),(i8)tile->improvements),civ,
              known_arg5,known_arg6);
  }
  return;
}

/* 005b8b65 auto */
uint __cdecl tile_visible_to(iptr x,iptr y,iptr civ)

{
  uint result;
  Tile *tile;
  
  if (civ < 0) {
    result = 1;
  }
  else {
    tile = tile_at(x,y);
    result = (uint)tile->visible & 1 << ((byte)civ & 0x1f);
  }
  return result;
}

/* 005b8c18 auto */
uint __cdecl tile_get_fertility(iptr x,iptr y)

{
  Tile *tile;
  
  tile = tile_at(x,y);
  return tile->owner & 0xf;
}

/* 005b8c42 auto */
int __cdecl tile_owner_or_fertility(i32 x,iptr y)

{
  int result;
  
  result = tile_radius_civ(x,y);
  if (((result == 0) && (result = tile_get_fertility(x,y), result != 0)) &&
     (result < 9)) {
    result = 8;
  }
  return result;
}

/* 005b8ca6 auto */
i32 __cdecl tile_city_owner(iptr x,iptr y)

{
  int in_bounds;
  i32 result;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds == 0) {
    result = 0xffffffff;
  }
  else {
    result = tile_improvements(x,y);
    if (((byte)result & 0x42) == 2) {
      result = tile_owner(x,y);
    }
    else {
      result = 0xffffffff;
    }
  }
  return result;
}

/* 005b8d15 auto */
i32 __cdecl tile_airbase_owner(i32 x,i32 y)

{
  i32 result;
  
  result = tile_improvements(x,y);
  if (((byte)result & 0x42) == 0x42) {
    result = tile_owner(x,y);
  }
  else {
    result = 0xffffffff;
  }
  return result;
}

/* 005b8d62 auto */
i32 __cdecl tile_unit_owner(i32 x,i32 y)

{
  uint improvements2;
  i32 result;
  
  improvements2 = tile_improvements(x,y);
  if ((improvements2 & TILE_UNIT) == 0) {
    result = 0xffffffff;
  }
  else {
    result = tile_owner(x,y);
  }
  return result;
}

/* 005b8da4 auto */
int __cdecl tile_city_or_unit_owner(iptr x,i32 y)

{
  int owner;
  
  owner = tile_city_owner(x,y);
  if (owner < 0) {
    owner = tile_unit_owner(x,y);
  }
  return owner;
}

/* 005b8dec auto */
i32 __cdecl tile_owner_at_peace(iptr x,iptr y,iptr civ)

{
  int radius_civ;
  
  radius_civ = map_in_bounds(x,y);
  if ((((radius_civ != 0) && (0 < civ)) && (radius_civ = tile_is_ocean(x,y), radius_civ == 0)) &&
     ((radius_civ = tile_radius_civ(x,y), 0 < radius_civ && (radius_civ != civ)))) {
    if ((*(byte *)&civs[civ].treaties[radius_civ] & TREATY_ALLIANCE) != 0) {
      return 0xffffffff;
    }
    if ((*(byte *)&civs[civ].treaties[radius_civ] & TREATY_PEACE) != 0) {
      return radius_civ;
    }
  }
  return 0xffffffff;
}

/* 005b8ee1 auto */
i32 __cdecl tile_resource(int x,int y)

{
  int in_bounds;
  byte *tile;
  uint diag_u;
  uint diag_v;
  uint bit_mask;
  i32 resource;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds == 0) {
    resource = 0;
  }
  else {
    tile = (byte *)tile_at(x,y);
    if ((*tile & 0x40) == 0) {
      if ((*tile & 0xf) == 2) {
        resource = 0;
      }
      else {
        diag_u = y + x >> 1;
        diag_v = x - (y + x >> 1);
        if ((diag_u & 3) + (diag_v & 3) * 4 ==
            ((y + x >> 3) * 0xb + ((int)diag_v >> 2) * 0xd + (uint)map.seed & 0xf)) {
          resource = 1;
          bit_mask = 1 << ((byte)(map.seed >> 4) & 3);
          if ((bit_mask & diag_u) == (bit_mask & diag_v)) {
            resource = 2;
          }
        }
        else {
          resource = 0;
        }
      }
    }
    else {
      resource = 0;
    }
  }
  return resource;
}

/* 005b8ffa auto */
i32 __cdecl tile_has_hut(int x,int y)

{
  int check;
  i32 result;
  uint diag_v;
  
  check = map_in_bounds(x,y);
  if (check == 0) {
    result = 0;
  }
  else {
    check = tile_is_ocean(x,y);
    if (check == 0) {
      check = tile_owner(x,y);
      if (check < 0) {
        diag_v = x - (x + y >> 1);
        if ((x + y >> 1 & 3U) + (diag_v & 3) * 4 ==
            ((x + y >> 3) * 0xb + ((int)diag_v >> 2) * 0xd + (uint)map.seed + 8 & 0x1f))
        {
          result = 1;
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
      result = 0;
    }
  }
  return result;
}

/* 005b90df auto */
void __cdecl tile_add_pollution(iptr x,iptr y)

{
  int in_bounds;
  uint improvements2;
  int civ;
  
  in_bounds = map_in_bounds(x,y);
  if ((in_bounds != 0) && (improvements2 = tile_improvements(x,y), (improvements2 & TILE_POLLUTION) == 0)) {
    tile_set_improvements(x,y,0x80,1,1);
    for (civ = 1; civ < 8; civ++) {
      tile_update_known(x,y,civ);
    }
    game.polluted_tiles++;
  }
  return;
}

/* 005b9431 auto */
uint __cdecl landmass_borders_body(int landmass,iptr other_body)

{
  iptr byte_idx;
  uint bit_mask;
  
  bit_index_split(other_body,(iptr)&byte_idx,(iptr)&bit_mask);
  return (uint)((bit_mask & (byte)landmass_table_block.landmass_recs[landmass].border_bits[byte_idx]) != 0);
}

/* 005b947f auto */
int __cdecl landmass_count_bordering(iptr body)

{
  int borders;
  int landmass2;
  i32 count;
  
  count = 0;
  for (landmass2 = 1; landmass2 < 0x3f; landmass2++) {
    borders = landmass_borders_body(landmass2,body);
    if (borders != 0) {
      count++;
    }
  }
  return count;
}

/* 005b94d5 auto */
uint __cdecl tile_improvements(iptr x,iptr y)

{
  Tile *tile;
  
  tile = tile_at(x,y);
  return (uint)tile->improvements;
}

/* 005b94fc auto */
void __cdecl tile_set_improvements(iptr x,iptr y,iptr bits,iptr set,int sync)

{
  char old_value;
  Tile *tile;
  
  tile = tile_at(x,y);
  old_value = (char)tile->improvements;
  if (set == 0) {
    tile->improvements = tile->improvements & ~(byte)bits;
  }
  else {
    tile->improvements = tile->improvements | (byte)bits;
  }
  if (((sync != 0) && ((char)tile->improvements != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(0,x,y,bits,set,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_FEATURE_SET,0,x,y,bits,set,1,0,0,0);
      }
      else {
        net_send_message(NM_FEATURE_SET,0xff,x,y,bits,set,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b9646 auto */
void __cdecl tile_set_terrain(iptr x,iptr y,iptr terrain,iptr sync)

{
  byte old_value;
  byte *tile;
  
  tile = (byte *)tile_at(x,y);
  old_value = *tile;
  *tile = *tile & 0xf0;
  *tile = *tile | (byte)terrain;
  if (((sync != 0) && (*tile != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(1,x,y,terrain,0,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_TERRAIN_SET,0,x,y,terrain,1,0,0,0,0);
      }
      else {
        net_send_message(NM_TERRAIN_SET,0xff,x,y,terrain,0,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b976d auto */
void __cdecl tile_set_visible(iptr x,iptr y,iptr bits,iptr set,int sync)

{
  char old_value;
  Tile *tile;
  
  tile = tile_at(x,y);
  old_value = (char)tile->visible;
  if (set == 0) {
    tile->visible = tile->visible & ~(byte)bits;
  }
  else {
    tile->visible = tile->visible | (byte)bits;
  }
  if (((sync != 0) && ((char)tile->visible != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(2,x,y,bits,set,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_SEEN_SET,0,x,y,bits,set,1,0,0,0);
      }
      else {
        net_send_message(NM_SEEN_SET,0xff,x,y,bits,set,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b98b7 auto */
void __cdecl tile_set_fertility(iptr x,iptr y,iptr fertility,iptr sync)

{
  char old_value;
  Tile *tile;
  
  tile = tile_at(x,y);
  old_value = (char)tile->owner;
  tile->owner = tile->owner & 0xf0;
  tile->owner = tile->owner | (byte)fertility & 0xf;
  if (((sync != 0) && ((char)tile->owner != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(3,x,y,fertility,0,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_SITE_SET,0,x,y,fertility,1,0,0,0,0);
      }
      else {
        net_send_message(NM_SITE_SET,0xff,x,y,fertility,0,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b99e8 auto */
void __cdecl tile_set_owner(iptr x,iptr y,iptr owner,iptr sync)

{
  char old_value;
  Tile *tile;
  
  if ((owner < 0) || (8 < owner)) {
    owner = 0xf;
  }
  tile = tile_at(x,y);
  old_value = (char)tile->owner;
  tile->owner = tile->owner & 0xf;
  tile->owner = tile->owner | (byte)(owner << 4);
  if (((sync != 0) && ((char)tile->owner != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(4,x,y,owner,0,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_OWNER_SET,0,x,y,owner,1,0,0,0,0);
      }
      else {
        net_send_message(NM_OWNER_SET,0xff,x,y,owner,0,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b9b35 auto */
void __cdecl tile_set_landmass(iptr x,iptr y,iptr landmass,iptr sync)

{
  char old_value;
  Tile *tile;
  
  tile = tile_at(x,y);
  old_value = (char)tile->landmass;
  tile->landmass = (i8)landmass;
  if (((sync != 0) && ((char)tile->landmass != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(5,x,y,landmass,0,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_REGION_SET,0,x,y,landmass,1,0,0,0,0);
      }
      else {
        net_send_message(NM_REGION_SET,0xff,x,y,landmass,0,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b9c49 auto */
void __cdecl tile_set_radius_civ(iptr x,iptr y,iptr radius_civ,iptr sync)

{
  char old_value;
  Tile *tile;
  
  tile = tile_at(x,y);
  old_value = (char)tile->radius;
  tile->radius = tile->radius & 0x1f;
  tile->radius = (byte)((radius_civ & 7U) << 5) | tile->radius;
  if (((sync != 0) && ((char)tile->radius != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(6,x,y,radius_civ,0,0);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_CITY_USING_SET,0,x,y,radius_civ,1,0,0,0,0);
      }
      else {
        net_send_message(NM_CITY_USING_SET,0xff,x,y,radius_civ,0,0,0,0,0);
      }
    }
  }
  return;
}

/* 005b9d81 auto */
void __cdecl known_set(iptr x,iptr y,uptr bits,iptr civ,int merge,int sync)

{
  byte old_value;
  byte *known;
  
  known = known_at(x,y,civ);
  old_value = *known;
  if (merge == 0) {
    *known = (byte)bits;
  }
  else {
    *known = *known | (byte)bits;
  }
  if (((sync != 0) && (*known != old_value)) && (2 < game.multiplayer)) {
    if ((net.host_flag == '\0') && (net.tile_batch_active != '\0')) {
      net_batch_add(7,x,y,bits & 0xff,civ,merge);
    }
    else if (net.tile_sync_enabled != '\0') {
      if (net.host_flag == '\0') {
        net_send_message(NM_FEAT_LOC_SET,0,x,y,bits & 0xff,civ,merge,1,0,0);
      }
      else {
        net_send_message(NM_FEAT_LOC_SET,0xff,x,y,bits & 0xff,civ,merge,0,0,0);
      }
    }
  }
  return;
}
