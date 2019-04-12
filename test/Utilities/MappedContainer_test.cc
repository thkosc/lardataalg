/**
 * @file    MappedContainer_test.cc
 * @brief   Tests the classes in `MappedContainer.h`.
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    March 22, 2019
 * @version 1.0
 * @see     `lardataalg/Utilities/MappedContainer.h`
 */

// Boost libraries
#define BOOST_TEST_MODULE ( MappedContainer_test )
#include <cetlib/quiet_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// LArSoft libraries
#include "lardataalg/Utilities/MappedContainer.h"
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"
#include "larcorealg/CoreUtils/ContainerMeta.h" // util::collection_value_t<>

// C/C++ standard libraries
#include <iostream> // std::cout
#include <array>
#include <functional> // std::ref()
#include <algorithm> // std::copy()
#include <iterator> // std::back_inserter()
#include <type_traits> // std::is_copy_assignable_v<>...
#include <cstddef> // std::size_t
#include <cmath> // std::nan()


//------------------------------------------------------------------------------
//--- Test code
//

struct TestDataMakerBase {
  static constexpr std::size_t Dim = 10U;
  static std::array<int, Dim> const sourceValues;
  static constexpr std::size_t sourceSize() { return Dim; }
};

// BUG the double brace syntax is required to work around clang bug 21629
// (https://bugs.llvm.org/show_bug.cgi?id=21629)
std::array<int, TestDataMakerBase::Dim> const TestDataMakerBase::sourceValues
  {{ 0, -1, -2, -3, -4, -5, -6, -7, -8, -9 }};

template <typename Cont>
struct TestDataMakerClass: public TestDataMakerBase {
  using Container_t = Cont;
  static Container_t make()
    {
      Container_t data;
      std::copy
        (sourceValues.cbegin(), sourceValues.cend(), std::back_inserter(data));
      return data;
    }
}; // TestDataMakerClass

template <typename T, std::size_t N>
struct TestDataMakerClass<std::array<T, N>>: public TestDataMakerBase {
  using Container_t = std::array<T, N>;
  static Container_t make()
    {
      static_assert(N >= sourceSize());
      Container_t data;
      std::copy(sourceValues.cbegin(), sourceValues.cend(), data.begin());
      return data;
    }
}; // TestDataMakerClass<std::array>

template <typename T>
struct TestDataMakerClass<std::unique_ptr<T[]>>: public TestDataMakerBase {
  using Container_t = std::unique_ptr<T[]>;
  static Container_t make()
    {
      Container_t data { new T[sourceSize()] };
      std::copy(sourceValues.cbegin(), sourceValues.cend(), data.get());
      return data;
    }
}; // TestDataMakerClass<std::array>

template <typename Cont>
auto makeTestData() { return TestDataMakerClass<std::decay_t<Cont>>::make(); }


