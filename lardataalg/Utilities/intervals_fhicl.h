/**
 * @file   lardataalg/Utilities/intervals_fhicl.h
 * @brief  Utilities to read interval and point quantity FHiCL configuration.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 27, 2019
 *
 */

#ifndef LARDATAALG_UTILITIES_INTERVALS_FHICL_H
#define LARDATAALG_UTILITIES_INTERVALS_FHICL_H

// LArSoft libraries
#include "larcorealg/CoreUtils/StdUtils.h" // util::to_string()
#include "lardataalg/Utilities/intervals.h"
#include "lardataalg/Utilities/quantities_fhicl.h"

// support libraries
#include "fhiclcpp/coding.h"

// C++ libraries
#include <any>
#include <string>
#include <string_view>

namespace util::quantities::concepts {

  // --- BEGIN -- FHiCL encoding ---------------------------------------------
  /// @name FHiCL encoding
  /// @{

  // these utilities need to be defined in the same namespace as `Quantity`

  /**
   * @brief Decodes an interval.
   * @tparam Args types defining the interval type
   * @param src the data to decode
   * @param iv the interval where to store the result
   *
   * This function fills the object `iv` with information decoded from `src`.
   *
   * The decoding happens with `util::quantities::makeInterval()`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  void decode(std::any const& src, Interval<Args...>& iv);

  /**
   * @brief Decodes a quantity point.
   * @tparam Args types defining the quantity point type
   * @param src the data to decode
   * @param p the quantity point where to store the result
   *
   * This function fills the object `iv` with information decoded from `src`.
   *
   * The decoding happens with `util::quantities::makePoint()`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  void decode(std::any const& src, Point<Args...>& p);

  /**
   * @brief Encodes a quantity interval into a FHiCL parameter set atom.
   * @tparam Args types defining the quantity interval type
   * @param iv the interval to be encoded
   * @return the interval encoded into a FHiCL parameter set atom
   *
   * This function returns a parameter set atom with the content of the
   * quantity interval `iv`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  ::fhicl::detail::ps_atom_t encode(Interval<Args...> const& iv);

  /**
   * @brief Encodes a quantity point into a FHiCL parameter set atom.
   * @tparam Args types defining the quantity point type
   * @param pt the quantity point to be encoded
   * @return the point encoded into a FHiCL parameter set atom
   *
   * This function returns a parameter set atom with the content of the
   * quantity point `p`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  ::fhicl::detail::ps_atom_t encode(Point<Args...> const& pt);

  /// @}
  // --- END -- FHiCL encoding -----------------------------------------------

} // namespace util::quantities::concepts

// -----------------------------------------------------------------------------
// ---  template implementation
// -----------------------------------------------------------------------------
template <typename... Args>
void util::quantities::concepts::decode(std::any const& src, Interval<Args...>& iv)
{
  using interval_t = Interval<Args...>;
  using quantity_t = typename interval_t::quantity_t;

  quantity_t q;
  util::quantities::concepts::decode(src, q);
  iv = interval_t{q};

} // util::quantities::concepts::decode(Interval)

// -----------------------------------------------------------------------------
template <typename... Args>
void util::quantities::concepts::decode(std::any const& src, Point<Args...>& pt)
{
  using point_t = Point<Args...>;
  using quantity_t = typename point_t::quantity_t;

  quantity_t q;
  util::quantities::concepts::decode(src, q);
  pt = point_t{q};

} // util::quantities::concepts::decode(Point)

// -----------------------------------------------------------------------------
template <typename... Args>
::fhicl::detail::ps_atom_t util::quantities::concepts::encode(Interval<Args...> const& iv)
{
  return ::fhicl::detail::encode(util::to_string(iv));
} // util::quantities::concepts::encode(Interval)

// -----------------------------------------------------------------------------
template <typename... Args>
::fhicl::detail::ps_atom_t util::quantities::concepts::encode(Point<Args...> const& p)
{
  return ::fhicl::detail::encode(util::to_string(p));
} // util::quantities::concepts::encode(Point)

// -----------------------------------------------------------------------------

#endif // LARDATAALG_UTILITIES_INTERVALS_FHICL_H
