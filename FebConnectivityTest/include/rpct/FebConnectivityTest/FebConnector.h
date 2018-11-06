#ifndef rpct_FebConnectivityTest_FebConnector_h
#define rpct_FebConnectivityTest_FebConnector_h

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"

#include "rpct/FebConnectivityTest/fwd.h"
#include "rpct/FebConnectivityTest/FebConnectorStrips.h"
#include "rpct/tools/RollId.h"

#include <stdint.h>
#include <cstddef>

namespace rpct {
namespace fct {

class FebConnector
    : public hwd::Device
{
public:
    FebConnector(hwd::System & _system
                 , hwd::integer_type _id
                 , hwd::DeviceType const & _devicetype
                 , hwd::DeviceConfiguration const & _properties);

    static hwd::DeviceType & registerDeviceType(hwd::System & _system);
    static FebConnector & registerDevice(hwd::System & _system
                                         , int _rpcid
                                         , rpct::tools::RollId const & _roll
                                         , int _roll_connector
                                         , ::uint16_t _pins
                                         , int _first_strip
                                         , int _slope
                                         , FebPart & _febpart
                                         , LinkBoard & _linkboard
                                         , int _position);

    int getRPCId() const;
    rpct::tools::RollId const & getRoll() const;
    std::size_t getRollConnector() const;
    FebConnectorStrips const & getStrips() const;

    FebPart const & getFebPart() const;
    FebPart & getFebPart();
    LinkBoard const & getLinkBoard() const;
    LinkBoard & getLinkBoard();
    std::size_t getPosition() const;

    bool isSelected() const;
    void setSelected(bool _selected);

    void addConnection(hwd::Connection & _connection);

protected:
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    int rpcid_;
    rpct::tools::RollId roll_;
    std::size_t roll_connector_;
    FebConnectorStrips strips_;

    FebPart * febpart_;
    LinkBoard * linkboard_;
    std::size_t position_;

    hwd::Parameter const & selected_parameter_;
    bool selected_;
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/FebConnector-inl.h"

#endif // rpct_FebConnectivityTest_FebConnector_h
