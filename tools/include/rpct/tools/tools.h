/* Filip Thyssen */

#ifndef rpct_tools_tools_h_
#define rpct_tools_tools_h_

#include <stdint.h>

namespace rpct {
namespace tools {

/** Count bits in variable
    @{
*/
unsigned int bit_count(::uint64_t _value);
unsigned int bit_count(::uint32_t _value);
unsigned int bit_count(::uint16_t _value);
unsigned int bit_count(unsigned char _value);
/** @} */

} // namespace tools
} // namespace rpct

#include "rpct/tools/tools-inl.h"

#endif // rpct/tools_tools_h_
