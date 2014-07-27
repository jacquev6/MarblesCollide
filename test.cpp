#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <boost/make_shared.hpp>

#include "collide.hpp"
using namespace collide;
using namespace bu;

namespace ba = boost::assign;

#define CHECK_CLOSE(m1, m2) BOOST_CHECK(abs((m1 - m2) / (m1 + m2)) < .0001)


BOOST_AUTO_TEST_CASE(AdvanceMarble) {
    auto m = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 2 * meter, 3 * meter_per_second, 4 * meter_per_second);
    CHECK_CLOSE(m->x(), 1 * meter);
    CHECK_CLOSE(m->y(), 2 * meter);
    m->advanceTo(5 * second);
    CHECK_CLOSE(m->x(), 16 * meter);
    CHECK_CLOSE(m->y(), 22 * meter);
}

BOOST_AUTO_TEST_CASE(AdvanceMarbleByOneBigStep) {
    auto m = Marble::create(0, 0, 1000000000 * meter, 0, 0.001 * meter_per_second, 0);
    m->advanceTo(1000 * second);
    CHECK_CLOSE(m->x(), 1000000001 * meter);
}

BOOST_AUTO_TEST_CASE(AdvanceMarbleByManySmallSteps) {
    // This test demonstrates that the naive implementation of advanceTo
    //    Time dt = t - _t;
    //    _x += dt * _vx;
    //    _y += dt * _vy;
    //    _t = t;
    // does not work when doing small steps at low speed on an already large
    // coordinate, and that we need the t0-x0-y0-vx-vy-based implementation.
    auto m = Marble::create(0, 0, 1000000000 * meter, 0, 0.001 * meter_per_second, 0);
    for(int i = 0; i < 1000; ++i) {
        m->advanceTo(i * second);
    }
    m->advanceTo(1000 * second);
    CHECK_CLOSE(m->x(), 1000000001 * meter);
}

BOOST_AUTO_TEST_CASE(ChangeMarbleSpeed) {
    auto m = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 2 * meter, 3 * meter_per_second, 4 * meter_per_second);
    m->advanceTo(5 * second);
    m->setSpeed(5 * meter_per_second, 6 * meter_per_second);
    m->advanceTo(10 * second);
    CHECK_CLOSE(m->x(), 41 * meter);
    CHECK_CLOSE(m->y(), 52 * meter);
}

BOOST_AUTO_TEST_CASE(AdvanceSimulation) {
    auto m = Marble::create(0, 0, 1 * meter, 1 * meter, 1 * meter_per_second, 1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m));
    CHECK_CLOSE(m->x(), 1 * meter);
    s->advanceTo(3 * second);
    CHECK_CLOSE(m->x(), 4 * meter);
}

