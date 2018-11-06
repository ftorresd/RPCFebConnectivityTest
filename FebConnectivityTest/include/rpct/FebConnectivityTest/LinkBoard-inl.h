#ifndef rpct_FebConnectivityTest_LinkBoard_inl_h
#define rpct_FebConnectivityTest_LinkBoard_inl_h

#include "rpct/FebConnectivityTest/LinkBoard.h"

#include <stdexcept>

namespace rpct {
namespace fct {

inline hwd::Parameter const & LinkBoardType::getDurationParameter() const
{
    if (duration_parameter_)
        return *duration_parameter_;
    throw std::logic_error("DurationParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getWindowOpenParameter() const
{
    if (window_open_parameter_)
        return *window_open_parameter_;
    throw std::logic_error("WindowOpenParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getWindowClosedParameter() const
{
    if (window_closed_parameter_)
        return *window_closed_parameter_;
    throw std::logic_error("WindowClosedParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getCountsParameter() const
{
    if (counts_parameter_)
        return *counts_parameter_;
    throw std::logic_error("CountsParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getWindowedCountsParameter() const
{
    if (windowed_counts_parameter_)
        return *windowed_counts_parameter_;
    throw std::logic_error("WindowedCountsParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getRunPulserParameter() const
{
    if (run_pulser_parameter_)
        return *run_pulser_parameter_;
    throw std::logic_error("RunPulserParameter not available yet");
}

inline hwd::Parameter const & LinkBoardType::getPulserDataParameter() const
{
    if (pulser_data_parameter_)
        return *pulser_data_parameter_;
    throw std::logic_error("PulserDataParameter not available yet");
}

inline int LinkBoard::getRPCId() const
{
    return rpcid_;
}

inline std::string const & LinkBoard::getName() const
{
    return name_;
}

inline LinkBox const & LinkBoard::getLinkBox() const
{
    if (linkbox_)
        return *linkbox_;
    throw std::logic_error("LinkBox not available yet");
}

inline LinkBox & LinkBoard::getLinkBox()
{
    if (linkbox_)
        return *linkbox_;
    throw std::logic_error("LinkBox not available yet");
}

inline std::size_t LinkBoard::getPosition() const
{
    return position_;
}

inline std::vector<FebConnector *> const & LinkBoard::getFebConnectors()
{
    return febconnectors_;
}

inline FebConnector const & LinkBoard::getFebConnector(std::size_t _position) const
{
    if (_position < 1 || _position > nfebconnectors_)
        throw std::logic_error("No FebConnector available at this position");
    FebConnector * _febconnector = febconnectors_.at(_position - 1);
    if (_febconnector)
        return *_febconnector;
    throw std::out_of_range("No FebConnector available at this position");
}

inline FebConnector & LinkBoard::getFebConnector(std::size_t _position)
{
    if (_position < 1 || _position > nfebconnectors_)
        throw std::logic_error("No FebConnector available at this position");
    FebConnector * _febconnector = febconnectors_.at(_position - 1);
    if (_febconnector)
        return *_febconnector;
    throw std::out_of_range("No FebConnector available at this position");
}

inline rpct::tools::Time const & LinkBoard::getDuration() const
{
    return duration_;
}

inline std::size_t LinkBoard::getWindowOpen() const
{
    return window_open_;
}

inline std::size_t LinkBoard::getWindowClosed() const
{
    return window_closed_;
}

inline double LinkBoard::getWindowFraction() const
{
    return (double)(window_closed_ - window_open_) / (double)(max_window_);
}

inline std::vector< ::uint32_t> const & LinkBoard::getCounts() const
{
    return counts_;
}

inline ::uint32_t LinkBoard::getCount(std::size_t _channel) const
{
    if (_channel < nchannels_)
        return counts_.at(_channel);
    throw std::out_of_range("Requestion LinkBoard count beyond its size");
}

inline double LinkBoard::getRate(std::size_t _channel) const
{
    if (_channel < nchannels_)
        return (double)(counts_.at(_channel)) / (double)(duration_);
    throw std::out_of_range("Requestion LinkBoard rate beyond its size");
}

inline double LinkBoard::getAvgRate(std::size_t _febconnector) const
{
    return getAvgCount(_febconnector) / (double)(duration_);
}

inline std::vector< ::uint32_t> const & LinkBoard::getWindowedCounts() const
{
    return windowed_counts_;
}

inline ::uint32_t LinkBoard::getWindowedCount(std::size_t _channel) const
{
    if (_channel < nchannels_)
        return windowed_counts_.at(_channel);
    throw std::out_of_range("Requestion LinkBoard windowed count beyond its size");
}

inline double LinkBoard::getWindowedRate(std::size_t _channel) const
{
    if (_channel < nchannels_)
        return (double)(windowed_counts_.at(_channel)) / (double)(duration_);
    throw std::out_of_range("Requestion LinkBoard windowed rate beyond its size");
}

inline double LinkBoard::getAvgWindowedRate(std::size_t _febconnector) const
{
    return getAvgWindowedCount(_febconnector) / (double)(duration_);
}

inline bool LinkBoard::getRunPulser() const
{
    return run_pulser_;
}

inline std::vector< ::uint32_t> const & LinkBoard::getPulserData() const
{
    return pulser_data_;
}

inline ::uint32_t LinkBoard::getPulserData(std::size_t _bx) const
{
    if (_bx < pulser_data_.size())
        return pulser_data_.at(_bx);
    return 0;
}

#ifndef OFFLINE_ANALYSIS
inline void LinkBoard::setRpctLinkBoard(rpct::LinkBoard & _linkboard)
{
    rpct_linkboard_ = &_linkboard;
}

inline rpct::LinkBoard const & LinkBoard::getRpctLinkBoard() const
{
    if (rpct_linkboard_)
        return *rpct_linkboard_;
    throw std::logic_error("rpct::LinkBoard not available yet");
}

inline rpct::LinkBoard & LinkBoard::getRpctLinkBoard()
{
    if (rpct_linkboard_)
        return *rpct_linkboard_;
    throw std::logic_error("rpct::LinkBoard not available yet");
}

inline void LinkBoard::setRpctSynCoder(rpct::SynCoder & _syncoder)
{
    rpct_syncoder_ = &_syncoder;
}

inline rpct::SynCoder const & LinkBoard::getRpctSynCoder() const
{
    if (rpct_syncoder_)
        return *rpct_syncoder_;
    throw std::logic_error("rpct::SynCoder not available yet");
}

inline rpct::SynCoder & LinkBoard::getRpctSynCoder()
{
    if (rpct_syncoder_)
        return *rpct_syncoder_;
    throw std::logic_error("rpct::SynCoder not available yet");
}
#endif // OFFLINE_ANALYSIS

} // namespace fct
} // namespace rpct

#endif // rpct_FebConnectivityTest_LinkBoard_inl_h
