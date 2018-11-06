#include "rpct/hwd/State.h"

#include <ostream>

namespace rpct {
namespace hwd {

std::string const State::parametertype_name_ = std::string("State");

std::string const State::descriptions_[]
= {
    std::string("ok"),
    std::string("notset"),
    std::string("info"),
    std::string("warn"),
    std::string("error"),
    std::string("unknown")
};

std::string const State::colors_[]
= {
    std::string("#67e667"),
    std::string("#eee"),
    std::string("#6C8Ce5"),
    std::string("#ffd073"),
    std::string("#ff7373"),
    std::string("#ffd073")
};

std::string const State::bgcolors_[]
= {
    std::string("#dfd"),
    std::string("#efefef"),
    std::string("#ddf"),
    std::string("#ffb"),
    std::string("#fdd"),
    std::string("#ffb")
};

State::State(unsigned char _state)
    : state_(_state)
{}

State::~State()
{}

std::ostream & operator<<(std::ostream & _os, State const & _state)
{
    return (_os << _state.getDescription());
}

} // namespace hwd
} // namespace rpct
