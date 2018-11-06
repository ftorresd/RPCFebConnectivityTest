/* Filip Thyssen */

#ifndef rpct_tools_Timer_inl_h_
#define rpct_tools_Timer_inl_h_

#include "rpct/tools/Timer.h"

namespace rpct {
namespace tools {

inline void Timer::set(Time const & _time)
{
    duration_ = _time;
}

inline void Timer::set(::int64_t _seconds, ::int64_t _nanoseconds)
{
    duration_.set(_seconds, _nanoseconds);
}

inline void Timer::reset()
{
    pause_ = false;
    intermediate_.set();
}

inline bool Timer::pause()
{
    if (pause_)
        return false;
    pause_ = true;
    intermediate_ = (intermediate_ - Time());
    return true;
}

inline bool Timer::resume()
{
    if (!pause_)
        return false;
    pause_ = false;
    intermediate_ += Time();
    return true;
}

inline Time Timer::duration() const
{
    if (pause_)
        return (duration_ + intermediate_);
    return (duration_ - (Time() - intermediate_));
}

inline Time Timer::end() const
{
    if (pause_)
        return (Time() + duration_ + intermediate_);
    return (intermediate_ + duration_);
}

inline bool Timer::ready() const
{
    return (end() < Time());
}

inline bool Timer::sleep() const
{
    return duration().sleep();
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_Timer_inl_h_
