/**
 * @file   test/Utilities/MultipleChoiceSelection_test.cc
 * @brief  Unit test for `intervals_fhicl.h` header
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 27, 2019
 * @see    lardataalg/Utilities/intervals_fhicl.h
 *
 */

// Boost libraries
#define BOOST_TEST_MODULE (MultipleChoiceSelection_test)
#include <boost/test/unit_test.hpp>

// LArSoft libraries
#include "lardataalg/Utilities/MultipleChoiceSelection.h"

// C/C++ standard libraries
#include <iostream>
#include <string>

// -----------------------------------------------------------------------------
void MultipleChoiceSelection_test()
{

  enum class Color { white, gray, black, blue };

  using OptionSelector_t = util::MultipleChoiceSelection<Color>;

  OptionSelector_t options{{Color::black, "black"}, {Color::gray, "gray", "grey"}};

  BOOST_TEST(options.size() == 2U);

  BOOST_CHECK_THROW(options.addAlias(Color::white, "blanche"), // Color::white not yet added
                    OptionSelector_t::UnknownOptionError);

  auto const& opWhite0 = options.addOption(Color::white, "white");
  auto const& opWhite0again = options.addAlias(Color::white, "blanche");
  BOOST_TEST(&opWhite0again == &opWhite0);
  BOOST_TEST(options.size() == 3U);

  std::cout << "Options:\n" << options.optionListDump(" * ") << std::endl;

  BOOST_TEST(options.hasOption(Color::white));
  BOOST_TEST(options.hasOption(Color::gray));
  BOOST_TEST(!options.hasOption(Color::blue));
  BOOST_TEST(options.hasOption("white"));
  BOOST_TEST(options.hasOption("blanche"));
  BOOST_TEST(options.hasOption("wHite"));
  BOOST_TEST(!options.hasOption("blue"));

  //
  // white
  //
  BOOST_TEST(options.hasOption(opWhite0));

  auto const opWhite1 = options.parse("white");
  static_assert(std::is_same_v<std::decay_t<decltype(opWhite1)>, OptionSelector_t::Option_t>);
  BOOST_TEST((opWhite1 == Color::white));
  BOOST_TEST(opWhite1 == "white");
  BOOST_TEST(opWhite1 == opWhite0);

  auto const opWhite2 = options.parse("blanche");
  static_assert(std::is_same_v<std::decay_t<decltype(opWhite2)>, OptionSelector_t::Option_t>);
  BOOST_TEST((opWhite2 == Color::white));
  BOOST_TEST(opWhite2 == "white");
  BOOST_TEST(opWhite2 == "blanche");
  BOOST_TEST(opWhite2 == "Blanche");
  BOOST_TEST(opWhite2 == opWhite0);

  auto const opWhite3 = options.get("white");
  static_assert(std::is_same_v<std::decay_t<decltype(opWhite1)>, OptionSelector_t::Option_t>);
  BOOST_TEST(opWhite3 == "white");

  //
  // gray
  //
  auto const opGray0 = options.get("gray");
  static_assert(std::is_same_v<std::decay_t<decltype(opWhite1)>, OptionSelector_t::Option_t>);
  BOOST_TEST(opWhite0 == "white");

  auto const opGray1 = options.parse("gray");
  static_assert(std::is_same_v<std::decay_t<decltype(opWhite1)>, OptionSelector_t::Option_t>);
  BOOST_TEST((opGray1 == Color::gray));
  BOOST_TEST(opGray1 == "gray");
  BOOST_TEST(opGray1 == "grey");
  BOOST_TEST(opGray1 == opGray0);

  Color color = opGray1;
  BOOST_TEST((color == Color::gray));

  //
  // blue
  //
  BOOST_CHECK_THROW(options.get("blue"), OptionSelector_t::UnknownOptionError);
  BOOST_CHECK_THROW(options.parse("blue"), OptionSelector_t::UnknownOptionError);

} // MultipleChoiceSelection_testcase()

// -----------------------------------------------------------------------------
// BEGIN Test cases  -----------------------------------------------------------
// -----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(MultipleChoiceSelection_testcase)
{

  MultipleChoiceSelection_test();

} // BOOST_AUTO_TEST_CASE(MultipleChoiceSelection_testcase)

// -----------------------------------------------------------------------------
// END Test cases  -------------------------------------------------------------
// -----------------------------------------------------------------------------
