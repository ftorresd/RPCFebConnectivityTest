#ifndef rpct_FebConnectivityTest_FebConnectivityTest_h
#define rpct_FebConnectivityTest_FebConnectivityTest_h

#include "rpct/tools/Time.h"
#include "rpct/tools/RollId.h"
#include "rpct/tools/RollSelection.h"

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"
#include "rpct/hwd/DeviceType.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <string>

namespace rpct {
namespace fct {

class FebConnectivityTest
    : public hwd::Device
{
public:
    FebConnectivityTest(hwd::System & _system
                        , hwd::integer_type _id
                        , hwd::DeviceType const & _devicetype
                        , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static FebConnectivityTest & registerDevice(hwd::System & _system
                                                , rpct::tools::RollSelection const &  _roll_selection
                                                , rpct::tools::Time const & _count_duration
                                                , unsigned int _vth_low, unsigned int _vth_high
                                                , unsigned int _vmon
                                                , bool _auto_correct
                                                , bool _use_pulser
                                                , bool _include_disabled
                                                , std::string const & _snapshot_name);

    rpct::tools::RollSelection const & getRollSelection() const;
    rpct::tools::Time const & getCountDuration() const;

    unsigned int getVThLow() const;
    unsigned int getVThHigh() const;
    unsigned int getVMon() const;
    FebConnector const & getActiveFebConnector() const;
    FebConnector & getActiveFebConnector();
    rpct::hwd::integer_type getActiveFebConnectorId() const;

    bool getAutoCorrect() const;
    bool getUsePulser() const;
    bool getIncludeDisabled() const;

    std::string const & getSnapshotName() const;

protected:
    void iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask);
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    rpct::tools::RollSelection roll_selection_;
    rpct::tools::Time count_duration_;

    unsigned int vth_low_, vth_high_;
    unsigned int vmon_;
    rpct::hwd::integer_type active_febconnector_id_;

    bool auto_correct_;
    bool use_pulser_;
    bool include_disabled_;

    std::string snapshot_name_;
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebConnectivityTest-inl.h"

#endif // rpct_FebConnectivityTest_FebConnectivityTest_h
