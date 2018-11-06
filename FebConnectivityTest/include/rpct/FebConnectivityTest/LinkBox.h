#ifndef rpct_FebConnectivityTest_LinkBox_h
#define rpct_FebConnectivityTest_LinkBox_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"

#include "rpct/FebConnectivityTest/fwd.h"

#include <cstddef>
#include <vector>
#include <string>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class LinkBox;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class LinkBox
    : public hwd::Device
{
public:
    static std::size_t const nboards_ = 20;
    static std::size_t const nlinkboards_ = 18;
    static std::size_t const ncontrolboards_ = 2;

public:
    LinkBox(hwd::System & _system
            , hwd::integer_type _id
            , hwd::DeviceType const & _devicetype
            , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static LinkBox & registerDevice(hwd::System & _system
                                    , int _rpcid
                                    , hwd::DeviceFlags const & _deviceflags
                                    , std::string const & _name
                                    , std::string const & _location);

    int getRPCId() const;
    std::string const & getName() const;
    std::string const & getLocation() const;

    std::vector<LinkBoard *> const & getLinkBoards();
    LinkBoard const & getLinkBoard(std::size_t _position) const;
    LinkBoard & getLinkBoard(std::size_t _position);

    std::vector<ControlBoard *> const & getControlBoards();
    ControlBoard const & getControlBoard(std::size_t _position) const;
    ControlBoard & getControlBoard(std::size_t _position);

    bool isActive() const;
    void setActive(bool _active);

#ifndef OFFLINE_ANALYSIS
    void setRpctLinkBox(rpct::LinkBox & _linkbox);
    rpct::LinkBox const & getRpctLinkBox() const;
    rpct::LinkBox & getRpctLinkBox();
#endif // OFFLINE_ANALYSIS

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceFlagsMask const & _mask);
    void iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask);

    void imonitor(hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    int rpcid_;
    std::string name_;
    std::string location_;

    std::vector<LinkBoard *> linkboards_;
    std::vector<ControlBoard *> controlboards_;

    hwd::Parameter const & active_parameter_;
    bool active_;

#ifndef OFFLINE_ANALYSIS
    rpct::LinkBox * rpct_linkbox_;
#endif // OFFLINE_ANALYSIS
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/LinkBox-inl.h"

#endif // rpct_FebConnectivityTest_LinkBox_h
