#include "rpct/tools/Timer.h"

namespace rpct {
namespace tools {

Timer::Timer(Time const & _time)
    : pause_(false)
    , duration_(_time)
    , intermediate_()
{}

Timer::Timer(::int64_t _seconds, ::int64_t _nanoseconds)
    : pause_(false)
    , duration_(_seconds, _nanoseconds)
    , intermediate_()
{}

Timer::Timer(::timespec const & _ts)
    : pause_(false)
    , duration_(_ts)
    , intermediate_()
{}

Timer::Timer(::timeval const & _tv)
    : pause_(false)
    , duration_(_tv)
    , intermediate_()
{}

std::ostream & operator<<(std::ostream & _os, Timer const & _timer)
{
    Time _duration(_timer.duration());
    return (_os << _duration);
}

} // namespace tools
} // namespace rpct