//------------------------------------------------------------------------------
template <typename Cont>
void copyTest() {
  
  using Container_t = Cont;
  using Literal_t = util::collection_value_t<Container_t>;
  using Data_t = Literal_t;
  
  constexpr Literal_t defaultValue { 42 };
  
  constexpr auto InvalidIndex = util::MappedContainerBase::invalidIndex();
  
  auto const data = makeTestData<Cont>();
  
  static_assert(std::is_copy_assignable_v<std::decay_t<decltype(data)>>);
  static_assert(std::is_copy_constructible_v<std::decay_t<decltype(data)>>);
  static_assert(std::is_move_assignable_v<std::decay_t<decltype(data)>>);
  static_assert(std::is_move_constructible_v<std::decay_t<decltype(data)>>);
  
  // BUG the double brace syntax is required to work around clang bug 21629
  // (https://bugs.llvm.org/show_bug.cgi?id=21629)
  std::array<std::size_t, 12U> const mapping1 = {{
    4U, 3U, 2U, 1U, 0U, InvalidIndex,
    9U, 8U, 7U, 6U, 5U, InvalidIndex,
  }};
  
  std::array<Data_t, 12U> const expectedMappedData1 = {{
    -4, -3, -2, -1,  0, defaultValue,
    -9, -8, -7, -6, -5, defaultValue
  }};
  
  util::MappedContainer const mappedData1
    (data, mapping1, expectedMappedData1.size(), defaultValue);
  
  BOOST_CHECK_EQUAL(mappedData1.size(), expectedMappedData1.size());
  BOOST_CHECK_EQUAL(mappedData1.minimal_size(), expectedMappedData1.size());
  
  BOOST_CHECK_EQUAL(mappedData1.front(), expectedMappedData1.front());
  BOOST_CHECK_EQUAL(mappedData1.back(), expectedMappedData1.back());
  
  std::size_t i = 0;
  auto const beginIterator = mappedData1.begin();
  auto const secondIterator = beginIterator + 1;
  auto mappedIterator = beginIterator;
  for (auto&& mappedValue: mappedData1) {
    BOOST_TEST_CHECKPOINT("mapped item: " << i);
    
    Data_t const expectedValue = expectedMappedData1[i];
    Data_t const& expectedRef = (mapping1[i] == InvalidIndex)
      ? mappedData1.defaultValue()
      : data[mapping1[i]]
      ;
    BOOST_CHECK_EQUAL(expectedRef, expectedValue); // test the test
    
    BOOST_CHECK_EQUAL(mappedValue, expectedMappedData1[i]);
    BOOST_CHECK_EQUAL(mappedData1[i], expectedMappedData1[i]);
    BOOST_CHECK_EQUAL(mappedData1.at(i), expectedMappedData1[i]);
    BOOST_CHECK_EQUAL(beginIterator[i], expectedMappedData1[i]);
    BOOST_CHECK_EQUAL(*(beginIterator + i), expectedMappedData1[i]);
    BOOST_CHECK_EQUAL(*mappedIterator, expectedMappedData1[i]);
    if (i >= 1) {
      BOOST_CHECK_EQUAL(secondIterator[i-1], expectedMappedData1[i]);
      BOOST_CHECK_EQUAL(*(secondIterator + i - 1), expectedMappedData1[i]);
      BOOST_CHECK( (mappedIterator != beginIterator));
      BOOST_CHECK(!(mappedIterator == beginIterator));
      BOOST_CHECK( (mappedIterator >  beginIterator));
      BOOST_CHECK( (mappedIterator >= beginIterator));
      BOOST_CHECK(!(mappedIterator <  beginIterator));
      BOOST_CHECK(!(mappedIterator <= beginIterator));
      BOOST_CHECK( (beginIterator != mappedIterator));
      BOOST_CHECK(!(beginIterator == mappedIterator));
      BOOST_CHECK(!(beginIterator >  mappedIterator));
      BOOST_CHECK(!(beginIterator >= mappedIterator));
      BOOST_CHECK( (beginIterator <  mappedIterator));
      BOOST_CHECK( (beginIterator <= mappedIterator));
    }
    else {
      BOOST_CHECK(!(mappedIterator != beginIterator));
      BOOST_CHECK( (mappedIterator == beginIterator));
      BOOST_CHECK(!(mappedIterator >  beginIterator));
      BOOST_CHECK( (mappedIterator >= beginIterator));
      BOOST_CHECK(!(mappedIterator <  beginIterator));
      BOOST_CHECK( (mappedIterator <= beginIterator));
      BOOST_CHECK(!(beginIterator != mappedIterator));
      BOOST_CHECK( (beginIterator == mappedIterator));
      BOOST_CHECK(!(beginIterator >  mappedIterator));
      BOOST_CHECK( (beginIterator >= mappedIterator));
      BOOST_CHECK(!(beginIterator <  mappedIterator));
      BOOST_CHECK( (beginIterator <= mappedIterator));
    }
    
    BOOST_CHECK_NE(&(mappedData1[i]), &(expectedMappedData1[i]));
    ++i;
    ++mappedIterator;
  } // for
  BOOST_CHECK_EQUAL(i, expectedMappedData1.size());
  
  // let's check a bit of overflow
  while (i < expectedMappedData1.size() + 3U) {
    BOOST_TEST_CHECKPOINT("mapped item: " << i << " (overflow)");
    BOOST_CHECK_THROW(mappedData1.at(i), std::out_of_range);
    ++i;
  } // for
  
  
} // copyTest()


