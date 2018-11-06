/* output of
 * ./a.out rpct/tools/ RPCDetId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Padding padding 0 1 31 Roll ro 1 1 4 SubSector su 0 1 6 Layer la 0 1 2 Sector se 0 1 12 Station st 0 1 4 Ring ri 0 -5 2 Region re 0 -1 1
 */

#ifndef rpct_tools_RPCDetId_h
#define rpct_tools_RPCDetId_h

#ifndef PROJECT_NAME // defined as "CMSSW" by SCRAM

#include <stdint.h>
#include <string>
#include <iosfwd>
#include <climits>

namespace rpct {
namespace tools {

class RPCDetId
{
protected:
    /// always-true bits in mask because of fields not using wildcards
    static const ::uint32_t min_id_mask_ = 0xfffc7fff;

public:
    static const int wildcard_ = INT_MIN;

    /** \name Field Ranges
     * @{ */
    static const int min_det_ = 0;
    static const int max_det_ = 15;
    static const int wc_det_ = 0;
    static const int min_subdet_ = 0;
    static const int max_subdet_ = 7;
    static const int wc_subdet_ = 0;
    static const int min_subdettype_ = 0; ///< 0 RPCDetId, 1 RPCCompDetId
    static const int max_subdettype_ = 3;
    static const int wc_subdettype_ = 0;

    static const int min_roll_ = 1; ///< bwd/A, mid/B, fwd/C, D
    static const int max_roll_ = 4;
    static const int wc_roll_ = 1;
    static const int min_subsector_ = 1; ///< "", --, -, +, ++
    static const int max_subsector_ = 6;
    static const int wc_subsector_ = 0;
    static const int min_layer_ = 1; ///< in, out
    static const int max_layer_ = 2;
    static const int wc_layer_ = 0;
    static const int min_sector_ = 1; ///< 1..12 brl, 1..6 ec
    static const int max_sector_ = 12;
    static const int wc_sector_ = 0;
    static const int min_station_ = 1; ///< 1..4
    static const int max_station_ = 4;
    static const int wc_station_ = 0;

    static const int min_ring_ = -5; ///< -2..2 brl, 1..3 ec
    static const int max_ring_ = 2;
    static const int wc_ring_ = 0;
    static const int min_barrel_ring_ = -5;
    static const int max_barrel_ring_ = 2;
    static const int min_endcap_ring_ = 1;
    static const int max_endcap_ring_ = 3;

    static const int min_region_ = -1; // 0 brl, -1 RE-, 1 RE+
    static const int max_region_ = 1;
    static const int wc_region_ = 0;
    /** @} */

protected:
    /** \name Field Positions and Masks
     * @{ */
    static const int pos_det_ = 28;
    static const ::uint32_t mask_det_ = 0xf0000000;
    static const int pos_subdet_ = 25;
    static const ::uint32_t mask_subdet_ = 0x0e000000;
    static const int pos_subdettype_ = 23;
    static const ::uint32_t mask_subdettype_ = 0x01800000;
    static const int pos_roll_ = 15;
    static const ::uint32_t mask_roll_ = 0x00038000;
    static const int pos_subsector_ = 12;
    static const ::uint32_t mask_subsector_ = 0x00007000;
    static const int pos_layer_ = 11;
    static const ::uint32_t mask_layer_ = 0x00000800;
    static const int pos_sector_ = 7;
    static const ::uint32_t mask_sector_ = 0x00000780;
    static const int pos_station_ = 5;
    static const ::uint32_t mask_station_ = 0x00000060;
    static const int pos_ring_ = 2;
    static const ::uint32_t mask_ring_ = 0x0000001c;
    static const int pos_region_ = 0;
    static const ::uint32_t mask_region_ = 0x00000003;
    /** @} */

    static const ::uint32_t mask_chamber_id_ = 0xffffffff & (~mask_roll_);

public:
    RPCDetId();
    RPCDetId(::uint32_t const & _id);
    RPCDetId(int _region
             , int _ring
             , int _station = 1
             , int _sector = 1
             , int _layer = 1
             , int _subsector = 1
             , int _roll = wildcard_);

    ::uint32_t id() const;
    operator ::uint32_t() const;
    ::uint32_t mask() const;

    bool matches(RPCDetId const & _in) const;

    void id(::uint32_t const & _in);
    void reset();

    /** \name Field Getters
     * @{ */
    int det() const;
    int subdet() const;
    int subdettype() const;
    int roll() const;
    int subsector() const;
    int layer() const;
    int sector() const;
    int station() const;
    int ring() const;
    int region() const;
    /** @} */

    /** \name Derived Getters
     * @{ */
    RPCDetId getChamberId() const;

    bool barrel() const;
    bool endcap() const;
    /** @}*/

    /** \name Field Setters
     * A cms::Exception("OutOfRange") is thrown for out-of-range input values.
     * @{ */
    void setDet();
    void setSubDet();
    void setSubDetType();
    void setRoll(int _roll = wildcard_);
    void setSubSector(int _subsector = 1);
    void setLayer(int _layer = 1);
    void setSector(int _sector = 1);
    void setStation(int _station = 1);
    void setRing(int _ring = -5);
    void setRegion(int _region = -1);
    /** @} */

    std::string name() const;

    bool operator<(RPCDetId const & _in) const;
    bool operator==(RPCDetId const & _in) const;
    bool operator!=(RPCDetId const & _in) const;
    bool operator<(::uint32_t const & _in) const;
    bool operator==(::uint32_t const & _in) const;
    bool operator!=(::uint32_t const & _in) const;

protected:
    void bf_remask() const;
    int bf_get(int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const;
    void bf_set(int const _wc, int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value);
    void bf_stream(std::ostream & _os, int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const;

protected:
    ::uint32_t id_;
    mutable ::uint32_t mask_;
};

std::ostream & operator<<(std::ostream & _os, RPCDetId const & _in);

} // namespace tools
} // namespace rpct

#include "rpct/tools/RPCDetId-inl.h"

#else // PROJECT_NAME

#include "DataFormats/MuonDetId/interface/RPCDetId.h"

namespace rpct {
namespace tools {

typedef ::RPCDetId RPCDetId;

} // namespace tools
} // namespace rpct

#endif // PROJECT_NAME

#endif // rpct_tools_RPCDetId_h
