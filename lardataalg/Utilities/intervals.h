/**
 * @file   lardataalg/Utilities/intervals.h
 * @brief  Defines point and interval variables based on quantities.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 29, 2019
 * @see    lardataalg/Utilities/quantites.h
 *
 */

#ifndef LARDATAALG_UTILITIES_INTERVALS_H
#define LARDATAALG_UTILITIES_INTERVALS_H

// LArSoft libraries
#include "lardataalg/Utilities/quantities.h"

// C/C++ standard libraries
#include <ostream>
// #include <string>
// #include <string_view>
// #include <ratio>
// #include <limits>
// #include <functional> // std::hash<>
#include <type_traits> // std::enable_if_t<>, ...


namespace util::quantities {
  
  namespace concepts {

    namespace details {

      //------------------------------------------------------------------------
      //--- Unit-related
      //------------------------------------------------------------------------
      /// Trait: `true_type` if `I` is a `Interval` specialization.
      template <typename I>
      struct is_interval;

      /// Trait: `true` if `I` is a `Interval` specialization.
      template <typename I>
      constexpr bool is_interval_v = is_interval<I>();

      //------------------------------------------------------------------------

    } // namespace details
    
    
    /** ************************************************************************
     * @brief An interval (duration, length, distance) between two
     *        quantity points.
     * @tparam Q quantity the interval is based on
     *
     * An interval shares most of the concepts of a `Quantity`, but it interacts
     * only with other intervals rather than with bare `Quantity` objects,
     * with the exception of construction.
     * In this sense, the relation between `Interval` and `Quantity` is similar
     * to the one between `Quantity` and its base type (`Quantity::value_t`).
     *
     */
    template <typename Q>
    struct Interval: private Q {

        public:
      
      using interval_t = Interval<Q>; ///< This type.
      
      
      // --- BEGIN -- Types from the base quantity -----------------------------
      /// @name Types from the base quantity
      /// @{
          
      using quantity_t = Q; /// Quantity the interval is based on.
      
      /// A quantity in the same unit, but possibly a different scale.
      template <typename R>
      using scaled_quantity_t = rescale<quantity_t, R>;
      
      /// Type of the stored value.
      using value_t = typename quantity_t::value_t;
      
      /// Description of the scaled unit.
      using unit_t = typename quantity_t::unit_t;

      /// Description of the unscaled unit.
      using baseUnit_t = typename quantity_t::baseUnit_t;
      
      /// @}
      // --- END -- Types from the base quantity -------------------------------

      /// Constructor: value is left uninitialized.
      // NOTE: this is not `constexpr` because using it in a constexpr would
      //       yield an uninitialized constant
      explicit Interval() = default;

      /// Constructor: takes a value in the intended representation.
      explicit constexpr Interval(value_t v): Interval(quantity_t{ v }) {}
      
      /**
       * @brief Constructor: converts from a quantity.
       * @tparam OQ type of the quantity
       * @param q quantity to be converted from
       *
       * The quantity is required to be in the same unit as this interval
       * (unit scale may differ).
       * The value in `q` is converted from its native scale into the one of
       * this interval.
       */
      template <
        typename OQ,
        typename std::enable_if_t<details::is_quantity_v<OQ>>* = nullptr
        >
      constexpr Interval(OQ q): quantity_t(quantity_t{ q }) {}

      /**
       * @brief Constructor: converts from another interval.
       * @tparam I type of the other interval
       * @param iv interval to be converted from
       *
       * Intervals are required to be in the same unit (unit scale may differ).
       * The value in `iv` is converted from its native scale into the one of
       * this interval.
       */
      template <
        typename I,
        typename std::enable_if_t<details::is_interval_v<I>>* = nullptr
        >
      constexpr Interval(I iv): Interval(quantity_t { iv.quantity() }) {}
      
      /// Returns the value of the interval as a quantity.
      constexpr quantity_t const& quantity() const { return *this; }

      /// Returns the value of the interval as plain value.
      using quantity_t::value;

