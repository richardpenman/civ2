#!/usr/bin/env python3
"""Drive the original Civ2 under Wine through an input script (the recordings
the native builds are compared against).

Default mode is deterministic replay: the script is executed inside the game
process by the hook runtime (hook/replay.c) on a virtual clock, so the run
depends only on the seed and the script.  --xdotool drives the game from
outside in wall-clock time instead (not reproducible over long runs).

Runs inside the civ2-dev container.  Usage:
    run.py --build orig --seed N --script FILE --out DIR

Script language (one command per line, '#' comments):
    wait MS             sleep for MS milliseconds
    settle [MS]         wait until the screen stops changing (default 1500ms quiet)
    key KEYS...         xdotool key names, e.g. "key Return", "key alt+f"
    type TEXT           type literal text
    click X Y [BTN]     click at screen coords (default button 1)
    shot NAME           save screenshot DIR/NAME.png
"""
import argparse, os, shutil, subprocess, sys, time
from PIL import Image, ImageChops
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'tools'))
import paths  # noqa: E402  (the game's directory)

ROOT = '/work'
DISPLAY = ':7'
SCREEN = (1024, 768)


def grab():
    from io import BytesIO
    for attempt in range(10):
        xwd = subprocess.run(['xwd', '-root', '-silent', '-display', DISPLAY], capture_output=True).stdout
        p = subprocess.run(['convert', 'xwd:-', 'png:-'], input=xwd, capture_output=True).stdout
        try:
            return Image.open(BytesIO(p)).convert('RGB')
        except Exception:
            time.sleep(0.2)
    raise RuntimeError('screen grab failed')


def settle(quiet_ms=1500, timeout_ms=30000):
    t0 = time.time(); last = grab(); quiet_since = time.time()
    while (time.time() - t0) * 1000 < timeout_ms:
        time.sleep(0.25)
        cur = grab()
        if ImageChops.difference(cur, last).getbbox() is None:
            if (time.time() - quiet_since) * 1000 >= quiet_ms:
                return cur
        else:
            quiet_since = time.time(); last = cur
    return last


def xdo(*args):
    subprocess.run(['xdotool'] + list(args), env=dict(os.environ, DISPLAY=DISPLAY))


def prepare_game_dir(build, gamedir):
    # (recorded without the CD's videos and music: KINGS, Video, Music left
    # out, as tools/native-run.sh does; CIV2_KEEP_MEDIA=1 keeps them)
    keep = os.environ.get('CIV2_KEEP_MEDIA')
    shutil.copytree(paths.GAME, gamedir, ignore=None if keep else
                    lambda d, names: [n for n in names if d == paths.GAME and n.lower() in ('kings', 'video', 'music')])
    for d, _, fs in os.walk(gamedir):        # (the game may be mounted read-only)
        for f in fs:
            os.chmod(os.path.join(d, f), 0o644)
    # Hook runtime: proxy XDaemon.dll in front of the real one.
    os.rename(f'{gamedir}/XDaemon.dll', f'{gamedir}/XDaemon_real.dll')
    shutil.copy(f'{ROOT}/hook/XDaemon.dll', f'{gamedir}/XDaemon.dll')
    # Pre-seeded settings: skips the one-time "heralds" RAM question.
    shutil.copy(f'{ROOT}/harness/CIV2.DAT', f'{gamedir}/CIV2.DAT')


WIN_INI = '/wine/drive_c/windows/CIV.INI'


