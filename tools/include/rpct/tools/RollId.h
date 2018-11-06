/* output of
 * ./a.out rpct/tools/ RollId Det det 0 0 15 SubDet subdet 0 0 7 SubDetType type 0 0 3 Region re 1 -1 1 Station st 1 1 4 Layer la 1 1 2 Ring ri 1 -2 3 Sector se 1 1 12 SubSector su 1 0 2 SubSubSector ss 1 0 2 Roll ro 1 1 4 Gap gp 1 1 2
 */

#ifndef rpct_tools_RollId_h
#define rpct_tools_RollId_h

#include <stdint.h>
#include <string>
#include <iosfwd>
#include <climits>

namespace rpct {
namespace tools {

class RPCDetId;

class RollId
{
protected:
    /// always-true bits in mask because of fields not using wildcards
    static const ::uint32_t min_id_mask_ = 0xff800000;

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
    static const int min_region_ = -1; // 0 brl, -1 RE-, 1 RE+
    static const int max_region_ = 1;
    static const int wc_region_ = 1;
    static const int min_station_ = 1; ///< 1..4
    static const int max_station_ = 4;
    static const int wc_station_ = 1;
    static const int min_layer_ = 1; ///< in, out
    static const int max_layer_ = 2;
    static const int wc_layer_ = 1;
    static const int min_ring_ = -2; ///< -2..2 brl, 1..3 ec
    static const int max_ring_ = 3;
    static const int wc_ring_ = 1;
    static const int min_sector_ = 1; ///< 1..12 brl, 1..6 ec
    static const int max_sector_ = 12;
    static const int wc_sector_ = 1;
    static const int min_subsector_ = 0; ///< "", -, +
    static const int max_subsector_ = 2;
    static const int wc_subsector_ = 1;
    static const int min_subsubsector_ = 0; ///< "", -, +
    static const int max_subsubsector_ = 2;
    static const int wc_subsubsector_ = 1;
    static const int min_roll_ = 1; ///< bwd/A, mid/B, fwd/C, D
    static const int max_roll_ = 4;
    static const int wc_roll_ = 1;
    static const int min_gap_ = 1; ///< DW, UP
    static const int max_gap_ = 2;
    static const int wc_gap_ = 1;
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
    static const int pos_region_ = 21;
    static const ::uint32_t mask_region_ = 0x00600000;
    static const int pos_station_ = 18;
    static const ::uint32_t mask_station_ = 0x001c0000;
    static const int pos_layer_ = 16;
    static const ::uint32_t mask_layer_ = 0x00030000;
    static const int pos_ring_ = 13;
    static const ::uint32_t mask_ring_ = 0x0000e000;
    static const int pos_sector_ = 9;
    static const ::uint32_t mask_sector_ = 0x00001e00;
    static const int pos_subsector_ = 7;
    static const ::uint32_t mask_subsector_ = 0x00000180;
    static const int pos_subsubsector_ = 5;
    static const ::uint32_t mask_subsubsector_ = 0x00000060;
    static const int pos_roll_ = 2;
    static const ::uint32_t mask_roll_ = 0x0000001c;
    static const int pos_gap_ = 0;
    static const ::uint32_t mask_gap_ = 0x00000003;
    /** @} */

    static const ::uint32_t mask_chamber_id = 0xffffffe0;
    static const ::uint32_t mask_roll_id    = 0xfffffffc;

public:
    RollId();
    /** Construct from a packed id.
     * The id must have Det field part Muon, SubDet RPC
     * and SubDetType RPCDetId or RPCCompDetId.
     * An exception will be thrown otherwise.
     */
    RollId(::uint32_t const & _id);
    /** Construct using the individual fields.
     * An exception will be thrown if a field is out of range.
     */
    RollId(int _region
           , int _station
           , int _layer = wildcard_
           , int _ring = wildcard_
           , int _sector = wildcard_
           , int _subsector = wildcard_
           , int _subsubsector = wildcard_
           , int _roll = wildcard_
           , int _gap = wildcard_);
    /** Additional constructor from RPCDetid */
    RollId(RPCDetId const & _rpcdetid, int _gap = wildcard_);
    /** Additional constructor from DataBase name */
    RollId(std::string const & _chamber, std::string const & _roll = std::string(""));

    ::uint32_t id() const;
    operator ::uint32_t() const;
    ::uint32_t mask() const;

    bool matches(RollId const & _in) const;

    void id(::uint32_t const & _in);
    void reset();

    /** \name Field Getters
     * @{ */
    int det() const;
    int subdet() const;
    int subdettype() const;
    int region() const;
    int station() const;
    int layer() const;
    int ring() const;
    int sector() const;
    int subsector() const;
    int subsubsector() const;
    int roll() const;
    int gap() const;
    /** @} */

    /** \name Derived Getters
     * @{ */
    RollId getChamberId() const;
    RollId getRollId() const;

    RPCDetId getRPCDetId() const;

    bool barrel() const;
    bool endcap() const;

    int wheel() const;
    int disk() const;
    int endcapsector() const;

    bool reference() const;
    /** @}*/

    /** \name Field Setters
     * A std::out_of_range is thrown for out-of-range input values.
     * @{ */
    void setDet();
    void setSubDet();
    void setSubDetType();
    void setRegion(int _region = wildcard_);
    void setStation(int _station = wildcard_);
    void setLayer(int _layer = wildcard_);
    void setRing(int _ring = wildcard_);
    void setSector(int _sector = wildcard_);
    void setSubSector(int _subsector = wildcard_);
    void setSubSubSector(int _subsubsector = wildcard_);
    void setRoll(int _roll = wildcard_);
    void setGap(int _gap = wildcard_);
    /** @} */

    /** modified */
    std::string name() const;
    /** additional */
    std::ostream & stream(std::ostream & _os) const;

    bool operator<(RollId const & _in) const;
    bool operator==(RollId const & _in) const;
    bool operator!=(RollId const & _in) const;
    bool operator<(::uint32_t const & _in) const;
    bool operator==(::uint32_t const & _in) const;
    bool operator!=(::uint32_t const & _in) const;

    /** additional */
    void set(RPCDetId const & _rpcdetid);
    void parseDBName(std::string const & _chamber, std::string const & _roll = std::string(""));
    void parseName(std::string const & _roll);

protected:
    void bf_remask() const;
    int bf_get(int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const;
    void bf_set(int const _wc, int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value);
    void bf_stream(std::ostream & _os, int const _wc, int const _min, ::uint32_t const _mask, int const _pos) const;

protected:
    ::uint32_t id_;
    mutable ::uint32_t mask_;
};

std::ostream & operator<<(std::ostream & _os, RollId const & _in);

} // namespace tools
} // namespace rpct

#include "rpct/tools/RollId-inl.h"

#endif // rpct_tools_RollId_h
