#pragma once

template <int hi, int lo, typename TExpr>
constexpr TExpr GetBits(TExpr v) {
  assert(hi >= lo);
  return (v >> lo) & ((1 << (hi - lo + 1)) - 1);
}

template <int bit, typename TExpr>
constexpr bool GetBit(TExpr v) {
  return (v >> bit) & 1;
}

template <int hi, int lo, typename TExpr>
constexpr TExpr CombineFieldBits(TExpr src, TExpr value) {
  assert(hi >= lo);
  constexpr uint32_t kBits = hi - lo + 1;
  constexpr uint32_t kShift = lo;
  constexpr uint32_t kMask = ((1 << kBits) - 1) << kShift;
  src &= ~kMask;
  return src | ((value << kShift) & kMask);
}

template <typename TResult, typename TBase>
TResult RefWithOffset(TBase base, uint64_t ofs) {
  return reinterpret_cast<TResult>(reinterpret_cast<uint64_t>(base) + ofs);
}
