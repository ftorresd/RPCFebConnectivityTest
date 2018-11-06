#include <unistd.h>

#include <cstdlib>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "rpct/tools/RPCDetId.h"
#include "rpct/tools/RollId.h"
#include "rpct/tools/RollSelection.h"

/** Print usage manual */
void manual(std::string const & _name)
{
    std::cerr << "\n"
              << "Usage: " << _name << " [-R RPCDetId] [-r RPCRollId]\n"
              << "       " << std::setw(_name.size()) << " " << "   [-n Name] [-d DB_Name]\n"
              << "       " << std::setw(_name.size()) << " " << "   [-l] [-L]\n"
              << "       " << std::setw(_name.size()) << " " << "   [-O output_filename]\n"
              << "Convert between RPC names and ids.  Options can be repeated.\n"
              << "\n"
              << "optional arguments:\n"
              << "  -R RPCDetId           from RPCDetId id\n"
              << "  -r RPCRollId          from Roll id\n"
              << "\n"
              << "  -n Name               from name (RE+1/2/36:A, RB1in/W+1/S04:fwd, RB4/W0/S4++:bwd)\n"
              << "                        Different parts can be take a wildcard (*)\n"
              << "                        but not sector and region at the same time.\n"
              << "                        The rolls are called bwd, mid, fwd / A, B, C\n"
              << "  -d DB_Name            from name as given in DB\n"
              << "\n"
              << "  -l                    list matching rolls\n"
              << "  -L                    list matching chambers\n"
              << "\n"
              << "  -O output_filename    set the filename for text output\n"
              << "                        default: - (stdout)\n"
              << std::endl;
}

