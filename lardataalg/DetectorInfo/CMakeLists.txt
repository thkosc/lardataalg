cet_make_library(
         SOURCE DetectorClocksStandard.cxx
                DetectorPropertiesData.cc
                DetectorPropertiesStandard.cxx
                ElecClock.cxx
                LArPropertiesStandard.cxx
                RunHistoryStandard.cxx
         LIBRARIES
                   canvas::canvas
                   messagefacility::MF_MessageLogger
                   cetlib_except::cetlib_except
                   fhiclcpp::fhiclcpp
                   ROOT::Core
                   ROOT::Hist
         PUBLIC    larcorealg::Geometry
                   larcorealg::CoreUtils
                   lardataobj::RawData
)

install_headers()
install_fhicl()
install_source()
