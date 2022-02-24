cet_make_library(
         SOURCE DetectorClocksStandard.cxx
                DetectorPropertiesData.cc
                DetectorPropertiesStandard.cxx
                ElecClock.cxx
                LArPropertiesStandard.cxx
                RunHistoryStandard.cxx
         LIBRARIES larcorealg::Geometry
                   larcorealg::CoreUtils
                   canvas::canvas
                   messagefacility::MF_MessageLogger
                   cetlib_except::cetlib_except
                   fhiclcpp::fhiclcpp
                   ROOT::Core
                   ROOT::Hist)

install_headers()
install_fhicl()
install_source()