int main(int _argc, char * _argv[])
{
    rpct::tools::RollSelection roll_selection_;
    bool list_(false), chambers_(false);

    std::ostream * output_ = &(std::cout);
    std::ofstream * output_file_(0);

    { // parse parameters
        if (_argc == 1) {
            manual(_argv[0]);
            exit(EXIT_FAILURE);
        }

        int _opt;
        std::stringstream _parser;
        uint32_t _id;
        rpct::tools::RollId _rollid;

        while ((_opt = ::getopt(_argc, _argv, "R:r:n:d:lLO:h")) != -1) {
            _parser.clear();
            _parser.str("");
            switch (_opt) {
            case 'R':
                _parser << ::optarg; _parser >> _id;
                try {
                    _rollid.set(rpct::tools::RPCDetId(_id));
                } catch(std::runtime_error const & _error) {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.select(_rollid);
                break;
            case 'r':
                _parser << ::optarg; _parser >> _id;
                try {
                    _rollid.id(_id);
                } catch(std::runtime_error const & _error) {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.select(_rollid);
                break;
            case 'n':
                try {
                    _rollid.parseName(::optarg);
                } catch(std::runtime_error const & _error) {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.select(_rollid);
                break;
            case 'd':
                try {
                    _rollid.parseDBName(::optarg);
                } catch(std::runtime_error const & _error) {
                    std::cerr << _error.what() << std::endl;
                    exit(EXIT_FAILURE);
                }
                roll_selection_.select(_rollid);
                break;
            case 'l':
                list_ = true;
                break;
            case 'L':
                list_ = true;
                chambers_ = true;
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

    *output_ << "==================================================\n"
             << std::setw(10) << "RPCRollId"
             << std::setw(11) << "RPCDetId"
             << std::setw(26) << (chambers_ ? "Chamber" : "Roll") << '\n'
             << "--------------------------------------------------" << std::endl;
    for (std::set<rpct::tools::RollId>::const_iterator _roll = roll_selection_.getSelected().begin()
             ; _roll != roll_selection_.getSelected().end() ; ++_roll)
        *output_ << std::setw(10) << _roll->id() << " " << std::setw(36) << *_roll << std::endl;

    if (!list_)
        {
            *output_ << "==================================================" << std::endl;
            exit(EXIT_SUCCESS);
        }

    *output_ << "--------------------------------------------------" << std::endl;
    // Barrel
    rpct::tools::RollId _candidate;
    _candidate.setRegion(0);
    if (roll_selection_.matches(_candidate))
        for (int _station = 1 ; _station <= 4 ; ++_station) {
            _candidate.setStation(_station);
            int _nlayers(_station > 2 ? 1 : 2);
            if (roll_selection_.matches(_candidate))
                for (int _layer = 1 ; _layer <= _nlayers ; ++_layer) {
                    _candidate.setLayer(_layer);
                    if (roll_selection_.matches(_candidate))
                        for (int _ring = -2 ; _ring <= 2 ; ++_ring) {
                            _candidate.setRing(_ring);
                            if (roll_selection_.matches(_candidate))
                                for (int _sector = 1 ; _sector <= 12 ; ++_sector) {
                                    _candidate.setSector(_sector);
                                    int _min_subsector(_station > 2 && !(_station == 4 && (_sector == 9 || _sector == 10)) ? 1 : 0);
                                    int _max_subsector(_station > 2 && !(_station == 4 && (_sector == 9 || _sector == 10)) ? 2 : 0);
                                    if (roll_selection_.matches(_candidate))
                                        for (int _subsector = _min_subsector ; _subsector <= _max_subsector ; ++_subsector) {
                                            _candidate.setSubSector(_subsector);
                                            int _max_subsubsector(_station == 4 && _sector == 4 ? 2 : 0);
                                            if (roll_selection_.matches(_candidate))
                                                for (int _subsubsector = 0 ; _subsubsector <= _max_subsubsector ; ++_subsubsector) {
                                                    _candidate.setSubSubSector(_subsubsector);
                                                    if ((_subsubsector == 0 || _subsubsector == _subsector)
                                                        && roll_selection_.matches(_candidate)) {
                                                        if (chambers_)
                                                            *output_ << std::setw(10) << _candidate.id() << " "
                                                                     << std::setw(10) << _candidate.getRPCDetId().id() << " "
                                                                     << std::setw(25) << _candidate << std::endl;
                                                        else
                                                            for (int _roll = 1 ; _roll <= 3 ; ++_roll) {
                                                                _candidate.setRoll(_roll);
                                                                if ((_roll != 2 || _candidate.reference())
                                                                    && roll_selection_.matches(_candidate))
                                                                    *output_ << std::setw(10) << _candidate.id() << " "
                                                                             << std::setw(10) << _candidate.getRPCDetId().id() << " "
                                                                             << std::setw(25) << _candidate << std::endl;
                                                            }
                                                        _candidate.setRoll();
                                                    }
                                                }
                                            _candidate.setSubSubSector();
                                        }
                                    _candidate.setSubSector();
                                }
                            _candidate.setSector();
                        }
                    _candidate.setRing();
                }
            _candidate.setLayer();
        }
    _candidate.setStation();

    // Endcap
    _candidate.setLayer(1);
    for (int _region = -1 ; _region <= 1 ; ++_region) {
        if (!_region)
            ++_region;
        _candidate.setRegion(_region);
        if (roll_selection_.matches(_candidate))
            for (int _station = 1 ; _station <= 4 ; ++_station) {
                _candidate.setStation(_station);
                if (roll_selection_.matches(_candidate))
                    for (int _ring = 2 ; _ring <= 3 ; ++_ring) {
                        _candidate.setRing(_ring);
                        if (roll_selection_.matches(_candidate))
                            for (int _sector = 1 ; _sector <= 6 ; ++_sector) {
                                _candidate.setSector(_sector);
                                if (roll_selection_.matches(_candidate))
                                    for (int _subsector = 0 ; _subsector <= 2 ; ++_subsector) {
                                        _candidate.setSubSector(_subsector);
                                        if (roll_selection_.matches(_candidate))
                                            for (int _subsubsector = 0 ; _subsubsector <= 1 ; ++_subsubsector) {
                                                _candidate.setSubSubSector(_subsubsector);
                                                if (roll_selection_.matches(_candidate)) {
                                                    if (chambers_)
                                                        *output_ << std::setw(10) << _candidate.id() << " "
                                                                 << std::setw(10) << _candidate.getRPCDetId().id() << " "
                                                                 << std::setw(25) << _candidate << std::endl;
                                                    else
                                                        for (int _roll = 1 ; _roll <= 3 ; ++_roll) {
                                                            _candidate.setRoll(_roll);
                                                            if (roll_selection_.matches(_candidate))
                                                                *output_ << std::setw(10) << _candidate.id() << " "
                                                                         << std::setw(10) << _candidate.getRPCDetId().id() << " "
                                                                         << std::setw(25) << _candidate << std::endl;
                                                        }
                                                    _candidate.setRoll();
                                                }
                                            }
                                        _candidate.setSubSubSector();
                                    }
                                _candidate.setSubSector();
                            }
                        _candidate.setSector();
                    }
                _candidate.setRing();
            }
        _candidate.setStation();
    }

    *output_ << "==================================================" << std::endl;

    if (output_file_) {
        output_file_->close();
        delete output_file_;
    }

    exit(EXIT_SUCCESS);
}
