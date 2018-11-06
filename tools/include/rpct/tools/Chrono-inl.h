/* Filip Thyssen */

#ifndef rpct_tools_Chrono_inl_h_
#define rpct_tools_Chrono_inl_h_

#include "rpct/tools/Chrono.h"

namespace rpct {
namespace tools {

inline void Chrono::set()
{
    reference_.set();
    if (pause_)
        intermediate_.set(reference_);
}

inline void Chrono::set(Time const & _time)
{
    pause_ = false;
    reference_ = _time;
}

inline void Chrono::set(::int64_t _seconds, ::int32_t _nanoseconds)
{
    pause_ = false;
    reference_.set(_seconds, _nanoseconds);
}

inline Time const & Chrono::get() const
{
    return reference_;
}

inline void Chrono::reset()
{
    pause_ = false;
    reference_.set();
}

inline bool Chrono::pause()
{
    if (pause_)
        return false;
    pause_ = true;
    intermediate_.set();
    return true;
}

inline bool Chrono::resume()
{
    if (!pause_)
        return false;
    pause_ = false;
    reference_ += (Time() - intermediate_);
    return true;
}

inline Time Chrono::duration() const
{
    if (pause_)
        return (intermediate_ - reference_);
    return (Time() - reference_);
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_Chrono_inl_h_
