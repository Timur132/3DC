#include "console_draw.h"
#include <chrono>
#include <pdcurses/curses.h>

using namespace std;

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    curs_set(0);
    start_color_and_pairs();

    Point2i center = {COLS / 2, LINES / 2};
    Point2i points[4];
    points[0]    = center + Point2i{-5, -5};
    points[1]    = center + Point2i{5, -5};
    points[2]    = center + Point2i{-5, 5};
    points[3]    = center + Point2i{5, 5};
    int cur      = 0;
    int ch       = '#';
    int color[3] = {1000, 1000, 1000};

    while (true) {
        clear();
        set_color(0, 0, 0, 1000, 0, 0);
        printw(" %i", color[0]);
        set_color(0, 0, 0, 0, 1000, 0);
        printw(" %i", color[1]);
        set_color(0, 0, 0, 0, 0, 1000);
        printw(" %i", color[2]);
        for (int i = 0; i < 4; ++i) {
            set_color(0, 0, 0, 1000, 1000, 1000);
            addch(' ');
            if (i == cur) {
                // set_color(1000, 1000, 1000, 0, 0, 0);
                attron(A_UNDERLINE);
            }
            printw("(%i %i)", points[i][0], points[i][1]);
            attroff(A_UNDERLINE);
        }
        set_color(0, 0, 0, color[0], color[1], color[2]);
        auto start = chrono::high_resolution_clock::now();
        drawTri(points, ch);
        drawTri(points + 1, ch);
        set_color(0, 0, 0, 1000, 1000, 1000);
        mvprintw(1, 0, "Time spent: %lliu",
                 chrono::duration_cast<chrono::microseconds>(
                     chrono::high_resolution_clock::now() - start)
                     .count());
        refresh();

        auto input = getch();
        switch (input) {
            case 60419:
                points[cur][1]--;
                break;
            case 60418:
                points[cur][1]++;
                break;
            case 60420:
                points[cur][0]--;
                break;
            case 60421:
                points[cur][0]++;
                break;
            case ' ':
                cur = (cur + 1) % 4;
                break;
            case 'q':
                goto end;
            case 'j':
                color[0] -= 100;
                break;
            case 'u':
                color[0] += 100;
                break;
            case 'k':
                color[1] -= 100;
                break;
            case 'i':
                color[1] += 100;
                break;
            case 'l':
                color[2] -= 100;
                break;
            case 'o':
                color[2] += 100;
                break;
            default:
                ch = input;
                break;
        }
    }

end:
    return 0;
}
