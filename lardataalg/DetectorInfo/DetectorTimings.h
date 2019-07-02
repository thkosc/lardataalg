/**
 * @file   lardataalg/DetectorInfo/DetectorTimings.h
 * @brief  Interface to `detinfo::DetectorClocks`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 30, 2019
 * 
 */

#ifndef LARDATAALG_DETECTORINFO_DETECTORTIMINGS_H
#define LARDATAALG_DETECTORINFO_DETECTORTIMINGS_H

// LArSoft libraries
#include "lardataalg/DetectorInfo/DetectorTimingTypes.h" // ns. timescales
#include "lardataalg/DetectorInfo/DetectorClocks.h"
#include "lardataalg/Utilities/quantities/spacetime.h"
#include "lardataalg/Utilities/quantities/frequency.h"
#include "lardataalg/Utilities/quantities/electronics.h"

// C/C++ standard libraries
#include <type_traits> // std::enable_if_t, std::is_same_v, std::void_t


namespace detinfo {
  
  
  // ---------------------------------------------------------------------------
  /**
   * @brief A partial `detinfo::DetectorClocks` supporting units.
   * 
   * This class is instantiated based on a `detinfo::DetectorClocks`, which
   * is relied upon to provide the underlying functionality.
   * 
   * Example of usage:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * detinfo::DetectorClocksWithUnits const& detClocks
   *   = detinfo::makeDetectorClocksWithUnits
   *   (lar::providerFrom<detinfo::DetectorClocks>())
   *   ;
   * 
   * util::quantities::nanosecond simulStartTime
   *   = detClocks.G4ToElecTime(0.0);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  class DetectorClocksWithUnits {
    
    /// The backend instance of `detinfo::DetectorClocks` this object uses.
    detinfo::DetectorClocks const* fDetClocks = nullptr;
    
      public:
    
    // import types
    using nanosecond = util::quantities::nanosecond;
    using microsecond = util::quantities::microsecond;
    using megahertz = util::quantities::megahertz;
    using ticks_d = util::quantities::ticks_d;
    
    
    // @{
    /// Constructor: uses `detClocks` for internal conversions.
    DetectorClocksWithUnits(detinfo::DetectorClocks const* detClocks)
      : fDetClocks(detClocks)
      {}
    DetectorClocksWithUnits(detinfo::DetectorClocks const& detClocks)
      : DetectorClocksWithUnits(&detClocks)
      {}
    // @}
    
    
    /// Returns the detector clocks service provider in use.
    detinfo::DetectorClocks const& detClocks() const { return *fDetClocks; }
    
    
    /// Equivalent to `detinfo::DetectorClocks::TriggerTime()`.
    microsecond TriggerTime() const
      { return microsecond{ detClocks().TriggerTime() }; }
    
    /// Equivalent to `detinfo::DetectorClocks::BeamGateTime()`.
    microsecond BeamGateTime() const
      { return microsecond{ detClocks().BeamGateTime() }; }
    
    /// Equivalent to `detinfo::DetectorClocks::TPCTime()`.
    microsecond TPCTime() const
      { return microsecond{ detClocks().TPCTime() }; }
    
    // @{
    /// Equivalent to `detinfo::DetectorClocks::G4ToElecTime()`.
    microsecond G4ToElecTime(nanosecond simTime) const
      { return microsecond{ detClocks().G4ToElecTime(simTime.value()) }; }
    microsecond G4ToElecTime(double simTime) const
      { return G4ToElecTime(nanosecond{simTime}); }
    // @}
    
    
    // @{
    /// Equivalent to `detinfo::DetectorClocks::G4ToElecTime()`.
    ticks_d TPCTick2TDC(ticks_d tpcticks) const
      { return ticks_d{ detClocks().TPCTick2TDC(tpcticks.value()) }; }
    ticks_d TPCTick2TDC(double tpcticks) const
      { return TPCTick2TDC(ticks_d{tpcticks}); }
    // @}
    
    
    /// Equivalent to
    /// `detinfo::DetectorClocks::OpticalClock().TickPeriod()`.
    microsecond OpticalClockPeriod() const
      { return microsecond{ detClocks().OpticalClock().TickPeriod() }; }
    
    /// Equivalent to
    /// `detinfo::DetectorClocks::OpticalClock().TickPeriod()`.
    megahertz OpticalClockFrequency() const
      { return megahertz{ detClocks().OpticalClock().Frequency() }; }
    
    
  }; // class DetectorClocksWithUnits
  
  
  /// Transforms a `detinfo::DetectorClocks` into a
  /// `detinfo::DetectorClocksWithUnits`.
  detinfo::DetectorClocksWithUnits makeDetectorClocksWithUnits
    (detinfo::DetectorClocks const& detClocks)
    { return { detClocks }; }
  
  
  // ---------------------------------------------------------------------------
  /**
   * @brief A class exposing an updated version of `detinfo::DetectorClocks`.
   * 
   * This object actually wraps around an existing `detinfo::DetectorClocks`
   * object. For example, in _art_/LArSoft, one can be created as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto const& detTimings = detinfo::makeDetectorTimings
   *   (lar::provider_from<detinfo::DetectorClocksService>());
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * It can also wrap around an existing `detinfo::DetectorClocksWithUnits`
   * object:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto detClocksWU = detinfo::makeDetectorClocksWithUnits
   *   (lar::provider_from<detinfo::DetectorClocksService>());
   * auto const& timings = detinfo::makeDetectorTimings(detClocksWU);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, the `detinfo::DetectorClocksWithUnits` object must exist for
   * all the lifetime of `timings`, since it is referenced by the latter.
   * 
   * 
   * Data types
   * ===========
   * 
   * Several data types are designed to resolve the ambiguity of which time,
   * which time scale and which time unit are used.
   * The class distinguish between time points, which identify events in
   * absolute terms, and time intervals and durations, which identify the time
   * lapsed between two events or time points.
   * 
   * The time duration is generally expressed by a single type, since it is
   * common to all main time scales expressed in real time:
   * * `time_interval` is natively represented in microseconds
   * 
   * Note that while each data type has a native representation (usually
   * microseconds), these objects can convert into other units to remain
   * consistent. Still, they are not fool-proof...
   * 
   * Different time point types are defined to reflect the different time
   * scales:
   * * `electronics_time` for times on the
   *   @ref DetectorClocksElectronicsTime "electronics time axis", starting
   *   at the @ref DetectorClocksElectronicsStartTime "electronics start time",
   *   natively expressed in microseconds. There is no clock specific to
   *   this time scale, but we identify the ticks with the TPC electronics
   *   clock. Tick types are nevertheless specific to this time scale and they
   *   are called `electronics_tick` (integral tick number) and
   *   `electronics_tick_d` (real tick number).
   * * `TPCelectronics_time` for times on the
   *   @ref DetectorClocksTPCelectronicsTime "TPC electronics time axis",
   *   starting at the
   *   @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time",
   *   natively expressed in microseconds.
   *   Tick types are called `TPCelectronics_tick` (integral tick number)
   *   and `TPCelectronics_tick_d` (real tick number).
   * * `optical_time` for times on the
   *   * @ref DetectorClocksOpticalElectronicsTime "optical detector electronics time axis",
   *   starting at the
   *   @ref DetectorClocksOpticalElectronicsStartTime "optical detector electronics start time",
   *   natively expressed in microseconds.
   *   Tick types are called `optical_tick` (integral tick number)
   *   and `optical_tick_d` (real tick number).
   *   @note Unfortunately, this time scale is not well defined, so it is
   *         assumed to start at the same time as the electronics scale.
   * * `trigger_time` for times on the
   *   @ref DetectorClocksTriggerTime "trigger time axis", starting at the
   *   @ref DetectorClocksHardwareTrigger "hardware trigger time",
   *   natively expressed in microseconds.
   *   Tick types are called `trigger_tick` (integral tick number)
   *   and `trigger_tick_d` (real tick number).
   * * `simulation_time` for times on the
   *   @ref DetectorClocksSimulationTime "simulation time axis", starting at the
   *   @ref DetectorClocksGeant4TimeStart "GEANT4 time start",
   *   natively expressed in nanoseconds.
   * 
   * 
   * For each time scale, a "category" is defined (e.g.
   * `electronics_time::category_t`), and traits are available for each category
   * as `timescale_traits` objects (for example
   * `detinfo::timescales::timescale_traits<electronics_time::category_t>`).
   * From the traits, it is possible to access the type of time point
   * (`time_point_t`, which is equivalent e.g. to `electronics_time`,
   * `simulation_time` etc.) as well as the time interval type
   * (`time_interval_t`), the tick type in integral units (`tick_t`) and real
   * units (`tick_d_t`), the name of the time scale itself (`name()`) and
   * little more.
   * 
   * Note that not all time scales support all features. For example, simulation
   * time is not actually associated to any clock, so that for example an
   * attempt to convert a time into simulation time ticks will result in a
   * compilation failure.
   * 
   */
  class DetectorTimings: private detinfo::DetectorClocksWithUnits {
    
