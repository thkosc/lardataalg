/**
 * \file ElecClock.h
 *
 * \ingroup DetectorClocks
 *
 * \brief Class def header for a class ElecClock
 *
 * @author Kazuhiro Terao (kterao@fnal.gov)
 */

/** \addtogroup DetectorClocks

    @{*/
#ifndef ElecClock_H
#define ElecClock_H

#include "ClockConstants.h"
#include "DetectorClocksException.h"

namespace detinfo {
  /**
   * @brief Class representing the time measured by an electronics clock.
   *
   * This class represents the status of a running electronics clock.
   * As such, it has:
   *
   * * _tick frequency_: how many times the clock ticks in one second
   * * _frame period_: the duration of a single frame; clock time is organised
   *   into two levels:
   *     * the time is split into frames
   *     * each frame is split in samples; the first one of a each frame is
   *       sample number 0
   * * _current time_ of the clock, with respect to the time (always "0") the
   *   clock was started at
   *
   * Note that this object is actually able to manage any (continuous) value of
   * time, and the concepts of frame and sample are not used to store the clock
   * state, which is instead defined by its current time.
   *
   * All these quantities are stored in real time units as opposed to clock tick
   * counts. The nominal units for all times and frequencies are microseconds
   * and megahertz, respectively, but `ElecClock` will give consistent results
   * as long as the units of frame period and time are the same, and reciprocal
   * to the frequency unit.
   *
   * The clock can update its time directly (`SetTime()`) or through operators.
   *
   * The clock started at time 0, with the sample 0 of the frame 0 (that is also
   * tick 0). This implies that *all times and ticks returned by the clock
   * implicitly have the same reference as the input time specified by the
   * constructors or by the last call to `SetTime()`*.
   *
   * Some usage examples:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * // get a clock with a period of 500 ns (2 MHz) and a frame of 1.6 ms
   * detinfo::ElecClock clock(0.0, 1600.0, 2.0);
   *
   * std::cout << "New clock:"
   *   << "\n  current time:      " << clock.Time()       // 0.0 us
   *   << "\n  samples per frame: " << clock.FrameTicks() // 3200
   *   << std::endl;
   *
   * clock.SetTime(1, 20); // sets the time to sample #20 of frame 1
   * std::cout << "Time set to sample #20 of frame #1:"
   *   << "\n  current time: " << clock.Time()  // 1610.0 us
   *   << "\n  current tick: " << clock.Tick()  // 3220
   *   << std::endl;
   *
   * clock += 3.7; // add 3.7 us to the current time
   * std::cout << "Added 3.7 microseconds:"
   *   << "\n  current time:  " << clock.Time()       // 1613.7 us
   *   << "\n  current tick:  " << clock.Tick()       // 3227
   *   << "\n  discrete time: " << clock.Time(Time()) // 1613.5 us
   *   << std::endl;
   *
   * clock += 3; // add 3 more ticks (1.5 us) to the current time
   * std::cout << "Added 3 ticks:"
   *   << "\n  current time:  " << clock.Time()       // 1615.2 us
   *   << "\n  current tick:  " << clock.Tick()       // 3230
   *   << "\n  discrete time: " << clock.Time(Time()) // 1615.0 us
   *   << std::endl;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * @note Most methods and operators have different behaviour according to
   *       whether their argument is of type `double`, in which case it is
   *       interpreted as a time, or `int`, where it is interpreted as a clock
   *       tick. Be especially careful when utilizing data types which are
   *       neither `int` not `double`, because a conversion to one of them will
   *       occur, and you need to be in control of which one.
   */
  class ElecClock {
  public:
    /**
     * @brief Constructor: sets all values.
     * @param time starting time of the clock [&micro;s]
     * @param frame_period period of the clock [&micro;s]
     * @param frequency clock frequency [MHz]
     *
     */
    ElecClock(double const time, double const frame_period, double const frequency)
      : ElecClock{time, frame_period, frequency, std::nothrow}
    {
      if (fFrequency <= 0)
        throw detinfo::DetectorClocksException("Only positive frequency allowed.");
    }

    constexpr ElecClock
    WithTime(double const time) const noexcept
    {
      return {time, fFramePeriod, fFrequency, std::nothrow};
    }

    constexpr ElecClock
    WithTick(int const tick, int const frame = 0) const noexcept
    {
      return {Time(tick, frame), fFramePeriod, fFrequency, std::nothrow};
    }

    constexpr ElecClock
    AdvanceTimeBy(double const time) const noexcept
    {
      return {fTime + time, fFramePeriod, fFrequency, std::nothrow};
    }

    constexpr ElecClock
    AdvanceTicksBy(int const ticks) const noexcept
    {
      return {fTime + Time(ticks), fFramePeriod, fFrequency, std::nothrow};
    }

    /**
     * @brief Current time (as stored) in microseconds.
     *
     * Note that this is different than `Time(Time())`, which is discretized.
     */
    constexpr double
    Time() const noexcept
    {
      return fTime;
    }

    /**
     * @brief Returns the absolute time of the start of the specified sample.
     * @param sample sample number within the specified frame
     * @param frame number of the frame the specified sample is in
     * @return time [us] corresponding to the start of the specified sample
     *
     * The sample number is not checked to be actually within the specified
     * frame.
     *
     * The returned time is not related to the current time of the clock.
     */
    constexpr double
    Time(int const sample, int const frame) const noexcept
    {
      return (sample / fFrequency + frame * fFramePeriod);
    }

    /**
     * @brief Returns the discretized value of the specified time.
     * @param time a clock time [&micro;s]
     * @return discretized time [&micro;s] (as a floating point number)
     *
     * The returned time is the start time of the sample `time` falls in.
     *
     * It is not related to the current time of the clock.
     */
    constexpr double
    Time(double const time) const noexcept
    {
      return Time(Sample(time), Frame(time));
    }

