cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

####################
# Compile the .po localization files

set(LANGUAGES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LINGUAS")

file(STRINGS ${LANGUAGES_FILE} languages)

foreach(language IN LISTS languages)
  gettext_process_po_files(${language} PO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${language}.gmo" DESTINATION "${CMAKE_INSTALL_LOCALEDIR}/${language}/LC_MESSAGES" RENAME "${PROJECT_NAME}.mo")
endforeach()

# Note: gettext_process_po_files() creates a global custom target named pofiles.
# (https://cmake.org/cmake/help/latest/module/FindGettext.html#command:gettext_process_po_files)

add_custom_target(build_po ALL DEPENDS ${LANGUAGES_FILE} pofiles COMMENT "Compiling the .po localization files")

message(STATUS "###")
message(STATUS "Note: The translations can be tested during development locally by installing to a stage folder")
message(STATUS "(e.g. by setting the -DCMAKE_INSTALL_PREFIX=stage when calling cmake to configure the project and running the install target after building)")
message(STATUS "and, in the stage folder, executing (for example for the de_DE locale):")
message(STATUS " mkdir -p lib/locale")
message(STATUS " localedef -i de_DE -f UTF-8 lib/locale/de_DE")
message(STATUS " sudo LOCPATH=lib/locale LC_ALL=de_DE bin/synaptic")
message(STATUS "###")
