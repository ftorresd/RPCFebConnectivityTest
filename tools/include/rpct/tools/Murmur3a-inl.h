/* Filip Thyssen */

#ifndef rpct_tools_Murmur3a_inl_h_
#define rpct_tools_Murmur3a_inl_h_

#include "rpct/tools/Murmur3a.h"

namespace rpct {
namespace tools {

inline ::uint32_t Murmur3a::rotl(::uint32_t _x, ::int8_t _r)
{
    return (_x << _r) | (_x >> (32 - _r));
}

inline ::uint32_t Murmur3a::hash(std::string const & _data
                                 , ::uint32_t _seed)
{
    return hash((unsigned char const *)(&(_data[0])), _data.size(), _seed);
}

inline ::uint32_t Murmur3a::hash(std::vector<unsigned char> const & _data
                                 , ::uint32_t _seed)
{
    return hash(&(_data[0]), _data.size(), _seed);
}

} // namespace tools
} // namespace rpct

#endif // rpct/tools_Murmur3a_inl_h_
