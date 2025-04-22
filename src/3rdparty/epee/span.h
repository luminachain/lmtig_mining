



























#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>

namespace epee
{
  
  template<typename T>
  class span
  {
    template<typename U>
    static constexpr bool safe_conversion() noexcept
    {
      
      using with_const = typename std::add_const<U>::type;
      return std::is_same<T, U>() ||
        (std::is_const<T>() && std::is_same<T, with_const>());
    }

  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = pointer;
    using const_iterator = const_pointer;

    constexpr span() noexcept : ptr(nullptr), len(0) {}
    constexpr span(std::nullptr_t) noexcept : span() {}

    
    template<typename U, typename = typename std::enable_if<safe_conversion<U>()>::type>
    constexpr span(U* const src_ptr, const std::size_t count) noexcept
      : ptr(src_ptr), len(count) {}

    
    template<std::size_t N>
    constexpr span(T (&src)[N]) noexcept : span(src, N) {}

    constexpr span(const span&) noexcept = default;
    span& operator=(const span&) noexcept = default;

    
    std::size_t remove_prefix(std::size_t amount) noexcept
    {
        amount = std::min(len, amount);
        ptr += amount;
        len -= amount;
        return amount;
    }

    constexpr iterator begin() const noexcept { return ptr; }
    constexpr const_iterator cbegin() const noexcept { return ptr; }

    constexpr iterator end() const noexcept { return begin() + size(); }
    constexpr const_iterator cend() const noexcept { return cbegin() + size(); }

    constexpr bool empty() const noexcept { return size() == 0; }
    constexpr pointer data() const noexcept { return ptr; }
    constexpr std::size_t size() const noexcept { return len; }
    constexpr std::size_t size_bytes() const noexcept { return size() * sizeof(value_type); }

    T &operator[](size_t idx) noexcept { return ptr[idx]; }
    const T &operator[](size_t idx) const noexcept { return ptr[idx]; }

  private:
    T* ptr;
    std::size_t len;
  };

  
  template<typename T>
  constexpr span<const typename T::value_type> to_span(const T& src)
  {
    
    return {src.data(), src.size()};
  }

  
  template<typename T>
  constexpr span<typename T::value_type> to_mut_span(T& src)
  {
    
    return {src.data(), src.size()};
  }

  template<typename T>
  constexpr bool has_padding() noexcept
  {
    return !std::is_standard_layout<T>() || alignof(T) != 1;
  }

  
  template<typename T>
  span<const std::uint8_t> to_byte_span(const span<const T> src) noexcept
  {
    static_assert(!has_padding<T>(), "source type may have padding");
    return {reinterpret_cast<const std::uint8_t*>(src.data()), src.size_bytes()};
  }

  
  template<typename T>
  span<const std::uint8_t> as_byte_span(const T& src) noexcept
  {
    static_assert(!std::is_empty<T>(), "empty types will not work -> sizeof == 1");
    static_assert(!has_padding<T>(), "source type may have padding");
    return {reinterpret_cast<const std::uint8_t*>(std::addressof(src)), sizeof(T)};
  }

  
  template<typename T>
  span<std::uint8_t> as_mut_byte_span(T& src) noexcept
  {
    static_assert(!std::is_empty<T>(), "empty types will not work -> sizeof == 1");
    static_assert(!has_padding<T>(), "source type may have padding");
    return {reinterpret_cast<std::uint8_t*>(std::addressof(src)), sizeof(T)};
  }

  
  template<typename T>
  span<const T> strspan(const std::string &s) noexcept
  {
    static_assert(std::is_same<T, char>() || std::is_same<T, unsigned char>() || std::is_same<T, int8_t>() || std::is_same<T, uint8_t>(), "Unexpected type");
    return {reinterpret_cast<const T*>(s.data()), s.size()};
  }
}
