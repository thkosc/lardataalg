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
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/intervals.h"
#include "lardataalg/Utilities/quantities/spacetime.h"
#include "larcorealg/CoreUtils/DebugUtils.h" // lar::debug::static_assert_on<>()


// C/C++ standard libraries
#include <type_traits> // std::decay_t<>

// -----------------------------------------------------------------------------
// --- implementation detail tests
// -----------------------------------------------------------------------------
struct ObjectWithoutCategory {};
struct ObjectWithCategory {
  using category_t = void;
};

static_assert
  (!util::quantities::concepts::details::has_category_v<ObjectWithoutCategory>);
static_assert
  ( util::quantities::concepts::details::has_category_v<ObjectWithCategory>);
static_assert(util::quantities::concepts::details::has_category_v
  <util::quantities::intervals::seconds>
  );
static_assert(util::quantities::concepts::details::has_category_v
  <util::quantities::points::second>
  );


// -----------------------------------------------------------------------------
static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::intervals::seconds>
  );
static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::intervals::microseconds>
  );
static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::intervals::microseconds_as<float>>
  );

static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::intervals::seconds>
  );
static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::intervals::microseconds>
  );
static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::intervals::microseconds_as<float>>
  );

static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::intervals::seconds>
  );
static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::intervals::microseconds>
  );
static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::intervals::microseconds_as<float>>
  );

static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::points::second>
  );
static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::points::microsecond>
  );
static_assert( util::quantities::concepts::details::has_unit_v
  <util::quantities::points::microsecond_as<float>>
  );

static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::points::second>
  );
static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::points::microsecond>
  );
static_assert(!util::quantities::concepts::details::is_quantity_v
  <util::quantities::points::microsecond_as<float>>
  );

static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::points::second>
  );
static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::points::microsecond>
  );
static_assert( util::quantities::concepts::details::has_quantity_v
  <util::quantities::points::microsecond_as<float>>
  );


// -----------------------------------------------------------------------------
// --- is_point, is_interval etc.
// -----------------------------------------------------------------------------
static_assert(!util::quantities::concepts::is_interval_v<double>);
static_assert
  (!util::quantities::concepts::is_interval_v<ObjectWithoutCategory>);
static_assert(!util::quantities::concepts::is_interval_v<ObjectWithCategory>);
static_assert(
  util::quantities::concepts::is_interval_v
    <util::quantities::intervals::seconds>
  );
static_assert(
  !util::quantities::concepts::is_interval_v
    <util::quantities::points::second>
  );

static_assert(!util::quantities::concepts::is_point_v<double>);
static_assert
  (!util::quantities::concepts::is_point_v<ObjectWithoutCategory>);
static_assert(!util::quantities::concepts::is_point_v<ObjectWithCategory>);
static_assert(
  !util::quantities::concepts::is_point_v
    <util::quantities::intervals::seconds>
  );
static_assert(
  util::quantities::concepts::is_point_v
    <util::quantities::points::second>
  );

static_assert(!util::quantities::concepts::is_interval_or_point_v<double>);
static_assert
  (!util::quantities::concepts::is_interval_or_point_v<ObjectWithoutCategory>);
static_assert(!util::quantities::concepts::is_interval_or_point_v<ObjectWithCategory>);
static_assert(
  util::quantities::concepts::is_interval_or_point_v
    <util::quantities::intervals::seconds>
  );
static_assert(
  util::quantities::concepts::is_interval_or_point_v
    <util::quantities::points::second>
  );


//------------------------------------------------------------------------------
//--- Interval tests
//------------------------------------------------------------------------------
void test_interval_construction() {

  using namespace util::quantities::time_literals;
 
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  microseconds ct0; // can't be constant since it's uninitialized
  (void) ct0; // don't let this fall unused
  
  constexpr microseconds ct1 { 2.0 };
  static_assert(ct1.value() == 2.0);
  
  constexpr microseconds ct2 { +6.0_us };
  static_assert(ct2.value() == 6.0);
  
  constexpr microseconds ct3 { -4000.0_ns };
  static_assert(ct3.value() == -4.0);
  
} // void test_interval_construction()


