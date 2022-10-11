/**
 * @file   lardataalg/Utilities/constexpr_math.h
 * @brief  Mathematical functions that C++ standard doesn't require `constexpr`.
 * @author Gianluca Petrillo
 * @date   January 11, 2019
 *
 * This is a header-only library.
 */

#ifndef LARDATAALG_UTILITIES_CONSTEXPR_MATH_H
#define LARDATAALG_UTILITIES_CONSTEXPR_MATH_H

namespace util {

  // --- BEGIN simple mathematical functions -----------------------------------
  /// @name simple mathematical functions
  /// @{

  /**
   * @brief Returns the absolute value of the argument.
   * @tparam T type of the argument
   * @param v value to be processed
   * @return the absolute value of v
   *
   * If the value `v` is negative, its opposite is returned.
   * Note that this implementation does not work with data types that are not
   * comparable (like `std::complex`).
   *
   *
   * Requirements
   * -------------
   *
   * * constexpr construction of a `T` value from the literal `0`
   * * `operator- (T) constexpr`
   * * `operator< (T, T) constexpr` convertible to `bool`
   *
   */
  // this bizarre implementation is supposed to guarantee that `abs(-0)` is `0`
  template <typename T>
  constexpr auto abs(T v)
  {
    return (-T(0) < v) ? v : -v;
  }

  /// @}

  // --- END simple mathematical functions -------------------------------------

} // namespace util

#endif // LARDATAALG_UTILITIES_CONSTEXPR_MATH_H
