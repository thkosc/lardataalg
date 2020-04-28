/**
 * @file   DetectorTimingsStandard_test.cc
 * @brief  Test of `detinfo::DetectorTimings` with `DetectorClocksStandard`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   June 27, 2019
 */

// LArSoft libraries
#include "larcorealg/CoreUtils/DebugUtils.h" // lar::debug::static_assert_on<>()
#include "larcorealg/CoreUtils/MetaUtils.h"  // util::is_same_decay_v
#include "larcorealg/CoreUtils/RealComparisons.h"
#include "larcorealg/TestUtils/unit_test_base.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandard.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandardTestHelpers.h"
#include "lardataalg/DetectorInfo/DetectorTimingTypes.h"
#include "lardataalg/DetectorInfo/DetectorTimings.h"
#include "lardataalg/Utilities/quantities/frequency.h" // megahertz
#include "lardataalg/Utilities/quantities/spacetime.h" // microsecond, ...

// framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h" // mf namespace

//------------------------------------------------------------------------------
//---  The test environment
//---

/*
 * TesterEnvironment, configured with a "standard" configuration object, is used
 * in a non-Boost-unit-test context.
 * It provides:
 * - `detinfo::DetectorClocks const* Provider<detinfo::DetectorClocks>()`
 *
 */
using TestEnvironment = testing::TesterEnvironment<testing::BasicEnvironmentConfiguration>;

//------------------------------------------------------------------------------
//---  The tests
//---
unsigned int
testTriggerTime(detinfo::DetectorTimings const& timings)
{

  unsigned int nErrors = 0U;

  //
  // trigger time
  //
  double const expectedTime = timings.detClocks().TriggerTime(); // us

  // `TriggerTime()` returns a time in electronics time scale [us]
  auto const time = timings.TriggerTime();
  static_assert(util::is_same_decay_v<decltype(time.quantity()), util::quantities::microsecond>);

  if (time.value() == expectedTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test") << "DetectorTimings::TriggerTime() => " << time;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Trigger time expected to be " << expectedTime << " us in electronics time, but got "
      << time << " instead";
  }

  return nErrors;
} // testTriggerTime()

//------------------------------------------------------------------------------
unsigned int
testBeamGateTime(detinfo::DetectorTimings const& timings)
{

  unsigned int nErrors = 0U;

  //
  // trigger time
  //
  double const expectedTime = timings.detClocks().BeamGateTime(); // us

  // `BeamGateTime()` returns a time in electronics time scale [us]
  auto const time = timings.BeamGateTime();
  static_assert(util::is_same_decay_v<decltype(time.quantity()), util::quantities::microsecond>);

  if (time.value() == expectedTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::BeamGateTime() => " << time;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Beam gate time expected to be " << expectedTime << " us in electronics time, but got "
      << time << " instead";
  }

  return nErrors;
} // testBeamGateTime()

