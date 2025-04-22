






#ifndef FMT_PRINTF_H_
#define FMT_PRINTF_H_

#include <algorithm>  
#include <limits>     

#include "ostream.h"

FMT_BEGIN_NAMESPACE
namespace detail {



template <bool IsSigned> struct int_checker {
  template <typename T> static bool fits_in_int(T value) {
    unsigned max = max_value<int>();
    return value <= max;
  }
  static bool fits_in_int(bool) { return true; }
};

template <> struct int_checker<true> {
  template <typename T> static bool fits_in_int(T value) {
    return value >= (std::numeric_limits<int>::min)() &&
           value <= max_value<int>();
  }
  static bool fits_in_int(int) { return true; }
};

class printf_precision_handler {
 public:
  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  int operator()(T value) {
    if (!int_checker<std::numeric_limits<T>::is_signed>::fits_in_int(value))
      FMT_THROW(format_error("number is too big"));
    return (std::max)(static_cast<int>(value), 0);
  }

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  int operator()(T) {
    FMT_THROW(format_error("precision is not integer"));
    return 0;
  }
};


class is_zero_int {
 public:
  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  bool operator()(T value) {
    return value == 0;
  }

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  bool operator()(T) {
    return false;
  }
};

template <typename T> struct make_unsigned_or_bool : std::make_unsigned<T> {};

template <> struct make_unsigned_or_bool<bool> { using type = bool; };

template <typename T, typename Context> class arg_converter {
 private:
  using char_type = typename Context::char_type;

  basic_format_arg<Context>& arg_;
  char_type type_;

 public:
  arg_converter(basic_format_arg<Context>& arg, char_type type)
      : arg_(arg), type_(type) {}

  void operator()(bool value) {
    if (type_ != 's') operator()<bool>(value);
  }

  template <typename U, FMT_ENABLE_IF(std::is_integral<U>::value)>
  void operator()(U value) {
    bool is_signed = type_ == 'd' || type_ == 'i';
    using target_type = conditional_t<std::is_same<T, void>::value, U, T>;
    if (const_check(sizeof(target_type) <= sizeof(int))) {
      
      if (is_signed) {
        arg_ = detail::make_arg<Context>(
            static_cast<int>(static_cast<target_type>(value)));
      } else {
        using unsigned_type = typename make_unsigned_or_bool<target_type>::type;
        arg_ = detail::make_arg<Context>(
            static_cast<unsigned>(static_cast<unsigned_type>(value)));
      }
    } else {
      if (is_signed) {
        
        
        
        arg_ = detail::make_arg<Context>(static_cast<long long>(value));
      } else {
        arg_ = detail::make_arg<Context>(
            static_cast<typename make_unsigned_or_bool<U>::type>(value));
      }
    }
  }

  template <typename U, FMT_ENABLE_IF(!std::is_integral<U>::value)>
  void operator()(U) {}  
};





template <typename T, typename Context, typename Char>
void convert_arg(basic_format_arg<Context>& arg, Char type) {
  visit_format_arg(arg_converter<T, Context>(arg, type), arg);
}


template <typename Context> class char_converter {
 private:
  basic_format_arg<Context>& arg_;

 public:
  explicit char_converter(basic_format_arg<Context>& arg) : arg_(arg) {}

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  void operator()(T value) {
    arg_ = detail::make_arg<Context>(
        static_cast<typename Context::char_type>(value));
  }

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  void operator()(T) {}  
};



template <typename Char> struct get_cstring {
  template <typename T> const Char* operator()(T) { return nullptr; }
  const Char* operator()(const Char* s) { return s; }
};



template <typename Char> class printf_width_handler {
 private:
  using format_specs = basic_format_specs<Char>;

  format_specs& specs_;

 public:
  explicit printf_width_handler(format_specs& specs) : specs_(specs) {}

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  unsigned operator()(T value) {
    auto width = static_cast<uint32_or_64_or_128_t<T>>(value);
    if (detail::is_negative(value)) {
      specs_.align = align::left;
      width = 0 - width;
    }
    unsigned int_max = max_value<int>();
    if (width > int_max) FMT_THROW(format_error("number is too big"));
    return static_cast<unsigned>(width);
  }

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  unsigned operator()(T) {
    FMT_THROW(format_error("width is not integer"));
    return 0;
  }
};

template <typename Char, typename Context>
void vprintf(buffer<Char>& buf, basic_string_view<Char> format,
             basic_format_args<Context> args) {
  Context(buffer_appender<Char>(buf), format, args).format();
}
}  


template <typename Char, typename Context>
FMT_DEPRECATED void printf(detail::buffer<Char>& buf,
                           basic_string_view<Char> format,
                           basic_format_args<Context> args) {
  return detail::vprintf(buf, format, args);
}
using detail::vprintf;

