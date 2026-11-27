# The contact sheet of harness/hd/hd-talks-all.sh: for each tribe (rows) and
# gender, the HD room's portrait panel beside the original's, labelled.  With
# OpenCiv2's historical portraits (data/native/portraits: make own-assets) each
# tribe's own is drawn (its run's hook.log names it: checked); without them the
# tribes src/game/hd_graphics.cpp's hd_portraits gives no pack portrait
# (ORIGINAL) must show the original's enlarged: checked.
import os, sys
from PIL import Image, ImageChops, ImageDraw, ImageStat
out = sys.argv[1]
TRIBES = ("Romans Babylonians Germans Egyptians Americans Greeks Indians Russians Zulus French Aztecs "
          "Chinese English Mongols Celts Japanese Vikings Spanish Persians Carthaginians Sioux").split()
BOX = (330, 140, 560, 440)          # the portrait in the 1024x768 frame
PIC = (365, 160, 535, 420)          # inside it: the picture alone
ORIGINAL = {8}                      # the Zulus
W, H = BOX[2] - BOX[0], BOX[3] - BOX[1]


def shot(d, name):
    """A shot's file: PNG (tools/native-run.sh), or BMP (CIV2_KEEP_BMP=1, older runs)."""
    p = os.path.join(d, name + ".png")
    return p if os.path.exists(p) else os.path.join(d, name + ".bmp")


cells = []
for t in range(21):
    for g in "mf":
        d = os.path.join(out, "%d-%s" % (t, g))
        if os.path.exists(shot(d, "room-hd")):
            cells.append((t, g, d))
cols = 4                            # two tribes a row: (HD, original) x (m, f)
sheet = Image.new("RGB", (cols * (W + 8), ((len(cells) + 1) // 2) * (H + 24)), "white")
draw = ImageDraw.Draw(sheet)
for i, (t, g, d) in enumerate(cells):
    hd = Image.open(shot(d, "room-hd")).convert("RGB").resize((1024, 768), Image.LANCZOS).crop(BOX)
    orig = Image.open(shot(d, "room")).convert("RGB").crop(BOX)
    # the original's kept: the HD picture should be it enlarged, their mean difference small
    a = Image.open(shot(d, "room-hd")).convert("L").resize((1024, 768), Image.BOX).crop(PIC)
    b = Image.open(shot(d, "room")).convert("L").crop(PIC)
    diff = sum(ImageStat.Stat(ImageChops.difference(a, b)).mean)
    want = "portrait/%d" % (220 + t + (21 if g == "f" else 0))
    log = open(os.path.join(d, "hook.log"), errors="replace").read() if os.path.exists(os.path.join(d, "hook.log")) else ""
    if os.path.exists(os.path.join("data", "native", "portraits", want[9:].join(("leader-", ".webp")))):
        print("%-14s %s  historical %s%s" % (TRIBES[t], g, want, "" if ": " + want + "\n" in log else "  CHECK: not drawn"))
    elif t in ORIGINAL:
        print("%-14s %s  the original's: difference %5.1f%s" % (TRIBES[t], g, diff, "  CHECK" if diff > 20 else ""))
    else:
        print("%-14s %s  the pack's (look)" % (TRIBES[t], g))
    x = (i % 2) * 2 * (W + 8); y = (i // 2) * (H + 24)
    sheet.paste(hd, (x, y + 20)); sheet.paste(orig, (x + W + 8, y + 20))
    draw.text((x + 4, y + 4), "%s %s: HD | original" % (TRIBES[t], "male" if g == "m" else "female"), fill="black")
sheet.save(os.path.join(out, "contact.png"))
print("wrote", os.path.join(out, "contact.png"), len(cells), "leaders")
