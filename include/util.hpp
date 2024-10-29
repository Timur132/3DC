#pragma once

#include <string>

static std::string ldtos(long double in) {
    auto out = std::to_string(in);
    while (out.size() != 0) {
        if (out.back() == '0') {
            out.pop_back();
        } else {
            if (out.back() == '.') {
                out.pop_back();
            }
            break;
        }
    }
    return out;
}
