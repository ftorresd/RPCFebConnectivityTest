#ifndef rpct_FebConnectivityTest_FebPart_inl_h
#define rpct_FebConnectivityTest_FebPart_inl_h

#include "rpct/FebConnectivityTest/FebPart.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline hwd::Parameter const & FebPartType::getTemperatureParameter() const
{
    if (temperature_parameter_)
        return *temperature_parameter_;
    throw std::logic_error("TemperatureParameter not available yet");
}

inline FebBoard const & FebPart::getFebBoard() const
{
    if (febboard_)
        return *febboard_;
    throw std::logic_error("FebBoard not available yet");
}

inline FebBoard & FebPart::getFebBoard()
{
    if (febboard_)
        return *febboard_;
    throw std::logic_error("FebBoard not available yet");
}

inline std::size_t FebPart::getPosition() const
{
    return position_;
}

inline FebConnector const & FebPart::getFebConnector() const
{
    if (febconnector_)
        return *febconnector_;
    throw std::logic_error("FebConnector not available yet");
}

inline FebConnector & FebPart::getFebConnector()
{
    if (febconnector_)
        return *febconnector_;
    throw std::logic_error("FebConnector not available yet");
}

inline std::vector<FebChip *> const & FebPart::getFebChips()
{
    return febchips_;
}

inline FebChip const & FebPart::getFebChip(std::size_t _position) const
{
    if (_position >= nfebchips_)
        throw std::logic_error("No FebChip available at this position");
    FebChip * _febchip = febchips_.at(_position);
    if (_febchip)
        return *_febchip;
    throw std::out_of_range("No FebChip available at this position");
}

inline FebChip & FebPart::getFebChip(std::size_t _position)
{
    if (_position >= nfebchips_)
        throw std::logic_error("No FebChip available at this position");
    FebChip * _febchip = febchips_.at(_position);
    if (_febchip)
        return *_febchip;
    throw std::out_of_range("No FebChip available at this position");
}

inline float FebPart::getTemperature() const
{
    return temperature_;
}

#ifndef OFFLINE_ANALYSIS
inline void FebPart::setRpctFebPart(rpct::FebPart & _febpart)
{
    rpct_febpart_ = &_febpart;
}

inline rpct::FebPart const & FebPart::getRpctFebPart() const
{
    if (rpct_febpart_)
        return *rpct_febpart_;
    throw std::logic_error("rpct::FebPart not available yet");
}

inline rpct::FebPart & FebPart::getRpctFebPart()
{
    if (rpct_febpart_)
        return *rpct_febpart_;
    throw std::logic_error("rpct::FebPart not available yet");
}
#endif // OFFLINE_ANALYSIS

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebPart_inl_h
