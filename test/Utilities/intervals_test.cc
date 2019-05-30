/**
 * @file   test/Utilities/intervals_test.cc
 * @brief  Unit test for `intervals.h` header
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 29, 2019
 * @see    lardataalg/Utilities/intervals.h
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE ( intervals_test )
#include <cetlib/quiet_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// LArSoft libraries
#include "lardataalg/Utilities/intervals.h"
#include "lardataalg/Utilities/quantities/spacetime.h"

// C/C++ standard libraries
#include <type_traits> // std::decay_t<>

// -----------------------------------------------------------------------------
void test_interval_comparisons() {
  
  using namespace util::quantities::time_literals;
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  constexpr microseconds const t1 {         +6.0_us };
  constexpr nanoseconds  const t2 { -4'000'000.0_ps };
  
  constexpr microseconds const t1s = t1;

  // implicit conversion
  static_assert(t1 ==     6.0); // in microseconds
  static_assert(t2 == -4000.0); // in nanoseconds
  
  static_assert(  t1      == 6_us    );
  static_assert(  t1      == 6000_ns );
  static_assert(  6_us    == t1      );
  static_assert(  6000_ns == t1      );
  static_assert(  t1      == t1s     );
  static_assert(!(t1      == 5_us   ));
  static_assert(!(t1      == 5_ns   ));
  static_assert(!(5_us    == t1     ));
  static_assert(!(5_ns    == t1     ));
  static_assert(!(t1      == t2     ));
  
  static_assert(!(t1      != 6_us   ));
  static_assert(!(t1      != 6000_ns));
  static_assert(!(6_us    != t1     ));
  static_assert(!(6000_ns != t1     ));
  static_assert(!(t1      != t1s    ));
  static_assert(  t1      != 5_us    );
  static_assert(  t1      != 5_ns    );
  static_assert(  5_us    != t1      );
  static_assert(  5_ns    != t1      );
  static_assert(  t1      != t2      );
  
  static_assert(  t1      >= 6_us    );
  static_assert(  t1      >= 6000_ns );
  static_assert(  6_us    >= t1      );
  static_assert(  6000_ns >= t1      );
  static_assert(  t1      >= t1s     );
  static_assert(  t1      >= 5_us    );
  static_assert(  t1      >= 5000_ns );
  static_assert(!(5_us    >= t1     ));
  static_assert(!(5000_ns >= t1     ));
  static_assert(!(t1      >= 8_us   ));
  static_assert(!(t1      >= 8000_ns));
  static_assert(  8_us    >= t1      );
  static_assert(  8000_ns >= t1      );
  static_assert(  t1      >= t2      );
  
  static_assert(!(t1      >  6_us   ));
  static_assert(!(t1      >  6000_ns));
  static_assert(!(6_us    >  t1     ));
  static_assert(!(6000_ns >  t1     ));
  static_assert(!(t1      >  t1s    ));
  static_assert(  t1      >  5_us    );
  static_assert(  t1      >  5000_ns );
  static_assert(!(5_us    >  t1     ));
  static_assert(!(5000_ns >  t1     ));
  static_assert(!(t1      >  8_us   ));
  static_assert(!(t1      >  8000_ns));
  static_assert(  8_us    >  t1      );
  static_assert(  8000_ns >  t1      );
  static_assert(  t1      >  t2      );
  
  static_assert(  t1      <= 6_us    );
  static_assert(  t1      <= 6000_ns );
  static_assert(  6_us    <= t1      );
  static_assert(  6000_ns <= t1      );
  static_assert(  t1      <= t1s     );
  static_assert(!(t1      <= 5_us   ));
  static_assert(!(t1      <= 5000_ns));
  static_assert(  5_us    <= t1      );
  static_assert(  5000_ns <= t1      );
  static_assert(  t1      <= 8_us    );
  static_assert(  t1      <= 8000_ns );
  static_assert(!(8_us    <= t1     ));
  static_assert(!(8000_ns <= t1     ));
  static_assert(!(t1      <= t2     ));
  
  static_assert(!(t1      <  6_us   ));
  static_assert(!(t1      <  6000_ns));
  static_assert(!(6_us    <  t1     ));
  static_assert(!(6000_ns <  t1     ));
  static_assert(!(t1      <  t1s    ));
  static_assert(!(t1      <  5_us   ));
  static_assert(!(t1      <  5000_ns));
  static_assert(  5_us    <  t1      );
  static_assert(  5000_ns <  t1      );
  static_assert(  t1      <  8_us    );
  static_assert(  t1      <  8000_ns );
  static_assert(!(8_us    <  t1     ));
  static_assert(!(8000_ns <  t1     ));
  static_assert(!(t1      <  t2     ));
  
} // test_interval_comparisons()


// -----------------------------------------------------------------------------
void test_interval_queries() {
  
  using namespace util::quantities::time_literals;
 
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  constexpr microseconds t1 {         +6.0_us };
  constexpr nanoseconds  t2 { -4'000'000.0_ps };
  
  static_assert(std::is_same_v<microseconds::quantity_t, util::quantities::microsecond>);
  static_assert(std::is_same_v<microseconds::unit_t, util::quantities::microsecond::unit_t>);
  
  static_assert(t1.quantity() ==  6_us   );
  static_assert(t2.quantity() == -4_us   );
  static_assert(t1.value()    ==     6.0 );
  static_assert(t2.value()    == -4000.0 );
  
} // test_interval_queries()


// -----------------------------------------------------------------------------
void test_interval_operations() {

  using namespace util::quantities::time_literals;
 
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  constexpr microseconds ct1 {    +6.0_us };
  constexpr microseconds ct2 { -4000.0_ns };
  
  static_assert(std::is_same_v<decltype(ct1 + ct2), microseconds>);
  static_assert(ct1 + ct2 == 2_us);

  static_assert(std::is_same_v<decltype(ct1 - ct2), microseconds>);
  static_assert(ct1 - ct2 == 10_us);

  static_assert(std::is_same_v<decltype(+ct1), microseconds>);
  static_assert(+ct1 == 6_us);

  static_assert(std::is_same_v<decltype(-ct1), microseconds>);
  static_assert(-ct1 == -6_us);

  static_assert(std::is_same_v<decltype(ct1.abs()), microseconds>);
  static_assert(ct1.abs() == 6_us);
  static_assert(ct2.abs() == 4_us);

  static_assert(std::is_same_v<decltype(ct1 * 3.0), microseconds>);
  static_assert(ct1 * 3.0 == 6_us * 3.0);
  
  static_assert(std::is_same_v<decltype(3.0 * ct1), microseconds>);
  static_assert(3.0 * ct1 == 6_us * 3.0);
  
  static_assert(std::is_same_v<decltype(ct1 / 2.0), microseconds>);
  static_assert(ct1 / 2.0 == 6_us / 2.0);
  
  microseconds t1 {    +6.0_us };
  nanoseconds t2  { -4000.0_ns };

  decltype(auto) incr = (t1 += 2000_ns);
  static_assert(std::is_same_v<decltype(incr), microseconds&>);
  BOOST_CHECK_EQUAL(t1, 8_us);
  BOOST_CHECK_EQUAL(&incr, &t1);
  
  decltype(auto) incr2 = (t1 += t2);
  static_assert(std::is_same_v<decltype(incr2), microseconds&>);
  BOOST_CHECK_EQUAL(t1,  4_us);
  BOOST_CHECK_EQUAL(t2, -4_us);
  BOOST_CHECK_EQUAL(&incr2, &t1);
  
  decltype(auto) decr = (t1 -= 2000_ns);
  static_assert(std::is_same_v<decltype(decr), microseconds&>);
  BOOST_CHECK_EQUAL(t1, 2_us);
  BOOST_CHECK_EQUAL(&decr, &t1);
  
  decltype(auto) decr2 = (t1 -= t2);
  static_assert(std::is_same_v<decltype(decr2), microseconds&>);
  BOOST_CHECK_EQUAL(t1,  6_us);
  BOOST_CHECK_EQUAL(t2, -4_us);
  BOOST_CHECK_EQUAL(&decr2, &t1);
  
  decltype(auto) expand = (t1 *= 2);
  static_assert(std::is_same_v<decltype(expand), microseconds&>);
  BOOST_CHECK_EQUAL(t1, 12_us);
  BOOST_CHECK_EQUAL(&expand, &t1);
  
  decltype(auto) shrink = (t1 /= 2);
  static_assert(std::is_same_v<decltype(shrink), microseconds&>);
  BOOST_CHECK_EQUAL(t1, 6_us);
  BOOST_CHECK_EQUAL(&shrink, &t1);
  

} // test_interval_operations()


// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(intervals_testcase) {

  test_interval_queries();
  test_interval_comparisons();
  test_interval_operations();

} // BOOST_AUTO_TEST_CASE(intervals_testcase)

// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
