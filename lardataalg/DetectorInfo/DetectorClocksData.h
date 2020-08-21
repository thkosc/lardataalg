#ifndef LARDATAALG_DETECTORINFO_DETECTORCLOCKSDATA_H
#define LARDATAALG_DETECTORINFO_DETECTORCLOCKSDATA_H

/** **************************************************************************
   * Time definitions
   * =================
   *
   * @anchor DetectorClocksTimeDefinitions
   * Many different components contribute to the operation of a LArTPC, and more
   * are needed for the simulation.
   * Some of the relevant events and time points associated with time are:
   * * *hardware trigger*:
   *     @anchor DetectorClocksHardwareTrigger
   *     the instant the trigger happens or is expected; data acquisition reacts
   *     at this time (_multi-event_)
   * * *beam gate opening*:
   *     @anchor DetectorClocksBeamGateOpening
   *     the instant the beam particles are expected to enter the detector
   *     (_multi-event_)
   * * *software trigger*:
   *     @anchor DetectorClocksSoftwareTrigger
   *     additional logic can provide a more detailed estimation of the physics
   *     event time; this usually comes too late for the electronics time to be
   *     defined by it (_multi-event_)
   * * *electronics start time*:
   *     @anchor DetectorClocksElectronicsStartTime
   *     a reference time instant; this is usually matching the
   *     @ref DetectorClocksTPCelectronicsStartTime "instant the TPC readout starts recording raw digits"
   *     (_multi-event_)
   * * *TPC electronics start time*:
   *     @anchor DetectorClocksTPCelectronicsStartTime
   *     the instant corresponding to the first sample of the raw digits from
   *     TPC; for untampered `raw::RawDigit` objects, this time matches the
   *     general @ref DetectorClocksElectronicsStartTime "electronics start time"
   *     definition, and for the model of data acquisition described here, this
   *     time is by definition function of the
   *     @ref DetectorClocksHardwareTrigger "hardware trigger". Trimmed
   *     `raw::RawDigit` with a start portion clipped away will break the
   *     assumption of electronics time matching the TPC one (which is defined
   *     as seen by `raw::RawDigit`) (_multi-event_)
   * * other electronics start times (*optical detector*, *cosmic ray tagger*,
   *     etc.)
   *     @anchor DetectorClocksOpticalElectronicsStartTime
   *     @anchor DetectorClocksExternalElectronicsStartTime
   *     represent the instant the respective data acquisition subsystems start
   *     digitizing the data from their detectors (_multi-event_)
   * * *event trigger*:
   *     @anchor DetectorClocksExternalEventTrigger
   *     the instant the physics event is detected (in reconstruction jargon
   *     this is a @f$ t_{0} @f$); ideally, for most events involving beam
   *     particles this is the a little after
   *     @ref DetectorClocksBeamGateOpening "beam gate opening", and on top of
   *     the @ref DetectorClocksHardwareTrigger "hardware trigger" (_per event_)
   * * *charge collection*:
   *     @anchor DetectorClocksChargeCollection
   *     the instant the ionisation from a particle reaches the charge readout
   *     (_per particle_)
   * * *photon collection*:
   *     @anchor DetectorClocksPhotonCollection
   *     the instant scintillation photons from a particle reaches the optical
   *     readout (_per particle_)
   * * *physics event time*:
   *     @anchor DetectorClocksPhysicsEvent
   *     the instant the main interaction happens; this is assigned by the event
   *     generator (_per event_)
   * * *Geant4 time start*:
   *     @anchor DetectorClocksGeant4TimeStart
   *     assigned to each particle by the simulation of propagation
   *     through matter (Geant4); note that this is not necessarily the same as
   *     the @ref DetectorClocksPhysicsEvent "event time". (_per event_)
   *
   * In parentheses, it is indicated if such instant is defined for each
   * considered particle, or for the complete physics process ("event") or for
   * all physics processes recorded in the same time window ("multi-event").
   *
   * @anchor DetectorClocksTimeFrameDefinitions
   * We can also define many ways to measure these times with respect to the
   * others. We use the following time scales and frames:
   * * *electronics time*:
   *     @anchor DetectorClocksElectronicsTime
   *     from the
   *     @ref DetectorClocksElectronicsStartTime "electronics start time"; this
   *     is the "glue" time frame connecting most of the others _[&micro;s]_
   * * *TPC time*:
   *     @anchor DetectorClocksTPCelectronicsTime
   *     from the
   *     @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time"
   *     _[&micro;s]_
   * * other electronics times (
   *     @anchor DetectorClocksOpticalElectronicsTime
   *     *optical electronics time*,
   *     @anchor DetectorClocksExternalElectronicsTime
   *     *external electronics time*, etc.):
   *     from the respective start times; note that in LArSoft the definition of
   *     these times is not fully defined (see
   *     @ref DetectorClocksElectronicsTimesNote "below").
   *     _[&micro;s]_
   * * *beam gate time*:
   *     @anchor DetectorClocksBeamGateTime
   *     from the @ref DetectorClocksBeamGateOpening "beam gate opening time"
   *     _[&micro;s]_
   * * *trigger time*:
   *     @anchor DetectorClocksTriggerTime
   *     from the @ref DetectorClocksHardwareTrigger "hardware trigger"
   *     _[&micro;s]_
   * * *event time*:
   *     @anchor DetectorClocksEventTime
   *     from an @ref DetectorClocksPhysicsEvent "event trigger" _[&micro;s]_
   * * *generation time*:
   *     @anchor DetectorClocksGenerationTime
   *     from the time the generators assigns to the
   *     @ref DetectorClocksPhysicsEvent "main physics event"; this is
   *     stored in the trajectory points of `simb::MCParticle` (usually the
   *     generators store a single trajectory point for each particle) _[ns]_
   * * *Geant4 time*:
   *     @anchor DetectorClocksGeant4Time
   *     from the @ref DetectorClocksGeant4TimeStart "Geant4 time start"; this
   *     is stored into each trajectory point of `simb::MCParticle` produced by
   *     `LArG4` module or equivalent _[ns]_
   * * *simulation time*:
   *     @anchor DetectorClocksSimulationTime
   *     the same as @ref DetectorClocksGeant4Time "Geant4 time" (and also
   *     generation time, which in LArSoft happens to match the former) _[ns]_
   *
   * The list above reports in square brackets the "standard" unit, used for
   * that times in that scale by all LArSoft code unless explicitly specified
   * otherwise.
   * For the times bound to readout, there is also an equivalent tick count time
   * (from the TDC), which is an integral value representing the number of
   * elapsed sampling periods of the detector they refer too (and that period
   * may be different for each piece of hardware). Be sure to read the
   * documentation about the methods you want to use to convert ticks, because
   * their implications and assumptions may be quite subtle.
   *
   *
   * Time conversions
   * =================
   *
   * While this system is carefully designed to give physicists an headache, the
   * `detinfo::DetectorClocks` service provider attempts to tame this complexity
   * by providing methods to convert from one time to the other.
   *
   * The following table represents the available conversion functions, with the
   * time in the first column as the time to be converted and in the columns the
   * times to convert to:
   *
   * |                                                     to &rarr; | electronics time      | (_ticks_)            | TPC time ticks     | trigger time              | trigger clock ticks | beam gate time            | Optical clock ticks   | External clock ticks   |
   * | ------------------------------------------------------------: | --------------------- | -------------------- | ------------------ | ------------------------- | ------------------- | ------------------------- | --------------------- | ---------------------- |
   * | (unit)                                                        | &micro;s              |                      |                    | &micro;s                  |                     | &micro;s                  |                       |                        |
   * | @ref DetectorClocksHardwareTrigger         "hardware trigger" | `TriggerTime()`       | `TPCClock()`         |                    |                           | `TriggerClock()`    |                           | `OpticalClock()`      | `ExternalClock()`      |
   * | @ref DetectorClocksBeamGateOpening         "beam gate point"  | `BeamGateTime()`      |                      |                    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksElectronicsTime         "electronics time" |                       |                      | `Time2Tick()`      |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   |                       |                      | `TPCTDC2Tick()`    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksTPCelectronicsTime      "TPC time"         |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `TPCTick2Time()`      | `TPCTick2TDC()`      |                    | `TPCTick2TrigTime()`      |                     | `TPCTick2BeamTime()`      |                       |                        |
   * | @ref DetectorClocksTriggerTime             "trigger time"     |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   |                       |                      |                    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksOpticalElectronicsTime  "Optical"          |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `OpticalTick2Time()`  | `OpticalTick2TDC()`  |                    | `OpticalTick2TrigTime()`  |                     | `OpticalTick2BeamTime()`  |                       |                        |
   * | @ref DetectorClocksExternalElectronicsTime "External"         |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `ExternalTick2Time()` | `ExternalTick2TDC()` |                    | `ExternalTick2TrigTime()` |                     | `ExternalTick2BeamTime()` |                       |                        |
   * | @ref DetectorClocksSimulationTime          "simulation time"  | `G4ToElecTime()`      | `TPCG4Time2TDC()`    | `TPCG4Time2Tick()` |                           |                     |                           | `OpticalG4Time2TDC()` | `ExternalG4Time2TDC()` |
   *
   * Note that the complete definition of optical and external time requires
   * additional information: see the
   * @ref DetectorClocksElectronicsTimesNote "note on electonics time frames"
   * below.
   *
   * The names are not thoroughly consistent, but they roughly follow the
   * following rules:
   * * the "trigger time" is named as `TrigTime` (but see below)
   * * the "beam time" is named as `BeamTime` (but see below)
   * * the @ref DetectorClocksElectronicsTime "electronics time" is named simply
   *     as `Time`; note that e.g. `TriggerTime()` and `BeamGateTime()` must be
   *     read as `Trigger` and `BeamGate` plus `Time()`, that is they return a
   *     `Time` value (i.e., electronics time) of the
   *     @ref DetectorClocksHardwareTrigger "hardware trigger instant" and
   *     @ref DetectorClocksBeamGateOpening "beam gate opening" instant, in the
   *     electronics time frame
   * * the @ref DetectorClocksElectronicsTime "electronics time ticks" are
   *     labelled `TDC`
   * * the @ref DetectorClocksSimulationTime "simulation time" is labelled `G4`
   *     or `G4Time`
   * * `TPCTick` is @ref DetectorClocksTPCelectronicsTime "TPC electronics clock"
   *     ticks, measured from the
   *     @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time"
   * * `Tick` (without further qualification) is equivalent to "TPCTick" above
   * * `TPC` is usually used in conjunction with `Tick`, where it means
   *   `TPCTick`; but in `TPCG4Time2TDC()` it is a misnomer (the `TDC` wins in
   *   that the result is in ticks in
   *   @ref DetectorClocksElectronicsTime "electronics time" reference, not in
   *   @ref DetectorClocksTPCelectronicsTime "TPC electronics time")
   *
   *
   * @anchor DetectorClocksElectronicsTimesNote
   * Note on electronics time frames
   * --------------------------------
   *
   * The conversion of some electronic time ticks, especially the
   * @ref DetectorClocksOpticalElectronicsTime "optical" and
   * @ref DetectorClocksExternalElectronicsTime "external" ones, assumes that
   * the tick values to be converted are relative to an arbitrary reference,
   * meaning that the absolute (or, e.g., electronics) time for tick `0` is not
   * centrally defined. Instead, it is expected that additional information is
   * provided to locate when this tick `0` actually happens. This is usually
   * done via another pair of arguments, the frame number and the sample number
   * within that frame. In the end, this just moves the questions to when frame
   * `0` sample `0` happens: as those are input arguments, that absolute
   * reference is left to the caller to define; but usually it is referring to
   * the @ref DetectorClocksElectronicsStartTime "electronics start time".
   *
   *
   * @anchor DetectorClocksIntroClocks
   * Clocks
   * -------
   *
   * A clock object (`ElecClock`) contains settings for a specific
   * hardware clock. `DetectorClocks` provides four clock objects:
   *
   * clock name        | purpose                               | default time
   * ----------------- | ------------------------------------- | ---------------
   * `TPCClock()`      | TPC readout and "general" electronics | `TriggerTime()`
   * `OpticalClock()`  | optical detector electronics          | `TriggerTime()`
   * `TriggerClock()`  | hardware trigger electronics          | `TriggerTime()`
   * `ExternalClock()` | _not specified_                       | `TriggerTime()`
   *
   * A clock object _does not change the time frame_: in the conversions between
   * times and ticks, and the other way around, it always assumes that at tick 0
   * (and frame 0, sample 0, if needed), the time is also 0 (see the
   * @ref DetectorClocksElectronicsTimesNote "note on electronics time frames"
   * above). Therefore, once again, **a clock object does not help in converting
   * between different time scales**, and the output times are in the same time
   * frame as the input.
   *
   * The "default time" is the time the clock is set when returned by the
   * methods with no argument (e.g. `TriggerClock()`).
   *
   * All times (including frame lengths) are measured in microseconds, and all
   * the frequencies are measured in megahertz.
   *
   * `TPCClock()` deals with TPC readout, and the "electronics time" clock is
   * defined to have the same parameters as the TPC readout one.
   *
   * @note The `TriggerClock()` clock has little in common with `TriggerTime()`:
   *       the former is an electronics setting for a piece of hardware
   *       generating the trigger, while the latter is the instant the
   *       @ref DetectorClocksHardwareTrigger "hardware trigger" actually
   *       happened.
   *
   */

