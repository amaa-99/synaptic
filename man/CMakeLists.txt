cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/synaptic.8" DESTINATION "${CMAKE_INSTALL_MANDIR}/man8")

set(TARGET_FOLDER "share/man") # Relative to the install prefix

file(GLOB manpages CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.*.8")

foreach(manpage IN LISTS manpages)

  get_filename_component(filename "${manpage}" EXT)
  get_filename_component(filename "${filename}" EXT)

  install(FILES "${manpage}" DESTINATION "${CMAKE_INSTALL_MANDIR}/${lang}" RENAME "synaptic.8")

endforeach()
