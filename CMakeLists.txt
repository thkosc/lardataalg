# ======================================================================
#  larcorealg main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcorealg/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
# cmake_policy(VERSION 3.19) ### MIGRATE-ACTION-COMPATIBILITY (migrate-3.05.03) - Uncomment to preseve compatibility with older CMake versions

find_package(cetmodules REQUIRED)
project(lardataalg VERSION 09.09.02 LANGUAGES CXX)

# cetbuildtools contains our cmake modules

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags() ### MIGRATE-ACTION-RECOMMENDED (migrate-3.05.03) - add args: REPORT_THRESHOLD VERBOSE

find_package(cetlib_except REQUIRED EXPORT)
find_package(cetlib REQUIRED EXPORT)
find_package(canvas REQUIRED EXPORT)
find_package(canvas_root_io REQUIRED)
find_package(nusimdata REQUIRED EXPORT)
find_package(ROOT COMPONENTS Core EG Hist REQUIRED EXPORT)
find_package(Boost REQUIRED)
# these are minimum required versions, not the actual product versions
find_package( larcoreobj REQUIRED )
find_package( larcorealg REQUIRED )
find_package( lardataobj REQUIRED )

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(CetMake)
include(BasicPlugin)

# source
add_subdirectory(lardataalg)

# tests
add_subdirectory(test)

# ups - table and config files

# packaging utility
cet_cmake_config()