template <typename Char>
class basic_printf_parse_context : public basic_format_parse_context<Char> {
  using basic_format_parse_context<Char>::basic_format_parse_context;
};
template <typename OutputIt, typename Char> class basic_printf_context;


template <typename OutputIt, typename Char>
class printf_arg_formatter : public detail::arg_formatter_base<OutputIt, Char> {
 public:
  using iterator = OutputIt;

 private:
  using char_type = Char;
  using base = detail::arg_formatter_base<OutputIt, Char>;
  using context_type = basic_printf_context<OutputIt, Char>;

  context_type& context_;

  void write_null_pointer(char) {
    this->specs()->type = 0;
    this->write("(nil)");
  }

  void write_null_pointer(wchar_t) {
    this->specs()->type = 0;
    this->write(L"(nil)");
  }

 public:
  using format_specs = typename base::format_specs;

  
  printf_arg_formatter(iterator iter, format_specs& specs, context_type& ctx)
      : base(iter, &specs, detail::locale_ref()), context_(ctx) {}

  template <typename T, FMT_ENABLE_IF(fmt::detail::is_integral<T>::value)>
  iterator operator()(T value) {
    
    
    if (std::is_same<T, bool>::value) {
      format_specs& fmt_specs = *this->specs();
      if (fmt_specs.type != 's') return base::operator()(value ? 1 : 0);
      fmt_specs.type = 0;
      this->write(value != 0);
    } else if (std::is_same<T, char_type>::value) {
      format_specs& fmt_specs = *this->specs();
      if (fmt_specs.type && fmt_specs.type != 'c')
        return (*this)(static_cast<int>(value));
      fmt_specs.sign = sign::none;
      fmt_specs.alt = false;
      fmt_specs.fill[0] = ' ';  
      
      
      if (fmt_specs.align == align::none || fmt_specs.align == align::numeric)
        fmt_specs.align = align::right;
      return base::operator()(value);
    } else {
      return base::operator()(value);
    }
    return this->out();
  }

  template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
  iterator operator()(T value) {
    return base::operator()(value);
  }

  
  iterator operator()(const char* value) {
    if (value)
      base::operator()(value);
    else if (this->specs()->type == 'p')
      write_null_pointer(char_type());
    else
      this->write("(null)");
    return this->out();
  }

  
  iterator operator()(const wchar_t* value) {
    if (value)
      base::operator()(value);
    else if (this->specs()->type == 'p')
      write_null_pointer(char_type());
    else
      this->write(L"(null)");
    return this->out();
  }

  iterator operator()(basic_string_view<char_type> value) {
    return base::operator()(value);
  }

  iterator operator()(monostate value) { return base::operator()(value); }

  
  iterator operator()(const void* value) {
    if (value) return base::operator()(value);
    this->specs()->type = 0;
    write_null_pointer(char_type());
    return this->out();
  }

  
  iterator operator()(typename basic_format_arg<context_type>::handle handle) {
    handle.format(context_.parse_context(), context_);
    return this->out();
  }
};

template <typename T> struct printf_formatter {
  printf_formatter() = delete;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const T& value, FormatContext& ctx) -> decltype(ctx.out()) {
    detail::format_value(detail::get_container(ctx.out()), value);
    return ctx.out();
  }
};


template <typename OutputIt, typename Char> class basic_printf_context {
 public:
  
  using char_type = Char;
  using iterator = OutputIt;
  using format_arg = basic_format_arg<basic_printf_context>;
  using parse_context_type = basic_printf_parse_context<Char>;
  template <typename T> using formatter_type = printf_formatter<T>;

 private:
  using format_specs = basic_format_specs<char_type>;

  OutputIt out_;
  basic_format_args<basic_printf_context> args_;
  parse_context_type parse_ctx_;

  static void parse_flags(format_specs& specs, const Char*& it,
                          const Char* end);

  
  
  format_arg get_arg(int arg_index = -1);

  
  int parse_header(const Char*& it, const Char* end, format_specs& specs);

 public:
  
  basic_printf_context(OutputIt out, basic_string_view<char_type> format_str,
                       basic_format_args<basic_printf_context> args)
      : out_(out), args_(args), parse_ctx_(format_str) {}

  OutputIt out() { return out_; }
  void advance_to(OutputIt it) { out_ = it; }

  detail::locale_ref locale() { return {}; }

  format_arg arg(int id) const { return args_.get(id); }

  parse_context_type& parse_context() { return parse_ctx_; }

  FMT_CONSTEXPR void on_error(const char* message) {
    parse_ctx_.on_error(message);
  }

  
  template <typename ArgFormatter = printf_arg_formatter<OutputIt, Char>>
  OutputIt format();
};

