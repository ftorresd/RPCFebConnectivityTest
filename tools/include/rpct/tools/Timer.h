/* Filip Thyssen */

#ifndef rpct_tools_Timer_h_
#define rpct_tools_Timer_h_

#include <iosfwd>

#include "rpct/tools/Time.h"

namespace rpct {
namespace tools {

/** Timer class
 * Can be used as a countdown timer
 */
class Timer
{
public:
    Timer(Time const & _time);
    Timer(::int64_t _seconds = 0, ::int64_t _nanoseconds = 0);
    Timer(::timespec const & _ts);
    Timer(::timeval const & _tv);

    /** Set the duration of the timer */
    void set(Time const & _time);
    /** Set the duration of the timer */
    void set(::int64_t _seconds = 0, ::int64_t _nanoseconds = 0);

    void reset();
    bool pause();
    bool resume();

    /** Get the remaining timer duration */
    Time duration() const;

    /** Get the Time at which the timer ends */
    Time end() const;

    /** Check if the timer ended */
    bool ready() const;

    /** sleep until the timer ends
     * \return false if a signal interrupted
     */
    bool sleep() const;

protected:
    bool pause_;
    Time duration_
        , intermediate_;
};

std::ostream & operator<<(std::ostream & _os, Timer const & _timer);

} // namespace tools
} // namespace rpct

#include "rpct/tools/Timer-inl.h"

#endif // rpct_tools_Timer_h_
