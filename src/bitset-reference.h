#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
template <typename U>
class b_iterator;
template <typename U>
class b_view;

template <class T>
class b_reference {
private:
  T* word_;
  std::size_t index_;
  static constexpr std::size_t word_size_ = 64;
  friend class bitset;
  template <typename U>
  friend class b_iterator;
  template <typename U>
  friend class b_view;
  friend class b_reference<std::remove_const_t<T>>;

  b_reference(T* word, const std::size_t index)
      : word_{word}
      , index_{index} {}

public:
  b_reference() = delete;

  b_reference& operator=(const bool value) {
    if (value) {
      *word_ |= static_cast<T>(1) << (word_size_ - index_ - 1);
    } else {
      *word_ &= ~(static_cast<T>(1) << (word_size_ - index_ - 1));
    }
    return *this;
  }

  friend bool operator==(const b_reference& left, const b_reference& right) {
    return static_cast<bool>(left) == static_cast<bool>(right);
  }

  b_reference& flip()
    requires (!std::is_const_v<T>)
  {
    *word_ ^= static_cast<T>(1) << (word_size_ - index_ - 1);
    return *this;
  }

  operator bool() const {
    return (*word_ & static_cast<T>(1) << (word_size_ - index_ - 1)) != 0;
  }

  operator b_reference<const T>() const {
    return {word_, index_};
  }
};
