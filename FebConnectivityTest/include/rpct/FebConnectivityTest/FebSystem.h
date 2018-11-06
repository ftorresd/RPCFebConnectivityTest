#ifndef rpct_FebConnectivityTest_FebSystem_h
#define rpct_FebConnectivityTest_FebSystem_h

#include <string>
#include <vector>

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/System.h"

#include "rpct/FebConnectivityTest/fwd.h"

namespace rpct {
namespace fct {

class FebSystem
    : public hwd::System
{
public:
    FebSystem(hwd::HardwareStorage & _hardwarestorage
              , std::string const & _name = std::string("FebSystem"));

    std::string const & getTower() const;

    std::vector<LinkBox *> getLinkBoxes();

    FebConnectivityTest & getFebConnectivityTest();
    FebThresholdScan & getFebThresholdScan();

    void registerSystem(std::string const & _tower);
    void loadSystem();

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);

    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    std::string tower_;
    hwd::DeviceType const * linkbox_type_
        , * febconnectivitytest_type_
        , * febthresholdscan_type_;
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebSystem-inl.h"

#endif // rpct_FebConnectivityTest_FebSystem_h
