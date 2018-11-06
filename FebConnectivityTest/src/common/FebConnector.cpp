#include "rpct/FebConnectivityTest/FebConnector.h"

#include "rpct/hwd/DeviceType.h"
#include "rpct/hwd/ParameterType.h"
#include "rpct/hwd/Parameter.h"
#include "rpct/hwd/Service.h"
#include "rpct/hwd/Connector.h"
#include "rpct/hwd/Connection.h"
#include "rpct/hwd/DeviceConfiguration.h"
#include "rpct/hwd/DeviceObservables.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/hwd/HardwareStorage.h"

#include "rpct/FebConnectivityTest/FebPart.h"
#include "rpct/FebConnectivityTest/LinkBoard.h"

namespace rpct {
namespace fct {

FebConnector::FebConnector(hwd::System & _system
                           , hwd::integer_type _id
                           , hwd::DeviceType const & _devicetype
                           , hwd::DeviceConfiguration const & _properties)
    : hwd::Device(_system, _id, _devicetype, _properties)
    , rpcid_(0), roll_(0), roll_connector_(0), strips_()
    , febpart_(0), linkboard_(0), position_(0)
    , selected_parameter_(_devicetype.getParameter("Selected", 0))
    , selected_(false)
{
    hwd::Parameter const & _rpcid = getDeviceType().getParameter("RpcId", 0);
    if (_properties.hasParameterSetting(_rpcid))
        _properties.getParameterSetting(_rpcid, rpcid_);

    hwd::Parameter const & _roll = getDeviceType().getParameter("Roll", 0);
    if (_properties.hasParameterSetting(_roll))
        {
            ::uint32_t _roll_uint;
            _properties.getParameterSetting(_roll, _roll_uint);
            roll_.id(_roll_uint);
        }

    hwd::Parameter const & _roll_connector = getDeviceType().getParameter("RollConnector", 0);
    if (_properties.hasParameterSetting(_roll_connector))
        _properties.getParameterSetting(_roll_connector, roll_connector_);

    hwd::Parameter const & _pins_param = getDeviceType().getParameter("Pins", 0);
    if (_properties.hasParameterSetting(_pins_param))
        {
            ::uint16_t _pins;
            _properties.getParameterSetting(_pins_param, _pins);
            strips_.setPins(_pins);
        }
    hwd::Parameter const & _first_strip_param = getDeviceType().getParameter("FirstStrip", 0);
    if (_properties.hasParameterSetting(_first_strip_param))
        {
            int _first_strip;
            _properties.getParameterSetting(_first_strip_param, _first_strip);
            strips_.setFirstStrip(_first_strip);
        }
    hwd::Parameter const & _slope_param = getDeviceType().getParameter("Slope", 0);
    if (_properties.hasParameterSetting(_slope_param))
        {
            int _slope;
            _properties.getParameterSetting(_slope_param, _slope);
            strips_.setSlope(_slope);
        }
}

hwd::DeviceType & FebConnector::registerDeviceType(hwd::System & _system)
{
    _system.registerDeviceType<FebConnector>("FebConnector");
    hwd::DeviceType & _febconnectortype = _system.registerDeviceType("FebConnector");

    hwd::ParameterType const & _rpcid_type
        = _system.registerParameterType("RpcId", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_rpcid_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _roll_type
        = _system.registerParameterType("Roll", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_roll_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _roll_connector_type
        = _system.registerParameterType("RollConnector", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_roll_connector_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _pins_type
        = _system.registerParameterType("Pins", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_pins_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _first_strip_type
        = _system.registerParameterType("FirstStrip", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_first_strip_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _slope_type
        = _system.registerParameterType("Slope", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_slope_type, 0, hwd::Parameter::is_property_);

    hwd::ParameterType const & _selected_type
        = _system.registerParameterType("Selected", hwd::DataType::integer_);
    _febconnectortype.registerParameter(_selected_type, 0, hwd::Parameter::is_observable_);

    hwd::Service const & _signal
        = _system.registerService("Signal");
    _febconnectortype.registerConnector(_signal, 0, hwd::in_);
    _febconnectortype.registerConnector(_signal, 0, hwd::out_);

    return _febconnectortype;
}

FebConnector & FebConnector::registerDevice(hwd::System & _system
                                            , int _rpcid
                                            , rpct::tools::RollId const & _roll
                                            , int _roll_connector
                                            , ::uint16_t _pins
                                            , int _first_strip
                                            , int _slope
                                            , FebPart & _febpart
                                            , LinkBoard & _linkboard
                                            , int _position)
{
    hwd::DeviceType const & _febconnectortype = _system.getDeviceType("FebConnector");
    hwd::StandaloneDeviceConfiguration _properties
        = _system.getHardwareStorage().registerDeviceTypeConfiguration(_system, _febconnectortype);

    _properties.registerParameterSetting(_febconnectortype.getParameter("RpcId", 0), _rpcid);
    _properties.registerParameterSetting(_febconnectortype.getParameter("Roll", 0), _roll.id());
    _properties.registerParameterSetting(_febconnectortype.getParameter("RollConnector", 0), _roll_connector);
    _properties.registerParameterSetting(_febconnectortype.getParameter("Pins", 0), _pins);
    _properties.registerParameterSetting(_febconnectortype.getParameter("FirstStrip", 0), _first_strip);
    _properties.registerParameterSetting(_febconnectortype.getParameter("Slope", 0), _slope);
    hwd::Device & _febconnector = _system.registerDevice(_febconnectortype, _properties);

    hwd::Connection _fpconnection
        = _system.getHardwareStorage().registerConnection(_febpart
                                                          , _febpart.getDeviceType().getConnector("Signal", 0, hwd::out_)
                                                          , _febconnector
                                                          , _febconnectortype.getConnector("Signal", 0, hwd::in_));

    _febpart.addConnection(_fpconnection);
    _febconnector.addConnection(_fpconnection);

    hwd::Connection _lbconnection
        = _system.getHardwareStorage().registerConnection(_febconnector
                                                          , _febconnectortype.getConnector("Signal", 0, hwd::out_)
                                                          , _linkboard
                                                          , _linkboard.getDeviceType().getConnector("Signal", _position, hwd::in_));

    _febconnector.addConnection(_lbconnection);
    _linkboard.addConnection(_lbconnection);

    return dynamic_cast<FebConnector &>(_febconnector);
}

void FebConnector::setSelected(bool _selected)
{
    if (selected_ != _selected)
        {
            selected_ = _selected;
            getSystem().publish(*this, selected_parameter_, selected_);
        }
}

void FebConnector::addConnection(hwd::Connection & _connection)
{
    if (_connection.getDestination() == *this)
        febpart_ = (FebPart *)(&(_connection.getSource()));
    else // Source
        {
            linkboard_ = (LinkBoard *)(&(_connection.getDestination()));
            position_ = _connection.getDestinationConnector().getPosition();
        }
}

void FebConnector::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    if (_deviceconfiguration.hasParameterSetting(selected_parameter_))
        _deviceconfiguration.getParameterSetting(selected_parameter_, selected_);
}

} // namespace fct
} // namespace rpct
