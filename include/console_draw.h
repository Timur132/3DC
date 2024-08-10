#include "common_types.h"
#include <pdcurses/curses.h>

void start_color_and_pairs();
void set_color(int br, int bg, int bb, int fr, int fg, int fb);
void drawTri(Point2i *in, char ch);
void drawLine(Point2i from, Point2i to, char ch);