//------------------------------------------------------------------------------
unsigned int
testSimulationTimes(detinfo::DetectorTimings const& timings)
{

  using namespace detinfo::timescales; // simulation_time, electronics_time, ...
  using util::quantities::microsecond;

  unsigned int nErrors = 0U;

  constexpr lar::util::RealComparisons cmp(1e-4);

  //
  // start time
  //

  // simulation time start in electronics time
  double const expectedStartTime_us = timings.detClocks().G4ToElecTime(0.0); // ns
  microsecond const expectedStartTime{expectedStartTime_us};

  // `startTime()` returns a time in electronics time scale [us]
  auto const startTime = timings.startTime<simulation_time>();
  static_assert(
    util::is_same_decay_v<decltype(startTime.quantity()), util::quantities::microsecond>);

  if (startTime == expectedStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<simulation_time>() => " << startTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Simulation time expected to start at " << expectedStartTime
      << " in electronics time, but got " << startTime << " instead";
  }

  // electronics time start in simulation time
  auto const elecStartTime = timings.startTime<electronics_time, simulation_time>();
  static_assert(
    util::is_same_decay_v<decltype(elecStartTime.quantity()), util::quantities::nanosecond>);

  if (elecStartTime == -expectedStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<electronics_time, simulation_time>() => " << elecStartTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Electronics time expected to start at " << (-expectedStartTime)
      << " in simulation time, but got " << elecStartTime << " instead";
  }

  //
  // time conversions
  //

  // to electronics time:
  double const inputTime_ns = 100.0;
  double const expectedTime = timings.detClocks().G4ToElecTime(inputTime_ns);

  simulation_time inputTime{inputTime_ns};
  static_assert(
    util::is_same_decay_v<decltype(inputTime.quantity()), util::quantities::nanosecond>);

  auto const time = timings.toElectronicsTime(inputTime);
  static_assert(util::is_same_decay_v<decltype(time.quantity()), util::quantities::microsecond>);

  if (time.value() == expectedTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toElectronicsTime<simulation_time>(" << inputTime << ") => " << time;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "A simulation time of " << inputTime << " is expected to be " << expectedTime
      << " us in electronics time, but got " << time << " instead";
  }

  // back to simulation time:

  auto const simulTime = timings.toSimulationTime(electronics_time{expectedTime});
  static_assert(
    util::is_same_decay_v<decltype(simulTime.quantity()), util::quantities::nanosecond>);

  if (simulTime.value() == inputTime_ns) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTimeScale<electronics_time>(" << expectedTime << ") => " << simulTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "An electronics time of " << expectedTime << " us is expected to be " << inputTime
      << " in simulation time, but got " << simulTime << " instead";
  }

  // to TPC electronics ticks:
  double const expectedTPCtick = timings.detClocks().TPCG4Time2Tick(inputTime_ns);

  auto const TPCtick = timings.toTick<TPCelectronics_tick_d>(inputTime);
  if (cmp.equal(TPCtick.value(), expectedTPCtick)) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTick<TPCelectronics_tick_d>(" << inputTime << ") => " << TPCtick;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "A simulation time of " << inputTime << " is expected to be at " << expectedTPCtick
      << " TPC electronics tick, but got " << TPCtick << " instead";
  }

  // let's try some more complex transformations...

  // which simulation times is the trigger time plus 100 TPC ticks?
  auto const trigTime = timings.TriggerTime();
  electronics_time_ticks const tickOffset{100};
  simulation_time const expectedSimulTick =
    timings.toTimeScale<simulation_time>(trigTime) +
    tickOffset.value() * timings.ClockPeriodFor<electronics_time>();
  auto const tick = timings.toTick<electronics_tick>(trigTime) + tickOffset;
  auto const simulTick = timings.toTimeScale<simulation_time>(tick);

  if (simulTick == expectedSimulTick) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTimeScale<simulation_time>(" << trigTime << " + " << tickOffset
      << " = " << tick << ") => " << simulTick;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "An offset of " << tickOffset << " on trigger time (" << trigTime << ") is expected to be "
      << expectedSimulTick << " in simulation time, but got " << simulTick << " instead";
  }

  return nErrors;
} // testSimulationTimes()

//------------------------------------------------------------------------------
unsigned int
testTriggerTimes(detinfo::DetectorTimings const& timings)
{

  using namespace detinfo::timescales; // trigger_time, electronics_time

  unsigned int nErrors = 0U;

  //
  // start time
  //

  // trigger time start in electronics time
  double const expectedStartTime = timings.detClocks().TriggerTime(); // us

  // `startTime()` returns a time in electronics time scale [us]
  auto const startTime = timings.startTime<trigger_time>();
  static_assert(
    util::is_same_decay_v<decltype(startTime.quantity()), util::quantities::microsecond>);

  if (startTime.value() == expectedStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<trigger_time>() => " << startTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Trigger time expected to start at " << expectedStartTime
      << " us in electronics time, but got " << startTime << " instead";
  }

  // electronics time start in trigger time
  auto const elecStartTime = timings.startTime<electronics_time, trigger_time>();
  static_assert(
    util::is_same_decay_v<decltype(elecStartTime.quantity()), util::quantities::microsecond>);

  if (elecStartTime.value() == -expectedStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<electronics_time, trigger_time>() => " << elecStartTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Electronics time expected to start at " << (-expectedStartTime)
      << " us in trigger time, but got " << elecStartTime << " instead";
  }

  //
  // time conversions
  //

  // to electronics time:
  double const inputTime_us = 100.0;
  double const expectedTime = timings.detClocks().TriggerTime() + inputTime_us;

  trigger_time inputTime{inputTime_us};
  static_assert(
    util::is_same_decay_v<decltype(inputTime.quantity()), util::quantities::microsecond>);

  auto const time = timings.toElectronicsTime(inputTime);
  static_assert(util::is_same_decay_v<decltype(time.quantity()), util::quantities::microsecond>);

  if (time.value() == expectedTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toElectronicsTime<trigger_time>(" << inputTime << ") => " << time;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "A trigger time of " << inputTime << " is expected to be " << expectedTime
      << " us in electronics time, but got " << time << " instead";
  }

  // back to trigger time:

  auto const trigTime = timings.toTriggerTime(electronics_time{expectedTime});
  static_assert(
    util::is_same_decay_v<decltype(trigTime.quantity()), util::quantities::microsecond>);

  if (trigTime.value() == inputTime_us) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTriggerTime<electronics_time>(" << expectedTime << ") => " << trigTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "An electronics time of " << expectedTime << " us is expected to be " << inputTime
      << " in trigger time, but got " << trigTime << " instead";
  }

  return nErrors;
} // testTriggerTimes()

