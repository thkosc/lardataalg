/**
 * @file   lardataalg/DetectorInfo/DetectorTimingTypes.h
 * @brief  Data types for `detinfo::DetectorTimings`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 31, 2019
 * 
 */

#ifndef LARDATAALG_DETECTORINFO_DETECTORTIMINGTYPES_H
#define LARDATAALG_DETECTORINFO_DETECTORTIMINGTYPES_H

// LArSoft libraries
#include "lardataalg/Utilities/quantities/spacetime.h"
#include "lardataalg/Utilities/quantities/frequency.h"
#include "lardataalg/Utilities/quantities/electronics.h"


/// Namespace including different time scales as defined in LArSoft.
namespace detinfo::timescales {
  
  
  /**
   * @brief A collection of traits for a time scale.
   * @tparam Cat category the traits belong to
   * 
   * The included traits are documented in
   * `details::timescale_traits_base`.
   * 
   * 
   * Writing traits for a category
   * ==============================
   * 
   * Traits are specified by specializing the `timescale_traits` class
   * for the category. The easiest way is to derive the specialized class
   * from `details::timescale_traits_base<Cat>` and add the necessary
   * customization:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace detinfo::timescales {
   *   template <>
   *   struct timescale_traits<MyCat>: details::timescale_traits_base<MyCat>
   *   {
   *     // more customization as needed
   *   };
   * } // namespace detinfo::timescales
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Category compatibility
   * -----------------------
   * 
   * Among the traits, one offers whether a specified category is "compatible"
   * with this one. The easiest way to implement that is to define a set of
   * compatible categories, and then an object which returns whether a
   * category is among them. This object should override
   * `category_compatible_with` member of each relevant category trait.
   * A way to achieve that is to define a specific trait base class:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct TimeOnePointOneCategory: util::quantities::concepts::CategoryBase
   *   { static std::string name() { return "time 1.1"; } };
   * 
   * struct TimeOnePointTwoCategory: util::quantities::concepts::CategoryBase
   *   { static std::string name() { return "time 1.2"; } };
   * 
   * template <typename TimeOneCat>
   * struct timeone_traits_base
   *   : detinfo::timescales::details::timescale_traits_base<TimeOneCat>
   * {
   *   template <typename OC>
   *   static constexpr bool category_compatible_with = util::is_any_of_v
   *     <OC, TimeOnePointOneCategory, TimeOnePointTwoCategory>;
   * };
   * 
   * namespace detinfo::timescales {
   *   template <>
   *   struct timescale_traits<::TimeOnePointOneCategory>
   *     : ::timeone_traits_base<::TimeOnePointOneCategory>
   *   {};
   *   
   *   template <>
   *   struct timescale_traits<::TimeOnePointTwoCategory>
   *     : ::timeone_traits_base<::TimeOnePointTwoCategory>
   *   {};
   *   
   * } // namespace detinfo::timescales
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  template <typename Cat>
  struct timescale_traits;
  
  
  /// Type of time interval (natively in microseconds).
  /// Intentionally cross-category.
  using time_interval = util::quantities::intervals::microseconds;
  
  
  namespace details {
    
    template <typename Cat, typename = void>
    struct category_of_t { using type = Cat; };
    
    template <typename Cat>
    struct category_of_t<Cat, std::void_t<typename Cat::category_t>>
      { using type = typename Cat::category_t; };
    
    template <typename Cat>
    using category_of = typename category_of_t<Cat>::type;
    
    /// A template traits for time scale of category `Cat`.
    template <typename Cat, typename TimeUnit = util::quantities::microsecond>
    struct timescale_traits_base {
      
      /// The category this time scale belongs to.
      using category_t = Cat;
      
      /// Type of a time interval in this scale.
      using time_interval_t = time_interval;
      
      /// Type of a point on this time scale.
      using time_point_t = util::quantities::concepts::Point
        <TimeUnit, category_t, time_interval_t>;
      
      /// Type of frequency for this time scale.
      using frequency_t
        = decltype(1.0 / std::declval<typename time_interval_t::quantity_t>());
      
      /// An interval on this time scale expressed in its ticks (integral).
      using tick_interval_t = util::quantities::concepts::Interval
        <util::quantities::tick, category_t>;
      
      /// An interval on this time scale expressed in its ticks (real).
      using tick_interval_d_t = util::quantities::concepts::Interval
        <util::quantities::tick_d, category_t>;
      
      /// A point on this time scale expressed in its ticks.
      using tick_t = util::quantities::concepts::Point
        <util::quantities::tick, category_t, tick_interval_t>;
      
      /// A point on this time scale expressed in its ticks (real).
      using tick_d_t = util::quantities::concepts::Point
        <util::quantities::tick_d, category_t, tick_interval_d_t>;
      
      /// Name of this time scale.
      static std::string name() { return category_t::name(); }
      
      /// Returns whether the category `OC` is the same as this one.
      template <typename OC>
      static constexpr bool same_category_as
        = std::is_same_v<category_of<OC>, category_t>;
      
      /// Returns whether the category `OC` is compatible with this one.
      template <typename OC>
      static constexpr bool category_compatible_with
        = same_category_as<category_of<OC>>;
      
    }; // timescale_traits_base<>
    
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  /// Category for electronics time scale.
  struct ElectronicsTimeCategory: util::quantities::concepts::CategoryBase {
    static std::string name() { return { "electronics time" }; }
  }; // struct ElectronicsTimeCategory
  
  
  /**
   * @brief Timing types for electronics time scale.
   * 
   * This object collects data types meant to represent a time on the
   * @ref DetectorClocksElectronicsTime "electronics time axis", starting at the
   * @ref DetectorClocksElectronicsStartTime "electronics start time".
   * 
   * It exposes the interface documented in `details::timescale_traits_base`.
   */
  template <>
  struct timescale_traits<ElectronicsTimeCategory>
    : public details::timescale_traits_base<ElectronicsTimeCategory>
    {};
  
  
  // ---------------------------------------------------------------------------
  
