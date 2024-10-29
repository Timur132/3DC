#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <pdcurses/curses.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "util.hpp"

class ParamInterface {
protected:
    std::string name;
    ParamInterface() {}

public:
    virtual ~ParamInterface() {}
    virtual void print(WINDOW* win) {}
    virtual void input(int ch) {}
};

template <typename T>
class Param : public ParamInterface {
protected:
    T val;

public:
    Param<T>(std::string name) {
        this->name = name;
        this->val  = T{};
    }
    Param<T>(std::string name, T def) {
        this->name = name;
        this->val        = def;
    }
    virtual T operator()() { return val; }
};

template <typename T = int>
class IntParam : public Param<T> {
public:
    using Param<T>::Param;
    void print(WINDOW* win) override {
        wprintw(win, "%s: %lld", this->name.c_str(), this->val);
    }
    void input(int ch) override {
        if (ch == 8) {
            this->val /= 10;
        } else if (ch == '-') {
            this->val *= -1;
        } else if (ch >= '0' && ch <= '9') {
            this->val = this->val * 10 + ch - '0';
        } else if (ch == '[') {
            this->val--;
        } else if (ch == ']') {
            this->val++;
        }
    }
};

static void float_string_editor(std::string& str, bool& dot, int ch) {
    if (ch == 8) {
        if (str.back() == '.') {
            dot = false;
        }
        str.pop_back();
        if (str.size() == 0 || (str.size() == 1 && str[0] == '-')) {
            str.push_back('0');
        }
    } else if (ch == '.' || ch == ',') {
        if (!dot) {
            str.push_back('.');
            dot = true;
        }
    } else if (ch == '-') {
        if (str[0] == '-') {
            str.erase(0, 1);
        } else {
            str.insert(str.begin(), '-');
        }
    } else if (ch >= '0' && ch <= '9') {
        if ((str.size() == 1 || (str.size() == 2 && str[0] == '-')) && str.back() == '0') {
            str.pop_back();
        }
        str.push_back(ch);
    } else if (ch == ']') {
        int carry = 1;
        for (int i = str.size() - 1; i >= 0 && carry > 0; --i) {
            if (!isdigit(str[i])) {
                continue;
            }
            carry = (str[i] == '9') ? 1 : 0;
            str[i] = ((str[i] - '0' + 1) % 10) + '0';
        }
        if (carry == 1) {
            str.insert(str[0] == '-' ? 1 : 0, "1");
        }
    }
}

template <typename T = float>
class FloatParam : public Param<T> {
    std::string str = "0";
    bool dot        = false;

public:
    FloatParam<T>(std::string name, T def = 0) : Param<T>(name, def) {
        str = ldtos(def);
        dot = float(int(def)) != def;
    }

    void print(WINDOW* win) override {
        wprintw(win, "%s: %s", this->name.c_str(), str.c_str());
    }
    void input(int ch) override {
        float_string_editor(str, dot, ch);
        this->val = std::stold(str);
    }
};

template <int Rows, int Cols, typename Scalar = float>
class MatrixParam : public Param<Eigen::Matrix<Scalar, Rows, Cols>> {
    using T      = Eigen::Matrix<Scalar, Rows, Cols>;
    int selected = 0;
    int width    = 1;
    std::array<std::string, Rows * Cols> arr;
    std::array<bool, Rows * Cols> dots;

public:
    MatrixParam<Rows, Cols, Scalar>(std::string name) : Param<T>(name) {
        arr.fill("0");
        dots.fill(false);
    }
    MatrixParam<Rows, Cols, Scalar>(std::string name, T def)
        : Param<T>(name, def) {
        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                arr[i * Cols + j]  = ldtos(def(i, j));
                dots[i * Cols + j] = float(int(def(i, j))) != def(i, j);
                width = std::max(width, int(arr[i * Cols + j].size()));
            }
        }
    }

    void print(WINDOW* win) override {
        std::string nameOffset(this->name.size() + 3, ' ');
        for (int i = 0; i < Rows; ++i) {
            if (i == 0) {
                wprintw(win, "%s: ", this->name.c_str());
            } else {
                wprintw(win, "\n%s", nameOffset.c_str());
            }
            for (int j = 0; j < Cols; ++j) {
                int cur = i * Cols + j;
                if (cur == selected) {
                    wattron(win, A_UNDERLINE);
                }
                wprintw(win, "%s", arr[cur].c_str());
                if (cur == selected) {
                    wattroff(win, A_UNDERLINE);
                }
                for (int k = width - arr[cur].size(); k >= 0; --k) {
                    waddch(win, ' ');
                }
            }
        }
    }
    void input(int ch) override {
        if (ch == 'h') {
            selected = std::max(0, selected - 1);
        } else if (ch == 'l') {
            selected = std::min(Cols * Rows - 1, selected + 1);
        } else {
            float_string_editor(arr[selected], dots[selected], ch);
            width = std::max(width, int(arr[selected].size()));
            this->val(selected / Cols, selected % Cols) = stold(arr[selected]);
        }
    }
};

