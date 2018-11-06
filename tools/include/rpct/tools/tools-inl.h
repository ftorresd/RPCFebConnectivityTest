/* Filip Thyssen */

#ifndef rpct_tools_tools_inl_h_
#define rpct_tools_tools_inl_h_

#include "rpct/tools/tools.h"

namespace rpct {
namespace tools {

inline unsigned int bit_count(::uint64_t _value)
{
    // sum per nibble
    _value -= (
               (  (_value >> 1) & 0x7777777777777777)
               + ((_value >> 2) & 0x3333333333333333)
               + ((_value >> 3) & 0x1111111111111111)
               );
    _value = (_value + (_value >> 4)) & 0x0F0F0F0F0F0F0F0F;
    _value += _value >> 8;
    _value += _value >> 16;
    _value += _value >> 32;
    return (_value & 0x7F);
}

inline unsigned int bit_count(::uint32_t _value)
{
    // sum per nibble
    _value -= (
               (  (_value >> 1) & 0x77777777)
               + ((_value >> 2) & 0x33333333)
               + ((_value >> 3) & 0x11111111)
               );
    _value = (_value + (_value >> 4)) & 0x0F0F0F0F;
    _value += _value >> 8;
    _value += _value >> 16;
    return (_value & 0x3F);
}

inline unsigned int bit_count(::uint16_t _value)
{
    // sum per nibble
    _value -= (
               (  (_value >> 1) & 0x7777)
               + ((_value >> 2) & 0x3333)
               + ((_value >> 3) & 0x1111)
               );
    _value = (_value + (_value >> 4)) & 0x0F0F;
    return ((_value + (_value >> 8)) & 0x1F);
}

inline unsigned int bit_count(unsigned char _value)
{
    // sum per nibble
    _value -= (
               (  (_value >> 1) & 0x77)
               + ((_value >> 2) & 0x33)
               + ((_value >> 3) & 0x11)
               );
    return ((_value + (_value >> 4)) & 0x0F);
}

} // namespace tools
} // namespace rpct

#endif // rpct/tools_tools_inl_h_
