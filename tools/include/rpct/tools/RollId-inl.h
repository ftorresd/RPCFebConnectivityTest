/* output of
 * ./a.out rpct/tools/ RollId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Region re 1 -1 1 Station st 1 1 4 Layer la 1 1 2 Ring ri 1 -2 3 Sector se 1 1 12 SubSector su 1 0 2 SubSubSector ss 1 0 2 Roll ro 1 1 4 Gap gp 1 1 2
 */

#ifndef rpct_tools_RollId_inl_h
#define rpct_tools_RollId_inl_h

#include "rpct/tools/RollId.h"

#include <stdexcept>
#include <sstream>
#include <cstdlib>

namespace rpct {
namespace tools {

inline ::uint32_t RollId::id() const
{
    return id_;
}
inline RollId::operator ::uint32_t() const
{
    return id_;
}
inline ::uint32_t RollId::mask() const
{
    return mask_;
}

inline bool RollId::matches(RollId const & _in) const
{
    if (!mask_ && (id_ || min_id_mask_))
        bf_remask();
    if (mask_ && !_in.mask_ && (_in.id_ || _in.min_id_mask_))
        _in.bf_remask();
    return ((id_ & _in.mask_) == (mask_ & _in.id_));
}

inline void RollId::reset()
{
    id_ = 0x0;
    mask_ = min_id_mask_;
    setDet();
    setSubDet();
    setSubDetType();
}

// Field Getters
inline int RollId::det() const
{
    return bf_get(wc_det_, min_det_, mask_det_, pos_det_);
}
inline int RollId::subdet() const
{
    return bf_get(wc_subdet_, min_subdet_, mask_subdet_, pos_subdet_);
}
inline int RollId::subdettype() const
{
    return bf_get(wc_subdettype_, min_subdettype_, mask_subdettype_, pos_subdettype_);
}
inline int RollId::region() const
{
    return bf_get(wc_region_, min_region_, mask_region_, pos_region_);
}
inline int RollId::station() const
{
    return bf_get(wc_station_, min_station_, mask_station_, pos_station_);
}
inline int RollId::layer() const
{
    return bf_get(wc_layer_, min_layer_, mask_layer_, pos_layer_);
}
inline int RollId::ring() const
{
    return bf_get(wc_ring_, min_ring_, mask_ring_, pos_ring_);
}
inline int RollId::sector() const
{
    return bf_get(wc_sector_, min_sector_, mask_sector_, pos_sector_);
}
inline int RollId::subsector() const
{
    return bf_get(wc_subsector_, min_subsector_, mask_subsector_, pos_subsector_);
}
inline int RollId::subsubsector() const
{
    return bf_get(wc_subsubsector_, min_subsubsector_, mask_subsubsector_, pos_subsubsector_);
}
inline int RollId::roll() const
{
    return bf_get(wc_roll_, min_roll_, mask_roll_, pos_roll_);
}
inline int RollId::gap() const
{
    return bf_get(wc_gap_, min_gap_, mask_gap_, pos_gap_);
}

// Derived
inline RollId RollId::getChamberId() const
{
    return RollId(id_ & mask_chamber_id);
}

inline RollId RollId::getRollId() const
{
    return RollId(id_ & mask_roll_id);
}

inline bool RollId::barrel() const
{
    return (region() == 0);
}

inline bool RollId::endcap() const
{
    return (std::abs(region()) == 1);
}

inline int RollId::wheel() const
{
    if (!endcap())
        return ring();
    return wildcard_;
}

inline int RollId::disk() const
{
    if (endcap())
        return station();
    return wildcard_;
}

inline int RollId::endcapsector() const
{
    int _sector(sector())
        , _subsector(subsector())
        , _subsubsector(subsubsector());
    if (!barrel() && _sector != wildcard_ && _subsector != wildcard_)
        {
            if (station() == 1 && ring() == 1)
                return (_sector - 1) * 3 + _subsector + 1;
            else if (_subsubsector != wildcard_)
                return (_sector - 1) * 6 + _subsector * 2 + _subsubsector + 1;
        }
    return wildcard_;
}

inline bool RollId::reference() const
{
    if (station() == 2 && region() != wildcard_)
        {
            if (barrel())
                {
                    if (std::abs(ring()) < 2)
                        return layer() == 1;
                    return layer() == 2;
                }
            return (std::abs(ring()) < 3 || (roll() != wildcard_ && roll() > 1));
        }
    return false;
}

// Field Setters
inline void RollId::setDet()
{
    return bf_set(wc_det_, min_det_, max_det_, mask_det_, pos_det_, 2);
}
inline void RollId::setSubDet()
{
    return bf_set(wc_subdet_, min_subdet_, max_subdet_, mask_subdet_, pos_subdet_, 3);
}
inline void RollId::setSubDetType()
{
    return bf_set(wc_subdettype_, min_subdettype_, max_subdettype_, mask_subdettype_, pos_subdettype_, 1);
}
inline void RollId::setRegion(int _region)
{
    return bf_set(wc_region_, min_region_, max_region_, mask_region_, pos_region_, _region);
}
inline void RollId::setStation(int _station)
{
    return bf_set(wc_station_, min_station_, max_station_, mask_station_, pos_station_, _station);
}
inline void RollId::setLayer(int _layer)
{
    return bf_set(wc_layer_, min_layer_, max_layer_, mask_layer_, pos_layer_, _layer);
}
inline void RollId::setRing(int _ring)
{
    return bf_set(wc_ring_, min_ring_, max_ring_, mask_ring_, pos_ring_, _ring);
}
inline void RollId::setSector(int _sector)
{
    return bf_set(wc_sector_, min_sector_, max_sector_, mask_sector_, pos_sector_, _sector);
}
inline void RollId::setSubSector(int _subsector)
{
    return bf_set(wc_subsector_, min_subsector_, max_subsector_, mask_subsector_, pos_subsector_, _subsector);
}
inline void RollId::setSubSubSector(int _subsubsector)
{
    return bf_set(wc_subsubsector_, min_subsubsector_, max_subsubsector_, mask_subsubsector_, pos_subsubsector_, _subsubsector);
}
inline void RollId::setRoll(int _roll)
{
    return bf_set(wc_roll_, min_roll_, max_roll_, mask_roll_, pos_roll_, _roll);
}
inline void RollId::setGap(int _gap)
{
    return bf_set(wc_gap_, min_gap_, max_gap_, mask_gap_, pos_gap_, _gap);
}

inline bool RollId::operator<(RollId const & _in) const
{
    return (id_ < _in.id_);
}
inline bool RollId::operator==(RollId const & _in) const
{
    return (id_ == _in.id_);
}
inline bool RollId::operator!=(RollId const & _in) const
{
    return (id_ != _in.id_);
}
inline bool RollId::operator<(::uint32_t const & _in) const
{
    return (id_ < _in);
}
inline bool RollId::operator==(::uint32_t const & _in) const
{
    return (id_ == _in);
}
inline bool RollId::operator!=(::uint32_t const & _in) const
{
    return (id_ != _in);
}
inline int RollId::bf_get(int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const
{
    ::uint32_t _value(id_ & _mask);
    if (_wc && _value == 0)
        return wildcard_;
    return _min + (int)(_value >> _pos) - _wc;
}
inline void RollId::bf_set(int const _wc, int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value)
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
            _errormsg << "Out-of-range input for RollId::bf_set, position " << _pos << ": " << _value;
            throw std::out_of_range(_errormsg.str());
        }
}
inline void RollId::bf_stream(std::ostream & os, int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const
{
    ::uint32_t _value(id_ & _mask);
    if (_wc && _value == 0)
        os << '*';
    else
        os << (_min + (int)(_value >> _pos) - _wc);
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_RollId_inl_h
