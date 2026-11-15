# The contact sheet of harness/hd/hd-talks-all.sh: for each tribe (rows) and
# gender, the HD room's portrait panel beside the original's, labelled.
import os, sys
from PIL import Image, ImageChops, ImageDraw, ImageStat
out = sys.argv[1]
TRIBES = ("Romans Babylonians Germans Egyptians Americans Greeks Indians Russians Zulus French Aztecs "
          "Chinese English Mongols Celts Japanese Vikings Spanish Persians Carthaginians Sioux").split()
BOX = (330, 140, 560, 440)          # the portrait in the 1024x768 frame
PIC = (365, 160, 535, 420)          # inside it: the picture alone
W, H = BOX[2] - BOX[0], BOX[3] - BOX[1]
cells = []
for t in range(21):
    for g in "mf":
        d = os.path.join(out, "%d-%s" % (t, g))
        if os.path.exists(os.path.join(d, "room-hd.bmp")):
            cells.append((t, g, d))
cols = 4                            # two tribes a row: (HD, original) x (m, f)
sheet = Image.new("RGB", (cols * (W + 8), ((len(cells) + 1) // 2) * (H + 24)), "white")
draw = ImageDraw.Draw(sheet)
for i, (t, g, d) in enumerate(cells):
    hd = Image.open(os.path.join(d, "room-hd.bmp")).convert("RGB").resize((1024, 768), Image.LANCZOS).crop(BOX)
    orig = Image.open(os.path.join(d, "room.bmp")).convert("RGB").crop(BOX)
    # the HD picture should be the original's enlarged: their mean difference small
    a = Image.open(os.path.join(d, "room-hd.bmp")).convert("L").resize((1024, 768), Image.BOX).crop(PIC)
    b = Image.open(os.path.join(d, "room.bmp")).convert("L").crop(PIC)
    diff = sum(ImageStat.Stat(ImageChops.difference(a, b)).mean)
    print("%-14s %s  difference %5.1f%s" % (TRIBES[t], g, diff, "  CHECK" if diff > 20 else ""))
    x = (i % 2) * 2 * (W + 8); y = (i // 2) * (H + 24)
    sheet.paste(hd, (x, y + 20)); sheet.paste(orig, (x + W + 8, y + 20))
    draw.text((x + 4, y + 4), "%s %s: HD | original" % (TRIBES[t], "male" if g == "m" else "female"), fill="black")
sheet.save(os.path.join(out, "contact.png"))
print("wrote", os.path.join(out, "contact.png"), len(cells), "leaders")
