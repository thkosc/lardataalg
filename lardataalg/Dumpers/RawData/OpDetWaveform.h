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
    
    /// Base functor for printing time according to tick number.
    struct TimeLabelMaker {
      
      virtual ~TimeLabelMaker() = default;
      
      /// Returns the label to be written for the specified `tick` number.
      virtual std::string label
        (raw::OpDetWaveform const& waveform, unsigned int tick) const = 0;
      
      /// Length of padded label. For best results, it should be a constant.
      virtual unsigned int labelWidth
        (raw::OpDetWaveform const& waveform, unsigned int tick) const
        { return 10U; }
      
    }; // struct TimeLabelMaker
    
    
    /// Time label: tick number relative to the waveform.
    struct TickLabelMaker: public TimeLabelMaker {
      
      /// Returns the label to be written for the specified `tick` number.
      virtual std::string label
        (raw::OpDetWaveform const&, unsigned int tick) const override
        { return std::to_string(tick); }
      
      /// Length of padded label. For best results, it should be a constant.
      virtual unsigned int labelWidth
        (raw::OpDetWaveform const& waveform, unsigned int) const override
        { return digitsOf(waveform.size()); }
      
    }; // struct TickLabelMaker
    
    
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
     * @brief Chooses which time label maker to use.
     * @param timeLabelMaker a pointer to the external time label maker
     * 
     * A time label maker is an object derived from `TimeLabelMaker` which can
     * convert a waveform and tick into a string representing that tick within
     * the waveform. If never specified or if `nullptr`, no time label will be
     * printed at all.
     */
    void setTimeLabelMaker(TimeLabelMaker const* timeLabelMaker)
      { fTimeLabelMaker = timeLabelMaker; }
    

    /**
     * @brief Dumps the content of a waveform into the specified output stream.
     * @tparam Stream type of stream to dump data into
     * @param stream stream to dump data into
     * @param waveform the object to be dumped
     *
     * Indentation is regulated via base class methods (see `setIndent()`).
     */
    template <typename Stream>
    void dump(Stream&& stream, raw::OpDetWaveform const& waveform);

    /// An alias of `dump()`.
    template <typename Stream>
    void operator()(Stream&& stream, raw::OpDetWaveform const& waveform)
      { dump(std::forward<Stream>(stream), waveform); }


    /// Pads the specified string to the right, truncating its right if needed.
    static std::string padRight
      (std::string const& s, unsigned int width, std::string padding = " ");
    
    /// Pads the specified string to the right, truncating its right if needed.
    static unsigned int digitsOf(unsigned int n);
    
      private:
    
    raw::ADC_Count_t fPedestal; ///< ADC pedestal (subtracted from readings).
    unsigned int fDigitsPerLine;  ///< ADC readings per line in the output.
    
    /// The functor to be used to extract the time label.
    TimeLabelMaker const* fTimeLabelMaker = nullptr;
    
  }; // class OpDetWaveformDumper

} // namespace dump::raw


// -----------------------------------------------------------------------------
// ---  Template implementation
// -----------------------------------------------------------------------------
template <typename Stream>
void dump::raw::OpDetWaveformDumper::dump
  (Stream&& stream, raw::OpDetWaveform const& waveform)
{
  static std::string const headerSep = " | ";
  
  auto const& data = waveform;
  using Count_t = raw::ADC_Count_t;

  auto out = indenter(std::forward<Stream>(stream));
  
  // print a header for the raw digits
  out.start()
    << "on channel #" << waveform.ChannelNumber() << " (time stamp: "
    << waveform.TimeStamp() << "): " << data.size() << " time ticks";
  
  // print the content of the channel
  if (fDigitsPerLine == 0) return;

  // save and change indentation
  saveIndentSettings().set(indent() + "  ");
  
  unsigned int repeat_count = 0U; // additional lines like the last one
  unsigned int index = 0U;
  unsigned int firstLineTick = 0U;
  
  // local function for printing and resetting the repeat count
  auto flushRepeatCount
    = [this, &out, &waveform, &firstLineTick](unsigned int& count)
    -> decltype(auto)
    {
      if (count > 0) {
        out.newline();
        if (fTimeLabelMaker) {
          out
            << padRight("", fTimeLabelMaker->labelWidth(waveform, firstLineTick))
            << headerSep
            ;
        }
        out << " [ ... repeated " << count << " more times ]";
        count = 0;
      }
      return out;
    };

  std::vector<Count_t> DigitBuffer(fDigitsPerLine), LastBuffer;

  lar::util::MinMaxCollector<Count_t> Extrema;
  out.newline()
    << "content of the channel (" << fDigitsPerLine << " ticks per line):";
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
    firstLineTick = index;
    index += line_size;

    // if the new buffer is the same as the old one, just mark it
    if (DigitBuffer == LastBuffer) {
      repeat_count += 1;
      continue;
    }
    
    // if there are previous repeats, write that on screen
    // before the new, different line
    flushRepeatCount(repeat_count);

    // dump the new line of ticks
    out.newline();
    if (fTimeLabelMaker) {
      out << padRight(
          fTimeLabelMaker->label(waveform, firstLineTick),
          fTimeLabelMaker->labelWidth(waveform, firstLineTick)
        )
        << headerSep
        ;
    }
    for (auto digit: DigitBuffer)
      out << " " << std::setw(4) << digit;
    
    
    // quick way to assign DigitBuffer to LastBuffer
    // (we don't care we lose the former)
    std::swap(LastBuffer, DigitBuffer);
    
  } // while
  flushRepeatCount(repeat_count);
  if (Extrema.min() != Extrema.max()) {
    out.newline()
      << "  range of " << data.size()
      << " samples: [" << Extrema.min() << ";" << Extrema.max() << "]";
  }
  
  restoreIndentSettings();
  
} // dump::raw::OpDetWaveformDumper::dump()


//----------------------------------------------------------------------------
std::string dump::raw::OpDetWaveformDumper::padRight
  (std::string const& s, unsigned int width, std::string padding /* = " " */)
{

  if (s.length() > width) { // too long already?
    // truncate on the right
    return { s, 0U, width }; // { string, start index, character count }
  }

  std::string padded;
  padded.reserve(width);
  
  // this is how many full padding strings we need to prepend
  unsigned int nPadding
    = (s.length() >= width)? 0U: (width - s.length()) / padding.length();
  
  // if there is need for some more partial padding beyond that:
  if (nPadding * padding.length() + s.length() < width) {
    // immediately prepend from the rightmost part of the padding string
    padded.append(
      padding.end() - (width - (nPadding * padding.length() + s.length())),
      padding.end()
      );
  }
  
  // add the rest of the padding and the string, and then we are done
  while (nPadding-- > 0) padded += padding;
  padded += s;
  
  assert(padded.length() == width);
  
  return padded;
} // dump::raw::OpDetWaveformDumper::padRight()


//----------------------------------------------------------------------------
unsigned int dump::raw::OpDetWaveformDumper::digitsOf(unsigned int n) {
  unsigned int digits = 1U;
  while (n >= 10) {
    ++digits;
    n /= 10;
  }
  return digits;
} // dump::raw::OpDetWaveformDumper::digitsOf()


//----------------------------------------------------------------------------


#endif // LARDATAALG_DUMPERS_RAWDATA_OPDETWAVEFORM_H