  /// Category for TPC electronics time scale.
  struct TPCelectronicsTimeCategory: util::quantities::concepts::CategoryBase {
    static std::string name() { return { "TPC electronics time" }; }
  }; // struct TPCelectronicsTimeCategory
  
  
  /**
   * @brief Timing types for TPC electronics time scale.
   * 
   * This object collects data types meant to represent a time on the
   * @ref DetectorClocksTPCelectronicsTime "TPC electronics time axis", starting
   * at the
   * @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time".
   * 
   * It exposes the interface documented in `details::timescale_traits_base`.
   * 
   * This time is natively expressed in microseconds.
   */
  template <>
  struct timescale_traits<TPCelectronicsTimeCategory>
    : public details::timescale_traits_base<TPCelectronicsTimeCategory>
    {};
  
  
  // ---------------------------------------------------------------------------
  
  /// Category for electronics time scale.
  struct OpticalTimeCategory: util::quantities::concepts::CategoryBase {
    static std::string name() { return { "optical electronics time" }; }
  }; // struct OpticalTimeCategory
  
  
  /**
   * @brief Timing types for optical detector time scale.
   * 
   * This object collects data types meant to represent a time on the
   * @ref DetectorClocksOpticalElectronicsTime "optical detector electronics time axis",
   * starting at the
   * @ref DetectorClocksOpticalElectronicsStartTime "optical detector electronics start time".
   * 
   * @note Unfortunately, that time is not defined, so the functions that need
   *       it pin it down to the start of the electronics time scale.
   * 
   * This traits expose the interface documented in
   * `details::timescale_traits_base`.
   * 
   * This time is natively expressed in microseconds.
   */
  template <>
  struct timescale_traits<OpticalTimeCategory>
    : public details::timescale_traits_base<OpticalTimeCategory>
    {};
  
  
  // ---------------------------------------------------------------------------
  
  /// Category for trigger time scale.
  struct TriggerTimeCategory: util::quantities::concepts::CategoryBase {
    static std::string name() { return { "hardware trigger time" }; }
  }; // struct TriggerTimeCategory
  
  
  /**
   * @brief Timing types for trigger electronics time scale.
   * 
   * This object collects data types meant to represent a time on the
   * @ref DetectorClocksTriggerTime "trigger time axis", starting at the
   * @ref DetectorClocksHardwareTrigger "hardware trigger time".
   * 
   * This traits expose the interface documented in
   * `details::timescale_traits_base`.
   * 
   * This time is natively expressed in microseconds.
   */
  template <>
  struct timescale_traits<TriggerTimeCategory>
    : public details::timescale_traits_base<TriggerTimeCategory>
    {};
  
  
  // ---------------------------------------------------------------------------
  
