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