    /// The traits of a specific `TimeScale`.
    template <typename TimeScale>
    using traits_of = detinfo::timescales::timescale_traits
      <typename TimeScale::category_t>;
    
    /// The time interval type of a specific `TimeScale`.
    template <typename TimeScale>
    using time_interval_for = typename traits_of<TimeScale>::time_interval_t;
    
    /// The frequency type of a specific `TimeScale`.
    template <typename TimeScale>
    using frequency_for = typename traits_of<TimeScale>::frequency_t;
    
      public:
    
    // --- BEGIN -- Imported time scale types ----------------------------------
    /// @name Imported time scale types
    /// @{
    
    using electronics_time = detinfo::timescales::electronics_time;
    using TPCelectronics_time = detinfo::timescales::TPCelectronics_time;
    using optical_time = detinfo::timescales::optical_time;
    using trigger_time = detinfo::timescales::trigger_time;
    using simulation_time = detinfo::timescales::simulation_time;
    
    using electronics_tick = detinfo::timescales::electronics_tick;
    using TPCelectronics_tick = detinfo::timescales::TPCelectronics_tick;
    using optical_tick = detinfo::timescales::optical_tick;
    using trigger_tick = detinfo::timescales::trigger_tick;
    
    using electronics_tick_d = detinfo::timescales::electronics_tick_d;
    using TPCelectronics_tick_d = detinfo::timescales::TPCelectronics_tick_d;
    using optical_tick_d = detinfo::timescales::optical_tick_d;
    using trigger_tick_d = detinfo::timescales::trigger_tick_d;
    
