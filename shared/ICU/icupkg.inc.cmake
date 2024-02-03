#[[
This source file is part of the Swift.org open source project

Copyright (c) 2019 Saleem Abdulrasool.
Copyright (c) 2022 Apple Inc. and the Swift project authors
Licensed under Apache License v2.0 with Runtime Library Exception

See https://swift.org/LICENSE.txt for license information
See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
#]]

if (WIN32)
    set(GENCCODE_ASSEMBLY_TYPE -a gcc)
    set(SO dll)
    set(SOBJ dll)
    set(A lib)
    set(LIBPREFIX @CMAKE_STATIC_LIBRARY_PREFIX@)
    set(LIB_EXT_ORDER .@PROJECT_VERSION_MAJOR@)
    set(LIBFLAGS)
    set(GENLIB "@CMAKE_COMMAND@" -E echo GENLIB)
    set(LDICUDTFLAGS)
    set(AR @CMAKE_AR@)
    set(ARFLAGS)
    set(RANLIB)
    set(INSTALL_CMD "@CMAKE_COMMAND@" -E echo INSTALL)
else()
    set(GENCCODE_ASSEMBLY_TYPE -a gcc)
    set(SO so)
    set(SOBJ so)
    set(A a)
    set(LIBPREFIX @CMAKE_SHARED_LIBRARY_PREFIX@)
    set(LIB_EXT_ORDER .@PROJECT_VERSION_MAJOR@)
    set(COMPILE "@CMAKE_COMMAND@" -E echo COMPILE)
    set(LIBFLAGS)
    set(GENLIB "@CMAKE_COMMAND@" -E echo GENLIB)
    set(LDICUDTFLAGS -nodefaultlibs -nostdlibs)
    set(AR @CMAKE_AR@)
    set(ARFLAGS)
    set(RANLIB @CMAKE_RANLIB@)
    set(INSTALL_CMD "@CMAKE_COMMAND@" -E echo INSTALL)
endif()
