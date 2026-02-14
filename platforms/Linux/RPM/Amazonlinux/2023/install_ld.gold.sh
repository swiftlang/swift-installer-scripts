# This source file is part of the Swift.org open source project
#
# Copyright (c) 2021 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See http://swift.org/LICENSE.txt for license information
# See http://swift.org/CONTRIBUTORS.txt for Swift project authors

#!/usr/bin/env bash

dnf install gmp-devel mpfr-devel texinfo bison git gcc-c++ -y

mkdir ld.gold && cd ld.gold
git clone --depth 1 git://sourceware.org/git/binutils-gdb.git binutils

mkdir build && cd build
../binutils/configure --enable-gold --enable-plugins --disable-werror
make all-gold
cd gold
make all-am
cd ..

cp gold/ld-new /usr/bin/ld.gold
cd ../..

/usr/bin/ld.gold -v