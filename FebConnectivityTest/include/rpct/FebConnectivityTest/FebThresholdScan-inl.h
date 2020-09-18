#ifndef rpct_FebConnectivityTest_FebThresholdScan_inl_h
#define rpct_FebConnectivityTest_FebThresholdScan_inl_h

#include "rpct/FebConnectivityTest/FebThresholdScan.h"

namespace rpct
{
    namespace fct
    {

        inline rpct::tools::RollSelection const &FebThresholdScan::getRollSelection() const
        {
            return roll_selection_;
        }

        inline rpct::tools::Time const &FebThresholdScan::getCountDuration() const
        {
            return count_duration_;
        }

        inline unsigned int FebThresholdScan::getVThMin() const
        {
            return vth_min_;
        }

        inline unsigned int FebThresholdScan::getVThNSteps() const
        {
            return vth_n_steps_;
        }

        inline unsigned int FebThresholdScan::getVThStepSize() const
        {
            return vth_step_size_;
        }

        inline unsigned int FebThresholdScan::getVMon() const
        {
            return vmon_;
        }

        inline unsigned int FebThresholdScan::getActiveVTh() const
        {
            return active_vth_;
        }

        inline bool FebThresholdScan::getAutoCorrect() const
        {
            return auto_correct_;
        }

        inline bool FebThresholdScan::getUsePulser() const
        {
            return use_pulser_;
        }

        inline bool FebThresholdScan::getIncludeDisabled() const
        {
            return include_disabled_;
        }

        inline std::string const &FebThresholdScan::getSnapshotName() const
        {
            return snapshot_name_;
        }

    } // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebThresholdScan_inl_h
