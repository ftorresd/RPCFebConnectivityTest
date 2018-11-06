#ifndef rpct_tools_RollSelection_inl_h
#define rpct_tools_RollSelection_inl_h

#include "rpct/tools/RollSelection.h"

namespace rpct {
namespace tools {

inline std::set<RollId> const &
RollSelection::getSelected() const
{
    return selected_rolls_;
}

inline std::set<RollId> const &
RollSelection::getMasked() const
{
    return masked_rolls_;
}

inline void RollSelection::select(RollId const & _roll)
{
    masked_rolls_.erase(_roll);
    selected_rolls_.insert(_roll);
}

inline void RollSelection::deselect(RollId const & _roll)
{
    selected_rolls_.erase(_roll);
}

inline void RollSelection::mask(RollId const & _roll)
{
    selected_rolls_.erase(_roll);
    masked_rolls_.insert(_roll);
}

inline void RollSelection::unmask(RollId const & _roll)
{
    masked_rolls_.erase(_roll);
}

inline void RollSelection::clear()
{
    selected_rolls_.clear();
    masked_rolls_.clear();
}

} // namespace tools
} // namespace rpct

#endif // rpct_tools_RollSelection_inl_h
