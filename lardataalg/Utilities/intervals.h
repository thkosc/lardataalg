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
#include <type_traits> // std::enable_if_t<>, ...

/**
 * @def LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION
 * @brief Enable implicit conversions of `Point` and `Interval` into `value_t`.
 * @see LARDATAALG_UTILITIES_QUANTITIES_ENABLE_IMPLICIT_CONVERSION
 *
 * This is likely going to break a number of features, and enabling it should
 * be followed by intense scrutiny of the tests.
 */
#undef LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION

/**
 * @def LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
 * @brief Enable comparisons of `Point` and `Interval` with `value_t`.
 * @see LARDATAALG_UTILITIES_QUANTITIES_ENABLE_VALUE_COMPARISONS
 *
 * This is likely going to break some code, and it is dangerous in that a
 * statement like `t > 5` does not really make sure `5` is the right unit.
 * With this feature disabled (which is recommended), the same comparison would
 * be written `t > 5_ms`, thus ensuring the unit is known (this comes with the
 * hassle of `using namespace util::quantities::time_literals` or such).
 */
#undef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS


namespace util::quantities {

  /// Category type for intervals and point not belonging to any category.
  struct NoCategory;


  namespace concepts {

    namespace details {

      //------------------------------------------------------------------------

      template <typename Cat, typename = void>
      struct category_of_type;

      template <typename Cat>
      using category_of = typename category_of_type<Cat>::type;


      //------------------------------------------------------------------------
      /// An object belonging to a category `Cat`.
      template <typename Cat>
      struct WithCategory;


      //------------------------------------------------------------------------

    } // namespace details


    //--------------------------------------------------------------------------
    /// An non-mandatory base class for interval and point categories.
    struct CategoryBase {
      /// Returns the name of this category. Optional.
      static std::string name() = delete;
    }; // struct CategoryBase


    //--------------------------------------------------------------------------
    //--- traits
    //--------------------------------------------------------------------------
    /// Trait: `true_type` if `IV` is a `Interval` specialization.
    template <typename IV>
    struct is_interval;

    /// Trait: `true` if `IV` is a `Interval` specialization.
    template <typename IV>
    constexpr bool is_interval_v = is_interval<IV>();


    //--------------------------------------------------------------------------
    /// Trait: `true_type` if `PT` is a `Point` specialization.
    template <typename PT>
    struct is_point;

    /// Trait: `true` if `PT` is a `Point` specialization.
    template <typename PT>
    constexpr bool is_point_v = is_point<PT>();


    //--------------------------------------------------------------------------
    /// Trait: `true_type` if `PT` is a specialization of `Interval` or `Point`.
    template <typename T>
    using is_interval_or_point = std::disjunction<is_interval<T>, is_point<T>>;

    /// Trait: `true` if `PT` is a specialization of `Interval` or `Point`.
    template <typename T>
    constexpr bool is_interval_or_point_v = is_interval_or_point<T>();


    //--------------------------------------------------------------------------

    /// Type of interval contained in specified type `T`.
    template <typename T>
    using interval_of = typename T::interval_t;

    //--------------------------------------------------------------------------

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
     * In addition, an interval can be added to a quantity point (`Point`) to
     * translate it, and it can be obtained as difference between two quantity
     * points.
     */
    template <typename Q, typename Cat = NoCategory>
    struct Interval: private Q, public details::WithCategory<Cat> {

      using category_base_t = details::WithCategory<Cat>;

      /// Traits of the category.
      using traits_t = typename category_base_t::traits_t;

      template <typename OC, typename Type = void>
      using enable_if_compatible_t
       = std::enable_if_t
         <category_base_t::template category_compatible_with<OC>(), Type>
       ;

        public:

      using interval_t = Interval<Q, Cat>; ///< This type.


      // --- BEGIN -- Types from the base quantity -----------------------------
      /// @name Types from the base quantity
      /// @{

      using quantity_t = Q; /// Quantity the interval is based on.

      /// The category this point belongs to.
      using category_t = typename category_base_t::category_t;

      /// A quantity in the same unit, but possibly a different scale.
      template <typename R>
      using scaled_quantity_t = rescale<quantity_t, R>;

      /// Type of the stored value.
      using value_t = typename quantity_t::value_t;

      /// Description of the scaled unit.
      using unit_t = typename quantity_t::unit_t;

      /// Description of the unscaled unit.
      using baseunit_t = typename quantity_t::baseunit_t;

