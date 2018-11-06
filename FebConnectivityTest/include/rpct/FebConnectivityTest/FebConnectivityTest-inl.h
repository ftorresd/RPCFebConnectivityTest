#ifndef rpct_FebConnectivityTest_FebConnectivityTest_inl_h
#define rpct_FebConnectivityTest_FebConnectivityTest_inl_h

#include "rpct/FebConnectivityTest/FebConnectivityTest.h"

namespace rpct {
namespace fct {

inline rpct::tools::RollSelection const & FebConnectivityTest::getRollSelection() const
{
    return roll_selection_;
}

inline rpct::tools::Time const & FebConnectivityTest::getCountDuration() const
{
    return count_duration_;
}

inline unsigned int FebConnectivityTest::getVThLow() const
{
    return vth_low_;
}

inline unsigned int FebConnectivityTest::getVThHigh() const
{
    return vth_high_;
}

inline unsigned int FebConnectivityTest::getVMon() const
{
    return vmon_;
}

inline rpct::hwd::integer_type FebConnectivityTest::getActiveFebConnectorId() const
{
    return active_febconnector_id_;
}

inline bool FebConnectivityTest::getAutoCorrect() const
{
    return auto_correct_;
}

inline bool FebConnectivityTest::getUsePulser() const
{
    return use_pulser_;
}

inline bool FebConnectivityTest::getIncludeDisabled() const
{
    return include_disabled_;
}

inline std::string const & FebConnectivityTest::getSnapshotName() const
{
    return snapshot_name_;
}

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebConnectivityTest_inl_h
