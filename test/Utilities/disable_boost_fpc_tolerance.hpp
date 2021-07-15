#ifndef LARDATAALG_TEST_UTILITIES_DISABLE_BOOST_FPC_TOLERANCE_HPP
#define LARDATAALG_TEST_UTILITIES_DISABLE_BOOST_FPC_TOLERANCE_HPP

#include <boost/test/tools/floating_point_comparison.hpp>

#include "lardataalg/Utilities/quantities.h"
#include "lardataalg/Utilities/quantities/electronics.h"

// Because util::quantites::seconds (etc.) has std::numeric_limits<>
// specializations, the Boost unit test suite assumes they are
// suitable for floating-point comparisons, particularly tolerance
// testing (due to the inexact nature of the representation).  The
// following specializations disable such tolerance testing.

namespace boost::math::fpc {
  template <>
  struct tolerance_based<util::quantities::seconds> : std::false_type {};
  template <>
  struct tolerance_based<util::quantities::milliseconds> : std::false_type {};
  template <>
  struct tolerance_based<util::quantities::microseconds> : std::false_type {};
  template <>
  struct tolerance_based<util::quantities::nanoseconds> : std::false_type {};

  // Intervals
  template <>
  struct tolerance_based<util::quantities::intervals::microseconds> : std::false_type {};

  // Points
  template <>
  struct tolerance_based<util::quantities::points::millisecond> : std::false_type {};
  template <>
  struct tolerance_based<util::quantities::points::microsecond> : std::false_type {};

  // Electronics
  template <>
  struct tolerance_based<util::quantities::tick_d> : std::false_type {};
}

#endif /* LARDATAALG_TEST_UTILITIES_DISABLE_BOOST_FPC_TOLERANCE_HPP */
