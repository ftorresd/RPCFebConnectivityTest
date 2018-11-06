#ifndef rpct_hwd_State_h
#define rpct_hwd_State_h

#include <string>
#include <iosfwd>

namespace rpct {
namespace hwd {

class State
{
public:
    static std::string const parametertype_name_;

    static unsigned char const ok_      = 0;
    static unsigned char const notset_  = 1;
    static unsigned char const info_    = 2;
    static unsigned char const warn_    = 3;
    static unsigned char const error_   = 4;
    static unsigned char const unknown_ = 5;
    static unsigned char const default_ = notset_;
    static unsigned char const max_     = 5;

protected:
    static std::string const descriptions_[6];
    static std::string const colors_[6];
    static std::string const bgcolors_[6];

public:
    State(unsigned char _state = default_);
    virtual ~State();

    unsigned char getState() const;
    virtual void setState(State const & _state);
    virtual void setState(unsigned char _state);
    virtual void reset();

    std::string const & getDescription() const;
    std::string const & getColor() const;
    std::string const & getBackgroundColor() const;

    void setOk();
    bool isOk() const;
    void setNotSet();
    bool isNotSet() const;
    void setInfo();
    bool isInfo() const;
    void setWarn();
    bool isWarn() const;
    void setError();
    bool isError() const;

    bool operator< (State const & _state);
    bool operator==(State const & _state);
    bool operator!=(State const & _state);

protected:
    unsigned char state_;
};

std::ostream & operator<<(std::ostream & _os, State const & _state);

} // namespace hwd
} // namespace rpct

#include "rpct/hwd/State-inl.h"

#endif // rpct_hwd_State_h
