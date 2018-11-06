/* output of
 * ./a.out rpct/tools/ RollId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Region re 1 -1 1 Station st 1 1 4 Layer la 1 1 2 Ring ri 1 -2 3 Sector se 1 1 12 SubSector su 1 0 2 SubSubSector ss 1 0 2 Roll ro 1 1 4 Gap gp 1 1 2
 */

#include "rpct/tools/RollId.h"

#include <ostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <cmath>

#include "rpct/tools/RPCDetId.h"

namespace rpct {
namespace tools {

RollId::RollId()
    : id_(0x0)
    , mask_(min_id_mask_)
{
    setDet();
    setSubDet();
    setSubDetType();
}

RollId::RollId(::uint32_t const & _id)
    : id_(_id)
    , mask_(0x0)
{
    id(_id);
}

RollId::RollId(int _region
               , int _station
               , int _layer
               , int _ring
               , int _sector
               , int _subsector
               , int _subsubsector
               , int _roll
               , int _gap)
    : id_(0x0)
    , mask_(min_id_mask_)
{
    setDet();
    setSubDet();
    setSubDetType();
    setRegion(_region);
    setStation(_station);
    setLayer(_layer);
    setRing(_ring);
    setSector(_sector);
    setSubSector(_subsector);
    setSubSubSector(_subsubsector);
    setRoll(_roll);
    setGap(_gap);
}

RollId::RollId(RPCDetId const & _rpcdetid, int _gap)
    : id_(0x0)
    , mask_(0x0)
{
    set(_rpcdetid);
    setGap(_gap);
}

RollId::RollId(std::string const & _chamber, std::string const & _roll)
    : id_(0x0)
    , mask_(0x0)
{
    // parse calls reset
    parseDBName(_chamber, _roll);
}

void RollId::id(::uint32_t const & _id)
{
    id_ = _id;
    mask_ = 0x0;
    if (_id == 0x0)
        {
            setDet();
            setSubDet();
            setSubDetType();
        }
    else
        {
            if (det() != 2 || subdet() != 3)
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Invalid id in RollId::id(::uint32_t const & _id): " << _id
                              << " (det: " << det()
                              << ", subdet: " << subdet()
                              << ", subdettype: " << subdettype()
                              << ")";
                    throw std::logic_error(_errormsg.str());
                }
            if (subdettype() == 0) // re-initialize
                {
                    reset();
                    set(RPCDetId(_id));
                }
            else if (subdettype() != 1)
                {
                    std::ostringstream _errormsg;
                    _errormsg << "Invalid id in RollId::id(::uint32_t const & _id): " << _id
                              << " (det: " << det()
                              << ", subdet: " << subdet()
                              << ", subdettype: " << subdettype()
                              << ")";
                    throw std::logic_error(_errormsg.str());
                }
        }
}

RPCDetId RollId::getRPCDetId() const
{
    if (!mask_)
        bf_remask();
    if ((mask_ & mask_chamber_id) != mask_chamber_id)
        return RPCDetId(); // is it a fully specified RPCDetId?
    int _region(region())
        , _station(station())
        , _ring(ring())
        , _sector(sector())
        , _subsector(subsector())
        , _roll(roll());

    if (_region == 0)
        {
            if (_sector == 4 && _station == 4)
                {
                    if (subsubsector() == 0)
                        ++_subsector;
                    else
                        _subsector *= _subsector;
                }
            else if (_subsector == 0)
                _subsector = 1;
        }
    else if (_station == 1 && _ring == 1)
        ++_subsector;
    else
        _subsector = _subsector * 2 + subsubsector() + 1;

    return RPCDetId(_region
                    , _ring
                    , _station
                    , _sector
                    , layer()
                    , _subsector
                    , (_roll == wildcard_ ? 0 : _roll) // 0 is the wildcard for a roll in RPCDetId
                    );
}

std::string RollId::name() const
{
    std::ostringstream ss;
    stream(ss);
    return ss.str();
}

std::ostream & RollId::stream(std::ostream & _os) const
{
    // region.station.layer/ring/sector.subsector.subsubsector:roll_gap = chamber(r, eta, phi)/(inside chamber)
    // R*.*/(R,W)*/(C,S)*.*.*
    // RE-1/R2/C09:A, RB1in/W-2/S07+:fwd

    if ((id_ | ~(min_id_mask_)) == 0x00)
        return (_os << "R*");

    int _region(region())
        , _station(station())
        , _layer(layer())
        , _ring(ring())
        , _sector(sector())
        , _subsector(subsector())
        , _subsubsector(subsubsector())
        , _roll(roll())
        , _gap(gap());

    _os << 'R';
    std::ios_base::fmtflags _flags(_os.flags());
    _os << std::noshowpos;
    char _osfill(_os.fill('0'));

    // RB1in, RE-1
    switch (_region) {
    case -1: _os << "E-"; break;
    case 0:  _os << "B";  break;
    case 1:  _os << "E+"; break;
    default: _os << '*';  break;
    }

    switch (_station) {
    case wildcard_: (_region == wildcard_ ? _os : _os << '*'); break;
    default:        _os << _station; break;
    }

    if (_station < 3 && (_region == 0 || _region == wildcard_))
        switch (_layer) {
        case 1: _os << "in";  break;
        case 2: _os << "out"; break;
        }
    else if (_layer > 1)
        _os << ".?";

    _os << '/';

    // W-2, R2
    switch (_region) {
    case 0:  _os << 'W';  break;
    case -1:
    case 1:  _os << 'R'; break;
    }
    if (_ring == wildcard_)
        _os << '*';
    else if (_region == 0 && _ring < 3)
        _os << std::showpos << _ring << std::noshowpos;
    else if (std::abs(_region) == 1 && _ring > 0)
        _os << _ring;
    else if (_region == wildcard_)
        _os << _ring;
    else
        _os << '?';
    _os << '/';


    // S07+, C09
    if (_region == 0)
        {
            _os << 'S';
            if (_sector != wildcard_)
                _os << std::setw(2) << _sector;
            else
                _os << '*';
            if (_subsector != wildcard_ || _subsubsector != wildcard_)
                {
                    switch (_subsector) {
                    case wildcard_: _os << '*'; break;
                    case 2: _os << '+'; break;
                    case 1: _os << '-'; break;
                    case 0:
                        if (_subsubsector != 0)
                            _os << ".0.";
                        break;
                    }
                    switch (_subsubsector) {
                    case wildcard_: _os << '*'; break;
                    case 2: _os << '+'; break;
                    case 1: _os << '-'; break;
                    }
                }
        }
    else if (_region != wildcard_ // endcap
             && (_sector != wildcard_
                 && _subsector != wildcard_
                 && (_subsubsector != wildcard_ || (_station == 1 && _ring == 1))
                 )
             )
        {
            if (_station == 1 && _ring == 1)
                _os << 'C' << std::setw(2) << ((_sector - 1) * 3 + _subsector + 1);
            else
                _os << 'C' << std::setw(2)
                    << ((_sector - 1) * 6 + _subsector * 2 + _subsubsector + 1);
        }
    else
        {
            if (_sector == wildcard_)
                _os << "*";
            else
                _os << std::setw(2) << _sector;
            if (_subsector == wildcard_)
                _os << ".*";
            else
                _os << '.' << _subsector;
            if (_subsubsector == wildcard_)
                _os << ".*";
            else
                _os << '.' << _subsubsector;
        }

    // roll
    if (_roll != wildcard_)
        {
            if (_region == wildcard_)
                _os << ':' << _roll;
            else if (_region == 0)
                switch (_roll) {
                case 1:  _os << ":bwd"; break;
                case 2:  _os << ":mid"; break;
                case 3:  _os << ":fwd"; break;
                default: _os << ":?";   break;
                }
            else
                switch (_roll) {
                case 1:  _os << ":A"; break;
                case 2:  _os << ":B"; break;
                case 3:  _os << ":C"; break;
                case 4:  _os << ":D"; break;
                default: _os << ":?"; break;
                }
        }

    if (_gap != wildcard_)
        {
            switch (_gap) {
            case 1: _os << "_DW"; break;
            case 2: _os << "_UP"; break;
            }
        }

    _os.fill(_osfill);
    _os.flags(_flags);
    return _os;
}

void RollId::bf_remask() const
{
    mask_ = min_id_mask_;
    if (id_ & mask_region_)
        mask_ |= mask_region_;
    if (id_ & mask_station_)
        mask_ |= mask_station_;
    if (id_ & mask_layer_)
        mask_ |= mask_layer_;
    if (id_ & mask_ring_)
        mask_ |= mask_ring_;
    if (id_ & mask_sector_)
        mask_ |= mask_sector_;
    if (id_ & mask_subsector_)
        mask_ |= mask_subsector_;
    if (id_ & mask_subsubsector_)
        mask_ |= mask_subsubsector_;
    if (id_ & mask_roll_)
        mask_ |= mask_roll_;
    if (id_ & mask_gap_)
        mask_ |= mask_gap_;
}

void RollId::set(RPCDetId const & _rpcdetid)
{
    reset();

    int _region(_rpcdetid.region());
    int _station(_rpcdetid.station());
    int _ring(_rpcdetid.ring());
    int _sector(_rpcdetid.sector());
    int _subsector(_rpcdetid.subsector());
    int _roll(_rpcdetid.roll());

    setRegion(_region);
    setStation(_station);
    setLayer(_rpcdetid.layer());
    setRing(_ring);
    setSector(_sector);
    setRoll(_roll > 0 ? _roll : wildcard_);

    if (_region == 0)
        {
            if (_station == 4 && _sector == 4)
                { // 1, 2, 3, 4 to 1.1, 1.0, 2.0, 2.2
                    setSubSector(_subsector > 2 ? 2 : 1);
                    if (_subsector == 2 || _subsector == 3)
                        setSubSubSector(0);
                    else if (_subsector == 1)
                        setSubSubSector(1);
                    else
                        setSubSubSector(2);
                }
            else if (_station == 3
                     || (_station == 4 && _sector != 9 && _sector != 11))
                { // 1, 2 to 1.0, 2.0
                    setSubSector(_subsector);
                    setSubSubSector(0);
                }
            else
                { // 1 to 0.0
                    setSubSector(0);
                    setSubSubSector(0);
                }
        }
    else if (_station == 1 && _ring == 1)
        { // 1, 2, 3 to 0.0, 1.0, 2.0
            setSubSector(_subsector - 1);
            setSubSubSector(0);
        }
    else
        { // 1..6 to 0.0, 0.1, 1.0, 1.1, 2.0, 2.1
            setSubSector((_subsector - 1) / 2);
            setSubSubSector((_subsector - 1) % 2);
        }
}

void RollId::parseDBName(std::string const & _chamber, std::string const & _roll)
{
    std::size_t _size(_chamber.size());
    if (_size == 0)
        throw std::runtime_error("Invalid empty chamber name");

    reset();
    std::size_t _pos(0), _next(0);
    int _temp(0);
    std::istringstream _parser;

    switch (_chamber.at(_pos))
        {
        case 'W': _temp = 0; break;
        case 'R': _temp = 1; break;
        default:
            throw std::runtime_error(std::string("Chamber with invalid Region: expected W or R, got ")
                                     + _chamber.at(_pos) + " in " + _chamber);
            break;
        }
    if (_temp == 0)
        { // barrel: W_ring_/RB_station_layer_/_sector_subsector
            // region
            setRegion(0);
            // ring
            if ((++_pos) >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _next = _chamber.find('/', _pos);
            if (_next == std::string::npos)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, _next - _pos));
            _parser >> _temp;
            setRing(_temp);
            // station: skip RB, 1 digit
            _pos = _next + 3;
            if (_pos + 1 >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, 1));
            _parser >> _temp;
            setStation(_temp);
            // layer
            ++_pos;
            if (_chamber.at(_pos) == 'o')
                setLayer(2);
            else
                setLayer(1);
            // sector
            _next = _chamber.find('/', _pos);
            if (_next == std::string::npos || (_pos = _next + 1) >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _next = _chamber.find_first_not_of("0123456789", _pos);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, (_next == std::string::npos ? _next : (_next - _pos))));
            _parser >> _temp;
            setSector(_temp);
            // subsector, subsubsector
            if (_next == std::string::npos)
                {
                    setSubSector(0);
                    setSubSubSector(0);
                }
            else
                {
                    switch (_chamber.at(_next)) {
                    case '+': setSubSector(2); break;
                    case '-': setSubSector(1); break;
                    default:  setSubSector(0); break;
                    }
                    // subsubsector
                    if ((++_next) >= _size)
                        setSubSubSector(0);
                    else
                        switch (_chamber.at(_next)) {
                        case '+': setSubSubSector(2); break;
                        case '-': setSubSubSector(1); break;
                        default:  setSubSubSector(0); break;
                        }
                }
            // roll
            if (!_roll.empty())
                switch (_roll.at(0))
                    {
                    case 'B': setRoll(1); break;
                    case 'C': setRoll(2); break;
                    case 'F': setRoll(3); break;
                    default:
                        throw std::runtime_error(std::string("Invalid roll name for barrel: ") + _roll);
                    }
        }
    else
        { // forward: RE_station_/_ring_/_sector
            // region, station
            _pos = _chamber.find("RE");
            if (_pos == std::string::npos || (_pos += 2) >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _next = _chamber.find('/', _pos);
            if (_next == std::string::npos)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, _next - _pos));
            _parser >> _temp;
            if (_temp < 0)
                {
                    setRegion(-1);
                    setStation(-1 * _temp);
                }
            else
                {
                    setRegion(1);
                    setStation(_temp);
                }
            // layer
            setLayer(1);
            // ring: 1 digit
            _pos = _next + 1;
            if (_pos + 1 >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, 1));
            _parser >> _temp;
            setRing(_temp);
            // sector, subsector, subsubsector
            if ((_pos += 2) >= _size)
                throw std::runtime_error(std::string("Chamber name too short: ") + _chamber);
            _next = _chamber.find_first_not_of("0123456789", _pos);
            _parser.clear();
            _parser.str(_chamber.substr(_pos, (_next == std::string::npos ? _next : (_next - _pos))));
            _parser >> _temp;
            if (station() == 1 && ring() == 1)
                { // 1..18 to (1..6)*(0..2)
                    setSector(((_temp - 1) / 3) + 1);
                    setSubSector((_temp - 1) % 3);
                    setSubSubSector(0);
                }
            else
                { // 1..36 to (1..6)*(0..2)*(0..1)
                    setSector((_temp - 1) / 6 + 1);
                    setSubSector(((_temp - 1) % 6) / 2);
                    setSubSubSector((_temp - 1) % 2);
                }
            // roll
            if (!_roll.empty())
                switch (_roll.at(0))
                    {
                    case 'A': setRoll(1); break;
                    case 'B': setRoll(2); break;
                    case 'C': setRoll(3); break;
                    case 'D': setRoll(4); break;
                    default:
                        throw std::runtime_error(std::string("Invalid roll name for endcap: ") + _roll);
                    }
        }
}