BOOST_AUTO_TEST_CASE(MarbleCollidesOnRightWall) {
    auto m = Marble::create(1 * meter, 0, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m));
    s->advanceTo(8 * second);
    CHECK_CLOSE(m->x(), 9 * meter);
    CHECK_CLOSE(m->vx(), 1 * meter_per_second);
    s->advanceTo(12 * second);
    CHECK_CLOSE(m->x(), 5 * meter);
    CHECK_CLOSE(m->vx(), -1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideFrontalyOnHorizontalTrajectory) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 9 * meter, 5 * meter, -1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    s->advanceTo(3.1 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideFrontalyOnVerticalTrajectory) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 5 * meter, 1 * meter, 0, 1 * meter_per_second);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 5 * meter, 9 * meter, 0, -1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vy(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), -1 * meter_per_second);
    s->advanceTo(3.1 * second);
    CHECK_CLOSE(m1->vy(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), 1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideFrontalyOnDescendingDiagonalTrajectory) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 1 * meter, 1 * meter_per_second, 1 * meter_per_second);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 9 * meter, 9 * meter, -1 * meter_per_second, -1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), -1 * meter_per_second);
    s->advanceTo(5 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), 1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideFrontalyOnAscendingDiagonalTrajectory) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 9 * meter, 1 * meter_per_second, -1 * meter_per_second);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 9 * meter, 1 * meter, -1 * meter_per_second, 1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), 1 * meter_per_second);
    s->advanceTo(5 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), -1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideSidewayOnDiagonalTrajectories) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 9 * meter, 1 * meter_per_second, -1 * meter_per_second);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 1 * meter, 1 * meter_per_second, 1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), 1 * meter_per_second);
    s->advanceTo(5 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m1->vy(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vy(), -1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(TwoMarblesCollideFrontalyOnEachOtherAndOnWallsSeveralTimes) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 9 * meter, 5 * meter, -1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(3 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    s->advanceTo(6 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    s->advanceTo(9 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    s->advanceTo(12 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
    s->advanceTo(15 * second);
    CHECK_CLOSE(m1->vx(), 1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), -1 * meter_per_second);
    s->advanceTo(18 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(CollisionWithWallIsCanceled) {
    auto m = Marble::create(1 * meter, 0, 1 * meter, 2 * meter, 1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m));
    s->advanceTo(7 * second); // 1s before collision
    m->setSpeed(-1 * meter_per_second, 0);
    s->advanceTo(9 * second);
    CHECK_CLOSE(m->vx(), -1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(CollisionBetweenMarbesIsCanceled) {
    auto m1 = Marble::create(1 * meter, 1 * kilogram, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    auto m2 = Marble::create(1 * meter, 1 * kilogram, 9 * meter, 5 * meter, -1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m1)(m2));
    s->advanceTo(2 * second); // 1 s before collision
    m1->setSpeed(-1 * meter_per_second, 0);
    m2->setSpeed(1 * meter_per_second, 0);
    s->advanceTo(5 * second);
    CHECK_CLOSE(m1->vx(), -1 * meter_per_second);
    CHECK_CLOSE(m2->vx(), 1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(MarbleCollidesOnVerticalWallsTwice) {
    auto m = Marble::create(1 * meter, 0, 1 * meter, 5 * meter, 1 * meter_per_second, 0);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m));
    s->advanceTo(8 * second);
    CHECK_CLOSE(m->x(), 9 * meter);
    CHECK_CLOSE(m->vx(), 1 * meter_per_second);
    s->advanceTo(16 * second);
    CHECK_CLOSE(m->x(), 1 * meter);
    CHECK_CLOSE(m->vx(), -1 * meter_per_second);
    s->advanceTo(24 * second);
    CHECK_CLOSE(m->x(), 9 * meter);
    CHECK_CLOSE(m->vx(), 1 * meter_per_second);
    s->advanceTo(32 * second);
    CHECK_CLOSE(m->x(), 1 * meter);
    CHECK_CLOSE(m->vx(), -1 * meter_per_second);
}

BOOST_AUTO_TEST_CASE(MarbleCollidesOnHorizontalWallsTwice) {
    auto m = Marble::create(1 * meter, 0, 5 * meter, 1 * meter, 0, 1 * meter_per_second);
    auto s = Simulation::create(10 * meter, 10 * meter, ba::list_of(m));
    s->advanceTo(8 * second);
    CHECK_CLOSE(m->y(), 9 * meter);
    CHECK_CLOSE(m->vy(), 1 * meter_per_second);
    s->advanceTo(16 * second);
    CHECK_CLOSE(m->y(), 1 * meter);
    CHECK_CLOSE(m->vy(), -1 * meter_per_second);
    s->advanceTo(24 * second);
    CHECK_CLOSE(m->y(), 9 * meter);
    CHECK_CLOSE(m->vy(), 1 * meter_per_second);
    s->advanceTo(32 * second);
    CHECK_CLOSE(m->y(), 1 * meter);
    CHECK_CLOSE(m->vy(), -1 * meter_per_second);
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
    auto s = Simulation::create(10 * meter, 10 * meter, std::vector<Marble::Ptr>(), handler);
    BOOST_CHECK_EQUAL(handler->events, 1);
    s->scheduleTickIn(1 * second);
    s->scheduleTickIn(2 * second);
    BOOST_CHECK_EQUAL(handler->events, 1);
    s->advanceTo(2 * second);
    BOOST_CHECK_EQUAL(handler->events, 2);
    s->advanceTo(2.5 * second);
    BOOST_CHECK_EQUAL(handler->events, 3);
}
