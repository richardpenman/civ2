#!/usr/bin/env python3
"""The advisors' backgrounds (tiles.dll 50 to 59, 65, 66) OpenCiv2 made and
gave the art pack (openciv2-assets pull request 6; docs/HD-GRAPHICS.md): generate
candidates with Gemini 3 Pro Image, and prepare a chosen one for the game.
(Kept for their record, the prompts below, and to make others; the game
draws the pack's now, made by make hd-art.)

    GK=KEY gen_hd_backgrounds.py generate OUTDIR ID [ID...] [--n 2] [--tag a]
    gen_hd_backgrounds.py prepare ID IMAGE OUT.png

generate: for each tiles.dll id, N candidates OUTDIR/ID-TAGn.(png|jpg) from
the prompt below (TEMPLATE with the id's SUBJECTS line), two of the art
pack's backgrounds as style references (REFS; from openciv2-assets,
backgrounds/diplomacy-monarchy-native.png and council-room-native.png, as
JPEG, in REFS: the art is not committed, so put them there first); the
usage (output tokens) appended to OUTDIR/usage.jsonl.  The Gemini API key
is read from the environment (GK), never stored.

prepare: the generated picture scaled to cover the area the windows show of
the 640x480 picture at density 2 (the advisors' 600x400: 1200x800; the tax
rates' 400x256 on small screens, 65: 800x512; their 600x384 on large ones,
66, prepared as the advisors' from 65's painting),
cropped about its middle, and the rest of the 1280x960 mirrored from it;
(OpenCiv2's were then encoded as lossy WebP, cwebp 1.5.0 -q 90 -m 4
-sharp_yuv -metadata none; the pack has them as PNG.)
"""
import argparse, base64, json, os, sys, threading, time, urllib.error, urllib.request

MODEL = 'gemini-3-pro-image'
REFS = ('run/gemini-test/ref-diplomacy-monarchy.jpg', 'run/gemini-test/ref-council-room.jpg')

TEMPLATE = (
    "A painted background for a strategy game's {report}, in the same painterly digital-illustration "
    "style as the first two reference images. "
    "Subject: {subject} "
    "Treatment: very dark and muted, almost monochrome charcoal and slate grey with a slight {tint} tint; low "
    "contrast; soft, even lighting with no bright highlights; overall brightness about 35% (average colour near "
    "RGB 90, 90, 90). "
    "Composition: the left two thirds and the top quarter must be calm, empty, low-detail areas, because rows of "
    "white and yellow text and small icons are drawn over them; keep the busier details near the right and "
    "bottom edges. "
    "No text, letters, numbers, labels, logos, frames, borders or vignette; {nowriting} Landscape, 3:2. "
    "Match the brushwork and palette of the reference images.")

# id: (report, subject, tint, no-writing clause)
SUBJECTS = {
    50: ("domestic advisor's city report",
         "a steward's closed leather ledger and household goods on a long wooden table: a few grain sacks, an "
         "unlit oil lamp and a ring of iron keys, gathered at the far right edge.",
         'warm', 'the ledger is closed and nothing has writing on it.'),
    51: ("military advisor report",
         "an antique world map spread on a war-room table, with faint brass dividers, a few wooden army markers "
         "and a rolled campaign chart at the far right edge.",
         'cool', 'the map has no writing on it.'),
    52: ("foreign minister's intelligence report",
         "an envoy's writing desk with a few folded, sealed letters, red wax seals, a stick of sealing wax and a "
         "quill in an inkwell, at the far right edge.",
         'warm', 'the letters are folded and sealed, with no writing visible.'),
    53: ("attitude advisor's report on the people's mood",
         "a crowded town square seen from high above at dusk: small figures among market stalls and a "
         "fountain, the houses' roofs around it, the crowd thickest towards the right and bottom edges.",
         'cool', 'no signs or banners with writing.'),
    54: ("trade advisor report",
         "a merchant's table with stacks of plain gold and silver coins, a brass balance scale and trade goods: "
         "bolts of cloth and a few spice jars, at the far right edge.",
         'warm', 'the coins are plain, with no markings, and nothing has writing on it.'),
    55: ("science advisor report",
         "a scholar's table with a brass astrolabe, a brass telescope on a stand and star charts drawn only as "
         "dots and faint lines, the instruments at the far right edge.",
         'cool', 'the charts have no writing or numbers on them.'),
    56: ("wonders of the world report",
         "monuments in a dim museum gallery: classical statues on pedestals, a small model pyramid and a row of "
         "columns, receding towards the right edge.",
         'warm', 'no plaques or inscriptions.'),
    57: ("report of the five greatest cities",
         "a city skyline at night seen across a river, its buildings from every era side by side: an ancient "
         "temple, medieval towers, domes and modern skyscrapers, few and dim lit windows, the tallest buildings "
         "towards the right edge.",
         'cool', 'no signs or billboards.'),
    58: ("demographics report",
         "a clerk's table with tall stacks of closed census ledgers and record books, a wooden abacus and a "
         "rolled scroll, at the far right edge.",
         'warm', 'the books are closed and nothing has writing on it.'),
    59: ("civilization score report",
         "laurel wreaths, a silver cup and small bronze trophies standing on a dark stone plinth, at the far "
         "right edge.",
         'warm', 'no engravings or inscriptions.'),
    65: ("tax rate dialog",
         "a treasury table with open wooden coin chests full of plain coins, a few coin stacks and a wooden "
         "tally board with notches, at the far right edge.",
         'warm', 'the coins are plain and the tally board has only notches, no writing.'),
}

