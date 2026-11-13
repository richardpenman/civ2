/* The game, multiplayer messages: what the players send each other, checked on arrival, queued and dispatched: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* Receive boundaries (independent reconstruction BF-177/179/180/183/223/246/280,
 * then BF-130..133/137/167/169/171/178/196/203/205..211/222/227/228/233/240..278).
 * Memory safety applies in classic and network games too (docs/BUGS.md).
 * The captured size, not the packet's advertised size, bounds every read.
 * Each message type needs the bytes its handler reads (net_message_min_size,
 * from the senders: net_send_message, net_msg_build_packet and the raw
 * packets), and the peer's civ, unit, city and table indices that index our
 * arrays must lie inside them. Nothing the original sends is refused. */

/* The fewest bytes a message of this type has: what its handler reads. */
static uint net_message_min_size(int type)
{
  switch(type) {
  case NM_DATAGRAM:
    return sizeof(NetNamedMsg);
  case NM_PLAYER_LIST:
    return sizeof(NetPlayerListMsg);
  case NM_SERVER_PATH:
    return 0x21c;
  case NM_SERVER_ANSWER:
    return sizeof(NetServerAnswerMsg);
  case NM_PLAYER_INFO:
    return sizeof(NetPlayerInfoMsg);
  case NM_PLAYER_LIST_NODE:
    return sizeof(NetPlayerNodeMsg);
  case NM_START_WHOSE_TURN:
    return sizeof(NetWhoseTurnMsg);
  /* the rule tables: the arguments, then the table */
  case NM_COSMIC:
    return sizeof(NetMsg) + 0x29;
  case NM_CIVILIZE:
    return sizeof(NetMsg) + 0x640;
  case NM_IMPROVE:
    return sizeof(NetMsg) + 0x218;
  case NM_ENDWONDER:
  case NM_GOVT:
    return sizeof(NetMsg) + 0x1c;
  case NM_UNITS:
    return sizeof(NetMsg) + 0x4d8;
  case NM_TERRAIN:
    return sizeof(NetMsg) + 0x318;
  case NM_GOVT_TITLE:
    return sizeof(NetMsg) + 0x38;
  case NM_LEADERS:
    return sizeof(NetMsg) + 0x3f0;
  case NM_KINGS:
    return sizeof(NetMsg) + 0x7524;
  case NM_CARAVAN:
    return sizeof(NetMsg) + 0x40;
  case NM_ORDERS:
    return sizeof(NetMsg) + 0x68;
  case NM_DIFFICULTY:
    return sizeof(NetMsg) + 0x18;
  case NM_ATTITUDES:
    return sizeof(NetMsg) + 0x24;
  /* a city or unit record after the arguments */
  case NM_CITY_DELETED:
    return sizeof(NetMsg) + 0x58;
  case NM_SLIDE_PIECE:
    return sizeof(NetMsg) + 0x20;
  /* the arguments (net_msg_get_args) or a header past them */
  case NM_FULL_GAMESTATE: case NM_DIFF_GAMESTATE: case NM_JOIN_GAMESTATE: case NM_MAP_STRUCT:
  case NM_DIFFERENCE_ENGINE: case NM_ALPHA_DIFFERENCE_ENGINE: case NM_MAP_PACKETS:
  case NM_EVENTSLOADED: case NM_GAME_GOING: case NM_LOCK_DATA: case NM_QUIT_REQUEST:
  case NM_LOCK_NATION: case NM_UNLOCK_NATION: case NM_MULTIOPTIONS_CHANGE: case NM_KILL_OR_QUIT:
  case NM_DELETE_UNIT: case NM_UNIT_DELETED: case NM_DELETE_CITY: case NM_CREATE_CITY:
  case NM_CITY_CREATED: case NM_CREATE_UNIT: case NM_UNIT_CREATED: case NM_PICK_UP_UNIT:
  case NM_PUT_DOWN_UNIT: case NM_MOVE_TO_BOTTOM: case NM_RELOCATE_UNIT: case NM_ACQUIRE_WONDER:
  case NM_WONDER_ACQUIRED: case NM_STACK_SHIP: case NM_SHIP_STACKED: case NM_STACK_UNIT:
  case NM_DELETE_SAFELY: case NM_DELETE_VISIBLY: case NM_LOCK_MAP: case NM_UNLOCK_MAP:
  case NM_LOCK_MAP_ACK: case NM_UNLOCK_MAP_ACK: case NM_LOCK_MAP_SYNC: case NM_NEWTURNTIMER_REPLY:
  case NM_PMCHANGE_REPLY: case NM_REALTIME_STACKER: case NM_NEW_CIV: case NM_KILL_CIV:
  case NM_NEW_CIV_ACK: case NM_KILL_CIV_ACK: case NM_BRIBED_UNIT: case NM_DEATH_STACK:
  case NM_POPUP: case NM_END_SEQUENCE: case NM_TIMER_CHANGE: case NM_POSSIBLY_CENTER:
  case NM_DRAW_SQUARE: case NM_DRAW_TWO: case NM_DRAW_AREA: case NM_DRAW_MAP_REGION:
  case NM_CANESCAPE_REPLY: case NM_SOUND_PLAY: case NM_DESERT_REPLY: case NM_KABOOM:
  case NM_SHOW_NUKE: case NM_ASK_ALLY: case NM_REPORTS_UPDATE: case NM_PARLEYREQUEST_REPLY:
  case NM_CITYWIN_CHECK_UNIT: case NM_CITYWIN_CHECK_CITY: case NM_CITYWIN_DELETED:
  case NM_PRODUCTION_COMPLETE: case NM_HUMAN_MOVE_COMPLETE: case NM_FEATURE_SET:
  case NM_TERRAIN_SET: case NM_SEEN_SET: case NM_SITE_SET: case NM_OWNER_SET: case NM_REGION_SET:
  case NM_CITY_USING_SET: case NM_FEAT_LOC_SET: case NM_SET_ATTITUDE: case NM_DO_AIPARLEY:
  case NM_SET_SLIDE_SOUND: case NM_SET_CIV: case NM_PICK_GOVT: case NM_POLITICAL_VIOLATIONS:
  case NM_POLITICAL_ATTITUDE: case NM_TURN_FINISHED: case NM_PRETEXT: case NM_REVEAL_CITY_INFO:
  case NM_HIDE_CITY_INFO: case NM_REVEAL_CITY_ORIGIN: case NM_HIDE_CITY_ORIGIN:
    return sizeof(NetMsg);
  /* a parley offer: its header and first item (net_parley_offer_valid) */
  case NM_PARLEY_TRANSACTION:
  case NM_PARLEY_EXECUTE:
    return sizeof(ParleyOfferMsg);
  default:
    /* the header only (header_init) */
    return 16;
  }
}

/* A parley offer's item: inside the bytes left, and its ids too (the kinds
   the sender lists, ParleyWindow::pack_offer_items). */
static int net_parley_item_valid(const u8 *item_ptr,uint avail)
{
  ParleyOfferItem item;
  uint per_id;

  if (avail < sizeof item) {
    return 0;
  }
  memcpy(&item,item_ptr,sizeof item);
  if ((uint)item.size < sizeof item || avail < (uint)item.size) {
    return 0;
  }
  per_id = 0;
  if ((item.type == 8) || (item.type == 10) || (item.type == 0xc)) {
    per_id = 4;
  }
  else if (item.type == 0x11) {
    per_id = 8;
  }
  if ((per_id != 0) && ((item.size - sizeof item) / per_id < (uint)item.value)) {
    return 0;
  }
  return 1;
}

/* BF-178: a parley offer (0x82, sent on as 0x86): the first item after the
   0x20-byte header, and for an exchange (mode 0xe) the second item
   item.size bytes further (ParleyWindow::send_offer, build_description,
   parley_execute_transaction). */
static int net_parley_offer_valid(const void *data,uint size)
{
  const ParleyOfferMsg *offer;
  uint first;

  offer = (const ParleyOfferMsg *)data;
  if (!net_parley_item_valid((const u8 *)data + 0x20,size - 0x20)) {
    return 0;
  }
  if (offer->mode == 0xe) {
    first = (uint)offer->item.size;
    if (!net_parley_item_valid((const u8 *)data + 0x20 + first,size - 0x20 - first)) {
      return 0;
    }
  }
  return 1;
}

/* BF-130..133, BF-241, BF-275: a popup for us (0x6a, net_send_text_args):
   the payload is all the rest (extra_len), its strings and numbers fit
   text_args, and the argument a popup kind draws with (net_popup_show:
   entry[4] is args[3], entry[5] args[4]) indexes inside its table. */
static int net_popup_args_valid(const NetMsg *msg,uint size)
{
  const char *text;
  const char *end;
  const char *nul;
  int i;
  int kind;
  int arg_a;
  int arg_b;

  if ((uint)msg->args[6] != size - sizeof(NetMsg) ||
      (uint)msg->args[1] > 24 || (uint)msg->args[2] > 6) {   /* text_args: 24 strings, 6 numbers */
    return 0;
  }
  text = (const char *)msg + sizeof(NetMsg);
  end = text + msg->args[6];
  for (i = 0; i < msg->args[1]; i++) {
    nul = (const char *)memchr(text,0,(uint)(end - text) < 0x104 ? (uint)(end - text) : 0x104);
    if (nul == 0) {
      return 0;
    }
    text = nul + 1;
  }
  if ((uint)(end - text) < (uint)msg->args[2] * 4) {
    return 0;
  }
  kind = msg->args[0];
  arg_a = msg->args[3];
  arg_b = msg->args[4];
  switch(kind) {
  case 0xb: case 0xe: case 0x17: case 0x3d: case 0x57: case 0x5a:   /* a civ */
    return (uint)arg_a < 8;
  case 0x23: case 0x24:                                             /* a civ and a unit */
    return (uint)arg_a < 8 && (uint)arg_b < 0x800;
  case 0x1b: case 0x1c: case 0x1d: case 0x1e:                       /* a civ and a city */
    return (uint)arg_a < 8 && (uint)arg_b < 0x100;
  case 2: case 0xf: case 0x10: case 0x15: case 0x1f: case 0x20: case 0x22: case 0x25:
  case 0x2d: case 0x2e: case 0x2f: case 0x30: case 0x33: case 0x34: case 0x4e: case 0x51:
  case 0x52: case 0x5f: case 0x60: case 0x61:                       /* a unit */
    return (uint)arg_a < 0x800;
  case 0x11: case 0x26: case 0x27: case 0x28: case 0x37: case 0x3e: /* a unit sprite (0x3e: ransom) */
    return (uint)arg_a < 0x3f;
  case 0x12: case 0x13: case 0x14: case 0x19: case 0x1a: case 0x31: case 0x32:
  case 0x4a: case 0x4b: case 0x4c:                                  /* an improvement or wonder */
    return (uint)arg_a < 0x43;
  case 0x59:                                                        /* an improvement and a civ */
    return (uint)arg_a < 0x43 && (uint)arg_b < 8;
  case 0x47:                                                        /* a wonder, else an improvement */
    return arg_a < 0 ? (uint)arg_b < 0x43 : arg_a < 0x1c;
  case 0x4d:                                                        /* an advance and a wonder */
    return (uint)arg_a < 100 && (uint)arg_b < 0x43;
  case 99:                                                          /* an advance */
    return (uint)arg_a < 100;
  case 0x2b: case 0x2c:                                             /* a government */
    return (uint)arg_a < 7;
  case 0x18: case 0x46:                                             /* a city */
    return (uint)arg_a < 0x100;
  }
  return 1;
}

/* BF-213, BF-216, BF-218, BF-221: a rule table from the host: the advance,
   category and epoch and terrain indices in it inside their tables (an
   advance -2..99: -1 nil, -2 no; a terrain -2..10). The name ordinals are
   not checked here. */
static int net_rule_table_valid(int type,const u8 *table)
{
  int i;

  switch(type) {
  case NM_CIVILIZE:
    for (i = 0; i < 100; i++) {
      const u8 *advance = table + i * 0x10;
      if ((i8)advance[0xe] < -2 || 99 < (i8)advance[0xe] ||
          (i8)advance[0xf] < -2 || 99 < (i8)advance[0xf] ||
          4 < advance[0xc] || 3 < advance[0xd]) {
        return 0;
      }
    }
    break;
  case NM_IMPROVE:
    for (i = 0; i < 0x43; i++) {
      if ((i8)table[i * 8 + 6] < -2 || 99 < (i8)table[i * 8 + 6]) {
        return 0;
      }
    }
    break;
  case NM_ENDWONDER:
    for (i = 0; i < 0x1c; i++) {
      if ((i8)table[i] < -2 || 99 < (i8)table[i]) {
        return 0;
      }
    }
    break;
  case NM_UNITS:
    for (i = 0; i < 0x3e; i++) {
      if ((i8)table[i * 0x14 + 8] < -2 || 99 < (i8)table[i * 0x14 + 8] ||
          (i8)table[i * 0x14 + 0x13] < -2 || 99 < (i8)table[i * 0x14 + 0x13]) {
        return 0;
      }
    }
    break;
  case NM_TERRAIN:
    for (i = 0; i < 0xb; i++) {   /* the base terrains carry the changes */
      if ((i8)table[i * 0x18 + 0xd] < -2 || 10 < (i8)table[i * 0x18 + 0xd] ||
          (i8)table[i * 0x18 + 0xe] < -2 || 10 < (i8)table[i * 0x18 + 0xe] ||
          (i8)table[i * 0x18 + 0xf] < -2 || 10 < (i8)table[i * 0x18 + 0xf]) {
        return 0;
      }
    }
  }
  return 1;
}

/* A message's body: its strings terminated, its records inside it and the
   indices its handler uses inside their arrays (civs 0..7, units 0..2047,
   cities 0..255). */
