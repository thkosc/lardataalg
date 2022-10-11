/**
 * @file   lardataalg/Utilities/quantities/energy.h
 * @brief  Dimensioned variables representing energy.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   January 20, 2020
 * @see    lardataalg/Utilities/quantities.h
 *
 * Set of basic quantities related to energy. Currently, quantities
 * are defined based on the following units:
 * * electronvolt (meV, eV, keV, MeV, GeV, TeV)
 *
 * This is a header-only library.
 *
 * @todo Also belong here: joule, watt, eV/c, eV/c^2
 *
 */

#ifndef LARDATAALG_UTILITIES_QUANTITIES_ENERGY_H
#define LARDATAALG_UTILITIES_QUANTITIES_ENERGY_H

// LArSoft libraries
#include "lardataalg/Utilities/quantities.h"

// C/C++ standard libraries
#include <ratio>
#include <string_view>

//------------------------------------------------------------------------------
namespace util::quantities {

  namespace units {

    using namespace std::string_view_literals; // for operator""sv()

    struct ElectronVolt : public concepts::UnitBase {
      static constexpr auto symbol = "eV"sv;
      static constexpr auto name = "electronvolt"sv;
    };

  } // namespace units

  // -- BEGIN Particle energy --------------------------------------------------
  /**
   * @name Particle energy quantities
   *
   * These energy quantities are tied to
   * `util::quantities::units::ElectronVolt`.
   * A few options are provided:
   *
   * * most general template, `scaled_electronvolt`, allowing to choose both the
   *     scale of the unit (e.g. `std::exa` for exaelectronvolt) and the type of
   *     the numerical representation
   * * generic templates (e.g. `electronvolt_as`), allowing to choose which
   *     numerical representation to use
   * * double precision (e.g. `electronvolt`), ready for use
   *
   */
  /// @{

  /// The most generic `units::ElectronVolt`-based quantity.
  template <typename R, typename T = double>
  using scaled_electronvolt = concepts::scaled_quantity<units::ElectronVolt, R, T>;

  //
  // electronvolt
  //
  /// Type of energy stored in electronvolt.
  template <typename T = double>
  using electronvolt_as = scaled_electronvolt<std::ratio<1>, T>;

  /// Type of energy stored in electronvolts, in double precision.
  using electronvolt = electronvolt_as<>;

  //
  // microelectronvolt
  //
  /// Type of energy stored in microelectronvolt.
  template <typename T = double>
  using microelectronvolt_as = concepts::rescale<electronvolt_as<T>, std::micro>;

  /// Type of energy stored in microelectronvolt, in double precision.
  using microelectronvolt = microelectronvolt_as<>;

  //
  // millielectronvolt
  //
  /// Type of energy stored in millielectronvolt.
  template <typename T = double>
  using millielectronvolt_as = concepts::rescale<electronvolt_as<T>, std::milli>;

  /// Type of energy stored in millielectronvolt, in double precision.
  using millielectronvolt = millielectronvolt_as<>;

  //
  // kiloelectronvolt
  //
  /// Type of energy stored in kiloelectronvolt.
  template <typename T = double>
  using kiloelectronvolt_as = concepts::rescale<electronvolt_as<T>, std::kilo>;

  /// Type of energy stored in kiloelectronvolt, in double precision.
  using kiloelectronvolt = kiloelectronvolt_as<>;

  //
  // megaelectronvolt
  //
  /// Type of energy stored in megaelectronvolt.
  template <typename T = double>
  using megaelectronvolt_as = concepts::rescale<electronvolt_as<T>, std::mega>;

  /// Type of energy stored in megaelectronvolt, in double precision.
  using megaelectronvolt = megaelectronvolt_as<>;

  //
  // gigaelectronvolt
  //
  /// Type of energy stored in gigaelectronvolt.
  template <typename T = double>
  using gigaelectronvolt_as = concepts::rescale<electronvolt_as<T>, std::giga>;

  /// Type of energy stored in gigaelectronvolt, in double precision.
  using gigaelectronvolt = gigaelectronvolt_as<>;

  //
  // teraelectronvolt
  //
  /// Type of energy stored in teraelectronvolt.
  template <typename T = double>
  using teraelectronvolt_as = concepts::rescale<electronvolt_as<T>, std::tera>;

  /// Type of energy stored in teraelectronvolt, in double precision.
  using teraelectronvolt = teraelectronvolt_as<>;

  /// @}
  // -- END Particle energy ----------------------------------------------------

  /**
   * @brief Literal constants for energy quantities.
   *
   * These functions allow a simplified syntax for specifying a energy quantity.
   * In order to use these, their namespace must be used:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * using namespace util::quantities::energy_literals;
   *
   * // definition of `util::quantities::megaelectronvolt` constant:
   * constexpr auto muon_MeV = 105.6583745_MeV;
   *
   * // assignment (likely to a quantity) of
   * // `util::quantities::gigaelectronvolt{1.5}`
   * E = 1.5_GeV;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   */
  namespace energy_literals {

    // @{
    /// Literal electronvolt value.
    constexpr electronvolt operator""_eV(long double v)
    {
      return electronvolt{static_cast<double>(v)};
    }
    constexpr electronvolt operator""_eV(unsigned long long int v)
    {
      return electronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal microelectronvolt value.
    constexpr microelectronvolt operator""_ueV(long double v)
    {
      return microelectronvolt{static_cast<double>(v)};
    }
    constexpr microelectronvolt operator""_ueV(unsigned long long int v)
    {
      return microelectronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal millielectronvolt value.
    constexpr millielectronvolt operator""_meV(long double v)
    {
      return millielectronvolt{static_cast<double>(v)};
    }
    constexpr millielectronvolt operator""_meV(unsigned long long int v)
    {
      return millielectronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal kilovolt value.
    constexpr kiloelectronvolt operator""_keV(long double v)
    {
      return kiloelectronvolt{static_cast<double>(v)};
    }
    constexpr kiloelectronvolt operator""_keV(unsigned long long int v)
    {
      return kiloelectronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal megaelectronvolt value.
    constexpr megaelectronvolt operator""_MeV(long double v)
    {
      return megaelectronvolt{static_cast<double>(v)};
    }
    constexpr megaelectronvolt operator""_MeV(unsigned long long int v)
    {
      return megaelectronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal gigaelectronvolt value.
    constexpr gigaelectronvolt operator""_GeV(long double v)
    {
      return gigaelectronvolt{static_cast<double>(v)};
    }
    constexpr gigaelectronvolt operator""_GeV(unsigned long long int v)
    {
      return gigaelectronvolt{static_cast<double>(v)};
    }
    // @}

    // @{
    /// Literal teraelectronvolt value.
    constexpr teraelectronvolt operator""_TeV(long double v)
    {
      return teraelectronvolt{static_cast<double>(v)};
    }
    constexpr teraelectronvolt operator""_TeV(unsigned long long int v)
    {
      return teraelectronvolt{static_cast<double>(v)};
    }
    // @}

  } // energy_literals

} // namespace util::quantities

//------------------------------------------------------------------------------

#endif // LARDATAALG_UTILITIES_QUANTITIES_ENERGY_H
