#include <stdio.h>
#include <stdlib.h>
#include <cassert>

[[noreturn]] void Panic(const char* s) {
  puts(s);
  exit(EXIT_FAILURE);
}
#include "rect.h"

int main() {
  Rect zero = {0, 0, 0, 0};
  Rect zero_with_offset = {1, 1, 0, 0};
  Rect one = {0, 0, 1, 1};
  Rect one_with_offset = {1, 1, 1, 1};
  Rect two = {0, 0, 2, 2};
  assert(zero == zero);
  assert(one == one);
  assert(one_with_offset == one_with_offset);
  assert(one != zero);
  assert(one != one_with_offset);
  assert(one.GetIntersectionWith(one_with_offset) == zero_with_offset);
  assert(two.GetIntersectionWith(one) == one);
  assert(two.GetIntersectionWith(one_with_offset) == one_with_offset);
  puts("PASS");
  return 0;
}
