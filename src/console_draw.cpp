#include "console_draw.h"

#define COLOR_DEPTH 8
#define USED_COLORS (COLOR_DEPTH * COLOR_DEPTH * COLOR_DEPTH)
#define COLOR_STEP (1000 / (COLOR_DEPTH - 1))

void start_color_and_pairs() {
    start_color();
    for (int r = 0; r < COLOR_DEPTH; ++r) {
        for (int g = 0; g < COLOR_DEPTH; ++g) {
            for (int b = 0; b < COLOR_DEPTH; ++b) {
                init_extended_color(
                    r * COLOR_DEPTH * COLOR_DEPTH + g * COLOR_DEPTH + b,
                    r * COLOR_STEP, g * COLOR_STEP, b * COLOR_STEP);
            }
        }
    }
    for (int i = 0; i < USED_COLORS; ++i) {
        for (int j = 0; j < USED_COLORS; ++j) {
            init_extended_pair(j * USED_COLORS + i + 1, i, j);
        }
    }
}

void set_color(int br, int bg, int bb, int fr, int fg, int fb) {
    br /= COLOR_STEP;
    bg /= COLOR_STEP;
    bb /= COLOR_STEP;
    fr /= COLOR_STEP;
    fg /= COLOR_STEP;
    fb /= COLOR_STEP;
    int background = br * COLOR_DEPTH * COLOR_DEPTH +
                     bg * COLOR_DEPTH + bb;
    int foreground = fr * COLOR_DEPTH * COLOR_DEPTH +
                     fg * COLOR_DEPTH + fb;
    attron(COLOR_PAIR(background * USED_COLORS + foreground + 1));
}

void _drawLineX(int x0, int y0, int x1, int y1, char ch) {
    int dx  = x1 - x0;
    int dy  = y1 - y0;
    int inc = (dy > 0) ? 1 : -1;
    dy      = abs(dy);
    int D   = 2 * dy - dx;
    for (; x0 <= x1; ++x0) {
        mvaddch(y0, x0, ch);
        if (D > 0) {
            y0 += inc;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void _drawLineY(int x0, int y0, int x1, int y1, char ch) {
    int dx  = x1 - x0;
    int dy  = y1 - y0;
    int inc = (dx > 0) ? 1 : -1;
    dx      = abs(dx);
    int D   = 2 * dx - dy;
    for (; y0 <= y1; ++y0) {
        mvaddch(y0, x0, ch);
        if (D > 0) {
            x0 += inc;
            D -= 2 * dy;
        }
        D += 2 * dx;
    }
}

void drawLine(Point2i from, Point2i to, char ch) {
    int x0 = from[0];
    int y0 = from[1];
    int x1 = to[0];
    int y1 = to[1];
    if (abs(x0 - x1) > abs(y0 - y1)) {
        if (x1 < x0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        _drawLineX(x0, y0, x1, y1, ch);
    } else {
        if (y1 < y0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        _drawLineY(x0, y0, x1, y1, ch);
    }
}

void drawTri(Point2i* in, char ch) {
    drawLine(in[0], in[1], ch);
    drawLine(in[1], in[2], ch);
    drawLine(in[2], in[0], ch);
}
