#ifndef rpct_hwd_DeviceFlags_inl_h
#define rpct_hwd_DeviceFlags_inl_h

#include "rpct/hwd/DeviceFlags.h"

namespace rpct {
namespace hwd {

inline unsigned char DeviceFlags::getFlags() const
{
    return flags_;
}

inline void DeviceFlags::setFlags(unsigned char _flags)
{
    flags_ = _flags;
}

inline void DeviceFlags::reset()
{
    flags_ = 0x00;
}

inline bool DeviceFlags::isDisabled() const
{
    return is(disabled_);
}

inline bool DeviceFlags::isBroken() const
{
    return is(broken_);
}

inline bool DeviceFlags::isWarm() const
{
    return is(warm_);
}

inline bool DeviceFlags::isConfigured() const
{
    return is(configured_);
}


inline void DeviceFlags::setDisabled(bool _disabled)
{
    set(disabled_, _disabled);
}

inline void DeviceFlags::setBroken(bool _broken)
{
    set(broken_, _broken);
}

inline void DeviceFlags::setWarm(bool _warm)
{
    set(warm_, _warm);
}

inline void DeviceFlags::setConfigured(bool _configured)
{
    set(configured_, _configured);
}


inline bool DeviceFlags::is(unsigned int _field) const
{
    return (flags_ & (0x1 << _field));
}

inline void DeviceFlags::set(unsigned int _field, bool _set)
{
    if (_set)
        flags_ |=  (0x1 << _field);
    else
        flags_ &= ~(0x1 << _field);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DeviceFlags_inl_h