      /// An interval based on a different quantity but with the same category.
      template <typename OQ, typename OI>
      using other_interval_t = Interval<OQ, category_t>;

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
      template <typename... Args>
      constexpr Interval(Quantity<Args...> const& q)
        : quantity_t(quantity_t{ q })
        {}

      /**
       * @brief Constructor: converts from another interval.
       * @tparam I type of the other interval
       * @param iv interval to be converted from
       *
       * Intervals are required to be in the same unit (unit scale may differ).
       * The value in `iv` is converted from its native scale into the one of
       * this interval.
       */
      template
        <typename IV, typename std::enable_if_t<is_interval_v<IV>>* = nullptr>
      constexpr Interval(IV iv): Interval(quantity_t { iv.quantity() }) {}

      /// Returns the value of the interval as a quantity.
      constexpr quantity_t const& quantity() const { return *this; }

      /// Returns the value of the interval as plain value.
      using quantity_t::value;

      /// Conversion to the base quantity.
#ifndef LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION
      explicit
#endif // !LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION
      constexpr operator value_t() const
        { return value(); }


      // -- BEGIN Access to the scaled unit ------------------------------------
      /// @name Access to the scaled unit.
      /// @{

      using quantity_t::unit;
      using quantity_t::unitName;
      using quantity_t::unitSymbol;
      using quantity_t::baseUnit;

      /*
       * due to C++ (up to 17 at least) syntax limitations, we can't import
       * a dependent template (in this case, `Q::baseUnit<U>`, which depends
       * on a template parameter `Q`) as such: C++ will import it if we use the
       * "standard" syntax `using quantity_t::sameUnitAs;`, but without
       * knowning it as a template; the correct syntax to express this would be
       * `using quantity_t::template sameUnitAs;`, which is not allowed in C++.
       * So we have to redefine the thing entirely.
       */

      /// Returns whether objects of type `OU` have the same base unit as this.
      template <typename OU>
      static constexpr bool sameBaseUnitAs()
        { return quantity_t::template sameBaseUnitAs<OU>(); }

      /// Returns whether objects of type `OU` have same unit and scale as this.
      template <typename OU>
      static constexpr bool sameUnitAs()
        { return quantity_t::template sameUnitAs<OU>(); }


      /// @}
      // -- END Access to the scaled unit --------------------------------------


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
       * @note The addition and subtraction of intervals is conceptually
       *       symmetric. This leaves open the question whether the result of
       *       `a + b` should be of the type of `a` or the one of `b`, and
       *       whether having `b + a` of a different type than `a + b` is
       *       acceptable. Here we decide it's not, and therefore the operation
       *       `a + b` (and `a - b` as well) are supported only if `a` and `b`
       *       have the same type.
       *       On the other end, if either `a` or `b` are not an interval but
       *       rather a simple quantity, it is clear that the result must still
       *       be an interval and of the same type as the interval operand.
       *       We are in this case allowed to accept quantities of any unit
       *       scale.
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
      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, value_t>
      operator/ (Interval<OQ, OC> const denom) const
        { return quantity() / denom.quantity(); }

      /// Add a quantity (possibly converted) to this one.
      template <typename R>
      interval_t& operator+= (scaled_quantity_t<R> const other)
        { quantity_t::operator+= (other); return *this; }

      /// Add the `other` interval (possibly converted) to this one.
      template <typename OQ, typename OC>
      enable_if_compatible_t<Interval<OQ, OC>, interval_t&>
      operator+= (Interval<OQ, OC> const other)
        { return operator+= (other.quantity()); }

      /// Subtract a quantity (possibly converted) from this one.
      template <typename R>
      interval_t& operator-= (scaled_quantity_t<R> const other)
        { quantity_t::operator-= (other); return *this; }

      /// Subtract the `other` interval (possibly converted) from this one.
      template <typename OQ, typename OC>
      enable_if_compatible_t<Interval<OQ, OC>, interval_t&>
      operator-= (Interval<OQ, OC> const other)
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

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator==(Interval<OQ, OC> const other) const
        { return quantity_t::operator==(other.quantity()); }

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator!=(Interval<OQ, OC> const other) const
        { return quantity_t::operator!=(other.quantity()); }

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator>=(Interval<OQ, OC> const other) const
        { return quantity_t::operator>=(other.quantity()); }

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator>(Interval<OQ, OC> const other) const
        { return quantity_t::operator>(other.quantity()); }

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator<=(Interval<OQ, OC> const other) const
        { return quantity_t::operator<=(other.quantity()); }

      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, bool>
      operator<(Interval<OQ, OC> const other) const
        { return quantity_t::operator<(other.quantity()); }


