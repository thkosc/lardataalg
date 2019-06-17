/**
 * @file   lardataalg/Utilities/quantities/spacetime.h
 * @brief  Dimensioned variables representing space or time quantities.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   October 30, 2018
 * @see    lardataalg/Utilities/quantities.h
 *
 * Set of basic quantities related to space and time. Currently, quantities are
 * defined based on the following units:
 * * seconds (ps, ns, us, ms, s), with intervals in `intervals` namespace
 *
 * This is a header-only library.
 *
 */

#ifndef LARDATAALG_UTILITIES_QUANTITIES_SPACETIME_H
#define LARDATAALG_UTILITIES_QUANTITIES_SPACETIME_H

// LArSoft libraries
#include "lardataalg/Utilities/intervals.h"
#include "lardataalg/Utilities/quantities.h"

// C/C++ standard libraries
#include <string_view>
#include <ratio>


//------------------------------------------------------------------------------
namespace util::quantities {

  namespace units {

    using namespace std::string_view_literals; // for operator""sv()

    struct Second: public concepts::UnitBase {
      static constexpr auto symbol = "s"sv;
      static constexpr auto name   = "second"sv;
    };

  } // namespace units


  // -- BEGIN Time -------------------------------------------------------------
  /**
   * @name Time quantities
   *
   * These time quantities are tied to `util::quantities::units::Second`.
   * A few options are provided:
   *
   * * most general template, `scaled_second`, allowing to choose both the scale
   *     of the unit (e.g. `std::milli` for milliseconds) and the type of the
   *     numerical representation
   * * generic templates (e.g. `second_as`), allowing to choose which numerical
   *     representation to use
   * * double precision (e.g. `second`), ready for use
   *
   * For this unit in particular, additional options are provided to accommodate
   * the custom of using the unit in plural form: `seconds_as` and `seconds`
   * are exactly equivalent to the singular-named counterparts.
   */
  /// @{


  /// The most generic `units::Second`-based quantity.
  template <typename R, typename T = double>
  using scaled_second = concepts::scaled_quantity<units::Second, R, T>;

  //
  // seconds
  //
  /// Type of time stored in seconds.
  template <typename T = double>
  using second_as = scaled_second<std::ratio<1>, T>;

  /// Alias for common language habits.
  template <typename T = double>
  using seconds_as = second_as<T>;

  /// Type of time stored in seconds, in double precision.
  using second = second_as<>;

  /// Alias for common language habits.
  using seconds = second;

  //
  // milliseconds
  //
  /// Type of time stored in milliseconds.
  template <typename T = double>
  using millisecond_as = concepts::rescale<second_as<T>, std::milli>;

  /// Alias for common language habits.
  template <typename T = double>
  using milliseconds_as = millisecond_as<T>;

  /// Type of time stored in milliseconds, in double precision.
  using millisecond = millisecond_as<>;

  /// Alias for common language habits.
  using milliseconds = millisecond;

  //
  // microseconds
  //
  /// Type of time stored in microseconds.
  template <typename T = double>
  using microsecond_as = concepts::rescale<second_as<T>, std::micro>;

  /// Alias for common language habits.
  template <typename T = double>
  using microseconds_as = microsecond_as<T>;

  /// Type of time stored in microseconds, in double precision.
  using microsecond = microsecond_as<>;

  /// Alias for common language habits.
  using microseconds = microsecond;

  //
  // nanoseconds
  //
  /// Type of time stored in nanoseconds.
  template <typename T = double>
  using nanosecond_as = concepts::rescale<second_as<T>, std::nano>;

  /// Alias for common language habits.
  template <typename T = double>
  using nanoseconds_as = nanosecond_as<T>;

  /// Type of time stored in nanoseconds, in double precision.
  using nanosecond = nanosecond_as<>;

  /// Alias for common language habits.
  using nanoseconds = nanosecond;

  //
  // picoseconds
  //
  /// Type of time stored in picoseconds.
  template <typename T = double>
  using picosecond_as = concepts::rescale<second_as<T>, std::pico>;

  /// Alias for common language habits.
  template <typename T = double>
  using picoseconds_as = picosecond_as<T>;

  /// Type of time stored in picoseconds, in double precision.
  using picosecond = picosecond_as<>;

  /// Alias for common language habits.
  using picoseconds = picosecond;

  /**
   * @brief Literal constants for time quantities.
   *
   * These functions allow a simplified syntax for specifying a time quantity.
   * In order to use these, their namespace must be used:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * using namespace util::quantities::time_literals;
   *
   * // definition of `util::quantities::second` constant:
   * constexpr auto t_s = 12_s;
   *
   * // assignment (likely to a quantity) of
   * // `util::quantities::millisecond{500.0}`
   * t_s = 500_ms;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   */
  namespace time_literals {

    // @{
    /// Literal second value.
    constexpr second operator""_s (long double v)
      { return second{ static_cast<double>(v) }; }
    constexpr second operator""_s (unsigned long long int v)
      { return second{ static_cast<double>(v) }; }
    // @}

