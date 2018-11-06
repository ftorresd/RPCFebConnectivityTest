#include "rpct/tools/Chrono.h"

namespace rpct {
namespace tools {

Chrono::Chrono()
    : pause_(false)
    , reference_()
    , intermediate_(Time(0))
{}

Chrono::Chrono(Time const & _time)
    : pause_(false)
    , reference_(_time)
    , intermediate_(Time(0))
{}

Chrono::Chrono(::int64_t _seconds, ::int32_t _nanoseconds)
    : pause_(false)
    , reference_(_seconds, _nanoseconds)
    , intermediate_(Time(0))
{}

Chrono::Chrono(::timespec const & _ts)
    : pause_(false)
    , reference_(_ts)
    , intermediate_(Time(0))
{}

Chrono::Chrono(::timeval const & _tv)
    : pause_(false)
    , reference_(_tv)
    , intermediate_(Time(0))
{}

std::ostream & operator<<(std::ostream & _os, Chrono const & _chrono)
{
    Time _duration(_chrono.duration());
    return (_os << _duration);
}

} // namespace tools
} // namespace rpct
