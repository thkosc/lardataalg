////////////////////////////////////////////////////////////////////////
//
// DetectorClocks.h
//
//     This class provides electronics various electronics clocks. Currently supports
//     three types of clocks: TPC, Optical, and Trigger in order to support the
//     MicroBooNE experiment.
//
//     Formally known as TimeService.
//
////////////////////////////////////////////////////////////////////////

#ifndef LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARD_H
#define LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARD_H

#include <stddef.h>
#include <string>
#include <vector>

#include "fhiclcpp/fwd.h"
#include "lardataalg/DetectorInfo/DetectorClocks.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandardTriggerLoader.h"
#include "lardataalg/DetectorInfo/ElecClock.h"

namespace detinfo {

  /**
   * @brief Implementation of `detinfo::DetectorClocks` interface with fixed
   *        settings from configuration.
   *
   * In this implementation, all quantities are defined at configuration time
   * and are unchanged for the duration of the entire job, with the exception
   * of the trigger time (see below).
   *
   * Configuration parameters
   * =========================
   *
   * All configuration parameters are mandatory, unless explicitly stated.
   *
   * * *G4RefTime* (_nanoseconds_):
   *     @ref DetectorClocksSimulationTime "simulation (Geant4) start time" in
   *     @ref DetectorClocksElectronicsTime "electronics time scale", i.e. when
   *     time `0.0` of simulation happens in the electronics time scale
   * * *TriggerOffsetTPC*: time elapsed between the
   *     @ref DetectorClocksTPCelectronicsStartTime "start of the TPC readout clock"
   *     and the @ref DetectorClocksHardwareTrigger "hardware trigger"; it can
   *     be expressed in one of two ways:
   *       * negative number [&micro;s]: the offset of the start of the TPC
   *         readout clock start respect to the trigger time (where negative
   *         means that the clock starts _before_ the trigger arrives)
   *       * positive number [ticks]: the number of TPC readout clock tick at
   *         which the trigger arrives; despite this being a tick number, it can
   *         be fractional for added precision
   *
   *     For example, `TriggerOffsetTPC` of `-1600.0` means that the TDC clock
   *     starts 1.6 milliseconds before the hardware trigger. `TriggerOffsetTPC`
   *     of `3200.0` means that the trigger arrives at the exact start of tick
   *     3200 of the TPC readout. In this example, if the sampling frequency of
   *     that readout is 2 MHz, these two settings are equivalent.
   * * *FramePeriod* (_microseconds_): duration of an electronics clock frame;
   *     @ref DetectorClocksIntroClocks "all clocks" share the same frame period
   * * *ClockSpeedTPC* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "TPC electronics clock"
   * * *ClockSpeedOptical* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "optical electronics clock"
   * * *ClockSpeedTrigger* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "trigger electronics clock"
   * * *ClockSpeedExternal* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "external electronics clock"
   * * *DefaultTrigTime* (_microseconds_): the default
   *     @ref DetectorClocksHardwareTrigger "hardware trigger time", measured in
   *     the @ref DetectorClocksElectronicsTime "electronics time frame"
   * * *DefaultBeamTime* (_microseconds_): the default
   *     @ref DetectorClocksBeamGateOpening "beam gate opening time", measured
   *     in the @ref DetectorClocksElectronicsTime "electronics time frame"
   * * *TrigModuleName* (_string_): input tag for the trigger data product
   *     (see "Trigger time" section below)
   * * *InheritClockConfig* (_boolean_): whether to inherit the configuration
   *     from previous jobs (see "Consistency check" below)
   *
   *
   * Consistency check
   * ------------------
   *
   * The consistency check feature verifies that the current configuration of
   * `detinfo::DetectorClocksStandard` is compatible with the one from previous
   * jobs. It is expected that when this feature is enabled
   * (`InheritClockConfig` is configured to be `true`), either the configuration
   * is overridden to follow the previous ones, or an exception is thrown in
   * case of inconsistency.
   *
   * The service provider (`detinfo::DetectorClocksStandard`) does not provide
   * a facility to inherit configuration from a previous job, but it stores the
   * expectation whether this should happen (`InheritClockConfig()`).
   * The service provider manager is in charge of implementing this feature
   * (see `detinfo::DetectorClocksStandardService` for the manager in the _art_
   * environment).
   *
   *
   * Timing specifics
   * =================
   *
   * For the general timing requirements, see the documentation of
   * `detinfo::DetectorClocks`.
   *
   *
   * Trigger time
   * -------------
   *
   * In this implementation, the trigger time and the beam gate time both
   * default to 0.0 &micro;s.
   *
   * The manager of this provider is expected to set those values by calling
   * `detinfo::DetectorClocksStandard::SetTriggerTime()`. See
   * `detinfo::DetectorClocksStandardService` for the manager in the _art_
   * environment.
   *
   *
   * @bug `ExternalClock()` clock is never initialized!
   *
   */
  class DetectorClocksStandard final : public DetectorClocks {
  public:
    DetectorClocksStandard(fhicl::ParameterSet const& pset);
    DetectorClocksStandard(DetectorClocksStandard const&) = delete;