      /// @}
      // -- END Asymmetric arithmetic operations -------------------------------

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

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator==
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() == b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator==
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b == a; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator!=
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() != b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator!=
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b != a; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator<=
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() <= b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator<=
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b >= a; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator<
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() < b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator<
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b > a; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator>=
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() >= b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator>=
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b <= a; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator>
      (Interval<Q, Cat> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() > b; }

    template <typename Q, typename Cat, typename... Args>
    constexpr bool operator>
      (Quantity<Args...> const a, Interval<Q, Cat> const b) noexcept
      { return b < a; }


    /*
     * To refrain the wild matching of every type with `V`, and still retain
     * the ability to compare to e.g. an integer rather than a double, instead
     * of forcing `V` and `T` to match, we ask `V` to be _implicitly_
     * convertible into `T`.
     * In particular, any requirement must exclude `Quantity` types from
     * matching to `V`, or the interplay with the `Quantity` vs. `Quantity`
     * comparisons will be messy at best. Currently `Quantity` converts to
     * its `value_t` only via explicit conversion. If this is going to change,
     * the `V` to `T` matching requirement must become stricter, e.g.
     * `std::is_arithmetic_v<V>`.
     */
    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator== (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() == value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator== (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv == value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator!= (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() != value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator!= (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv != value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator<= (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() <= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator<= (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv >= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator< (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() < value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator< (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv > value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator>= (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() >= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator>= (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv <= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator> (Interval<Q, Cat> const iv, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv.quantity() > value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t
      <std::is_convertible_v<T, typename Interval<Q, Cat>::value_t>, bool>
    operator> (T const value, Interval<Q, Cat> const iv) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return iv < value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif


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
    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q, Cat>>
    operator* (Interval<Q, Cat> const iv, T const factor)
      { return Interval<Q, Cat>{ iv.quantity() * factor }; }
    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q, Cat>>
    operator* (T const factor, Interval<Q, Cat> const iv) { return iv * factor; }
    //@}

    /// Multiplication between quantities is forbidden.
    template <typename AQ, typename AC, typename BQ, typename BC>
    constexpr auto operator* (Interval<AQ, AC> const, Interval<BQ, BC> const)
//       -> decltype(std::declval<AQ>() * std::declval<AQ>())
      = delete;

    // Division by a scalar.
    template <typename Q, typename Cat, typename T>
    constexpr std::enable_if_t<std::is_arithmetic_v<T>, Interval<Q, Cat>>
    operator/ (Interval<Q, Cat> const iv, T const quot)
      { return Interval<Q, Cat>{ iv.quantity() / quot }; }

    /// @}
    // -- END Arithmetic operations --------------------------------------------


    // -------------------------------------------------------------------------
    /// Type of an interval like `IV`, but with a different unit scale `R`.
    template <typename IV, typename R, typename T = typename IV::value_t>
    using rescale_interval = Interval
      <rescale<typename IV::quantity_t, R, T>, typename IV::category_t>;


    // -------------------------------------------------------------------------
    template <typename Q, typename Cat>
    std::string to_string(Interval<Q, Cat> const& iv)
      { return util::to_string(iv.quantity()); }


    // -------------------------------------------------------------------------

    /** ************************************************************************
     * @brief A quantity point.
     * @tparam Q quantity the interval is based on
     * @tparam Cat category this point belongs to (`NoCategory` by default)
     *
     * A point shares most of the concepts of a `Quantity`, but it interacts
     * only with other points rather than with bare `Quantity` objects,
     * with the exception of construction.
     * In this sense, the relation between `Point` and `Quantity` is similar
     * to the one between `Quantity` and its base type (`Quantity::value_t`).
     *
     * In addition, `Point` has some interaction with the corresponding
     * `Interval`: an interval can be seen as the distance, or difference,
     * between two quantity points.
     *
     * The point belongs to a category, which is just a tag that prevents
     * different points from being mixed up. Note that intervals do not have a
     * category.
     */
    template <
      typename Q, typename Cat = NoCategory,
      typename IV = Interval<Q, Cat>
      >
    struct Point: private Q, public details::WithCategory<Cat> {

      using category_base_t = details::WithCategory<Cat>;

      /// Traits of the category.
      using traits_t = typename category_base_t::traits_t;

      template <typename OC, typename Type = void>
      using enable_if_compatible_t
       = std::enable_if_t
         <category_base_t::template category_compatible_with<OC>(), Type>
       ;


        public:

      using point_t = Point<Q, Cat, IV>; ///< This type.


      // --- BEGIN -- Types from the base quantity -----------------------------
      /// @name Types from the base quantity
      /// @{

      using quantity_t = Q; ///< Quantity the interval is based on.

      /// The category this point belongs to.
      using category_t = typename category_base_t::category_t;

      /// The interval type corresponding to the unit of this point.
      using interval_t = IV;

      /// A quantity in the same unit, but possibly a different scale.
      template <typename R>
      using scaled_quantity_t = rescale<quantity_t, R>;

      /// Type of the stored value.
      using value_t = typename quantity_t::value_t;

      /// Description of the scaled unit.
      using unit_t = typename quantity_t::unit_t;

      /// Description of the unscaled unit.
      using baseunit_t = typename quantity_t::baseunit_t;

      /// A point based on a different quantity but with the same category.
      template <typename OQ, typename OI>
      using other_point_t = Point<OQ, category_t, OI>;

      /// @}
      // --- END -- Types from the base quantity -------------------------------

      /// Constructor: value is left uninitialized.
      // NOTE: this is not `constexpr` because using it in a constexpr would
      //       yield an uninitialized constant
      explicit Point() = default;

      /// Constructor: takes a value in the intended representation.
      explicit constexpr Point(value_t v): Point(quantity_t{ v }) {}

      /**
       * @brief Constructor: converts from a quantity.
       * @tparam OQ type of the quantity
       * @param q quantity to be converted from
       *
       * The quantity is required to be in the same unit as this point
       * (unit scale may differ).
       * The value in `q` is converted from its native scale into the one of
       * this point.
       */
      template <typename... Args>
      constexpr Point(Quantity<Args...> const q)
        : quantity_t(quantity_t{ q }) {}

      /**
       * @brief Constructor: converts from another point.
       * @tparam PT type of the other point
       * @param p point to be converted from
       *
       * Points are required to be in the same unit (unit scale may differ).
       * The value in `p` is converted from its native scale into the one of
       * this point.
       */
      template
        <typename PT, typename std::enable_if_t<is_point_v<PT>>* = nullptr>
      constexpr Point(PT const p): Point(quantity_t{ p.quantity() }) {}

      /// Returns the value of the interval as a quantity.
      constexpr quantity_t const& quantity() const { return *this; }

      /// Returns the value of the interval as plain value.
      using quantity_t::value;

      /// Conversion to the base quantity.
#ifndef LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION
      explicit
#endif // LARDATAALG_UTILITIES_INTERVALS_ENABLE_IMPLICIT_CONVERSION
      constexpr operator value_t() const
        { return value(); }



      // -- BEGIN Asymmetric arithmetic operations -----------------------------
      /**
       * @name Asymmetric operand arithmetic operations
       *
       * These arithmetic operations take care of preserving the point unit
       * through them.
       * Not all possible (or reasonable) operations are supported yet.
       * Some operations that may be symmetric (like addition of interval)
       * are implemented as free functions rather than methods.
       *
       * @note These operations are essentially provided by convenience. There
       *       are many cases (corner and not) where the implicit conversion
       *       to the base type kicks in. This implementation does not aim to
       *       _prevent_ that from happening, but requests are welcome to add
       *       features to _allow_ that not to happen, with some care of the
       *       user.
       *
       * @note Differently from `Interval`, the addition and subtraction of
       *       interval to _points_ is not symmetric, i.e. adding an interval
       *       to a point is supported (resulting in another point) but adding
       *       a point to an interval is not. Another difference is that since
       *       the result is unmistakably associated with the one operand of
       *       point type, the result has the same unit and type as that operand
       *       and the other operand, the interval, can be converted from a
       *       different unit scale.
       */
      /// @{

      /// Returns the sum of this point plus an interval (from quantity)
      /// (note that addition is not symmetric).
      template <typename R>
      constexpr point_t operator+ (scaled_quantity_t<R> const delta) const
        { return point_t(quantity().plus(delta)); }

      /// Returns the sum of this point plus an interval
      /// (note that addition is not symmetric).
      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, point_t>
      operator+ (Interval<OQ, OC> const delta) const
        { return operator+ (delta.quantity()); }

      /// Returns the value of this point after subtraction of an interval.
      template <typename R>
      constexpr point_t operator- (scaled_quantity_t<R> const delta) const
        { return point_t(quantity().minus(delta)); }

      /// Returns the value of this point after subtraction of an interval.
      template <typename OQ, typename OC>
      constexpr enable_if_compatible_t<Interval<OQ, OC>, point_t>
      operator- (Interval<OQ, OC> const delta) const
        { return operator- (delta.quantity()); }

      /// Add a quantity (possibly converted) to this one.
      template <typename R>
      point_t& operator+= (scaled_quantity_t<R> const other)
        { quantity_t::operator+= (other); return *this; }

      /// Add the `other` interval (possibly converted) to this point.
      template <typename OQ, typename OC>
      enable_if_compatible_t<Interval<OQ, OC>, point_t&> operator+=
        (Interval<OQ, OC> const other)
        { return operator+= (other.quantity()); }

      /// Subtract a quantity (possibly converted) from this one.
      template <typename R>
      point_t& operator-= (scaled_quantity_t<R> const other)
        { quantity_t::operator-= (other); return *this; }

      /// Subtract the `other` interval (possibly converted) from this point.
      template <typename OQ, typename OC>
      enable_if_compatible_t<Interval<OQ, OC>, point_t&> operator-=
        (Interval<OQ, OC> const other)
        { return operator-= (other.quantity()); }

      /// Returns a point with same value.
      constexpr point_t operator+() const { return point_t(quantity()); }

      /// Returns a parity-changed point.
      constexpr point_t operator-() const { return point_t(-quantity()); }

      /// @}
      // -- END Asymmetric arithmetic operations -------------------------------


      // -- BEGIN Comparisons --------------------------------------------------
      /**
       * @name Comparisons.
       *
       * Comparisons with plain numbers are managed by implicit conversion.
       * More care is needed for quantities.
       * Comparisons between two point instances `a` and `b` work this way:
       * * if `a` and `b` belong to different categories, they are _not_
       *     comparable
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

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool> operator==(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator==(other.quantity()); }

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool>
      operator!=(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator!=(other.quantity()); }

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool>
      operator>=(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator>=(other.quantity()); }

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool>
      operator>(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator>(other.quantity()); }

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool>
      operator<=(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator<=(other.quantity()); }

      template <typename OQ, typename OI>
      constexpr enable_if_compatible_t<other_point_t<OQ, OI>, bool>
      operator<(other_point_t<OQ, OI> const other) const
        { return quantity_t::operator<(other.quantity()); }


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
      template <typename PT>
      std::enable_if_t<is_point_v<PT>, PT> convertInto() { return PT(*this); }

      /**
       * @brief Returns a new point initialized with the specified value.
       * @tparam U type to initialize the quantity with
       * @param value the value to initialize the point with
       * @return a new `Point` object initialized with `value`
       *
       * The `value` is cast into `value_t` via `static_cast()`.
       */
      template <typename U>
      static point_t castFrom(U value)
        { return point_t{ static_cast<value_t>(value) }; }


    }; // struct Point

    template <typename... Args>
    std::ostream& operator<< (std::ostream& out, Point<Args...> const p)
      { return out << p.quantity(); }


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


    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator==
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() == b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator==
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b == a; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator!=
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() != b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator!=
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b != a; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator<=
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() <= b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator<=
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b >= a; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator<
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() < b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator<
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b > a; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator>=
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() >= b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator>=
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b <= a; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator>
      (Point<Q, Cat, IV> const a, Quantity<Args...> const b) noexcept
      { return a.quantity() > b; }

    template <typename Q, typename Cat, typename IV, typename... Args>
    constexpr bool operator>
      (Quantity<Args...> const a, Point<Q, Cat, IV> const b) noexcept
      { return b < a; }


    /*
     * To refrain the wild matching of every type with `V`, and still retain
     * the ability to compare to e.g. an integer rather than a double, instead
     * of forcing `V` and `T` to match, we ask `V` to be _implicitly_
     * convertible into `T`.
     * In particular, any requirement must exclude `Quantity` types from
     * matching to `V`, or the interplay with the `Quantity` vs. `Quantity`
     * comparisons will be messy at best. Currently `Quantity` converts to
     * its `value_t` only via explicit conversion. If this is going to change,
     * the `V` to `T` matching requirement must become stricter, e.g.
     * `std::is_arithmetic_v<V>`.
     */
    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator== (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() == value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator== (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p == value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator!= (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() != value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator!= (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p != value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator<= (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() <= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator<= (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p >= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator< (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() < value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator< (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p > value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator>= (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() >= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator>= (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p <= value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator> (Point<Q, Cat, IV> const p, T const value) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p.quantity() > value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif

    template <typename Q, typename Cat, typename IV, typename T>
    constexpr
    std::enable_if_t
      <std::is_convertible_v<T, typename Point<Q, Cat, IV>::value_t>, bool>
    operator> (T const value, Point<Q, Cat, IV> const p) noexcept
#ifdef LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      { return p < value; }
#else // LARDATAALG_UTILITIES_INTERVALS_ENABLE_VALUE_COMPARISONS
      = delete; // comparison with unqualified value not allowed
#endif


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

    template <typename Q, typename Cat, typename IV, typename OQ, typename OC>
    constexpr Point<Q, Cat, IV> operator+
      (Interval<OQ, OC> const delta, Point<Q, Cat, IV> const p)
      = delete; // use `point + interval`, not `interval + point`

    template <typename Q, typename Cat, typename IV, typename OQ, typename OC>
    constexpr Point<Q, Cat, IV> operator-
      (Interval<OQ, OC> const delta, Point<Q, Cat, IV> const p)
      = delete; // use `point + interval`, not `interval + point`

    // two different points but with the same interval type
    template
      <typename Q, typename Cat, typename IV, typename OQ, typename OCat>
    constexpr
    typename Point<Q, Cat, IV>
      ::template enable_if_compatible_t<Point<OQ, OCat, IV>, IV>
    operator- (Point<Q, Cat, IV> const a, Point<OQ, OCat, IV> const b)
      { return IV(a.quantity() - b.quantity()); }

    /// @}
    // -- END Arithmetic operations --------------------------------------------


    // -------------------------------------------------------------------------
    /// Type of a point like `PT`, but with a different unit scale `R`.
    template <typename PT, typename R, typename T = typename PT::value_t>
    using rescale_point
      = Point<rescale<typename PT::quantity_t, R, T>, typename PT::category_t>;


    // -------------------------------------------------------------------------
    template <typename Q, typename Cat, typename IV>
    std::string to_string(Point<Q, Cat, IV> const& p)
      { return util::to_string(p.quantity()); }

    // -------------------------------------------------------------------------

  } // namespace concepts


  // ---------------------------------------------------------------------------
  struct NoCategory: concepts::CategoryBase {

    static std::string name() { return "generic"; }

  }; // struct NoCategory


  // ---------------------------------------------------------------------------
  // @{
  /**
   * @brief Returns an interval of the specified type parsed from a string.
   * @tparam IV the type of quantity interval to be returned
   * @param s the string to be parsed
   * @param unitOptional (default: `false`) whether unit is not required in `s`
   * @return a quantity interval of the specified type parsed from a string
   * @throw MissingUnit `s` does not contain the required unit
   * @throw ValueError the numerical value in `s` is not parseable
   * @throw ExtraCharactersError spurious characters after the numeric value
   * @see `util::makeQuantity()`
   *
   * This function behaves like `util::makeQuantity()`.
   */
  template <typename IV>
  IV makeInterval(std::string_view s, bool unitOptional = false);

  template <typename IV>
  IV makeInterval(std::string const& s, bool unitOptional = false);

  template <typename IV>
  IV makeInterval(char const* s, bool unitOptional = false);

  //@}


  // @{
  /**
   * @brief Returns a point of the specified type parsed from a string.
   * @tparam PT the type of quantity point to be returned
   * @param s the string to be parsed
   * @param unitOptional (default: `false`) whether unit is not required in `s`
   * @return a quantity point of the specified type parsed from a string
   * @throw MissingUnit `s` does not contain the required unit
   * @throw ValueError the numerical value in `s` is not parseable
   * @throw ExtraCharactersError spurious characters after the numeric value
   * @see `util::makeQuantity()`
   *
   * This function behaves like `util::makeQuantity()`.
   */
  template <typename PT>
  PT makePoint(std::string_view s, bool unitOptional = false);

  template <typename PT>
  PT makePoint(std::string const& s, bool unitOptional = false);

  template <typename PT>
  PT makePoint(char const* s, bool unitOptional = false);

  //@}


  // ---------------------------------------------------------------------------

} // namespace util::quantities


//------------------------------------------------------------------------------
//---  template implementation
//------------------------------------------------------------------------------
namespace util::quantities::concepts::details {

  //----------------------------------------------------------------------------
  template <typename, typename = std::void_t<>>
  struct has_category: std::false_type {};

  template <typename Obj>
  struct has_category<Obj, std::void_t<typename Obj::category_t>>
    : std::true_type
  {};

  template <typename Obj>
  constexpr bool has_category_v = has_category<Obj>();

  //----------------------------------------------------------------------------
  template <typename, typename = std::void_t<>>
  struct category_has_name: std::false_type {};

  template <typename Cat>
  struct category_has_name<Cat, std::void_t<decltype(Cat::name())>>
    : std::true_type {};

  template <typename Cat>
  struct category_traits {

    using category_t = Cat; ///< The categories the traits are about.

    /// Whether the category supports `name()` call.
    static constexpr bool has_name = category_has_name<category_t>();

    /// Returns whether the category `OC` is "compatible" with this one.
    template <typename OC>
    static constexpr bool compatible_with()
      { return util::is_any_of_v<OC, NoCategory, category_t>; }

  }; // struct category_traits<>


  //----------------------------------------------------------------------------
  template <typename Cat, typename /* = void */>
  struct category_of_type { using type = Cat; };

  template <typename Cat>
  struct category_of_type<Cat, std::void_t<typename Cat::category_t>>
    { using type = typename Cat::category_t; };


  //----------------------------------------------------------------------------
  template <typename Cat>
  struct WithCategory {

    using category_t = Cat; ///< The category of this object.

    /// Traits of this category.
    using traits_t = category_traits<category_t>;

    /// Returns an instance of the category of this object.
    static constexpr category_t category();

    // @{
    /// Returns whether the type `OC` belongs to `category_t`.
    template <typename OC>
    static constexpr bool same_category_as();

    template <typename OC>
    static constexpr bool same_category_as(OC const&);
    // @}

    // @{
    /// Returns whether `OC` has a category compatible with this one.
    template <typename OC>
    static constexpr bool category_compatible_with();

    template <typename OC>
    static constexpr bool category_compatible_with(OC const&);
    // @}


    /// Returns whether this category has a name.
    static constexpr bool hasCategoryName();

    /// Returns the name of the category of this object.
    static std::string categoryName();

  }; // struct WithCategory<>


  //----------------------------------------------------------------------------
  //--- WithCategory
  //----------------------------------------------------------------------------
  template <typename Cat>
  constexpr auto WithCategory<Cat>::category() -> category_t { return {}; }


  //----------------------------------------------------------------------------
  template <typename Cat>
  template <typename OC>
  constexpr bool WithCategory<Cat>::same_category_as() {
    return details::has_category_v<OC>
      && std::is_same_v<typename OC::category_t, category_t>;
  } // WithCategory<>::same_category_as()

  template <typename Cat>
  template <typename OC>
  constexpr bool WithCategory<Cat>::same_category_as(OC const&)
    { return same_category_as<OC>(); }


  //----------------------------------------------------------------------------
  template <typename Cat>
  template <typename OC>
  constexpr bool WithCategory<Cat>::category_compatible_with()
    { return traits_t::template compatible_with<category_of<OC>>(); }


  template <typename Cat>
  template <typename OC>
  constexpr bool WithCategory<Cat>::category_compatible_with(OC const&)
    { return category_compatible_with<OC>(); }

  //----------------------------------------------------------------------------
  template <typename Cat>
  constexpr bool WithCategory<Cat>::hasCategoryName()
    { return details::category_traits<Cat>::has_name; }


  //----------------------------------------------------------------------------
  template <typename Cat>
  std::string WithCategory<Cat>::categoryName()
    { return Cat::name(); }


  //----------------------------------------------------------------------------

} // namespace util::quantities::concepts::details


//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
namespace util::quantities::concepts {

  //----------------------------------------------------------------------------
  template <typename>
  struct is_interval: public std::false_type {};

  template <typename... Args>
  struct is_interval<Interval<Args...>>: public std::true_type {};


  //----------------------------------------------------------------------------
  template <typename>
  struct is_point: public std::false_type {};

  template <typename... Args>
  struct is_point<Point<Args...>>: public std::true_type {};


  //----------------------------------------------------------------------------

} // namespace util::quantities::concepts


//------------------------------------------------------------------------------
//---  Template implementation
//------------------------------------------------------------------------------
template <typename IV>
IV util::quantities::makeInterval
  (std::string_view s, bool unitOptional /* = false */)
{
  using quantity_t = typename IV::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makeInterval(string_view)


//------------------------------------------------------------------------------
template <typename IV>
IV util::quantities::makeInterval
  (std::string const& s, bool unitOptional /* = false */)
{
  using quantity_t = typename IV::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makeInterval(string)


//------------------------------------------------------------------------------
template <typename IV>
IV util::quantities::makeInterval
  (char const* s, bool unitOptional /* = false */)
{
  using quantity_t = typename IV::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makeInterval(C-string)


//------------------------------------------------------------------------------
template <typename PT>
PT util::quantities::makePoint
  (std::string_view s, bool unitOptional /* = false */)
{
  using quantity_t = typename PT::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makePoint(string_view)


//------------------------------------------------------------------------------
template <typename PT>
PT util::quantities::makePoint
  (std::string const& s, bool unitOptional /* = false */)
{
  using quantity_t = typename PT::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makePoint(string)


//------------------------------------------------------------------------------
template <typename PT>
PT util::quantities::makePoint
  (char const* s, bool unitOptional /* = false */)
{
  using quantity_t = typename PT::quantity_t;
  return { util::quantities::makeQuantity<quantity_t>(s, unitOptional) };
} // util::quantities::makePoint(C-string)


//------------------------------------------------------------------------------
//---  Standard library extensions
//------------------------------------------------------------------------------
namespace std {

  // ---------------------------------------------------------------------------
  /// Hash function of a interval or point is delegated to its quantity.
  template <typename Q, typename Cat>
  struct hash<util::quantities::concepts::Interval<Q, Cat>> {
    constexpr auto operator()
      (util::quantities::concepts::Interval<Q, Cat> key) const
      noexcept(noexcept(std::hash(key.quantity())))
      { return std::hash(key.quantity()); }
  }; // hash<Interval>

  template <typename Q, typename Cat, typename IV>
  struct hash<util::quantities::concepts::Point<Q, Cat, IV>> {
    constexpr auto operator()
      (util::quantities::concepts::Point<Q, Cat, IV> key) const
      noexcept(noexcept(std::hash(key.quantity())))
      { return std::hash(key.quantity()); }
  }; // hash<Interval>


  // ---------------------------------------------------------------------------
  /// Limits of a interval or point are the same as the underlying quantity and
  /// base type.
  template <typename Q, typename Cat>
  class numeric_limits<util::quantities::concepts::Interval<Q, Cat>>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Interval<Q, Cat>>
  {};

  template <typename Q, typename Cat>
  class numeric_limits<util::quantities::concepts::Interval<Q, Cat> const>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Interval<Q, Cat> const>
  {};

  template <typename Q, typename Cat>
  class numeric_limits<util::quantities::concepts::Interval<Q, Cat> volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Interval<Q, Cat> volatile>
  {};

  template <typename Q, typename Cat>
  class numeric_limits
    <util::quantities::concepts::Interval<Q, Cat> const volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Interval<Q, Cat> const volatile>
  {};


  template <typename Q, typename Cat, typename IV>
  class numeric_limits<util::quantities::concepts::Point<Q, Cat, IV>>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Point<Q, Cat, IV>>
  {};

  template <typename Q, typename Cat, typename IV>
  class numeric_limits<util::quantities::concepts::Point<Q, Cat, IV> const>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Point<Q, Cat, IV> const>
  {};

  template <typename Q, typename Cat, typename IV>
  class numeric_limits<util::quantities::concepts::Point<Q, Cat, IV> volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Point<Q, Cat, IV> volatile>
  {};

  template <typename Q, typename Cat, typename IV>
  class numeric_limits
    <util::quantities::concepts::Point<Q, Cat, IV> const volatile>
    : public util::quantities::concepts::details::numeric_limits
      <util::quantities::concepts::Point<Q, Cat, IV> const volatile>
  {};


  // ---------------------------------------------------------------------------

} // namespace std


//------------------------------------------------------------------------------

#endif // LARDATAALG_UTILITIES_INTERVALS_H
