cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)
project(emoji-picker VERSION 0.8.0 LANGUAGES CXX)

set(PROJECT_NAME_FULL x11-emoji-picker)
set(PROJECT_ORGANIZATION gazatu.xyz)

cmake_policy(SET CMP0074 NEW)

add_definitions(
  -DPROJECT_ORGANIZATION="${PROJECT_ORGANIZATION}"
  -DPROJECT_NAME="${PROJECT_NAME}"
  -DPROJECT_VERSION="${PROJECT_VERSION}"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

find_package(ICU REQUIRED COMPONENTS uc)

if (UNIX AND NOT APPLE) # LINUX (X11)
  find_library(XDOTOOL_LIB xdo REQUIRED)
  add_library(XDOTOOL STATIC SHARED IMPORTED)
  set_target_properties(XDOTOOL PROPERTIES
    IMPORTED_LOCATION ${XDOTOOL_LIB}
  )
endif ()

include_directories(${PROJECT_SOURCE_DIR}/src)

set(COMMON_SRC_FILES
  src/main.cpp
  src/main.qrc
  src/emojis.cpp
  src/emojis.qrc
  src/EmojiLineEdit.cpp
  src/EmojiLabel.cpp
  src/EmojiPicker.cpp
  src/EmojiPickerSettings.cpp
  src/EmojiTranslator.cpp
)

if (UNIX AND NOT APPLE) # LINUX
  set(PLATFORM_SRC_FILES
  )
elseif (WIN32)
  set(PLATFORM_SRC_FILES
  )
endif ()

set(SRC_FILES
  ${COMMON_SRC_FILES}
  ${PLATFORM_SRC_FILES}
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  WIN32_EXECUTABLE ON # add WinMain(...) if windows
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  ICU::uc
)

if (UNIX AND NOT APPLE) # LINUX (X11)
  target_link_libraries(${PROJECT_NAME}
    XDOTOOL
  )
endif ()

set(STACK_SIZE_DEC 4194304) # 4MB
set(STACK_SIZE_HEX 0x400000) # 4MB

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # clang++ / clang-cl
  if (WIN32) # lld-link
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /STACK:${STACK_SIZE_DEC})
  elseif (APPLE) # apple ld
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-stack_size,${STACK_SIZE_HEX})
  endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # g++ / mingw g++
  if (WIN32) # mingw ld
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,--stack,${STACK_SIZE_DEC})
  elseif (APPLE) # apple ld
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-stack_size,${STACK_SIZE_HEX})
  endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # msvc
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /STACK:${STACK_SIZE_DEC})
endif()

if (UNIX AND NOT APPLE) # LINUX
  set(CMAKE_INSTALL_PREFIX /usr)

  install(TARGETS ${PROJECT_NAME})
  install(FILES ${PROJECT_SOURCE_DIR}/res/${PROJECT_NAME_FULL}.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
  install(FILES ${PROJECT_SOURCE_DIR}/res/${PROJECT_NAME_FULL}.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/72x72/apps)

  add_custom_target(appimage
    COMMAND make install DESTDIR=_AppDir
    COMMAND VERSION=${PROJECT_VERSION} linuxdeployqt.AppImage _AppDir/usr/share/applications/${PROJECT_NAME_FULL}.desktop -appimage -no-translations
  )

  set(CPACK_PACKAGE_NAME ${PROJECT_NAME_FULL})
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  set(CPACK_PACKAGE_VENDOR ${PROJECT_ORGANIZATION})

  set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CMAKE_SYSTEM_PROCESSOR})
  set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

  find_program(LSB_RELEASE_EXEC lsb_release)
  if (LSB_RELEASE_EXEC)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
      OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  endif ()

  if (LSB_RELEASE_ID_SHORT MATCHES "openSUSE")
    set(CPACK_GENERATOR "RPM")
  elseif (LSB_RELEASE_ID_SHORT MATCHES "Debian")
    set(CPACK_GENERATOR "DEB")
  endif ()

  set(CPACK_RPM_PACKAGE_LICENSE "MIT")
  set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
    ${CMAKE_INSTALL_PREFIX}/share
    ${CMAKE_INSTALL_PREFIX}/share/applications
    ${CMAKE_INSTALL_PREFIX}/share/icons
    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor
    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/72x72
    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/72x72/apps
  )
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${PROJECT_ORGANIZATION})
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

  include(CPack)
endif ()
