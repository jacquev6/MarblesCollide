#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>
#include <boost/make_shared.hpp>

#include "collide.hpp"

namespace ba = boost::assign;

namespace collide {
std::ostream& operator<<(std::ostream& s, const Position& p) {
    return s << "x=" << p.x << ",y=" << p.y;
}
std::ostream& operator<<(std::ostream& s, const Displacement& d) {
    return s << "dx=" << d.dx << ",dy=" << d.dy;
}
std::ostream& operator<<(std::ostream& s, const Date& d) {
    return s << "t=" << d.t;
}
std::ostream& operator<<(std::ostream& s, const Duration& d) {
    return s << "dt=" << d.dt;
}
std::ostream& operator<<(std::ostream& s, const Velocity& v) {
    return s << "vx=" << v.vx << ",vy=" << v.vy;
}

bool operator==(const Position& a, const Position& b) {
    return a.x == b.x && a.y == b.y;
}
bool operator==(const Displacement& a, const Displacement& b) {
    return a.dx == b.dx && a.dy == b.dy;
}
bool operator==(const Date& a, const Date& b) {
    return a.t == b.t;
}
bool operator==(const Duration& a, const Duration& b) {
    return a.dt == b.dt;
}
bool operator==(const Velocity& a, const Velocity& b) {
    return a.vx == b.vx && a.vy == b.vy;
}
}

using namespace collide;

BOOST_AUTO_TEST_CASE(BasicQuantitiesArithmetics) {
    BOOST_CHECK_EQUAL(Position(5, 6) - Position(3, 2), Displacement(2, 4));
    BOOST_CHECK_EQUAL(Position(3, 2) + Displacement(2, 4), Position(5, 6));
    BOOST_CHECK_EQUAL(Velocity(3, 2) + Velocity(2, 4), Velocity(5, 6));
    BOOST_CHECK_EQUAL(Velocity(3, 2) - Velocity(2, 4), Velocity(1, -2));
    BOOST_CHECK_EQUAL(Displacement(2, 4) / Duration(2), Velocity(1, 2));
    BOOST_CHECK_EQUAL(Velocity(1, 2) * Duration(2), Displacement(2, 4));
    BOOST_CHECK_EQUAL(Date(5) - Date(3), Duration(2));
    BOOST_CHECK_EQUAL(Date(3) + Duration(2), Date(5));
    BOOST_CHECK_EQUAL(Displacement(3, 4).length(), 5);
    BOOST_CHECK_EQUAL(3 * Displacement(3, 4), Displacement(9, 12));
    BOOST_CHECK_EQUAL(Displacement(9, 12) / 3, Displacement(3, 4));
    BOOST_CHECK_EQUAL(3 * Duration(3), Duration(9));
    BOOST_CHECK_EQUAL(Duration(9) / 3, Duration(3));
    BOOST_CHECK_EQUAL(3 * Velocity(3, 4), Velocity(9, 12));
    BOOST_CHECK_EQUAL(Velocity(9, 12) / 3, Velocity(3, 4));
    BOOST_CHECK(Date(5) > Date(3));
    BOOST_CHECK(Date(3) < Date(5));
}


BOOST_AUTO_TEST_CASE(MarbleConstants) {
    Marble m("foobar", 1, 2, Position(0, 0), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m.name(), "foobar");
    BOOST_CHECK_EQUAL(m.r(), 1);
    BOOST_CHECK_EQUAL(m.m(), 2);
}

BOOST_AUTO_TEST_CASE(MarblePosition) {
    Marble m("1", 1, 1, Position(2, 3), Velocity(4, 5));
    BOOST_CHECK_EQUAL(m.p(Date(0)), Position(2, 3));
    BOOST_CHECK_EQUAL(m.p(Date(6)), Position(26, 33));
}

BOOST_AUTO_TEST_CASE(MarbleVelocity) {
    Marble m("1", 1, 1, Position(2, 3), Velocity(4, 5));
    BOOST_CHECK_EQUAL(m.v(), Velocity(4, 5));
    m.setVelocity(Date(6), Velocity(7, 8));
    BOOST_CHECK_EQUAL(m.p(Date(9)), Position(47, 57));
    BOOST_CHECK_EQUAL(m.v(), Velocity(7, 8));
}

#define CHECK_NO_COLLISION(p1, v1, p2, v2) \
BOOST_CHECK_EQUAL(collisions::nextCollisionDate(Date(1), Marble("1", 1, 1, p1, v1), Marble("2", 2, 1, p2, v2)), boost::optional<Date>());