//------------------------------------------------------------------------------
unsigned int
testOpticalClockTimings(detinfo::DetectorTimings const& timings)
{

  using namespace util::quantities::time_literals;
  using namespace detinfo::timescales; // optical_time, optical_tick

  unsigned int nErrors = 0U;

  //
  // clocks
  //

  // frequency
  double const expectedFrequency = timings.detClocks().OpticalClock().Frequency(); // MHz

  auto const frequency = timings.OpticalClockFrequency();
  static_assert(util::is_same_decay_v<decltype(frequency), util::quantities::megahertz>);

  if (frequency.value() == expectedFrequency) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::OpticalClockFrequency() => " << frequency;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Optical clock frequency expected to be " << expectedFrequency << " MHz, but got "
      << frequency << " instead";
  }

  // period
  double const expectedPeriod = timings.detClocks().OpticalClock().TickPeriod(); // MHz

  auto const period = timings.OpticalClockPeriod();
  static_assert(util::is_same_decay_v<decltype(period)::quantity_t, util::quantities::microsecond>);

  if (period.value() == expectedPeriod) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::OpticalClockPeriod() => " << period;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Optical clock period expected to be " << expectedPeriod << " us, but got " << period
      << " instead";
  }

  if (std::abs(period.quantity() * frequency - 1.0) > 1e-4) {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Optical clock period (" << period << ") and frequency (" << frequency
      << " should have been one the inverse of the other! (their product is "
      << (period.quantity() * frequency) << ")";
  }

  //
  // start time
  //

  // in electronics scale
  // optical time has no defined start time, and DetectorTimings sets it to
  // the same time as the start of the electronics time
  auto const expectedStartTime = 0_us;

  auto const startTime = timings.startTime<optical_time>();
  static_assert(
    util::is_same_decay_v<decltype(startTime.quantity()), util::quantities::microsecond>);

  if (startTime == expectedStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<optical_time>() => " << startTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Start of optical time scale is expected to be " << expectedStartTime
      << " in electronics time, but got " << startTime << " instead";
  }

  // in trigger scale
  auto const expectedTrigStartTime = expectedStartTime - timings.detClocksUnits().TriggerTime();

  auto const trigStartTime = timings.startTime<optical_time, trigger_time>();
  static_assert(
    util::is_same_decay_v<decltype(trigStartTime.quantity()), util::quantities::microsecond>);

  if (trigStartTime == expectedTrigStartTime) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::startTime<optical_time, trigger_time>() => " << trigStartTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Start of optical time scale is expected to be " << expectedTrigStartTime
      << " in trigger time, but got " << trigStartTime << " instead";
  }

  //
  // time conversions
  //
  double const inputTick_count = 100.5;

  //
  // to electronics time (real tick):
  //
  double const expectedTime_us = inputTick_count * timings.detClocks().OpticalClock().TickPeriod();

  optical_tick_d inputTick_d{inputTick_count};
  static_assert(util::is_same_decay_v<decltype(inputTick_d.quantity()), util::quantities::tick_d>);

  auto const time = timings.toElectronicsTime(inputTick_d);
  static_assert(util::is_same_decay_v<decltype(time.quantity()), util::quantities::microsecond>);

  if (time.value() == expectedTime_us) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toElectronicsTime<optical_tick>(" << inputTick_d << ") => " << time;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Optical tick #" << inputTick_count << " is expected to be " << expectedTime_us
      << " us in electronics time, but got " << time << " instead";
  }

  // back to optical tick:
  auto const tick = timings.toTick<optical_tick_d>(time);
  static_assert(util::is_same_decay_v<decltype(tick.quantity()), util::quantities::tick_d>);

  if (tick == inputTick_d) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTick<optical_tick, "
      << detinfo::timescales::timescale_traits<decltype(time)::category_t>::name() << ">(" << time
      << ") => " << tick;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << detinfo::timescales::timescale_traits<decltype(time)::category_t>::name() << " " << time
      << " is expected to be optical tick " << inputTick_d << ", but got " << tick << " instead";
  }

  // to trigger time (truncated tick):

  double const expectedTrigTime_us = expectedTime_us - timings.detClocks().TriggerTime();
  double const expectedTrigTime_truncated_us =
    static_cast<int>(inputTick_count) * timings.detClocks().OpticalClock().TickPeriod() -
    timings.detClocks().TriggerTime();

  auto const inputTick = optical_tick::castFrom(inputTick_count);
  static_assert(util::is_same_decay_v<decltype(inputTick.quantity()), util::quantities::tick>);

  auto const trigTime = timings.toTriggerTime(inputTick);
  static_assert(
    util::is_same_decay_v<decltype(trigTime.quantity()), util::quantities::microsecond>);

  if (trigTime.value() == expectedTrigTime_truncated_us) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTriggerTime<optical_tick>(" << inputTick << ") => " << trigTime;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Optical tick " << inputTick << " is expected to be " << expectedTrigTime_truncated_us
      << " us in trigger time, but got " << trigTime << " instead";
  }

  // back to optical tick:
  auto const trigTick = timings.toTick<optical_tick>(trigger_time{expectedTrigTime_us});
  static_assert(util::is_same_decay_v<decltype(trigTick.quantity()), util::quantities::tick>);

  if (trigTick == inputTick) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTick<optical_tick, trigger_time>(" << expectedTrigTime_us << ") => "
      << trigTick;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Trigger time " << expectedTrigTime_us << " us is expected to be optical tick "
      << inputTick << ", but got " << trigTick << " instead";
  }

  //
  // interval to ticks
  //

  double const inputInterval_us = 200.008;

  time_interval const inputInterval{inputInterval_us};
  static_assert(
    util::is_same_decay_v<decltype(inputInterval.quantity()), util::quantities::microsecond>);

  // real
  double const expectedTicksD = inputInterval_us / timings.detClocks().OpticalClock().TickPeriod();

  auto const ticks_d = timings.toTicks<optical_time_ticks_d>(inputInterval);
  static_assert(util::is_same_decay_v<decltype(ticks_d.quantity()), util::quantities::tick_d>);

  if (ticks_d.value() == expectedTicksD) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTicks<optical_time_ticks_d>(" << inputInterval << ") => " << ticks_d;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Time interval " << inputInterval << " is expected to last " << expectedTicksD
      << " optical ticks, but got " << ticks_d << " instead";
  }

  // integer
  int const expectedTicks = static_cast<int>(expectedTicksD);

  auto const ticks = timings.toTicks<optical_time_ticks>(inputInterval);
  static_assert(util::is_same_decay_v<decltype(ticks.quantity()), util::quantities::tick>);

  if (ticks.value() == expectedTicks) {
    mf::LogVerbatim("DetectorTimingsStandard_test")
      << "DetectorTimings::toTicks<optical_time_ticks>(" << inputInterval << ") => " << ticks;
  }
  else {
    ++nErrors;
    mf::LogProblem("DetectorTimingsStandard_test")
      << "Time interval " << inputInterval << " is expected to last " << expectedTicks
      << " optical integer ticks, but got " << ticks << " instead";
  }

  return nErrors;
} // testOpticalClockTimings()

