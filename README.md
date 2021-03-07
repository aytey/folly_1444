## Reproducer for Folly #1444

This repo acts as a reproducer for [Folly](https://github.com/facebook/folly) [Issue #1444](https://github.com/facebook/folly/issues/1444).

The error looks like this:

```bash
pre_generated_1444.cpp: In function ‘void FixedStringEraseTestCEraseTest()’:
pre_generated_1444.cpp:3765:42:   in ‘constexpr’ expansion of ‘x.folly::BasicFixedString<char, 26>::cerase(x.folly::BasicFixedString<char, 26>::size(), ((std::size_t)folly::detail::fixedstring::FixedStringBase_<>::npos))’
pre_generated_1444.cpp:1790:20:   in ‘constexpr’ expansion of ‘((const folly::BasicFixedString<char, 26>*)this)->folly::BasicFixedString<char, 26>::creplace<1>(pos, folly::detail::fixedstring::checkOverflowOrNpos(count, (((long unsigned int)((const folly::BasicFixedString<char, 26>*)this)->folly::BasicFixedString<char, 26>::size_) - folly::detail::fixedstring::checkOverflow(pos, ((std::size_t)((const folly::BasicFixedString<char, 26>*)this)->folly::BasicFixedString<char, 26>::size_)))), A{'\000'})’
pre_generated_1444.cpp:2212:20:   in ‘constexpr’ expansion of ‘((const folly::BasicFixedString<char, 26>*)this)->folly::BasicFixedString<char, 26>::creplace<1>(this_pos, this_count, (* & that), 0, (1 - 1))’
pre_generated_1444.cpp:2252:49:   in ‘constexpr’ expansion of ‘folly::detail::fixedstring::checkNullTerminated<char, 1>((* & that))’
pre_generated_1444.cpp:493:61:   in ‘constexpr’ expansion of ‘folly::constexpr_strlen<char>(((const char*)a))’
pre_generated_1444.cpp:382:43:   in ‘constexpr’ expansion of ‘folly::detail::constexpr_strlen_internal<char>(s, 0)’
pre_generated_1444.cpp:329:29: error: ‘__builtin_strlen(((const char*)(&<anonymous>)))’ is not a constant expression
  329 |   return FOLLY_DETAIL_STRLEN(s);
      |                             ^
pre_generated_1444.cpp:3766:21: error: non-constant condition for static assertion
 3766 |   static_assert(26u == tmp0.size(), "");
      |                 ~~~~^~~~~~~~~~~~~~
pre_generated_1444.cpp:3767:19: error: non-constant condition for static assertion
 3767 |   static_assert(y == tmp0, "");
      |                 ~~^~~~~~~
pre_generated_1444.cpp:3769:21: error: non-constant condition for static assertion
 3769 |   static_assert(24u == tmp1.size(), "");
      |                 ~~~~^~~~~~~~~~~~~~
pre_generated_1444.cpp:3770:44: error: non-constant condition for static assertion
 3770 |   static_assert("abcdefghijklmnopqrstuvwx" == tmp1, "");
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~
pre_generated_1444.cpp:3772:20: error: non-constant condition for static assertion
 3772 |   static_assert(2u == tmp2.size(), "");
      |                 ~~~^~~~~~~~~~~~~~
pre_generated_1444.cpp:3773:22: error: non-constant condition for static assertion
 3773 |   static_assert("ax" == tmp2, "");
      |                 ~~~~~^~~~~~~
```

You can re-generate a reproducer by running the script: `build_reproducer.sh`. This script merges the files:

* `folly/CPortability.h`
* `folly/portability/Constexpr.h`
* `folly/FixedString.h`
* `folly/test/FixedStringTest.cpp`

into a single file and adds an additional "header block" to ensure that the file compiles.

If you do not wish to run `build_reproducer.sh`, a pre-build version exists in this repo: `pre_generated_1444.cpp`.

This has been verified on:

```bash
$ lsb-release -a
LSB Version:	n/a
Distributor ID:	openSUSE
Description:	openSUSE Tumbleweed
Release:	20210223
Codename:	n/a

$ ldd --version ldd
ldd (GNU libc) 2.33
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Written by Roland McGrath and Ulrich Drepper.

$ g++ --version
g++ (SUSE Linux) 10.2.1 20201202 [revision e563687cf9d3d1278f45aaebd03e0f66531076c9]
Copyright (C) 2020 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

