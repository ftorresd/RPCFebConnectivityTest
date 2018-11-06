#ifndef rpct_hwd_DeviceFlagsMask_h
#define rpct_hwd_DeviceFlagsMask_h

namespace rpct {
namespace hwd {

class DeviceFlags;

class DeviceFlagsMask
{
public:
    static DeviceFlagsMask const default_;
    static DeviceFlagsMask const any_;
    static DeviceFlagsMask const not_disabled_;

public:
    /** Constructor defaults to not disabled, not broken, not warm and not configured */
    DeviceFlagsMask(unsigned char _flags = 0x00
                    , unsigned char _mask = 0x00);
    virtual ~DeviceFlagsMask();

    unsigned char getFlags() const;
    virtual void setFlags(unsigned char _flags);
    unsigned char getMask() const;
    virtual void setMask(unsigned char _mask);

    virtual void reset();

    void requireDisabled(bool _disabled);
    void requireBroken(bool _broken);
    void requireWarm(bool _warm);
    void requireConfigured(bool _configured);

    bool includesDisabled() const;
    bool includesEnabled() const;
    bool includesBroken() const;
    bool includesWorking() const;
    bool includesWarm() const;
    bool includesCold() const;
    bool includesConfigured() const;
    bool includesNotConfigured() const;

    bool matches(DeviceFlags const & _deviceflags) const;

protected:
    void require(unsigned int _field, bool _set);
    bool includes(unsigned int _field, bool _set) const;

protected:
    unsigned char flags_;
    unsigned char mask_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DeviceFlagsMask-inl.h"

#endif // rpct_hwd_DeviceFlagsMask_h
