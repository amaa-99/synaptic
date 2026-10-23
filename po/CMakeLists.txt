cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

####################
# Compile the .po localization files

set(LANGUAGES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LINGUAS")

file(STRINGS ${LANGUAGES_FILE} languages)

foreach(language IN LISTS languages)
  gettext_process_po_files(${language} INSTALL_DESTINATION ${CMAKE_INSTALL_LOCALEDIR} PO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po)
endforeach()

# Note: gettext_process_po_files() creates a global custom target named pofiles.
# (https://cmake.org/cmake/help/latest/module/FindGettext.html#command:gettext_process_po_files)

add_custom_target(build_po ALL DEPENDS ${LANGUAGES_FILE} pofiles COMMENT "Compiling the .po localization files")
