cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# CMake Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECTNAME} PROPERTY CXX_STANDARD 17)
endif()

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
else()
    set(LINUX FALSE)
endif()


# Define project
set(PROJECTNAME "Clavis")
set(PROJECTNAME_BIN "clavis")
set(CLAVIS_VERSION "2.1.0")
set(VERSION_PATCH "2")

# Version depending files
if (LINUX)
    configure_file(bundled/assets/clavis.desktop.in     ${CMAKE_CURRENT_BINARY_DIR}/clavis.desktop              @ONLY)
    configure_file(bundled/assets/PKGBUILD-base.in      ${CMAKE_CURRENT_SOURCE_DIR}/aur-clavis/PKGBUILD         @ONLY)
    configure_file(bundled/assets/PKGBUILD-git.in       ${CMAKE_CURRENT_SOURCE_DIR}/aur-clavis-git/PKGBUILD     @ONLY)
endif()

if (APPLE)
    configure_file(bundled/assets/Info.plist.in         ${CMAKE_CURRENT_SOURCE_DIR}/macos/Info.plist            @ONLY)
endif()

project (${PROJECTNAME})

# Find source files
file (GLOB_RECURSE SOURCE_CXX CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable (${PROJECTNAME} WIN32 ${SOURCE_CXX})
set_target_properties(${PROJECTNAME} PROPERTIES OUTPUT_NAME ${PROJECTNAME_BIN})
target_include_directories(${PROJECTNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include/")

set(HEADER_MACROS "${CMAKE_CURRENT_SOURCE_DIR}/src/macros.h")
if(MSVC)
    add_definitions(/FI"${HEADER_MACROS}" /std:c++17)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()  # GCC or Clang
    add_compile_options(-std=c++17)
    add_definitions(-include "${HEADER_MACROS}")
endif()

if (APPLE)
  set_target_properties(${PROJECTNAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/macos/Info.plist
  )
endif()

    # GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
target_link_libraries(${PROJECTNAME} PRIVATE PkgConfig::GTKMM)

# GPGME
pkg_check_modules(GPGME REQUIRED IMPORTED_TARGET gpgme)
target_link_libraries(${PROJECTNAME} PRIVATE PkgConfig::GPGME)

include(CheckFunctionExists)
set(CMAKE_REQUIRED_INCLUDES ${GPGME_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${GPGME_LIBRARIES})
check_function_exists(gpgme_op_setownertrust HAVE_GPGME_OP_SETOWNERTRUST)


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
    $<$<BOOL:${HAVE_GPGME_OP_SETOWNERTRUST}>:HAVE_GPGME_OP_SETOWNERTRUST>
)

# Compiler defines for Iffing
if (WIN32)
    add_definitions("-D__WINDOWS__")
elseif(APPLE)
    add_definitions("-D__MACOS__")
elseif(LINUX)
    add_definitions("-D__LINUX__")
endif()



if(WIN32 AND MINGW)
    set(MSYS2_BIN "C:/msys64/mingw64/bin")

    # Copy all runtime DLLs (GTK, GPGME, MinGW CRT, etc.) from the active MSYS2 installation.
    file(GLOB ALL_MSYS2_DLLS "${MSYS2_BIN}/*.dll")
    add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ALL_MSYS2_DLLS}
            $<TARGET_FILE_DIR:${PROJECTNAME}>
    )

    # Copy GPG executables from the SAME MSYS2 installation so they share the same DLL
    # versions as the DLLs above. Requires mingw-w64-x86_64-gnupg to be installed.
    set(GPG_EXE_NAMES
        gpg.exe gpg-agent.exe gpgconf.exe gpgsm.exe gpgtar.exe gpgv.exe
        gpg-connect-agent.exe dirmngr.exe dirmngr_ldap.exe pinentry-basic.exe
        gpgme-w32spawn.exe gpg-card.exe gpg-check-pattern.exe gpg-preset-passphrase.exe
        gpg-wks-client.exe keyboxd.exe scdaemon.exe
    )
    set(GPG_EXES_TO_COPY "")
    foreach(exe_name ${GPG_EXE_NAMES})
        if(EXISTS "${MSYS2_BIN}/${exe_name}")
            list(APPEND GPG_EXES_TO_COPY "${MSYS2_BIN}/${exe_name}")
        endif()
    endforeach()
    if(GPG_EXES_TO_COPY)
        add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GPG_EXES_TO_COPY}
                $<TARGET_FILE_DIR:${PROJECTNAME}>
        )
    endif()

    # Copy GPG batch helper scripts
    file(GLOB GPG_BATS "${MSYS2_BIN}/*.bat")
    if(GPG_BATS)
        add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GPG_BATS}
                $<TARGET_FILE_DIR:${PROJECTNAME}>
        )
    endif()
