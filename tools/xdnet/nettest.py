#!/usr/bin/env python3
"""Two-game network test: two copies of the game (ours and/or the original
under Wine), each in its own civ2-dev container on the docker bridge, driven
by xdotool through host -> join -> start -> N turns -> quit; then the two
XDaemon logs are checked.

usage (from the project root, on the docker host):
  tools/xdnet/nettest.py ours-ours   [--turns N] [--run NAME] [--exe EXE] [--keep] [--sniff]
  tools/xdnet/nettest.py ours-orig   (ours hosts, the original joins)
  tools/xdnet/nettest.py orig-ours   (the original hosts, ours joins)
  tools/xdnet/nettest.py orig-orig
  options: --exe civ2-linux32asan   (our build to run; ASan reports are checked)
           --inet                   Internet mode over 127.0.0.1 (both games in one
                                    network namespace, displays :7 and :8)
           --chat                   Chat with Kings both ways, after the turns
           --diplo                  emissary, audience, peace treaty (after the turns)
           --after-late fixed|classic  with --diplo: one more audience after the late grant (bug B6)
           --save / --load          the host saves in its turn 2; --load: then loads it, N more turns
           --rejoin                 the client leaves and joins the game in progress
           --third ours|orig        a third player: the host relays
           --keep                   leave the containers running (drive them by hand)
           --extras --migrate --kick --lobby --simultaneous --timer N --features LIST --save-end
           --exe civ2-linux32cov (gcov data: tools/xdnet/netcov.py)  --exe OpenCiv2.exe (Windows build, Wine)
  (docs/xdaemon-protocol.md section 11.1)

Everything goes to run/xd/cap/NAME/: xdlog-{host,client}.txt (the XD logs),
msgs-{host,client}.txt (the game messages, tools/xdnet/xdmsgs.py), shots/
(screenshots at each step), ours-*.log / wine-*.log, result.txt.

Waits are on conditions where there is one: the XD logs (a message type
sent or received, the turn's NM_START_WHOSE_TURN), the screen (unchanged
for a moment, so a dialog has finished drawing), a process's window; fixed
pauses only between keys.

Checks (result.txt, exit status 0 = all passed):
  - each log's game messages are well formed (magic, size = the size sent);
  - what one side sent over TCP the other received, same order and bytes;
  - both games reached turn N (NM_START_WHOSE_TURN for civ 0 starts a turn);
  - the expected message types in order (join, lock, state transfer,
    turns, city founding, quit);
  - no crash: both processes alive until told to quit, no ASan report,
    no Wine 'Unhandled exception'.
"""
import argparse, os, re, subprocess, sys, time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import xdmsgs  # noqa: E402

try:
    from PIL import Image, ImageChops
except ImportError:  # screen waits degrade to fixed pauses
    Image = None

NM = {}


def nm(name):
    if not NM:
        xdmsgs.load_names()
        NM.update({v: k for k, v in xdmsgs.NAMES.items()})
    return NM[name]


def log(*a):
    print('%7.1f' % (time.time() - T0), *a, flush=True)


T0 = time.time()


class Fail(Exception):
    pass


def sh(*cmd, check=True, capture=False, timeout=None):
    r = subprocess.run(cmd, stdout=subprocess.PIPE if capture else subprocess.DEVNULL,
                       stderr=subprocess.STDOUT if capture else subprocess.DEVNULL, timeout=timeout)
    if check and r.returncode:
        raise Fail('command failed: %s' % ' '.join(cmd))
    return r.stdout.decode(errors='replace') if capture else r.returncode