def seed_wine_ini():
    """Reset C:\\windows\\CIV.INI to the harness copy so every run starts identically."""
    src = f'{ROOT}/harness/CIV.INI'
    if os.path.exists(src):
        shutil.copy(src, WIN_INI)
    elif os.path.exists(WIN_INI):
        os.remove(WIN_INI)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', default='orig', choices=['orig'])
    ap.add_argument('--seed', default='1')
    ap.add_argument('--script', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--exe', default='civ2.exe')
    ap.add_argument('--xdotool', action='store_true', help='external wall-clock input instead of replay')
    ap.add_argument('--timeout', type=int, default=900, help='replay mode: max seconds')
    ap.add_argument('--rand-stack', default='0', help='log this many callers per rand() call')
    a = ap.parse_args()

    os.makedirs(a.out, exist_ok=True)
    gamedir = '/tmp/game'
    shutil.rmtree(gamedir, ignore_errors=True)
    prepare_game_dir(a.build, gamedir)

    # Leftovers from a previous run in the same container would stop Xvfb.
    subprocess.run(['wineserver', '-k'], capture_output=True)
    for stale in ('/tmp/.X7-lock', '/tmp/.X11-unix/X7'):
        if os.path.exists(stale):
            os.remove(stale)
    xvfb = subprocess.Popen(['Xvfb', DISPLAY, '-screen', '0', '%dx%dx24' % SCREEN, '-nolisten', 'tcp'],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1.5)
    env = dict(os.environ, DISPLAY=DISPLAY, CIV2_SEED=str(a.seed),
               CIV2_HOOK_LOG='Z:' + os.path.abspath(f'{a.out}/hook.log'),
               CIV2_RAND_LOG='Z:' + os.path.abspath(f'{a.out}/rand.log'), CIV2_HOOKS='none',
               CIV2_CALL_LOG='Z:' + os.path.abspath(f'{a.out}/calls.log'),
               CIV2_RAND_STACK=a.rand_stack, CIV2_LOG_KEYS='1')
    if not os.path.exists('/wine/system.reg'):
        subprocess.run(['wineboot', '-i'], env=env, capture_output=True)
    seed_wine_ini()
    # A crash must end the process, not open wine's debugger and wait.
    subprocess.run(['wine', 'reg', 'delete', r'HKLM\Software\Microsoft\Windows NT\CurrentVersion\AeDebug',
                    '/v', 'Debugger', '/f'], env=env, capture_output=True)
    if not a.xdotool:
        env['CIV2_SCRIPT'] = 'Z:' + os.path.abspath(a.script)
        env['CIV2_SHOT_DIR'] = 'Z:' + os.path.abspath(a.out)
    game = subprocess.Popen(['wine', a.exe], cwd=gamedir, env=env,
                            stdout=open(f'{a.out}/wine.log', 'w'), stderr=subprocess.STDOUT)
    try:
        if not a.xdotool:
            # Wait for the script to finish; a crash (wine prints "Unhandled
            # ...") ends the run at once instead of waiting for the timeout.
            t0 = time.time()
            while game.poll() is None:
                if time.time() - t0 > a.timeout:
                    print('replay timed out')
                    break
                if b'Unhandled' in open(f'{a.out}/wine.log', 'rb').read():
                    print('game crashed')
                    time.sleep(1)
                    break
                time.sleep(1)
            for f in sorted(os.listdir(a.out)):
                if f.endswith('.bmp'):
                    Image.open(os.path.join(a.out, f)).convert('RGB').save(os.path.join(a.out, f[:-4] + '.png'))
                    os.remove(os.path.join(a.out, f))
                    print('shot', f[:-4], flush=True)
            if 'replay: script finished' not in open(f'{a.out}/hook.log', 'rb').read().decode('latin1'):
                print('game exited before the script finished')
            return
        for lineno, line in enumerate(open(a.script), 1):  # --xdotool mode
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            cmd, *args = line.split()
            if cmd == 'wait':
                time.sleep(int(args[0]) / 1000)
            elif cmd == 'settle':
                settle(int(args[0]) if args else 1500)
            elif cmd == 'key':
                xdo('key', '--delay', '80', *args)
            elif cmd == 'type':
                xdo('type', '--delay', '80', line.split(None, 1)[1])
            elif cmd == 'click':
                xdo('mousemove', args[0], args[1]); time.sleep(0.1)
                xdo('click', args[2] if len(args) > 2 else '1')
            elif cmd == 'shot':
                grab().save(f'{a.out}/{args[0]}.png')
                print('shot', args[0], flush=True)
            else:
                sys.exit(f'{a.script}:{lineno}: unknown command {cmd}')
            if game.poll() is not None:
                print('game exited with', game.returncode); break
    finally:
        # Logs are memory-mapped (hook/maplog.c), so a hard kill loses nothing
        # and avoids exercising the shutdown path at a random moment.
        game.kill()
        # Keep files the game created/changed (INI settings, saves) for inspection.
        for log in ('hook.log', 'rand.log'):
            lp = f'{a.out}/{log}'
            if os.path.exists(lp):
                data = open(lp, 'rb').read().rstrip(b'\0')
                open(lp, 'wb').write(data)
        os.makedirs(f'{a.out}/files', exist_ok=True)
        if os.path.exists(WIN_INI):
            shutil.copy(WIN_INI, f'{a.out}/files/CIV.INI')
        for f in os.listdir(gamedir):
            src = os.path.join(gamedir, f)
            orig = os.path.join(paths.GAME, f)
            if os.path.isfile(src) and (not os.path.exists(orig) or open(src, 'rb').read() != open(orig, 'rb').read()):
                if not f.startswith('XDaemon'):
                    shutil.copy(src, f'{a.out}/files/{f}')
        subprocess.run(['wineserver', '-k'], capture_output=True)
        xvfb.terminate()
        try:
            xvfb.wait(5)
        except subprocess.TimeoutExpired:
            xvfb.kill()


if __name__ == '__main__':
    main()
