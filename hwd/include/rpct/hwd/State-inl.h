#ifndef rpct_hwd_State_inl_h
#define rpct_hwd_State_inl_h

#include "rpct/hwd/State.h"

#include <stdexcept>

namespace rpct {
namespace hwd {

inline unsigned char State::getState() const
{
    return state_;
}

inline void State::setState(State const & _state)
{
    state_ = _state.getState();
}

inline void State::setState(unsigned char _state)
{
    if (_state <= max_)
        state_ = _state;
    else
        throw std::logic_error("Attempt to set State to invalid level.");
}

inline void State::reset()
{
    setState(default_);
}

inline std::string const & State::getDescription() const
{
    return descriptions_[state_];
}

inline std::string const & State::getColor() const
{
    return colors_[state_];
}

inline std::string const & State::getBackgroundColor() const
{
    return bgcolors_[state_];
}

inline void State::setOk()
{
    setState(ok_);
}

inline bool State::isOk() const
{
    return (state_ == ok_);
}

inline void State::setNotSet()
{
    setState(notset_);
}

inline bool State::isNotSet() const
{
    return (state_ == notset_);
}

inline void State::setInfo()
{
    setState(info_);
}

inline bool State::isInfo() const
{
    return (state_ == info_);
}

inline void State::setWarn()
{
    setState(warn_);
}

inline bool State::isWarn() const
{
    return (state_ == warn_);
}

inline void State::setError()
{
    setState(error_);
}

inline bool State::isError() const
{
    return (state_ == error_);
}

inline bool State::operator< (State const & _state)
{
    return (state_ < _state.getState());
}

inline bool State::operator==(State const & _state)
{
    return (state_ == _state.getState());
}

inline bool State::operator!=(State const & _state)
{
    return (state_ != _state.getState());
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_State_inl_h