# the windows' visible area of each picture at density 2 (prepare)
AREA = {65: (800, 512)}
DEFAULT_AREA = (1200, 800)


def prompt(i):
    report, subject, tint, nowriting = SUBJECTS[i]
    return TEMPLATE.format(report=report, subject=subject, tint=tint, nowriting=nowriting)


def generate(outdir, i, tag, lock):
    key = os.environ['GK']
    refs = [base64.b64encode(open(r, 'rb').read()).decode() for r in REFS]
    body = {'contents': [{'parts': [{'text': prompt(i)}] +
                          [{'inline_data': {'mime_type': 'image/jpeg', 'data': r}} for r in refs]}],
            'generationConfig': {'responseModalities': ['IMAGE'],
                                 'imageConfig': {'aspectRatio': '3:2', 'imageSize': '2K'}}}
    req = urllib.request.Request(
        'https://generativelanguage.googleapis.com/v1beta/models/%s:generateContent' % MODEL,
        data=json.dumps(body).encode(), headers={'Content-Type': 'application/json', 'x-goog-api-key': key})
    t = time.time()
    for attempt in range(3):
        try:
            d = json.load(urllib.request.urlopen(req, timeout=400))
            break
        except (urllib.error.HTTPError, urllib.error.URLError, TimeoutError) as e:
            msg = e.read()[:300] if hasattr(e, 'read') else str(e)
            print(i, tag, 'error', getattr(e, 'code', ''), msg, flush=True)
            time.sleep(20)
    else:
        return
    saved = None
    for c in d.get('candidates', []):
        for p in c.get('content', {}).get('parts', []):
            if 'inlineData' in p:
                ext = 'png' if 'png' in p['inlineData']['mimeType'] else 'jpg'
                saved = os.path.join(outdir, '%d-%s.%s' % (i, tag, ext))
                open(saved, 'wb').write(base64.b64decode(p['inlineData']['data']))
    usage = d.get('usageMetadata', {})
    with lock:
        with open(os.path.join(outdir, 'usage.jsonl'), 'a') as f:
            f.write(json.dumps({'id': i, 'tag': tag, 'file': saved, 'seconds': round(time.time() - t, 1),
                                'output_tokens': usage.get('candidatesTokenCount'), 'usage': usage}) + '\n')
    print(i, tag, saved, usage.get('candidatesTokenCount'), flush=True)


def prepare(i, src, out):
    from PIL import Image
    im = Image.open(src).convert('RGB')
    aw, ah = AREA.get(i, DEFAULT_AREA)
    s = max(aw / im.width, ah / im.height)
    w, h = round(im.width * s), round(im.height * s)
    im = im.resize((w, h), Image.LANCZOS)
    x, y = (w - aw) // 2, (h - ah) // 2
    im = im.crop((x, y, x + aw, y + ah))
    full = Image.new('RGB', (1280, 960))
    full.paste(im, (0, 0))
    # the right margin: the area mirrored about its right edge, as often as needed
    x = aw
    while x < 1280:
        strip = im.transpose(Image.FLIP_LEFT_RIGHT).crop((0, 0, min(aw, 1280 - x), ah))
        full.paste(strip, (x, 0))
        x += strip.width
    row = full.crop((0, 0, 1280, ah))
    y = ah
    while y < 960:
        strip = row.transpose(Image.FLIP_TOP_BOTTOM).crop((0, 0, 1280, min(ah, 960 - y)))
        full.paste(strip, (0, y))
        y += strip.height
    full.save(out, optimize=True)


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest='cmd', required=True)
    g = sub.add_parser('generate')
    g.add_argument('outdir')
    g.add_argument('ids', nargs='+', type=int)
    g.add_argument('--n', type=int, default=2)
    g.add_argument('--tag', default='')
    p = sub.add_parser('prepare')
    p.add_argument('id', type=int)
    p.add_argument('image')
    p.add_argument('out')
    sub.add_parser('prompt').add_argument('id', type=int)
    a = ap.parse_args()
    if a.cmd == 'prompt':
        print(prompt(a.id))
    elif a.cmd == 'prepare':
        prepare(a.id, a.image, a.out)
    else:
        if 'GK' not in os.environ:
            sys.exit('gen_hd_backgrounds: the Gemini API key in GK')
        os.makedirs(a.outdir, exist_ok=True)
        lock = threading.Lock()
        th = [threading.Thread(target=generate, args=(a.outdir, i, '%s%d' % (a.tag, n + 1), lock))
              for i in a.ids for n in range(a.n)]
        for t in th:
            t.start()
            time.sleep(1)
        for t in th:
            t.join()


if __name__ == '__main__':
    main()
