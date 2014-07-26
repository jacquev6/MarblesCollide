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