static int net_message_body_valid(const void *data,uint size,int type)
{
  const NetMsg *msg;
  const u8 *body;
  uint left;
  uint pos;
  int count;
  i32 record[3];

  msg = (const NetMsg *)data;
  body = (const u8 *)data + sizeof(NetMsg);
  left = size - sizeof(NetMsg);
  switch(type) {
  case NM_SERVER_PATH:   /* BF-222: name[260] at 0x12, subdir[262] at 0x116 */
    return memchr((const u8 *)data + 0x12,0,0x104) != 0 && memchr((const u8 *)data + 0x116,0,0x106) != 0;
  case NM_SERVER_ANSWER: {   /* BF-247: what recv_server_info copies */
    const NetGameInfo *info = &((const NetServerAnswerMsg *)data)->game;
    return memchr(info->host_address,0,sizeof info->host_address) != 0 &&
           memchr(info->version,0,32) != 0 &&   /* into the 32-byte server_version */
           memchr(info->game_name,0,sizeof info->game_name) != 0 &&
           memchr(info->host_name,0,sizeof info->host_name) != 0;
  }
  case NM_PLAYER_LIST_NODE:   /* BF-248: the name add_player copies */
    return memchr(((const NetPlayerNodeMsg *)data)->slot.name,0,0x20) != 0;
  case NM_PLAYER_INFO: {   /* BF-171, BF-249: net_host_register_player_civ */
    const NetPlayerInfoMsg *info = (const NetPlayerInfoMsg *)data;
    if ((uint)info->civ >= 8 || (uint)info->leader_index >= 0x15 || (uint)info->gender >= 2 ||
        (uint)info->city_style >= 4 ||
        !memchr(info->leader_name,0,sizeof info->leader_name) ||
        !memchr(info->tribe_name,0,sizeof info->tribe_name) ||
        !memchr(info->adjective,0,sizeof info->adjective)) {
      return 0;
    }
    for (pos = 0; pos < 7; pos++) {
      if (!memchr(info->govt_titles[pos],0,sizeof info->govt_titles[pos])) {
        return 0;
      }
    }
    return 1;
  }
  case NM_START_WHOSE_TURN:   /* BF-203, BF-278 */
    return (uint)((const NetWhoseTurnMsg *)data)->civ < 8;
  case NM_CIVILIZE: case NM_IMPROVE: case NM_ENDWONDER: case NM_UNITS: case NM_TERRAIN:
    return net_rule_table_valid(type,body);
  case NM_DIFFERENCE_ENGINE:
  case NM_ALPHA_DIFFERENCE_ENGINE:   /* BF-277: the records (recv_state_diff) */
    pos = 0;
    for (count = msg->args[0]; 0 < count; count--) {
      if (left - pos < sizeof record) {
        return 0;
      }
      memcpy(record,body + pos,sizeof record);
      pos += sizeof record;
      if (left - pos < (uint)record[2]) {
        return 0;
      }
      pos += (uint)record[2];
    }
    return 1;
  case NM_MAP_PACKETS:   /* BF-243: the records (net_apply_tile_updates) */
    if (left < 4) {
      return 0;
    }
    memcpy(&count,body,4);
    pos = 4;
    for (; 0 < count; count--) {
      if (left - pos < 4) {
        return 0;
      }
      memcpy(record,body + pos,4);
      if ((uint)record[0] >= 8 || (left - pos) / 4 < (uint)net_batch.record_sizes[record[0]]) {
        return 0;
      }
      if (record[0] == 7) {   /* known_set: x, y, bits, civ (its map of what the civ knows) */
        memcpy(record,body + pos + 16,4);
        if ((uint)record[0] >= 8) {
          return 0;
        }
      }
      pos += net_batch.record_sizes[record[0]] * 4;
    }
    return 1;
  case NM_POPUP:
    return net_popup_args_valid(msg,size);
  case NM_PARLEY_TRANSACTION:
  case NM_PARLEY_EXECUTE:
    return net_parley_offer_valid(data,size);
  case NM_EVENTSLOADED:   /* BF-240: 110 events fill the 50000-byte pool */
    return (uint)msg->args[0] < 2 && (uint)msg->args[1] <= 50000 / 0x1c4;
  case NM_QUIT_REQUEST: case NM_KILL_OR_QUIT: case NM_NEW_CIV: case NM_LOCK_MAP: case NM_UNLOCK_MAP:
  case NM_LOCK_MAP_ACK: case NM_UNLOCK_MAP_ACK: case NM_LOCK_MAP_SYNC: case NM_PRODUCTION_COMPLETE:
  case NM_HUMAN_MOVE_COMPLETE: case NM_TURN_FINISHED:
    return (uint)msg->args[0] < 8;
  case NM_KILL_CIV:   /* the conqueror may be -1 */
    return (uint)msg->args[0] < 8 && -1 <= msg->args[1] && msg->args[1] < 8;
  case NM_SET_ATTITUDE: case NM_DO_AIPARLEY: case NM_POLITICAL_VIOLATIONS:
  case NM_POLITICAL_ATTITUDE: case NM_PRETEXT:
    return (uint)msg->args[0] < 8 && (uint)msg->args[1] < 8;
  case NM_CREATE_CITY:   /* x, y, owner */
    return (uint)msg->args[2] < 8;
  case NM_CREATE_UNIT:   /* type, owner, x, y */
    return (uint)msg->args[0] < 0x3e && (uint)msg->args[1] < 8;
  case NM_DELETE_CITY:   /* city_delete ignores negatives */
    return msg->args[0] < 0x100;
  case NM_CITY_DELETED:   /* BF-233: the city record copied into cities[] */
    return (uint)msg->args[0] < 0x100;
  case NM_CITY_CREATED:   /* -1, or the city record and the city count */
    if (msg->args[0] == -1) {
      return 1;
    }
    return left >= 0x58 && (uint)msg->args[0] < 0x100 && (uint)msg->args[1] <= 0x100;
  case NM_UNIT_CREATED:   /* -1, or the unit and the unit count */
    return -1 <= msg->args[0] && msg->args[0] < 0x800 && (msg->args[0] == -1 || (uint)msg->args[1] <= 0x800);
  case NM_DELETE_UNIT: case NM_STACK_UNIT:   /* unit_delete, unit_lift_off_map: -1 is none */
    return -1 <= msg->args[0] && msg->args[0] < 0x800;
  case NM_PICK_UP_UNIT: case NM_PUT_DOWN_UNIT: case NM_MOVE_TO_BOTTOM: case NM_RELOCATE_UNIT:
  case NM_STACK_SHIP: case NM_DELETE_SAFELY: case NM_DELETE_VISIBLY:
    return (uint)msg->args[0] < 0x800;
  case NM_ACQUIRE_WONDER:   /* civ, city, wonder, cost, improvement */
    return (uint)msg->args[0] < 8 && (uint)msg->args[1] < 0x100 && (uint)msg->args[2] < 0x1c &&
           (uint)msg->args[4] < 0x43;
  case NM_REALTIME_STACKER:   /* civ, the seeing civ (-1: none; only a bit of it is used), unit */
    return (uint)msg->args[0] < 8 && (uint)msg->args[2] < 0x800;
  case NM_BRIBED_UNIT:   /* unit, new owner */
    return (uint)msg->args[0] < 0x800 && (uint)msg->args[1] < 8;
  case NM_DEATH_STACK:   /* the unit lost and its killer */
    return (uint)msg->args[0] < 0x800 && (uint)msg->args[1] < 0x800;
  case NM_SET_CIV:   /* civ_give_advance: civ, advance, from whom (< 0: none) */
    return (uint)msg->args[0] < 8 && (uint)msg->args[1] < 100 && msg->args[2] < 8;
  case NM_ASK_ALLY:   /* diplo_ally_demand_help: the ally, its enemy */
    return (uint)msg->args[0] < 8 && (uint)msg->args[1] < 8;
  case NM_SLIDE_PIECE:   /* unit, x, y, direction (-1: a paradrop with none), then the unit's record */
    return (uint)msg->args[0] < 0x800 && -1 <= msg->args[3] && msg->args[3] < 8 &&
           body[6] < 0x3e && body[7] < 8;
  case NM_CITYWIN_CHECK_UNIT:   /* a unit (< 0: none), x, y */
    return msg->args[0] < 0x800;
  case NM_FEAT_LOC_SET:   /* known_set: x, y, bits, civ (the coordinates where it is applied) */
    return (uint)msg->args[3] < 8;
  }
  return 1;
}

static int net_received_message_valid(const void *data,uint size)
{
  u32 header[4];
  if (data == 0 || size < sizeof header) {
    return 0;
  }
  memcpy(header,data,sizeof header);
  if (header[0] != 0x66606660 ||
      header[1] >= sizeof net.nm_recv_counts_by_type / sizeof net.nm_recv_counts_by_type[0]) {
    return 0;
  }
  if (size < net_message_min_size((int)header[1]) ||
      !net_message_body_valid(data,size,(int)header[1])) {
    return 0;
  }
  if (header[1] == NM_DATAGRAM) {
    const NetNamedMsg *named = (const NetNamedMsg *)data;
    if ((uint)named->inner_type >= sizeof net.nm_recv_counts_by_type / sizeof net.nm_recv_counts_by_type[0] ||
        !memchr(named->address,0,sizeof named->address) ||
        !memchr(named->name,0,sizeof named->name) ||
        !memchr(named->address2,0,sizeof named->address2)) {
      return 0;
    }
    if (named->inner_type == NM_SERVER_INFO) {
      if (size < sizeof(NetServerInfoMsg)) {
        return 0;
      }
      const NetGameInfo *info = &((const NetServerInfoMsg *)data)->game;
      if (!memchr(info->host_address,0,sizeof info->host_address) ||
          !memchr(info->version,0,sizeof info->version) ||
          !memchr(info->game_name,0,sizeof info->game_name) ||
          !memchr(info->host_name,0,sizeof info->host_name) ||
          !memchr(info->scenario_name,0,sizeof info->scenario_name)) {
        return 0;
      }
    }
  }
  if (header[1] == NM_CHAT) {
    if (size <= sizeof(NetMsg) || size - sizeof(NetMsg) > 257) {
      return 0;
    }
    const NetMsg *msg = (const NetMsg *)data;
    if ((uint)msg->args[0] >= 8 || (uint)msg->args[1] >= 8 ||
        !memchr((const char *)data + sizeof(NetMsg),0,size - sizeof(NetMsg))) {
      return 0;
    }
  }
  return 1;
}

/* 0046af70 auto */
i8 NetSession::send_to(int player,NetMsg *msg)

{
  i8 ok;
  int sent;
  
  if ((((player < 0) || (7 < player)) && (player != 0xff)) && (player != 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_sock_0_sock_max_net_players_sock_broadca,(const char*)&str_d_ss_franklinton_netmgr_send_cpp,0x139);
  }
  if (msg->size == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pmsg_size_0,(const char*)&str_d_ss_franklinton_netmgr_send_cpp_2,0x13a);
  }
  msg->seq = net_send_sequence;
  net_send_sequence++;
  sent = XD_SendSecureData((ushort)player,(void *)msg,(ulong)msg->size,0);
  last_send_result = sent;
  if (last_send_result < 0) {
    dbg_logf3((const char *)&str_sendtoconnections_failed_on_send_to_conn,player,last_send_result,(i32)msg->hdr.msg_type);
    if ((int)msg->hdr.msg_type == NM_POPUP) {
      dbg_logf_str((const char *)&str_popup_type_s,(const char *)(((char *)&str_nm_pop_retiredie) + msg->args[0] * 0x20))
      ;
    }
    ok = 0;
  }
  else {
    ok = 1;
  }
  return ok;
}

/* 0046b0a1 auto */
bool NetSession::send_to_all(NetMsg *msg)

{
  bool failed;
  int sent;
  
  msg->seq = net_send_sequence;
  net_send_sequence++;
  sent = XD_SendBroadcastData((void *)msg,(ulong)msg->size,0);
  last_send_result = sent;
  failed = last_send_result < msg->size;
  if (failed) {
    dbg_logf((const char *)&str_sendtoeveryone_failed_return_val_d,last_send_result);
  }
  return !failed;
}

/* Part of net_send_message (0046b14d): the messages sent as a packet of
   their arguments (and a record or table where there is one,
   net_msg_build_packet): the packet, NULL if it could not be built.  KNOWN:
   0 for a type that is not one of them. */