      /// Implicit conversion to the base quantity.
      constexpr operator value_t() const { return value(); }
      

      // -- BEGIN Asymmetric arithmetic operations -----------------------------
      /**
       * @name Asymmetric operand arithmetic operations
       *
       * These arithmetic operations take care of preserving the interval unit
       * through them.
       * Not all possible (or reasonable) operations are supported yet.
       * Some operations that may be symmetric (like multiplication by a scalar)
       * are implemented as free functions rather than methods.
       *
       * @note These operations are essentially provided by convenience. There
       *       are many cases (corner and not) where the implicit conversion
       *       to the base type kicks in. This implementation does not aim to
       *       _prevent_ that from happening, but requests are welcome to add
       *       features to _allow_ that not to happen, with some care of the
       *       user.
       *
       */
      /// @{
      
      /// Returns an interval sum of this and `other`
      /// (must have exactly the same unit _and_ scale).
      constexpr interval_t operator+(interval_t const other) const
        { return interval_t{ quantity() + other.quantity() }; }

      /// Returns an interval difference of this and `other`
      /// (must have exactly the same unit _and_ scale).
      constexpr interval_t operator-(interval_t const other) const
        { return interval_t{ quantity() - other.quantity() }; }
      
      /// Division by an interval, returns a pure number.
      template <typename OQ>
      constexpr value_t operator/ (Interval<OQ> const denom) const
        { return quantity() / denom.quantity(); }

      /// Add a quantity (possibly converted) to this one.
      template <typename R>
      interval_t& operator+= (scaled_quantity_t<R> const other)
        { quantity_t::operator+= (other); return *this; }

      /// Add the `other` interval (possibly converted) to this one.
      template <typename OQ>
      interval_t& operator+= (Interval<OQ> const other)
        { return operator+= (other.quantity()); }

      /// Subtract a quantity (possibly converted) from this one.
      template <typename R>
      interval_t& operator-= (scaled_quantity_t<R> const other)
        { quantity_t::operator-= (other); return *this; }

      /// Subtract the `other` interval (possibly converted) from this one.
      template <typename OQ>
      interval_t& operator-= (Interval<OQ> const other)
        { return operator-= (other.quantity()); }

      /// Scale this interval by a factor.
      template <typename T>
      interval_t& operator*=(T factor)
        { quantity_t::operator*= (factor); return *this; }

      /// Scale the interval dividing it by a quotient.
      template <typename T>
      interval_t& operator/=(T factor)
        { quantity_t::operator/= (factor); return *this; }

      /// Returns an interval with same value.
      constexpr interval_t operator+() const { return interval_t(quantity()); }

      /// Returns an interval with same value but the sign flipped.
      constexpr interval_t operator-() const { return interval_t(-quantity()); }

      /// Returns an interval with the absolute value of this one.
      constexpr interval_t abs() const { return interval_t(quantity().abs()); }

      /// @}
      // -- END Asymmetric arithmetic operations -------------------------------


      // -- BEGIN Comparisons --------------------------------------------------
      /**
       * @name Comparisons.
       *
       * Comparisons with plain numbers are managed by implicit conversion.
       * More care is needed for quantities.
       * Comparisons between two quantity instances `a` and `b` work this way:
       * * if `a` and `b` do not have the same unit, they are _not_ comparable
       * * if `a` and `b` have the same unit, one is converted to the other and
       *     the comparison is performed there
       * * if `a` and `b` have the same scaled unit, their values are compared
       *     directly
       *
       * Value storage types are compared according to C++ rules.
       *
       */
      /// @{
      
      using quantity_t::operator==;
      using quantity_t::operator!=;
      using quantity_t::operator>=;
      using quantity_t::operator>;
      using quantity_t::operator<=;
      using quantity_t::operator<;
      
      template <typename OQ>
      constexpr bool operator==(Interval<OQ> const other) const
        { return operator==(other.quantity()); }

      template <typename OQ>
      constexpr bool operator!=(Interval<OQ> const other) const
        { return operator!=(other.quantity()); }

