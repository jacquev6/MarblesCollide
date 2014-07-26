#ifndef collide_hpp
#define collide_hpp

#include <string>

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>


namespace bu {
    using namespace boost::units;
    using namespace boost::units::si;
} // Namespace

namespace collide {

typedef bu::quantity<bu::length> Length;
typedef bu::quantity<bu::velocity> Velocity;
typedef bu::quantity<bu::mass> Mass;
typedef bu::quantity<bu::time> Time;

class Marble {
public:
    Marble(Length r, Mass m, Length x, Length y, Velocity vx, Velocity vy);

    Length r() const;
    Mass m() const;
    Length x() const;
    Length y() const;
    Velocity vx() const;
    Velocity vy() const;

    void advanceTo(Time);
    void setSpeed(Velocity vx, Velocity vy);

private:
    Length _r;
    Mass _m;
    Length _x0;
    Length _y0;
    Velocity _vx;
    Velocity _vy;
    Time _t0;
    Time _t;
};
/*
class EventsHandler {
    void tick(const Simulation&);
};
*/
class Simulation {
public:
    Simulation(Length width, Length height, const std::vector<Marble>&);

    Length width() const;
    Length height() const;
    const std::vector<Marble>& marbles() const;

    void advanceTo(Time);

private:
    std::vector<Marble> _marbles;
};

} // Namespace

#endif // Include guard
