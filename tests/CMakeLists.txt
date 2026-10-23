cmake_minimum_required(VERSION 4.0)

####################

# Enable testing for the current directory and below
# (https://cmake.org/cmake/help/latest/command/enable_testing.html)
enable_testing()

# Note: Testing must be enabled on the parent/root project too for the tests to be
# visible when building the parent/root.

####################
# Settings to be used by all targets below

include_directories(
  "${CMAKE_BINARY_DIR}"
  "${CMAKE_SOURCE_DIR}/common"
  "${CMAKE_SOURCE_DIR}/gtk"
)

link_libraries(
  PkgConfig::APT_PKG
  PkgConfig::GLIB
  PkgConfig::GTK
  PkgConfig::PANGO
  PkgConfig::POLKIT
  Threads::Threads
  ${UTIL_LIB}
  ${X11_LIB}
)

if(APT_INST_LIB)
  link_libraries(${APT_INST_LIB})
endif()

if(APT_PKG_FOUND)
  link_libraries(PkgConfig::APT_PKG)
endif()

if (VTE_FOUND)
  link_libraries(PkgConfig::VTE)
endif()

if (XAPIAN_FOUND)
  link_libraries(PkgConfig::XAPIAN)
endif()

# Include debug information on the test executables
add_compile_options(-g3)

####################

# The GUI sources minus the implementation of main()
set(GUI_TEST_SOURCES ${GUI_SOURCES})
list(REMOVE_ITEM GUI_TEST_SOURCES "${CMAKE_SOURCE_DIR}/gtk/gsynaptic.cc")

####################

add_executable(test_rpackage test_rpackage.cc ${CONFIG_H} ${COMMON_SOURCES})
add_test(NAME test_rpackage COMMAND test_rpackage)
set_tests_properties(test_rpackage PROPERTIES ENVIRONMENT "LC_ALL=C")

####################

add_executable(test_rpackagefilter test_rpackagefilter.cc ${CONFIG_H} ${COMMON_SOURCES})
add_test(NAME test_rpackagefilter COMMAND test_rpackagefilter)
set_tests_properties(test_rpackagefilter PROPERTIES ENVIRONMENT "LC_ALL=C")

####################

add_executable(test_rpackageview test_rpackageview.cc ${CONFIG_H} ${COMMON_SOURCES})
add_test(NAME test_rpackageview COMMAND test_rpackageview)
set_tests_properties(test_rpackageview PROPERTIES ENVIRONMENT "LC_ALL=C")

####################

#add_executable(test_gtkpkglist test_gtkpkglist.cc ${CONFIG_H} ${COMMON_SOURCES} ${GUI_TEST_SOURCES})
#add_test(NAME test_gtkpkglist COMMAND test_gtkpkglist)
#set_tests_properties(test_gtkpkglist PROPERTIES ENVIRONMENT "LC_ALL=C")

####################

find_package(GTest REQUIRED)

if(GTest_FOUND)

  ####################

  add_executable(test_rsources test_rsources.cc fake_sources_dir.h ${CONFIG_H} ${COMMON_SOURCES})
  target_link_libraries(test_rsources PRIVATE GTest::gtest_main)
  add_test(NAME test_rsources COMMAND test_rsources)
  set_tests_properties(test_rsources PROPERTIES ENVIRONMENT "LC_ALL=C")

  ####################

endif()
