#!/bin/bash

set -euxo pipefail

# What repo are we cloning?
repo="https://github.com/facebook/folly.git"

# What revision?
rev="2f6126e"

# Where are we cloning to?
dest="folly"

# What's the name of file we're creating?
fname="repro_1444.cpp"

# CFLAGS for compiling out reproducer
declare -a cflags=(
  "-DBOOST_ALL_NO_LIB"
  "-DBOOST_CONTEXT_DYN_LINK"
  "-DBOOST_FILESYSTEM_DYN_LINK"
  "-DBOOST_PROGRAM_OPTIONS_DYN_LINK"
  "-DBOOST_REGEX_DYN_LINK"
  "-DBOOST_SYSTEM_DYN_LINK"
  "-DBOOST_THREAD_DYN_LINK"
  "-DFMT_LOCALE"
  "-DFMT_SHARED"
  "-DFOLLY_XLOG_STRIP_PREFIXES=\"$(pwd)\""
  "-DGFLAGS_IS_A_DLL=0"
  "-DGTEST_LINKED_AS_SHARED_LIBRARY=1"
  "-D_GNU_SOURCE"
  "-D_REENTRANT"
  "-g"
  "-std=gnu++1z"
  "-finput-charset=UTF-8"
  "-fsigned-char"
  "-Wall"
  "-Wno-deprecated"
  "-Wno-deprecated-declarations"
  "-Wno-sign-compare"
  "-Wno-unused"
  "-Wunused-label"
  "-Wunused-result"
  "-Wshadow-compatible-local"
  "-Wno-noexcept-type"
  "-faligned-new"
  "-fopenmp"
  "-pthread"
  "-std=gnu++17")

# Remove the old clone
rm -rf ${dest}

# Clone our repo
git clone ${repo} ${dest}
cd ${dest}
git reset --hard ${rev}
cd ..

# Remove the old reproducer
rm -rf ${fname}

# Add a generic header (used to avoid dependencies on the rest of the folly
# code)
cat > ${fname} << EOF
/* vvvv generic header  vvvv */
#include <cassert>
#include <functional>
#include <stdexcept>
namespace folly {
template <typename T> constexpr T constexpr_max(T b...) { return b; }
template <typename T> constexpr T constexpr_min(T a, T b) {
  return b < a ? b : a;
}
constexpr auto kIsDebug = 1;
template <typename, typename Args> void throw_exception(Args);
template <class Iter> struct Range {
  constexpr Range(Iter start, size_t size) : b_(start), e_(start + size) {}
  template <class Container>
  constexpr Range(Container &container)
      : Range(container.data(), container.size()) {}
  constexpr Iter begin() const { return b_; }
  constexpr Iter end() const { return e_; }
  Iter b_;
  Iter e_;
};
typedef Range<const char *> StringPiece;
enum ordering { lt = -1, eq, gt };
} // namespace folly

#define TEST(x, y) void x ## y (void)
#define EXPECT_STREQ(x, y) 
#define EXPECT_THROW(x, y)
#define EXPECT_EQ(x, y)
#define EXPECT_FALSE(x)
#define EXPECT_TRUE(x)
/* ^^^^ generic header ^^^^ */

EOF

# What are the files we want use from folly?
declare -a merge_files=(
  "folly/CPortability.h"
  "folly/portability/Constexpr.h"
  "folly/FixedString.h"
  "folly/test/FixedStringTest.cpp")

# Merge all of the folly files into our reproducer
for to_merge in "${merge_files[@]}"; do

  {
    # Header block so we know where it came from
    echo -e "\n/* vvvv ${to_merge} vvvv */"

    # We don't want #includes!
    sed "s#^\#include \(.*\)\$#\/\* \#include \1 \*\/#g" "${dest}/${to_merge}"

    # Trailer block so we know where it came from
    echo -e "/* ^^^^ ${to_merge} ^^^^ */\n"

  # Output into merged file
  } >>"${fname}"

done

# Remove the pragmas (not all of our files have pragmas, so we do it once here)
sed -i "s#^\#pragma \(.*\)\$#\/\* \#prama \1 \*\/#g" "${fname}"

# Now we can check if the file compiles
ret=0
gcc -c "${cflags[*]}" "${fname}" || ret=$?

if [[ $ret -eq 0 ]]; then
  echo "ERROR: ${fname} compiled!"
  exit 1
fi

set +x
echo "########################"
echo "# You can upload:      #"
echo "#     ${fname}   #"
echo "# to godbolt           #"
echo "########################"

# EOF
