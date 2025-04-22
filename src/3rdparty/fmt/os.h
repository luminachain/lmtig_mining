






#ifndef FMT_OS_H_
#define FMT_OS_H_

#if defined(__MINGW32__) || defined(__CYGWIN__)

#  undef __STRICT_ANSI__
#endif

#include <cerrno>
#include <clocale>  
#include <cstddef>
#include <cstdio>
#include <cstdlib>  

#if defined __APPLE__ || defined(__FreeBSD__)
#  include <xlocale.h>  
#endif

#include "format.h"


#if FMT_HAS_INCLUDE("winapifamily.h")
#  include <winapifamily.h>
#endif
#if (FMT_HAS_INCLUDE(<fcntl.h>) || defined(__APPLE__) || \
     defined(__linux__)) &&                              \
    (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
#  include <fcntl.h>  
#  define FMT_USE_FCNTL 1
#else
#  define FMT_USE_FCNTL 0
#endif

#ifndef FMT_POSIX
#  if defined(_WIN32) && !defined(__MINGW32__)

#    define FMT_POSIX(call) _##call
#  else
#    define FMT_POSIX(call) call
#  endif
#endif


#ifdef FMT_SYSTEM
#  define FMT_POSIX_CALL(call) FMT_SYSTEM(call)
#else
#  define FMT_SYSTEM(call) ::call
#  ifdef _WIN32

#    define FMT_POSIX_CALL(call) ::_##call
#  else
#    define FMT_POSIX_CALL(call) ::call
#  endif
#endif



#ifndef _WIN32
#  define FMT_RETRY_VAL(result, expression, error_result) \
    do {                                                  \
      (result) = (expression);                            \
    } while ((result) == (error_result) && errno == EINTR)
#else
#  define FMT_RETRY_VAL(result, expression, error_result) result = (expression)
#endif

#define FMT_RETRY(result, expression) FMT_RETRY_VAL(result, expression, -1)

FMT_BEGIN_NAMESPACE


template <typename Char> class basic_cstring_view {
 private:
  const Char* data_;

 public:
  
  basic_cstring_view(const Char* s) : data_(s) {}

  
  basic_cstring_view(const std::basic_string<Char>& s) : data_(s.c_str()) {}

  
  const Char* c_str() const { return data_; }
};

using cstring_view = basic_cstring_view<char>;
using wcstring_view = basic_cstring_view<wchar_t>;


class error_code {
 private:
  int value_;

 public:
  explicit error_code(int value = 0) FMT_NOEXCEPT : value_(value) {}

  int get() const FMT_NOEXCEPT { return value_; }
};

#ifdef _WIN32
namespace detail {


class utf16_to_utf8 {
 private:
  memory_buffer buffer_;

 public:
  utf16_to_utf8() {}
  FMT_API explicit utf16_to_utf8(wstring_view s);
  operator string_view() const { return string_view(&buffer_[0], size()); }
  size_t size() const { return buffer_.size() - 1; }
  const char* c_str() const { return &buffer_[0]; }
  std::string str() const { return std::string(&buffer_[0], size()); }

  
  
  
  FMT_API int convert(wstring_view s);
};

FMT_API void format_windows_error(buffer<char>& out, int error_code,
                                  string_view message) FMT_NOEXCEPT;
}  


class windows_error : public system_error {
 private:
  FMT_API void init(int error_code, string_view format_str, format_args args);

 public:
  
  template <typename... Args>
  windows_error(int error_code, string_view message, const Args&... args) {
    init(error_code, message, make_format_args(args...));
  }
};



FMT_API void report_windows_error(int error_code,
                                  string_view message) FMT_NOEXCEPT;
#endif  


class buffered_file {
 private:
  FILE* file_;

  friend class file;

  explicit buffered_file(FILE* f) : file_(f) {}

 public:
  buffered_file(const buffered_file&) = delete;
  void operator=(const buffered_file&) = delete;

  
  buffered_file() FMT_NOEXCEPT : file_(nullptr) {}

  
  FMT_API ~buffered_file() FMT_NOEXCEPT;

 public:
  buffered_file(buffered_file&& other) FMT_NOEXCEPT : file_(other.file_) {
    other.file_ = nullptr;
  }

  buffered_file& operator=(buffered_file&& other) {
    close();
    file_ = other.file_;
    other.file_ = nullptr;
    return *this;
  }

  
  FMT_API buffered_file(cstring_view filename, cstring_view mode);

  
  FMT_API void close();

  
  FILE* get() const FMT_NOEXCEPT { return file_; }

  
  
  FMT_API int(fileno)() const;

  void vprint(string_view format_str, format_args args) {
    fmt::vprint(file_, format_str, args);
  }

  template <typename... Args>
  inline void print(string_view format_str, const Args&... args) {
    vprint(format_str, make_format_args(args...));
  }
};

#if FMT_USE_FCNTL






class file {
 private:
  int fd_;  

  
  explicit file(int fd) : fd_(fd) {}

 public:
  
  enum {
    RDONLY = FMT_POSIX(O_RDONLY),  
    WRONLY = FMT_POSIX(O_WRONLY),  
    RDWR = FMT_POSIX(O_RDWR),      
    CREATE = FMT_POSIX(O_CREAT),   
    APPEND = FMT_POSIX(O_APPEND)   
  };

  
  file() FMT_NOEXCEPT : fd_(-1) {}

  
  FMT_API file(cstring_view path, int oflag);

 public:
  file(const file&) = delete;
  void operator=(const file&) = delete;

  file(file&& other) FMT_NOEXCEPT : fd_(other.fd_) { other.fd_ = -1; }

  file& operator=(file&& other) FMT_NOEXCEPT {
    close();
    fd_ = other.fd_;
    other.fd_ = -1;
    return *this;
  }

  
  FMT_API ~file() FMT_NOEXCEPT;

  
  int descriptor() const FMT_NOEXCEPT { return fd_; }

  
  FMT_API void close();

  
  
  FMT_API long long size() const;

  
  FMT_API size_t read(void* buffer, size_t count);

  
  FMT_API size_t write(const void* buffer, size_t count);

  
  
  FMT_API static file dup(int fd);

  
  
  FMT_API void dup2(int fd);

  
  
  FMT_API void dup2(int fd, error_code& ec) FMT_NOEXCEPT;

  
  
  FMT_API static void pipe(file& read_end, file& write_end);

  
  
  FMT_API buffered_file fdopen(const char* mode);
};


long getpagesize();

namespace detail {

struct buffer_size {
  size_t value = 0;
  buffer_size operator=(size_t val) const {
    auto bs = buffer_size();
    bs.value = val;
    return bs;
  }
};

struct ostream_params {
  int oflag = file::WRONLY | file::CREATE;
  size_t buffer_size = BUFSIZ > 32768 ? BUFSIZ : 32768;

  ostream_params() {}

  template <typename... T>
  ostream_params(T... params, int oflag) : ostream_params(params...) {
    this->oflag = oflag;
  }

  template <typename... T>
  ostream_params(T... params, detail::buffer_size bs)
      : ostream_params(params...) {
    this->buffer_size = bs.value;
  }
};
}  

static constexpr detail::buffer_size buffer_size;


class ostream : private detail::buffer<char> {
 private:
  file file_;

  void flush() {
    if (size() == 0) return;
    file_.write(data(), size());
    clear();
  }

  void grow(size_t) final;

  ostream(cstring_view path, const detail::ostream_params& params)
      : file_(path, params.oflag) {
    set(new char[params.buffer_size], params.buffer_size);
  }

 public:
  ostream(ostream&& other)
      : detail::buffer<char>(other.data(), other.size(), other.capacity()),
        file_(std::move(other.file_)) {
    other.set(nullptr, 0);
  }
  ~ostream() {
    flush();
    delete[] data();
  }

  template <typename... T>
  friend ostream output_file(cstring_view path, T... params);

  void close() {
    flush();
    file_.close();
  }

  template <typename S, typename... Args>
  void print(const S& format_str, const Args&... args) {
    format_to(detail::buffer_appender<char>(*this), format_str, args...);
  }
};


template <typename... T>
inline ostream output_file(cstring_view path, T... params) {
  return {path, detail::ostream_params(params...)};
}
#endif  

#ifdef FMT_LOCALE

class locale {
 private:
#  ifdef _WIN32
  using locale_t = _locale_t;

  static void freelocale(locale_t loc) { _free_locale(loc); }

  static double strtod_l(const char* nptr, char** endptr, _locale_t loc) {
    return _strtod_l(nptr, endptr, loc);
  }
#  endif

  locale_t locale_;

 public:
  using type = locale_t;
  locale(const locale&) = delete;
  void operator=(const locale&) = delete;

  locale() {
#  ifndef _WIN32
    locale_ = FMT_SYSTEM(newlocale(LC_NUMERIC_MASK, "C", nullptr));
#  else
    locale_ = _create_locale(LC_NUMERIC, "C");
#  endif
    if (!locale_) FMT_THROW(system_error(errno, "cannot create locale"));
  }
  ~locale() { freelocale(locale_); }

  type get() const { return locale_; }

  
  
  double strtod(const char*& str) const {
    char* end = nullptr;
    double result = strtod_l(str, &end, locale_);
    str = end;
    return result;
  }
};
using Locale FMT_DEPRECATED_ALIAS = locale;
#endif  
FMT_END_NAMESPACE

#endif  
