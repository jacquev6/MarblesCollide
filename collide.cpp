#include "collide.hpp"

#include <boost/make_shared.hpp>
#include <boost/assign.hpp>

//#define DEBUG(s) std::cout << s << std::endl
#ifdef DEBUG
#include <iostream>
#else
#define DEBUG(s)
#endif


namespace collide {

Position::Position(float x_, float y_) : x(x_), y(y_) {}

Displacement::Displacement(float dx_, float dy_) : dx(dx_), dy(dy_) {}
float Displacement::length() const {
    return sqrt(length2());
}
float Displacement::length2() const {
    return dx * dx + dy * dy;
}

Date::Date(float t_) : t(t_) {}

Duration::Duration(float dt_) : dt(dt_) {}

Velocity::Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}

Displacement operator-(const Position& a, const Position& b) {
    return Displacement(a.x - b.x, a.y - b.y);
}

Position operator+(const Position& p, const Displacement& d) {
    return Position(p.x + d.dx, p.y + d.dy);
}

Displacement operator*(float f, const Displacement& d) {
    return Displacement(f * d.dx, f * d.dy);
}

Displacement operator/(const Displacement& d, float f) {
    return Displacement(d.dx / f, d.dy / f);
}

Velocity operator/(const Displacement& d, const Duration& t) {
    return Velocity(d.dx / t.dt, d.dy / t.dt);
}

Velocity operator*(float f, const Velocity& v) {
    return Velocity(f * v.vx, f * v.vy);
}

Velocity operator/(const Velocity& v, float f) {
    return Velocity(v.vx / f, v.vy / f);
}

Velocity operator+(const Velocity& a, const Velocity& b) {
    return Velocity(a.vx + b.vx, a.vy + b.vy);
}

Velocity operator-(const Velocity& a, const Velocity& b) {
    return Velocity(a.vx - b.vx, a.vy - b.vy);
}

Displacement operator*(const Velocity& v, const Duration& d) {
    return Displacement(v.vx * d.dt, v.vy * d.dt);
}

Duration operator-(const Date& a, const Date& b) {
    return Duration(a.t - b.t);
}

Duration operator*(float f, const Duration& d) {
    return Duration(f * d.dt);
}

Duration operator/(const Duration& d, float f) {
    return Duration(d.dt / f);
}

Date operator+(const Date& t, const Duration& d) {
    return Date(t.t + d.dt);
}

bool operator>(const Date& a, const Date& b) {
    return a.t > b.t;
}

bool operator<(const Date& a, const Date& b) {
    return a.t < b.t;
}


Marble::Marble(std::string name, float r, float m, Position p, Velocity v) :
    _name(name),
    _r(r),
    _m(m),
    _p0(p),
    _t0(0),
    _v(v)
{
}

std::string Marble::name() const {
    return _name;
}

float Marble::r() const {
    return _r;
}

float Marble::m() const {
    return _m;
}

Position Marble::p(Date t) const {
    return _p0 + _v * (t - _t0);
}

Date Marble::t0() const {
    return _t0;
}

Velocity Marble::v() const {
    return _v;
}

void Marble::setVelocity(Date t, Velocity v) {
    _p0 = p(t);
    _t0 = t;
    _v = v;
}

namespace collisions {
    boost::optional<Date> nextCollisionDate(const Date& after, const Marble& m1, const Marble& m2) {
        boost::optional<Date> t = collisionDate(m1, m2);
        if(t && *t > after) {
            return t;
        }
        return boost::optional<Date>();
    }

