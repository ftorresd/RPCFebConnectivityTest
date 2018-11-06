#ifndef rpct_FebConnectivityTest_FebBoard_inl_h
#define rpct_FebConnectivityTest_FebBoard_inl_h

#include "rpct/FebConnectivityTest/FebBoard.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline int FebBoard::getRPCId() const
{
    return rpcid_;
}

inline std::string const & FebBoard::getName() const
{
    return name_;
}

inline int FebBoard::getFebType() const
{
    return feb_type_;
}

inline FebDistributionBoard const & FebBoard::getFebDistributionBoard() const
{
    if (febdistributionboard_)
        return *febdistributionboard_;
    throw std::logic_error("FebDistributionBoard not available yet");
}

inline FebDistributionBoard & FebBoard::getFebDistributionBoard()
{
    if (febdistributionboard_)
        return *febdistributionboard_;
    throw std::logic_error("FebDistributionBoard not available yet");
}

inline std::size_t FebBoard::getPosition() const
{
    return position_;
}

inline std::vector<FebPart *> const & FebBoard::getFebParts()
{
    return febparts_;
}

inline FebPart const & FebBoard::getFebPart(std::size_t _position) const
{
    if (_position >= nfebparts_[feb_type_])
        throw std::logic_error("No FebPart available at this position");
    FebPart * _febpart = febparts_.at(_position);
    if (_febpart)
        return *_febpart;
    throw std::out_of_range("No FebPart available at this position");
}

inline FebPart & FebBoard::getFebPart(std::size_t _position)
{
    if (_position >= nfebparts_[feb_type_])
        throw std::logic_error("No FebPart available at this position");
    FebPart * _febpart = febparts_.at(_position);
    if (_febpart)
        return *_febpart;
    throw std::out_of_range("No FebPart available at this position");
}

#ifndef OFFLINE_ANALYSIS
inline void FebBoard::setRpctFebBoard(rpct::FebBoard & _febboard)
{
    rpct_febboard_ = &_febboard;
}

inline rpct::FebBoard const & FebBoard::getRpctFebBoard() const
{
    if (rpct_febboard_)
        return *rpct_febboard_;
    throw std::logic_error("rpct::FebBoard not available yet");
}

inline rpct::FebBoard & FebBoard::getRpctFebBoard()
{
    if (rpct_febboard_)
        return *rpct_febboard_;
    throw std::logic_error("rpct::FebBoard not available yet");
}
#endif // OFFLINE_ANALYSIS

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebBoard_inl_h
