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

Simulation::Simulation(Length, Length, const std::vector<Marble>& marbles) :
    _marbles(marbles)
{
}

const std::vector<Marble>& Simulation::marbles() const {
    return _marbles;
}

void Simulation::advanceTo(Time t) {
    for(Marble& m: _marbles) {
        m.advanceTo(t);
    }
}

} // Namespace
