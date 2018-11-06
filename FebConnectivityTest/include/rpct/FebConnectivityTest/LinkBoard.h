#ifndef rpct_FebConnectivityTest_LinkBoard_h
#define rpct_FebConnectivityTest_LinkBoard_h

#include "log4cplus/logger.h"

#include "rpct/hwd/fwd.h"
#include "rpct/hwd/Device.h"
#include "rpct/hwd/DeviceType.h"

#include "rpct/FebConnectivityTest/fwd.h"
#include "rpct/tools/Time.h"

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <string>

#ifndef OFFLINE_ANALYSIS
namespace rpct {
class LinkBoard;
class SynCoder;
} // namespace rpct
#endif // OFFLINE_ANALYSIS

namespace rpct {
namespace fct {

class LinkBoardType
    : public hwd::DeviceType
{
public:
    LinkBoardType(hwd::System & _system
                  , hwd::integer_type _id
                  , std::string const & _name = "LinkBoard");

    static LinkBoardType & registerDeviceType(hwd::System & _system);

    hwd::Parameter const & getDurationParameter() const;
    hwd::Parameter const & getWindowOpenParameter() const;
    hwd::Parameter const & getWindowClosedParameter() const;
    hwd::Parameter const & getCountsParameter() const;
    hwd::Parameter const & getWindowedCountsParameter() const;
    hwd::Parameter const & getRunPulserParameter() const;
    hwd::Parameter const & getPulserDataParameter() const;

protected:
    hwd::Parameter const & addParameter(hwd::integer_type _id
                                        , hwd::ParameterType const & _parametertype
                                        , hwd::integer_type _position = 0
                                        , unsigned char _properties = 0x0);

protected:
    hwd::Parameter const * duration_parameter_;
    hwd::Parameter const * window_open_parameter_;
    hwd::Parameter const * window_closed_parameter_;
    hwd::Parameter const * counts_parameter_;
    hwd::Parameter const * windowed_counts_parameter_;
    hwd::Parameter const * run_pulser_parameter_;
    hwd::Parameter const * pulser_data_parameter_;
};

class LinkBoard
    : public hwd::Device
{
public:
    static std::size_t const nfebconnectors_ = 6;
    static std::size_t const nchannels_ = 96;
    static ::uint32_t const nanoseconds_per_bx_ = 25;
    static ::uint32_t const bx_per_second_ = 40000000;
    static unsigned char const max_window_ = 239;
    static double const picoseconds_per_window_unit_;

protected:
    static log4cplus::Logger logger_;

public:
    LinkBoard(hwd::System & _system
              , hwd::integer_type _id
              , hwd::DeviceType const & _devicetype
              , hwd::DeviceConfiguration const & _properties);

    static LinkBoardType & registerDeviceType(hwd::System & _system);
    static LinkBoard & registerDevice(hwd::System & _system
                                      , int _rpcid
                                      , hwd::DeviceFlags const & _deviceflags
                                      , std::string const & _name
                                      , LinkBox & _linkbox
                                      , std::size_t _position);

    int getRPCId() const;
    std::string const & getName() const;

    LinkBox const & getLinkBox() const;
    LinkBox & getLinkBox();
    std::size_t getPosition() const;

    std::vector<FebConnector *> const & getFebConnectors();
    FebConnector const & getFebConnector(std::size_t _position) const;
    FebConnector & getFebConnector(std::size_t _position);

    rpct::tools::Time const & getDuration() const;
    std::size_t getWindowOpen() const;
    std::size_t getWindowClosed() const;
    double getWindowFraction() const;

    std::vector< ::uint32_t> const & getCounts() const;
    ::uint32_t getCount(std::size_t _channel) const;
    double getRate(std::size_t _channel) const;
    double getAvgCount(std::size_t _febconnector) const;
    double getAvgRate(std::size_t _febconnector) const;

    std::vector< ::uint32_t> const & getWindowedCounts() const;
    ::uint32_t getWindowedCount(std::size_t _channel) const;
    double getWindowedRate(std::size_t _channel) const;
    double getAvgWindowedCount(std::size_t _febconnector) const;
    double getAvgWindowedRate(std::size_t _febconnector) const;

    bool getRunPulser() const;
    std::vector< ::uint32_t> const & getPulserData() const;
    ::uint32_t getPulserData(std::size_t _bx) const;

#ifndef OFFLINE_ANALYSIS
    void setRpctLinkBoard(rpct::LinkBoard & _linkboard);
    rpct::LinkBoard const & getRpctLinkBoard() const;
    rpct::LinkBoard & getRpctLinkBoard();

    void setRpctSynCoder(rpct::SynCoder & _syncoder);
    rpct::SynCoder const & getRpctSynCoder() const;
    rpct::SynCoder & getRpctSynCoder();
#endif // OFFLINE_ANALYSIS

    void addConnection(hwd::Connection & _connection);

protected:
    void iconfigure(hwd::DeviceConfiguration const & _deviceconfiguration, hwd::DeviceFlagsMask const & _mask);
    void imonitor(hwd::DeviceObservables const & _deviceobservables, hwd::DeviceFlagsMask const & _mask);

    void iassume(hwd::Configuration const & _configuration);
    void iassume(hwd::DeviceConfiguration const & _deviceconfiguration);

protected:
    int rpcid_;
    std::string name_;

    LinkBox * linkbox_;
    std::size_t position_;
    std::vector<FebConnector *> febconnectors_;

    LinkBoardType const & linkboard_type_;
    rpct::tools::Time duration_;
    std::size_t window_open_, window_closed_;
    std::vector< ::uint32_t> counts_, windowed_counts_;
    bool run_pulser_;
    std::vector< ::uint32_t> pulser_data_;

#ifndef OFFLINE_ANALYSIS
    rpct::LinkBoard * rpct_linkboard_;
    rpct::SynCoder * rpct_syncoder_;
#endif // OFFLINE_ANALYSIS
};

} // namespace fct
} // namespace rpct

#include "rpct/FebConnectivityTest/LinkBoard-inl.h"

#endif // rpct_FebConnectivityTest_LinkBoard_h