    using time_interval = detinfo::timescales::time_interval;
    
    using electronics_time_ticks = detinfo::timescales::electronics_time_ticks;
    using TPCelectronics_time_ticks
      = detinfo::timescales::TPCelectronics_time_ticks;
    using optical_time_ticks = detinfo::timescales::optical_time_ticks;
    using trigger_time_ticks = detinfo::timescales::trigger_time_ticks;
    
    /// @}
    // --- END -- Imported time scale types ------------------------------------
    
    
    // note that `makeDetectorTimings()` provides an additional construction way
    // @{
    /// Constructor: wraps around a specified `detinfo::DetectorClocks` object.
    DetectorTimings(detinfo::DetectorClocks const& detClocks)
      : detinfo::DetectorClocksWithUnits(detClocks) {}
    DetectorTimings(detinfo::DetectorClocks const* detClocks)
      : detinfo::DetectorClocksWithUnits(detClocks) {}
    // @}
    
    
    /// --- BEGIN -- Access to underlying service providers --------------------
    /// @name Access to underlying service providers.
    /// @{
    
    /// Returns a DetectorClocksWithUnits object.
    detinfo::DetectorClocksWithUnits const& detClocksUnits() const
      { return static_cast<detinfo::DetectorClocksWithUnits const&>(*this); }
    
    /// Returns the detector clocks service provider in use.
    detinfo::DetectorClocks const& detClocks() const
      { return detClocksUnits().detClocks(); }
    
    /// @}
    /// --- END -- Access to underlying service providers ----------------------
    
    
    // --- BEGIN -- Electronics times ------------------------------------------
    
    /// Returns the trigger time as a point in electronics time.
    /// @see `detinfo::DetectorClocks::TriggerTime()`
    electronics_time TriggerTime() const
      { return electronics_time{ detClocksUnits().TriggerTime() }; }
    
    /// Returns the beam gate time as a point in electronics time.
    /// @see `detinfo::DetectorClocks::BeamGateTime()`
    electronics_time BeamGateTime() const
      { return electronics_time{ detClocksUnits().TriggerTime() }; }
    
    // --- END -- Electronics times --------------------------------------------
    
    
    // --- BEGIN -- Conversions ------------------------------------------------
    /// @name Conversions
    /// @{
    
