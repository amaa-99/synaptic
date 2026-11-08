#include "rpackage.h"

#include <gtest/gtest.h>
#include <string>

TEST(RPackageTest, parseDescription)
{
   // Each line of the long description must start with a space, and blank lines should contain a single '.' following the space.

   {  // Empty string.
      const std::string value = "";
      const std::string expected = "";
      EXPECT_EQ(RPackage::parseDescription(value), expected);
   }

   {  // One line, no line feed.
      const std::string value = "1234 5678 90";
      const std::string expected = "";
      EXPECT_EQ(RPackage::parseDescription(value), expected);
   }

   {  // One line, with line feed.
      const std::string value = "1234 5678 90/n";
      const std::string expected = "";
      EXPECT_EQ(RPackage::parseDescription(value), expected);
   }

   {  // One line, with line feed and space.
      const std::string value = "1234 5678 90/n ";
      const std::string expected = "";
      EXPECT_EQ(RPackage::parseDescription(value), expected);
   }

   {  // Description string
      const std::string value =
         "Description: GNU grep, egrep and fgrep.\n"
         " The GNU family of grep utilities may be the \"fastest grep in the west\".\n"
         " .\n"
         " GNU grep is based on a fast lazy-state deterministic matcher\n"
         "   (about twice as fast as stock Unix egrep)\n"
         " hybridized with a Boyer-Moore-Gosper...";

      const std::string expected =
         "The GNU family of grep utilities may be the \"fastest grep in the west\".\n"
         "\n"
         "GNU grep is based on a fast lazy-state deterministic matcher\n"
         "  (about twice as fast as stock Unix egrep)\n"
         "hybridized with a Boyer-Moore-Gosper...";

      EXPECT_EQ(RPackage::parseDescription(value), expected);
   }
}
