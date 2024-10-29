#include "console_draw.h"
#include <pdcurses/curses.h>

void print_matrix(Eigen::Matrix4f mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printw("%.3f ", mat(i, j));
        }
        addch('\n');
    }
}

int color_to_pair(CharColor color) {
    int br         = int(color.bg[0] * 1000) / COLOR_STEP;
    int bg         = int(color.bg[1] * 1000) / COLOR_STEP;
    int bb         = int(color.bg[2] * 1000) / COLOR_STEP;
    int fr         = int(color.fg[0] * 1000) / COLOR_STEP;
    int fg         = int(color.fg[1] * 1000) / COLOR_STEP;
    int fb         = int(color.fg[2] * 1000) / COLOR_STEP;
    int background = br * COLOR_DEPTH * COLOR_DEPTH + bg * COLOR_DEPTH + bb;
    int foreground = fr * COLOR_DEPTH * COLOR_DEPTH + fg * COLOR_DEPTH + fb;
    return background * USED_COLORS + foreground + 1;
}

attr_t color_to_attr(CharColor color) {
    return COLOR_PAIR(color_to_pair(color));
}

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
    attron(color_to_attr({0, 0, 0, 1, 1, 1}));
}

ScreenBuffer::ScreenBuffer(int width, int height) { resize(width, height); }

void ScreenBuffer::resize(int width, int height) {
    for (auto& i : buf) {
        i.resize(width, ' ');
    }
    buf.resize(height, std::basic_string<chtype>(width, ' '));
    this->height = height;
    this->width  = width;
}

void ScreenBuffer::clear() {
    buf.clear();
    buf.resize(height, std::basic_string<chtype>(width, ' '));
}

void ScreenBuffer::print(int x, int y) {
    for (int i = 0; i < height; ++i) {
        mvaddchstr(i + y, x, buf[i].c_str());
    }
}

void ScreenBuffer::set_attr(attr_t attr) { this->attr = attr; }

void ScreenBuffer::set_color(CharColor color) {
    colorPair = color_to_pair(color);
}

void ScreenBuffer::put(int x, int y, chtype ch) {
    put(x, y, ch, attr | COLOR_PAIR(colorPair));
}

void ScreenBuffer::put(int x, int y, chtype ch, CharColor color, attr_t attr) {
    put(x, y, ch, attr | COLOR_PAIR(color_to_pair(color)));
}

void ScreenBuffer::put(int x, int y, chtype ch, attr_t attr) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    buf[y][x] = ch | attr;
}

void ScreenBuffer::draw_line(Point2i from, Point2i to, char ch) {
    int x0 = from[0];
    int y0 = from[1];
    int x1 = to[0];
    int y1 = to[1];
    if (abs(x0 - x1) > abs(y0 - y1)) {
        if (x1 < x0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx  = x1 - x0;
        int dy  = y1 - y0;
        int inc = (dy > 0) ? 1 : -1;
        dy      = abs(dy);
        int D   = 2 * dy - dx;
        for (; x0 <= x1; ++x0) {
            put(x0, y0, ch);
            if (D > 0) {
                y0 += inc;
                D -= 2 * dx;
            }
            D += 2 * dy;
        }
    } else {
        if (y1 < y0) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx  = x1 - x0;
        int dy  = y1 - y0;
        int inc = (dx > 0) ? 1 : -1;
        dx      = abs(dx);
        int D   = 2 * dx - dy;
        for (; y0 <= y1; ++y0) {
            put(x0, y0, ch);
            if (D > 0) {
                x0 += inc;
                D -= 2 * dy;
            }
            D += 2 * dx;
        }
    }
}

void ScreenBuffer::draw_tri(Point2i a, Point2i b, Point2i c, char ch) {
    draw_line(a, b, ch);
    draw_line(a, c, ch);
    draw_line(b, c, ch);
}
