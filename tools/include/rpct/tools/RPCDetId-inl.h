/* output of
 * ./a.out rpct/tools/ RPCDetId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Padding padding 0 1 31 Roll ro 1 1 4 SubSector su 0 1 6 Layer la 0 1 2 Sector se 0 1 12 Station st 0 1 4 Ring ri 0 -5 2 Region re 0 -1 1
 */

#ifndef rpct_tools_RPCDetId_icc
#define rpct_tools_RPCDetId_icc

#include "rpct/tools/RPCDetId.h"

#include <stdexcept>
#include <sstream>

namespace rpct {
namespace tools {

inline ::uint32_t RPCDetId::id() const
{
    return id_;
}
inline RPCDetId::operator ::uint32_t() const
{
    return id_;
}
inline ::uint32_t RPCDetId::mask() const
{
    return mask_;
}

inline bool RPCDetId::matches(RPCDetId const & _in) const
{
    if (!mask_ && (id_ || min_id_mask_))
        bf_remask();
    if (mask_ && !_in.mask_ && (_in.id_ || _in.min_id_mask_))
        _in.bf_remask();
    return ((id_ & _in.mask_) == (mask_ & _in.id_));
}

inline void RPCDetId::id(::uint32_t const & _id)
{
    id_ = _id;
    if (det() != 2 || subdet() != 3 || subdettype() != 0)
        throw std::logic_error("Attempt to set invalid RPCDetId");
    mask_ = 0x0;
}

inline void RPCDetId::reset()
{
    id_ = 0x0;
    mask_ = min_id_mask_;
}

// Field Getters
inline int RPCDetId::det() const
{
    return bf_get(wc_det_, min_det_, mask_det_, pos_det_);
}
inline int RPCDetId::subdet() const
{
    return bf_get(wc_subdet_, min_subdet_, mask_subdet_, pos_subdet_);
}
inline int RPCDetId::subdettype() const
{
    return bf_get(wc_subdettype_, min_subdettype_, mask_subdettype_, pos_subdettype_);
}
inline int RPCDetId::roll() const
{
    int _roll = bf_get(wc_roll_, min_roll_, mask_roll_, pos_roll_);
    if (_roll == wildcard_)
        return 0;
    return _roll;
}
inline int RPCDetId::subsector() const
{
    return bf_get(wc_subsector_, min_subsector_, mask_subsector_, pos_subsector_);
}
inline int RPCDetId::layer() const
{
    return bf_get(wc_layer_, min_layer_, mask_layer_, pos_layer_);
}
inline int RPCDetId::sector() const
{
    return bf_get(wc_sector_, min_sector_, mask_sector_, pos_sector_);
}
inline int RPCDetId::station() const
{
    return bf_get(wc_station_, min_station_, mask_station_, pos_station_);
}
inline int RPCDetId::ring() const
{
    if (barrel())
        return bf_get(wc_ring_, min_barrel_ring_, mask_ring_, pos_ring_);
    else
        return bf_get(wc_ring_, min_endcap_ring_, mask_ring_, pos_ring_);
}
inline int RPCDetId::region() const
{
    return bf_get(wc_region_, min_region_, mask_region_, pos_region_);
}

inline RPCDetId RPCDetId::getChamberId() const
{
    return RPCDetId(id_ & mask_chamber_id_);
}

inline bool RPCDetId::barrel() const
{
    return (region() == 0);
}
inline bool RPCDetId::endcap() const
{
    return (region() != 0);
}

// Field Setters
inline void RPCDetId::setDet()
{
    return bf_set(wc_det_, min_det_, max_det_, mask_det_, pos_det_, 2);
}
inline void RPCDetId::setSubDet()
{
    return bf_set(wc_subdet_, min_subdet_, max_subdet_, mask_subdet_, pos_subdet_, 3);
}
inline void RPCDetId::setSubDetType()
{
    return bf_set(wc_subdettype_, min_subdettype_, max_subdettype_, mask_subdettype_, pos_subdettype_, 0);
}
inline void RPCDetId::setRoll(int _roll)
{
    if (_roll == 0)
        _roll = wildcard_;
    return bf_set(wc_roll_, min_roll_, max_roll_, mask_roll_, pos_roll_, _roll);
}
inline void RPCDetId::setSubSector(int _subsector)
{
    return bf_set(wc_subsector_, min_subsector_, max_subsector_, mask_subsector_, pos_subsector_, _subsector);
}
inline void RPCDetId::setLayer(int _layer)
{
    return bf_set(wc_layer_, min_layer_, max_layer_, mask_layer_, pos_layer_, _layer);
}
inline void RPCDetId::setSector(int _sector)
{
    return bf_set(wc_sector_, min_sector_, max_sector_, mask_sector_, pos_sector_, _sector);
}
inline void RPCDetId::setStation(int _station)
{
    return bf_set(wc_station_, min_station_, max_station_, mask_station_, pos_station_, _station);
}
inline void RPCDetId::setRing(int _ring)
{
    if (barrel())
        return bf_set(wc_ring_, min_barrel_ring_, max_barrel_ring_, mask_ring_, pos_ring_, _ring);
    else
        return bf_set(wc_ring_, min_endcap_ring_, max_endcap_ring_, mask_ring_, pos_ring_, _ring);
}
inline void RPCDetId::setRegion(int _region)
{
    return bf_set(wc_region_, min_region_, max_region_, mask_region_, pos_region_, _region);
}

inline bool RPCDetId::operator<(RPCDetId const & _in) const
{
    return (id_ < _in.id_);
}
inline bool RPCDetId::operator==(RPCDetId const & _in) const
{
    return (id_ == _in.id_);
}
inline bool RPCDetId::operator!=(RPCDetId const & _in) const
{
    return (id_ != _in.id_);
}
inline bool RPCDetId::operator<(::uint32_t const & _in) const
{
    return (id_ < _in);
}
inline bool RPCDetId::operator==(::uint32_t const & _in) const
{
    return (id_ == _in);
}
inline bool RPCDetId::operator!=(::uint32_t const & _in) const
{
    return (id_ != _in);
}
inline int RPCDetId::bf_get(int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const
{
    ::uint32_t _value(id_ & _mask);
    if (_wc && _value == 0)
        return wildcard_;
    return _min + (int)(_value >> _pos) - _wc;
}
inline void RPCDetId::bf_set(int const _wc, int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value)
{
    if (!mask_ && (id_ || min_id_mask_))
        bf_remask();
    if (_value >= _min && _value <= _max)
        {
            id_ &= ~_mask;
            id_ |= (((::uint32_t)(_value - _min + _wc) << _pos) & _mask);
            mask_ |= _mask;
        }
    else if (_wc && _value == wildcard_)
        {
            id_ &= ~_mask;
            mask_ &= ~_mask;
        }
    else
        {
            std::ostringstream _errormsg;
            _errormsg << "Out-of-range input for RPCDetId::bf_set, position " << _pos << ": " << _value;
            throw std::out_of_range(_errormsg.str());
        }
}
inline void RPCDetId::bf_stream(std::ostream & os, int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const
{
    ::uint32_t _value(id_ & _mask);
    if (_wc && _value == 0)
        os << '*';
    else
        os << (_min + (int)(_value >> _pos) - _wc);
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_RPCDetId_icc