//------------------------------------------------------------------------------
template <typename Cont>
void referenceTest() {
  
  using Container_t = Cont;
  using Literal_t = util::collection_value_t<Container_t>;
  using Data_t = Literal_t;
  
  constexpr Literal_t defaultValue { 42 };
  
  constexpr auto InvalidIndex = util::MappedContainerBase::invalidIndex();
  
  auto const data = makeTestData<Cont>();
  
  // BUG the double brace syntax is required to work around clang bug 21629
  // (https://bugs.llvm.org/show_bug.cgi?id=21629)
  std::array<std::size_t, 12U> const mapping1 = {{
    4U, 3U, 2U, 1U, 0U, InvalidIndex,
    9U, 8U, 7U, 6U, 5U, InvalidIndex,
  }};
  
  std::array<Literal_t, 12U> const expectedMappedData1 = {{
    -4, -3, -2, -1,  0, defaultValue,
    -9, -8, -7, -6, -5, defaultValue
  }};
  
  util::MappedContainer const mappedData1(
    std::ref(data), mapping1, expectedMappedData1.size(),
    defaultValue
    );
  
  // a similar mapping, but the mapping itself is not copied
  util::MappedContainer const mappedData2(std::ref(data), std::ref(mapping1));
  
  static_assert(
    std::is_same_v<
      typename decltype(mappedData1)::const_iterator::reference,
      Data_t const&
      >
    );
  
  BOOST_CHECK_EQUAL(mappedData1.size(), expectedMappedData1.size());
  BOOST_CHECK_EQUAL(mappedData1.minimal_size(), expectedMappedData1.size());
  
  BOOST_CHECK_EQUAL(mappedData1.front(), expectedMappedData1.front());
  BOOST_CHECK_EQUAL(mappedData1.back(), expectedMappedData1.back());
  
  std::size_t i = 0;
  auto const beginIterator = mappedData1.begin();
  auto const secondIterator = beginIterator + 1;
  auto mappedIterator = beginIterator;
  for (auto&& mappedValue: mappedData1) {
    BOOST_TEST_CHECKPOINT("mapped item: " << i);
    
    std::size_t const& expectedMappedIndex = mapping1[i];
    Data_t const expectedValue = expectedMappedData1[i];
    Data_t const& expectedRef = (expectedMappedIndex == InvalidIndex)
      ? mappedData1.defaultValue()
      : data[expectedMappedIndex]
      ;
    BOOST_CHECK_EQUAL(expectedRef, expectedValue); // test the test
    
    decltype(auto) mappedDataRef = mappedData1[i];
    static_assert(std::is_reference_v<decltype(mappedDataRef)>);
    
    BOOST_CHECK_EQUAL(mappedData1.map_index(i), expectedMappedIndex);
    BOOST_CHECK_EQUAL(mappedValue, expectedValue);
    BOOST_CHECK_EQUAL(mappedDataRef, expectedValue);
    BOOST_CHECK_EQUAL(mappedData1.at(i), expectedValue);
    BOOST_CHECK_EQUAL(beginIterator[i], expectedValue);
    BOOST_CHECK_EQUAL(*(beginIterator + i), expectedValue);
    BOOST_CHECK_EQUAL(*mappedIterator, expectedValue);
    if (i >= 1) {
      BOOST_CHECK_EQUAL(secondIterator[i-1], expectedValue);
      BOOST_CHECK_EQUAL(*(secondIterator + i - 1), expectedValue);
      BOOST_CHECK( (mappedIterator != beginIterator));
      BOOST_CHECK(!(mappedIterator == beginIterator));
      BOOST_CHECK( (mappedIterator >  beginIterator));
      BOOST_CHECK( (mappedIterator >= beginIterator));
      BOOST_CHECK(!(mappedIterator <  beginIterator));
      BOOST_CHECK(!(mappedIterator <= beginIterator));
      BOOST_CHECK( (beginIterator != mappedIterator));
      BOOST_CHECK(!(beginIterator == mappedIterator));
      BOOST_CHECK(!(beginIterator >  mappedIterator));
      BOOST_CHECK(!(beginIterator >= mappedIterator));
      BOOST_CHECK( (beginIterator <  mappedIterator));
      BOOST_CHECK( (beginIterator <= mappedIterator));
    }
    else {
      BOOST_CHECK(!(mappedIterator != beginIterator));
      BOOST_CHECK( (mappedIterator == beginIterator));
      BOOST_CHECK(!(mappedIterator >  beginIterator));
      BOOST_CHECK( (mappedIterator >= beginIterator));
      BOOST_CHECK(!(mappedIterator <  beginIterator));
      BOOST_CHECK( (mappedIterator <= beginIterator));
      BOOST_CHECK(!(beginIterator != mappedIterator));
      BOOST_CHECK( (beginIterator == mappedIterator));
      BOOST_CHECK(!(beginIterator >  mappedIterator));
      BOOST_CHECK( (beginIterator >= mappedIterator));
      BOOST_CHECK(!(beginIterator <  mappedIterator));
      BOOST_CHECK( (beginIterator <= mappedIterator));
    }
    
    // since `data` is captured via reference, the returned elements should be
    // exactly the same object (same physical memory location) as in the
    // original collection:
    BOOST_CHECK_EQUAL(&mappedDataRef, &expectedRef);
    
    // mapping by reference should be equivalent to mapping by value;
    // because of out test choice, the default values are different though
    if (mapping1[i] != InvalidIndex)
      BOOST_CHECK_EQUAL(mappedData2[i], mappedData1[i]);
    BOOST_CHECK_EQUAL(&mappedData2.map_index(i), &expectedMappedIndex);
    
    ++i;
    ++mappedIterator;
  } // for
  BOOST_CHECK_EQUAL(i, expectedMappedData1.size());
  
  // let's check a bit of overflow
  while (i < expectedMappedData1.size() + 3U) {
    BOOST_TEST_CHECKPOINT("mapped item: " << i << " (overflow)");
    BOOST_CHECK_THROW(mappedData1.at(i), std::out_of_range);
    ++i;
  } // for
  
} // referenceTest()


