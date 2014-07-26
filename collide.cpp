#include "collide.hpp"

#include <boost/make_shared.hpp>


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

Length Marble::r() const {
    return _r;
}

Mass Marble::m() const {
    return _m;
}

Length Marble::x() const {
    return _x0 + _vx * (_t - _t0);
}

Length Marble::y() const {
    return _y0 + _vy * (_t - _t0);
}

Velocity Marble::vx() const {
    return _vx;
}

Velocity Marble::vy() const {
    return _vy;
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

struct Simulation::Event {
    //virtual bool isStillValid(const Simulation&) const = 0;
    virtual void apply(Simulation&) = 0;
    virtual ~Event() {};
};

struct Simulation::RightWallCollision : public Simulation::Event {
    RightWallCollision(Marble& m) :
        _m(m)
    {}

    void apply(Simulation& s) {
        _m.setSpeed(-_m.vx(), _m.vy());
    }

private:
    Marble& _m;
};

Simulation::Simulation(Length w, Length h, const std::vector<Marble>& marbles) :
    _w(w),
    _h(h),
    _marbles(marbles)
{
    for(Marble& m: _marbles) {
        Time timeToRightWall = (_w - m.x() - m.r()) / m.vx();
        _events.insert(std::make_pair(timeToRightWall, boost::make_shared<RightWallCollision>(m)));
    }
}

const std::vector<Marble>& Simulation::marbles() const {
    return _marbles;
}

void Simulation::advanceTo(Time t) {
    while(!_events.empty()) {
        std::pair<Time, boost::shared_ptr<Event>> e = *_events.begin();
        if(e.first < t) {
            advanceMarblesTo(e.first);
            e.second->apply(*this);
            _events.erase(_events.begin());
        } else {
            break;
        }
    }
    advanceMarblesTo(t);
}

void Simulation::advanceMarblesTo(Time t) {
    for(Marble& m: _marbles) {
        m.advanceTo(t);
    }
}

} // Namespace