#define CHECK_COLLISION(p1, v1, p2, v2, t) \
BOOST_CHECK_EQUAL(collisions::nextCollisionDate(Date(1), Marble("1", 1, 1, p1, v1), Marble("2", 2, 1, p2, v2)), boost::optional<Date>(t));

BOOST_AUTO_TEST_CASE(NextCollisionBetweenMarbles) {
    //       0  1  2  3  4  5  6  7  8  9
    // Parallel trajectories
    // t=0 (-0>)
    //                 (----5--->)
    // t=0    (-1>)
    //                    (----6--->)
    // ...
    CHECK_NO_COLLISION(Position(0, 0), Velocity(1, 0), Position(0, 5), Velocity(1, 0));
    CHECK_NO_COLLISION(Position(0, 0), Velocity(0, 1), Position(5, 0), Velocity(0, 1));
    // Chase at same speed
    // t=0 (-0>)    (----4--->)
    // t=1    (-1>)    (----5--->)
    // ...
    CHECK_NO_COLLISION(Position(0, 0), Velocity(1, 0), Position(4, 0), Velocity(1, 0));
    CHECK_NO_COLLISION(Position(0, 0), Velocity(0, 1), Position(0, 4), Velocity(0, 1));
    // Chase at slower speed
    // t=0 (-0>)    (----4--->)
    // t=1    (-1>)       (----6--->)
    // ...
    CHECK_NO_COLLISION(Position(0, 0), Velocity(1, 0), Position(4, 0), Velocity(2, 0));
    CHECK_NO_COLLISION(Position(0, 0), Velocity(0, 1), Position(0, 4), Velocity(0, 2));
    // Chase at slower speed, starting with intersection
    // t=0 (-0>)
    //        (----2--->)
    // t=1    (-1>)
    //              (----4--->)
    // ...
    CHECK_NO_COLLISION(Position(0, 0), Velocity(1, 0), Position(2, 0), Velocity(2, 0));
    CHECK_NO_COLLISION(Position(0, 0), Velocity(0, 1), Position(0, 2), Velocity(0, 2));
    // Frontal horizontal collision
    // t=0 (-0>)                   (<---9----)
    // t=1    (-1>)             (<---8----)
    // t=2       (-2>)       (<---7----)
    // t=3          (-3>)!(<---6----)
    CHECK_COLLISION(Position(0, 0), Velocity(1, 0), Position(9, 0), Velocity(-1, 0), Date(3));
    CHECK_COLLISION(Position(0, 0), Velocity(0, 1), Position(0, 9), Velocity(0, -1), Date(3));
    // Chase at faster speed
    // t=0 (-0>)          (----6--->)
    // t=1       (-2>)       (----7--->)
    // t=2             (-4>)    (----8--->)
    // t=3                   (-6>)!(----9--->)
    CHECK_COLLISION(Position(0, 0), Velocity(2, 0), Position(6, 0), Velocity(1, 0), Date(3));
    CHECK_COLLISION(Position(0, 0), Velocity(0, 2), Position(0, 6), Velocity(0, 1), Date(3));
    // Side touch
    // t=0 (-0>)
    //              (<---4--->)
    // t=1    (-1>)
    //           (<---3--->)
    // t=2       (-2>)
    //        (<---2--->)
    CHECK_COLLISION(Position(0, 0), Velocity(1, 0), Position(4, 3), Velocity(-1, 0), Date(2));
    CHECK_COLLISION(Position(0, 0), Velocity(0, 1), Position(3, 4), Velocity(0, -1), Date(2));
    // Just two unrelated routes
    CHECK_NO_COLLISION(Position(0, 0), Velocity(1, 0), Position(5, 5), Velocity(1, 1));
}

BOOST_AUTO_TEST_CASE(HorizontalFrontalCollisionWithStillMarble) {
    Marble m1("1", 1, 1, Position(0, 0), Velocity(1, 0));
    Marble m2("2", 1, 1, Position(2, 0), Velocity(0, 0));
    collisions::performCollision(Date(0), m1, m2);
    BOOST_CHECK_EQUAL(m1.v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2.v(), Velocity(1, 0));
}