    /**
     * @brief Returns a `time` point in a different time scale.
     * @tparam TargetTime the desired time scale
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted into `TargetTime` time scale
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * electronics_time nuElecTime 
     *   = timings.toTimeScale<electronics_time>(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * is equivalent to use `detinfo::DetectorClocks::G4ToElecTime(47.5)`.
     */
    template <typename TargetTime, typename FromTime>
    TargetTime toTimeScale(FromTime time) const;
    
    /**
     * @brief Returns a `time` point as a tick on a different time scale.
     * @tparam TargetTick the desired time scale
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted as tick into `TargetTick` time scale
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * TPCelectronics_tick_d nuTPCtick
     *   = timings.toTick<TPCelectronics_tick_d>(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename TargetTick, typename FromTime>
    TargetTick toTick(FromTime time) const;
    
    
    /**
     * @brief Returns the number of ticks corresponding to a `time` interval.
     * @tparam Ticks type of tick interval returned
     * @return the `time` interval expressed as number of ticks
     * 
     * The `time` interval is represented by the number of ticks on the time
     * scale of `Ticks` which fits in that interval.
     * If `Ticks` is based on an integral type, the resulting number of ticks is
     * usually truncated.
     */
    template <typename Ticks>
    Ticks toTicks(time_interval time) const
      { return Ticks::castFrom(time / ClockPeriodFor<Ticks>()); }
    
    
    /**
     * @brief Converts a `time` point into electronics time scale.
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted into electronics time scale.
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * electronics_time nuElecTime = timings.toElectronicsTime(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * is equivalent to use `detinfo::DetectorClocks::G4ToElecTime(47.5)`.
     */
    template <typename FromTime>
    electronics_time toElectronicsTime(FromTime time) const
      { return toTimeScale<electronics_time>(time); }
    
    
    /**
     * @brief Converts a `time` point into electronics tick (real).
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted into electronics tick number.
     * @see `toTick()`, `toElectronicsTime()`, `toElectronicsTick()`
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * electronics_tick_d nuElecTick = timings.toElectronicsTime(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename FromTime>
    electronics_tick_d toElectronicsTickD(FromTime time) const
      { return toTick<electronics_tick_d>(time); }
    
    /**
     * @brief Converts a `time` point into electronics tick (truncated).
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted into electronics tick number.
     * @see `toTick()`, `toElectronicsTime()`, `toElectronicsTickD()`
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * electronics_tick_d nuElecTick = timings.toElectronicsTime(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename FromTime>
    electronics_tick toElectronicsTick(FromTime time) const
      { return toTick<electronics_tick>(time); }
    
    
    /**
     * @brief Converts a `time` point into trigger time scale.
     * @tparam FromTime the time scale the input `time` is measured in
     * @param time the time instant to be converted, in `FromTime` scale
     * @return the time instant converted into trigger time scale.
     * 
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * simulation_time nuTime = 47.5_ns;
     * trigger_time nuTriggerTime = timings.toTriggerTime(nuTime);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename FromTime>
    trigger_time toTriggerTime(FromTime time) const
      { return toTimeScale<trigger_time>(time); }
    
    /// @}
    // --- END -- Conversions --------------------------------------------------
    
    
    // --- BEGIN -- Clocks -----------------------------------------------------
    /// @name Clocks
    /// @{
    
    /// Returns the electronics clock for the specified time scale.
    template <typename TimeScale>
    detinfo::ElecClock const& ClockFor() const;
    
    /// Returns the period of the clock for the specified time scale.
    template <typename TimeScale>
    time_interval_for<TimeScale> ClockPeriodFor() const
      {
        return time_interval_for<TimeScale>
          { microsecond{ ClockFor<TimeScale>().TickPeriod() } }; 
      }
    
    /// Returns the frequency of the clock for the specified time scale.
    template <typename TimeScale>
    frequency_for<TimeScale> ClockFrequencyFor() const
      {
        return frequency_for<TimeScale>
          { megahertz{ ClockFor<TimeScale>().Frequency() } };
      }
    /// @}
    // --- END -- Clocks -------------------------------------------------------
    
    
    // --- BEGIN -- Optical clock ----------------------------------------------
    
    /// Returns the duration of the optical clock period and tick.
    auto OpticalClockPeriod() const
      { return ClockPeriodFor<optical_time>(); }
    
    /// Returns the frequency of the optical clock tick.
    megahertz OpticalClockFrequency() const
      { return ClockFrequencyFor<optical_time>(); }
    
    /**
     * @brief Returns the optical ticks corresponding to a `time` interval.
     * @tparam Ticks type of tick interval returned
     *               (default: _optical_time_ticks_)
     * @return the `time` interval expressed as number of optical ticks
     * 
     * The `time` interval is represented by the number of optical detector
     * electronics ticks fitting in it. If `Ticks` is based on an integral type,
     * the resulting number of ticks is usually truncated.
     */
    template <typename Ticks = optical_time_ticks>
    Ticks toOpticalTicks(time_interval time) const
      {
        static_assert(traits_of<Ticks>::template same_category_as<optical_tick>,
          "Specified ticks are not for optical time scale!");
        return toTicks<Ticks>(time); 
      }
    
    
    /**
     * @brief Returns the `time` point converted in units of optical time ticks.
     * @tparam TimePoint type of the time to be converted
     * @param time time point to be converted into an optical tick
     * @return the `time` point expressed as integral number of ticks
     * @see `toTick()`, `toOpticalTickD()`
     * 
     * The specified time point is converted in ticks from the start of the
     * optical time scale (i.e., from `startTime<optical_time>()`).
     * The ticks are from the optical detector clock (`OpticalClockPeriod()`).
     * The number of ticks is *truncated*.
     */
    template <typename TimePoint>
    optical_tick toOpticalTick(TimePoint time) const
      { return toTick<optical_tick>(time); }
    
