/* Filip Thyssen */

#ifndef rpct_tools_Murmur3a_h_
#define rpct_tools_Murmur3a_h_

#include <stdint.h>

#include <string>
#include <vector>

namespace rpct {
namespace tools {

/**
 *  Implementation of Austin Appleby's MurmurHash3
 *  https://code.google.com/p/smhasher/
 */
class Murmur3a {
protected:
    static ::uint32_t const c1_ = 0xcc9e2d51;
    static ::uint32_t const c2_ = 0x1b873593;

protected:
    static ::uint32_t rotl(::uint32_t _x, int8_t _r);

public:
    static ::uint32_t hash(std::string const & _data
                           , ::uint32_t _seed = 0);
    static ::uint32_t hash(std::vector<unsigned char> const & _data
                           , ::uint32_t _seed = 0);
    static ::uint32_t hash(unsigned char const * _data, int _size
                           , ::uint32_t _seed = 0);
};

} // namespace tools
} // namespace rpct

#include "rpct/tools/Murmur3a-inl.h"

#endif // rpct/tools_Murmur3a_h_
