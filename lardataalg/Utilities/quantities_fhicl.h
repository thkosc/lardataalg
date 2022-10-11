/**
 * @file   lardataalg/Utilities/quantities_fhicl.h
 * @brief  Utilities to read and write quantities in FHiCL configuration.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 27, 2019
 *
 */

#ifndef LARDATAALG_UTILITIES_QUANTITIES_FHICL_H
#define LARDATAALG_UTILITIES_QUANTITIES_FHICL_H

// LArSoft libraries
#include "larcorealg/CoreUtils/StdUtils.h" // util::to_string()
#include "lardataalg/Utilities/quantities.h"

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
   * @brief Decodes a quantity.
   * @tparam Args types defining the quantity type
   * @param src the data to decode
   * @param q the quantity where to store the result
   * @throw std::bad_any_cast if `src` does not provide the necessary data
   *
   * This function fills the object `q` with information decoded from `src`.
   *
   * The decoding happens with `util::quantities::makeQuantity()`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  void decode(std::any const& src, Quantity<Args...>& q);

  /**
   * @brief Encodes a quantity into a FHiCL parameter set atom.
   * @tparam Args types defining the quantity type
   * @param q the quantity to be encoded
   * @return the quantity encoded into a FHiCL parameter set atom
   *
   * This function returns a parameter set atom with the content of the quantity
   * `q`.
   *
   * @note The signature of this function is dictated by FHiCL requirements.
   */
  template <typename... Args>
  ::fhicl::detail::ps_atom_t encode(Quantity<Args...> const& q);

  /// @}
  // --- END -- FHiCL encoding -----------------------------------------------

} // namespace util::quantities::concepts

// -----------------------------------------------------------------------------
// ---  template implementation
// -----------------------------------------------------------------------------
template <typename... Args>
void util::quantities::concepts::decode(std::any const& src, Quantity<Args...>& q)
{
  using quantity_t = Quantity<Args...>;

  std::string s;
  ::fhicl::detail::decode(src, s);

  q = util::quantities::makeQuantity<quantity_t>(s);

} // util::quantities::concepts::decode(Quantity)

// -----------------------------------------------------------------------------
template <typename... Args>
::fhicl::detail::ps_atom_t util::quantities::concepts::encode(Quantity<Args...> const& q)
{
  return ::fhicl::detail::encode(util::to_string(q));
} // util::quantities::concepts::encode()

// -----------------------------------------------------------------------------

#endif // LARDATAALG_UTILITIES_QUANTITIES_FHICL_H