    /**
     * @brief Returns the `time` point converted in units of optical time ticks.
     * @tparam TimePoint type of the time to be converted
     * @param time time point to be converted into an optical tick
     * @return the `time` point expressed as real number of ticks
     * @see `toTick()`, `toOpticalTickD()`
     * 
     * The specified time point is converted in ticks from the start of the
     * optical time scale (i.e., from `startTime<optical_time>()`).
     * The ticks are from the optical detector clock (`OpticalClockPeriod()`).
     * The number of ticks may be fractional.
     */
    template <typename TimePoint>
    optical_tick_d toOpticalTickD(TimePoint time) const
      { return toTick<optical_tick_d>(time); }
    
    // --- END -- Optical clock ------------------------------------------------
    
    
    // --- BEGIN -- Reference times --------------------------------------------
    /// @name Reference times
    /// @{
    /**
     * @brief Converts a electronics time point into a duration from the start
     *        of electronics time.
     * @param time electronics time point to be converted
     * @return time elapsed from the start of electronics time to `time`
     * 
     * This is mostly a logic operation, since the value of the returned time
     * duration is the same as the value of the `time` point (i.e. the start
     * time is 0).
     */
    time_interval fromStart(electronics_time time) const;
    
    
    /**
     * @brief Returns the start time of the specified time scale.
     * @tparam TimePoint the type of time point on the requested scale
     * @tparam TimeScale the type of scale to get the result in
     *                   (default: `electronics_time`)
     * 
     * This method returns the start time of a time scale, in another time
     * scale (or in the very same, in which case the result is trivially `0`).
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * using namespace detinfo::timescales;
     * 
     * electronics_time const TPCstartTime
     *   = detTimings.startTime<TPCelectronics_time>();
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * sets `TPCstartTime` to the instant the TPC electronics time starts,
     * in the electronics time scale. This example in particular is yielding
     * the same value as calling `detinfo::DetectorClocks()::TPCTime()`.
     * 
     * 
     * Implemented scales
     * -------------------
     * 
     * * all where `TimePoint` is the same as `TimeScale` (trivially, `0`)
     * * `TPCelectronics_time` to:
     *     * `electronics_time`
     * 
     */
    template <typename TimePoint, typename TimeScale = electronics_time>
    constexpr TimeScale startTime() const;
    
    
    /**
     * @brief Returns the start tick of the specified time tick scale.
     * @tparam TickPoint the type of tick point on the requested scale
     * @tparam TimeTickScale the type of scale to get the result in, also a time
     *                       tick point (default: `electronics_tick`)
     * 
     * This method returns the start tick of a time tick scale, in another time
     * tick scale (or in the very same, in which case the result is trivially
     * `0`).
     * Example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * using namespace detinfo::timescales;
     * 
     * electronics_time_tick const TPCstartTick
     *   = detTimings.startTick<TPCelectronics_time_tick>();
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * sets `TPCstartTime` to the instant the TPC electronics time starts,
     * in the electronics time tick scale. This example in particular is
     * yielding the same value as calling
     * `detinfo::DetectorClocks()::TPCTick2TDC(0)`.
     * 
     * @todo The example is not even supported yet!
     * 
     * 
     * Implemented scales
     * -------------------
     * 
     * * all where `TickPoint` is the same as `TimeScale` (trivially, `0`)
     * * `TPCelectronics_time` to:
     *     * `electronics_time`
     * 
     * 
     */
    template
      <typename TickPoint, typename TimeTickScale = electronics_tick>
    constexpr TimeTickScale startTick() const;
    
