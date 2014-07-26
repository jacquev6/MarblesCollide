#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "collide.hpp"
using namespace collide;
using namespace bu;

BOOST_AUTO_TEST_CASE(test_hello) {
    BOOST_CHECK_EQUAL(collide::hello(), "Hello, World");
}

BOOST_AUTO_TEST_CASE(AdvanceMarble) {
    Marble m(1 * meter, 1 * kilogram, 1 * meter, 2 * meter, 3 * meter_per_second, 4 * meter_per_second);
    BOOST_CHECK_EQUAL(m.x(), 1 * meter);
    BOOST_CHECK_EQUAL(m.y(), 2 * meter);
    m.advanceTo(5 * second);
    BOOST_CHECK_EQUAL(m.x(), 16 * meter);
    BOOST_CHECK_EQUAL(m.y(), 22 * meter);
}

BOOST_AUTO_TEST_CASE(AdvanceMarbleByOneBigStep) {
    Marble m(0, 0, 1000000000 * meter, 0, 0.001 * meter_per_second, 0);
    m.advanceTo(1000 * second);
    BOOST_CHECK_EQUAL(m.x(), 1000000001 * meter);
}

BOOST_AUTO_TEST_CASE(AdvanceMarbleByManySmallSteps) {
    // This test demonstrates that the naive implementation of advanceTo
    //    Time dt = t - _t;
    //    _x += dt * _vx;
    //    _y += dt * _vy;
    //    _t = t;
    // does not work when doing small steps at low speed on an already large
    // coordinate, and that we need the t0-x0-y0-vx-vy-based implementation.
    Marble m(0, 0, 1000000000 * meter, 0, 0.001 * meter_per_second, 0);
    for(int i = 0; i < 1000; ++i) {
        m.advanceTo(i * second);
    }
    m.advanceTo(1000 * second);
    BOOST_CHECK_EQUAL(m.x(), 1000000001 * meter);
}

BOOST_AUTO_TEST_CASE(ChangeMarbleSpeed) {
    Marble m(1 * meter, 1 * kilogram, 1 * meter, 2 * meter, 3 * meter_per_second, 4 * meter_per_second);
    m.advanceTo(5 * second);
    m.setSpeed(5 * meter_per_second, 6 * meter_per_second);
    m.advanceTo(10 * second);
    BOOST_CHECK_EQUAL(m.x(), 41 * meter);
    BOOST_CHECK_EQUAL(m.y(), 52 * meter);
}
