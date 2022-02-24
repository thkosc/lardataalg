/**
 * @file   lardataalg/DetectorInfo/DetectorClocksStandardTriggerLoader.h
 * @brief  Functions to load trigger time in `detinfo::DetectorClocksStandard`.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 21, 2018
 *
 * This header contains framework-dependent functions to:
 * * set `DetectorClocksStandard` trigger time from a `raw::Trigger`
 * * set `DetectorClocksStandard` trigger time from a `raw::Trigger` in an event
 * * do the former, and fall back to default values on failure
 *
 * These functions are compatible with both _art_ and _gallery_.
 *
 * This is a pure header library. Effective dependencies include:
 * * `lardata_DetectorInfo`
 * * `lardataobj_RawData`
 * * `canvas`
 * * `cetlib_except`
 *
 */

#ifndef LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H
#define LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H

// LArSoft libraries
#include "lardataobj/RawData/TriggerData.h" // raw::Trigger

// framework libraries
#include "canvas/Utilities/InputTag.h"
#include "cetlib_except/exception.h"

// C++ standard libraries
#include <optional>
#include <vector>

namespace detinfo {

  /**
   * @brief Loads `DetectorClocksStandard` trigger times.
   * @tparam Event type of event where trigger data might be stored
   * @param triggerTag tag of the `raw::Trigger` collection data product to read
   * @param event the event the trigger objects are stored into
   * @return optional pair of trigger and beam gate time, empty if not found
   * @throws cet::exception (category `"setDetectorClocksStandardTrigger"`)
   *                        if trigger data product has more than one trigger
   *
   * This function returns the relative trigger and beam gate times read from an `event`.
   * It attempts to read the information from a `raw::Trigger` collection data product
   * with tag `triggerTag` in the `event`. Times are expected to be in microseconds
   * on the electronics time scale.
   * In case that data product does not exist, or it exists but empty,
   * an empty result is quietly returned.
   * If instead there are multiple trigger objects in the collection,
   * no choice is made, and an exception is thrown.
   */
  template <typename Event>
  std::optional<std::pair<double, double>>
  trigger_times_for_event(art::InputTag const& triggerTag, Event const& event)
  {
    // try to read the trigger from the event
    // fetch the trigger data product
    using TriggerHandle_t = typename Event::template HandleT<std::vector<raw::Trigger>>;

    TriggerHandle_t triggerHandle;
    if (!event.template getByLabel(triggerTag, triggerHandle)) { return std::nullopt; }

    // check that we do have a trigger
    // (we have already checked whether the handle is valid above)
    auto const& triggers = *triggerHandle;
    if (triggers.empty()) { return std::nullopt; }

    // select which trigger to set (i.e., the only one!)
    if (triggers.size() != 1) {
      throw cet::exception("setDetectorClocksStandardTrigger")
        << "Found " << triggers.size() << " trigger objects in '" << triggerTag.encode()
        << "' (only one trigger per event is supported)\n";
    }

    auto const& trigger = triggers.front();
    return std::make_optional(std::make_pair(trigger.TriggerTime(), trigger.BeamGateTime()));
  }

  /**
   * @brief Loads `DetectorClocksStandard` G4Ref correction times.
   * @tparam Event type of event where trigger data might be stored
   * @param triggerTag tag of the `raw::Trigger` collection data product to read
   * @param event the event the trigger objects are stored into
   * @return optional G4 reference time value, empty if not found
   * @throws cet::exception (category `"setDetectorClocksStandardTrigger"`)
   *                        if trigger data product has more than one trigger
   *
   * This function returns the simulation (G4) reference time from an `event`.
   * It is assumed to match the trigger time (or, it is assumed that the trigger
   * fired at the time the event was generated).
   * The function attempts to read the information from a `raw::Trigger` collection
   * data product with tag `triggerTag` in the `event`. The time is expected to be
   * in microseconds on the electronics time scale.
   * In case that data product does not exist, or it exists but empty,
   * an empty result is quietly returned.
   * If instead there are multiple trigger objects in the collection,
   * no choice is made, and an exception is thrown.
      */
  template <typename Event>
  std::optional<double>
  g4ref_time_for_event(art::InputTag const& triggerTag, Event const& event)
  {
    // fetch the trigger data product
    using TriggerHandle_t = typename Event::template HandleT<std::vector<raw::Trigger>>;

    TriggerHandle_t triggerHandle;
    if (!event.template getByLabel(triggerTag, triggerHandle)) return std::nullopt;

    // check that we do have a trigger
    // (we have already checked whether the handle is valid above)
    auto const& triggers = *triggerHandle;
    if (triggers.empty()) return std::nullopt;

    // select which trigger to set (i.e., the only one!)
    if (triggers.size() != 1) {
      throw cet::exception("setDetectorClocksStandardTrigger")
        << "Found " << triggers.size() << " trigger objects in '" << triggerTag.encode()
        << "' (only one trigger per event is supported)\n";
    }

    return std::make_optional(triggers.front().TriggerTime());
  }

} // namespace detinfo

#endif // LARDATAALG_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H