    /// @}
    // --- END -- Reference times ----------------------------------------------
    
      private:
    friend detinfo::DetectorTimings makeDetectorTimings
      (detinfo::DetectorClocksWithUnits const&);
    
  }; // class DetectorTimings
  
  
  
  /// Returns `DetectorTimings` object from specified `detinfo::DetectorClocks`.
  detinfo::DetectorTimings makeDetectorTimings
    (detinfo::DetectorClocks const& detClocks)
    { return { detClocks }; }
  
  /// Returns `DetectorTimings` object from specified `detinfo::DetectorClocks`.
  detinfo::DetectorTimings makeDetectorTimings
    (detinfo::DetectorClocks const* detClocks)
    { return makeDetectorTimings(*detClocks); }
  
  /// Returns `DetectorTimings` object from specified
  /// `detinfo::DetectorClocksWithUnits`.
  detinfo::DetectorTimings makeDetectorTimings
    (detinfo::DetectorClocksWithUnits const& detClocksWU)
    { return static_cast<detinfo::DetectorTimings const&>(detClocksWU); }
  
  
  // ---------------------------------------------------------------------------
  
  
} // namespace detinfo



//------------------------------------------------------------------------------
//---  template implementation
//------------------------------------------------------------------------------
namespace detinfo {
  
  // ---------------------------------------------------------------------------
  namespace details {
    
    using namespace util::quantities::time_literals;
    using namespace util::quantities::electronics_literals;
    
    // -------------------------------------------------------------------------
    // --- StartTimeImpl
    // -------------------------------------------------------------------------
    template
      <typename TimePoint, typename TimeScale = TimePoint, typename = void>
    struct StartTimeImpl {
      
      static constexpr TimeScale startTime [[noreturn]] (DetectorTimings const*)
        {
          static_assert(util::always_false_v<TimePoint>,
            "Start time not implemented for this time in this time scale."
            );
          throw false;
        }
      
    }; // StartTimeImpl
    
    
    // -------------------------------------------------------------------------
    template <typename TimePoint>
    struct StartTimeImpl<TimePoint, TimePoint> {
      
