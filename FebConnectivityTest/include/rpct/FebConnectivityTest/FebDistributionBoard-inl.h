#ifndef rpct_FebConnectivityTest_FebDistributionBoard_inl_h
#define rpct_FebConnectivityTest_FebDistributionBoard_inl_h

#include "rpct/FebConnectivityTest/FebDistributionBoard.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline ControlBoard const & FebDistributionBoard::getControlBoard() const
{
    if (controlboard_)
        return *controlboard_;
    throw std::logic_error("ControlBoard not available yet");
}

inline ControlBoard & FebDistributionBoard::getControlBoard()
{
    if (controlboard_)
        return *controlboard_;
    throw std::logic_error("ControlBoard not available yet");
}

inline std::size_t FebDistributionBoard::getPosition() const
{
    return position_;
}

inline std::vector<FebBoard *> const & FebDistributionBoard::getFebBoards()
{
    return febboards_;
}

inline FebBoard const & FebDistributionBoard::getFebBoard(std::size_t _position) const
{
    if (_position >= nfebboards_)
        throw std::logic_error("No FebBoard available at this position");
    FebBoard * _febboard = febboards_.at(_position);
    if (_febboard)
        return *_febboard;
    throw std::out_of_range("No FebBoard available at this position");
}

inline FebBoard & FebDistributionBoard::getFebBoard(std::size_t _position)
{
    if (_position >= nfebboards_)
        throw std::logic_error("No FebBoard available at this position");
    FebBoard * _febboard = febboards_.at(_position);
    if (_febboard)
        return *_febboard;
    throw std::out_of_range("No FebBoard available at this position");
}

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebDistributionBoard_inl_h
