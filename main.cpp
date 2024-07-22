#include <ncursesw/curses.h>

using namespace std;

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    float angle = 0;

    while (true) {
        clear();
        printw("Current angle: %0.1f", angle);

        int x = 

        refresh();
        
        switch (getch()) {
            case KEY_UP:
                angle += 0.5;
                break;
            case KEY_DOWN:
                angle -= 0.5;
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
