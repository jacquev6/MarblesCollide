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
    std::string hello();

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

    private:
        Length _r;
        Mass _m;
        Length _x;
        Length _y;
        Velocity _vx;
        Velocity _vy;
        Time _t;
    };
} // Namespace

#endif // Include guard