static void *net_send_build_packet(int msg_type,i32 arg1,i32 arg2,i32 arg3,i32 arg4,i32 arg5,i32 arg6,
                                   const char *text,NetMsg *raw_msg,int &known)
{
  size_t text_len;
  void *packet;

  packet = (void *)0x0;
  known = 1;
  switch(msg_type) {
  case NM_COSMIC:
    packet = net_msg_build_packet(msg_type,&cosmic,0,0,0,0,0,0,0x29);
    break;
  case NM_CIVILIZE:
    packet = net_msg_build_packet(msg_type,&advances,0,0,0,0,0,0,0x640);
    break;
  case NM_IMPROVE:
    packet = net_msg_build_packet(msg_type,&improvements,0,0,0,0,0,0,0x218);
    break;
  case NM_ENDWONDER:
    packet = net_msg_build_packet(msg_type,((u8 *)wonder_expires),0,0,0,0,0,0,0x1c);
    break;
  case NM_UNITS:
    packet = net_msg_build_packet(msg_type,((u8 *)unit_types),0,0,0,0,0,0,0x4d8);
    break;
  case NM_TERRAIN:
    packet = net_msg_build_packet(msg_type,&terrain_types,0,0,0,0,0,0,0x318);
    break;
  case NM_GOVT:
    packet = net_msg_build_packet(msg_type,&government_names,0,0,0,0,0,0,0x1c);
    break;
  case NM_GOVT_TITLE:
    packet = net_msg_build_packet(msg_type,((u8 *)government_titles),0,0,0,0,0,0,0x38);
    break;
  case NM_LEADERS:
    packet = net_msg_build_packet(msg_type,((u8 *)leaders),0,0,0,0,0,0,0x3f0);
    break;
  case NM_KINGS:
    packet = net_msg_build_packet(msg_type,&civs,0,0,0,0,0,0,0x7524);
    break;
  case NM_CARAVAN:
    packet = net_msg_build_packet(msg_type,&commodity_names,0,0,0,0,0,0,0x40);
    break;
  case NM_ORDERS:
    packet = net_msg_build_packet(msg_type,((u8 *)order_names),0,0,0,0,0,0,0x68);
    break;
  case NM_DIFFICULTY:
    packet = net_msg_build_packet(msg_type,((u8 *)difficulty_names),0,0,0,0,0,0,0x18);
    break;
  case NM_ATTITUDES:
    packet = net_msg_build_packet(msg_type,&attitude_names,0,0,0,0,0,0,0x24);
    break;
  case NM_EVENTSLOADED:
  case NM_STACK_SHIP:
  case NM_LOCK_MAP_ACK:
  case NM_KILL_CIV:
  case NM_DRAW_SQUARE:
  case NM_DRAW_AREA:
  case NM_SOUND_PLAY:
  case NM_SHOW_NUKE:
  case NM_ASK_ALLY:
  case NM_SET_SLIDE_SOUND:
  case NM_POLITICAL_VIOLATIONS:
  case NM_POLITICAL_ATTITUDE:
  case NM_PRETEXT:
  case NM_REVEAL_CITY_INFO:
  case NM_HIDE_CITY_INFO:
  case NM_REVEAL_CITY_ORIGIN:
  case NM_HIDE_CITY_ORIGIN:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_GAME_GOING:
  case NM_QUIT_REQUEST:
  case NM_KILL_OR_QUIT:
  case NM_DELETE_UNIT:
  case NM_DELETE_CITY:
  case NM_PICK_UP_UNIT:
  case NM_STACK_UNIT:
  case NM_DELETE_SAFELY:
  case NM_DELETE_VISIBLY:
  case NM_MOVE_TO_BOTTOM:
  case NM_WONDER_ACQUIRED:
  case NM_SHIP_STACKED:
  case NM_UNLOCK_MAP:
  case NM_UNLOCK_MAP_ACK:
  case NM_NEWTURNTIMER_REPLY:
  case NM_PMCHANGE_REPLY:
  case NM_NEW_CIV:
  case NM_NEW_CIV_ACK:
  case NM_KILL_CIV_ACK:
  case NM_TIMER_CHANGE:
  case NM_CANESCAPE_REPLY:
  case NM_DESERT_REPLY:
  case NM_REPORTS_UPDATE:
  case NM_PARLEYREQUEST_REPLY:
  case NM_CITYWIN_CHECK_CITY:
  case NM_CITYWIN_DELETED:
  case NM_PRODUCTION_COMPLETE:
  case NM_HUMAN_MOVE_COMPLETE:
  case NM_PICK_GOVT:
  case NM_TURN_FINISHED:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_LOCK_DATA:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    net.nm_lock_data_count++;
    break;
  case NM_MULTIOPTIONS_CHANGE:
  case NM_CREATE_CITY:
  case NM_PUT_DOWN_UNIT:
  case NM_RELOCATE_UNIT:
  case NM_BRIBED_UNIT:
  case NM_END_SEQUENCE:
  case NM_POSSIBLY_CENTER:
  case NM_KABOOM:
  case NM_CITYWIN_CHECK_UNIT:
  case NM_SET_ATTITUDE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_UNIT_DELETED:
    packet = net_msg_build_packet(msg_type,(units + arg1),arg1,(int)game.num_units,arg3,arg4,arg5,arg6,0x20);
    break;
  case NM_CITY_DELETED:
    packet = net_msg_build_packet(msg_type,(cities + arg1),arg1,(int)game.num_cities,arg3,arg4,arg5,arg6,0x58);
    break;
  case NM_CITY_CREATED:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(cities + arg1),arg1,(int)game.num_cities,arg3,arg4,arg5,arg6,0x58);
    }
    break;
  case NM_CREATE_UNIT:
  case NM_DRAW_TWO:
  case NM_DRAW_MAP_REGION:
  case NM_TERRAIN_SET:
  case NM_SITE_SET:
  case NM_OWNER_SET:
  case NM_REGION_SET:
  case NM_CITY_USING_SET:
  case NM_DO_AIPARLEY:
  case NM_SET_CIV:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_UNIT_CREATED:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(units + arg1),arg1,(int)game.num_units,arg3,arg4,arg5,arg6,0x20);
    }
    break;
  case NM_ACQUIRE_WONDER:
  case NM_LOCK_MAP:
  case NM_LOCK_MAP_SYNC:
  case NM_DEATH_STACK:
  case NM_FEATURE_SET:
  case NM_SEEN_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_MAP_PACKETS:
    packet = net_msg_build_packet(msg_type,&net_batch_buffer,0,0,0,0,0,0,net_batch.used_dwords << 2);
    break;
  case NM_REALTIME_STACKER:
  case NM_FEAT_LOC_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,arg6,0);
    break;
  case NM_CHAT:
    text_len = _strlen((char *)text);
    packet = net_msg_build_packet(msg_type,text,arg1,arg2,arg3,0,0,0,text_len + 1);
    break;
  case NM_POPUP:
    packet = net_msg_build_packet(msg_type,(const void *)raw_msg,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_SLIDE_PIECE:
    packet = net_msg_build_packet(msg_type,(units + arg1),arg1,arg2,arg3,arg4,arg5,arg6,0x20);
    break;
  default:
    known = 0;
  }
  return packet;
}

/* Part of net_send_message (0046b14d): a message that is only its header. */
static int net_send_header_message(int msg_type,i32 dest)
{
  char sent;
  NetMsgHeader msg[2];

  msg->header_init(msg_type);
  sent = net.send_to(dest,(NetMsg *)msg);
  return (int)sent;
}

/* 0046b14d auto */
/* WARNING: Removing unreachable block (ram,0x0046cdce) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl
net_send_message(int msg_type,i32 dest,i32 arg1,i32 arg2,i32 arg3,i32 arg4,
            i32 arg5,i32 arg6,const char *text,NetMsg *raw_msg)

{
  char sent;
  NetWhoseTurnMsg whose_turn_msg[1];
  RECT caption_rect;
  NetServerAnswerMsg answer_msg[1];
  uint image_result;
  void *image_buf;
  NetPlayerNodeMsg player_node_msg[1];
  NetPlayerListMsg player_list_msg[1];
  /* (the original's locals lay so: the message whole) */
  struct { NetMsgHeader hdr[2]; i16 kind; i8 name[260]; i8 subdir[262]; } server_path_msg[1];
  NetServerInfoMsg server_info_msg[1];
  NetMsgHeader announce_msg[15];
  void *packet;
  int result;
  int known;
  
  packet = (void *)0x0;
  switch(msg_type) {
  case NM_GET_SERVERS:
    ((NetNamedMsg *)announce_msg)->init_header(msg_type,(const char *)&str_empty_80);
    sent = net.send_to_all((NetMsg *)announce_msg);
    return (int)sent;
  case NM_SERVER_INFO:
    _memset(server_info_msg,0,sizeof server_info_msg);   /* (BF-293) no stack bytes in the gaps */
    ((NetServerInfoMsg *)server_info_msg)->init();
    server_info_msg->game.session_phase = (i16)net.session_phase;
    sent = net.send_to_all((NetMsg *)server_info_msg);
    return (int)sent;
  case NM_PLAYER_LIST:
    ((NetPlayerListMsg *)player_list_msg)->init();
    crt_memcpy(&player_list_msg->player_count,((iptr *)&net.net_player_count),0x270);
    sent = net.send_to(dest,(NetMsg *)player_list_msg);
    return (int)sent;
  case NM_SERVER_PATH:
    _memset(server_path_msg,0,sizeof server_path_msg);   /* (BF-286) no stack bytes after the names */
    server_path_msg->hdr->server_path_init();
    server_path_msg->kind = net.loaded_file_kind;
    crt_strcpy(server_path_msg->name,((i8 *)&net.loaded_file_name));
    crt_strcpy(server_path_msg->subdir,((i8 *)&net.loaded_file_subdir));
    sent = net.send_to(dest,(NetMsg *)server_path_msg);
    return (int)sent;
  case NM_FULL_GAMESTATE:
  case NM_DIFF_GAMESTATE:
  case NM_JOIN_GAMESTATE:
  case NM_MAP_STRUCT:
    image_buf = (void *)0x0;
    if (msg_type == NM_FULL_GAMESTATE) {
      image_result = diffeng_build_full_image(&image_buf);
    }
    else if (msg_type == NM_DIFF_GAMESTATE) {
      image_result = diffeng_build_delta_image(&image_buf);
    }
    else if (msg_type == NM_JOIN_GAMESTATE) {
      image_result = diffeng_build_civ_image(&image_buf);
    }
    else if (msg_type == NM_MAP_STRUCT) {
      image_result = diffeng_build_map_header_image(&image_buf);
    }
    image_result = (uint)(image_result != -1);
    if (image_buf == (void *)0x0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_address_null,(const char*)&str_d_ss_franklinton_netmgr_send_cpp_3,0x22c);
    }
    image_buf = net_msg_build_packet(msg_type,image_buf,image_result,0,0,0,0,0,0);
    sent = net.send_to(dest,(NetMsg *)image_buf);
    image_result = (uint)sent;
    operator_delete(image_buf);
    return image_result;
  case NM_SERVER_ANSWER:
    _memset(answer_msg,0,sizeof answer_msg);   /* (BF-286) no stack bytes in the gaps */
    ((NetServerAnswerMsg *)answer_msg)->init();
    sent = net.send_to(dest,(NetMsg *)answer_msg);
    return (int)sent;
  case NM_DIFFERENCE_ENGINE:
  case NM_ALPHA_DIFFERENCE_ENGINE:
  case NM_PARLEY_TRANSACTION:
  case NM_PARLEY_EXECUTE:
    sent = net.send_to(dest,raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return raw_msg->size;
  case NM_PLAYER_INFO:
    sent = net.send_to(dest,(NetMsg *)&net.ping_message);
    return (int)sent;
  case NM_PLAYER_LIST_NODE:
    _memset(player_node_msg,0,sizeof player_node_msg);   /* (BF-286) no stack bytes after the name */
    ((NetPlayerNodeMsg *)player_node_msg)->init(arg1);
    sent = net.send_to(dest,(NetMsg *)player_node_msg);
    return (int)sent;
  case NM_LOCK_NATION:
    if (net.host_flag == '\0') {
      packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
      break;
    }
    if ((1 << ((byte)arg2 & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) == 0) {
      (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) | (ushort)(1 << ((byte)arg2 & 0x1f));
      net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
      net.nm_nation_accept_count++;
      return 1;
    }
    net.nm_nation_reject_count++;
  case NM_UNLOCK_NATION:
    if (net.host_flag != '\0') {
      game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
      if ((1 << ((byte)arg2 & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
        (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)arg2 & 0x1f));
        game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
        net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
        net.nm_lock_data_count++;
      }
      return 1;
    }
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_START_WHOSE_TURN:
    ((NetWhoseTurnMsg *)whose_turn_msg)->init(arg1);
    map_views_block.main_view_timer_cache_i32 = 0xffffffff;
    ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
    caption_rect.left = map_views_block.main_view_frame_left;
    caption_rect.top = map_views_block.main_view_frame_top;
    caption_rect.right = map_views_block.main_view_frame_right;
    caption_rect.bottom = map_views_block.main_view_client_top;
    ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&caption_rect);
    sent = net.send_to(dest,(NetMsg *)whose_turn_msg);
    return (int)sent;
  case NM_SERVER_REJECT:
  case NM_SERVER_CANCELED_START:
  case NM_LOST_SERVER_ON_JOIN:
  case NM_CLIENT_QUIT:
  case NM_SERVER_QUIT_REQUEST:
  case NM_SERVER_QUIT:
  case NM_SERVER_QUIT_ACK:
  case NM_SERVER_FULL:
  case NM_CLIENT_CANCELED_START:
  case NM_SERVER_QUERY:
  case NM_CIVILIZATION_PING:
  case NM_START_CIVILIZATION:
  case NM_NATION_ACCEPT:
  case NM_NATION_REJECT:
  case NM_HEART_BEAT:
  case NM_HEART_BEAT_ACK:
  case NM_KILL_OR_QUIT_ACK:
  case NM_PARLEYREQUEST_CANCEL:
  case NM_UNIT_PICKED_UP:
  case NM_UNIT_STACKED:
  case NM_SAFELY_DELETED:
  case NM_VISIBLY_DELETED:
  case NM_UNIT_PUT_DOWN:
  case NM_BOTTOMED:
  case NM_UNIT_RELOCATED:
  case NM_DO_APOLLO:
  case NM_REALTIME_STACKER_ACKER:
  case NM_HOLD_FOR_JOIN:
  case NM_CONTINUE_AFTER_JOIN:
  case NM_ABORT_KABOOM:
  case NM_DRAG_HOLD_ON:
  case NM_DRAG_HOLD_OFF:
  case NM_START_TURN_TIMER:
  case NM_STOP_TURN_TIMER:
  case NM_RESTART_TURN_TIMER:
  case NM_DRAW_MAP:
  case NM_INFO_STATUS:
  case NM_INFO_UPDATE:
  case NM_PARLEY_ACCEPT_OFFER:
  case NM_PARLEY_NO_THANKS:
  case NM_PARLEY_COUNTER_OFFER:
  case NM_CITYWIN_REDRAW:
  case NM_PRODUCTION_BEGIN:
  case NM_AI_MOVEMENT:
  case NM_HUMAN_MOVEMENT:
  case NM_READY_TO_START:
  case NM_KINGDOM_RATES:
  case NM_TAX_CHECK:
    return net_send_header_message(msg_type,dest);
  default:
    packet = net_send_build_packet(msg_type,arg1,arg2,arg3,arg4,arg5,arg6,text,raw_msg,known);
    if (known == 0) {
      return 0;
    }
  }
  if (packet == (void *)0x0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_address,(const char*)&str_d_ss_franklinton_netmgr_send_cpp_4,0x523);
    result = 0;
  }
  else {
    sent = net.send_to(dest,(NetMsg *)packet);
    result = (int)sent;
    operator_delete(packet);
  }
  return result;
}

/* 0046d5a0 auto */
NetMsgHeader *NetMsgHeader::header_init(u32 type)

{
  ((NetMsg *)this)->size = 0x10;
  msg_magic = 0x66606660;
  msg_type = type;
  return this;
}

/* 0046d5f0 auto */
NetMsgHeader *NetNamedMsg::init_header(u32 msg_type,const char *name2)

{
  ((NetAddrMsg *)this)->init(0);
  inner_type = msg_type;
  _memset(name,0,0x20);
  _strncpy(name,(char *)name2,0x20);
  _memset(address2,0,0x20);
  _strncpy(address2,((i8 *)&menu_prefs.net_ip_address),0x20);
  size = 0x74;
  return &hdr;
}

/* 0046d6a0 auto */
NetMsgHeader *NetAddrMsg::init(i32 msg_type)

{
  hdr.header_init(msg_type);
  _memset(address,0,0x20);
  _strncpy(address,((i8 *)&menu_prefs.net_ip_address),0x20);
  size = 0x30;
  return &hdr;
}

/* 0046d720 auto */
NetMsgHeader *NetServerInfoMsg::init()

{
  named.init_header(NM_SERVER_INFO,(const char *)&str_empty_81);
  net_fill_game_info(&game);
  named.size = 0x198;
  return &named.hdr;
}

/* 0046d780 auto */
NetMsgHeader *NetPlayerNodeMsg::init(u32 player_id)

