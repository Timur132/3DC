#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>

typedef Eigen::Vector2i Point2i;
typedef Eigen::Vector3i Point3i;

typedef Eigen::Vector3f Color;
struct CharColor {
    Color bg;
    Color fg;
    CharColor(float br, float bg, float bb, float fr, float fg, float fb) {
        this->bg = {br, bg, bb};
        this->fg = {fr, fg, fb};
    }
};
