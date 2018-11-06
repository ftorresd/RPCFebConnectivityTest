#ifndef rpct_FebConnectivityTest_FebChip_h
#define rpct_FebConnectivityTest_FebChip_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"
#include "rpct/hwd/DeviceType.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class FebChip;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class FebChipType
    : public hwd::DeviceType
{
public:
    FebChipType(hwd::System & _system
                , hwd::integer_type _id
                , std::string const & _name = "FebChip");

    static FebChipType & registerDeviceType(hwd::System & _system);

    hwd::Parameter const & getVThSetParameter() const;
    hwd::Parameter const & getVMonSetParameter() const;
    hwd::Parameter const & getAutoCorrectParameter() const;

    hwd::Parameter const & getVThWriteParameter() const;
    hwd::Parameter const & getVMonWriteParameter() const;

    hwd::Parameter const & getVThReadParameter() const;
    hwd::Parameter const & getVMonReadParameter() const;

protected:
    hwd::Parameter const & addParameter(hwd::integer_type _id
                                        , hwd::ParameterType const & _parametertype
                                        , hwd::integer_type _position = 0
                                        , unsigned char _properties = 0x0);

protected:
    hwd::Parameter const * vth_set_parameter_, * vmon_set_parameter_, * auto_correct_parameter_;
    hwd::Parameter const * vth_write_parameter_, * vmon_write_parameter_;
    hwd::Parameter const * vth_read_parameter_, * vmon_read_parameter_;
};

class FebChip
    : public hwd::Device
{
public:
    FebChip(hwd::System & _system
            , hwd::integer_type _id
            , hwd::DeviceType const & _devicetype
            , hwd::DeviceConfiguration const & _properties);

    static FebChipType & registerDeviceType(hwd::System & _system);
    static FebChip & registerDevice(hwd::System & _system
                                    , int _rpcid
                                    , hwd::DeviceFlags const & _deviceflags
                                    , FebPart & _febpart
                                    , std::size_t _position);

    int getRPCId() const;

    FebPart const & getFebPart() const;
    FebPart & getFebPart();
    std::size_t getPosition() const;

    unsigned int getVThSet() const;
    unsigned int getVMonSet() const;
    bool getAutoCorrect() const;

    unsigned int getVThWrite() const;
    unsigned int getVMonWrite() const;
    unsigned int getVThRead() const;
    unsigned int getVMonRead() const;

#ifndef OFFLINE_ANALYSIS
    void setRpctFebChip(rpct::FebChip & _febchip);
    rpct::FebChip const & getRpctFebChip() const;
    rpct::FebChip & getRpctFebChip();
#endif // OFFLINE_ANALYSIS

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    int rpcid_;

    FebPart * febpart_;
    std::size_t position_;

    FebChipType const & febchip_type_;
    unsigned int vth_set_, vmon_set_;
    bool auto_correct_;
    unsigned int vth_write_, vmon_write_;
    unsigned int vth_read_, vmon_read_;

#ifndef OFFLINE_ANALYSIS
    rpct::FebChip * rpct_febchip_;
#endif // OFFLINE_ANALYSIS
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebChip-inl.h"

#endif // rpct_FebConnectivityTest_FebChip_h