class Game:
    """One game: KIND 'ours' or 'orig', ROLE 'host' or 'client', in CONTAINER on display DISP."""

    def __init__(self, kind, role, container, disp, run, exe):
        self.kind, self.role, self.c, self.disp, self.run, self.exe = kind, role, container, disp, run, exe
        self.out = os.path.join(ROOT, 'run/xd/cap', run)
        self.shots = os.path.join(self.out, 'shots')
        self.xdlog = os.path.join(self.out, 'xdlog-%s.txt' % role)
        self.n = 0
        self.slow = kind == 'orig'      # Wine: give it more time between keys
        self.civ = None
        self.logpos, self.wins, self.shown, self.pending = 0, {}, set(), None
        self.turn_seen, self.turn_t, self.popped, self.last_pop = 0, 0, False, 0

    # ---- driving ----
    def ex(self, cmd, **kw):
        return sh('docker', 'exec', '-e', 'DISPLAY=:%d' % self.disp, self.c, 'sh', '-c', cmd, **kw)

    def keys(self, *keys, pause=None):
        for k in keys:
            self.ex('xdotool key --delay 120 %s' % k)
            time.sleep(pause if pause is not None else (0.8 if self.slow else 0.4))

    def type(self, text):
        self.ex("xdotool type --delay 120 '%s'" % text)
        time.sleep(0.5)

    def click(self, x, y):
        self.ex('xdotool mousemove %d %d click 1' % (x, y))
        time.sleep(0.6 if self.slow else 0.3)

    # ---- where the game's windows are ----
    # The coordinates of clicks in a game's windows are the original's (its
    # main window moved and sized by fit_window: the map window's frame at
    # y 38).  Ours has no caption of its own (the host's frame has it; but
    # with CIV2_CLASSIC=1), so its main window's client area is taller and
    # the windows centred in it are higher: found on the screen (frames.py).
    ORIG_MAP_TOP = 38

    def frames(self, col=None):
        """The windows' frames on the screen now: [(x, y)] of their top-left corners."""
        p = os.path.join(self.shots, '.frames-%s.png' % self.role).replace(ROOT, '/work')
        self.ex('xwd -root -silent -display :%d | convert xwd:- png:%s' % (self.disp, p), check=False)
        out = self.ex('python3 /work/tools/xdnet/frames.py %s %s' % (p, '' if col is None else col),
                      check=False, capture=True)
        return [tuple(int(v) for v in line.split()) for line in out.splitlines()
                if len(line.split()) == 2 and all(v.isdigit() for v in line.split())]

    def centred_dy(self):
        """How much higher (negative) a window centred in the main window sits
        than the original's: half the difference of the map windows' tops."""
        if getattr(self, 'dy', None) is None:
            tops = [y for x, y in self.frames(0) if y < 100]
            self.dy = -((self.ORIG_MAP_TOP - tops[0] + 1) // 2) if tops else 0
            log(self.role, 'windows centred in the game: %+d px from the original\'s' % self.dy)
        return self.dy

    def locate(self, x, y):
        """The window whose frame's top-left corner is at (X, Y) in the
        original: its offset (dy) here, found on the screen (else estimated)."""
        found = [fy - y for fx, fy in self.frames(x) if abs(fy - y) <= 40]
        if found:
            return min(found, key=abs)
        return self.centred_dy()

    def wclick(self, x, y, dy=None):
        """A click at the original's (X, Y) in a window centred in the game."""
        self.click(x, y + (self.centred_dy() if dy is None else dy))

    def shot(self, name=None):
        self.n += 1
        name = name or 'x'
        p = os.path.join(self.shots, '%s%02d-%s.png' % (self.role[0] + self.role[-1:].strip('abcdefghijklmnopqrstuvwxyz'), self.n, name))
        for attempt in range(3):      # (xwd can fail while windows come and go)
            if self.ex('xwd -root -silent -display :%d | convert xwd:- png:%s' % (
                    self.disp, p.replace(ROOT, '/work')), check=False) == 0:
                break
            time.sleep(0.5)
        return p

    def screen(self):
        p = os.path.join(self.shots, '.cur-%s.png' % self.role)
        self.ex('xwd -root -silent -display :%d | convert xwd:- png:%s' % (
            self.disp, p.replace(ROOT, '/work')), check=False)
        try:
            return Image.open(p).convert('RGB')
        except Exception:
            return None

    def wait_stable(self, timeout=30, still=2, tol=600):
        """Wait until the screen stops changing (fewer than TOL pixels differ
        between shots STILL seconds apart: a blinking unit is allowed)."""
        if Image is None:
            time.sleep(still + 2)
            return True
        end = time.time() + timeout
        prev = self.screen()
        while time.time() < end:
            time.sleep(still)
            cur = self.screen()
            if prev is not None and cur is not None:
                bbox = ImageChops.difference(prev, cur).convert('L').point(lambda v: 255 if v > 8 else 0)
                if sum(1 for v in bbox.getdata() if v) < tol:
                    return True
            prev = cur
        log(self.role, 'screen not stable after %ds' % timeout)
        return False

    def step(self, name, *keys, settle=True):
        self.keys(*keys)
        if settle:
            self.wait_stable()
        return self.shot(name)

    # ---- state ----
    def alive(self):
        pat = 'civ2.exe' if self.kind == 'orig' else 'OpenCiv2.exe' if self.exe == 'OpenCiv2.exe' else 'build/%s' % self.exe
        # (a dead game stays a zombie: its container's init doesn't reap it)
        return self.ex('ps -eo stat=,args= | grep -v "^Z" | grep -v grep | grep -q "%s"' % pat, check=False) == 0

    def msgs(self):
        """[(ms, dir, chan, type, size, seq, data)]"""
        out = []
        if not os.path.exists(self.xdlog):
            return out
        for m in xdmsgs.parse(self.xdlog):
            d = xdmsgs.describe(m)
            if d:
                out.append((m['t'], m['dir'], m['chan'], d[0], d[1], d[2], m['data']))
        return out

    def count(self, direction, typ, pred=None):
        return sum(1 for m in self.msgs() if m[1] == direction and m[3] == typ and m[2] == 'tcp'
                   and (pred is None or pred(m)))

    def wait_msg(self, direction, typ, n=1, timeout=300, pred=None, what=None):
        end = time.time() + timeout
        while time.time() < end:
            if self.count(direction, typ, pred) >= n:
                return True
            if not self.alive():
                raise Fail('%s: the game is gone (waiting for %s)' % (self.role, what or typ))
            time.sleep(1)
        raise Fail('%s: no %s %s #%d within %ds' % (self.role, direction, what or typ, n, timeout))

    def turns(self):
        """NM_START_WHOSE_TURN messages seen, as [(ms, civ)].  (Simultaneous
        moves: NM_HUMAN_MOVEMENT starts the humans' moves, all at once: as
        civ 0's turn then this game's.)"""
        d = 'tx' if self.role == 'host' else 'rx'
        r = []
        for m in self.msgs():
            if m[1] == d and m[3] == 105 and m[2] == 'tcp' and len(m[6]) >= 20:
                r.append((m[0], int.from_bytes(m[6][16:20], 'little', signed=True)))
            elif m[1] == d and m[3] == 142 and m[2] == 'tcp':
                r += [(m[0], 0), (m[0], self.civ)]
        return r


def docker_up(name, volume, net=None):
    sh('docker', 'rm', '-f', name, check=False)
    cmd = ['docker', 'run', '-d', '--name', name] + ([] if net else ['--hostname', name]) + ['-v', '%s:/wine' % volume,
           '-v', '%s:/work' % ROOT, '-v', '%s:/civ2:ro' % os.environ.get('CIV2_GAME', os.path.expanduser('~/Civ2')),
           '-w', '/work']
    if net:
        cmd += ['--network', 'container:%s' % net]
    sh(*cmd, 'civ2-dev', 'sleep', 'infinity')


def start(g, inet, save=None):
    env = 'DISP=%d' % g.disp
    if SIMULTANEOUS:
        env += ' SIMUL=1'
    if save:
        env += ' SAVE=%s' % save.replace(ROOT, '/work')
    if g.kind == 'orig':
        g.ex('%s sh /work/tools/xdnet/start1.sh %s %s' % (env, g.run, g.role))
    else:
        asan = 'ASAN_OPTIONS=halt_on_error=0:detect_leaks=0:quarantine_size_mb=16:malloc_context_size=8:log_path=%s/asan-%s' % (
            g.out.replace(ROOT, '/work'), g.role)
        gdb = 'GDB=1' if g.gdb else ''
        if 'cov' in g.exe:      # the coverage build: its gcov data apart (tools/xdnet/netcov.py)
            env += ' GCOV_PREFIX=/work/run/xd/cov/%s-%s GCOV_PREFIX_STRIP=4' % (g.run, g.role)
        g.ex('%s EXE=%s %s %s sh /work/tools/xdnet/start_ours.sh %s %s' % (env, g.exe, asan, gdb, g.run, g.role))
    end = time.time() + 120
    while time.time() < end:
        if g.ex('xdotool search --name "Civilization II" >/dev/null 2>&1', check=False) == 0:
            break
        time.sleep(1)
    else:
        raise Fail('%s: no game window' % g.role)
    if g.kind == 'orig':
        fit_window(g)
    # the title sequence, then the start menu
    time.sleep(8 if g.slow else 3)
    g.wait_stable(timeout=60, still=3)
    g.shot('menu')


def fit_window(g):
    """The original under Wine opens a 640x480 window (its screen, 1024x768,
    asks for the large layouts: the parley and chat windows came out half off
    it).  Moved and sized as our client area (the screen less our 19-pixel
    caption), its dialogs and windows come where ours do: one set of clicks
    for both.  (Done at once, while the main window is the only one.)"""
    out = g.ex('xdotool search --onlyvisible --name "^Civilization II Multiplayer Gold$"',
               check=False, capture=True)
    for w in out.split():
        g.ex('xdotool windowmove %s 0 19 windowsize %s 1024 749' % (w, w), check=False)


def to_network_menu(g, inet):
    g.step('multi', 'Down', 'Down', 'Down', 'Down', 'Down', 'Return')   # Multiplayer Game
    if inet:
        g.step('internet', 'Down', 'Down', 'Return')                      # Internet Game
    else:
        g.step('network', 'Down', 'Return')                               # Network Game
        g.step('lan', 'Return')                                           # TCP/IP


FEATURES = {'open': 508, 'kill': 540, 'positions': 572, 'chat': 604, 'production': 636, 'movement': 668,
            'simultaneous': 700}   # x 79 (simultaneous: when CIV.INI allows it, --simultaneous)
SIMULTANEOUS = []
AFTER_LATE = None                   # --after-late


def host_setup(g, inet, name='Alice', game='TestGame', chat=False, rejoin=False, features=(), timer=0):
    to_network_menu(g, inet)
    g.step('new', 'Return')                     # Start a New Multiplayer Game
    g.step('world', 'Up', 'Return')             # Small world
    g.step('difficulty', 'Return')
    g.step('civs', 'Down', 'Down', 'Down', 'Down', 'Return')   # 3 civilizations
    g.step('aggression', 'Return')
    g.step('rules', 'Return')                   # standard rules
    # Select Multiplayer Features: the boxes ticked
    ticks = list(features) + (['open'] if rejoin else []) + (['chat'] if chat else [])
    time.sleep(1)
    for f in ticks:
        g.click(79, FEATURES[f])
        time.sleep(1)
    g.step('features', 'Return')
    g.step('timer', *(['Down'] * timer + ['Return']))   # turn timer: unlimited, 30 s, 1 min ...
    g.type(name)
    g.step('netname', 'Return')
    g.type(game)
    if inet:
        g.step('gamename', 'Return')            # -> Please Verify Your IP Address
    g.keys('Return')
    g.wait_stable(timeout=40)
    g.shot('lobby')                             # Available Players


def client_join(g, host, inet, name='Bob', n=1, again=False):
    """AGAIN: from the network menu, Join already picked (a player that left the lobby)."""
    if not again:
        to_network_menu(g, inet)
    if inet:
        g.step('join', 'Down', 'Down', 'Down', 'Down', 'Down', 'Return')
        g.type(name)
        g.step('netname', 'Return')
        g.ex('xdotool key ctrl+a', check=False)
        g.type('127.0.0.1')
        g.step('address', 'Return')
    else:
        g.step('join', *(['Return'] if again else ['Down', 'Down', 'Down', 'Down', 'Down', 'Return']))  # Join a Multiplayer Game
        seen = sum(1 for m in g.msgs() if m[1] == 'rx' and m[2] == 'udp' and m[4] == 408)
        g.type(name)
        g.keys('Return')
        # the game list fills from the host's NM_SERVER_INFO broadcasts
        end = time.time() + 60
        while time.time() < end and sum(
                1 for m in g.msgs() if m[1] == 'rx' and m[2] == 'udp' and m[4] == 408) <= seen:
            time.sleep(1)
        g.wait_stable()
        g.shot('gamelist')
        g.keys('Return')                        # Join Game
    g.wait_msg('tx', nm('NM_PLAYER_LIST_NODE'), n, timeout=90, what='PLAYER_LIST_NODE')
    g.wait_stable()
    g.shot('joined')


def choose_civ(g):
    """gender, tribe, leader name, city style: the defaults (a client may
    not be asked all four: it is done when it has sent its NM_PLAYER_INFO)"""
    for what in ('gender', 'tribe', 'leader', 'style'):
        if g.role.startswith('client') and g.count('tx', nm('NM_PLAYER_INFO')):
            break
        g.wait_stable(timeout=60)
        g.step('after-' + what, 'Return')


MAIN_WINDOWS = re.compile(r'^(Civilization II.*|World|Status|.* Map \(.*\)|Default IME|.*Wine.*|Hot Join|New Player Receiving Game State|Local Area Network \(TCP/IP\)|Internet \(TCP/IP\)|Negotiating with .*|Chat with Kings.*|)$')     # (waits: Hot Join, the host's while it sends a joining player the game, which closes by itself; Return = Cancel)


EXTRA = []      # a third player (--third): Games besides the host and the client


def popups(g, raw=False):
    """The titles of the game's dialogs showing now (not its main windows).
    The original under Wine: its top-level windows are X windows.  Ours:
    from the platform log (CreateWindowEx then the first child's parent
    gives the handle; ShowWindow shows and hides it)."""
    if g.kind == 'orig':
        out = g.ex('for w in $(xdotool search --onlyvisible --name . 2>/dev/null); do '
                   'xdotool getwindowname $w; done', check=False, capture=True)
        titles = out.splitlines()
        if raw:     # (the main window and the start menu have the same title)
            main = 'Civilization II Multiplayer Gold'
            return [t for t in titles if t != main] + [main] * (titles.count(main) - 1)
        return [t for t in titles if not MAIN_WINDOWS.match(t)]
    p = os.path.join(g.out, 'ours-%s.log' % g.role)
    try:
        with open(p, errors='replace') as f:
            f.seek(g.logpos)
            data = f.read()
    except OSError:
        return []
    cut = data.rfind('\n') + 1
    g.logpos += len(data[:cut].encode('utf-8', 'replace'))
    created = False
    for line in data[:cut].splitlines():
        m = re.match(r'CreateWindowEx\(\w+, (\w+), "(.*)", (\w+), .*parent (\w+)\)$', line)
        if m:
            if g.pending is not None and m.group(4) not in g.wins:
                g.wins[m.group(4)] = g.pending
            g.pending = m.group(2) if m.group(1) == 'MSWindowClass' and m.group(3).startswith('8') else None
            created = g.pending is None
            continue
        m = re.match(r'ShowWindow\((\w+), (\d+)\)$', line)
        if m and created and m.group(1) in g.wins:
            # a window just created (not a dialog) shown: a dialog's handle
            # reused (DestroyWindow is not logged), no longer that dialog
            del g.wins[m.group(1)]
            g.shown.discard(m.group(1))
        created = False
        if m and m.group(1) in g.wins:
            if m.group(2) == '0':
                g.shown.discard(m.group(1))
            else:
                g.shown.add(m.group(1))
        m = re.match(r'DestroyWindow\((\w+)\)', line)
        if m:
            g.shown.discard(m.group(1))
    return [g.wins[h] for h in sorted(g.shown) if raw or not MAIN_WINDOWS.match(g.wins[h])]


def dismiss(g, titles):
    t = titles[-1]
    g.popped, g.last_pop = True, time.time()
    g.shot('popup')
    log(g.role, 'popup:', ', '.join(titles))
    g.keys('Escape' if t.startswith('City of') else 'Return')


def turn_actions(g, turn):
    """What we do in our turn TURN (no popup showing), then end it."""
    if turn == 1:
        g.step('move', 'KP_6')                  # the settler one square east
    elif turn == 2:
        g.step('build', 'b')                    # Build New City -> name it
        g.step('name', 'Return')                # -> Found New City, tutorial, city window
        end = time.time() + 60
        while time.time() < end:
            pops = popups(g)
            if pops:
                dismiss(g, pops)
            elif g.wait_stable(timeout=10, still=3) and not popups(g):
                break
        g.step('citywin', 'Escape')             # the city window (ours: not in popups())
    else:
        g.keys('s')                             # sentry a unit if one is waiting
    g.keys('ctrl+n')                            # End Player Turn
    if turn > 2:                                # ('End of Turn (Press ENTER)' once the units have moved)
        time.sleep(3)
        if not popups(g):
            g.keys('Return')


def my_turn_over(g):
    t = g.turns()
    mine = [i for i, x in enumerate(t) if x[1] == g.civ]
    return bool(mine) and any(x[1] != g.civ for x in t[mine[-1] + 1:])


def play(host, client, nturns, opts, cont=False):
    """NTURNS turns each (CONT: N more, counting from the turns in the logs)."""
    games = {'host': host, 'client': client}
    games.update((g.role, g) for g in EXTRA)
    # whose civ: a client's own PLAYER_INFO names it (args[0]); the host is
    # the other human
    for g in games.values():
        if g is host:
            continue
        for m in g.msgs():
            if m[1] == 'tx' and m[3] == nm('NM_PLAYER_INFO') and len(m[6]) >= 20:
                g.civ = int.from_bytes(m[6][16:20], 'little')
    if client.civ is None:      # (a loaded game: the second civilization)
        client.civ = 2
    host.civ = min({1, 2, 3, 4} - {g.civ for g in games.values() if g is not host})
    log('civs:', ', '.join('%s %s' % (r, g.civ) for r, g in games.items()))
    acted = {r: 0 for r in games}
    if cont:
        acted = {r: sum(1 for x in g.turns() if x[1] == g.civ) for r, g in games.items()}
    target = {r: acted[r] + nturns for r in acted}
    since = {r: 0 for r in games}
    end = time.time() + 300 + 300 * nturns
    while True:
        for g in games.values():
            if not g.alive():
                raise Fail('%s: the game is gone (turns %s)' % (g.role, acted))
        if all(acted[r] >= target[r] for r in acted) and all(
                my_turn_over(g) or sum(1 for x in g.turns() if x[1] == g.civ) > acted[r] for r, g in games.items()):
            break
        if time.time() > end:
            raise Fail('turns: not done in time (%s)' % acted)
        for role, g in games.items():
            pops = popups(g)
            if pops:
                dismiss(g, pops)
                time.sleep(1.5 if g.slow else 0.8)
                continue
            mine = sum(1 for x in g.turns() if x[1] == g.civ)
            if mine > acted[role] and acted[role] < target[role]:
                # our turn: once its 'It is our turn to move' popup has gone
                # (the turn's set-up done) and the screen has settled, act
                if g.turn_seen != mine:
                    g.turn_seen, g.turn_t = mine, time.time()
                    g.popped = time.time() - g.last_pop < 20     # (its popup may come first)
                if not g.popped and time.time() - g.turn_t < 90:
                    continue
                if not g.wait_stable(timeout=20) or popups(g):
                    continue
                acted[role] += 1
                since[role] = time.time()
                log(role, 'turn', acted[role])
                g.shot('turn%d' % acted[role])
                turn_actions(g, acted[role])
                if acted[role] == 2 and role == 'host' and opts.save:
                    save(g)
            elif mine == acted[role] and mine and not my_turn_over(g) and time.time() - since[role] > 25:
                # still our turn: the end-of-turn key was lost (a popup took it)
                log(role, 'turn %d: still ours, ending it again' % acted[role])
                g.shot('stillturn')
                since[role] = time.time()
                g.keys('Escape', 'Return', 'ctrl+n')   # a city window; 'End of Turn (Press ENTER)'
                g.stuck = getattr(g, 'stuck', 0) + 1
                if g.stuck % 2 == 0:            # (the throne room, a reward: 'Click mouse to continue')
                    for i in range(3):
                        g.click(512, 450)
                        time.sleep(3)
        time.sleep(1)
    log('turns done', acted)


def rejoin(host, client, opts):
    """The client leaves the game (Quit), starts again and joins the game
    in progress (an open game), takes a civilization; one more turn each."""
    quit_one(client)
    host.wait_msg('rx', nm('NM_QUIT_REQUEST'), timeout=60, what='QUIT_REQUEST')
    settle(host, 30)
    for f in ('xdlog-client.txt', 'ours-client.log', 'wine-client.log'):
        p = os.path.join(client.out, f)
        if os.path.exists(p):
            os.rename(p, p[:-4] + '-left' + p[-4:])
    client.logpos, client.wins, client.shown, client.pending = 0, {}, set(), None
    client.turn_seen, client.turn_t, client.popped, client.last_pop = 0, 0, False, 0
    log('client: joining again')
    start(client, opts.inet)
    client_join(client, host, opts.inet)
    end = time.time() + 300
    while time.time() < end and not client.count('tx', nm('NM_PLAYER_INFO')):
        for g in (client, host):
            if g.wait_stable(timeout=8, still=2):
                pops = popups(g)
                if pops and not any('Waiting' in t or 'TCP' in t for t in pops):
                    g.shot('rejoin')
                    log(g.role, 'rejoin:', ', '.join(pops))
                    g.keys('Return')
        time.sleep(1)
    if not client.count('tx', nm('NM_PLAYER_INFO')):
        raise Fail('client: not back in the game')
    client.civ = None
    log('client: back in the game')
    if opts.stop_after == 'rejoin':
        raise Fail('stopped after rejoin')
    play(host, client, 1, opts, cont=True)


def wait_popup(g, pred, timeout=30):
    end = time.time() + timeout
    while time.time() < end:
        pops = popups(g)
        if any(pred(t) for t in pops):
            return pops
        time.sleep(1)
    g.shot('nopopup')
    raise Fail('%s: no dialog %s' % (g.role, getattr(pred, '__doc__', '') or ''))


# The parley window (its large layout: 1024x768 screens; the original's
# window is fitted to ours, fit_window, so the same places serve both)
PARLEY_ACTION = {'treaty': 225, 'proposal': 260, 'gift': 295, 'barter': 330, 'threat': 365}   # x 480 (the label)
PARLEY_TREATY = {'ceasefire': 245, 'peace': 275, 'alliance': 305, 'cancel': 335}              # x 130
PARLEY_GIFT = {'knowledge': 230, 'money': 260, 'unit': 290, 'territory': 320, 'maps': 350}      # x 130
PARLEY_PROPOSAL = {'war': 276, 'maps': 306}                                                     # x 130
PARLEY_BARTER = {'money': 212, 'knowledge': 238, 'unit': 264, 'territory': 290, 'maps': 316,
                 'war': 342, 'treaty': 368}                                     # x 130 (offer), 530 (in exchange)
PARLEY_BUTTON = {'left': 246, 'mid': 510, 'right': 776}                         # y 400: Check Intelligence /
#   Continue, Transmit Offer / Never Mind; when an offer came: Accept Offer / No Thanks / Make Counter Offer
PARLEY_REPLY = {'accept': ('left', 'NM_PARLEY_ACCEPT_OFFER'), 'nothanks': ('mid', 'NM_PARLEY_NO_THANKS'),
                'counter': ('right', 'NM_PARLEY_COUNTER_OFFER')}
PARLEY_REVEAL = (512, 368)          # Reveal Unit/City Information (waiting for an offer)
PARLEY_END = (511, 661)             # End Negotiations


def dialogs(g):
    """The dialogs showing, but those waiting for the other king (Return
    there is Cancel: they close by themselves)."""
    return [t for t in popups(g) if not t.startswith('Awaiting')]


def answer_dialogs(g, secs=6):
    """Return to each dialog that comes within SECS (the responses to an
    offer: 'Opponent's Response...', and the like)."""
    end = time.time() + secs
    n = 0
    while time.time() < end:
        pops = dialogs(g)
        if pops:
            g.shot('parley-popup')
            log(g.role, 'parley popup:', ', '.join(pops))
            g.keys('Return')
            n += 1
            time.sleep(1.5)
            end = max(end, time.time() + 2)
        else:
            time.sleep(1)
    return n


def request_audience(req, rec, answer=1):
    """REQ: Foreign Minister, Send Emissary to the other king.  REC's
    'We request an audience' gets ANSWER (1 Granted, 0 Go away, None: no
    answer, it closes by itself after 20 s).  Returns REC's reply (from the
    wire: NM_PARLEYREQUEST_REPLY's first argument)."""
    n = rec.count('tx', nm('NM_PARLEYREQUEST_REPLY'))
    req.keys('F3')
    wait_popup(req, lambda t: t == 'Foreign Minister')
    time.sleep(1.5)
    req.shot('foreign')
    req.wclick(404, 511)                        # Send Emissary
    if answer != 'auto':                        # ('auto': REC answers by itself, busy)
        wait_popup(rec, lambda t: '(' in t)     # 'Dictator Caesar of the Romans (Icy)'
        time.sleep(1.5)
        rec.shot('audience')
    if answer == 'late':                        # granted after the requester has given up (15 s)
        time.sleep(16)
        answer = 1
    if answer in (0, 1):
        rec.keys(*(['Down'] * answer + ['Return']))
    rec.wait_msg('tx', nm('NM_PARLEYREQUEST_REPLY'), n + 1, timeout=40, what='PARLEYREQUEST_REPLY')
    m = [x for x in rec.msgs() if x[1] == 'tx' and x[3] == nm('NM_PARLEYREQUEST_REPLY') and x[2] == 'tcp'][-1]
    reply = int.from_bytes(m[6][16:20], 'little', signed=True)
    log(rec.role, 'audience reply', reply)
    return reply


def parley_click(g, x, y, pause=1.5):
    g.wclick(x, y)
    time.sleep(pause)


def offer(g, other, action, *picks, reply='accept', amount=None, dblclick=False):
    """G (in the parley, picking an action) makes an offer: ACTION, then
    PICKS (radio buttons: treaty / gift / proposal kind; barter: what we
    offer, what we want), AMOUNT typed in a gold box, Transmit; OTHER gives
    REPLY.  Checked on the wire: the offer and the reply both ways."""
    sent = g.count('tx', nm('NM_PARLEY_TRANSACTION'))
    got = other.count('rx', nm('NM_PARLEY_TRANSACTION'))
    log(g.role, 'offers', action, ' '.join(picks), amount or '', '->', reply)
    # (a side whose turn to offer comes back starts in the action it chose
    # last: Never Mind back to 'Pick an Action', twice from a barter)
    parley_click(g, PARLEY_BUTTON['right'], 400)
    parley_click(g, PARLEY_BUTTON['right'], 400)
    if dblclick:                                            # (a double-click on the action: Continue)
        g.ex('xdotool mousemove 480 %d click --repeat 2 --delay 80 1' % (PARLEY_ACTION[action] + g.centred_dy()))
        time.sleep(1.5)
    else:
        parley_click(g, 480, PARLEY_ACTION[action])
        parley_click(g, PARLEY_BUTTON['mid'], 400)          # Continue
    if action == 'treaty':
        parley_click(g, 130, PARLEY_TREATY[picks[0]])
    elif action == 'gift':
        parley_click(g, 130, PARLEY_GIFT[picks[0]])
    elif action == 'proposal':
        parley_click(g, 130, PARLEY_PROPOSAL[picks[0]])
    elif action == 'barter':
        parley_click(g, 130, PARLEY_BARTER[picks[0]])
        parley_click(g, 530, PARLEY_BARTER[picks[1]])
        parley_click(g, PARLEY_BUTTON['mid'], 400)          # Continue
        if picks[1] == 'maps':                              # (another map, then the whole map again)
            parley_click(g, 537, 305)
            parley_click(g, 537, 279)
    elif action == 'threat':                                # (demand maps, then money again)
        parley_click(g, 130, 334)
        parley_click(g, 130, 230)
    if amount is not None:                                  # (the gold box has the focus, its 0 selected)
        g.type(str(amount))
    time.sleep(1)
    g.shot('offer-' + action)
    parley_click(g, PARLEY_BUTTON['mid'], 400)              # Transmit Offer
    g.wait_msg('tx', nm('NM_PARLEY_TRANSACTION'), sent + 1, timeout=30, what='PARLEY_TRANSACTION')
    other.wait_msg('rx', nm('NM_PARLEY_TRANSACTION'), got + 1, timeout=30, what='PARLEY_TRANSACTION')
    answer_dialogs(other, 4)
    time.sleep(2)
    other.shot('offered-' + action)
    if action == 'treaty' and picks[0] == 'cancel':         # (no reply: in force at once)
        answer_dialogs(other)
        answer_dialogs(g, 3)
        return
    button, msg = PARLEY_REPLY[reply]
    n = other.count('tx', nm(msg))
    parley_click(other, PARLEY_BUTTON[button], 400)
    other.wait_msg('tx', nm(msg), n + 1, timeout=30, what=msg)
    g.wait_msg('rx', nm(msg), timeout=30, what=msg)
    answer_dialogs(g)
    answer_dialogs(other)
    g.shot('replied')
    other.shot('replied')


def intel(g):
    """Check Intelligence (no embassy: 'no intelligence' popup)."""
    parley_click(g, PARLEY_BUTTON['left'], 400)
    answer_dialogs(g, 4)


def window_moves(g, other):
    """A key typed to the window; its caption's middle button (the small
    layout); Escape there ends the negotiations (NM_PARLEYREQUEST_CANCEL)."""
    g.wclick(511, 140)
    g.keys('a')
    time.sleep(1)
    n = g.count('tx', nm('NM_PARLEYREQUEST_CANCEL'))
    parley_click(g, 153, 146)                   # (the small layout)
    time.sleep(2)
    g.shot('parley-small')
    g.keys('Escape')
    g.wait_msg('tx', nm('NM_PARLEYREQUEST_CANCEL'), n + 1, timeout=30, what='PARLEYREQUEST_CANCEL')
    answer_dialogs(other)
    answer_dialogs(g, 3)
    other.shot('ended')


def end_parley(g, other):
    """G ends the negotiations (NM_PARLEYREQUEST_CANCEL); the other's
    window closes ('...have broken off negotiations')."""
    n = g.count('tx', nm('NM_PARLEYREQUEST_CANCEL'))
    parley_click(g, *PARLEY_END)
    g.wait_msg('tx', nm('NM_PARLEYREQUEST_CANCEL'), n + 1, timeout=30, what='PARLEYREQUEST_CANCEL')
    answer_dialogs(other)
    answer_dialogs(g, 3)
    other.shot('ended')


def diplomacy(host, client):
    """Both kings negotiate (the parley window), each making offers in turn:
    gifts of gold and maps, treaties (cease-fire, peace, alliance, then
    cancelled), a proposal to share maps, maps bartered for maps, a threat
    (gold or else); declined (No Thanks), countered and accepted.  Before
    that an audience is left unanswered: it times out on both sides
    (PARLEYGOAWAY; the request closes by itself, 'Go away'), and the next is
    granted.  After it, an audience granted too late (late_grant).
    Knowledge, units, cities and war declarations need a game long enough
    for advances, units and other contacts: not in this test."""
    for g in (host, client):
        settle(g, 40)
    # an audience nobody answers: the requester gives up (15 s), the
    # request closes by itself (20 s) and answers 'Go away'
    reply = request_audience(host, client, answer=None)
    if reply != 0:
        raise Fail('client: audience left unanswered: reply %d (want 0, Go away)' % reply)
    answer_dialogs(host, 10)
    answer_dialogs(client, 5)
    for g in (host, client):
        settle(g, 20)
    reply = request_audience(host, client)
    if reply != 1:
        raise Fail('client: audience granted: reply %d (want 1)' % reply)
    wait_popup(host, lambda t: t.startswith('Opponent'))
    host.shot('granted')
    host.keys('Return')
    time.sleep(3)
    run_offers(host, client)          # (the host ends the negotiations)
    for g in (host, client):
        settle(g, 20)
    late_grant(host, client)
    if AFTER_LATE:
        audience_after_late(host, client)
    log('diplomacy done')


def audience_after_late(host, client):
    """After late_grant: one more audience granted.  The requester (the
    host) still counts the cancel the late window sent; the original (and
    ours with CIV2_CLASSIC=1) ends this audience at once ('...have broken
    off negotiations', no parley window, nothing sent); ours, bug B6 fixed
    (docs/BUGS.md), opens the parley window, whose End Negotiations sends a
    cancel."""
    reply = request_audience(host, client)
    if reply != 1:
        raise Fail('client: audience after the late grant: reply %d (want 1)' % reply)
    wait_popup(host, lambda t: t.startswith('Opponent'))
    host.shot('after-late-granted')
    host.keys('Return')
    time.sleep(3)
    host.shot('after-late-host')
    client.shot('after-late-client')
    n = host.count('tx', nm('NM_PARLEYREQUEST_CANCEL'))
    parley_click(host, 368, 519)                # End Negotiations (the window has its small layout here)
    try:
        host.wait_msg('tx', nm('NM_PARLEYREQUEST_CANCEL'), n + 1, timeout=30, what='PARLEYREQUEST_CANCEL')
        opened = True
    except Fail:
        opened = False
    log('audience after the late grant: the requester\'s parley window %s' % ('opened' if opened else 'did not open'))
    host.shot('after-late-end')
    answer_dialogs(host, 5)
    answer_dialogs(client, 3)
    if not opened:
        # (the client's window, opened alone, is closed by hand)
        parley_click(client, *PARLEY_END)
        answer_dialogs(client, 3)
        answer_dialogs(host, 3)
    for g in (host, client):
        settle(g, 20)
    if opened != (AFTER_LATE == 'fixed'):
        raise Fail('host: audience after the late grant: parley window %s, want %s (B6)' %
                   ('opened' if opened else 'not opened', 'opened' if AFTER_LATE == 'fixed' else 'not opened'))


def late_grant(host, client):
    """An audience granted after the requester has given up (15 s): the
    other's parley window opens alone, waiting for an offer; a new request
    meanwhile is answered 'busy' (2) at once; End Negotiations there closes
    it.  (Its cancel stays counted on the requester, the original's code and
    ours: the next audience granted would end at once, '...have broken off
    negotiations'.  So this comes last.)"""
    reply = request_audience(host, client, answer='late')
    answer_dialogs(host, 8)
    time.sleep(3)
    host.shot('late-host')
    client.shot('late-client')
    busy = request_audience(host, client, answer='auto')
    answer_dialogs(host, 8)
    log('late grant: reply %d, then %d' % (reply, busy))
    if reply != 1 or busy != 2:
        raise Fail('client: audience granted late: replies %d, %d (want 1, then 2: busy)' % (reply, busy))
    n = client.count('tx', nm('NM_PARLEYREQUEST_CANCEL'))
    parley_click(client, *PARLEY_END)
    client.wait_msg('tx', nm('NM_PARLEYREQUEST_CANCEL'), n + 1, timeout=30, what='PARLEYREQUEST_CANCEL')
    answer_dialogs(host, 5)
    answer_dialogs(client, 3)
    for g in (host, client):
        settle(g, 20)
    host.shot('late-after')
    client.shot('late-after')


def run_offers(h, c, skip=0):
    """The offers, H the side picking first."""
    steps = [lambda: intel(h),
             lambda: offer(h, c, 'gift', 'money', amount=10, reply='nothanks'),
             lambda: offer(h, c, 'gift', 'money', amount=10, reply='accept'),
             lambda: parley_click(h, *PARLEY_REVEAL),       # (the side waiting for an offer: reveal,
             lambda: parley_click(h, *PARLEY_REVEAL),       #  then conceal)
             lambda: offer(c, h, 'treaty', 'ceasefire', reply='counter'),
             lambda: offer(h, c, 'treaty', 'peace', reply='accept', dblclick=True),
             lambda: offer(c, h, 'barter', 'maps', 'maps', reply='accept'),
             lambda: offer(h, c, 'proposal', 'maps', reply='accept'),
             lambda: offer(c, h, 'threat', amount=5, reply='accept'),
             lambda: offer(h, c, 'treaty', 'alliance', reply='accept'),
             lambda: offer(c, h, 'gift', 'maps', reply='accept'),
             lambda: offer(h, c, 'treaty', 'cancel'),
             lambda: window_moves(h, c)]
    for step in steps[skip:]:
        step()


CHAT_WINDOW = (99, 123)             # the original's Chat with Kings: its frame's top-left corner


def chat(host, client):
    """Chat with Kings both ways: the client writes to the host, the host's
    chat window opens and it answers."""
    for g in (client, host):
        settle(g, 40)
    for g, other, text in ((client, host, 'hello from Bob'), (host, client, 'hi Bob, Alice here')):
        n = g.count('tx', nm('NM_CHAT'))
        r = other.count('rx', nm('NM_CHAT'))
        if g is client or not other.count('tx', nm('NM_CHAT')):
            if g.kind == 'ours':
                g.keys('Escape')                # (a city window left open from the turns takes Ctrl+C)
            g.keys('ctrl+c')                    # (the host's opens by itself on a message)
        time.sleep(3)
        dy = g.locate(*CHAT_WINDOW)
        g.wclick(188, 251, dy)                  # [x] Send (to the one other king)
        g.wclick(400, 350, dy)
        g.type(text)
        g.wclick(870, 333, dy)                  # Send
        g.wait_msg('tx', nm('NM_CHAT'), n + 1, timeout=30, what='CHAT')
        other.wait_msg('rx', nm('NM_CHAT'), r + 1, timeout=30, what='CHAT')
        time.sleep(3)
        g.shot('chat')
        other.shot('chat')
    # the client: the macros (Ctrl+F1 an adjective, Ctrl+F2 a leader, Ctrl+F3
    # a line of CHATMAC1.TXT) into its input, Clear; the checkboxes (mute
    # the other king, Remain Anonymous, Mute Anonymous) on and off; the
    # window dragged by its caption and back
    g = client
    dy = g.locate(*CHAT_WINDOW)
    g.wclick(400, 350, dy)
    for k in ('ctrl+F1', 'ctrl+F2', 'ctrl+F3'):
        g.keys(k)
    g.shot('chat-macros')
    g.wclick(870, 372, dy)                      # Clear
    for x, y in ((154, 251), (130, 185), (396, 185)):
        g.wclick(x, y, dy)
        time.sleep(1)
        g.wclick(x, y, dy)
        time.sleep(1)
    for (x0, y0), (x1, y1) in (((300, 146), (340, 176)), ((340, 176), (300, 146))):
        g.ex('xdotool mousemove %d %d mousedown 1 mousemove %d %d mousemove %d %d mouseup 1' % (
            x0, y0 + dy, (x0 + x1) // 2, (y0 + y1) // 2 + dy, x1, y1 + dy))
        time.sleep(2)
    g.shot('chat-dragged')
    for g in (client, host):
        g.wclick(511, 661, g.locate(*CHAT_WINDOW))      # End Chat
        time.sleep(2)
    log('chat done')


def save(g, name='saved.net'):
    """Game menu: Save Game, the name it proposes; the file is copied to
    the run's directory as NAME (saved.net: for --load)."""
    settle(g, 30)
    g.step('save', 'ctrl+s')
    g.step('saved', 'Return')
    settle(g, 20)
    d = '/tmp/g' if g.kind == 'orig' else '/tmp/ours%d' % g.disp
    g.ex('cp "$(ls -t %s/*.net | head -1)" %s/%s' % (d, g.out.replace(ROOT, '/work'), name))
    if not os.path.exists(os.path.join(g.out, name)):
        raise Fail('%s: no saved game' % g.role)
    log(g.role, 'saved the game as', name)


def compare_saves(host, client, result):
    """Both players save the game where it stands; the two saves must hold
    the same game (tools/xdnet/savecmp.py): no desync."""
    for g in (host, client):
        save(g, 'end-%s.net' % g.role)
    r = subprocess.run([sys.executable, os.path.join(ROOT, 'tools/xdnet/savecmp.py'),
                        os.path.join(host.out, 'end-host.net'), os.path.join(client.out, 'end-client.net')],
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    text = r.stdout.decode(errors='replace')
    open(os.path.join(host.out, 'savecmp.txt'), 'w').write(text)
    log('saves compared:\n' + text)
    result.append('%s both saves hold the same game (savecmp.txt)' % ('PASS' if r.returncode == 0 else 'FAIL'))


def host_load(g, inet, name='Alice', game='LoadedGame'):
    """Load a Saved Multiplayer Game (saved.net), then as a new game."""
    to_network_menu(g, inet)
    g.step('load', 'Down', 'Down', 'Down', 'Down', 'Return')
    time.sleep(2)
    g.type('saved.net')
    g.step('loadfile', 'Return')
    g.type(name)
    g.step('netname', 'Return')
    g.type(game)
    if inet:
        g.step('gamename', 'Return')
    g.keys('Return')
    g.wait_stable(timeout=40)
    g.shot('lobby')


def start_loaded(host, client):
    """After the join: civilizations from the saved game (the defaults),
    names; until the host starts the first turn."""
    end = time.time() + 300
    while time.time() < end and host.count('tx', nm('NM_START_WHOSE_TURN')) == 0:
        for g in (host, client):
            if g.wait_stable(timeout=8, still=2):
                pops = popups(g)
                if pops and not any('Waiting' in t or 'TCP' in t for t in pops):
                    g.shot('setup')
                    log(g.role, 'set-up:', ', '.join(pops))
                    g.keys('Return')
        time.sleep(1)
    if host.count('tx', nm('NM_START_WHOSE_TURN')) == 0:
        raise Fail('the loaded game did not start')


def settle(g, secs=60):
    """Dismiss the dialogs showing until none comes for a moment."""
    end = time.time() + secs
    while time.time() < end:
        pops = popups(g)
        if pops:
            dismiss(g, pops)
            time.sleep(1)
        elif g.wait_stable(timeout=10, still=2) and not popups(g):
            return


def quit_one(g):
    """Game menu: Quit, 'Do you really want to quit?' Yes; then whatever
    else it asks, until the game has gone."""
    settle(g, 30)
    g.keys('Escape')                            # a city window
    for attempt in range(4):                    # (a key can be lost while the game is busy)
        settle(g, 20)
        g.keys('ctrl+q')
        end = time.time() + 15
        while time.time() < end and not popups(g):
            time.sleep(1)
        if 'Confirmation' in popups(g):
            break
        pops = popups(g)
        if pops:                                # (another dialog came first)
            dismiss(g, pops)
        else:
            g.keys('Escape')
    g.shot('quit')
    g.keys('Down', 'Return')                    # Yes
    # a network game ends at the start menu: Cancel there leaves the game
    end = time.time() + 90
    while time.time() < end and g.alive():
        pops = popups(g)
        if pops:                                # (Escape: from the set-up menus back out)
            g.shot('popup')
            log(g.role, 'quitting:', ', '.join(pops))
            g.keys('Escape')
            time.sleep(1)
        elif 'Civilization II Multiplayer Gold' in popups(g, raw=True):
            g.wait_stable()
            g.shot('menu-again')
            g.keys('Escape')
            time.sleep(3)
        time.sleep(2)
    if g.alive():
        g.shot('still-running')
        raise Fail('%s: still running after Quit' % g.role)
    log(g.role, 'quit')


def extras(host, client):
    """What a player may do besides moving, during a network game: the
    advisors and reports (F1..F12, the client while it is not its turn
    perhaps), Multiplayer Options (Ctrl+Y), the host changes the turn timer
    (Ctrl+T: 2 minutes)."""
    for g in (client, host):
        settle(g, 30)
    for key in ('F1', 'F2', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F11', 'F12'):
        client.keys(key)
        time.sleep(3)
        client.shot('report-' + key)
        client.keys('Escape')
        time.sleep(1)
        settle(client, 10)
    for g in (client, host):
        g.keys('ctrl+y')                        # Multiplayer Options
        time.sleep(3)
        g.shot('mpoptions')
        g.keys('Return')
        settle(g, 10)
    host.keys('ctrl+t')                         # Change Timer: 2 minutes
    time.sleep(3)
    host.shot('timer')
    host.keys('Down', 'Down', 'Down', 'Return')
    settle(host, 10)
    settle(client, 10)
    log('extras done')


def migrate(host, client, opts):
    """The host leaves first: the client becomes the new host (host
    migration, docs/xdaemon-protocol.md 10.1: NM_SERVER_QUIT_REQUEST/_ACK,
    NM_SERVER_QUIT, then it initialises its sockets as a server), plays on
    alone one turn, then quits."""
    quit_one(host)
    end = time.time() + 120
    while time.time() < end:
        if any('server=1' in l for l in open(client.xdlog, errors='replace') if 'InitializeSocketsTCP' in l):
            break
        if not client.alive():
            raise Fail('client: gone after the host left')
        settle(client, 5)
        time.sleep(1)
    else:
        raise Fail('client: did not become the host')
    log('client: the new host')
    for i in range(3):                          # ('You are the new hosting player' and the turn's popups)
        settle(client, 20)
        client.keys('ctrl+n')
        time.sleep(5)
    settle(client, 30)
    client.shot('new-host')
    if not client.alive():
        raise Fail('client: gone after becoming the host')
    quit_one(client)


def lobby_empty_reject(host):
    """--lobby: Reject Player while Available Players lists no one (only its
    "waiting" line): nothing happens (find_waiting_entry)."""
    host.click(363, 496)                        # Reject Player
    time.sleep(2)
    host.wait_stable()
    host.shot('reject-nobody')


def lobby_leave(host, g, inet):
    """--lobby: G, waiting in the host's lobby, cancels (NM_CLIENT_CANCELED_START:
    the host takes it off Available Players), then joins again."""
    host.shot('lobby-full')
    for attempt in range(6):                    # (a key can be lost while the game is busy)
        if attempt % 2:
            g.keys('Return')                    # Waiting for the game machine: Cancel
        else:
            g.click(515, 438)                   # (its Cancel button)
        end = time.time() + 10
        while time.time() < end and not host.count('rx', nm('NM_CLIENT_CANCELED_START')):
            time.sleep(1)
        if host.count('rx', nm('NM_CLIENT_CANCELED_START')):
            break
    host.wait_msg('rx', nm('NM_CLIENT_CANCELED_START'), timeout=30, what='CLIENT_CANCELED_START')
    g.wait_stable()
    g.shot('left-lobby')                        # back in the network menu, Join picked
    host.wait_stable()
    host.shot('lobby-left')
    log(g.role, 'left the lobby')
    if g.kind == 'orig':
        # (the original, its join cancelled, lists no games when it tries
        # again: its game list never asks for them, no XD_SendBroadcastData;
        # so it leaves and starts again)
        end = time.time() + 90
        while time.time() < end and g.alive():
            g.keys('Escape')
            time.sleep(2)
        if g.alive():
            raise Fail('%s: still running after leaving the lobby' % g.role)
        for f in ('xdlog-%s.txt' % g.role, 'wine-%s.log' % g.role, 'hook-%s.log' % g.role):
            p = os.path.join(g.out, f)
            if os.path.exists(p):
                os.rename(p, p[:-4] + '-left' + p[-4:])
        g.logpos, g.wins, g.shown, g.pending = 0, {}, set(), None
        start(g, inet)
        client_join(g, host, inet, name='Carol')
    else:
        client_join(g, host, inet, name='Carol', n=2, again=True)
    host.wait_msg('rx', nm('NM_PLAYER_LIST_NODE'), 3, timeout=60, what='PLAYER_LIST_NODE')
    host.wait_stable()
    log(g.role, 'joined again')


def kick(host, g):
    """In Available Players the host picks the last player who joined (G)
    and rejects it (XD_ServerCloseConnection): G loses its connection; it
    is then made to leave."""
    host.shot('kick-before')
    host.click(510, 334)                        # the second player listed
    time.sleep(1)
    host.click(363, 496)                        # Reject Player
    time.sleep(3)
    host.shot('kicked')
    end = time.time() + 60
    while time.time() < end and not any('ConnectionLost' in l for l in open(g.xdlog, errors='replace')):
        time.sleep(1)
    else:
        if time.time() >= end:
            raise Fail('%s: not disconnected by Reject Player' % g.role)
    log(g.role, 'rejected by the host')
    time.sleep(3)
    g.shot('rejected')
    end = time.time() + 90                      # (its dialogs, back out to the start menu, and out)
    while time.time() < end and g.alive():
        g.keys('Escape')
        time.sleep(2)
        if 'Civilization II Multiplayer Gold' in popups(g, raw=True):
            g.keys('Escape')
    if g.alive():
        raise Fail('%s: still running after it was rejected' % g.role)
    host.wait_stable()
    host.shot('after-kick')


def quit_games(host, client):
    n = host.count('rx', nm('NM_QUIT_REQUEST'))
    for g in EXTRA + [client]:
        quit_one(g)
        n += 1
        host.wait_msg('rx', nm('NM_QUIT_REQUEST'), n, timeout=60, what='QUIT_REQUEST')
    quit_one(host)


# ---- checks ----

EXPECT_CLIENT_TX = ['NM_PLAYER_LIST_NODE', 'NM_LOCK_NATION', 'NM_PLAYER_INFO', 'NM_CREATE_CITY',
                    'NM_TURN_FINISHED', 'NM_QUIT_REQUEST']
EXPECT_HOST_TX = ['NM_PLAYER_LIST', 'NM_GAME_GOING', 'NM_SERVER_PATH', 'NM_JOIN_GAMESTATE', 'NM_LOCK_DATA',
                  'NM_NATION_ACCEPT', 'NM_MAP_STRUCT', 'NM_START_CIVILIZATION', 'NM_COSMIC', 'NM_ATTITUDES',
                  'NM_READY_TO_START', 'NM_START_WHOSE_TURN', 'NM_CITY_CREATED']


def subsequence(have, want):
    i = 0
    for h in have:
        if i < len(want) and h == want[i]:
            i += 1
    return want[i:]


def relay_check(games, rep):
    """Three players: the host relays.  Every message a game received was
    sent by another (same type, sequence number and bytes); every message a
    client sent to everyone (to=255) reached both others (but for the
    last few, when one had left)."""
    sent = {}
    for g in games:
        for m in xdmsgs.parse(g.xdlog):
            d = xdmsgs.describe(m)
            if d and m['dir'] == 'tx' and m['chan'] == 'tcp':
                sent.setdefault((d[0], d[2], m['data']), []).append((g, m['to']))
    for g in games:
        got = [(xdmsgs.describe(m), m['data']) for m in xdmsgs.parse(g.xdlog)
               if m['dir'] == 'rx' and m['chan'] == 'tcp']
        unknown = [d for d, data in got if d and (d[0], d[2], data) not in sent]
        rep(not unknown, '%s: all %d received messages were sent by another game%s' % (
            g.role, len(got), '' if not unknown else ' (%d not, the first %s)' % (len(unknown), unknown[0][:3])))
    for g in games:
        if g.role == 'host':
            continue
        allm = [(d[0], d[2], m['data']) for m in xdmsgs.parse(g.xdlog)
                for d in [xdmsgs.describe(m)] if d and m['dir'] == 'tx' and m['chan'] == 'tcp' and m['to'] == 255]
        for o in games:
            if o is g:
                continue
            rx = {(d[0], d[2], m['data']) for m in xdmsgs.parse(o.xdlog)
                  for d in [xdmsgs.describe(m)] if d and m['dir'] == 'rx' and m['chan'] == 'tcp'}
            missed = [k for k in allm if k not in rx]
            rep(len(missed) <= 3, '%s -> %s: %d of %d messages to everyone received' % (
                g.role, o.role, len(allm) - len(missed), len(allm)))


def check(host, client, nturns, result, opts=None):
    ok = True

    def rep(good, text):
        nonlocal ok
        ok &= good
        result.append(('PASS ' if good else 'FAIL ') + text)

    everyone = [host, client] + EXTRA
    for g in everyone:
        r = subprocess.run([sys.executable, os.path.join(ROOT, 'tools/xdnet/xdmsgs.py'), g.xdlog, '--check'],
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        text = r.stdout.decode(errors='replace')
        open(os.path.join(g.out, 'msgs-%s.txt' % g.role), 'w').write(text)
        rep(r.returncode == 0, '%s: records well formed (%d messages)%s' % (
            g.role, text.count('\n'), '' if r.returncode == 0 else ': ' + '; '.join(
                l for l in text.splitlines() if l.startswith('FAIL'))[:300]))
    if EXTRA:
        relay_check(everyone, rep)
    # what each sent over TCP the other received, in order, byte for byte
    for a, b in (((host, client), (client, host)) if not EXTRA else ()):
        sent = [(m[3], m[5], m[6]) for m in a.msgs() if m[1] == 'tx' and m[2] == 'tcp']
        got = [(m[3], m[5], m[6]) for m in b.msgs() if m[1] == 'rx' and m[2] == 'tcp']
        if opts is not None and opts.kick and b is host:   # (and from the player it rejected)
            got = [m for m in got if m in sent]
        if got and got[0] in sent:      # (a client that joined again: from its first message)
            sent = sent[sent.index(got[0]):]
        elif sent and sent[0] in got:   # (the host heard the client's first game as well)
            got = got[len(got) - 1 - got[::-1].index(sent[0]):]
        # the last few may still be in flight when a game quits
        n = min(len(sent), len(got))
        same = sent[:n] == got[:n]
        first = next((i for i in range(n) if sent[i] != got[i]), None)
        # (what the host sent after the client had left is not received)
        left = (b.role == 'client' and b.count('tx', nm('NM_QUIT_REQUEST')) > 0) or (
            b.role == 'host' and opts is not None and opts.migrate)     # (the host left first)
        rep(same and (len(got) >= len(sent) - 3 or left), '%s -> %s: %d sent, %d received, %s%s' % (
            a.role, b.role, len(sent), len(got), 'identical' if same else 'first difference at #%d: %s / %s' % (
                first, sent[first][:2], got[first][:2]),
            ' (the rest after it left)' if left and len(got) < len(sent) else ''))
    for g in everyone:
        t = sum(1 for x in g.turns() if x[1] == 0)
        want = 1 if g is client and opts is not None and opts.rejoin else nturns   # (since it joined again)
        rep(t >= want, '%s: reached turn %d (want %d)' % (g.role, t, want))
    expect_client = EXPECT_CLIENT_TX
    if opts is not None and opts.rejoin:
        expect_client = ['NM_PLAYER_LIST_NODE', 'NM_TURN_FINISHED', 'NM_QUIT_REQUEST']
    if opts is not None and opts.migrate:       # (the host left first: no quit request, the hand-over acked)
        expect_client = expect_client[:-1] + ['NM_SERVER_QUIT_ACK']
    expect_host = EXPECT_HOST_TX
    if SIMULTANEOUS:        # (the humans' moves start with NM_HUMAN_MOVEMENT; a client's turn ends with none)
        expect_client = [w for w in expect_client if w != 'NM_TURN_FINISHED']
    if SIMULTANEOUS:
        expect_host = [w if w != 'NM_START_WHOSE_TURN' else 'NM_HUMAN_MOVEMENT' for w in expect_host]
    for g, want in [(c, expect_client) for c in [client] + EXTRA] + [(host, expect_host)]:
        have = [xdmsgs.NAMES.get(m[3]) for m in g.msgs() if m[1] == 'tx' and m[2] == 'tcp']
        missing = subsequence(have, want)
        rep(not missing, '%s: sent %s in order%s' % (g.role, ' '.join(w[3:] for w in want),
                                                    '' if not missing else ' (missing from %s)' % missing[0]))
    want = []
    if opts is not None and opts.chat:
        want += [(g, 'NM_CHAT') for g in (host, client)]
    if opts is not None and opts.diplo:
        want += [(client, 'NM_PARLEYREQUEST_REPLY'), (host, 'NM_PARLEY_TRANSACTION'), (client, 'NM_PARLEY_TRANSACTION'),
                 (client, 'NM_PARLEY_ACCEPT_OFFER'), (host, 'NM_PARLEY_ACCEPT_OFFER'), (client, 'NM_PARLEY_NO_THANKS'),
                 (host, 'NM_PARLEY_COUNTER_OFFER'), (client, 'NM_PARLEY_EXECUTE'), (client, 'NM_PARLEYREQUEST_CANCEL'),
                 (host, 'NM_PARLEYREQUEST_CANCEL')]
    for g, name in want:
        other = client if g is host else host
        rep(g.count('tx', nm(name)) > 0 and other.count('rx', nm(name)) > 0,
            '%s -> %s: %s' % (g.role, other.role, name[3:]))
    for g in everyone:
        problems = []
        for f in os.listdir(g.out):
            p = os.path.join(g.out, f)
            if f.startswith('asan-%s.' % g.role):
                problems.append('ASan report %s' % f)
            if f in ('wine-%s.log' % g.role, 'ours-%s.log' % g.role):
                t = open(p, errors='replace').read()
                bad = [l for l in t.splitlines() if re.search(
                    r'Unhandled|AddressSanitizer|Segmentation|^fatal:|^civ2: |SIGSEGV|SIGABRT', l)]
                if bad:
                    problems.append('%s: %s' % (f, bad[0][:200]))
        rep(not problems, '%s: no crash%s' % (g.role, '' if not problems else ': ' + '; '.join(problems)))
    return ok


def main():
    global AFTER_LATE
    ap = argparse.ArgumentParser()
    ap.add_argument('pairing', choices=['ours-ours', 'ours-orig', 'orig-ours', 'orig-orig'])
    ap.add_argument('--turns', type=int, default=3)
    ap.add_argument('--run')
    ap.add_argument('--exe', default='civ2-linux32')
    ap.add_argument('--exe-host', help='our build for the host only (e.g. ASan on one side: memory)')
    ap.add_argument('--exe-client')
    ap.add_argument('--gdb', choices=['host', 'client', 'both'], help='run our game(s) under gdb: a backtrace on a crash or fatal error')
    ap.add_argument('--rejoin', action='store_true', help='after the turns the client leaves and joins again (an open game), one more turn')
    ap.add_argument('--third', choices=['ours', 'orig'], help='a third player (LAN), joining after the client: the host relays')
    ap.add_argument('--inet', action='store_true')
    ap.add_argument('--keep', action='store_true')
    ap.add_argument('--sniff', action='store_true')
    ap.add_argument('--save', action='store_true', help='the host saves the game in its turn 2')
    ap.add_argument('--features', default='', help='multiplayer features ticked: kill,positions,production,movement (open: --rejoin, chat: --chat)')
    ap.add_argument('--timer', type=int, default=0, help='turn timer: 0 unlimited, 1 30 s, 2 1 min, 3 2 min ...')
    ap.add_argument('--simultaneous', action='store_true', help='simultaneous moves (CIV.INI Simultaneous=1, the feature ticked)')
    ap.add_argument('--kick', action='store_true', help='with --third: the host rejects the third player in its lobby; two play on')
    ap.add_argument('--extras', action='store_true', help='after the turns: advisors and reports, Multiplayer Options, the host changes the timer')
    ap.add_argument('--lobby', action='store_true', help='with --third --kick: Reject Player with no one listed; the third player '
                    'cancels in the lobby and joins again before it is rejected')
    ap.add_argument('--migrate', action='store_true', help='at the end the host leaves first: the client becomes the host, plays a turn')
    ap.add_argument('--save-end', action='store_true', help='at the end both save the game: the saves must agree (savecmp.py)')
    ap.add_argument('--load', action='store_true', help='then (after quitting) the host loads that game, the client joins, N more turns (implies --save)')
    ap.add_argument('--chat', action='store_true', help='host: chat from the start; the client chats in its turn 2')
    ap.add_argument('--diplo', action='store_true', help='after the turns: emissary, audience, peace treaty (implies --chat: contact)')
    ap.add_argument('--after-late', choices=['fixed', 'classic'], help='with --diplo: one more audience after the late grant; '
                    'fixed: the requester\'s parley window must open (bug B6 fixed), classic: must not (the original\'s bug)')
    ap.add_argument('--stop-after', choices=['lobby', 'join', 'civ', 'turns', 'rejoin'], help='stop there (with --keep: drive by hand)')
    opts = ap.parse_args()
    opts.chat = opts.chat or opts.diplo
    AFTER_LATE = opts.after_late
    if opts.simultaneous:
        SIMULTANEOUS.append(1)
        opts.features = ','.join([f for f in opts.features.split(',') if f] + ['simultaneous'])
    opts.save = opts.save or opts.load
    kh, kc = opts.pairing.split('-')
    run = opts.run or 'test-%s%s' % (opts.pairing, '-inet' if opts.inet else '')
    out = os.path.join(ROOT, 'run/xd/cap', run)
    if os.path.exists(out):
        sh('rm', '-rf', out)
    os.makedirs(os.path.join(out, 'shots'))
    # two containers: the bridge gives each its address (LAN); for Internet
    # mode the client shares the host's network namespace and uses :8
    if 'orig' in (kh, kc, opts.third) and not os.path.exists(os.path.join(ROOT, 'run/xd/logproxy/XDaemon.dll')):
        # the originals' XDaemon log proxy (docs/xdaemon-protocol.md section 11)
        os.makedirs(os.path.join(ROOT, 'run/xd/logproxy'), exist_ok=True)
        sh('docker', 'run', '--rm', '-v', '%s:/work' % ROOT, '-w', '/work/tools/xdnet', 'civ2-dev', 'sh', '-c',
           'i686-w64-mingw32-gcc -O1 -shared -o /work/run/xd/logproxy/XDaemon.dll xdlog.c xdlog.def '
           '-static-libgcc -Wl,--enable-stdcall-fixup')
    docker_up('xdh', 'civ2-wine2')
    docker_up('xdc', 'civ2-wine', net='xdh' if opts.inet else None)
    sniffer = None
    if opts.sniff:
        sh('docker', 'rm', '-f', 'xdsniff', check=False)
        sh('docker', 'run', '-d', '--name', 'xdsniff', '--network', 'container:xdh', '--user', 'root', '-v',
           '%s:/work' % ROOT, 'civ2-dev', 'python3', '-I', '/work/tools/xdnet/sniff.py',
           '/work/run/xd/cap/%s/capture.pcap' % run)
        sniffer = 'xdsniff'
    host = Game(kh, 'host', 'xdh', 7, run, opts.exe_host or opts.exe)
    client = Game(kc, 'client', 'xdc', 8 if opts.inet else 7, run, opts.exe_client or opts.exe)
    host.gdb = opts.gdb in ('host', 'both')
    client.gdb = opts.gdb in ('client', 'both')
    if opts.third:
        # a third player (LAN only): its own container; an original needs a
        # Wine prefix the other games' originals don't use
        vols = [v for v, g in (('civ2-wine2', host), ('civ2-wine', client)) if g.kind != 'orig']
        if opts.third == 'orig' and not vols:
            raise SystemExit('--third orig: both Wine prefixes are in use')
        docker_up('xdc2', vols[0] if vols else 'civ2-wine')
        third = Game(opts.third, 'client2', 'xdc2', 7, run, opts.exe_client or opts.exe)
        third.gdb = False
        EXTRA.append(third)
    result = []
    ok = False
    try:
        log('starting', opts.pairing)
        start(host, opts.inet)
        start(client, opts.inet)
        for g in EXTRA:
            start(g, opts.inet)
        log('host: new game')
        host_setup(host, opts.inet, chat=opts.chat, rejoin=opts.rejoin,
                   features=[f for f in opts.features.split(',') if f], timer=opts.timer)
        if opts.lobby:
            lobby_empty_reject(host)
        if opts.stop_after == 'lobby':
            raise Fail('stopped after lobby')
        log('client: join')
        client_join(client, host, opts.inet)
        for g in EXTRA:
            log('%s: join' % g.role)
            client_join(g, host, opts.inet, name='Carol' if opts.lobby else 'Bob')
        host.wait_msg('rx', nm('NM_PLAYER_LIST_NODE'), 1 + len(EXTRA), timeout=60, what='PLAYER_LIST_NODE')
        host.wait_stable()
        if opts.lobby:
            lobby_leave(host, EXTRA[-1], opts.inet)
        if opts.kick:
            kick(host, EXTRA.pop())
        host.step('players', 'Return')              # Start Game
        host.step('profile', 'Return')              # Game Profile: OK
        for g in [client] + EXTRA:
            g.wait_msg('rx', nm('NM_JOIN_GAMESTATE'), timeout=60, what='JOIN_GAMESTATE')
            g.wait_stable()
            g.step('profile', 'Return')
        if opts.stop_after == 'join':
            raise Fail('stopped after join')
        log('choosing civilizations')
        choose_civ(host)
        choose_civ(client)
        for g in EXTRA:
            choose_civ(g)
        log('building the world')
        host.wait_msg('tx', nm('NM_READY_TO_START'), timeout=600, what='READY_TO_START')
        if opts.stop_after == 'civ':
            raise Fail('stopped after civ')
        play(host, client, opts.turns, opts)
        if opts.rejoin:
            rejoin(host, client, opts)
        if opts.chat:
            chat(host, client)
        if opts.diplo:
            diplomacy(host, client)
        if opts.extras:
            extras(host, client)
        if opts.save_end:
            compare_saves(host, client, result)
        if opts.stop_after == 'turns':
            raise Fail('stopped after turns')
        log('quitting')
        if opts.migrate:
            migrate(host, client, opts)
        else:
            quit_games(host, client)
        if opts.load:
            # the game saved in it, loaded by the host, the client joining
            check(host, client, opts.turns, result, opts)
            result[:] = [r[:5] + 'game 1: ' + r[5:] for r in result]
            for g in (host, client):
                for f in os.listdir(out):
                    if f.startswith(('xdlog-', 'msgs-')) and not f.endswith('-game1.txt'):
                        os.rename(os.path.join(out, f), os.path.join(out, f[:-4] + '-game1.txt'))
                g.logpos, g.wins, g.shown, g.pending = 0, {}, set(), None
                g.turn_seen, g.turn_t, g.popped, g.last_pop = 0, 0, False, 0
            for f in os.listdir(out):
                if f.startswith('ours-') and not f.endswith('-game1.log'):
                    os.rename(os.path.join(out, f), os.path.join(out, f[:-4] + '-game1.log'))
            log('loading the saved game')
            start(host, opts.inet, save=os.path.join(out, 'saved.net'))
            start(client, opts.inet)
            host_load(host, opts.inet)
            client_join(client, host, opts.inet)
            host.wait_msg('rx', nm('NM_PLAYER_LIST_NODE'), timeout=60, what='PLAYER_LIST_NODE')
            host.wait_stable()
            host.step('players', 'Return')
            host.step('profile', 'Return')
            client.wait_msg('rx', nm('NM_JOIN_GAMESTATE'), timeout=60, what='JOIN_GAMESTATE')
            client.wait_stable()
            client.step('profile', 'Return')
            start_loaded(host, client)
            opts.chat = opts.diplo = opts.save = False
            play(host, client, opts.turns, opts)
            log('quitting the loaded game')
            quit_games(host, client)
    except Fail as e:
        result.append('FAIL ' + str(e))
        log('FAIL', e)
        for g in (host, client):
            try:
                g.shot('fail')
            except Exception:
                pass
    except KeyboardInterrupt:
        result.append('FAIL interrupted')
    finally:
        if sniffer:
            sh('docker', 'stop', sniffer, check=False)
            sh('docker', 'rm', '-f', sniffer, check=False)
            pc = os.path.join(out, 'capture.pcap')
            if os.path.exists(pc):
                with open(os.path.join(out, 'records.txt'), 'w') as f:
                    subprocess.run([sys.executable, '-I', os.path.join(ROOT, 'tools/xdnet/xdparse.py'), pc], stdout=f)
        ok = check(host, client, opts.turns, result, opts) and not any(r.startswith('FAIL') for r in result)
        if not opts.keep:
            sh('docker', 'rm', '-f', 'xdc2', check=False)
            sh('docker', 'rm', '-f', 'xdc', check=False)
            sh('docker', 'rm', '-f', 'xdh', check=False)
    result.append('%s %s %s (%d turns, %s)' % ('PASS' if ok else 'FAIL', opts.pairing,
                                                'inet' if opts.inet else 'lan', opts.turns, opts.exe))
    open(os.path.join(out, 'result.txt'), 'w').write('\n'.join(result) + '\n')
    print('\n'.join(result))
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
