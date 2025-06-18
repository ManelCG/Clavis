cmake_minimum_required (VERSION 3.8)

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
set(PROJECTNAME_BIN "clavis")
set(CLAVIS_VERSION "2.0.3")
set(VERSION_PATCH "2")

configure_file(
        bundled/assets/clavis.desktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/clavis.desktop
        @ONLY
)

configure_file(
        bundled/assets/PKGBUILD-base.in
        ${CMAKE_CURRENT_SOURCE_DIR}/aur-clavis/PKGBUILD
        @ONLY
)

configure_file(
        bundled/assets/PKGBUILD-git.in
        ${CMAKE_CURRENT_SOURCE_DIR}/aur-clavis-git/PKGBUILD
        @ONLY
)

project (${PROJECTNAME})

# Find source files
file (GLOB_RECURSE SOURCE_CXX CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable (${PROJECTNAME} WIN32 ${SOURCE_CXX})
set_target_properties(${PROJECTNAME} PROPERTIES OUTPUT_NAME ${PROJECTNAME_BIN})

target_include_directories(${PROJECTNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/")

target_compile_definitions(
    ${PROJECTNAME}
    PUBLIC
    CLAVIS_VERSION=\"${CLAVIS_VERSION}\"
    VERSION_PATCH=\"${VERSION_PATCH}\"
    CLAVIS_VERSION_NAME=\"${CLAVIS_VERSION}-${VERSION_PATCH}\"
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

if(WIN32 AND MINGW)
    # Your output dir for the built executable
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>")  # Adjust if needed

    # Directory where GTK and all DLLs live
    set(GTK_DLL_DIR "C:/msys64/mingw64/bin")  # Change to your actual path!

    # Get all DLL files in that directory
    file(GLOB GTK_DLLS "${GTK_DLL_DIR}/*.dll")

    # After build, copy all DLLs to output folder
    add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GTK_DLLS}
            ${OUTPUT_DIR}
    )
endif()

# Copy all assets
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/")

if (WIN32)
    file(GLOB GPG_BINARIES "${CMAKE_CURRENT_SOURCE_DIR}/bundled/gpgbin/*")
    file(COPY ${GPG_BINARIES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
endif()



# Compiler defines for Iffing
if (WIN32)
    add_definitions("-D__WINDOWS__")
elseif(UNIX)
    add_definitions("-D__LINUX__")
elseif(APPLE)
    add_definitions("-D__APPLE__")
endif()

# === INSTALL STEP ===

# Define install paths
set(CLAVIS_SHARE_DIR "/usr/lib/clavis")
set(CLAVIS_ASSETS_DIR "/usr/lib/clavis/assets")

set(CLAVIS_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/clavis.desktop")
set(CLAVIS_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CLAVIS_ICON_FILE_256 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/256px.png")
set(CLAVIS_ICON_FILE_128 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/128px.png")
set(CLAVIS_ICON_FILE_64 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/64px.png")
set(CLAVIS_ICON_FILE_32 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/32px.png")
set(CLAVIS_ICON_FILE_16 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/16px.png")


# Install the executable to /usr/bin/clavis
install(TARGETS ${PROJECTNAME}
        RUNTIME DESTINATION /usr/bin
)

# Install assets (excluding clavis.desktop and icon)
install(DIRECTORY bundled/assets/
        DESTINATION ${CLAVIS_ASSETS_DIR}
        FILES_MATCHING PATTERN "*"
        PATTERN "clavis.desktop" EXCLUDE
        PATTERN "app_icon/256.png" EXCLUDE
)

install(FILES ${CLAVIS_DESKTOP_FILE} DESTINATION /usr/share/applications/)
install(FILES ${CLAVIS_LICENSE_FILE} DESTINATION ${CLAVIS_SHARE_DIR})

install(FILES ${CLAVIS_ICON_FILE_256} DESTINATION /usr/share/pixmaps/ RENAME clavis.png)
install(FILES ${CLAVIS_ICON_FILE_256} DESTINATION /usr/share/icons/hicolor/256x256/apps/ RENAME clavis.png)
install(FILES ${CLAVIS_ICON_FILE_128} DESTINATION /usr/share/icons/hicolor/128x128/apps/ RENAME clavis.png)
install(FILES ${CLAVIS_ICON_FILE_64} DESTINATION /usr/share/icons/hicolor/64x64/apps/ RENAME clavis.png)
install(FILES ${CLAVIS_ICON_FILE_32} DESTINATION /usr/share/icons/hicolor/32x32/apps/ RENAME clavis.png)
install(FILES ${CLAVIS_ICON_FILE_16} DESTINATION /usr/share/icons/hicolor/16x16/apps/ RENAME clavis.png)

# === ARCHLINUX SETUP ===

if(DEFINED ENV{BDIR})
    set(PACKAGE_DIR "$ENV{BDIR}")
else()
    set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/package")
endif()

add_custom_target(archlinux ALL
    COMMENT "Prepare Arch Linux package layout"
)

add_custom_command(TARGET archlinux
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/lib/clavis
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/bin
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/applications
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/pixmaps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/icons/hicolor/256x256/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/icons/hicolor/128x128/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/icons/hicolor/64x64/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/icons/hicolor/32x32/apps
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/usr/share/icons/hicolor/16x16/apps

        # Copy executable
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECTNAME}> ${PACKAGE_DIR}/usr/bin/clavis

        # Copy assets and locale recursively
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/bundled/assets ${PACKAGE_DIR}/usr/lib/clavis/assets

        # Copy license
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_LICENSE_FILE} ${PACKAGE_DIR}/usr/lib/clavis/

        # Copy desktop file
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_DESKTOP_FILE} ${PACKAGE_DIR}/usr/share/applications/

        # Copy pixmaps icon
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_256} ${PACKAGE_DIR}/usr/share/pixmaps/clavis.png

        # Copy icon to hicolor
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_256} ${PACKAGE_DIR}/usr/share/icons/hicolor/256x256/apps/clavis.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_128} ${PACKAGE_DIR}/usr/share/icons/hicolor/128x128/apps/clavis.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_64} ${PACKAGE_DIR}/usr/share/icons/hicolor/64x64/apps/clavis.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_32} ${PACKAGE_DIR}/usr/share/icons/hicolor/32x32/apps/clavis.png
        COMMAND ${CMAKE_COMMAND} -E copy ${CLAVIS_ICON_FILE_16} ${PACKAGE_DIR}/usr/share/icons/hicolor/16x16/apps/clavis.png

)