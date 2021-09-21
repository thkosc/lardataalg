cet_make(LIBRARIES larcorealg_Geometry
                   larcorealg_CoreUtils
                   messagefacility::MF_MessageLogger
                   cetlib_except::cetlib_except
                   fhiclcpp::fhiclcpp
                   ROOT::Core
                   ROOT::Hist)

install_headers()
install_fhicl()
install_source()
