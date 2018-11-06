#include "rpct/FebConnectivityTest/FebConnectorStrips.h"

namespace rpct {
namespace fct {

FebConnectorStrips::FebConnectorStrips(::uint16_t _pins
                                       , int _first_strip
                                       , int _slope)
    : first_strip_(_first_strip > 0 && _first_strip <= 128 ? _first_strip : 0)
    , slope_(_slope < 0 ? 0 : 1)
    , pins_(_pins)
{}

} //namespace fct
} // namespace rpct