#include "lardataalg/DetectorInfo/ElecClock.h"

namespace detinfo {

  class DetectorClocksData {
  public:
    DetectorClocksData(double const g4_ref_time,
                       double const trigger_offset_tpc,
                       double const trig_time,
                       double const beam_time,
                       ElecClock const& tpc_clock,
                       ElecClock const& optical_clock,
                       ElecClock const& trigger_clock,
                       ElecClock const& external_clock)
      : fTriggerTime{trig_time}
      , fTriggerOffsetTPC{trigger_offset_tpc}
      , fBeamGateTime{beam_time}
      , fG4RefTime{g4_ref_time}
      , fTPCClock{tpc_clock}
      , fOpticalClock{optical_clock}
      , fTriggerClock{trigger_clock}
      , fExternalClock{external_clock}
    {}

    /**
     * @see `detinfo::DetectorClocks::TriggerOffsetTPC()`
     *
     * This offset is set via configuration parameter `TriggerOffsetTPC`.
     */
    double
    TriggerOffsetTPC() const
    {
      if (fTriggerOffsetTPC < 0)
        return fTriggerOffsetTPC;
      else
        return -fTriggerOffsetTPC / fTPCClock.Frequency(); // convert ticks to
                                                           // us
    }

    /// Returns the @ref DetectorClocksTPCelectronicsStartTime "TPC electronics
    /// start time" in @ref DetectorClocksElectronicsTime "electronics time".
    double
    TPCTime() const
    {
      return doTPCTime();
    }

