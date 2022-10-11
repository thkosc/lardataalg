/**
 * @file   test/Utilities/space_test.cc
 * @brief  Unit test for `lardataalg/Utilities/quantities/spacetime.h` header.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   January 13, 2020
 * @see    `lardataalg/Utilities/quantities.h`
 *
 * This test covers only the space units of `spacetime.h`.
 * It's not overwhelmingly complete, either.
 */

// Boost libraries
#define BOOST_TEST_MODULE (space_test)
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/quantities/spacetime.h"

// C/C++ standard libraries
#include <iostream>
#include <type_traits> // std::decay_t<>

// -----------------------------------------------------------------------------
void test_meter_literals()
{

  using namespace util::quantities::space_literals;

  //
  // NOTE: the test with physics constants is cool, but floating point
  //       arithmetics having finite precision, static tests based on equality
  //       may fail... if that happens, my magic recipe is... use less digits.
  //       Wow.
  //

  constexpr auto l_m = 299792458_m; // space covered by a photon in vacuum in 1"
  static_assert(std::is_same<decltype(l_m), util::quantities::meter const>());
  static_assert(l_m.value() == 299792458);
  static_assert(l_m == 299792458_m);
  std::cout << "Tested " << l_m << std::endl;

  constexpr auto l_km = 299792.458_km;
  static_assert(std::is_same<decltype(l_km), util::quantities::kilometer const>());
  static_assert(l_km == 299792.458_km);
  static_assert(l_km.value() == 299792.458);
  static_assert(l_km == 299792458_m);
  std::cout << "Tested " << l_km << std::endl;

  constexpr auto l_cm = 29979245800_cm;
  static_assert(std::is_same<decltype(l_cm), util::quantities::centimeter const>());
  static_assert(l_cm == 29979245800_cm);
  static_assert(l_cm.value() == 29979245800);
  static_assert(l_cm == 299792458_m);
  std::cout << "Tested " << l_cm << std::endl;

  constexpr auto l_mm = 299792458000_mm;
  static_assert(std::is_same<decltype(l_mm), util::quantities::millimeter const>());
  static_assert(l_mm == 299792458000_mm);
  static_assert(l_mm.value() == 299792458000);
  static_assert(l_mm == 299792458_m);
  std::cout << "Tested " << l_mm << std::endl;

  constexpr auto l_um = 1063_um; // dE/dl peak of CMB according to Wikipedia
  static_assert(std::is_same<decltype(l_um), util::quantities::micrometer const>());
  static_assert(l_um == 1063_um);
  static_assert(l_um.value() == 1063);
  static_assert(l_um == 1.063_mm);
  std::cout << "Tested " << l_um << std::endl;

  constexpr auto l_nm = 1063000_nm;
  static_assert(std::is_same<decltype(l_nm), util::quantities::nanometer const>());
  static_assert(l_nm == 1063000_nm);
  static_assert(l_nm.value() == 1063000);
  static_assert(l_nm == 1063_um);
  std::cout << "Tested " << l_nm << std::endl;

  constexpr auto l_pm = 1063000000_pm;
  static_assert(std::is_same<decltype(l_pm), util::quantities::picometer const>());
  static_assert(l_pm == 1063000000_pm);
  static_assert(l_pm.value() == 1063000000);
  static_assert(l_pm == 1063000_nm);
  std::cout << "Tested " << l_pm << std::endl;

  constexpr auto l_fm = 52918_fm; // Bohr radius (rounded)
  static_assert(std::is_same<decltype(l_fm), util::quantities::femtometer const>());
  static_assert(l_fm == 52918_fm);
  static_assert(l_fm.value() == 52918);
  static_assert(l_fm == 52.918_pm);
  std::cout << "Tested " << l_fm << std::endl;

} // test_meter_literals()

// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(meter_testcase)
{

  test_meter_literals();

} // BOOST_AUTO_TEST_CASE(meter_testcase)

// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
