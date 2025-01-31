#pragma once
#include "bitset-reference.h"

#include <iterator>

template <class T>
class b_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = bool;
  using reference = b_reference<T>;
  using pointer = void;
  using word_type = T;

private:
  word_type* word_;
  std::size_t index_;
  static constexpr std::size_t word_size_ = 64;
  template <typename U>
  friend class b_view;
  friend class bitset;
  friend b_iterator<std::remove_const_t<T>>;

  word_type word(const std::size_t index, const b_iterator& end_it) const {
    return (*(word_ + index) << index_) + ((index_ != 0 && (end_it - 1).word_pointer() > word_ + index)
                                               ? *(word_ + index + 1) >> (word_size_ - index_)
                                               : 0);
  }

  word_type* word_pointer() const {
    return word_;
  }

  std::size_t get_index() const {
    return index_;
  }

  void put_word(const std::size_t index, const word_type value, const b_iterator& end_it) {
    *(word_ + index) &= (index_ == 0 ? 0 : ~((static_cast<word_type>(1) << (word_size_ - index_)) - 1));
    *(word_ + index) |= value >> index_;

    if (index_ != 0 && (end_it - 1).word_pointer() > word_ + index) {
      *(word_ + index + 1) &= (static_cast<word_type>(1) << (word_size_ - index_)) - 1;
      *(word_ + index + 1) |= value << (word_size_ - index_);
    }
  }

  b_iterator(word_type* word, const std::size_t index)
      : word_(word)
      , index_(index) {}

public:
  b_iterator& operator=(const b_iterator& other) = default;

  b_iterator() = default;

  b_iterator(const b_iterator& other) = default;

  operator b_iterator<const T>() const {
    return {word_, index_};
  }

  reference operator*() const {
    return {word_, index_};
  }

  b_iterator& operator+=(difference_type ind) {
    auto casted_index = static_cast<difference_type>(index_);
    constexpr auto casted_size = static_cast<difference_type>(word_size_);
    casted_index += ind;
    word_ += casted_index / casted_size - (casted_index >= 0 ? 0 : (-casted_index) % casted_size > 0);
    index_ = ((casted_index % casted_size) + casted_size) % casted_size;
    return *this;
  }

  b_iterator& operator-=(const difference_type ind) {
    operator+=(-ind);
    return *this;
  }

  b_iterator& operator++() {
    index_ += 1;
    if (index_ == word_size_) {
      word_ += 1;
      index_ = 0;
    }
    return *this;
  }

  b_iterator operator++(int) {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  b_iterator& operator--() {
    if (index_ == 0) {
      word_ -= 1;
      index_ = word_size_ - 1;
    } else {
      --index_;
    }
    return *this;
  }

  b_iterator operator--(int) {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  reference operator[](const difference_type ind) const {
    auto tmp = *this;
    tmp += ind;
    return *tmp;
  }

  friend bool operator==(const b_iterator& left, const b_iterator& right) {
    return left.word_ == right.word_ && left.index_ == right.index_;
  }

  friend bool operator!=(const b_iterator& left, const b_iterator& right) {
    return !(left == right);
  }

  friend bool operator<(const b_iterator& left, const b_iterator& right) {
    return left.word_ < right.word_ || (left.word_ == right.word_ && left.index_ < right.index_);
  }

  friend bool operator>(const b_iterator& left, const b_iterator& right) {
    return right < left;
  }

  friend bool operator<=(const b_iterator& left, const b_iterator& right) {
    return !(right < left);
  }

  friend bool operator>=(const b_iterator& left, const b_iterator& right) {
    return !(left < right);
  }

  friend b_iterator operator+(const b_iterator& left, const difference_type ind) {
    auto tmp = left;
    tmp += ind;
    return tmp;
  }

  friend b_iterator operator+(const difference_type ind, const b_iterator& right) {
    return right + ind;
  }

  friend b_iterator operator-(const b_iterator& left, const difference_type ind) {
    auto tmp = left;
    tmp -= ind;
    return tmp;
  }

  friend difference_type operator-(const b_iterator& left, const b_iterator& right) {
    const auto tmp1 = left.word_ - right.word_;
    const difference_type tmp2 = static_cast<difference_type>(left.index_) - static_cast<difference_type>(right.index_);
    return tmp1 * static_cast<difference_type>(word_size_) + tmp2;
  }

  void swap(b_iterator& other) {
    std::swap(word_, other.word_);
    std::swap(index_, other.index_);
  }
};
