/**
 * @file   lardataalg/Dumpers/DumperBase.h
 * @brief  Helper base class for implementing data product dump algorithms.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 8th, 2017
 * 
 * Currently this is a header-only library.
 * 
 */

#ifndef LARDATAALG_DUMPERS_DUMPERBASE_H
#define LARDATAALG_DUMPERS_DUMPERBASE_H


// LArSoft includes
#include "lardataalg/Utilities/StatCollector.h" // lar::util::MinMaxCollector
#include "lardataobj/RawData/OpDetWaveform.h"

// C//C++ standard libraries
#include <string>
#include <vector>
#include <algorithm> // std::min()
#include <ios> // std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <utility> // std::forward(), std::swap()


/// Collection of utilities for dumping data on screen.
namespace dump {
  
  /**
   * @brief Base class for data dumpers.
   * 
   * This class provides some basic and common infrastructure:
   * * managing of indentation strings
   * 
   */
  class DumperBase {
    
    std::string fIndent = ""; ///< Default indentation string.
    std::string fFirstIndent = ""; ///< Indentation string for the first line.
    
      public:
    
    /// Default constructor: no indentation.
    DumperBase() = default;
    
    /**
     * @brief Constructor: sets indentation.
     * @param indent indentation for all lines except the first one
     *               (see `indent()`)
     * @param firstIndent indentation for the first line (see `firstIndent()`)
     */
    DumperBase(std::string const& indent, std::string const& firstIndent)
      : fIndent(indent), fFirstIndent(firstIndent)
      {}
    
    /**
     * @brief Constructor: sets indentation.
     * @param indent indentation for all lines
     */
    DumperBase(std::string const& indent): DumperBase(indent, indent) {}
    
    
    /**
     * @name Indentation.
     * 
     * Indentation strings are expected to be written by the dumping algorithms
     * at the beginning of each output line to ensure the proper alignment of
     * the dump. The `indent()` string should be written at the beginning of
     * every new output line, with the exception of the first one, for which
     * `firstIndent()` should be written instead.
     * 
     * Note that this infrastructure only keeps track of the strings needed to
     * indent the output, but it does not provide any tool to apply the
     * indentation.
     */
    /// @{
    
    /// Returns the indentation string currently configured for all lines.
    std::string const& indent() const { return fIndent; }
    
    /// Returns the indentation string currently configured for the first line.
    std::string const& firstIndent() const { return fFirstIndent; }
    
    /// Sets indentation strings to the specified values.
    void setIndent(std::string const& indent, std::string const& firstIndent)
      { fIndent = indent; fFirstIndent = firstIndent; }
    
    /// Sets both indentation strings to the same specified value.
    void setIndent(std::string const& indent) { setIndent(indent, indent); }
    
    
    /// Writes the indentation into a stream, and returns it for further output.
    template <typename Stream>
    Stream& indented(Stream&& out, bool first = false) const
      { out << (first? firstIndent(): indent()); return out; }
      
    /// Writes first line indentation into a stream, and returns it for further
    /// output.
    template <typename Stream>
    Stream& firstIndented(Stream&& out) const
      { return indented(std::forward<Stream>(out), true); }
      
    /// Initiates a new output line, including indentation, and returns the
    /// stream for further output.
    template <typename Stream>
    Stream& newline(Stream&& out) const
      { return indented(std::forward<Stream>(out)); }
    
    /// @}
    
    
    /**
     * @brief Helper class to keep track of indenting.
     * @tparam Stream type of output stream the class is using
     *
     * This class is single use, tied to a dumper and to a stream instance.
     * It does not attempt to keep track of whether the output line is the
     * first or not, and it does not try to detect end-of-line characters
     * in the passed strings to insert internal indentation.
     * 
     * Example of usage:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * void dump(dump::DumperBase::Indenter<std::ostream>& out) {
     *   
     *   out.start() << "Here is the list of things to do:"; // first line
     *   
     *   out.newline() << "  (1) first thing";
     *   out.newline() << "  (2) third thing";
     *   out.newline() << "  (3) fourth thing"
     *                 << "\n" << out.indentString() << "      (if allowed)";
     *   out << std::endl;
     *   
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Note how the manual indentation is more complicate than the "implicit"
     * one.
     */
    template <typename Stream>
    class Indenter {
      Stream out;
      DumperBase const& dumper;
      
        public:
      using indenter_t = Indenter<Stream>; ///< This type.
      
      /// Records the underlying stream and the dumper associated.
      Indenter(Stream out, DumperBase const& dumper)
        : out(std::forward<Stream>(out)), dumper(dumper) {}
      
      /// Returns the default indentation string.
      std::string const& indentString() const { return dumper.indent(); }
      
      /// Returns the indentation string for the first line.
      std::string const& firstIndentString() const
        { return dumper.firstIndent(); }
      
      /// Forwards data to the underlying stream.
      template <typename T>
      indenter_t& operator<< (T&& v)
        { out << std::forward<T>(v); return *this; }
      
      /// Inserts an indentation and returns the indenter for further output.
      indenter_t& indent(bool first = false)
        { dumper.indented(out, first); return *this; }
      
      /// Breaks the current line and returns the indenter for further output.
      indenter_t& newline() { out << '\n'; return indent(); }
      
      /// Inserts a first-line indentation and returns the indenter for further
      /// output.
      indenter_t& start() { return indent(true); }
      
    }; // Indenter<>
    
    
    /// Returns an `Indenter` object tied to this dumper and `out` stream.
    template <typename Stream>
    decltype(auto) indenter(Stream&& out) const
      { return Indenter<Stream>(std::forward<Stream>(out), *this); }
    
  }; // DumperBase
  
  
  /// Changes the indentation settings of a dumper class and returns it back.
  template <typename Dumper>
  auto withIndentation
    (Dumper&& dumper, std::string const& indent, std::string const& firstIndent)
    { dumper.setIndent(indent, firstIndent); return dumper; }

  /// Changes the indentation settings of a dumper class and returns it back.
  template <typename Dumper>
  auto withIndentation
    (Dumper&& dumper, std::string const& indent)
    { return withIndentation(std::forward<Dumper>(dumper), indent, indent); }
  
} // namespace dump


//----------------------------------------------------------------------------
  

#endif // LARDATAALG_DUMPERS_DUMPERBASE_H
