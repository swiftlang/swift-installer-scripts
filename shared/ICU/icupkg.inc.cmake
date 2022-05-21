#[[
This source file is part of the Swift.org open source project

Copyright (c) 2019 Saleem Abdulrasool.
Copyright (c) 2022 Apple Inc. and the Swift project authors
Licensed under Apache License v2.0 with Runtime Library Exception

See https://swift.org/LICENSE.txt for license information
See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
#]]

GENCCODE_ASSEMBLY_TYPE=-a gcc
SO=so
SOBJ=so
A=a
LIBPREFIX=@CMAKE_SHARED_LIBRARY_PREFIX@
LIB_EXT_ORDER=.@PROJECT_VERSION_MAJOR@
COMPILE="@CMAKE_COMMAND@" -E echo COMPILE
LIBFLAGS=
GENLIB="@CMAKE_COMMAND@" -E echo GENLIB
LDICUDTFLAGS=-nodefaultlibs -nostdlibs
AR=@CMAKE_AR@
ARFLAGS=
RANLIB=@CMAKE_RANLIB@
INSTALL_CMD="@CMAKE_COMMAND@" -E echo INSTALL
