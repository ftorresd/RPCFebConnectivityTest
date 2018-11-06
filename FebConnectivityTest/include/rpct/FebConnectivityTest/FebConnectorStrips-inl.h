/* Filip Thyssen */

#ifndef rpct_FebConnectivityTest_FebConnectorStrips_inl_h_
#define rpct_FebConnectivityTest_FebConnectorStrips_inl_h_

#include "rpct/FebConnectivityTest/FebConnectorStrips.h"

#include "rpct/tools/tools.h"

namespace rpct {
namespace fct {

// getters
inline ::uint16_t FebConnectorStrips::getPins() const
{
    return pins_;
}

inline int FebConnectorStrips::getNPins() const
{
    return npins_;
}

inline int FebConnectorStrips::getFirstStrip() const
{
    return first_strip_;
}

inline int FebConnectorStrips::getSlope() const
{
    return slope_ ? 1 : -1;
}

// derived
inline int FebConnectorStrips::getNStrips() const
{
    return rpct::tools::bit_count(pins_);
}

inline int FebConnectorStrips::getLastStrip() const
{
    return first_strip_ + getSlope() * (getNStrips() - 1);
}

inline bool FebConnectorStrips::isPinActive(int _pin) const
{
    if (_pin > 0 && _pin <= npins_)
        return (pins_ & (0x1 << (_pin - 1)));
    return false;
}

inline int FebConnectorStrips::getStrip(int _pin) const
{
    if (!isPinActive(_pin))
        return -1;
    int _active_pins = rpct::tools::bit_count(::uint16_t(pins_ & ((0x1 << (_pin - 1)) - 1)));
    return first_strip_ + getSlope() * _active_pins;
}

inline bool FebConnectorStrips::hasStrip(int _strip) const
{
    _strip = getSlope() * (_strip - first_strip_);
    return (_strip >=0 && _strip < getNStrips());
}

inline int FebConnectorStrips::getPin(int _strip) const
{
    _strip = getSlope() * (_strip - first_strip_);
    if (_strip < 0)
        return -1;
    for (int _pin = 1 ; _pin <= npins_ ; ++_pin)
        if (pins_ & (0x1 << (_pin - 1)))
            {
                if (!_strip)
                    return _pin;
                --_strip;
            }
    return -1;
}


// setters
inline void FebConnectorStrips::setPins(::uint16_t _pins)
{
    pins_ = _pins;
}

inline void FebConnectorStrips::setFirstStrip(int _first_strip)
{
    first_strip_ = (_first_strip > 0 && _first_strip <= 128 ? _first_strip : 0);
}

inline void FebConnectorStrips::setSlope(int _slope)
{
    slope_ = (_slope < 0 ? 0 : 1);
}

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebConnectorStrips_inl_h_