    boost::optional<Date> collisionDate(const Marble& m1, const Marble& m2) {
        // Collision at t (to be solved for t)
        // <=> (m1.p(t) - m2.p(t)).length() == m1.r() + m2.r()
        // <=> ((m1.p(0) + m1.v() * t) - (m2.p(0) + m2.v() * t)).length2() == (m1.r() + m2.r())²
        float r2 = (m1.r() + m2.r()) * (m1.r() + m2.r());
        // <=>   ((m1.p(0).x + m1.v().vx * t) - (m2.p(0).x + m2.v().vx * t))²
        //     + ((m1.p(0).y + m1.v().vy * t) - (m2.p(0).y + m2.v().vy * t))²
        //     == r2
        // <=>   ((m1.p(0).x - m2.p(0).x) + (m1.v().vx - m2.v().vx) * t)²
        //     + ((m1.p(0).y - m2.p(0).y) + (m1.v().vy - m2.v().vy) * t)²
        //     == r2
        float dx = m1.p(0).x - m2.p(0).x;
        float dy = m1.p(0).y - m2.p(0).y;
        float dvx = m1.v().vx - m2.v().vx;
        float dvy = m1.v().vy - m2.v().vy;
        // <=> (dx + dvx * t)² + (dy + dvy * t)² == r2
        // <=> (dx² + 2 * dx * dvx * t + dvx² * t²) + (dy² + 2 * dy * dvy * t + dvy² * t²) == r2
        // <=> (dvx² + dvy²) * t² + 2 * (dx * dvx + dy * dvy) * t + (dx² + dy² - r2) == 0
        float a = dvx * dvx + dvy * dvy;
        float b = dx * dvx + dy * dvy;
        float c = dx * dx + dy * dy - r2;
        // <=> a * t² + 2 * b * t + c == 0

        // Some properties of this parabol:
        //  - it's concav (decreasing first then increasing) because two constant-velocity objects first get closer to each other, then get further.
        //    Also, this is proven by the fact that a is a sum of squares, hence positive or null.
        assert(a >= 0);
        //  - it degenerates to an horizontal line when m1.v() == m2.v(), meaning that two objects with same velocity have a constant distance between them.
        //    This is consistent with a being the norm of m1.v() - m2.v(), null in that case.
        //    And if a == 0, then dvx == 0 and dvy == 0, so b == 0 too: two objects with same velocity never collide (unless they always touch each other, which we don't model)

        if(a != 0) {
            float delta = b * b - a * c;
            if(delta >= 0) {
                // a > 0 so we know which root is smaller.
                // This is the only root we're interrested in, because the other one corresponds to when marbles "touch" after intersecting
                return Date((-b - sqrt(delta)) / a);
            }
        }
        return boost::optional<Date>();
    }

    void performCollision(const Date& t, Marble& m1, Marble& m2) {
        // "All models are wrong, some are useful" http://en.wikiquote.org/wiki/George_E._P._Box#Empirical_Model-Building_and_Response_Surfaces_.281987.29
        // So we use the model of a perfect elastic collision, neglecting energy dissipation, spin, etc.
        // - total energy is unchanged: m1 * |v1|² + m2 * |v2|² = const
        // - total movement is unchanged: m1 * v1 + m2 * v2 = const
        // - force impulse is aligned with the centers, so there is no change on the composents of velocity normal to this vector

        // Normal vector
        Displacement centers = m2.p(t) - m1.p(t);
        float nx = centers.dx / centers.length();
        float ny = centers.dy / centers.length();

        // Vrel
        float v = (m2.v().vx - m1.v().vx) * nx + (m2.v().vy - m1.v().vy) * ny;
        Velocity vrel(v * nx, v * ny);

        Velocity v1 = m1.v() + 2 * m2.m() / (m1.m() + m2.m()) * vrel;
        Velocity v2 = m2.v() - 2 * m1.m() / (m1.m() + m2.m()) * vrel;

        m1.setVelocity(t, v1);
        m2.setVelocity(t, v2);
    }
}


Simulation::Simulation(float width, float height, const std::vector<boost::shared_ptr<Marble>>& marbles) :
    _w(width),
    _h(height),
    _marbles(marbles),
    _t(0),
    _events()
{
    scheduleInitialEvents();
}

Date Simulation::t() const {
    return _t;
}

float Simulation::width() const {
    return _w;
}

float Simulation::height() const {
    return _h;
}

const std::vector<boost::shared_ptr<Marble>>& Simulation::marbles() const {
    return _marbles;
}


void Simulation::runUntil(const Date& t) {
    while(!_events.empty() && _events.top()->t() < t) {
        auto e = _events.top();
        _t = e->t();
        e->apply(*this);
        _events.pop();
    }
    _t = t;
}

Simulation::Event::Event(const Date& t, std::vector<boost::shared_ptr<Marble>> marbles) :
    _t(t),
    _marbles(marbles.begin(), marbles.end())
{
}

Date Simulation::Event::t() const {
    return _t;
}

