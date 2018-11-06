#ifndef rpct_FebConnectivityTest_FebBoard_h
#define rpct_FebConnectivityTest_FebBoard_h

#include "log4cplus/logger.h"

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <string>
#include <vector>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class FebBoard;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class FebBoard
    : public hwd::Device
{
public:
    static std::size_t const nfebparts_[2];
    static std::size_t const max_nfebparts_ = 2;

protected:
    static log4cplus::Logger logger_;

public:
    FebBoard(hwd::System & _system
             , hwd::integer_type _id
             , hwd::DeviceType const & _devicetype
             , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static FebBoard & registerDevice(hwd::System & _system
                                     , int _rpcid
                                     , hwd::DeviceFlags const & _deviceflags
                                     , std::string const & _name
                                     , int _feb_type
                                     , FebDistributionBoard & _febdistributionboard
                                     , std::size_t _position);

    int getRPCId() const;
    std::string const & getName() const;
    int getFebType() const;

    FebDistributionBoard const & getFebDistributionBoard() const;
    FebDistributionBoard & getFebDistributionBoard();
    std::size_t getPosition() const;

    std::vector<FebPart *> const & getFebParts();
    FebPart const & getFebPart(std::size_t _position) const;
    FebPart & getFebPart(std::size_t _position);

    void addConnection(hwd::Connection & _connection);

#ifndef OFFLINE_ANALYSIS
    void setRpctFebBoard(rpct::FebBoard & _febboard);
    rpct::FebBoard const & getRpctFebBoard() const;
    rpct::FebBoard & getRpctFebBoard();
#endif // OFFLINE_ANALYSIS

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);

protected:
    int rpcid_;
    std::string name_;
    int feb_type_;

    FebDistributionBoard * febdistributionboard_;
    std::size_t position_;
    std::vector<FebPart *> febparts_;

#ifndef OFFLINE_ANALYSIS
    rpct::FebBoard * rpct_febboard_;

    FebPartType const & febpart_type_;
    FebChipType const & febchip_type_;
#endif // OFFLINE_ANALYSIS
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebBoard-inl.h"

#endif // rpct_FebConnectivityTest_FebBoard_h
