#!/usr/bin/env python3
"""Write src/game/rules_default.cpp, the game's built-in rules (rules.h),
from an install's RULES.TXT: the values and names the original's parser
(src/auto/civ.cpp, rules_load_*, rules_parse_sections) takes from it, read
the way it reads them (text_read_line, text_next_token, text_next_int, the
abbreviations of advances and terrain).  The file's comments and prose are
not copied.  A dev tool: the output is checked against the install by
`civ2-unittest --rules GAME` (make test).

    python3 tools/gen_rules.py [RULES.TXT] [OUT.cpp]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import paths  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def enum_names(prefix, enum):
    """The enumerators PREFIX_* of enum ENUM in enums.h, by value."""
    src = open(os.path.join(ROOT, 'src/include/enums.h')).read()
    body = re.search(r'enum %s\b[^{]*\{(.*?)\};' % enum, src, re.S).group(1)
    names = {}
    for name, value in re.findall(r'\b(%s\w+)\s*=\s*(\w+)' % prefix, body):
        v = int(value, 0)
        names.setdefault(v, name)
    return names


def init_strings(var):
    """The abbreviations in gamedata.cpp's VAR_init (the records' first field)."""
    src = open(os.path.join(ROOT, 'src/data/gamedata.cpp')).read()
    lit = re.search(r'static const char %s_init\[\] = "(.*?)";' % var, src).group(1)
    return [s for s in lit.split('\\0') if s]


ADV = enum_names('ADV_', 'AdvanceId')
TERRAIN = enum_names('TERRAIN_', 'TerrainId')
DOMAIN = enum_names('DOMAIN_', 'UnitDomain')
ROLE = enum_names('ROLE_', 'UnitRole')
UTYPE = enum_names('UTYPE_', 'UnitTypeFlag')
ADV_ABBREV = init_strings('advances')
TERRAIN_ABBREV = init_strings('terrain_types')[:11]


class Rules:
    """RULES.TXT as the game's text reader sees it."""

    def __init__(self, path):
        data = open(path, 'rb').read().decode('latin-1')
        self.lines = [l.rstrip('\r\n') for l in data.split('\n')]
        self.pos = 0
        self.cursor = ''

    def section(self, name):
        tag = '@' + name.upper()
        for i, line in enumerate(self.lines):
            if line.strip(' \t').upper() == tag:
                self.pos = i + 1
                return
        raise SystemExit('no @%s' % name)

    def line(self):
        """text_read_line(1): the next line (254 bytes at most), trimmed."""
        line = self.lines[self.pos][:254] if self.pos < len(self.lines) else ''
        self.pos += 1
        self.cursor = line.strip(' \t')
        return self.cursor

    def token(self):
        """text_next_token: up to the next comma, trimmed."""
        tok, _, rest = self.cursor.partition(',')
        self.cursor = rest
        return tok.strip(' \t')

    def int(self, tok=None):
        """text_next_int (str_parse_int_prefixed: 0x, 0b, 0d, else atoi)."""
        t = self.token() if tok is None else tok
        if t[:1] == '0' and t[1:2].upper() in ('X', 'B', 'D'):
            base = {'X': 16, 'B': 2, 'D': 10}[t[1].upper()]
            m = re.match(r'[0-9a-fA-F]*' if base == 16 else r'[01]*' if base == 2 else r'[+-]?\d*', t[2:])
            return int(m.group(0), base) if m.group(0) not in ('', '+', '-') else 0
        m = re.match(r'\s*([+-]?\d+)', t)
        return int(m.group(1)) if m else 0

    def binary(self):
        """text_next_binary."""
        v = 0
        for c in self.token():
            if c not in '01':
                break
            v = v * 2 + (c == '1')
        return v & 0xffff


def clamp(v, lo, hi):
    return max(lo, min(hi, v))


def advance(tok):
    """rules_parse_advance_abbrev, as a C expression."""
    if tok == 'no':
        return 'RULES_NO'
    if tok == 'nil':
        return 'RULES_NIL'
    if tok in ADV_ABBREV:
        return ADV[ADV_ABBREV.index(tok)]
    raise SystemExit('unknown advance %r' % tok)


def terrain(tok):
    """rules_parse_terrain_abbrev, as a C expression."""
    if tok.lower() == 'no':
        return 'TERRAIN_RULE_NO'
    if tok.lower() == 'yes':
        return 'TERRAIN_RULE_YES'
    if tok in TERRAIN_ABBREV:
        return TERRAIN[TERRAIN_ABBREV.index(tok)]
    raise SystemExit('unknown terrain %r' % tok)


def cstr(s):
    out = '"'
    for ch in s:
        if ch in '"\\':
            out += '\\' + ch
        elif ' ' <= ch <= '~':
            out += ch
        else:
            out += '\\%03o' % ord(ch)
    return out + '"'


