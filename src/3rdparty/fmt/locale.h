






#ifndef FMT_LOCALE_H_
#define FMT_LOCALE_H_

#include <locale>

#include "format.h"

FMT_BEGIN_NAMESPACE

namespace detail {
template <typename Char>
typename buffer_context<Char>::iterator vformat_to(
    const std::locale& loc, buffer<Char>& buf,
    basic_string_view<Char> format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args) {
  using af = arg_formatter<typename buffer_context<Char>::iterator, Char>;
  return vformat_to<af>(buffer_appender<Char>(buf), to_string_view(format_str),
                        args, detail::locale_ref(loc));
}

template <typename Char>
std::basic_string<Char> vformat(
    const std::locale& loc, basic_string_view<Char> format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args) {
  basic_memory_buffer<Char> buffer;
  detail::vformat_to(loc, buffer, format_str, args);
  return fmt::to_string(buffer);
}
}  

template <typename S, typename Char = char_t<S>>
inline std::basic_string<Char> vformat(
    const std::locale& loc, const S& format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args) {
  return detail::vformat(loc, to_string_view(format_str), args);
}

template <typename S, typename... Args, typename Char = char_t<S>>
inline std::basic_string<Char> format(const std::locale& loc,
                                      const S& format_str, Args&&... args) {
  return detail::vformat(
      loc, to_string_view(format_str),
      fmt::make_args_checked<Args...>(format_str, args...));
}

template <typename S, typename OutputIt, typename... Args,
          typename Char = enable_if_t<
              detail::is_output_iterator<OutputIt>::value, char_t<S>>>
inline OutputIt vformat_to(
    OutputIt out, const std::locale& loc, const S& format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args) {
  decltype(detail::get_buffer<Char>(out)) buf(detail::get_buffer_init(out));
  using af =
    detail::arg_formatter<typename buffer_context<Char>::iterator, Char>;
  vformat_to<af>(detail::buffer_appender<Char>(buf), to_string_view(format_str),
                 args, detail::locale_ref(loc));
  return detail::get_iterator(buf);
}

template <typename OutputIt, typename S, typename... Args,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt>::value&&
                            detail::is_string<S>::value)>
inline OutputIt format_to(OutputIt out, const std::locale& loc,
                          const S& format_str, Args&&... args) {
  const auto& vargs = fmt::make_args_checked<Args...>(format_str, args...);
  return vformat_to(out, loc, to_string_view(format_str), vargs);
}

FMT_END_NAMESPACE

#endif  
