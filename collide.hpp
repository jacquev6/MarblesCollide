#ifndef collide_hpp
#define collide_hpp

#include <vector>
#include <map>

#include <boost/units/systems/si.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace bu {
    using namespace boost::units;
    using namespace boost::units::si;
} // Namespace

namespace collide {

typedef bu::quantity<bu::length> Length;
typedef bu::quantity<bu::velocity> Velocity;
typedef bu::quantity<bu::mass> Mass;
typedef bu::quantity<bu::time> Time;

class Marble : private boost::noncopyable {
public:
    typedef boost::shared_ptr<Marble> Ptr;
    static Ptr create(std::string name, Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy);

    std::string name() const;
    Length r() const;
    Mass m() const;
    Length x() const;
    Length y() const;
    Time t0() const;
    Velocity vx() const;
    Velocity vy() const;

    void advanceTo(Time);
    void setSpeed(Velocity vx, Velocity vy);

private:
    Marble(std::string name, Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy);

private:
    std::string _name;
    Length _r;
    Mass _m;
    Length _x0;
    Length _y0;
    Velocity _vx;
    Velocity _vy;
    Time _t0;
    Time _t;
};

class Simulation;

struct EventsHandler {
    typedef boost::shared_ptr<EventsHandler> Ptr;
    virtual void begin(Simulation*) = 0;
    virtual void tick() = 0;
    virtual void collision(Marble::Ptr, Marble::Ptr) = 0;
};

class Simulation {
    static EventsHandler::Ptr nullEventsHandler;

public:
    typedef boost::shared_ptr<Simulation> Ptr;
    static Ptr create(Length width, Length height, const std::vector<Marble::Ptr>&, EventsHandler::Ptr=nullEventsHandler);

    Time t() const;
    Length width() const;
    Length height() const;
    const std::vector<Marble::Ptr>& marbles() const;

    void scheduleTickIn(Time);
    void scheduleNextEventsFor(Marble::Ptr, Marble::Ptr=Marble::Ptr());
    void advanceTo(Time);

private:
    Simulation(Length, Length, const std::vector<Marble::Ptr>&, EventsHandler::Ptr);

    void scheduleNextCollisionsWithWalls(Marble::Ptr);
    void scheduleNextCollisionsWithOtherMarbles(Marble::Ptr, Marble::Ptr);
    void scheduleNextCollisionBetween(Marble::Ptr, Marble::Ptr);

    class Event;
    class WallCollision;
    class MarblesCollision;
    class Tick;

    void advanceMarblesTo(Time);
    void scheduleEventIn(Time, boost::shared_ptr<Event>);

private:
    Time _t;
    Length _w;
    Length _h;
    std::vector<Marble::Ptr> _marbles;
    std::multimap<Time, boost::shared_ptr<Event>> _events;
    boost::shared_ptr<EventsHandler> _eventsHandler;
};

} // Namespace

#endif // Include guard