      template <typename OQ>
      constexpr bool operator>=(Interval<OQ> const other) const
        { return operator>=(other.quantity()); }

      template <typename OQ>
      constexpr bool operator>(Interval<OQ> const other) const
        { return operator>(other.quantity()); }

      template <typename OQ>
      constexpr bool operator<=(Interval<OQ> const other) const
        { return operator<=(other.quantity()); }

      template <typename OQ>
      constexpr bool operator<(Interval<OQ> const other) const
        { return operator<(other.quantity()); }

      
      /// @}
      // -- END Asymmetric arithmetic operations -------------------------------


      // -- BEGIN Access to the scaled unit ------------------------------------
      /// @name Access to the scaled unit.
      /// @{
      
      using quantity_t::unit;
      using quantity_t::baseUnit;
      using quantity_t::unitName;
      using quantity_t::unitSymbol;
      
      /// @}
      // -- END Access to the scaled unit --------------------------------------

      /// Convert this interval into the specified one.
      template <typename IV>
      IV convertInto() { return IV(*this); }
      
      /**
       * @brief Returns a new interval initialized with the specified value.
       * @tparam U type to initialize the quantity with
       * @param value the value to initialize the interval with
       * @return a new `Interval` object initialized with `value`
       * 
       * The `value` is cast into `value_t` via `static_cast()`.
       */
      template <typename U>
      static interval_t castFrom(U value)
        { return interval_t{ static_cast<value_t>(value) }; }
      
      
    }; // struct Interval

    template <typename... Args>
    std::ostream& operator<< (std::ostream& out, Interval<Args...> const iv)
      { return out << iv.quantity(); }


    // -- BEGIN Comparison operations ------------------------------------------
    /**
     * @name Comparison operations on `Interval`
     *
     * These operations, as well as the ones implemented as member functions,
     * are provided for convenience.
     *
     * Here the symmetric operations are defined, where different operands can
     * be swapped.
     *
     */
    /// @{
    
    template <typename Q, typename... Args>
    constexpr bool operator== (Quantity<Args...> const a, Interval<Q> const b)
      { return b == a; }

    template <typename Q, typename... Args>
    constexpr bool operator!= (Quantity<Args...> const a, Interval<Q> const b)
      { return b != a; }

    template <typename Q, typename... Args>
    constexpr bool operator<= (Quantity<Args...> const a, Interval<Q> const b)
      { return b >= a; }

    template <typename Q, typename... Args>
    constexpr bool operator< (Quantity<Args...> const a, Interval<Q> const b)
      { return b > a; }

    template <typename Q, typename... Args>
    constexpr bool operator>= (Quantity<Args...> const a, Interval<Q> const b)
      { return b <= a; }

    template <typename Q, typename... Args>
    constexpr bool operator> (Quantity<Args...> const a, Interval<Q> const b)
      { return b < a; }


    /// @}
    // -- END Comparison operations --------------------------------------------
    

    // -- BEGIN Arithmetic operations ------------------------------------------
    /**
     * @name Arithmetic operations on `Quantity`
     *
     * These operations, as well as the ones implemented as member functions,
     * are provided for convenience.
     *
     * Here the symmetric operations are defined, where different operands can
     * be swapped.
     *
     */
    /// @{

    //@{
    /// Multiplication with a scalar.
    template <typename Q, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q>>
    operator* (Interval<Q> const iv, T const factor)
      { return Interval<Q>{ iv.quantity() * factor }; }
    template <typename Q, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q>>
    operator* (T const factor, Interval<Q> const iv) { return iv * factor; }
    //@}

    /// Multiplication between quantities is forbidden.
    template <typename AQ, typename BQ>
    constexpr auto operator* (Interval<AQ> const, Interval<BQ> const)
//       -> decltype(std::declval<AQ>() * std::declval<AQ>())
      = delete;

    // Division by a scalar.
    template <typename Q, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q>>
    operator/ (Interval<Q> const iv, T const quot)
      { return Interval<Q>{ iv.quantity() / quot }; }

    /// @}
    // -- END Arithmetic operations --------------------------------------------


