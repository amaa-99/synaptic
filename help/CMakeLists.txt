cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

####################
# Install the XML (yelp) manua

set(HELP_INSTALL_ROOT "${CMAKE_INSTALL_DATADIR}/gnome/help/synaptic")

foreach(lang IN ITEMS C es hr sv)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${lang}/synaptic.xml ${CMAKE_CURRENT_SOURCE_DIR}/${lang}/gpl.xml DESTINATION ${HELP_INSTALL_ROOT}/${lang})
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lang}/figures DESTINATION ${HELP_INSTALL_ROOT}/${lang})
endforeach()

####################
# Compile and install the  HTML manual (fallback for the Help menu when yelp is missing (rgmainwindow.cc))

find_program(XMLTO_EXECUTABLE NAMES xmlto REQUIRED)

set(SYNAPTIC_XML "${CMAKE_CURRENT_SOURCE_DIR}/C/synaptic.xml")
set(HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
set(HTML_FILE "${HTML_DIR}/synaptic.html")

file(GLOB_RECURSE HTML_FIGURES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/C/figures/*")

add_custom_command(
  OUTPUT "${HTML_FILE}" "${HTML_DIR}"
  DEPENDS "${XMLTO_EXECUTABLE}" ${SYNAPTIC_XML} ${HTML_FIGURES}
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${HTML_DIR}"
  COMMAND "${XMLTO_EXECUTABLE}" html-nochunks --skip-validation -o ${HTML_DIR} ${SYNAPTIC_XML}
  COMMAND rm -f ${HTML_DIR}/synaptic.proc
  COMMENT "Generating the html manual"
  VERBATIM
)

install(DIRECTORY "${HTML_DIR}" DESTINATION "${CMAKE_INSTALL_DATADIR}")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/C/figures" DESTINATION "${CMAKE_INSTALL_DATADIR}/html")

add_custom_target(build_help ALL DEPENDS "${HTML_FILE}" COMMENT "Compiling the help files")

####################