// -----------------------------------------------------------------------------
void test_interval_comparisons() {
  
  using namespace util::quantities::time_literals;
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  constexpr microseconds const t1 {         +6.0_us };
  constexpr nanoseconds  const t2 { -4'000'000.0_ps };
  
  constexpr microseconds const t1s = t1;

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
  
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS  
  
  static_assert(  t1      == 6       );
  static_assert(  6       == t1      );
  static_assert(!(t1      == 5      ));
  static_assert(!(5       == t1     ));
  
  static_assert(!(t1      != 6      ));
  static_assert(!(6       != t1     ));
  static_assert(  t1      != 5       );
  static_assert(  5       != t1      );
  
  static_assert(  t1      >= 6       );
  static_assert(  6       >= t1      );
  static_assert(  t1      >= 5       );
  static_assert(!(5       >= t1     ));
  static_assert(!(t1      >= 8      ));
  static_assert(  8       >= t1      );
  
  static_assert(!(t1      >  6      ));
  static_assert(!(6       >  t1     ));
  static_assert(  t1      >  5       );
  static_assert(!(5       >  t1     ));
  static_assert(!(t1      >  8      ));
  static_assert(  8       >  t1      );
  
  static_assert(  t1      <= 6       );
  static_assert(  6       <= t1      );
  static_assert(!(t1      <= 5      ));
  static_assert(  5       <= t1      );
  static_assert(  t1      <= 8       );
  static_assert(!(8       <= t1     ));
  
  static_assert(!(t1      <  6      ));
  static_assert(!(6       <  t1     ));
  static_assert(!(t1      <  5      ));
  static_assert(  5       <  t1      );
  static_assert(  t1      <  8       );
  static_assert(!(8       <  t1     ));
  
#endif // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS  
  
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
  
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION  
  // implicit conversion
  constexpr double const scalar_t1 = t1;
  static_assert(scalar_t1 ==     6.0); // in microseconds
  constexpr double const scalar_t2 = t2;
  static_assert(scalar_t2 == -4000.0); // in nanoseconds
#endif // LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION  
  
  
} // test_interval_queries()


// -----------------------------------------------------------------------------
void test_interval_operations() {

  using namespace util::quantities::time_literals;
 
  using util::quantities::intervals::microseconds;
  using util::quantities::intervals::nanoseconds;
  
  constexpr microseconds ct1 {    +6.0_us };
  constexpr microseconds ct2 { -4000.0_ns };
  
  static_assert(std::is_same_v<decltype(ct1 + -4000.0_ns), microseconds>);
  static_assert(ct1 + -4000.0_ns == 2_us);

  static_assert(std::is_same_v<decltype(ct1 + ct2), microseconds>);
  static_assert(ct1 + ct2 == 2_us);

  static_assert(std::is_same_v<decltype(ct1 - -4000.0_ns), microseconds>);
  static_assert(ct1 - -4000.0_ns == 10_us);

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
  BOOST_TEST((t1 == 8_us));
  BOOST_TEST((&incr == &t1));
  
  decltype(auto) incr2 = (t1 += t2);
  static_assert(std::is_same_v<decltype(incr2), microseconds&>);
  BOOST_TEST((t1 ==  4_us));
  BOOST_TEST((t2 == -4_us));
  BOOST_TEST((&incr2 == &t1));
  
  decltype(auto) decr = (t1 -= 2000_ns);
  static_assert(std::is_same_v<decltype(decr), microseconds&>);
  BOOST_TEST((t1 == 2_us));
  BOOST_TEST((&decr == &t1));
  
  decltype(auto) decr2 = (t1 -= t2);
  static_assert(std::is_same_v<decltype(decr2), microseconds&>);
  BOOST_TEST((t1 ==  6_us));
  BOOST_TEST((t2 == -4_us));
  BOOST_TEST((&decr2 == &t1));
  
  decltype(auto) expand = (t1 *= 2);
  static_assert(std::is_same_v<decltype(expand), microseconds&>);
  BOOST_TEST((t1 == 12_us));
  BOOST_TEST((&expand == &t1));
  
  decltype(auto) shrink = (t1 /= 2);
  static_assert(std::is_same_v<decltype(shrink), microseconds&>);
  BOOST_TEST((t1 == 6_us));
  BOOST_TEST((&shrink == &t1));
  
} // test_interval_operations()


// -----------------------------------------------------------------------------
// --- Point tests
// -----------------------------------------------------------------------------
void test_point_construction() {

  using namespace util::quantities::time_literals;
 
  using util::quantities::points::microsecond;
  using util::quantities::points::nanosecond;
  
  microsecond ct0;
  (void) ct0; // don't let this fall unused
  
  constexpr microsecond ct1 { 2.0 };
  static_assert(ct1.value() == 2.0);
  
  constexpr microsecond ct2 {    +6.0_us };
  static_assert(ct2.value() == 6.0);
  
  constexpr microsecond ct3 { -4000.0_ns };
  static_assert(ct3.value() == -4.0);
  
} // void test_point_construction()


// -----------------------------------------------------------------------------
void test_point_comparisons() {
  using namespace util::quantities::time_literals;
  using util::quantities::points::microsecond;
  using util::quantities::points::nanosecond;
  
  constexpr microsecond const t1 {         +6.0_us };
  constexpr nanosecond  const t2 { -4'000'000.0_ps };
  
  constexpr microsecond const t1s = t1;

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
  
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS  
  
  static_assert(  t1      == 6       );
  static_assert(  6       == t1      );
  static_assert(!(t1      == 5      ));
  static_assert(!(5       == t1     ));
  
  static_assert(!(t1      != 6      ));
  static_assert(!(6       != t1     ));
  static_assert(  t1      != 5       );
  static_assert(  5       != t1      );
  
  static_assert(  t1      >= 6       );
  static_assert(  6       >= t1      );
  static_assert(  t1      >= 5       );
  static_assert(!(5       >= t1     ));
  static_assert(!(t1      >= 8      ));
  static_assert(  8       >= t1      );
  
  static_assert(!(t1      >  6      ));
  static_assert(!(6       >  t1     ));
  static_assert(  t1      >  5       );
  static_assert(!(5       >  t1     ));
  static_assert(!(t1      >  8      ));
  static_assert(  8       >  t1      );
  
  static_assert(  t1      <= 6       );
  static_assert(  6       <= t1      );
  static_assert(!(t1      <= 5      ));
  static_assert(  5       <= t1      );
  static_assert(  t1      <= 8       );
  static_assert(!(8       <= t1     ));
  
  static_assert(!(t1      <  6      ));
  static_assert(!(6       <  t1     ));
  static_assert(!(t1      <  5      ));
  static_assert(  5       <  t1      );
  static_assert(  t1      <  8       );
  static_assert(!(8       <  t1     ));
  
#endif // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS  
  
} // test_point_comparisons()


