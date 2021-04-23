#include <unistd.h>

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>

#include "log4cplus/configurator.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include "rpct/tools/Time.h"
#include "rpct/tools/Date.h"

#include "rpct/tools/RPCDetId.h"
#include "rpct/tools/RollId.h"
#include "rpct/tools/RollSelection.h"

#include "rpct/hwd/HardwareStorage.h"
#include "rpct/hwd/Snapshot.h"
#include "rpct/hwd/SnapshotType.h"

#include "rpct/FebConnectivityTest/FebChip.h"
#include "rpct/FebConnectivityTest/FebPart.h"
#include "rpct/FebConnectivityTest/FebBoard.h"
#include "rpct/FebConnectivityTest/FebDistributionBoard.h"
#include "rpct/FebConnectivityTest/ControlBoard.h"
#include "rpct/FebConnectivityTest/FebConnectorStrips.h"
#include "rpct/FebConnectivityTest/FebConnector.h"
#include "rpct/FebConnectivityTest/LinkBoard.h"
#include "rpct/FebConnectivityTest/LinkBox.h"
#include "rpct/FebConnectivityTest/FebSystem.h"
#include "rpct/FebConnectivityTest/FebConnectivityTest.h"

#include "analysis/TangoColors.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TPaveText.h"

/** Comparison class to sort FebConnectors for end-user ease; not efficient, but effective */
class FebConnectorCompare
{
public:
    FebConnectorCompare(rpct::fct::FebSystem const &_febsystem)
        : febsystem_(_febsystem)
    {
    }

    bool operator()(rpct::hwd::integer_type _lh, rpct::hwd::integer_type _rh) const
    {
        rpct::fct::FebConnector const &_lhc((rpct::fct::FebConnector const &)(febsystem_.getDevice(_lh)));
        rpct::fct::FebConnector const &_rhc((rpct::fct::FebConnector const &)(febsystem_.getDevice(_rh)));
        return (_lhc.getRoll() < _rhc.getRoll() || (_lhc.getRoll() == _rhc.getRoll() && _lhc.getRollConnector() < _rhc.getRollConnector()));
    }

protected:
    rpct::fct::FebSystem const &febsystem_;
};

/** Comparison class to sort FebChips for end-user ease; not efficient, but effective */
class FebChipCompare
{
public:
    FebChipCompare(rpct::fct::FebSystem const &_febsystem)
        : febsystem_(_febsystem)
    {
    }

    bool operator()(rpct::hwd::integer_type _lh, rpct::hwd::integer_type _rh) const
    {
        rpct::fct::FebChip const &_lhc((rpct::fct::FebChip const &)(febsystem_.getDevice(_lh)));
        rpct::fct::FebChip const &_rhc((rpct::fct::FebChip const &)(febsystem_.getDevice(_rh)));
        return (_lhc.getFebPart().getFebConnector().getRoll() < _rhc.getFebPart().getFebConnector().getRoll() || (_lhc.getFebPart().getFebConnector().getRoll() == _rhc.getFebPart().getFebConnector().getRoll() && (_lhc.getFebPart().getFebConnector().getRollConnector() < _rhc.getFebPart().getFebConnector().getRollConnector() || (_lhc.getFebPart().getFebConnector().getRollConnector() == _rhc.getFebPart().getFebConnector().getRollConnector() && _lhc.getPosition() < _rhc.getPosition()))));
    }

protected:
    rpct::fct::FebSystem const &febsystem_;
};

/** Print usage manual */
void manual(std::string const &_name)
{
    std::cerr << "\n"
              << "Usage: " << _name << " [-p offset] [-i offset]\n"
              << "       " << std::setw(_name.size()) << " "
              << "   [-a rate] [-n rate] [-f fraction]\n"
              << "       " << std::setw(_name.size()) << " "
              << "   [-s chamber] [-m chamber]\n"
              << "       " << std::setw(_name.size()) << " "
              << "   [-O output_filename]\n"
              << "       " << std::setw(_name.size()) << " "
              << "   db_filename\n"
              << "Analyse the output of a connectivity test.\n"
              << "\n"
              << "positional arguments:\n"
              << "  db_filename           the sqlite3 output of the connectivity test to analyse\n"
              << "\n"
              << "optional arguments:\n"
              << "  -p offset             set maximum VTh offset before flagging it problematic (mV)\n"
              << "                        default: 9.77 mV\n"
              << "  -i offset             set maximum VTh offset before ignoring chip (mV)\n"
              << "                        default: 48.83 mV\n"
              << "  -a rate               set the lower rate limit from which a strip is declared\n"
              << "                        active at low threshold (Hz)\n"
              << "                        default: 1e5 Hz\n"
              << "  -n rate               set the lower rate limit from which a strip is declared\n"
              << "                        noisy at high threshold (Hz)\n"
              << "                        default: 1e4 Hz\n"
              << "  -f fraction           the fraction of channels in a connector that should be\n"
              << "                        active or noisy to flag the full chip or connector\n"
              << "                        default: 0.4\n"
              << "  -s chamber            select: show results for the given chamber only\n"
              << "                        for example RE+1/2/36, RB1in/W+1/S04, RB4/W0/S04++\n"
              << "                        can contain wildcards and can be repeated\n"
              << "  -m chamber            mask: exclude this chamber from the results\n"
              << "                        for example RE+1/2/36, RB1in/W+1/S04, RB4/W0/S04++\n"
              << "                        can contain wildcards and can be repeated\n"
              << "  -O output_filename    set the filename for text output\n"
              << "                        default: - (stdout)\n"
              << std::endl;
}

