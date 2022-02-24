cet_test(constexpr_math_test)
cet_test(quantities_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(quantities_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    fhiclcpp::types
    cetlib_except::cetlib_except
    lardataalg::UtilitiesHeaders
  )
cet_test(intervals_test
         LIBRARIES PRIVATE cetlib_except::cetlib_except
                           lardataalg::UtilitiesHeaders
         USE_BOOST_UNIT)
cet_test(intervals_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    fhiclcpp::types
    cetlib_except::cetlib_except
    lardataalg::UtilitiesHeaders
  )
cet_test(space_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(frequency_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(energy_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(datasize_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(StatCollector_test USE_BOOST_UNIT)
cet_test(MappedContainer_test LIBRARIES lardataalg::UtilitiesHeaders USE_BOOST_UNIT)
cet_test(MultipleChoiceSelection_test USE_BOOST_UNIT)

install_fhicl()
install_source()
