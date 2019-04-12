/**
 * @file   test/Utilities/constexpr_math_test.cc
 * @brief  Unit test for `lardataalg/Utilities/quantities/datasize.h` header.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 2, 2018
 * @see    `lardataalg/Utilities/quantities.h`
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE ( constexpr_math_test )
#include <cetlib/quiet_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// LArSoft libraries
#include "lardataalg/Utilities/constexpr_math.h"

// C/C++ standard libraries


// -----------------------------------------------------------------------------
void test_abs() {

  static_assert(util::abs(-0.0) == 0.0);
  static_assert(util::abs(-2.0) == 2.0);
  static_assert(util::abs(-2.0F) == 2.0F);
  static_assert(util::abs(-2) == 2);
  static_assert(util::abs(-2L) == 2L);
  static_assert(util::abs(-2LL) == 2LL);

} // test_abs()


// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(constexpr_math_testcase) {

  test_abs();

} // BOOST_AUTO_TEST_CASE(constexpr_math_testcase)


// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
