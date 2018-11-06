#ifndef rpct_hwd_fwd_inl_h
#define rpct_hwd_fwd_inl_h

#include "rpct/hwd/fwd.h"

#include <algorithm>
#include <cstring>

namespace rpct {
namespace hwd {

template<typename t_cpp_type>
DataType const & storage_type<t_cpp_type, true, true>::datatype()
{
    return DataType::integer_;
}

template<typename t_cpp_type>
inline typename storage_type<t_cpp_type, true, true>::sqlite_type
storage_type<t_cpp_type, true, true>::get(cpp_type const & _value)
{
    return _value;
}

template<typename t_cpp_type>
inline void
storage_type<t_cpp_type, true, true>::set(sqlite_type const & _in, cpp_type & _out)
{
    _out = _in;
}

template<typename t_cpp_type>
DataType const & storage_type<t_cpp_type, true, false>::datatype()
{
    return DataType::float_;
}

template<typename t_cpp_type>
inline typename storage_type<t_cpp_type, true, false>::sqlite_type
storage_type<t_cpp_type, true, false>::get(cpp_type const & _value)
{
    return _value;
}

template<typename t_cpp_type>
inline void
storage_type<t_cpp_type, true, false>::set(sqlite_type const & _in, cpp_type & _out)
{
    _out = _in;
}

inline storage_type<std::string, false, false>::sqlite_type
storage_type<std::string, false, false>::get(std::string const & _value)
{
    return _value;
}

inline void
storage_type<std::string, false, false>::set(sqlite_type const & _in, std::string & _out)
{
    _out = _in;
}

template<typename t_cpp_type>
DataType const & storage_type<t_cpp_type, false, false>::datatype()
{
    return DataType::blob_;
}

template<typename t_cpp_type>
inline typename storage_type<t_cpp_type, false, false>::sqlite_type
storage_type<t_cpp_type, false, false>::get(cpp_type const & _value)
{
    return sqlite_type((unsigned char const *)(&_value)
                       , (unsigned char const *)(&_value) + sizeof(_value));
}

template<typename t_cpp_type>
inline void
storage_type<t_cpp_type, false, false>::set(sqlite_type const & _in, cpp_type & _out)
{
    std::memcpy(&_out, &(_in[0]), std::min(sizeof(cpp_type), _in.size()));
}

template<typename t_cpp_type>
DataType const & storage_type<std::vector<t_cpp_type>, false, false>::datatype()
{
    return DataType::blob_;
}

template<typename t_cpp_type>
inline typename storage_type<std::vector<t_cpp_type>, false, false>::sqlite_type
storage_type<std::vector<t_cpp_type>, false, false>::get(cpp_type const & _value)
{
    return sqlite_type((unsigned char const *)(&(_value[0]))
                       , (unsigned char const *)(&(_value[0]))
                       + sizeof(t_cpp_type) * _value.size());
}

template<typename t_cpp_type>
inline void
storage_type<std::vector<t_cpp_type>, false, false>::set(sqlite_type const & _in, cpp_type & _out)
{
    _out.assign((t_cpp_type const *)(&(_in[0]))
                , (t_cpp_type const *)(&(_in[0]))
                + _in.size()/sizeof(t_cpp_type));
}

template<typename t_cpp_type>
DataType const & storage_type<std::set<t_cpp_type>, false, false>::datatype()
{
    return DataType::blob_;
}

template<typename t_cpp_type>
inline typename storage_type<std::set<t_cpp_type>, false, false>::sqlite_type
storage_type<std::set<t_cpp_type>, false, false>::get(cpp_type const & _value)
{
    sqlite_type _return;
    _return.reserve(sizeof(t_cpp_type) * _value.size());
    for (typename cpp_type::const_iterator _it = _value.begin()
             ; _it != _value.end() ; ++_it)
        {
            unsigned char const * _uc_end = (unsigned char const *)(&(*_it)) +  sizeof(t_cpp_type);
            for (unsigned char const * _uc = (unsigned char const *)(&(*_it))
                     ; _uc < _uc_end ; ++_uc)
                _return.push_back(*_uc);
        }
    return _return;
}

template<typename t_cpp_type>
inline void
storage_type<std::set<t_cpp_type>, false, false>::set(sqlite_type const & _in, cpp_type & _out)
{
    _out.insert((t_cpp_type const *)(&(_in[0]))
                , (t_cpp_type const *)(&(_in[0]))
                + _in.size()/sizeof(t_cpp_type));
}

} // namespace hwd
} // namespace rpct

#endif // rpct_hwd_fwd_inl_h
