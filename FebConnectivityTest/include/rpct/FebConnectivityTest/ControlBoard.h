#ifndef rpct_FebConnectivityTest_ControlBoard_h
#define rpct_FebConnectivityTest_ControlBoard_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <string>
#include <vector>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class ControlBoard;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class ControlBoard
    : public hwd::Device
{
public:
    static std::size_t const nfebdistributionboards_ = 8;

public:
    ControlBoard(hwd::System & _system
                 , hwd::integer_type _id
                 , hwd::DeviceType const & _devicetype
                 , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static ControlBoard & registerDevice(hwd::System & _system
                                         , int _rpcid
                                         , hwd::DeviceFlags const & _deviceflags
                                         , std::string const & _name
                                         , LinkBox & _linkbox
                                         , std::size_t _position);

    int getRPCId() const;
    std::string const & getName() const;

    LinkBox const & getLinkBox() const;
    LinkBox & getLinkBox();
    std::size_t getPosition() const;

    std::vector<FebDistributionBoard *> const & getFebDistributionBoards();
    FebDistributionBoard const & getFebDistributionBoard(std::size_t _position) const;
    FebDistributionBoard & getFebDistributionBoard(std::size_t _position);

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);

protected:
    int rpcid_;
    std::string name_;

    LinkBox * linkbox_;
    std::size_t position_;
    std::vector<FebDistributionBoard *> febdistributionboards_;
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/ControlBoard-inl.h"

#endif // rpct_FebConnectivityTest_ControlBoard_h
