#include "rpct/tools/Date.h"

#include <ostream>
#include <sstream>
#include <iomanip>

#include "rpct/tools/Time.h"

namespace rpct {
namespace tools {

Date::Date()
    : id_(0x0)
{
    set();
}

Date::Date(::uint64_t const & _id)
    : id_(_id)
{}

Date::Date(int _year
           , int _month
           , int _day
           , int _hour
           , int _minute
           , int _second
           , int _microseconds
           , int _daylightsavingstime)
    : id_(0x0)
{
    setYear(_year);
    setMonth(_month);
    setDay(_day);
    setHour(_hour);
    setMinute(_minute);
    setSecond(_second);
    setMicroSeconds(_microseconds);
    setDaylightSavingsTime(_daylightsavingstime);
}

Date::Date(Time const & _time)
{
    ::tm _tm;
    ::time_t _seconds = _time.seconds();
    if (::localtime_r(&_seconds, &_tm))
        {
            setYear(_tm.tm_year + 1900);
            setMonth(_tm.tm_mon + 1);
            setDay(_tm.tm_mday);
            setHour(_tm.tm_hour);
            setMinute(_tm.tm_min);
            setSecond(_tm.tm_sec);
            setMicroSeconds(_time.nanoseconds() / Time::nanoseconds_per_microsecond_);
            setDaylightSavingsTime(_tm.tm_isdst);
        }
}

void Date::set()
{
    ::timeval _tv;
    ::gettimeofday(&_tv, 0);
    ::tm _tm;
    if (::localtime_r(&(_tv.tv_sec), &_tm))
        {
            setYear(_tm.tm_year + 1900);
            setMonth(_tm.tm_mon + 1);
            setDay(_tm.tm_mday);
            setHour(_tm.tm_hour);
            setMinute(_tm.tm_min);
            setSecond(_tm.tm_sec);
            setMicroSeconds(_tv.tv_usec);
            setDaylightSavingsTime(_tm.tm_isdst);
        }
}

void Date::set(Time const & _time)
{
    ::tm _tm;
    ::time_t _seconds = _time.seconds();
    if (::localtime_r(&_seconds, &_tm))
        {
            setYear(_tm.tm_year + 1900);
            setMonth(_tm.tm_mon + 1);
            setDay(_tm.tm_mday);
            setHour(_tm.tm_hour);
            setMinute(_tm.tm_min);
            setSecond(_tm.tm_sec);
            setMicroSeconds(_time.nanoseconds() / Time::nanoseconds_per_microsecond_);
            setDaylightSavingsTime(_tm.tm_isdst);
        }
}

Time Date::time() const
{
    ::tm _tm;
    _tm.tm_year = year() - 1900;
    _tm.tm_mon = month() - 1;
    _tm.tm_mday = day();
    _tm.tm_hour = hour();
    _tm.tm_min = minute();
    _tm.tm_sec = minute();
    _tm.tm_isdst = daylightsavingstime();
    ::time_t _seconds = ::mktime(&_tm);
    return Time(_seconds, microseconds() * Time::nanoseconds_per_microsecond_);
}

std::string Date::name() const
{
    std::ostringstream _ss;
    _ss.fill('0');
    bf_stream(_ss << std::setw(4), min_year_, mask_year_, pos_year_);
    bf_stream(_ss << '-' << std::setw(2), min_month_, mask_month_, pos_month_);
    bf_stream(_ss << '-' << std::setw(2), min_day_, mask_day_, pos_day_);
    bf_stream(_ss << ' ' << std::setw(2), min_hour_, mask_hour_, pos_hour_);
    bf_stream(_ss << ':' << std::setw(2), min_minute_, mask_minute_, pos_minute_);
    bf_stream(_ss << ':' << std::setw(2), min_second_, mask_second_, pos_second_);
    bf_stream(_ss << '.' << std::setw(6), min_microseconds_, mask_microseconds_, pos_microseconds_);
    if (daylightsavingstime())
        _ss << " dst";
    return _ss.str();
}

std::string Date::filename() const
{
    std::ostringstream _ss;
    _ss.fill('0');
    bf_stream(_ss << std::setw(4), min_year_, mask_year_, pos_year_);
    bf_stream(_ss << std::setw(2), min_month_, mask_month_, pos_month_);
    bf_stream(_ss << std::setw(2), min_day_, mask_day_, pos_day_);
    bf_stream(_ss << '_' << std::setw(2), min_hour_, mask_hour_, pos_hour_);
    bf_stream(_ss << std::setw(2), min_minute_, mask_minute_, pos_minute_);
    bf_stream(_ss << std::setw(2), min_second_, mask_second_, pos_second_);
    bf_stream(_ss << '_' << std::setw(6), min_microseconds_, mask_microseconds_, pos_microseconds_);
    if (daylightsavingstime())
        _ss << "dst";
    return _ss.str();
}

std::ostream & operator<<(std::ostream & _os, Date const & _in)
{
    return (_os << _in.name());
}

} // namespace tools
} // namespace rpct