void Simulation::Event::apply(Simulation& s) {
    for(ImpactedMarble m: _marbles) {
        if(!m.check()) {
            DEBUG("Cancelling event at t=" << t().t);
            return;
        }
    }
    doApply(s);
    for(ImpactedMarble m: _marbles) {
        s.scheduleNextEvents(m.marble);
    }
}

class Simulation::MarblesCollision : public Event {
public:
    MarblesCollision(const Date& t, boost::shared_ptr<Marble> m1, boost::shared_ptr<Marble> m2) :
        Event(t, boost::assign::list_of(m1)(m2)),
        _m1(m1),
        _m2(m2)
    {}

public:
    void doApply(Simulation&) {
        DEBUG("Executing collision between " << _m1->name() << " and " << _m2->name() << " at t=" << t().t);
        collisions::performCollision(t(), *_m1, *_m2);
    }

private:
    boost::shared_ptr<Marble> _m1;
    boost::shared_ptr<Marble> _m2;
};

class Simulation::WallCollision : public Event {
public:
    WallCollision(const Date& t, boost::shared_ptr<Marble> m, bool h, bool v) :
        Event(t, boost::assign::list_of(m)),
        _m(m),
        _h(h),
        _v(v)
    {}

public:
    void doApply(Simulation&) {
        DEBUG("Executing collision between " << _m->name() << " and wall at t=" << t().t);
        float vx = _m->v().vx;
        float vy = _m->v().vy;
        if(_h) vx *= -1;
        if(_v) vy *= -1;
        _m->setVelocity(t(), Velocity(vx, vy));
    }

private:
    boost::shared_ptr<Marble> _m;
    bool _h;
    bool _v;
};

void Simulation::scheduleInitialEvents() {
    for(boost::shared_ptr<Marble> m1: _marbles) {
        for(boost::shared_ptr<Marble> m2: _marbles) {
            if(m1 < m2) {
                boost::optional<Date> t = collisions::nextCollisionDate(_t, *m1, *m2);
                if(t) {
                    DEBUG("Scheduling initial collision between " << m1->name() << " and " << m2->name() << " at t=" << t->t);
                    _events.push(boost::make_shared<MarblesCollision>(*t, m1, m2));
                }
            }
        }
        scheduleNextWallCollision(m1);
    }
}

void Simulation::scheduleNextEvents(boost::shared_ptr<Marble> m1) {
    for(boost::shared_ptr<Marble> m2: _marbles) {
        boost::optional<Date> t = collisions::nextCollisionDate(_t, *m1, *m2);
        if(t) {
            DEBUG("Scheduling next collision between " << m1->name() << " and " << m2->name() << " at t=" << t->t);
            _events.push(boost::make_shared<MarblesCollision>(*t, m1, m2));
        }
    }
    scheduleNextWallCollision(m1);
}

void Simulation::scheduleNextWallCollision(boost::shared_ptr<Marble> m1) {
    if(m1->v().vx > 0) {
        Date t(_t.t + (_w - m1->p(_t).x - m1->r()) / m1->v().vx);
        DEBUG("Scheduling collision between " << m1->name() << " and right wall at t=" << t.t);
        _events.push(boost::make_shared<WallCollision>(t, m1, true, false));
    }
    if(m1->v().vx < 0) {
        Date t(_t.t - (m1->p(_t).x - m1->r()) / m1->v().vx);
        DEBUG("Scheduling collision between " << m1->name() << " and left wall at t=" << t.t);
        _events.push(boost::make_shared<WallCollision>(t, m1, true, false));
    }
    if(m1->v().vy > 0) {
        Date t(_t.t + (_h - m1->p(_t).y - m1->r()) / m1->v().vy);
        DEBUG("Scheduling collision between " << m1->name() << " and bottom wall at t=" << t.t);
        _events.push(boost::make_shared<WallCollision>(t, m1, false, true));
    }
    if(m1->v().vy < 0) {
        Date t(_t.t - (m1->p(_t).y - m1->r()) / m1->v().vy);
        DEBUG("Scheduling collision between " << m1->name() << " and top wall at t=" << t.t);
        _events.push(boost::make_shared<WallCollision>(t, m1, false, true));
    }
}
} // Namespace
