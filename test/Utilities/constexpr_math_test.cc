#include "lardataalg/Utilities/constexpr_math.h"

int main()
{
  static_assert(util::abs(-0.0) == 0.0);
  static_assert(util::abs(-2.0) == 2.0);
  static_assert(util::abs(-2.0F) == 2.0F);
  static_assert(util::abs(-2) == 2);
  static_assert(util::abs(-2L) == 2L);
  static_assert(util::abs(-2LL) == 2LL);
}