// -----------------------------------------------------------------------------
void test_point_queries() {
  
  using namespace util::quantities::time_literals;
 
  using util::quantities::points::microsecond;
  using util::quantities::points::nanosecond;
  
  constexpr microsecond t1 {         +6.0_us };
  constexpr nanosecond  t2 { -4'000'000.0_ps };
  
  static_assert(std::is_same_v<microsecond::quantity_t, util::quantities::microsecond>);
  static_assert(std::is_same_v<microsecond::unit_t, util::quantities::microsecond::unit_t>);
  
  static_assert(t1.quantity() ==  6_us   );
  static_assert(t2.quantity() == -4_us   );
  static_assert(t1.value()    ==     6.0 );
  static_assert(t2.value()    == -4000.0 );
  
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION  
  // implicit conversion
  constexpr double const scalar_t1 = t1;
  static_assert(scalar_t1 ==     6.0); // in microseconds
  constexpr double const scalar_t2 = t2;
  static_assert(scalar_t2 == -4000.0); // in nanoseconds
#endif // LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION  
  
} // test_points_queries()


// -----------------------------------------------------------------------------
void test_point_operations() {

  using namespace util::quantities::time_literals;
 
  using util::quantities::points::microsecond;
  using util::quantities::points::nanosecond;
  
  constexpr microsecond cp1 {    +6.0_us };
  constexpr util::quantities::intervals::microseconds ct { 3.0_us };
  
  static_assert(std::is_same_v<decltype(cp1 + 3000_ns), microsecond>);
  static_assert(cp1 + 3000_ns == 9_us);

  static_assert(std::is_same_v<decltype(cp1 + ct), microsecond>);
  static_assert(cp1 + ct == 9_us);

  static_assert(std::is_same_v<decltype(cp1 - 3000_ns), microsecond>);
  static_assert(cp1 - 3000_ns == 3_us);

  static_assert(std::is_same_v<decltype(cp1 - ct), microsecond>);
  static_assert(cp1 - ct == 3_us);

  static_assert(std::is_same_v<decltype(+cp1), microsecond>);
  static_assert(+cp1 == 6_us);

  static_assert(std::is_same_v<decltype(-cp1), microsecond>);
  static_assert(-cp1 == -6_us);

  microsecond p1 {    +6.0_us };
  microsecond p2 { -4000.0_ns };
  util::quantities::intervals::nanoseconds t { 3000.0_ns };

  decltype(auto) incr = (p1 += 2000_ns);
  static_assert(std::is_same_v<decltype(incr), microsecond&>);
  BOOST_TEST((p1 == 8_us));
  BOOST_TEST((&incr == &p1));
  
  decltype(auto) incr2 = (p1 += t);
  static_assert(std::is_same_v<decltype(incr2), microsecond&>);
  BOOST_TEST((p1 == 11_us));
  BOOST_TEST((t ==   3_us));
  BOOST_TEST((&incr2 == &p1));
  
  decltype(auto) decr = (p1 -= 2000_ns);
  static_assert(std::is_same_v<decltype(decr), microsecond&>);
  BOOST_TEST((p1 == 9_us));
  BOOST_TEST((&decr == &p1));
  
  decltype(auto) decr2 = (p1 -= t);
  static_assert(std::is_same_v<decltype(decr2), microsecond&>);
  BOOST_TEST((p1 ==  6_us));
  BOOST_TEST((t ==   3_us));
  BOOST_TEST((&decr2 == &p1));
  
  decltype(auto) diff = (p1 - p2);
  static_assert
   (std::is_same_v<decltype(diff), util::quantities::intervals::microseconds>);
  BOOST_TEST((p1 ==  6_us));
  BOOST_TEST((p2 == -4_us));
  BOOST_TEST((diff == 10_us));
  
} // test_point_operations()


// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(intervals_testcase) {

  test_interval_construction();
  test_interval_queries();
  test_interval_comparisons();
  test_interval_operations();

} // BOOST_AUTO_TEST_CASE(intervals_testcase)

// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(points_testcase) {

  test_point_construction();
  test_point_queries();
  test_point_comparisons();
  test_point_operations();

} // BOOST_AUTO_TEST_CASE(points_testcase)

// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