    // @{
    /// Literal millisecond value.
    constexpr millisecond operator""_ms (long double v)
      { return millisecond{ static_cast<double>(v) }; }
    constexpr millisecond operator""_ms (unsigned long long int v)
      { return millisecond{ static_cast<double>(v) }; }
    // @}

    // @{
    /// Literal microsecond value.
    constexpr microsecond operator""_us (long double v)
      { return microsecond{ static_cast<double>(v) }; }
    constexpr microsecond operator""_us (unsigned long long int v)
      { return microsecond{ static_cast<double>(v) }; }
    // @}

    // @{
    /// Literal nanosecond value.
    constexpr nanosecond operator""_ns (long double v)
      { return nanosecond{ static_cast<double>(v) }; }
    constexpr nanosecond operator""_ns (unsigned long long int v)
      { return nanosecond{ static_cast<double>(v) }; }
    // @}

    // @{
    /// Literal picosecond value.
    constexpr picosecond operator""_ps (long double v)
      { return picosecond{ static_cast<double>(v) }; }
    constexpr picosecond operator""_ps (unsigned long long int v)
      { return picosecond{ static_cast<double>(v) }; }
    // @}

  } // time_literals


  // --- BEGIN Time intervals --------------------------------------------------
  
  namespace intervals {
    
    /// The most generic `units::Second`-based interval.
    template <typename R, typename T = double>
    using scaled_seconds
      = concepts::Interval<util::quantities::scaled_second<R, T>>;

    //
    // seconds
    //
    /// Type of time interval stored in seconds.
    template <typename T = double>
    using seconds_as = scaled_seconds<std::ratio<1>, T>;

    /// Type of time stored in seconds, in double precision.
    using seconds = seconds_as<>;

    //
    // milliseconds
    //
    
    /// Type of time interval stored in milliseconds.
    template <typename T = double>
    using milliseconds_as = scaled_seconds<std::milli, T>;

    /// Type of time interval stored in milliseconds, in double precision.
    using milliseconds = milliseconds_as<>;

    //
    // microseconds
    //
    
    /// Type of time interval stored in microseconds.
    template <typename T = double>
    using microseconds_as = scaled_seconds<std::micro, T>;

    /// Type of time interval stored in microseconds, in double precision.
    using microseconds = microseconds_as<>;

    //
    // nanoseconds
    //
    
    /// Type of time interval stored in nanoseconds.
    template <typename T = double>
    using nanoseconds_as = scaled_seconds<std::nano, T>;

    /// Type of time interval stored in nanoseconds, in double precision.
    using nanoseconds = nanoseconds_as<>;

    //
    // picoseconds
    //
    
    /// Type of time interval stored in picoseconds.
    template <typename T = double>
    using picoseconds_as = scaled_seconds<std::pico, T>;

    /// Type of time interval stored in picoseconds, in double precision.
    using picoseconds = picoseconds_as<>;

  
  } // namespace intervals
  
  // --- END Time intervals ----------------------------------------------------
  
  
  // --- BEGIN Time points -----------------------------------------------------
  
  namespace points {
    
    /// The most generic `units::Second`-based point.
    template <typename R, typename T = double>
    using scaled_second
      = concepts::Point<util::quantities::scaled_second<R, T>>;

    //
    // second
    //
    /// Type of time point stored in seconds.
    template <typename T = double>
    using second_as = scaled_second<std::ratio<1>, T>;

    /// Type of time point stored in seconds, in double precision.
    using second = second_as<>;

    //
    // millisecond
    //
    
    /// Type of time point stored in milliseconds.
    template <typename T = double>
    using millisecond_as = scaled_second<std::milli, T>;

    /// Type of time point stored in milliseconds, in double precision.
    using millisecond = millisecond_as<>;

    //
    // microsecond
    //
    
    /// Type of time point stored in microseconds.
    template <typename T = double>
    using microsecond_as = scaled_second<std::micro, T>;

    /// Type of time point stored in microseconds, in double precision.
    using microsecond = microsecond_as<>;

    //
    // nanosecond
    //
    
    /// Type of time point stored in nanoseconds.
    template <typename T = double>
    using nanosecond_as = scaled_second<std::nano, T>;

    /// Type of time point stored in nanoseconds, in double precision.
    using nanosecond = nanosecond_as<>;

    //
    // picosecond
    //
    
    /// Type of time point stored in picoseconds.
    template <typename T = double>
    using picosecond_as = scaled_second<std::pico, T>;

    /// Type of time point stored in picoseconds, in double precision.
    using picosecond = picosecond_as<>;

  
  } // namespace points
  
  // --- END Time points -------------------------------------------------------
  
  
  /// @}
  // -- END Time ---------------------------------------------------------------
  
  
  
  
} // namespace util::quantities

//------------------------------------------------------------------------------


#endif // LARDATAALG_UTILITIES_QUANTITIES_SPACETIME_H
