// SOURCE src/game/lang.cpp own_name
// OpenCiv2's name for the game in the game's texts (plat_own_name, normal
// play): "Civilization II", "Civ II", "Civ2", a translation's "文明帝國II" ...
// become "OpenCiv2"; a line naming a CD, the original game or credits keeps
// the original's name (it is the original product); a longer name that would
// not fit the buffer is left; words that only start like the name are kept.
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <string>

#include "production.inc"

static std::string named(const char *in, size_t size = 0x100)
{
    char buf[0x200];
    snprintf(buf, sizeof buf, "%s", in);
    own_name(buf, size);
    return buf;
}

int main()
{
    // the install's texts (GAME.TXT, LABELS.TXT, MENU.TXT, DEBUG.TXT, HELP.TXT)
    assert(named("@title=Civilization II Multiplayer Gold") == "@title=OpenCiv2");
    assert(named("@title=Civilization II Multiplayer Gold %STRING0") == "@title=OpenCiv2 %STRING0");
    assert(named("Civilization II Gold") == "OpenCiv2");
    assert(named("Civilization II Map Editor") == "OpenCiv2 Map Editor");
    assert(named("CIVILIZATION II: HALL OF FAME") == "OPENCIV2: HALL OF FAME");
    assert(named("&About Civilization II") == "&About OpenCiv2");
    assert(named("@title=Civ2 Rules: Palace") == "@title=OpenCiv2 Rules: Palace");
    assert(named("^less than zero, Civ II forces it to zero.") == "^less than zero, OpenCiv2 forces it to zero.");
    assert(named("^surrounded by Forests, CIV II would use orientation #16.") ==
           "^surrounded by Forests, OpenCiv2 would use orientation #16.");
    assert(named("search the Civ 2 sound folder.") == "search the OpenCiv2 sound folder.");
    // the original product: its CD, its manual, its credits
    assert(named("^^Civilization II CD, Civilization II Multiplayer Gold CD,") ==
           "^^Civilization II CD, Civilization II Multiplayer Gold CD,");
    assert(named("^Editor. A good explanation is in the original Civ II game manual.") ==
           "^Editor. A good explanation is in the original Civ II game manual.");
    assert(named("View Civ II Credits") == "View Civ II Credits");
    assert(named("^Diras al la ludo ludi sonan trakon. Sur \xe6iuj KD-oj de Civ II la unua") ==
           "^Diras al la ludo ludi sonan trakon. Sur \xe6iuj KD-oj de Civ II la unua");
    // not the name: other words, file names, OpenCiv2 itself
    assert(named("Civilization III") == "Civilization III");
    assert(named("Civ2Art.dll, Civ2Map.exe, Civ2.ini") == "Civ2Art.dll, Civ2Map.exe, Civ2.ini");
    assert(named("OpenCiv2 Map Editor") == "OpenCiv2 Map Editor");
    assert(named("Civilization Advances") == "Civilization Advances");
    // translations (UTF-8): Traditional Chinese, Greek
    assert(named("\xe6\x96\x87\xe6\x98\x8e\xe5\xb8\x9d\xe5\x9c\x8bII \xe5\xa4\x9a\xe4\xba\xba\xe9\x81\x8a\xe6\x88\xb2"
                 "\xe9\xbb\x83\xe9\x87\x91\xe7\x89\x88") == "OpenCiv2");
    assert(named("\xe6\x96\x87\xe6\x98\x8e\xe5\xb8\x9d\xe5\x9c\x8bII \xe5\x9c\xb0\xe5\x9c\x96") == "OpenCiv2 \xe5\x9c\xb0\xe5\x9c\x96");
    assert(named("\xce\xa4\xce\xbf Civilization II") == "\xce\xa4\xce\xbf OpenCiv2");
    // a longer name that would not fit is left (size: the buffer's)
    assert(named("Civ2 Rules", 11) == "Civ2 Rules");
    assert(named("Civ2 Rules", 15) == "OpenCiv2 Rules");
    std::puts("own_name: ok");
    return 0;
}
