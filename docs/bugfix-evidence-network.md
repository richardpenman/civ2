# Network-input boundaries

These checks apply in every mode under [BUGS.md](BUGS.md)'s memory-error policy, including
`CIV2_CLASSIC` and mixed original/OpenCiv2 games. Accepted messages retain their bytes and
normal handling. There is no new packet format or change to valid gameplay. Malformed
messages are dropped; diagnostic message names outside the table are printed numerically.

`BF-*` below are Peter Johnson's independent reconstruction's ledger identifiers. The
corresponding defects remain in base `17842e4497c5a6d3d5be9245cf080ce0dde535fc`.

## First tranche: envelope, discovery and chat

| Evidence | Original/current boundary | Correction |
| --- | --- | --- |
| BF-179, BF-280 | `0059bc41` secure callback; `0047f13d` within `NetSession::process_messages` | Require all 16 header bytes within the captured transport/queue size before header fields or receive counters. Guard the 169-entry message-counter/name domain too. `NetMsgQueue` already retains the captured allocation length; do not trust the peer's advertised length instead. |
| BF-179, BF-181 | `0059bc41` invalid-message diagnostic scan | Search only candidate offsets with a complete four-DWORD header. Copy unaligned candidates to a local header. Bound the diagnostic name table; unknown values are logged as numbers, never used as pointers. |
| BF-180, BF-223, BF-246 | `0059bb54` broadcast filtering; `recv_hello` / `game_list_add` | Require a complete named discovery record, an in-range inner type and terminated address/filter strings before comparisons. Server announcements require the complete record and a NUL inside each fixed string that later list/UI consumers read. |
| BF-177, BF-183, BF-276 | `00480317`, `NM_CHAT` | Require the seven-argument prefix, 1..257 captured text bytes and a NUL within that payload. The 257-byte limit comes from the actual kind-4 `chat.input_buffer` allocation and sender. Sender/recipient must be civilization slots 0..7 before naming, mute-array or relay use. |

The shared guard runs before queue admission in both callbacks and before counters/dispatch
after dequeue. This also protects queue-full diagnostic names from out-of-table outer types.
Discovery filtering keeps the original semantics: an empty or matching target is accepted,
and a nonempty self-address is ignored. Chat text is rejected rather than truncated or
terminated artificially. Bytes after an existing NUL remain unchanged.

Run the focused executable regression:

```sh
CXX='g++ -m32' python3 tests/source_probe.py tests/network_input_test.cpp
# Or: set CXX=cl, in an x86 Visual Studio developer shell, then the Python command.
```

It compiles the actual guard and both callbacks with production message layouts and small
queue/log providers. Debug and optimized runs check every short header, discovery-prefix
and server-announcement extent; unterminated strings; invalid outer/inner types; retained
target/self filtering; valid, empty, unterminated and maximum/oversized chats; invalid civ
slots; and fitting/nonfitting unaligned diagnostic headers. The copy/search providers check
their reads against the captured extent even when the test allocation has extra backing.
Valid secure packets must enqueue byte-for-byte unchanged. Full poll-loop execution,
physical transport, mixed-peer sessions and replay equivalence are not established here.

## Second tranche: message bodies

The same guard (`net_received_message_valid`) now also checks each message's body before it is
queued or dispatched, in every mode. The sizes come from the senders (`net_send_message`,
`net_msg_build_packet`, the raw difference-engine and parley packets) and from what each handler
reads; a message the original sends is never refused.

