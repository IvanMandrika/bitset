#pragma once

#include "bitset-iterator.h"

#include <algorithm>
#include <functional>
#include <limits>

class bitset;

template <class T>
class b_view {
public:
  using value_type = bool;
  using reference = b_reference<T>;
  using const_reference = b_reference<const T>;
  using iterator = b_iterator<T>;
  using const_iterator = b_iterator<const T>;
  using const_view = b_view<const T>;
  using view = b_view<T>;
  using word_type = T;
  static constexpr std::size_t npos = -1;

private:
  b_iterator<T> begin_;
  b_iterator<T> end_;
  static constexpr std::size_t word_size_ = 64;

  friend class bitset;
  friend b_view<std::remove_const<T>>;

  template <typename Func>
  b_view operator_helper(const const_view& other, const Func& operation) const {
    const b_iterator it = other.begin();
    for (std::size_t i = 0; i < size() / word_size_; ++i) {
      begin().put_word(i, operation(begin().word(i, end()), it.word(i, other.end())), end());
    }
    if (size() % word_size_ != 0) {
      auto tmp = begin().word(size() / word_size_, end()) &
                 ((static_cast<uint64_t>(1) << (word_size_ - size() % word_size_)) - 1);
      const uint64_t put_val =
          (operation(begin().word(size() / word_size_, end()), it.word(size() / word_size_, other.end())) &
           ~((static_cast<uint64_t>(1) << (word_size_ - size() % word_size_)) - 1)) |
          tmp;
      begin().put_word(size() / word_size_, put_val, end());
    }
    return *this;
  }

  template <typename Func>
  b_view unary_operator_helper(const Func& operation) const {
    for (std::size_t i = 0; i < size() / word_size_; ++i) {
      begin().put_word(i, operation(begin().word(i, end())), end());
    }

    if (size() % word_size_ != 0) {
      auto tmp = begin().word(size() / word_size_, end()) &
                 ((static_cast<uint64_t>(1) << (word_size_ - size() % word_size_)) - 1);
      const uint64_t put_val = (operation(begin().word(size() / word_size_, end())) &
                                ~((static_cast<uint64_t>(1) << (word_size_ - size() % word_size_)) - 1)) |
                               tmp;
      begin().put_word(size() / word_size_, put_val, end());
    }
    return *this;
  }

  word_type take_word(const std::size_t index) const {
    return begin().word(index, end());
  }

  void put_word(const std::size_t index, const uint64_t value) {
    begin().put_word(index, value);
  }

public:
  b_view(const iterator begin, const iterator end)
      : begin_{begin}
      , end_{end} {}

  b_view(const b_view& other) = default;

  b_view() = default;

  operator const_view() const {
    return const_view(begin(), end());
  }

  friend bool operator==(const b_view& left, const b_view& right) {
    if (left.size() != right.size()) {
      return false;
    }
    for (std::size_t i = 0; i < left.size() / word_size_; ++i) {
      if (left.take_word(i) != right.take_word(i)) {
        return false;
      }
    }
    return left.size() % word_size_ == 0 ||
           (left.take_word(left.size() / word_size_) >> (word_size_ - left.size() % word_size_) ==
            right.take_word(left.size() / word_size_) >> (word_size_ - left.size() % word_size_));
  }

  iterator begin() const {
    return begin_;
  }

  iterator end() const {
    return end_;
  }

  std::size_t size() const {
    return end_ - begin_;
  }

  b_view& operator=(const b_view& other) = default;

  void swap(view& other) {
    std::swap(begin_, other.begin_);
    std::swap(end_, other.end_);
  }

  friend void swap(b_view& lhs, b_view& rhs) {
    lhs.swap(rhs);
  }

  bool empty() const {
    return size() == 0;
  }

  reference operator[](std::size_t index) const {
    return {(begin() + index).word_pointer(), (begin() + index).get_index()};
  }

  b_view operator&=(const const_view& other) const {
    return operator_helper(other, std::bit_and());
  }

  b_view operator|=(const const_view& other) const {
    return operator_helper(other, std::bit_or());
  }

  b_view operator^=(const const_view& other) const {
    return operator_helper(other, std::bit_xor());
  }

  b_view flip() const {
    return unary_operator_helper([](const word_type word) { return ~word; });
  }

  b_view set() const {
    return unary_operator_helper([](const word_type) { return std::numeric_limits<uint64_t>::max(); });
  }

  b_view reset() const {
    return unary_operator_helper([](const word_type) { return static_cast<word_type>(0); });
  }

  bool all() const {
    return std::all_of(begin(), end(), std::identity{});
  }

  bool any() const {
    return std::any_of(begin(), end(), std::identity{});
  }

  std::size_t count() const {
    return std::count(begin(), end(), true);
  }

  b_view subview(std::size_t offset = 0, std::size_t count = npos) const {
    if (offset > size()) {
      return {end(), end()};
    }
    if (count <= size() - offset) {
      return {begin() + offset, begin() + offset + count};
    }
    return {begin() + offset, end()};
  }
};
