cet_make(LIBRARIES larcorealg_Geometry
                   larcorealg_CoreUtils
                   ${MF_MESSAGELOGGER}
                   cetlib_except
                   ${FHICLCPP}
                   ${ROOT_CORE}
                   ${ROOT_HIST})

install_headers()
install_fhicl()
install_source()
