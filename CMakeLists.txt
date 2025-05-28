cmake_minimum_required (VERSION 3.8)

# TODO: Make vcpkg relative
if (WIN32)
	include("C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
    include_directories("C:/vcpkg/installed/x64-windows/include/gtkmm-4.0/gtkmm")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# CMake Standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
set_property(TARGET ${PROJECTNAME} PROPERTY CXX_STANDARD 17)
endif()


# Define project
set(PROJECTNAME "Clavis")
set(CLAVIS_VERSION "2.0.1")

project (${PROJECTNAME})

# Find source files
file (GLOB_RECURSE SOURCE_CXX CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable (${PROJECTNAME} ${SOURCE_CXX})
target_include_directories(${PROJECTNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/")

target_compile_definitions(
    ${PROJECTNAME}
    PUBLIC
    CLAVIS_VERSION=\"${CLAVIS_VERSION}\"
	$<$<CONFIG:Debug>:__DEBUG__>
    $<$<CONFIG:RelWithDebInfo>:__RELEASE__>
    $<$<CONFIG:Release>:__RELEASE__>
    $<$<CONFIG:MinSizeRel>:__RELEASE__>
)

set(HEADER_MACROS "${CMAKE_CURRENT_SOURCE_DIR}/src/macros.h")
if(MSVC)
    add_definitions(/FI"${HEADER_MACROS}" /std:c++17)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # GCC or Clang
    add_definitions(-include "${HEADER_MACROS}")
endif()


# GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
target_link_libraries(${PROJECTNAME} PRIVATE PkgConfig::GTKMM)

pkg_check_modules(GPGME REQUIRED IMPORTED_TARGET gpgme)
target_link_libraries(${PROJECTNAME} PRIVATE PkgConfig::GPGME)

# Copy all assets
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/")
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bundled/share" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/")


# Copy all assets
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/")


# Compiler defines for Iffing
if (WIN32)
    add_definitions("-D__WINDOWS__")
elseif(UNIX)
    add_definitions("-D__LINUX__")
elseif(APPLE)
    add_definitions("-D__APPLE__")
endif()