      static constexpr TimePoint startTime(DetectorTimings const*)
        { return TimePoint{ 0_us }; }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    // --- to electronics time
    // ---
    template <>
    struct StartTimeImpl<
      detinfo::timescales::TPCelectronics_time, // source
      detinfo::timescales::electronics_time     // destination
      >
    {
      static detinfo::timescales::electronics_time startTime
        (DetectorTimings const* detTiming)
        {
          return detinfo::timescales::electronics_time
            (detTiming->detClocksUnits().TPCTime());
        }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    template <>
    struct StartTimeImpl<
      detinfo::timescales::optical_time,    // source
      detinfo::timescales::electronics_time // destination
      >
    {
      static constexpr detinfo::timescales::electronics_time startTime
        (DetectorTimings const*)
        { return detinfo::timescales::electronics_time(0_us); }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    template <>
    struct StartTimeImpl<
      detinfo::timescales::simulation_time, // source
      detinfo::timescales::electronics_time // destination
      >
    {
      static detinfo::timescales::electronics_time startTime
        (DetectorTimings const* detTiming)
        {
          return detinfo::timescales::electronics_time
            { detTiming->detClocksUnits().G4ToElecTime(0.0_ns) };
        } // startTime()
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    template <>
    struct StartTimeImpl<
      detinfo::timescales::trigger_time,    // scale to convert the start of
      detinfo::timescales::electronics_time // destination scale
      >
    {
      static detinfo::timescales::electronics_time startTime
        (DetectorTimings const* detTiming)
        { return detTiming->TriggerTime(); }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    // --- from electronics time
    // ---
    template <typename TimeScale>
    struct StartTimeImpl<
      detinfo::timescales::electronics_time, // source
      TimeScale,                             // destination
      std::enable_if_t
        <!std::is_same_v<TimeScale, detinfo::timescales::electronics_time>>
      >
    {
      static TimeScale startTime(DetectorTimings const* detTiming)
        {
          return TimeScale{
            -(detTiming->startTime
                <TimeScale, detinfo::timescales::electronics_time>()
              .quantity())
            };
        }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    // --- via electronics time
    // ---
    
    /*
     * this is a very generic version, which I hope C++ rules will prevent from
     * clashing with:
     * * the most generic version (this is a specialization)
     * * the one with same target and destination (explicitly vetoed)
     * * any one involving electronics_time (explicitly vetoed)
     * * any full specialization (this being partial, the full ones should win)
     */
    template <typename TimePoint, typename TimeScale>
    struct StartTimeImpl<
      TimePoint, // source
      TimeScale, // destination
      std::enable_if_t<
           !std::is_same_v<TimePoint, detinfo::timescales::electronics_time>
        && !std::is_same_v<TimeScale, detinfo::timescales::electronics_time>
        && !std::is_same_v<TimeScale, TimePoint>
        >
      >
    {
      static TimeScale startTime(DetectorTimings const* detTiming)
        {
          return detTiming->toTimeScale<TimeScale>
            (detTiming->startTime<TimePoint>());
        }
    }; // StartTimeImpl<> (via electronics_time)
    
    
    // -------------------------------------------------------------------------
    // --- StartTickImpl
    // -------------------------------------------------------------------------
    template <typename TickPoint, typename TimeTickScale = TickPoint>
    struct StartTickImpl;
    
    
    // -------------------------------------------------------------------------
    template <typename TickPoint>
    struct StartTickImpl<TickPoint, TickPoint> {
      
      static constexpr TickPoint startTick(DetectorTimings const*)
        { return TickPoint::castFrom(0.0); }
      
    }; // StartTickImpl<>
    
    
    // -------------------------------------------------------------------------
    template <>
    struct StartTickImpl<
      detinfo::timescales::TPCelectronics_tick_d, // source
      detinfo::timescales::electronics_tick_d     // destination
      >
    {
      static detinfo::timescales::electronics_tick_d startTick
        (DetectorTimings const* detTiming)
        {
          return detinfo::timescales::electronics_tick_d
            (detTiming->detClocksUnits().TPCTick2TDC(0_tickd)); 
        }
      
    }; // StartTimeImpl<>
    
    
    // -------------------------------------------------------------------------
    template <>
    struct StartTickImpl<
      detinfo::timescales::optical_tick,    // source
      detinfo::timescales::electronics_tick_d // destination
      >
    {
      static constexpr detinfo::timescales::electronics_tick_d startTick
        (DetectorTimings const*)
        { return 0_tickd; }
      
    }; // StartTickImpl<>
    
    
    // -------------------------------------------------------------------------
    
    // default implementation (for any time)
    template <typename FromTime, typename TargetTime, typename = void>
    struct TimeScaleConverter {
      
      static TargetTime convert
        (FromTime time, DetectorTimings const* timings)
        { return timings->startTime<FromTime, TargetTime>() + time.quantity(); }
      
    };
    
    // special implementation when source and target are the same: passthrough
    template <typename TargetTime>
    struct TimeScaleConverter<TargetTime, TargetTime> {
      
      static TargetTime convert
        (TargetTime time, DetectorTimings const*)
        { return time; }
      
    }; // TimeScaleConverter<TargetTime, TargetTime>
    
    
    // special implementation for ticks as source
    template <typename FromTick, typename TargetTime>
    struct TimeScaleConverter<FromTick, TargetTime,
      std::enable_if_t<detinfo::timescales::is_tick_v<FromTick>>
      >
    {
      
      static TargetTime convert
        (FromTick tick, DetectorTimings const* timings)
        {
          using FromTime
            = typename detinfo::timescales::timescale_traits
              <typename FromTick::category_t>
            ::time_point_t
            ;
          return timings->toTimeScale<TargetTime>(
            FromTime{
              tick.value() * timings->ClockPeriodFor<FromTick>().quantity()
            });
        }
      
    }; // TimeScaleConverter<FromTick>
    
    
    // -------------------------------------------------------------------------
    // default implementation (for any time)
    template <typename FromTime, typename TargetTick, typename = void>
    struct TickConverter {
      
      static TargetTick convert
        (FromTime time, DetectorTimings const* timings)
        {
          using TargetTime = typename detinfo::timescales::timescale_traits
            <typename TargetTick::category_t>::time_point_t;
          auto const timeFromStart
            = time - timings->startTime<TargetTime, FromTime>();
          auto const clockPeriod = timings->ClockPeriodFor<TargetTick>();
          return TargetTick::castFrom(timeFromStart / clockPeriod); 
        } // convert()
      
    }; // TickConverter
    
    
    // -------------------------------------------------------------------------
    
    
    // -------------------------------------------------------------------------
    template <typename TimeScale, typename = void>
    struct ClockForImpl {
      
      static detinfo::ElecClock const& get [[noreturn]] (DetectorTimings const*)
        {
          static_assert(util::always_false_v<TimeScale>,
            "Electronics clock not defined for this time scale.");
          throw false;
        }
      
    }; // struct ClockForImpl
    
    
    template <typename TimeScale>
    struct ClockForImpl<TimeScale, std::void_t<typename TimeScale::category_t>>
      : ClockForImpl<typename TimeScale::category_t>
    {};
    
    template <>
    struct ClockForImpl<detinfo::timescales::TPCelectronicsTimeCategory>
    {
      static detinfo::ElecClock const& get(DetectorTimings const* timings)
        { return timings->detClocks().TPCClock(); }
    };
    
    template <>
    struct ClockForImpl<detinfo::timescales::OpticalTimeCategory>
    {
      static detinfo::ElecClock const& get(DetectorTimings const* timings)
        { return timings->detClocks().OpticalClock(); }
    };
    
    template <>
    struct ClockForImpl<detinfo::timescales::TriggerTimeCategory>
    {
      static detinfo::ElecClock const& get(DetectorTimings const* timings)
        { return timings->detClocks().TriggerClock(); }
    };
    
    template <>
    struct ClockForImpl<detinfo::timescales::ElectronicsTimeCategory>
      : ClockForImpl<detinfo::timescales::TPCelectronicsTimeCategory>
    {};
    
    
    // -------------------------------------------------------------------------
    
  } // namespace details
  
  
  // ---------------------------------------------------------------------------
  template <typename TimePoint, typename TimeScale /* = electronics_time */>
  constexpr TimeScale DetectorTimings::startTime() const
    { return details::StartTimeImpl<TimePoint, TimeScale>::startTime(this); }
  
  
  // ---------------------------------------------------------------------------
  template
    <typename TickPoint, typename TimeTickScale /* = electronics_tick */>
  constexpr TimeTickScale DetectorTimings::startTick() const {
    return details::StartTickImpl<TickPoint, TimeTickScale>::startTick(this); 
  }
  
  
  // ---------------------------------------------------------------------------
  template <typename TargetTime, typename FromTime>
  TargetTime DetectorTimings::toTimeScale(FromTime time) const {
    return
      details::TimeScaleConverter<FromTime, TargetTime>::convert(time, this);
  } // DetectorTimings::toTimeScale()
  
  
  // ---------------------------------------------------------------------------
  template <typename TargetTick, typename FromTime>
  TargetTick DetectorTimings::toTick(FromTime time) const {
    return details::TickConverter<FromTime, TargetTick>::convert(time, this);
  } // DetectorTimings::toTick()
  
  
  // ---------------------------------------------------------------------------
  template <typename TimeScale>
  detinfo::ElecClock const& DetectorTimings::ClockFor() const
    { return details::ClockForImpl<TimeScale>::get(this); }
  
  
  // ---------------------------------------------------------------------------
  inline auto DetectorTimings::fromStart(electronics_time time) const
    -> time_interval
    { return time - startTime<electronics_time, electronics_time>(); }
  
  
  // ---------------------------------------------------------------------------
  
  
} // namespace detinfo



//------------------------------------------------------------------------------


#endif // LARDATAALG_DETECTORINFO_DETECTORTIMINGS_H
