/**
 * @file   test/Utilities/quantities_fhicl_test.cc
 * @brief  Unit test for `quantities_fhicl.h` header
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 27, 2019
 * @see    lardataalg/Utilities/quantities_fhicl.h
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE ( quantities_fhicl_test )
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/quantities/spacetime.h"
#include "lardataalg/Utilities/quantities_fhicl.h"

// support libraries
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/ParameterSet.h"

// C/C++ standard libraries
#include <type_traits> // std::is_same_v<>


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename Config>
fhicl::Table<Config> validateConfig(fhicl::ParameterSet const& pset) {
  fhicl::Table<Config> validatedConfig { fhicl::Name("validatedConfig") };
  
  std::cout << std::string(80, '-') << std::endl;
  std::cout << "===> FHiCL configuration:";
  if (pset.is_empty()) std::cout << " <empty>";
  else                 std::cout << "\n" << pset.to_indented_string();
  std::cout << std::endl;
  validatedConfig.print_allowed_configuration
    (std::cout << "===> Expected configuration: ");
  std::cout << std::endl;
  
  validatedConfig.validate_ParameterSet(pset);
  return validatedConfig;
} // validateConfig()


// -----------------------------------------------------------------------------
template <typename Config>
fhicl::Table<Config> validateConfig(std::string const& configStr) {
  fhicl::ParameterSet pset;
  pset = fhicl::ParameterSet::make(configStr);
  return validateConfig<Config>(pset);
} // validateConfig(Config)


// -----------------------------------------------------------------------------
// --- Quantity tests
// -----------------------------------------------------------------------------
void test_makeQuantity() {

  using namespace std::string_view_literals;
  using namespace util::quantities::time_literals;
  using util::quantities::microsecond;

  auto t = util::quantities::makeQuantity<microsecond>("-7e1 ms"sv);
  static_assert(std::is_same_v<decltype(t), microsecond>);
  BOOST_TEST((t == -70000_us));
  BOOST_TEST((t == -70_ms));

  t = util::quantities::makeQuantity<microsecond>("7e1ms"sv);
  BOOST_TEST((t == 70000_us));
  BOOST_TEST((t == 70_ms));

  t = util::quantities::makeQuantity<microsecond>("7e1"sv, true);
  BOOST_TEST((t == 70_us));

  BOOST_CHECK_THROW(
    util::quantities::makeQuantity<microsecond>("7e1"sv),
    util::quantities::MissingUnit
    );

  BOOST_CHECK_THROW(
    util::quantities::makeQuantity<microsecond>("7g ms"sv),
    util::quantities::ExtraCharactersError
    );

  BOOST_CHECK_THROW(
    util::quantities::makeQuantity<microsecond>("g7 ms"sv),
    util::quantities::ValueError
    );

  BOOST_CHECK_THROW(
    util::quantities::makeQuantity<microsecond>(""sv),
    util::quantities::MissingUnit
    );

  BOOST_CHECK_THROW(
    util::quantities::makeQuantity<microsecond>(""sv, true),
    util::quantities::ValueError
    );

} // test_makeQuantity()


// -----------------------------------------------------------------------------
void test_readQuantity() {
  
  using namespace util::quantities::time_literals;
  using util::quantities::microsecond;
  
  struct Config {
    
    fhicl::Atom<util::quantities::microsecond> start
      { fhicl::Name("start"), 0_us };
    
    fhicl::Atom<util::quantities::microsecond> end
      { fhicl::Name("end"), 6_ms };
    
  }; // struct Config
  
  std::string const configStr { "end: 16ms" };
  util::quantities::microsecond const expectedStart = 0_us;
  util::quantities::microsecond const expectedEnd = 16_ms;
  
  auto validatedConfig = validateConfig<Config>(configStr)();
  
  auto start = validatedConfig.start();
  static_assert(std::is_same_v<decltype(start), util::quantities::microsecond>);
  BOOST_TEST((start == expectedStart));
  
  auto end = validatedConfig.end();
  static_assert(std::is_same_v<decltype(end), util::quantities::microsecond>);
  BOOST_TEST((end == expectedEnd));
  
} // test_readQuantity()


// -----------------------------------------------------------------------------
void test_writeQuantity() {
  
  using namespace util::quantities::time_literals;
  using util::quantities::microsecond;
  
  struct Config {
    
    fhicl::Atom<util::quantities::microsecond> start
      { fhicl::Name("start"), 0_us };
    
    fhicl::Atom<util::quantities::microsecond> end
      { fhicl::Name("end"), 6_ms };
    
  }; // struct Config
  
  fhicl::ParameterSet pset;
  pset.put<util::quantities::microsecond>("end", 16_ms);
  util::quantities::microsecond const expectedStart = 0_us;
  util::quantities::microsecond const expectedEnd = 16_ms;
  
  auto validatedConfig = validateConfig<Config>(pset)();
  
  auto start = validatedConfig.start();
  static_assert(std::is_same_v<decltype(start), util::quantities::microsecond>);
  BOOST_TEST((start == expectedStart));
  
  auto end = validatedConfig.end();
  static_assert(std::is_same_v<decltype(end), util::quantities::microsecond>);
  BOOST_TEST((end == expectedEnd));
  
} // test_writeQuantity()


// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(quantities_testcase) {

  test_makeQuantity();

} // BOOST_AUTO_TEST_CASE(quantities_testcase)

// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(quantities_fhicl_testcase) {

  test_readQuantity();
  test_writeQuantity();

} // BOOST_AUTO_TEST_CASE(quantities_fhicl_testcase)

// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
