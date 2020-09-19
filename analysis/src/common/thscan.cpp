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
#include "rpct/FebConnectivityTest/FebThresholdScan.h"

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

    // Get ConnectivityTest Configuration
    rpct::fct::FebThresholdScan &febthscan_ = febsystem_.getFebThresholdScan();
    rpct::hwd::SnapshotType const &_snapshottype = febsystem_.getSnapshotType(febthscan_.getSnapshotName());

    // Get Snapshots and assume the first one
    std::vector<rpct::hwd::Snapshot> const _snapshots = febsystem_.getSnapshots(_snapshottype);
    std::vector<rpct::hwd::Snapshot>::const_iterator _previous = _snapshots.begin();
    febsystem_.assume(*_previous);

    { // Print Configuration
        *output_ << "================================================================================\n"
                 << " FebTHScanTest Configuration\n"
                 << "================================================================================\n"
                 << "--------------------------------------------------------------------------------\n"
                 << " Selection\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "Tower: " << febsystem_.getTower() << std::endl;
        *output_ << std::setw(25) << "Selected Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = febthscan_.getRollSelection().getSelected().begin(); _roll != febthscan_.getRollSelection().getSelected().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << std::setw(25) << "Masked Rolls: " << std::endl;
        for (std::set<rpct::tools::RollId>::const_iterator _roll = febthscan_.getRollSelection().getMasked().begin(); _roll != febthscan_.getRollSelection().getMasked().end(); ++_roll)
            *output_ << std::setw(25) << " " << *_roll << std::endl;
        *output_ << std::setw(25) << "Include Disabled: " << (febthscan_.getIncludeDisabled() ? "yes" : "no") << std::endl;
        *output_ << "--------------------------------------------------------------------------------\n"
                 << " FEB Parameters\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "VTh Min: " << febthscan_.getVThMin() * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "VTh N Steps: " << febthscan_.getVThNSteps() << " steps" << std::endl;
        *output_ << std::setw(25) << "VMon: " << febthscan_.getVMon() * 2500. / 1024. << " mV" << std::endl;
        *output_ << std::setw(25) << "Auto Correct: " << (febthscan_.getAutoCorrect() ? "yes" : "no") << std::endl;
        *output_ << "--------------------------------------------------------------------------------\n"
                 << " LinkBoard Parameters\n"
                 << "--------------------------------------------------------------------------------\n";
        *output_ << std::setw(25) << "Count Duration: " << febthscan_.getCountDuration() << " s" << std::endl;
        *output_ << std::setw(25) << "Use Pulser: " << (febthscan_.getUsePulser() ? "yes" : "no") << std::endl;
        *output_ << "================================================================================\n"
                 << std::endl;

    } // end print configuration

    int _nsnapshots = _snapshots.size();
    int _nsnapshot = 1;

    // Loop over Snapshots
    for (std::vector<rpct::hwd::Snapshot>::const_iterator _snapshot = _snapshots.begin(); _snapshot != _snapshots.end(); ++_snapshot, ++_nsnapshot)
    {
        // Load the next snapshot
        febsystem_.assumeChanges(*_snapshot, *_previous);
        _previous = _snapshot;

        LOG4CPLUS_INFO(logger_, "Snapshot " << _nsnapshot << "/" << _nsnapshots << ": " << rpct::tools::Date(_snapshot->getTime()));

        // skip the first connectivitytest step
        // needed ?
        if (_nsnapshot == 1)
            continue;

        // DEBUG: only the first snapshot
        if (_nsnapshot != 20)
            continue; //DEBUG

        std::vector<rpct::fct::LinkBox *> _linkboxes = febsystem_.getLinkBoxes();
        for (std::vector<rpct::fct::LinkBox *>::const_iterator _linkbox_it = _linkboxes.begin(); _linkbox_it != _linkboxes.end(); ++_linkbox_it)
        {
            if (*_linkbox_it && (*_linkbox_it)->isActive())
            {
                for (std::vector<rpct::fct::LinkBoard *>::const_iterator _linkboard_it = (*_linkbox_it)->getLinkBoards().begin(); _linkboard_it != (*_linkbox_it)->getLinkBoards().end(); ++_linkboard_it)
                {
                    if (*_linkboard_it)
                    {
                        rpct::fct::LinkBoard &_linkboard = *(*_linkboard_it);
                        for (std::vector<rpct::fct::FebConnector *>::const_iterator _febconnector_it = _linkboard.getFebConnectors().begin(); _febconnector_it != _linkboard.getFebConnectors().end(); ++_febconnector_it)
                        {
                            if (*_febconnector_it && roll_selection_.matches((*_febconnector_it)->getRoll()))
                            {
                                rpct::fct::FebConnector const &_febconnector = *(*_febconnector_it);
                                if (_febconnector.isSelected())
                                {
                                    rpct::fct::FebChip const &_febchip_0 = _febconnector.getFebPart().getFebChip(0);
                                    rpct::fct::FebChip const &_febchip_1 = _febconnector.getFebPart().getFebChip(1);

                                    std::cout << "Roll: " << _febconnector.getRoll().name() << 
                                        " - Connector Position: " << _febconnector.getPosition()-1 <<
                                        // " - _febchip_0: " << _febchip_0 <<
                                        // " - Avg Rate: " << _linkboard.getAvgRate(_febconnector.getPosition()) << std::endl;
                                        std::endl;

                                    // std::cout << "Roll Name: " << _febconnector.getRoll().name() << std::endl;
                                    // int _channel = (_febconnector.getPosition() - 1) * rpct::fct::FebConnectorStrips::npins_;
                                    // std::cout << "Channel: " << _channel << std::endl;
                                    // for (int _pin = 1; _pin <= rpct::fct::FebConnectorStrips::npins_; ++_pin, ++_channel)
                                    // {
                                    //     if (_febconnector.getStrips().isPinActive(_pin))
                                    //     {
                                    //         // if (_linkboard.getRate(_channel) != 0) {
                                    //         std::cout << "Pin: " << _pin << " - Rate: " << _linkboard.getRate(_channel) << std::endl;
                                    //         // }
                                    //     }
                                    // }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (output_file_)
    {
        output_file_->close();
        delete output_file_;
    }

    exit(EXIT_SUCCESS);
}
