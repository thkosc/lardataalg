/**
 * \file lardataalg/DetectorInfo/DetectorClocks.h
 *
 * \brief pure virtual base interface for detector clocks
 *
 * \author jpaley@fnal.gov
 *
 */
#ifndef LARDATAALG_DETECTORINFO_DETECTORCLOCKS_H
#define LARDATAALG_DETECTORINFO_DETECTORCLOCKS_H

#include "lardataalg/DetectorInfo/DetectorClocksData.h"
#include "lardataalg/DetectorInfo/ElecClock.h"

// C++ libraries
#include <string>
#include <vector>

namespace detinfo {

  enum ConfigType_t {
    kG4RefTime = 0,
    kTriggerOffsetTPC,
    kFramePeriod,
    kClockSpeedTPC,
    kClockSpeedOptical,
    kClockSpeedTrigger,
    kClockSpeedExternal,
    kDefaultTrigTime,
    kDefaultBeamTime,
    kConfigTypeMax
  };

  /** **************************************************************************
   * @brief Class used for the conversion of times between different
   *        formats and references.
   *
   * `detinfo::DetectorClocks` is an abstract interface enclosing the set of
   * supported queries. A concrete implementation of this interface needs to be
   * provided in order to use this facility. LArSoft provides
   * `detinfo::DetectorClocksStandard` as a simple, reasonable implementation.
   * Initialization and configuration are designed by each implementation.
   *
   */

  class DetectorClocks {
  public:
    virtual ~DetectorClocks() = default;

    /// Returns a list of provider configuration names.
    /// @see ConfigValues()
    virtual std::vector<std::string> const& ConfigNames() const = 0;

    /// Returns a list of provider configuration values
    /// (same order as `ConfigNames()`).
    /// @see ConfigNames()
    virtual std::vector<double> const& ConfigValues() const = 0;

    virtual DetectorClocksData DataForJob() const = 0;

    virtual DetectorClocksData DataFor(double g4_ref_time,
                                       double trigger_time,
                                       double beam_time) const = 0;
  }; // class DetectorClocks

} // namespace detinfo

#endif // LARDATAALG_DETECTORINFO_DETECTORCLOCKS_H
