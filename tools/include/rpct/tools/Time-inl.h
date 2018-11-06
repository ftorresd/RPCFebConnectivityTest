/* Filip Thyssen */

#ifndef rpct_tools_Time_inl_h_
#define rpct_tools_Time_inl_h_

#include "rpct/tools/Time.h"

#include <limits>

namespace rpct {
namespace tools {

inline void Time::validate_input()
{
    // bring nanoseconds in 0 <= nanoseconds_ < nanoseconds_per_second_
    int _pseconds(nanoseconds_ < 0
                  ? -1 - ((-nanoseconds_ - 1) / nanoseconds_per_second_)
                  : (nanoseconds_ / nanoseconds_per_second_));

    seconds_ += _pseconds;
    nanoseconds_ -= (_pseconds * nanoseconds_per_second_);
    if (seconds_ < 0 && nanoseconds_ > 0)
        {
            seconds_ += 1;
            nanoseconds_ -= nanoseconds_per_second_;
        }
}

inline void Time::validate_sign()
{
    if (seconds_ < 0 && nanoseconds_ > 0)
        {
            nanoseconds_ -= nanoseconds_per_second_;
            ++seconds_;
        }
    else if (seconds_ > 0 && nanoseconds_ < 0)
        {
            nanoseconds_ += nanoseconds_per_second_;
            --seconds_;
        }
}

inline void Time::validate_operation()
{
    if (nanoseconds_ >= nanoseconds_per_second_)
        {
            nanoseconds_ -= nanoseconds_per_second_;
            ++seconds_;
        }
    else if (nanoseconds_ <= -nanoseconds_per_second_)
        {
            nanoseconds_ += nanoseconds_per_second_;
            --seconds_;
        }
    validate_sign();
}

inline void Time::set(::int64_t _seconds, ::int64_t _nanoseconds)
{
    seconds_ = _seconds;
    nanoseconds_ = _nanoseconds;
    validate_input();
}

inline void Time::seconds(::int64_t _seconds)
{
    seconds_ = _seconds;
    validate_sign();
}

inline void Time::nanoseconds(::int64_t _nanoseconds)
{
    nanoseconds_ = _nanoseconds;
    validate_input();
}

inline ::int64_t Time::seconds() const
{
    return seconds_;
}

inline ::int64_t Time::nanoseconds() const
{
    return nanoseconds_;
}

inline ::timespec Time::timespec() const
{
    ::timespec _ts;
    _ts.tv_sec = (seconds_ > 0 ? seconds_ : 0);
    _ts.tv_nsec = (nanoseconds_ > 0 ? nanoseconds_ : 0);
    return _ts;
}

inline ::timeval Time::timeval() const
{
    ::timeval _tv;
    _tv.tv_sec = (seconds_ > 0 ? seconds_ : 0);
    _tv.tv_usec = (nanoseconds_ > 0 ? nanoseconds_ / nanoseconds_per_microsecond_ : 0);
    return _tv;
}

inline ::int64_t Time::ustime() const
{
    return seconds_ * seconds_per_microsecond_ + nanoseconds_ / nanoseconds_per_microsecond_;
}

inline Time::operator double() const
{
    return ((double)nanoseconds_)/nanoseconds_per_second_ + seconds_;
}

inline bool Time::sleep() const
{
    ::timespec _ts(this->timespec());
    return (::nanosleep(&_ts, 0) == 0);
}

inline Time & Time::operator+=(Time const & _rh)
{
    seconds_ += _rh.seconds_;
    nanoseconds_ += _rh.nanoseconds_;
    validate_operation();
    return *this;
}

inline Time Time::operator+(Time const & _rh) const
{
    Time _out(*this);
    _out += _rh;
    return _out;
}

inline Time Time::operator+() const
{
    return Time(seconds_, nanoseconds_, false);
}

inline Time & Time::operator-=(Time const & _rh)
{
    seconds_ -= _rh.seconds_;
    nanoseconds_ -= _rh.nanoseconds_;
    validate_operation();
    return *this;
}

inline Time Time::operator-(Time const & _rh) const
{
    Time _out(*this);
    _out -= _rh;
    return _out;
}

inline Time Time::operator-() const
{
    return Time(-seconds_, -nanoseconds_, false);
}

template<typename t_rh_type>
inline Time & Time::operator*=(t_rh_type const & _rh)
{
    if (std::numeric_limits<t_rh_type>::is_integer)
        {
            seconds_ *= (::int64_t)_rh;
            nanoseconds_ *= (::int64_t)_rh;
        }
    else
        {
            double _seconds = (double)(seconds_) * _rh;
            seconds_ = (::int64_t)(_seconds);
            nanoseconds_ = (::int64_t)((double)(nanoseconds_) * _rh)
                + (::int64_t)((_seconds - (double)((::int64_t)(_seconds))) * nanoseconds_per_second_);
        }
    validate_input();
    return *this;
}

template<typename t_rh_type>
inline Time Time::operator*(t_rh_type const & _rh) const
{
    Time _out(*this);
    _out *= _rh;
    return _out;
}

inline bool Time::operator==(Time const & _rh) const
{
    return (seconds_ == _rh.seconds_
            && nanoseconds_ == _rh.seconds_);
}

inline bool Time::operator<(Time const & _rh) const
{
    return (seconds_ < _rh.seconds_
            || (seconds_ == _rh.seconds_ && nanoseconds_ < _rh.seconds_));
}

inline bool Time::operator>(Time const & _rh) const
{
    return (seconds_ > _rh.seconds_
            || (seconds_ == _rh.seconds_ && nanoseconds_ > _rh.seconds_));
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_Time_inl_h_
