#include "collide.hpp"

#include <boost/make_shared.hpp>
#include <boost/units/cmath.hpp>

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


struct Simulation::MarblesCollision : public Simulation::Event {
    MarblesCollision(Marble::Ptr m1, Marble::Ptr m2) :
        _m1(m1),
        _m2(m2),
        _t01(m1->t0()),
        _t02(m2->t0())
    {}

    void apply(Simulation& s) {
        if(_m1->t0() == _t01 && _m2->t0() == _t02) {
            // "All models are wrong, some are useful" http://en.wikiquote.org/wiki/George_E._P._Box#Empirical_Model-Building_and_Response_Surfaces_.281987.29
            // So we use the model of a perfect elastic collision, neglecting energy dissipation, spin, etc.
            // - total energy is unchanged: m1 * |v1|² + m2 * |v2|² = const
            // - total movement is unchanged: m1 * v1 + m2 * v2 = const
            // - force impulse is aligned with the centers, so there is no change on the composents of velocity normal to this vector

            // Normal vector
            auto nx = (_m2->x() - _m1->x()) / sqrt((_m2->x() - _m1->x()) * (_m2->x() - _m1->x()) + (_m2->y() - _m1->y()) * (_m2->y() - _m1->y()));
            auto ny = (_m2->y() - _m1->y()) / sqrt((_m2->x() - _m1->x()) * (_m2->x() - _m1->x()) + (_m2->y() - _m1->y()) * (_m2->y() - _m1->y()));

            // Vrel
            Velocity vrelx = ((_m2->vx() - _m1->vx()) * nx + (_m2->vy() - _m1->vy()) * ny) * nx;
            Velocity vrely = ((_m2->vx() - _m1->vx()) * nx + (_m2->vy() - _m1->vy()) * ny) * ny;

            Velocity v1x = _m1->vx() + _m2->m() / (_m1->m() + _m2->m()) * vrelx + _m2->m() / (_m1->m() + _m2->m()) * vrelx;
            Velocity v1y = _m1->vy() + _m2->m() / (_m1->m() + _m2->m()) * vrely + _m2->m() / (_m1->m() + _m2->m()) * vrely;

            Velocity v2x = _m2->vx() - _m1->m() / (_m1->m() + _m2->m()) * vrelx - _m1->m() / (_m1->m() + _m2->m()) * vrelx;
            Velocity v2y = _m2->vy() - _m1->m() / (_m1->m() + _m2->m()) * vrely - _m1->m() / (_m1->m() + _m2->m()) * vrely;

            _m1->setSpeed(v1x, v1y);
            _m2->setSpeed(v2x, v2y);
            s.scheduleNextEventsFor(_m1);
            s.scheduleNextEventsFor(_m2);
        }
    }

private:
    Marble::Ptr _m1;
    Marble::Ptr _m2;
    Time _t01;
    Time _t02;
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
        scheduleNextCollisionsWithWalls(m);
        for(Marble::Ptr m2: _marbles) {
            if(m2.get() > m.get()) {
                scheduleNextCollisionBetween(m, m2);
            }
        }
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
    scheduleNextCollisionsWithWalls(m);
    scheduleNextCollisionsWithOtherMarbles(m);
}

void Simulation::scheduleNextCollisionsWithWalls(Marble::Ptr m) {
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

void Simulation::scheduleNextCollisionsWithOtherMarbles(Marble::Ptr m) {
    for(auto m2: _marbles) {
        if(m2 != m) {
            scheduleNextCollisionBetween(m, m2);
        }
    }
}

void Simulation::scheduleNextCollisionBetween(Marble::Ptr m1, Marble::Ptr m2) {
    // m1(r1, x1, y1, vx1, vy1) collides with m2(r2, x2, y2, vx2, vy2)?
    // x1(t) = x1 + vx1 * t
    // y1(t) = y1 + vy1 * t
    // x2(t) = x2 + vx2 * t
    // y2(t) = y2 + vy2 * t
    // Collision <=> dist((x1(t), y1(t)), (x2(t), y2(t))) == r1 + r2
    // (to be solved for t)
    // <=> (x1(t) - x2(t))² + (y1(t) - y2(t))² == (r1 + r2)²
    // <=> (x1 + vx1 * t - x2 - vx2 * t) ² + (y1 + vy1 * t - y2 - vy2 * t)² == (r1 + r2)²
    // <=> (x1 - x2 + (vx1 - vx2) * t)² + (y1 - y2 + (vy1 - vy2) * t)² == (r1 + r2)²
    // <=> (x1 - x2)² + 2 * (x1 - x2) * (vx1 - vx2) * t + (vx1 - vx2)² * t² + (y1 - y2)² + 2 * (y1 - y2) * (vy1 - vy2) * t + (vy1 - vy2)² * t² == (r1 + r2)²
    // <=> ((x1 - x2)² + (y1 - y2)² - (r1 + r2)²) + 2 * ((x1 - x2) * (vx1 - vx2) + (y1 - y2) * (vy1 - vy2)) * t + ((vx1 - vx2)² + (vy1 - vy2)²) * t² == 0
    // Let a = (vx1 - vx2)² + (vy1 - vy2)²
    //     b = (x1 - x2) * (vx1 - vx2) + (y1 - y2) * (vy1 - vy2)
    //     c = (x1 - x2)² + (y1 - y2)² - (r1 + r2)²
    // Collision <=> a * t² + 2 * b * t + c == 0
    auto a = (m1->vx() - m2->vx()) * (m1->vx() - m2->vx()) + (m1->vy() - m2->vy()) * (m1->vy() - m2->vy());
    auto b = (m1->x() - m2->x()) * (m1->vx() - m2->vx()) + (m1->y() - m2->y()) * (m1->vy() - m2->vy());
    auto c = (m1->x() - m2->x()) * (m1->x() - m2->x()) + (m1->y() - m2->y()) * (m1->y() - m2->y()) - (m1->r() + m2->r()) * (m1->r() + m2->r());
    if(a == 0 * meter * meter / second / second) {
        // @todo When does it degenerate to a linear equation? Do we need to handle this case?
    } else {
        auto delta = b * b - a * c;
        if(delta >= 0 * meter * meter * meter * meter / second / second) {
            Time t1 = (sqrt(delta) - b) / a;
            Time t2 = (-sqrt(delta) - b) / a;
            Time t = std::min(t1, t2);
            if(t <= 0 * second) {
                t = std::max(t1, t2);
            }
            if(t > 0 * second) {
                scheduleEventIn(t, boost::make_shared<MarblesCollision>(m1, m2));
            }
        }
    }
}

void Simulation::scheduleEventIn(Time dt, boost::shared_ptr<Event> e) {
    assert(dt >= 0 * second);
    _events.insert(std::make_pair(_t + dt, e));
}

void Simulation::scheduleTickIn(Time dt) {
    scheduleEventIn(dt, boost::make_shared<Tick>());
}

} // Namespace