int main(int _argc, char *_argv[])
{
    // Configure Log4cplus
#ifdef LOG4CPLUS_DISABLE_DEBUG
    log4cplus::BasicConfigurator log4cplus_config_;
#else
    log4cplus::PropertyConfigurator log4cplus_config_("cfg/log4cplus.cfg");
#endif // LOG4CPLUS_DISABLE_DEBUG
    log4cplus_config_.configure();
    log4cplus::Logger logger_ = log4cplus::Logger::getInstance("Main");

    // Configure SQLite
    ::sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);

    // Global Variables
    std::string db_filename_, filename_base_;

    std::ostream *output_ = &(std::cout);
    std::ofstream *output_file_(0);

    int vth_offset_problem_(4), vth_offset_ignore_(20);
    double rate_noisy_(1e4), rate_active_(1e5);
    double channel_fraction_(.4);
    rpct::tools::RollSelection roll_selection_;

    { // parse parameters
        if (_argc == 1)
        {
            manual(_argv[0]);
            exit(EXIT_FAILURE);
        }

        db_filename_ = _argv[_argc - 1];
        int _opt;
        std::stringstream _parser;
        double _offset;
        rpct::tools::RollId _rollid;
        while ((_opt = ::getopt(_argc, _argv, "p:i:a:n:f:s:m:O:h")) != -1)
        {
            _parser.clear();
            _parser.str("");
            switch (_opt)
            {
            case 'p':
                _parser << ::optarg;
                _parser >> _offset;
                vth_offset_problem_ = (int)(_offset * 1024. / 2500. + .5);
                break;
            case 'i':
                _parser << ::optarg;
                _parser >> _offset;
                vth_offset_ignore_ = (int)(_offset * 1024. / 2500. + .5);
                break;
            case 'a':
                _parser << ::optarg;
                _parser >> rate_active_;
                break;
            case 'n':
                _parser << ::optarg;
                _parser >> rate_noisy_;
                break;
            case 'f':
                _parser << ::optarg;
                _parser >> channel_fraction_;
                break;
            case 's':
                try
                {
                    _rollid.parseName(::optarg);
                }
                catch (std::runtime_error const &_error)
                {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.select(_rollid.getChamberId());
                break;
            case 'm':
                try
                {
                    _rollid.parseName(::optarg);
                }
                catch (std::runtime_error const &_error)
                {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.mask(_rollid.getChamberId());
                break;
            case 'O':
                if (::optarg[0] != '-')
                    output_ = (output_file_ = new std::ofstream(::optarg));
                break;
            case 'h':
                manual(_argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                manual(_argv[0]);
                exit(EXIT_FAILURE);
                break;
            }
        }
    }

    if (roll_selection_.getSelected().empty())
        roll_selection_.select(rpct::tools::RollId(0));

    { // Filename
        std::size_t _db_filename_start = db_filename_.find_last_of("/\\");
        if (_db_filename_start == std::string::npos)
            _db_filename_start = 0;
        else
            ++_db_filename_start;
        filename_base_ = db_filename_.substr(_db_filename_start, db_filename_.find_last_of('.') - _db_filename_start);
    }

    // Open file and load system
    rpct::hwd::HardwareStorage hwstorage_(db_filename_, rpct::hwd::HardwareStorage::read_);
    rpct::fct::FebSystem febsystem_(hwstorage_, "FebSystem");
    febsystem_.loadSystem();

    // Bookkeeping
    FebConnectorCompare febconnector_compare_(febsystem_);
    FebChipCompare febchip_compare_(febsystem_);
    std::vector<rpct::hwd::integer_type> dead_connector_;
    std::vector<rpct::hwd::integer_type> partially_dead_connector_;
    std::map<rpct::hwd::integer_type, std::vector<rpct::hwd::integer_type>, FebConnectorCompare> swap_connectors_(febconnector_compare_);

    std::map<rpct::hwd::integer_type, int, FebChipCompare> offset_chip_ignore_(febchip_compare_);
    std::map<rpct::hwd::integer_type, int, FebChipCompare> offset_chip_problem_(febchip_compare_);
    std::set<rpct::hwd::integer_type> offset_connector_ignore_index_;

    std::vector<rpct::hwd::integer_type> noisy_connector_ignore_;
    std::vector<rpct::hwd::integer_type> noisy_connector_problem_;
    std::set<rpct::hwd::integer_type> noisy_connector_ignore_index_;

    std::map<rpct::hwd::integer_type, THStack *> linkboard_stacks_;
    std::map<rpct::hwd::integer_type, TLegend *> linkboard_legends_;
    std::map<rpct::hwd::integer_type, TH1F *> linkboard_histograms_;

    std::map<rpct::hwd::integer_type, THStack *> channel_stacks_;
    std::map<rpct::hwd::integer_type, TLegend *> channel_legends_;

    std::map<rpct::tools::RollId, THStack *> strip_stacks_;
    std::map<rpct::tools::RollId, TLegend *> strip_legends_;
    std::map<rpct::tools::RollId, TH1F *> strip_histograms_;

    // Get ConnectivityTest Configuration
    rpct::fct::FebConnectivityTest &febconntest_ = febsystem_.getFebConnectivityTest();
    rpct::hwd::SnapshotType const &_snapshottype = febsystem_.getSnapshotType(febconntest_.getSnapshotName());
    // Get Snapshots and assume the first one
    std::vector<rpct::hwd::Snapshot> const _snapshots = febsystem_.getSnapshots(_snapshottype);
    std::vector<rpct::hwd::Snapshot>::const_iterator _previous = _snapshots.begin();
    febsystem_.assume(*_previous);

    { // Print Configuration
        *output_ << "================================================================================\n"
                 << " FebConnectivityTest Configuration\n"
                 << "================================================================================\n"
                 << "--------------------------------------------------------------------------------\n"
                 << " Selection\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "Tower: " << febsystem_.getTower() << std::endl;
        *output_ << std::setw(25) << "Selected Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = febconntest_.getRollSelection().getSelected().begin(); _roll != febconntest_.getRollSelection().getSelected().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << std::setw(25) << "Masked Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = febconntest_.getRollSelection().getMasked().begin(); _roll != febconntest_.getRollSelection().getMasked().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << std::setw(25) << "Include Disabled: " << (febconntest_.getIncludeDisabled() ? "yes" : "no") << std::endl;
        *output_ << "--------------------------------------------------------------------------------\n"
                 << " FEB Parameters\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "VTh Low: " << febconntest_.getVThLow() * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "VTh High: " << febconntest_.getVThHigh() * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "VMon: " << febconntest_.getVMon() * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "Auto Correct: " << (febconntest_.getAutoCorrect() ? "yes" : "no") << std::endl;
        *output_ << "--------------------------------------------------------------------------------\n"
                 << " LinkBoard Parameters\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "Count Duration: " << febconntest_.getCountDuration() << " s" << std::endl;
        *output_ << std::setw(25) << "Use Pulser: " << (febconntest_.getUsePulser() ? "yes" : "no") << std::endl;
        *output_ << "================================================================================\n"
                 << std::endl;

        *output_ << "================================================================================\n"
                 << " Analyser Configuration\n"
                 << "================================================================================\n"
                 << " Selection\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "Selected Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = roll_selection_.getSelected().begin(); _roll != roll_selection_.getSelected().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << std::setw(25) << "Masked Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = roll_selection_.getMasked().begin(); _roll != roll_selection_.getMasked().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "VTh Offset Problem:" << vth_offset_problem_ * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "VTh Offset Ignore: " << vth_offset_ignore_ * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "Rate Threshold Active: " << rate_active_ << " Hz" << std::endl;
        *output_ << std::setw(25) << "Rate Threshold Noisy: " << rate_noisy_ << " Hz" << std::endl;
        *output_ << std::setw(25) << "Connector Fraction: " << channel_fraction_ << std::endl;
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    // Open ROOT File
    TFile root_file_((filename_base_ + ".root").c_str(), "RECREATE");
    TDirectory *working_directory_ = root_file_.mkdir("working_directory");
    working_directory_->cd();
    TangoColors::createColors();
    std::stringstream name_, title_;

    { // Prepare Histogram Stacks
        // Style Histograms
        TH1F *_linkboard_style_histogram = new TH1F("LinkBoardHistogramStyle", "LinkBoardHistogramStyle", 120, .5, 20.5);
        _linkboard_style_histogram->GetXaxis()->SetTitle("LinkBoard in LinkBox");
        _linkboard_style_histogram->GetYaxis()->SetTitle("Average Count per LinkBoard Connector");
        _linkboard_style_histogram->SetStats(false);
        _linkboard_style_histogram->Write();

        TH1F *_channel_style_histogram = new TH1F("ChannelHistogramStyle", "ChannelHistogramStyle", 96, -.5, 95.5);
        _channel_style_histogram->GetXaxis()->SetTitle("Channel in LinkBoard");
        _channel_style_histogram->GetYaxis()->SetTitle("Rate per Channel (Hz)");
        _channel_style_histogram->SetStats(false);
        _channel_style_histogram->Write();

        TH1F *_strip_style_histogram = new TH1F("StripHistogramStyle", "StripHistogramStyle", 96, .5, 96.5);
        _strip_style_histogram->GetXaxis()->SetTitle("Strip in Roll");
        _strip_style_histogram->GetYaxis()->SetTitle("Rate per Strip (Hz)");
        _strip_style_histogram->SetStats(false);
        _strip_style_histogram->Write();

        // Stacks and Legends
        std::vector<rpct::fct::LinkBox *> _linkboxes = febsystem_.getLinkBoxes();
        for (std::vector<rpct::fct::LinkBox *>::const_iterator _linkbox_it = _linkboxes.begin(); _linkbox_it != _linkboxes.end(); ++_linkbox_it)
        {
            rpct::fct::LinkBox &_linkbox = *(*_linkbox_it);
            bool _linkbox_selected(false);
            for (std::vector<rpct::fct::LinkBoard *>::const_iterator _linkboard_it = _linkbox.getLinkBoards().begin(); _linkboard_it != _linkbox.getLinkBoards().end(); ++_linkboard_it)
                if (*_linkboard_it)
                {
                    rpct::fct::LinkBoard &_linkboard = *(*_linkboard_it);
                    // loop over febconnectors
                    bool _linkboard_selected(false);
                    for (std::vector<rpct::fct::FebConnector *>::const_iterator _febconnector_it = _linkboard.getFebConnectors().begin(); _febconnector_it != _linkboard.getFebConnectors().end(); ++_febconnector_it)
                        if (*_febconnector_it && (*_febconnector_it)->isSelected())
                            if (roll_selection_.matches((*_febconnector_it)->getRoll()))
                            {
                                _linkboard_selected = true;
                                rpct::fct::FebConnector &_febconnector = *(*_febconnector_it);
                                if (strip_stacks_.find(_febconnector.getRoll()) == strip_stacks_.end())
                                {
                                    name_.str("");
                                    name_ << "RPCDetId_" << _febconnector.getRoll().getRPCDetId().id();
                                    THStack *_strip_stack = (strip_stacks_[_febconnector.getRoll()] = new THStack(name_.str().c_str(), _febconnector.getRoll().name().c_str()));
                                    _strip_stack->SetHistogram(_strip_style_histogram);
                                    _strip_stack->SetMinimum(.9);
                                    _strip_stack->SetMaximum(5e7);
                                    TLegend *_strip_legend = (strip_legends_[_febconnector.getRoll()] = new TLegend(.50, .15, .85, .35));
                                    _strip_legend->SetFillColor(0);
                                }
                            }
                    if (_linkboard_selected)
                    {
                        _linkbox_selected = true;
                        name_.str("");
                        name_ << _linkboard.getLinkBox().getName() << "_LB" << _linkboard.getPosition();
                        title_.str("");
                        title_ << _linkboard.getLinkBox().getName() << " LB" << _linkboard.getPosition()
                               << " / " << _linkboard.getLinkBox().getLocation();
                        THStack *_channel_stack = (channel_stacks_[_linkboard.getId()] = new THStack(name_.str().c_str(), title_.str().c_str()));
                        _channel_stack->SetHistogram(_channel_style_histogram);
                        _channel_stack->SetMinimum(.9);
                        _channel_stack->SetMaximum(5e7);
                        TLegend *_channel_legend = (channel_legends_[_linkboard.getId()] = new TLegend(.50, .15, .85, .35));
                        _channel_legend->SetFillColor(0);
                    }
                }
            if (_linkbox_selected)
            {
                title_.str("");
                title_ << _linkbox.getName() << " / " << _linkbox.getLocation();
                THStack *_linkboard_stack = (linkboard_stacks_[_linkbox.getId()] = new THStack(_linkbox.getName().c_str(), title_.str().c_str()));
                _linkboard_stack->SetHistogram(_linkboard_style_histogram);
                _linkboard_stack->SetMinimum(.9);
                _linkboard_stack->SetMaximum(5e7);
                TLegend *_linkboard_legend = (linkboard_legends_[_linkbox.getId()] = new TLegend(.35, .15, .85, .35));
                _linkboard_legend->SetFillColor(0);
                _linkboard_legend->SetNColumns(2);
            }
        }
    }

    { // First snapshot, already assumed
        std::vector<rpct::fct::LinkBox *> _linkboxes = febsystem_.getLinkBoxes();
        for (std::vector<rpct::fct::LinkBox *>::const_iterator _linkbox_it = _linkboxes.begin(); _linkbox_it != _linkboxes.end(); ++_linkbox_it)
            if (*_linkbox_it && (*_linkbox_it)->isActive())
                for (std::vector<rpct::fct::LinkBoard *>::const_iterator _linkboard_it = (*_linkbox_it)->getLinkBoards().begin(); _linkboard_it != (*_linkbox_it)->getLinkBoards().end(); ++_linkboard_it)
                    if (*_linkboard_it)
                    {
                        rpct::fct::LinkBoard &_linkboard = *(*_linkboard_it);
                        for (std::vector<rpct::fct::FebConnector *>::const_iterator _febconnector_it = _linkboard.getFebConnectors().begin(); _febconnector_it != _linkboard.getFebConnectors().end(); ++_febconnector_it)
                            if (*_febconnector_it && roll_selection_.matches((*_febconnector_it)->getRoll()))
                            {
                                rpct::fct::FebConnector const &_febconnector = *(*_febconnector_it);
                                bool _ignore_connector(false);
                                { // offset?
                                    rpct::fct::FebChip const &_febchip_0 = _febconnector.getFebPart().getFebChip(0);
                                    rpct::fct::FebChip const &_febchip_1 = _febconnector.getFebPart().getFebChip(1);

                                    if (std::abs((int)_febchip_0.getVThRead() - (int)_febchip_0.getVThSet()) > vth_offset_ignore_)
                                    {
                                        _ignore_connector = true;
                                        offset_chip_ignore_.insert(std::pair<rpct::hwd::integer_type, int>(_febchip_0.getId(), (int)_febchip_0.getVThRead() - _febchip_0.getVThSet()));
                                    }
                                    if (std::abs((int)_febchip_1.getVThRead() - (int)_febchip_1.getVThSet()) > vth_offset_ignore_)
                                    {
                                        _ignore_connector = true;
                                        offset_chip_ignore_.insert(std::pair<rpct::hwd::integer_type, int>(_febchip_1.getId(), (int)_febchip_1.getVThRead() - _febchip_1.getVThSet()));
                                    }
                                    if (_ignore_connector)
                                        offset_connector_ignore_index_.insert(_febconnector.getId());
                                }

                                if (!_ignore_connector)
                                { // noisy?
                                    int _channel = (_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                                    int _noisy_channels(0);
                                    for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                                        if (_febconnector.getStrips().isPinActive(_pin) && _linkboard.getRate(_channel) > rate_noisy_)
                                            ++_noisy_channels;
                                    if ((double)_noisy_channels / (double)_febconnector.getStrips().getNStrips() > channel_fraction_)
                                    {
                                        noisy_connector_ignore_.push_back(_febconnector.getId());
                                        noisy_connector_ignore_index_.insert(_febconnector.getId());
                                    }
                                }
                            }
                    }
    }

    int _nsnapshots(_snapshots.size()), _nsnapshot(1);
    // Loop over Snapshots
    for (std::vector<rpct::hwd::Snapshot>::const_iterator _snapshot = _snapshots.begin(); _snapshot != _snapshots.end(); ++_snapshot, ++_nsnapshot)
    {
        // Load the next snapshot
        febsystem_.assumeChanges(*_snapshot, *_previous);
        _previous = _snapshot;

        LOG4CPLUS_INFO(logger_, "Snapshot " << _nsnapshot << "/" << _nsnapshots << ": " << rpct::tools::Date(_snapshot->getTime()));
        TDirectory *_directory = working_directory_->mkdir(rpct::tools::Date(_snapshot->getTime()).filename().c_str());
        _directory->cd();

        int _linkboard_color(20000), _channel_color(20000), _strip_color(20000);

        if (_nsnapshot > 1 ) // skip the first connectivitytest step
        {
            if (febconntest_.getActiveFebConnectorId() 
                && roll_selection_.matches(febconntest_.getActiveFebConnector().getRoll()))
            {
                rpct::fct::FebConnector &_active_febconnector = febconntest_.getActiveFebConnector();
                rpct::tools::RollId _active_roll = _active_febconnector.getRoll();
                rpct::tools::RollId _active_chamber = _active_roll.getChamberId();
                rpct::fct::LinkBoard &_active_linkboard = _active_febconnector.getLinkBoard();
                rpct::fct::LinkBox &_active_linkbox = _active_linkboard.getLinkBox();
                bool _active_is_dead(false);

                { // offset?
                    rpct::fct::FebChip const &_febchip_0 = _active_febconnector.getFebPart().getFebChip(0);
                    rpct::fct::FebChip const &_febchip_1 = _active_febconnector.getFebPart().getFebChip(1);

                    if (offset_chip_ignore_.find(_febchip_0.getId()) == offset_chip_ignore_.end() && std::abs((int)_febchip_0.getVThRead() - (int)_febchip_0.getVThSet()) > vth_offset_problem_)
                        offset_chip_problem_.insert(std::pair<rpct::hwd::integer_type, int>(_febchip_0.getId(), (int)_febchip_0.getVThRead() - _febchip_0.getVThSet()));
                    if (offset_chip_ignore_.find(_febchip_1.getId()) == offset_chip_ignore_.end() && std::abs((int)_febchip_1.getVThRead() - (int)_febchip_1.getVThSet()) > vth_offset_problem_)
                        offset_chip_problem_.insert(std::pair<rpct::hwd::integer_type, int>(_febchip_1.getId(), (int)_febchip_1.getVThRead() - _febchip_1.getVThSet()));
                }

                { // active?
                    int _channel = (_active_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                    int _active_channels[2] = {0, 0};
                    for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                        if (_active_febconnector.getStrips().isPinActive(_pin) && _active_linkboard.getRate(_channel) > rate_active_)
                            ++(_active_channels[(_pin - 1) / (rpct::fct::FebConnectorStrips::npins_ / 2)]);

                    if ((double)(_active_channels[0] + _active_channels[1]) / (double)_active_febconnector.getStrips().getNStrips() < channel_fraction_ && offset_connector_ignore_index_.find(_active_febconnector.getId()) == offset_connector_ignore_index_.end())
                    {
                        dead_connector_.push_back(_active_febconnector.getId());
                        _active_is_dead = true;
                    }
                    else if (((double)(_active_channels[0]) / (double)_active_febconnector.getStrips().getNStrips() < channel_fraction_ || (double)(_active_channels[1]) / (double)_active_febconnector.getStrips().getNStrips() < channel_fraction_) && offset_connector_ignore_index_.find(_active_febconnector.getId()) == offset_connector_ignore_index_.end())
                        partially_dead_connector_.push_back(_active_febconnector.getId());
                }

                _strip_color = _active_febconnector.getRollConnector() + 20000;
                _channel_color = _active_febconnector.getPosition() + 20000;
                _linkboard_color = _active_linkboard.getPosition() + 20000;

                // Indicator Histograms and LinkBoard Histogram
                if (linkboard_histograms_.find(_active_linkboard.getId()) == linkboard_histograms_.end())
                {
                    name_.str("");
                    name_ << _active_linkbox.getName() << "_LB" << _active_linkboard.getPosition() << "_ind";
                    title_.str("");
                    title_ << "LB" << _active_linkboard.getPosition()
                           << "CB" << _active_febconnector.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getPosition()
                           << " J" << _active_febconnector.getFebPart().getFebBoard().getFebDistributionBoard().getPosition()
                           << " / " << _active_febconnector.getRoll();
                    TH1F *_linkboard_indicator_histogram = new TH1F(name_.str().c_str(), title_.str().c_str(), 120, .5, 20.5);
                    _linkboard_indicator_histogram->SetFillColor(_linkboard_color);
                    _linkboard_indicator_histogram->SetLineColor(_linkboard_color);
                    for (std::vector<rpct::fct::FebConnector *>::const_iterator _febconnector_it = _active_linkboard.getFebConnectors().begin(); _febconnector_it != _active_linkboard.getFebConnectors().end(); ++_febconnector_it)
                        if (*_febconnector_it && (*_febconnector_it)->isSelected())
                            _linkboard_indicator_histogram->SetBinContent((_active_linkboard.getPosition() - 1) * rpct::fct::LinkBoard::nfebconnectors_ + (*_febconnector_it)->getPosition(), 2);
                    _linkboard_indicator_histogram->Write();
                    linkboard_stacks_[_active_linkbox.getId()]->Add(_linkboard_indicator_histogram);
                    linkboard_legends_[_active_linkbox.getId()]->AddEntry(_linkboard_indicator_histogram, _linkboard_indicator_histogram->GetTitle(), "f");

                    name_.str("");
                    name_ << _active_linkbox.getName() << "_LB" << _active_linkboard.getPosition();
                    TH1F *_linkboard_histogram =
                        (linkboard_histograms_[_active_linkboard.getId()] = new TH1F(name_.str().c_str(), title_.str().c_str(), 120, .5, 20.5));
                    _linkboard_histogram->SetLineColor(_linkboard_color);
                    _linkboard_histogram->SetLineWidth(2);
                    linkboard_stacks_[_active_linkbox.getId()]->Add(_linkboard_histogram);
                }

                name_.str("");
                title_.str("");
                name_ << _active_linkboard.getName() << "_" << _active_febconnector.getPosition() << "_ind";
                title_ << "LB C" << _active_febconnector.getPosition() << " - " << _active_roll << " C" << _active_febconnector.getRollConnector();
                TH1F *_channel_indicator_histogram = new TH1F(name_.str().c_str(), title_.str().c_str(), 96, -.5, 95.5);
                _channel_indicator_histogram->SetFillColor(_channel_color);
                _channel_indicator_histogram->SetLineColor(_channel_color);
                channel_stacks_[_active_linkboard.getId()]->Add(_channel_indicator_histogram);
                channel_legends_[_active_linkboard.getId()]->AddEntry(_channel_indicator_histogram, _channel_indicator_histogram->GetTitle(), "f");

                name_.str("");
                title_.str("");
                name_ << "RPCDetId_" << _active_roll.getRPCDetId().id() << "_" << _active_febconnector.getRollConnector() << "_ind";
                title_ << "RPC C" << _active_febconnector.getRollConnector() << " - " << _active_linkbox.getName() << " LB" << _active_linkboard.getPosition() << " C" << _active_febconnector.getPosition();
                TH1F *_strip_indicator_histogram = new TH1F(name_.str().c_str(), title_.str().c_str(), 96, .5, 96.5);
                _strip_indicator_histogram->SetFillColor(_strip_color);
                _strip_indicator_histogram->SetLineColor(_strip_color);
                strip_stacks_[_active_roll]->Add(_strip_indicator_histogram);
                strip_legends_[_active_roll]->AddEntry(_strip_indicator_histogram, _strip_indicator_histogram->GetTitle(), "f");

                // Fill the indicator histograms
                int _channel = (_active_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                    if (_active_febconnector.getStrips().isPinActive(_pin))
                    {
                        _channel_indicator_histogram->SetBinContent(_channel + 1, 2);
                        _strip_indicator_histogram->SetBinContent(_active_febconnector.getStrips().getStrip(_pin), 2);
                    }

                _channel_indicator_histogram->Write();
                _strip_indicator_histogram->Write();

                // Fill Histograms
                TH1F *_linkboard_histogram = linkboard_histograms_[_active_linkboard.getId()];

                for (std::vector<rpct::fct::LinkBoard *>::const_iterator _linkboard_it = _active_linkbox.getLinkBoards().begin(); _linkboard_it != _active_linkbox.getLinkBoards().end(); ++_linkboard_it)
                    if (*_linkboard_it)
                        for (std::vector<rpct::fct::FebConnector *>::const_iterator _febconnector_it = (*_linkboard_it)->getFebConnectors().begin(); _febconnector_it != (*_linkboard_it)->getFebConnectors().end(); ++_febconnector_it)
                            if (*_febconnector_it)
                            {
                                rpct::fct::FebConnector const &_febconnector(*(*_febconnector_it));
                                if (_febconnector.isSelected())
                                    _linkboard_histogram->AddBinContent(((*_linkboard_it)->getPosition() - 1) * rpct::fct::LinkBoard::nfebconnectors_ + (*_febconnector_it)->getPosition(), (*_linkboard_it)->getAvgRate((*_febconnector_it)->getPosition()));
                                if (_febconnector.getRoll().getChamberId() == _active_chamber)
                                {
                                    if (strip_histograms_.find(_febconnector.getRoll()) == strip_histograms_.end())
                                    {
                                        name_.str("");
                                        name_ << "RPCDetId_" << _febconnector.getRoll().getRPCDetId().id();
                                        TH1F *_strip_histogram = (strip_histograms_[_febconnector.getRoll()] = new TH1F(name_.str().c_str(), _active_roll.name().c_str(), 96, .5, 96.5));
                                        _strip_histogram->SetLineColor(_strip_color);
                                        _strip_histogram->SetLineWidth(2);
                                        strip_stacks_[_febconnector.getRoll()]->Add(_strip_histogram);
                                    }
                                    TH1F *_strip_histogram = strip_histograms_[_febconnector.getRoll()];
                                    // Fill the histogram
                                    int _channel = (_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                                    for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                                        if (_febconnector.getStrips().isPinActive(_pin))
                                            _strip_histogram->SetBinContent(_febconnector.getStrips().getStrip(_pin), _febconnector.getLinkBoard().getRate(_channel));
                                }
                                if (_febconnector != _active_febconnector)
                                { // active or noisy?
                                    int _channel = (_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                                    int _active_channels(0), _noisy_channels(0);
                                    for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                                        if (_febconnector.getStrips().isPinActive(_pin))
                                        {
                                            if (_febconnector.getLinkBoard().getRate(_channel) > rate_active_)
                                                ++_active_channels;
                                            if (_febconnector.getLinkBoard().getRate(_channel) > rate_noisy_)
                                                ++_noisy_channels;
                                        }

                                    if (_active_is_dead && (double)_active_channels / (double)_febconnector.getStrips().getNStrips() > channel_fraction_ && offset_connector_ignore_index_.find(_febconnector.getId()) == offset_connector_ignore_index_.end() && noisy_connector_ignore_index_.find(_febconnector.getId()) == noisy_connector_ignore_index_.end())
                                        swap_connectors_[_active_febconnector.getId()].push_back(_febconnector.getId());

                                    if ((double)_noisy_channels / (double)_febconnector.getStrips().getNStrips() > channel_fraction_ && offset_connector_ignore_index_.find(_febconnector.getId()) == offset_connector_ignore_index_.end())
                                        noisy_connector_problem_.push_back(_febconnector.getId());
                                }
                            }
                for (std::map<rpct::tools::RollId, TH1F *>::iterator _strip_histogram_it = strip_histograms_.begin(); _strip_histogram_it != strip_histograms_.end(); ++_strip_histogram_it)
                    _strip_histogram_it->second->Write();
                strip_histograms_.clear();

                // Channel Histogram
                TH1F *_channel_histogram = new TH1F(_active_linkboard.getName().c_str(), _active_linkboard.getName().c_str(), 96, -.5, 95.5);
                _channel_histogram->SetLineColor(_channel_color);
                _channel_histogram->SetLineWidth(2);
                for (std::size_t _channel = 0; _channel < rpct::fct::LinkBoard::nchannels_; ++_channel)
                    _channel_histogram->SetBinContent(_channel + 1, _active_linkboard.getRate(_channel));
                _channel_histogram->Write();
                channel_stacks_[_active_linkboard.getId()]->Add(_channel_histogram);
            }
        }
    }
    for (std::map<rpct::hwd::integer_type, TH1F *>::iterator _linkboard_histogram_it = linkboard_histograms_.begin(); _linkboard_histogram_it != linkboard_histograms_.end(); ++_linkboard_histogram_it)
        _linkboard_histogram_it->second->Write();
    linkboard_histograms_.clear();

    // Create pdf file
    TCanvas _canvas("DummyCanvas");
    std::string _pdf_filename = filename_base_ + ".pdf[";
    _canvas.Print(_pdf_filename.c_str());
    _pdf_filename = filename_base_ + ".pdf";

    { // Print Configuration
        TCanvas *_canvas = new TCanvas("Configuration");
        TPaveText *_configuration = new TPaveText(.05, .05, .95, .95);
        std::stringstream _configuration_ss;
        _configuration->SetFillColor(0);
        _configuration->SetBorderSize(0);
        _configuration->SetTextAlign(11);

        _configuration->AddText("========================================");
        _configuration->AddText(" FebConnectivityTest Configuration");
        _configuration->AddText(rpct::tools::Date(_snapshots.front().getTime()).name().c_str());
        _configuration->AddText("========================================");
        _configuration->AddText("----------------------------------------");
        _configuration->AddText(" Selection");
        _configuration->AddText("----------------------------------------");
        _configuration->AddText((std::string("Tower: ") + febsystem_.getTower()).c_str());
        _configuration->AddText("Selected Rolls: ");
        for (std::set<rpct::tools::RollId>::const_iterator _roll = febconntest_.getRollSelection().getSelected().begin(); _roll != febconntest_.getRollSelection().getSelected().end(); ++_roll)
            _configuration->AddText((std::string(" - ") + _roll->name()).c_str());
        if (!febconntest_.getRollSelection().getMasked().empty())
        {
            _configuration->AddText("Masked Rolls: ");
            for (std::set<rpct::tools::RollId>::const_iterator _roll = febconntest_.getRollSelection().getMasked().begin(); _roll != febconntest_.getRollSelection().getMasked().end(); ++_roll)
                _configuration->AddText((std::string(" - ") + _roll->name()).c_str());
        }
        _configuration->AddText((std::string("Include Disabled: ") + (febconntest_.getIncludeDisabled() ? "yes" : "no")).c_str());

        _configuration->AddText("----------------------------------------");
        _configuration->AddText(" FEB Parameters");
        _configuration->AddText("----------------------------------------");
        _configuration_ss.str("");
        _configuration_ss << "VTh Low: " << febconntest_.getVThLow() * 2500. / 1024. << " mV";
        _configuration->AddText(_configuration_ss.str().c_str());
        _configuration_ss.str("");
        _configuration_ss << "VTh High: " << febconntest_.getVThHigh() * 2500. / 1024. << " mV" << std::endl;
        _configuration->AddText(_configuration_ss.str().c_str());
        _configuration_ss.str("");
        _configuration_ss << "VMon: " << febconntest_.getVMon() * 2500. / 1024. << " mV" << std::endl;
        _configuration->AddText(_configuration_ss.str().c_str());
        _configuration->AddText((std::string("Auto Correct: ") + (febconntest_.getAutoCorrect() ? "yes" : "no")).c_str());

        _configuration->AddText("----------------------------------------");
        _configuration->AddText("LinkBoard Parameters");
        _configuration->AddText("----------------------------------------");
        _configuration_ss.str("");
        _configuration_ss << "Count Duration: " << febconntest_.getCountDuration() << " s" << std::endl;
        _configuration->AddText(_configuration_ss.str().c_str());
        _configuration->AddText((std::string("Use Pulser: ") + (febconntest_.getUsePulser() ? "yes" : "no")).c_str());
        _configuration->AddText("========================================");

        _configuration->Draw();
        _canvas->Write();
        _canvas->Print(_pdf_filename.c_str());
    }

    for (std::map<rpct::hwd::integer_type, THStack *>::iterator _linkboard_stack = linkboard_stacks_.begin(); _linkboard_stack != linkboard_stacks_.end(); ++_linkboard_stack)
    {
        working_directory_->cd();
        _linkboard_stack->second->Write();
        linkboard_legends_[_linkboard_stack->first]->Write();
        root_file_.cd();
        TCanvas *_canvas = new TCanvas(_linkboard_stack->second->GetName());
        _canvas->SetLogy();
        _canvas->SetGridy();
        _linkboard_stack->second->Draw("nostack");
        linkboard_legends_[_linkboard_stack->first]->Draw();
        _canvas->Write();
        _canvas->Print(_pdf_filename.c_str());
    }
    linkboard_stacks_.clear();
    linkboard_legends_.clear();

    for (std::map<rpct::hwd::integer_type, THStack *>::iterator _channel_stack = channel_stacks_.begin(); _channel_stack != channel_stacks_.end(); ++_channel_stack)
    {
        working_directory_->cd();
        _channel_stack->second->Write();
        channel_legends_[_channel_stack->first]->Write();
        root_file_.cd();
        TCanvas *_canvas = new TCanvas(_channel_stack->second->GetName());
        _canvas->SetLogy();
        _canvas->SetGridy();
        _channel_stack->second->Draw("nostack");
        channel_legends_[_channel_stack->first]->Draw();
        _canvas->Write();
        _canvas->Print(_pdf_filename.c_str());
    }
    channel_stacks_.clear();
    channel_legends_.clear();

    for (std::map<rpct::tools::RollId, THStack *>::iterator _strip_stack = strip_stacks_.begin(); _strip_stack != strip_stacks_.end(); ++_strip_stack)
    {
        working_directory_->cd();
        _strip_stack->second->Write();
        strip_legends_[_strip_stack->first]->Write();
        root_file_.cd();
        TCanvas *_canvas = new TCanvas(_strip_stack->second->GetName());
        _canvas->SetLogy();
        _canvas->SetGridy();
        _strip_stack->second->Draw("nostack");
        strip_legends_[_strip_stack->first]->Draw();
        _canvas->Write();
        _canvas->Print(_pdf_filename.c_str());
    }
    strip_stacks_.clear();
    strip_legends_.clear();

    _pdf_filename = filename_base_ + ".pdf]";
    _canvas.Print(_pdf_filename.c_str());

    root_file_.Close();

    if (!(offset_chip_ignore_.empty()))
    {
        *output_ << "================================================================================\n"
                 << " Ignored Chips (VTh Offset at start)\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll"
                 << std::setw(11) << "Chip"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "CB"
                 << std::setw(4) << "J"
                 << std::setw(11) << "ID"
                 << std::setw(11) << "Offset" << std::endl;
        for (std::map<rpct::hwd::integer_type, int>::const_iterator _chip = offset_chip_ignore_.begin(); _chip != offset_chip_ignore_.end(); ++_chip)
        {
            rpct::fct::FebChip const &_febchip = (rpct::fct::FebChip const &)(febsystem_.getDevice(_chip->first));
            *output_ << std::setw(25) << _febchip.getFebPart().getFebConnector().getRoll()
                     << " " << std::setw(10) << _febchip.getPosition()
                     << " " << std::setw(10) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getLinkBox().getLocation()
                     << " " << std::setw(3) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getPosition()
                     << " " << std::setw(3) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getPosition()
                     << " " << std::setw(10) << _febchip.getRPCId()
                     << " " << std::setw(10) << 2.5 / 1.024 * _chip->second << std::endl;
        }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(noisy_connector_ignore_.empty()))
    {
        std::sort(noisy_connector_ignore_.begin(), noisy_connector_ignore_.end(), febconnector_compare_);
        *output_ << "================================================================================\n"
                 << " Ignored Connectors (Noise at start)\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll_Connector"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "LB"
                 << std::setw(4) << "Pos" << std::endl;
        for (std::vector<rpct::hwd::integer_type>::const_iterator _connector = noisy_connector_ignore_.begin(); _connector != noisy_connector_ignore_.end(); ++_connector)
        {
            rpct::fct::FebConnector const &_febconnector = (rpct::fct::FebConnector const &)(febsystem_.getDevice(*_connector));
            *output_ << std::setw(22) << _febconnector.getRoll() << "_" << std::setw(2) << std::setfill('0') << _febconnector.getRollConnector() << std::setfill(' ')
                     << " " << std::setw(10) << _febconnector.getLinkBoard().getLinkBox().getLocation()
                     << " " << std::setw(3) << _febconnector.getLinkBoard().getPosition()
                     << " " << std::setw(3) << _febconnector.getPosition() << std::endl;
        }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(offset_chip_problem_.empty()))
    {
        *output_ << "================================================================================\n"
                 << " Chips with VTh Offset problem\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll"
                 << std::setw(11) << "Chip"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "CB"
                 << std::setw(4) << "J"
                 << std::setw(11) << "ID"
                 << std::setw(11) << "Offset" << std::endl;
        for (std::map<rpct::hwd::integer_type, int, FebChipCompare>::const_iterator _chip = offset_chip_problem_.begin(); _chip != offset_chip_problem_.end(); ++_chip)
        {
            rpct::fct::FebChip const &_febchip = (rpct::fct::FebChip const &)(febsystem_.getDevice(_chip->first));
            *output_ << std::setw(25) << _febchip.getFebPart().getFebConnector().getRoll()
                     << " " << std::setw(10) << _febchip.getPosition()
                     << " " << std::setw(10) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getLinkBox().getLocation()
                     << " " << std::setw(3) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getPosition()
                     << " " << std::setw(3) << _febchip.getFebPart().getFebBoard().getFebDistributionBoard().getPosition()
                     << " " << std::setw(10) << _febchip.getRPCId()
                     << " " << std::setw(10) << 2.5 / 1.024 * _chip->second << std::endl;
        }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(noisy_connector_problem_.empty()))
    {
        std::sort(noisy_connector_problem_.begin(), noisy_connector_problem_.end(), febconnector_compare_);
        *output_ << "================================================================================\n"
                 << " Connectors with Noise problem\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll_Connector"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "LB"
                 << std::setw(4) << "Pos" << std::endl;
        for (std::vector<rpct::hwd::integer_type>::const_iterator _connector = noisy_connector_problem_.begin(); _connector != noisy_connector_problem_.end(); ++_connector)
        {
            rpct::fct::FebConnector const &_febconnector = (rpct::fct::FebConnector const &)(febsystem_.getDevice(*_connector));
            *output_ << std::setw(22) << _febconnector.getRoll() << "_" << std::setw(2) << std::setfill('0') << _febconnector.getRollConnector() << std::setfill(' ')
                     << " " << std::setw(10) << _febconnector.getLinkBoard().getLinkBox().getLocation()
                     << " " << std::setw(3) << _febconnector.getLinkBoard().getPosition()
                     << " " << std::setw(3) << _febconnector.getPosition() << std::endl;
        }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(dead_connector_.empty()) && dead_connector_.size() != swap_connectors_.size())
    {
        std::sort(dead_connector_.begin(), dead_connector_.end(), febconnector_compare_);
        *output_ << "================================================================================\n"
                 << " Quiet Connectors\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll_Connector"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "LB"
                 << std::setw(4) << "Pos" << std::endl;
        for (std::vector<rpct::hwd::integer_type>::const_iterator _connector = dead_connector_.begin(); _connector != dead_connector_.end(); ++_connector)
            if (swap_connectors_.find(*_connector) == swap_connectors_.end())
            {
                rpct::fct::FebConnector const &_febconnector = (rpct::fct::FebConnector const &)(febsystem_.getDevice(*_connector));
                *output_ << std::setw(22) << _febconnector.getRoll() << "_" << std::setw(2) << std::setfill('0') << _febconnector.getRollConnector() << std::setfill(' ')
                         << " " << std::setw(10) << _febconnector.getLinkBoard().getLinkBox().getLocation()
                         << " " << std::setw(3) << _febconnector.getLinkBoard().getPosition()
                         << " " << std::setw(3) << _febconnector.getPosition() << std::endl;
            }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(partially_dead_connector_.empty()))
    {
        std::sort(partially_dead_connector_.begin(), partially_dead_connector_.end(), febconnector_compare_);
        *output_ << "================================================================================\n"
                 << " Partially Quiet Connectors\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(25) << "Roll_Connector"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "LB"
                 << std::setw(4) << "Pos" << std::endl;
        for (std::vector<rpct::hwd::integer_type>::const_iterator _connector = partially_dead_connector_.begin(); _connector != partially_dead_connector_.end(); ++_connector)
            if (swap_connectors_.find(*_connector) == swap_connectors_.end())
            {
                rpct::fct::FebConnector const &_febconnector = (rpct::fct::FebConnector const &)(febsystem_.getDevice(*_connector));
                *output_ << std::setw(22) << _febconnector.getRoll() << "_" << std::setw(2) << std::setfill('0') << _febconnector.getRollConnector() << std::setfill(' ')
                         << " " << std::setw(10) << _febconnector.getLinkBoard().getLinkBox().getLocation()
                         << " " << std::setw(3) << _febconnector.getLinkBoard().getPosition()
                         << " " << std::setw(3) << _febconnector.getPosition() << std::endl;
            }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (!(swap_connectors_.empty()))
    {
        *output_ << "================================================================================\n"
                 << " Quiet or swapped Connectors\n"
                 << "================================================================================" << std::endl;
        *output_ << std::setw(30) << "Roll_Connector"
                 << std::setw(11) << "LBB"
                 << std::setw(4) << "LB"
                 << std::setw(4) << "Pos"
                 << std::setw(4) << "CB"
                 << std::setw(4) << "J" << std::endl;
        for (std::map<rpct::hwd::integer_type, std::vector<rpct::hwd::integer_type>>::iterator swap_connectors_it = swap_connectors_.begin(); swap_connectors_it != swap_connectors_.end(); ++swap_connectors_it)
        {
            rpct::fct::FebConnector const &_source = (rpct::fct::FebConnector const &)(febsystem_.getDevice(swap_connectors_it->first));
            *output_ << std::setw(27) << _source.getRoll() << "_" << std::setw(2) << std::setfill('0') << _source.getRollConnector() << std::setfill(' ')
                     << " " << std::setw(10) << _source.getLinkBoard().getLinkBox().getLocation()
                     << " " << std::setw(3) << _source.getLinkBoard().getPosition()
                     << " " << std::setw(3) << _source.getPosition()
                     << " " << std::setw(3) << _source.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getPosition()
                     << " " << std::setw(3) << _source.getFebPart().getFebBoard().getFebDistributionBoard().getPosition() << std::endl;
            std::sort(swap_connectors_it->second.begin(), swap_connectors_it->second.end(), febconnector_compare_);
            for (std::vector<rpct::hwd::integer_type>::const_iterator _candidates_it = swap_connectors_it->second.begin(); _candidates_it != swap_connectors_it->second.end(); ++_candidates_it)
            {
                rpct::fct::FebConnector const &_target = (rpct::fct::FebConnector const &)(febsystem_.getDevice(*_candidates_it));
                *output_ << "-> " << std::setw(24) << _target.getRoll() << "_" << std::setw(2) << std::setfill('0') << _target.getRollConnector() << std::setfill(' ')
                         << " " << std::setw(10) << _target.getLinkBoard().getLinkBox().getLocation()
                         << " " << std::setw(3) << _target.getLinkBoard().getPosition()
                         << " " << std::setw(3) << _target.getPosition()
                         << " " << std::setw(3) << _target.getFebPart().getFebBoard().getFebDistributionBoard().getControlBoard().getPosition()
                         << " " << std::setw(3) << _target.getFebPart().getFebBoard().getFebDistributionBoard().getPosition() << std::endl;
            }
            *output_ << std::endl;
        }
        *output_ << "================================================================================\n"
                 << std::endl;
    }

    if (output_file_)
    {
        output_file_->close();
        delete output_file_;
    }

    exit(EXIT_SUCCESS);
}
