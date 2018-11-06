#ifndef rpct_FebConnectivityTest_FebDistributionBoard_h
#define rpct_FebConnectivityTest_FebDistributionBoard_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <vector>

namespace rpct {
namespace fct {

class FebDistributionBoard
    : public hwd::Device
{
public:
    static std::size_t const nfebboards_ = 8;

public:
    FebDistributionBoard(hwd::System & _system
                         , hwd::integer_type _id
                         , hwd::DeviceType const & _devicetype
                         , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static FebDistributionBoard & registerDevice(hwd::System & _system
                                                 , ControlBoard & _controlboard
                                                 , std::size_t _position);

    ControlBoard const & getControlBoard() const;
    ControlBoard & getControlBoard();
    std::size_t getPosition() const;

    std::vector<FebBoard *> const & getFebBoards();
    FebBoard const & getFebBoard(std::size_t _position) const;
    FebBoard & getFebBoard(std::size_t _position);

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);

protected:
    ControlBoard * controlboard_;
    std::size_t position_;
    std::vector<FebBoard *> febboards_;
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebDistributionBoard-inl.h"

#endif // rpct_FebConnectivityTest_FebDistributionBoard_h