    /// Given Geant4 time [ns], returns relative time [us] w.r.t. electronics
    /// time T0
    double
    G4ToElecTime(double const g4_time) const
    {
      return g4_time * 1.e-3 - fG4RefTime;
    }

    /// Trigger electronics clock time in [us]
    double
    TriggerTime() const
    {
      return fTriggerTime;
    }

    /// Beam gate electronics clock time in [us]
    double
    BeamGateTime() const
    {
      return fBeamGateTime;
    }

    //
    // Getters of TPC ElecClock
    //
    /// Borrow a const TPC clock with time set to Trigger time [us]
    ElecClock const&
    TPCClock() const noexcept
    {
      return fTPCClock;
    }

    //
    // Getters of Optical ElecClock
    //
    /// Borrow a const Optical clock with time set to Trigger time [us]
    ElecClock const&
    OpticalClock() const noexcept
    {
      return fOpticalClock;
    }

    //
    // Getters of Trigger ElecClock
    //
    /// Borrow a const Trigger clock with time set to Trigger time [us]
    ElecClock const&
    TriggerClock() const noexcept
    {
      return fTriggerClock;
    }

    //
    // Getters of External ElecClock
    //
    /// Borrow a const Trigger clock with time set to External Time [us]
    ElecClock const&
    ExternalClock() const noexcept
    {
      return fExternalClock;
    }