  /// Category for electronics time scale.
  struct SimulationTimeCategory: util::quantities::concepts::CategoryBase {
    static std::string name() { return { "simulation time" }; }
  }; // struct SimulationTimeCategory
  
  
  /**
   * @brief Timing types for simulation time scale.
   * 
   * This object collects data types meant to represent a time on the
   * @ref DetectorClocksSimulationTime "simulation time axis", starting at the
   * @ref DetectorClocksGeant4TimeStart "GEANT4 time start".
   * 
   * This traits expose the interface documented in
   * `details::timescale_traits_base`.
   * 
   * This time is natively expressed in nanoseconds.
   */
  template <>
  struct timescale_traits<SimulationTimeCategory>
    : public details::timescale_traits_base
      <SimulationTimeCategory, util::quantities::nanosecond>
    {};
  
  
  // ---------------------------------------------------------------------------
  
  // --- BEGIN -- Continuous times ---------------------------------------------
  /// @name Continuous times
  /// @{
  
  /**
   * @brief A point in time on the electronics time scale.
   * 
   * This object is nothing special, but it is meant to represent a time on
   * the @ref DetectorClocksElectronicsTime "electronics time axis", starting
   * at the @ref DetectorClocksElectronicsStartTime "electronics start time".
   * 
   * This time is natively expressed in microseconds.
   */
  using electronics_time
    = timescale_traits<ElectronicsTimeCategory>::time_point_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  /**
   * @brief A point in time on the TPC electronics time scale.
   * 
   * This object is meant to represent a time on the
   * @ref DetectorClocksTPCelectronicsTime "TPC electronics time axis",
   * starting at the
   * @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time".
   * 
   * This time is natively expressed in microseconds.
   */
  using TPCelectronics_time
    = timescale_traits<TPCelectronicsTimeCategory>::time_point_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  /**
   * @brief A point in time on the optical detector electronics time scale.
   * 
   * This object is meant to represent a time on the
   * @ref DetectorClocksOpticalElectronicsTime "optical detector electronics time axis",
   * starting at the
   * @ref DetectorClocksOpticalElectronicsStartTime "optical detector electronics start time".
   * 
   * @note Unfortunately, that time is not defined, so the functions that need
   *       it pin it down to the start of the electronics time scale.
   * 
   * This time is natively expressed in microseconds.
   */
  using optical_time
    = timescale_traits<OpticalTimeCategory>::time_point_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  /**
   * @brief A point in time on the trigger time scale.
   * 
   * This object is nothing special, but it is meant to represent a time on
   * the @ref DetectorClocksTriggerTime "trigger time axis", starting
   * at the @ref DetectorClocksHardwareTrigger "hardware trigger time".
   * 
   * This time is natively expressed in microseconds.
   */
  using trigger_time = timescale_traits<TriggerTimeCategory>::time_point_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /**
   * @brief A point in time on the simulation time scale.
   * 
   * This object is meant to represent a time on the
   * @ref DetectorClocksSimulationTime "simulation time axis", starting at the
   * @ref DetectorClocksGeant4TimeStart "GEANT4 time start".
   * 
   * This time is natively expressed in nanoseconds.
   */
  using simulation_time
    = timescale_traits<SimulationTimeCategory>::time_point_t;
  
  
  /// @}
  // --- END -- Continuous times ---------------------------------------------
  
  
  // --- BEGIN -- Tick-based times -------------------------------------------
  /// @name Tick-based times
  /// @{
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /// A point on the electronics time scale expressed in its ticks.
  using electronics_tick
    = timescale_traits<ElectronicsTimeCategory>::tick_t;
  
  /// A point on the electronics time scale expressed in its ticks (real).
  using electronics_tick_d
    = timescale_traits<ElectronicsTimeCategory>::tick_d_t;
  
  /// An interval on the electronics time scale expressed in its ticks.
  using electronics_time_ticks
    = timescale_traits<ElectronicsTimeCategory>::tick_interval_t;
  
