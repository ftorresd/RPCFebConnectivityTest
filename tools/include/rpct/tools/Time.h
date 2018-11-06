/* Filip Thyssen */

#ifndef rpct_tools_Time_h_
#define rpct_tools_Time_h_

#include <stdint.h>
#include <time.h> // ::timespec, ::nanosleep, ::tm
#include <sys/time.h> // ::timeval

#include <iosfwd>

namespace rpct {
namespace tools {

/** Time class
 * Can be used to represent durations or absolute times
 */
class Time
{
public:
    static ::int64_t const nanoseconds_per_second_      = 1000000000;
    static ::int64_t const nanoseconds_per_millisecond_ = 1000000;
    static ::int64_t const nanoseconds_per_microsecond_ = 1000;
    static ::int64_t const seconds_per_microsecond_     = 1000000;

protected:
    void validate_input();
    void validate_sign();
    void validate_operation();
    Time(::int64_t _seconds, ::int64_t _nanoseconds
         , bool _validate);

public:
    /** Construct the Time to contain the moment of calling */
    Time();
    Time(::int64_t _seconds, ::int64_t _nanoseconds = 0);
    Time(::timespec const & _ts);
    Time(::timeval const & _tv);
    Time(::tm const & _tm);

    /** Create a Time using the ustime format
     *\param _ustime 10^6*seconds + microseconds
     */
    static Time ustime(::int64_t _ustime);

    /** Set the time to the moment of calling */
    void set();
    /** Set the time
     * \param _seconds seconds
     * \param _nanoseconds nanoseconds
     */
    void set(::int64_t _seconds, ::int64_t _nanoseconds = 0);

    void seconds(::int64_t _seconds);
    void nanoseconds(::int64_t _nanoseconds);

    ::int64_t seconds() const;
    ::int64_t nanoseconds() const;
    ::timespec timespec() const;
    ::timeval timeval() const;
    /** Get the time in the ustime format:
     * 10^6*seconds + microseconds
     * useful for Database storage
     */
    ::int64_t ustime() const;
    operator double() const;

    /** sleep for the duration of time
     * \return false if a signal interrupted
     */
    bool sleep() const;

    /**
     * Basic arithmetic operators
     * @{
     */
    Time & operator+=(Time const & _rh);
    Time   operator+ (Time const & _rh) const;
    Time   operator+ () const;
    Time & operator-=(Time const & _rh);
    Time   operator- (Time const & _rh) const;
    Time   operator- () const;
    template<typename t_rh_type>
    Time & operator*=(t_rh_type const & _rh);
    template<typename t_rh_type>
    Time   operator* (t_rh_type const & _rh) const;
    /**@}*/

    bool operator==(Time const & _rh) const;
    bool operator<(Time const & _rh) const;
    bool operator>(Time const & _rh) const;

protected:
    ::int64_t seconds_;
    ::int64_t nanoseconds_;
};

std::ostream & operator<<(std::ostream & _os, Time const & _time);

} // namespace tools
} // namespace rpct

#include "rpct/tools/Time-inl.h"

#endif // rpct_tools_Time_h_
