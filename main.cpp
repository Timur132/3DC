#include "common_types.h"
#include "console_draw.h"
#include "matrices.hpp"
#include "param_menu.hpp"
#include <Eigen/src/Core/Matrix.h>
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

    Point3f orgPoints[4] = {{0, -1, 2}, {2, -1, -2}, {-2, -1, -2}, {0, 0, 0}};
    Point3f points[4];
    size_t faces[4][3] = {{0, 1, 2}, {0, 1, 3}, {0, 2, 3}, {1, 2, 3}};

    ParamMenu pm(COLS * 2 / 3, 0, COLS / 3, LINES);
    wattrset(pm.get_win(), color_to_attr({0, 0, 0, 1, 1, 1}));

    MatrixParam<1, 4> order("order", {1, 2, 3, 4});
    pm.add_param(&order);

    int ch      = '#';
    Color color = {1, 1, 1};
    ScreenBuffer buf(COLS, LINES - 1);
    buf.set_attr(A_ITALIC);

    while (true) {
        for (int i = 0; i < 4; ++i) {
            points[i] = orgPoints[i];
            for (int j = 0; j < 4; ++j) {
                auto mat = Eigen::Matrix4f::Identity().eval();
                switch (int(order()(j))) {
                    case 1:
                        mat = scale_matrix(
                            pm.emplace_param<FloatParam<>>("scale_x", 1),
                            pm.emplace_param<FloatParam<>>("scale_y", 1),
                            pm.emplace_param<FloatParam<>>("scale_z", 1));
                        break;
                    case 2:
                        mat = rotation_matrix(
                            pm.emplace_param<FloatParam<>>("rot_x"),
                            pm.emplace_param<FloatParam<>>("rot_y"),
                            pm.emplace_param<FloatParam<>>("rot_z"));
                        break;
                    case 3:
                        mat = look_at_camera_matrix(
                            pm.emplace_param<MatrixParam<1, 3>>("camera", Point3f{0, 0, -1}),
                            pm.emplace_param<MatrixParam<1, 3>>("target"),
                            {0, 1, 0});
                        break;
                    case 4:
                        mat = horizontal_fov_projection_matrix(
                            pm.emplace_param<FloatParam<>>("fov", 60),
                            pm.emplace_param<FloatParam<>>("aspect_ratio", 1),
                            pm.emplace_param<FloatParam<>>("close", 0.5),
                            pm.emplace_param<FloatParam<>>("far", 100));
                        break;
                    default:
                        break;
                }
                points[i] = m4_cross_v3(mat, points[i]);
            }
            points[i] += Point3f{float(COLS) / 2, float(LINES) / 2, 0};
        }

        clear();
        attron(color_to_attr({0, 0, 0, 1, 0, 0}));
        printw(" %.3f", color[0]);
        attron(color_to_attr({0, 0, 0, 0, 1, 0}));
        printw(" %.3f", color[1]);
        attron(color_to_attr({0, 0, 0, 0, 0, 1}));
        printw(" %.3f", color[2]);
        for (int i = 0; i < 4; ++i) {
            attron(color_to_attr({0, 0, 0, 1, 1, 1}));
            addch(' ');
            printw("(%.3f %.3f %.3f)", points[i][0], points[i][1],
                   points[i][2]);
            attroff(A_UNDERLINE);
        }
        auto start = chrono::high_resolution_clock::now();
        buf.clear();
        buf.set_color({0, 0, 0, color[0], color[1], color[2]});
        for (auto i : faces) {
            buf.draw_tri({round(points[i[0]][0]), round(points[i[0]][1])},
                         {round(points[i[1]][0]), round(points[i[1]][1])},
                         {round(points[i[2]][0]), round(points[i[2]][1])}, ch);
        }
        buf.print(0, 1);
        attron(color_to_attr({0, 0, 0, 1, 1, 1}));
        mvprintw(1, 0, "Time spent: %lliu",
                 chrono::duration_cast<chrono::microseconds>(
                     chrono::high_resolution_clock::now() - start)
                     .count());

        refresh();
        pm.draw();

        if (pm.active) {
            pm.process();
        } else {
            auto input = getch();
            switch (input & 0xFF | (input > 0xFF ? 0x100 : 0)) {
                case KEY_F(3):
                    pm.active = true;
                    break;
                case 'q':
                    goto end;
                case 'j':
                    color[0] -= 0.125;
                    break;
                case 'u':
                    color[0] += 0.125;
                    break;
                case 'k':
                    color[1] -= 0.125;
                    break;
                case 'i':
                    color[1] += 0.125;
                    break;
                case 'l':
                    color[2] -= 0.125;
                    break;
                case 'o':
                    color[2] += 0.125;
                    break;
                default:
                    ch = input;
                    break;
            }
        }
    }

end:
    return 0;
}