    // -------------------------------------------------------------------------
    /// Type of an interval like `IV`, but with a different unit scale `R`.
    template <typename IV, typename R, typename T = typename IV::value_t>
    using rescale_interval = Interval<rescale<typename IV::quantity_t, R, T>>;


    // -------------------------------------------------------------------------

  } // namespace concepts


#if 0
  // ---------------------------------------------------------------------------
  /**
   * @brief Definitions of additional prefixes.
   *
   * Quantities are based on _scaled_ units, which are units with a scaling
   * factor.
   *
   * Prefixes describe these scaling factors, as a rational number represented
   * via a fraction. In this library, prefix objects must expose the same
   * interface as `std::ratio` template.
   *
   * The standard ratio classes defined in C++ (e.g. `std::milli`, `std::giga`)
   * provide most of the needed prefixes.
   * In this namespace, custom prefixes can be defined.
   *
   * A prefix can be associated with its own symbol. In that case, the prefix
   * should specialize the template `util::quantites::concepts::Prefix` and
   * provide:
   *
   * * static, `constexpr` method for the full name (`name`), e.g. `"milli"`
   * * static, `constexpr` method for the short name (`symbol`), e.g. `"m"`
   *
   * For example, this prefix should work like 1'000, but has its own symbol:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace util::quantities::prefixes {
   *
   *   struct grant: public std::ratio<1000> {};
   *
   *   template <>
   *   struct Prefix<grant> {
   *
   *     /// Returns the symbol of the prefix.
   *     static constexpr auto symbol() { return "k"sv; }
   *
   *     /// Returns the full name of the prefix.
   *     static constexpr auto name() { return "grant"sv; }
   *
   *   }; // struct Prefix<grant>
   *
   * } // namespace util::quantities::prefixes
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Note however that the current implementation tends to bring back to
   * `std::ratio`. Also note that defining an alias, like in
   * `using grant = std::ratio<1000>`, is not enough to allow for a different
   * symbol.
   *
   * The namespace also imports the prefixes from C++ standard library for
   * convenience.
   */
  namespace prefixes { // we expect other libraries to fill it

    // ratios imported from C++ standard library:
    using
      std::atto, std::femto, std::pico, std::nano, std::micro,
      std::milli, std::centi, std::deci,
      std::deca, std::hecto, std::kilo,
      std::mega, std::giga, std::tera, std::peta, std::exa
      ;

  } // namespace prefixes