| Evidence | Boundary | Correction |
| --- | --- | --- |
| BF-242..245, 249..275, 277, 278 | `net_message_min_size` | One minimum per type: 16 for header-only messages, 44 for those whose handler reads the seven arguments, 44 + N for the rule tables and the city/unit records, and the fixed structures (player list 0x280, server path 0x21c, answer 0x134, player 0x118, player node 0x64, whose turn 20). |
| BF-130..133, 241, 275 | `NM_POPUP`, `net_popup_args_valid` | The payload is the rest of the message (`extra_len`), its strings end within it and within a 260-byte `text_args` slot (24 slots), its numbers (6) fit; the argument each popup kind draws or names with lies in its table (civ 0..7, unit 0..2047, unit sprite 0..62, improvement 0..66, wonder 0..27, advance 0..99, government 0..6, city 0..255). |
| BF-178 | `NM_PARLEY_TRANSACTION`, `NM_PARLEY_EXECUTE`, `net_parley_offer_valid` | The first item, and for an exchange the second `item.size` bytes further, inside the message; the ids of advance, unit, war-target and city items too. |
| BF-243 | `NM_MAP_PACKETS` | Every record's type 0..7 (the `net_batch.record_sizes` index) and its arguments inside the message. |
| BF-092, BF-277 | `NM_DIFFERENCE_ENGINE`, `NM_ALPHA_DIFFERENCE_ENGINE`; `net_state_diff_record_fits` in `recv_state_diff` | The records inside the message; each one's section 0..0x17 and the bytes it writes, raw or decoded, inside the section's padded size, as the sender's are. A record that is not ends the diff. |
| BF-092 | `diffeng_rle_decode` (`src/auto/game.cpp`) | Code 0x7fff is the encoder's 32767-byte run, decoded as the size pass counts it, not as a 65535-byte literal; every code is checked against both ends. The game's own streams never have it: difference records are at most `packet_flush_size` (1 KB) and the full image (`NM_FULL_GAMESTATE`), the other user, is never sent. |
| BF-213, 216, 218, 221 | `net_rule_table_valid` | The advances' prerequisites (-2..99), category (0..4) and epoch (0..3); improvement, wonder-expiry and unit advance indices (-2..99); the base terrains' transform, irrigation and mine results (-2..10). |
| BF-137, 167, 169, 196, 203, 205..211, 227, 228, 233, 240, 263 and `NM_QUIT_REQUEST` | `net_message_body_valid` | Civ selectors 0..7 (the conqueror of `NM_KILL_CIV` -1..7), map-lock rows 0..7, units 0..2047 (-1 where the callee ignores it), cities 0..255 (`NM_DELETE_CITY` keeps the negatives `city_delete` ignores), wonders 0..27, unit types 0..61, city and unit counts at most 256 and 2048, events loaded 0/1 and at most 110. |
| BF-222, 247, 248, 171, 249 | strings and the player record | A NUL inside each string the handlers copy (server path, answer, player node, player record and its titles); the player's civ, leader, gender and city style inside their tables. The host's scenario folder and file, after our directory, must fit the 260-byte paths `net_join_load_scenario` builds, else the join fails as for a missing folder. |

Elsewhere: host migration stops (fails, as when no one came) where the original stopped at its
assertion for a player missing from the old list, and writes `net_civ_slots` only for a civ 0..7
(BF-162); `diffeng_buf_append` copies nothing past its buffer (BF-083); `net_send_message` zeroes
the discovery, answer, path and player-node messages before filling them, and
`NetPlayerInfoMsg::init` its record (BF-286, BF-293); the Leonardo notice and the caption skip a
civ whose player left (`net_civ_slots` -1, BF-194, BF-203); the diplomat's chat (network games
only) takes 0 for attitudes past its 15-entry table (BF-289).

A message refused at the secure callback, a diff record outside its section and a run-length
record that does not decode are logged (`SecureReceiveFunc: refused ...`, `recv_state_diff: ...`,
debug lines in our log): the two-game tests (tools/xdnet/nettest.py, ours with the original both
ways, chat, parley, save and load, 20 turns) log none.

The focused regression above now also covers every listed minimum (one byte short refused), the
selectors at and past their limits, popup payloads and kinds, parley items, map batches, diff
records, run-length streams (an encoded 40000-byte run and 33000-byte literal round trip, the
0x7fff run, cut and overlong codes) and the record-in-section check. `make test` runs the three
source probes (tests/*_test.cpp) after the recorded cases.

## Remaining work

- Name ordinals in the rule tables (BF-219, BF-239): `label_string` walks the string pool by
  ordinal; checking them needs the receiver's pool count (`strpool.count`), and a host with other
  rules text would then be refused. Not done.
- The contents of state images and difference records: active units' types, owners and stack
  links, city records, map dimensions, region and owner nibbles, turn and objective civs (BF-093,
  BF-094, BF-168, BF-170, BF-230, BF-232, BF-236, BF-285, BF-287, BF-288, BF-291), and the section
  headers of full images (`diffeng_apply_image`, never sent by the game). Saves now check them
  ([bugfix-evidence-files.md](bugfix-evidence-files.md)); a join's records arrive apart, so the
  same check after each record would refuse valid joins.
- Parley item ids (advances, units, cities, civs) and items whose kind does not match the mode;
  the city of a joining unit's home (BF-233's cross-check).
- `net_slots[net_civ_slots[civ]]` with a civ whose player left (-1) is used in about 170 other
  places, mostly as a send target.
- Map coordinates in map and unit messages are not checked here; the tile accessors bound them.
