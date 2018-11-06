#ifndef rpct_FebConnectivityTest_FebPart_h
#define rpct_FebConnectivityTest_FebPart_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"
#include "rpct/hwd/DeviceType.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <vector>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class FebPart;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class FebPartType
    : public hwd::DeviceType
{
public:
    FebPartType(hwd::System & _system
                , hwd::integer_type _id
                , std::string const & _name = "FebPart");

    static FebPartType & registerDeviceType(hwd::System & _system);

    hwd::Parameter const & getTemperatureParameter() const;

protected:
    hwd::Parameter const & addParameter(hwd::integer_type _id
                                        , hwd::ParameterType const & _parametertype
                                        , hwd::integer_type _position = 0
                                        , unsigned char _properties = 0x0);

protected:
    hwd::Parameter const * temperature_parameter_;
};

class FebPart
    : public hwd::Device
{
public:
    static std::size_t const nfebchips_ = 2;

public:
    FebPart(hwd::System & _system
            , hwd::integer_type _id
            , hwd::DeviceType const & _devicetype
            , hwd::DeviceConfiguration const & _properties);

    static FebPartType & registerDeviceType(hwd::System & _system);
    static FebPart & registerDevice(hwd::System & _system
                                    , FebBoard & _febboard
                                    , std::size_t _position);

    FebBoard const & getFebBoard() const;
    FebBoard & getFebBoard();
    std::size_t getPosition() const;

    FebConnector const & getFebConnector() const;
    FebConnector & getFebConnector();

    std::vector<FebChip *> const & getFebChips();
    FebChip const & getFebChip(std::size_t _position) const;
    FebChip & getFebChip(std::size_t _position);

    float getTemperature() const;

#ifndef OFFLINE_ANALYSIS
    void setRpctFebPart(rpct::FebPart & _febpart);
    rpct::FebPart const & getRpctFebPart() const;
    rpct::FebPart & getRpctFebPart();
#endif // OFFLINE_ANALYSIS

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    FebBoard * febboard_;
    std::size_t position_;
    std::vector<FebChip *> febchips_;
    FebConnector * febconnector_;

    FebPartType const & febpart_type_;
    float temperature_;

#ifndef OFFLINE_ANALYSIS
    rpct::FebPart * rpct_febpart_;
#endif // OFFLINE_ANALYSIS
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebPart-inl.h"

#endif // rpct_FebConnectivityTest_FebPart_h
