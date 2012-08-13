////////////////////////////////////////////////////////////////////////
// \file DetectorProperties.h
//
// \brief service to contain information about detector electronics, etc
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_DETECTORPROPERTIES_H
#define UTIL_DETECTORPROPERTIES_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"


///General LArSoft Utilities
namespace util{
    class DetectorProperties {
    public:

      DetectorProperties(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~DetectorProperties();
      
      void reconfigure(fhicl::ParameterSet const& p);

      const double       SamplingRate()      const { return fSamplingRate;   }
      const int          TriggerOffset()     const { return fTriggerOffset;  }
      const double       ElectronsToADC()    const { return fElectronsToADC; }
      const unsigned int NumberTimeSamples() const { return fNumberTimeSamples; }
      const unsigned int ReadOutWindowSize() const { return fReadOutWindowSize; }
   
      const double       TimeOffsetU()       const { return fTimeOffsetU; };
      const double       TimeOffsetV()       const { return fTimeOffsetV; };
      const double       TimeOffsetW()       const { return fTimeOffsetW; };

      double             ConvertXToTicks(double X,     int p, int t, int c) ;
      double             ConvertTicksToX(double ticks, int p, int t, int c) ;

      double             GetXTicksOffset(int p, int t, int c) ;
      double             GetXTicksCoefficient() ;      

    private:

      void         CalculateXTicksParams();

      double       fSamplingRate;      ///< in ns
      int    	   fTriggerOffset;     ///< in # of clock ticks					       	 
      double 	   fElectronsToADC;    ///< conversion factor for # of ionization electrons to 1 ADC count
      unsigned int fNumberTimeSamples; ///< number of clock ticks per event
      unsigned int fReadOutWindowSize; ///< number of clock ticks per readout window
      double       fTimeOffsetU;       ///< time offsets to convert spacepoint
      double       fTimeOffsetV;       ///< coordinates to hit times on each
      double       fTimeOffsetW;       ///< view

      double       fXTicksCoefficient; ///< Parameters for x<-->ticks
      bool         fXTicksParamsLoaded;///<  calculations
      std::vector<std::vector<std::vector<double> > > fXTicksOffsets;

    }; // class DetectorProperties
} //namespace utils
#endif // UTIL_DETECTOR_PROPERTIES_H