//------------------------------------------------------------------------------
void defaultConstructorTest() {
  
  using Mapping_t = std::array<std::size_t, 6U>;
  
  (void) util::MappedContainer<std::array<float, 4U>                        , Mapping_t>();
  (void) util::MappedContainer<float[4U]                                    , Mapping_t>();
  (void) util::MappedContainer<float*                                       , Mapping_t>();
  (void) util::MappedContainer<float const*                                 , Mapping_t>();
  (void) util::MappedContainer<std::unique_ptr<float>                       , Mapping_t>();
  (void) util::MappedContainer<std::reference_wrapper<std::array<float, 4U>>, Mapping_t>();
  
} // defaultConstructorTest()


//------------------------------------------------------------------------------
void autosizeTest() {
  /*
   * Tests that the size is correctly evinced from the mapping.
   */
  constexpr auto InvalidIndex = util::MappedContainerBase::invalidIndex();
  
  // BUG the double brace syntax is required to work around clang bug 21629
  // (https://bugs.llvm.org/show_bug.cgi?id=21629)
  std::array<double, 4U> const data {{ 0.0, -1.0, -2.0, -3.0 }};
    
  std::array<std::size_t, 6U> const mapping = {{
    1U, 0U, InvalidIndex,
    3U, 2U, InvalidIndex,
  }};
  
  util::MappedContainer const mappedData(std::ref(data), mapping);
  
  BOOST_CHECK_EQUAL(mappedData.size(), mapping.size());
  BOOST_CHECK_EQUAL(mappedData.defaultValue(), double{});
  
} // autosizeTest()


//------------------------------------------------------------------------------
void classDoc1Test() {
  /*
   * constexpr auto InvalidIndex = util::MappedContainerBase::invalidIndex();
   * 
   * std::array<double, 4U> const data { 0.0, -1.0, -2.0, -3.0 };
   *   
   * std::array<std::size_t, 6U> const mapping = {
   *   1U, 0U, InvalidIndex,
   *   3U, 2U, InvalidIndex,
   * };
   *   
   * util::MappedContainer const mappedData
   *   (std::ref(data), mapping, 6U, std::numeric_limits<double>::quiet_NaN());
   * 
   * for (std::size_t i = 0; i < 6U; ++i) {
   *   std::cout
   *     << "Mapped element #" << i << ": " << mappedData[i] << std::endl;
   * }
   */
  // BEGIN example -------------------------------------------------------------
  constexpr auto InvalidIndex = util::MappedContainerBase::invalidIndex();
  
  // BUG the double brace syntax is required to work around clang bug 21629
  // (https://bugs.llvm.org/show_bug.cgi?id=21629)
  std::array<double, 4U> const data {{ 0.0, -1.0, -2.0, -3.0 }};
    
  std::array<std::size_t, 6U> const mapping = {{
    1U, 0U, InvalidIndex,
    3U, 2U, InvalidIndex,
  }};
    
  util::MappedContainer const mappedData
    (std::ref(data), mapping, 6U, std::numeric_limits<double>::quiet_NaN());
  
  for (std::size_t i = 0; i < 6U; ++i) {
    std::cout
      << "Mapped element #" << i << ": " << mappedData[i] << std::endl;
  }
  // END example ---------------------------------------------------------------
  
} // classDoc1Test()


//------------------------------------------------------------------------------
//--- registration of tests
//

//
// stat collector tests
//
BOOST_AUTO_TEST_CASE(TestCase) {
  copyTest<std::array<double, 10U>>();
  referenceTest<std::array<double, 10U>>();
  referenceTest<std::unique_ptr<double[]>>();
  defaultConstructorTest();
  autosizeTest();
} // TestCase

BOOST_AUTO_TEST_CASE(DocumentationTestCase) {
  classDoc1Test();
} // DocumentationTestCase

