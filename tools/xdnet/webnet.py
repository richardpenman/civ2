#!/usr/bin/env python3
"""A network game between the browser version and a native game, through
the relay (tools/webrelay.py): tools/xdnet/nettest.py's steps, one player
being the browser version in Chrome's headless shell on this machine
(src/web/test/player.mjs, driven by keys as xdotool drives the native one).
The native game runs in a civ2-dev container (as nettest.py runs ours) on
the docker bridge; the relay on this machine connects the browser to it, or
listens for it on the bridge's gateway address.  Internet games (by
address): the browser has no LAN discovery.

usage (from the project root, on the docker host; after make web and the
native build, e.g. make port for EXE=OpenCiv2):
  tools/xdnet/webnet.py native-web [--turns N] [--exe OpenCiv2]   (the native game hosts, the browser joins)
  tools/xdnet/webnet.py web-native                                (the browser hosts, the native game joins)

Everything in run/xd/cap/web-PAIRING/ as nettest.py writes it (xdlog-*,
msgs-*, ours-*.log, shots/, result.txt), and relay.log; the same checks
(nettest.check): the records well formed, what one sent the other received
byte for byte, both reached turn N, the expected messages in order, no crash.
"""
import argparse
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nettest  # noqa: E402
from nettest import Game, Fail, log, nm, sh, docker_up, start, host_setup, to_network_menu, choose_civ, play, \
    quit_games, check  # noqa: E402

ROOT = nettest.ROOT
NODE = os.path.join(ROOT, 'tools', 'toolchain', 'node', 'bin', 'node')
RELAY_PORT = 8765


class WebGame(Game):
    """The browser version: the same interface as a Game, its keys, screen
    and logs through src/web/test/player.mjs."""

    def __init__(self, role, run, relay):
        Game.__init__(self, 'ours', role, None, 0, run, 'web')
        self.relay = relay
        self.proc = None

    def launch(self):
        os.makedirs(self.shots, exist_ok=True)
        self.err = open(os.path.join(self.out, 'player-%s.log' % self.role), 'w')
        self.proc = subprocess.Popen([NODE, os.path.join(ROOT, 'src/web/test/player.mjs'), self.out, self.role,
                                      self.relay], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=self.err,
                                     text=True, bufsize=1)
        r = self.read(600)
        if not r.get('ready'):
            raise Fail('%s: the browser game did not start: %s' % (self.role, r.get('error')))
        time.sleep(3)
        self.wait_stable(timeout=60, still=3)
        self.shot('menu')

    def read(self, timeout=120):
        line = self.proc.stdout.readline()
        if not line:
            raise Fail('%s: the browser player stopped' % self.role)
        return json.loads(line)

    def cmd(self, **c):
        self.proc.stdin.write(json.dumps(c) + '\n')
        self.proc.stdin.flush()
        r = self.read()
        if r.get('ok') is False and 'error' in r:
            raise Fail('%s: %s: %s' % (self.role, c, r['error']))
        return r

    def ex(self, cmd, **kw):
        raise Fail('the browser game has no shell (%s)' % cmd)

    def keys(self, *keys, pause=None):
        for k in keys:
            self.cmd(cmd='key', key=k)
            time.sleep(pause if pause is not None else 0.4)

    def type(self, text):
        self.cmd(cmd='type', text=text)
        time.sleep(0.5)

    def click(self, x, y):
        self.cmd(cmd='click', x=x, y=y)
        time.sleep(0.3)

    def frames(self, col=None):
        return []

    def centred_dy(self):
        return 0

    def shot(self, name=None):
        self.n += 1
        p = os.path.join(self.shots, 'w%s%02d-%s.png' % (self.role[0], self.n, name or 'x'))
        self.cmd(cmd='shot', path=p)
        return p

    def wait_stable(self, timeout=30, still=2, tol=600):
        ok = self.cmd(cmd='stable', quiet=int(still * 1000), tol=tol, most=int(timeout * 1000)).get('ok')
        if not ok:
            log(self.role, 'screen not stable after %ds' % timeout)
        return ok

    def alive(self):
        if not self.proc or self.proc.poll() is not None:
            return False
        return bool(self.cmd(cmd='status').get('alive'))

    def stop(self):
        if self.proc and self.proc.poll() is None:
            try:
                self.cmd(cmd='quit')
                self.proc.wait(30)
            except Exception:
                self.proc.kill()


