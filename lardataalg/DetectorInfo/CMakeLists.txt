cet_make(LIBRARIES larcorealg_Geometry
                   larcorealg_CoreUtils
                   canvas::canvas
                   messagefacility::MF_MessageLogger
                   cetlib_except::cetlib_except
                   fhiclcpp::fhiclcpp
                   ROOT::Core
                   ROOT::Hist) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.05.03) - deprecated: use cet_make_library(), build_dictonary(), cet_plugin() with explicit source lists and plugin base types

install_headers()
install_fhicl()
install_source()