void RollId::parseName(std::string const & _roll)
{
    // region.station.layer/ring/sector.subsector.subsubsector:roll_gap = chamber(r, eta, phi)/(inside chamber)
    // R*/(R,W)*/(C,S)*(:roll)
    // RE-1/R2/C09:A, RB1in/W-2/S07+:fwd
    std::size_t _size(_roll.size());
    if (_size < 2)
        throw std::runtime_error("Invalid empty chamber name");

    reset();
    std::size_t _pos(0), _next(0);
    int _temp(0);
    std::istringstream _parser;

    int _region(wildcard_), _station(wildcard_);

    // region
    // RB, RE+, RE-, RE*, RE/, R*
    // can't set it to RE, need +/-
    if (_roll.at(_pos) != 'R')
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");

    if ((++_pos) == _size)
        return;
    switch (_roll.at(_pos)) {
    case 'B': setRegion(_region = 0); break;
    case 'E':
        if ((++_pos) == _size)
            return;
        switch (_roll.at(_pos)) {
        case '-': setRegion(_region = -1); break;
        case '+': setRegion(_region = 1); break;
        case '*': break;
        case '/': --_pos; break;
        default:
            throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                     + "; stopped after " + _roll.substr(0, _pos)
                                     + " (" + name() + ")");
        }
        break;
    case '*': break;
    default:
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    }
    if ((++_pos) == _size)
        return;

    // station
    // previous plus station, *, /
    switch (_roll.at(_pos)) {
    case '1':
    case '2':
    case '3':
    case '4': setStation((_station = 1 + _roll.at(_pos) - '1'));
    case '*': ++_pos; break;
    case '/': break;
    default:
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    }
    if (_pos == _size)
        return;

    // layer
    // in, out, *, /
    switch (_roll.at(_pos)) {
    case 'i': setLayer(1); _pos += 2; break;
    case 'o': setLayer(2); _pos += 3; break;
    case '*': ++_pos; break;
    case '/': break;
    default:
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    }

    if (std::abs(_region) == 1 || _station == 3 || _station == 4)
        setLayer(1);

    // jump over /
    if ((++_pos) >= _size)
        return;

    // ring
    // W, R
    switch (_roll.at(_pos)) {
    case 'W':
        if (_region == wildcard_)
            setRegion(_region = 0);
    case 'R':
        if ((++_pos) == _size)
            return;
        break;
    }

    // * or ring
    if (_roll.at(_pos) == '*')
        ++_pos;
    else
        {
            _next = _roll.find_first_not_of("+-0123", _pos);
            if (_next == std::string::npos)
                _next = _size;
            if (_next == _pos)
                throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                         + "; stopped after " + _roll.substr(0, _pos)
                                         + " (" + name() + ")");
            _parser.clear();
            _parser.str(_roll.substr(_pos, _next - _pos));
            _parser >> _temp;
            setRing(_temp);
            _pos = _next;
        }

    // jump over /
    if ((++_pos) >= _size)
        return;

    // sector
    // S, C
    if ((std::abs(_region) == 1 && _roll.at(_pos) == 'S')
        || (_region == 0 && _roll.at(_pos) == 'C'))
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    if (!_region)
        {
            if (_roll.at(_pos) == 'S')
                {
                    ++_pos;
                    if (_pos == _size)
                        return;
                }
            if (_roll.at(_pos) != '*')
                {
                    _next = _roll.find_first_not_of("0123456789", _pos);
                    if (_next == std::string::npos)
                        _next = _size;
                    if (_next == _pos)
                        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                                 + "; stopped after " + _roll.substr(0, _pos)
                                                 + " (" + name() + ")");
                    _parser.clear();
                    _parser.str(_roll.substr(_pos, _next - _pos));
                    _parser >> _temp;
                    setSector(_temp);
                    _pos = _next;
                }
            else
                ++_pos;
            // subsector, subsubsector
            if (_pos >= _size)
                return;
            else
                {
                    switch (_roll.at(_pos)) {
                    case '+': setSubSector(2); ++_pos; break;
                    case '-': setSubSector(1); ++_pos; break;
                    case '*': ++_pos; break;
                    }
                    // subsubsector
                    if (_pos >= _size)
                        {
                            setSubSubSector(subsector() == wildcard_ ? wildcard_ : 0);
                            return;
                        }
                    else
                        switch (_roll.at(_pos)) {
                        case '+': setSubSubSector(2); ++_pos; break;
                        case '-': setSubSubSector(1); ++_pos; break;
                        case '*': ++_pos; break;
                        default:
                            setSubSubSector(subsector() == wildcard_ ? wildcard_ : 0);
                        }
                }
        }
    else if (std::abs(_region) == 1)
        {
            if (_roll.at(_pos) == 'C')
                {
                    ++_pos;
                    if (_pos == _size)
                        return;
                }
            if (_roll.at(_pos) != '*')
                {
                    _next = _roll.find_first_not_of("0123456789", _pos);
                    if (_next == std::string::npos)
                        _next = _size;
                    if (_next == _pos)
                        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                                 + "; stopped after " + _roll.substr(0, _pos)
                                                 + " (" + name() + ")");
                    _parser.clear();
                    _parser.str(_roll.substr(_pos, _next - _pos));
                    _parser >> _temp;
                    if (station() == 1 && ring() == 1)
                        { // 1..18 to (1..6)*(0..2)
                            setSector(((_temp - 1) / 3) + 1);
                            setSubSector((_temp - 1) % 3);
                            setSubSubSector(0);
                        }
                    else
                        { // 1..36 to (1..6)*(0..2)*(0..1)
                            setSector((_temp - 1) / 6 + 1);
                            setSubSector(((_temp - 1) % 6) / 2);
                            setSubSubSector((_temp - 1) % 2);
                        }
                    _pos = _next;
                }
            else
                ++_pos;
        }
    else {
        switch (_roll.at(_pos)) {
        case 'C':
        case 'S':
        case '*':
            if ((++_pos) == _size)
                return;
        case ':': break;
        default:
            throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                     + "; stopped after " + _roll.substr(0, _pos)
                                     + " (" + name() + ")");
        }
        switch (_roll.at(_pos)) {
        case '*':
            if ((++_pos) == _size)
                return;
        case ':': break;
        default:
            throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                     + "; stopped after " + _roll.substr(0, _pos)
                                     + " (" + name() + ")");
        }
    }

    if (_pos >= _size)
        return;

    if (_roll.at(_pos) != ':')
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    if ((++_pos) == _size)
        return;
    switch (_roll.at(_pos)) {
    case 'b': setRoll(1); break;
    case 'm': setRoll(2); break;
    case 'f': setRoll(3); break;
    case 'A':
    case 'B':
    case 'C':
    case 'D': setRoll(1 + _roll.at(_pos) - 'A'); break;
    case '*': break;
    default:
        throw std::runtime_error(std::string("Invalid Roll name: ") + _roll
                                 + "; stopped after " + _roll.substr(0, _pos)
                                 + " (" + name() + ")");
    }
}

std::ostream & operator<<(std::ostream & _os, RollId const & _in)
{
    return (_os << _in.name());
}

} // namespace tools
} // namespace rpct
