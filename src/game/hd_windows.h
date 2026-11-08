#ifndef CIV2_HD_WINDOWS_H
#define CIV2_HD_WINDOWS_H
#include "../platform/hd.h"
void hd_city_panorama(CityViewWindow *);
int hd_throne_room(ThroneRoomWindow *, bool choices);
int hd_spaceship(SpaceshipWindow *);
int hd_military(DiploLeaderWindow *);
void hd_diplomacy_flag(DibSurface *, int color);
void hd_knowledge(DiploLeaderWindow *, int advance, int y, GameFont &);
bool hd_advance(DibSurface *, int advance, int x, int y, int w, int h);
void hd_stockpile(DibSurface *, const RECT &, int value, int capacity, int threshold, const char *role);
void hd_report_background();
void hd_report_row(int y, int height, int row);
#endif
