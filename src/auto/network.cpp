/* The game, multiplayer: network sessions, negotiation, lobby and chat: functions translated from the original
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


/* 00421f40 auto */
i8 NetSession::is_host()

{
  return host_flag;
}

/* 00421f70 auto */
NetGamelistView *NetGamelistView::view_construct()

{
  next_game_id = 1;
  first_option_value = 0;
  refresh_timer = 0;
  return this;
}

/* Part of net_session_init (00421fcd): where it goes on after the scenario
   (net_join_load_scenario). */
enum NetJoinScenario { JOIN_GO_ON, JOIN_FAIL, JOIN_SCENARIO_CANCELLED };

/* Part of net_session_init (00421fcd): Internet: the host's address, typed or
   the lobby's, and the connection to it, tried until it answers. 0: cancelled
   or failed (the caller fails). */
static int net_join_by_address(NetGamelistView *self,int conn_type,int retries,Dialog &dlg)
{
  int result;
  int popup;
  size_t len1;
  size_t len2;
  char address[260];
  
  /* Internet: the host's address typed (or the lobby's) */
  for (;;) {
    retries++;
    if (net.lobby_launch_mode != -1) {
      crt_strcpy(self->server_address,((i8 *)&net.lobby_launch_info));
      crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&net.lobby_launch_info));
    }
    else {
      dlg.load_text_section((const char *)&str_game_7,"IPOFGAME",0x1f,(iptr)"",0,0,0,1);
      result = dlg.run_modal((iptr)address,0);
      if (result == -1) {
        self->server_address[0] = '\0';
        return 0;
      }
      if (_strlen(address) == 0) continue;
      crt_strcpy(self->server_address,address);
      crt_strcpy(((i8 *)&net.net_server_address),address);
    }
    result = XD_OpenConnection(self->server_address,net.timeout_seconds * 1000);
    if (result < 0) {
      dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
      crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
      popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
      if ((conn_type == 2) || (conn_type == 3)) return 0;
      continue;
    }
    if ((conn_type == 0) && (net.lobby_launch_mode != -1)) {
      dbg_log("IGZ - Sending DPLSYS_CONNECTIONSETTINGSREAD to lobby");
      result = XD_LobbySendMessage(1);
      if (result != 0) {
        dbg_logf("IGZ - XD_LobbySendMessage returned error - %d",result);
      }
      dbg_log("IGZ - Sending DPLSYS_DPLAYCONNECTSUCCEEDED to lobby");
      result = XD_LobbySendMessage(3);
      if (result != 0) {
        dbg_logf("IGZ - XD_LobbySendMessage returned error - %d",result);
      }
    }
    net.wait_start_ticks = timer_ticks();
    dlg_style.pending_idle_callback = net_join_wait_idle_cb;
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
    if (popup == -1) return 0;
    if (net.server_conn_id != -1) break;
    /* no id from the host: the lobby's game is tried again (3 times) */
    if ((conn_type == 0) && (net.lobby_launch_mode != -1) && (retries <= 3)) {
      XD_CloseConnection();
      continue;
    }
    popup_game_with_sprite("SERVERCONNECTFAIL",(iptr)&popup_info_icon_sprite,0);
    return 0;
  }
  /* the host's version */
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(NM_SERVER_QUERY,0,0,0,0,0,0,0,0,0);
  dlg_style.pending_idle_callback = net_join_wait_answer_idle;
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_reject_count != 0) || (net.nm_server_canceled_count != 0)) {
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_reject_count == 0) {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("REJECTEDLAN",(iptr)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  len1 = _strlen(((i8 *)&net.net_server_version));
  len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
  if ((len1 != len2) || (_strcmp(((i8 *)&net.net_server_version),str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
    crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.net_server_version));
    crt_strcpy(((i8 *)&text_args.string1),str_ptr_5_4_0f_multiplayer_26_march_99);
    popup_game_with_sprite("WRONGVERSION",(iptr)&popup_info_icon_sprite,0);
    return 0;
  }
  return 1;
}

/* Part of net_session_init (00421fcd): LAN: the games the hosts announce, in
   a list until the player picks one and the connection to it opens. 0:
   cancelled or failed (the caller fails). */
static int net_join_from_list(NetGamelistView *self,int conn_type)
{
  int result;
  NetGameInfo *found;
  int civs_count;
  int players_count;
  uint mask;
  int bit;
  size_t len1;
  size_t len2;
  Dialog *list_dialog;
  
  net.free_game_list();
  list_dialog = (Dialog *)operator_new(0x2f4);
  if (list_dialog != (Dialog *)0x0) {
    list_dialog = list_dialog->Dialog::construct(0x4000);
  }
  self->list_dialog = list_dialog;
  if (self->list_dialog == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)"pJoinPopup != NULL",(iptr)"D:\\Ss\\Franklinton\\startup joinbox.cpp",0xdc);
  }
  self->saved_button_label = dlg_style.button_labels[0];
  str_clear(self->button_label);
  str_append_label((iptr)self->button_label,0x126);
  dlg_style.button_labels[0] = (i8 *)self->button_label;
  self->list_dialog->Dialog::reset(0x1081801);
  self->list_dialog->set_width(0x276);
  self->list_dialog->set_height(0x153);
  text_clear();
  text_append_label(0x287);
  self->list_dialog->set_title((const char *)&textbuf.buf);
  self->list_dialog->set_current_pane(0);
  self->list_dialog->set_listbox(0xb,0xd2,0);
  self->list_dialog->set_select_change_fn((DialogSelectFn)net_gamelist_show_game_info);
  self->add_info_lines();
  text_clear();
  text_append_label(0x286);
  self->add_entry((const char *)&textbuf.buf,0);
  self->list_dialog->set_default_choice(0);
  self->update_details();
  self->list_dialog->render();
  ((ModalMenuWindow *)(self->list_dialog->surf_wnd + 0x48))->set_modal_idle_callback(net_gamelist_poll_idle);
  for (;;) {
    net.listing_games = 1;
    self->start_timer((TimerFn)net_gamelist_poll_tick);
    result = self->list_dialog->show_modal(0);
    net.listing_games = 0;
    self->kill_timer();
    if (result < 0) {
      dlg_style.button_labels[0] = self->saved_button_label;
      self->clear();
      if (self->list_dialog != 0) {
        self->list_dialog->scalar_destruct(1);
      }
      self->list_dialog = 0;
      self->server_address[0] = '\0';
      return 0;
    }
    for (found = net.found_games; (found != 0 && (found->list_id != result)); found = found->next) {
    }
    if (found == 0) continue;
    civs_count = 0;
    mask = (uint)found->civs_alive;
    for (bit = 0; bit < 8; bit++) {
      if ((mask & 1) != 0) {
        civs_count++;
      }
      mask = (int)mask >> 1;
    }
    players_count = 0;
    mask = (uint)found->player_mask;
    for (bit = 0; bit < 8; bit++) {
      if ((mask & 1) != 0) {
        players_count++;
      }
      mask = (int)mask >> 1;
    }
    if ((players_count >= net.net_max_players) || (players_count >= civs_count) || (found->open_game == 0)) {
      if ((net.net_max_players <= players_count) || (civs_count <= players_count)) {
        popup_game_with_sprite("JOINEDMAX",(iptr)&popup_info_icon_sprite,0);
      }
      continue;
    }
    len1 = _strlen(found->version);
    len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
    if ((len1 != len2) || (_strcmp(found->version,str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
      dlg_style.button_labels[0] = self->saved_button_label;
      crt_strcpy(((i8 *)&text_args.strings),found->version);
      crt_strcpy(((i8 *)&text_args.string1),str_ptr_5_4_0f_multiplayer_26_march_99);
      popup_game_with_sprite("WRONGVERSION",(iptr)&popup_info_icon_sprite,0);
      dlg_style.button_labels[0] = (i8 *)self->button_label;
      continue;
    }
    if ((conn_type == 2) || (conn_type == 3)) {
      self->server_address[0] = '\0';
    }
    else {
      crt_strcpy(self->server_address,found->host_address);
    }
    crt_strcpy(self->server_game_name,found->game_name);
    crt_strcpy(self->server_info_c8,found->host_name);
    map.width = found->map_width;
    map.height = found->map_height;
    if ((conn_type == 2) || (conn_type == 3)) {
      net.net_server_address[0] = 0;
    }
    else {
      crt_strcpy(((i8 *)&net.net_server_address),found->host_address);
    }
    crt_strcpy(((i8 *)&net.net_game_name),found->game_name);
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),found->host_name);
    result = XD_OpenConnection(self->server_address,net.timeout_seconds * 1000);
    if (-1 < result) break;
    dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup_game_with_sprite("SERVERCONNECTFAIL",(iptr)&popup_info_icon_sprite,0);
  }
  self->list_dialog->close();
  dlg_style.button_labels[0] = self->saved_button_label;
  self->clear();
  if (self->list_dialog != 0) {
    self->list_dialog->scalar_destruct(1);
  }
  self->list_dialog = 0;
  wnd_flush_paint();
  return 1;
}

/* Part of net_session_init (00421fcd): a scenario: its directory, its intro.
   Returns where the caller goes on. */
static NetJoinScenario net_join_load_scenario(Dialog &scen_dlg,GamePalette &scen_palette)
{
  int civ;
  int gov;
  int sex;
  int label;
  iptr unit;
  i32 year;
  char *tribe_label;
  i8 scen_file[260];
  char subdir_path[260];
  bool paths_fit;
  
  /* (BF-222) the host's folder and file names, after our directory, must fit
     the 260-byte paths (subdir_path, scen_file); a longer one is a folder we
     cannot have */
  paths_fit = _strlen(((i8 *)&net_map_lock_slots_block.scenario_dir_path)) + _strlen(((i8 *)&net.loaded_file_subdir)) +
              _strlen(((i8 *)&net.loaded_file_name)) + 6 < 260;
  if (paths_fit) {
    crt_strcpy(subdir_path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(subdir_path,((i8 *)&net.loaded_file_subdir));
  }
  if (!paths_fit || (__chdir(subdir_path) < 0)) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    text_set_string_arg(0,((i8 *)&game_dirs.home));
    text_set_string_arg(1,((i8 *)&net.loaded_file_subdir));
    popup_game_with_sprite("DATAPATHFAIL",(iptr)&popup_info_icon_sprite,0);
    return JOIN_FAIL;
  }
  crt_strcat(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&net.loaded_file_subdir));
  rules_load(0);
  if ((net.loaded_file_kind == 2) || ((game.game_flags & GAME_SCENARIO) != 0)) {
    scen_dlg.Dialog::construct(0x4000);
    scen_palette.init();
    for (civ = 1; civ < 8; civ++) {
      if ((civs[civ].flags & CIVF_FEMALE_LEADER) == 0) {
        leaders[civs[civ].tribe].female = '\0';
      }
      else {
        leaders[civs[civ].tribe].female = '\x01';
      }
      civs[civ].leader_portrait = (u8)civs[civ].tribe;
      if (leaders[civs[civ].tribe].female != '\0') {
        civs[civ].leader_portrait = civs[civ].leader_portrait + '\x15';
      }
      if (leaders[civs[civ].tribe].plural < 1) {
        label = ~(int)leaders[civs[civ].tribe].plural + 1;
      }
      else {
        label = (int)leaders[civs[civ].tribe].plural;
      }
      tribe_label = (char *)label_string(label);
      if (_strcmp(civ_names[civ].tribe_name,tribe_label) == 0) {
        for (gov = 0; gov < 7; gov++) {
          for (sex = 0; sex < 2; sex++) {
            leaders[civs[civ].tribe].titles[gov][sex] = (short)(&government_titles[gov].male)[sex];
          }
        }
      }
      if (0 < leaders[civs[civ].tribe].adjective) {
        leaders[civs[civ].tribe].leader_name = -leaders[civs[civ].tribe].leader_name;
      }
      if (0 < leaders[civs[civ].tribe].adjective) {
        leaders[civs[civ].tribe].plural = -leaders[civs[civ].tribe].plural;
      }
      if (0 < leaders[civs[civ].tribe].adjective) {
        leaders[civs[civ].tribe].adjective = -leaders[civs[civ].tribe].adjective;
      }
    }
    if (cd_music.available == 0) {
      snd_cd_stop();
      snd_play_effect(SND_MENULOOP,0,1,0);
    }
    ui_flush_input();
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    if ((file_exists("TITLE.GIF") != 0) &&
       (((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_file("TITLE.GIF",10,0xc0,(iptr)&scen_palette) != 0)) {
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      main_frame_wnd_block.main_frame_wnd.maximize();
      wnd_flush_paint();
      input_flush_events();
    }
    __chdir(((i8 *)&game_dirs.home));
    crt_strcpy(((i8 *)&textbuf.buf),((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(((i8 *)&textbuf.buf),"\\");
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&net.loaded_file_name));
    path_set_extension(scen_file,(char *)(&textbuf.buf),lang_file_ext);
    if (cd_music.available != 0) {
      snd_play_effect(SND_MENUOK,0,0,0);
    }
    game.game_options = prefs.game_options & 0xffff7fff;
    game.message_options = prefs.message_options;
    text_set_string_arg(0,(const char *)&scenario.name);
    text_set_number_arg(0,(int)scenario.tech_paradigm);
    text_clear();
    year = game_turn_to_year((int)game.turn);
    text_append_year(year);
    text_set_string_arg(1,(const char *)&textbuf.buf);
    text_clear();
    if (scenario.max_turns == 0) {
      text_append_year(2000);
    }
    else {
      year = game_turn_to_year((int)scenario.max_turns);
      text_append_year(year);
    }
    text_set_string_arg(2,(const char *)&textbuf.buf);
    if (cd_music.available != 0) {
      snd_menu_end();
    }
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    dialog_state.timeout_start = timer_ticks();
    scen_dlg.init_default();
    if (scen_dlg.load_text((const char *)scen_file,"SCENARIO",0x4008001) == 0) {
      unit = (iptr)game.active_unit;
      if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
         ((int)units[unit].owner == (int)scenario.objective_civ)) {
        scen_dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0,0);
      }
      if (scen_dlg.show_modal(0) < 0) {
        snd_play_effect(SND_MENUOK,0,0,0);
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&scen_palette);
        main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
        main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
        wnd_flush_paint();
        return JOIN_SCENARIO_CANCELLED;
      }
    }
    else if (popup_game_flags("SCENARIOLOADED",1) < 0) {
      return JOIN_SCENARIO_CANCELLED;
    }
    scen_palette.destroy();
    scen_dlg.Dialog::destruct();
  }
  return JOIN_GO_ON;
}

/* 00421fad auto */
void NetGamelistView::view_destruct()

{
  net.free_game_list();
  return;
}

/* (net_session_init) After a scenario's intro: the set-up backdrop back. */
static void net_join_restore_backdrop(GamePalette *palette)
{
  if (net.loaded_file_kind == 2) {
    snd_play_effect(SND_MENUOK,0,0,0);
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    wnd_flush_paint();
  }
}

/* 00421fcd (by hand: the decompiler lost the lobby retry count and the stack dialog) */
/* Joining a network game: the game list (LAN) or the host's address
   (Internet; conn_type 0), the connection, the host's answers, the scenario,
   the player's profile and tribe, then the wait for the game.  1: joined. */
i32 NetGamelistView::net_session_init(int conn_type)

{
  int result;
  int popup;
  int retries;
  Dialog scen_dlg;
  GamePalette scen_palette;
  GamePalette palette;
  Dialog dlg;

  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  retries = 0;
  net_reset_msg_counts();
  if ((conn_type == 2) || (conn_type == 3)) {
    /* (modem, serial: no address) */
    server_address[0] = '\0';
    result = XD_OpenConnection(server_address,net.timeout_seconds * 1000);
    if (result < 0) {
      if ((net.net_protocol != 2) && (net.net_protocol != 3)) {
        dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
        crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
        popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
      }
      return 0;
    }
  }
  else if (conn_type == 0) {
    if (net_join_by_address(this,conn_type,retries,dlg) == 0) return 0;
  }
  else {
  /* LAN: the games the hosts announce, in a list */
    if (net_join_from_list(this,conn_type) == 0) return 0;
  }

  /* the host's id */
  dlg_style.pending_idle_callback = net_join_wait_server_idle;
  net.wait_start_ticks = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_reject_count != 0) || (net.nm_server_canceled_count != 0) ||
      (net.nm_server_full_count != 0)) {
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_reject_count != 0) {
      popup_game_with_sprite("REJECTEDLAN",(iptr)&popup_info_icon_sprite,0);
    }
    else if (net.nm_server_canceled_count != 0) {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("JOINEDMAX",(iptr)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  if (popup == -2) {
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    crt_strcpy(((i8 *)&text_args.string1),((i8 *)&net.net_game_name));
    popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
    return 0;
  }
  /* the game's state */
  diffeng_register_blocks();
  dlg_style.pending_idle_callback = net_join_wait_gamestate_idle;
  net.wait_start_ticks = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITTOJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if ((popup == -1) || (net.nm_server_canceled_count != 0)) {
    if (popup == -1) {
      net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    }
    else {
      text_set_string_arg(1,server_game_name);
      crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    XD_FlushSendBuffer(180000);
    return 0;
  }
  /* a scenario: its directory, its intro */
  if ((0 < net.loaded_file_kind) && (_strlen(((i8 *)&net.loaded_file_subdir)) != 0)) {
    switch (net_join_load_scenario(scen_dlg,scen_palette)) {
    case JOIN_GO_ON:
      break;
    case JOIN_FAIL:
      return 0;
    case JOIN_SCENARIO_CANCELLED:
      /* the scenario's intro cancelled: out of the game */
      net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
      net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
      XD_FlushSendBuffer(180000);
      scen_palette.destroy();
      scen_dlg.Dialog::destruct();
      return 0;
    }
  }

  /* the player: profile, tribe */
  passwords_refresh_flags();
  if (game_profile_popup(1) == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if (((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) &&
     (net.session_phase < 2)) {
    result = setup_choose_tribe(net.local_slot,3,0);
  }
  else {
    result = net_choose_civ_dialog(net.local_slot);
  }
  if (result != 0) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  /* in: wait for the player list, then the game */
  net_send_message(NM_PLAYER_INFO,0,0,0,0,0,0,0,0,0);
  diffeng_register_blocks();
  dlg_style.pending_idle_callback = net_join_wait_player_list_idle;
  net.wait_start_ticks = timer_ticks();
  net_reset_msg_counts();
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_canceled_count == 0) {
      popup_game_with_sprite("LOSTSERVER",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  net.nm_start_civ_count = 0;
  dlg_style.pending_idle_callback = net_join_wait_start_civ_idle;
  net.wait_start_ticks = timer_ticks();
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if (net.nm_server_canceled_count != 0) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    return 0;
  }
  net_reset_msg_counts();
  text_close_file();
  snd_menu_end();
  return 1;
}

/* 0042415c auto */
void __cdecl net_gamelist_poll_idle(void)

{
  net.process_messages(1,0);
  return;
}

/* 0042417a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_server_idle(void)

{
  timer_ticks();
  net.process_messages(1,0);
  if ((((net.nm_player_list_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_server_reject_count != 0)) || (net.nm_server_full_count != 0))
  {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004241f8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_gamestate_idle(void)

{
  int elapsed;
  size_t text_len;
  char *marker;
  char *text;
  
  elapsed = timer_ticks();
  elapsed -= net.wait_start_ticks;
  net.process_messages(1,0);
  if ((net.nm_join_gamestate_count == 0) && (net.nm_server_canceled_count == 0)) {
    if (2 < elapsed) {
      text = *(char **)((int)(*(Dialog * *)&net.dialog_stack_top)->line_list->next + 8);
      text_len = _strlen(text);
      marker = _strchr(text,0x3e);
      if (text + (text_len - 1) == marker) {
        *marker = '<';
      }
      else {
        *marker = '-';
        marker[1] = '>';
      }
      marker = _strchr(text,0x3c);
      if (marker == (char *)0x0) {
        text = _strchr(text,0x3e);
        text[-1] = '<';
      }
      else if (marker == text) {
        *marker = '>';
      }
      else {
        *marker = '-';
        marker[-1] = '<';
      }
      (*(Dialog * *)&net.dialog_stack_top)->render();
      net.wait_start_ticks = timer_ticks();
    }
  }
  else {
    (*(Dialog * *)&net.dialog_stack_top)->style_flags = (*(Dialog * *)&net.dialog_stack_top)->style_flags | 0x400;
    ((ModalMenuWindow *)((*(Dialog * *)&net.dialog_stack_top)->surf_wnd + 0x48))->end_modal_loop();
  }
  return;
}

/* 0042433c auto */
void __cdecl net_join_wait_player_list_idle(void)

{
  net.process_messages(1,0);
  if (((net.nm_player_list_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_lost_server_on_join_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0042439c auto */
void __cdecl net_join_wait_start_civ_idle(void)

{
  net.process_messages(1,0);
  if ((net.nm_start_civ_count != 0) || (net.nm_server_canceled_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004243ef auto */
void __cdecl net_join_wait_answer_idle(void)

{
  net.process_messages(1,0);
  if (((net.nm_server_answer_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_server_reject_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0042444f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_idle_cb(void)

{
  int elapsed;
  
  elapsed = timer_ticks();
  elapsed -= net.wait_start_ticks;
  net.process_messages(1,0);
  if ((net.server_conn_id == -1) && (0x4d8 < elapsed)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  else if (net.server_conn_id != -1) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004244e0 auto */
void NetGamelistView::restore_selection()

{
  int option_count;
  DialogOption *option;
  int top_index;
  int selected_index;
  iptr scroll_pos;
  
  option_count = list_dialog->count_group_options(0);
  (*(Scrollbar **)((iptr)list_dialog + 0x284))->set_range(0,option_count - 1);
  option = find_option((iptr)&selected_game_name,(iptr)&selected_host_name);
  list_dialog->selected_option = option;
  if (option_count < 0xb) {
    list_dialog->list_top_option[0] = first_option_value;
  }
  else {
    option = find_option((iptr)&top_game_name,(iptr)&top_host_name);
    list_dialog->list_top_option[0] = option;
  }
  if (*(int *)((iptr)list_dialog + 0x1b8) != 0) {
    top_index = list_dialog->option_group_index(list_dialog->list_top_option[0]);
    option_count = (int)list_dialog->list_rows[0];
    selected_index = list_dialog->option_group_index(list_dialog->selected_option);
    if (option_count + top_index <= selected_index) {
      option_count = list_dialog->option_group_index(list_dialog->selected_option);
      option = (list_dialog->option_at(
                         (option_count - (int)list_dialog->list_rows[0]) + 1));
      list_dialog->list_top_option[0] = option;
    }
    option_count = list_dialog->option_group_index(list_dialog->list_top_option[0]);
    top_index = list_dialog->option_group_index(list_dialog->selected_option);
    if (top_index < option_count) {
      list_dialog->list_top_option[0] = list_dialog->selected_option;
    }
  }
  scroll_pos = list_dialog->option_group_index(list_dialog->list_top_option[0]);
  (*(Scrollbar **)((iptr)list_dialog + 0x284))->set_pos(scroll_pos);
  return;
}

/* 00424688 auto */
DialogOption *NetGamelistView::find_option(iptr game_name,iptr host_name)

{
  int cmp;
  DialogOption *option;
  NetGameInfo *game2;
  
  option = first_option_value;
  game2 = net.found_games;
  while( true ) {
    if (game2 == 0) {
      return first_option_value;
    }
    cmp = _strcmp(game2->game_name,(char *)game_name);
    if ((cmp == 0) && (cmp = _strcmp(game2->host_name,(char *)host_name), cmp == 0))
    break;
    option = option->next;
    game2 = game2->next;
  }
  return option;
}

/* 0042472a auto */
void NetGamelistView::update_details()

{
  if (net.found_games == 0) {
    selected_game_name[0] = '\0';
    selected_host_name[0] = '\0';
    top_game_name[0] = '\0';
    top_host_name[0] = '\0';
  }
  else {
    get_entry_names((iptr)list_dialog->selected_option,(iptr)&selected_game_name,
                       (iptr)&selected_host_name);
    get_entry_names((iptr)list_dialog->list_top_option[0],(iptr)&top_game_name,
                       (iptr)&top_host_name);
  }
  return;
}

/* 004247b2 auto */
void NetGamelistView::get_entry_names(iptr option,iptr out_game_name,iptr out_host_name)

{
  int remaining;
  NetGameInfo *game2;
  
  for (game2 = net.found_games;
      (game2 != 0 && (*(int *)(option + 4) != game2->list_id));
      game2 = game2->next) {
  }
  if (game2 == 0) {
    game2 = net.found_games;
    for (remaining = list_dialog->option_group_index((DialogOption *)option);
        (game2->next != 0 && (remaining != 0)); remaining--) {
      game2 = game2->next;
    }
  }
  crt_strcpy((char *)((void *)out_game_name),game2->game_name);
  crt_strcpy((char *)((void *)out_host_name),game2->host_name);
  return;
}

/* 0042486f auto */
void NetGamelistView::add_info_lines()

{
  int line_y;
  DialogLine *line;
  int spacing;
  
  list_dialog->compute_layout();
  line_y = (int)list_dialog->title_height + (int)list_dialog->field_b8 + 2;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_14,0xe6,line_y,0x80,0));
  info_lines[0] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_15,0xe6,line_y,0x80,0));
  info_lines[1] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_16,0xe6,line_y,0x80,0));
  info_lines[2] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_17,0xe6,line_y,0x80,0));
  info_lines[3] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_18,0xe6,line_y,0x80,0));
  info_lines[4] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_19,0xe6,line_y,0x80,0));
  info_lines[5] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_20,0xe6,line_y,0x80,0));
  info_lines[6] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_21,0xe6,line_y,0x80,0));
  info_lines[7] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_22,0xe6,line_y + spacing,0x80,0));
  info_lines[8] = (u32)line->number;
  ((GameFont *)list_dialog->body_font)->line_spacing();
  return;
}

/* 00424ae9 auto */
i32 __cdecl net_gamelist_show_game_info(void *dialog,i32 unused)

{
  size_t text_len;
  i32 year;
  int seconds;
  int minutes;
  iptr scroll_pos;
  uint civ_bits;
  int civ_count;
  int bit;
  char text[256];
  NetGameInfo *found_game;
  
  found_game = net.found_games;
  if (net.found_games == 0) {
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[0],(const char *)&str_empty_23);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[1],(const char *)&str_empty_24);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[2],(const char *)&str_empty_25);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[3],(const char *)&str_empty_26);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[4],(const char *)&str_empty_27);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[5],(const char *)&str_empty_28);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[6],(const char *)&str_empty_29);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[7],(const char *)&str_empty_30);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[8],(const char *)&str_empty_31);
  }
  else {
    for (; (found_game != 0 &&
           (found_game->list_id != *(int *)((int)net.gamelist_view->list_dialog->selected_option + 4)));
        found_game = found_game->next) {
    }
    if (found_game != 0) {
      str_clear((char *)text);
      str_append_label((iptr)text,0x26f);
      str_append_space((iptr)text);
      crt_strcat(text,found_game->game_name);
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      str_append_label((iptr)text,(found_game->open_game == 0) + 0x277);
      str_append_rparen((iptr)text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[0],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x270);
      str_append_space((iptr)text);
      crt_strcat(text,found_game->host_name);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[1],(const char *)text);
      str_clear((char *)text);
      text_len = _strlen(found_game->scenario_name);
      if (text_len == 0) {
        str_append_label((iptr)text,0x34c);
      }
      else {
        crt_strcpy(text,found_game->scenario_name);
      }
      text_len = _strlen(text);
      if (0x19 < text_len) {
        text[21] = 0;           /* a long scenario name cut */
      }
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      game.game_flags = found_game->game_flags;
      game.first_launch_turn = found_game->first_launch_turn;
      scenario.year_increment[0] = found_game->year_increment;
      scenario.start_year = found_game->start_year;
      game.difficulty = found_game->difficulty;
      year = game_turn_to_year((int)found_game->turn);
      str_append_year((iptr)text,year);
      str_append_rparen((iptr)text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[2],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,found_game->difficulty + 0x279);
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      str_append_label((iptr)text,0x34e - (uint)(found_game->simultaneous == 0));
      str_append_rparen((iptr)text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[3],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x36c);
      str_append_space((iptr)text);
      str_append_int((iptr)text,(int)found_game->map_width / 2);
      crt_strcat(text,((i8 *)&gamelist_map_size_sep_str));
      str_append_int((iptr)text,(int)found_game->map_height);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[4],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x350 - (uint)(found_game->double_production == 0));
      crt_strcat(text,((i8 *)&gamelist_double_opts_sep_str));
      str_append_label((iptr)text,0x352 - (uint)(found_game->double_moves == 0));
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[5],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x273);
      str_append_space((iptr)text);
      str_append_label((iptr)text,found_game->barbarian_activity + 0x27f);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[6],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x276);
      str_append_space((iptr)text);
      if (found_game->turn_time_limit_ms == 0) {
        str_append_label((iptr)text,0x285);
      }
      else {
        seconds = found_game->turn_time_limit_ms / 1000;
        minutes = seconds / 0x3c;
        seconds = seconds % 0x3c;
        if (minutes < 10) {
          str_append_int((iptr)text,0);
        }
        str_append_int((iptr)text,minutes);
        crt_strcat(text,((i8 *)&gamelist_timer_colon_str));
        if (seconds < 10) {
          str_append_int((iptr)text,0);
        }
        str_append_int((iptr)text,seconds);
      }
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[7],(const char *)text);
      bit = 1;
      civ_count = 0;
      civ_bits = (uint)found_game->civs_alive;
      for (; civ_bits = (int)civ_bits >> 1, bit < 8; bit++) {
        if ((civ_bits & 1) != 0) {
          civ_count++;
        }
      }
      str_clear((char *)text);
      str_append_label((iptr)text,0x274);
      str_append_space((iptr)text);
      str_append_int((iptr)text,civ_count);
      crt_strcat(text,((i8 *)&gamelist_open_paren_str));
      str_append_label((iptr)text,0x275);
      str_append_space((iptr)text);
      bit = 0;
      civ_count = 0;
      civ_bits = (uint)found_game->player_mask;
      for (; bit < 8; bit++) {
        if ((civ_bits & 1) != 0) {
          civ_count++;
        }
        civ_bits = (int)civ_bits >> 1;
      }
      if ((found_game->session_phase < 3) || (civ_count == 0)) {
        str_append_label((iptr)text,0x353);
      }
      else {
        str_append_int((iptr)text,civ_count);
      }
      str_append_rparen((iptr)text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[8],(const char *)text);
    }
  }
  scroll_pos = net.gamelist_view->list_dialog->option_group_index(net.gamelist_view->list_dialog->list_top_option[0]);
  (*(Scrollbar **)((int)net.gamelist_view->list_dialog + 0x284))->set_pos(scroll_pos);
  net.gamelist_view->list_dialog->render();
  return 1;
}

/* 004253ef auto */
void NetGamelistView::clear()

{
  void *next;
  void *option;
  
  option = (void *)first_option_value;
  update_details();
  while (option != (void *)0x0) {
    next = *(void **)((int)option + 0x10);
    operator_delete(*(void **)((int)option + 8));
    operator_delete(option);
    option = next;
  }
  first_option_value = 0;
  list_dialog->option_count = 0;
  list_dialog->option_list = 0;
  list_dialog->selected_option = 0;
  return;
}

/* 004254a8 auto */
DialogOption *NetGamelistView::add_entry(const char *name2,uint id)

{
  int *count_ptr;
  DialogOption *entry;
  size_t name_len;
  void *name_copy;
  int entry_width;
  int max_width;
  DialogOption *last;
  DialogOption *option;
  
  last = 0;
  for (option = first_option_value; option != 0; option = option->next) {
    last = option;
  }
  entry = (DialogOption *)(operator_new(0x18));
  entry->next = 0;
  entry->prev = last;
  if (last == 0) {
    first_option_value = entry;
    list_dialog->option_list = entry;
  }
  else {
    last->next = entry;
  }
  entry->flags = 0;
  name_len = _strlen((char *)name2);
  name_copy = operator_new(name_len + 1);
  entry->text = (char *)name_copy;
  crt_strcpy((char *)((void *)entry->text),(const char *)((const void *)name2));
  if (*(char *)name2 == '\0') {
    entry->flags = entry->flags | 1;
  }
  entry->id = id;
  entry->group = 0;
  entry_width = ((GameFont *)list_dialog->body_font)->text_width((const char *)entry->text);
  max_width = list_dialog->option_icon_width();
  entry_width = entry_width + max_width + (int)list_dialog->option_extra_width + 5;
  max_width = (int)list_dialog->option_width;
  if (max_width <= entry_width) {
    max_width = entry_width;
  }
  list_dialog->option_width = max_width;
  count_ptr = (int *)((iptr)list_dialog + 0x28);
  *count_ptr = *count_ptr + 1;
  return entry;
}

/* 00425607 auto */
void NetGamelistView::start_timer(TimerFn callback)

{
  u32 timer;
  
  if (refresh_timer != 0) {
    kill_timer();
  }
  timer = game_timer_create(callback,0x32,1);
  refresh_timer = timer;
  return;
}

/* 00425650 auto */
void NetGamelistView::kill_timer()

{
  if (refresh_timer != 0) {
    game_timer_destroy(refresh_timer);
    refresh_timer = 0;
  }
  return;
}

/* 00425695 auto */
void __cdecl net_gamelist_poll_tick(void)

{
  i32 timer;
  
  net_send_message(NM_GET_SERVERS,0,0,0,0,0,0,0,0,0);
  net_gamelist_expire_stale();
  if (net.gamelist_view->refresh_timer != 0) {
    game_timer_destroy((i32)net.gamelist_view->refresh_timer);
  }
  timer = game_timer_create((TimerFn)net_gamelist_poll_tick,0xfa,1);
  net.gamelist_view->refresh_timer = timer;
  return;
}

/* 0042570c auto */
void __cdecl net_gamelist_expire_stale(void)

{
  bool removed;
  int now;
  NetGameInfo *next;
  NetGameInfo *game2;
  
  removed = false;
  game2 = net.found_games;
  now = timer_ticks();
  while (game2 != 0) {
    if (net.timeout_seconds * 0x3c < now - game2->last_seen) {
      if (net.found_games == game2) {
        net.found_games = game2->next;
      }
      if (game2->prev != 0) {
        game2->prev->next = game2->next;
      }
      if (game2->next != 0) {
        game2->next->prev = game2->prev;
      }
      next = game2->next;
      operator_delete(game2);
      removed = true;
      game2 = next;
    }
    else {
      game2 = game2->next;
    }
  }
  if (removed) {
    net_gamelist_refresh();
  }
  return;
}

/* 004257fe auto */
void __cdecl net_gamelist_refresh(void)

{
  DialogOption *entry;
  i32 civ_bits;
  i32 bit;
  i32 total_civs;
  NetGameInfo *game2;
  i32 human_count;
  
  if (net.gamelist_view != 0) {
    net.gamelist_view->clear();
    if (net.found_games == 0) {
      text_clear();
      text_append_label(0x286);
      net.gamelist_view->add_entry((const char *)&textbuf.buf,0);
      net.gamelist_view->update_details();
    }
    else {
      for (game2 = net.found_games; game2 != 0; game2 = game2->next) {
        if (game2->list_id == 0) {
          game2->list_id = net.gamelist_view->next_game_id;
          net.gamelist_view->next_game_id = net.gamelist_view->next_game_id + 1;
        }
        entry = (net.gamelist_view->add_entry(game2->game_name,
                                    game2->list_id));
        bit = 0;
        total_civs = 0;
        civ_bits = (uint)game2->civs_alive;
        for (; bit < 8; bit++) {
          if ((civ_bits & 1) != 0) {
            total_civs++;
          }
          civ_bits = civ_bits >> 1;
        }
        bit = 0;
        human_count = 0;
        civ_bits = (uint)game2->player_mask;
        for (; bit < 8; bit++) {
          if ((civ_bits & 1) != 0) {
            human_count++;
          }
          civ_bits = civ_bits >> 1;
        }
        if (((net.net_max_players <= human_count) || (total_civs <= human_count)) ||
           (game2->open_game == 0)) {
          entry->flags = entry->flags | 1;
        }
      }
    }
    net.gamelist_view->restore_selection();
    net_gamelist_show_game_info((void *)net.gamelist_view->list_dialog,0);
  }
  return;
}

/* 00430822 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_meeting_wait_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  now = timer_ticks();
  if (((net.timeout_seconds * 0x3c <= now - civ_rating_title_font_block.mp_meeting_wait_start_i32) || (advisor_window_block.mp_meeting_reply != -1)) ||
     (net.nm_parley_cancel_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    if (advisor_window_block.mp_meeting_reply == -1) {
      advisor_window_block.mp_meeting_reply = 0;
    }
  }
  return;
}

/* 0044528e auto */
i32 __cdecl net_prompt_ip_address(void)

{
  char *proto_addr;
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if ((net.lobby_launch_mode == -1) || (net.lobby_launch_mode == 1)) {
    proto_addr = XD_GetCurrentProtoAddr(net.adapter_index,net.net_protocol);
    if (proto_addr == (char *)0x0) {
      do {
        dlg.load_text_section((const char *)&str_game_7,(const char *)&str_ipaddress,0x1f,(iptr)&menu_prefs.net_ip_address,0,0,0,1);
        choice = dlg.run_modal((iptr)input,0);
        if (choice == -1) {
          result = 0;
          return result;
        }
        input_len = _strlen(input);
      } while (input_len == 0);
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),input);
    }
    else {
      proto_addr = XD_GetCurrentProtoAddr(net.adapter_index,net.net_protocol);
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),proto_addr);
    }
  }
  else {
    crt_strcpy(((i8 *)&menu_prefs.net_ip_address),((i8 *)&net.lobby_launch_info));
  }
  result = 1;
  return result;
}

/* 0044542c auto */
i32 __cdecl net_prompt_player_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (net.lobby_launch_mode == -1) {
    do {
      dlg.load_text_section((const char *)&str_game_7,(const char *)&str_netname,0x1f,(iptr)&menu_prefs.net_player_name,0,0,0,1);
      choice = dlg.run_modal((iptr)input,0);
      if (choice == -1) {
        result = 0;
        return result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&menu_prefs.net_player_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&menu_prefs.net_player_name),0,0x20);
    _strncpy(((i8 *)&menu_prefs.net_player_name),((i8 *)&net.lobby_short_name),0x1f);
  }
  result = 1;
  return result;
}

/* 00445592 auto */
i32 __cdecl net_prompt_game_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (net.lobby_launch_mode == -1) {
    do {
      dlg.load_text_section((const char *)&str_game_7,(const char *)&str_gamename,0x1f,(iptr)&menu_prefs.net_game_name,0,0,0,1);
      choice = dlg.run_modal((iptr)input,0);
      if (choice == -1) {
        result = 0;
        return result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&menu_prefs.net_game_name),input);
    crt_strcpy(((i8 *)&net.net_game_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&menu_prefs.net_game_name),0,0x20);
    _strncpy(((i8 *)&menu_prefs.net_game_name),((i8 *)&net.lobby_session_name),0x1f);
  }
  result = 1;
  return result;
}

/* 00445e46 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_choose_civ_dialog(int title_offset)

{
  iptr civ2;
  iptr choice;
  int result;
  iptr leader_str;
  i32 ret;
  i32 arg6;
  i32 arg7;
  i32 arg8;
  iptr civ;
  u8 is_female;
  i8 input[260];
  int index;
  int available_count;
  int option_count2;
  iptr selected2;
  Dialog dlg;
  DialogOption *option;
  iptr option_civs[8];
  int wait_start;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  selected2 = 0;
  net.player_info_has_names = 0;
  net.player_info_has_titles = 0;
  do {
    dlg.Dialog::reset(0x801);
    dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
    index = 0;
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
        text_clear();
        civ2 = (iptr)(civ_name_plural(civ));
        text_append((const char *)civ2);
        text_append_space();
        text_append_lparen();
        civ2 = (iptr)(civ_leader_name(civ));
        text_append((const char *)civ2);
        text_append_rparen();
        text_append_space();
        text_append_lparen();
        text_append_int((int)civs[civ].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_65);
        text_append_int((int)civs[civ].num_military_units);
        text_append_space();
        text_append_label(0xc4);
        text_append((const char *)&str_empty_66);
        result = index;
        index++;
        option = (DialogOption *)((byte *)dlg.add_option((const char *)&textbuf.buf,result,0));
        option_civs[option->id] = civ;
      }
    }
    option_count2 = index;
    for (;;) {
      do {
        available_count = 0;
        for (index = 0; index < option_count2; index++) {
          option = (DialogOption *)((byte *)dlg.find_option(index));
          if (option != (DialogOption *)0x0) {
            (*((char *)&civ + 0)) = (byte)option_civs[option->id];
            if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
               ((1 << ((byte)civ & 0x1f) & (int)civ_saved_cursor_x_block.net_claimed_civs_mask) == 0)) {
              available_count++;
              dlg.set_option_disabled(option->id,0);
              if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
                ((RadioGroup *)dlg.radio_group)->enable_item(option->id);
              }
            }
            else {
              dlg.set_option_disabled(option->id,1);
              if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
                ((RadioGroup *)dlg.radio_group)->disable_item(option->id);
              }
            }
          }
        }
        if (available_count == 0) {
          popup_game((const char *)&str_joinedmax);
          dlg.close();
          ret = 1;
          return ret;
        }
        for (index = 0; index < option_count2; index++) {
          option = (DialogOption *)((byte *)dlg.find_option(selected2));
          if ((option != (DialogOption *)0x0) && ((option->flags_byte & 1) == 0)) {
            dlg.set_default_choice(option->id);
            if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
              ((RadioGroup *)dlg.radio_group)->set_selected(option->id);
            }
            break;
          }
          selected2++;
          if (selected2 == option_count2) {
            selected2 = 0;
          }
        }
        net_reset_msg_counts();
        net.wait_start_ticks = timer_ticks();
        dlg.style_flags = dlg.style_flags & 0xfffffbff;
        dlg.result_id = 0;
        dlg.result_extra = 0;
        dlg_style.pending_idle_callback = net_lock_wait_idle_cb;
        choice = dlg.show_modal(0);
        selected2 = ((RadioGroup *)dlg.radio_group)->get_selected();
        if (choice < 0) {
          ret = 1;
          return ret;
        }
      } while (net.nm_lock_data_count != 0);
      if (net.nm_server_canceled_count != 0) {
        popup_game_with_sprite((const char *)&str_gamecanceled,(iptr)&popup_info_icon_sprite,0);
        ret = 1;
        return ret;
      }
      civ2 = option_civs[choice];
      net.player_info_civ = civ2;
      selected2 = choice;
      net_send_message(NM_LOCK_NATION,0,0,civ2,0,0,0,0,0,0);
      if (net.host_flag == '\0') {
        net_reset_msg_counts();
        wait_start = timer_ticks();
        while (((net.nm_nation_accept_count == 0 && (net.nm_nation_reject_count == 0)) &&
               (result = timer_ticks(), result - wait_start < net.timeout_seconds * 0x3c))) {
          net.process_messages(1,0);
        }
      }
      if ((net.nm_nation_accept_count == 0) && (net.nm_nation_reject_count == 0)) {
        dlg.close();
        popup_game_with_sprite((const char *)&str_serverconnectfail,(iptr)&popup_info_icon_sprite,0);
        ret = 1;
        return ret;
      }
      if (net.nm_nation_reject_count == 0) break;
      popup_game_with_sprite((const char *)&str_alreadychosen,(iptr)&popup_info_icon_sprite,0);
    }
    dlg.close();
    (*((char *)&civ + 0)) = (byte)civ2;
    game.human_players = game.human_players | (byte)(1 << ((byte)civ & 0x1f));
    game.player_civ = (byte)civ;
    game.turn_civ = (byte)civ;
    game.game_options = game.game_options & ~OPT_TUTORIAL;
    human_civ[0] = civ2;
    result = civ_check_password(civ2);
    if (result == 0) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
    }
    else {
      dlg.load_game_section((const char *)&str_gender_4,1);
      dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      dlg_style.pending_idle_callback = net_pump_idle_cb;
      result = dlg.show_modal(0);
      net.player_info_gender = result;
      snd_play_effect(SND_MENUOK,0,0,0);
      if (result < 0) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
        net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
      }
      else {
        is_female = (u8)result;
        leaders[civs[game.player_civ].tribe].female = is_female;
        civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
        if (result != 0) {
          civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
        }
        arg8 = 1;
        arg7 = 0;
        arg6 = 0;
        ret = 0;
        leader_str = (iptr)(civ_leader_name((int)game.player_civ));
        result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_4,0x17,leader_str,ret,arg6,arg7,arg8);
        if (result == 0) {
          dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
          dlg_style.pending_idle_callback = net_pump_idle_cb;
          result = dlg.run_modal((iptr)input,0);
          if (result == 0) {
            crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),input);
            crt_strcpy(((i8 *)&net.player_info_leader_name),input);
            net.player_info_tribe_name[0] = 0;
            net.player_info_adjective[0] = 0;
            ret = 0;
            return ret;
          }
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
        }
      }
    }
  } while( true );
}

/* 00447170 auto */
void *NetGamelistView::view_scalar_destruct(uint flags)

{
  view_destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004471c0 auto */
LobbyList *LobbyList::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 00450480 auto */
void ParleyWindow::create_advance_list_pane(RECT *rect,iptr pane,int their_side)

{
  int result;
  int scroll_width;
  Button *new_ctrl;
  Wnd *parent22;
  Wnd *parent2;
  u32 hotspot;
  u32 scrollbar;
  uptr advance;
  RECT sb_rect;
  int can_see_other;
  iptr civ;
  
  can_see_other = 0;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  result = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS);
  if ((((result == 0) && (result = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), result == 0)) &&
      ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_EMBASSY) == 0)) &&
     (((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_ALLIANCE) == 0 &&
      (game.reveal_map == '\0')))) {
    can_see_other = 0;
  }
  else {
    can_see_other = 1;
  }
  list_counts[pane] = 0;
  civ = human_civ[0];
  if ((their_side != 0) && (can_see_other != 0)) {
    civ = (int)parley_wnd_block.other_civ;
  }
  for (advance = 0; (int)advance < 100; advance++) {
    if (their_side == 0) {
      result = civ_has_advance(human_civ[0],advance);
      if ((result != 0) &&
         ((can_see_other == 0 || (result = civ_has_advance((int)parley_wnd_block.other_civ,advance), result == 0)))) {
        list_items[(u32)list_counts[pane] + pane * 0x801] = advance;
        list_selected[(u32)list_counts[pane] + pane * 0x801] = 0;
        list_counts[pane]++;
      }
    }
    else if (((human_civ[0] == civ) ||
             (result = civ_has_advance(human_civ[0],advance), result != 0)) ||
            (result = civ_has_advance(civ,advance), result == 0)) {
      if (((human_civ[0] == civ) &&
          (result = civ_has_advance(human_civ[0],advance), result == 0)) &&
         (((int)advance < 0x59 && (advances[advance].enabled == 1)))) {
        list_items[(u32)list_counts[pane] + pane * 0x801] = advance;
        list_selected[(u32)list_counts[pane] + pane * 0x801] = 0;
        list_counts[pane]++;
      }
    }
    else {
      list_items[(u32)list_counts[pane] + pane * 0x801] = advance;
      list_selected[(u32)list_counts[pane] + pane * 0x801] = 0;
      list_counts[pane]++;
    }
  }
  if ((int)(u32)list_counts[pane] < 1) {
    list_first_row[pane] = 0xffffffff;
  }
  else {
    list_first_row[pane] = 0;
  }
  list_columns[pane] = 3;
  list_rects[pane * 4] = (u32)rect->left;
  list_rects[pane * 4 + 1] = (u32)rect->top;
  list_rects[pane * 4 + 2] = (u32)rect->right;
  list_rects[pane * 4 + 3] = (u32)rect->bottom;
  if ((int)(u32)list_rows[pane] < (int)(u32)list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    result = sb_rect.right;
    list_rects[pane * 4 + 2] -= scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = result - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (u32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      parent2 = (Wnd *)0x0;
    }
    else {
      parent2 = &window;
    }
    ((Scrollbar *)list_scrollbars[pane])->create((iptr)parent2,pane + 0x411,
               (iptr)&sb_rect,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               ((u32)list_counts[pane] - 1) - (u32)list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size((u32)list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (u32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    parent22 = (Wnd *)0x0;
  }
  else {
    parent22 = &window;
  }
  ((Hotspot *)list_hotspots[pane])->create((iptr)parent22,pane + 0x413,
             (iptr)((u8 *)this + pane * 0x10 + 0x3c0));
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_advance_list_on_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_advance_list_on_click);
  parley_draw_advance_list(pane);
  return;
}

/* 00450ae6 auto */
void __cdecl parley_advance_list0_on_scroll(i32 pos)

{
  parley_advance_list_on_scroll(0,pos);
  return;
}

/* 00450b04 auto */
void __cdecl parley_advance_list1_on_scroll(i32 pos)

{
  parley_advance_list_on_scroll(1,pos);
  return;
}

/* 00450b22 auto */
void __cdecl parley_advance_list_on_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)window + 0x48))->activate();
  window->list_first_row[pane] = pos;
  parley_draw_advance_list(pane);
  return;
}

/* 00450b83 auto */
void __cdecl parley_advance_list_on_click(int control_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *window;
  
  row = wnd_get_active_child();
  if (row == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(row - 0x48);
  }
  ((Wnd *)((iptr)window + 0x48))->activate();
  pane = control_id - 0x413;
  ((WndBase *)((iptr)window + 0x48))->get_cursor_pos((iptr)&cursor_x,(iptr)&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_advance_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = ((Wnd *)((iptr)window + 0x48))->is_shift_down();
    if (key_down == 0) {
      key_down = ((Wnd *)((iptr)window + 0x48))->is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[i + pane * 0x801] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(row * 4 + pane * 0x2004 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(row * 4 + pane * 0x2004 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[i + pane * 0x801] = 0;
      }
      if (window->list_anchor_row[pane] < row) {
        i = window->list_anchor_row[pane];
        range_end = row;
      }
      else {
        range_end = window->list_anchor_row[pane];
        i = row;
      }
      for (; i <= range_end; i++) {
        window->list_selected[i + pane * 0x801] = 1;
      }
    }
    parley_draw_advance_list(pane);
  }
  return;
}

/* 00450df6 auto */
int __cdecl parley_advance_list_hit_test(int px,int py,int pane)

{
  iptr child;
  int result;
  int list_height;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (py < (int)window->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane * 4 + 3]) {
    if (px < (int)window->list_rects[pane * 4]) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane * 4 + 2]) {
      result = (int)window->list_rects[pane * 4 + 1];
      list_height = rect_height((RECT *)(pane * 0x10 + (iptr)window + 0x3c0));
      result = (py - result) / (list_height / (window->list_rows[pane] + 1)) +
              window->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 00450f0b auto */
void __cdecl parley_draw_advance_list(iptr pane)

{
  u8 *has_sel;
  int tmp;
  int tmp2;
  const char *name_label;
  int icon_x;
  i32 bg_color2;
  i8 sprite_rect[16];
  int text_y_offset;
  iptr advance;
  int icon_step;
  int row;
  int row_top;
  int list_left;
  int list_width2;
  int shadow_color2;
  int text_x_offset;
  Sprite *icon_sprite;
  int text_color2;
  ParleyWindow *window;
  int selected2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(tmp - 0x48);
  }
  window->get_clip((RECT *)saved_clip);
  window->set_clip((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  window->fill_rect((iptr)(((u8 *)window + 0x3c0) + pane * 0x10),dlg_style.list_bg);
  if (window->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  list_left = (int)window->list_rects[pane * 4];
  list_width2 = rect_width((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  row_height = row_height / (window->list_rows[pane] + 1);
  tmp = row_height / 2;
  tmp2 = font2->font_cached_height();
  text_y_offset = tmp - tmp2 / 2;
  has_sel = (u8 *)&window->side_selected[pane];
  has_sel[0] = '\0';
  has_sel[1] = '\0';
  has_sel[2] = '\0';
  has_sel[3] = '\0';
  for (row = 0; row < window->list_counts[pane]; row++) {
    if (window->list_selected[pane * 0x801 + row] != 0) {
      has_sel = (u8 *)&window->side_selected[pane];
      has_sel[0] = 1;
      has_sel[1] = '\0';
      has_sel[2] = '\0';
      has_sel[3] = '\0';
      break;
    }
  }
  for (row = 0; row < window->list_rows[pane] + 1;
      row = row + 1) {
    row_top = (int)window->list_rects[pane * 4 + 1] + row * row_height +
               panel_layout.margin_y;
    SetRect(&row_rect,list_left,row_top,list_left + list_width2,row_top + row_height);
    if (((row == 0) || (window->list_first_row[pane] != -1)) &&
       (window->list_first_row[pane] + row <
        window->list_counts[pane])) {
      selected2 = (int)window->list_selected[pane * 0x801 + (window->list_first_row[pane] + row)];
      if (selected2 == 0) {
        text_color2 = dlg_style.list_text;
        shadow_color2 = dlg_style.list_shadow;
      }
      else {
        text_color2 = dlg_style.sel_text_color;
        shadow_color2 = dlg_style.sel_text_shadow_color;
      }
      advance = (int)window->list_items[pane * 0x801 + (window->list_first_row[pane] + row)];
      icon_sprite = (Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[advance].category * 0xf0 +
                 (char)advances[advance].epoch * 0x3c);
      tmp = icon_sprite->get_width();
      text_x_offset = panel_layout.margin_x * 3 + tmp * 2;
      row_rect.left += text_x_offset;
      if (selected2 == 0) {
        bg_color2 = dlg_style.list_bg;
      }
      else {
        bg_color2 = dlg_style.list_sel_bg;
      }
      window->fill_rect((iptr)&row_rect,bg_color2);
      if (window->list_first_row[pane] == -1) {
        text_clear();
        if ((pane == 0) || ((int)parley_wnd_block.mode == 7)) {
          text_append_label(0x36b);
        }
        else {
          text_append_label(0x36a);
        }
        has_sel = (u8 *)&window->side_selected[pane];
        has_sel[0] = '\0';
        has_sel[1] = '\0';
        has_sel[2] = '\0';
        has_sel[3] = '\0';
        tmp = rect_width((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
        tmp2 = font2->text_width((const char *)&textbuf.buf);
        text_x_offset = (tmp - tmp2) / 2;
      }
      else {
        icon_step = icon_sprite->get_width();
        icon_step += panel_layout.margin_x * 2;
        icon_x = icon_step;
        if ((window->list_first_row[pane] + row & 1U) == 0) {
          icon_x = panel_layout.margin_x;
        }
        tmp2 = row_top + row_height / 2;
        tmp = icon_sprite->get_height();
        icon_sprite->draw((iptr)sprite_rect,(iptr)&parley_wnd_block,list_left + icon_x,tmp2 - tmp / 2);
        text_clear();
        name_label = (label_string(advances[advance].name));
        text_append(name_label);
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        window->draw_text((iptr)font2,(const char *)&textbuf.buf,list_left + text_x_offset + 2,
                   row_top + text_y_offset + 1,5);
        gfx_set_text_color_index(text_color2);
        window->draw_text((iptr)font2,(const char *)&textbuf.buf,list_left + text_x_offset + 1,
                   row_top + text_y_offset,5);
      }
      gfx_set_text_color_index(text_color2);
      window->draw_text((iptr)font2,(const char *)&textbuf.buf,list_left + text_x_offset,
                 row_top + text_y_offset,5);
    }
  }
  window->set_clip((RECT *)saved_clip);
  window->update_buttons();
  ((Control *)window->list_hotspots[pane])->show_and_redraw()
  ;
  return;
}

/* 004683f0 auto */
void ParleyWindow::create_unit_list_pane(RECT *rect,int pane,i32 own_side)

{
  u8 *field;
  int sb_right_orig;
  int scroll_width;
  Button *new_ctrl;
  u8 *parent22;
  u8 *parent2;
  i32 hotspot;
  i32 scrollbar;
  RECT sb_rect;
  int window;
  
  window = wnd_get_active_child();
  if (window == 0) {
    window = 0;
  }
  else {
    window -= 0x48;
  }
  prefs_main_menu_choice_block.parley_list_own_side[pane] = own_side;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  prefs_main_menu_choice_block.parley_units_by_city = 0;
  build_unit_list(pane,0);
  if (list_counts[pane] < 1) {
    field = (u8 *)&list_first_row[pane];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&list_first_row[pane];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&list_columns[pane];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  list_rects[pane * 4] = rect->left;
  list_rects[pane * 4 + 1] = rect->top;
  list_rects[pane * 4 + 2] = rect->right;
  list_rects[pane * 4 + 3] = rect->bottom;
  if (list_rows[pane] <
      list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    sb_right_orig = sb_rect.right;
    list_rects[pane * 4 + 2] =
         (int)list_rects[pane * 4 + 2] - scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = sb_right_orig - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      parent2 = (u8 *)0x0;
    }
    else {
      parent2 = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[pane])->create((iptr)parent2,
               pane + 0x3fa,(iptr)&sb_rect,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               list_counts[pane] -
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size(
               list_rows[pane] - 1);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    parent22 = (u8 *)0x0;
  }
  else {
    parent22 = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[pane])->create((iptr)parent22,
             pane + 0x3fc,(iptr)(((u8 *)this + 0x39c) + pane * 0x10 + 0x24));
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_unit_list_on_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_unit_list_on_click);
  parley_draw_unit_list(pane);
  return;
}

/* 00468797 auto */
void __cdecl parley_unit_list_scroll_left(i32 pos)

{
  parley_unit_list_on_scroll(0,pos);
  return;
}

/* 004687b5 auto */
void __cdecl parley_unit_list_scroll_right(i32 pos)

{
  parley_unit_list_on_scroll(1,pos);
  return;
}

/* 004687d3 auto */
void __cdecl parley_unit_list_on_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)window + 0x48))->activate();
  window->list_first_row[pane] = pos;
  parley_draw_unit_list(pane);
  return;
}

/* 00468834 auto */
void __cdecl parley_unit_list_on_click(int control_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *window;
  
  row = wnd_get_active_child();
  if (row == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(row - 0x48);
  }
  ((Wnd *)((iptr)window + 0x48))->activate();
  pane = control_id - 0x3fc;
  ((WndBase *)((iptr)window + 0x48))->get_cursor_pos((iptr)&cursor_x,(iptr)&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_unit_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = ((Wnd *)((iptr)window + 0x48))->is_shift_down();
    if (key_down == 0) {
      key_down = ((Wnd *)((iptr)window + 0x48))->is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[pane * 0x801 + i] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(pane * 0x2004 + row * 4 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(pane * 0x2004 + row * 4 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[pane * 0x801 + i] = 0;
      }
      if (window->list_anchor_row[pane] < row) {
        i = window->list_anchor_row[pane];
        range_end = row;
      }
      else {
        range_end = window->list_anchor_row[pane];
        i = row;
      }
      for (; i <= range_end; i++) {
        window->list_selected[pane * 0x801 + i] = 1;
      }
    }
    parley_draw_unit_list(pane);
  }
  return;
}

/* 00468aa7 auto */
int __cdecl parley_unit_list_hit_test(int px,int py,int pane)

{
  iptr child;
  int result;
  int list_height;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (py < (int)window->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane * 4 + 3]) {
    if (px < (int)window->list_rects[pane * 4]) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane * 4 + 2]) {
      result = (int)window->list_rects[pane * 4 + 1];
      list_height = rect_height((RECT *)(pane * 0x10 + (iptr)window + 0x3c0));
      result = (py - result) / (list_height / window->list_rows[pane]) +
              window->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 00468bb9 auto */
void __cdecl parley_draw_unit_list(iptr pane)

{
  int stale;
  u8 *has_sel;
  int tmp;
  int tmp2;
  iptr str;
  int row_mid;
  i32 denom;
  int icon_x;
  i32 bg_color2;
  int icon_width;
  i8 sprite_rect[16];
  int text_y_offset;
  iptr unit;
  int icon_step;
  int row;
  int row_top;
  int list_left;
  int list_width2;
  int shadow_color2;
  int text_x_offset;
  Sprite *unit_sprite;
  int text_color2;
  ParleyWindow *this2;
  int selected2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(tmp - 0x48);
  }
  this2->get_clip((RECT *)saved_clip);
  this2->set_clip((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  for (;;) {
    this2->fill_rect((iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10),dlg_style.list_bg);
    if (this2->large_layout == 0) {
      font2 = (GameFont *)((i8 *)&font_size14);
    }
    else {
      font2 = (GameFont *)((i8 *)&font_size21);
    }
    list_left = (int)this2->list_rects[pane * 4];
    list_width2 = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
    row_height = rect_height((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
    row_height = row_height / this2->list_rows[pane];
    tmp = row_height / 2;
    tmp2 = font2->font_cached_height();
    text_y_offset = tmp - tmp2 / 2;
    has_sel = (u8 *)&this2->side_selected[pane];
    has_sel[0] = '\0';
    has_sel[1] = '\0';
    has_sel[2] = '\0';
    has_sel[3] = '\0';
    for (row = 0; row < this2->list_counts[pane];
        row = row + 1) {
      if (this2->list_selected[row + pane * 0x801] != 0) {
        has_sel = (u8 *)&this2->side_selected[pane];
        has_sel[0] = 1;
        has_sel[1] = '\0';
        has_sel[2] = '\0';
        has_sel[3] = '\0';
        break;
      }
    }
    stale = 0;
    for (row = 0; row < this2->list_rows[pane]; row++) {
      row_top = (int)this2->list_rects[pane * 4 + 1] + row_height * row;
      SetRect(&row_rect,list_left,row_top,list_width2 + list_left,row_height + row_top);
      if ((row != 0) && (this2->list_first_row[pane] == -1)) {
        break;
      }
      if (this2->list_first_row[pane] + row <
          this2->list_counts[pane]) {
        selected2 = (int)this2->list_selected[pane * 0x801 + (this2->list_first_row[pane] + row)];
        if (selected2 == 0) {
          text_color2 = dlg_style.list_text;
          shadow_color2 = dlg_style.list_shadow;
        }
        else {
          text_color2 = dlg_style.sel_text_color;
          shadow_color2 = dlg_style.sel_text_shadow_color;
        }
        unit = 0;
        if ((0 < this2->list_counts[pane]) &&
           (unit = this2->find_unit_by_id(
                                  (i32)this2->list_items_b[pane * 0x801 + (this2->list_first_row[pane] + row)]), unit < 0)) {
          stale = 1;  /* not found: the list is rebuilt and drawn again */
          break;
        }
        unit_sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c);
        if (this2->large_layout == 0) {
          tmp = unit_sprite->get_width();
          icon_width = (tmp * 2) / 3;
        }
        else {
          icon_width = unit_sprite->get_width();
        }
        text_x_offset = (-(uint)(this2->large_layout == 0) & 0xfffffffc) + 10 + icon_width * 2;
        row_rect.left = row_rect.left + text_x_offset - 3;
        if (selected2 == 0) {
          bg_color2 = dlg_style.list_bg;
        }
        else {
          bg_color2 = dlg_style.list_sel_bg;
        }
        this2->fill_rect((iptr)&row_rect,bg_color2);
        if (this2->list_first_row[pane] == -1) {
          text_clear();
          str = (iptr)(label_string(label_table.string_ids[0x2df]));
          text_append((const char *)str);
          has_sel = (u8 *)&this2->side_selected[pane];
          has_sel[0] = '\0';
          has_sel[1] = '\0';
          has_sel[2] = '\0';
          has_sel[3] = '\0';
          tmp = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
          tmp2 = font2->text_width((const char *)&textbuf.buf);
          text_x_offset = (tmp - tmp2) / 2;
        }
        else {
          denom = 0x30;
          tmp = font2->font_cached_height();
          gfx_set_scale_ratio(tmp * 2,denom);
          tmp = font2->font_cached_height();
          icon_step = (tmp << 7) / 0x30;
          icon_x = icon_step;
          if ((this2->list_first_row[pane] + row & 1U) == 0) {
            icon_x = 0;
          }
          row_mid = row_top + row_height / 2;
          tmp = unit_sprite->get_height();
          tmp2 = font2->font_cached_height();
          unit_sprite->draw((iptr)sprite_rect,(iptr)&parley_wnd_block,list_left + icon_x,
                     row_mid - (tmp * tmp2) / 0x30);
          gfx_set_scale_ratio(1,1);
          if (prefs_main_menu_choice_block.parley_units_by_city == 0) {
            text_clear();
            text_append_string_id(unit_types[units[unit].type].name);
            if ((((int)parley_wnd_block.mode == 9) ||
                ((((int)parley_wnd_block.mode == 0xe && (pane == 0)) && (this2->local_units_revealed != '\0')))) ||
               (((((int)parley_wnd_block.mode == 0xe && (pane == 1)) && (this2->remote_units_revealed != '\0')) ||
                (((int)parley_wnd_block.mode == 0xf && (this2->remote_units_revealed != '\0')))))) {
              text_append_str((const char *)&str_empty_76);
              str = unit_home_city_name(unit);
              text_append_str((const char *)str);
              text_append_str((const char *)&str_empty_77);
            }
          }
          else {
            text_clear();
            str = unit_home_city_name(unit);
            text_append_str((const char *)str);
            text_append_str((const char *)&str_empty_78);
            text_append_string_id(unit_types[units[unit].type].name);
            text_append_str((const char *)&str_empty_79);
          }
        }
        if (shadow_color2 != text_color2) {
          gfx_set_text_color_index(shadow_color2);
          this2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_offset + list_left + 2,
                     text_y_offset + row_top + 1,5);
          gfx_set_text_color_index(text_color2);
          this2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_offset + list_left + 1,
                     text_y_offset + row_top,5);
        }
        gfx_set_text_color_index(text_color2);
        this2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_offset + list_left,
                   text_y_offset + row_top,5);
      }
    }
    if (stale == 0) {
      this2->set_clip((RECT *)saved_clip);
      this2->update_buttons();
      ((Control *)this2->list_hotspots[pane])->show_and_redraw();
      return;
    }
    this2->build_unit_list(pane,1);
    if (this2->list_counts[pane] == 0) {
      has_sel = (u8 *)&this2->list_first_row[pane];
      has_sel[0] = 0xff;
      has_sel[1] = 0xff;
      has_sel[2] = 0xff;
      has_sel[3] = 0xff;
    }
    else if (this2->list_counts[pane] <
             this2->list_counts[pane]) {
      this2->list_first_row[pane] =
           this2->list_counts[pane] - 1;
    }
  }
}

/* 004692ea auto */
void __cdecl parley_reveal_city_origin_clicked(void)

{
  iptr child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)window + 0x48))->activate();
  ((Control *)window->info_hide_buttons[0])->show_and_redraw();
  ((Control *)window->info_show_buttons[0])->hide();
  window->local_units_revealed = 1;
  ((Button *)window->sort_by_unit_buttons[0])->enable();
  ((Button *)window->sort_by_city_buttons[0])->enable();
  net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)window->other_civ]].id,
                     human_civ[0],(int)(char)window->local_units_revealed,0,0,0,0,0,0);
  parley_draw_unit_list(0);
  return;
}

/* 004693c5 auto */
void __cdecl parley_hide_city_origin_clicked(void)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((u8 *)window + 0x48))->activate();
  ((Control *)window->info_show_buttons[0])->show_and_redraw();
  ((Control *)window->info_hide_buttons[0])->hide();
  window->local_units_revealed = 0;
  ((Button *)window->sort_by_unit_buttons[0])->disable();
  ((Button *)window->sort_by_city_buttons[0])->disable();
  net_send_message(NM_HIDE_CITY_ORIGIN,net.net_slots[net.net_civ_slots[window->other_civ]].id,human_civ[0]
                     ,(int)(char)window->local_units_revealed,0,0,0,0,0,0);
  window->sort_unit_list(0,(u32)window->list_counts[0] - 1,0);
  parley_draw_unit_list(0);
  return;
}

/* 004694b7 auto */
void __cdecl parley_request_city_origin_clicked(void)

{
  iptr child;
  ParleyWindow *window;
  const char *civ_plural;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((u8 *)window + 0x48))->activate();
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(100,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
                    human_civ[0],0);
  return;
}

/* 00469547 auto */
void __cdecl parley_sort_by_city_left_clicked(void)

{
  parley_sort_by_city_clicked(0);
  return;
}

/* 00469561 auto */
void __cdecl parley_sort_by_city_right_clicked(void)

{
  parley_sort_by_city_clicked(1);
  return;
}

/* 0046957b auto */
void __cdecl parley_sort_by_city_clicked(iptr pane)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((int)window + 0x48))->activate();
  window->sort_units_by_city(0,window->list_counts[pane] - 1,pane);
  parley_draw_unit_list(pane);
  return;
}

/* 004695e9 auto */
void __cdecl parley_sort_by_unit_left_clicked(void)

{
  parley_sort_by_unit_clicked(0);
  return;
}

/* 00469603 auto */
void __cdecl parley_sort_by_unit_right_clicked(void)

{
  parley_sort_by_unit_clicked(1);
  return;
}

/* 0046961d auto */
void __cdecl parley_sort_by_unit_clicked(iptr pane)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((u8 *)window + 0x48))->activate();
  window->sort_unit_list(0,(u32)window->list_counts[pane] - 1,pane);
  parley_draw_unit_list(pane);
  return;
}

/* 0046968b auto */
void ParleyWindow::sort_unit_list(int first,int last,iptr pane)

{
  i32 swap;
  iptr child;
  int cmp;
  int j;
  int i;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (1 < window->list_counts[pane]) {
    prefs_main_menu_choice_block.parley_units_by_city = 0;
    for (i = first; j = i, i < last; i++) {
      while (j = j + 1, j <= last) {
        cmp = _strcmp(*(char **)(((i8 *)&prefs_main_menu_choice_block.unit_type_name_ptrs) +
                                  (uint)units[(int)window->list_items[i + pane * 0x801]].type * 4),
                        *(char **)(((i8 *)&prefs_main_menu_choice_block.unit_type_name_ptrs) +
                                  (uint)units[(int)window->list_items[j + pane * 0x801]].type * 4));
        if (0 < cmp) {
          swap = (i32)window->list_items[i + pane * 0x801];
          window->list_items[i + pane * 0x801] =
               (i32)window->list_items[j + pane * 0x801];
          window->list_items[j + pane * 0x801] = swap;
          swap = (i32)window->list_items_b[i + pane * 0x801];
          window->list_items_b[i + pane * 0x801] =
               (i32)window->list_items_b[j + pane * 0x801];
          window->list_items_b[j + pane * 0x801] = swap;
          swap = (i32)window->list_selected[i + pane * 0x801];
          window->list_selected[i + pane * 0x801] =
               (i32)window->list_selected[j + pane * 0x801];
          window->list_selected[j + pane * 0x801] = swap;
        }
      }
    }
  }
  return;
}

/* 0046990a auto */
void ParleyWindow::sort_units_by_city(int first,int last,iptr pane)

{
  i32 swap;
  iptr child;
  char *_Str2;
  char *_Str1;
  int cmp;
  int j;
  int i;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (window->list_counts[pane] < 2) {
    return;
  }
  prefs_main_menu_choice_block.parley_units_by_city = 1;
  for (i = first; j = i, i < last; i++) {
    for (;;) {
      j++;
      if (last < j) break;
      if (units[(int)window->list_items[i + pane * 0x801]].home_city != -1) {
        if (units[(int)window->list_items[j + pane * 0x801]].home_city != -1) {
          _Str2 = (char *)unit_home_city_name((iptr)window->list_items[j + pane * 0x801]);
          _Str1 = (char *)unit_home_city_name((iptr)window->list_items[i + pane * 0x801]);
          cmp = _strcmp(_Str1,_Str2);
          if (cmp < 1) continue;
        }
        swap = (i32)window->list_items[i + pane * 0x801];
        window->list_items[i + pane * 0x801] =
             (i32)window->list_items[j + pane * 0x801];
        window->list_items[j + pane * 0x801] = swap;
        swap = (i32)window->list_items_b[i + pane * 0x801];
        window->list_items_b[i + pane * 0x801] =
             (i32)window->list_items_b[j + pane * 0x801];
        window->list_items_b[j + pane * 0x801] = swap;
        swap = (i32)window->list_selected[i + pane * 0x801];
        window->list_selected[i + pane * 0x801] =
             (i32)window->list_selected[j + pane * 0x801];
        window->list_selected[j + pane * 0x801] = swap;
      }
    }
  }
  return;
}

/* 00469bdc auto */
void ParleyWindow::build_unit_list(int pane,int keep_selection)

{
  int *count_ptr;
  short x2;
  short y2;
  bool near_land;
  iptr child;
  iptr x;
  int y;
  int in_bounds;
  iptr unit;
  int old_count;
  int k;
  void *selected_ids;
  iptr dir;
  ParleyWindow *window;
  
  selected_ids = (void *)0x0;
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (keep_selection != 0) {
    selected_ids = operator_new(window->list_counts[pane] << 2);
    if (selected_ids == (void *)0x0) {
      keep_selection = 0;
    }
    else {
      old_count = window->list_counts[pane];
      for (unit = 0; unit < old_count; unit++) {
        if (window->list_selected[pane * 0x801 + unit] == 0) {
          *(i32 *)((int)selected_ids + unit * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)selected_ids + unit * 4) =
               (i32)window->list_items_b[pane * 0x801 + unit];
        }
      }
    }
  }
  window->list_counts[pane] = 0;
  unit = 0;
  do {
    if (game.num_units <= unit) {
      if ((keep_selection != 0) && (selected_ids != (void *)0x0)) {
        operator_delete(selected_ids);
      }
      if (prefs_main_menu_choice_block.parley_units_by_city == 0) {
        sort_unit_list(0,window->list_counts[pane] - 1,pane);
      }
      else {
        sort_units_by_city(0,window->list_counts[pane] - 1,pane);
      }
      return;
    }
    if ((units[unit].id != 0) &&
       (((prefs_main_menu_choice_block.parley_list_own_side[pane] != 0 && (units[unit].owner == (*(i8 *)&human_civ))) ||
        (((prefs_main_menu_choice_block.parley_list_own_side[pane] == 0 &&
          ((int)units[unit].owner == other_civ)) &&
         (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0 ||
          ((int)units[unit].owner == ((*(i8 *)&human_civ) & 0xffU))))))))) {
      x2 = units[unit].x;
      y2 = units[unit].y;
      near_land = false;
      for (dir = 0; dir < 8; dir++) {
        x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
        y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
        in_bounds = map_in_bounds(x,y);
        if ((in_bounds != 0) && (y = tile_is_ocean(x,y), y == 0)) {
          near_land = true;
          break;
        }
      }
      if ((near_land) || (y = unit_stack_sum(unit,2), y == 1)) {
        window->list_items[pane * 0x801 + window->list_counts[pane]] =
             unit;
        window->list_items_b[pane * 0x801 + window->list_counts[pane]] =
             units[unit].id;
        window->list_selected[pane * 0x801 + window->list_counts[pane]] = 0;
        if ((keep_selection == 1) && (selected_ids != (void *)0x0)) {
          for (k = 0; k < old_count; k++) {
            if (units[unit].id == *(int *)((int)selected_ids + k * 4)) {
              window->list_selected[pane * 0x801 + window->list_counts[pane]] =
                   1;
              break;
            }
          }
        }
        count_ptr = (int *)((iptr)window + 1000 + pane * 4);
        *count_ptr = *count_ptr + 1;
      }
    }
    unit++;
  } while( true );
}

/* 0046af70 auto */
i8 NetSession::send_to(int player,NetMsg *msg)

{
  i8 ok;
  int sent;
  
  if ((((player < 0) || (7 < player)) && (player != 0xff)) && (player != 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_sock_0_sock_max_net_players_sock_broadca,(iptr)&str_d_ss_franklinton_netmgr_send_cpp,0x139);
  }
  if (msg->size == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pmsg_size_0,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_2,0x13a);
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

/* 0046b14d auto */
/* WARNING: Removing unreachable block (ram,0x0046cdce) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl
net_send_message(int msg_type,i32 dest,iptr arg1,iptr arg2,iptr arg3,iptr arg4,
            iptr arg5,i32 arg6,const char *text,iptr raw_msg)

{
  char sent;
  size_t text_len;
  NetMsgHeader msg_5b[2];
  NetMsgHeader msg_2b[2];
  NetMsgHeader msg_9c[2];
  NetMsgHeader msg_58[2];
  NetMsgHeader msg_8d[2];
  NetMsgHeader msg_8b[2];
  NetMsgHeader msg_26[2];
  NetMsgHeader msg_46[2];
  NetMsgHeader msg_44[2];
  NetMsgHeader msg_42[2];
  NetMsgHeader msg_40[2];
  NetMsgHeader msg_8a[2];
  NetMsgHeader msg_83[2];
  NetMsgHeader msg_35[2];
  NetMsgHeader msg_79[2];
  NetMsgHeader msg_78[2];
  NetMsgHeader msg_74[2];
  NetWhoseTurnMsg whose_turn_msg[1];
  RECT caption_rect;
  NetMsgHeader msg_16[2];
  NetMsgHeader msg_5d[2];
  NetMsgHeader msg_14[2];
  NetServerAnswerMsg answer_msg[1];
  NetMsgHeader msg_12[2];
  NetMsgHeader msg_6f[2];
  NetMsgHeader msg_6e[2];
  NetMsgHeader msg_6d[2];
  NetMsgHeader simple_msg[2];
  uint image_result;
  void *image_buf;
  NetPlayerNodeMsg player_node_msg[1];
  NetPlayerListMsg player_list_msg[1];
  /* (the original's locals lay so: the message whole) */
  struct { NetMsgHeader hdr[2]; i16 kind; i8 name[260]; i8 subdir[262]; } server_path_msg[1];
  NetMsgHeader msg_05[2];
  NetServerInfoMsg server_info_msg[1];
  NetMsgHeader announce_msg[15];
  void *packet;
  int result;
  
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
  default:
    return 0;
  case NM_PLAYER_LIST:
    ((NetPlayerListMsg *)player_list_msg)->init();
    crt_memcpy(&player_list_msg->player_count,((iptr *)&net.net_player_count),0x270);
    sent = net.send_to(dest,(NetMsg *)player_list_msg);
    return (int)sent;
  case NM_SERVER_REJECT:
    msg_05->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_05);
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
      image_result = diffeng_build_full_image((iptr)&image_buf);
    }
    else if (msg_type == NM_DIFF_GAMESTATE) {
      image_result = diffeng_build_delta_image((iptr)&image_buf);
    }
    else if (msg_type == NM_JOIN_GAMESTATE) {
      image_result = diffeng_build_civ_image((iptr)&image_buf);
    }
    else if (msg_type == NM_MAP_STRUCT) {
      image_result = diffeng_build_map_header_image((iptr)&image_buf);
    }
    image_result = (uint)(image_result != -1);
    if (image_buf == (void *)0x0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_address_null,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_3,0x22c);
    }
    image_buf = net_msg_build_packet(msg_type,(iptr)image_buf,image_result,0,0,0,0,0,0);
    sent = net.send_to(dest,(NetMsg *)image_buf);
    image_result = (uint)sent;
    operator_delete(image_buf);
    return image_result;
  case NM_SERVER_CANCELED_START:
  case NM_LOST_SERVER_ON_JOIN:
  case NM_CLIENT_QUIT:
  case NM_SERVER_QUIT_REQUEST:
  case NM_SERVER_QUIT:
  case NM_SERVER_QUIT_ACK:
  case NM_SERVER_FULL:
  case NM_CLIENT_CANCELED_START:
    simple_msg->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)simple_msg);
    return (int)sent;
  case NM_SERVER_QUERY:
    msg_12->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_12);
    return (int)sent;
  case NM_SERVER_ANSWER:
    _memset(answer_msg,0,sizeof answer_msg);   /* (BF-286) no stack bytes in the gaps */
    ((NetServerAnswerMsg *)answer_msg)->init();
    sent = net.send_to(dest,(NetMsg *)answer_msg);
    return (int)sent;
  case NM_CIVILIZATION_PING:
    msg_14->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_14);
    return (int)sent;
  case NM_DIFFERENCE_ENGINE:
  case NM_ALPHA_DIFFERENCE_ENGINE:
    sent = net.send_to(dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case NM_START_CIVILIZATION:
    msg_16->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_16);
    return (int)sent;
  case NM_COSMIC:
    packet = net_msg_build_packet(msg_type,(iptr)&cosmic,0,0,0,0,0,0,0x29);
    break;
  case NM_CIVILIZE:
    packet = net_msg_build_packet(msg_type,(iptr)&advances,0,0,0,0,0,0,0x640);
    break;
  case NM_IMPROVE:
    packet = net_msg_build_packet(msg_type,(iptr)&improvements,0,0,0,0,0,0,0x218);
    break;
  case NM_ENDWONDER:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)wonder_expires),0,0,0,0,0,0,0x1c);
    break;
  case NM_UNITS:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)unit_types),0,0,0,0,0,0,0x4d8);
    break;
  case NM_TERRAIN:
    packet = net_msg_build_packet(msg_type,(iptr)&terrain_types,0,0,0,0,0,0,0x318);
    break;
  case NM_GOVT:
    packet = net_msg_build_packet(msg_type,(iptr)&government_names,0,0,0,0,0,0,0x1c);
    break;
  case NM_GOVT_TITLE:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)government_titles),0,0,0,0,0,0,0x38);
    break;
  case NM_LEADERS:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)leaders),0,0,0,0,0,0,0x3f0);
    break;
  case NM_KINGS:
    packet = net_msg_build_packet(msg_type,(iptr)&civs,0,0,0,0,0,0,0x7524);
    break;
  case NM_CARAVAN:
    packet = net_msg_build_packet(msg_type,(iptr)&commodity_names,0,0,0,0,0,0,0x40);
    break;
  case NM_ORDERS:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)order_names),0,0,0,0,0,0,0x68);
    break;
  case NM_DIFFICULTY:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)difficulty_names),0,0,0,0,0,0,0x18);
    break;
  case NM_ATTITUDES:
    packet = net_msg_build_packet(msg_type,(iptr)&attitude_names,0,0,0,0,0,0,0x24);
    break;
  case NM_EVENTSLOADED:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_NATION_ACCEPT:
  case NM_NATION_REJECT:
    msg_26->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_26);
    return (int)sent;
  case NM_GAME_GOING:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_LOCK_DATA:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    net.nm_lock_data_count++;
    break;
  case NM_HEART_BEAT:
  case NM_HEART_BEAT_ACK:
    msg_2b->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_2b);
    return (int)sent;
  case NM_QUIT_REQUEST:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
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
      net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
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
        net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
        net.nm_lock_data_count++;
      }
      return 1;
    }
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_MULTIOPTIONS_CHANGE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_KILL_OR_QUIT:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_KILL_OR_QUIT_ACK:
  case NM_PARLEYREQUEST_CANCEL:
    msg_35->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_35);
    return (int)sent;
  case NM_DELETE_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_UNIT_DELETED:
    packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,(int)game.num_units
                                         ,arg3,arg4,arg5,arg6,0x20);
    break;
  case NM_DELETE_CITY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_CITY_DELETED:
    packet = net_msg_build_packet(msg_type,(iptr)(cities + arg1),arg1,
                                         (int)game.num_cities,arg3,arg4,arg5,arg6,0x58);
    break;
  case NM_CREATE_CITY:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_CITY_CREATED:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(iptr)(cities + arg1),arg1,
                                           (int)game.num_cities,arg3,arg4,arg5,arg6,0x58
                                          );
    }
    break;
  case NM_CREATE_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_UNIT_CREATED:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,
                                           (int)game.num_units,arg3,arg4,arg5,arg6,0x20)
      ;
    }
    break;
  case NM_PICK_UP_UNIT:
  case NM_STACK_UNIT:
  case NM_DELETE_SAFELY:
  case NM_DELETE_VISIBLY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_UNIT_PICKED_UP:
  case NM_UNIT_STACKED:
  case NM_SAFELY_DELETED:
  case NM_VISIBLY_DELETED:
    msg_40->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_40);
    return (int)sent;
  case NM_PUT_DOWN_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_UNIT_PUT_DOWN:
    msg_42->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_42);
    return (int)sent;
  case NM_MOVE_TO_BOTTOM:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_BOTTOMED:
    msg_44->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_44);
    return (int)sent;
  case NM_RELOCATE_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_UNIT_RELOCATED:
    msg_46->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_46);
    return (int)sent;
  case NM_ACQUIRE_WONDER:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_WONDER_ACQUIRED:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_STACK_SHIP:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_SHIP_STACKED:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_LOCK_MAP:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_UNLOCK_MAP:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_LOCK_MAP_ACK:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_UNLOCK_MAP_ACK:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_LOCK_MAP_SYNC:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_NEWTURNTIMER_REPLY:
  case NM_PMCHANGE_REPLY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_DO_APOLLO:
    msg_58->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_58);
    return (int)sent;
  case NM_MAP_PACKETS:
    packet = net_msg_build_packet(msg_type,(iptr)&net_batch_buffer,0,0,0,0,0,0,net_batch.used_dwords << 2);
    break;
  case NM_REALTIME_STACKER:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,arg6,0
                                        );
    break;
  case NM_REALTIME_STACKER_ACKER:
  case NM_HOLD_FOR_JOIN:
  case NM_CONTINUE_AFTER_JOIN:
  case NM_ABORT_KABOOM:
    msg_5b->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_5b);
    return (int)sent;
  case NM_DRAG_HOLD_ON:
  case NM_DRAG_HOLD_OFF:
    msg_5d->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_5d);
    return (int)sent;
  case NM_NEW_CIV:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_KILL_CIV:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_NEW_CIV_ACK:
  case NM_KILL_CIV_ACK:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_BRIBED_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_DEATH_STACK:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_CHAT:
    text_len = _strlen((char *)text);
    packet = net_msg_build_packet(msg_type,(iptr)text,arg1,arg2,arg3,0,0,0,text_len + 1);
    break;
  case NM_START_WHOSE_TURN:
    ((NetWhoseTurnMsg *)whose_turn_msg)->init(arg1);
    map_views_block.main_view_timer_cache_i32 = 0xffffffff;
    ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
    caption_rect.left = map_views_block.main_view_frame_left;
    caption_rect.top = map_views_block.main_view_frame_top;
    caption_rect.right = map_views_block.main_view_frame_right;
    caption_rect.bottom = map_views_block.main_view_client_top;
    ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&caption_rect);
    sent = net.send_to(dest,(NetMsg *)whose_turn_msg);
    return (int)sent;
  case NM_POPUP:
    packet = net_msg_build_packet(msg_type,raw_msg,arg1,arg2,arg3,arg4,arg5,0,
                                         0);
    break;
  case NM_END_SEQUENCE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_TIMER_CHANGE:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_START_TURN_TIMER:
    msg_6d->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_6d);
    return (int)sent;
  case NM_STOP_TURN_TIMER:
    msg_6e->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_6e);
    return (int)sent;
  case NM_RESTART_TURN_TIMER:
    msg_6f->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_6f);
    return (int)sent;
  case NM_SLIDE_PIECE:
    packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,arg2,arg3,
                                         arg4,arg5,arg6,0x20);
    break;
  case NM_POSSIBLY_CENTER:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_DRAW_SQUARE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_DRAW_TWO:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_DRAW_MAP:
    msg_74->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_74);
    return (int)sent;
  case NM_DRAW_AREA:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_DRAW_MAP_REGION:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_CANESCAPE_REPLY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_INFO_STATUS:
    msg_78->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_78);
    return (int)sent;
  case NM_INFO_UPDATE:
    msg_79->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_79);
    return (int)sent;
  case NM_SOUND_PLAY:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_DESERT_REPLY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_KABOOM:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_SHOW_NUKE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_ASK_ALLY:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_REPORTS_UPDATE:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_PARLEYREQUEST_REPLY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_PARLEY_TRANSACTION:
    sent = net.send_to(dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case NM_PARLEY_ACCEPT_OFFER:
  case NM_PARLEY_NO_THANKS:
  case NM_PARLEY_COUNTER_OFFER:
    msg_83->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_83);
    return (int)sent;
  case NM_PARLEY_EXECUTE:
    sent = net.send_to(dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case NM_CITYWIN_CHECK_UNIT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_CITYWIN_CHECK_CITY:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_CITYWIN_DELETED:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_CITYWIN_REDRAW:
    msg_8a->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_8a);
    return (int)sent;
  case NM_PRODUCTION_BEGIN:
    msg_8b->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_8b);
    return (int)sent;
  case NM_PRODUCTION_COMPLETE:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_AI_MOVEMENT:
  case NM_HUMAN_MOVEMENT:
    msg_8d->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_8d);
    return (int)sent;
  case NM_HUMAN_MOVE_COMPLETE:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_FEATURE_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_TERRAIN_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_SEEN_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case NM_SITE_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_OWNER_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_REGION_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_CITY_USING_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_FEAT_LOC_SET:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,arg6,0
                                        );
    break;
  case NM_SET_ATTITUDE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case NM_DO_AIPARLEY:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_SET_SLIDE_SOUND:
  case NM_POLITICAL_VIOLATIONS:
  case NM_POLITICAL_ATTITUDE:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_SET_CIV:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case NM_READY_TO_START:
  case NM_KINGDOM_RATES:
  case NM_TAX_CHECK:
    msg_9c->header_init(msg_type);
    sent = net.send_to(dest,(NetMsg *)msg_9c);
    return (int)sent;
  case NM_PICK_GOVT:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_TURN_FINISHED:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case NM_PRETEXT:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_REVEAL_CITY_INFO:
  case NM_HIDE_CITY_INFO:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case NM_REVEAL_CITY_ORIGIN:
  case NM_HIDE_CITY_ORIGIN:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
  }
  if (packet == (void *)0x0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_address,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_4,0x523);
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
int __cdecl net_stacked_draw_push(int kind,iptr args,iptr unit_data)

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
    net_stacked_draw_push(0xa3,(iptr)dummy_args,0);
    result = net_stacked_draw_push(0x74,(iptr)dummy_args,0);
  }
  else {
    if ((net.stacked_draw_first < 0) || (99 < net.stacked_draw_first)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_firststackeddraw_0_gnetmgr_first,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp,0x5a);
    }
    if ((net.stacked_draw_last < 0) || (99 < net.stacked_draw_last)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_laststackeddraw_0_gnetmgr_lastst,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_2,0x5b);
    }
    net.stacked_draw[net.stacked_draw_last].kind = kind;
    for (i = 0; i < 7; i++) {
      *(i32 *)(((i8 *)&net.stacked_draw_args) + i * 4 + net.stacked_draw_last * 0x40) =
           *(i32 *)(args + i * 4);
    }
    if (kind == 0x70) {
      crt_memcpy((void *)(net.stacked_draw_last * 0x40 + (iptr)(((u8 *)&net.stacked_draw_arg4 + 12))),(const void *)unit_data,0x20);
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
      mapviews_ensure_visible(x,y,(int)power_graph_history_block.stacked_draw_unit_owner);
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
  Dialog join_dialog;
  int wait_start;
  
  if (net.host_flag == '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_bserver,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_4,0x22a);
  }
  if (net.host_handoff_stage == '\0') {
    city = net_host_register_player_civ((NetPlayerInfoMsg *)msg_hdr,sender);
    if (city == 0) {
      net_send_message(NM_LOST_SERVER_ON_JOIN,sender,0,0,0,0,0,0,0,0);
    }
    else {
      net.nm_lock_nation_count++;
      if (2 < (int)(*(uptr *)&net.session_phase)) {
        join_dialog.Dialog::construct(0x4000);
        ON_SCOPE_EXIT(join_dialog.Dialog::destruct());
        net_send_message(NM_HOLD_FOR_JOIN,0xff,0,0,0,0,0,0,0,0);
        mp_turn_timer_stop(0);
        join_dialog.load_game_section((const char *)&str_waitongamexmit_2,0x2000000);
        join_dialog.show_modal(0);
        net_send_message(NM_MULTIOPTIONS_CHANGE,sender,(int)mp_doubles.production,(int)mp_doubles.moves,
                           (int)mp_options.simultaneous_setting,0,0,0,0,0);
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
    net_send_message(NM_NATION_ACCEPT,sender,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
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
  city = city_find_nearest((int)units[a.unit].x,(int)units[a.unit].y,-1,0xffffffff,-1)
  ;
  if ((-1 < city) && ((int)cities[city].owner == civ2)) {
    home_city_id = (i8)city;
    units[unit5].home_city = home_city_id;
  }
  unit_set_seen_by(unit5,(int)civ);
  tile_set_owner((int)units[unit5].x,(int)units[unit5].y,civ2,1);
  net_send_state_diff(0xff,2,0,0,1);
}

/* Part of NetSession::process_messages (0047e94e): the
   wait-while-the-game-is-sent dialog (0x65), modal until the host is done. */
void NetSession::recv_wait_game_xmit()
{
  Dialog wait_dialog;
  u32 saved_busy;
  
  wait_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(wait_dialog.Dialog::destruct());
  saved_busy = (u32)popup_in_progress;
  popup_in_progress = 1;
  mp_turn_timer_stop(0);
  net.nm_continue_after_join_count = 0;
  dlg_style.pending_idle_callback = net_game_xmit_wait_idle;
  popup_game_sprite_flags((const char *)&str_waitongamexmit,0x2000000,(iptr)&popup_info_icon_sprite,0);
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
  net_stacked_draw_push(0xa3,(iptr)draw_args,0);
  net_stacked_draw_push(0x74,(iptr)draw_args,0);
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
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&caption_rect);
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
    ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&timer_rect);
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
    city = popup_game_with_sprite((const char *)&str_pretext,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
    if (city == 1) {
      diplo_set_treaty_flags(a.unit,a.civ3,0x2000);
      civs[a.civ3].last_contact_turn[a.unit] = game.turn;
    }
  }
  else {
    city = popup_game_with_sprite((const char *)&str_pretextallied,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
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
    if (*(short *)((u32)blocked_input_window + 0x2dc) != 0) {
      if (blocked_input_window == 0) {
        click_wnd = 0;
      }
      else {
        click_wnd = (u32)blocked_input_window + 0x48;
      }
      wnd_set_active_child(click_wnd);
      map_window_click(blocked_input_arg1,(u32)blocked_input_arg2,
                         blocked_input_arg3);
    }
    break;
  case 2:
    if (*(short *)((u32)blocked_input_window + 0x2dc) != 0) {
      if (blocked_input_window == 0) {
        dblclick_wnd = 0;
      }
      else {
        dblclick_wnd = (u32)blocked_input_window + 0x48;
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

/* Part of NetSession::process_messages (0047e94e): one civ message, by type
   (msg[1]). msg by reference: an image message (7 to 10) moves its payload to
   the front and reallocates it, and the caller frees it. */
void NetSession::dispatch_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a)
{
  int *drop_counter;
  u32 now;
  i32 x;
  iptr ret_val;
  int city;
  int y4;
  int x3;
  iptr unit4;
  int image_kind;
  i32 header_size;
  int image_size;
  iptr result;
  
  switch(msg[1]) {
  case NM_DATAGRAM:
    recv_hello(msg,gamelist_dirty);
    break;
  default:
    dbg_log((const char *)&str_received_unknown_message_type);
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
  case NM_DIFFERENCE_ENGINE:
  case NM_ALPHA_DIFFERENCE_ENGINE:
    recv_state_diff(msg,payload,a);
    break;
  case NM_START_CIVILIZATION:
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
      net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
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
  case NM_NEWTURNTIMER_REPLY:
  case NM_PMCHANGE_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.vote_replies_pending--;
    if (a.unit == 2) {
      net.vote_status = 0xffffffff;
    }
    break;
  case NM_DO_APOLLO:
    wonder_apollo_reveal_map();
    break;
  case NM_MAP_PACKETS:
    net_apply_tile_updates((iptr)payload);
    break;
  case NM_REALTIME_STACKER:
    recv_stack_relocate(msg,sender,a);
    break;
  case NM_REALTIME_STACKER_ACKER:
    net.nm_stacker_ack_count = 1;
    break;
  case NM_DRAG_HOLD_ON:
    net.drag_hold = 1;
    dbg_log((const char *)&str_hold_on);
    break;
  case NM_DRAG_HOLD_OFF:
    net.drag_hold = 0;
    dbg_log((const char *)&str_hold_off);
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
  case NM_HOLD_FOR_JOIN:
    recv_wait_game_xmit();
    break;
  case NM_CONTINUE_AFTER_JOIN:
    break;
  case NM_CHAT:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    header_size = 0x2c;
    chat_wnd_block.chat_receive_message(a.unit,a.civ3,a.unit3,(iptr)(msg + 0xb)
              );
    break;
  case NM_START_WHOSE_TURN:
    recv_turn_civ(msg_hdr);
    break;
  case NM_POPUP:
    recv_popup_request(msg,sender,a);
    break;
  case NM_END_SEQUENCE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    game.game_flags = (u16)a.unit;
    (*(uptr *)&game_over_reason) = a.civ3;
    (*(uptr *)&turn_flags.spaceship_winner_civ) = a.unit3;
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
    net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,(iptr)payload);
    break;
  case NM_POSSIBLY_CENTER:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    break;
  case NM_DRAW_SQUARE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    }
    break;
  case NM_DRAW_TWO:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if ((city != 0) || (city = mapviews_tile_visible(a.unit3,a.y2), city != 0)) {
      net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    }
    break;
  case NM_DRAW_MAP:
    net_stacked_draw_push(msg[1],(iptr)&a.unit,0);
    break;
  case NM_DRAW_AREA:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    }
    break;
  case NM_DRAW_MAP_REGION:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    city = mapviews_tile_visible(a.unit,a.civ3);
    if (city != 0) {
      net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    }
    break;
  case NM_CANESCAPE_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&sprite_picker_sprite_block.net_can_escape_reply) = a.unit;
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
  case NM_DESERT_REPLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    font_size16_bold_block.net_bribe_answer = a.unit;
    break;
  case NM_KABOOM:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    break;
  case NM_SHOW_NUKE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
    break;
  case NM_ASK_ALLY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ally_demand_help(a.unit,a.civ3);
    break;
  case NM_REPORTS_UPDATE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    advisor_refresh_if_open(a.unit);
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
    known_set(a.unit,a.civ3,a.unit3,a.y2,a.x2,a.y3);
    break;
  case NM_SET_ATTITUDE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    *(char *)&civs[a.unit].attitude[a.civ3] = (char)a.unit3;
    break;
  case NM_DO_AIPARLEY:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net_meet_pending_flags[a.civ3] = 1;
    net_meet_pending.ai_civ[a.civ3] = a.unit;
    net_meet_pending.civ[a.civ3] = a.civ3;
    net_meet_pending.x[a.civ3] = a.unit3;
    net_meet_pending.y[a.civ3] = a.y2;
    break;
  case NM_SET_SLIDE_SOUND:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    (*(uptr *)&net.move_anim_sound) = a.unit;
    (*(uptr *)&net.move_anim_sound_arg) = a.civ3;
    break;
  case NM_SET_CIV:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    civ_give_advance(a.unit,a.civ3,a.unit3,a.y2,0);
    break;
  case NM_READY_TO_START:
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
  case NM_POLITICAL_VIOLATIONS:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ai_review_relations(a.unit,a.civ3);
    break;
  case NM_POLITICAL_ATTITUDE:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    diplo_ai_seek_war_ally(a.unit,a.civ3);
    break;
  case NM_TURN_FINISHED:
    net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
    net.civ_turn_ended[a.unit] = net.civ_turn_ended[a.unit] + 1;
    break;
  case NM_ABORT_KABOOM:
    net_stacked_draw_push(msg[1],(iptr)&a.unit,0);
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
  }
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
      civ = msg_queue.dequeue((iptr)&sender,(iptr)&msg,
                         (iptr)&msg_size,dequeue_wait);
      if (civ == '\0') break;
      if (msg == (int *)0x0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pdata_null,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_3,0x195);
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

/* 00482337 auto */
void __cdecl net_game_xmit_wait_idle(void)

{
  net.process_messages(1,0);
  if (net.nm_continue_after_join_count != 0) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004823d6 auto */
void __cdecl net_msg_get_args(NetMsg *msg,i32 *args)

{
  *args = msg->args[0];
  args[1] = msg->args[1];
  args[2] = msg->args[2];
  args[3] = msg->args[3];
  args[4] = msg->args[4];
  args[5] = msg->args[5];
  args[6] = msg->args[6];
  return;
}

/* 00482470 auto */
void __cdecl net_server_quit_wait_idle(void)

{
  int human_count;
  
  net.process_messages(1,0);
  if (((net.players_awaited != 0) &&
      (human_count = bits_count8((uint)(game.human_players == '\0')), human_count == 0)) &&
     (1 < net.net_player_count[0])) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 004824e3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_host_quit_handoff(void)

{
  byte sign;
  int human_count;
  int slot;
  
  net.host_handoff_stage = 1;
  net_drop_flagged_players();
  game.human_players = net.pending_human_players | game.human_players;
  net.pending_human_players = 0;
  if (1 < net.net_player_count[0]) {
    game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
    net.players_awaited = net.net_player_count[0] - 1;
    net_send_message(NM_SERVER_QUIT_REQUEST,0xff,0,0,0,0,0,0,0,0);
    net_drop_flagged_players();
    dlg_style.pending_idle_callback = net_server_quit_wait_idle;
    popup_game_sprite_flags((const char *)&str_serverquitwait,0x2000000,(iptr)&popup_info_icon_sprite,0);
    human_count = bits_count8((uint)(game.human_players == '\0'));
    if (human_count == 0) {
      net_send_message(NM_SERVER_QUIT,0xff,0,0,0,0,0,0,0,0);
      sign = (byte)(net.mp_turn_civ + 1 >> 0x1f);
      saved_turn.civ = (((byte)(net.mp_turn_civ + 1) ^ sign) - sign & 7 ^ sign) - sign;
      for (slot = 1; (slot < 7 && ((((i8 *)&net.slot_active_flags))[slot * 0x54] == '\0'));
          slot = slot + 1) {
      }
      if (slot < 7) {
        crt_memcpy(((i8 *)&net.player_slots),((i8 *)&net.player_slots) + slot * 0x54,0x54);
        net.slot_host_flags[0] = 1;
        net.player_slots[0] = 0;
        net.net_civ_slots[0] = net.net_civ_slots[slot];
        net.net_player_count[0]--;
        net.net_civ_slots[slot] = 0xffffffff;
        _memset(((i8 *)&net.player_slots) + slot * 0x54,0,0x54);
        net_send_state_diff(0xff,2,0,1,0);
        net_send_message(NM_START_CIVILIZATION,0xff,0,0,0,0,0,0,0,0);
        net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
    }
  }
  return;
}

/* 00482724 auto */
void __cdecl net_client_quit_wait_idle(void)

{
  net.process_messages(1,0);
  if (net.host_handoff_stage == 2) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0048276d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_start_civ_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.nm_start_civ_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 004827d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_player_list_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.nm_player_list_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 0048283d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_players_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.players_awaited != 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 004828a5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_client_transfer_server(void)

{
  char transport_ok;
  i32 result;
  int cmp;
  int slot;
  int old_slot;
  struct { int slots[2]; char addresses[76]; int civ_ids[135]; } saved;  /* a copy of the player table */
  int status;
  int slot_civ;
  
  net.nm_server_quit_count = 0;
  net.nm_start_civ_count = 0;
  net.nm_player_list_count = 0;
  net_send_message(NM_SERVER_QUIT_ACK,0,0,0,0,0,0,0,0,0);
  dlg_style.pending_idle_callback = net_client_quit_wait_idle;
  popup_game_sprite_flags((const char *)&str_clientquitwait,0x2000000,(iptr)&popup_info_icon_sprite,0);
  net.host_handoff_stage = 0;
  dlg_style.pending_idle_callback = net_quit_wait_start_civ_idle;
  map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
  popup_game_sprite_flags((const char *)&str_clientquitwait_2,0x2000000,(iptr)&popup_info_icon_sprite,0);
  if (net.nm_start_civ_count == 0) {
    dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_star);
    crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
    popup_game_with_sprite((const char *)&str_lanconnectfail_3,(iptr)&popup_info_icon_sprite,0);
    player_turn_active = 0;
    turn_loop.active = 0;
    result = 0;
  }
  else {
    turn_loop.resume_civ = saved_turn.civ;
    dlg_style.pending_idle_callback = net_quit_wait_player_list_idle;
    map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
    popup_game_sprite_flags((const char *)&str_clientquitwait_3,0x2000000,(iptr)&popup_info_icon_sprite,0);
    if (net.nm_player_list_count == 0) {
      dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_play);
      crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
      popup_game_with_sprite((const char *)&str_lanconnectfail_4,(iptr)&popup_info_icon_sprite,0);
      player_turn_active = 0;
      turn_loop.active = 0;
      result = 0;
    }
    else {
      crt_memcpy(&saved,((iptr *)&net.net_player_count),0x270);
      cmp = _strcmp(((i8 *)&menu_prefs.net_ip_address),((i8 *)&net.slot_addresses));
      if (cmp == 0) {
        dbg_log((const char *)&str_clienttransferserver_disconnecting_from);
        net.shutdown(0);
        transport_ok = net.init_transport(net.net_protocol,1);
        status = (int)transport_ok;
        if (status == 0) {
          dbg_log((const char *)&str_clienttransferserver_could_not_reconnect);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_5,(iptr)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_reconnected_to_network_as_server);
        crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&menu_prefs.net_ip_address));
        net.server_conn_id = 0;
        net.local_slot = 0;
        diffeng_scan.mirror_end = (*(int *)&diffeng_scan.mirror_size) + diffeng.mirror_buf;
        net.players_awaited = saved.slots[0] - 1;
        net.players_awaited_initial = net.players_awaited;
        if (net.players_awaited < 1) {
          slot_civ = saved.civ_ids[0];
          if ((uint)slot_civ < 8) {   /* (BF-162) a civ of the host's list */
            net.net_civ_slots[saved.civ_ids[0]] = 0;
          }
          net.slot_civs[0] = saved.civ_ids[0];
        }
        else {
          dlg_style.pending_idle_callback = net_quit_wait_players_idle;
          map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
          popup_game_sprite_flags((const char *)&str_clientquitwait_4,0x2000000,(iptr)&popup_info_icon_sprite,0);
          cmp = timer_ticks();
          if ((net.timeout_seconds * 0x3c < cmp - map_views_block.net_handoff_wait_ticks_i32) && (net.players_awaited != 0)) {
            crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
            if (net.players_awaited == net.players_awaited_initial) {
              popup_game_with_sprite((const char *)&str_nooneshowed,(iptr)&popup_info_icon_sprite,0);
              turn_loop.active = 0;
              player_turn_active = 0;
              return 0;
            }
            popup_game_with_sprite((const char *)&str_someshowed,(iptr)&popup_info_icon_sprite,0);
          }
          for (slot = 0; slot < 7; slot++) {
            if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
              old_slot = 0;
              while ((old_slot < 7 &&
                     (cmp = _strcmp(saved.addresses + old_slot * 0x54,
                                      ((i8 *)&net.slot_addresses) + slot * 0x54), cmp != 0))) {
                old_slot++;
              }
              if (6 < old_slot) {
                ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_oldindex_max_net_players,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_5,0x8a7);
                /* (BF-162) the original stopped at its assertion: a player
                   not in the old list has no civ (civ_ids[7 * 0x15] is past
                   the copy); the transfer fails as when no one came */
                turn_loop.active = 0;
                player_turn_active = 0;
                return 0;
              }
              slot_civ = saved.civ_ids[old_slot * 0x15];
              if ((uint)slot_civ < 8) {   /* a civ of the host's list */
                net.net_civ_slots[slot_civ] = slot;
              }
              (((i32 *)&net.slot_civs))[slot * 0x15] = slot_civ;
            }
          }
          net.mp_simultaneous_moves = *(i8 *)&mp_options.simultaneous_setting;
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,(int)mp_doubles.production,(int)mp_doubles.moves,(int)(*(i8 *)&mp_options.simultaneous_setting),0,0,0,0
                             ,0);
          net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(60000);
        }
        _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots),-1,0xc0);
        net.session_phase = 3;
        net_reset_error_counts();
        net.nm_server_quit_count = 0;
        net.nm_start_civ_count = 0;
        net.nm_player_list_count = 0;
        if (game_over_reason != 2) {
          popup_game_with_sprite((const char *)&str_hostchangeserver,(iptr)&popup_info_icon_sprite,0);
        }
      }
      else {
        dbg_logf_str((const char *)&str_disconnecting_from_server_s,(const char *)&net.net_server_address);
        status = XD_CloseConnection();
        if (status != 0) {
          dbg_logf_str_int((const char *)&str_clienttransferserver_disconnection_from,(const char *)&net.net_server_address,status);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_6,(iptr)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&net.slot_addresses));
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        do {
          cmp = timer_ticks();
        } while (cmp - map_views_block.net_handoff_wait_ticks_i32 < 0x3c);
        dbg_logf_str((const char *)&str_attempting_to_reconnect_to_new_server_s,(const char *)&net.net_server_address);
        status = XD_OpenConnection(((i8 *)&net.net_server_address),net.timeout_seconds * 1000);
        if (status < 0) {
          dbg_logf((const char *)&str_connecttogame_open_connection_failed_err,status);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite((const char *)&str_serverconnectfail_2,(iptr)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_connection_to_new_server_successful);
        net.nm_server_quit_count = 0;
        net.nm_start_civ_count = 0;
        dlg_style.pending_idle_callback = net_quit_wait_player_list_idle;
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        popup_game_sprite_flags((const char *)&str_clientquitwait_5,0x2000000,(iptr)&popup_info_icon_sprite,0);
        if (net.nm_player_list_count == 0) {
          dbg_log((const char *)&str_clienttransferserver_client_timed_out_wa);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_7,(iptr)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.slot_player_names));
        net_reset_error_counts();
        net.nm_player_list_count = 0;
        popup_game_with_sprite((const char *)&str_hostchangeclient,(iptr)&popup_info_icon_sprite,0);
      }
      result = 1;
    }
  }
  return result;
}

/* 0048bf51 auto */
void __cdecl net_client_hot_wait_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_start_civ_count != 0) || (net.host_handoff_stage != '\0')) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit != '\0' || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0048d9ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_send(iptr civ)

{
  if (net.drag_hold == '\0') {
    net.nm_heartbeat_ack_count = 0;
    net_send_message(NM_HEART_BEAT,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,0,
                       0,0,0,0);
  }
  else {
    net.nm_heartbeat_ack_count = 1;
  }
  if (((int)parley_wnd_block.closed == 1) && (net_popups.head == 0)) {
    diplo_meeting.mp_pending = 0;
    net.nm_parley_cancel_count = 0;
  }
  map_views_block.net_heartbeat_last_tick_i32 = timer_ticks();
  return;
}

/* 0048da51 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_check(iptr civ)

{
  int now;
  
  now = timer_ticks();
  if (0xe0f < now - map_views_block.net_heartbeat_last_tick_i32) {
    if (net.nm_heartbeat_ack_count == 0) {
      net.civ_turn_ended[civ] = net.civ_turn_ended[civ] + 1;
      if (net.host_flag == '\0') {
        net.lost_conn_total++;
      }
    }
    else {
      net_heartbeat_send(civ);
    }
  }
  return;
}

/* 0048dab9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_check_lost_connections(void)

{
  int slot_civ;
  bool no_player;
  int civ;
  int dropped_count;
  int lost_counts[7];
  iptr slot;
  
  if (net.host_flag == '\0') {
    if (net.lost_conn_total != 0) {
      turn_loop.active = 0;
      dbg_log((const char *)&str_we_lost_connection_to_the_server_heartbe);
      net.shutdown(1);
      popup_game_with_sprite((const char *)&str_lostserver,(iptr)&popup_info_icon_sprite,0);
      return 1;
    }
  }
  else {
    dropped_count = net_drop_flagged_players();
    for (civ = 1; civ < 8; civ++) {
      no_player = true;
      if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        for (slot = 0; slot < 7; slot++) {
          if (((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') &&
             ((((i32 *)&net.slot_civs))[slot * 0x15] == civ)) {
            no_player = false;
            break;
          }
        }
        if (no_player) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net.nm_lock_nation_count++;
          }
          dropped_count++;
        }
      }
    }
    if (net.lost_conn_total != 0) {
      crt_memcpy(lost_counts,((i8 *)&net.lost_conn_by_player),0x1c);
      net_reset_error_counts();
      for (slot = 1; slot < 7; slot++) {
        if ((0 < lost_counts[slot]) && ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0')) {
          dropped_count++;
          slot_civ = (((i32 *)&net.slot_civs))[slot * 0x15];
          text_set_string_arg(0,(const char *)(((i8 *)&net.slot_player_names) + slot * 0x54));
          net_send_text_args(0x53,0xff,1,0,0,0);
          net.civ_drop_requested[slot_civ] = 0;
          (*((char *)&civ + 0)) = (byte)slot_civ;
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net.remove_player(net.net_slots[slot].id);
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net.nm_lock_nation_count++;
          }
          popup_game_with_sprite((const char *)&str_lostclient,(iptr)&popup_info_icon_sprite,0);
        }
      }
    }
    if (dropped_count != 0) {
      net.net_player_count[0] = 0;
      for (slot = 0; slot < 7; slot++) {
        if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
          net.net_player_count[0]++;
        }
      }
      net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
      net_send_state_diff(0xff,2,0,0,1);
    }
  }
  return 0;
}

/* 0048de75 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl net_drop_flagged_players(void)

{
  int slot_civ;
  byte civ_bit;
  int dropped_count;
  int slot;
  
  dropped_count = 0;
  for (slot = 1; slot < 7; slot++) {
    if (((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') &&
       (net.civ_drop_requested[(((i32 *)&net.slot_civs))[slot * 0x15]] != 0)) {
      dropped_count++;
      slot_civ = (((i32 *)&net.slot_civs))[slot * 0x15];
      net.civ_drop_requested[slot_civ] = 0;
      net.lost_conn_by_player[net.net_slots[slot].id] = 0;
      civ_bit = (byte)slot_civ;
      game.human_players = game.human_players & ~(byte)(1 << (civ_bit & 0x1f));
      net.remove_player(net.net_slots[slot].id);
      if ((1 << (civ_bit & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
        (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << (civ_bit & 0x1f));
        net.nm_lock_nation_count++;
      }
    }
  }
  if (dropped_count != 0) {
    net_send_message(NM_LOCK_DATA,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
  }
  return dropped_count;
}

/* 004923f0 auto */
void __cdecl chat_insert_macro_text(EditControl *edit,i32 macro_id)

{
  bool all_contact;
  int civ;
  iptr name2;
  size_t text_len;
  int tries;
  
  text_clear();
  tries = 0;
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (civ = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), civ == 0))
     && (game.reveal_map == '\0')) {
    all_contact = false;
  }
  else {
    all_contact = true;
  }
  switch(macro_id) {
  case 0x2b0:
    while (((((civ = chat_macros.next_adj_civ, tries < 8 && (human_civ[0] != chat_macros.next_adj_civ)) && (!all_contact))
            && ((mp_options.chat_from_start == 0 ||
                ((1 << ((byte)chat_macros.next_adj_civ & 0x1f) & (uint)game.human_players) == 0)))) &&
           ((*(byte *)&civs[chat_macros.next_adj_civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0))) {
      tries++;
      chat_macros.next_adj_civ++;
      if (chat_macros.next_adj_civ == 8) {
        chat_macros.next_adj_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    chat_macros.next_adj_civ++;
    name2 = civ_adjective(civ);
    text_append((const char *)name2);
    text_append_space();
    if (chat_macros.next_adj_civ == 8) {
      chat_macros.next_adj_civ = 1;
    }
    break;
  case 0x2b1:
    while ((((tries < 8 && (human_civ[0] != chat_macros.next_leader_civ)) &&
            ((!all_contact &&
             ((mp_options.chat_from_start == 0 ||
              ((1 << ((byte)chat_macros.next_leader_civ & 0x1f) & (uint)game.human_players) == 0)))))) &&
           ((*(byte *)&civs[chat_macros.next_leader_civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0))) {
      tries++;
      chat_macros.next_leader_civ++;
      if (chat_macros.next_leader_civ == 8) {
        chat_macros.next_leader_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    name2 = civ_leader_title(chat_macros.next_leader_civ);
    text_append((const char *)name2);
    text_append_space();
    civ = chat_macros.next_leader_civ;
    chat_macros.next_leader_civ++;
    name2 = (iptr)(civ_leader_name(civ));
    text_append((const char *)name2);
    text_append_space();
    if (chat_macros.next_leader_civ == 8) {
      chat_macros.next_leader_civ = 1;
    }
    break;
  case 0x2b2:
    chat_read_macro_line((const char *)&str_chatmac1_txt,(iptr)&chat_macros.macro1_offset);
    civ = chat_macros.macro1_offset;
    if (civ == 0) {
      return;
    }
    break;
  case 0x2b3:
    chat_read_macro_line((const char *)&str_chatmac2_txt,(iptr)&chat_macros.macro2_offset);
    civ = chat_macros.macro2_offset;
    if (civ == 0) {
      return;
    }
    break;
  case 0x2b4:
    chat_read_macro_line((const char *)&str_chatmac3_txt,(iptr)&chat_macros.macro3_offset);
    civ = chat_macros.macro3_offset;
    if (civ == 0) {
      return;
    }
    break;
  }
  edit->replace_sel((const char *)&textbuf.buf);
  civ = edit->get_sel_end();
  text_len = _strlen(((i8 *)&textbuf.buf));
  edit->set_sel(civ - text_len,civ);
  return;
}

/* 0049275a auto */
void __cdecl chat_read_macro_line(const char *path,iptr file_offset)

{
  int ok;
  size_t line_len;
  i8 file[160];
  
  ((FileObject *)file)->init();
  ON_SCOPE_EXIT(((FileObject *)&file)->close());
  ok = ((FileObject *)file)->open(path);
  if (ok == 0) {
    *(i32 *)file_offset = 0;
  }
  else {
    ok = ((FileObject *)file)->get_size();
    if (ok <= *(int *)file_offset) {
      *(i32 *)file_offset = 0;
    }
    ok = ((FileObject *)file)->seek_cur(*(i32 *)file_offset);
    if (ok == 0) {
      *(i32 *)file_offset = 0;
      ((FileObject *)file)->map_close();
    }
    else {
      ((FileObject *)file)->read_line((char *)&textbuf.buf,0x100);
      ((FileObject *)file)->map_close();
      line_len = _strlen(((i8 *)&textbuf.buf));
      *(size_t *)file_offset = *(int *)file_offset + line_len + 2;
    }
  }
  return;
}

/* 004af3e0 auto */
void ParleyWindow::build_civ_list(RECT *rect,int side)

{
  u8 *field;
  int has_wonder;
  int scroll_width;
  Button *new_ctrl;
  u8 *hotspot_parent;
  u8 *scroll_parent;
  i32 hotspot;
  i32 scrollbar;
  u32 other_civ;
  RECT rect2;
  int all_contact;
  
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0)) &&
     (game.reveal_map == '\0')) {
    all_contact = 0;
  }
  else {
    all_contact = 1;
  }
  field = (u8 *)&list_counts[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  for (other_civ = 1; (int)other_civ < 8; other_civ++) {
    if (((human_civ[0] != other_civ) && (this->other_civ != other_civ)) &&
       ((all_contact != 0 || ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CONTACT) != 0))))
    {
      list_items[side * 0x801 + list_counts[side]] = other_civ;
      list_selected[side * 0x801 + list_counts[side]] =
           0;
      list_counts[side] =
           list_counts[side] + 1;
    }
  }
  if (list_counts[side] < 1) {
    field = (u8 *)&list_first_row[side];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&list_first_row[side];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&list_columns[side];
  field[0] = 2;
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  list_rects[side * 4] = rect->left;
  list_rects[side * 4 + 1] = rect->top;
  list_rects[side * 4 + 2] = rect->right;
  list_rects[side * 4 + 3] = rect->bottom;
  if (list_rows[side] <
      list_counts[side]) {
    scroll_width = GetSystemMetrics(2);
    has_wonder = rect2.right;
    list_rects[side * 4 + 2] =
         (int)list_rects[side * 4 + 2] - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = has_wonder - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[side] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[side])->create((iptr)scroll_parent,
               side + 0x40d,(iptr)&rect2,1);
    ((Scrollbar *)list_scrollbars[side])->set_range(0,
               list_counts[side] -
               list_rows[side]);
    ((Scrollbar *)list_scrollbars[side])->set_pos(0);
    ((Scrollbar *)list_scrollbars[side])->set_page_size(
               list_rows[side]);
    ((Scrollbar *)list_scrollbars[side])->set_callback(
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
    ((Scrollbar *)list_scrollbars[side])->set_track_callback(
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[side] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[side])->create((iptr)hotspot_parent,
             side + 0x40f,(iptr)(((u8 *)this + 0x39c) + side * 0x10 + 0x24));
  ((Hotspot *)list_hotspots[side])->set_click_callback(parley_civ_list_click);
  ((Hotspot *)list_hotspots[side])->set_dblclick_callback(parley_civ_list_click);
  parley_draw_civ_list(side);
  return;
}

/* 004af867 auto */
void __cdecl parley_civ_list_scroll_local(i32 pos)

{
  parley_civ_list_scroll(0,pos);
  return;
}

/* 004af885 auto */
void __cdecl parley_civ_list_scroll_remote(i32 pos)

{
  parley_civ_list_scroll(1,pos);
  return;
}

/* 004af8a3 auto */
void __cdecl parley_civ_list_scroll(int side,i32 pos)

{
  int active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  ((Wnd *)((iptr)wnd2 + 0x48))->activate();
  wnd2->list_first_row[side] = pos;
  parley_draw_civ_list(side);
  return;
}

/* 004af904 auto */
void __cdecl parley_civ_list_click(int ctrl_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int side;
  ParleyWindow *wnd2;
  
  row = wnd_get_active_child();
  if (row == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(row - 0x48);
  }
  ((Wnd *)((iptr)wnd2 + 0x48))->activate();
  side = ctrl_id - 0x40f;
  ((WndBase *)((iptr)wnd2 + 0x48))->get_cursor_pos((iptr)&cursor_x,(iptr)&cursor_y);
  if (((wnd2->list_counts[side] != 0) &&
      (row = parley_civ_list_hit_test(cursor_x,cursor_y,side), -1 < row)) &&
     (row < wnd2->list_counts[side])) {
    key_down = ((Wnd *)((iptr)wnd2 + 0x48))->is_shift_down();
    if (key_down == 0) {
      key_down = ((Wnd *)((iptr)wnd2 + 0x48))->is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < wnd2->list_counts[side];
            i = i + 1) {
          if (i != row) {
            wnd2->list_selected[i + side * 0x801] = 0;
          }
        }
        wnd2->list_anchor_row[side] = row;
        sel_flag = (uint *)(row * 4 + side * 0x2004 + 0x8400 + (iptr)wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        wnd2->list_anchor_row[side] = row;
        sel_flag = (uint *)(row * 4 + side * 0x2004 + 0x8400 + (iptr)wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < wnd2->list_counts[side]; i++)
      {
        wnd2->list_selected[i + side * 0x801] = 0;
      }
      if (wnd2->list_anchor_row[side] < row) {
        i = wnd2->list_anchor_row[side];
        range_end = row;
      }
      else {
        range_end = wnd2->list_anchor_row[side];
        i = row;
      }
      for (; i <= range_end; i++) {
        wnd2->list_selected[i + side * 0x801] = 1;
      }
    }
    parley_draw_civ_list(side);
  }
  return;
}

/* 004afb77 auto */
int __cdecl parley_civ_list_hit_test(int x,int y,int side)

{
  iptr active;
  int row;
  int list_height;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  if (y < (int)wnd2->list_rects[side * 4 + 1]) {
    row = -1;
  }
  else if (y < (int)wnd2->list_rects[side * 4 + 3]) {
    if (x < (int)wnd2->list_rects[side * 4]) {
      row = -3;
    }
    else if (x < (int)wnd2->list_rects[side * 4 + 2]) {
      row = (int)wnd2->list_rects[side * 4 + 1];
      list_height = rect_height((RECT *)(side * 0x10 + (iptr)wnd2 + 0x3c0));
      row = (y - row) / (list_height / wnd2->list_rows[side]) +
              wnd2->list_first_row[side];
    }
    else {
      row = -4;
    }
  }
  else {
    row = -2;
  }
  return row;
}

/* 004afc89 auto */
void __cdecl parley_draw_civ_list(int side)

{
  u8 *field;
  int xLeft;
  int selected;
  iptr active;
  int list_width;
  int text_dy;
  int row_top;
  int text_dx;
  i32 font_height;
  const char *text;
  i32 bg_color2;
  int row;
  int shadow_color2;
  int text_color2;
  ParleyWindow *wnd2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd2->get_clip((RECT *)saved_clip);
  wnd2->set_clip((RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  wnd2->fill_rect((iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10),dlg_style.list_bg);
  if (wnd2->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  xLeft = (int)wnd2->list_rects[side * 4];
  list_width = rect_width((RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  row_height = row_height / wnd2->list_rows[side];
  text_dy = row_height / 2;
  row_top = font2->font_cached_height();
  text_dy -= row_top / 2;
  field = (u8 *)&wnd2->side_selected[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  for (row = 0; row < wnd2->list_counts[side]; row++) {
    if (wnd2->list_selected[side * 0x801 + row] != 0) {
      field = (u8 *)&wnd2->side_selected[side];
      field[0] = 1;
      field[1] = '\0';
      field[2] = '\0';
      field[3] = '\0';
      break;
    }
  }
  for (row = 0; row < wnd2->list_rows[side];
      row = row + 1) {
    row_top = (int)wnd2->list_rects[side * 4 + 1] + row_height * row;
    SetRect(&row_rect,xLeft,row_top,list_width + xLeft,row_height + row_top);
    if (((row == 0) || (wnd2->list_first_row[side] != -1)) &&
       (wnd2->list_first_row[side] + row <
        wnd2->list_counts[side])) {
      selected = (int)wnd2->list_selected[side * 0x801 + (wnd2->list_first_row[side] + row)];
      if (selected == 0) {
        text_color2 = dlg_style.list_text;
        shadow_color2 = dlg_style.list_shadow;
      }
      else {
        text_color2 = dlg_style.sel_text_color;
        shadow_color2 = dlg_style.sel_text_shadow_color;
      }
      text_dx = (-(uint)(wnd2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_layout.margin_x;
      row_rect.left = row_rect.left + text_dx - 3;
      if (selected == 0) {
        bg_color2 = dlg_style.list_bg;
      }
      else {
        bg_color2 = dlg_style.list_sel_bg;
      }
      wnd2->fill_rect((iptr)&row_rect,bg_color2);
      if (wnd2->list_first_row[side] == -1) {
        text_clear();
        text = (label_string(label_table.string_ids[0x2e1]));
        text_append(text);
        field = (u8 *)&wnd2->side_selected[side];
        field[0] = '\0';
        field[1] = '\0';
        field[2] = '\0';
        field[3] = '\0';
      }
      else {
        font_height = font2->font_cached_height();
        parley_draw_civ_capital((i32)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)],
                           wnd2->list_first_row[side] + row,
                           xLeft + 2,row_top,font_height);
        text_clear();
        text = civ_name_plural((iptr)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)]);
        text_append(text);
        text_append_space();
        text_append_lparen();
        text = civ_leader_name((iptr)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)]);
        text_append(text);
        text_append_rparen();
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        wnd2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_dx + xLeft + 2,
                   text_dy + row_top + 1,5);
        gfx_set_text_color_index(text_color2);
        wnd2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_dx + xLeft + 1,
                   text_dy + row_top,5);
      }
      gfx_set_text_color_index(text_color2);
      wnd2->draw_text((iptr)font2,(const char *)&textbuf.buf,text_dx + xLeft,text_dy + row_top,5)
      ;
    }
  }
  wnd2->set_clip((RECT *)saved_clip);
  wnd2->update_buttons();
  ((Control *)wnd2->list_hotspots[side])->show_and_redraw()
  ;
  return;
}

/* 004b0157 auto */
void __cdecl parley_draw_civ_capital(int civ,uint row,int x,int y,int row_height)

{
  iptr active;
  int has_palace;
  iptr city2;
  i32 zoom2;
  iptr city;
  int draw_x;
  int icon_width;
  ParleyWindow *wnd2;
  int best_score;
  int score;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  if (wnd2->large_layout == 0) {
    zoom2 = 0xfffffffc;
  }
  else {
    zoom2 = 0xfffffffe;
  }
  best_score = 0;
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      score = (int)cities[city].size;
      has_palace = city_has_improvement(city,IMP_PALACE);
      if (has_palace != 0) {
        score += 200;
      }
      if (cities[city].building == -IMP_PALACE) {
        score += 100;
      }
      if (best_score < score) {
        best_score = score;
        city2 = city;
      }
    }
  }
  if (-1 < city2) {
    draw_x = x;
    if (wnd2->large_layout == 0) {
      icon_width = 0x18;
    }
    else {
      icon_width = 0x24;
    }
    if ((row & 1) != 0) {
      draw_x = x + icon_width + 2;
    }
    has_palace = gfx_zoom_scale(0x30,zoom2);
    gfx_draw_city((iptr)&parley_wnd_block,city2,0,draw_x,y - (has_palace - row_height) / 2,zoom2);
  }
  return;
}

/* 004b0b53 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl
net_send_state_diff(i32 dest,uint flags,int limit,i32 force,i32 alt_msg)

{
  int *mirror_start;
  int *run_start;
  int encoded;
  int bytes_changed;
  uint run_section;
  uint run_size;
  int *cur_ptr;
  RleStream rle;  /* (the original's locals in a row) */
  int *start_ptr;
  uint run_offset;
  GameState *game_mirror;
  uint *out_ptr;
  int record_count;
  int runs_left;
  int deadline;
  int *section_end;
  
  if (game.multiplayer < 3) {
    bytes_changed = 0;
  }
  else if (((char)force == '\0') && (net.net_player_count[0] == 1)) {
    bytes_changed = 0;
  }
  else {
    bytes_changed = 0;
    runs_left = 0;
    deadline = 0;
    start_ptr = (*(int * *)&diffeng_scan.scan_ptr);
    diffeng.send_count++;
    if ((flags & 1) != 0) {
      diffeng_invalidate_mirror();
    }
    if ((flags & 4) == 0) {
      if ((flags & 8) != 0) {
        runs_left = limit;
      }
    }
    else {
      deadline = timer_ticks();
      deadline = limit + deadline;
    }
    cur_ptr = (int *)((iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr + diffeng_scan.scan_offset);
    section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr);
    if ((*(uint *)&diffeng_scan.scan_section) == 1) {
      game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
      game_mirror->active_unit = game.active_unit;
      game_mirror->moving_unit = game.moving_unit;
      game_mirror->multiplayer = game.multiplayer;
      game_mirror->player_civ = game.player_civ;
      game_mirror->saved_current_civ = game.saved_current_civ;
      game_mirror->turn_civ = game.turn_civ;
      game_mirror->game_options = game.game_options;
      game_mirror->message_options = game.message_options;
    }
    diffeng_scan.packet = 0x66606660;
    if ((char)alt_msg == '\0') {
      diffeng_scan.packet_msg_type = 0x15;
    }
    else {
      diffeng_scan.packet_msg_type = 0x5c;
    }
    out_ptr = ((uint *)&diffeng_scan.packet_payload);
    record_count = 0;
    do {
      run_start = cur_ptr;
      mirror_start = (*(int * *)&diffeng_scan.scan_ptr);
      if (*cur_ptr == *(*(int * *)&diffeng_scan.scan_ptr)) {
        (*(int * *)&diffeng_scan.scan_ptr)++;
        if ((*(int * *)&diffeng_scan.scan_ptr) < (*(int * *)&diffeng_scan.mirror_end)) {
          cur_ptr++;
          if (section_end <= cur_ptr) {
            (*(uint *)&diffeng_scan.scan_section)++;
            if ((*(uint *)&diffeng_scan.scan_section) == 1) {
              game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
              game_mirror->active_unit = game.active_unit;
              game_mirror->moving_unit = game.moving_unit;
              game_mirror->multiplayer = game.multiplayer;
              game_mirror->player_civ = game.player_civ;
              game_mirror->saved_current_civ = game.saved_current_civ;
              game_mirror->turn_civ = game.turn_civ;
              game_mirror->game_options = game.game_options;
              game_mirror->message_options = game.message_options;
            }
            diffeng_scan.scan_offset = 0;
            cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
            section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr
                             );
          }
        }
        else {
          (*(int * *)&diffeng_scan.scan_ptr) = (int *)diffeng.mirror_buf;
          (*(uint *)&diffeng_scan.scan_section) = 0;
          diffeng_scan.scan_offset = 0;
          cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
          section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
        }
      }
      else {
        run_size = 0;
        do {
          (*(int * *)&diffeng_scan.scan_ptr)++;
          if ((*(int * *)&diffeng_scan.scan_ptr) < (*(int * *)&diffeng_scan.mirror_end)) {
            cur_ptr++;
            if (section_end <= cur_ptr) {
              run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
              run_size = (int)section_end - (int)run_start;
              run_section = (*(uint *)&diffeng_scan.scan_section);
              (*(uint *)&diffeng_scan.scan_section)++;
              if ((*(uint *)&diffeng_scan.scan_section) == 1) {
                game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
                game_mirror->active_unit = game.active_unit;
                game_mirror->moving_unit = game.moving_unit;
                game_mirror->multiplayer = game.multiplayer;
                game_mirror->player_civ = game.player_civ;
                game_mirror->saved_current_civ = game.saved_current_civ;
                game_mirror->turn_civ = game.turn_civ;
                game_mirror->game_options = game.game_options;
                game_mirror->message_options = game.message_options;
              }
              diffeng_scan.scan_offset = 0;
              cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
              section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size +
                               (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr);
            }
          }
          else {
            run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
            run_size = (int)(*(int * *)&diffeng_scan.mirror_end) - (int)mirror_start;
            run_section = (*(uint *)&diffeng_scan.scan_section);
            (*(int * *)&diffeng_scan.scan_ptr) = (int *)diffeng.mirror_buf;
            (*(uint *)&diffeng_scan.scan_section) = 0;
            diffeng_scan.scan_offset = 0;
            cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
            section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
          }
        } while (((run_size == 0) &&
                 ((int)(*(int * *)&diffeng_scan.scan_ptr) + (-(iptr)&diffeng_scan.packet - (int)mirror_start) + (int)out_ptr < diffeng.packet_flush_size)) &&
                (*cur_ptr != *(*(int * *)&diffeng_scan.scan_ptr)));
        if (0x400 < run_size) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_difflength_diff_engine_message_length,(iptr)&str_d_ss_franklinton_difference_engine_cpp_2,0xee);
        }
        if (run_size == 0) {
          run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
          run_section = (*(uint *)&diffeng_scan.scan_section);
          run_size = (int)(*(int * *)&diffeng_scan.scan_ptr) - (int)mirror_start;
        }
        if (0x400 < run_size) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_difflength_diff_engine_message_length_2,(iptr)&str_d_ss_franklinton_difference_engine_cpp_3,0xf7);
        }
        if ((flags & 0x10) == 0) {
          crt_memcpy(mirror_start,run_start,run_size);
        }
        rle.src = (u8 *)run_start;
        rle.src_end = (u8 *)run_start + run_size;
        rle.dst = (u8 *)(out_ptr + 3);
        rle.dst_end = (u8 *)&diffeng_scan.mirror_size;  /* (the packet buffer's end) */
        encoded = diffeng_rle_encode(&rle);
        if (encoded == 0) {
          crt_memcpy(out_ptr + 3,run_start,run_size);
          *out_ptr = run_section;
          out_ptr[1] = run_offset;
          out_ptr[2] = run_size;
          out_ptr = (uint *)((int)out_ptr + run_size + 0xc);
        }
        else {
          *out_ptr = run_section | 0x8000;
          out_ptr[1] = run_offset;
          out_ptr[2] = rle.out_len;
          out_ptr = (uint *)((int)out_ptr + rle.out_len + 0xc);
        }
        record_count++;
        bytes_changed += run_size;
        if (diffeng.packet_flush_size <= (int)((char *)out_ptr - (char *)&diffeng_scan.packet)) {
          diffeng_scan.packet_size = (i32)((char *)out_ptr - (char *)&diffeng_scan.packet);
          diffeng_scan.packet_record_count = record_count;
          net_send_message(NM_ALPHA_DIFFERENCE_ENGINE,dest,0,0,0,0,0,0,0,(iptr)&diffeng_scan.packet);
          if ((flags & 1) != 0) {
            XD_FlushSendBuffer(5000);
          }
          out_ptr = ((uint *)&diffeng_scan.packet_payload);
          record_count = 0;
        }
        if ((((flags & 4) != 0) && (encoded = timer_ticks(), deadline < encoded)) ||
           (((flags & 8) != 0 && (runs_left = runs_left - 1, runs_left == 0)))) break;
      }
    } while (start_ptr != (*(int * *)&diffeng_scan.scan_ptr));
    if (((uint *)&diffeng_scan.packet_payload) < out_ptr) {
      diffeng_scan.packet_size = (i32)((char *)out_ptr - (char *)&diffeng_scan.packet);
      diffeng_scan.packet_record_count = record_count;
      net_send_message(NM_ALPHA_DIFFERENCE_ENGINE,dest,0,0,0,0,0,0,0,(iptr)&diffeng_scan.packet);
      if ((flags & 1) != 0) {
        XD_FlushSendBuffer(5000);
      }
    }
    diffeng_scan.scan_offset = (int)cur_ptr - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
  }
  return bytes_changed;
}

/* 004b401c auto */
void __cdecl parley_wnd_global_init(void)

{
  parley_wnd_static_init();
  parley_wnd_reg_atexit();
  return;
}

/* 004b4036 auto */
void __cdecl parley_wnd_static_init(void)

{
  parley_wnd_block.construct((RECT *)&menu_prefs.parley_wnd_rect);
  return;
}

/* 004b405b auto */
void __cdecl parley_wnd_reg_atexit(void)

{
  _atexit(parley_wnd_static_dtor);
  return;
}

/* 004b4078 auto */
void __cdecl parley_wnd_static_dtor(void)

{
  parley_wnd_block.destruct();
  return;
}

/* 004b4092 auto */
void __cdecl chat_wnd_global_init(void)

{
  chat_wnd_static_init();
  chat_wnd_reg_atexit();
  return;
}

/* 004b40ac auto */
void __cdecl chat_wnd_static_init(void)

{
  chat_wnd_block.construct((RECT *)&menu_prefs.chat_wnd_rect);
  return;
}

/* 004b40d1 auto */
void __cdecl chat_wnd_reg_atexit(void)

{
  _atexit(chat_wnd_static_dtor);
  return;
}

/* 004b40ee auto */
void __cdecl chat_wnd_static_dtor(void)

{
  chat_wnd_block.destruct();
  return;
}

/* 004b4108 auto */
ParleyWindow *ParleyWindow::construct(RECT *rect)

{
  u8 *field;
  int width;
  int i;
  
  this->SurfaceWindow::construct();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  init_rect = (u32)rect;
  closed = 1;
  prefs_load_civ2dat();
  width = rect_width((RECT *)init_rect);
  if (width < 1) {
    wnd_x = 0;
    wnd_y = 0;
  }
  else {
    wnd_x = *(u32 *)init_rect;
    wnd_y = *(u32 *)(init_rect + 4);
    if (width < 0x321) {
      large_layout = 0;
    }
    else {
      large_layout = 1;
    }
  }
  close_button = 0;
  send_button = 0;
  clear_button = 0;
  conceal_button = 0;
  reveal_button = 0;
  mode_button = 0;
  intel_button = 0;
  mode2_button = 0;
  never_mind_button = 0;
  accept_button = 0;
  no_thanks_button = 0;
  counter_button = 0;
  for (i = 0; i < 8; i++) {
    field = (u8 *)&send_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&mute_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&send_to_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&muted_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  remain_anonymous = 0;
  mute_anonymous = 0;
  broadcast_all = 0;
  broadcast_checkbox = 0;
  anonymous_checkbox = 0;
  mute_anonymous_checkbox = 0;
  desc_edit = 0;
  other_civ = 0xffffffff;
  received_offer = 0;
  exchange_kind[0] = 0;
  exchange_kind[1] = 1;
  demand_kind = 0;
  selected_action = 0xffffffff;
  gift_kind = 0;
  map_choice[1] = 0;
  map_choice[0] = map_choice[1];
  gold_amount[1] = 0;
  gold_amount[0] = gold_amount[1];
  proposal_kind = 0;
  treaty_kind = 0;
  side_selected[1] = 0;
  side_selected[0] = side_selected[1];
  caption_buttons[0].hotspot = 0;
  caption_buttons[1].hotspot = 0;
  caption_buttons[2].hotspot = 0;
  chat_input_edit = 0;
  chat_log_edit = 0;
  (*(i32 *)&chat.input_buffer) = 0;
  (*(i32 *)&chat.log_buffer) = 0;
  font_size16_bold_block.chat_log_capacity = 0;
  for (i = 0; i < 2; i++) {
    field = (u8 *)&side_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&side_radios[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&gold_edits[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&list_scrollbars[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&list_hotspots[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&sort_by_unit_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&sort_by_city_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&info_show_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&info_hide_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  remote_units_revealed = '\0';
  local_units_revealed = '\0';
  local_cities_revealed = '\0';
  remote_cities_revealed = '\0';
  return this;
}

/* 004b4593 auto */
void ParleyWindow::destruct()

{
  int created;
  size_t log_len;
  FileObject log_file;
  uint unwind_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  unwind_state = 0;
  if (mode == 4) {
    log_file.init();
    unwind_state = (unwind_state & 0xffffff00 | (byte)(1));
    if ((menu_prefs.clear_chat_on_boot != 0) && (chat.log_buffer != (char *)0x0)) {
      *chat.log_buffer = '\0';
      created = log_file.create((const char *)&str_chatlog_txt_2);
      if (created != 0) {
        log_len = _strlen(chat.log_buffer);
        log_file.write((iptr)chat.log_buffer,log_len);
        log_file.map_close();
      }
    }
    if (chat.log_buffer != (char *)0x0) {
      operator_delete(chat.log_buffer);
      chat.log_buffer = (char *)0x0;
    }
    if (chat.input_buffer != (void *)0x0) {
      operator_delete(chat.input_buffer);
      chat.input_buffer = (void *)0x0;
    }
    unwind_state = unwind_state & 0xffffff00;
    /* inlined unwind funclet 004b4705 */ { log_file.close(); }
  }
  destroy_controls();
  wnd_destroy_with_surface();
  unwind_state = 0xffffffff;
  /* inlined unwind funclet 004b4711 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 004b4735 auto */
void ParleyWindow::create(u32 mode)

{
  int file_size;
  i32 height;
  i32 width;
  i8 *palette;
  i8 *parent;
  u8 *wnd2;
  i8 log_file[160];
  
  closed = 1;
  if (this == (ParleyWindow *)0x0) {
    wnd2 = (u8 *)0x0;
  }
  else {
    wnd2 = ((u8 *)this + 0x48);
  }
  wnd_set_active_child((iptr)wnd2);
  this->mode = mode;
  if (this->mode == 4) {
    (*(UINT *)&font_size16_bold_block.chat_log_capacity) = GetPrivateProfileIntA
                             (str_civilization_gold_7,str_chatshowsize,-1,
                              str_civ_ini_7);
    if (((*(UINT *)&font_size16_bold_block.chat_log_capacity) == 0xffffffff) || ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < 0x2000)) {
      WritePrivateProfileStringA
                (str_civilization_gold_8,str_chatshowsize_2,((i8 *)&chat_show_size_default_str),str_civ_ini_8
                );
      (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0x2000;
    }
    else if (0xdfff < (int)(*(UINT *)&font_size16_bold_block.chat_log_capacity)) {
      WritePrivateProfileStringA
                (str_civilization_gold_9,str_chatshowsize_3,str_57344,
                 str_civ_ini_9);
      (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0xe000;
    }
    if ((*&chat.log_buffer) == (i8 *)0x0) {
      (*&chat.log_buffer) = (i8 *)(operator_new((*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1));
    }
    _memset((*&chat.log_buffer),0,(*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1);
    if (chat.input_buffer == (void *)0x0) {
      chat.input_buffer = operator_new(0x101);
    }
    _memset(chat.input_buffer,0,0x101);
    ((FileObject *)log_file)->init();
    ON_SCOPE_EXIT(((FileObject *)&log_file)->close());
    if (menu_prefs.clear_chat_on_new_game == 0) {
      file_size = ((FileObject *)log_file)->open((const char *)&str_chatlog_txt_3);
      if (file_size != 0) {
        file_size = ((FileObject *)log_file)->get_size();
        if ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < file_size) {
          ((FileObject *)log_file)->seek_cur(file_size - (*(UINT *)&font_size16_bold_block.chat_log_capacity));
        }
        ((FileObject *)log_file)->read(*&chat.log_buffer,(*(UINT *)&font_size16_bold_block.chat_log_capacity));
        ((FileObject *)log_file)->map_close();
      }
      else {
        *(*&chat.log_buffer) = 0;
      }
    }
    else {
      *(*&chat.log_buffer) = 0;
    }
  }
  if ((wnd_x == 0) && (wnd_y == 0)) {
    pick_size_mode();
  }
  crt_strcpy((char *)((u8 *)this + 0x15c),((i8 *)&parley_wnd_empty_title_str));
  if (this->mode == 4) {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)init_rect);
    width = rect_width((RECT *)init_rect);
    wnd_create_with_surface(0,0x602,*(i32 *)init_rect,
               *(i32 *)(init_rect + 4),width,height,(iptr)palette,(iptr)parent);
  }
  else {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)init_rect);
    width = rect_width((RECT *)init_rect);
    wnd_create_with_surface(0,0x602,*(i32 *)init_rect,
               *(i32 *)(init_rect + 4),width,height,(iptr)palette,(iptr)parent);
  }
  calc_rect();
  ((WndBase *)((u8 *)this + 0x48))->move_to_rect(init_rect);
  built_layout = 0xffffffff;
  left_kind = parley_panes.mode3_left_kind;
  right_kind = parley_panes.mode3_right_kind;
  shown_left_kind = 0x15;
  shown_right_kind = 0x15;
  ((WndHandlers *)((u8 *)this + 0x58))->set_close_handler(diplo_wnd_close_active);
  set_redraw_callback(parley_wnd_redraw);
  ((WndHandlers *)((u8 *)this + 0x58))->set_move_handler((WndMouseFn)parley_wnd_on_move);
  ((WndHandlers *)((u8 *)this + 0x58))->set_keydown_handler(parley_wnd_on_key);
  calc_layout();
  sync_sides();
  if (((this->mode == 3) || (this->mode == 2)) || (this->mode == 1)) {
    ((Button *)never_mind_button)->disable();
  }
  update_buttons();
  return;
}

/* 004b4c09 auto */
void __cdecl parley_wnd_on_move(void)

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  ((WndBase *)((iptr)wnd2 + 0x48))->get_rect_in_parent((iptr)wnd2->init_rect);
  wnd2->wnd_x = *(i32 *)wnd2->init_rect;
  wnd2->wnd_y = *(i32 *)((int)wnd2->init_rect + 4);
  return;
}

/* 004b4c81 auto */
void ParleyWindow::pick_size_mode()

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd2->large_layout = 0;
  if (999 < caption_font_block.screen_width) {
    wnd2->large_layout = 1;
  }
  calc_rect();
  return;
}

/* 004b4cf0 auto */
void ParleyWindow::calc_rect()

{
  int size;
  int font_height;
  int height2;
  int width2;
  u32 top;
  u32 left;
  
  if (large_layout == 0) {
    width2 = 0x208;
  }
  else {
    width2 = 800;
  }
  if (large_layout == 0) {
    height2 = 0x14f;
  }
  else {
    height2 = 0x1cc;
  }
  if (large_layout == 0) {
    font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + caption_height * 2 + 0xe;
  size = rect_width((RECT *)init_rect);
  if (size == 0) {
    size = main_frame_wnd_block.main_frame_wnd.client_width();
    wnd_x = (size >> 1) - (width2 >> 1);
    left = wnd_x;
    size = main_frame_wnd_block.main_frame_wnd.client_height();
    wnd_y = (size >> 1) - (height2 >> 1);
    top = wnd_y;
  }
  else {
    left = wnd_x;
    top = wnd_y;
  }
  SetRect((LPRECT)init_rect,left,top,width2 + left,height2 + top);
  ((WndBase *)((u8 *)this + 0x48))->set_caption_height(caption_height);
  return;
}

/* 004b4e8a auto */
void ParleyWindow::calc_layout()

{
  u32 dim;
  int font_height;
  
  dim = get_width();
  width = dim;
  dim = get_height();
  height = dim;
  if (large_layout == 0) {
    font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + font_height;
  SetRect((LPRECT)&client_left,0,0,width,height);
  SetRect((LPRECT)&panel_left,0,0,width + (panel_layout.padding_x + panel_layout.margin_x) * -2,
          height - (panel_layout.margin_y * 2 + caption_height * 2 + panel_layout.padding_x));
  OffsetRect((LPRECT)&panel_left,panel_layout.padding_x + panel_layout.margin_x,caption_height + panel_layout.margin_y);
  return;
}

/* 004b4fb2 auto */
void ParleyWindow::sync_sides()

{
  if (built_layout == large_layout) {
    if (shown_left_kind != left_kind) {
      add_side_dialog(left_kind,0);
      shown_left_kind = left_kind;
    }
    if (shown_right_kind != right_kind) {
      add_side_dialog(right_kind,1);
      shown_right_kind = right_kind;
    }
  }
  else {
    destroy_controls();
    create_controls();
    add_side_dialog(left_kind,0);
    add_side_dialog(right_kind,1);
    shown_left_kind = left_kind;
    shown_right_kind = right_kind;
    built_layout = large_layout;
  }
  return;
}

/* 004b50cf auto */
void ParleyWindow::destroy_controls()

{
  u8 *slot;
  int i;
  
  if (received_offer != 0) {
    operator_delete((void *)received_offer);
    received_offer = 0;
  }
  for (i = 0; i < 3; i++) {
    if (caption_buttons[i].hotspot != 0) {
      if ((Hotspot *)caption_buttons[i].hotspot != (Hotspot *)0x0) {
        ((Hotspot *)caption_buttons[i].hotspot)->scalar_destruct(1);
      }
      slot = (u8 *)&caption_buttons[i].hotspot;
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  if (close_button != 0) {
    if ((Button *)close_button != (Button *)0x0) {
      ((Button *)close_button)->scalar_destruct(1);
    }
    close_button = 0;
  }
  if (send_button != 0) {
    if ((Button *)send_button != (Button *)0x0) {
      ((Button *)send_button)->scalar_destruct(1);
    }
    send_button = 0;
  }
  if (clear_button != 0) {
    if ((Button *)clear_button != (Button *)0x0) {
      ((Button *)clear_button)->scalar_destruct(1);
    }
    clear_button = 0;
  }
  if (reveal_button != 0) {
    if ((Button *)reveal_button != (Button *)0x0) {
      ((Button *)reveal_button)->scalar_destruct(1);
    }
    reveal_button = 0;
  }
  if (conceal_button != 0) {
    if ((Button *)conceal_button != (Button *)0x0) {
      ((Button *)conceal_button)->scalar_destruct(1);
    }
    conceal_button = 0;
  }
  if (mode_button != 0) {
    if ((Button *)mode_button != (Button *)0x0) {
      ((Button *)mode_button)->scalar_destruct(1);
    }
    mode_button = 0;
  }
  if (intel_button != 0) {
    if ((Button *)intel_button != (Button *)0x0) {
      ((Button *)intel_button)->scalar_destruct(1);
    }
    intel_button = 0;
  }
  if (mode2_button != 0) {
    if ((Button *)mode2_button != (Button *)0x0) {
      ((Button *)mode2_button)->scalar_destruct(1);
    }
    mode2_button = 0;
  }
  if (never_mind_button != 0) {
    if ((Button *)never_mind_button != (Button *)0x0) {
      ((Button *)never_mind_button)->scalar_destruct(1);
    }
    never_mind_button = 0;
  }
  if (accept_button != 0) {
    if ((Button *)accept_button != (Button *)0x0) {
      ((Button *)accept_button)->scalar_destruct(1);
    }
    accept_button = 0;
  }
  if (no_thanks_button != 0) {
    if ((Button *)no_thanks_button != (Button *)0x0) {
      ((Button *)no_thanks_button)->scalar_destruct(1);
    }
    no_thanks_button = 0;
  }
  if (counter_button != 0) {
    if ((Button *)counter_button != (Button *)0x0) {
      ((Button *)counter_button)->scalar_destruct(1);
    }
    counter_button = 0;
  }
  if (chat_log_edit != 0) {
    if ((Button *)chat_log_edit != (Button *)0x0) {
      ((EditControl *)chat_log_edit)->scalar_destruct(1);
    }
    chat_log_edit = 0;
  }
  if (chat_input_edit != 0) {
    if ((Button *)chat_input_edit != (Button *)0x0) {
      ((EditControl *)chat_input_edit)->scalar_destruct(1);
    }
    chat_input_edit = 0;
  }
  if (anonymous_checkbox != 0) {
    if ((Checkbox *)anonymous_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)anonymous_checkbox)->scalar_destruct(1);
    }
    anonymous_checkbox = 0;
  }
  if (mute_anonymous_checkbox != 0) {
    if ((Checkbox *)mute_anonymous_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)mute_anonymous_checkbox)->scalar_destruct(1);
    }
    mute_anonymous_checkbox = 0;
  }
  if (desc_edit != 0) {
    if ((Button *)desc_edit != (Button *)0x0) {
      ((EditControl *)desc_edit)->scalar_destruct(1);
    }
    desc_edit = 0;
  }
  if (broadcast_checkbox != 0) {
    if ((Checkbox *)broadcast_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)broadcast_checkbox)->scalar_destruct(1);
    }
    broadcast_checkbox = 0;
  }
  for (i = 0; i < 8; i++) {
    if (send_checkboxes[i] != 0) {
      if ((Checkbox *)send_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)send_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&send_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (mute_checkboxes[i] != 0) {
      if ((Checkbox *)mute_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)mute_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&mute_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  for (i = 0; i < 2; i++) {
    if (side_checkboxes[i] != 0) {
      if ((Checkbox *)side_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)side_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&side_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (side_radios[i] != 0) {
      if ((RadioGroup *)side_radios[i] != (RadioGroup *)0x0) {
        ((RadioGroup *)side_radios[i])->scalar_destruct(1);
      }
      slot = (u8 *)&side_radios[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (gold_edits[i] != 0) {
      if ((Button *)gold_edits[i] != (Button *)0x0) {
        ((EditControl *)gold_edits[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&gold_edits[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (list_scrollbars[i] != 0) {
      if ((Button *)list_scrollbars[i] != (Button *)0x0) {
        ((Scrollbar *)list_scrollbars[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&list_scrollbars[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (list_hotspots[i] != 0) {
      if ((Hotspot *)list_hotspots[i] != (Hotspot *)0x0) {
        ((Hotspot *)list_hotspots[i])->scalar_destruct(1);
      }
      slot = (u8 *)&list_hotspots[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (sort_by_unit_buttons[i] != 0) {
      if ((Button *)sort_by_unit_buttons[i] != (Button *)0x0) {
        ((Button *)sort_by_unit_buttons[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&sort_by_unit_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (sort_by_city_buttons[i] != 0) {
      if ((Button *)sort_by_city_buttons[i] != (Button *)0x0) {
        ((Button *)sort_by_city_buttons[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&sort_by_city_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (info_show_buttons[i] != 0) {
      if ((Button *)info_show_buttons[i] != (Button *)0x0) {
        ((Button *)info_show_buttons[i])->scalar_destruct(1);
      }
      slot = (u8 *)&info_show_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (info_hide_buttons[i] != 0) {
      if ((Button *)info_hide_buttons[i] != (Button *)0x0) {
        ((Button *)info_hide_buttons[i])->scalar_destruct(1);
      }
      slot = (u8 *)&info_hide_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  return;
}

/* Part of ParleyWindow::create_controls (004b5c93): the three caption
   buttons: the sprite, size and place of each. */
void ParleyWindow::create_caption_buttons()
{
  u8 *slot_ptr;
  i32 sprite_dim;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  u8 *caption_parent;
  i32 hotspot;
  int caption_x;
  int i;
  tagRECT rect;
  int ctrl_height;
  
  caption_x = panel_layout.padding_x;
  for (i = 0; i < 3; i++) {
    if (i == 0) {
      caption_buttons[0].sprite = (Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite);
    }
    else if (i == 1) {
      caption_buttons[1].sprite = (Sprite *)((i8 *)&caption_zoom_out_sprite);
    }
    else {
      caption_buttons[i].sprite = (Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite);
    }
    caption_buttons[i].id = i + 1;
    sprite_dim = caption_buttons[i].sprite->width();
    caption_buttons[i].width = sprite_dim;
    sprite_dim = caption_buttons[i].sprite->height();
    caption_buttons[i].height = sprite_dim;
    slot_ptr = (u8 *)&caption_buttons[i].hotspot;
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
    ctrl_width = (int)(caption_buttons[i].width * caption_height) / 0x18;
    ctrl_height = (int)(caption_buttons[i].height * caption_height) / 0x18;
    ctrl_left = ((int)caption_height >> 1) - (ctrl_height >> 1);
    SetRect(&rect,caption_x,ctrl_left,ctrl_width + caption_x,ctrl_left + ctrl_height);
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      hotspot = 0;
    }
    else {
      hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
    }
    caption_buttons[i].hotspot = hotspot;
    if (this == (ParleyWindow *)0x0) {
      caption_parent = (u8 *)0x0;
    }
    else {
      caption_parent = ((u8 *)this + 0x48);
    }
    ((Hotspot *)caption_buttons[i].hotspot)->create((iptr)caption_parent,
               caption_buttons[i].id + 1000,(iptr)&rect);
    ((Hotspot *)caption_buttons[i].hotspot)->set_click_callback(parley_wnd_on_caption_click);
    caption_x = caption_x + ctrl_width + panel_layout.margin_x;
  }
}

/* Part of ParleyWindow::create_controls (004b5c93): the Close button across
   the bottom. */
void ParleyWindow::create_close_button()
{
  u32 wnd_width;
  int y_delta;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  int yTop;
  u8 *close_parent;
  iptr close_label;
  int close_line_h2;
  int close_line_h;
  uptr close_btn;
  i8 *button_font;
  tagRECT rect;
  int ctrl_height;
  
  if (large_layout == 0) {
    button_font = ((i8 *)&font_size16);
  }
  else {
    button_font = ((i8 *)&font_size24);
  }
  ui_set_button_font((iptr)button_font);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    close_btn = 0;
  }
  else {
    close_btn = (uptr)(button_ctor(new_ctrl));
  }
  close_button = close_btn;
  ctrl_width = panel_layout.bevel_width + panel_layout.margin_x + 2;
  if (large_layout == 0) {
    close_line_h = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    close_line_h = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  yTop = height - ((panel_layout.margin_y + panel_layout.padding_y) * 2 + close_line_h);
  wnd_width = width;
  y_delta = panel_layout.margin_x * 2;
  ctrl_left = panel_layout.bevel_width * 2;
  if (large_layout == 0) {
    close_line_h2 = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    close_line_h2 = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  ctrl_height = panel_layout.padding_y * 2 + close_line_h2;
  SetRect(&rect,ctrl_width,yTop,(wnd_width - (y_delta + ctrl_left + 5)) + ctrl_width,yTop + ctrl_height);
  if (mode == 4) {
    close_label = (iptr)(label_string(label_table.string_ids[0x2cb]));
  }
  else {
    close_label = (iptr)(label_string(label_table.string_ids[0x2cc]));
  }
  if (this == (ParleyWindow *)0x0) {
    close_parent = (u8 *)0x0;
  }
  else {
    close_parent = ((u8 *)this + 0x48);
  }
  ((Button *)close_button)->create((iptr)close_parent,0x3ed,(iptr)&rect,(const char *)close_label);
  ((Button *)close_button)->set_callback(diplo_wnd_escape_close);
}

/* Part of ParleyWindow::create_controls (004b5c93): the chat history and
   input edits with the Send and Clear buttons; leaves ctrl_top and
   ctrl_height for the row of buttons above them. */
void ParleyWindow::create_chat_controls(int &ctrl_top,int &ctrl_height)
{
  int y_delta;
  LONG xLeft;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  EditControl *new_edit;
  iptr label;
  int footer_line_h;
  u8 *clear_parent;
  u8 *send_parent;
  u8 *input_parent;
  int input_font_h4;
  int input_font_h3;
  int input_font_h2;
  int input_font_h1;
  u8 *history_parent;
  int hist_font_h4;
  int hist_font_h3;
  int hist_font_h2;
  int hist_font_h1;
  uptr clear_button;
  uptr send_button;
  i8 *input_font;
  uptr input_edit;
  i8 *history_font;
  uptr history_edit;
  tagRECT rect;
  i32 input_style;
  
  new_edit = (EditControl *)(operator_new(0x48));
  if (new_edit == (EditControl *)0x0) {
    history_edit = 0;
  }
  else {
    history_edit = (uptr)(edit_ctor(new_edit));
  }
  chat_log_edit = history_edit;
  ctrl_left = panel_left + panel_layout.margin_x;
  ctrl_width = rect_width((RECT *)&panel_left);
  ctrl_width += panel_layout.margin_x * -2;
  if (mode == 4) {
    if (large_layout == 0) {
      hist_font_h3 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h3 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_top = panel_bottom + (panel_layout.margin_y + hist_font_h3) * -9;
    if (large_layout == 0) {
      hist_font_h4 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h4 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = hist_font_h4 * 9 + panel_layout.margin_y * 8;
  }
  else {
    if (large_layout == 0) {
      hist_font_h1 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h1 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_top = panel_bottom + (panel_layout.margin_y + hist_font_h1) * -6;
    if (large_layout == 0) {
      hist_font_h2 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h2 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = panel_layout.margin_y * 5 + hist_font_h2 * 6;
  }
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (large_layout == 0) {
    history_font = ((i8 *)&font_size14);
  }
  else {
    history_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((iptr)history_font);
  if (this == (ParleyWindow *)0x0) {
    history_parent = (u8 *)0x0;
  }
  else {
    history_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)chat_log_edit)->create_styled((iptr)history_parent,0x3ee,(iptr)&rect,((const char *)(*(iptr *)&chat.log_buffer)),
             0x122,0);
  ((EditControl *)chat_log_edit)->set_max_len(0x2000);
  ((EditControl *)chat_log_edit)->set_callback(parley_edit_escape_filter);
  ((Control *)chat_log_edit)->show_and_redraw();
  new_edit = (EditControl *)(operator_new(0x48));
  if (new_edit == (EditControl *)0x0) {
    input_edit = 0;
  }
  else {
    input_edit = (uptr)(edit_ctor(new_edit));
  }
  chat_input_edit = input_edit;
  ctrl_width -= ctrl_width / 10;
  if (mode == 4) {
    if (large_layout == 0) {
      input_font_h3 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h3 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    y_delta = panel_layout.margin_y * -4 + input_font_h3 * -3;
    if (large_layout == 0) {
      input_font_h4 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h4 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = input_font_h4 * 3 + panel_layout.margin_y * 3;
  }
  else {
    if (large_layout == 0) {
      input_font_h1 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h1 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    y_delta = panel_layout.margin_y * -3 + input_font_h1 * -2;
    if (large_layout == 0) {
      input_font_h2 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h2 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = panel_layout.margin_y * 2 + input_font_h2 * 2;
  }
  ctrl_top += y_delta;
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (large_layout == 0) {
    input_font = ((i8 *)&font_size14);
  }
  else {
    input_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((iptr)input_font);
  if (mode == 4) {
    input_style = 0x200;
  }
  else {
    input_style = 0x220;
  }
  if (this == (ParleyWindow *)0x0) {
    input_parent = (u8 *)0x0;
  }
  else {
    input_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)chat_input_edit)->create_styled((iptr)input_parent,0x3ef,(iptr)&rect,((const char *)(*(iptr *)&chat.input_buffer)),
             input_style,0);
  ((EditControl *)chat_input_edit)->set_max_len(0x100);
  ((EditControl *)chat_input_edit)->set_callback(chat_input_on_key);
  ((Control *)chat_input_edit)->set_initial_focus();
  ((Control *)chat_input_edit)->show_and_redraw();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    send_button = 0;
  }
  else {
    send_button = (uptr)(button_ctor(new_ctrl));
  }
  xLeft = rect.right;
  this->send_button = send_button;
  ctrl_width = ctrl_width / 9;
  ctrl_height = ctrl_height / 2;
  SetRect(&rect,rect.right,ctrl_top,ctrl_width + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    send_parent = (u8 *)0x0;
  }
  else {
    send_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2cd]));
  ((Button *)this->send_button)->create((iptr)send_parent,0x3f0,(iptr)&rect,(const char *)label);
  ((Button *)this->send_button)->set_callback((CtrlNotifyFn)chat_send_button_on_click);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    clear_button = 0;
  }
  else {
    clear_button = (uptr)(button_ctor(new_ctrl));
  }
  this->clear_button = clear_button;
  ctrl_top += ctrl_height;
  SetRect(&rect,xLeft,ctrl_top,ctrl_width + xLeft,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    clear_parent = (u8 *)0x0;
  }
  else {
    clear_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2ce]));
  ((Button *)this->clear_button)->create((iptr)clear_parent,0x41e,(iptr)&rect,(const char *)label);
  ((Button *)this->clear_button)->set_callback((CtrlNotifyFn)chat_clear_button_on_click);
  ctrl_top -= ctrl_height;
  if (large_layout == 0) {
    footer_line_h = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    footer_line_h = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  ctrl_height = panel_layout.padding_y * 2 + footer_line_h;
  ctrl_top -= (panel_layout.margin_y + ctrl_height);
  divider_y = ctrl_top - panel_layout.margin_y;
}

/* Part of ParleyWindow::create_controls (004b5c93): the parley buttons
   (conceal, reveal, intelligence, the two mode buttons, never mind, accept,
   no thanks, counter) in a row above the chat. */
void ParleyWindow::create_parley_buttons(int ctrl_top,int ctrl_height)
{
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  iptr label;
  u8 *counter_parent;
  u8 *no_thanks_parent;
  u8 *accept_parent;
  u8 *never_mind_parent;
  u8 *mode2_parent;
  u8 *mode_parent;
  u8 *intel_parent;
  u8 *reveal_parent;
  u8 *conceal_parent;
  uptr counter_button;
  uptr no_thanks_button;
  uptr accept_button;
  uptr never_mind_button;
  uptr mode2_button;
  uptr mode_button;
  uptr intel_button;
  uptr reveal_button;
  uptr conceal_button;
  tagRECT rect;
  
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    conceal_button = 0;
  }
  else {
    conceal_button = (uptr)(button_ctor(new_ctrl));
  }
  this->conceal_button = conceal_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left += panel_layout.margin_x * -2;
  SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
  if (this == (ParleyWindow *)0x0) {
    conceal_parent = (u8 *)0x0;
  }
  else {
    conceal_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x36d]));
  ((Button *)this->conceal_button)->create((iptr)conceal_parent,0x431,(iptr)&rect,(const char *)label);
  ((Button *)this->conceal_button)->set_callback((CtrlNotifyFn)parley_on_conceal_info);
  ((Control *)this->conceal_button)->hide();
  ((Button *)this->conceal_button)->disable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    reveal_button = 0;
  }
  else {
    reveal_button = (uptr)(button_ctor(new_ctrl));
  }
  this->reveal_button = reveal_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
  if (this == (ParleyWindow *)0x0) {
    reveal_parent = (u8 *)0x0;
  }
  else {
    reveal_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x36e]));
  ((Button *)this->reveal_button)->create((iptr)reveal_parent,0x432,(iptr)&rect,(const char *)label);
  ((Button *)this->reveal_button)->set_callback((CtrlNotifyFn)parley_on_reveal_info);
  ((Control *)this->reveal_button)->hide();
  ((Button *)this->reveal_button)->disable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    intel_button = 0;
  }
  else {
    intel_button = (uptr)(button_ctor(new_ctrl));
  }
  this->intel_button = intel_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left = (ctrl_left + panel_layout.margin_x * -2) / 3;
  SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    intel_parent = (u8 *)0x0;
  }
  else {
    intel_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x68]));
  ((Button *)this->intel_button)->create((iptr)intel_parent,0x428,(iptr)&rect,(const char *)label);
  ((Button *)this->intel_button)->set_callback((CtrlNotifyFn)parley_on_check_intel);
  ((Button *)this->intel_button)->enable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mode_button = 0;
  }
  else {
    mode_button = (uptr)(button_ctor(new_ctrl));
  }
  this->mode_button = mode_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    mode_parent = (u8 *)0x0;
  }
  else {
    mode_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2cf]));
  ((Button *)this->mode_button)->create((iptr)mode_parent,0x3f1,(iptr)&rect,(const char *)label);
  ((Button *)this->mode_button)->set_callback(parley_on_mode_chosen);
  ((Button *)this->mode_button)->enable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mode2_button = 0;
  }
  else {
    mode2_button = (uptr)(button_ctor(new_ctrl));
  }
  this->mode2_button = mode2_button;
  if (this == (ParleyWindow *)0x0) {
    mode2_parent = (u8 *)0x0;
  }
  else {
    mode2_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x344]));
  ((Button *)this->mode2_button)->create((iptr)mode2_parent,0x427,(iptr)&rect,(const char *)label);
  ((Button *)this->mode2_button)->set_callback(parley_on_mode_chosen);
  ((Button *)this->mode2_button)->disable();
  ((Control *)this->mode2_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    never_mind_button = 0;
  }
  else {
    never_mind_button = (uptr)(button_ctor(new_ctrl));
  }
  this->never_mind_button = never_mind_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    never_mind_parent = (u8 *)0x0;
  }
  else {
    never_mind_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d0]));
  ((Button *)this->never_mind_button)->create((iptr)never_mind_parent,0x3f2,(iptr)&rect,(const char *)label);
  ((Button *)this->never_mind_button)->set_callback((CtrlNotifyFn)parley_on_never_mind);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    accept_button = 0;
  }
  else {
    accept_button = (uptr)(button_ctor(new_ctrl));
  }
  this->accept_button = accept_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left = (ctrl_left + panel_layout.margin_x * -2) / 3;
  SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    accept_parent = (u8 *)0x0;
  }
  else {
    accept_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d1]));
  ((Button *)this->accept_button)->create((iptr)accept_parent,0x3f2,(iptr)&rect,(const char *)label);
  ((Button *)this->accept_button)->set_callback((CtrlNotifyFn)parley_on_accept_offer);
  ((Control *)this->accept_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    no_thanks_button = 0;
  }
  else {
    no_thanks_button = (uptr)(button_ctor(new_ctrl));
  }
  this->no_thanks_button = no_thanks_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    no_thanks_parent = (u8 *)0x0;
  }
  else {
    no_thanks_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d2]));
  ((Button *)this->no_thanks_button)->create((iptr)no_thanks_parent,0x3f2,(iptr)&rect,(const char *)label);
  ((Button *)this->no_thanks_button)->set_callback((CtrlNotifyFn)parley_on_no_thanks);
  ((Control *)this->no_thanks_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    counter_button = 0;
  }
  else {
    counter_button = (uptr)(button_ctor(new_ctrl));
  }
  this->counter_button = counter_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    counter_parent = (u8 *)0x0;
  }
  else {
    counter_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d3]));
  ((Button *)this->counter_button)->create((iptr)counter_parent,0x3f2,(iptr)&rect,(const char *)label);
  ((Button *)this->counter_button)->set_callback((CtrlNotifyFn)parley_on_counter_offer);
  ((Control *)this->counter_button)->hide();
}

/* 004b5c93 auto */
void ParleyWindow::create_controls()

{
  int ctrl_top;
  int ctrl_height;
  
  create_caption_buttons();
  create_close_button();
  create_chat_controls(ctrl_top,ctrl_height);
  if (mode != 4) {
    create_parley_buttons(ctrl_top,ctrl_height);
  }
  return;
}

/* 004b74c4 auto */
void __cdecl parley_wnd_on_caption_click(int control_id)

{
  iptr child_wnd;
  ParleyWindow *parley_wnd_ptr;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    parley_wnd_ptr = (ParleyWindow *)0x0;
  }
  else {
    parley_wnd_ptr = (ParleyWindow *)(child_wnd - 0x48);
  }
  ((Wnd *)((u8 *)parley_wnd_ptr + 0x48))->activate();
  if (control_id == 0x3e9) {
    diplo_wnd_close_active();
  }
  else if (control_id == 0x3ea) {
    if (0 < (int)parley_wnd_ptr->large_layout) {
      ((WndBase *)((u8 *)parley_wnd_ptr + 0x48))->hide();
      parley_wnd_ptr->large_layout--;
      parley_wnd_ptr->calc_rect();
      ((WndBase *)((u8 *)parley_wnd_ptr + 0x48))->move_to_rect(parley_wnd_ptr->init_rect);
      parley_wnd_ptr->SurfaceWindow::show();
    }
  }
  else if (((control_id == 0x3eb) && ((int)parley_wnd_ptr->large_layout < 1)) && (999 < caption_font_block.screen_width)) {
    ((WndBase *)((u8 *)parley_wnd_ptr + 0x48))->hide();
    parley_wnd_ptr->large_layout++;
    parley_wnd_ptr->calc_rect();
    ((WndBase *)((u8 *)parley_wnd_ptr + 0x48))->move_to_rect(parley_wnd_ptr->init_rect);
    parley_wnd_ptr->SurfaceWindow::show();
  }
  return;
}

/* 004b7645 auto */
/* WARNING: Removing unreachable block (ram,0x004b766b) */

void __cdecl parley_wnd_close(void)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b768d auto */
/* WARNING: Removing unreachable block (ram,0x004b76b3) */

void __cdecl chat_wnd_close(void)

{
  wnd_set_active_child((iptr)&chat_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b7885 auto */
void __cdecl parley_wnd_redraw(void)

{
  iptr child_wnd;
  i32 caret_pos;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    wnd2->calc_layout();
    wnd2->build_title();
    gfx_set_draw_target((iptr)wnd2);
    wnd2->draw_frame();
    wnd2->sync_sides();
    wnd2->wnd_blit_surface();
    wnd2->SurfaceWindow::show();
    caret_pos = ((EditControl *)wnd2->chat_log_edit)->get_text_end();
    ((EditControl *)wnd2->chat_log_edit)->set_caret(caret_pos);
    caret_pos = ((EditControl *)wnd2->chat_input_edit)->get_sel_end();
    ((EditControl *)wnd2->chat_input_edit)->set_caret(caret_pos);
  }
  return;
}

/* 004b794a auto */
void ParleyWindow::draw_frame()

{
  int free_width;
  int title_width;
  i8 sprite_rect[16];
  int icon_height;
  int icon_width;
  int y2;
  int x2;
  GameFont *title_font2;
  int i;
  tagRECT rect;
  
  reset_clip();
  crt_memcpy(&rect,&client_left,0x10);
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)this,&rect,(uint)dlg_style.bevel_light_color,(uint)dlg_style.bevel_dark_color);
    rect_deflate((iptr)&rect,1,1);
  }
  fill_background(&rect,1);
  rect_deflate((iptr)&rect,panel_layout.padding_x + panel_layout.bevel_width * -2,0);
  rect.top = rect.top + caption_height + panel_layout.bevel_width * -2;
  free_width = rect_height((RECT *)&client_left);
  rect.bottom = (free_width - caption_height) + panel_layout.bevel_width * -2;
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)this,&rect,(uint)dlg_style.bevel_dark_color,(uint)dlg_style.bevel_light_color);
    rect_deflate((iptr)&rect,1,1);
  }
  fill_background(&rect,2);
  x2 = panel_layout.padding_x;
  for (i = 0; i < 3; i++) {
    icon_width = (int)(caption_buttons[i].width * caption_height) / 0x18;
    icon_height = (int)(caption_buttons[i].height * caption_height) / 0x18;
    y2 = ((int)caption_height >> 1) - (icon_height >> 1);
    SetRect(&rect,x2,y2,icon_width + x2,icon_height + y2);
    gfx_set_scale_ratio(caption_height,0x18);
    caption_buttons[i].sprite->draw((iptr)sprite_rect,
               (iptr)this,x2,y2);
    gfx_reset_zoom_scale();
    x2 = x2 + panel_layout.margin_x + icon_width;
  }
  if (large_layout == 0) {
    title_font2 = (GameFont *)((i8 *)&font_size20);
  }
  else {
    title_font2 = (GameFont *)((i8 *)&font_size30);
  }
  x2 -= panel_layout.margin_x;
  free_width = (client_right - x2) - panel_layout.padding_x;
  title_width = title_font2->text_width(title);
  x2 += (free_width / 2 - (title_width >> 1));
  y2 = panel_layout.bevel_width + panel_layout.margin_y;
  gfx_set_text_color_index(10);
  draw_text((iptr)title_font2,title,x2 + 2,y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  draw_text((iptr)title_font2,title,x2 + 1,y2,5);
  draw_text((iptr)title_font2,title,x2,y2,5);
  set_clip((RECT *)&panel_left);
  return;
}

/* 004b7c90 auto */
void ParleyWindow::fill_background(RECT *rect,int area_kind)

{
  i32 height;
  i32 width;
  i32 tile_x;
  i32 tile_y;
  
  if ((dlg_style.frame_tile_surface == 0) || (area_kind != 1)) {
    if ((dlg_style.client_tile_surface == 0) || (area_kind != 2)) {
      gfx_fill_rect((iptr)this,rect,(uint)dlg_style.bg_color);
    }
    else {
      tile_y = 0;
      tile_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill((iptr)this,dlg_style.client_tile_surface,rect->left,rect->top
                         ,width,height,tile_x,tile_y);
    }
  }
  else {
    tile_y = 0;
    tile_x = 0;
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_tile_fill((iptr)this,dlg_style.frame_tile_surface,rect->left,rect->top,
                       width,height,tile_x,tile_y);
  }
  return;
}

/* 004b7d72 auto */
void ParleyWindow::build_title()

{
  const char *text;
  iptr attitude_level;
  
  text_clear();
  if (mode == 4) {
    text = (label_string(label_table.string_ids[0x2d5]));
    text_append(text);
    crt_strcpy((char *)((u8 *)this + 0x15c),((i8 *)&textbuf.buf));
  }
  else {
    text = (label_string(label_table.string_ids[0x2d4]));
    text_append(text);
    text_append_space();
    text = (civ_name_plural(other_civ));
    text_append(text);
    text_append_space();
    text_append_lparen();
    if (*(char *)&civs[other_civ].attitude[human_civ[0]] == '\0') {
      attitude_level = 0;
    }
    else {
      attitude_level = diplo_attitude_level((uint)civs[other_civ].attitude[human_civ[0]]);
    }
    text_append_string_id(attitude_names[attitude_level]);
    text_append_rparen();
    crt_strcpy((char *)((u8 *)this + 0x15c),((i8 *)&textbuf.buf));
  }
  return;
}

/* 004b7eb6 auto */
/* WARNING: Removing unreachable block (ram,0x004b8008) */
/* WARNING: Removing unreachable block (ram,0x004b8051) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::open(iptr other_civ,u32 mode)

{
  bool can_see_all;
  bool has_contact;
  iptr has_wonder;
  iptr child_wnd;
  i32 caret_pos;
  int civ;
  ParleyWindow *wnd2;
  
  if (mode == 4) {
    has_contact = false;
    if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0)
        ) && (game.reveal_map == '\0')) {
      can_see_all = false;
    }
    else {
      can_see_all = true;
    }
    for (civ = 1; civ < 8; civ++) {
      if ((((human_civ[0] != civ) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (((can_see_all || (mp_options.chat_from_start != 0)) ||
          ((*(byte *)&civs[civ].treaties[human_civ[0]] & TREATY_CONTACT) != 0)))) {
        has_contact = true;
        break;
      }
    }
    if (!has_contact) {
      popup_game((const char *)&str_noforeignhuman);
      return;
    }
    wnd_set_active_child((iptr)&chat_wnd_block.window);
    net.chat_unread_count = 0;
    net.chat_missed_message_count = 0;
  }
  else {
    wnd_set_active_child((iptr)&parley_wnd_block.window);
    parley_wnd_block.selected_action = 0xffffffff;
    remote_units_revealed = '\0';
    local_units_revealed = '\0';
    local_cities_revealed = '\0';
    remote_cities_revealed = '\0';
    if (mode == 3) {
      parley_wnd_block.set_mode_sides(0);
    }
  }
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    wnd2->SurfaceWindow::show();
  }
  else {
    ((u8 *)wnd2 + 0x0)[0x114] = '\0';
    ((u8 *)wnd2 + 0x0)[0x115] = '\0';
    ((u8 *)wnd2 + 0x0)[0x116] = '\0';
    ((u8 *)wnd2 + 0x0)[0x117] = '\0';
    wnd2->other_civ = other_civ;
    if (mode != 3) {
      wnd2->mode = mode;
      wnd2->left_kind = (((i32 *)&parley_panes.mode_left_kinds))[mode * 2];
      wnd2->right_kind = (((i32 *)&parley_panes.mode_right_kinds))[mode * 2];
    }
    wnd2->build_title();
    ((u8 *)wnd2 + 0x1f8)[0] = 21;
    ((u8 *)wnd2 + 0x1f9)[0] = '\0';
    ((u8 *)wnd2 + 0x1fa)[0] = '\0';
    ((u8 *)wnd2 + 0x1fb)[0] = '\0';
    ((u8 *)wnd2 + 0x1fc)[0] = 21;
    ((u8 *)wnd2 + 0x1fd)[0] = '\0';
    ((u8 *)wnd2 + 0x1fe)[0] = '\0';
    ((u8 *)wnd2 + 0x1ff)[0] = '\0';
    wnd2->repaint();
    wnd2->SurfaceWindow::show();
    caret_pos = ((EditControl *)wnd2->chat_log_edit)->get_text_end();
    ((EditControl *)wnd2->chat_log_edit)->set_caret(caret_pos);
    caret_pos = ((EditControl *)wnd2->chat_input_edit)->get_sel_end();
    ((EditControl *)wnd2->chat_input_edit)->set_caret(caret_pos);
  }
  snd_play_effect(((uint)map.seed + other_civ & 7) + 0x53,1,0,0);
  return;
}

/* 004b81dd auto */
/* WARNING: Removing unreachable block (ram,0x004b8211) */

void __cdecl parley_poll_net_replies(void)

{
  const char *civ_name;
  
  net.process_messages(1,0);
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((net.nm_parley_transaction_count == 0) || ((int)parley_wnd_block.mode != 2)) {
    if ((net.nm_parley_accept_offer_count == 0) || ((int)parley_wnd_block.mode != 1)) {
      if ((net.nm_parley_no_thanks_count == 0) || ((int)parley_wnd_block.mode != 1)) {
        if ((net.nm_parley_counter_offer_count != 0) && ((int)parley_wnd_block.mode == 1)) {
          net.nm_parley_counter_offer_count = 0;
          parley_wnd_block.mode = 2;
          parley_wnd_block.left_kind = parley_panes.mode2_left_kind;
          parley_wnd_block.right_kind = parley_panes.mode2_right_kind;
          parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
          parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
          parley_wnd_block.add_side_dialog(parley_panes.mode2_left_kind,0);
          parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
          civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
          text_set_string_arg(0,civ_name);
          dlg_style.pending_idle_callback = parley_poll_net_replies;
          popup_game_with_sprite((const char *)&str_parleycounteroffer,(iptr)&popup_info_icon_sprite,0);
        }
      }
      else {
        net.nm_parley_no_thanks_count = 0;
        parley_wnd_block.mode = 3;
        parley_wnd_block.selected_action = 0xffffffff;
        parley_wnd_block.set_mode_sides(1);
        civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
        text_set_string_arg(0,civ_name);
        dlg_style.pending_idle_callback = parley_poll_net_replies;
        popup_game_with_sprite((const char *)&str_parleynothanks,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else {
      net.nm_parley_accept_offer_count = 0;
      if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
        parley_wnd_block.received_offer->hdr.msg_type = NM_PARLEY_EXECUTE;
        if (net.host_flag == '\0') {
          net_send_message(NM_PARLEY_EXECUTE,0,0,0,0,0,0,0,0,(iptr)(void *)parley_wnd_block.received_offer);
        }
        else {
          parley_execute_transaction(parley_wnd_block.received_offer);
        }
        operator_delete((void *)parley_wnd_block.received_offer);
        parley_wnd_block.received_offer = 0;
      }
      parley_wnd_block.mode = 2;
      parley_wnd_block.left_kind = parley_panes.mode2_left_kind;
      parley_wnd_block.right_kind = parley_panes.mode2_right_kind;
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_wnd_block.add_side_dialog(parley_panes.mode2_left_kind,0);
      parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
      dlg_style.pending_idle_callback = parley_poll_net_replies;
      popup_game_with_sprite((const char *)&str_parleyaccept,(iptr)&popup_info_icon_sprite,0);
    }
  }
  else {
    net.nm_parley_transaction_count = 0;
    if ((parley_wnd_block.received_offer->mode == 6) && (parley_wnd_block.received_offer->item.value == 3)) {
      civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
      text_set_string_arg(0,civ_name);
      civ_name = (civ_name_plural(human_civ[0]));
      text_set_string_arg(1,civ_name);
      if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_PEACE) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_CEASEFIRE) != 0) {
            diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,2);
            popup_game((const char *)&str_breakcease);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,4);
          popup_game((const char *)&str_cancelpeace);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,8);
        popup_game((const char *)&str_cancelallied);
      }
    }
    else {
      parley_wnd_block.mode = 0;
      parley_wnd_block.left_kind = parley_panes.mode_left_kinds[0];
      parley_wnd_block.right_kind = parley_panes.mode_right_kinds[0];
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_wnd_block.add_side_dialog(parley_panes.mode_left_kinds[0],0);
      parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
    }
  }
  return;
}

/* 004b8676 auto */
/* WARNING: Removing unreachable block (ram,0x004b869f) */

void ParleyWindow::set_mode_sides(int refresh)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((i32)parley_wnd_block.selected_action == -1) {
    parley_wnd_block.mode = 3;
    ((Button *)parley_wnd_block.never_mind_button)->disable();
  }
  else if (parley_wnd_block.selected_action == 0) {
    parley_wnd_block.mode = 6;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 1) {
    parley_wnd_block.mode = 0xc;
    if (parley_wnd_block.proposal_kind != 0) {
      parley_wnd_block.mode = 0xd;
    }
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 2) {
    if (parley_wnd_block.gift_kind == 0) {
      parley_wnd_block.mode = 7;
    }
    else if ((int)parley_wnd_block.gift_kind == 1) {
      parley_wnd_block.mode = 8;
    }
    else if ((int)parley_wnd_block.gift_kind == 2) {
      parley_wnd_block.mode = 9;
    }
    else if ((int)parley_wnd_block.gift_kind == 3) {
      parley_wnd_block.mode = 10;
    }
    else {
      parley_wnd_block.mode = 0xb;
    }
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 3) {
    parley_wnd_block.mode = 5;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 4) {
    parley_wnd_block.mode = 0xf;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  parley_wnd_block.left_kind = (((i32 *)&parley_panes.mode_left_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.right_kind = (((i32 *)&parley_panes.mode_right_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
  parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
  if (refresh != 0) {
    parley_wnd_block.add_side_dialog((i32)parley_wnd_block.left_kind,0);
    parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
  }
  return;
}

/* 004b888e auto */
void ParleyWindow::clear_side_controls(int side)

{
  u8 *slot_ptr;
  int half_width;
  int i;
  
  if ((side != 0) && (side != 1)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_leftright_left_leftright_right,(iptr)&str_d_ss_franklinton_parleywin_cpp,0x4ea);
  }
  if (side == 1) {
    if (desc_edit != 0) {
      if ((Button *)desc_edit != (Button *)0x0) {
        ((EditControl *)desc_edit)->scalar_destruct(1);
      }
      desc_edit = 0;
    }
    if (anonymous_checkbox != 0) {
      if ((Checkbox *)anonymous_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)anonymous_checkbox)->scalar_destruct(1);
      }
      anonymous_checkbox = 0;
    }
    if (mute_anonymous_checkbox != 0) {
      if ((Checkbox *)mute_anonymous_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)mute_anonymous_checkbox)->scalar_destruct(1);
      }
      mute_anonymous_checkbox = 0;
    }
    if (broadcast_checkbox != 0) {
      if ((Checkbox *)broadcast_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)broadcast_checkbox)->scalar_destruct(1);
      }
      broadcast_checkbox = 0;
    }
    for (i = 0; i < 8; i++) {
      if (send_checkboxes[i] != 0) {
        if ((Checkbox *)send_checkboxes[i] != (Checkbox *)0x0) {
          ((Checkbox *)send_checkboxes[i])->scalar_destruct(1);
        }
        slot_ptr = (u8 *)&send_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
      if (mute_checkboxes[i] != 0) {
        if ((Checkbox *)mute_checkboxes[i] != (Checkbox *)0x0) {
          ((Checkbox *)mute_checkboxes[i])->scalar_destruct(1);
        }
        slot_ptr = (u8 *)&mute_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
    }
  }
  if (side_checkboxes[side] != 0) {
    if ((Checkbox *)side_checkboxes[side] != (Checkbox *)0x0) {
      ((Checkbox *)side_checkboxes[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&side_checkboxes[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (side_radios[side] != 0) {
    if ((RadioGroup *)side_radios[side] != (RadioGroup *)0x0) {
      ((RadioGroup *)side_radios[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&side_radios[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (gold_edits[side] != 0) {
    if ((Button *)gold_edits[side] != (Button *)0x0) {
      ((EditControl *)gold_edits[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&gold_edits[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (list_scrollbars[side] != 0) {
    if ((Button *)list_scrollbars[side] != (Button *)0x0) {
      ((Scrollbar *)list_scrollbars[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&list_scrollbars[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (list_hotspots[side] != 0) {
    if ((Hotspot *)list_hotspots[side] != (Hotspot *)0x0) {
      ((Hotspot *)list_hotspots[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&list_hotspots[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (sort_by_unit_buttons[side] != 0) {
    if ((Button *)sort_by_unit_buttons[side] != (Button *)0x0) {
      ((Button *)sort_by_unit_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&sort_by_unit_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (sort_by_city_buttons[side] != 0) {
    if ((Button *)sort_by_city_buttons[side] != (Button *)0x0) {
      ((Button *)sort_by_city_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&sort_by_city_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (info_show_buttons[side] != 0) {
    if ((Button *)info_show_buttons[side] != (Button *)0x0) {
      ((Button *)info_show_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&info_show_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (info_hide_buttons[side] != 0) {
    if ((Button *)info_hide_buttons[side] != (Button *)0x0) {
      ((Button *)info_hide_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&info_hide_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  fill_left = panel_left;
  fill_top = panel_top;
  fill_right = panel_right;
  fill_bottom = panel_bottom;
  fill_bottom = divider_y;
  if (side == 0) {
    half_width = rect_width((RECT *)&panel_left);
    fill_right = half_width / 2;
  }
  else {
    half_width = rect_width((RECT *)&panel_left);
    fill_left = half_width / 2;
  }
  fill_background((RECT *)&fill_left,2);
  return;
}

/* 004b8e5c auto */
void __cdecl parley_gold_edit_on_change(int control_id)

{
  char *src_ptr;
  size_t len;
  int amount;
  size_t text_len;
  uptr i;
  char gold_text[32];
  uint edit_index;
  i32 modified;
  char text[32];
  uint caret;
  
  modified = 0;
  _memset(text,0,0x20);
  _memset(gold_text,0,0x20);
  edit_index = (uint)(control_id != 0x3f8);
  caret = ((EditControl *)parley_wnd_block.gold_edits[edit_index])->get_sel_end();
  ((Control *)parley_wnd_block.gold_edits[edit_index])->Control::get_text((char *)text);
  __itoa(civs[human_civ[0]].gold,gold_text,10);
  for (i = 0; len = _strlen(text), i < len; i++) {
    if ((text[i] < '0') || ('9' < text[i])) {
      modified = 1;
      crt_strcpy(text + i,text + i + 1);
    }
  }
  amount = _atoi(text);
  if ((((int)parley_wnd_block.mode == 0xe) && (edit_index == 0)) || (((int)parley_wnd_block.mode != 0xf && ((int)parley_wnd_block.mode != 0xe)))
     ) {
    len = _strlen(gold_text);
    text_len = _strlen(text);
    if ((len < text_len) || (civs[human_civ[0]].gold < amount)) {
      modified = 1;
      src_ptr = text + caret;
      amount = caret - 1;
      caret--;
      crt_strcpy(text + amount,src_ptr);
    }
  }
  amount = _atoi(text);
  if (amount < 1) {
    parley_wnd_block.side_selected[edit_index] = 0;
  }
  else {
    parley_wnd_block.side_selected[edit_index] = 1;
  }
  parley_wnd_block.update_buttons();
  if (modified != 0) {
    len = _strlen(text);
    if (len < caret) {
      caret = _strlen(text);
    }
    ((Control *)parley_wnd_block.gold_edits[edit_index])->set_text((const char *)text);
    ((EditControl *)parley_wnd_block.gold_edits[edit_index])->set_caret(caret);
  }
  return;
}

/* 004b90ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::chat_receive_message(iptr civ,iptr civ2,iptr is_private,iptr message)

{
  u32 muted;
  size_t log_len;
  const void *label;
  int listener;
  
  ((Control *)chat_wnd_block.chat_log_edit)->Control::get_text(chat.log_buffer);
  text_clear();
  log_len = _strlen(chat.log_buffer);
  if (log_len != 0) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_newline_str));
  }
  if ((char)is_private == '\0') {
    label = (label_string(label_table.string_ids[0x2d8]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_year_close_str));
  }
  else {
    label = (label_string(label_table.string_ids[0x2d6]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    label = (label_string(label_table.string_ids[0x2d7]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ2);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_private_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_private_year_close_str));
  }
  crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)message));
  if ((civ == 0) && (chat_wnd_block.mute_anonymous != 0)) {
    return;
  }
  /* a public message: dropped if its sender is muted, unless the parley with him is open */
  if ((char)is_private == '\0') {
    if ((parley_wnd_block.closed != 0) || ((int)parley_wnd_block.other_civ != civ)) {
      muted = muted_civs[civ];
      if (muted != 0) {
        return;
      }
    }
  }
  for (listener = 1; listener < 8; listener++) {
    if ((((listener != civ) && (human_civ[0] != listener)) &&
        ((1 << ((byte)listener & 0x1f) & (uint)game.human_players) != 0)) &&
       ((*(byte *)(listener * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)) {
      net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[listener]].id,
                         civ,civ2,1,0,0,0,(const char *)message,0);
    }
  }
  chat_log_append((iptr)&textbuf.buf);
  if (chat_wnd_block.closed != 0) {
    net.chat_unread_count++;
    net.chat_missed_message_count++;
  }
  if ((parley_wnd_block.closed != 0) && (chat_wnd_block.closed != 0)) {
    chat_wnd_block.open(0,4);
  }
  return;
}

/* 004b93a2 auto */
void __cdecl chat_log_append(iptr line)

{
  int capacity;
  size_t log_len;
  size_t line_len;
  i32 caret_pos;
  uptr trim_len;
  
  capacity = font_size16_bold_block.chat_log_capacity;
  trim_len = 0;
  log_len = _strlen(chat.log_buffer);
  line_len = _strlen((char *)line);
  if (capacity - (log_len + 1) < line_len + 1) {
    trim_len = _strlen((char *)line);
    do {
      trim_len++;
      if ((chat.log_buffer[trim_len] == '\r') || (chat.log_buffer[trim_len] == '\n')) break;
    } while (chat.log_buffer[trim_len] != '\0');
    if (chat.log_buffer[trim_len] != '\0') {
      for (; (chat.log_buffer[trim_len] == '\r' || (chat.log_buffer[trim_len] == '\n'));
          trim_len = trim_len + 1) {
      }
    }
  }
  crt_strcpy(chat.log_buffer,chat.log_buffer + trim_len);
  crt_strcat(chat.log_buffer,(const char *)((const void *)line));
  ((Control *)chat_wnd_block.chat_log_edit)->set_text((const char *)chat.log_buffer);
  ((Control *)parley_wnd_block.chat_log_edit)->set_text((const char *)chat.log_buffer);
  caret_pos = ((EditControl *)chat_wnd_block.chat_log_edit)->get_text_end();
  ((EditControl *)chat_wnd_block.chat_log_edit)->set_caret(caret_pos);
  caret_pos = ((EditControl *)chat_wnd_block.chat_log_edit)->get_text_end();
  ((EditControl *)parley_wnd_block.chat_log_edit)->set_caret(caret_pos);
  return;
}

/* 004b9504 auto */
void __cdecl chat_append_sender_name(int civ)

{
  i32 name_text;
  
  if (civ == 0) {
    name_text = (i32)(label_string(label_table.string_ids[0x2d9]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
  }
  else {
    name_text = civ_leader_title(civ);
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
    text_append_space();
    name_text = (i32)(civ_leader_name(civ));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
  }
  return;
}

/* 004b957e auto */
void __cdecl parley_wnd_on_key(int key)

{
  if (key == 0xd2) {
    diplo_wnd_escape_close(0);
  }
  return;
}

/* 004b95c2 auto */
void __cdecl chat_send_button_on_click(void)

{
  chat_input_on_key(0,0xd0);
  return;
}

/* 004b95e1 auto */
void __cdecl chat_clear_button_on_click(void)

{
  _memset((*(void * *)&chat.log_buffer),0,font_size16_bold_block.chat_log_capacity + 1);
  ((Control *)chat_wnd_block.chat_log_edit)->set_text((const char *)(*(void * *)&chat.log_buffer));
  ((Control *)parley_wnd_block.chat_log_edit)->set_text((const char *)(*(void * *)&chat.log_buffer));
  ((Control *)parley_wnd_block.chat_input_edit)->set_focus();
  return;
}

/* 004b9635 auto */
i32 __cdecl parley_edit_escape_filter(i32 control,int key)

{
  i32 result;
  
  if ((key == 0x1b) || (key == 0xd2)) {
    diplo_wnd_escape_close(0);
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 004b968a auto */
i32 __cdecl chat_input_on_key(i32 control,int key)

{
  iptr child_wnd;
  size_t len;
  const void *label;
  iptr relay_sender;
  iptr sender;
  int count;
  int civ;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (key < 0x2b5) {
    if (0x2af < key) {
      chat_insert_macro_text((EditControl *)wnd2->chat_input_edit,key);
      return 0;
    }
    if (key == 0xd0) {
      ((Control *)wnd2->chat_input_edit)->Control::get_text((*(char * *)&chat.input_buffer));
      len = _strlen((*(char * *)&chat.input_buffer));
      if (len == 0) {
        return 0;
      }
      ((Control *)chat_wnd_block.chat_log_edit)->Control::get_text(chat.log_buffer);
      text_clear();
      len = _strlen(chat.log_buffer);
      if (len != 0) {
        crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_newline_str));
      }
      label = (label_string(label_table.string_ids[0x2da]));
      crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
      text_append_space();
      if ((int)wnd2->mode == 4) {
        count = 0;
        for (civ = 1; civ < 8; civ++) {
          if (chat_wnd_block.send_to_civs[civ] != 0) {
            if (count != 0) {
              crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_recipient_sep_str));
            }
            chat_append_sender_name(civ);
            count++;
          }
        }
      }
      else {
        chat_append_sender_name((i32)wnd2->other_civ);
        count = 1;
      }
      if (count == 0) {
        return 0;
      }
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_year_open_str));
      text_append_year((int)game.year);
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_year_close_str));
      crt_strcat(((i8 *)&textbuf.buf),(*(char * *)&chat.input_buffer));
      chat_log_append((iptr)&textbuf.buf);
      if ((int)wnd2->mode == 4) {
        for (civ = 1; civ < 8; civ++) {
          if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != civ)) && (chat_wnd_block.send_to_civs[civ] != 0)) {
            if (chat_wnd_block.remain_anonymous == 0) {
              sender = human_civ[0];
            }
            else {
              sender = 0;
            }
            net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[civ]].id,
                               sender,civ,0,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
            for (count = 1; count < 8; count++) {
              if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
                  (human_civ[0] != count)) &&
                 ((count != civ &&
                  ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
                if (chat_wnd_block.remain_anonymous == 0) {
                  relay_sender = human_civ[0];
                }
                else {
                  relay_sender = 0;
                }
                net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[count]].id,relay_sender,
                                   civ,1,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
              }
            }
          }
        }
      }
      else {
        net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[(int)wnd2->other_civ]].id,
                           human_civ[0],(iptr)wnd2->other_civ,0,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
        for (count = 1; count < 8; count++) {
          if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != count)) &&
             (((int)wnd2->other_civ != count &&
              ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
            net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[count]].id
                               ,human_civ[0],(iptr)wnd2->other_civ,1,0,0,0,(const char *)(*(char * *)&chat.input_buffer),
                               0);
          }
        }
      }
      *(*(char * *)&chat.input_buffer) = '\0';
      ((Control *)wnd2->chat_input_edit)->set_text((const char *)(*(char * *)&chat.input_buffer));
      ((Control *)wnd2->chat_input_edit)->set_focus();
      return 0;
    }
    if (key == 0xd2) {
      diplo_wnd_escape_close(0);
      return 0;
    }
  }
  return 1;
}

/* 004db690 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 *ParleyWindow::send_offer(int mode)

{
  i32 offer_kind;
  i32 *msg;
  int left_items;
  int i;
  uint total_size;
  int right_size;
  int left_size;
  
  offer_kind = parley_offer_kinds[mode];
  parley_wnd_block.offer_counts[0] = 0;
  parley_wnd_block.offer_counts[1] = 0;
  right_size = parley_item_header_sizes[side_kind[1]];
  switch(side_kind[1]) {
  case 8:
  case 0xc:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = offer_counts[1] * 4 + right_size;
    break;
  case 10:
  case 0x11:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = offer_counts[1] * 4 + right_size;
    if (side_kind[1] == 0x11) {
      right_size = offer_counts[1] * 4 + right_size;
    }
  }
  total_size = right_size + 0x20;
  if (mode == 0xe) {
    left_size = parley_item_header_sizes[side_kind[0]];
    switch(side_kind[0]) {
    case 8:
    case 0xc:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = offer_counts[0] * 4 + left_size;
      break;
    case 10:
    case 0x11:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = offer_counts[0] * 4 + left_size;
      if (side_kind[0] == 0x11) {
        left_size = offer_counts[0] * 4 + left_size;
      }
    }
    total_size += left_size;
  }
  msg = (i32 *)(operator_new(total_size));
  if (msg == (i32 *)0x0) {
    dbg_log((const char *)&str_failed_to_allocate_buffer_for_parley_tra);
    msg = (i32 *)0x0;
  }
  else {
    *msg = 0x66606660;
    msg[1] = 0x82;
    msg[2] = total_size;
    msg[4] = this->mode;
    msg[5] = human_civ[0];
    msg[6] = other_civ;
    msg[7] = offer_kind;
    if (total_size + (int)msg < (uint)(right_size + (int)(msg + 8))) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_charptr_pchunk_sizechunk0_charptr_pmessa,(iptr)&str_d_ss_franklinton_parleywin_transaction_c,0x9f);
    }
    pack_offer_items((ParleyOfferItem *)(iptr)(msg + 8),side_kind[1],right_size,1);
    if (mode == 0xe) {
      left_items = (int)msg + right_size + 0x20;
      if (left_size + left_items != total_size + (int)msg) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_charptr_pchunk_sizechunk1_charptr_pmessa,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_2,0xa4);
      }
      pack_offer_items((ParleyOfferItem *)left_items,side_kind[0],left_size,0);
    }
    net_send_message(NM_PARLEY_TRANSACTION,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0
                       ,0,0,0,0,0,(iptr)msg);
  }
  return msg;
}

/* 004dbab4 auto */
void ParleyWindow::pack_offer_items(ParleyOfferItem *dest,i32 item_type,i32 item_size,iptr side)

{
  int gold;
  i32 *item_ptr2;
  int packed_count;
  int i;
  i32 *pair_ptr;
  char gold_text[32];
  
  switch(item_type) {
  case 5:
  case 6:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = treaty_kind;
    break;
  case 8:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    item_ptr2 = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 9:
    dest->type = item_type;
    dest->size = item_size;
    ((Control *)parley_wnd_block.gold_edits[side])->Control::get_text((char *)gold_text);
    gold = _atoi(gold_text);
    dest->value = gold;
    break;
  case 10:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    pair_ptr = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)list_items_b[side * 0x801 + i];
        pair_ptr++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xc:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    item_ptr2 = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xd:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)map_choice[side];
    break;
  case 0x11:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    pair_ptr = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)list_items_b[side * 0x801 + i];
        pair_ptr[1] = (i32)list_selected_b[side * 0x801 + i];
        pair_ptr += 2;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
  }
  return;
}

/* Part of ParleyWindow::build_description (004dbee6): an exchange (mode 0xe):
   the item given (the second item, the GIVE texts), by kind. */
void ParleyWindow::describe_barter_give(ParleyOfferMsg *second_offer,i32 *second_items)
{
  iptr label;
  
  switch(*second_items) {
  case 5:
  case 6:
    label = (iptr)(label_string(label_table.string_ids[0x347]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_treaty_crlf_str));
    describe_treaty(second_offer->item.value,1);
    break;
  case 8:
    label = (iptr)(label_string(label_table.string_ids[0x32e]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_adv_crlf_str));
    parley_describe_advances((ParleyOfferItem *)second_items);
    break;
  case 9:
    label = (iptr)(label_string(label_table.string_ids[0x32f]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_gold_crlf_str));
    str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_140);
    parley_describe_gold(second_offer->item.value);
    break;
  case 10:
    label = (iptr)(label_string(label_table.string_ids[0x330]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_units_crlf_str));
    parley_describe_units((ParleyOfferItem *)second_items);
    break;
  case 0xc:
    label = (iptr)(label_string(label_table.string_ids[0x333]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_war_crlf_str));
    parley_describe_war_targets((ParleyOfferItem *)second_items);
    break;
  case 0xd:
    label = (iptr)(label_string(label_table.string_ids[0x332]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_maps_crlf_str));
    parley_describe_maps(second_offer->item.value,0);
    break;
  case 0x11:
    label = (iptr)(label_string(label_table.string_ids[0x331]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_cities_crlf_str));
    parley_describe_cities((ParleyOfferItem *)second_items);
  }
}

/* Part of ParleyWindow::build_description (004dbee6): an exchange (mode 0xe):
   the item wanted for it (the first item, the WANT/FOR texts), by kind. */
void ParleyWindow::describe_barter_want(ParleyOfferMsg *offer,int *first_items)
{
  iptr label;
  
  switch(*first_items) {
  case 5:
  case 6:
    label = (iptr)(label_string(label_table.string_ids[0x348]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_treaty_crlf_str));
    describe_treaty(offer->item.value,1);
    break;
  case 8:
    label = (iptr)(label_string(label_table.string_ids[0x334]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_adv_crlf_str));
    parley_describe_advances((ParleyOfferItem *)first_items);
    break;
  case 9:
    label = (iptr)(label_string(label_table.string_ids[0x335]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_gold_crlf_str));
    str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_141);
    parley_describe_gold(offer->item.value);
    break;
  case 10:
    label = (iptr)(label_string(label_table.string_ids[0x336]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_units_crlf_str));
    parley_describe_units((ParleyOfferItem *)first_items);
    break;
  case 0xc:
    label = (iptr)(label_string(label_table.string_ids[0x339]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_war_targets_crlf_str));
    parley_describe_war_targets((ParleyOfferItem *)first_items);
    break;
  case 0xd:
    label = (iptr)(label_string(label_table.string_ids[0x338]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_maps_crlf_str));
    parley_describe_maps(offer->item.value,1);
    break;
  case 0x11:
    label = (iptr)(label_string(label_table.string_ids[0x337]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_cities_crlf_str));
    parley_describe_cities((ParleyOfferItem *)first_items);
  }
}

/* Part of ParleyWindow::build_description (004dbee6): a demand (mode 0xf):
   its title by kind, then what is demanded. */
void ParleyWindow::describe_threat(ParleyOfferMsg *offer,int *first_items)
{
  iptr label;
  
  label = (iptr)(label_string(label_table.string_ids[0x32b]));
  str_concat((iptr)&text_section_index_pool_block.parley_description,label);
  str_append_space((iptr)&text_section_index_pool_block.parley_description);
  if (*first_items == 0xc) {
    label = (iptr)(label_string(label_table.string_ids[0x346]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
  }
  else if ((*first_items == 5) || (*first_items == 6)) {
    label = (iptr)(label_string(label_table.string_ids[0x345]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x32c]));
    str_concat((iptr)&text_section_index_pool_block.parley_description,label);
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_threat_crlf_str));
  switch(*first_items) {
  case 5:
  case 6:
    describe_treaty(offer->item.value,1);
    break;
  case 8:
    parley_describe_advances((ParleyOfferItem *)first_items);
    break;
  case 9:
    parley_describe_gold(offer->item.value);
    break;
  case 10:
    parley_describe_units((ParleyOfferItem *)first_items);
    break;
  case 0xc:
    parley_describe_war_targets((ParleyOfferItem *)first_items);
    break;
  case 0xd:
    parley_describe_maps(offer->item.value,1);
    break;
  case 0x11:
    parley_describe_cities((ParleyOfferItem *)first_items);
  }
}

/* 004dbee6 auto */
void ParleyWindow::build_description(ParleyOfferMsg *offer,int received)

{
  int maps_kind;
  int *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  size_t desc_len;
  iptr label;
  
  if (offer != 0) {
    first_items = (int *)((iptr)offer + 0x20);
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)((iptr)second_offer + 0x20);
    str_clear((char *)&text_section_index_pool_block.parley_description);
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (desc_len != 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_strlen_parleydescription_0,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_3,0x120);
    }
    if (received == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x31c]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_transmitted_crlf_str));
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_strlen_parleydescription_2048,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_4,0x126);
    }
    switch(offer->mode) {
    case 6:
      describe_treaty(offer->item.value,0);
      break;
    case 7:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_adv_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x327]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_adv_head_crlf_str));
      parley_describe_advances((ParleyOfferItem *)first_items);
      break;
    case 8:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_gold_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x329]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_gold_head_crlf_str));
      str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_139);
      parley_describe_gold(offer->item.value);
      break;
    case 9:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_units_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x328]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_units_head_crlf_str));
      parley_describe_units((ParleyOfferItem *)first_items);
      break;
    case 10:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_cities_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x32a]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_cities_head_crlf_str));
      parley_describe_cities((ParleyOfferItem *)first_items);
      break;
    case 0xb:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_maps_crlf_str));
      parley_describe_maps(offer->item.value,0);
      break;
    case 0xc:
      label = (iptr)(label_string(label_table.string_ids[0x321]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_war_offer_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x325]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_war_head_crlf_str));
      parley_describe_war_targets((ParleyOfferItem *)first_items);
      break;
    case 0xd:
      label = (iptr)(label_string(label_table.string_ids[0x321]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_maps_offer_crlf_str));
      maps_kind = offer->item.value;
      if (maps_kind == 0) {
        label = (iptr)(label_string(label_table.string_ids[0x322]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else if (maps_kind == 1) {
        label = (iptr)(label_string(label_table.string_ids[0x323]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else if (maps_kind == 2) {
        label = (iptr)(label_string(label_table.string_ids[0x324]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_maps_kind_crlf_str));
      break;
    case 0xe:
      label = (iptr)(label_string(label_table.string_ids[0x32d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_barter_crlf_str));
      describe_barter_give(second_offer,second_items);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_barter_sep_crlf_str));
      describe_barter_want(offer,first_items);
      break;
    case 0xf:
      describe_threat(offer,first_items);
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_strlen_parleydescription_2048_2,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_5,0x1f1);
    }
  }
  return;
}

/* 004dcafa auto */
void __cdecl parley_describe_advances(ParleyOfferItem *items)

{
  int count;
  iptr adv_name;
  int i;
  int *adv_ptr;
  
  count = items->value;
  adv_ptr = (int *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    adv_name = (iptr)(label_string(advances[*adv_ptr].name));
    str_concat((iptr)&text_section_index_pool_block.parley_description,adv_name);
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        adv_name = (iptr)(label_string(label_table.string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,adv_name);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_period_str));
      }
    }
    adv_ptr++;
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_crlf_str));
  return;
}

/* 004dcc0c auto */
void __cdecl parley_describe_gold(int gold)

{
  const void *label;
  char gold_text[16];
  
  __itoa(gold,gold_text,10);
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),gold_text);
  str_append_space((iptr)&text_section_index_pool_block.parley_description);
  label = (label_string(label_table.string_ids[0x33b]));
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),(const char *)label);
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_gold_crlf_str));
  return;
}

/* 004dcc83 auto */
void __cdecl parley_describe_units(ParleyOfferItem *items)

{
  iptr unit;
  iptr label;
  int type_counts[62];
  int listed;
  iptr unit_ids;
  iptr unit_type;
  int distinct_types;
  int unit_count;
  char count_text[16];
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    type_counts[unit_type] = 0;
  }
  unit_count = items->value;
  unit_ids = (iptr)items + 0xc;
  distinct_types = 0;
  for (unit_type = 0; unit_type < unit_count; unit_type++) {
    unit = parley_wnd_block.find_unit_by_id(*(i32 *)(unit_ids + unit_type * 4));
    if (-1 < unit) {
      if (type_counts[units[unit].type] == 0) {
        distinct_types++;
      }
      type_counts[units[unit].type] = type_counts[units[unit].type] + 1;
    }
  }
  listed = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (type_counts[unit_type] != 0) {
      str_append_string_id((iptr)&text_section_index_pool_block.parley_description,unit_types[unit_type].name);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_open_paren_str));
      __itoa(type_counts[unit_type],count_text,10);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),count_text);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_close_paren_str));
      if (1 < unit_count) {
        if (listed < distinct_types - 2) {
          crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_comma_str));
        }
        else if (distinct_types - 2 == listed) {
          str_append_space((iptr)&text_section_index_pool_block.parley_description);
          label = (iptr)(label_string(label_table.string_ids[0x33a]));
          str_concat((iptr)&text_section_index_pool_block.parley_description,label);
          str_append_space((iptr)&text_section_index_pool_block.parley_description);
        }
        else {
          crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_period_str));
        }
      }
      listed++;
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_crlf_str));
  return;
}

/* 004dcea5 auto */
void __cdecl parley_describe_cities(ParleyOfferItem *items)

{
  int count;
  int city;
  iptr label;
  int i;
  char size_text[16];
  
  count = items->value;
  for (i = 0; i < count; i++) {
    city = parley_wnd_block.find_city_by_id(*(i32 *)((iptr)items + 0xc + i * 8));
    str_append((char *)&text_section_index_pool_block.parley_description,(const char *)&cities[city].name);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_open_paren_str));
    __itoa(*(int *)((iptr)items + 0x10 + i * 8),size_text,10);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),size_text);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_close_paren_str));
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        label = (iptr)(label_string(label_table.string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_period_str));
      }
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_crlf_str));
  return;
}

/* 004dd016 auto */
void __cdecl parley_describe_maps(int map_kind,int exchange)

{
  iptr label;
  
  if (map_kind == 0) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x33c]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x33d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (map_kind == 1) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x33e]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x33f]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (map_kind == 2) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x340]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x341]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_maps_crlf_str));
  return;
}

/* 004dd176 auto */
void __cdecl parley_describe_war_targets(ParleyOfferItem *items)

{
  int count;
  iptr text;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    text = (iptr)(civ_name_plural(*civ_ptr));
    str_concat((iptr)&text_section_index_pool_block.parley_description,text);
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        text = (iptr)(label_string(label_table.string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,text);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_period_str));
      }
    }
    civ_ptr++;
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_crlf_str));
  return;
}

/* 004dd285 auto */
void __cdecl parley_execute_transaction(ParleyOfferMsg *offer)

{
  i32 *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  iptr civ;
  iptr civ2;
  
  if (offer != 0) {
    dbg_logf((const char *)&str_start_executeparleytransaction_message_d,net_send_sequence);
    civ = offer->civ;
    civ2 = offer->other_civ;
    first_items = (i32 *)((iptr)offer + 0x20);
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)((iptr)second_offer + 0x20);
    switch(offer->mode) {
    case 6:
      parley_apply_treaty(offer->item.value,civ,civ2);
      break;
    case 7:
      parley_transfer_advances(civ2,civ,(ParleyOfferItem *)first_items);
      break;
    case 8:
      parley_transfer_gold(civ2,civ,offer->item.value);
      break;
    case 9:
      parley_transfer_units(civ2,(ParleyOfferItem *)first_items);
      break;
    case 10:
      parley_transfer_cities(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xb:
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xc:
      parley_apply_war_pact(civ,(ParleyOfferItem *)first_items);
      parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xd:
      parley_give_maps(civ,civ2,offer->item.value);
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xe:
      switch(*second_items) {
      case 5:
      case 6:
        parley_apply_treaty(second_offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ2,civ,(ParleyOfferItem *)second_items);
        break;
      case 9:
        parley_transfer_gold(civ2,civ,second_offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ2,(ParleyOfferItem *)second_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ,(ParleyOfferItem *)second_items);
        break;
      case 0xd:
        parley_give_maps(civ2,civ,second_offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ2,(ParleyOfferItem *)second_items);
      }
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
      break;
    case 0xf:
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
    }
    net_send_state_diff(0xff,2,0,0,0);
    if (human_civ[0] == civ2) {
      status_redraw(1);
    }
    else {
      net_send_message(NM_INFO_UPDATE,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0,0,
                         0,0,0,0);
    }
    if (human_civ[0] == civ) {
      status_redraw(1);
    }
    else {
      net_send_message(NM_INFO_UPDATE,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,0,
                         0,0,0,0);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ2] == 0) && (civs[civ2].num_cities == 0)) {
      civ_check_destroyed(civ2,civ);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ] == 0) && (civs[civ].num_cities == 0)) {
      civ_check_destroyed(civ,civ2);
    }
    dbg_logf((const char *)&str_end_executeparleytransaction_message_d,net_send_sequence);
  }
  return;
}

/* 004dd8ad auto */
void __cdecl parley_give_maps(iptr receiver,iptr civ,int map_kind)

{
  byte recv_bits;
  byte giver_bits;
  uint recv_mask;
  uint giver_mask;
  Tile *tile;
  byte *recv_known;
  byte *giver_known;
  Tile *tile2;
  Tile *tile3;
  int y;
  iptr x;
  int in_bounds;
  i32 set_flag;
  i32 redraw_flag;
  iptr target_civ;
  iptr unit;
  byte known_bits;
  byte recv_bit;
  iptr city;
  uint y2;
  uint x2;
  iptr i;
  
  recv_mask = 1 << ((byte)receiver & 0x1f);
  giver_mask = 1 << ((byte)civ & 0x1f);
  y2 = 0;
  x2 = 0;
  net_batch_begin();
  for (i = 0; i < map.area; i++) {
    tile = tile_at(x2,y2);
    recv_known = known_at(x2,y2,receiver);
    giver_known = known_at(x2,y2,civ);
    if ((giver_mask & tile->visible) != 0) {
      if ((recv_mask & tile->visible) == 0) {
        known_set(x2,y2,0,receiver,0,1);
        tile_set_visible(x2,y2,recv_mask,1,1);
      }
      if (map_kind == 0) {
        if (((*recv_known & 0x80) == 0) || ((*giver_known & 0x80) == 0)) {
          giver_bits = *giver_known;
          recv_bits = *recv_known;
          tile2 = tile_at(x2,y2);
          known_bits = (giver_bits | recv_bits) & tile2->improvements & TILE_POLLUTION;
        }
        else {
          known_bits = 0x80;
        }
        redraw_flag = 1;
        set_flag = 0;
        target_civ = receiver;
        recv_known = known_at(x2,y2,receiver);
        known_set(x2,y2,*recv_known & 0xffffff7f | (uint)known_bits,target_civ,set_flag,redraw_flag);
        redraw_flag = 1;
        set_flag = 1;
        x = receiver;
        tile3 = tile_at(x2,y2);
        giver_bits = tile3->improvements;
        recv_known = known_at(x2,y2,civ);
        known_set(x2,y2,(uint)(giver_bits & *recv_known),x,set_flag,redraw_flag);
      }
    }
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  recv_bit = (byte)recv_mask;
  if (map_kind == 0) {
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         (((giver_mask & units[unit].visibility) != 0 || (units[unit].owner == civ)))) {
        units[unit].visibility = units[unit].visibility | recv_bit;
        y = (int)(tile_at((int)units[unit].x,(int)units[unit].y));
        if (((recv_mask & *(byte *)(y + 4)) == 0) || ((giver_mask & *(byte *)(y + 4)) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  if ((map_kind == 0) || (map_kind == 1)) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) &&
         (((giver_mask & (int)(char)cities[city].known_to) != 0 ||
          (cities[city].owner == civ)))) {
        if (cities[city].owner == civ) {
          *(i8 *)&cities[city].revealed_size[receiver] = cities[city].size;
        }
        else {
          *(i8 *)&cities[city].revealed_size[receiver] =
               *(i8 *)&cities[city].revealed_size[civ];
        }
        cities[city].known_to = cities[city].known_to | recv_bit;
        y = (int)(tile_at((int)cities[city].x,(int)cities[city].y));
        if (((recv_mask & *(byte *)(y + 4)) == 0) || ((giver_mask & *(byte *)(y + 4)) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  net_batch_flush();
  net_send_state_diff(0xff,2,0,0,0);
  if (human_civ[0] == receiver) {
    map_redraw_all_views(human_civ[0],1);
  }
  else {
    net_send_message(NM_DRAW_MAP,net.net_slots[net.net_civ_slots[receiver]].id,0,0,0,0,
                       0,0,0,0);
  }
  return;
}

/* 004dde9e auto */
void __cdecl parley_apply_war_pact(iptr civ,ParleyOfferItem *items)

{
  int count;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    diplo_set_treaty_flags(civ,*civ_ptr,0x2000);
    civ_ptr++;
  }
  return;
}

/* 004ddf04 auto */
void __cdecl parley_transfer_gold(iptr civ2,iptr civ,i32 amount)

{
  int attitude_value;
  
  if (civs[civ].gold < amount) {
    amount = civs[civ].gold;
  }
  civs[civ].gold -= amount;
  civs[civ2].gold += amount;
  attitude_value = diplo_gold_attitude_value(amount);
  civ_adjust_attitude(civ2,civ,-((attitude_value * 3) / 2));
  return;
}

/* 004ddfb2 auto */
void __cdecl parley_transfer_advances(iptr civ,iptr civ2,ParleyOfferItem *items)

{
  int count;
  int has_adv;
  iptr *adv_ptr;
  int i;
  
  count = items->value;
  adv_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    has_adv = civ_has_advance(civ2,*adv_ptr);
    if (has_adv != 0) {
      has_adv = civ_has_advance(civ,*adv_ptr);
      if (has_adv == 0) {
        civ_give_advance(civ,*adv_ptr,civ2,1,0);
      }
    }
    adv_ptr++;
  }
  return;
}

/* 004de049 auto */
void __cdecl parley_transfer_cities(iptr civ,ParleyOfferItem *items)

{
  int count;
  iptr city;
  i32 *city_id_ptr;
  int i;
  
  count = items->value;
  city_id_ptr = (i32 *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    city = parley_wnd_block.find_city_by_id(*city_id_ptr);
    if ((-1 < city) && (cities[city].id != 0)) {
      parley_cede_city(city,civ);
    }
    city_id_ptr += 2;
  }
  return;
}

/* 004de0e2 auto */
void __cdecl parley_cede_city(uptr city,iptr civ)

{
  char *count_ptr;
  char civ3;
  iptr civ2;
  int x;
  int y;
  iptr x2;
  iptr y3;
  iptr in_bounds;
  int y2;
  iptr in_bounds2;
  Tile *tile;
  uptr leonardo_city;
  iptr improvement;
  iptr unit;
  iptr unit_type;
  iptr i;
  int defense_ratio;
  
  civ3 = cities[city].owner;
  civ2 = (iptr)civ3;
  x = (int)cities[city].x;
  y = (int)cities[city].y;
  city_set_improvement(city,1,0);
  city_set_improvement(city,4,0);
  city_set_improvement(city,0xb,0);
  city_set_improvement(city,7,0);
  civs[civ].last_city_turn = game.turn;
  civs[civ].num_cities++;
  if (civs[civ2].num_cities != 0) {
    civs[civ2].num_cities--;
  }
  net_batch_begin();
  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x);
    y3 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y;
    in_bounds = map_in_bounds(x2,y3);
    if (in_bounds != 0) {
      tile_set_radius_civ(x2,y3,civ,1);
      tile_set_visible(x2,y3,1 << ((byte)civ & 0x1f),1,1);
    }
  }
  cities[city].owner = (byte)civ;
  city_reveal_to_civ(city,civ2);
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_city_taken((iptr)&cities[city].name,civ,civ2);
  }
  tile_set_owner(x,y,civ,1);
  unit = (iptr)game.num_units;
  while (unit = unit - 1, -1 < unit) {
    if ((units[unit].id != 0) && (units[unit].owner == civ2)) {
      if (((byte)units[unit].home_city == city) &&
         ((cities[city].x != units[unit].x || (cities[city].y != units[unit].y)))) {
        unit_delete_with_cargo(unit,1);
      }
      else if ((cities[city].x == units[unit].x) && (cities[city].y == units[unit].y))
      {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (byte)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = ORDER_NONE;
        city = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,
                                     0xffffffff,-1);
        if ((-1 < (int)city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
      }
    }
  }
  if ((cities[city].building < '\0') ||
     (y2 = civ_can_build_unit(civ,city,(int)cities[city].building), y2 != 0)) {
    if (!(-1 < cities[city].building)) {
      if (cities[city].building < 1) {
        improvement = ~(int)cities[city].building + 1;
      }
      else {
        improvement = (iptr)cities[city].building;
      }
      y2 = city_can_build_improvement(civ,city,improvement);
    }
  }
  /* (y2: 0 if the new owner cannot build the city's production) */
  if (y2 == 0) {
    cities[city].building = UNIT_SETTLERS;
    for (unit_type = 0x3e; -1 < unit_type; unit_type--) {
      if ((unit_types[unit_type].role == ROLE_DEFEND) &&
         (y2 = civ_can_build_unit(civ,city,unit_type), y2 != 0)) {
        defense_ratio = ((int)(char)unit_types[unit_type].defense << 3) /
                  (int)(char)unit_types[unit_type].cost;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          defense_ratio++;
        }
        if (0 < defense_ratio) {
          cities[city].building = (i8)unit_type;
        }
        break;
      }
    }
  }
  map_redraw_neighbors_all_views((int)cities[city].x,(int)cities[city].y);
  net_send_state_diff(0xff,2,0,0,0);
  net_send_message(NM_DRAW_AREA,0xff,(int)cities[city].x,(int)cities[city].y,0,0,0,0,0,0);
  y2 = (int)(tile_at(x,y));
  if ((1 << ((byte)civ & 0x1f) & (uint)*(byte *)(y2 + 4)) == 0) {
    tile_set_visible(x,y,1 << ((byte)civ & 0x1f),1,1);
    map_redraw_tile_all_views(x,y);
  }
  for (i = 0; i < 8; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[i] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[i] + y;
    in_bounds2 = map_in_bounds(x2,y2);
    if ((in_bounds2 != 0) &&
       (tile = (Tile *)(int)(tile_at(x2,y2)),
       (1 << ((byte)civ & 0x1f) & (uint)tile->visible) == 0)) {
      tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),1,1);
      map_redraw_tile_all_views(x2,y2);
    }
  }
  net_batch_flush();
  net_send_message(NM_DRAW_AREA,0xff,x,y,0,0,0,0,0,0);
  cities[city].founder = civ3;
  cities[city].turns_captured = (u8)game.turn;
  leonardo_city = wonder_get_city(WONDER_LEONARDO);
  if (leonardo_city == city) {
    civ_leonardo_upgrade(civ);
  }
  return;
}

/* 004de990 auto */
void __cdecl parley_transfer_units(iptr civ,ParleyOfferItem *items)

{
  char *count_ptr;
  i32 unit_id;
  int result;
  iptr x3;
  iptr unit;
  iptr city;
  iptr unit2;
  int i;
  iptr y2;
  iptr x2;
  int count;
  iptr civ2;
  iptr y3;
  i32 *id_ptr;
  
  count = items->value;
  id_ptr = (i32 *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    unit_id = *id_ptr;
    id_ptr++;
    unit = parley_wnd_block.find_unit_by_id(unit_id);
    if ((-1 < unit) && (civ2 = (iptr)units[unit].owner, civ != civ2)) {
      if ((unit_types[units[unit].type].domain == DOMAIN_SEA) &&
         (result = unit_stack_sum(unit,2), 1 < result)) {
        while (result = unit_stack_sum(unit,2), 1 < result) {
          unit2 = unit_stack_first(unit);
          if (unit2 == unit) {
            unit2 = unit_next_in_stack(unit2);
          }
          result = parley_find_unit_drop_tile(unit2,(iptr)&x3,(iptr)&y3,
                                     (int)units[unit2].owner,1);
          if (result == 0) {
            unit_delete_with_cargo(unit2,1);
          }
          else {
            unit_relocate(unit2,x3,y3,1);
          }
        }
      }
      result = parley_find_unit_drop_tile(unit,(iptr)&x2,(iptr)&y2,civ,0);
      if (result != 0) {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (i8)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = ORDER_NONE;
        city = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,
                                      0xffffffff,-1);
        if ((-1 < city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
        unit_relocate(unit,x2,y2,1);
        unit_reveal_surroundings(unit,0);
      }
    }
  }
  XD_FlushSendBuffer(5000);
  return;
}

/* 004ded07 auto */
i32 __cdecl parley_find_unit_drop_tile(iptr unit,iptr x,iptr y,iptr civ,int must_move)

{
  iptr x2;
  int y2;
  int result;
  i32 wrapped_x;
  iptr i;
  i32 found;
  
  *(int *)x = (int)units[unit].x;
  x2 = *(iptr *)x;
  *(int *)y = (int)units[unit].y;
  y2 = *(int *)y;
  found = 1;
  if (((must_move != 0) || (result = tile_city_owner(x2,y2), result != -1)) ||
     (result = unit_stack_sum(unit,2), 1 < result)) {
    found = 0;
    for (i = 0; i < 0x2d; i++) {
      wrapped_x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
      *(i32 *)x = wrapped_x;
      *(int *)y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
      if ((((((must_move == 0) || (*(int *)x != x2)) || (*(int *)y != y2)) &&
           (result = map_in_bounds(*(iptr *)x,*(iptr *)y), result != 0)) &&
          (((result = tile_city_owner(*(iptr *)x,*(i32 *)y), result == civ ||
            (result = tile_unit_owner(*(i32 *)x,*(i32 *)y),
            result == civ)) ||
           ((result = tile_city_owner(*(iptr *)x,*(i32 *)y), result == -1 &&
            (result = tile_unit_owner(*(i32 *)x,*(i32 *)y), result == -1))))
          )) && ((((unit_types[units[unit].type].domain == DOMAIN_LAND &&
                   (result = tile_is_ocean(*(i32 *)x,*(i32 *)y), result == 0
                   )) || ((unit_types[units[unit].type].domain == DOMAIN_SEA &&
                          (result = tile_is_ocean(*(i32 *)x,*(i32 *)y),
                          result != 0)))) || (unit_types[units[unit].type].domain == DOMAIN_AIR)))) {
        return 1;
      }
    }
  }
  return found;
}

/* 004def54 auto */
void ParleyWindow::describe_treaty(int treaty_kind,int exchange)

{
  iptr label;
  
  if (exchange == 0) {
    switch(treaty_kind) {
    case 0:
      label = (iptr)(label_string(label_table.string_ids[0x31d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 1:
      label = (iptr)(label_string(label_table.string_ids[0x31e]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 2:
      label = (iptr)(label_string(label_table.string_ids[0x31f]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 3:
      label = (iptr)(label_string(label_table.string_ids[0x320]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (exchange == 1) {
    if (treaty_kind == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x2f3]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else if (treaty_kind == 1) {
      label = (iptr)(label_string(label_table.string_ids[0x2f4]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else if (treaty_kind == 2) {
      label = (iptr)(label_string(label_table.string_ids[0x2f5]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_treaty_crlf_str));
  return;
}

/* 004df10f auto */
void __cdecl parley_apply_treaty(i32 treaty_kind,iptr civ,iptr other_civ)

{
  switch(treaty_kind) {
  case 0:
    diplo_set_treaty_flags(civ,other_civ,2);
    break;
  case 1:
    diplo_set_treaty_flags(civ,other_civ,4);
    break;
  case 2:
    diplo_set_treaty_flags(civ,other_civ,8);
    break;
  case 3:
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_CEASEFIRE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,2);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_PEACE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,4);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,8);
    }
  }
  return;
}

/* 00511880 auto */
void __cdecl
net_send_text_args(iptr popup_kind,i32 player_id,int string_count,int number_count,iptr arg_a,iptr arg_b)

{
  size_t len;
  void *write_ptr;
  uint payload_size;
  int i;
  void *payload;
  
  payload = (void *)0x0;
  payload_size = 0;
  if (2 < game.multiplayer) {
    if (string_count != 0) {
      for (i = 0; i < string_count; i++) {
        len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
        payload_size = payload_size + len + 1;
      }
    }
    if (number_count != 0) {
      payload_size = number_count * 4 + payload_size;
    }
    if (payload_size != 0) {
      payload = operator_new(payload_size);
      write_ptr = payload;
      if (string_count != 0) {
        for (i = 0; i < string_count; i++) {
          crt_strcpy((char *)write_ptr,((i8 *)&text_args.strings) + i * 0x104);
          len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
          write_ptr = (void *)((int)write_ptr + len + 1);
        }
      }
      if (number_count != 0) {
        for (i = 0; i < number_count; i++) {
          crt_memcpy(write_ptr,((i8 *)&text_args.numbers) + i * 4,4);
          write_ptr = (void *)((int)write_ptr + 4);
        }
      }
    }
    net_send_message(NM_POPUP,player_id,popup_kind,string_count,number_count,arg_a,arg_b,0,0,(iptr)payload);
  }
  return;
}

/* 00511a0e auto */
void __cdecl
net_popup_queue_push(i32 popup_kind,i32 string_count,i32 number_count,i32 arg_a,
            i32 arg_b,i32 payload,int payload_size)

{
  i32 *new_head;
  i32 *entry;
  
  entry = (i32 *)(operator_new(payload_size + 0x1c));
  *entry = 0;
  entry[1] = popup_kind;
  entry[2] = string_count;
  entry[3] = number_count;
  entry[4] = arg_a;
  entry[5] = arg_b;
  entry[6] = payload_size;
  if (payload_size != 0) {
    crt_memcpy(entry + 7,(const void *)payload,payload_size);
  }
  new_head = entry;
  if ((*(i32 * *)&net_popups.head) != (i32 *)0x0) {
    *(*(i32 * *)&net_popups.tail) = (i32)entry;
    new_head = (*(i32 * *)&net_popups.head);
  }
  (*(i32 * *)&net_popups.head) = new_head;
  (*(i32 * *)&net_popups.tail) = entry;
  return;
}

/* 00511acf auto */
void __cdecl net_popup_queue_clear(void)

{
  i32 *entry;
  
  while (entry = (*(i32 * *)&net_popups.head), (*(i32 * *)&net_popups.head) != (i32 *)0x0) {
    (*(i32 * *)&net_popups.head) = (i32 *)*(*(i32 * *)&net_popups.head);
    operator_delete(entry);
  }
  net_popups.tail = 0;
  return;
}

/* 00511b2d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_idle_autoclose(void)

{
  int now;
  
  net.process_messages(1,0);
  if (((net.host_handoff_stage == '\0') && (net.nm_parley_cancel_count == 0)) &&
     (now = timer_ticks(), now - dialog_state.timeout_start < 0x4b1)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* Part of net_popup_queue_show_next (00511ba2): a request to meet (0x3d):
   accepted, the parley window opens (returns 1: the entry is freed and the
   dialog destroyed, the caller returns); refused or cancelled, the answer is
   sent. */
static int net_popup_parley_request(int *entry,Dialog &dlg)
{
  int choice;
  iptr text;
  iptr attitude_level;
  i32 eh_state;
  
  civs[human_civ[0]].last_contact_turn[entry[4]] = game.turn;
  civs[entry[4]].last_contact_turn[human_civ[0]] = game.turn;
  if ((int)parley_wnd_block.other_civ == -1) {
    parley_wnd_block.other_civ = -2;
    text_clear();
    text = civ_leader_title(entry[4]);
    text_append((const char *)text);
    text_append_space();
    text = (iptr)(civ_leader_name(entry[4]));
    text_append((const char *)text);
    text_append_space();
    text_append_label(0x8c);
    text_append_space();
    text = (iptr)(civ_name_plural(entry[4]));
    text_append((const char *)text);
    text_set_string_arg(0,(const char *)&textbuf.buf);
    text = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)text);
    text = (iptr)(civ_name_plural(human_civ[0]));
    text_set_string_arg(2,(const char *)text);
    if (*(char *)&civs[entry[4]].attitude[human_civ[0]] == '\0') {
      attitude_level = 0;
    }
    else {
      attitude_level = diplo_attitude_level((uint)civs[entry[4]].attitude[human_civ[0]]);
    }
    text = (iptr)(label_string(attitude_names[attitude_level]));
    text_set_string_arg(3,(const char *)text);
    dlg_style.pending_idle_callback = net_popup_idle_autoclose;
    choice = popup_game_with_sprite((const char *)&str_parleyrequest,(iptr)&popup_info_icon_sprite,0);
    if (net.host_handoff_stage == '\0') {
      if (net.nm_parley_cancel_count == 0) {
        net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,choice,0,0,0,0,0,0,0);
      }
      if (net.nm_parley_cancel_count == 0) {
        if (choice == 1) {
          text = entry[4];
          operator_delete(entry);
          net.popup_in_progress = 0;
          parley_wnd_block.open(text,2);
          eh_state = 0xffffffff;
          /* inlined unwind funclet 0051399d */ { dlg.Dialog::destruct(); }
          return 1;
        }
      }
      else {
        net.nm_parley_cancel_count = 0;
        text = (iptr)(civ_name_plural(entry[4]));
        text_set_string_arg(0,(const char *)text);
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite((const char *)&str_parleycancel_4,(iptr)&popup_info_icon_sprite,0);
      }
      parley_wnd_block.other_civ = -1;
      diplo_meeting.mp_pending = 0;
    }
    else {
      net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                         2,0,0,0,0,0,0,0);
      parley_wnd_block.other_civ = -1;
      diplo_meeting.mp_pending = 0;
    }
  }
  else {
    net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,2,
                       0,0,0,0,0,0,0);
  }
  return 0;
}

/* Part of net_popup_queue_show_next (00511ba2): the other side asks to see
   where our units come from (100): our answer, sent, and the parley window's
   unit pane updated. */
static void net_popup_reveal_units(int *entry,Dialog &dlg)
{
  iptr text;
  
  if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
    parley_prompts.units_open = 1;
    dlg.load_game_section((const char *)&str_revealunitorigins,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x375]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x376]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x14);
    if (dlg.result_extra == 1) {
      parley_wnd_block.local_units_revealed = 1;
      if ((int)parley_wnd_block.mode == 2) {
        ((Control *)parley_wnd_block.conceal_button)->show_and_redraw();
        ((Button *)parley_wnd_block.conceal_button)->enable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      else {
        ((Control *)parley_wnd_block.conceal_button)->hide();
        ((Button *)parley_wnd_block.conceal_button)->disable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                         (int)(char)parley_wnd_block.local_units_revealed,0,0,0,0,0,0);
      if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
         (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
        ((Control *)parley_wnd_block.info_hide_buttons[0])->show_and_redraw();
        ((Control *)parley_wnd_block.info_show_buttons[0])->hide();
        parley_wnd_block.local_units_revealed = 1;
        ((Button *)parley_wnd_block.sort_by_unit_buttons[0])->enable();
        ((Button *)parley_wnd_block.sort_by_city_buttons[0])->enable();
        parley_draw_unit_list(0);
      }
    }
    parley_prompts.units_open = 0;
  }
}

/* Part of net_popup_queue_show_next (00511ba2): the other side asks to see
   our city information (0x65): our answer, sent, and the parley window's city
   pane updated. */
static void net_popup_reveal_cities(int *entry,Dialog &dlg)
{
  iptr text;
  
  if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
    parley_prompts.city_open = 1;
    dlg.load_game_section((const char *)&str_revealcityinfo,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x377]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x376]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x14);
    if (dlg.result_extra == 1) {
      parley_wnd_block.local_cities_revealed = 1;
      if ((int)parley_wnd_block.mode == 2) {
        ((Control *)parley_wnd_block.conceal_button)->show_and_redraw();
        ((Button *)parley_wnd_block.conceal_button)->enable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      else {
        ((Control *)parley_wnd_block.conceal_button)->hide();
        ((Button *)parley_wnd_block.conceal_button)->disable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                         (int)(char)parley_wnd_block.local_cities_revealed,0,0,0,0,0,0);
      if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
         (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
        ((Control *)parley_wnd_block.info_hide_buttons[0])->show_and_redraw();
        ((Control *)parley_wnd_block.info_show_buttons[0])->hide();
        parley_draw_city_list(0);
      }
    }
    parley_prompts.city_open = 0;
  }
}

/* Part of net_popup_queue_show_next (00511ba2): the entry's arguments
   (entry[2] strings, then entry[3] numbers) into the text arguments. */
static void net_popup_load_args(int *entry)
{
  size_t len;
  int i;
  int *arg_ptr;
  
  if ((entry[2] != 0) || (entry[3] != 0)) {
    arg_ptr = entry + 7;
  }
  if (entry[2] != 0) {
    for (i = 0; i < entry[2]; i++) {
      crt_strcpy(((i8 *)&text_args.strings) + i * 0x104,(const char *)arg_ptr);
      len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
      arg_ptr = (int *)((int)arg_ptr + len + 1);
    }
  }
  if (entry[3] != 0) {
    for (i = 0; i < entry[3]; i++) {
      crt_memcpy(((i8 *)&text_args.numbers) + i * 4,arg_ptr,4);
      arg_ptr++;
    }
  }
}

/* Part of net_popup_queue_show_next (00511ba2): the popup, by kind
   (entry[1]). Returns 1 when the caller is to return at once (a meeting
   accepted: the entry is freed and the dialog destroyed). */
static int net_popup_show(int *entry,Dialog &dlg)
{
  int choice;
  iptr text;
  i8 *icon_sprite;
  int i;
  
  switch(entry[1]) {
  case 0:
    popup_game((const char *)&str_retiredie);
    break;
  case 1:
    popup_game((const char *)&str_retireai);
    break;
  case 2:
    dlg_game_txt_with_unit((const char *)&str_barbarians_2,entry[4]);
    break;
  case 3:
    popup_game_with_sprite((const char *)&str_globalwarming_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 4:
    popup_game_with_sprite((const char *)&str_fearwarming_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 5:
    historian_report_show();
    break;
  case 6:
    popup_game_with_sprite((const char *)&str_eaglehaslanded_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 7:
    popup_game_with_sprite((const char *)&str_scenarioends_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 8:
    popup_game_with_sprite((const char *)&str_scenarioend_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 9:
    text = civ_leader_title(human_civ[0]);
    text_set_string_arg(0,(const char *)text);
    text = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)text);
    popup_game_with_sprite((const char *)&str_planretire_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 10:
    text = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)text);
    popup_game_with_sprite((const char *)&str_doretire_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 0xb:
    popup_game((const char *)&str_launched);
    spaceship_wnd_run(-entry[4]);
    break;
  case 0xc:
    popup_game((const char *)&str_spacereturns_2);
    break;
  case 0xd:
    popup_game((const char *)&str_spacedestroyed_2);
    break;
  case 0xe:
    music_play_track(2,1);
    popup_game_with_sprite((const char *)&str_destroyed_2,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + entry[4] * 0x3c),0);
    break;
  case 0xf:
    snd_play_effect((-(uint)(entry[5] == 0) & 0x14) + 0x16,1,0,0);
    dlg_game_txt_with_unit((const char *)&str_caravan_6,entry[4]);
    break;
  case 0x10:
    snd_play_effect((-(uint)(entry[5] == 0) & 0x14) + 0x16,1,0,0);
    dlg_game_txt_with_unit((const char *)&str_foodcaravan_4,entry[4]);
    break;
  case 0x11:
    dlg_game_txt_with_unit_flags((const char *)&str_caravanother_3,entry[4],entry[5]);
    break;
  case 0x12:
    dlg.load_game_text((const char *)&str_startwonder_2);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x13:
    dlg.load_game_text((const char *)&str_switchwonder_2);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x14:
    dlg.load_game_text((const char *)&str_abandonwonder_2);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x15:
    dlg_game_txt_with_unit((const char *)&str_paradrop_2,entry[4]);
    break;
  case 0x16:
    popup_game((const char *)&str_schism_2);
    break;
  case 0x17:
    text = popup_game_with_sprite((const char *)&str_canescape,(iptr)&popup_info_icon_sprite,0);
    net_send_message(NM_CANESCAPE_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x18:
    popup_game_with_city((const char *)&str_escape,entry[4]);
    break;
  case 0x19:
    dlg.load_game_text((const char *)&str_capturewonder);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x1a:
    dlg.load_game_text((const char *)&str_lostwonder);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x1b:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citycapture,entry[5],0x46,1,entry[4]);
    break;
  case 0x1c:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citywinally,entry[5],0x46,1,entry[4]);
    break;
  case 0x1d:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_cityloseally,entry[5],0x46,1,entry[4]);
    break;
  case 0x1e:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citycapture2,entry[5],0x46,1,entry[4]);
    break;
  case 0x1f:
    dlg_game_txt_with_unit((const char *)&str_partisans,entry[4]);
    break;
  case 0x20:
    dlg_game_txt_with_unit((const char *)&str_promoted,entry[4]);
    break;
  case 0x21:
    popup_game_with_sprite((const char *)&str_incorruptible_2,(iptr)&government_sprites_block.government_sprite_democracy,0);
    break;
  case 0x22:
    dlg_game_txt_with_unit((const char *)&str_deserted_2,entry[4]);
    break;
  case 0x23:
    text = dlg_game_txt_with_unit((const char *)&str_desert_3,entry[5]);
    net_send_message(NM_DESERT_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x24:
    text = dlg_game_txt_with_unit((const char *)&str_desert2,entry[5]);
    net_send_message(NM_DESERT_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x25:
    dlg_game_txt_with_unit((const char *)&str_blewitup_2,entry[4]);
    break;
  case 0x26:
    popup_game_with_sprite((const char *)&str_bond007_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
    break;
  case 0x27:
    popup_game_with_sprite((const char *)&str_bond_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
    break;
  case 0x28:
    popup_game_with_sprite((const char *)&str_bondglory_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
    break;
  case 0x29:
    popup_game_with_improvement((const char *)&str_useweapons,IMP_MANHATTAN,8);
    break;
  case 0x2a:
    popup_game_with_improvement((const char *)&str_sdi,IMP_SDI_DEFENSE,8);
    break;
  case 0x2b:
    popup_game_with_sprite((const char *)&str_changed,(iptr)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c),entry[5]);
    break;
  case 0x2c:
    popup_game_with_sprite((const char *)&str_overthrown,(iptr)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c),entry[5]);
    break;
  case 0x2d:
    dlg_game_txt_with_unit((const char *)&str_breakcease_2,entry[4]);
    break;
  case 0x2e:
    dlg_game_txt_with_unit((const char *)&str_sneak,entry[4]);
    break;
  case 0x2f:
    dlg_game_txt_with_unit((const char *)&str_missileattack,entry[4]);
    break;
  case 0x30:
    dlg_game_txt_with_unit((const char *)&str_pearlharbor,entry[4]);
    break;
  case 0x31:
    popup_game_with_improvement((const char *)&str_battery2,entry[4],8);
    break;
  case 0x32:
    popup_game_with_improvement((const char *)&str_battery,entry[4],8);
    break;
  case 0x33:
    dlg_game_txt_with_unit((const char *)&str_scramble,entry[4]);
    break;
  case 0x34:
    dlg_game_txt_with_unit((const char *)&str_amphibmotize,entry[4]);
    break;
  case 0x35:
    popup_game((const char *)&str_multiplelose);
    break;
  case 0x36:
    popup_game((const char *)&str_multiplewin);
    break;
  case 0x37:
    dlg_game_txt_with_unit_flags((const char *)&str_ransom,entry[4],0);
    break;
  case 0x38:
    popup_game_with_sprite((const char *)&str_activateally_2,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    break;
  case 0x39:
    popup_game_with_sprite((const char *)&str_allyhelps_2,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
    break;
  case 0x3a:
    popup_game((const char *)&str_cancelpeace_2);
    break;
  case 0x3b:
    popup_game((const char *)&str_allyunderattack);
    break;
  case 0x3c:
    popup_game((const char *)&str_allyattacking);
    break;
  case 0x3d:
    if (net_popup_parley_request(entry,dlg) != 0) {
      return 1;
    }
    break;
  case 0x3e:
    dlg_game_txt_with_unit_flags((const char *)&str_upgrade_2,entry[4],(entry[5] == 0) - 1 & 8);
    break;
  case 0x3f:
    if (net.net_slots[net.net_civ_slots[human_civ[0]]].id != entry[4])
    {
      popup_game_with_sprite((const char *)&str_newplayer_2,(iptr)&popup_info_icon_sprite,0);
    }
    break;
  case 0x40:
    dlg.load_game_section((const char *)&str_newturntimerclient,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x354]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x355]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x28);
    net_send_message(NM_NEWTURNTIMER_REPLY,0,dlg.result_extra,0,0,0,0,0,0,0);
    break;
  case 0x41:
    popup_game_with_sprite((const char *)&str_newturntimerno,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x42:
    popup_game_with_sprite((const char *)&str_newturntimeryes,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x43:
    dlg.load_game_section((const char *)&str_pmchangeclient,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x354]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x355]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0);
    net_send_message(NM_PMCHANGE_REPLY,0,dlg.result_extra,0,0,0,0,0,0,0);
    break;
  case 0x44:
    popup_game_with_sprite((const char *)&str_pmchangeno_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x45:
    popup_game_with_sprite((const char *)&str_pmchangeyes_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x46:
    snd_play_effect(SND_LARGEXPL,1,0,0);
    city_notify_dialog_ex((iptr)&str_manhattan_2,entry[4],1,(iptr)&caption_close_button_sprite_block.city_notify_sprite);
    break;
  case 0x47:
    dlg.load_game_text((const char *)&str_built2_2);
    if (entry[4] < 0) {
      icon_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c;
    }
    else {
      icon_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + entry[4] * 0x3c;
    }
    dlg.add_sprite((iptr)icon_sprite,0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x48:
    popup_game((const char *)&str_movecapital_2);
    break;
  case 0x49:
    popup_game((const char *)&str_astronauts_2);
    break;
  case 0x4a:
    dlg.load_game_text((const char *)&str_almostwonder_2);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4b:
    dlg.load_game_text((const char *)&str_stillwonder1);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4c:
    dlg.load_game_text((const char *)&str_stillwonder2);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4d:
    dlg.load_game_text((const char *)&str_endwonder_4);
    dlg.add_sprite(
               (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                     (char)advances[entry[4]].category * 0xf0 +
                     (char)advances[entry[4]].epoch * 0x3c),0,0);
    dlg.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4e:
    dlg_game_txt_with_unit((const char *)&str_barbariansland,entry[4]);
    break;
  case 0x4f:
    dlg.init_default();
    for (i = 0; i < entry[2]; i++) {
      dlg.add_line((const char *)(((i8 *)&text_args.strings) + i * 0x104),0xffffffff,
                 0xffffffff,0xffffffff,0);
    }
    dlg.show_modal(0);
    break;
  case 0x50:
    popup_game_with_sprite((const char *)&str_warends,(iptr)&ceasefire_icon_sprite,0);
    break;
  case 0x51:
    dlg_game_txt_with_unit((const char *)&str_militaryaid1,entry[4]);
    break;
  case 0x52:
    dlg_game_txt_with_unit((const char *)&str_militaryaid2,entry[4]);
    break;
  case 0x53:
    popup_game_with_sprite((const char *)&str_lostclient_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x54:
    popup_game_with_advance((const char *)&str_goldenage_2,ADV_PHILOSOPHY,8);
    break;
  case 0x55:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite((const char *)&str_enemyembassy_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x56:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite((const char *)&str_enemyinvestigate_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x57:
    choice = civ_has_advance(entry[4],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game_with_sprite((const char *)&str_steal_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x58:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite((const char *)&str_foiledagain,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x59:
    choice = civ_has_advance(entry[5],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game_with_sprite((const char *)&str_sabotageone_2,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),8);
    break;
  case 0x5a:
    choice = civ_has_advance(entry[4],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game((const char *)&str_sabotagetwo_2);
    break;
  case 0x5b:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite((const char *)&str_watersupply_2,(iptr)&popup_info_icon_sprite,0);
    break;
  case 0x5c:
    text_clear();
    text_append((const char *)&str_plantednuke_2);
    popup_game_with_improvement((const char *)&textbuf.buf,IMP_MANHATTAN,8);
    break;
  case 0x5d:
    text_clear();
    text_append((const char *)&str_plantednuke2);
    popup_game_with_improvement((const char *)&textbuf.buf,IMP_MANHATTAN,8);
    break;
  case 0x5e:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game((const char *)&str_civilwar_2);
    break;
  case 0x5f:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upmine,entry[4]);
    break;
  case 0x60:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upyours,entry[4]);
    break;
  case 0x61:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upyourstoo,entry[4]);
    break;
  case 0x62:
    popup_game((const char *)&str_mercdeclare_2);
    break;
  case 99:
    popup_game_with_advance((const char *)&str_tookciv,entry[4],0);
    break;
  case 100:
    net_popup_reveal_units(entry,dlg);
    break;
  case 0x65:
    net_popup_reveal_cities(entry,dlg);
  }
  return 0;
}

/* 00511ba2 auto */
/* WARNING: Removing unreachable block (ram,0x0051343e) */
/* WARNING: Removing unreachable block (ram,0x0051347e) */
/* WARNING: Removing unreachable block (ram,0x0051369c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_queue_show_next(void)

{
  int *entry;
  Dialog dlg;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  entry = (*(int * *)&net_popups.head);
  dlg.Dialog::construct(0x4000);
  eh_state = 0;
  if (entry != (int *)0x0) {
    net.popup_in_progress = 1;
    (*(int * *)&net_popups.head) = (int *)*entry;
    if ((*(int * *)&net_popups.head) == (int *)0x0) {
      net_popups.tail = 0;
    }
    net_popup_load_args(entry);
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    if (net_popup_show(entry,dlg) != 0) {
      return;
    }
    operator_delete(entry);
    net.popup_in_progress = 0;
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0051399d */ { dlg.Dialog::destruct(); }
  return;
}

/* 00514220 auto */
NetMsgQueue *NetMsgQueue::construct()

{
  _memset(this,0,24000);
  reset();
  return this;
}

/* 00514254 auto */
void NetMsgQueue::clear()

{
  reset();
  return;
}

/* 00514272 auto */
void NetMsgQueue::reset()

{
  u8 *field_ptr;
  int i;
  
  for (i = 0; i < 2000; i++) {
    if (slots[i].data != 0) {
      operator_delete((void *)slots[i].data);
      field_ptr = (u8 *)&slots[i].data;
      field_ptr[0] = '\0';
      field_ptr[1] = '\0';
      field_ptr[2] = '\0';
      field_ptr[3] = '\0';
    }
    field_ptr = (u8 *)&slots[i].size;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
    field_ptr = (u8 *)&slots[i].sender;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  main_head = 400;
  main_tail = 400;
  main_count = 0;
  prio_head = 0;
  prio_tail = 0;
  prio_count = 0;
  return;
}

/* 0051435f auto */
i32 __cdecl net_msg_is_priority(int msg_type)

{
  i32 is_priority;
  
  if ((msg_type < 0x2a) || (0x66 < msg_type)) {
    is_priority = 0;
  }
  else {
    is_priority = 1;
  }
  return is_priority;
}

/* 0051438f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 NetMsgQueue::enqueue(int sender,NetMsg *msg,uint msg_size)

{
  int is_priority;
  void *msg_buf;
  i8 lost_msg_text[256];
  iptr msg_copy;
  i8 lost_prio_text[256];
  
  if ((((game.multiplayer == 5) || (game.multiplayer == 6)) && (sender != 0)) &&
     (sender != 0xff)) {
    if (net.host_flag == '\0') {
      sender = 0;
    }
    else {
      sender = 1;
    }
  }
  if ((int)msg->hdr.msg_type == NM_HEART_BEAT) {
    net.nm_heart_beat_count++;
    net_send_message(NM_HEART_BEAT_ACK,sender,0,0,0,0,0,0,0,0);
  }
  else if ((int)msg->hdr.msg_type == NM_HEART_BEAT_ACK) {
    net.nm_heartbeat_ack_count++;
  }
  else {
    is_priority = net_msg_is_priority((i32)msg->hdr.msg_type);
    if (is_priority == 0) {
      if ((main_head == main_tail) && (main_count != 0)) {
        crt_strcpy(lost_msg_text,str_message_lost_in_enqueuemessage);
        crt_strcat(lost_msg_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_msg_text,str_from_d_size_d_2);
        dbg_logf_int2((const char *)lost_msg_text,sender,msg_size);
        return 0;
      }
      if (((int)main_head < 400) || (1999 < (int)main_head)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_head_400_head_max_msgs_in_queue,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_4,0x130);
      }
      if (((int)main_tail < 400) || (1999 < (int)main_tail)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_tail_400_tail_max_msgs_in_queue,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_5,0x131);
      }
      msg_buf = operator_new(msg_size);
      slots[main_tail].data = (i32)msg_buf;
      if (slots[main_tail].data == 0) {
        dbg_logf_int2((const char *)&str_enqueuemessage_primary_failed_to_allocat,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (slots[main_tail].data == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_msgqueue_tail_pdata,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_6,0x136);
      }
      crt_memcpy((void *)slots[main_tail].data,(const void *)msg,msg_size);
      slots[main_tail].sender = sender;
      slots[main_tail].size = msg_size;
      main_tail = (int)(main_tail + 1) % 2000;
      if (main_tail == 0) {
        main_tail = 400;
      }
      main_count++;
      if ((int)net.msgq_normal_peak < (int)main_count) {
        net.msgq_normal_peak = main_count;
      }
    }
    else {
      if ((prio_head == prio_tail) && (prio_count != 0)) {
        crt_strcpy(lost_prio_text,str_message_lost_in_alpha_enqueuemessage);
        crt_strcat(lost_prio_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_prio_text,str_from_d_size_d);
        dbg_logf_int2((const char *)lost_prio_text,sender,msg_size);
        return 0;
      }
      if (((int)prio_head < 0) || (399 < (int)prio_head)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_alphahead_0_alphahead_400,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp,0x10c);
      }
      if (((int)prio_tail < 0) || (399 < (int)prio_tail)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_alphatail_0_alphatail_400,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_2,0x10d);
      }
      msg_buf = operator_new(msg_size);
      slots[prio_tail].data = (i32)msg_buf;
      if (slots[prio_tail].data == 0) {
        msg_copy = (iptr)msg;
        dbg_logf_int2((const char *)&str_enqueuemessage_alpha_failed_to_allocate,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (slots[prio_tail].data == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_msgqueue_alphatail_pdata,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_3,0x112);
      }
      crt_memcpy((void *)slots[prio_tail].data,(const void *)msg,msg_size);
      slots[prio_tail].sender = sender;
      slots[prio_tail].size = msg_size;
      prio_tail = (int)(prio_tail + 1) % 400;
      prio_count++;
      if ((int)net.msgq_priority_peak < (int)prio_count) {
        net.msgq_priority_peak = prio_count;
      }
    }
  }
  return 1;
}

/* 005149d6 auto */
i32 NetMsgQueue::dequeue(iptr out_sender,iptr out_msg,iptr out_size,int priority_only)

{
  u8 *slot_field;
  i32 result;
  
  if (prio_count == 0) {
    if ((priority_only == 0) && (main_count != 0)) {
      *(i32 *)out_sender = slots[main_head].sender;
      slot_field = (u8 *)&slots[main_head].sender;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *(i32 *)out_msg = slots[main_head].data;
      slot_field = (u8 *)&slots[main_head].data;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *(i32 *)out_size = slots[main_head].size;
      slot_field = (u8 *)&slots[main_head].size;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      main_head = (int)(main_head + 1) % 2000;
      if (main_head == 0) {
        main_head = 400;
      }
      main_count--;
      result = 1;
    }
    else {
      result = 0;
    }
  }
  else {
    *(i32 *)out_sender = slots[prio_head].sender;
    slot_field = (u8 *)&slots[prio_head].sender;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *(i32 *)out_msg = slots[prio_head].data;
    slot_field = (u8 *)&slots[prio_head].data;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *(i32 *)out_size = slots[prio_head].size;
    slot_field = (u8 *)&slots[prio_head].size;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    prio_head = (int)(prio_head + 1) % 400;
    prio_count--;
    result = 1;
  }
  return result;
}

/* 0051ea0c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_vote_wait_idle_cb(void)

{
  int now_ticks;
  
  net.process_messages(1,0);
  if ((((net.host_handoff_stage == '\0') && (now_ticks = timer_ticks(), now_ticks - dialog_state.timeout_start < 0x961)) &&
      (net.vote_replies_pending != 0)) && (net.vote_status != -1)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* 0051f11a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_lock_wait_idle_cb(void)

{
  timer_ticks();
  net.process_messages(1,0);
  if ((net.nm_lock_data_count != 0) || (net.nm_server_canceled_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0051f17e auto */
void __cdecl net_pump_idle_cb(void)

{
  net.process_messages(1,0);
  return;
}

/* 005233fc auto */
i32 __cdecl net_host_register_player_civ(NetPlayerInfoMsg *player_info,int player_id)

{
  int is_female;
  i8 saved_player_civ;
  i8 saved_turn_civ_val;
  i32 result;
  iptr saved_human_civ;
  int title_label_id;
  iptr join_civ;
  int slot;
  u8 female_byte;
  int title_idx;
  u8 portrait;
  
  if (net.host_flag == '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_bserver_2,(iptr)&str_d_ss_franklinton_startup_multiplayer_cpp_2,0x778);
  }
  saved_human_civ = human_civ[0];
  saved_turn_civ_val = game.turn_civ;
  saved_player_civ = game.player_civ;
  if (((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) && (net.session_phase < 2)) {
    is_female = player_info->gender;
    result = player_info->leader_index;
    join_civ = player_info->civ;
    game.player_civ = (byte)join_civ;
    civs[(char)(byte)join_civ].tribe = (i16)result;
    portrait = (u8)result;
    civs[game.player_civ].leader_portrait = portrait;
    if (is_female != 0) {
      civs[game.player_civ].leader_portrait += 21;
    }
    human_civ[0] = (iptr)game.player_civ;
    if (net.session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    female_byte = (u8)is_female;
    leaders[civs[game.player_civ].tribe].female = female_byte;
    leaders[civs[game.player_civ].tribe].leader_name =
         leaders[civs[game.player_civ].tribe].names[((uint)leaders[civs[game.player_civ].tribe].female)];
    result = (i32)(label_string((int)leaders[civs[game.player_civ].tribe].leader_name));
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),(const char *)((const void *)result));
    if (0 < leaders[civs[game.player_civ].tribe].leader_name) {
      leaders[civs[game.player_civ].tribe].leader_name =
           -leaders[civs[game.player_civ].tribe].leader_name;
    }
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),(const char *)((const void *)((iptr)player_info + 0x20)));
    if (player_info->has_names != 0) {
      leaders[civs[game.player_civ].tribe].plural = -leaders[civs[game.player_civ].tribe].plural;
      leaders[civs[game.player_civ].tribe].adjective =
           -leaders[civs[game.player_civ].tribe].adjective;
      for (title_idx = 0; title_idx < 7; title_idx++) {
        if (leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] < 1)
        {
          title_label_id = ~(int)leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] + 1;
        }
        else {
          title_label_id = (int)leaders[civs[game.player_civ].tribe].titles[title_idx][is_female];
        }
        result = (i32)(label_string(title_label_id));
        crt_strcpy((char *)(&civ_names[game.player_civ].titles[title_idx]),(const char *)((const void *)result));
        leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] =
             -leaders[civs[game.player_civ].tribe].titles[title_idx][is_female];
      }
      crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),(const char *)((const void *)((iptr)player_info + 0x38)));
      crt_strcpy((char *)(&civ_names[game.player_civ].adjective),(const char *)((const void *)((iptr)player_info + 0x50)));
      if (player_info->has_titles != 0) {
        for (title_idx = 0; title_idx < 7; title_idx++) {
          crt_strcpy((char *)(&civ_names[game.player_civ].titles[title_idx]),
                       (const char *)(title_idx * 0x18 + (iptr)player_info + 0x6c));
        }
      }
    }
    civ_names[game.player_civ].city_style = (i16)player_info->city_style;
  }
  else {
    join_civ = player_info->civ;
    if (net.session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    game.player_civ = (byte)join_civ;
    game.turn_civ = (byte)join_civ;
    human_civ[0] = join_civ;
    crt_strcpy((char *)(&civ_names[((char)(byte)join_civ)].leader_name),(const char *)((const void *)((iptr)player_info + 0x20)));
    if (0 < leaders[civs[game.player_civ].tribe].leader_name) {
      leaders[civs[game.player_civ].tribe].leader_name =
           -leaders[civs[game.player_civ].tribe].leader_name;
    }
    is_female = player_info->gender;
    civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
    if (is_female != 0) {
      civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
    }
    female_byte = (u8)is_female;
    leaders[civs[game.player_civ].tribe].female = female_byte;
  }
  for (slot = 0; (slot < 7 && (net.net_slots[slot].id != player_id));
      slot = slot + 1) {
  }
  if (slot < 7) {
    net.net_civ_slots[game.player_civ] = slot;
    (((i32 *)&net.slot_civs))[slot * 0x15] = (int)game.player_civ;
    (((i8 *)&net.slot_active_flags))[slot * 0x54] = 1;
    if (player_id == 0) {
      (((i8 *)&net.slot_host_flags))[slot * 0x54] = 1;
    }
    else {
      (((i8 *)&net.slot_host_flags))[slot * 0x54] = 0;
    }
    result = 1;
  }
  else {
    if (net.session_phase < 3) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    net_send_message(NM_UNLOCK_NATION,0,player_info->leader_index,player_info->civ,0,0,0,0,0,0);
    result = 0;
  }
  human_civ[0] = saved_human_civ;
  game.turn_civ = saved_turn_civ_val;
  game.player_civ = saved_player_civ;
  return result;
}

/* 00523d8a auto */
void __cdecl net_send_rules_tables(i32 dest_player)

{
  net_send_message(NM_COSMIC,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_CIVILIZE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_IMPROVE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ENDWONDER,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_UNITS,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_TERRAIN,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_GOVT,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_GOVT_TITLE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_CARAVAN,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ORDERS,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_DIFFICULTY,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ATTITUDES,dest_player,0,0,0,0,0,0,0,0);
  return;
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the description of an
   offer (side kind 0x13; received: 0x14, the offer under review) in a
   read-only edit control filling the pane. */
void ParleyWindow::show_description(int received)
{
  EditControl *new_input;
  uptr desc_edit;
  uptr cur_x;
  int col_width;
  int cur_y;
  int block_height;
  tagRECT ctrl_rect;
  i8 *desc_font;
  u8 *desc_parent;

  build_description(received_offer,received);
  new_input = (EditControl *)(operator_new(0x48));
  if (new_input == (EditControl *)0x0) {
    desc_edit = 0;
  }
  else {
    desc_edit = (uptr)(edit_ctor(new_input));
  }
  this->desc_edit = desc_edit;
  cur_x = panel_left + panel_layout.margin_x;
  col_width = rect_width((RECT *)&panel_left);
  col_width += panel_layout.margin_x * -2;
  cur_y = panel_top + panel_layout.margin_y;
  block_height = divider_y - cur_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (large_layout == 0) {
    desc_font = ((i8 *)&font_size14);
  }
  else {
    desc_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((iptr)desc_font);
  if (this == (ParleyWindow *)0x0) {
    desc_parent = (u8 *)0x0;
  }
  else {
    desc_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)this->desc_edit)->create_styled((iptr)desc_parent,0x426,(iptr)&ctrl_rect,(const char *)&text_section_index_pool_block.parley_description,0x122
             ,0);
  ((EditControl *)this->desc_edit)->set_max_len(0x2000);
  ((EditControl *)this->desc_edit)->set_callback(parley_edit_escape_filter);
  ((Control *)this->desc_edit)->show_and_redraw();
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the chat pane (side kind
   1): the anonymous, mute and broadcast checkboxes and the send-to checkbox
   per civ. */
void ParleyWindow::add_chat_pane(u32 side_x)
{
  int tmp;
  iptr label;
  int header_x;
  Button *new_ctrl;
  uptr draw_x;
  i32 text_align;
  u8 *send_parent;
  u8 *mute_parent;
  u8 *broadcast_parent;
  u8 *mute_anon_parent;
  u8 *anon_parent;
  int chat_font_height;
  i32 send_checkbox;
  i32 mute_checkbox;
  i8 *right_header_font;
  i8 *left_header_font;
  uptr broadcast_checkbox;
  uptr mute_anon_checkbox;
  uptr anon_checkbox;
  i8 *chat_font;
  uptr left_col_x;
  uint shown_civ_count;
  int civ;
  u32 right_col_x;
  int has_full_intel;
  int checkbox_indent;
  int cur_y;
  uptr cur_x;
  int col_width;
  int block_height;
  
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (tmp = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), tmp == 0)
      ) && (game.reveal_map == '\0')) {
    has_full_intel = 0;
  }
  else {
    has_full_intel = 1;
  }
  right_col_x = side_x;
  left_col_x = panel_left + panel_layout.margin_x;
  cur_y = panel_top + panel_layout.margin_y;
  cur_x = left_col_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 3;
  if (large_layout == 0) {
    chat_font_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    chat_font_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + chat_font_height;
  if (large_layout == 0) {
    chat_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    chat_font = ((i8 *)&font_size24_bold);
  }
  ui_set_ctrl_font((iptr)chat_font);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    anon_checkbox = 0;
  }
  else {
    anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  anonymous_checkbox = anon_checkbox;
  if (this == (ParleyWindow *)0x0) {
    anon_parent = (u8 *)0x0;
  }
  else {
    anon_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2e9]));
  ((Checkbox *)anonymous_checkbox)->create_fixed((iptr)anon_parent,0x415,cur_x,cur_y,col_width,
             (const char *)label);
  ((Checkbox *)anonymous_checkbox)->set_checked((i32)chat_wnd_block.remain_anonymous);
  ((Checkbox *)anonymous_checkbox)->enable();
  ((Checkbox *)anonymous_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mute_anon_checkbox = 0;
  }
  else {
    mute_anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  mute_anonymous_checkbox = mute_anon_checkbox;
  if (this == (ParleyWindow *)0x0) {
    mute_anon_parent = (u8 *)0x0;
  }
  else {
    mute_anon_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x361]));
  ((Checkbox *)mute_anonymous_checkbox)->create_fixed((iptr)mute_anon_parent,0x430,col_width + cur_x,cur_y,
             col_width,(const char *)label);
  ((Checkbox *)mute_anonymous_checkbox)->set_checked((i32)chat_wnd_block.mute_anonymous);
  ((Checkbox *)mute_anonymous_checkbox)->enable();
  ((Checkbox *)mute_anonymous_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    broadcast_checkbox = 0;
  }
  else {
    broadcast_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  this->broadcast_checkbox = broadcast_checkbox;
  if (this == (ParleyWindow *)0x0) {
    broadcast_parent = (u8 *)0x0;
  }
  else {
    broadcast_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2ea]));
  ((Checkbox *)this->broadcast_checkbox)->create_fixed((iptr)broadcast_parent,0x416,col_width * 2 + cur_x,
             cur_y,col_width,(const char *)label);
  ((Checkbox *)this->broadcast_checkbox)->set_checked((i32)chat_wnd_block.broadcast_all);
  ((Checkbox *)this->broadcast_checkbox)->enable();
  ((Checkbox *)this->broadcast_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  cur_y += block_height;
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    left_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    left_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = left_col_x;
  tmp = cur_y;
  label = (iptr)(label_string(label_table.string_ids[0x34b]));
  draw_text((iptr)left_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    right_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    right_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  header_x = right_col_x - ((large_layout == 0) - 1 & 0x17);
  tmp = cur_y;
  label = (iptr)(label_string(label_table.string_ids[0x34b]));
  draw_text((iptr)right_header_font,(const char *)label,header_x,tmp,text_align);
  cur_x = left_col_x;
  if (large_layout == 0) {
    checkbox_indent = 5;
  }
  else {
    checkbox_indent = 0x17;
  }
  cur_y += block_height;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  shown_civ_count = 0;
  for (civ = 1; civ < 8; civ++) {
    if (((human_civ[0] == civ) ||
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) ||
       (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
        (((has_full_intel == 0 && (mp_options.chat_from_start == 0)) &&
         ((*(byte *)&civs[civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0)))))) {
      chat_wnd_block.send_to_civs[civ] = 0;
      chat_wnd_block.muted_civs[civ] = 0;
    }
    else {
      new_ctrl = (Button *)(operator_new(0x3c));
      if (new_ctrl == (Button *)0x0) {
        mute_checkbox = 0;
      }
      else {
        mute_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
      }
      mute_checkboxes[civ] = mute_checkbox;
      if (this == (ParleyWindow *)0x0) {
        mute_parent = (u8 *)0x0;
      }
      else {
        mute_parent = ((u8 *)this + 0x48);
      }
      ((Checkbox *)mute_checkboxes[civ])->create_fixed((iptr)mute_parent,
                 civ + 0x428,cur_x + checkbox_indent,cur_y,0x23,(const char *)&str_empty_167);
      ((Checkbox *)mute_checkboxes[civ])->set_checked(
                 (i32)chat_wnd_block.muted_civs[civ]);
      ((Checkbox *)mute_checkboxes[civ])->enable();
      ((Checkbox *)mute_checkboxes[civ])->set_callback((CheckboxToggleFn)chat_on_mute_checkbox);
      new_ctrl = (Button *)(operator_new(0x3c));
      if (new_ctrl == (Button *)0x0) {
        send_checkbox = 0;
      }
      else {
        send_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
      }
      send_checkboxes[civ] = send_checkbox;
      if (this == (ParleyWindow *)0x0) {
        send_parent = (u8 *)0x0;
      }
      else {
        send_parent = ((u8 *)this + 0x48);
      }
      label = civ_adjective(civ);
      ((Checkbox *)send_checkboxes[civ])->create_fixed((iptr)send_parent,
                 civ + 0x416,cur_x + checkbox_indent + 0x23,cur_y,
                 (col_width - 0x23) - checkbox_indent,(const char *)label);
      ((Checkbox *)send_checkboxes[civ])->set_checked(
                 (i32)chat_wnd_block.send_to_civs[civ]);
      ((Checkbox *)send_checkboxes[civ])->enable();
      ((Checkbox *)send_checkboxes[civ])->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
      shown_civ_count++;
      if ((shown_civ_count & 1) == 0) {
        cur_x = left_col_x;
      }
      else {
        cur_x = right_col_x;
      }
      if ((shown_civ_count & 1) == 0) {
        if (large_layout == 0) {
          checkbox_indent = 5;
        }
        else {
          checkbox_indent = 0x17;
        }
      }
      else if (large_layout == 0) {
        checkbox_indent = 5;
      }
      else {
        checkbox_indent = 0;
      }
      if ((shown_civ_count & 1) == 0) {
        cur_y += block_height;
      }
    }
  }
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the pane choosing what to
   do (side kind 2): the mode radio group under its title. */
void ParleyWindow::add_mode_pane(int side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr title_x;
  i32 text_align;
  u32 selected_mode;
  u8 *mode_radio_parent;
  int mode_title_height;
  i32 mode_radio;
  i8 *mode_radio_font;
  i8 *mode_title_font;
  int max_label_width;
  int label_width;
  iptr label_idx;
  iptr mode_labels[5];
  int cur_y;
  uptr cur_x;
  int col_width;
  u32 side_x;
  tagRECT ctrl_rect;
  int block_height;
  
  mode_labels[0] = (iptr)(label_string(label_table.string_ids[0x2e4]));
  mode_labels[1] = (iptr)(label_string(label_table.string_ids[0x2e5]));
  mode_labels[2] = (iptr)(label_string(label_table.string_ids[0x2e6]));
  mode_labels[3] = (iptr)(label_string(label_table.string_ids[0x2e7]));
  mode_labels[4] = (iptr)(label_string(label_table.string_ids[0x2e8]));
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    label = (iptr)(label_string(label_table.string_ids[0x2e3]));
    max_label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x2e3]));
    max_label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)label);
  }
  title_x = (panel_left + (int)(panel_right - panel_left) / 2) - max_label_width / 2;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    mode_title_font = ((i8 *)&font_size20);
  }
  else {
    mode_title_font = ((i8 *)&font_size30);
  }
  text_align = 5;
  cur_y = tmp;
  cur_x = title_x;
  label = (iptr)(label_string(label_table.string_ids[0x2e3]));
  draw_text((iptr)mode_title_font,(const char *)label,title_x,tmp,text_align);
  if (large_layout == 0) {
    max_label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)mode_labels[0]);
  }
  else {
    max_label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)mode_labels[0]);
  }
  for (label_idx = 1; label_idx < 5; label_idx++) {
    if (large_layout == 0) {
      label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)mode_labels[label_idx]);
    }
    else {
      label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)mode_labels[label_idx]);
    }
    if (max_label_width < label_width) {
      max_label_width = label_width;
    }
  }
  side_x = (panel_left + (int)(panel_right - panel_left) / 2) - max_label_width / 2;
  fill_left = side_x;
  if ((int)cur_x < (int)side_x) {
    side_x = cur_x;
    fill_left = cur_x;
  }
  if (large_layout == 0) {
    mode_radio_font = ((i8 *)&font_size20);
  }
  else {
    mode_radio_font = ((i8 *)&font_size30);
  }
  ctrl_set_pending_font((iptr)mode_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    mode_radio = 0;
  }
  else {
    mode_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = mode_radio;
  cur_x = side_x;
  col_width = max_label_width + max_label_width / 2;
  if (large_layout == 0) {
    mode_title_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    mode_title_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y * 2 + mode_title_height;
  block_height = divider_y - cur_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    mode_radio_parent = (u8 *)0x0;
  }
  else {
    mode_radio_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)mode_radio_parent,0x3f3,
             (iptr)&ctrl_rect,1,5,(iptr)mode_labels);
  if (selected_action == 0xffffffff) {
    selected_mode = 0;
  }
  else {
    selected_mode = selected_action;
  }
  ((RadioGroup *)side_radios[side])->set_selected(selected_mode);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the left side's exchange
   kind radio group (side kind 3). */
void ParleyWindow::add_exchange_kind_pane_left(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *exch_parent_l;
  int exch_title_height_l;
  i32 exch_radio_l;
  i8 *exch_radio_font_l;
  i8 *exch_title_font_l;
  i32 exch_labels_l[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  exch_labels_l[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  exch_labels_l[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  exch_labels_l[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  exch_labels_l[3] = (i32)(label_string(label_table.string_ids[750]));
  exch_labels_l[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  exch_labels_l[5] = (i32)(label_string(label_table.string_ids[0x2f0]));
  exch_labels_l[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    exch_title_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_title_font_l = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x303]));
  draw_text((iptr)exch_title_font_l,(const char *)label,text_x,tmp,text_align);
  if (large_layout == 0) {
    exch_radio_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_radio_font_l = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)exch_radio_font_l);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    exch_radio_l = 0;
  }
  else {
    exch_radio_l = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = exch_radio_l;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    exch_title_height_l = ((GameFont *)((i8 *)&font_size16))->font_cached_height();
  }
  else {
    exch_title_height_l = ((GameFont *)((i8 *)&font_size24))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y + exch_title_height_l;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  if (this == (ParleyWindow *)0x0) {
    exch_parent_l = (u8 *)0x0;
  }
  else {
    exch_parent_l = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)exch_parent_l,0x3f4,
             (iptr)&ctrl_rect,1,7,(iptr)exch_labels_l);
  if (exchange_kind[1] == 0) {
    ((RadioGroup *)side_radios[side])->disable_item(0);
  }
  else if (exchange_kind[1] == 6) {
    ((RadioGroup *)side_radios[side])->disable_item(6);
  }
  ((RadioGroup *)side_radios[side])->set_selected(exchange_kind[0]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_exchange_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the right side's exchange
   kind radio group (side kind 4). */
void ParleyWindow::add_exchange_kind_pane_right(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *exch_parent_r;
  int exch_title_height_r;
  i32 exch_radio_r;
  i8 *exch_radio_font_r;
  i8 *exch_title_font_r;
  i32 exch_labels_r[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  exch_labels_r[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  exch_labels_r[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  exch_labels_r[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  exch_labels_r[3] = (i32)(label_string(label_table.string_ids[750]));
  exch_labels_r[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  exch_labels_r[5] = (i32)(label_string(label_table.string_ids[0x2f1]));
  exch_labels_r[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    exch_title_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_title_font_r = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x304]));
  draw_text((iptr)exch_title_font_r,(const char *)label,text_x,tmp,text_align);
  if (large_layout == 0) {
    exch_radio_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_radio_font_r = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)exch_radio_font_r);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    exch_radio_r = 0;
  }
  else {
    exch_radio_r = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = exch_radio_r;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  cur_x = side_x;
  if (large_layout == 0) {
    exch_title_height_r = ((GameFont *)((i8 *)&font_size16))->font_cached_height();
  }
  else {
    exch_title_height_r = ((GameFont *)((i8 *)&font_size24))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y + exch_title_height_r;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  if (this == (ParleyWindow *)0x0) {
    exch_parent_r = (u8 *)0x0;
  }
  else {
    exch_parent_r = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)exch_parent_r,0x3f5,
             (iptr)&ctrl_rect,1,7,(iptr)exch_labels_r);
  if (exchange_kind[0] == 0) {
    ((RadioGroup *)side_radios[side])->disable_item(0);
  }
  else if (exchange_kind[0] == 6) {
    ((RadioGroup *)side_radios[side])->disable_item(6);
  }
  ((RadioGroup *)side_radios[side])->set_selected(exchange_kind[1]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_exchange_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the treaty radio group
   (side kind 5). */
void ParleyWindow::add_treaty_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  u8 *treaty_parent;
  int treaty_row_height;
  int treaty_row_height_alt;
  int treaty_title_height;
  iptr treaty_title;
  int treaty_line_height;
  int treaty_line_height_alt;
  int treaty_side_height;
  iptr treaty_side_title;
  i32 treaty_radio;
  i8 *treaty_radio_font;
  i8 *treaty_title_font;
  i8 *treaty_side_font;
  i32 treaty_labels[4];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  treaty_labels[0] = (i32)(label_string(label_table.string_ids[0x2f3]));
  treaty_labels[1] = (i32)(label_string(label_table.string_ids[0x2f4]));
  treaty_labels[2] = (i32)(label_string(label_table.string_ids[0x2f5]));
  treaty_labels[3] = (i32)(label_string(label_table.string_ids[0x2f6]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if ((is_exchange_side != '\0') || (is_demand_side != '\0')) {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      treaty_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      treaty_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)treaty_side_title);
    if (large_layout == 0) {
      treaty_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      treaty_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)treaty_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      treaty_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + treaty_side_height;
  }
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      treaty_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = treaty_line_height * 5 + panel_layout.margin_y * 4;
  }
  else {
    if (large_layout == 0) {
      treaty_line_height_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_line_height_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = panel_layout.margin_y * 3 + treaty_line_height_alt * 4;
  }
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (is_demand_side == '\0' && is_exchange_side == '\0') {
    treaty_title = (iptr)(label_string(label_table.string_ids[0x2e4]));
  }
  else {
    treaty_title = (iptr)(label_string(label_table.string_ids[0x342]));
  }
  if (large_layout == 0) {
    treaty_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    treaty_title_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)treaty_title_font,(const char *)treaty_title,cur_x,cur_y,5)
  ;
  if (large_layout == 0) {
    treaty_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    treaty_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)treaty_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    treaty_radio = 0;
  }
  else {
    treaty_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = treaty_radio;
  cur_x = side_x;
  if (large_layout == 0) {
    treaty_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    treaty_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + treaty_title_height;
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      treaty_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = panel_layout.margin_y * 3 + treaty_row_height * 4;
  }
  else {
    if (large_layout == 0) {
      treaty_row_height_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_row_height_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = treaty_row_height_alt * 3 + panel_layout.margin_y * 2;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    treaty_parent = (u8 *)0x0;
  }
  else {
    treaty_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)treaty_parent,0x3f6,
             (iptr)&ctrl_rect,1,(is_demand_side == '\0' && is_exchange_side == '\0') + 3,(iptr)treaty_labels);
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CEASEFIRE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(0),
     treaty_kind == 0)) {
    treaty_kind++;
  }
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_PEACE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(1),
     treaty_kind == 1)) {
    treaty_kind++;
  }
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_ALLIANCE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(2),
     treaty_kind == 2)) {
    if (is_demand_side == '\0' && is_exchange_side == '\0') {
      treaty_kind = 3;
    }
    else {
      treaty_kind = 0;
    }
  }
  if ((((is_exchange_side == '\0') && (is_demand_side == '\0')) &&
      ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) &&
     (((RadioGroup *)side_radios[side])->disable_item(3),
     treaty_kind == 3)) {
    treaty_kind = 0;
  }
  ((RadioGroup *)side_radios[side])->set_selected(treaty_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_treaty_type_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the framed status box
   (side kind 6). */
void ParleyWindow::add_status_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  uptr draw_x;
  i32 text_align;
  int status_box_offset;
  int status_header_height;
  int status_title_height;
  int status_line_height;
  i8 *status_text_font;
  i8 *status_header_font;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 - panel_layout.margin_x;
  if (large_layout == 0) {
    status_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  if (large_layout == 0) {
    status_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + status_line_height * 4 + status_title_height;
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    status_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + panel_layout.margin_y + status_header_height / 2;
  if (large_layout == 0) {
    status_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    status_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 0;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x2f7]));
  draw_text((iptr)status_header_font,(const char *)label,draw_x,tmp,text_align);
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  if (large_layout == 0) {
    status_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + status_box_offset;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  tmp = rect_height(&ctrl_rect);
  cur_y = ctrl_rect.top + tmp / 2;
  text_clear();
  if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_ALLIANCE) == 0) {
    if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_PEACE) == 0) {
      if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CEASEFIRE) == 0) {
        if ((*(byte *)(human_civ[0] * 0x594 + (iptr)((u8 *)&civs + 33) + other_civ * 4) & 0x20) == 0) {
          label = (iptr)(label_string(label_table.string_ids[0x2fb]));
          text_append((const char *)label);
        }
        else {
          label = (iptr)(label_string(label_table.string_ids[0x2fa]));
          text_append((const char *)label);
        }
      }
      else {
        label = (iptr)(label_string(label_table.string_ids[0x2f3]));
        text_append((const char *)label);
      }
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x2f9]));
      text_append((const char *)label);
    }
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x2f8]));
    text_append((const char *)label);
  }
  if (large_layout == 0) {
    status_text_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    status_text_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)status_text_font,(const char *)&textbuf.buf,cur_x,cur_y,0);
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the gift kind radio group
   (side kind 7). */
void ParleyWindow::add_gift_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr draw_x;
  i32 text_align;
  u8 *gift_parent;
  int gift_row_height;
  int gift_header_height;
  int gift_title_height;
  int gift_line_height;
  i32 gift_radio;
  i8 *gift_radio_font;
  i8 *gift_title_font;
  i32 gift_labels[5];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gift_labels[0] = (i32)(label_string(label_table.string_ids[0x2fd]));
  gift_labels[1] = (i32)(label_string(label_table.string_ids[0x2fe]));
  gift_labels[2] = (i32)(label_string(label_table.string_ids[0x2ff]));
  gift_labels[3] = (i32)(label_string(label_table.string_ids[0x300]));
  gift_labels[4] = (i32)(label_string(label_table.string_ids[0x301]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  if (large_layout == 0) {
    gift_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  if (large_layout == 0) {
    gift_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 5 + gift_line_height * 5 + gift_title_height;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (large_layout == 0) {
    gift_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gift_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x2fc]));
  draw_text((iptr)gift_title_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    gift_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gift_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)gift_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    gift_radio = 0;
  }
  else {
    gift_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = gift_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    gift_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + gift_header_height;
  if (large_layout == 0) {
    gift_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = gift_row_height * 5 + panel_layout.margin_y * 4;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    gift_parent = (u8 *)0x0;
  }
  else {
    gift_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)gift_parent,0x3f7,
             (iptr)&ctrl_rect,1,5,(iptr)gift_labels);
  ((RadioGroup *)side_radios[side])->set_selected(gift_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_gift_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the advance list (side
   kind 8). */
void ParleyWindow::add_tech_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  uptr draw_x;
  i32 text_align;
  int tech_header_height;
  int tech_side_height;
  iptr tech_side_title;
  i8 *tech_header_font;
  i8 *tech_side_font;
  i32 tech_list_pick_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 7;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      tech_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      tech_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)tech_side_title);
    if (large_layout == 0) {
      tech_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      tech_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)tech_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      tech_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      tech_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + tech_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    tech_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    tech_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + tech_header_height / 2;
  if (large_layout == 0) {
    tech_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    tech_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 0;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x302]));
  draw_text((iptr)tech_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  tech_list_pick_mode = 0;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    tech_list_pick_mode = 1;
  }
  create_advance_list_pane(&ctrl_rect,side,tech_list_pick_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the gold amount edit
   (side kind 9). */
void ParleyWindow::add_gold_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  int tmp;
  EditControl *new_input;
  iptr label;
  u8 *gold_edit_parent;
  int gold_prompt_width;
  int gold_edit_text_width;
  int gold_box_offset;
  int gold_header_height;
  iptr gold_side_title;
  int gold_title_height;
  int gold_line_height;
  i8 *gold_edit_font;
  i32 gold_edit;
  i8 *gold_prompt_font;
  i8 *gold_header_font;
  i8 *gold_side_font;
  char gold_text[32];
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    gold_line_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    gold_line_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    gold_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + gold_line_height * 4 + gold_title_height;
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      gold_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      gold_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)gold_side_title);
    if (large_layout == 0) {
      gold_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gold_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)gold_side_font,(const char *)&textbuf.buf,cur_x,
               panel_top + panel_layout.margin_y,5);
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    gold_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + gold_header_height / 2;
  str_clear((char *)&textbuf.buf);
  if (is_demand_side == '\0') {
    label = (iptr)(label_string(label_table.string_ids[0x306]));
    text_append_str((const char *)label);
    text_append_space();
    text_append_int(civs[human_civ[0]].gold);
    text_append_space();
    label = (iptr)(label_string(label_table.string_ids[0x307]));
    text_append_str((const char *)label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x305]));
    text_append_str((const char *)label);
  }
  if (large_layout == 0) {
    gold_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)gold_header_font,(const char *)&textbuf.buf,cur_x,cur_y,0);
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  if (large_layout == 0) {
    gold_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + gold_box_offset;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  str_clear((char *)&textbuf.buf);
  text_append_str((const char *)&str_000000);
  if (large_layout == 0) {
    gold_edit_text_width = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
  }
  else {
    gold_edit_text_width = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
  }
  col_width = (gold_edit_text_width * 3) / 2;
  str_clear((char *)&textbuf.buf);
  label = (iptr)(label_string(label_table.string_ids[0x308]));
  text_append_str((const char *)label);
  text_append_space();
  if (large_layout == 0) {
    gold_prompt_width = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
  }
  else {
    gold_prompt_width = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
  }
  tmp = rect_width(&ctrl_rect);
  cur_x = (ctrl_rect.left + tmp / 2) - (col_width + gold_prompt_width) / 2;
  tmp = rect_height(&ctrl_rect);
  cur_y = ctrl_rect.top + tmp / 2;
  if (large_layout == 0) {
    gold_prompt_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_prompt_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)gold_prompt_font,(const char *)&textbuf.buf,cur_x,cur_y,1);
  new_input = (EditControl *)(operator_new(0x48));
  if (new_input == (EditControl *)0x0) {
    gold_edit = 0;
  }
  else {
    gold_edit = (i32)(edit_ctor(new_input));
  }
  gold_edits[side] = gold_edit;
  if (large_layout == 0) {
    gold_edit_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_edit_font = ((i8 *)&font_size24_bold);
  }
  ui_set_default_font((iptr)gold_edit_font);
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
    cur_x += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
    cur_x += tmp;
  }
  if (large_layout == 0) {
    block_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    block_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y -= block_height / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  __itoa((int)gold_amount[side],gold_text,10);
  if (this == (ParleyWindow *)0x0) {
    gold_edit_parent = (u8 *)0x0;
  }
  else {
    gold_edit_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)gold_edits[side])->create((iptr)gold_edit_parent,
             side + 0x3f8,(iptr)&ctrl_rect,(const char *)gold_text);
  ((EditControl *)gold_edits[side])->set_max_len(6);
  ((EditControl *)gold_edits[side])->set_change_callback(parley_gold_edit_on_change);
  ((Control *)gold_edits[side])->set_focus();
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the unit list with its
   sort, hide and reveal buttons (side kind 10). */
void ParleyWindow::add_unit_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  Button *new_ctrl;
  uptr draw_x;
  i32 text_align;
  u8 *sort_city_parent;
  u8 *sort_unit_parent;
  int unit_line_spacing;
  int unit_btn_offset;
  u8 *hide_parent;
  iptr hide_label;
  u8 *reveal_parent;
  iptr reveal_label;
  int unit_line_spacing_alt;
  int unit_btn_offset_alt;
  int unit_header_height;
  int unit_side_height;
  iptr unit_side_title;
  i32 sort_city_button;
  i32 sort_unit_button;
  i32 hide_button;
  i32 reveal_button;
  i8 *unit_header_font;
  i8 *unit_side_font;
  tagRECT btn_rect;
  int btn_area_bottom;
  int btn_width;
  int btn_top;
  int btn_height;
  i32 unit_list_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 5;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      unit_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      unit_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)unit_side_title);
    if (large_layout == 0) {
      unit_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      unit_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)unit_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      unit_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + unit_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    unit_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    unit_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + unit_header_height / 2;
  cur_y = tmp;
  if (is_exchange_side == '\0') {
    if (large_layout == 0) {
      unit_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      unit_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 0;
    draw_x = cur_x;
    label = (iptr)(label_string(label_table.string_ids[0x309]));
    draw_text((iptr)unit_header_font,(const char *)label,draw_x,tmp,text_align);
  }
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      unit_btn_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_btn_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    btn_top = ctrl_rect.top + unit_btn_offset;
    btn_width = col_width / 2;
    if (large_layout == 0) {
      unit_line_spacing = ((GameFont *)((i8 *)&font_size16))->line_spacing();
    }
    else {
      unit_line_spacing = ((GameFont *)((i8 *)&font_size24))->line_spacing();
    }
    btn_height = panel_layout.padding_y * 2 + unit_line_spacing;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
  }
  else {
    btn_top = panel_layout.margin_y * 2 + ctrl_rect.top - 1;
    if (large_layout == 0) {
      unit_btn_offset_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_btn_offset_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    if (large_layout == 0) {
      unit_line_spacing_alt = ((GameFont *)((i8 *)&font_size16))->line_spacing();
    }
    else {
      unit_line_spacing_alt = ((GameFont *)((i8 *)&font_size24))->line_spacing();
    }
    btn_area_bottom = ctrl_rect.top + panel_layout.padding_y * 2 + unit_btn_offset_alt + unit_line_spacing_alt;
    btn_width = col_width + panel_layout.margin_x * -3;
    btn_height = (btn_area_bottom - btn_top) / 2;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      reveal_button = 0;
    }
    else {
      reveal_button = (i32)(button_ctor(new_ctrl));
    }
    info_show_buttons[side] = reveal_button;
    if (side == 0) {
      reveal_label = (iptr)(label_string(label_table.string_ids[0x36f]));
    }
    else {
      reveal_label = (iptr)(label_string(label_table.string_ids[0x370]));
    }
    if (this == (ParleyWindow *)0x0) {
      reveal_parent = (u8 *)0x0;
    }
    else {
      reveal_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_show_buttons[side])->create((iptr)reveal_parent,
               side + 0x404,(iptr)&btn_rect,(const char *)reveal_label);
    ((Button *)info_show_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_reveal_city_origin_clicked : parley_request_city_origin_clicked));
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      hide_button = 0;
    }
    else {
      hide_button = (i32)(button_ctor(new_ctrl));
    }
    info_hide_buttons[side] = hide_button;
    if (side == 0) {
      hide_label = (iptr)(label_string(label_table.string_ids[0x371]));
    }
    else {
      hide_label = (iptr)(label_string(label_table.string_ids[0x370]));
    }
    if (this == (ParleyWindow *)0x0) {
      hide_parent = (u8 *)0x0;
    }
    else {
      hide_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_hide_buttons[side])->create((iptr)hide_parent,
               side + 0x406,(iptr)&btn_rect,(const char *)hide_label);
    ((Button *)info_hide_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_hide_city_origin_clicked : parley_request_city_origin_clicked));
    if ((side == 0) && (local_units_revealed != '\0')) {
      ((Control *)info_show_buttons[0])->hide();
      ((Control *)info_hide_buttons[0])->show_and_redraw();
    }
    else if (side == 0) {
      ((Control *)info_show_buttons[0])->show_and_redraw();
      ((Control *)info_hide_buttons[0])->hide();
    }
    if ((side == 1) && (remote_units_revealed != '\0')) {
      ((Control *)info_show_buttons[1])->hide();
      ((Control *)info_hide_buttons[1])->show_and_redraw();
      ((Button *)info_hide_buttons[1])->disable();
    }
    else if (side == 1) {
      ((Control *)info_show_buttons[1])->show_and_redraw();
      ((Control *)info_hide_buttons[1])->hide();
    }
    btn_top += btn_height;
    btn_width = col_width / 2;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
  }
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    sort_unit_button = 0;
  }
  else {
    sort_unit_button = (i32)(button_ctor(new_ctrl));
  }
  sort_by_unit_buttons[side] = sort_unit_button;
  if (this == (ParleyWindow *)0x0) {
    sort_unit_parent = (u8 *)0x0;
  }
  else {
    sort_unit_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x30a]));
  ((Button *)sort_by_unit_buttons[side])->create((iptr)sort_unit_parent,
             side + 0x400,(iptr)&btn_rect,(const char *)label);
  ((Button *)sort_by_unit_buttons[side])->set_callback(
             (CtrlNotifyFn)(side == 0 ? parley_sort_by_unit_left_clicked : parley_sort_by_unit_right_clicked));
  if (((int)parley_wnd_block.mode != 9) &&
     (((side == 0 && (local_units_revealed == '\0')) ||
      ((side == 1 && (remote_units_revealed == '\0')))))) {
    ((Button *)sort_by_unit_buttons[side])->disable();
  }
  SetRect(&btn_rect,btn_rect.right,btn_top,btn_rect.right + btn_width + panel_layout.margin_x * -3,
          btn_top + btn_height);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    sort_city_button = 0;
  }
  else {
    sort_city_button = (i32)(button_ctor(new_ctrl));
  }
  sort_by_city_buttons[side] = sort_city_button;
  if (this == (ParleyWindow *)0x0) {
    sort_city_parent = (u8 *)0x0;
  }
  else {
    sort_city_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x30b]));
  ((Button *)sort_by_city_buttons[side])->create((iptr)sort_city_parent,
             side + 0x402,(iptr)&btn_rect,(const char *)label);
  ((Button *)sort_by_city_buttons[side])->set_callback(
             (CtrlNotifyFn)(side == 0 ? parley_sort_by_city_left_clicked : parley_sort_by_city_right_clicked));
  if (((int)parley_wnd_block.mode != 9) &&
     (((side == 0 && (local_units_revealed == '\0')) ||
      ((side == 1 && (remote_units_revealed == '\0')))))) {
    ((Button *)sort_by_city_buttons[side])->disable();
  }
  ctrl_rect.top = btn_rect.bottom;
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  unit_list_mode = 1;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    unit_list_mode = 0;
  }
  create_unit_list_pane(&ctrl_rect,side,unit_list_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the proposal radio group
   (side kind 0xb). */
void ParleyWindow::add_proposal_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *proposal_parent;
  int proposal_row_height;
  int proposal_title_height;
  int proposal_line_height;
  i32 proposal_radio;
  i8 *proposal_radio_font;
  i8 *proposal_title_font;
  i32 proposal_labels[2];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    proposal_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = proposal_line_height * 3 + panel_layout.margin_y * 2;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  cur_x = side_x;
  if (large_layout == 0) {
    proposal_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    proposal_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x30c]));
  draw_text((iptr)proposal_title_font,(const char *)label,text_x,tmp,text_align);
  proposal_labels[0] = (i32)(label_string(label_table.string_ids[0x30d]));
  proposal_labels[1] = (i32)(label_string(label_table.string_ids[0x30e]));
  if (large_layout == 0) {
    proposal_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    proposal_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)proposal_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    proposal_radio = 0;
  }
  else {
    proposal_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = proposal_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    proposal_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + proposal_title_height;
  if (large_layout == 0) {
    proposal_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = proposal_row_height * 2 + panel_layout.margin_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    proposal_parent = (u8 *)0x0;
  }
  else {
    proposal_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)proposal_parent,0x40c,
             (iptr)&ctrl_rect,1,2,(iptr)proposal_labels);
  ((RadioGroup *)side_radios[side])->set_selected(proposal_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_proposal_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the civ list (side kind
   0xc). */
void ParleyWindow::add_civ_list_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  int civlist_item_height;
  int civlist_box_offset;
  int civlist_header_height;
  int civlist_title_height;
  int civlist_row_height;
  int civlist_side_height;
  iptr civlist_side_title;
  i8 *civlist_header_font;
  i8 *civlist_side_font;
  iptr civlist_title_idx;
  iptr civlist_titles[4];
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  civlist_titles[0] = (iptr)(label_string(label_table.string_ids[0x314]));
  civlist_titles[1] = (iptr)(label_string(label_table.string_ids[0x315]));
  civlist_titles[2] = (iptr)(label_string(label_table.string_ids[0x316]));
  civlist_titles[3] = (iptr)(label_string(label_table.string_ids[0x343]));
  if (is_exchange_side == '\0') {
    if (is_demand_side == '\0') {
      civlist_title_idx = 0;
    }
    else {
      civlist_title_idx = 3;
    }
  }
  else if (side == 0) {
    civlist_title_idx = 1;
  }
  else {
    civlist_title_idx = 2;
  }
  flag_ptr = (u8 *)&list_rows[side];
  flag_ptr[0] = 5;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      civlist_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      civlist_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)civlist_side_title);
    if (large_layout == 0) {
      civlist_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      civlist_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)civlist_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      civlist_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      civlist_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + civlist_side_height;
  }
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    civlist_row_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    civlist_row_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    civlist_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = list_rows[side] * civlist_row_height +
             list_rows[side] * panel_layout.margin_y + panel_layout.margin_y * 4 +
             civlist_title_height;
  cur_y += (int)((divider_y - cur_y) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    civlist_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + civlist_header_height / 2;
  if (large_layout == 0) {
    civlist_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    civlist_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)civlist_header_font,(const char *)civlist_titles[civlist_title_idx],cur_x,cur_y,0);
  if (large_layout == 0) {
    civlist_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + civlist_box_offset;
  if (large_layout == 0) {
    civlist_item_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    civlist_item_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  ctrl_rect.bottom =
       list_rows[side] * civlist_item_height +
       list_rows[side] * panel_layout.margin_y + ctrl_rect.top + 2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  build_civ_list(&ctrl_rect,side);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the map radio group (side
   kind 0xd). */
void ParleyWindow::add_map_pane(int side,u32 side_x,char is_exchange_side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  u8 *map_parent;
  int map_box_offset;
  int map_header_height;
  int map_title_height;
  int map_row_height;
  int map_side_height;
  iptr map_side_title;
  i32 map_radio;
  i8 *map_radio_font;
  i8 *map_header_font;
  i8 *map_side_font;
  uptr map_title_idx;
  iptr map_labels[5];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  map_labels[0] = (iptr)(label_string(label_table.string_ids[0x30f]));
  map_labels[1] = (iptr)(label_string(label_table.string_ids[0x310]));
  map_title_idx = 0;
  map_labels[2] = (iptr)(label_string(label_table.string_ids[0x311]));
  map_labels[3] = (iptr)(label_string(label_table.string_ids[0x312]));
  map_labels[4] = (iptr)(label_string(label_table.string_ids[0x313]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  if (is_exchange_side != '\0') {
    map_title_idx = (uptr)(side != 0);
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      map_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      map_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)map_side_title);
    if (large_layout == 0) {
      map_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      map_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)map_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      map_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      map_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + map_side_height;
  }
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    map_row_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    map_row_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    map_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = map_row_height * 3 + panel_layout.margin_y * 9 + map_title_height;
  cur_y += (int)((divider_y - cur_y) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    map_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + map_header_height / 2;
  if (large_layout == 0) {
    map_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    map_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((iptr)map_header_font,(const char *)map_labels[map_title_idx],cur_x,cur_y,0);
  if (large_layout == 0) {
    map_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + map_box_offset;
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  if (large_layout == 0) {
    map_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    map_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)map_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    map_radio = 0;
  }
  else {
    map_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = map_radio;
  if (this == (ParleyWindow *)0x0) {
    map_parent = (u8 *)0x0;
  }
  else {
    map_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)map_parent,side + 0x424,
             (iptr)&ctrl_rect,1,3,(iptr)(map_labels + 2));
  ((RadioGroup *)side_radios[side])->set_selected(
             (i32)map_choice[side]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_map_choice_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the offer radio group
   (side kind 0xf). */
void ParleyWindow::add_offer_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr draw_x;
  i32 text_align;
  u8 *offer_parent;
  int offer_row_height;
  int offer_header_height;
  int offer_title_height;
  int offer_line_height;
  i32 offer_radio;
  i8 *offer_radio_font;
  i8 *offer_header_font;
  i8 *offer_title_font;
  i32 offer_labels[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  offer_labels[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  offer_labels[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  offer_labels[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  offer_labels[3] = (i32)(label_string(label_table.string_ids[750]));
  offer_labels[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  offer_labels[5] = (i32)(label_string(label_table.string_ids[0x2f1]));
  offer_labels[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  if (large_layout == 0) {
    offer_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 7 + offer_line_height * 8;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (large_layout == 0) {
    offer_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x317]));
  draw_text((iptr)offer_title_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    offer_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + panel_layout.margin_y * 2 + offer_title_height;
  if (large_layout == 0) {
    offer_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x318]));
  draw_text((iptr)offer_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    offer_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + offer_header_height;
  if (large_layout == 0) {
    offer_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((iptr)offer_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    offer_radio = 0;
  }
  else {
    offer_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = offer_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    offer_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 5 + offer_row_height * 6;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    offer_parent = (u8 *)0x0;
  }
  else {
    offer_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((iptr)offer_parent,0x41f,
             (iptr)&ctrl_rect,1,7,(iptr)offer_labels);
  ((RadioGroup *)side_radios[side])->set_selected(demand_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_demand_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the city list with its
   hide and reveal buttons (side kind 0x11). */
void ParleyWindow::add_city_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  Button *new_ctrl;
  u8 *city_hide_parent;
  iptr city_hide_label;
  u8 *city_reveal_parent;
  iptr city_reveal_label;
  int city_btn_font_height;
  int city_header_height;
  int city_side_height;
  iptr city_side_title;
  i8 *city_header_font;
  i32 city_hide_button;
  i32 city_reveal_button;
  i8 *city_side_font;
  tagRECT city_btn_rect;
  int city_btn_width;
  int city_btn_top;
  iptr city_header_label;
  int city_btn_height;
  i32 city_list_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 7;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      city_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      city_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)city_side_title);
    if (large_layout == 0) {
      city_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      city_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)city_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      city_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      city_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + city_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target((iptr)&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    city_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    city_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y += city_header_height / 2;
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    city_header_label = (iptr)(label_string(label_table.string_ids[0x31a]));
    if (large_layout == 0) {
      city_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      city_header_font = ((i8 *)&font_size24_bold);
    }
    draw_text((iptr)city_header_font,(const char *)city_header_label,cur_x,cur_y,0);
  }
  else {
    city_btn_top = panel_layout.margin_y * 2 + ctrl_rect.top - 1;
    city_btn_width = col_width + panel_layout.margin_x * -3;
    if (large_layout == 0) {
      city_btn_font_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      city_btn_font_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    city_btn_height = city_btn_font_height - (panel_layout.margin_y * 2 - 1);
    SetRect(&city_btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,city_btn_top,ctrl_rect.left + city_btn_width,
            city_btn_height + city_btn_top);
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      city_reveal_button = 0;
    }
    else {
      city_reveal_button = (i32)(button_ctor(new_ctrl));
    }
    info_show_buttons[side] = city_reveal_button;
    if (side == 0) {
      city_reveal_label = (iptr)(label_string(label_table.string_ids[0x372]));
    }
    else {
      city_reveal_label = (iptr)(label_string(label_table.string_ids[0x373]));
    }
    if (this == (ParleyWindow *)0x0) {
      city_reveal_parent = (u8 *)0x0;
    }
    else {
      city_reveal_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_show_buttons[side])->create((iptr)city_reveal_parent,
               side + 0x408,(iptr)&city_btn_rect,(const char *)city_reveal_label);
    ((Button *)info_show_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_reveal_city_info : parley_request_city_info));
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      city_hide_button = 0;
    }
    else {
      city_hide_button = (i32)(button_ctor(new_ctrl));
    }
    info_hide_buttons[side] = city_hide_button;
    if (side == 0) {
      city_hide_label = (iptr)(label_string(label_table.string_ids[0x374]));
    }
    else {
      city_hide_label = (iptr)(label_string(label_table.string_ids[0x373]));
    }
    if (this == (ParleyWindow *)0x0) {
      city_hide_parent = (u8 *)0x0;
    }
    else {
      city_hide_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_hide_buttons[side])->create((iptr)city_hide_parent,
               side + 0x40a,(iptr)&city_btn_rect,(const char *)city_hide_label);
    ((Button *)info_hide_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_hide_city_info : parley_request_city_info));
    if ((side == 0) && (local_cities_revealed != '\0')) {
      ((Control *)info_show_buttons[0])->hide();
      ((Control *)info_hide_buttons[0])->show_and_redraw();
    }
    else if (side == 0) {
      ((Control *)info_show_buttons[0])->show_and_redraw();
      ((Control *)info_hide_buttons[0])->hide();
    }
    if ((side == 1) && (remote_cities_revealed != '\0')) {
      ((Control *)info_show_buttons[1])->hide();
      ((Control *)info_hide_buttons[1])->show_and_redraw();
      ((Button *)info_hide_buttons[1])->disable();
    }
    else if (side == 1) {
      ((Control *)info_show_buttons[1])->show_and_redraw();
      ((Control *)info_hide_buttons[1])->hide();
    }
  }
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate((iptr)&ctrl_rect,1,1);
  fill_rect((iptr)&ctrl_rect,dlg_style.list_bg);
  city_list_mode = 1;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    city_list_mode = 0;
  }
  create_city_list_pane(&ctrl_rect,side,city_list_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the
   waiting-for-the-other-civ text (side kind 0x12). */
void ParleyWindow::add_waiting_pane()
{
  iptr label;
  int waiting_font_height;
  i8 *waiting_font;
  int waiting_text_width;
  i8 waiting_text[80];
  int cur_y;
  uptr cur_x;
  
  label = civ_get_adjective(other_civ);
  text_set_string_arg_b(0,(const char *)label);
  text_clear();
  text_append_label(0x2e2);
  text_expand_vars((iptr)&textbuf.buf,(iptr)waiting_text);
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    waiting_text_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)waiting_text);
  }
  else {
    waiting_text_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)waiting_text);
  }
  cur_x = (panel_left + (int)(panel_right - panel_left) / 2) - waiting_text_width / 2;
  if (large_layout == 0) {
    waiting_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    waiting_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  cur_y = (panel_top + (int)(divider_y - panel_top) / 2 + panel_layout.margin_y) -
             waiting_font_height / 2;
  if (large_layout == 0) {
    waiting_font = ((i8 *)&font_size20);
  }
  else {
    waiting_font = ((i8 *)&font_size30);
  }
  draw_text((iptr)waiting_font,(const char *)waiting_text,cur_x,cur_y,5);
  ((Control *)mode2_button)->hide();
  ((Button *)mode2_button)->disable();
  ((Control *)mode_button)->show_and_redraw();
  ((Button *)mode_button)->disable();
  ((Button *)never_mind_button)->disable();
  if ((local_units_revealed == '\0') && (local_cities_revealed == '\0')) {
    ((Control *)conceal_button)->hide();
    ((Button *)conceal_button)->disable();
    ((Control *)this->reveal_button)->show_and_redraw();
    ((Button *)this->reveal_button)->enable();
  }
  else {
    ((Control *)conceal_button)->show_and_redraw();
    ((Button *)conceal_button)->enable();
    ((Control *)this->reveal_button)->hide();
    ((Button *)this->reveal_button)->disable();
  }
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the offer under review
   (side kind 0x14): its description, and Accept enabled only if we have every
   advance it asks for. */
void ParleyWindow::add_review_pane()
{
  int tmp;
  iptr label;
  ParleyOfferMsg *item_buf;
  int *item_kind_ptr;
  iptr *tech_ptr;
  int tech_idx;
  int tech_count;
  
  ((SurfaceWindow *)((i8 *)&parley_wnd_block))->SurfaceWindow::show();
  show_description(1);
  parley_wnd_block.offer_acceptable = 1;
  if (received_offer != 0) {
    item_buf = received_offer;
    item_kind_ptr = (int *)((uptr)item_buf + 0x20);
    if (((received_offer->mode == 0xf) && (*item_kind_ptr == 8)) ||
       ((received_offer->mode == 0xe && (*item_kind_ptr == 8)))) {
      tech_count = item_buf->item.value;
      tech_ptr = (iptr *)((uptr)item_buf + 0x2c);
      for (tech_idx = 0; tech_idx < tech_count; tech_idx++) {
        label = *tech_ptr;
        tech_ptr++;
        tmp = civ_has_advance(human_civ[0],label);
        if (tmp == 0) {
          offer_acceptable = 0;
        }
      }
    }
  }
  if (offer_acceptable == 0) {
    ((Button *)accept_button)->disable();
  }
  else {
    ((Button *)accept_button)->enable();
  }
}

/* 00526ca0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::add_side_dialog(int side_kind,int side)

{
  u8 *flag_ptr;
  int tmp;
  char is_demand_side;
  u32 side_x;
  char is_exchange_side;
  
  if ((side != 0) && (side != 1)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_leftright_left_leftright_right_2,(iptr)&str_d_ss_franklinton_parleywin_add_dialog_cp,0xc);
  }
  if (side_kind < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_thedialog_pdt_null_pdt_null_pdt_count,(iptr)&str_d_ss_franklinton_parleywin_add_dialog_cp_2,0xd);
  }
  is_exchange_side = '\0';
  is_demand_side = '\0';
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = '\0';
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
  if (side == 0) {
    side_x = panel_left + panel_layout.margin_x;
  }
  else if (side == 1) {
    tmp = rect_width((RECT *)&panel_left);
    side_x = panel_left + tmp / 2 + panel_layout.margin_x;
  }
  if (side_kind == 0xe) {
    is_exchange_side = 1;
    switch((i32)exchange_kind[side]) {
    case 0:
      side_kind = 9;
      break;
    case 1:
      side_kind = 8;
      break;
    case 2:
      side_kind = 10;
      break;
    case 3:
      side_kind = 0x11;
      break;
    case 4:
      side_kind = 0xd;
      break;
    case 5:
      side_kind = 0xc;
      break;
    case 6:
      side_kind = 5;
    }
  }
  else if (side_kind == 0x10) {
    is_demand_side = 1;
    switch(demand_kind) {
    case 0:
      side_kind = 9;
      break;
    case 1:
      side_kind = 8;
      break;
    case 2:
      side_kind = 10;
      break;
    case 3:
      side_kind = 0x11;
      break;
    case 4:
      side_kind = 0xd;
      break;
    case 5:
      side_kind = 0xc;
      break;
    case 6:
      side_kind = 5;
    }
  }
  clear_side_controls(side);
  this->side_kind[side] = side_kind;
  if (((parley_wnd_block.closed == 0) && (side_kind != 1)) && (side_kind != 0)) {
    if (side_kind == 0x14) {
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)never_mind_button)->hide();
      ((Control *)accept_button)->show_and_redraw();
      ((Control *)intel_button)->hide();
      ((Control *)no_thanks_button)->show_and_redraw();
      ((Control *)counter_button)->show_and_redraw();
    }
    else {
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->enable();
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)never_mind_button)->show_and_redraw();
      ((Control *)accept_button)->hide();
      ((Control *)intel_button)->show_and_redraw();
      ((Control *)no_thanks_button)->hide();
      ((Control *)counter_button)->hide();
    }
  }
  switch(side_kind) {
  case 0:
    break;
  case 1:
    add_chat_pane(side_x);
    break;
  case 2:
    add_mode_pane(side);
    break;
  case 3:
    add_exchange_kind_pane_left(side,side_x);
    break;
  case 4:
    add_exchange_kind_pane_right(side,side_x);
    break;
  case 5:
    add_treaty_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 6:
    add_status_pane(side,side_x);
    break;
  case 7:
    add_gift_pane(side,side_x);
    break;
  case 8:
    add_tech_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 9:
    add_gold_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 10:
    add_unit_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0xb:
    add_proposal_pane(side,side_x);
    break;
  case 0xc:
    add_civ_list_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0xd:
    add_map_pane(side,side_x,is_exchange_side);
    break;
  case 0xf:
    add_offer_pane(side,side_x);
    break;
  case 0x11:
    add_city_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0x12:
    add_waiting_pane();
    break;
  case 0x13:
    show_description(0);
    break;
  case 0x14:
    add_review_pane();
  }
  update_buttons();
  wnd_blit_surface_rect((iptr)&fill_left);
  return;
}

/* 0052d4d2 auto */
void __cdecl parley_on_treaty_type_select(i32 ctrl_id,i32 treaty_type)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((iptr)parley + 0x48))->activate();
  parley->treaty_kind = treaty_type;
  return;
}

/* 0052d523 auto */
void __cdecl parley_on_demand_kind_select(i32 ctrl_id,u32 demand_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  this2->demand_kind = demand_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d588 auto */
void __cdecl parley_on_map_choice_select(int side,i32 map_choice)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((iptr)parley + 0x48))->activate();
  *(i32 *)((iptr)parley - 0xe6c + side * 4) = map_choice;
  return;
}

/* 0052d5dd auto */
void __cdecl parley_on_gift_kind_select(i32 ctrl_id,u32 gift_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  this2->gift_kind = gift_kind;
  switch(gift_kind) {
  case 0:
    this2->mode = 7;
    break;
  case 1:
    this2->mode = 8;
    break;
  case 2:
    this2->mode = 9;
    break;
  case 3:
    this2->mode = 10;
    break;
  case 4:
    this2->mode = 0xb;
  }
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d6ff auto */
void __cdecl parley_on_proposal_kind_select(i32 ctrl_id,u32 proposal_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  this2->proposal_kind = proposal_kind;
  if (proposal_kind == 0) {
    this2->mode = 0xc;
  }
  else if (proposal_kind == 1) {
    this2->mode = 0xd;
  }
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d7dc auto */
void __cdecl parley_on_exchange_kind_select(int ctrl_id,i32 exchange_kind)

{
  iptr active_child;
  int selected;
  ParleyWindow *parley;
  uint is_right_side;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((iptr)parley + 0x48))->activate();
  is_right_side = (uint)(ctrl_id == 0x3f4);
  switch(exchange_kind) {
  case 0:
    selected = ((RadioGroup *)parley->side_radios[is_right_side])->get_selected();
    ((RadioGroup *)parley->side_radios[is_right_side])->disable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(6);
    ((RadioGroup *)parley->side_radios[0])->enable_item(6);
    if (selected == 0) {
      ((RadioGroup *)parley->side_radios[is_right_side])->set_selected(1);
    }
    break;
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
    ((RadioGroup *)parley->side_radios[0])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(6);
    ((RadioGroup *)parley->side_radios[0])->enable_item(6);
    break;
  case 6:
    selected = ((RadioGroup *)parley->side_radios[is_right_side])->get_selected();
    ((RadioGroup *)parley->side_radios[0])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(0);
    ((RadioGroup *)parley->side_radios[is_right_side])->disable_item(6);
    if (selected == 6) {
      ((RadioGroup *)parley->side_radios[is_right_side])->set_selected(0);
    }
  }
  return;
}

/* 0052d9a1 auto */
void __cdecl parley_on_radio_dblclick(i32 ctrl_id)

{
  iptr active_child;
  int enabled;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  enabled = ((Button *)parley->mode_button)->ctrl_is_enabled();
  if ((enabled != 0) ||
     (enabled = ((Button *)parley->mode2_button)->ctrl_is_enabled(), enabled != 0)) {
    ((Wnd *)((u8 *)parley + 0x48))->activate();
    parley_on_mode_chosen(ctrl_id);
  }
  return;
}

/* 0052da23 auto */
void __cdecl parley_on_conceal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 0;
  parley->local_cities_revealed = 0;
  net_send_message(NM_HIDE_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(NM_HIDE_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  ((Control *)parley->conceal_button)->hide();
  ((Button *)parley->conceal_button)->disable();
  ((Control *)parley->reveal_button)->show_and_redraw();
  ((Button *)parley->reveal_button)->enable();
  return;
}

/* 0052db3d auto */
void __cdecl parley_on_reveal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  wnd_get_active_child();
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 1;
  parley->local_cities_revealed = 1;
  net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  ((Control *)parley->conceal_button)->show_and_redraw();
  ((Button *)parley->conceal_button)->enable();
  ((Control *)parley->reveal_button)->hide();
  ((Button *)parley->reveal_button)->disable();
  return;
}

/* 0052dc7e auto */
void __cdecl parley_on_check_intel(void)

{
  bool has_intel_wonder;
  iptr active_child;
  int has_wonder;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS);
  has_intel_wonder = true;
  if (has_wonder == 0) {
    has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO);
    if (has_wonder == 0) {
      has_intel_wonder = false;
    }
  }
  if ((((*(byte *)&civs[human_civ[0]].treaties[(int)parley->other_civ] & TREATY_EMBASSY) != 0)
      || (has_intel_wonder)) || (game.reveal_map != '\0')) {
    intel_report_show(human_civ[0],(iptr)parley->other_civ);
  }
  else {
    popup_game_with_sprite((const char *)&str_nointel_2,(iptr)&popup_info_icon_sprite,0);
  }
  return;
}

/* 0052dd73 auto */
void __cdecl parley_on_mode_chosen(i32 ctrl_id)

{
  iptr active_child;
  i32 value;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  switch(this2->mode) {
  case 3:
    value = ((RadioGroup *)this2->side_radios[1])->get_selected();
    this2->selected_action = value;
    if (this2->selected_action == 0) {
      this2->mode = 6;
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 1) {
      this2->mode = 0xc;
      if (this2->proposal_kind != 0) {
        this2->mode = 0xd;
      }
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 2) {
      if (this2->gift_kind == 0) {
        this2->mode = 7;
      }
      else if ((int)this2->gift_kind == 1) {
        this2->mode = 8;
      }
      else if ((int)this2->gift_kind == 2) {
        this2->mode = 9;
      }
      else if ((int)this2->gift_kind == 3) {
        this2->mode = 10;
      }
      else {
        this2->mode = 0xb;
      }
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 3) {
      this2->mode = 5;
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 4) {
      this2->mode = 0xf;
      ((Button *)this2->never_mind_button)->enable();
    }
    this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    this2->add_side_dialog((i32)this2->left_kind,0);
    this2->add_side_dialog((i32)this2->right_kind,1);
    break;
  case 5:
    value = ((RadioGroup *)this2->side_radios[0])->get_selected();
    this2->exchange_kind[0] = value;
    value = ((RadioGroup *)this2->side_radios[1])->get_selected();
    this2->exchange_kind[1] = value;
    this2->mode = 0xe;
    this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    this2->add_side_dialog((i32)this2->left_kind,0);
    this2->add_side_dialog((i32)this2->right_kind,1);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    if (this2->received_offer != 0) {
      operator_delete((void *)this2->received_offer);
      ((u8 *)this2 + 0x21c)[0x18] = '\0';
      ((u8 *)this2 + 0x21c)[0x19] = '\0';
      ((u8 *)this2 + 0x21c)[0x1a] = '\0';
      ((u8 *)this2 + 0x21c)[0x1b] = '\0';
    }
    value = (i32)(this2->send_offer(this2->mode));
    this2->received_offer = (ParleyOfferMsg *)value;
    if ((this2->mode == 6) && (this2->treaty_kind == 3)) {
      if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_PEACE) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_CEASEFIRE) != 0) {
            diplo_clear_treaty_flags(human_civ[0],this2->other_civ,2);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],this2->other_civ,4);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],this2->other_civ,8);
      }
      this2->add_side_dialog((i32)this2->left_kind,0);
      this2->add_side_dialog((i32)this2->right_kind,1);
    }
    else if (this2->received_offer != 0) {
      this2->mode = 1;
      this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
      this2->left_kind = (i32)this2->shown_left_kind;
      this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
      this2->right_kind = (i32)this2->shown_right_kind;
      this2->add_side_dialog((i32)this2->left_kind,0);
      this2->add_side_dialog((i32)this2->right_kind,1);
      ((Button *)this2->never_mind_button)->disable();
    }
  }
  return;
}

/* 0052e326 auto */
void __cdecl parley_on_never_mind(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  switch(this2->mode) {
  case 3:
    break;
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xf:
    this2->mode = 3;
    ((Button *)this2->never_mind_button)->disable();
    this2->shown_left_kind = parley_panes.mode3_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_panes.mode3_right_kind;
    this2->right_kind = this2->shown_right_kind;
    this2->add_side_dialog(this2->left_kind,0);
    this2->add_side_dialog(this2->right_kind,1);
    break;
  case 0xe:
    this2->mode = 5;
    this2->shown_left_kind = parley_panes.mode5_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_panes.mode5_right_kind;
    this2->right_kind = this2->shown_right_kind;
    this2->add_side_dialog(this2->left_kind,0);
    this2->add_side_dialog(this2->right_kind,1);
  }
  return;
}

/* 0052e4c9 auto */
void __cdecl parley_on_accept_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)parley + 0x48))->activate();
  net_send_message(NM_PARLEY_ACCEPT_OFFER,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  dlg_style.pending_idle_callback = turn_popup_timeout_idle;
  popup_game_with_sprite((const char *)&str_parleyaccept2,(iptr)&popup_info_icon_sprite,0);
  parley->mode = 3;
  parley->set_mode_sides(1);
  return;
}

/* 0052e57c auto */
void __cdecl parley_on_no_thanks(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)this2 + 0x48))->activate();
  net_send_message(NM_PARLEY_NO_THANKS,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  this2->mode = 2;
  this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
  this2->left_kind = this2->shown_left_kind;
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->left_kind,0);
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052e685 auto */
void __cdecl parley_on_counter_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  ((Wnd *)((u8 *)parley + 0x48))->activate();
  net_send_message(NM_PARLEY_COUNTER_OFFER,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  parley->mode = 3;
  parley->set_mode_sides(1);
  return;
}

/* 0052e71a auto */
void __cdecl chat_on_mute_checkbox(int ctrl_id)

{
  i32 checked;
  int chat_wnd2;
  
  chat_wnd2 = wnd_get_active_child();
  if (chat_wnd2 == 0) {
    chat_wnd2 = 0;
  }
  else {
    chat_wnd2 -= 0x48;
  }
  ((Wnd *)(chat_wnd2 + 0x48))->activate();
  if ((0x428 < ctrl_id) && (ctrl_id < 0x430)) {
    checked = ((Checkbox *)chat_wnd_block.mute_checkboxes[(ctrl_id - 0x428)])->is_checked();
    chat_wnd_block.muted_civs[(ctrl_id - 0x428)] = checked;
  }
  return;
}

/* 0052e7b7 auto */
void __cdecl chat_on_send_checkbox(int ctrl_id)

{
  i32 checked;
  int slot;
  int chat_wnd2;
  
  chat_wnd2 = wnd_get_active_child();
  if (chat_wnd2 == 0) {
    chat_wnd2 = 0;
  }
  else {
    chat_wnd2 -= 0x48;
  }
  ((Wnd *)(chat_wnd2 + 0x48))->activate();
  switch(ctrl_id) {
  case 0x415:
    chat_wnd_block.remain_anonymous = ((Checkbox *)chat_wnd_block.anonymous_checkbox)->is_checked();
    break;
  case 0x416:
    chat_wnd_block.broadcast_all = ((Checkbox *)chat_wnd_block.broadcast_checkbox)->is_checked();
    if (chat_wnd_block.broadcast_all != 0) {
      for (slot = 1; slot < 8; slot++) {
        if (chat_wnd_block.send_checkboxes[slot] != 0) {
          ((Checkbox *)chat_wnd_block.send_checkboxes[slot])->set_checked(1);
          ((Control *)chat_wnd_block.send_checkboxes[slot])->show_and_redraw();
          chat_wnd_block.send_to_civs[slot] = 1;
        }
      }
    }
    break;
  case 0x417:
  case 0x418:
  case 0x419:
  case 0x41a:
  case 0x41b:
  case 0x41c:
  case 0x41d:
    ctrl_id -= 0x416;
    checked = ((Checkbox *)chat_wnd_block.send_checkboxes[ctrl_id])->is_checked();
    chat_wnd_block.send_to_civs[ctrl_id] = checked;
    if ((chat_wnd_block.send_to_civs[ctrl_id] == 0) && (chat_wnd_block.broadcast_all != 0)) {
      ((Checkbox *)chat_wnd_block.broadcast_checkbox)->set_checked(0);
      ((Control *)chat_wnd_block.broadcast_checkbox)->show_and_redraw();
      chat_wnd_block.broadcast_all = 0;
    }
    break;
  case 0x430:
    chat_wnd_block.mute_anonymous = ((Checkbox *)chat_wnd_block.mute_anonymous_checkbox)->is_checked();
  }
  return;
}

/* 0052e971 auto */
void ParleyWindow::update_buttons()

{
  if ((mode_button != 0) && (mode2_button != 0)) {
    if (mode != 2) {
      ((Control *)conceal_button)->hide();
      ((Button *)conceal_button)->disable();
      ((Control *)reveal_button)->hide();
      ((Button *)reveal_button)->disable();
    }
    switch(mode) {
    case 1:
    case 2:
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->disable();
      if (mode == 2) {
        if ((local_units_revealed == '\0') && (local_cities_revealed == '\0')) {
          ((Control *)conceal_button)->hide();
          ((Button *)conceal_button)->disable();
          ((Control *)reveal_button)->show_and_redraw();
          ((Button *)reveal_button)->enable();
        }
        else {
          ((Control *)conceal_button)->show_and_redraw();
          ((Button *)conceal_button)->enable();
          ((Control *)reveal_button)->hide();
          ((Button *)reveal_button)->disable();
        }
      }
      break;
    case 3:
    case 5:
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->enable();
      break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xf:
      ((Control *)mode2_button)->show_and_redraw();
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      if (side_selected[1] == 0) {
        ((Button *)mode2_button)->disable();
      }
      else {
        ((Button *)mode2_button)->enable();
      }
      break;
    case 0xe:
      ((Control *)mode2_button)->show_and_redraw();
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      if ((side_selected[0] == 0) || (side_selected[1] == 0)) {
        ((Button *)mode2_button)->disable();
      }
      else {
        ((Button *)mode2_button)->enable();
      }
    }
  }
  return;
}

/* 0052ec47 auto */
iptr ParleyWindow::find_unit_by_id(int unit_id)

{
  short x2;
  short y2;
  bool near_land;
  iptr x;
  int y;
  int in_bounds;
  iptr unit;
  iptr dir;
  
  unit = (iptr)game.num_units;
  do {
    unit--;
    if (unit < 0) {
      return 0xffffffff;
    }
  } while ((units[unit].id == 0) || (units[unit].id != unit_id));
  x2 = units[unit].x;
  y2 = units[unit].y;
  near_land = false;
  for (dir = 0; dir < 8; dir++) {
    x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
    in_bounds = map_in_bounds(x,y);
    if ((in_bounds != 0) && (y = tile_is_ocean(x,y), y == 0)) {
      near_land = true;
      break;
    }
  }
  if ((!near_land) && (y = unit_stack_sum(unit,2), y != 1)) {
    unit = -2;
  }
  return unit;
}

/* 0052ed95 auto */
iptr ParleyWindow::find_city_by_id(int city_id)

{
  iptr city;
  
  city = (iptr)game.num_cities;
  do {
    city--;
    if (city < 0) {
      return 0xffffffff;
    }
  } while ((cities[city].id == 0) || (cities[city].id != city_id));
  return city;
}

/* 0055ae80 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_stop(int notify)

{
  if (net.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  if (mp_timer.handle != 0) {
    game_timer_destroy(mp_timer.handle);
    mp_timer.handle = 0;
    status_draw_timer_bar(100,0xffffffff);
    if (((notify != 0) && (2 < game.multiplayer)) && (player_turn_active != '\0')) {
      net_send_message(NM_STOP_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
    }
  }
  return;
}

/* 0055af2e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_start(int notify)

{
  RECT caption_rect;
  
  if (net.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
  setup_picwnd_block.g_timer_blink[0] = 0;
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  caption_rect.left = map_views_block.main_view_frame_left;
  caption_rect.top = map_views_block.main_view_frame_top;
  caption_rect.right = map_views_block.main_view_frame_right;
  caption_rect.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&caption_rect);
  if (mp_timer.handle != 0) {
    game_timer_destroy(mp_timer.handle);
  }
  mp_timer.handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
  mp_timer.last_tick = timer_ticks();
  if ((notify != 0) && (2 < game.multiplayer)) {
    net_send_message(NM_START_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
  }
  return;
}

/* 0055b046 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_resume(int notify)

{
  city_window_close();
  intel_report_end_modal();
  throneroom_close();
  pedia_end_modal();
  if ((mp_timer.seconds_left != 0) && (map_views_block.main_view_timer_shown[0] != 0)) {
    if (mp_timer.handle != 0) {
      game_timer_destroy(mp_timer.handle);
    }
    mp_timer.handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
    mp_timer.last_tick = timer_ticks();
    if ((notify != 0) && (2 < game.multiplayer)) {
      net_send_message(NM_RESTART_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
    }
  }
  return;
}

/* 0055b0fb: written by hand in src/game/timer_display.cpp */

/* 0055b2c6 auto */
void __cdecl mp_cmd_change_timer(void)

{
  int cancelled;
  RECT caption_rect;
  
  mp_turn_timer_stop(1);
  GetAsyncKeyState(0x1b);
  cancelled = setup_choose_game_timer(1);
  if (cancelled == 0) {
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0) {
      map_views_block.main_view_timer_shown[0] = 0;
      ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
      caption_rect.left = map_views_block.main_view_frame_left;
      caption_rect.top = map_views_block.main_view_frame_top;
      caption_rect.right = map_views_block.main_view_frame_right;
      caption_rect.bottom = map_views_block.main_view_client_top;
      ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&caption_rect);
    }
    else {
      map_views_block.main_view_timer_shown[0] = 1;
      mp_turn_timer_start(1);
    }
    if (2 < game.multiplayer) {
      net_send_message(NM_TIMER_CHANGE,0xff,civ_saved_cursor_x_block.mp_turn_time_limit_ms,0,0,0,0,0,0,0);
    }
  }
  else {
    mp_turn_timer_resume(1);
  }
  if (autoplay.active != 0) {
    player_turn_active = 0;
  }
  return;
}

/* 0055b3c8 auto */
void __cdecl mp_mouse_watchdog_stop(void)

{
  if (mp_timer.mouse_watchdog != 0) {
    game_timer_destroy(mp_timer.mouse_watchdog);
    mp_timer.mouse_watchdog = 0;
  }
  return;
}

/* 0055b3fd auto */
void __cdecl mp_mouse_watchdog_start(void)

{
  if (2 < game.multiplayer) {
    if (mp_timer.mouse_watchdog != 0) {
      game_timer_destroy(mp_timer.mouse_watchdog);
    }
    mp_timer.mouse_watchdog = game_timer_create(mp_mouse_watchdog_tick,500,1);
  }
  return;
}

/* 0055b451 auto */
i32 __cdecl mp_window_drag_mouse_hook(void)

{
  if (mp_drag.active != '\0') {
    mp_mouse_watchdog_tick(0,0);
  }
  return 0;
}

/* 0055b47e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_window_drag_begin(void)

{
  dbg_log((const char *)&str_enter_window_drag);
  mp_drag.active = 1;
  mp_drag.start_ticks = timer_ticks();
  mp_drag.flag = 0;
  mp_drag.wnd = wnd_get_active_child();
  if (net.host_flag != '\0') {
    net_send_message(NM_DRAG_HOLD_ON,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  setup_picwnd_block.mp_window_drag_hook = SetWindowsHookExA(7,(HOOKPROC)((void *)mp_window_drag_mouse_hook),err_report_ctx_block.app_hinstance,0);
  return;
}

/* 0055b515 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_window_drag_end(void)

{
  dbg_log((const char *)&str_exit_window_drag);
  mp_drag.active = 0;
  mp_drag.start_ticks = 0;
  mp_drag.flag = 0;
  mp_drag.wnd = 0;
  UnhookWindowsHookEx(setup_picwnd_block.mp_window_drag_hook);
  if (net.host_flag != '\0') {
    net_send_message(NM_DRAG_HOLD_OFF,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}

/* 0055b59e auto */
void __cdecl mp_release_mouse_capture(void)

{
  NativeWindow *wnd;
  UINT Msg;
  WPARAM wParam;
  LPARAM lParam;
  
  if (mp_drag.active != '\0') {
    if ((*(WndBase * *)&mp_drag.wnd) != (WndBase *)0x0) {
      lParam = 0;
      wParam = 0;
      Msg = 0x202;
      wnd = (*(WndBase * *)&mp_drag.wnd)->get_window();
      PostMessageA(wnd->hwnd,Msg,wParam,lParam);
      (*(WndBase * *)&mp_drag.wnd)->release_capture();
    }
    mp_drag.active = '\0';
  }
  return;
}

/* 0055b5fa auto */
void __cdecl mp_mouse_watchdog_tick(i32 timer_id,i32 timer_arg)

{
  int now_ticks;
  
  if ((((2 < game.multiplayer) && (net.net_player_count[0] != 1)) && (mp_drag.active != '\0')) &&
     ((now_ticks = timer_ticks(), mp_drag.start_ticks + 0x26c < now_ticks && (net.player_has_quit == '\0')))) {
    mp_release_mouse_capture();
  }
  mp_mouse_watchdog_start();
  return;
}

/* 0056e270 auto */
LobbyList *LobbyList::construct()

{
  u8 *slot_ptr;
  int i;
  
  next_id = 1;
  entries = 0;
  for (i = 0; i < 7; i++) {
    slot_ptr = (u8 *)&drop_counts[i];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  return this;
}

/* 0056e2c9 auto */
void LobbyList::destruct()

{
  net.free_game_list();
  return;
}

/* 0056e2e9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 LobbyList::host_wait_players(i32 map_arg,int launch_mode)

{
  uint *flags_ptr;
  Dialog *this_00;
  int result;
  int entry;
  int waiting_entry;
  i32 ret;
  uptr new_dialog;
  i32 seh_frame;
  
  if ((launch_mode == 2) || (launch_mode == 3)) {
    net_send_message(NM_SERVER_INFO,0,0,0,0,0,0,0,0,0);
    dialog = 0;
    net.net_server_address[0] = 0;
    crt_strcpy(((i8 *)&net.net_game_name),((i8 *)&menu_prefs.net_game_name));
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),((i8 *)&menu_prefs.net_player_name));
  }
  else {
    this_00 = (Dialog *)(operator_new(0x2f4));
    if (this_00 == (Dialog *)0x0) {
      new_dialog = 0;
    }
    else {
      new_dialog = (uptr)(this_00->Dialog::construct(0x4000));
    }
    dialog = (Dialog *)new_dialog;
    if (dialog == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pplayerpopup_null,(iptr)&str_d_ss_franklinton_startup_playerbox_cpp,0x34);
    }
    saved_button_label = dlg_style.button_labels[0];
    str_clear(title);
    str_append_label((iptr)title,0x28b);
    dlg_style.button_labels[0] = (i8 *)title;
    dialog->Dialog::reset(0x81801);
    text_clear();
    text_append_label(0x288);
    if (launch_mode == 0) {
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&lobby_ip_sep_str));
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&menu_prefs.net_ip_address));
    }
    dialog->set_title((const char *)&textbuf.buf);
    text_clear();
    text_append_label(0x120);
    dialog->add_button((const char *)&textbuf.buf);
    add_waiting_entry();
    dialog->set_listbox(7,0x1b7,0);
    dialog->set_default_choice(next_id - 1);
    dialog->render();
    ((ModalMenuWindow *)((int)dialog->surf_wnd + 0x48))->set_modal_idle_callback(lobby_dialog_idle);
    if (net.lobby_launch_mode == 2) {
      dbg_log((const char *)&str_igz_sending_dplsys_connectionsettingsrea);
      result = XD_LobbySendMessage(1);
      if (result != 0) {
        dbg_logf((const char *)&str_igz_xd_lobbysendmessage_returned_error_d_2,result);
      }
      dbg_log((const char *)&str_igz_sending_dplsys_dplayconnectsucceeded);
      result = XD_LobbySendMessage(3);
      if (result != 0) {
        dbg_logf((const char *)&str_igz_xd_lobbysendmessage_returned_error_d_3,result);
      }
    }
    while( true ) {
      net_send_message(NM_SERVER_INFO,0,0,0,0,0,0,0,0,0);
      flags_ptr = (uint *)((iptr)dialog + 0x3c);
      *flags_ptr = *flags_ptr & 0xfffffbff;
      dialog->result_id = 0;
      dialog->result_extra = (i32)dialog->result_id;
      result = dialog->show_modal(0);
      if ((int)dialog->result_extra != 1) break;
      entry = find_entry_by_id(result);
      result = slot_for_entry_id(result);
      if ((entry != 0) && (waiting_entry = find_waiting_entry(), waiting_entry == 0)) {
        net_send_message(NM_SERVER_REJECT,net.net_slots[result].id,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(180000);
        XD_ServerCloseConnection((ushort)net.net_slots[result].id);
        ret = entry_neighbor(entry);
        dialog->selected_option = (DialogOption *)ret;
        remove_entry((void *)entry);
        net.remove_player(net.net_slots[result].id);
        add_waiting_entry();
      }
    }
    if (result < 0) {
      net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
      dlg_style.button_labels[0] = saved_button_label;
      clear_player_list();
      if (dialog != (Dialog *)0x0) {
        dialog->scalar_destruct(1);
      }
      dialog = 0;
      XD_StopConnections();
      ret = 0;
      return ret;
    }
    crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&menu_prefs.net_ip_address));
    crt_strcpy(((i8 *)&net.net_game_name),((i8 *)&menu_prefs.net_game_name));
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),((i8 *)&menu_prefs.net_player_name));
    dialog->close();
    dlg_style.button_labels[0] = saved_button_label;
    clear_player_list();
    if (dialog != (Dialog *)0x0) {
      dialog->scalar_destruct(1);
    }
    dialog = 0;
    wnd_flush_paint();
  }
  if (1 < net.net_player_count[0]) {
    net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
  }
  net.session_phase = 1;
  net_send_message(NM_GAME_GOING,0xff,1,0,0,0,0,0,0,0);
  crt_strcpy(((i8 *)&net.slot_player_names),((i8 *)&menu_prefs.net_player_name));
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(NM_SERVER_PATH,0xff,0,0,0,0,0,0,0,0);
  net_send_message(NM_JOIN_GAMESTATE,0xff,0,0,0,0,0,0,0,0);
  net.local_slot = 0;
  result = game_profile_popup(1);
  if (result == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    ret = 0;
  }
  else {
    if ((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) {
      leaders_reset_custom_names();
      game.civs_alive = '\0';
      result = setup_choose_tribe(0,3,0);
      if (result != 0) {
        if (1 < net.net_player_count[0]) {
          net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    else {
      result = net_choose_civ_dialog(0);
      if (result != 0) {
        if (1 < net.net_player_count[0]) {
          net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    net.net_civ_slots[game.player_civ] = 0;
    net.slot_civs[0] = (int)game.player_civ;
    net.slot_active_flags[0] = 1;
    net.slot_host_flags[0] = 1;
    if (1 < net.net_player_count[0]) {
      crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
      dlg_style.pending_idle_callback = lobby_wait_civs_chosen_idle;
      net.wait_start_ticks = timer_ticks();
      result = popup_game_sprite_flags((const char *)&str_waitingonjoin,0x2000001,(iptr)&popup_info_icon_sprite,0);
      if (result == -1) {
        net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    if (1 < net.net_player_count[0]) {
      net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
    }
    if ((turn_loop.resume_civ == '\0') && (turn_resume_new_game == 0)) {
      ret = setup_choose_hotseat_players();
      setup_choose_opponents(ret);
      prefs_save_civ2dat();
      rules_load_advances(1);
      rules_load_units(1);
      rules_load_improvements(1);
      snd_menu_end();
      map_generate(map_arg);
      diffeng_register_blocks();
      diffeng_alloc_mirror();
      game_init_new();
      if (setup_dlg.accelerated_start != 0) {
        game_accelerated_start(setup_dlg.accelerated_start - 1);
      }
      if (turn_loop.resume_civ != '\0') {
        game.player_civ = turn_loop.resume_civ;
        human_civ[0] = (iptr)turn_loop.resume_civ;
      }
    }
    else {
      diffeng_register_blocks();
      diffeng_alloc_mirror();
    }
    text_close_file();
    if (1 < net.net_player_count[0]) {
      net_send_message(NM_MAP_STRUCT,0xff,0,0,0,0,0,0,0,0);
      net_send_state_diff(0xff,1,0,0,0);
      XD_FlushSendBuffer(5000);
      net_send_message(NM_EVENTSLOADED,0xff,scenario_events_loaded,unit_types_block.scenario_event_count,0,0,0,0,0,0);
      net_send_message(NM_START_CIVILIZATION,0xff,0,0,0,0,0,0,0,0);
      XD_FlushSendBuffer(180000);
    }
    net_reset_msg_counts();
    net.session_phase = 3;
    net_send_message(NM_GAME_GOING,0xff,3,0,0,0,0,0,0,0);
    ret = 1;
  }
  return ret;
}

/* 0056ec92 auto */
void __cdecl lobby_wait_civs_chosen_idle(void)

{
  int slot;
  int pending_count;
  
  net.process_messages(1,0);
  pending_count = 0;
  for (slot = 1; slot < 7; slot++) {
    if (((((i8 *)&net.slot_active_flags))[slot * 0x54] == 1) && ((((i32 *)&net.slot_civs))[slot * 0x15] == -1)) {
      pending_count++;
    }
  }
  if (pending_count == 0) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    net.session_phase = 2;
    net_send_message(NM_GAME_GOING,0xff,2,0,0,0,0,0,0,0);
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0056ed62 auto */
void __cdecl lobby_dialog_idle(void)

{
  net.process_messages(1,0);
  return;
}

/* 0056edd3 auto */
iptr LobbyList::entry_neighbor(iptr entry)

{
  iptr neighbor;
  
  neighbor = entry;
  if (*(int *)(entry + 0x10) != 0) {
    neighbor = *(iptr *)(entry + 0x10);
  }
  if ((neighbor == entry) && (*(int *)(entry + 0x14) != 0)) {
    neighbor = *(iptr *)(entry + 0x14);
  }
  return neighbor;
}

/* 0056ee2c auto */
void LobbyList::clear_player_list()

{
  void *next;
  void *entry;
  
  entry = (void *)entries;
  while (entry != (void *)0x0) {
    next = *(void **)((int)entry + 0x10);
    operator_delete(*(void **)((int)entry + 8));
    operator_delete(entry);
    entry = next;
  }
  entries = 0;
  dialog->option_count = 0;
  dialog->option_list = 0;
  dialog->selected_option = 0;
  return;
}

/* 0056eed7 auto */
void LobbyList::remove_entry(void *entry)

{
  int *count_ptr;
  
  if (entry != (void *)0x0) {
    if (*(int *)((int)entry + 0x14) == 0) {
      dialog->option_list = (DialogOption *)*(i32 *)((int)entry + 0x10);
      entries = (uptr)dialog->option_list;
    }
    else {
      *(i32 *)(*(int *)((int)entry + 0x14) + 0x10) = *(i32 *)((int)entry + 0x10);
    }
    if (*(int *)((int)entry + 0x10) != 0) {
      *(i32 *)(*(int *)((int)entry + 0x10) + 0x14) = *(i32 *)((int)entry + 0x14);
    }
    operator_delete(*(void **)((int)entry + 8));
    operator_delete(entry);
    count_ptr = (int *)((iptr)dialog + 0x28);
    *count_ptr = *count_ptr - 1;
  }
  return;
}

/* 0056ef93 auto */
uint *LobbyList::add_player_entry(const char *name2,uint entry_id)

{
  int *count_ptr;
  uint *entry;
  size_t name_len;
  void *name_copy;
  int width;
  int max_width;
  uptr tail2;
  uptr node2;
  
  tail2 = 0;
  for (node2 = entries; node2 != 0; node2 = *(uptr *)(node2 + 0x10)) {
    tail2 = node2;
  }
  entry = (uint *)(operator_new(0x18));
  entry[4] = 0;
  entry[5] = tail2;
  if (tail2 == 0) {
    entries = (uptr)entry;
    *(uint **)((iptr)dialog + 0x228) = entry;
  }
  else {
    *(uint **)(tail2 + 0x10) = entry;
  }
  *entry = 0;
  name_len = _strlen((char *)name2);
  name_copy = operator_new(name_len + 1);
  entry[2] = (uint)name_copy;
  crt_strcpy((char *)((void *)entry[2]),(const char *)((const void *)name2));
  if (*(char *)name2 == '\0') {
    *entry = *entry | 1;
  }
  entry[1] = entry_id;
  entry[3] = 0;
  width = ((GameFont *)dialog->body_font)->text_width((const char *)entry[2]);
  max_width = dialog->option_icon_width();
  width = width + max_width + (int)dialog->option_extra_width + 5;
  max_width = (int)dialog->option_width;
  if (max_width <= width) {
    max_width = width;
  }
  dialog->option_width = max_width;
  count_ptr = (int *)((iptr)dialog + 0x28);
  *count_ptr = *count_ptr + 1;
  dialog->option_list = (DialogOption *)entries;
  dialog->list_top_option[0] = dialog->option_list;
  return entry;
}

/* 0056f113 auto */
void __cdecl lobby_refresh_player_list(void)

{
  int selected_id;
  int slot;
  uptr entry;
  
  if (net.net_lobby_list != (LobbyList *)0x0) {
    selected_id = *(int *)((int)net.net_lobby_list->dialog->selected_option + 4);
    net.net_lobby_list->clear_player_list();
    if (net.net_player_count[0] == 0) {
      net.net_lobby_list->add_waiting_entry();
    }
    else {
      for (slot = 1; slot < 7; slot++) {
        if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
          if (net.net_slots[slot].lobby_id == 0) {
            net.net_slots[slot].lobby_id = net.net_lobby_list->next_id;
            net.net_lobby_list->next_id++;
          }
          net.net_lobby_list->add_player_entry((const char *)(((i8 *)&net.slot_player_names) + slot * 0x54),
                     net.net_slots[slot].lobby_id);
        }
      }
      net.net_lobby_list->add_waiting_entry();
    }
    for (entry = net.net_lobby_list->entries; (entry != 0 && (*(int *)(entry + 4) != selected_id));
        entry = *(uptr *)(entry + 0x10)) {
    }
    if (entry == 0) {
      for (entry = net.net_lobby_list->entries;
          (*(int *)(entry + 0x10) != 0 && (*(int *)(entry + 4) < selected_id));
          entry = *(uptr *)(entry + 0x10)) {
      }
      if (entry == 0) {
        entry = net.net_lobby_list->entries;
      }
    }
    net.net_lobby_list->dialog->option_list = (DialogOption *)net.net_lobby_list->entries;
    net.net_lobby_list->dialog->list_top_option[0] = net.net_lobby_list->dialog->option_list;
    net.net_lobby_list->dialog->set_default_choice(*(iptr *)(entry + 4));
    net.net_lobby_list->dialog->render();
  }
  return;
}

/* 0056f301 auto */
void LobbyList::add_waiting_entry()

{
  i32 entry_id;
  
  if (entries == 0) {
    text_clear();
    text_append_label(0x28c);
    entry_id = (*(i32 * *)&net.net_lobby_list)[1];
    (*(i32 * *)&net.net_lobby_list)[1]++;
    add_player_entry((const char *)&textbuf.buf,entry_id);
    ((Dialog *)*(*(i32 * *)&net.net_lobby_list))->set_default_choice(*(iptr *)((*(i32 * *)&net.net_lobby_list)[10] + 4));
  }
  return;
}

/* 0056f372 auto */
i32 LobbyList::find_waiting_entry()

{
  int cmp;
  uptr entry;
  
  text_clear();
  text_append_label(0x28c);
  entry = entries;
  if (((entry != 0) && (*(int *)(entry + 0x10) == 0)) &&
     (cmp = _strcmp(((i8 *)&textbuf.buf),*(char **)(entry + 8)), cmp == 0)) {
    return entry;
  }
  return 0;
}

/* 0056f3e0 auto */
uptr LobbyList::find_entry_by_id(int entry_id)

{
  uptr entry;
  
  for (entry = entries; (entry != 0 && (*(int *)(entry + 4) != entry_id));
      entry = *(uptr *)(entry + 0x10)) {
  }
  return entry;
}

/* 0056f42b auto */
int LobbyList::slot_for_entry_id(int entry_id)

{
  int slot;
  
  for (slot = 0; (slot < 7 && (net.net_slots[slot].lobby_id != entry_id));
      slot = slot + 1) {
  }
  return slot;
}

/* 00587a90 auto */
void ParleyWindow::create_city_list_pane(RECT *rect,int pane,i32 own_cities)

{
  u8 *field_ptr;
  int right;
  int scroll_width;
  Button *new_ctrl;
  u8 *hotspot_parent;
  u8 *scroll_parent;
  i32 hotspot;
  i32 scrollbar;
  RECT rect2;
  i32 seh_frame;
  
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  sprite_picker_sprite_block.parley_pane_own_cities[pane] = own_cities;
  build_city_list(pane,0);
  if (list_counts[pane] < 1) {
    field_ptr = (u8 *)&list_first_row[pane];
    field_ptr[0] = 0xff;
    field_ptr[1] = 0xff;
    field_ptr[2] = 0xff;
    field_ptr[3] = 0xff;
  }
  else {
    field_ptr = (u8 *)&list_first_row[pane];
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  field_ptr = (u8 *)&list_columns[pane];
  field_ptr[0] = 4;
  field_ptr[1] = '\0';
  field_ptr[2] = '\0';
  field_ptr[3] = '\0';
  list_rects[pane * 4] = rect->left;
  list_rects[pane * 4 + 1] = rect->top;
  list_rects[pane * 4 + 2] = rect->right;
  list_rects[pane * 4 + 3] = rect->bottom;
  if (list_rows[pane] <
      list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    right = rect2.right;
    list_rects[pane * 4 + 2] =
         (int)list_rects[pane * 4 + 2] - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = right - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[pane])->create((iptr)scroll_parent,
               pane + 0x420,(iptr)&rect2,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               list_counts[pane] -
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size(
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[pane])->create((iptr)hotspot_parent,
             pane + 0x422,(iptr)(((u8 *)this + 0x39c) + pane * 0x10 + 0x24));
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_city_list_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_city_list_click);
  parley_draw_city_list(pane);
  return;
}

/* 00587e05 auto */
void __cdecl parley_city_list_scroll_local(i32 pos)

{
  parley_city_list_scroll(0,pos);
  return;
}

/* 00587e23 auto */
void __cdecl parley_city_list_scroll_remote(i32 pos)

{
  parley_city_list_scroll(1,pos);
  return;
}

/* 00587e41 auto */
void __cdecl parley_reveal_city_info(void)

{
  iptr child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)parley_wnd2 + 0x48))->activate();
  ((Control *)parley_wnd2->info_hide_buttons[0])->show_and_redraw();
  ((Control *)parley_wnd2->info_show_buttons[0])->hide();
  parley_wnd2->local_cities_revealed = 1;
  net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd2->other_civ]].id,
                     human_civ[0],(int)(char)parley_wnd2->local_cities_revealed,0,0,0,0,0,0);
  parley_draw_city_list(0);
  return;
}

/* 00587f00 auto */
void __cdecl parley_hide_city_info(void)

{
  iptr child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)parley_wnd2 + 0x48))->activate();
  ((Control *)parley_wnd2->info_show_buttons[0])->show_and_redraw();
  ((Control *)parley_wnd2->info_hide_buttons[0])->hide();
  parley_wnd2->local_cities_revealed = 0;
  net_send_message(NM_HIDE_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd2->other_civ]].id,
                     human_civ[0],(int)(char)parley_wnd2->local_cities_revealed,0,0,0,0,0,0);
  parley_draw_city_list(0);
  return;
}

/* 00587fbf auto */
void __cdecl parley_request_city_info(void)

{
  iptr child;
  ParleyWindow *window;
  const char *civ_plural;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((u8 *)window + 0x48))->activate();
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(0x65,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
                    human_civ[0],0);
  return;
}

/* 0058804f auto */
void __cdecl parley_city_list_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  ((Wnd *)((iptr)parley_wnd2 + 0x48))->activate();
  parley_wnd2->list_first_row[pane] = pos;
  parley_draw_city_list(pane);
  return;
}

/* 005880b0 auto */
void __cdecl parley_city_list_click(int control_id)

{
  uint *sel_flag;
  int hit_row;
  int key_down;
  int sel_end;
  int row;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *parley_wnd2;
  
  hit_row = wnd_get_active_child();
  if (hit_row == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(hit_row - 0x48);
  }
  ((Wnd *)((iptr)parley_wnd2 + 0x48))->activate();
  pane = control_id - 0x422;
  ((WndBase *)((iptr)parley_wnd2 + 0x48))->get_cursor_pos((iptr)&cursor_x,(iptr)&cursor_y);
  if (((parley_wnd2->list_counts[pane] != 0) &&
      (hit_row = parley_city_list_hit_test(cursor_x,cursor_y,pane), 0 < hit_row)) &&
     (hit_row < parley_wnd2->list_counts[pane])) {
    key_down = ((Wnd *)((iptr)parley_wnd2 + 0x48))->is_shift_down();
    if (key_down == 0) {
      key_down = ((Wnd *)((iptr)parley_wnd2 + 0x48))->is_ctrl_down();
      if (key_down == 0) {
        for (row = 0; row < parley_wnd2->list_counts[pane];
            row = row + 1) {
          if (row != hit_row) {
            parley_wnd2->list_selected[pane * 0x801 + row] = 0;
          }
        }
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = (uint *)(pane * 0x2004 + hit_row * 4 + 0x8400 + (iptr)parley_wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = (uint *)(pane * 0x2004 + hit_row * 4 + 0x8400 + (iptr)parley_wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (row = 0; row < parley_wnd2->list_counts[pane]; row++)
      {
        parley_wnd2->list_selected[pane * 0x801 + row] = 0;
      }
      if (parley_wnd2->list_anchor_row[pane] < hit_row) {
        row = parley_wnd2->list_anchor_row[pane];
        sel_end = hit_row;
      }
      else {
        sel_end = parley_wnd2->list_anchor_row[pane];
        row = hit_row;
      }
      for (; row <= sel_end; row++) {
        if (0 < row) {
          parley_wnd2->list_selected[pane * 0x801 + row] = 1;
        }
      }
    }
    parley_draw_city_list(pane);
  }
  return;
}

/* 0058832d auto */
int __cdecl parley_city_list_hit_test(int x,int y,int pane)

{
  iptr child;
  int result;
  int height;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (y < (int)parley_wnd2->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (y < (int)parley_wnd2->list_rects[pane * 4 + 3]) {
    if (x < (int)parley_wnd2->list_rects[pane * 4]) {
      result = -3;
    }
    else if (x < (int)parley_wnd2->list_rects[pane * 4 + 2]) {
      result = (int)parley_wnd2->list_rects[pane * 4 + 1];
      height = rect_height((RECT *)(pane * 0x10 + (iptr)parley_wnd2 + 0x3c0));
      result = (y - result) / (height / parley_wnd2->list_rows[pane]) +
              parley_wnd2->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 0058843f auto */
void ParleyWindow::sort_city_list(int first,int last,int pane)

{
  i32 tmp;
  iptr child;
  int cmp;
  int j;
  int i;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (1 < parley_wnd2->list_counts[pane]) {
    for (i = first; j = i, i < last; i++) {
      while (j = j + 1, j <= last) {
        cmp = city_has_improvement((iptr)parley_wnd2->list_items[j + pane * 0x801],IMP_PALACE);
        if ((cmp != 0) ||
           ((cmp = city_has_improvement((iptr)parley_wnd2->list_items[i + pane * 0x801]
                                        ,IMP_PALACE), cmp == 0 &&
            (cmp = _strcmp(cities[(int)parley_wnd2->list_items[i + pane * 0x801]].
                             name,cities[(int)parley_wnd2->list_items[j + pane * 0x801]
                                        ].name), 0 < cmp)))) {
          tmp = (i32)parley_wnd2->list_items[i + pane * 0x801];
          parley_wnd2->list_items[i + pane * 0x801] =
               (i32)parley_wnd2->list_items[j + pane * 0x801];
          parley_wnd2->list_items[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_selected[i + pane * 0x801];
          parley_wnd2->list_selected[i + pane * 0x801] =
               (i32)parley_wnd2->list_selected[j + pane * 0x801];
          parley_wnd2->list_selected[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_selected_b[i + pane * 0x801];
          parley_wnd2->list_selected_b[i + pane * 0x801] =
               (i32)parley_wnd2->list_selected_b[j + pane * 0x801];
          parley_wnd2->list_selected_b[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_items_b[i + pane * 0x801];
          parley_wnd2->list_items_b[i + pane * 0x801] =
               (i32)parley_wnd2->list_items_b[j + pane * 0x801];
          parley_wnd2->list_items_b[j + pane * 0x801] = tmp;
        }
      }
    }
  }
  return;
}

/* 0058878e auto */
void __cdecl parley_draw_city_list(int pane)

{
  int stale;
  u8 *field_ptr;
  int xLeft;
  iptr child;
  int width;
  int text_offset_y;
  int row_y;
  int text_offset_x;
  i32 font_height;
  int selected;
  const char *label;
  i32 bg_color2;
  int city;
  int row;
  uint shadow_color2;
  uint text_color2;
  ParleyWindow *this2;
  tagRECT row_rect;
  int row_height;
  int sprite_style;
  i8 saved_clip[16];
  GameFont *font2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(child - 0x48);
  }
  this2->get_clip((RECT *)saved_clip);
  this2->set_clip((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  for (;;) {
    this2->fill_rect((iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10),dlg_style.list_bg);
    if (this2->large_layout == 0) {
      font2 = (GameFont *)((i8 *)&font_size14);
    }
    else {
      font2 = (GameFont *)((i8 *)&font_size21);
    }
    xLeft = (int)this2->list_rects[pane * 4];
    width = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
    row_height = rect_height((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
    row_height = row_height / this2->list_rows[pane];
    text_offset_y = row_height / 2;
    row_y = font2->font_cached_height();
    text_offset_y -= row_y / 2;
    field_ptr = (u8 *)&this2->side_selected[pane];
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
    for (row = 0; row < this2->list_counts[pane];
        row = row + 1) {
      if (this2->list_selected[pane * 0x801 + row] != 0) {
        field_ptr = (u8 *)&this2->side_selected[pane];
        field_ptr[0] = 1;
        field_ptr[1] = '\0';
        field_ptr[2] = '\0';
        field_ptr[3] = '\0';
        break;
      }
    }
    stale = 0;
    for (row = 0; row < this2->list_rows[pane]; row++) {
      row_y = (int)this2->list_rects[pane * 4 + 1] + row * row_height;
      SetRect(&row_rect,xLeft,row_y,width + xLeft,row_y + row_height);
      if ((row != 0) && (this2->list_first_row[pane] == -1)) {
        break;
      }
      if (this2->list_first_row[pane] + row <
          this2->list_counts[pane]) {
        selected = (int)this2->list_selected[pane * 0x801 + (this2->list_first_row[pane] + row)];
        if (selected == 0) {
          if (this2->list_first_row[pane] + row == 0) {
            text_color2 = (uint)dlg_style.highlight_text_color;
            shadow_color2 = (*(uint *)&dlg_style.sel_text_shadow_color);
          }
          else {
            text_color2 = (*(uint *)&dlg_style.list_text);
            shadow_color2 = (*(uint *)&dlg_style.list_shadow);
          }
        }
        else {
          text_color2 = (*(uint *)&dlg_style.sel_text_color);
          shadow_color2 = (*(uint *)&dlg_style.sel_text_shadow_color);
        }
        city = 0;
        if ((0 < this2->list_counts[pane]) &&
           (city = this2->find_city_by_id(
                                  (i32)this2->list_items_b[pane * 0x801 + (this2->list_first_row[pane] + row)]), city == -1)) {
          stale = 1;  /* not found: the list is rebuilt and drawn again */
          break;
        }
        text_offset_x = (-(uint)(this2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_layout.margin_x;
        row_rect.left = row_rect.left + text_offset_x - 3;
        if (selected == 0) {
          bg_color2 = dlg_style.list_bg;
        }
        else {
          bg_color2 = dlg_style.list_sel_bg;
        }
        this2->fill_rect((iptr)&row_rect,bg_color2);
        if (pane == 0) {
          if (this2->local_cities_revealed == '\0') {
            sprite_style = 3;
          }
          else {
            sprite_style = 0;
          }
        }
        else if (this2->remote_cities_revealed == '\0') {
          sprite_style = 3;
        }
        else {
          sprite_style = 0;
        }
        if ((int)parley_wnd_block.mode == 10) {
          sprite_style = 0;
        }
        if (this2->list_first_row[pane] == -1) {
          text_clear();
          label = (label_string(label_table.string_ids[0x2de]));
          text_append(label);
          field_ptr = (u8 *)&this2->side_selected[pane];
          field_ptr[0] = '\0';
          field_ptr[1] = '\0';
          field_ptr[2] = '\0';
          field_ptr[3] = '\0';
        }
        else {
          selected = sprite_style;
          font_height = font2->font_cached_height();
          parley_draw_city_sprite(city,this2->list_first_row[pane] + row,
                             xLeft + 2,row_y,font_height,selected);
          text_clear();
          text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
          text_append_space();
          selected = city_has_improvement(city,IMP_PALACE);
          if ((selected == 0) && (sprite_style == 0)) {
            text_append_lparen();
            text_append_int(*(iptr *)(pane * 0x2004 +
                                         (this2->list_first_row[pane] +
                                         row) * 4 + 0xc408 + (int)this2));
            text_append_space();
            if ((int)this2->list_selected_b[pane * 0x801 + (this2->list_first_row[pane] + row)] < 2) {
              label = (label_string(label_table.string_ids[0x2dc]));
              text_append(label);
            }
            else {
              label = (label_string(label_table.string_ids[0x2db]));
              text_append(label);
            }
          }
          else {
            selected = city_has_improvement(city,IMP_PALACE);
            if (selected != 0) {
              label = (label_string(label_table.string_ids[0x2dd]));
              text_append(label);
            }
          }
        }
        if (shadow_color2 != text_color2) {
          gfx_set_text_color_index(shadow_color2);
          this2->draw_text((iptr)font2,(const char *)&textbuf.buf,xLeft + text_offset_x + 2,
                     row_y + text_offset_y + 1,5);
          gfx_set_text_color_index(text_color2);
          this2->draw_text((iptr)font2,(const char *)&textbuf.buf,xLeft + text_offset_x + 1,row_y + text_offset_y,
                     5);
        }
        gfx_set_text_color_index(text_color2);
        this2->draw_text((iptr)font2,(const char *)&textbuf.buf,xLeft + text_offset_x,row_y + text_offset_y,5);
      }
    }
    if (stale == 0) {
      this2->set_clip((RECT *)saved_clip);
      this2->update_buttons();
      ((Control *)this2->list_hotspots[pane])->show_and_redraw();
      return;
    }
    this2->build_city_list(pane,1);
    if (this2->list_counts[pane] == 0) {
      field_ptr = (u8 *)&this2->list_first_row[pane];
      field_ptr[0] = 0xff;
      field_ptr[1] = 0xff;
      field_ptr[2] = 0xff;
      field_ptr[3] = 0xff;
    }
    else if (this2->list_counts[pane] <
             this2->list_counts[pane]) {
      this2->list_first_row[pane] =
           this2->list_counts[pane] - 1;
    }
  }
}

/* 00588e47 auto */
void __cdecl
parley_draw_city_sprite(iptr city,uint row_index,int x,int y,int row_height,i32 style)

{
  iptr child;
  int sprite_height2;
  i32 zoom2;
  i32 draw_x;
  i32 sprite_width2;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (parley_wnd2->large_layout == 0) {
    zoom2 = 0xfffffffc;
  }
  else {
    zoom2 = 0xfffffffe;
  }
  draw_x = x;
  if (parley_wnd2->large_layout == 0) {
    sprite_width2 = 0x18;
  }
  else {
    sprite_width2 = 0x24;
  }
  if ((row_index & 1) != 0) {
    draw_x = x + sprite_width2 + 2;
  }
  sprite_height2 = gfx_zoom_scale(0x30,zoom2);
  gfx_draw_city((iptr)&parley_wnd_block,city,style,draw_x,y - (sprite_height2 - row_height) / 2,zoom2);
  return;
}

/* 00588f36 auto */
void ParleyWindow::build_city_list(int pane,int keep_selection)

{
  int *count_ptr;
  iptr child;
  iptr city;
  int old_count;
  iptr unit;
  void *old_selection;
  ParleyWindow *parley_wnd2;
  
  old_selection = (void *)0x0;
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (keep_selection != 0) {
    old_selection = operator_new(parley_wnd2->list_counts[pane] << 2);
    if (old_selection == (void *)0x0) {
      keep_selection = 0;
    }
    else {
      old_count = parley_wnd2->list_counts[pane];
      for (city = 0; city < old_count; city++) {
        if (parley_wnd2->list_selected[pane * 0x801 + city] == 0) {
          *(i32 *)((int)old_selection + city * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)old_selection + city * 4) =
               (i32)parley_wnd2->list_items_b[pane * 0x801 + city];
        }
      }
    }
  }
  parley_wnd2->list_counts[pane] = 0;
  city = 0;
  do {
    if (game.num_cities <= city) {
      if (keep_selection != 0) {
        operator_delete(old_selection);
      }
      sort_city_list(0,parley_wnd2->list_counts[pane] - 1,pane);
      return;
    }
    if ((cities[city].id != 0) &&
       (((sprite_picker_sprite_block.parley_pane_own_cities[pane] != 0 && (cities[city].owner == (*(i8 *)&human_civ)))
        || (((sprite_picker_sprite_block.parley_pane_own_cities[pane] == 0 &&
             ((int)cities[city].owner == other_civ)) &&
            (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) != 0 ||
             ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))))))))) {
      parley_wnd2->list_items[pane * 0x801 + parley_wnd2->list_counts[pane]] =
           city;
      parley_wnd2->list_items_b[pane * 0x801 + parley_wnd2->list_counts[pane]] =
           cities[city].id;
      if (keep_selection == 0) {
        parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
      }
      else {
        parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
        for (unit = 0; unit < old_count; unit++) {
          if (cities[city].id == *(int *)((int)old_selection + unit * 4)) {
            parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 1;
            break;
          }
        }
      }
      parley_wnd2->list_selected_b[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
      for (unit = 0; unit < game.num_units; unit++) {
        if (((units[unit].id != 0) && (cities[city].x == units[unit].x)) &&
           (cities[city].y == units[unit].y)) {
          count_ptr = (int *)(pane * 0x2004 + parley_wnd2->list_counts[pane] * 4 + 0xc408 +
                          (iptr)parley_wnd2);
          *count_ptr = *count_ptr + 1;
        }
      }
      count_ptr = (int *)((iptr)parley_wnd2 + 1000 + pane * 4);
      *count_ptr = *count_ptr + 1;
    }
    city++;
  } while( true );
}

/* 00594d42 auto */
i32 __cdecl
net_lock_map(iptr civ,iptr src_x,iptr src_y,iptr dst_x,int dst_y,int wait_reply)

{
  i32 result;
  int start_ticks;
  int now_ticks;
  int src_conflict;
  int dst_conflict;
  int slot;
  
  net.lock_map_busy = 1;
  net_map_lock_slots_block.net_map_locks[civ].replies = 0;
  net_map_lock_slots_block.net_map_locks[civ].state = net_map_lock_slots_block.net_map_locks[civ].replies;
  if (net.host_flag == '\0') {
    net.lock_map_busy = 0;
    net_send_message(NM_LOCK_MAP,0,civ,src_x,src_y,dst_x,dst_y,0,0,0);
    XD_FlushSendBuffer(60000);
    if (wait_reply == 0) {
      result = 1;
    }
    else {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].replies != 0)) break;
        net.process_messages(1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].replies == 0) {
        dbg_log((const char *)&str_lockmap_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_8,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
        result = 0;
      }
      else if (net_map_lock_slots_block.net_map_locks[civ].replies == 1) {
        result = 1;
      }
      else {
        result = 0;
      }
    }
  }
  else {
    dst_conflict = -1;
    src_conflict = -1;
    for (slot = 1; slot < 8; slot++) {
      if (((net_map_lock_slots_block.net_map_locks[slot].src_x == src_x) &&
          (net_map_lock_slots_block.net_map_locks[slot].src_y == src_y)) ||
         ((net_map_lock_slots_block.net_map_locks[slot].dst_x == src_x &&
          (net_map_lock_slots_block.net_map_locks[slot].dst_y == src_y)))) {
        src_conflict = 0;
        break;
      }
      if (((net_map_lock_slots_block.net_map_locks[slot].src_x == dst_x) &&
          (net_map_lock_slots_block.net_map_locks[slot].src_y == dst_y)) ||
         ((net_map_lock_slots_block.net_map_locks[slot].dst_x == dst_x &&
          (net_map_lock_slots_block.net_map_locks[slot].dst_y == dst_y)))) {
        dst_conflict = 0;
        break;
      }
    }
    if ((src_conflict == -1) && (dst_conflict == -1)) {
      net_map_lock_slots_block.net_map_locks[civ].src_x = src_x;
      net_map_lock_slots_block.net_map_locks[civ].src_y = src_y;
      net_map_lock_slots_block.net_map_locks[civ].dst_x = dst_x;
      net_map_lock_slots_block.net_map_locks[civ].dst_y = dst_y;
      if ((human_civ[0] != civ) &&
         (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 &&
          ((((i8 *)&net.slot_active_flags))[net.net_civ_slots[civ] * 0x54] != '\0')))) {
        net_send_message(NM_LOCK_MAP_ACK,net.net_slots[net.net_civ_slots[civ]].id,
                           civ,1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net.lock_map_busy = 0;
      result = 1;
    }
    else {
      if (((human_civ[0] != civ) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         ((((i8 *)&net.slot_active_flags))[net.net_civ_slots[civ] * 0x54] != '\0')) {
        net_send_message(NM_LOCK_MAP_ACK,net.net_slots[net.net_civ_slots[civ]].id,
                           civ,-1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net.lock_map_busy = 0;
      result = 0;
    }
  }
  return result;
}

/* 0059511c auto */
void __cdecl net_unlock_map(iptr civ,int wait_reply)

{
  int start_ticks;
  int now_ticks;
  
  net.unlock_map_busy = 1;
  if (net.host_flag == '\0') {
    net.unlock_map_busy = 0;
    net_map_lock_slots_block.net_map_locks[civ].state = 0;
    net_send_message(NM_UNLOCK_MAP,0,civ,0,0,0,0,0,0,0);
    if (wait_reply != 0) {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].state != 0)) break;
        net.process_messages(1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].state == 0) {
        dbg_log((const char *)&str_unlockmap_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_9,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
    }
  }
  else {
    _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots) + civ * 0x18,-1,0x18);
    net_send_message(NM_LOCK_MAP_SYNC,0xff,civ,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    net.unlock_map_busy = 0;
  }
  return;
}

/* 0059a850 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl net_session_global_init(void)

{
  net_session_static_init();
  net_session_reg_atexit();
  return;
}

/* 0059a86a auto */
void __cdecl net_session_static_init(void)

{
  net.construct();
  return;
}

/* 0059a884 auto */
void __cdecl net_session_reg_atexit(void)

{
  _atexit(net_session_static_dtor);
  return;
}

/* 0059a8a1 auto */
void __cdecl net_session_static_dtor(void)

{
  net.destruct();
  return;
}

/* 0059a8bb auto */
NetSession *NetSession::construct()

{
  i32 seh_frame;
  
  player_info_msg.init();
  msg_queue.construct();
  ((u8 *)&connection_title)[0] = '\0';
  ((u8 *)&connection_title)[1] = '\0';
  ((u8 *)&connection_title)[2] = '\0';
  ((u8 *)&connection_title)[3] = '\0';
  XD_SetBroadcastReceive(net_on_broadcast_receive);
  XD_SetSecureReceive(net_on_secure_receive);
  XD_SetOnClientConnectionToServer(net_on_connected_to_server);
  XD_SetNewClientConnection(net_on_new_client);
  XD_SetOnConnectionLost(net_on_connection_lost);
  XD_SetOversizedMessageCB(50000,net_on_oversized_message);
  return this;
}

/* 0059a998 auto */
void NetSession::clear()

{
  int i;
  
  connected = 0;
  connected_to_server = 0;
  popup_in_progress = 0;
  listing_games = 0;
  session_phase = 0;
  found_games = 0;
  gamelist_view = 0;
  net_lobby_list = 0;
  dialog_stack_top = 0;
  server_conn_id = 0xffffffff;
  (*(i32 *)&net.xd_adapter_index) = 0;
  adapter_index = 0;
  net.active_protocol = 0xffffffff;
  net_protocol = 0xffffffff;
  mp_turn_civ = 0;
  player_has_quit = 0;
  poll_depth = 0;
  poll_depth_peak = 0;
  msgq_normal_peak = 0;
  msgq_priority_peak = 0;
  host_handoff_stage = 0;
  mp_simultaneous_moves = 0;
  tile_sync_enabled = 0;
  tile_batch_active = 0;
  chat_missed_message_count = 8;
  chat_unread_count = 0;
  timeout_seconds = 0x3c;
  move_anim_sound = 0xffffffff;
  move_anim_sound_arg = 0xffffffff;
  unit_create_busy = 0;
  unit_delete_busy = 0;
  city_create_busy = 0;
  city_delete_busy = 0;
  lock_map_busy = 0;
  unlock_map_busy = 0;
  ui_input_busy = 0;
  unit_stack_unlink_busy = 0;
  unit_stack_link_busy = 0;
  unit_stack_bottom_busy = 0;
  unit_relocate_busy = 0;
  city_acquire_wonder_busy = 0;
  civ_eliminate_busy = 0;
  civ_create_busy = 0;
  civ_check_destroyed_busy = 0;
  map_redraw_busy = 0;
  stacked_draw_count = 0;
  stacked_draw_first = 0;
  stacked_draw_last = 0;
  unit_stack_check_enabled = 0;
  stacked_draw_peak = 0;
  for (i = 0; i < 8; i++) {
    casualty_counts[i] = 0;
    casualty_reported_counts[i] = 0;
    casualty_log_first[i] = 0;
    casualty_log_next[i] = 0;
  }
  hotseat_human_players = 0;
  unit_gather_cargo_busy = 0;
  unit_lift_off_map_busy = 0;
  unit_delete_cargo_busy = 0;
  unit_destroy_redraw_busy = 0;
  diplo_meeting.mp_pending = 0;
  pending_human_players = 0;
  for (i = 0; i < 8; i++) {
    civ_new_player_flags[i] = 0;
    civ_turn_ended[i] = 0;
    civ_drop_requested[i] = 0;
  }
  blocked_input_kind = 0;
  drag_hold = 0;
  shutdown(1);
  net_reset_error_counts();
  return;
}

/* 0059ad40 auto */
void NetSession::destruct()

{
  i32 seh_frame;
  i32 seh_state;
  
  seh_state = 0;
  if (connection_title != 0) {
    operator_delete((void *)connection_title);
    connection_title = 0;
  }
  shutdown(1);
  net_reset_error_counts();
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0059adc8 */ { msg_queue.clear(); }
  return;
}

/* 0059adef auto */
i8 NetSession::init_transport(i32 protocol,int is_server)

{
  i32 saved_protocol;
  i8 result;
  UINT timeout_secs;
  int activate_error;
  int init_error;
  
  if (connected != '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_bconnectedtonet,(iptr)&str_d_ss_franklinton_netmgr_cpp,0x92);
  }
  saved_protocol = net.net_protocol;
  msg_queue.reset();
  clear();
  _memset((void *)net_player_count,0,0x270);
  switch(protocol) {
  case 0:
    init_error = XD_InitializeSocketsTCP
                        (3,0x1381,0x1382,(int)(char)is_server,7,NULL
                        );
    game.multiplayer = 3;
    if (connection_type == 0) {
      timeout_secs = GetPrivateProfileIntA
                        (str_civilization_gold_17,str_internet_timeout,-1,
                         str_civ_ini_17);
      if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x3c)) {
        WritePrivateProfileStringA
                  (str_civilization_gold_18,str_internet_timeout_2,((i8 *)&net_internet_timeout_default),
                   str_civ_ini_18);
        timeout_seconds = 0x3c;
      }
      else {
        timeout_seconds = timeout_secs;
      }
    }
    else {
      timeout_secs = GetPrivateProfileIntA
                        (str_civilization_gold_19,str_tcpip_timeout,-1,str_civ_ini_19
                        );
      if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0xf)) {
        WritePrivateProfileStringA
                  (str_civilization_gold_20,str_tcpip_timeout_2,((i8 *)&net_tcpip_timeout_default),
                   str_civ_ini_20);
        timeout_seconds = 0xf;
      }
      else {
        timeout_seconds = timeout_secs;
      }
    }
    break;
  case 1:
    init_error = XD_InitializeSocketsIPXSPX
                        (3,0x1381,0x1382,(int)(char)is_server,7,NULL
                        );
    game.multiplayer = 3;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_21,str_ipxspx_timeout,-1,str_civ_ini_21)
    ;
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0xf)) {
      WritePrivateProfileStringA
                (str_civilization_gold_22,str_ipxspx_timeout_2,((i8 *)&net_ipxspx_timeout_default),
                 str_civ_ini_22);
      timeout_seconds = 0xf;
    }
    else {
      timeout_seconds = timeout_secs;
    }
    break;
  case 2:
    game.multiplayer = 5;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_23,str_modem_timeout,-1,str_civ_ini_23);
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x1e)) {
      WritePrivateProfileStringA
                (str_civilization_gold_24,str_modem_timeout_2,((i8 *)&net_modem_timeout_default),
                 str_civ_ini_24);
      timeout_seconds = 0x1e;
    }
    else {
      timeout_seconds = timeout_secs;
    }
    init_error = XD_InitializeModem((int)(char)is_server);
    break;
  case 3:
    game.multiplayer = 6;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_25,str_direct_timeout,-1,str_civ_ini_25)
    ;
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x1e)) {
      WritePrivateProfileStringA
                (str_civilization_gold_26,str_direct_timeout_2,((i8 *)&net_direct_timeout_default),
                 str_civ_ini_26);
      timeout_seconds = 0x1e;
    }
    else {
      timeout_seconds = timeout_secs;
    }
    init_error = XD_InitializeSerial((int)(char)is_server);
    break;
  default:
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_0_6,(iptr)&str_d_ss_franklinton_netmgr_cpp_2,0xf4);
  }
  if (init_error == 0) {
    if (((char)is_server == '\0') || (activate_error = XD_ActivateServer(), activate_error == 0)) {
      host_flag = (char)is_server;
      connected = 1;
      net.active_protocol = protocol;
      net_protocol = protocol;
      net_reset_error_counts();
      net_reset_msg_counts();
      _memset((void *)slot_addresses,0,0x20);
      _strncpy((char *)slot_addresses,((i8 *)&menu_prefs.net_ip_address),0x20);
      _memset((void *)slot_player_names,0,0x20);
      _strncpy((char *)slot_player_names,((i8 *)&menu_prefs.net_player_name),0x20);
      net_player_count[0] = 1;
      slot_active_flags[0] = 1;
      slot_host_flags[0] = host_flag;
      result = 1;
    }
    else {
      dbg_logf((const char *)&str_connecttonet_xd_activateserver_failed_re,activate_error);
      result = 0;
      net.net_protocol = saved_protocol;
    }
  }
  else {
    dbg_logf((const char *)&str_connecttonet_xd_initialize_failed_return,init_error);
    result = 0;
    net.net_protocol = saved_protocol;
  }
  return result;
}

/* 0059b293 auto */
/* WARNING: Removing unreachable block (ram,0x0059b3ca) */

void NetSession::shutdown(int delay_after)

{
  dbg_log((const char *)&str_disconnecting_from_network);
  if (host_flag != '\0') {
    delay_after = 0;
  }
  if (connected == '\0') {
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary_2);
  }
  else {
    free_game_list();
    if (connected_to_server != '\0') {
      XD_FlushSendBuffer(60000);
      dbg_log((const char *)&str_send_buffer_has_flushed);
      msg_queue.reset();
      net_popup_queue_clear();
      XD_CloseConnection();
      msg_queue.reset();
      net_popup_queue_clear();
      dbg_log((const char *)&str_xd_closeconnection);
      close_hook_nop();
    }
    switch(net_protocol) {
    case 0:
      XD_ShutdownSockets();
      dbg_log((const char *)&str_xd_shutdownsockets);
      break;
    case 1:
      XD_ShutdownSockets();
      dbg_log((const char *)&str_xd_shutdownsockets_2);
      break;
    case 2:
      XD_ShutdownModem();
      dbg_log((const char *)&str_xd_shutdownmodem);
      break;
    case 3:
      XD_ShutdownTEN();
      dbg_log((const char *)&str_xd_shutdownten);
    }
    msg_queue.reset();
    net_popup_queue_clear();
    if (0 < poll_depth_peak) {
      dbg_logf((const char *)&str_maximum_gnetmgr_poll_call_depth_d,poll_depth_peak);
    }
    if (0 < (int)msgq_priority_peak) {
      dbg_logf((const char *)&str_maximum_alpha_message_queue_load_d,(iptr)msgq_priority_peak);
    }
    if (0 < (int)msgq_normal_peak) {
      dbg_logf((const char *)&str_maximum_message_queue_load_d,(iptr)msgq_normal_peak);
    }
    if (stacked_draw_peak != 0) {
      dbg_logf((const char *)&str_maximum_draw_stack_load_d,(iptr)stacked_draw_peak);
    }
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary);
    if (delay_after != 0) {
      dbg_log((const char *)&str_disconnection_delay_2_seconds);
      ui_delay_ticks(0x78);
    }
  }
  connected = 0;
  connected_to_server = 0;
  listing_games = 0;
  server_conn_id = 0xffffffff;
  net_reset_error_counts();
  msg_queue.reset();
  net_popup_queue_clear();
  return;
}

/* 0059b55b auto */
void NetSession::close_hook_nop()

{
  return;
}

/* 0059b571 auto */
void NetSession::game_list_add(NetServerInfoMsg *msg)

{
  int cmp;
  NetGameInfo *entry;
  i32 now_ticks;
  NetGameInfo *prev;
  NetGameInfo *node2;
  i32 saved_value;
  
  if (listing_games != '\0') {
    node2 = found_games;
    saved_value = 0;
    for (; node2 != 0; node2 = node2->next) {
      cmp = _strncmp(node2->host_address,msg->game.host_address,0x20);
      if (cmp == 0) {
        if (found_games == node2) {
          found_games = node2->next;
        }
        if (node2->prev != 0) {
          node2->prev->next = node2->next;
        }
        if (node2->next != 0) {
          node2->next->prev = node2->prev;
        }
        saved_value = node2->list_id;
        operator_delete(node2);
        break;
      }
    }
    node2 = found_games;
    prev = 0;
    while ((node2 != 0 &&
           (cmp = __strcmpi(msg->game.game_name,node2->game_name), 0 < cmp))) {
      prev = node2;
      node2 = node2->next;
    }
    while (((node2 != 0 &&
            (cmp = __strcmpi(msg->game.game_name,node2->game_name), cmp == 0)) &&
           (cmp = __strcmpi(msg->game.host_address,node2->host_address), 0 < cmp))) {
      prev = node2;
      node2 = node2->next;
    }
    entry = (NetGameInfo *)operator_new(0x124);
    if (entry == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pcur_null,(iptr)&str_d_ss_franklinton_netmgr_cpp_3,0x1cf);
    }
    crt_memcpy(entry,&msg->game,0x124);
    if (prev == 0) {
      entry->next = found_games;
      entry->prev = 0;
      if (found_games != 0) {
        found_games->prev = entry;
      }
      found_games = entry;
    }
    else {
      entry->next = prev->next;
      entry->prev = prev;
      prev->next = entry;
      if (entry->next != 0) {
        entry->next->prev = entry;
      }
    }
    now_ticks = timer_ticks();
    entry->last_seen = now_ticks;
    entry->list_id = saved_value;
  }
  return;
}

/* 0059b7fc auto */
void NetSession::add_player(NetPlayerNodeMsg *player_info)

{
  int slot;
  
  slot = 0;
  while( true ) {
    if (6 < slot) {
      return;
    }
    if (net_slots[slot].active == '\0') break;
    slot++;
  }
  net_slots[slot].id = player_info->slot.id;
  _memset((void *)((int)this + slot * 0x54 + 0x208),0,0x20);
  _strncpy((char *)((int)this + slot * 0x54 + 0x208),(char *)((iptr)player_info + 0x14),0x20);
  net_slots[slot].is_host = 0;
  net_slots[slot].active = 1;
  net_slots[slot].civ = 0xffffffff;
  crt_strcpy((char *)((void *)((int)this + slot * 0x54 + 0x228)),(const char *)((const void *)((iptr)player_info + 0x34)));
  net_slots[slot].field_44 = 0;
  net_slots[slot].lobby_id = 0;
  net_player_count[0] = net_player_count[0] + 1;
  players_awaited--;
  return;
}

/* 0059b96a auto */
void NetSession::remove_player(int player_id)

{
  u8 *civ_slot_ptr;
  int slot;
  
  slot = 0;
  while ((slot < 7 && (net_slots[slot].id != player_id))) {
    slot++;
  }
  if ((slot < 7) && (net_slots[slot].active != '\0')) {
    if ((0 < net_slots[slot].civ) &&
       (net_slots[slot].civ < 8)) {
      game.human_players =
           ~(byte)(1 << (*(byte *)((int)this + slot * 0x54 + 0x254) & 0x1f)) & game.human_players
      ;
    }
    net_slots[slot].id = 0;
    net_slots[slot].address[0] = 0;
    net_slots[slot].is_host = 0;
    net_slots[slot].active = 0;
    civ_slot_ptr = (u8 *)net_civ_slots + net_slots[slot].civ * 4;
    civ_slot_ptr[0] = 0xff;
    civ_slot_ptr[1] = 0xff;
    civ_slot_ptr[2] = 0xff;
    civ_slot_ptr[3] = 0xff;
    net_slots[slot].civ = 0xffffffff;
    net_player_count[0] = net_player_count[0] - 1;
  }
  return;
}

/* 0059baf0 auto */
void NetSession::free_game_list()

{
  NetGameInfo *next;
  
  while (found_games != 0) {
    next = found_games->next;
    operator_delete(found_games);
    found_games = next;
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
    net_send_message(NM_PLAYER_LIST_NODE,0,(int)conn_id,0,0,0,0,0,0,0);
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
net_msg_build_packet(i32 msg_type,iptr payload,i32 arg1,i32 arg2,
            i32 arg3,i32 arg4,i32 arg5,i32 arg6,
            size_t payload_len)

{
  void *packet;
  NetMsg header;  /* only the 16-byte header is used */
  size_t payload_size;
  
  header.hdr.header_init(msg_type);
  if (payload_len == 0) {
    if (payload == 0) {
      payload_size = 0;
      packet = operator_new(0x2c);
    }
    else {
      payload_size = __msize((void *)payload);
      packet = crt_realloc((void *)payload,payload_size + 0x2c);
      crt_memmove((void *)((int)packet + 0x2c),packet,payload_size);
    }
  }
  else {
    payload_size = payload_len;
    packet = operator_new(payload_len + 0x2c);
    crt_memcpy((void *)((int)packet + 0x2c),(const void *)payload,payload_size);
  }
  header.size = payload_size + 0x2c;
  crt_memcpy(packet,&header,0x10);
  crt_memcpy((void *)((int)packet + 0x10),&arg1,4);
  crt_memcpy((void *)((int)packet + 0x14),&arg2,4);
  crt_memcpy((void *)((int)packet + 0x18),&arg3,4);
  crt_memcpy((void *)((int)packet + 0x1c),&arg4,4);
  crt_memcpy((void *)((int)packet + 0x20),&arg5,4);
  crt_memcpy((void *)((int)packet + 0x24),&arg6,4);
  crt_memcpy((void *)((int)packet + 0x28),&payload_size,4);
  return packet;
}

/* 0059c276 auto */
void __cdecl net_reset_msg_counts(void)

{
  int i;
  
  for (i = 0; i < 0xa9; i++) {
    net.nm_recv_counts_by_type[i] = 0;
  }
  return;
}

/* 0059c2b8 auto */
void __cdecl net_reset_error_counts(void)

{
  int i;
  
  net.lost_conn_total = 0;
  for (i = 0; i < 7; i++) {
    net.lost_conn_by_player[i] = 0;
  }
  return;
}

/* 0059c301 auto */
void __cdecl net_modal_idle_poll(void)

{
  net.process_messages(1,0);
  return;
}

/* 0059c31f auto */
/* WARNING: Type propagation algorithm not settling */

void __cdecl net_fill_game_info(NetGameInfo *info)

{
  i32 bit_masks[9];
  
  bit_masks[0] = 0;
  bit_masks[1] = 1;
  bit_masks[2] = 3;
  bit_masks[3] = 7;
  bit_masks[4] = 0xf;
  bit_masks[5] = 0x1f;
  bit_masks[6] = 0x3f;
  bit_masks[7] = 0x7f;
  bit_masks[8] = 0xff;
  crt_strcpy(info->version,str_ptr_5_4_0f_multiplayer_26_march_99);
  _memset(info->host_address,0,0x20);
  _strncpy(info->host_address,((i8 *)&menu_prefs.net_ip_address),0x20);
  _memset(info->game_name,0,0x20);
  _strncpy(info->game_name,((i8 *)&net.net_game_name),0x20);
  _memset(info->host_name,0,0x20);
  _strncpy(info->host_name,((i8 *)&menu_prefs.net_player_name),0x20);
  info->open_game = mp_options.open_game;
  info->difficulty = game.difficulty;
  info->barbarian_activity = game.barbarian_activity;
  if (game.civs_alive == '\0') {
    info->civs_alive = *(i8 *)(bit_masks + (uint)game.num_enemies + 2);
  }
  else {
    info->civs_alive = game.civs_alive;
  }
  info->player_mask = *(i8 *)(bit_masks + net.net_player_count[0]);
  info->turn_time_limit_ms = civ_saved_cursor_x_block.mp_turn_time_limit_ms;
  crt_strcpy(info->scenario_name,((i8 *)&scenario.name));
  info->double_moves = mp_doubles.moves;
  info->double_production = mp_doubles.production;
  info->game_flags = game.game_flags;
  info->first_launch_turn = game.first_launch_turn;
  info->year_increment = scenario.year_increment[0];
  info->start_year = scenario.start_year;
  info->turn = game.turn;
  info->simultaneous = mp_options.simultaneous_setting;
  info->prev = 0;
  info->next = info->prev;
  info->list_id = 0;
  info->last_seen = info->list_id;
  info->map_width = map.width;
  info->map_height = map.height;
  return;
}

/* 0059d080 auto */
NetMsgHeader *NetPlayerInfoMsg::init()

{
  iptr j;
  int i;
  
  _memset(this,0,sizeof *this);   /* (BF-286) nothing of the last names after the new ones */
  hdr.header_init(0x2e);
  civ = 0;
  leader_index = 0;
  gender = 0;
  leader_name[0] = 0;
  tribe_name[0] = 0;
  adjective[0] = 0;
  city_style = 0;
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 0x18; j++) {
      govt_titles[i][j] = 0;
    }
  }
  has_names = 0;
  has_titles = 0;
  size = 0x118;
  return &hdr;
}

/* 005a9f30 auto */
void __cdecl net_host_wait_production_idle(void)

{
  bool all_done;
  int civ;
  
  net.process_messages(1,0);
  if ((net.nm_production_complete_count != 0) || (net.net_player_count[0] < 2)) {
    all_done = true;
    for (civ = 1; civ < 8; civ++) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        all_done = false;
        break;
      }
    }
    if (all_done) {
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
  }
  return;
}

/* 005aa004 auto */
void __cdecl net_host_wait_human_moves_idle(void)

{
  bool all_done;
  int civ;
  
  net.process_messages(1,0);
  if ((net.nm_human_move_complete_count != 0) || (net.net_player_count[0] < 2)) {
    all_done = true;
    for (civ = 1; civ < 8; civ++) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        all_done = false;
        break;
      }
    }
    if ((all_done) || (net.net_player_count[0] < 2)) {
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
  }
  return;
}

/* 005aa0e5: written by hand in src/game/bigframes.cpp */

/* 005ab0a3 auto */
void __cdecl net_client_wait_production_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_ai_movement_count != 0) || (net.nm_end_sequence_count != 0)) || (turn_loop.active == '\0')) ||
     (net.lost_conn_total != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab120 auto */
void __cdecl net_client_wait_ai_moves_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_human_movement_count != 0) || (net.nm_end_sequence_count != 0)) || (turn_loop.active == '\0')) ||
     (net.lost_conn_total != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab19d auto */
void __cdecl net_client_wait_human_moves_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_production_begin_count != 0) || (net.host_handoff_stage == 1)) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit == 1 || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab23a auto */
void __cdecl net_client_hotwait_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_production_begin_count != 0) || (net.host_handoff_stage != '\0')) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit != '\0' || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* Part of net_client_turn_loop (005ab2d5): our turn played by the player:
   production, the wait for the AI's and the other humans' moves, the
   our-turn-to-move popup (with the casualties), then the moves. 1: a
   connection lost (dialog2 destroyed; the caller returns 0). */
static int net_client_human_turn(Dialog &dialog2,i8 &turn_ok)
{
  iptr str;
  int status;
  int new_casualties;
  i32 seh_state;
  
  if (turn_loop.resume_civ == '\0') {
    civ_check_throne_room_upgrade();
  }
  if ((turn_loop.resume_civ == '\0') || (turn_resume_new_game != 0)) {
    civ_process_turn(human_civ[0]);
    net_send_state_diff(0xff,2,0,0,0);
  }
  XD_FlushSendBuffer(60000);
  game_turn_start_popups();
  net_send_message(NM_PRODUCTION_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
  net_heartbeat_send(net.slot_civs[0]);
  dlg_style.pending_idle_callback = net_client_wait_production_idle;
  popup_game_with_sprite((const char *)&str_waitproduction_3,(iptr)&popup_info_icon_sprite,0);
  while (((net.nm_ai_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
    net.process_messages(1,0);
    net_heartbeat_check(net.slot_civs[0]);
    status = net_check_lost_connections();
    if (status != 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      return 1;
    }
  }
  if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
    net.nm_ai_movement_count = 0;
    net_heartbeat_send(net.slot_civs[0]);
    dlg_style.pending_idle_callback = net_client_wait_ai_moves_idle;
    popup_game_with_sprite((const char *)&str_waitaimoves,(iptr)&popup_info_icon_sprite,0);
    while (((net.nm_human_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
      net.process_messages(1,0);
      net_heartbeat_check(net.slot_civs[0]);
      status = net_check_lost_connections();
      if (status != 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        return 1;
      }
    }
    if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
      net.nm_human_movement_count = 0;
      if ((civ_view.intro_shown[human_civ[0]] == 0) && (autoplay.active == 0)) {
        civ_intro_popup(human_civ[0]);
        civ_view.intro_shown[human_civ[0]] = 1;
      }
      new_casualties = net.casualty_counts[human_civ[0]] -
                 net.casualty_reported_counts[human_civ[0]];
      if (autoplay.active == 0) {
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        str = civ_leader_title(human_civ[0]);
        text_set_string_arg(0,(const char *)str);
        str = (iptr)(civ_leader_name(human_civ[0]));
        text_set_string_arg(1,(const char *)str);
        if (new_casualties < 1) {
          dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_ourturntomove_4);
          dialog2.add_sprite((iptr)&popup_info_icon_sprite,0,0);
          dialog2.set_image_zoom(0);
          input_flush_events();
          snd_play_effect(SND_NEWONDER,0,0,0);
          dialog2.show_modal(0);
        }
        else {
          net.casualty_reported_counts[human_civ[0]] =
               net.casualty_counts[human_civ[0]];
          text_set_number_arg(0,new_casualties);
          if (new_casualties == 1) {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualty_5);
          }
          else {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualties_5);
          }
          dialog2.add_sprite((iptr)&popup_info_icon_sprite,0,0);
          dialog2.set_image_zoom(0);
          input_flush_events();
          snd_play_effect(SND_NEWONDER,0,0,0);
          dialog2.show_modal(0);
          if (dialog2.result_extra != 0) {
            casualty_timeline_show(human_civ[0]);
          }
        }
      }
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_turn_timer_start(1);
      }
      game_player_move_loop();
    }
    else {
      net.nm_end_sequence_count = 0;
    }
  }
  else {
    turn_ok = 0;
  }
  return 0;
}

/* Part of net_client_turn_loop (005ab2d5): our turn played by the AI
   (autoplay). 1: a connection lost (dialog2 destroyed; the caller returns 0).
   */
static int net_client_autoplay_turn(Dialog &dialog2,i8 &turn_ok)
{
  SHORT esc_state;
  int status;
  i32 seh_state;
  
  map_views_block.saved_human_players = (uint)game.human_players;
  game.human_players = '\0';
  if ((turn_loop.resume_civ == '\0') || (turn_resume_new_game != 0)) {
    civ_process_turn(human_civ[0]);
  }
  net_send_message(NM_PRODUCTION_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
  net_heartbeat_send(net.slot_civs[0]);
  dlg_style.pending_idle_callback = net_client_wait_production_idle;
  popup_game_with_sprite((const char *)&str_waitproduction_2,(iptr)&popup_info_icon_sprite,0);
  while (((net.nm_ai_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
    net.process_messages(1,0);
    net_heartbeat_check(net.slot_civs[0]);
    status = net_check_lost_connections();
    if (status != 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      return 1;
    }
  }
  if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
    net.nm_ai_movement_count = 0;
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    ai_process_civ_units();
    game.human_players = (*(u8 *)&map_views_block.saved_human_players);
    esc_state = GetAsyncKeyState(0x1b);
    if (((int)esc_state & 0x8001U) != 0) {
      autoplay.turns_left = 1;
    }
  }
  else {
    turn_ok = 0;
  }
  return 0;
}

/* 005ab2d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_client_turn_loop(void)

{
  char resume_civ;
  iptr str;
  int status;
  i32 ret;
  iptr slot_civ;
  int civ;
  iptr unit;
  Dialog dialog2;
  i8 turn_ok;
  int centered;
  int is_first_turn;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  is_first_turn = 1;
  dialog2.Dialog::construct(0x4000);
  seh_state = 0;
  turn_ok = 1;
  slot_civ = net.slot_civs[0];
  str = (iptr)(civ_name_plural(net.slot_civs[0]));
  dbg_logf_str_int((const char *)&str_server_s_d_4,(const char *)str,slot_civ);
  for (civ = 1; civ < 8; civ++) {
    if (((net.net_slots[net.net_civ_slots[civ]].id != 0) &&
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      status = civ;
      str = (iptr)(civ_name_plural(civ));
      dbg_logf_str_int((const char *)&str_client_s_d_4,(const char *)str,status);
    }
  }
  net_reset_error_counts();
  human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
  turn_loop.resume_civ = saved_turn.civ;
  turn_resume_new_game = (int)saved_turn.resume_new_game;
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  game.player_civ = (byte)human_civ[0];
  game.turn_civ = (byte)human_civ[0];
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  centered = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y);
      centered++;
      break;
    }
  }
  if (centered == 0) {
    for (unit = 0; unit < game.num_cities; unit++) {
      if ((cities[unit].id != 0) && (cities[unit].owner == human_civ[0])) {
        ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)cities[unit].x,(int)cities[unit].y
                  );
        break;
      }
    }
  }
  status_draw_info(1);
  status_draw_civ_color(human_civ[0]);
  ui_redraw_all_maps(1);
  resume_civ = turn_loop.resume_civ;
  if ((1 << ((byte)human_civ[0] & 0x1f) & (uint)game.human_players) == 0) {
    net_heartbeat_send(net.slot_civs[0]);
    dlg_style.pending_idle_callback = net_client_hotwait_idle;
    popup_game_sprite_flags((const char *)&str_clienthotwait_2,0x2000000,(iptr)&popup_info_icon_sprite,0);
    resume_civ = turn_loop.resume_civ;
  }
  do {
    turn_loop.resume_civ = resume_civ;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
        unit_exhaust_moves(unit);
      }
    }
    if (is_first_turn == 0) {
      net_heartbeat_send(net.slot_civs[0]);
      dlg_style.pending_idle_callback = net_client_wait_human_moves_idle;
      popup_game_with_sprite((const char *)&str_waithumanmoves_2,(iptr)&popup_info_icon_sprite,0);
    }
    net_heartbeat_send(net.slot_civs[0]);
    while ((((net.nm_production_begin_count == 0 && (net.host_handoff_stage == '\0')) && (net.nm_end_sequence_count == 0)) &&
           ((net.player_has_quit == '\0' && (turn_loop.active != '\0'))))) {
      net.process_messages(1,0);
      net_heartbeat_check(net.slot_civs[0]);
      status = net_check_lost_connections();
      if (status != 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 0;
        return ret;
      }
    }
    if (net.host_handoff_stage != '\0') {
      status = net_client_transfer_server();
      if (status == 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 0;
        return ret;
      }
      if (net.host_flag != '\0') {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 1;
        return ret;
      }
      if (((net.nm_production_begin_count == 0) && (net.nm_end_sequence_count == 0)) &&
         ((net.player_has_quit == '\0' && (resume_civ = turn_loop.resume_civ, turn_loop.active != '\0'))))
      continue;
    }
    if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
      net.nm_start_civ_count = 0;
      net.nm_production_begin_count = 0;
      human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
      game.player_civ = (byte)human_civ[0];
      game.turn_civ = (byte)human_civ[0];
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
        map_views_block.main_view_timer_cache_i32 = 0xffffffff;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
      net.mp_turn_civ = human_civ[0];
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      map_redraw_all_views(human_civ[0],1);
      status_draw_info(1);
      status_draw_civ_color(human_civ[0]);
      if (autoplay.active == 0) {
        if (net_client_human_turn(dialog2,turn_ok) != 0) {
          ret = 0;
          return ret;
        }
      }
      else {
        if (net_client_autoplay_turn(dialog2,turn_ok) != 0) {
          ret = 0;
          return ret;
        }
      }
    }
    else {
      turn_ok = 0;
    }
    mp_turn_timer_stop(1);
    status_redraw(1);
    ui_pump_messages();
    if (game_over_reason != 0) {
      turn_loop.active = '\0';
    }
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_HUMAN_MOVE_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    saved_turn.civ = '\0';
    turn_loop.resume_civ = '\0';
    saved_turn.resume_new_game = 0;
    turn_resume_new_game = 0;
    is_first_turn = 0;
    resume_civ = '\0';
    if (turn_loop.active == '\0') {
      net_send_message(NM_QUIT_REQUEST,0,human_civ[0],0,0,0,0,0,0,0);
      XD_FlushSendBuffer(60000);
      net.shutdown(1);
      if (game_over_reason != 0) {
        game_handle_game_over();
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      ret = 0;
      return ret;
    }
  } while( true );
}

/* 005b9ec6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_begin(void)

{
  if (2 < game.multiplayer) {
    net.tile_sync_enabled = 0;
    net.tile_batch_active = 1;
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch.used_dwords = 1;
  }
  return;
}

/* 005b9f1c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_flush(void)

{
  if (2 < game.multiplayer) {
    net.tile_sync_enabled = 1;
    net.tile_batch_active = 0;
    if (net.host_flag == '\0') {
      if (1 < net_batch.used_dwords) {
        net_send_message(NM_MAP_PACKETS,0,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(5000);
        _memset(((i8 *)&net_batch_buffer),0,0x400);
        net_batch_buffer[0] = 0;
        net_batch.used_dwords = 1;
      }
    }
    else {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
  }
  return;
}

/* 005b9fde auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
net_batch_add(int record_type,i32 x,i32 y,i32 arg1,iptr arg2,
            i32 arg3)

{
  if (net.host_flag != '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_bserver_3,(iptr)&str_d_ss_franklinton_map_cpp,0x3de);
  }
  if (0x100 - net_batch.used_dwords < net_batch.record_sizes[record_type] + 1) {
    net_send_message(NM_MAP_PACKETS,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch.used_dwords = 1;
  }
  net_batch_buffer[0]++;
  net_batch_buffer[net_batch.used_dwords] = record_type;
  net_batch.used_dwords++;
  switch(record_type) {
  case 0:
  case 2:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg2;
    net_batch.used_dwords++;
    break;
  case 1:
  case 3:
  case 4:
  case 5:
  case 6:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    break;
  case 7:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg2;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg3;
    net_batch.used_dwords++;
  }
  return;
}

/* 005ba206 auto */
void __cdecl net_apply_tile_updates(iptr batch)

{
  int record_type;
  i8 saved_sync;
  iptr count_offset;
  int records_left;
  iptr arg_idx;
  iptr args[5];
  int pos;
  
  pos = 0;
  if (2 < game.multiplayer) {
    if (net.host_flag == '\0') {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_bserver_4,(iptr)&str_d_ss_franklinton_map_cpp_2,0x416);
    }
    saved_sync = net.tile_sync_enabled;
    net.tile_sync_enabled = 0;
    count_offset = pos * 4;
    pos++;
    for (records_left = *(int *)(batch + count_offset); 0 < records_left; records_left--) {
      record_type = *(int *)(batch + pos * 4);
      for (arg_idx = 0; pos = pos + 1, arg_idx < net_batch.record_sizes[record_type] - 1;
          arg_idx = arg_idx + 1) {
        args[arg_idx] = *(iptr *)(batch + pos * 4);
      }
      switch(record_type) {
      case 0:
        tile_set_improvements(args[0],args[1],args[2],args[3],1);
        break;
      case 1:
        tile_set_terrain(args[0],args[1],args[2],1);
        break;
      case 2:
        tile_set_visible(args[0],args[1],args[2],args[3],1);
        break;
      case 3:
        tile_set_fertility(args[0],args[1],args[2],1);
        break;
      case 4:
        tile_set_owner(args[0],args[1],args[2],1);
        break;
      case 5:
        tile_set_landmass(args[0],args[1],args[2],1);
        break;
      case 6:
        tile_set_radius_civ(args[0],args[1],args[2],1);
        break;
      case 7:
        known_set(args[0],args[1],args[2],args[3],args[4],1);
      }
    }
    net.tile_sync_enabled = saved_sync;
    net_send_state_diff(0xff,2,0,0,0);
  }
  return;
}
