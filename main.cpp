#include <cmath>
#include <ncursesw/curses.h>
#include <Eigen/Dense>

using namespace std;

typedef Eigen::Vector2i Point2i;
typedef Eigen::Vector3i Point3i;

void _drawLineX(int x0, int y0, int x1, int y1) {
    int dx  = x1 - x0;
    int dy  = y1 - y0;
    int inc = (dy > 0) ? 1 : -1;
    dy      = abs(dy);
    int D   = 2 * dy - dx;
    for (; x0 <= x1; ++x0) {
        mvaddch(y0, x0, '#');
        if (D > 0) {
            y0 += inc;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void _drawLineY(int x0, int y0, int x1, int y1) {
    int dx  = x1 - x0;
    int dy  = y1 - y0;
    int inc = (dx > 0) ? 1 : -1;
    dx      = abs(dx);
    int D   = 2 * dx - dy;
    for (; y0 <= y1; ++y0) {
        mvaddch(y0, x0, '#');
        if (D > 0) {
            x0 += inc;
            D -= 2 * dy;
        }
        D += 2 * dx;
    }
}

void drawLine(Point2i from, Point2i to) {
    int x0 = from[0];
    int y0 = from[1];
    int x1 = to[0];
    int y1 = to[1];
    if (abs(x0 - x1) > abs(y0 - y1)) {
        mvprintw(1, 0, "drawLineX ");
        if (x1 < x0) {
            printw("with swap");
            swap(x0, x1);
            swap(y0, y1);
        }
        _drawLineX(x0, y0, x1, y1);
    } else {
        mvprintw(1, 0, "drawLineY ");
        if (y1 < y0) {
            printw("with swap");
            swap(x0, x1);
            swap(y0, y1);
        }
        _drawLineY(x0, y0, x1, y1);
    }
}

void dda(Point2i from, Point2i to) {
    int x0 = from[0];
    int y0 = from[1];
    int x1 = to[0];
    int y1 = to[1];
    float dx = x1 - x0;
    float dy = y1 - y0;
    int step = max(abs(dx), abs(dy));
    dx /= step;
    dy /= step;
    float x = x0;
    float y = y0;
    for (int i = 0; i <= step; ++i) {
        mvchgat(y, x, 1, 0, 2, nullptr);
        x += dx;
        y += dy;
    }
}

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    attron(COLOR_PAIR(1));

    int x = COLS / 2;
    int y = LINES / 2;
    Point2i center = {x, y};

    while (true) {
        clear();
        printw("Current position: %i %i", x, y);

        drawLine(center, {x, y});
        dda(center, {x, y});

        refresh();

        switch (getch()) {
            case KEY_UP:
                y--;
                break;
            case KEY_DOWN:
                y++;
                break;
            case KEY_LEFT:
                x--;
                break;
            case KEY_RIGHT:
                x++;
                break;
            case 'q':
                goto end;
            default:
                break;
        }
    }

end:
    endwin();
    return 0;
}