class ParamMenu {
    WINDOW* win = nullptr;
    int startx  = 0;
    int starty  = 0;
    int width   = COLS;
    int height  = LINES;
    std::vector<ParamInterface*> params;
    std::unordered_map<std::string, ParamInterface*> namedParams;
    int scroll   = 0;
    int selected = 0;
    int bottom;

public:
    ParamMenu() { replace(0, 0, COLS, LINES); }
    ParamMenu(int x, int y, int width, int height) {
        replace(x, y, width, height);
    }

    ~ParamMenu() {
        for (auto i : params) {
            delete i;
        }
    }

    bool active = true;
    bool pause  = false;

    WINDOW* get_win() { return win; }
    void replace(int x, int y, int width, int height) {
        if (win != nullptr) {
            delwin(win);
        }
        win = newwin(height, width, y, x);

        startx       = x;
        starty       = y;
        this->width  = width;
        this->height = height;
        bottom       = height;
    }
    void add_param(ParamInterface* param) { params.push_back(param); }
    template <typename T, typename... Types>
    auto emplace_param(std::string name, Types... args) {
        T* cur;
        if (namedParams.count(name) == 0) {
            cur = new T(name, args...);
            params.push_back(reinterpret_cast<ParamInterface*>(cur));
            namedParams[name] = reinterpret_cast<ParamInterface*>(cur);
        } else {
            cur = reinterpret_cast<T*>(namedParams[name]);
        }
        return (*cur)();
    }
    void process() {
        do {
            int n = params.size();
            if (!active || n == 0 || win == nullptr) {
                return;
            }

            auto in = getch();
            switch (in & 0xFF | (in > 0xFF ? 0x100 : 0)) {
                case 'j':
                    selected = std::min(selected + 1, n - 1);
                    if (selected > bottom) {
                        scroll = std::min(scroll + 1, n - 1);
                    }
                    break;
                case 'k':
                    selected = std::max(selected - 1, 0);
                    scroll   = std::min(scroll, selected);
                    break;
                case KEY_F(3):
                    active = false;
                    break;
                case ' ':
                    pause = !pause;
                    break;
                default:
                    params[selected]->input(in);
                    break;
            }

            if (pause) {
                draw();
            }
        } while (pause);
    }

    void draw() {
        if (!active || win == nullptr) {
            return;
        }

        int n = params.size();
        int y = 0;
        int x = 0;
        wclear(win);
        std::string title = pause ? "DEBUG MENU (PAUSED)" : "DEBUG MENU";
        mvwprintw(win, 0, 0, "%s", std::string(width, '=').c_str());
        mvwprintw(win, 0, (width - title.size()) / 2, "%s", title.c_str());
        wmove(win, 1, 0);
        for (auto i = scroll; i < n && y < height; ++i) {
            if (i == selected) {
                wprintw(win, ">");
            } else {
                wprintw(win, " ");
            }

            params[i]->print(win);
            wprintw(win, "\n");

            bottom = i;
            getyx(win, y, x);
        }
        wrefresh(win);
    }
};
