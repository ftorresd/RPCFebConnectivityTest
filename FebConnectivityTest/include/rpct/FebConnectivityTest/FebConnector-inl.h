#ifndef rpct_FebConnectivityTest_FebConnector_inl_h
#define rpct_FebConnectivityTest_FebConnector_inl_h

#include "rpct/FebConnectivityTest/FebConnector.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline int FebConnector::getRPCId() const
{
    return rpcid_;
}

inline rpct::tools::RollId const & FebConnector::getRoll() const
{
    return roll_;
}

inline std::size_t FebConnector::getRollConnector() const
{
    return roll_connector_;
}

inline FebConnectorStrips const & FebConnector::getStrips() const
{
    return strips_;
}

inline FebPart const & FebConnector::getFebPart() const
{
    if (febpart_)
        return *febpart_;
    throw std::logic_error("FebPart not available yet");
}

inline FebPart & FebConnector::getFebPart()
{
    if (febpart_)
        return *febpart_;
    throw std::logic_error("FebPart not available yet");
}

inline LinkBoard const & FebConnector::getLinkBoard() const
{
    if (linkboard_)
        return *linkboard_;
    throw std::logic_error("LinkBoard not available yet");
}

inline LinkBoard & FebConnector::getLinkBoard()
{
    if (linkboard_)
        return *linkboard_;
    throw std::logic_error("LinkBoard not available yet");
}

inline std::size_t FebConnector::getPosition() const
{
    return position_;
}

inline bool FebConnector::isSelected() const
{
    return selected_;
}

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_FebConnector_inl_h
