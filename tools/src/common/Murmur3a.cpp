#include "rpct/tools/Murmur3a.h"

namespace rpct {
namespace tools {

::uint32_t Murmur3a::hash(unsigned char const * _data, int _size
                          , ::uint32_t _seed)
{
    ::uint32_t const * _blockdata = (::uint32_t const *)(_data);
    ::uint32_t const * _blockdata_end = _blockdata + (_size / 4);

    for( ; _blockdata < _blockdata_end ; ++_blockdata)
        {
            _seed ^= rotl(*_blockdata * c1_, 15) * c2_;
            _seed = rotl(_seed, 13);
            _seed = _seed * 5 + 0xe6546b64;
        }

    if ((_size & 3))
        _seed ^= rotl((*_blockdata_end & (0xffffffff >> (32 - 8*(_size & 3)))) * c1_, 15) * c2_;

    _seed ^= _size;

    _seed ^= _seed >> 16;
    _seed *= 0x85ebca6b;
    _seed ^= _seed >> 13;
    _seed *= 0xc2b2ae35;
    _seed ^= _seed >> 16;

    return _seed;
}

} // namespace tools
} // namespace rpct
