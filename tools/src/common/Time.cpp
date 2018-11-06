#include "rpct/tools/Time.h"

#include <sstream>
#include <ostream>
#include <iomanip>
#include <cstdlib>

namespace rpct {
namespace tools {

Time::Time(::int64_t _seconds, ::int64_t _nanoseconds
           , bool _validate)
    : seconds_(_seconds)
    , nanoseconds_(_nanoseconds)
{
    if (_validate)
        validate_input();
}

Time::Time()
{
    set();
}

Time::Time(::int64_t _seconds, ::int64_t _nanoseconds)
    : seconds_(_seconds)
    , nanoseconds_(_nanoseconds)
{
    validate_input();
}

Time::Time(::timespec const & _ts)
    : seconds_(_ts.tv_sec)
    , nanoseconds_(_ts.tv_nsec)
{
    validate_input();
}

Time::Time(::timeval const & _tv)
    : seconds_(_tv.tv_sec)
    , nanoseconds_(_tv.tv_usec * nanoseconds_per_microsecond_)
{
    validate_input();
}

Time::Time(::tm const & _tm)
    : seconds_(0)
    , nanoseconds_(0)
{
    ::tm _tm_copy = _tm;
    seconds_ = ::mktime(&_tm_copy);
}

Time Time::ustime(::int64_t _ustime)
{
    return Time(_ustime / seconds_per_microsecond_
                , (_ustime % seconds_per_microsecond_) * nanoseconds_per_microsecond_);
}

void Time::set()
{
    ::timeval _tv;
    ::gettimeofday(&_tv, 0);
    seconds_ = _tv.tv_sec;
    nanoseconds_ = _tv.tv_usec * nanoseconds_per_microsecond_;
}

std::ostream & operator<<(std::ostream & _os, Time const & _time)
{
    std::stringstream _ss;
    _ss << _time.seconds() << '.'
       << std::setw(9) << std::setfill('0')
       << std::abs(_time.nanoseconds());
    return (_os << _ss.str());
}

} // namespace tools
} // namespace rpct