def join_inet(g, address, name='Bob'):
    """Join an Internet game at ADDRESS (nettest.client_join's Internet branch, the address given)."""
    to_network_menu(g, True)
    g.step('join', 'Down', 'Down', 'Down', 'Down', 'Down', 'Return')
    g.type(name)
    g.step('netname', 'Return')
    g.keys('ctrl+a')
    g.type(address)
    g.step('address', 'Return')
    g.wait_msg('tx', nm('NM_PLAYER_LIST_NODE'), 1, timeout=90, what='PLAYER_LIST_NODE')
    g.wait_stable()
    g.shot('joined')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('pairing', choices=['native-web', 'web-native'])
    ap.add_argument('--turns', type=int, default=2)
    ap.add_argument('--exe', default='OpenCiv2', help='the native build (build/EXE) in the container')
    ap.add_argument('--keep', action='store_true', help='leave the container running')
    opts = ap.parse_args()
    opts.save = False
    run = 'web-' + opts.pairing
    out = os.path.join(ROOT, 'run/xd/cap', run)
    if os.path.exists(out):
        sh('rm', '-rf', out)
    os.makedirs(os.path.join(out, 'shots'))
    gateway = subprocess.run(['docker', 'network', 'inspect', 'bridge', '-f', '{{(index .IPAM.Config 0).Gateway}}'],
                             capture_output=True, text=True).stdout.strip() or '172.17.0.1'
    # the relay: the WebSocket side on this machine only; a browser's game
    # listened for on the bridge's gateway, where the container reaches it
    relay_log = open(os.path.join(out, 'relay.log'), 'w')
    relay = subprocess.Popen([sys.executable, os.path.join(ROOT, 'tools/webrelay.py'), '--port', str(RELAY_PORT),
                              '--listen-host', gateway, '-v'], stdout=relay_log, stderr=subprocess.STDOUT)
    web_role, native_role = ('client', 'host') if opts.pairing == 'native-web' else ('host', 'client')
    web = WebGame(web_role, run, 'ws://127.0.0.1:%d' % RELAY_PORT)
    native = Game('ours', native_role, 'xdh', 7, run, opts.exe)
    native.gdb = False
    host, client = (native, web) if native_role == 'host' else (web, native)
    result = []
    try:
        docker_up('xdh', 'civ2-wine2')
        native_ip = subprocess.run(['docker', 'inspect', '-f', '{{.NetworkSettings.IPAddress}}', 'xdh'],
                                   capture_output=True, text=True).stdout.strip()
        log('starting: %s hosts; the native game at %s, the relay listening for the browser at %s' % (
            host.role if host is native else 'the browser', native_ip, gateway))
        start(native, True)
        web.launch()
        log('host: new game')
        host_setup(host, True)
        log('client: join')
        join_inet(client, native_ip if host is native else gateway)
        host.wait_msg('rx', nm('NM_PLAYER_LIST_NODE'), 1, timeout=60, what='PLAYER_LIST_NODE')
        host.wait_stable()
        host.step('players', 'Return')              # Start Game
        host.step('profile', 'Return')              # Game Profile: OK
        client.wait_msg('rx', nm('NM_JOIN_GAMESTATE'), timeout=60, what='JOIN_GAMESTATE')
        client.wait_stable()
        client.step('profile', 'Return')
        log('choosing civilizations')
        choose_civ(host)
        choose_civ(client)
        log('building the world')
        host.wait_msg('tx', nm('NM_READY_TO_START'), timeout=600, what='READY_TO_START')
        play(host, client, opts.turns, opts)
        log('quitting')
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
        web.stop()
        relay.terminate()
        relay.wait()
        ok = check(host, client, opts.turns, result) and not any(r.startswith('FAIL') for r in result)
        if not opts.keep:
            sh('docker', 'rm', '-f', 'xdh', check=False)
    result.append('%s %s inet (%d turns, %s, through the relay)' % ('PASS' if ok else 'FAIL', opts.pairing, opts.turns,
                                                                    opts.exe))
    open(os.path.join(out, 'result.txt'), 'w').write('\n'.join(result) + '\n')
    print('\n'.join(result))
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