  /// An interval on the electronics time scale expressed in its ticks (real).
  using electronics_time_ticks_d
    = timescale_traits<ElectronicsTimeCategory>::tick_interval_d_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  /// A point on the TPC electronics time scale expressed in its ticks.
  using TPCelectronics_tick
    = timescale_traits<TPCelectronicsTimeCategory>::tick_t;
  
  /// A point on the TPC electronics time scale expressed in its ticks (real).
  using TPCelectronics_tick_d
    = timescale_traits<TPCelectronicsTimeCategory>::tick_d_t;
  
  /// An interval on the TPC electronics time scale expressed in its ticks.
  using TPCelectronics_time_ticks
    = timescale_traits<TPCelectronicsTimeCategory>::tick_interval_t;
  
  /// An interval on the TPC electronics time scale expressed in its ticks
  /// (real).
  using TPCelectronics_time_ticks_d
    = timescale_traits<TPCelectronicsTimeCategory>::tick_interval_d_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
  /// A point on the optical detector electronics time scale expressed in its
  /// ticks.
  using optical_tick = timescale_traits<OpticalTimeCategory>::tick_t;
  
  /// A point on the optical detector electronics time scale expressed in its
  /// ticks (real).
  using optical_tick_d = timescale_traits<OpticalTimeCategory>::tick_d_t;
  
  /// An interval on the optical detector electronics time scale expressed in
  /// its ticks.
  using optical_time_ticks
    = timescale_traits<OpticalTimeCategory>::tick_interval_t;
  
  /// An interval on the optical detector electronics time scale expressed in
  /// its ticks (real).
  using optical_time_ticks_d
    = timescale_traits<OpticalTimeCategory>::tick_interval_d_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  /// A point on the trigger time scale expressed in its ticks.
  using trigger_tick = timescale_traits<TriggerTimeCategory>::tick_t;
  
  /// A point on the trigger time scale expressed in its ticks (real).
  using trigger_tick_d = timescale_traits<TriggerTimeCategory>::tick_d_t;
  
  /// An interval on the trigger time scale expressed in its ticks.
  using trigger_time_ticks
    = timescale_traits<TriggerTimeCategory>::tick_interval_t;
  
  /// An interval on the trigger time scale expressed in its ticks (real).
  using trigger_time_ticks_d
    = timescale_traits<TriggerTimeCategory>::tick_interval_d_t;
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /// Evaluates to whether the specified time `T` is tick-based.
  template <typename T>
  struct is_tick_type;
  
  /// Whether the specified time `T` is tick-based.
  template <typename T>
  constexpr bool is_tick_v = is_tick_type<T>();
  
  
  /// @}
  // --- END -- Tick-based times ---------------------------------------------
  
  
} // namespace detinfo::timescales


//------------------------------------------------------------------------------

/// Namespace including different time scales as defined in LArSoft.
namespace detinfo::timescales {
  
  // ---------------------------------------------------------------------------
  namespace details {
    
    // -------------------------------------------------------------------------
    template <typename T, typename = void>
    struct is_tick_type_impl: std::false_type {};
    
    
    template <typename Q>
    struct is_tick_type_impl<Q, std::enable_if_t<
      std::is_same_v<typename Q::baseunit_t, util::quantities::units::Tick>
      >>
      : std::true_type
    {};
    
    
    // -------------------------------------------------------------------------
    
    // Generally undefined; suggested to derive from `timescale_traits_base`.
    template <typename, typename = void>
    struct custom_timescale_traits;
    
    // Specialization for quantities: same traits as quantity category.
    template <typename WC>
    struct custom_timescale_traits
      <WC, std::enable_if_t
        <util::quantities::concepts::is_interval_or_point_v<WC>>
      >
      : public timescale_traits<typename WC::category_t>
    {};
    
    
    // -------------------------------------------------------------------------
  
  } // namespace details
  
  
  // ---------------------------------------------------------------------------
  
  // For customization, pick on `details::custom_timescale_traits`.
  template <typename Cat>
  struct timescale_traits: public details::custom_timescale_traits<Cat> {};
  
  
  // ---------------------------------------------------------------------------
  template <typename T>
  struct is_tick_type: details::is_tick_type_impl<T> {};
  
  // ---------------------------------------------------------------------------
  
  
} // namespace detinfo::timescales


//------------------------------------------------------------------------------


#endif // LARDATAALG_DETECTORINFO_DETECTORTIMINGTYPES_H