    //
    // Getters for time [us] w.r.t. trigger given information from waveform
    //

    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. trigger
    /// time stamp
    double
    TPCTick2TrigTime(double const tick) const
    {
      return fTPCClock.TickPeriod() * tick + TriggerOffsetTPC();
    }
    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. beam gate
    /// time
    double
    TPCTick2BeamTime(double const tick) const
    {
      return TPCTick2TrigTime(tick) + TriggerTime() - BeamGateTime();
    }
    /// Given Optical time-tick (waveform index), sample and frame number,
    /// returns time [us] w.r.t. trigger time stamp
    double
    OpticalTick2TrigTime(double const tick, size_t const sample, size_t const frame) const
    {
      return fOpticalClock.TickPeriod() * tick + fOpticalClock.Time(sample, frame) - TriggerTime();
    }
    /// Given Optical time-tick (waveform index), sample and frame number,
    /// returns time [us] w.r.t. beam gate time stamp
    double
    OpticalTick2BeamTime(double const tick, size_t const sample, size_t const frame) const
    {
      return fOpticalClock.TickPeriod() * tick + fOpticalClock.Time(sample, frame) - BeamGateTime();
    }
    /// Given External time-tick (waveform index), sample and frame number,
    /// returns time [us] w.r.t. trigger time stamp
    double
    ExternalTick2TrigTime(double const tick, size_t const sample, size_t const frame) const
    {
      return fExternalClock.TickPeriod() * tick + fExternalClock.Time(sample, frame) -
             TriggerTime();
    }
    /// Given External time-tick (waveform index), sample and frame number,
    /// returns time [us] w.r.t. beam gate time stamp
    double
    ExternalTick2BeamTime(double const tick, size_t const sample, size_t const frame) const
    {
      return fExternalClock.TickPeriod() * tick + fExternalClock.Time(sample, frame) -
             BeamGateTime();
    }

