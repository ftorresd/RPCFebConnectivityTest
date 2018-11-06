#ifndef rpct_FebConnectivityTest_FebChip_inl_h
#define rpct_FebConnectivityTest_FebChip_inl_h

#include "rpct/FebConnectivityTest/FebChip.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline hwd::Parameter const & FebChipType::getVThSetParameter() const
{
    if (vth_set_parameter_)
        return *vth_set_parameter_;
    throw std::logic_error("VThSetParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getVMonSetParameter() const
{
    if (vmon_set_parameter_)
        return *vmon_set_parameter_;
    throw std::logic_error("VMonSetParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getAutoCorrectParameter() const
{
    if (auto_correct_parameter_)
        return *auto_correct_parameter_;
    throw std::logic_error("AutoCorrectParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getVThWriteParameter() const
{
    if (vth_write_parameter_)
        return *vth_write_parameter_;
    throw std::logic_error("VThWriteParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getVMonWriteParameter() const
{
    if (vmon_write_parameter_)
        return *vmon_write_parameter_;
    throw std::logic_error("VMonWriteParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getVThReadParameter() const
{
    if (vth_read_parameter_)
        return *vth_read_parameter_;
    throw std::logic_error("VThReadParameter not available yet");
}

inline hwd::Parameter const & FebChipType::getVMonReadParameter() const
{
    if (vmon_read_parameter_)
        return *vmon_read_parameter_;
    throw std::logic_error("VMonReadParameter not available yet");
}

inline int FebChip::getRPCId() const
{
    return rpcid_;
}

inline FebPart const & FebChip::getFebPart() const
{
    if (febpart_)
        return *febpart_;
    throw std::logic_error("FebPart not available yet");
}

inline FebPart & FebChip::getFebPart()
{
    if (febpart_)
        return *febpart_;
    throw std::logic_error("FebPart not available yet");
}

inline std::size_t FebChip::getPosition() const
{
    return position_;
}

inline unsigned int FebChip::getVThSet() const
{
    return vth_set_;
}

inline unsigned int FebChip::getVMonSet() const
{
    return vmon_set_;
}

inline bool FebChip::getAutoCorrect() const
{
    return auto_correct_;
}

inline unsigned int FebChip::getVThWrite() const
{
    return vth_write_;
}

inline unsigned int FebChip::getVMonWrite() const
{
    return vmon_write_;
}

inline unsigned int FebChip::getVThRead() const
{
    return vth_read_;
}

inline unsigned int FebChip::getVMonRead() const
{
    return vmon_read_;
}

#ifndef OFFLINE_ANALYSIS
inline void FebChip::setRpctFebChip(rpct::FebChip & _febchip)
{
    rpct_febchip_ = &_febchip;
}

inline rpct::FebChip const & FebChip::getRpctFebChip() const
{
    if (rpct_febchip_)
        return *rpct_febchip_;
    throw std::logic_error("rpct::FebChip not available yet");
}

inline rpct::FebChip & FebChip::getRpctFebChip()
{
    if (rpct_febchip_)
        return *rpct_febchip_;
    throw std::logic_error("rpct::FebChip not available yet");
}
#endif // OFFLINE_ANALYSIS

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebChip_inl_h
