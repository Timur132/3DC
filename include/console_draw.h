#include "common_types.h"
#include <pdcurses/curses.h>
#include <vector>
#include <string>

#define COLOR_DEPTH 9
#define USED_COLORS (COLOR_DEPTH * COLOR_DEPTH * COLOR_DEPTH)
#define COLOR_STEP (1000 / (COLOR_DEPTH - 1))

void start_color_and_pairs();
int color_to_pair(CharColor color);
attr_t color_to_attr(CharColor color);

class ScreenBuffer {
    std::vector<std::basic_string<chtype>> buf;
    int width  = 0;
    int height = 0;
    attr_t attr = 0;
    int colorPair = 0;

public:
    ScreenBuffer() = default;
    ScreenBuffer(int width, int height);

    void resize(int width, int height);
    void clear();
    void print(int x = 0, int y = 0);
    void put(int x, int y, chtype ch);
    void put(int x, int y, chtype ch, attr_t attr);
    void put(int x, int y, chtype ch, CharColor color, attr_t attr);
    void set_color(CharColor color);
    void set_attr(attr_t attr);
    void draw_tri(Point2i* in, char ch);
    void draw_line(Point2i from, Point2i to, char ch);
};
