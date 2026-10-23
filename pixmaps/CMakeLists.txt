cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

####################
# Install the application specific icons

set(hicolor_16x16_files
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-available-locked.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-available.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-broken.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-downgrade.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-install.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-installed-locked.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-installed-outdated.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-installed-updated.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-new.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-purge.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-reinstall.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-remove.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-upgrade.png
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/system-upgrade.png
)

install(FILES ${hicolor_16x16_files} DESTINATION "${PACKAGE_DATA_DIR}/icons/hicolor/16x16/actions")

####################
# Distro specific package-supported icon

cmake_host_system_information(RESULT DISTRIB_ID QUERY DISTRIB_ID)
message(STATUS "Linux distro: ${DISTRIB_ID}")

string(TOLOWER "${DISTRIB_ID}" DISTRIB_ID)

set(IMAGE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-supported.png.${DISTRIB_ID}")
if(NOT EXISTS "${IMAGE_FILE}")
  set(IMAGE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/hicolor/16x16/actions/package-supported.png")
endif()

install(FILES "${IMAGE_FILE}" DESTINATION "${PACKAGE_DATA_DIR}/icons/hicolor/16x16/actions" RENAME package-supported.png)

####################

set(hicolor_24x24_files
  ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/24x24/actions/system-upgrade.png
)

install(FILES ${hicolor_24x24_files} DESTINATION "${PACKAGE_DATA_DIR}/icons/hicolor/24x24/actions")

####################
# Install the application icons (referenced by the desktop files)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/256x256/apps/synaptic.png DESTINATION "${PACKAGE_DATA_DIR}/icons/hicolor/256x256/apps")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/hicolor/scalable/apps/synaptic.svg DESTINATION "${PACKAGE_DATA_DIR}/icons/hicolor/scalable/apps")

####################
