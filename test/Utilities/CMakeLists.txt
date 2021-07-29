cet_test(constexpr_math_test)
cet_test(quantities_test USE_BOOST_UNIT)
cet_test(quantities_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp fhiclcpp::types cetlib_except::cetlib_except
  )
cet_test(intervals_test USE_BOOST_UNIT)
cet_test(intervals_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp fhiclcpp::types cetlib_except::cetlib_except
  )
cet_test(space_test USE_BOOST_UNIT)
cet_test(frequency_test USE_BOOST_UNIT)
cet_test(energy_test USE_BOOST_UNIT)
cet_test(datasize_test USE_BOOST_UNIT)
cet_test(StatCollector_test USE_BOOST_UNIT)
cet_test(MappedContainer_test USE_BOOST_UNIT)
cet_test(MultipleChoiceSelection_test USE_BOOST_UNIT)

install_fhicl()
install_source()
