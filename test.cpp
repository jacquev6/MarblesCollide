#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "collide.hpp"

BOOST_AUTO_TEST_CASE(test_hello) {
    BOOST_CHECK_EQUAL(collide::hello(), "Hello, World");
}
