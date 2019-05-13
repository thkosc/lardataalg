/**
 * @file   lardataalg/MCDumpers/MCDumperUtils.h
 * @brief  Utility functions to print MC truth information.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 2, 2017
 *
 * Utilities for dumping on screen of Monte Carlo data.
 *
 */

#ifndef LARDATAALG_MCDUMPERS_MCDUMPERUTILS_H
#define LARDATAALG_MCDUMPERS_MCDUMPERUTILS_H

// nutools libraries
#include "nusimdata/SimulationBase/MCTruth.h" // simb::Origin_t

// C/C++ standard libraries
#include <string>


namespace sim {

  /// @{
  /// @name Function to name `simb::MCParticle` enumerators and codes.

  /// Returns a string with the name of particle the specified with PDG ID.
  std::string ParticleName(int pigid);

  /// Describes the status of a particle (`simb::MCParticle::StatusCode()`).
  std::string ParticleStatusName(int code);

  /// @}


  /// @{
  /// @name Functions to name `simb::MCTruth` enumerators and codes.
  
  /// Possible sources of rescattering code (which is generator-dependent).
  enum class RescatterCategory {
    GENIE_INukeFateHA, ///< GENIE `genie::EINukeFateHA_t`
    LArSoftDefault = GENIE_INukeFateHA
  };
  
  
  /// Returns a string representing the specified process origin.
  std::string TruthOriginName(simb::Origin_t origin);

  /// Returns a string representing the specified process from `simb::MCTruth`
  /// (CC or NC, nothing fancy).
  std::string TruthCCNCname(int ccnc);

  /// Returns the "mode" of the reaction (a lesser version of interaction type).
  std::string TruthReactionMode(int mode);

  /// Returns a string representing the specified interaction type as in
  /// `simb::MCTruth` convention.
  std::string TruthInteractionTypeName(int type);

  /**
   * @brief  The name of the specified rescattering code.
   * @param code the rescattering code
   * @param type the category of rescattering `code` belongs to
   * @return a string describing the rescattering code
   * 
   * The meaning of the rescattering code is generator-dependent, and also
   * within a generator it may be algorithm-dependent. The category pins down
   * which set of possible meaning the `code` belongs to. In LArSoft, if the
   * code is unassigned by the generator, it will default to
   * `simb::MCParticle::s_uninitialized`.
   */
  std::string RescatteringName
    (int code, RescatterCategory cat = RescatterCategory::LArSoftDefault);

  /// Description of a rescattering code from GENIE `INukeFateHA_t`.
  std::string GENIE_INukeFateHA_RescatteringName(int code);
  
  /// @}


} // namespace sim


#endif // LARDATAALG_MCDUMPERS_MCDUMPERUTILS_H
