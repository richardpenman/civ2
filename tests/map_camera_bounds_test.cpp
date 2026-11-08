// SOURCE src/game/map_camera.cpp horizontal_center
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
enum { RULE_FLAT_WORLD=1 };
static struct { int rules; } game;
static struct { int width; } map;
static double clamp(double v,double lo,double hi) { return v<lo ? lo : v>hi ? hi : v; }
#include "production.inc"
int main()
{
    for(int world: {20,80,150,256})for(int width: {320,740,1920})
    for(double scale: {.125,.1653125,.625,1.0,2.0}) {
        map.width=world;
        for(int flat=0;flat<2;flat++)for(double center: {-1000.,0.,37.25,100.,1000.}) {
            game.rules=flat;
            double x=horizontal_center(center,scale,width),half=width/(64*scale);
            if(!flat)assert(x==center); // No seam clamp, even multiple revolutions away.
            else if(2*half>=world+1)assert(x==(world+1)*.5);
            else {
                assert(x-half>=-1e-9 && x+half<=world+1+1e-9);
                if(center>=half && center<=world+1-half)assert(x==center);
            }
        }
    }
    puts("Camera bounds: wrapped panning stays free; fixed worlds clamp at terrain edges or center when fully visible.");
}