    /// Returns the specified electronics time in TDC electronics ticks.
    double
    Time2Tick(double const time) const
    {
      return doTime2Tick(time);
    }

    //
    // Getters for time [tdc] (electronics clock counting ... in double
    // precision)
    //

    /// Given TPC time-tick (waveform index), returns electronics clock count
    /// [tdc]
    double
    TPCTick2TDC(double const tick) const
    {
      return (doTPCTime() / fTPCClock.TickPeriod() + tick);
    }
    /// Given G4 time [ns], returns corresponding TPC electronics clock count
    /// [tdc]
    double
    TPCG4Time2TDC(double const g4time) const
    {
      return G4ToElecTime(g4time) / fTPCClock.TickPeriod();
    }
    /// Given Optical time-tick (waveform index), sample and frame number,
    /// returns time electronics clock count [tdc]
    double
    OpticalTick2TDC(double const tick, size_t const sample, size_t const frame) const
    {
      return fOpticalClock.Ticks(sample, frame) + tick;
    }
    /// Given G4 time [ns], returns corresponding Optical electronics clock
    /// count [tdc]
    double
    OpticalG4Time2TDC(double const g4time) const
    {
      return G4ToElecTime(g4time) / fOpticalClock.TickPeriod();
    }
    /// Given External time-tick (waveform index), sample and frame number,
    /// returns time electronics clock count [tdc]
    double
    ExternalTick2TDC(double const tick, size_t const sample, size_t const frame) const
    {
      return fExternalClock.Ticks(sample, frame) + tick;
    }
    /// Given G4 time [ns], returns corresponding External electronics clock
    /// count [tdc]
    double
    ExternalG4Time2TDC(double const g4time) const
    {
      return G4ToElecTime(g4time) / fExternalClock.TickPeriod();
    }

