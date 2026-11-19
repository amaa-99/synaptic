#include "rgutils.h"

#include <gtest/gtest.h>
#include <string_view>

using namespace testing;
using namespace std;

TEST(RGUtilsTest, formatBytes)
{
   {
      constexpr int64_t value = 0;
      constexpr string_view expected = "0 B";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 1234;
      constexpr string_view expected = "1234 B";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 12345;
      constexpr string_view expected = "12.3 kB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 12349;
      constexpr string_view expected = "12.3 kB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 12350;
      constexpr string_view expected = "12.4 kB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = -123456;
      constexpr string_view expected = "123 kB";
      EXPECT_EQ(RGSizeToStr(-value), expected);
   }

   {
      constexpr int64_t value = -1234567;
      constexpr string_view expected = "1.23 MB";
      EXPECT_EQ(RGSizeToStr(-value), expected);
   }

   {
      constexpr int64_t value = 1024LL * 1024 * 1024 * 1024 + 2048;
      constexpr string_view expected = "1.10 TB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 1024LL * 1024 * 1024 * 1024 * 1024 + 2048;
      constexpr string_view expected = "1.13 PB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }

   {
      constexpr int64_t value = 1024LL * 1024 * 1024 * 1024 * 1024 * 1024 + 2048;
      constexpr string_view expected = "1.15 EB";
      EXPECT_EQ(RGSizeToStr(value), expected);
   }
}
