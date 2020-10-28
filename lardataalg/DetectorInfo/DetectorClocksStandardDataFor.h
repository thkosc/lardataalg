/**
 * @file   lardataalg/DetectorInfo/DetectorClocksStandardDataFor.h
 * @brief  Helper to get clocks data from `detinfo::DetectorClocksStandard`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   October 14, 2020
 * 
 * This library is header-only.
 */

#ifndef LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARDDATAFOR_H
#define LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARDDATAFOR_H

// LArSoft libraries
#include "lardataalg/DetectorInfo/DetectorClocksStandardTriggerLoader.h"
#include "lardataobj/RawData/TriggerData.h" // raw::Trigger

// framework libraries
#include "canvas/Utilities/InputTag.h"
#include "cetlib_except/exception.h"

// C++ standard libraries
#include <optional>
#include <vector>

namespace detinfo {

  /**
   * @brief Returns `DetectorClocksData` tuned on the specified `event`.
   * @tparam Event type of framework event
   * @param detClocks service provider generating the data
   * @param event event to read information from
   * @return `DetectorClocksData` tuned on the specified `event`
   * 
   * This function takes care to extract all what is needed by
   * `DetectorClocksStandard` service provider in order to provide data for
   * the event.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * detinfo::DetectorClocksStandard const detClocks
   *   { config.get<fhicl::ParameterSet>("services.DetectorClocksService") };
   * 
   * for (gallery::Event event(inputFiles); !event.atEnd(); event.next()) {
   *   
   *   detinfo::DetectorClocksData const clockData
   *     = detinfo::detectorClocksStandardDataFor(detClocks, event);
   *   
   *   auto const triggerTime = clockData.TriggerTime();
   *   
   *   // etc...
   *   
   * } // for
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Requirements
   * -------------
   * 
   * The implementation is effectively dependent on the framework managing the
   * `event`, but it is not _formally_ dependent on any implementation.
   * Assumptions include everything that is required by other helper functions
   * like `detinfo::trigger_times_for_event()` and
   * `detinfo::g4ref_time_for_event()` (mostly, support for a call like
   * `Event::getByLabel(art::InputTag, Event::HandleT<T>)`).
   */
  template <typename Event>
  detinfo::DetectorClocksData detectorClocksStandardDataFor(
    detinfo::DetectorClocksStandard const& detClocks,
    Event const& event
  ) {

    auto const& config_values = detClocks.ConfigValues();
    // Trigger times
    double trig_time{config_values[kDefaultTrigTime]};
    double beam_time{config_values[kDefaultBeamTime]};
    if (auto times = trigger_times_for_event(detClocks.TrigModuleName(), event)) {
      std::tie(trig_time, beam_time) = *times;
    }

    double g4_ref_time{config_values[kG4RefTime]};
    if (auto sim_trig_time = g4ref_time_for_event(detClocks.G4RefCorrTrigModuleName(), event)) {
      g4_ref_time -= trig_time;
      g4_ref_time += *sim_trig_time;
    }
    return detClocks.DataFor(g4_ref_time, trig_time, beam_time);
  } // detinfo::detectorClocksStandardDataFor()


} // namespace detinfo



#endif // LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H