template <typename OutputIt, typename Char>
void basic_printf_context<OutputIt, Char>::parse_flags(format_specs& specs,
                                                       const Char*& it,
                                                       const Char* end) {
  for (; it != end; ++it) {
    switch (*it) {
    case '-':
      specs.align = align::left;
      break;
    case '+':
      specs.sign = sign::plus;
      break;
    case '0':
      specs.fill[0] = '0';
      break;
    case ' ':
      if (specs.sign != sign::plus) {
        specs.sign = sign::space;
      }
      break;
    case '#':
      specs.alt = true;
      break;
    default:
      return;
    }
  }
}

template <typename OutputIt, typename Char>
typename basic_printf_context<OutputIt, Char>::format_arg
basic_printf_context<OutputIt, Char>::get_arg(int arg_index) {
  if (arg_index < 0)
    arg_index = parse_ctx_.next_arg_id();
  else
    parse_ctx_.check_arg_id(--arg_index);
  return detail::get_arg(*this, arg_index);
}

template <typename OutputIt, typename Char>
int basic_printf_context<OutputIt, Char>::parse_header(const Char*& it,
                                                       const Char* end,
                                                       format_specs& specs) {
  int arg_index = -1;
  char_type c = *it;
  if (c >= '0' && c <= '9') {
    
    
    detail::error_handler eh;
    int value = parse_nonnegative_int(it, end, eh);
    if (it != end && *it == '$') {  
      ++it;
      arg_index = value;
    } else {
      if (c == '0') specs.fill[0] = '0';
      if (value != 0) {
        
        
        specs.width = value;
        return arg_index;
      }
    }
  }
  parse_flags(specs, it, end);
  
  if (it != end) {
    if (*it >= '0' && *it <= '9') {
      detail::error_handler eh;
      specs.width = parse_nonnegative_int(it, end, eh);
    } else if (*it == '*') {
      ++it;
      specs.width = static_cast<int>(visit_format_arg(
          detail::printf_width_handler<char_type>(specs), get_arg()));
    }
  }
  return arg_index;
}

template <typename OutputIt, typename Char>
template <typename ArgFormatter>
OutputIt basic_printf_context<OutputIt, Char>::format() {
  auto out = this->out();
  const Char* start = parse_ctx_.begin();
  const Char* end = parse_ctx_.end();
  auto it = start;
  while (it != end) {
    char_type c = *it++;
    if (c != '%') continue;
    if (it != end && *it == c) {
      out = std::copy(start, it, out);
      start = ++it;
      continue;
    }
    out = std::copy(start, it - 1, out);

    format_specs specs;
    specs.align = align::right;

    
    int arg_index = parse_header(it, end, specs);
    if (arg_index == 0) on_error("argument not found");

    
    if (it != end && *it == '.') {
      ++it;
      c = it != end ? *it : 0;
      if ('0' <= c && c <= '9') {
        detail::error_handler eh;
        specs.precision = parse_nonnegative_int(it, end, eh);
      } else if (c == '*') {
        ++it;
        specs.precision = static_cast<int>(
            visit_format_arg(detail::printf_precision_handler(), get_arg()));
      } else {
        specs.precision = 0;
      }
    }

    format_arg arg = get_arg(arg_index);
    
    
    if (specs.precision >= 0 && arg.is_integral())
      specs.fill[0] =
          ' ';  
    if (specs.precision >= 0 && arg.type() == detail::type::cstring_type) {
      auto str = visit_format_arg(detail::get_cstring<Char>(), arg);
      auto str_end = str + specs.precision;
      auto nul = std::find(str, str_end, Char());
      arg = detail::make_arg<basic_printf_context>(basic_string_view<Char>(
          str,
          detail::to_unsigned(nul != str_end ? nul - str : specs.precision)));
    }
    if (specs.alt && visit_format_arg(detail::is_zero_int(), arg))
      specs.alt = false;
    if (specs.fill[0] == '0') {
      if (arg.is_arithmetic() && specs.align != align::left)
        specs.align = align::numeric;
      else
        specs.fill[0] = ' ';  
                              
    }

    
    c = it != end ? *it++ : 0;
    char_type t = it != end ? *it : 0;
    using detail::convert_arg;
    switch (c) {
    case 'h':
      if (t == 'h') {
        ++it;
        t = it != end ? *it : 0;
        convert_arg<signed char>(arg, t);
      } else {
        convert_arg<short>(arg, t);
      }
      break;
    case 'l':
      if (t == 'l') {
        ++it;
        t = it != end ? *it : 0;
        convert_arg<long long>(arg, t);
      } else {
        convert_arg<long>(arg, t);
      }
      break;
    case 'j':
      convert_arg<intmax_t>(arg, t);
      break;
    case 'z':
      convert_arg<size_t>(arg, t);
      break;
    case 't':
      convert_arg<std::ptrdiff_t>(arg, t);
      break;
    case 'L':
      
      
      break;
    default:
      --it;
      convert_arg<void>(arg, c);
    }

    
    if (it == end) FMT_THROW(format_error("invalid format string"));
    specs.type = static_cast<char>(*it++);
    if (arg.is_integral()) {
      
      switch (specs.type) {
      case 'i':
      case 'u':
        specs.type = 'd';
        break;
      case 'c':
        visit_format_arg(detail::char_converter<basic_printf_context>(arg),
                         arg);
        break;
      }
    }

    start = it;

    
    out = visit_format_arg(ArgFormatter(out, specs, *this), arg);
  }
  return std::copy(start, it, out);
}

