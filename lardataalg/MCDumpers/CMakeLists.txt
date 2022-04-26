cet_make_library(SOURCE
  MCDumpers.h
  MCDumperUtils.cxx
  LIBRARIES
  PUBLIC
  larcorealg::geo_vectors_utils_TVector
  nusimdata::SimulationBase
  ROOT::Physics
  PRIVATE
  ${GENIE_LIB_LIST}
  ROOT::EG
)

install_headers()
install_source()
