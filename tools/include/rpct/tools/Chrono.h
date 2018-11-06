/* Filip Thyssen */

#ifndef rpct_tools_Chrono_h_
#define rpct_tools_Chrono_h_

#include <iosfwd>

#include "rpct/tools/Time.h"

namespace rpct {
namespace tools {

/** Chrono class
 * Can be used as a chronometer
 */
class Chrono
{
public:
    /** Construct the Chrono to start at the moment of calling*/
    Chrono();
    Chrono(Time const & _time);
    Chrono(::int64_t _seconds, ::int32_t _nanoseconds = 0);
    Chrono(::timespec const & _ts);
    Chrono(::timeval const & _tv);

    /** Set the chrono to start at the moment of calling */
    void set();
    /** Set the chrono start time and resume */
    void set(Time const & _time);
    /** Set the chrono start time and resume */
    void set(::int64_t _seconds, ::int32_t _nanoseconds = 0);

    /** Get the start time of the chronometer */
    Time const & get() const;

    /** Set the chrono to start at the moment of calling and resume */
    void reset();
    bool pause();
    bool resume();

    /** Get the chrono duration */
    Time duration() const;

protected:
    bool pause_;
    Time reference_
        , intermediate_;
};

std::ostream & operator<<(std::ostream & _os, Chrono const & _chrono);

} // namespace tools
} // namespace rpct

#include "rpct/tools/Chrono-inl.h"

#endif // rpct_tools_Chrono_h_