BOOST_AUTO_TEST_CASE(VerticalFrontalCollisionWithStillMarble) {
    Marble m1("1", 1, 1, Position(0, 0), Velocity(0, 1));
    Marble m2("2", 1, 1, Position(0, 2), Velocity(0, 0));
    collisions::performCollision(Date(0), m1, m2);
    BOOST_CHECK_EQUAL(m1.v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2.v(), Velocity(0, 1));
}

BOOST_AUTO_TEST_CASE(HorizontalFrontalCollisionBetweenMarbles) {
    Marble m1("1", 1, 1, Position(0, 0), Velocity(1, 0));
    Marble m2("2", 1, 1, Position(2, 0), Velocity(-1, 0));
    collisions::performCollision(Date(0), m1, m2);
    BOOST_CHECK_EQUAL(m1.v(), Velocity(-1, 0));
    BOOST_CHECK_EQUAL(m2.v(), Velocity(1, 0));
}


BOOST_AUTO_TEST_CASE(SimulationConstants) {
    auto m = boost::make_shared<Marble>("1", 1, 1, Position(2, 5), Velocity(1, 0));
    Simulation s(800, 600, ba::list_of(m));
    BOOST_CHECK_EQUAL(s.width(), 800);
    BOOST_CHECK_EQUAL(s.height(), 600);
    BOOST_CHECK_EQUAL(s.marbles().size(), 1);
}

BOOST_AUTO_TEST_CASE(RunSimulation) {
    auto m = boost::make_shared<Marble>("1", 1, 1, Position(2, 5), Velocity(1, 0));
    Simulation s(10, 10, ba::list_of(m));
    BOOST_CHECK_EQUAL(s.t(), Date(0));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(2, 5));
    s.runUntil(Date(4));
    BOOST_CHECK_EQUAL(s.t(), Date(4));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(6, 5));
}

BOOST_AUTO_TEST_CASE(SimulateFrontalCollision) {
    auto m1 = boost::make_shared<Marble>("1", 1, 1, Position(1, 5), Velocity(1, 0));
    auto m2 = boost::make_shared<Marble>("2", 1, 1, Position(9, 5), Velocity(-1, 0));
    Simulation s(10, 10, ba::list_of(m1)(m2));
    s.runUntil(Date(3));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(1, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(-1, 0));
    s.runUntil(Date(3.01));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(-1, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(1, 0));
}

BOOST_AUTO_TEST_CASE(SimulateChainOfFrontalCollision) {
    auto m1 = boost::make_shared<Marble>("1", 1, 1, Position(1, 5), Velocity(1, 0));
    auto m2 = boost::make_shared<Marble>("2", 1, 1, Position(4, 5), Velocity(0, 0));
    auto m3 = boost::make_shared<Marble>("3", 1, 1, Position(7, 5), Velocity(0, 0));
    Simulation s(100, 10, ba::list_of(m1)(m2)(m3));
    s.runUntil(Date(1));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(1, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(0, 0));
    s.runUntil(Date(1.01));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(1, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(0, 0));
    s.runUntil(Date(2));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(1, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(0, 0));
    s.runUntil(Date(2.1));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(1, 0));
    // Verify the anticipated collision between 1 and 3 is canceled
    s.runUntil(Date(4));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(1, 0));
    s.runUntil(Date(4.1));
    BOOST_CHECK_EQUAL(m1->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m2->v(), Velocity(0, 0));
    BOOST_CHECK_EQUAL(m3->v(), Velocity(1, 0));
}

BOOST_AUTO_TEST_CASE(SimulateCollisionsWithWalls) {
    auto m = boost::make_shared<Marble>("FOO", 1, 1, Position(1, 7), Velocity(4, 3));
    Simulation s(18, 14, ba::list_of(m));
    s.runUntil(Date(2));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(9, 13));
    s.runUntil(Date(2.1));
    BOOST_CHECK_EQUAL(m->v(), Velocity(4, -3));
    s.runUntil(Date(4));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(17, 7));
    s.runUntil(Date(4.1));
    BOOST_CHECK_EQUAL(m->v(), Velocity(-4, -3));
    s.runUntil(Date(6));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(9, 1));
    s.runUntil(Date(6.1));
    BOOST_CHECK_EQUAL(m->v(), Velocity(-4, 3));
    s.runUntil(Date(8));
    BOOST_CHECK_EQUAL(m->p(s.t()), Position(1, 7));
    s.runUntil(Date(8.1));
    BOOST_CHECK_EQUAL(m->v(), Velocity(4, 3));
}