    /**
     * @brief Returns the absolute start time of the specified tick.
     * @param ticks a clock time [&micro;s]
     * @return discretized time [&micro;s] (as a floating point number)
     *
     * The returned time is the start time of the tick which `time` falls in.
     *
     * It is not related to the current time of the clock.
     */
    constexpr double
    Time(int const ticks) const noexcept
    {
      return ticks / fFrequency;
    }

    /// Frequency in MHz.
    constexpr double
    Frequency() const
    {
      return fFrequency;
    }

    /// A single frame period in microseconds.
    constexpr double
    FramePeriod() const noexcept
    {
      return fFramePeriod;
    }

    /// Current clock tick (that is, the number of tick `Time()` falls in).
    constexpr int
    Ticks() const noexcept
    {
      return Ticks(fTime);
    }

    /**
     * @brief Returns the number of tick the specified time falls in.
     * @param time time to be converted in ticks [&micro;s]
     * @return the number of the tick containing the specified time
     *
     * The tick number 0 starts at time 0.0 &micro;s.
     *
     * @note The returned value (number of tick) can wrap if the real number of
     *       the tick is beyond the range of the returned data type (`int`).
     */
    constexpr int
    Ticks(double const time) const noexcept
    {
      return static_cast<int>(time * fFrequency);
    }

    /**
     * @brief Returns the number of tick the specified sample falls in.
     * @param sample number of sample in the specified frame
     * @param frame number of frame the specified sample is in
     * @return the number of the tick containing the specified sample
     *
     * The sample 0 of frame 0 is the tick number 0.
     *
     * @note The returned value (number of tick) can wrap if the real number of
     *       the tick is beyond the range of the returned data type (`int`).
     */
    constexpr int
    Ticks(int const sample, int const frame) const noexcept
    {
      return sample + frame * FrameTicks();
    }

    /**
     * @brief Returns number of the sample containing the clock current time.
     * @see `Sample(double)`
     *
     * The returned value is the number of the sample within a frame, which the
     * current clock time falls in. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame()`.
     */
    constexpr int
    Sample() const noexcept
    {
      return Sample(fTime);
    }

    /**
     * @brief Returns the number of the sample containing the specified time.
     * @param time a clock time [&micro;s]
     * @return number of the sample containing the specified time
     * @see `Frame(double)`
     *
     * The returned value is the number of the sample within a frame, which the
     * specified time falls in. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame(double)`.
     *
     * The result is not related to the current time of the clock.
     */
    constexpr int
    Sample(double const time) const noexcept
    {
      return static_cast<int>((time - Frame(time) * fFramePeriod) * fFrequency);
    }

    /**
     * @brief Returns the number of the sample containing the specified tick.
     * @param tick a clock tick number
     * @return number of the sample containing the specified tick
     * @see `Frame(int)`
     *
     * The returned value is the number of the sample within a frame, of the
     * specified tick. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame(int)`.
     *
     * The result is not related to the current time of the clock.
     */
    constexpr int
    Sample(int const tick) const noexcept
    {
      return (tick % static_cast<int>(FrameTicks()));
    }

    /**
     * @brief Returns the number of the frame containing the clock current time.
     * @see `Frame(double)`
     *
     * The returned value is the number of the frame which the current clock
     * time falls in.
     */
    constexpr int
    Frame() const noexcept
    {
      return Frame(fTime);
    }

    /**
     * @brief Returns the number of the frame containing the specified time.
     * @param time a clock time [&micro;s]
     * @return number of the frame containing the specified time
     * @see `Sample(double)`
     *
     * The returned value is the number of the frame which the specified time
     * falls in.
     *
     * The result is not related to the current time of the clock.
     */
    constexpr int
    Frame(double const time) const noexcept
    {
      return static_cast<int>(time / fFramePeriod);
    }

    /**
     * @brief Returns the number of the frame containing the specified tick.
     * @param tick a clock tick number
     * @return number of the frame containing the specified tick
     * @see `Frame(int)`
     *
     * The returned value is the number of the frame the specified tick belongs
     * to.
     *
     * The result is not related to the current time of the clock.
     */
    constexpr int
    Frame(int const tick) const noexcept
    {
      return (tick / static_cast<int>(FrameTicks()));
    }

    /// Number ticks in a frame.
    constexpr unsigned int
    FrameTicks() const noexcept
    {
      return static_cast<unsigned int>(fFramePeriod * fFrequency);
    }

    /// A single tick period in microseconds.
    constexpr double
    TickPeriod() const noexcept
    {
      return 1. / fFrequency;
    }

    //-- comparators --//

    constexpr bool
    operator<(const ElecClock& rhs) const noexcept
    {
      return fTime < rhs.Time();
    }
    constexpr bool
    operator>(const ElecClock& rhs) const noexcept
    {
      return fTime > rhs.Time();
    }
    constexpr bool
    operator<=(const ElecClock& rhs) const noexcept
    {
      return fTime <= rhs.Time();
    }
    constexpr bool
    operator>=(const ElecClock& rhs) const noexcept
    {
      return fTime >= rhs.Time();
    }

  private:
    constexpr ElecClock(double const time,
                        double const frame_period,
                        double const frequency,
                        std::nothrow_t) noexcept
      : fTime(time), fFramePeriod(frame_period), fFrequency(frequency)
    {}

    double fTime{};                 ///< Time in microseconds.
    double fFramePeriod{kTIME_MAX}; ///< Frame period in microseconds.
    double fFrequency{1e9};         ///< Clock speed in MHz.

  }; // class ElecClock

}
#endif
/** @} */ // end of doxygen group
