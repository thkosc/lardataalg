/**
 * @file   DetectorClocksStandard_test.cc
 * @brief  Simple instantiation-only test for DetectorClocksStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 6th, 2016
 */

// LArSoft libraries
#include "larcorealg/TestUtils/unit_test_base.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandard.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandardTestHelpers.h"

//------------------------------------------------------------------------------
//---  The test environment
//---

/*
 * TesterEnvironment, configured with a "standard" configuration object, is used
 * in a non-Boost-unit-test context.
 * It provides:
 *
 */
using TestEnvironment = testing::TesterEnvironment<testing::BasicEnvironmentConfiguration>;

//------------------------------------------------------------------------------
//---  The tests
//---

/** ****************************************************************************
 * @brief Runs the test
 * @param argc number of arguments in argv
 * @param argv arguments to the function
 * @return number of detected errors (0 on success)
 * @throw cet::exception most of error situations throw
 *
 * The arguments in argv are:
 * 0. name of the executable ("DetectorClocksStandard_test")
 * 1. (mandatory) path to the FHiCL configuration file
 * 2. FHiCL path to the configuration of the test
 *    (default: physics.analyzers.larptest)
 * 3. FHiCL path to the configuration of DetectorClocks service
 *    (default: services.DetectorClocksService)
 *
 */
//------------------------------------------------------------------------------
int
main(int argc, char const** argv)
{

  testing::BasicEnvironmentConfiguration config("clocks_test");

  //
  // parameter parsing
  //
  int iParam = 0;

  // first argument: configuration file (mandatory)
  if (++iParam < argc)
    config.SetConfigurationPath(argv[iParam]);
  else {
    std::cerr << "FHiCL configuration file path required as first argument!" << std::endl;
    return 1;
  }

  // second argument: path of the parameter set for geometry test configuration
  // (optional; default does not have any tester)
  if (++iParam < argc) config.SetMainTesterParameterSetPath(argv[iParam]);

  // third argument: path of the parameter set for DetectorClocks configuration
  // (optional; default: "services.DetectorClocks" from the inherited object)
  if (++iParam < argc) config.SetServiceParameterSetPath("DetectorClocksService", argv[iParam]);

  unsigned int nErrors = 0 /* Tester.Run() */;

  //
  // testing environment setup
  //
  TestEnvironment TestEnv(config);

  // DetectorClocksStandard supports the simple set up; so we invoke it
  TestEnv.SimpleProviderSetup<detinfo::DetectorClocksStandard>();

  // Note that here we are querying the abstract DetectorClocks interface;
  // this is the right way to go.
  auto const* detClocks = TestEnv.Provider<detinfo::DetectorClocks>();
  auto const& detClocksData = detClocks->DataForJob();
  mf::LogVerbatim("clocks_test")
    << "TPC clock frame: " << detClocksData.TPCClock().FramePeriod() << " us";

  detClocksData.debugReport(std::cout);

  // 4. And finally we cross fingers.
  if (nErrors > 0) { mf::LogError("clocks_test") << nErrors << " errors detected!"; }

  return nErrors;
} // main()
