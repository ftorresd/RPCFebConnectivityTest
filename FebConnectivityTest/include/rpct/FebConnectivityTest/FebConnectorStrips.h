/* Filip Thyssen */

#ifndef rpct_FebConnectivityTest_FebConnectorStrips_h_
#define rpct_FebConnectivityTest_FebConnectorStrips_h_

#include <stdint.h>

namespace rpct {
namespace fct {

class FebConnectorStrips
{
public:
    static const int npins_ = 16;

public:
    FebConnectorStrips(::uint16_t _pins = 0xffff
                       , int _first_strip = 1
                       , int _slope = 1);

    // getters
    ::uint16_t getPins() const;
    int getNPins() const;
    int getFirstStrip() const;
    int getSlope() const;

    // derived
    int  getNStrips() const;
    int  getLastStrip() const;
    bool isPinActive(int _pin) const;
    int  getStrip(int _pin) const;
    bool hasStrip(int _strip) const;
    int  getPin(int _strip) const;

    // setters
    void setPins(::uint16_t _pins);
    void setFirstStrip(int _first_strip);
    void setSlope(int _slope);

protected:
    unsigned char first_strip_; ///< strip in range [1-89] (allowing [1-128])
    unsigned char slope_;       ///< 0 for negative, 1 for positive
    ::uint16_t pins_;             ///< active pins in range [1-16]
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebConnectorStrips-inl.h"

#endif // rpct_FebConnectivityTest_FebConnectorStrips_h_