def flags(v):
    if not v:
        return '0'
    parts = [UTYPE[1 << b] if (1 << b) in UTYPE else '0x%x' % (1 << b) for b in range(16) if v & (1 << b)]
    return ' | '.join(parts)


def table(rows, header):
    """Rows of C fields, aligned in columns; header: a comment over them."""
    widths = [max(len(r[i]) for r in rows) for i in range(len(rows[0]))]
    out = ['    /* %s */' % header] if header else []
    for r in rows:
        cells = [c + ',' for c in r[:-1]] + [r[-1]]
        out.append('    {' + ' '.join(c.ljust(w + 1) for c, w in zip(cells[:-1], widths)) + ' ' + cells[-1] + '},')
    return '\n'.join(out)


COSMIC = [
    ('road_move_mult', 1, 10), ('trireme_loss_chance', 1, 100), ('food_per_citizen', 0, 10),
    ('food_box_rows', 4, 20), ('shield_box_rows', 4, 20), ('settler_food_low', 0, 10),
    ('settler_food_high', 0, 10), ('unhappy_size', 4, 12), ('riot_factor', 10, 100),
    ('aqueduct_size', 4, 50), ('sewer_size', 4, 50), ('tech_paradigm', 3, 10),
    ('transform_time', 5, 100), ('monarchy_free_support', 0, 8), ('communism_free_support', 0, 8),
    ('fundamentalism_free_support', 0, 8), ('communism_palace_dist', 1, 20),
    ('fund_science_loss', 0, 100), ('change_penalty', 0, 100), ('paradrop_range', 4, 100),
    ('mass_thrust_paradigm', 25, 200), ('fund_max_science', 0, 10),
]


