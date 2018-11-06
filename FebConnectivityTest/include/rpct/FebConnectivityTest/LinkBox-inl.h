#ifndef rpct_FebConnectivityTest_LinkBox_inl_h
#define rpct_FebConnectivityTest_LinkBox_inl_h

#include "rpct/FebConnectivityTest/LinkBox.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline int LinkBox::getRPCId() const
{
    return rpcid_;
}

inline std::string const & LinkBox::getName() const
{
    return name_;
}

inline std::string const & LinkBox::getLocation() const
{
    return location_;
}

inline std::vector<LinkBoard *> const & LinkBox::getLinkBoards()
{
    return linkboards_;
}

inline LinkBoard const & LinkBox::getLinkBoard(std::size_t _position) const
{
    if (_position == 10 || _position == 20)
        throw std::logic_error("Only ControlBoards populate positions 10 and 20");
    if (!_position)
        throw std::logic_error("LinkBoard positions count upwards from 1");
    _position = _position - (_position / 10) - 1;

    if (_position > nlinkboards_)
        throw std::logic_error("No LinkBoard available at this position");
    LinkBoard * _linkboard = linkboards_.at(_position);
    if (_linkboard)
        return *_linkboard;
    throw std::out_of_range("No LinkBoard available at this position");
}

inline LinkBoard & LinkBox::getLinkBoard(std::size_t _position)
{
    if (_position == 10 || _position == 20)
        throw std::logic_error("Only ControlBoards populate positions 10 and 20");
    if (!_position)
        throw std::logic_error("LinkBoard positions count upwards from 1");
    _position = _position - (_position / 10) - 1;

    if (_position > nlinkboards_)
        throw std::logic_error("No LinkBoard available at this position");
    LinkBoard * _linkboard = linkboards_.at(_position);
    if (_linkboard)
        return *_linkboard;
    throw std::out_of_range("No LinkBoard available at this position");
}

inline std::vector<ControlBoard *> const & LinkBox::getControlBoards()
{
    return controlboards_;
}

inline ControlBoard const & LinkBox::getControlBoard(std::size_t _position) const
{
    switch (_position) {
    case 10: _position = 0; break;
    case 20: _position = 1; break;
    default: throw std::logic_error("No ControlBoard available at this position");
    }
    ControlBoard * _controlboard = controlboards_.at(_position);
    if (_controlboard)
        return *_controlboard;
}

inline ControlBoard & LinkBox::getControlBoard(std::size_t _position)
{
    switch (_position) {
    case 10: _position = 0; break;
    case 20: _position = 1; break;
    default: throw std::logic_error("No ControlBoard available at this position");
    }
    ControlBoard * _controlboard = controlboards_.at(_position);
    if (_controlboard)
        return *_controlboard;
}

inline bool LinkBox::isActive() const
{
    return active_;
}

#ifndef OFFLINE_ANALYSIS
inline void LinkBox::setRpctLinkBox(rpct::LinkBox & _linkbox)
{
    rpct_linkbox_ = &_linkbox;
}

inline rpct::LinkBox const & LinkBox::getRpctLinkBox() const
{
    if (rpct_linkbox_)
        return *rpct_linkbox_;
    throw std::logic_error("rpct::LinkBox not available yet");
}

inline rpct::LinkBox & LinkBox::getRpctLinkBox()
{
    if (rpct_linkbox_)
        return *rpct_linkbox_;
    throw std::logic_error("rpct::LinkBox not available yet");
}
#endif // OFFLINE_ANALYSIS

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_LinkBox_inl_h
