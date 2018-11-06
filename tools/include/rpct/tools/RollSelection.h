#ifndef rpct_tools_RollSelection_h
#define rpct_tools_RollSelection_h

#include <set>

#include "rpct/tools/RollId.h"

namespace rpct {
namespace tools {

class RollSelection
{
public:
    RollSelection();

    std::set<RollId> const & getSelected() const;
    std::set<RollId> const & getMasked() const;

    void select(RollId const & _roll);
    void deselect(RollId const & _roll);
    void mask(RollId const & _roll);
    void unmask(RollId const & _roll);

    void clear();

    bool matches(RollId const & _roll) const;

protected:
    std::set<RollId> selected_rolls_, masked_rolls_;
};

} // namespace tools
} // namespace rpct

#include "rpct/tools/RollSelection-inl.h"

#endif // rpct_tools_RollSelection_h
