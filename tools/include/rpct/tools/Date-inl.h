/* Filip Thyssen */

#ifndef rpct_tools_Date_inl_h
#define rpct_tools_Date_inl_h

#include "rpct/tools/Date.h"

#include <stdexcept>
#include <sstream>

namespace rpct {
namespace tools {

inline ::uint64_t Date::id() const
{
    return id_;
}
inline Date::operator ::uint64_t() const
{
    return id_;
}

inline void Date::id(::uint64_t const & _id)
{
    id_ = _id;
}

// Field Getters
inline int Date::year() const
{
    return bf_get(min_year_, mask_year_, pos_year_);
}
inline int Date::month() const
{
    return bf_get(min_month_, mask_month_, pos_month_);
}
inline int Date::day() const
{
    return bf_get(min_day_, mask_day_, pos_day_);
}
inline int Date::hour() const
{
    return bf_get(min_hour_, mask_hour_, pos_hour_);
}
inline int Date::minute() const
{
    return bf_get(min_minute_, mask_minute_, pos_minute_);
}
inline int Date::second() const
{
    return bf_get(min_second_, mask_second_, pos_second_);
}
inline int Date::microseconds() const
{
    return bf_get(min_microseconds_, mask_microseconds_, pos_microseconds_);
}
inline int Date::daylightsavingstime() const
{
    return bf_get(min_daylightsavingstime_, mask_daylightsavingstime_, pos_daylightsavingstime_);
}

// Field Setters
inline void Date::setYear(int _year)
{
    return bf_set(min_year_, max_year_, mask_year_, pos_year_, _year);
}
inline void Date::setMonth(int _month)
{
    return bf_set(min_month_, max_month_, mask_month_, pos_month_, _month);
}
inline void Date::setDay(int _day)
{
    return bf_set(min_day_, max_day_, mask_day_, pos_day_, _day);
}
inline void Date::setHour(int _hour)
{
    return bf_set(min_hour_, max_hour_, mask_hour_, pos_hour_, _hour);
}
inline void Date::setMinute(int _minute)
{
    return bf_set(min_minute_, max_minute_, mask_minute_, pos_minute_, _minute);
}
inline void Date::setSecond(int _second)
{
    return bf_set(min_second_, max_second_, mask_second_, pos_second_, _second);
}
inline void Date::setMicroSeconds(int _microseconds)
{
    return bf_set(min_microseconds_, max_microseconds_, mask_microseconds_, pos_microseconds_, _microseconds);
}
inline void Date::setDaylightSavingsTime(int _daylightsavingstime)
{
    return bf_set(min_daylightsavingstime_, max_daylightsavingstime_, mask_daylightsavingstime_, pos_daylightsavingstime_, _daylightsavingstime);
}

inline bool Date::operator<(Date const & _in) const
{
    return (id_ < _in.id_);
}
inline bool Date::operator==(Date const & _in) const
{
    return (id_ == _in.id_);
}
inline bool Date::operator!=(Date const & _in) const
{
    return (id_ != _in.id_);
}
inline bool Date::operator<(::uint64_t const & _in) const
{
    return (id_ < _in);
}
inline bool Date::operator==(::uint64_t const & _in) const
{
    return (id_ == _in);
}
inline bool Date::operator!=(::uint64_t const & _in) const
{
    return (id_ != _in);
}
inline int Date::bf_get(int const _min, ::uint64_t const _mask, int const _pos) const
{
    return _min + ((id_ & _mask) >> _pos);
}
inline void Date::bf_set(int const _min, int const _max, ::uint64_t const _mask, int const _pos, int const _value)
{
    if (_value >= _min && _value <= _max)
        {
            id_ &= ~_mask;
            id_ |= (((::uint64_t)(_value - _min) << _pos) & _mask);
        }
    else
        {
            std::stringstream _errormsg;
            _errormsg << "Out-of-range input for Date::bf_set, position " << _pos << ": " << _value;
            throw std::out_of_range(_errormsg.str());
        }
}
inline void Date::bf_stream(std::ostream & os, int const _min, ::uint64_t const _mask, int const _pos) const
{
    os << bf_get(_min, _mask, _pos);
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_Date_inl_h
