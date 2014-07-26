#include "collide.hpp"

#include <boost/make_shared.hpp>

using namespace bu;

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

Marble::Ptr Marble::create(Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy) {
    return Ptr(new Marble(r, m, x, y, vx, vy));
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

Time Marble::t0() const {
    return _t0;
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

struct Simulation::WallCollision : public Simulation::Event {
    WallCollision(Marble::Ptr m, bool fx, bool fy) :
        _m(m),
        _t0(m->t0()),
        _fx(fx),
        _fy(fy)
    {}

    void apply(Simulation& s) {
        if(_m->t0() == _t0) {
            _m->setSpeed(_fx ? -_m->vx() : _m->vx(), _fy ? -_m->vy() : _m->vy());
            s.scheduleNextEventsFor(_m);
        }
    }

private:
    Marble::Ptr _m;
    Time _t0;
    bool _fx;
    bool _fy;
};

struct Simulation::Tick : public Simulation::Event {
    void apply(Simulation& s) {
        s._eventsHandler->tick();
    }
};


class NullEventsHandler : public EventsHandler {
    void begin(Simulation*) {}
    void tick() {}
};

boost::shared_ptr<EventsHandler> Simulation::nullEventsHandler = boost::make_shared<NullEventsHandler>();

Simulation::Simulation(Length w, Length h, const std::vector<Marble::Ptr>& marbles, boost::shared_ptr<EventsHandler> eventsHandler) :
    _t(0),
    _w(w),
    _h(h),
    _marbles(marbles),
    _eventsHandler(eventsHandler)
{
    for(Marble::Ptr m: _marbles) {
        scheduleNextEventsFor(m);
    }
    _eventsHandler->begin(this);
}

Simulation::Ptr Simulation::create(Length w, Length h, const std::vector<Marble::Ptr>& marbles, boost::shared_ptr<EventsHandler> eventsHandler) {
    return Ptr(new Simulation(w, h, marbles, eventsHandler));
}

Length Simulation::width() const {
    return _w;
}

Length Simulation::height() const {
    return _h;
}

const std::vector<Marble::Ptr>& Simulation::marbles() const {
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
    for(Marble::Ptr m: _marbles) {
        m->advanceTo(t);
    }
    _t = t;
}

void Simulation::scheduleNextEventsFor(Marble::Ptr m) {
    if(m->vx() > 0 * meter_per_second) {
        Time timeToWall = (_w - m->x() - m->r()) / m->vx();
        scheduleEventIn(timeToWall, boost::make_shared<WallCollision>(m, true, false));
    }
    if(m->vx() < 0 * meter_per_second) {
        Time timeToWall = (m->r() - m->x()) / m->vx();
        scheduleEventIn(timeToWall, boost::make_shared<WallCollision>(m, true, false));
    }
    if(m->vy() > 0 * meter_per_second) {
        Time timeToWall = (_h - m->y() - m->r()) / m->vy();
        scheduleEventIn(timeToWall, boost::make_shared<WallCollision>(m, false, true));
    }
    if(m->vy() < 0 * meter_per_second) {
        Time timeToWall = (m->r() - m->y()) / m->vy();
        scheduleEventIn(timeToWall, boost::make_shared<WallCollision>(m, false, true));
    }
}

void Simulation::scheduleEventIn(Time dt, boost::shared_ptr<Event> e) {
    _events.insert(std::make_pair(_t + dt, e));
}

void Simulation::scheduleTickIn(Time dt) {
    scheduleEventIn(dt, boost::make_shared<Tick>());
}

} // Namespace
