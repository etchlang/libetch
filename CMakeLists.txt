cmake_minimum_required(VERSION 3.6)

project(etch)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
	string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
	if(flag_already_set EQUAL -1)
		check_cxx_compiler_flag("${flag}" flag_supported)
		if(flag_supported)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
		endif()
		unset(flag_supported CACHE)
	endif()
endfunction()

if(WIN32)
else()
	enable_cxx_compiler_flag_if_supported("-Wall")
endif()
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-Werror")

include_directories(include)

add_definitions(
	-D__STDC_CONSTANT_MACROS
	-D__STDC_LIMIT_MACROS
	-D_CRT_SECURE_NO_WARNINGS
	-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
	-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)

set(ETCH_SRCS
	src/etch/codegen.cpp
	src/etch/parser.cpp
	src/etch/parser/expr.cpp
)

if(WIN32)
	set(WIN32_LIBS
		legacy_stdio_definitions.lib
		shlwapi.lib
	)
	set(ETCH_LIBS ${WIN32_LIBS})
endif()

if(UNIX)
	find_library(PTHREAD_LIB pthread)
	set(ETCH_LIBS ${ETCH_LIBS}
		${PTHREAD_LIB}
	)
endif()

# Boost

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(WIN32)
	find_package(Boost COMPONENTS system REQUIRED)
elseif(UNIX)
	find_package(Boost COMPONENTS system thread REQUIRED)
else()
endif()

set(ETCH_INCLUDE_DIRS
	${Boost_INCLUDE_DIRS}
)

set(ETCH_LIBRARY_DIRS
	${Boost_LIBRARY_DIRS}
)

set(ETCH_LIBS ${ETCH_LIBS}
	${Boost_LIBRARIES}
)

# LLVM

find_package(LLVM REQUIRED CONFIG)

set(ETCH_DEFINITIONS
	${LLVM_DEFINITIONS}
)

set(ETCH_INCLUDE_DIRS ${ETCH_INCLUDE_DIRS}
	${LLVM_INCLUDE_DIRS}
)

# macOS kludge

set(ETCH_FRAMEWORKS ${ETCH_LIBS})
list(FILTER ETCH_FRAMEWORKS INCLUDE REGEX "\.framework$")
list(FILTER ETCH_FRAMEWORKS EXCLUDE REGEX "^/System/")

# libetch

add_library(etch STATIC ${ETCH_SRCS})
llvm_config(etch)
target_compile_definitions(etch PRIVATE ${ETCH_DEFINITIONS})
target_include_directories(etch PRIVATE ${ETCH_INCLUDE_DIRS})
target_link_directories(etch PRIVATE ${ETCH_LIBRARY_DIRS})
target_link_libraries(etch ${ETCH_LIBS})
set_property(TARGET etch PROPERTY CXX_STANDARD 17)
set_property(TARGET etch PROPERTY CXX_STANDARD_REQUIRED ON)

set(ENABLE_CLANG_TIDY ON CACHE BOOL "Run clang-tidy on etch")
if(ENABLE_CLANG_TIDY)
	find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
	if(CLANG_TIDY_EXECUTABLE)
		message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
		set(CLANG_TIDY_CHECKS "modernize*,performance*")
		set_property(TARGET etch PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};-checks=${CLANG_TIDY_CHECKS};-header-filter='${CMAKE_CURRENT_SOURCE_DIR}/include/etch'")
	endif()
endif()

set(ETCH_LIBRARY_DIRS ${ETCH_LIBRARY_DIRS}
	${LLVM_LIBRARY_DIRS}
)

set(ETCH_LIBS ${ETCH_LIBS}
	${LLVM_AVAILABLE_LIBS}
)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
	set(ETCH_DEFINITIONS  ${ETCH_DEFINITIONS}  PARENT_SCOPE)
	set(ETCH_INCLUDE_DIRS ${ETCH_INCLUDE_DIRS} PARENT_SCOPE)
	set(ETCH_LIBRARY_DIRS ${ETCH_LIBRARY_DIRS} PARENT_SCOPE)
	set(ETCH_LIBS         ${ETCH_LIBS}         PARENT_SCOPE)
endif()
