/* output of
 * ./a.out rpct/tools/ RPCDetId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Padding padding 0 1 31 Roll ro 1 1 4 SubSector su 0 1 6 Layer la 0 1 2 Sector se 0 1 12 Station st 0 1 4 Ring ri 0 -5 2 Region re 0 -1 1
 */

#include "rpct/tools/RPCDetId.h"

#ifndef PROJECT_NAME // defined as "CMSSW" by SCRAM

#include <ostream>
#include <sstream>

namespace rpct {
namespace tools {

RPCDetId::RPCDetId()
    : id_(0x0)
    , mask_(min_id_mask_)
{
    setDet();
    setSubDet();
    setSubDetType();
}

RPCDetId::RPCDetId(::uint32_t const & _id)
    : id_(_id)
    , mask_(0x0)
{
    if (det() != 2 || subdet() != 3 || subdettype() != 0)
        throw std::logic_error("Attempt to create RPCDetId from invalid id");
}

RPCDetId::RPCDetId(int _region
                   , int _ring
                   , int _station
                   , int _sector
                   , int _layer
                   , int _subsector
                   , int _roll)
    : id_(0x0)
    , mask_(min_id_mask_)
{
    setDet();
    setSubDet();
    setSubDetType();

    setRegion(_region);
    setRing(_ring);
    setStation(_station);
    setSector(_sector);
    setLayer(_layer);
    setSubSector(_subsector);
    setRoll(_roll);
}

std::string RPCDetId::name() const
{
    std::ostringstream ss;
    ss << "re_";
    bf_stream(ss, wc_region_, min_region_, mask_region_, pos_region_);
    ss << "_ri_"
       <<  ring();
    ss << "_st_";
    bf_stream(ss, wc_station_, min_station_, mask_station_, pos_station_);
    ss << "_se_";
    bf_stream(ss, wc_sector_, min_sector_, mask_sector_, pos_sector_);
    ss << "_la_";
    bf_stream(ss, wc_layer_, min_layer_, mask_layer_, pos_layer_);
    ss << "_su_";
    bf_stream(ss, wc_subsector_, min_subsector_, mask_subsector_, pos_subsector_);
    ss << "_ro_";
    bf_stream(ss, wc_roll_, min_roll_, mask_roll_, pos_roll_);
    return ss.str();
}

void RPCDetId::bf_remask() const
{
    mask_ = min_id_mask_;
    if (id_ & mask_roll_)
        mask_ |= mask_roll_;
}

std::ostream & operator<<(std::ostream & _os, RPCDetId const & _in)
{
    return (_os << _in.name());
}

} // namespace tools
} // namespace rpct

#endif // PROJECT_NAME
