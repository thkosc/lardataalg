include(CetTest)
cet_enable_asserts()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_subdirectory(Utilities)
add_subdirectory(DetectorInfo)

