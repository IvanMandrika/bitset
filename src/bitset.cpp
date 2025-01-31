#include "bitset.h"

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <ostream>
#include <string>

bitset::bitset(std::size_t size)
    : size_{size}
    , capacity_{capacity_maker()} {
  if (empty()) {
    data_ = nullptr;
    return;
  }
  data_ = new word_type[capacity_ / word_size_]{};
}

std::size_t bitset::capacity_maker() const {
  return size_ / word_size_ * word_size_ + word_size_;
}

bitset::bitset()
    : size_{0}
    , capacity_{0}
    , data_{nullptr} {}

bitset::bitset(word_type* data, const std::size_t size)
    : size_{size}
    , capacity_{capacity_maker()}
    , data_{data} {}

bitset::bitset(const std::string_view str)
    : bitset(str.size()) {
  if (empty()) {
    return;
  }
  auto begin_pointer = str.begin();
  for (std::size_t i = 0; i < (capacity_ / word_size_); ++i) {
    data_[i] = 0;
    for (word_type shift = word_size_; begin_pointer < str.end() && shift > 0; ++begin_pointer, --shift) {
      if (*begin_pointer == '1') {
        data_[i] += static_cast<word_type>(1) << (shift - 1);
      }
    }
  }
}

bitset::bitset(const bitset& other)
    : bitset(other.begin(), other.end()) {}

bitset::bitset(const std::size_t size, const bool value)
    : bitset(size) {
  if (value) {
    set();
  } else {
    reset();
  }
}

bitset::bitset(const_iterator first, const_iterator last)
    : bitset() {
  auto copy = bitset(last - first, false);
  copy |= const_view(first, last);
  swap(copy);
}

bitset::bitset(const const_view& other)
    : bitset(other.begin(), other.end()) {}

bitset& bitset::operator=(const bitset& other) & {
  if (this == &other) {
    return *this;
  }
  bitset copy(other);
  swap(copy);
  return *this;
}

bitset& bitset::operator=(std::string_view str) & {
  bitset copy(str);
  swap(copy);
  return *this;
}

bitset& bitset::operator=(const const_view& other) & {
  bitset copy(other);
  swap(copy);
  return *this;
}

bitset::~bitset() {
  delete[] data_;
}

void bitset::swap(bitset& other) {
  std::swap(data_, other.data_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

std::size_t bitset::size() const {
  return size_;
}

bool bitset::empty() const {
  return size() == 0;
}

bitset::reference bitset::operator[](const std::size_t index) {
  return {data_ + index / word_size_, index % word_size_};
}

bitset::const_reference bitset::operator[](std::size_t index) const {
  return {data_ + index / word_size_, index % word_size_};
}

bitset::iterator bitset::begin() {
  return {data_, 0};
}

bitset::const_iterator bitset::begin() const {
  return {data_, 0};
}

bitset::iterator bitset::end() {
  return {data_ + size() / word_size_, size() % word_size_};
}

bitset::const_iterator bitset::end() const {
  return {data_ + size() / word_size_, size() % word_size_};
}

bitset& bitset::operator&=(const const_view& other) & {
  subview() &= other;
  return *this;
}

bitset& bitset::operator|=(const const_view& other) & {
  subview() |= other;
  return *this;
}

bitset& bitset::operator^=(const const_view& other) & {
  subview() ^= other;
  return *this;
}

bitset& bitset::operator<<=(std::size_t count) & {
  bitset bs(count + size(), false);
  bs.subview(0, size()) |= subview();
  swap(bs);
  return *this;
}

bitset& bitset::operator>>=(std::size_t count) & {
  bitset bs(subview(0, (size() <= count ? 0 : size() - count)));
  swap(bs);
  return *this;
}

bitset& bitset::flip() & {
  subview().flip();
  return *this;
}

bitset& bitset::set() & {
  subview().set();
  return *this;
}

bitset& bitset::reset() & {
  subview().reset();
  return *this;
}

bool bitset::all() const {
  return subview().all();
}

bool bitset::any() const {
  return subview().any();
}

std::size_t bitset::count() const {
  return subview().count();
}

bitset::operator const_view() const {
  return {begin(), end()};
}

bitset::operator view() {
  return {begin(), end()};
}

bitset operator<<(const bitset::const_view& bs, const std::size_t count) {
  bitset copy(bs);
  copy <<= count;
  return copy;
}

bitset operator>>(const bitset::const_view& bs, const std::size_t count) {
  bitset copy(bs);
  copy >>= count;
  return copy;
}

bitset::view bitset::subview(std::size_t offset, std::size_t count) {
  if (offset > size()) {
    return {end(), end()};
  }
  if (count <= size() - offset) {
    return {begin() + offset, begin() + offset + count};
  }
  return {begin() + offset, end()};
}

bitset::const_view bitset::subview(const std::size_t offset, const std::size_t count) const {
  return const_view(begin(), end()).subview(offset, count);
}

bool operator!=(const bitset::const_view& left, const bitset::const_view& right) {
  return !(left == right);
}

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset copy(lhs);
  copy &= rhs;
  return copy;
}

bitset operator|(const bitset::const_view& left, const bitset::const_view& right) {
  bitset copy(left);
  copy |= right;
  return copy;
}

bitset operator^(const bitset::const_view& lhs, const bitset::const_view& rhs) {
  bitset copy(lhs);
  copy ^= rhs;
  return copy;
}

bitset operator~(const bitset::const_view& bs) {
  bitset copy(bs);
  copy.flip();
  return copy;
}

void swap(bitset& lhs, bitset& rhs) {
  lhs.swap(rhs);
}

void swap(bitset::iterator& lhs, bitset::iterator& rhs) {
  lhs.swap(rhs);
}

std::string to_string(const bitset::const_view& bs) {
  std::string result;
  for (const auto bit : bs) {
    result += bit ? '1' : '0';
  }
  return result;
}

std::ostream& operator<<(std::ostream& out, const bitset::const_view& bs) {
  for (auto b : bs) {
    out << b;
  }
  return out;
}
