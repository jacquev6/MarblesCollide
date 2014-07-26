#include "collide.hpp"

namespace collide {

Marble::Marble(Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy) :
    _r(r),
    _m(m),
    _x(x),
    _y(y),
    _vx(vx),
    _vy(vy),
    _t(0)
{
}

Length Marble::x() const {
    return _x;
}

Length Marble::y() const {
    return _y;
}

void Marble::advanceTo(Time t) {
    Time dt = t - _t;
    _x += dt * _vx;
    _y += dt * _vy;
    _t = t;
}

std::string hello() {
    return "Hello, World";
}

}
