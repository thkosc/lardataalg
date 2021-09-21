/**
 * @file   test/Utilities/energy_test.cc
 * @brief  Unit test for `lardataalg/Utilities/quantities/energy.h` header.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   January 10, 2020
 * @see    `lardataalg/Utilities/quantities.h`
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE ( energy_test )
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/quantities/energy.h"

// C/C++ standard libraries
#include <iostream>
#include <type_traits> // std::decay_t<>


// -----------------------------------------------------------------------------
void test_electronvolt_literals() {

  using namespace util::quantities::energy_literals;
  
  //
  // NOTE: the test with physics constants is cool, but floating point
  //       arithmetics having finite precision, static tests based on equality
  //       may fail... if that happens, my magic recipe is... use less digits.
  //       Wow.
  //

  constexpr auto E_eV = 13.6_eV; // hydrogen binding energy
  static_assert
    (std::is_same<decltype(E_eV), util::quantities::electronvolt const>());
  static_assert(E_eV.value() == 13.6);
  static_assert(E_eV == 13.6_eV);
  std::cout << "Tested " << E_eV << std::endl;

  constexpr auto E_meV = 34.8_meV; // room temperature (300 K)
  static_assert
   (std::is_same<decltype(E_meV), util::quantities::millielectronvolt const>());
  static_assert(E_meV == 34.8_meV);
  static_assert(E_meV.value() == 34.8);
  static_assert(E_meV == 0.0348_eV);
  std::cout << "Tested " << E_meV << std::endl;

  constexpr auto E_ueV = 662.6_ueV; // CMB temperature
  static_assert
   (std::is_same<decltype(E_ueV), util::quantities::microelectronvolt const>());
  static_assert(E_ueV == 662.6_ueV);
  static_assert(E_ueV.value() == 662.6);
  static_assert(E_ueV == 0.6626_meV);
  std::cout << "Tested " << E_ueV << std::endl;

  constexpr auto E_keV = 510.99893_keV; // electron mass
  static_assert
    (std::is_same<decltype(E_keV), util::quantities::kiloelectronvolt const>());
  static_assert(E_keV == 510.99893_keV);
  static_assert(E_keV.value() == 510.99893);
  static_assert(E_keV == 0.51099893_MeV);
  std::cout << "Tested " << E_keV << std::endl;

  constexpr auto E_MeV = 2.22452_MeV; // deuteron binding energy
  static_assert
    (std::is_same<decltype(E_MeV), util::quantities::megaelectronvolt const>());
  static_assert(E_MeV == 2.22452_MeV);
  static_assert(E_MeV.value() == 2.22452);
  static_assert(E_MeV == 2224.52_keV);
  std::cout << "Tested " << E_MeV << std::endl;

  constexpr auto E_GeV = 91.1876_GeV; // Z boson mass
  static_assert
    (std::is_same<decltype(E_GeV), util::quantities::gigaelectronvolt const>());
  static_assert(E_GeV == 91.1876_GeV);
  static_assert(E_GeV.value() == 91.1876);
  static_assert(E_GeV == 91187.6_MeV);
  std::cout << "Tested " << E_GeV << std::endl;

  constexpr auto E_TeV = 0.980_TeV; // Tevatron beam energy
  static_assert
    (std::is_same<decltype(E_TeV), util::quantities::teraelectronvolt const>());
  static_assert(E_TeV == 0.980_TeV);
  static_assert(E_TeV.value() == 0.980);
  static_assert(E_TeV == 980_GeV);
  std::cout << "Tested " << E_TeV << std::endl;

} // test_electronvolt_literals()


// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(electronvolt_testcase) {

  test_electronvolt_literals();

} // BOOST_AUTO_TEST_CASE(electronvolt_testcase)


// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
