cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/synaptic.8" DESTINATION "${CMAKE_INSTALL_MANDIR}/man8")