endif()

# Copy all assets
if (WIN32 OR LINUX)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/")
endif()
# bundled/gpgbin is intentionally NOT copied here: the executables there may be built
# against different DLL versions than the current MSYS2 toolchain and will fail to load.



# === INSTALL STEP ===

set(CLAVIS_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/clavis.desktop")
set(CLAVIS_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CLAVIS_ICON_FILE_256 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/256px.png")
set(CLAVIS_ICON_FILE_128 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/128px.png")
set(CLAVIS_ICON_FILE_64 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/64px.png")
set(CLAVIS_ICON_FILE_32 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/32px.png")
set(CLAVIS_ICON_FILE_16 "${CMAKE_CURRENT_SOURCE_DIR}/bundled/assets/icons/Logos/16px.png")

if(LINUX)
  # Define install paths
  set(CLAVIS_SHARE_DIR "/usr/lib/clavis")
  set(CLAVIS_ASSETS_DIR "/usr/lib/clavis/assets")

  # Install the executable to /usr/bin/clavis
  install(TARGETS ${PROJECTNAME}
          RUNTIME DESTINATION /usr/bin
  )

  install(FILES ${CLAVIS_DESKTOP_FILE} DESTINATION /usr/share/applications/)

  install(FILES ${CLAVIS_ICON_FILE_256} DESTINATION /usr/share/pixmaps/ RENAME clavis.png)
  install(FILES ${CLAVIS_ICON_FILE_256} DESTINATION /usr/share/icons/hicolor/256x256/apps/ RENAME clavis.png)
  install(FILES ${CLAVIS_ICON_FILE_128} DESTINATION /usr/share/icons/hicolor/128x128/apps/ RENAME clavis.png)
  install(FILES ${CLAVIS_ICON_FILE_64} DESTINATION /usr/share/icons/hicolor/64x64/apps/ RENAME clavis.png)
  install(FILES ${CLAVIS_ICON_FILE_32} DESTINATION /usr/share/icons/hicolor/32x32/apps/ RENAME clavis.png)
  install(FILES ${CLAVIS_ICON_FILE_16} DESTINATION /usr/share/icons/hicolor/16x16/apps/ RENAME clavis.png)

  install(DIRECTORY bundled/assets/
          DESTINATION ${CLAVIS_ASSETS_DIR}
          FILES_MATCHING PATTERN "*"
          PATTERN "clavis.desktop" EXCLUDE
          PATTERN "app_icon/256.png" EXCLUDE
  )

  install(FILES ${CLAVIS_LICENSE_FILE} DESTINATION ${CLAVIS_SHARE_DIR})
elseif(APPLE)
  #Define install paths
  set(BUNDLE_PATH "${CMAKE_BINARY_DIR}/clavis.app")

  set(CLAVIS_RESOURCES_DIR "${BUNDLE_PATH}/Contents/Resources")
  set(CLAVIS_ASSETS_DIR "${CLAVIS_RESOURCES_DIR}/assets")

  set(CLAVIS_ICNS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/macos/clavis.icns")

  add_custom_command(TARGET ${PROJECTNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/bundled/assets"
      "${CLAVIS_ASSETS_DIR}"

    COMMAND ${CMAKE_COMMAND} -E copy
      "${CLAVIS_LICENSE_FILE}"
      "${CLAVIS_RESOURCES_DIR}/LICENSE"

      COMMAND ${CMAKE_COMMAND} -E copy
        "${CLAVIS_ICNS_FILE}"
        "${CLAVIS_RESOURCES_DIR}/clavis.icns"
  )

endif()


# === ARCHLINUX SETUP ===

if(DEFINED ENV{BDIR})
    set(PACKAGE_DIR "$ENV{BDIR}")
else()
    set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/package")
endif()

add_custom_target(archlinux
    COMMENT "Prepare Arch Linux package layout"

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
