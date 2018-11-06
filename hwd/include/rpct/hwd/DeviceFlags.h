#ifndef rpct_hwd_DeviceFlags_h
#define rpct_hwd_DeviceFlags_h

#include <string>

namespace rpct {
namespace hwd {

class DeviceFlags
{
public:
    static std::string const parametertype_name_;

    static DeviceFlags const default_;

    static unsigned int const disabled_   = 0;
    static unsigned int const broken_     = 1;
    static unsigned int const warm_       = 2;
    static unsigned int const configured_ = 3;

public:
    /** Constructor defaults to not disabled, not broken, not warm and not configured */
    DeviceFlags(unsigned char _flags = 0x00);
    virtual ~DeviceFlags();

    unsigned char getFlags() const;

    virtual void setFlags(unsigned char _flags);
    virtual void reset();

    bool isDisabled() const;
    bool isBroken() const;
    bool isWarm() const;
    bool isConfigured() const;

    void setDisabled(bool _disabled = true);
    void setBroken(bool _broken = true);
    void setWarm(bool _warm = true);
    void setConfigured(bool _configured = true);

protected:
    bool is(unsigned int _field) const;
    void set(unsigned int _field, bool _set);

protected:
    unsigned char flags_;
};

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/DeviceFlags-inl.h"

#endif // rpct_hwd_DeviceFlags_h