{
  hdr.header_init(0x2f);
  slot.id = player_id;
  _memset(slot.address,0,0x20);
  _strncpy(slot.address,((i8 *)&menu_prefs.net_ip_address),0x20);
  crt_strcpy(slot.name,((i8 *)&menu_prefs.net_player_name));
  slot.field_44 = 0;
  slot.lobby_id = 0;
  slot.is_host = 0;
  slot.active = 0;
  slot.civ = 0xffffffff;
  size = 100;
  return &hdr;
}

/* 0046d860 auto */
NetMsgHeader *NetPlayerListMsg::init()

{
  hdr.header_init(4);
  size = 0x280;
  return &hdr;
}

/* 0046d8a0 auto */
NetMsgHeader *NetMsgHeader::server_path_init()

{
  header_init(6);
  ((NetMsg *)this)->size = 0x21c;
  return this;
}

/* 0046d8e0 auto */
NetMsgHeader *NetServerAnswerMsg::init()

{
  hdr.header_init(0x13);
  net_fill_game_info(&game);
  size = 0x134;
  return &hdr;
}

/* 0046d930 auto */
NetMsgHeader *NetWhoseTurnMsg::init(u32 turn_civ)

{
  hdr.header_init(0x69);
  civ = turn_civ;
  size = 0x14;
  return &hdr;
}

/* 0047e0e5 auto */
int __cdecl net_stacked_draw_push(int kind,i32 *args,const void *unit_data)

{
  int result;
  i8 dummy_args[28];
  int i;
  
  if ((net.stacked_draw_last == net.stacked_draw_first) && (net.stacked_draw_count != 0)) {
    dbg_log((const char *)&str_stacked_draw_stack_full_flushing);
    if (net.unit_stack_check_enabled != 0) {
      net.stacked_draw_last = (net.stacked_draw_first + 1) % 100;
    }
    else {
      net.stacked_draw_first = 0;
      net.stacked_draw_last = 0;
    }
    net.stacked_draw_count = (uint)(net.unit_stack_check_enabled != 0);
    net_stacked_draw_push(0xa3,(i32 *)dummy_args,0);
    result = net_stacked_draw_push(0x74,(i32 *)dummy_args,0);
  }
  else {
    if ((net.stacked_draw_first < 0) || (99 < net.stacked_draw_first)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_firststackeddraw_0_gnetmgr_first,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp,0x5a);
    }
    if ((net.stacked_draw_last < 0) || (99 < net.stacked_draw_last)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_laststackeddraw_0_gnetmgr_lastst,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp_2,0x5b);
    }
    net.stacked_draw[net.stacked_draw_last].kind = kind;
    for (i = 0; i < 7; i++) {
      *(i32 *)(((i8 *)&net.stacked_draw_args) + i * 4 + net.stacked_draw_last * 0x40) =
           args[i];
    }
    if (kind == 0x70) {
      crt_memcpy((void *)(net.stacked_draw_last * 0x40 + (iptr)(((u8 *)&net.stacked_draw_arg4 + 12))),unit_data,0x20);
    }
    net.stacked_draw_count++;
    if ((int)net.stacked_draw_peak < (int)net.stacked_draw_count) {
      net.stacked_draw_peak = net.stacked_draw_count;
    }
    result = (net.stacked_draw_last + 1) / 100;
    net.stacked_draw_last = (net.stacked_draw_last + 1) % 100;
  }
  return result;
}

/* 0047e2b3 auto */
int __cdecl net_stacked_draw_run_next(void)

{
  int y;
  iptr visible;
  Tile *tile;
  iptr unit;
  iptr unit2;
  int count;
  int x;
  i32 sum_mode;
  int stack_count;
  
  switch(net.stacked_draw[net.stacked_draw_first].kind) {
  case 0x70:
    x = net.stacked_draw[net.stacked_draw_first].args[1];
    y = net.stacked_draw[net.stacked_draw_first].args[2];
    unit2 = net.stacked_draw[net.stacked_draw_first].args[3];
    unit_stack_validate(net.stacked_draw[net.stacked_draw_first].args[0]);
    visible = mapviews_tile_visible(x,y);
    if (visible != 0) {
      crt_memcpy(((short *)&power_graph_history_block.stacked_draw_unit_copy),(const void *)(net.stacked_draw_first * 0x40 + (iptr)(((u8 *)&net.stacked_draw_arg4 + 12))),0x20);
      power_graph_history_block.stacked_draw_unit_prev = -1;
      power_graph_history_block.stacked_draw_unit_next = -1;
      mapviews_ensure_visible(x,y,power_graph_history_block.stacked_draw_unit_owner);
      if (net.stacked_draw[net.stacked_draw_first].args[4] == -1) {
        snd_play_effect(SND_MOVPIECE,1,0,0);
      }
      tile = tile_at(x,y);
      sum_mode = 2;
      unit = tile_first_unit(x,y);
      stack_count = unit_stack_sum(unit,sum_mode);
      if ((power_graph_history_block.stacked_draw_unit_copy[0] != x) || (power_graph_history_block.stacked_draw_unit_y != y)) {
        stack_count++;
      }
      if (stack_count == 1) {
        tile->improvements = tile->improvements & ~TILE_UNIT;
      }
      else if (1 < stack_count) {
        combat_anim.origin_x = (int)power_graph_history_block.stacked_draw_unit_copy[0];
        combat_anim.origin_y = (int)power_graph_history_block.stacked_draw_unit_y;
        count = unit_stack_sum(0x801,5);
        if (count == 0) {
          if (power_graph_history_block.stacked_draw_unit_next < 0) {
            if (power_graph_history_block.stacked_draw_unit_prev < 0) {
              tile->improvements = tile->improvements & ~TILE_UNIT;
            }
            else {
              combat_anim.origin_stack_unit = (iptr)power_graph_history_block.stacked_draw_unit_prev;
            }
          }
          else {
            combat_anim.origin_stack_unit = (iptr)power_graph_history_block.stacked_draw_unit_next;
          }
        }
        else {
          count = unit_stack_sum(0x801,5);
          if (count < 2) {
            tile->improvements = tile->improvements & ~TILE_UNIT;
          }
          else {
            combat_anim.origin_stack_unit = unit_stack_first(0x801);
            while (!((combat_anim.origin_stack_unit != 0x801) &&
                     (unit_types[units[combat_anim.origin_stack_unit].type].domain == DOMAIN_SEA))) {
              combat_anim.origin_stack_unit = (iptr)units[combat_anim.origin_stack_unit].next_in_stack;
              if (combat_anim.origin_stack_unit < 0) {
                tile->improvements = tile->improvements & ~TILE_UNIT;
                break;
              }
            }
          }
        }
      }
      unit_move_anim_active = 1;
      map_animate_unit_move(0x801,x,y,unit2,net.move_anim_sound,net.move_anim_sound_arg);
      unit_move_anim_active = 0;
      sum_mode = 2;
      unit2 = tile_first_unit(x,y);
      count = unit_stack_sum(unit2,sum_mode);
      if (count != 0) {
        if (net.host_flag == '\0') {
          tile->improvements = tile->improvements | TILE_UNIT;
        }
        else {
          tile_set_improvements(x,y,1,1,1);
        }
      }
    }
    break;
  case 0x71:
    x = tile_visible_to(net.stacked_draw[net.stacked_draw_first].args[0],
                            net.stacked_draw[net.stacked_draw_first].args[1],human_civ[0]);
    if (x != 0) {
      mapviews_ensure_visible(net.stacked_draw[net.stacked_draw_first].args[0],
                         net.stacked_draw[net.stacked_draw_first].args[1],
                         net.stacked_draw[net.stacked_draw_first].args[2]);
    }
    break;
  case 0x72:
    map_redraw_tile_all_views(net.stacked_draw[net.stacked_draw_first].args[0],
                       net.stacked_draw[net.stacked_draw_first].args[1]);
    break;
  case 0x73:
    map_redraw_combat_tiles(net.stacked_draw[net.stacked_draw_first].args[0],
                       net.stacked_draw[net.stacked_draw_first].args[1],
                       net.stacked_draw[net.stacked_draw_first].args[2],
                       net.stacked_draw[net.stacked_draw_first].args[3]);
    break;
  case 0x74:
    map_redraw_all_views(human_civ[0],1);
    break;
  case 0x75:
    map_redraw_neighbors_all_views(net.stacked_draw[net.stacked_draw_first].args[0],
                       net.stacked_draw[net.stacked_draw_first].args[1]);
    break;
  case 0x76:
    map_redraw_area_all_views(net.stacked_draw[net.stacked_draw_first].args[0],
                       net.stacked_draw[net.stacked_draw_first].args[1],
                       net.stacked_draw[net.stacked_draw_first].args[2],
                       net.stacked_draw[net.stacked_draw_first].args[3],1);
    break;
  case 0x7c:
    x = mapviews_tile_visible(net.stacked_draw[net.stacked_draw_first].args[0],
                               net.stacked_draw[net.stacked_draw_first].args[1]);
    if (x != 0) {
      map_animate_explosion(net.stacked_draw[net.stacked_draw_first].args[0],
                         net.stacked_draw[net.stacked_draw_first].args[1],
                         net.stacked_draw[net.stacked_draw_first].args[2]);
    }
    break;
  case 0x7d:
    x = mapviews_tile_visible(net.stacked_draw[net.stacked_draw_first].args[0],
                               net.stacked_draw[net.stacked_draw_first].args[1]);
    if (x != 0) {
      map_animate_nuke(net.stacked_draw[net.stacked_draw_first].args[0],
                         net.stacked_draw[net.stacked_draw_first].args[1]);
    }
    break;
  case 0xa3:
    net.nm_abort_kaboom_count++;
  }
  (*(int *)&net.stacked_draw_count)--;
  x = net.stacked_draw_first + 1;
  net.stacked_draw_first = x % 100;
  return x / 100;
}

/* Part of NetSession::process_messages (0047e94e): a message's 7 arguments
   (net_msg_get_args, net_stacked_draw_push): together, as the original's
   locals were.  They keep their values from one message to the next (0xa3
   pushes the arguments the previous message left). */
struct NetSession::MsgArgs { uptr unit; uptr civ3; uptr unit3; iptr y2; iptr x2; int y3; int extra_len; };

/* Part of NetSession::process_messages (0047e94e): hello (0): a client
   looking for games (1) gets our answer if we host; a server's answer (2)
   goes into the game list. */
void NetSession::recv_hello(int *msg,char &gamelist_dirty)
{
  int *hello_msg;
  
  hello_msg = msg;
  net.nm_recv_counts_by_type[msg[0x1c]] =
       net.nm_recv_counts_by_type[msg[0x1c]] + 1;
  if (msg[0x1c] == 1) {
    if (((host_flag != '\0') && (net.host_handoff_stage == '\0')) &&
       (net.connection_type != 0)) {
      net_send_message(NM_SERVER_INFO,0,0,0,0,0,0,0,0,0);
    }
  }
  else if (msg[0x1c] == 2) {
    game_list_add((NetServerInfoMsg *)msg);
    gamelist_dirty = 1;
  }
}

/* Part of NetSession::process_messages (0047e94e): the server's address, game
   name and version (0x13, the answer to 0x12). */
void NetSession::recv_server_info(int *msg,int *msg_hdr)
{
  if (net.gamelist_view != 0) {
    crt_strcpy(net.gamelist_view->server_address,(const char *)(msg + 4));
    crt_strcpy(net.gamelist_view->server_game_name,(const char *)(msg_hdr + 0x20));
    crt_strcpy(net.gamelist_view->server_info_c8,(const char *)(msg_hdr + 0x28));
    crt_strcpy(net.gamelist_view->server_version,(const char *)(msg_hdr + 0x10));
  }
  crt_strcpy(((i8 *)&net.net_server_address),(const char *)(msg_hdr + 4));
  crt_strcpy(((i8 *)&net.net_game_name),(const char *)(msg_hdr + 0x20));
  crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
  crt_strcpy(((i8 *)&net.net_server_version),(const char *)(msg_hdr + 0x10));
}

/* BF-092, BF-277: a state diff's record (0x15, 0x5c) inside its section: a
   section of the table (bit 0x8000: run-length coded) and the bytes it
   writes, raw or decoded, within the section's padded size, as the sender's
   records are (net_send_state_diff). The record's bytes are inside the
   message (net_message_body_valid). */
static int net_state_diff_record_fits(uint section_id,uint offset,uint size,const void *payload)
{
  RleStream rle;
  uint section;
  uint padded;
  uint written;

  section = section_id & ~0x8000u;
  if (0x17 < section) {
    return 0;
  }
  written = size;
  if ((section_id & 0x8000) != 0) {
    rle.src = (u8 *)payload;
    rle.src_end = (u8 *)payload + size;
    written = (uint)diffeng_rle_decoded_size(&rle);
  }
  padded = diffeng_section_table[section].padded_size;
  return offset <= padded && written <= padded - offset;
}

/* Part of NetSession::process_messages (0047e94e): a state diff (0x15, 0x5c):
   the changed sections of the game state, raw or run-length coded, copied
   into the game and its mirror; the game's own fields (section 1) and, on the
   host, the unit positions (section 5) kept. */
