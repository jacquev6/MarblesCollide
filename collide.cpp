#include "collide.hpp"

namespace collide {

Marble::Marble(Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy) :
    _r(r),
    _m(m),
    _x0(x),
    _y0(y),
    _vx(vx),
    _vy(vy),
    _t0(0),
    _t(0)
{
}

Length Marble::x() const {
    return _x0 + _vx * (_t - _t0);
}

Length Marble::y() const {
    return _y0 + _vy * (_t - _t0);
}

void Marble::advanceTo(Time t) {
    assert(t >= _t);
    _t = t;
}

void Marble::setSpeed(Velocity vx, Velocity vy) {
    _x0 = x();
    _y0 = y();
    _t0 = _t;
    _vx = vx;
    _vy = vy;
}

std::string hello() {
    return "Hello, World";
}

}