  // ---------------------------------------------------------------------------
  /**
   * @brief Definitions of actual units.
   *
   * Units describe a reference quantity to measure a dimension.
   * The namespace `units` contains the definitions of actual units (e.g.
   * seconds, ampere...)
   *
   * Each unit is represented by a class. Each class should follow the interface
   * of `util::quantities::concepts::UnitBase`, but it does not have to inherit
   * from it.
   *
   * Each unit must provide its name and its symbol (no locale is supported!).
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace util::quantities::units {
   *
   *   /// Unit of data size.
   *   struct byte {
   *     static constexpr auto symbol = "B"sv;
   *     static constexpr auto name   = "byte"sv;
   *   }; // byte
   *
   * } // namespace util::quantities::units
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  namespace units {} // we expect other libraries to fill it


  // ---------------------------------------------------------------------------

#endif // 0
} // namespace util::quantities


//------------------------------------------------------------------------------
//---  template implementation
//------------------------------------------------------------------------------
namespace util::quantities::concepts::details {

#if 0
  //----------------------------------------------------------------------------
  template <std::intmax_t Num, std::intmax_t Den>
  struct invert_ratio<std::ratio<Num, Den>>
    { using type = std::ratio<Den, Num>; };


  //----------------------------------------------------------------------------
  template <std::intmax_t Num, std::intmax_t Den>
  struct ratio_simplifier<std::ratio<Num, Den>> {
    static constexpr auto gcd = boost::integer::gcd(Num, Den);
    using type = std::ratio<Num / gcd, Den / gcd>;
  }; // ratio_simplifier

#endif // 0

  //----------------------------------------------------------------------------
  template <typename I>
  struct is_interval: public std::false_type {};

  template <typename... Args>
  struct is_interval<Interval<Args...>>: public std::true_type {};

#if 0

  //----------------------------------------------------------------------------
  /// Limits of a quantity are the same as the underlying type.
  template <typename Q>
  class numeric_limits: public std::numeric_limits<typename Q::value_t> {
    
    static_assert(is_quantity_v<Q>);
    
    using quantity_t = Q;
    using value_traits_t = std::numeric_limits<typename quantity_t::value_t>;
    
      public:
    
    static constexpr quantity_t min() noexcept
      { return quantity_t{ value_traits_t::min() }; }
    static constexpr quantity_t max() noexcept
      { return quantity_t{ value_traits_t::max() }; }
    static constexpr quantity_t lowest() noexcept
      { return quantity_t{ value_traits_t::lowest() }; }
    static constexpr quantity_t epsilon() noexcept
      { return quantity_t{ value_traits_t::epsilon() }; }
    static constexpr quantity_t round_error() noexcept
      { return quantity_t{ value_traits_t::round_error() }; }
    static constexpr quantity_t infinity() noexcept
      { return quantity_t{ value_traits_t::infinity() }; }
    static constexpr quantity_t quiet_NaN() noexcept
      { return quantity_t{ value_traits_t::quiet_NaN() }; }
    static constexpr quantity_t signaling_NaN() noexcept
      { return quantity_t{ value_traits_t::signaling_NaN() }; }
    static constexpr quantity_t denorm_min() noexcept
      { return quantity_t{ value_traits_t::denorm_min() }; }
    
  }; // numeric_limits<Quantity>
  

#endif // 0
  //----------------------------------------------------------------------------
  
} // namespace util::quantities::concepts::details

#if 0

//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
//--- util::quantities::concepts::Prefix
//------------------------------------------------------------------------------
template <typename R>
constexpr auto util::quantities::concepts::Prefix<R>::names
  (bool Long /* = false */)
{
  if constexpr(std::is_same<ratio, std::tera>())
    return Long? "tera"sv: "T"sv;
  if constexpr(std::is_same<ratio, std::giga>())
    return Long? "giga"sv: "G"sv;
  if constexpr(std::is_same<ratio, std::mega>())
    return Long? "mega"sv: "M"sv;
  if constexpr(std::is_same<ratio, std::kilo>())
    return Long? "kilo"sv: "k"sv;
  if constexpr(std::is_same<ratio, std::ratio<1>>())
    return ""sv;
  if constexpr(std::is_same<ratio, std::deci>())
    return Long? "deci"sv: "d"sv;
  if constexpr(std::is_same<ratio, std::centi>())
    return Long? "centi"sv: "c"sv;
  if constexpr(std::is_same<ratio, std::milli>())
    return Long? "milli"sv: "m"sv;
  if constexpr(std::is_same<ratio, std::micro>())
    return Long? "micro"sv: "u"sv;
  if constexpr(std::is_same<ratio, std::nano>())
    return Long? "nano"sv:  "n"sv;
  if constexpr(std::is_same<ratio, std::pico>())
    return Long? "pico"sv:  "p"sv;
  if constexpr(std::is_same<ratio, std::femto>())
    return Long? "femto"sv:  "f"sv;
  // TODO complete the long list of prefixes

  // backup; can't use `to_string()` because of `constexpr` requirement
  return Long? "???"sv:  "?"sv;
} // util::quantities::concepts::Prefix<R>::names()


#endif // 0
//------------------------------------------------------------------------------
//---  util::quantities::concepts::Interval
//------------------------------------------------------------------------------
#if 0