void NetSession::recv_state_diff(int *msg,uint *payload,MsgArgs &a)
{
  uint section_id2;
  u8 saved_multiplayer;
  iptr unit2;
  RleStream rle;  /* (the original's locals in a row) */
  i16 saved_moving_unit;
  i8 saved_player_civ;
  int decoded_size;
  int section_idx;
  int units_saved;
  int turn_civ_changed;
  uint section_offset;
  int city_changed;
  u16 saved_msg_options;
  i8 saved_field_1c;
  i8 saved_turn_civ_val;
  uptr section_size;
  i16 saved_active_unit;
  u32 saved_game_options;
  
  city_changed = 0;
  units_saved = 0;
  turn_civ_changed = 0;
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if (diffeng.mirror_buf != 0) {
    for (section_idx = 0; section_idx < (int)a.unit; section_idx++) {
      section_id2 = *payload;
      section_offset = payload[1];
      section_size = payload[2];
      payload += 3;
      if (!net_state_diff_record_fits(section_id2,section_offset,section_size,payload)) {
        dbg_logf_int2("recv_state_diff: refused a record of section %d at %d",(i32)section_id2,(i32)section_offset);
        break;
      }
      if ((section_id2 & 0x7fff) == 1) {
        saved_active_unit = game.active_unit;
        saved_moving_unit = game.moving_unit;
        saved_multiplayer = game.multiplayer;
        saved_player_civ = game.player_civ;
        saved_field_1c = game.saved_current_civ;
        saved_turn_civ_val = game.turn_civ;
        saved_game_options = game.game_options;
        saved_msg_options = game.message_options;
      }
      else if (((net.host_flag == '\0') || ((section_id2 & 0x7fff) != 5)) || (units_saved != 0))
      {
        if ((section_id2 & 0x7fff) == 0xb) {
          turn_civ_changed = 1;
        }
      }
      else {
        units_saved = 1;
        for (unit2 = 0; unit2 < game.num_units; unit2++) {
          map_views_block.net_saved_unit_x[unit2] = units[unit2].x;
          map_views_block.net_saved_unit_y[unit2] = units[unit2].y;
          map_views_block.net_saved_unit_next[unit2] = units[unit2].next_in_stack;
          map_views_block.net_saved_unit_prev[unit2] = units[unit2].prev_in_stack;
        }
      }
      if ((section_id2 & 0x8000) == 0) {
        crt_memcpy((void *)((iptr)diffeng_section_table[section_id2].data_ptr + section_offset),payload,section_size);
        crt_memcpy((void *)((int)diffeng_section_table[section_id2].mirror_offset + section_offset + diffeng.mirror_buf),
                     payload,section_size);
      }
      else {
        section_id2 = section_id2 & 0x7fff;
        rle.src_end = (u8 *)payload + section_size;
        rle.src = (u8 *)payload;
        decoded_size = diffeng_rle_decoded_size(&rle);
        rle.dst = (u8 *)diffeng_section_table[section_id2].data_ptr + section_offset;
        rle.dst_end = rle.dst + decoded_size;
        if (diffeng_rle_decode(&rle) == 0) {
          dbg_logf_int2("recv_state_diff: a run-length record of section %d at %d did not decode",(i32)section_id2,(i32)section_offset);
        }
        crt_memcpy((void *)((int)diffeng_section_table[section_id2].mirror_offset + section_offset + diffeng.mirror_buf),
                     (const void *)((iptr)diffeng_section_table[section_id2].data_ptr + section_offset),decoded_size);
      }
      if (section_id2 == 1) {
        game.active_unit = saved_active_unit;
        game.moving_unit = saved_moving_unit;
        game.multiplayer = saved_multiplayer;
        game.player_civ = saved_player_civ;
        game.saved_current_civ = saved_field_1c;
        game.turn_civ = saved_turn_civ_val;
        game.game_options = saved_game_options;
        game.message_options = saved_msg_options;
      }
      if (section_id2 == 6) {
        city_changed = 1;
      }
      payload = (uint *)((int)payload + section_size);
    }
    if (city_changed != 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    if ((net.host_flag != '\0') && (units_saved != 0)) {
      for (unit2 = 0; unit2 < game.num_units; unit2++) {
        units[unit2].x = map_views_block.net_saved_unit_x[unit2];
        units[unit2].y = map_views_block.net_saved_unit_y[unit2];
        units[unit2].next_in_stack = map_views_block.net_saved_unit_next[unit2];
        units[unit2].prev_in_stack = map_views_block.net_saved_unit_prev[unit2];
      }
    }
    if (turn_civ_changed != 0) {
      turn_loop.resume_civ = saved_turn.civ;
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): a player asking to join
   with a civ (0x2e, on the host): registered, then, once the game is under
   way, sent the whole game behind a wait dialog. */
void NetSession::recv_join_request(int *msg_hdr,int sender)
{
  u8 *civ_flag;
  int city;
  int slot;
  int wait_start;
  
  if (net.host_flag == '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_bserver,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp_4,0x22a);
  }
  if (net.host_handoff_stage == '\0') {
    city = net_host_register_player_civ((NetPlayerInfoMsg *)msg_hdr,sender);
    if (city == 0) {
      net_send_message(NM_LOST_SERVER_ON_JOIN,sender,0,0,0,0,0,0,0,0);
    }
    else {
      net.nm_lock_nation_count++;
      if (2 < (int)(*(uptr *)&net.session_phase)) {
        Dialog join_dialog(0x4000);
        net_send_message(NM_HOLD_FOR_JOIN,0xff,0,0,0,0,0,0,0,0);
        mp_turn_timer_stop(0);
        join_dialog.load_game_section((const char *)&str_waitongamexmit_2,0x2000000);
        join_dialog.show_modal(0);
        net_send_message(NM_MULTIOPTIONS_CHANGE,sender,mp_doubles.production,mp_doubles.moves,
                           mp_options.simultaneous_setting,0,0,0,0,0);
        net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(5000);
        net_send_state_diff(0xff,2,0,0,0);
        XD_FlushSendBuffer(5000);
        net_send_message(NM_MAP_STRUCT,sender,0,0,0,0,0,0,0,0);
        net_send_state_diff(sender,1,0,0,0);
        XD_FlushSendBuffer(5000);
        net_send_message(NM_EVENTSLOADED,sender,(*(uptr *)&scenario_events_loaded),(*(uptr *)&unit_types_block.scenario_event_count),0,0,0,0,0,0);
        net_send_rules_tables(sender);
        XD_FlushSendBuffer(5000);
        net_send_message(NM_GAME_GOING,sender,(*(uptr *)&net.session_phase),0,0,0,0,0,0,0);
        net_send_message(NM_START_CIVILIZATION,sender,0,0,0,0,0,0,0,0);
        net_send_message(NM_READY_TO_START,0xff,0,0,0,0,0,0,0,0);
        wait_start = timer_ticks();
        do {
          city = XD_FlushSendBuffer(5000);
          if (city == 0) break;
          net.process_messages(100,0);
          city = timer_ticks();
        } while (17999 < city - wait_start);
        net_send_message(NM_CONTINUE_AFTER_JOIN,0xff,0,0,0,0,0,0,0,0);
        join_dialog.close();
        mp_turn_timer_resume(0);
        for (slot = 0;
            (slot < 7 && (net.net_slots[slot].id != sender));
            slot = slot + 1) {
        }
        if (slot < 7) {
          civ_flag = (u8 *)civ_new_player_flags + (((i32 *)&net.slot_civs))[slot * 0x15] * 4;
          civ_flag[0] = 1;
          civ_flag[1] = '\0';
          civ_flag[2] = '\0';
          civ_flag[3] = '\0';
        }
      }
    }
  }
  else {
    net_send_message(NM_SERVER_CANCELED_START,sender,0,0,0,0,0,0,0,0);
  }
}

/* Part of NetSession::process_messages (0047e94e): a new player in the lobby
   (0x2f, on the host). */
void NetSession::recv_player_node(int *msg,int sender)
{
  if (net.host_handoff_stage == '\0') {
    if (net.net_player_count[0] < net.net_max_players) {
      add_player((NetPlayerNodeMsg *)msg);
      if (((*(int * *)&net.net_lobby_list) == (int *)0x0) || (*(*(int * *)&net.net_lobby_list) == 0)) {
        if (0 < (int)(*(uptr *)&net.session_phase)) {
          net_send_message(NM_GAME_GOING,sender,(*(uptr *)&net.session_phase),0,0,0,0,0,0,0);
          net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
          net_send_message(NM_SERVER_PATH,sender,0,0,0,0,0,0,0,0);
          net_send_message(NM_JOIN_GAMESTATE,sender,0,0,0,0,0,0,0,0);
        }
      }
      else {
        lobby_refresh_player_list();
      }
    }
    else {
      net_send_message(NM_SERVER_FULL,sender,0,0,0,0,0,0,0,0);
    }
  }
  else {
    net_send_message(NM_SERVER_CANCELED_START,sender,0,0,0,0,0,0,0,0);
  }
}

/* Part of NetSession::process_messages (0047e94e): a player claiming a civ
   (0x30, on the host): granted if no one has it. */
void NetSession::recv_claim_civ(int *msg,int sender,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if (((int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & 1 << ((byte)a.civ3 & 0x1f)) == 0) {
    (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (ushort)(1 << ((byte)a.civ3 & 0x1f)) | (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
    net_send_message(NM_NATION_ACCEPT,sender,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net.nm_lock_nation_count++;
  }
  else {
    net_send_message(NM_NATION_REJECT,sender,0,0,0,0,0,0,0,0);
  }
}

/* Part of NetSession::process_messages (0047e94e): a unit stack moved (0x5a).
   */
void NetSession::recv_stack_relocate(int *msg,int sender,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  unit_lift_off_map(a.unit3,0);
  unit_stack_clear_visibility(a.unit3);
  if (a.y2 == 0) {
    unit_stack_set_seen_by(a.unit3,a.civ3);
  }
  unit_stack_relocate_all(a.unit3,a.x2,a.y3,0);
  if ((-1 < units[a.unit3].next_in_stack) &&
     (unit_types[units[a.unit3].type].attack == '\0')) {
    unit_move_to_stack_bottom(a.unit3,0);
  }
  if (net.mp_simultaneous_moves != '\0') {
    net_unlock_map(a.unit,0);
  }
  net_send_state_diff(0xff,2,0,0,1);
  net_send_message(NM_REALTIME_STACKER_ACKER,sender,0,0,0,0,0,0,0,0);
  XD_FlushSendBuffer(60000);
}

/* Part of NetSession::process_messages (0047e94e): a unit bought by another
   civ (99). */
void NetSession::recv_unit_bribed(int *msg,MsgArgs &a)
{
  char *type_count;
  char civ;
  int city;
  uptr civ2;
  uptr unit5;
  i8 home_city_id;
  i8 new_owner;
  
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  civ2 = a.civ3;
  unit5 = a.unit;
  civ = units[a.unit].owner;
  civs[a.civ3].gold -= a.unit3;
  type_count = (char *)(units[a.unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
  *type_count = *type_count - 1;
  type_count = (char *)(units[a.unit].type + (iptr)((u8 *)&civs + 216) + a.civ3 * 0x594);
  *type_count = *type_count + 1;
  civs[a.civ3].subversion_count++;
  new_owner = (i8)a.civ3;
  units[a.unit].owner = new_owner;
  units[a.unit].home_city = -1;
  units[a.unit].moves_used = '\0';
  units[a.unit].orders = ORDER_NONE;
  city = city_find_nearest(units[a.unit].x,units[a.unit].y,-1,0xffffffff,-1)
  ;
  if ((-1 < city) && ((int)cities[city].owner == civ2)) {
    home_city_id = (i8)city;
    units[unit5].home_city = home_city_id;
  }
  unit_set_seen_by(unit5,civ);
  tile_set_owner(units[unit5].x,units[unit5].y,civ2,1);
  net_send_state_diff(0xff,2,0,0,1);
}

/* Part of NetSession::process_messages (0047e94e): the
   wait-while-the-game-is-sent dialog (0x65), modal until the host is done. */
void NetSession::recv_wait_game_xmit()
{
  u32 saved_busy;
  
  Dialog wait_dialog(0x4000);
  saved_busy = (u32)popup_in_progress;
  popup_in_progress = 1;
  mp_turn_timer_stop(0);
  net.nm_continue_after_join_count = 0;
  dlg_style.pending_idle_callback = net_game_xmit_wait_idle;
  popup_game_sprite_flags(str_waitongamexmit,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
  wait_dialog.close();
  popup_in_progress = saved_busy;
  mp_turn_timer_resume(0);
}

/* Part of NetSession::process_messages (0047e94e): the turn passes to a civ
   (0x69): the pending draws ended, the turn civ set and the caption redrawn.
   */
void NetSession::recv_turn_civ(int *msg_hdr)
{
  bool had_pending;
  i8 draw_args[28];
  RECT caption_rect;
  
  had_pending = unit_stack_check_enabled != 0;
  if (had_pending) {
    net.stacked_draw_last = (net.stacked_draw_first + 1) % 100;
  }
  else {
    net.stacked_draw_first = 0;
    net.stacked_draw_last = 0;
  }
  net.stacked_draw_count = (uint)had_pending;
  net_stacked_draw_push(0xa3,(i32 *)draw_args,0);
  net_stacked_draw_push(0x74,(i32 *)draw_args,0);
  game.turn_civ = (i8)msg_hdr[4];
  net.mp_turn_civ = (int)game.turn_civ;
  mp_turn_timer_stop(1);
  menu_update_states();
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  caption_rect.left = map_views_block.main_view_frame_left;
  caption_rect.top = map_views_block.main_view_frame_top;
  caption_rect.right = map_views_block.main_view_frame_right;
  caption_rect.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&caption_rect);
}

/* Part of NetSession::process_messages (0047e94e): a popup for us (0x6a),
   queued; a meeting request refused when we cannot meet. */
void NetSession::recv_popup_request(int *msg,int sender,MsgArgs &a)
{
  int *popup_data;
  i32 header_size;
  
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  header_size = 0x2c;
  if ((a.unit == 0x3d) &&
     (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.human_players) == 0 ||
      (diplo_meeting.mp_pending != 0)))) {
    net_send_message(NM_PARLEYREQUEST_REPLY,sender,2,0,0,0,0,0,0,0);
  }
  else {
    if (a.unit == 0x3d) {
      diplo_meeting.mp_pending = 1;
    }
    if (((a.unit != 100) || (parley_prompts.units_open != 1)) &&
       ((a.unit != 0x65 || (parley_prompts.city_open != 1)))) {
      if (a.extra_len == 0) {
        popup_data = (int *)0x0;
      }
      else {
        popup_data = msg + 0xb;
      }
      net_popup_queue_push(a.unit,a.civ3,a.unit3,a.y2,a.x2,(i32)popup_data,a.extra_len);
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): the turn time limit
   (0x6c): the turn timer shown and started, or hidden. */
void NetSession::recv_turn_time_limit(int *msg,MsgArgs &a)
{
  RECT timer_rect;
  
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  (*(uptr *)&civ_saved_cursor_x_block.mp_turn_time_limit_ms) = a.unit;
  if (a.unit == 0) {
    map_views_block.main_view_timer_shown[0] = 0;
    ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
    timer_rect.left = map_views_block.main_view_frame_left;
    timer_rect.top = map_views_block.main_view_frame_top;
    timer_rect.right = map_views_block.main_view_frame_right;
    timer_rect.bottom = map_views_block.main_view_client_top;
    ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&timer_rect);
  }
  else {
    map_views_block.main_view_timer_shown[0] = 1;
    mp_turn_timer_start(1);
  }
}

/* Part of NetSession::process_messages (0047e94e): the PRETEXT question
   (0xa4; PRETEXTALLIED when we are allied): yes sets treaty flag 0x2000, or
   ends the alliance. */
void NetSession::recv_war_pretext(int *msg,MsgArgs &a)
{
  iptr ret_val;
  int city;
  
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  ret_val = (iptr)(civ_name_plural(a.unit));
  text_set_string_arg(0,(const char *)ret_val);
  ret_val = (iptr)(civ_name_plural(a.civ3));
  text_set_string_arg(1,(const char *)ret_val);
  if ((*(byte *)&civs[a.civ3].treaties[a.unit] & TREATY_ALLIANCE) == 0) {
    city = popup_game_with_sprite(str_pretext,(Sprite *)&war_icon_sprite_block.war_icon_sprite,0);
    if (city == 1) {
      diplo_set_treaty_flags(a.unit,a.civ3,0x2000);
      civs[a.civ3].last_contact_turn[a.unit] = game.turn;
    }
  }
  else {
    city = popup_game_with_sprite(str_pretextallied,(Sprite *)(((i8 *)&diplo_popup_portrait_sprite) +
                                              ((game.advance_known_by[ADV_COMMUNISM] != '\0') - 1 &
                                              0xffff7c80)),0);
    if (city == 1) {
      diplo_clear_treaty_flags(a.unit,a.civ3,8);
      civs[a.civ3].last_contact_turn[a.unit] = game.turn;
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): the other side hid its
   cities (0xa5) in the parley window. */
void NetSession::recv_parley_cities_hidden(int *msg,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
    parley_wnd_block.remote_cities_revealed = (byte)a.civ3;
    if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
       (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
      ((Button *)parley_wnd_block.info_hide_buttons[1])->disable();
      ((Button *)parley_wnd_block.info_show_buttons[1])->disable();
      parley_draw_city_list(1);
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): the other side revealed
   its cities (0xa6) in the parley window. */
void NetSession::recv_parley_cities_shown(int *msg,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
    parley_wnd_block.remote_cities_revealed = (byte)a.civ3;
    if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
       (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
      ((Button *)parley_wnd_block.info_hide_buttons[1])->enable();
      ((Button *)parley_wnd_block.info_show_buttons[1])->enable();
      parley_draw_city_list(1);
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): the other side hid its
   units (0xa7) in the parley window. */
void NetSession::recv_parley_units_hidden(int *msg,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
    parley_wnd_block.remote_units_revealed = (byte)a.civ3;
    if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
       (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
      ((Button *)parley_wnd_block.info_hide_buttons[1])->disable();
      ((Button *)parley_wnd_block.info_show_buttons[1])->disable();
      ((Button *)parley_wnd_block.sort_by_city_buttons[1])->enable();
      ((Button *)parley_wnd_block.sort_by_unit_buttons[1])->enable();
      parley_draw_unit_list(1);
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): the other side revealed
   its units (0xa8) in the parley window. */
void NetSession::recv_parley_units_shown(int *msg,MsgArgs &a)
{
  net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
  if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
    parley_wnd_block.remote_units_revealed = (byte)a.civ3;
    if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
       (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
      ((Button *)parley_wnd_block.info_hide_buttons[1])->enable();
      ((Button *)parley_wnd_block.info_show_buttons[1])->enable();
      ((Button *)parley_wnd_block.sort_by_city_buttons[1])->disable();
      ((Button *)parley_wnd_block.sort_by_unit_buttons[1])->disable();
      parley_wnd_block.sort_unit_list(0,parley_wnd_block.list_counts[1] - 1,1);
      parley_draw_unit_list(1);
    }
  }
}

/* Part of NetSession::process_messages (0047e94e): an input the map or city
   window could not handle while the network was busy (blocked_input_kind),
   handled now. */
void NetSession::run_blocked_input()
{
  iptr dblclick_wnd;
  iptr click_wnd;
  int input_kind;
  u32 deferred_input;
  uint input_char;
  
  deferred_input = (u32)blocked_input_kind;
  net_deferred_input_busy = 1;
  dbg_log((const char *)&str_poll_unstacking_xd_flushsendbuffer_block);
  blocked_input_kind = 0;
  switch(deferred_input) {
  case 1:
    if (blocked_input_window->open != 0) {
      if (blocked_input_window == (MapView *)0x0) {
        click_wnd = 0;
      }
      else {
        click_wnd = (iptr)&blocked_input_window->window;
      }
      wnd_set_active_child(click_wnd);
      map_window_click(blocked_input_arg1,(u32)blocked_input_arg2,
                         blocked_input_arg3);
    }
    break;
  case 2:
    if (blocked_input_window->open != 0) {
      if (blocked_input_window == (MapView *)0x0) {
        dblclick_wnd = 0;
      }
      else {
        dblclick_wnd = (iptr)&blocked_input_window->window;
      }
      wnd_set_active_child(dblclick_wnd);
      map_window_double_click(blocked_input_arg1,(u32)blocked_input_arg2);
    }
    break;
  case 4:
    input_kind = net.blocked_input_char_pending;
    input_char = (input_char & 0xffffff00 | (byte)(net.blocked_input_char));
    map_window_key(blocked_input_arg1);
    if (input_kind == 3) {
      map_window_ascii(input_char);
    }
    break;
  case 5:
    menu_command(blocked_input_arg1,(u32)blocked_input_arg2);
    break;
  case 6:
    if (((city_wnd.closed == 0) && (cities[blocked_input_arg1].id != 0)) &&
       ((int)cities[blocked_input_arg1].owner == (u32)blocked_input_arg2)) {
      city_window_on_mouse(blocked_input_arg3,(u32)blocked_input_arg4,
                         blocked_input_arg5);
    }
    break;
  case 7:
    if (((city_wnd.closed == 0) && (cities[blocked_input_arg1].id != 0)) &&
       ((int)cities[blocked_input_arg1].owner == (u32)blocked_input_arg2)) {
      city_button_buy(blocked_input_arg3);
    }
    break;
  case 8:
    if (((city_wnd.closed == 0) && (cities[blocked_input_arg1].id != 0)) &&
       ((int)cities[blocked_input_arg1].owner == (u32)blocked_input_arg2)) {
      city_button_change(blocked_input_arg3);
    }
    break;
  case 9:
    if (((city_wnd.closed == 0) && (cities[blocked_input_arg1].id != 0)) &&
       ((int)cities[blocked_input_arg1].owner == (u32)blocked_input_arg2)) {
      city_button_view(blocked_input_arg3);
    }
    break;
  case 10:
    if (((city_wnd.closed == 0) && (cities[blocked_input_arg1].id != 0)) &&
       ((int)cities[blocked_input_arg1].owner == (u32)blocked_input_arg2)) {
      city_button_rename(blocked_input_arg3);
    }
  }
  net_deferred_input_busy = '\0';
}

/* Part of NetSession::process_messages (0047e94e): a message about the session: joining and the lobby (hello, player lists, the server's path
   and answers, nations claimed, options), heart beats, quitting, chat.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_session_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  int *drop_counter;
  u32 now;
  iptr unit4;
  i32 header_size;
  
  switch(msg[1]) {
  case NM_DATAGRAM:
    recv_hello(msg,gamelist_dirty);
    break;
  case NM_PLAYER_LIST:
    crt_memcpy((void *)net_player_count,msg + 4,0x270);
    for (unit4 = 0; unit4 < 7; unit4++) {
      if ((net_slots[unit4].active != '\0') &&
         (net_slots[unit4].id == server_conn_id)) {
        local_slot = unit4;
        break;
      }
    }
    break;
  case NM_SERVER_REJECT:
    now = timer_ticks();
    wait_start_ticks = now;
    break;
  case NM_SERVER_PATH:
    net.loaded_file_kind = (i16)msg[4];
    crt_strcpy(((i8 *)&net.loaded_file_name),(const char *)((const void *)((int)msg + 0x12)));
    crt_strcpy(((i8 *)&net.loaded_file_subdir),(const char *)((const void *)((int)msg_hdr + 0x116)));
    break;
  case NM_SERVER_CANCELED_START:
  case NM_LOST_SERVER_ON_JOIN:
  case NM_SERVER_FULL:
    break;
  case NM_CLIENT_QUIT:
    break;
  case NM_SERVER_QUIT_REQUEST:
    host_handoff_stage++;
    break;
  case NM_SERVER_QUIT:
    host_handoff_stage++;
    break;
  case NM_SERVER_QUIT_ACK:
    net.players_awaited--;
    break;
  case NM_SERVER_QUERY:
    net_send_message(NM_SERVER_ANSWER,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_SERVER_ANSWER:
    recv_server_info(msg,msg_hdr);
    break;
  case NM_CIVILIZATION_PING:
    break;
  case NM_START_CIVILIZATION:
    break;
  case NM_NATION_ACCEPT:
    break;
  case NM_NATION_REJECT:
    break;
  case NM_GAME_GOING:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.session_phase) = a.unit;
    break;
  case NM_LOCK_DATA:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (ushort)a.unit;
    game.human_players = game.human_players & (byte)a.unit;
    net.nm_lock_nation_count++;
    break;
  case NM_HEART_BEAT:
    net_send_message(NM_HEART_BEAT_ACK,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_HEART_BEAT_ACK:
    break;
  case NM_QUIT_REQUEST:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.civ_drop_requested[a.unit] = 1;
    net_drop_flagged_players();
    break;
  case NM_PLAYER_INFO:
    recv_join_request(msg_hdr,sender);
    break;
  case NM_PLAYER_LIST_NODE:
    recv_player_node(msg,sender);
    break;
  case NM_LOCK_NATION:
    recv_claim_civ(msg,sender,a);
    break;
  case NM_UNLOCK_NATION:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
    if (((int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & 1 << ((byte)a.civ3 & 0x1f)) != 0) {
      (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = ~(ushort)(1 << ((byte)a.civ3 & 0x1f)) & (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
      game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
      net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
      net.nm_lock_nation_count++;
    }
    break;
  case NM_CLIENT_CANCELED_START:
    net.lost_conn_by_player[sender] = 0xffffffff;
    remove_player(sender);
    if ((net_lobby_list != 0) &&
       (drop_counter = (int *)&net_lobby_list->drop_counts[sender], *drop_counter = *drop_counter + 1,
       *(*(int * *)&net.net_lobby_list) != 0)) {
      lobby_refresh_player_list();
    }
    break;
  case NM_MULTIOPTIONS_CHANGE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    mp_doubles.production = (short)a.unit;
    mp_doubles.moves = (short)a.civ3;
    net.mp_simultaneous_moves = (char)a.unit3;
    mp_options.simultaneous_setting = (short)(char)a.unit3;
    break;
  case NM_KILL_OR_QUIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    civ_eliminate(a.unit,0,0);
    net_send_message(NM_KILL_OR_QUIT_ACK,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_KILL_OR_QUIT_ACK:
    net.kill_or_quit_ack = 1;
    break;
  case NM_HOLD_FOR_JOIN:
    recv_wait_game_xmit();
    break;
  case NM_CONTINUE_AFTER_JOIN:
    break;
  case NM_CHAT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    header_size = 0x2c;
    chat_wnd_block.chat_receive_message(a.unit,a.civ3,a.unit3,(const char *)(msg + 0xb)
              );
    break;
  case NM_READY_TO_START:
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): a message about the game's state: game and map images, state differences, the rules'
   tables, the scenario's events, map tiles, attitudes, advances, the end.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_state_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  int image_kind;
  i32 header_size;
  int image_size;
  
  switch(msg[1]) {
  case NM_FULL_GAMESTATE:
  case NM_DIFF_GAMESTATE:
  case NM_JOIN_GAMESTATE:
    header_size = 0x2c;
    image_kind = msg[4];
    image_size = msg_size - 0x2c;
    crt_memmove(msg,msg + 0xb,image_size);
    msg = (int *)crt_realloc(msg,image_size);
    diffeng_apply_image((int *)(&msg),image_kind);
    break;
  case NM_MAP_STRUCT:
    header_size = 0x2c;
    image_kind = msg[4];
    image_size = msg_size - 0x2c;
    crt_memmove(msg,msg + 0xb,image_size);
    msg = (int *)crt_realloc(msg,image_size);
    diffeng_apply_map_header_image((int *)(&msg),image_kind);
    break;
  case NM_DIFFERENCE_ENGINE:
  case NM_ALPHA_DIFFERENCE_ENGINE:
    recv_state_diff(msg,payload,a);
    break;
  case NM_COSMIC:
    crt_memcpy(&cosmic,payload,0x29);
    break;
  case NM_CIVILIZE:
    crt_memcpy(advances,payload,0x640);
    break;
  case NM_IMPROVE:
    crt_memcpy(improvements,payload,0x218);
    break;
  case NM_ENDWONDER:
    crt_memcpy(wonder_expires,payload,0x1c);
    break;
  case NM_UNITS:
    crt_memcpy(unit_types,payload,0x4d8);
    break;
  case NM_TERRAIN:
    crt_memcpy(terrain_types,payload,0x318);
    break;
  case NM_GOVT:
    crt_memcpy(government_names,payload,0x1c);
    break;
  case NM_GOVT_TITLE:
    crt_memcpy(government_titles,payload,0x38);
    break;
  case NM_LEADERS:
    crt_memcpy(leaders,payload,0x3f0);
    break;
  case NM_KINGS:
    crt_memcpy(civs,payload,0x7524);
    break;
  case NM_CARAVAN:
    crt_memcpy(commodity_names,payload,0x40);
    break;
  case NM_ORDERS:
    crt_memcpy(order_names,payload,0x68);
    break;
  case NM_DIFFICULTY:
    crt_memcpy(difficulty_names,payload,0x18);
    break;
  case NM_ATTITUDES:
    crt_memcpy(attitude_names,payload,0x24);
    break;
  case NM_EVENTSLOADED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&scenario_events_loaded) = a.unit;
    (*(uptr *)&unit_types_block.scenario_event_count) = a.civ3;
    if (a.unit != 0) {
      events_relink_pool();
    }
    break;
  case NM_MAP_PACKETS:
    net_apply_tile_updates((u8 *)payload);
    break;
  case NM_END_SEQUENCE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    game.game_flags = (u16)a.unit;
    (*(uptr *)&game_over_reason) = a.civ3;
    (*(uptr *)&turn_flags.spaceship_winner_civ) = a.unit3;
    break;
  case NM_FEATURE_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_improvements(a.unit,a.civ3,a.unit3,a.y2,a.x2);
    break;
  case NM_TERRAIN_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_terrain(a.unit,a.civ3,a.unit3,a.y2);
    break;
  case NM_SEEN_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_visible(a.unit,a.civ3,a.unit3,a.y2,a.x2);
    break;
  case NM_SITE_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_fertility(a.unit,a.civ3,a.unit3,a.y2);
    break;
  case NM_OWNER_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_owner(a.unit,a.civ3,a.unit3,a.y2);
    break;
  case NM_REGION_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_landmass(a.unit,a.civ3,a.unit3,a.y2);
    break;
  case NM_CITY_USING_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    tile_set_radius_civ(a.unit,a.civ3,a.unit3,a.y2);
    break;
  case NM_FEAT_LOC_SET:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    /* (port) known_at does not bound the square: one off the map is
       ignored (a peer's memory error; the civ is checked on arrival) */
    if (map_in_bounds(a.unit,a.civ3) != 0) {
      known_set(a.unit,a.civ3,a.unit3,a.y2,a.x2,a.y3);
    }
    break;
  case NM_SET_ATTITUDE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    *(char *)&civs[a.unit].attitude[a.civ3] = (char)a.unit3;
    break;
  case NM_SET_CIV:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    civ_give_advance(a.unit,a.civ3,a.unit3,a.y2,0);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): a message about the calls the host makes for a client and their answers: units and
   cities created, deleted, moved and stacked, wonders, map locks, civs
   created and destroyed, bribes, casualties.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_call_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  iptr ret_val;
  int y4;
  int x3;
  iptr result;
  
  switch(msg[1]) {
  case NM_DELETE_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    if ((-1 < (int)a.unit) && (units[a.unit].id != 0)) {
      x3 = (int)units[a.unit].x;
      y4 = (int)units[a.unit].y;
    }
    unit_delete(a.unit,1);
    net_send_message(NM_UNIT_DELETED,sender,a.unit,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_UNIT_DELETED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.unit_deleted_ack = 1;
    break;
  case NM_DELETE_CITY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city_delete(a.unit,0);
    net_send_message(NM_CITY_DELETED,sender,a.unit,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_CITY_DELETED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.city_deleted_ack = 1;
    crt_memcpy(cities + a.unit,payload,0x58);
    break;
  case NM_CREATE_CITY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    result = city_create(a.unit,a.civ3,a.unit3);
    net_send_message(NM_CITY_CREATED,sender,result,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_CITY_CREATED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.nm_city_created_result = a.unit;
    if (a.unit != 0xffffffff) {
      crt_memcpy(cities + a.unit,payload,0x58);
      game.num_cities = (i16)a.civ3;
    }
    break;
  case NM_CREATE_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    result = unit_create(a.unit,a.civ3,a.unit3,a.y2);
    net_send_message(NM_UNIT_CREATED,sender,result,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_UNIT_CREATED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.nm_unit_created_result = a.unit;
    if (a.unit != 0xffffffff) {
      game.num_units = (i16)a.civ3;
    }
    break;
  case NM_PICK_UP_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_stack_unlink(a.unit,1);
    net_send_message(NM_UNIT_PICKED_UP,sender,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_UNIT_PICKED_UP:
    net.nm_unit_picked_up_count = 1;
    break;
  case NM_PUT_DOWN_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_stack_link(a.unit,a.civ3,a.unit3,1);
    net_send_message(NM_UNIT_PUT_DOWN,sender,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_UNIT_PUT_DOWN:
    net.nm_unit_put_down_count = 1;
    break;
  case NM_MOVE_TO_BOTTOM:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_move_to_stack_bottom(a.unit,1);
    net_send_message(NM_BOTTOMED,sender,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_BOTTOMED:
    net.nm_bottomed_count = 1;
    break;
  case NM_RELOCATE_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_relocate(a.unit,a.civ3,a.unit3,1);
    net_send_message(NM_UNIT_RELOCATED,sender,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_UNIT_RELOCATED:
    net.nm_unit_relocated_count = 1;
    break;
  case NM_ACQUIRE_WONDER:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    result = city_acquire_wonder(a.unit,a.civ3,a.unit3,a.y2,a.x2);
    net_send_message(NM_WONDER_ACQUIRED,sender,result,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    break;
  case NM_WONDER_ACQUIRED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.nm_wonder_acquired_result) = a.unit;
    break;
  case NM_STACK_SHIP:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    ret_val = unit_gather_cargo(a.unit,a.civ3,1);
    net_send_message(NM_SHIP_STACKED,sender,ret_val,0,0,0,0,0,0,0);
    break;
  case NM_SHIP_STACKED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.nm_ship_stacked_result = a.unit;
    break;
  case NM_STACK_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_lift_off_map(a.unit,1);
    net_send_message(NM_UNIT_STACKED,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_UNIT_STACKED:
    net.nm_unit_stacked_count = 1;
    break;
  case NM_DELETE_SAFELY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_delete_with_cargo(a.unit,1);
    net_send_message(NM_SAFELY_DELETED,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_SAFELY_DELETED:
    net.nm_safely_deleted_count = 1;
    break;
  case NM_DELETE_VISIBLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    unit_destroy_redraw(a.unit,1);
    net_send_message(NM_VISIBLY_DELETED,sender,0,0,0,0,0,0,0,0);
    break;
  case NM_VISIBLY_DELETED:
    net.nm_visibly_deleted_count = 1;
    break;
  case NM_LOCK_MAP:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_lock_map(a.unit,a.civ3,a.unit3,a.y2,a.x2,0);
    break;
  case NM_UNLOCK_MAP:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_unlock_map(a.unit,0);
    net_send_message(NM_UNLOCK_MAP_ACK,sender,a.unit,0,0,0,0,0,0,0);
    break;
  case NM_LOCK_MAP_ACK:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_map_lock_slots_block.net_map_locks[a.unit].replies = a.civ3;
    break;
  case NM_UNLOCK_MAP_ACK:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_map_lock_slots_block.net_map_locks[a.unit].state = net_map_lock_slots_block.net_map_locks[a.unit].state + 1;
    break;
  case NM_LOCK_MAP_SYNC:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_map_lock_slots_block.net_map_locks[a.unit].src_x = a.civ3;
    net_map_lock_slots_block.net_map_locks[a.unit].src_y = a.unit3;
    net_map_lock_slots_block.net_map_locks[a.unit].dst_x = a.y2;
    net_map_lock_slots_block.net_map_locks[a.unit].dst_y = a.x2;
    net_map_lock_slots_block.net_map_locks[a.unit].replies = 0;
    net_map_lock_slots_block.net_map_locks[a.unit].state =
         net_map_lock_slots_block.net_map_locks[a.unit].replies;
    break;
  case NM_DO_APOLLO:
    wonder_apollo_reveal_map();
    break;
  case NM_REALTIME_STACKER:
    recv_stack_relocate(msg,sender,a);
    break;
  case NM_REALTIME_STACKER_ACKER:
    net.nm_stacker_ack_count = 1;
    break;
  case NM_NEW_CIV:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    result = civ_create(a.unit);
    net_send_message(NM_NEW_CIV_ACK,sender,result,0,0,0,0,0,0,0);
    break;
  case NM_KILL_CIV:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    result = civ_check_destroyed(a.unit,a.civ3);
    net_send_message(NM_KILL_CIV_ACK,sender,result,0,0,0,0,0,0,0);
    break;
  case NM_NEW_CIV_ACK:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.nm_new_civ_ack_result) = a.unit;
    break;
  case NM_KILL_CIV_ACK:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.nm_kill_civ_ack_result) = a.unit;
    break;
  case NM_BRIBED_UNIT:
    recv_unit_bribed(msg,a);
    break;
  case NM_DEATH_STACK:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    casualty_log_add(a.unit,a.civ3,a.unit3,a.y2,a.x2);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): a message about the turns: whose turn, popups, the turn timer, production and movement
   phases, the tax and government dialogs, votes and replies.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_turn_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  iptr unit4;
  
  switch(msg[1]) {
  case NM_NEWTURNTIMER_REPLY:
  case NM_PMCHANGE_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.vote_replies_pending--;
    if (a.unit == 2) {
      net.vote_status = 0xffffffff;
    }
    break;
  case NM_START_WHOSE_TURN:
    recv_turn_civ(msg_hdr);
    break;
  case NM_POPUP:
    recv_popup_request(msg,sender,a);
    break;
  case NM_TIMER_CHANGE:
    recv_turn_time_limit(msg,a);
    break;
  case NM_START_TURN_TIMER:
    if ((*(uptr *)&civ_saved_cursor_x_block.mp_turn_time_limit_ms) != 0) {
      map_views_block.main_view_timer_shown[0] = 1;
    }
    mp_turn_timer_start(0);
    break;
  case NM_STOP_TURN_TIMER:
    mp_turn_timer_stop(0);
    break;
  case NM_RESTART_TURN_TIMER:
    mp_turn_timer_resume(0);
    break;
  case NM_CANESCAPE_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&sprite_picker_sprite_block.net_can_escape_reply) = a.unit;
    break;
  case NM_DESERT_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    font_size16_bold_block.net_bribe_answer = a.unit;
    break;
  case NM_PRODUCTION_BEGIN:
    break;
  case NM_PRODUCTION_COMPLETE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    dialog_text_font_obj_block.net_civ_phase_done[a.unit] = dialog_text_font_obj_block.net_civ_phase_done[a.unit] + 1;
    break;
  case NM_AI_MOVEMENT:
    break;
  case NM_HUMAN_MOVEMENT:
    break;
  case NM_HUMAN_MOVE_COMPLETE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    dialog_text_font_obj_block.net_civ_phase_done[a.unit] = dialog_text_font_obj_block.net_civ_phase_done[a.unit] + 1;
    for (unit4 = 0; unit4 < game.num_units; unit4++) {
      if ((units[unit4].id != 0) && ((int)units[unit4].owner == a.unit)) {
        unit_exhaust_moves(unit4);
      }
    }
    break;
  case NM_KINGDOM_RATES:
    taxrate_dialog_show((*(i8 *)&human_civ));
    break;
  case NM_TAX_CHECK:
    taxrate_enforce_max_rate((*(i8 *)&human_civ));
    break;
  case NM_PICK_GOVT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    civ_select_government_dialog((*(i8 *)&human_civ),a.unit);
    break;
  case NM_TURN_FINISHED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.civ_turn_ended[a.unit] = net.civ_turn_ended[a.unit] + 1;
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): a message about what the players see: moves slid, the map drawn, explosions, the
   status bar, sounds, the city window and reports.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_display_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  i32 x;
  int city;
  
  switch(msg[1]) {
  case NM_DRAG_HOLD_ON:
    net.drag_hold = 1;
    dbg_log((const char *)&str_hold_on);
    break;
  case NM_DRAG_HOLD_OFF:
    net.drag_hold = 0;
    dbg_log((const char *)&str_hold_off);
    break;
  case NM_SLIDE_PIECE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    if (a.y3 != 0) {
      x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[a.y2] + a.civ3);
      city = city_at(x,(int)(((i8 *)&dir_dy_table_block.dir_dy_table))[a.y2] + a.unit3);
      if ((city < 0) && ((game.game_options & OPT_SHOW_ENEMY_MOVES) == 0)) {
        msg_hdr[1] = 0x72;
        a.unit = a.civ3;
        a.civ3 = a.unit3;
      }
    }
    net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,payload);
    break;
  case NM_POSSIBLY_CENTER:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    break;
  case NM_DRAW_SQUARE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    }
    break;
  case NM_DRAW_TWO:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if ((city != 0) || (city = mapviews_tile_visible(a.unit3,a.y2), city != 0)) {
      net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    }
    break;
  case NM_DRAW_MAP:
    net_stacked_draw_push(msg[1],(i32 *)&a.unit,0);
    break;
  case NM_DRAW_AREA:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    }
    break;
  case NM_DRAW_MAP_REGION:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    }
    break;
  case NM_INFO_STATUS:
    status_draw_info(1);
    break;
  case NM_INFO_UPDATE:
    status_redraw(1);
    break;
  case NM_SOUND_PLAY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    snd_play_effect(a.unit,a.civ3,0,0);
    break;
  case NM_KABOOM:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    break;
  case NM_SHOW_NUKE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(i32 *)&a.unit,0);
    break;
  case NM_REPORTS_UPDATE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    advisor_refresh_if_open(a.unit);
    break;
  case NM_CITYWIN_CHECK_UNIT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(a.unit,a.civ3,a.unit3);
    break;
  case NM_CITYWIN_CHECK_CITY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(a.unit);
    break;
  case NM_CITYWIN_DELETED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    ((CityWindow *)((i8 *)&city_wnd.window))->close_if_city(a.unit);
    break;
  case NM_CITYWIN_REDRAW:
    if (city_wnd.refresh_locked == 0) {
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    }
    break;
  case NM_SET_SLIDE_SOUND:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.move_anim_sound) = a.unit;
    (*(uptr *)&net.move_anim_sound_arg) = a.civ3;
    break;
  case NM_ABORT_KABOOM:
    net_stacked_draw_push(msg[1],(i32 *)&a.unit,0);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): a message about diplomacy: allies asked for help, parley requests and offers, the AI's
   meetings and politics, pretexts for war, what a parley shows.
   Returns 0 for a message of another kind. */
int NetSession::dispatch_diplomacy_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  switch(msg[1]) {
  case NM_ASK_ALLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ally_demand_help(a.unit,a.civ3);
    break;
  case NM_PARLEYREQUEST_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&advisor_window_block.mp_meeting_reply) = a.unit;
    break;
  case NM_PARLEYREQUEST_CANCEL:
    if ((((((i32 *)&net.slot_civs))[sender * 0x15] == parley_wnd_block.other_civ) || (parley_wnd_block.other_civ == 0xfffffffe))
       || (parley_wnd_block.closed == 0)) {
      if (parley_wnd_block.closed == 0) {
        parley_wnd_close();
      }
      else if (parley_wnd_block.other_civ == 0xfffffffe) {
        parley_wnd_block.other_civ = 0xffffffff;
      }
      diplo_meeting.mp_pending = 0;
    }
    break;
  case NM_PARLEY_TRANSACTION:
    if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
      operator_delete((void *)parley_wnd_block.received_offer);
      parley_wnd_block.received_offer = 0;
    }
    parley_wnd_block.received_offer = (ParleyOfferMsg *)operator_new(msg_size);
    if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
      crt_memcpy((void *)parley_wnd_block.received_offer,msg,msg_size);
    }
    parley_poll_net_replies();
    break;
  case NM_PARLEY_ACCEPT_OFFER:
    parley_poll_net_replies();
    break;
  case NM_PARLEY_NO_THANKS:
    parley_poll_net_replies();
    break;
  case NM_PARLEY_COUNTER_OFFER:
    parley_poll_net_replies();
    break;
  case NM_PARLEY_EXECUTE:
    parley_execute_transaction((ParleyOfferMsg *)msg);
    break;
  case NM_DO_AIPARLEY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_meet_pending_flags[a.civ3] = 1;
    net_meet_pending.ai_civ[a.civ3] = a.unit;
    net_meet_pending.civ[a.civ3] = a.civ3;
    net_meet_pending.x[a.civ3] = a.unit3;
    net_meet_pending.y[a.civ3] = a.y2;
    break;
  case NM_POLITICAL_VIOLATIONS:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ai_review_relations(a.unit,a.civ3);
    break;
  case NM_POLITICAL_ATTITUDE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ai_seek_war_ally(a.unit,a.civ3);
    break;
  case NM_PRETEXT:
    recv_war_pretext(msg,a);
    break;
  case NM_REVEAL_CITY_INFO:
    recv_parley_cities_hidden(msg,a);
    break;
  case NM_HIDE_CITY_INFO:
    recv_parley_cities_shown(msg,a);
    break;
  case NM_REVEAL_CITY_ORIGIN:
    recv_parley_units_hidden(msg,a);
    break;
  case NM_HIDE_CITY_ORIGIN:
    recv_parley_units_shown(msg,a);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of NetSession::process_messages (0047e94e): one civ message, by type
   (msg[1]), handled by its kind. msg by reference: an image message (7 to 10)
   moves its payload to the front and reallocates it, and the caller frees it. */
void NetSession::dispatch_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  if (dispatch_session_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  if (dispatch_state_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  if (dispatch_call_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  if (dispatch_turn_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  if (dispatch_display_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  if (dispatch_diplomacy_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a) != 0) return;
  dbg_log((const char *)&str_received_unknown_message_type);
}

/* 0047e94e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void NetSession::process_messages(int max_msgs,int wait_mode)

{
  char civ;
  iptr flush_pending;
  iptr flush_pending2;
  int dequeue_wait;
  uint msg_size;
  int sender;
  iptr unit4;
  uint *payload;
  i8 unused_flag;
  MsgArgs a;   /* (the original's locals in a row: a message's 7 arguments) */
  int *msg_hdr;
  int *msg;
  char gamelist_dirty;
  
  gamelist_dirty = '\0';
  unused_flag = 0;
  if ((((((((unit_create_busy == 0) && (unit_delete_busy == 0)) &&
          (city_create_busy == 0)) &&
         ((city_delete_busy == 0 && (lock_map_busy == 0)))) &&
        (unlock_map_busy == 0)) &&
       (((unit_stack_unlink_busy == 0 && (unit_stack_link_busy == 0)) &&
        ((unit_stack_bottom_busy == 0 &&
         (((unit_relocate_busy == 0 && (city_acquire_wonder_busy == 0)) && (civ_eliminate_busy == 0)
          ))))))) && ((civ_create_busy == 0 && (civ_check_destroyed_busy == 0)))) &&
     ((unit_gather_cargo_busy == 0 &&
      (((unit_lift_off_map_busy == 0 && (unit_delete_cargo_busy == 0)) &&
       ((unit_destroy_redraw_busy == 0 && (flush_pending = XD_InFlushSendBuffer(), flush_pending == 0)))))))) {
    if (((blocked_input_kind != 0) && (net_deferred_input_busy == '\0')) &&
       (flush_pending2 = XD_InFlushSendBuffer(), flush_pending2 == 0)) {
      run_blocked_input();
    }
    poll_depth = poll_depth + 1;
    if (poll_depth_peak < poll_depth) {
      poll_depth_peak = poll_depth;
    }
    while (max_msgs != 0) {
      if (((unit_stack_check_enabled == 0) && (map_redraw_busy == 0)) &&
         (0 < (int)stacked_draw_count)) {
        ((u8 *)&unit_stack_check_enabled)[0] = 1;
        ((u8 *)&unit_stack_check_enabled)[1] = '\0';
        ((u8 *)&unit_stack_check_enabled)[2] = '\0';
        ((u8 *)&unit_stack_check_enabled)[3] = '\0';
        if ((int)stacked_draw_count < 0x22) {
          unit4 = 1;
        }
        else {
          unit4 = 2;
        }
        for (; 0 < unit4; unit4--) {
          net_stacked_draw_run_next();
        }
        ((u8 *)&unit_stack_check_enabled)[0] = '\0';
        ((u8 *)&unit_stack_check_enabled)[1] = '\0';
        ((u8 *)&unit_stack_check_enabled)[2] = '\0';
        ((u8 *)&unit_stack_check_enabled)[3] = '\0';
      }
      if ((net.msgq_normal_count < 0x12d) && (net.msgq_priority_count < 0x22)) {
        ui_pump_messages();
        max_msgs--;
      }
      if (net.msgq_priority_count < 0x22) {
        dequeue_wait = wait_mode;
      }
      else {
        dequeue_wait = 1;
      }
      civ = msg_queue.dequeue(&sender,(i32 *)&msg,
                         (i32 *)&msg_size,dequeue_wait);
      if (civ == '\0') break;
      if (msg == (int *)0x0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pdata_null,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp_3,0x195);
      }
      /* BF-280: dequeue's captured size must cover reads before counters/dispatch. */
      if (!net_received_message_valid(msg,msg_size)) {
        operator_delete(msg);
        continue;
      }
      msg_hdr = msg;
      if (*msg == 0x66606660) {
        net.nm_recv_counts_by_type[msg[1]] = net.nm_recv_counts_by_type[msg[1]] + 1;
        if ((0x35 < msg[1]) && (civ_saved_cursor_x_block.game_setup_complete == 0)) {
          operator_delete(msg);
          break;
        }
        payload = (uint *)(msg + 0xb);
        dispatch_message(msg,msg_hdr,payload,sender,msg_size,gamelist_dirty,a);
        operator_delete(msg);
      }
      else {
        dbg_logf_int2((const char *)&str_poll_retrieved_non_civ_message_message_d,msg[1],msg[2]);
      }
    }
    if (((wait_mode == 0) && (listing_games != '\0')) && (gamelist_dirty != '\0'))
    {
      net_gamelist_refresh();
    }
    if (((wait_mode == 0) && (popup_in_progress == 0)) && (net_popups.head != 0)) {
      net_popup_queue_show_next();
    }
    poll_depth = poll_depth - 1;
  }
  return;
}

/* 0059bb54 auto */
void __cdecl net_on_broadcast_receive(void *data,ushort size,long header)

{
  int *msg = (int *)data;
  int cmp;
  
  if (!net_received_message_valid(data,size)) {
    dbg_log((const char *)&str_broadcastreceivefunc_killed_message_tiny);
  }
  else if (*msg == 0x66606660) {
    if ((msg[1] != 0) ||
       ((((char)msg[0xc] == '\0' ||
         (cmp = _strcmp((char *)(msg + 0xc),((i8 *)&menu_prefs.net_ip_address)), cmp == 0)) &&
        (((char)msg[0x14] == '\0' ||
         (cmp = _strcmp((char *)(msg + 0x14),((i8 *)&menu_prefs.net_ip_address)), cmp != 0)))))) {
      net.msg_queue.enqueue(0,(NetMsg *)msg,(uint)size);
    }
  }
  else {
    dbg_log((const char *)&str_broadcastreceivefunc_killed_message_not);
  }
  return;
}

/* 0059bc41 auto */
void __cdecl net_on_secure_receive(ushort conn_id,void *data,unsigned long size,short flags)

{
  int *msg = (int *)data;
  size_t len;
  int candidate[4];
  uptr byte_idx;
  uptr offset;
  char num_buf[32];
  i8 hex_line[64];
  uint byte_val;
  int *header;
  
  if (data != 0 && 3 < size) {
    if (*msg == 0x66606660) {
      if (!net_received_message_valid(data,size)) {
        if (16 <= size) {   /* (port) say so: a game we play never sends one */
          dbg_logf_int2("SecureReceiveFunc: refused a message of type %d, %d bytes",msg[1],(i32)size);
        }
        return;
      }
      net.msg_queue.enqueue(conn_id & 0xffff,(NetMsg *)msg,size);
      net.last_recv_callback_size = size;
      net.last_queued_send_seq = net_send_sequence;
      header = msg;
      net.last_recv_msg_size = msg[2];
      net_last_recv_msg_type = msg[1];
    }
    else {
      dbg_log((const char *)&str_securereceivefunc_killed_message_not_civ);
      if (-1 < net_last_recv_msg_type) {
        __ultoa(net.last_recv_callback_size,num_buf,10);
        dbg_logf_str((const char *)&str_last_callback_size_s,(const char *)num_buf);
        __itoa(net.last_queued_send_seq,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_sequence_number_s,(const char *)num_buf);
        __itoa(net_last_recv_msg_type,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_type_s,(const char *)num_buf);
        __ultoa(net.last_recv_msg_size,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_netmsg_size_s,(const char *)num_buf);
      }
      __itoa(net_send_sequence,num_buf,10);
      dbg_logf_str((const char *)&str_sequence_number_s,(const char *)num_buf);
      __ultoa(size,num_buf,10);
      dbg_logf_str((const char *)&str_message_size_s,(const char *)num_buf);
      for (offset = 0; (offset < 0x20 && (hex_line[0] = 0, offset <= size));
          offset = offset + 8) {
        for (byte_idx = offset; (byte_idx < offset + 8 && (byte_idx < size));
            byte_idx = byte_idx + 1) {
          byte_val = (uint)*(byte *)(byte_idx + (int)msg);
          __itoa(byte_val,num_buf,0x10);
          len = _strlen(num_buf);
          if (len == 1) {
            num_buf[1] = num_buf[0];
            num_buf[2] = 0;
            num_buf[0] = '0';
          }
          crt_strcat(hex_line,num_buf);
          crt_strcat(hex_line,((i8 *)&net_hexdump_space_str));
        }
        dbg_logf_str((const char *)&str_s_3,(const char *)hex_line);
      }
      dbg_log((const char *)&str_empty_184);
      net_last_recv_msg_type = -1;
      /* BF-179: a diagnostic candidate must contain the entire header too. */
      for (offset = 0; offset < 0x20 && offset <= size && size - offset >= sizeof candidate; offset++) {
        memcpy(candidate,(char *)msg + offset,sizeof candidate);
        if (candidate[0] == 0x66606660) {
          header = candidate;
          dbg_log((const char *)&str_potential_offset_message_found);
          __itoa(offset,num_buf,10);
          dbg_logf_str((const char *)&str_offset_s_bytes,(const char *)num_buf);
          /* BF-181: a peer's type is not an unchecked diagnostic-name index. */
          if ((uint)header[1] < sizeof str_nm_datagram / 0x20) {
            dbg_logf_str((const char *)&str_type_s,(const char *)(((char *)&str_nm_datagram) + header[1] * 0x20));
          }
          else {
            __itoa(header[1],num_buf,10);
            dbg_logf_str((const char *)&str_type_s,num_buf);
          }
          __ultoa(header[2],num_buf,10);
          dbg_logf_str((const char *)&str_size_s_bytes,(const char *)num_buf);
          __ultoa(header[3],num_buf,10);
          dbg_logf_str((const char *)&str_sequence_number_s_2,(const char *)num_buf);
          break;
        }
      }
    }
  }
  if (((300 < net.msgq_normal_count) || (0x21 < net.msgq_priority_count)) && (mp_drag.active != '\0')) {
    mp_release_mouse_capture();
  }
  return;
}

/* 0059bfb5 auto */
void __cdecl net_on_new_client(ushort connected_count,ushort conn_id)

{
  dbg_logf((const char *)&str_newclientreceivefunc_new_client_connecti,conn_id & 0xffff);
  return;
}

/* 0059bfdb auto */
void __cdecl net_on_connected_to_server(short conn_id)

{
  dbg_logf((const char *)&str_connectedtoserverfunc_new_server_connect,(int)conn_id);
  if ((-1 < conn_id) && (conn_id < 8)) {
    net.server_conn_id = (int)conn_id;
    net.connected_to_server = 1;
    net_send_message(NM_PLAYER_LIST_NODE,0,conn_id,0,0,0,0,0,0,0);
  }
  return;
}

/* 0059c04b auto */
void __cdecl net_on_connection_lost(ushort player_id)

{
  dbg_logf((const char *)&str_lostconnectionfunc_connection_to_d_has_s,(uint)player_id);
  net.lost_conn_total++;
  if (player_id < 7) {
    net.lost_conn_by_player[(uint)player_id] = net.lost_conn_by_player[(uint)player_id] + 1;
  }
  return;
}

/* 0059c0a4 auto */
void __cdecl net_on_oversized_message(unsigned long size)

{
  char msg_buf[256];
  
  _sprintf(msg_buf,str_oversized_xdaemon_message_ul,size);
  dbg_log((const char *)msg_buf);
  return;
}

/* 0059c0e1 auto */
void * __cdecl
net_msg_build_packet(i32 msg_type,const void *payload,i32 arg1,i32 arg2,
            i32 arg3,i32 arg4,i32 arg5,i32 arg6,
            size_t payload_len)

{
  NetMsg *packet;
  NetMsg header;  /* only the 16-byte header is used */
  size_t payload_size;
  
  header.hdr.header_init(msg_type);
  if (payload_len == 0) {
    if (payload == 0) {
      payload_size = 0;
      packet = (NetMsg *)operator_new(0x2c);
    }
    else {
      payload_size = __msize((void *)payload);
      packet = (NetMsg *)crt_realloc((void *)payload,payload_size + 0x2c);
      crt_memmove((void *)(packet + 1),packet,payload_size);
    }
  }
  else {
    payload_size = payload_len;
    packet = (NetMsg *)operator_new(payload_len + 0x2c);
    crt_memcpy((void *)(packet + 1),payload,payload_size);
  }
  header.size = payload_size + 0x2c;
  crt_memcpy(packet,&header,0x10);
  crt_memcpy(&packet->args[0],&arg1,4);
  crt_memcpy(&packet->args[1],&arg2,4);
  crt_memcpy(&packet->args[2],&arg3,4);
  crt_memcpy(&packet->args[3],&arg4,4);
  crt_memcpy(&packet->args[4],&arg5,4);
  crt_memcpy(&packet->args[5],&arg6,4);
  crt_memcpy(&packet->args[6],&payload_size,4);
  return packet;
}
