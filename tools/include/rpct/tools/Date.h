/* Filip Thyssen */

#ifndef rpct_tools_Date_h
#define rpct_tools_Date_h

#include <stdint.h>
#include <string>
#include <iosfwd>

#include <time.h> // ::timespec, ::nanosleep, ::tm
#include <sys/time.h> // ::timeval

namespace rpct {
namespace tools {

class Time;

class Date
{
public:
    /** \name Field Ranges
     * @{*/
    static const int min_year_ = -32767;
    static const int max_year_ = 32767;
    static const int min_month_ = 1;
    static const int max_month_ = 12;
    static const int min_day_ = 1;
    static const int max_day_ = 31;
    static const int min_hour_ = 0;
    static const int max_hour_ = 23;
    static const int min_minute_ = 0;
    static const int max_minute_ = 59;
    static const int min_second_ = 0;
    static const int max_second_ = 61;
    static const int min_microseconds_ = 0;
    static const int max_microseconds_ = 999999;
    static const int min_daylightsavingstime_ = 0;
    static const int max_daylightsavingstime_ = 1;
    /** @} */

protected:
    /** \name Field Positions and Masks
     * @{ */
    static const int pos_year_ = 47;
    static const ::uint64_t mask_year_ = 0x7fff800000000000;
    static const int pos_month_ = 43;
    static const ::uint64_t mask_month_ = 0x0000780000000000;
    static const int pos_day_ = 38;
    static const ::uint64_t mask_day_ = 0x000007c000000000;
    static const int pos_hour_ = 33;
    static const ::uint64_t mask_hour_ = 0x0000003e00000000;
    static const int pos_minute_ = 27;
    static const ::uint64_t mask_minute_ = 0x00000001f8000000;
    static const int pos_second_ = 21;
    static const ::uint64_t mask_second_ = 0x0000000007e00000;
    static const int pos_microseconds_ = 1;
    static const ::uint64_t mask_microseconds_ = 0x00000000001ffffe;
    static const int pos_daylightsavingstime_ = 0;
    static const ::uint64_t mask_daylightsavingstime_ = 0x0000000000000001;
    /** @} */

public:
    /** Construct the Date to contain the moment of calling */
    Date();
    Date(::uint64_t const & _id);
    Date(int _year
         , int _month
         , int _day = 1
         , int _hour = 0
         , int _minute = 0
         , int _second = 0
         , int _microseconds = 0
         , int _daylightsavingstime = 0);

    /** Construct the Date
     * \param _time a time
     */
    Date(Time const & _time);

    /** Set the Date to moment of calling */
    void set();
    /** Set the Date
     * \param Time const & _time
     */
    void set(Time const & _time);

    ::uint64_t id() const;
    operator ::uint64_t() const;

    void id(::uint64_t const & _in);

    Time time() const;

    /** \name Field Getters
     * @{*/
    int year() const;
    int month() const;
    int day() const;
    int hour() const;
    int minute() const;
    int second() const;
    int microseconds() const;
    int daylightsavingstime() const;
    /** @} */

    /** \name Field Setters
     * A std::out_of_range is thrown for out-of-range input values.
     * @{ */
    void setYear(int _year = 0);
    void setMonth(int _month = 1);
    void setDay(int _day = 1);
    void setHour(int _hour = 0);
    void setMinute(int _minute = 0);
    void setSecond(int _second = 0);
    void setMicroSeconds(int _microseconds = 0);
    void setDaylightSavingsTime(int _daylightsavingstime = 0);
    /** @} */

    std::string name() const;

    std::string filename() const;

    bool operator<(Date const & _in) const;
    bool operator==(Date const & _in) const;
    bool operator!=(Date const & _in) const;
    bool operator<(::uint64_t const & _in) const;
    bool operator==(::uint64_t const & _in) const;
    bool operator!=(::uint64_t const & _in) const;

protected:
    int bf_get(int const _min, ::uint64_t const _mask, int const _pos) const;
    void bf_set(int const _min, int const _max, ::uint64_t const _mask, int const _pos, int const _value);
    void bf_stream(std::ostream & _os, int const _min, ::uint64_t const _mask, int const _pos) const;

protected:
    ::uint64_t id_;
};

std::ostream & operator<<(std::ostream & _os, Date const & _in);

} // namespace tools
} // namespace rpct

#include "rpct/tools/Date-inl.h"

#endif // rpct_tools_Date_h