    //
    // Getters for time [us] (electronics clock counting ... in double
    // precision)
    //
    /// Given TPC time-tick (waveform index), returns electronics clock [us]
    double
    TPCTick2Time(double const tick) const
    {
      return doTPCTime() + tick * fTPCClock.TickPeriod();
    }
    /// Given Optical time-tick (waveform index), sample and frame number,
    /// returns electronics clock [us]
    double
    OpticalTick2Time(double const tick, size_t const sample, size_t const frame) const
    {
      return fOpticalClock.Time(sample, frame) + tick * fOpticalClock.TickPeriod();
    }
    /// Given External time-tick (waveform index), sample and frame number,
    /// returns electronics clock [us]
    double
    ExternalTick2Time(double const tick, size_t const sample, size_t const frame) const
    {
      return fExternalClock.Time(sample, frame) + tick * fExternalClock.TickPeriod();
    }

    //
    // Getters for time [ticks] (waveform index number)
    //

    /// Given electronics clock count [tdc] returns TPC time-tick
    double
    TPCTDC2Tick(double const tdc) const
    {
      return (tdc - doTPCTime() / fTPCClock.TickPeriod());
    }
    /// Given G4 time returns electronics clock count [tdc]
    double
    TPCG4Time2Tick(double const g4time) const
    {
      return (G4ToElecTime(g4time) - doTPCTime()) / fTPCClock.TickPeriod();
    }
    
    
    template <typename Stream>
    void debugReport(Stream& out) const
    {
      out
        <<   "Trigger  time @ " << fTriggerTime
        << "\nBeamGate time @ " << fBeamGateTime
        << "\nTrigOffsetTPC @ " << TriggerOffsetTPC()
        << "\nG4RefTime     @ " << fG4RefTime
        << "\nTPC     Freq. @ " << fTPCClock.Frequency()
        << "\nOptical Freq. @ " << fOpticalClock.Frequency()
        << "\nTrigger Freq. @ " << fTriggerClock.Frequency()
        << "\nExternal Freq. @ " << fExternalClock.Frequency()
        << "\nTPC start tick [tdc]             : " << TPCTick2TDC(0)
        << "\nTPC start tick from trigger [us] : " << TPCTick2TrigTime(0)
        << "\nTPC start tick from beam    [us] : " << TPCTick2BeamTime(0)
        << "\nTPC tdc=0 in tick     : " << TPCTDC2Tick(0)
        << "\nTPC G4 time 0 in tick : " << TPCG4Time2Tick(0)
        << "\nTrigger in TPC tick   : " << Time2Tick(TriggerTime())
        << "\n";
    }

  private:
    /// Trigger time in [us]
    double fTriggerTime;

    /// Time offset from trigger to TPC readout start
    double fTriggerOffsetTPC;

    /// BeamGate time in [us]
    double fBeamGateTime;

    /// Electronics clock counting start time in G4 time frame [us]
    double fG4RefTime;

    ElecClock fTPCClock;
    ElecClock fOpticalClock;
    ElecClock fTriggerClock;
    ElecClock fExternalClock;

    /// Implementation of `TPCTime()`.
    double
    doTPCTime() const
    {
      return fTriggerTime + fTriggerOffsetTPC;
    }

    /// Implementation of `Time2Tick()`.
    double
    doTime2Tick(double const time) const
    {
      return (time - doTPCTime()) / fTPCClock.TickPeriod();
    }

  }; // class DetectorClocksData

  inline int
  trigger_offset(DetectorClocksData const& data)
  {
    return data.TPCClock().Ticks(data.TriggerOffsetTPC() * -1.);
  }

  /**
   * @brief Returns the period of the TPC readout electronics clock.
   * @returns the period of the TPC readout electronics clock [&micro;s]
   * @see `detinfo::DetectorClocks::TPCClock()`
   */
  inline double
  sampling_rate(DetectorClocksData const& data)
  {
    return data.TPCClock().TickPeriod() * 1.e3;
  }

} // namespace detinfo

#endif // LARDATAALG_DETECTORINFO_DETECTORCLOCKSDATA_H
