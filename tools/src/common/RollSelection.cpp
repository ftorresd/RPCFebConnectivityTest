#include "rpct/tools/RollSelection.h"

namespace rpct {
namespace tools {

RollSelection::RollSelection()
{}

bool RollSelection::matches(RollId const & _roll) const
{
    for (std::set<RollId>::const_iterator _mask = masked_rolls_.begin()
             ; _mask != masked_rolls_.end() ; ++_mask)
        if (_mask->matches(_roll))
            return false;
    for (std::set<RollId>::const_iterator _select = selected_rolls_.begin()
             ; _select != selected_rolls_.end() ; ++_select)
        if (_select->matches(_roll))
            return true;
    return false;
}

} // namespace tools
} // namespace rpct
