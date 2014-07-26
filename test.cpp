#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <boost/make_shared.hpp>

#include "collide.hpp"
using namespace collide;
using namespace bu;

namespace ba = boost::assign;


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

BOOST_AUTO_TEST_CASE(AdvanceSimulation) {
    Marble m(0, 0, 1 * meter, 1 * meter, 1 * meter_per_second, 1 * meter_per_second);
    Simulation s(10 * meter, 10 * meter, ba::list_of(m));
    BOOST_CHECK_EQUAL(s.marbles()[0].x(), 1 * meter);
    s.advanceTo(3 * second);
    BOOST_CHECK_EQUAL(s.marbles()[0].x(), 4 * meter);
}

BOOST_AUTO_TEST_CASE(MarbleCollidesOnRightWall) {
    Marble m(1 * meter, 0, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    Simulation s(10 * meter, 10 * meter, ba::list_of(m));
    s.advanceTo(8 * second);
    BOOST_CHECK_EQUAL(s.marbles()[0].x(), 9 * meter);
    BOOST_CHECK_EQUAL(s.marbles()[0].vx(), 1 * meter_per_second);
    s.advanceTo(12 * second);
    BOOST_CHECK_EQUAL(s.marbles()[0].x(), 5 * meter);
    BOOST_CHECK_EQUAL(s.marbles()[0].vx(), -1 * meter_per_second);
}

struct EventsCounter : public EventsHandler {
    int events;
    void begin(Simulation*) {
        ++events;
    }
    void tick() {
        ++events;
    }
};

BOOST_AUTO_TEST_CASE(EventsHandlerIsCalled) {
    auto handler(boost::make_shared<EventsCounter>());
    BOOST_CHECK_EQUAL(handler->events, 0);
    Simulation s(10 * meter, 10 * meter, std::vector<Marble>(), handler);
    BOOST_CHECK_EQUAL(handler->events, 1);
    s.scheduleTickIn(1 * second);
    s.scheduleTickIn(2 * second);
    BOOST_CHECK_EQUAL(handler->events, 1);
    s.advanceTo(2 * second);
    BOOST_CHECK_EQUAL(handler->events, 2);
    s.advanceTo(2.5 * second);
    BOOST_CHECK_EQUAL(handler->events, 3);
}