//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr auto util::quantities::concepts::Quantity<U, T>::operator-
  (Quantity<OU, OT> const other) const
  -> quantity_t
{
  static_assert(std::is_same<Quantity<OU, OT>, quantity_t>(),
    "Only quantities with the same units can be subtracted."
    );
  return quantity_t(value() - other.value());
} // util::quantities::concepts::Quantity<>::operator+


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr auto util::quantities::concepts::Quantity<U, T>::operator/
  (Quantity<OU, OT> const q) const
  -> value_t
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't divide quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, divide
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() / q.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return (*this) / quantity_t(q);
  }
} // util::quantities::concepts::Quantity<>::operator/(Quantity)


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
auto util::quantities::concepts::Quantity<U, T>::operator+=
  (Quantity<OU, OT> const other)
  -> quantity_t&
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't add quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, add
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    fValue += other.value();
    return *this;
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return (*this += quantity_t(other));
  }
} // util::quantities::concepts::Quantity<>::operator+=()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
auto util::quantities::concepts::Quantity<U, T>::operator-=
  (Quantity<OU, OT> const other)
  -> quantity_t&
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't subtract quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, add
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    fValue -= other.value();
    return *this;
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return (*this -= quantity_t(other));
  }
} // util::quantities::concepts::Quantity<>::operator-=()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator==
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() == other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this == quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator==()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator!=
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() != other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this != quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator!=()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator<=
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() <= other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this <= quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator<=()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator>=
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() >= other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this >= quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator>=()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator<
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() < other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this < quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator<()


//------------------------------------------------------------------------------
template <typename U, typename T>
template <typename OU, typename OT>
constexpr bool util::quantities::concepts::Quantity<U, T>::operator>
  (Quantity<OU, OT> const other) const
{
  static_assert(unit_t::template sameUnitAs<OU>(),
    "Can't compare quantities with different base unit"
    );
  using other_t = Quantity<OU, OT>;

  // if the two quantities have the same *scaled* unit, just compare the values
  if constexpr (std::is_same<typename other_t::unit_t, unit_t>()) {
    return value() > other.value();
  }
  else {
    // otherwise, they have same base unit but different scale: convert `other`
    return *this > quantity_t(other);
  }
} // util::quantities::concepts::Quantity<>::operator>()



//------------------------------------------------------------------------------
//---  Standard library extensions
//------------------------------------------------------------------------------
namespace std {
  
  // ---------------------------------------------------------------------------
  template <typename... Args>
  std::string to_string
    (util::quantities::concepts::ScaledUnit<Args...> const& unit)
    {
      return
        std::string(unit.prefix().symbol()) + unit.baseUnit().symbol.data();
    }

  template <typename... Args>
  std::string to_string(util::quantities::concepts::Quantity<Args...> const& q)
    { return std::to_string(q.value()) + ' ' + std::to_string(q.unit()); }


  // ---------------------------------------------------------------------------
  /// Hash function of a quantity is delegated to its value
  template <typename... Args>
  struct hash<util::quantities::concepts::Quantity<Args...>> {
      private:
    using quantity_t = util::quantities::concepts::Quantity<Args...>;
    using value_t = typename quantity_t::value_t;

      public:
    constexpr auto operator()(quantity_t key) const
      { return std::hash<value_t>()(key.value()); }
  };
  
  
  // ---------------------------------------------------------------------------
  /// Limits of a quantity are the same as the underlying type.
  template <typename Unit, typename T>
  class numeric_limits<util::quantities::concepts::Quantity<Unit, T>>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Quantity<Unit, T>>
  {};
  
  template <typename Unit, typename T>
  class numeric_limits
    <util::quantities::concepts::Quantity<Unit, T> const>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Quantity<Unit, T> const>
  {};
  
  template <typename Unit, typename T>
  class numeric_limits
    <util::quantities::concepts::Quantity<Unit, T> volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Quantity<Unit, T> volatile>
  {};
  
  template <typename Unit, typename T>
  class numeric_limits
    <util::quantities::concepts::Quantity<Unit, T> const volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Quantity<Unit, T> const volatile>
  {};
  
  
  // ---------------------------------------------------------------------------
  
} // namespace std


#endif // 0

//------------------------------------------------------------------------------

#endif // LARDATAALG_UTILITIES_INTERVALS_H
