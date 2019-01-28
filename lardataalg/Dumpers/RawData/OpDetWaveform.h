/**
 * @file   lardataalg/Dumpers/RawData/OpDetWaveform.h
 * @brief  Utilities to dump `raw::OpDetWaveform` objects on screen.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 8th, 2017
 * 
 * Currently this is a header-only library.
 * 
 */

#ifndef LARDATAALG_DUMPERS_RAWDATA_OPDETWAVEFORM_H
#define LARDATAALG_DUMPERS_RAWDATA_OPDETWAVEFORM_H


// LArSoft includes
#include "lardataalg/Dumpers/DumperBase.h"
#include "lardataalg/Utilities/StatCollector.h" // lar::util::MinMaxCollector
#include "lardataobj/RawData/OpDetWaveform.h"

// C//C++ standard libraries
#include <vector>
#include <algorithm> // std::min()
#include <ios> // std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <utility> // std::forward(), std::swap()


namespace dump::raw {
  
  using namespace ::raw;
  
  /**
   * @brief Prints the content of optical detector waveforms on screen.
   * 
   * Example of usage:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * dump::raw::OpDetWaveformDumper dump;
   * dump.setIndent("  ");
   * 
   * for (raw::OpDetWaveform const& waveform: waveforms)
   *   dump(mf::LogVerbatim("dumper"), waveform);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  class OpDetWaveformDumper: public DumperBase {
      public:
    
    /**
     * @brief Constructor: sets waveform dump parameters.
     * @param pedestal (default: `0`) the pedestal to be automatically added to
     *                 all digits
     * @param digitsPerLine (default: `0`) how many ADC digits to print per
     *                      line; `0` disables the digit printing completely
     * 
     * Note that no indentation is set. If some is desired, set it with
     * `setIndent()` after construction.
     */
    OpDetWaveformDumper
      (raw::ADC_Count_t pedestal = 0, unsigned int digitsPerLine = 0U)
      : fPedestal(pedestal), fDigitsPerLine(digitsPerLine)
      {}
    
    
    /**
     * @brief Dumps the content of a waveform into the specified output stream.
     * @tparam Stream type of stream to dump data into
     * @param stream stream to dump data into
     * @param waveform the object to be dumped
     *
     * Indentation is regulated via base class methods (see `setIndent()`).
     */
    template <typename Stream>
    void dump(Stream&& stream, raw::OpDetWaveform const& waveform) const;
    
    /// An alias of `dump()`.
    template <typename Stream>
    void operator()(Stream&& stream, raw::OpDetWaveform const& waveform) const
      { dump(std::forward<Stream>(stream), waveform); }
    
    
      private:
    
    raw::ADC_Count_t fPedestal; ///< ADC pedestal (subtracted from readings).
    unsigned int fDigitsPerLine;  ///< ADC readings per line in the output.
    
  }; // class OpDetWaveformDumper
  
} // namespace dump::raw


// -----------------------------------------------------------------------------
// ---  Template implementation
// -----------------------------------------------------------------------------
template <typename Stream>
void dump::raw::OpDetWaveformDumper::dump
  (Stream&& stream, raw::OpDetWaveform const& waveform) const
{
  auto const& data = waveform;
  using Count_t = raw::ADC_Count_t;
  
  auto out = indenter(std::forward<Stream>(stream));
  
  // print a header for the raw digits
  out.start()
    << "on channel #" << waveform.ChannelNumber() << " (time stamp: "
    << waveform.TimeStamp() << "): " << data.size() << " time ticks";
  
  // print the content of the channel
  if (fDigitsPerLine == 0) return;
  
  std::vector<Count_t> DigitBuffer(fDigitsPerLine), LastBuffer;
  
  unsigned int repeat_count = 0; // additional lines like the last one
  unsigned int index = 0;
  
  lar::util::MinMaxCollector<Count_t> Extrema;
  out.newline()
    << "  content of the channel (" << fDigitsPerLine << " ticks per line):";
  auto iTick = data.cbegin(), tend = data.cend(); // const iterators
  while (iTick != tend) {
    // the next line will show at most fDigitsPerLine ticks
    unsigned int line_size
      = std::min(fDigitsPerLine, (unsigned int) data.size() - index);
    if (line_size == 0) break; // no more ticks
    
    // fill the new buffer (iTick will move forward)
    DigitBuffer.resize(line_size);
    auto iBuf = DigitBuffer.begin(), bend = DigitBuffer.end();
    while ((iBuf != bend) && (iTick != tend))
      Extrema.add(*(iBuf++) = *(iTick++) - fPedestal);
    index += line_size;
    
    // if the new buffer is the same as the old one, just mark it
    if (DigitBuffer == LastBuffer) {
      repeat_count += 1;
      continue;
    }
    
    // if there are previous repeats, write that on screen
    // before the new, different line
    if (repeat_count > 0) {
      out.newline()
        << "  [ ... repeated " << repeat_count << " more times ]";
      repeat_count = 0;
    }
    
    // dump the new line of ticks
    out.newline() << " ";
    for (auto digit: DigitBuffer)
      out << " " << std::setw(4) << digit;
    
    // quick way to assign DigitBuffer to LastBuffer
    // (we don't care we lose the former)
    std::swap(LastBuffer, DigitBuffer);
    
  } // while
  if (repeat_count > 0) {
    out.newline()
      << "  [ ... repeated " << repeat_count << " more times to the end]";
  }
  if (Extrema.min() != Extrema.max()) {
    out.newline()
      << "  range of " << data.size()
      << " samples: [" << Extrema.min() << ";" << Extrema.max() << "]";
  }

} // dump::raw::OpDetWaveformDumper::dump()


//----------------------------------------------------------------------------
  

#endif // LARDATAALG_DUMPERS_RAWDATA_OPDETWAVEFORM_H
