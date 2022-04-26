cet_make_library(INTERFACE
  SOURCE
  DumperBase.h
  RawData/OpDetWaveform.h
  LIBRARIES INTERFACE
  lardataalg::UtilitiesHeaders
  lardataobj::headers
)

install_headers(SUBDIRS RawData)
install_source(SUBDIRS RawData)