    void
    SetConfigValue(size_t i, double val)
    {
      fConfigValue[i] = val;
    }

    /**
     * @brief Returns the input tag of the trigger data product.
     * @return the input tag of the trigger data product (as string)
     *
     * The trigger module name is set directly in the configuration as
     * `TrigModuleName`.
     */
    std::string const&
    TrigModuleName() const
    {
      return fTrigModuleName;
    }

    /**
     * @brief Returns the input tag of the trigger data product for G4Ref correctons.
     * @return the input tag of the trigger data product (as string)
     *
     * The trigger module name is set directly in the configuration as
     * `G4RefCorrTrigModuleName`.
     */
    std::string
    G4RefCorrTrigModuleName() const
    {
      return fG4RefCorrTrigModuleName;
    }

    std::vector<std::string> const&
    ConfigNames() const override
    {
      return fConfigName;
    }
    std::vector<double> const&
    ConfigValues() const override
    {
      return fConfigValue;
    }

    DetectorClocksData
    DataForJob() const override
    {
      return DetectorClocksData{
        fConfigValue[kG4RefTime], // FIXME: Should be run-dependent?
        fTriggerOffsetTPC,
        fTriggerTime,
        fBeamGateTime,
        ElecClock{fTriggerTime, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedTPC]},
        ElecClock{fTriggerTime, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedOptical]},
        ElecClock{fTriggerTime, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedTrigger]},
        ElecClock{0, kDEFAULT_FRAME_PERIOD, kDEFAULT_FREQUENCY_EXTERNAL}};
    }

    DetectorClocksData
    DataFor(double const g4_ref_time,
            double const trigger_time,
            double const beam_time) const override
    {
      return DetectorClocksData{
        g4_ref_time,
        fTriggerOffsetTPC,
        trigger_time,
        beam_time,
        ElecClock{trigger_time, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedTPC]},
        ElecClock{trigger_time, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedOptical]},
        ElecClock{trigger_time, fConfigValue[kFramePeriod], fConfigValue[kClockSpeedTrigger]},
        ElecClock{0, kDEFAULT_FRAME_PERIOD, kDEFAULT_FREQUENCY_EXTERNAL}};
    }

    /// Internal function to apply loaded parameters to member attributes
    void ApplyParams();

    /// Internal function used to search for the right configuration set in the
    /// data file
    bool IsRightConfig(const fhicl::ParameterSet& ps) const;

  private:
    /**
     * @brief Setter for trigger times.
     * @param trig_time @ref DetectorClocksHardwareTrigger "hardware trigger
     * time" in @ref DetectorClocksElectronicsTime "electronics time scale"
     * @param beam_time @ref DetectorClocksBeamGateOpening "beam gate opening
     * time" in @ref DetectorClocksElectronicsTime "electronics time scale"
     *
     * The @ref DetectorClocksHardwareTrigger "hardware trigger" and
     * @ref DetectorClocksBeamGateOpening "beam gate opening" times are set, and
     * the electronic clocks are updated to store the new trigger time.
     */
    void
    SetTriggerTime(double const trig_time, double const beam_time)
    {
      fTriggerTime = trig_time;
      fBeamGateTime = beam_time;
      fTPCClock = ElecClock{fTriggerTime, fFramePeriod, fConfigValue[kClockSpeedTPC]};
    }

    double
    TriggerOffsetTPC() const
    {
      if (fTriggerOffsetTPC < 0)
        return fTriggerOffsetTPC;
      else
        return -fTriggerOffsetTPC / fTPCClock.Frequency(); // convert ticks to
                                                           // us
    }

    std::vector<std::string> fConfigName;
    std::vector<double> fConfigValue;

    std::string fTrigModuleName;
    std::string fG4RefCorrTrigModuleName;

    /// Time offset from trigger to TPC readout start
    double fTriggerOffsetTPC;

    /// Trigger time in [us]
    double fTriggerTime;

    /// BeamGate time in [us]
    double fBeamGateTime;

    /// Frame period
    double fFramePeriod;

    ElecClock fTPCClock;
  }; // class DetectorClocksStandard

} // namespace detinfo

#endif // LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARD_H