//------------------------------------------------------------------------------
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

  // we print some information from the service itself;
  // this is specific to the implementation, so we need to talk
  // to the concrete type; this is not the way we would normally operate:
  TestEnv.Provider<detinfo::DetectorClocksStandard>()->debugReport();

  //
  // run the test algorithm
  // (I leave it here for reference -- there is no test algorithm here)
  //

  // 1. we initialize it from the configuration in the environment,
  //  MyTestAlgo Tester(TestEnv.TesterParameters());

  // 2. we set it up with the geometry from the environment
  //  Tester.Setup(*(TestEnv.Provider<detinfo::DetectorClocks>()));

  // 3. then we run it!
  // Note that here we are querying the abstract DetectorClocks interface;
  // this is the right way to go.
  detinfo::DetectorClocks const* detClocks = TestEnv.Provider<detinfo::DetectorClocks>();

  // here we cheat and use the knowledge of which implementation we are using
  // (need to use pointers to use the feature of nullptr on conversion failure)
  auto const* detClocksStd = dynamic_cast<detinfo::DetectorClocksStandard const*>(detClocks);
  if (detClocksStd) { detClocksStd->debugReport(); }
  else {
    mf::LogWarning("clocks_test") << "Can't run DetectorClocksStandard-specific diagnostics.";
  }

  using namespace detinfo::timescales; // electronics_time
  detinfo::DetectorTimings timings(*detClocks);

  mf::LogVerbatim("DetectorTimingsStandard_test")
    << "Electronics clock: " << timings.ClockPeriodFor<electronics_time>() << ", "
    << timings.ClockFrequencyFor<electronics_time>();

  nErrors += testTriggerTime(*detClocks);
  nErrors += testBeamGateTime(*detClocks);
  nErrors += testSimulationTimes(*detClocks);
  nErrors += testTriggerTimes(*detClocks);
  nErrors += testOpticalClockTimings(*detClocks);

  // 4. And finally we cross fingers.
  if (nErrors > 0) { mf::LogError("clocks_test") << nErrors << " errors detected!"; }

  return nErrors;
} // main()
