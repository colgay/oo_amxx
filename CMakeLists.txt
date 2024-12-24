cmake_minimum_required(VERSION 3.13)
project(oo VERSION 1.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    public
    public/sdk
)

add_library(oo SHARED
    public/sdk/amxxmodule.cpp
    oo_amxx.cpp 
    oo_asm.cpp 
    oo_call.cpp 
    oo_forward.cpp 
    oo_manager.cpp 
    oo_native.cpp
)

set_target_properties(oo PROPERTIES PREFIX "")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(oo PRIVATE DEBUG _DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(oo PRIVATE NDEBUG)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(oo PRIVATE
        _CRT_SECURE_NO_DEPRECATE
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _ITERATOR_DEBUG_LEVEL=0
        HAVE_STDINT_H
        WIN32
        _WINDOWS
    )
    
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(oo PRIVATE /MTd /Od /RTC1)
        target_link_options(oo PRIVATE /NODEFAULTLIB:libcmt)

    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(oo PRIVATE /MT /Ox)
        target_link_options(oo PRIVATE /OPT:ICF /OPT:REF)
    endif ()

    target_compile_options(oo PRIVATE /W3 /EHsc /GR- /TP /Oy-)

    target_link_libraries(oo PRIVATE
        kernel32.lib
        user32.lib
        gdi32.lib
        winspool.lib
        comdlg32.lib
        advapi32.lib
        shell32.lib
        ole32.lib
        oleaut32.lib
        uuid.lib
        odbc32.lib
        odbccp32.lib
    )

    target_link_options(oo PRIVATE /MACHINE:X86 /SUBSYSTEM:WINDOWS)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.00)
        target_link_libraries(oo PRIVATE legacy_stdio_definitions.lib legacy_stdio_wide_specifiers.lib)
    endif()

    set_target_properties(oo PROPERTIES OUTPUT_NAME oo_amxx)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(oo PRIVATE _LINUX POSIX LINUX)

    target_compile_options(oo PRIVATE
        -pipe
        -fPIC
        -fno-strict-aliasing
        -Wall
        -Werror
        -Wno-uninitialized
        -Wno-unused
        -Wno-switch
        -Wno-format
        -Wno-format-security
        -m32
        -Wno-invalid-offsetof
        -Wno-parentheses
        $<$<COMPILE_LANGUAGE:C>:-std=c99>
        -fno-exceptions
        -fno-rtti
    )

    target_link_options(oo PRIVATE -m32 -ldl -lm -static-libgcc)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.0)
        target_compile_options(oo PRIVATE
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},4.0>:-fvisibility=hidden -fvisibility-inlines-hidden>
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},4.6>:-Wno-narrowing>
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},4.7>:-Wno-delete-non-virtual-dtor>
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},4.8>:-Wno-unused-result -Wno-error=sign-compare>
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},8.0>:-Wno-stringop-truncation>
	    $<$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},9.0>:-Wno-address-of-packed-member>
        )
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(oo PRIVATE /MTd /Od /RTC1)
        target_link_options(oo PRIVATE /NODEFAULTLIB:libcmt)

    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(oo PRIVATE -O2)
        target_link_options(oo PRIVATE /OPT:ICF /OPT:REF)
    endif ()

  
    # Function to find the path to libstdc++.a using the compiler
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -m32 -print-file-name=libstdc++.a
        OUTPUT_VARIABLE LIBSTDCXX_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    target_link_libraries(oo PRIVATE ${LIBSTDCXX_PATH})
    message(STATUS "Added libstdc++.a dependency for Linux")

    set_target_properties(oo PROPERTIES OUTPUT_NAME oo_amxx_i386)
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

target_compile_definitions(oo PRIVATE PAWN_CELL_SIZE=32)