template <typename Char>
using basic_printf_context_t =
    basic_printf_context<detail::buffer_appender<Char>, Char>;

using printf_context = basic_printf_context_t<char>;
using wprintf_context = basic_printf_context_t<wchar_t>;

using printf_args = basic_format_args<printf_context>;
using wprintf_args = basic_format_args<wprintf_context>;


template <typename... Args>
inline format_arg_store<printf_context, Args...> make_printf_args(
    const Args&... args) {
  return {args...};
}


template <typename... Args>
inline format_arg_store<wprintf_context, Args...> make_wprintf_args(
    const Args&... args) {
  return {args...};
}

template <typename S, typename Char = char_t<S>>
inline std::basic_string<Char> vsprintf(
    const S& format,
    basic_format_args<basic_printf_context_t<type_identity_t<Char>>> args) {
  basic_memory_buffer<Char> buffer;
  vprintf(buffer, to_string_view(format), args);
  return to_string(buffer);
}


template <typename S, typename... Args,
          typename Char = enable_if_t<detail::is_string<S>::value, char_t<S>>>
inline std::basic_string<Char> sprintf(const S& format, const Args&... args) {
  using context = basic_printf_context_t<Char>;
  return vsprintf(to_string_view(format), make_format_args<context>(args...));
}

template <typename S, typename Char = char_t<S>>
inline int vfprintf(
    std::FILE* f, const S& format,
    basic_format_args<basic_printf_context_t<type_identity_t<Char>>> args) {
  basic_memory_buffer<Char> buffer;
  vprintf(buffer, to_string_view(format), args);
  size_t size = buffer.size();
  return std::fwrite(buffer.data(), sizeof(Char), size, f) < size
             ? -1
             : static_cast<int>(size);
}


template <typename S, typename... Args,
          typename Char = enable_if_t<detail::is_string<S>::value, char_t<S>>>
inline int fprintf(std::FILE* f, const S& format, const Args&... args) {
  using context = basic_printf_context_t<Char>;
  return vfprintf(f, to_string_view(format),
                  make_format_args<context>(args...));
}

template <typename S, typename Char = char_t<S>>
inline int vprintf(
    const S& format,
    basic_format_args<basic_printf_context_t<type_identity_t<Char>>> args) {
  return vfprintf(stdout, to_string_view(format), args);
}


template <typename S, typename... Args,
          FMT_ENABLE_IF(detail::is_string<S>::value)>
inline int printf(const S& format_str, const Args&... args) {
  using context = basic_printf_context_t<char_t<S>>;
  return vprintf(to_string_view(format_str),
                 make_format_args<context>(args...));
}

template <typename S, typename Char = char_t<S>>
inline int vfprintf(
    std::basic_ostream<Char>& os, const S& format,
    basic_format_args<basic_printf_context_t<type_identity_t<Char>>> args) {
  basic_memory_buffer<Char> buffer;
  vprintf(buffer, to_string_view(format), args);
  detail::write_buffer(os, buffer);
  return static_cast<int>(buffer.size());
}


template <typename ArgFormatter, typename Char,
          typename Context =
              basic_printf_context<typename ArgFormatter::iterator, Char>>
typename ArgFormatter::iterator vprintf(
    detail::buffer<Char>& out, basic_string_view<Char> format_str,
    basic_format_args<type_identity_t<Context>> args) {
  typename ArgFormatter::iterator iter(out);
  Context(iter, format_str, args).template format<ArgFormatter>();
  return iter;
}


template <typename S, typename... Args, typename Char = char_t<S>>
inline int fprintf(std::basic_ostream<Char>& os, const S& format_str,
                   const Args&... args) {
  using context = basic_printf_context_t<Char>;
  return vfprintf(os, to_string_view(format_str),
                  make_format_args<context>(args...));
}
FMT_END_NAMESPACE

#endif  
