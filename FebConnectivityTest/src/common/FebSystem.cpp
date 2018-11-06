#include "rpct/FebConnectivityTest/FebSystem.h"

#include <stdexcept>

#include "rpct/hwd/Device.h"
#include "rpct/hwd/DataType.h"
#include "rpct/hwd/StandaloneDeviceConfiguration.h"

#include "rpct/FebConnectivityTest/LinkBox.h"
#include "rpct/FebConnectivityTest/LinkBoard.h"
#include "rpct/FebConnectivityTest/ControlBoard.h"
#include "rpct/FebConnectivityTest/FebDistributionBoard.h"
#include "rpct/FebConnectivityTest/FebBoard.h"
#include "rpct/FebConnectivityTest/FebPart.h"
#include "rpct/FebConnectivityTest/FebChip.h"
#include "rpct/FebConnectivityTest/FebConnector.h"

#include "rpct/FebConnectivityTest/FebConnectivityTest.h"
#include "rpct/FebConnectivityTest/FebThresholdScan.h"

namespace rpct {
namespace fct {

FebSystem::FebSystem(hwd::HardwareStorage & _hardwarestorage
                     , std::string const & _name)
    : hwd::System(_hardwarestorage, _name)
    , tower_("DefaultTower")
    , linkbox_type_(0)
{
    linkbox_type_ = &(LinkBox::registerDeviceType(*this));
    LinkBoard::registerDeviceType(*this);
    ControlBoard::registerDeviceType(*this);
    FebDistributionBoard::registerDeviceType(*this);
    FebBoard::registerDeviceType(*this);
    FebPart::registerDeviceType(*this);
    FebChip::registerDeviceType(*this);
    FebConnector::registerDeviceType(*this);

    febconnectivitytest_type_ = &(FebConnectivityTest::registerDeviceType(*this));
    febthresholdscan_type_ = &(FebThresholdScan::registerDeviceType(*this));
}

std::vector<LinkBox *> FebSystem::getLinkBoxes()
{
    std::vector<hwd::Device *> const & _devices = getDevices(*linkbox_type_);
    std::vector<LinkBox *> _linkboxes;
    _linkboxes.reserve(_devices.size());
    for (std::vector<hwd::Device *>::const_iterator _device = _devices.begin()
             ; _device != _devices.end() ; ++_device)
        _linkboxes.push_back(dynamic_cast<LinkBox *>(*_device));
    return _linkboxes;
}

FebConnectivityTest & FebSystem::getFebConnectivityTest()
{
    std::vector<hwd::Device *> const & _devices = getDevices(*febconnectivitytest_type_);
    if (_devices.empty())
        throw std::runtime_error("Could not find requested FebConnectivityTest");
    return dynamic_cast<FebConnectivityTest &>(*(_devices.front()));
}

FebThresholdScan & FebSystem::getFebThresholdScan()
{
    std::vector<hwd::Device *> const & _devices = getDevices(*febthresholdscan_type_);
    if (_devices.empty())
        throw std::runtime_error("Could not find requested FebThresholdScan");
    return dynamic_cast<FebThresholdScan &>(*(_devices.front()));
}

void FebSystem::registerSystem(std::string const & _tower)
{
    tower_ = _tower;

    // register DeviceType
    hwd::DeviceType & _febsystemtype = registerDeviceType("FebSystem");

    hwd::ParameterType const & _towertype = registerParameterType("Tower", hwd::DataType::text_);
    _febsystemtype.registerParameter(_towertype, 0, hwd::Parameter::is_property_);

    // register Properties
    hwd::StandaloneDeviceConfiguration _properties
        = getHardwareStorage().registerDeviceTypeConfiguration(*this, _febsystemtype);

    _properties.registerParameterSetting(_febsystemtype.getParameter("Tower", 0), _tower);

    // register System
    getHardwareStorage().registerSystem(*this, _febsystemtype, _properties);
}

void FebSystem::loadSystem()
{
    getHardwareStorage().loadSystem(*this);
}

void FebSystem::iconfigure(hwd::DeviceFlagsMask const & _mask)
{
    std::vector<hwd::Device *> const & _linkboxes = getDevices(*linkbox_type_);
    for (std::vector<hwd::Device *>::const_iterator _linkbox = _linkboxes.begin()
             ; _linkbox != _linkboxes.end() ; ++_linkbox)
        (*_linkbox)->configure(_mask);
}

void FebSystem::iconfigure(hwd::Configuration const & _configuration, hwd::DeviceFlagsMask const & _mask)
{
    std::vector<hwd::Device *> const & _linkboxes = getDevices(*linkbox_type_);
    for (std::vector<hwd::Device *>::const_iterator _linkbox = _linkboxes.begin()
             ; _linkbox != _linkboxes.end() ; ++_linkbox)
        (*_linkbox)->configure(_configuration, _mask);
}

void FebSystem::imonitor(hwd::DeviceFlagsMask const & _mask)
{
    std::vector<hwd::Device *> const & _linkboxes = getDevices(*linkbox_type_);
    for (std::vector<hwd::Device *>::const_iterator _linkbox = _linkboxes.begin()
             ; _linkbox != _linkboxes.end() ; ++_linkbox)
        (*_linkbox)->monitor(_mask);
}

void FebSystem::imonitor(hwd::Observables const & _observables, hwd::DeviceFlagsMask const & _mask)
{
    std::vector<hwd::Device *> const & _linkboxes = getDevices(*linkbox_type_);
    for (std::vector<hwd::Device *>::const_iterator _linkbox = _linkboxes.begin()
             ; _linkbox != _linkboxes.end() ; ++_linkbox)
        (*_linkbox)->monitor(_observables, _mask);
}

void FebSystem::iassume(hwd::Configuration const & _configuration)
{
    std::vector<hwd::Device *> const & _linkboxes = getDevices(*linkbox_type_);
    for (std::vector<hwd::Device *>::const_iterator _linkbox = _linkboxes.begin()
             ; _linkbox != _linkboxes.end() ; ++_linkbox)
        (*_linkbox)->assume(_configuration);
}

void FebSystem::iassume(hwd::DeviceConfiguration const & _deviceconfiguration)
{
    hwd::Parameter const & _tower = getDeviceType().getParameter("Tower", 0);
    if (_deviceconfiguration.hasParameterSetting(_tower))
        _deviceconfiguration.getParameterSetting(_tower, tower_);

}

} // namespace fct
} // namespace rpct