def generate(path):
    r = Rules(path)
    out = []
    w = out.append
    w('/*\n'
      ' * (port) The game\'s built-in rules (rules.h): what Civilization II\n'
      ' * Multiplayer Gold\'s RULES.TXT gives the game, used when there is no\n'
      ' * RULES.TXT to read.  Generated by tools/gen_rules.py from the game\'s\n'
      ' * RULES.TXT; checked against it by civ2-unittest --rules (make test).\n'
      ' */\n'
      '#include "game.h"\n'
      '#include "rules.h"\n')

    r.section('COSMIC')
    w('/* @COSMIC (as rules_load_cosmic clamps them) */\n'
      'const Cosmic rules_cosmic = {')
    for name, lo, hi in COSMIC:
        r.line()
        v = clamp(r.int(), lo, hi)
        if name == 'food_box_rows' and v & 1:
            v += 1
        w('    .%s = %d,' % (name, v))
    w('};\n')

    r.section('CIVILIZE')
    rows = []
    for i in range(100):
        line = r.line()
        if not line or line[0] == ';':
            break
        name = r.token()
        rows.append([cstr(name), str(r.int()), str(r.int()), '{' + advance(r.token()), advance(r.token()) + '}',
                     str(r.int()), str(r.int())])
    w('/* @CIVILIZE: the advances, the tech tree (the slots after them are\n'
      '   "Advance N", no prerequisites, never available) */\n'
      'const RulesAdvance rules_advances[] = {')
    w(table(rows, 'name, ai_value, civilized_mod, {prerequisites}, epoch, category'))
    w('};\nconst int rules_advance_count = (int)(sizeof rules_advances / sizeof rules_advances[0]);\n')

    r.section('IMPROVE')
    rows = []
    for i in range(67):
        r.line()
        rows.append([cstr(r.token()), str(r.int()), str(r.int()), advance(r.token())])
    w('/* @IMPROVE: improvements, the wonders from 39 */\n'
      'const RulesImprovement rules_improvements[IMP_COUNT] = {')
    w(table(rows, 'name, cost (x10 shields), upkeep, prerequisite'))
    w('};\n')

    r.section('ENDWONDER')
    ends = []
    for i in range(28):
        r.line()
        ends.append(advance(r.token()))
    w('/* @ENDWONDER: the advance that ends each wonder\'s effect */\n'
      'const i8 rules_wonder_expires[WONDER_COUNT] = {')
    for i, e in enumerate(ends):
        w('    %s,%s/* %s */' % (e, ' ' * (24 - len(e)), rows[39 + i][0].strip('"')))
    w('};\n')

    r.section('UNITS')
    rows = []
    for i in range(62):
        line = r.line()
        if not line or line[0] == ';':
            break
        name = r.token()
        obs = advance(r.token())
        domain = DOMAIN[r.int()]
        move, rng, att, dfn, hp, fp = (r.int() for _ in range(6))
        cost = clamp(r.int(), 1, 200)
        hold = r.int()
        role = ROLE[r.int()]
        preq = advance(r.token())
        rows.append([cstr(name), obs, domain, str(move), str(rng), str(att), str(dfn), str(hp), str(fp),
                     str(cost), str(hold), role, preq, flags(r.binary())])
    w('/* @UNITS: the unit types (move and hit points as in RULES.TXT: the\n'
      '   loader multiplies them by cosmic.road_move_mult and 10) */\n'
      'const RulesUnit rules_units[] = {')
    w(table(rows, 'name, obsolete_by, domain, move, range, attack, defense, hit_points, firepower, '
                  'cost, hold, role, prerequisite, flags'))
    w('};\nconst int rules_unit_count = (int)(sizeof rules_units / sizeof rules_units[0]);\n')

    r.section('TERRAIN')
    rows = []
    for i in range(33):
        r.line()
        row = [cstr(r.token()), str(r.int()), str(r.int()), '{%d, %d, %d}' % (r.int(), r.int(), r.int())]
        if i < 11:
            imp = []
            for slot in range(2):
                imp.append((terrain(r.token()), r.int(), r.int(), r.int()))
            row += ['{%s, %s}' % (imp[0][0], imp[1][0]), '{%d, %d}' % (imp[0][1], imp[1][1]),
                    '{%d, %d}' % (imp[0][2], imp[1][2]), '{%d, %d}' % (imp[0][3], imp[1][3]),
                    terrain(r.token())]
        else:
            row += ['{}', '{}', '{}', '{}', '0']
        rows.append(row)
    w('/* @TERRAIN: the 11 terrain types, then each one\'s two special resources\n'
      '   (they have no irrigation, mining, transformation of their own) */\n'
      'const RulesTerrain rules_terrain[TERRAIN_RECORDS] = {')
    w(table(rows, 'name, move_cost, defense, {food, shields, trade}, then for (irrigation, '
                  'mining): {result}, {bonus}, {turns}, {ai_government}, and transform_to'))
    w('};\n')

    r.section('GOVERNMENTS')
    rows = []
    for i in range(7):
        r.line()
        rows.append([cstr(r.token()), cstr(r.token()), cstr(r.token())])
    w('/* @GOVERNMENTS */\n'
      'const RulesGovernment rules_governments[7] = {')
    w(table(rows, 'name, male title, female title'))
    w('};\n')

    r.section('LEADERS')
    rows = []
    for i in range(21):
        r.line()
        male, female = r.token(), r.token()
        is_female = clamp(r.int(), 0, 1)
        color, style = r.int(), r.int()
        plural, adjective = r.token(), r.token()
        att, exp, civ = r.int(), r.int(), r.int()
        titles = []
        while True:
            g = r.int()
            if g <= 0:
                break
            titles.append('{%d, %s, %s}' % (g, cstr(r.token()), cstr(r.token())))
        rows.append([cstr(male), cstr(female), str(is_female), str(color), str(style), cstr(plural),
                     cstr(adjective), str(att), str(exp), str(civ), '{' + ', '.join(titles) + '}'])
    w('/* @LEADERS: the tribes */\n'
      'const RulesLeader rules_leaders[21] = {')
    w(table(rows, 'male and female leader, female (the one that leads), color, city style, plural, '
                  'adjective, attack, expand, civilize, {government, male, female title}'))
    w('};\n')

    r.section('CARAVAN')
    names = []
    for i in range(16):
        r.line()
        names.append(cstr(r.token()))
    w('/* @CARAVAN: the commodities */\n'
      'const char *const rules_commodities[16] = {\n    ' + ', '.join(names) + ',\n};\n')

    r.section('ORDERS')
    rows = []
    for i in range(12):
        r.line()
        name = r.token()
        rows.append([cstr(name), cstr(r.token())])
    w('/* @ORDERS: orders 1 to 12, the name and its key */\n'
      'const RulesOrder rules_orders[12] = {')
    w(table(rows, None))
    w('};\n')

    r.section('DIFFICULTY')
    names = [cstr(r.line()) for _ in range(6)]
    w('/* @DIFFICULTY */\n'
      'const char *const rules_difficulties[6] = {\n    ' + ', '.join(names) + ',\n};\n')

    r.section('ATTITUDES')
    names = [cstr(r.line()) for _ in range(9)]
    w('/* @ATTITUDES */\n'
      'const char *const rules_attitudes[9] = {\n    ' + ', '.join(names) + ',\n};')
    return '\n'.join(out) + '\n'


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else os.path.join(paths.GAME, 'RULES.TXT')
    dst = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, 'src/game/rules_default.cpp')
    text = generate(src)
    with open(dst, 'w') as f:
        f.write(text)
    print('wrote', dst)


if __name__ == '__main__':
    main()
