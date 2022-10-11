/**
 * @file   test/Utilities/intervals_fhicl_test.cc
 * @brief  Unit test for `intervals_fhicl.h` header
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 27, 2019
 * @see    lardataalg/Utilities/intervals_fhicl.h
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE (intervals_fhicl_test)
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/intervals_fhicl.h"
#include "lardataalg/Utilities/quantities/spacetime.h"
#include "test/Utilities/disable_boost_fpc_tolerance.hpp"

// support libraries
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"

// C/C++ standard libraries
#include <type_traits> // std::is_same_v<>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename Config>
Config validateConfig(fhicl::ParameterSet const& pset)
{
  fhicl::Table<Config> validatedConfig{fhicl::Name("validatedConfig")};

  std::cout << std::string(80, '-') << std::endl;
  std::cout << "===> FHiCL configuration:";
  if (pset.is_empty())
    std::cout << " <empty>";
  else
    std::cout << "\n" << pset.to_indented_string();
  std::cout << std::endl;
  validatedConfig.print_allowed_configuration(std::cout << "===> Expected configuration: ");
  std::cout << std::endl;

  validatedConfig.validate_ParameterSet(pset);
  return validatedConfig();
} // validateConfig()

// -----------------------------------------------------------------------------
template <typename Config>
Config validateConfig(std::string const& configStr)
{
  return validateConfig<Config>(fhicl::ParameterSet::make(configStr));
} // validateConfig(Config)

// -----------------------------------------------------------------------------
// --- Interval tests
// -----------------------------------------------------------------------------
void test_makeInterval()
{

  using namespace std::string_view_literals;
  using namespace util::quantities::time_literals;
  using util::quantities::intervals::microseconds;

  auto t = util::quantities::makeInterval<microseconds>("-7e1 ms"sv);
  static_assert(std::is_same_v<decltype(t), microseconds>);
  BOOST_TEST(t == -70000_us);
  BOOST_TEST(t == -70_ms);

  t = util::quantities::makeInterval<microseconds>("7e1ms"sv);
  BOOST_TEST(t == 70000_us);
  BOOST_TEST(t == 70_ms);

  t = util::quantities::makeInterval<microseconds>("7e1"sv, true);
  BOOST_TEST(t == 70_us);

  BOOST_CHECK_THROW(util::quantities::makeInterval<microseconds>("7e1"sv),
                    util::quantities::MissingUnit);

  BOOST_CHECK_THROW(util::quantities::makeInterval<microseconds>("7g ms"sv),
                    util::quantities::ExtraCharactersError);

  BOOST_CHECK_THROW(util::quantities::makeInterval<microseconds>("g7 ms"sv),
                    util::quantities::ValueError);

  BOOST_CHECK_THROW(util::quantities::makeInterval<microseconds>(""sv),
                    util::quantities::MissingUnit);

  BOOST_CHECK_THROW(util::quantities::makeInterval<microseconds>(""sv, true),
                    util::quantities::ValueError);

} // test_makeInterval()

// -----------------------------------------------------------------------------
void test_makePoint()
{

  using namespace std::string_view_literals;
  using namespace util::quantities::time_literals;
  using util::quantities::points::microsecond;

  auto t = util::quantities::makePoint<microsecond>("-7e1 ms"sv);
  static_assert(std::is_same_v<decltype(t), microsecond>);
  BOOST_TEST(t == -70000_us);
  BOOST_TEST(t == -70_ms);

  t = util::quantities::makePoint<microsecond>("7e1ms"sv);
  BOOST_TEST(t == 70000_us);
  BOOST_TEST(t == 70_ms);

  t = util::quantities::makePoint<microsecond>("7e1"sv, true);
  BOOST_TEST(t == 70_us);

  BOOST_CHECK_THROW(util::quantities::makePoint<microsecond>("7e1"sv),
                    util::quantities::MissingUnit);

  BOOST_CHECK_THROW(util::quantities::makePoint<microsecond>("7g ms"sv),
                    util::quantities::ExtraCharactersError);

  BOOST_CHECK_THROW(util::quantities::makePoint<microsecond>("g7 ms"sv),
                    util::quantities::ValueError);

  BOOST_CHECK_THROW(util::quantities::makePoint<microsecond>(""sv), util::quantities::MissingUnit);

  BOOST_CHECK_THROW(util::quantities::makePoint<microsecond>(""sv, true),
                    util::quantities::ValueError);

} // test_makePoint()

// -----------------------------------------------------------------------------
void test_read()
{

  using namespace util::quantities::time_literals;

  struct Config {

    fhicl::Atom<util::quantities::points::microsecond> start{fhicl::Name("start"), 0_us};

    fhicl::Atom<util::quantities::points::microsecond> end{fhicl::Name("end"), 6_ms};

    fhicl::Atom<util::quantities::intervals::microseconds> duration{fhicl::Name("duration"), 6_ms};

  }; // struct Config

  std::string const configStr{"start: 2ms duration: 16ms"};
  util::quantities::points::microsecond const expectedStart{2_ms};
  util::quantities::points::microsecond const expectedEnd{6_ms};
  util::quantities::intervals::microseconds const expectedDuration{16_ms};

  auto validatedConfig = validateConfig<Config>(configStr);
  BOOST_TEST(validatedConfig.start() == expectedStart);
  BOOST_TEST(validatedConfig.end() == expectedEnd);
  BOOST_TEST(validatedConfig.duration() == expectedDuration);

} // test_read()

// -----------------------------------------------------------------------------
void test_write()
{

  using namespace util::quantities::time_literals;
  struct Config {

    fhicl::Atom<util::quantities::points::microsecond> start{fhicl::Name("start"), 0_us};

    fhicl::Atom<util::quantities::points::microsecond> end{fhicl::Name("end"), 6_ms};

    fhicl::Atom<util::quantities::intervals::microseconds> duration{fhicl::Name("duration"), 6_ms};

  }; // struct Config

  util::quantities::points::microsecond const expectedStart{2_ms};
  util::quantities::points::microsecond const expectedEnd{6_ms};
  util::quantities::intervals::microseconds const expectedDuration{16_ms};

  fhicl::ParameterSet pset;
  pset.put("start", expectedStart);
  pset.put("duration", expectedDuration);

  auto validatedConfig = validateConfig<Config>(pset);
  BOOST_TEST(validatedConfig.start() == expectedStart);
  BOOST_TEST(validatedConfig.end() == expectedEnd);
  BOOST_TEST(validatedConfig.duration() == expectedDuration);

} // test_write()

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(intervals_fhicl_test)

BOOST_AUTO_TEST_CASE(intervals_testcase)
{
  test_makeInterval();
}

BOOST_AUTO_TEST_CASE(points_testcase)
{
  test_makePoint();
}

BOOST_AUTO_TEST_CASE(quantities_fhicl_testcase)
{
  test_read();
  test_write();
}

BOOST_AUTO_TEST_SUITE_END()
