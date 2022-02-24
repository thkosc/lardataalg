cet_make_library(
  SOURCE MCDumperUtils.cxx
  LIBRARIES
    nusimdata::SimulationBase
    ROOT::EG
    ROOT::Core
  )

install_headers()
install_source()
