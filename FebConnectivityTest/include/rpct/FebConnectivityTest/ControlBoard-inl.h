#ifndef rpct_FebConnectivityTest_ControlBoard_inl_h
#define rpct_FebConnectivityTest_ControlBoard_inl_h

#include "rpct/FebConnectivityTest/ControlBoard.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline int ControlBoard::getRPCId() const
{
    return rpcid_;
}

inline std::string const & ControlBoard::getName() const
{
    return name_;
}

inline LinkBox const & ControlBoard::getLinkBox() const
{
    if (linkbox_)
        return *linkbox_;
    throw std::logic_error("LinkBox not available yet");
}

inline LinkBox & ControlBoard::getLinkBox()
{
    if (linkbox_)
        return *linkbox_;
    throw std::logic_error("LinkBox not available yet");
}

inline std::size_t ControlBoard::getPosition() const
{
    return position_;
}

inline std::vector<FebDistributionBoard *> const & ControlBoard::getFebDistributionBoards()
{
    return febdistributionboards_;
}

inline FebDistributionBoard const & ControlBoard::getFebDistributionBoard(std::size_t _position) const
{
    if (_position < 1 || _position > nfebdistributionboards_)
        throw std::logic_error("No FebDistributionBoard available at this position");
    FebDistributionBoard * _febdistributionboard = febdistributionboards_.at(_position - 1);
    if (_febdistributionboard)
        return *_febdistributionboard;
    throw std::out_of_range("No FebDistributionBoard available at this position");
}

inline FebDistributionBoard & ControlBoard::getFebDistributionBoard(std::size_t _position)
{
    if (_position < 1 || _position > nfebdistributionboards_)
        throw std::logic_error("No FebDistributionBoard available at this position");
    FebDistributionBoard * _febdistributionboard = febdistributionboards_.at(_position - 1);
    if (_febdistributionboard)
        return *_febdistributionboard;
    throw std::out_of_range("No FebDistributionBoard available at this position");
}

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_ControlBoard_inl_h
