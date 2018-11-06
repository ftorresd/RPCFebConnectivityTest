#ifndef rpct_hwd_DeviceFlagsMask_inl_h
#define rpct_hwd_DeviceFlagsMask_inl_h

#include "rpct/hwd/DeviceFlagsMask.h"

#include "rpct/hwd/DeviceFlags.h"

namespace rpct {
namespace hwd {

inline unsigned char DeviceFlagsMask::getFlags() const
{
    return flags_;
}

inline void DeviceFlagsMask::setFlags(unsigned char _flags)
{
    flags_ = _flags;
}

inline unsigned char DeviceFlagsMask::getMask() const
{
    return mask_;
}

inline void DeviceFlagsMask::setMask(unsigned char _mask)
{
    mask_ = _mask;
}

inline void DeviceFlagsMask::reset()
{
    flags_ = 0x00;
    mask_  = 0x00;
}

inline void DeviceFlagsMask::requireDisabled(bool _disabled)
{
    require(DeviceFlags::disabled_, _disabled);
}

inline void DeviceFlagsMask::requireBroken(bool _broken)
{
    require(DeviceFlags::broken_, _broken);
}

inline void DeviceFlagsMask::requireWarm(bool _warm)
{
    require(DeviceFlags::warm_, _warm);
}

inline void DeviceFlagsMask::requireConfigured(bool _configured)
{
    require(DeviceFlags::configured_, _configured);
}


inline bool DeviceFlagsMask::includesDisabled() const
{
    return includes(DeviceFlags::disabled_, true);
}

inline bool DeviceFlagsMask::includesEnabled() const
{
    return includes(DeviceFlags::disabled_, false);
}

inline bool DeviceFlagsMask::includesBroken() const
{
    return includes(DeviceFlags::broken_, true);
}

inline bool DeviceFlagsMask::includesWorking() const
{
    return includes(DeviceFlags::broken_, false);
}

inline bool DeviceFlagsMask::includesWarm() const
{
    return includes(DeviceFlags::warm_, true);
}

inline bool DeviceFlagsMask::includesCold() const
{
    return includes(DeviceFlags::warm_, false);
}

inline bool DeviceFlagsMask::includesConfigured() const
{
    return includes(DeviceFlags::configured_, true);
}

inline bool DeviceFlagsMask::includesNotConfigured() const
{
    return includes(DeviceFlags::configured_, false);
}


inline bool DeviceFlagsMask::DeviceFlagsMask::matches(DeviceFlags const & _deviceflags) const
{
    return ((_deviceflags.getFlags() & mask_) == flags_);
}

inline void DeviceFlagsMask::require(unsigned int _field, bool _set)
{
    mask_ |= (0x1 << _field);
    if (_set)
        flags_ |=  (0x1 << _field);
    else
        flags_ &= ~(0x1 << _field);
}

inline bool DeviceFlagsMask::includes(unsigned int _field, bool _set) const
{
    if (!((mask_ & (0x1 << _field))))
        return true;
    else if ((flags_ & (0x1 << _field)))
        return _set;
    else
        return !(_set);
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_DeviceFlagsMask_inl_h
