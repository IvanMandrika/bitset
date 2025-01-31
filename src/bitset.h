#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"
#include "bitset-view.h"

#include <cstdint>
#include <string>
#include <string_view>

class bitset {
public:
  using word_type = uint64_t;
  using value_type = bool;
  using reference = b_reference<word_type>;
  using const_reference = b_reference<const word_type>;
  using iterator = b_iterator<uint64_t>;
  using const_iterator = b_iterator<const word_type>;
  using const_view = b_view<const word_type>;
  using view = b_view<word_type>;
  static constexpr std::size_t npos = -1;

private:
  static constexpr std::size_t word_size_ = 64;
  std::size_t size_;
  std::size_t capacity_;
  word_type* data_;

  std::size_t capacity_maker() const;

  bitset(word_type* data, std::size_t size);

  explicit bitset(std::size_t size);

public:
  bitset();

  explicit bitset(std::string_view str);

  bitset(const bitset& other);

  bitset(std::size_t size, bool value);

  bitset(const_iterator first, const_iterator last);

  explicit bitset(const const_view& other);

  bitset& operator=(const bitset& other) &;

  bitset& operator=(std::string_view str) &;

  bitset& operator=(const const_view& other) &;

  ~bitset();

  void swap(bitset& other);

  std::size_t size() const;

  bool empty() const;

  reference operator[](std::size_t index);

  const_reference operator[](std::size_t index) const;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  bitset& operator&=(const const_view& other) &;

  bitset& operator|=(const const_view& other) &;

  bitset& operator^=(const const_view& other) &;

  bitset& operator<<=(std::size_t count) &;

  bitset& operator>>=(std::size_t count) &;

  bitset& flip() &;

  bitset& set() &;

  bitset& reset() &;

  bool all() const;

  bool any() const;

  std::size_t count() const;

  operator const_view() const;

  operator view();

  view subview(std::size_t offset = 0, std::size_t count = npos);

  const_view subview(std::size_t offset = 0, std::size_t count = npos) const;
};

bool operator==(const bitset::const_view& left, const bitset::const_view& right);
bool operator!=(const bitset::const_view& left, const bitset::const_view& right);

void swap(bitset& lhs, bitset& rhs);
void swap(bitset::view& lhs, bitset::view& rhs);
void swap(bitset::iterator& lhs, bitset::iterator& rhs);

bitset operator&(const bitset::const_view& lhs, const bitset::const_view& rhs);

bitset operator|(const bitset::const_view& left, const bitset::const_view& right);

bitset operator^(const bitset::const_view& left, const bitset::const_view& right);

bitset operator~(const bitset::const_view& right);

bitset operator<<(const bitset::const_view& bs, std::size_t count);

bitset operator>>(const bitset::const_view& bs, std::size_t count);

std::string to_string(const bitset::const_view& bs);

std::ostream& operator<<(std::ostream& out, const bitset::const_view& bs);
