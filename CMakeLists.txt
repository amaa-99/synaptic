cmake_minimum_required(VERSION 4.0)

project(synaptic VERSION 0.91.7 LANGUAGES C CXX)

option(SCRIPTS "Enable the extension system (LUA)" OFF)
option(DPKG_PROGRESS "Build with support for the dpkg progress bar" ON)
option(LAUNCHPAD_INTEGRATION "Build with launchpad-integration support" OFF)
option(PKG_HOLD "Build with the experimental package hold feature" OFF)
option(APT_MULTIARCH_SUPPORT "Build with apt multiarch support" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -O3 -flto=auto -fuse-linker-plugin -pipe)
add_link_options(-Wall -O3 -flto=auto -fuse-linker-plugin -pipe)

# Since GLib 2.62, the older deprecation mechanism of hiding deprecated interfaces entirely from the compiler by using the preprocessor symbol G_DISABLE_DEPRECATED has been removed.
# (https://docs.gtk.org/glib/compiling.html)
# The option is still included here for backport builds (e.g. Ubunbtu 22.04 LTS has glibc version 2.35).
add_compile_options(-DG_DISABLE_DEPRECATED)

# GTK/GDK specific definitions (https://docs.gtk.org/gtk3/compiling.html)
add_compile_options(-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED)

# Deprecated GTK functions are annotated to make the compiler emit warnings when they are used
# (e.g. with gcc, you need to use the -Wdeprecated-declarations option).

####################

# Enable testing for the current directory and below
# (https://cmake.org/cmake/help/latest/module/CTest.html#module:CTest)
include(CTest)

####################

# Install paths (https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)
# For the install() commands, use the relative variables such as ${CMAKE_INSTALL_BINDIR} rather than the FULL variants. 
include(GNUInstallDirs)
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(bindir "${CMAKE_INSTALL_BINDIR}")
set(datadir "${CMAKE_INSTALL_DATADIR}")
set(localedir "${CMAKE_INSTALL_LOCALEDIR}")
set(localstatedir "${CMAKE_INSTALL_LOCALSTATEDIR}")

# FindGettext provides gettext_process_po_files() for compiling .po files into .mo files.
# https://cmake.org/cmake/help/latest/module/FindGettext.html
find_package(Gettext REQUIRED)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

pkg_check_modules(APT_PKG REQUIRED IMPORTED_TARGET "apt-pkg")
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET "glib-2.0")
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET "gtk+-3.0>=3.4.0")
pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET "pango>=1.0.0")
pkg_check_modules(POLKIT REQUIRED IMPORTED_TARGET "polkit-gobject-1")
pkg_check_modules(XAPIAN REQUIRED IMPORTED_TARGET "xapian-core")

find_library(UTIL_LIB REQUIRED NAMES util)
find_library(APT_INST_LIB NAMES apt-inst)

include(CheckIncludeFileCXX)

# Tell CMake to check in the include directories found by PkgConfig for apt-pkg
set(CMAKE_REQUIRED_INCLUDES ${APT_PKG_INCLUDE_DIRS})

check_include_file_cxx("apt-pkg/configuration.h" HAVE_APT_PKG_CONFIG)
check_include_file_cxx("apt-pkg/metaindex.h" HAVE_APT_PKG_METAINDEX)
check_include_file_cxx("apt-pkg/cdrom.h" HAVE_APT_PKG_CDROM)

unset(CMAKE_REQUIRED_INCLUDES)

####################
# Check for the rpm headers

set(RPM_INCLUDE_DIR "/usr/include/rpm")
set(CMAKE_REQUIRED_INCLUDES "${RPM_INCLUDE_DIR}")
check_include_file_cxx("rpm/rpmlib.h" HAVE_RPM_HEADER)
unset(CMAKE_REQUIRED_INCLUDES)

set(HAVE_RPM FALSE)
set(RPM_LIBRARIES "")

if(HAVE_RPM_HEADER)
  find_library(RPM_LIBRARY NAMES rpm)
  find_library(RPMIO_LIBRARY NAMES rpmio)
  find_library(RPMDB_LIBRARY NAMES rpmdb)
  find_library(POPT_LIBRARY NAMES popt)
  if(RPM_LIBRARY AND RPMIO_LIBRARY AND RPMDB_LIBRARY AND POPT_LIBRARY)
    # Prefer the RPM v4 library set.
    set(RPM_LIBRARIES "${RPM_LIBRARY}" "${RPMIO_LIBRARY}" "${RPMDB_LIBRARY}" "${POPT_LIBRARY}")
    set(HAVE_RPM TRUE)
  elseif(RPM_LIBRARY AND POPT_LIBRARY)
    # Fall back to the RPM v3 library set.
    set(RPM_LIBRARIES "${RPM_LIBRARY}" "${POPT_LIBRARY}")
    set(HAVE_RPM TRUE)
  endif()
endif()

if(HAVE_RPM)
  #target_include_directories(your_target PRIVATE "${RPM_INCLUDE_DIR}")
  #target_link_libraries(your_target PRIVATE ${RPM_LIBRARIES})
endif()

#

if(NOT HAVE_APT_PKG_CONFIG)
    message(FATAL_ERROR "Installed apt-pkg headers are required for compiling synaptic!")
endif()

if(DPKG_PROGRESS)
  pkg_check_modules(VTE IMPORTED_TARGET "vte-2.91>=0.5")
  if(NOT VTE_FOUND)
    message(FATAL_ERROR "Dpkg-progress requires VTE! Disable it with -DDPKG_PROGRESS=OFF!")
  endif()
endif()

pkg_check_modules(LAUNCHPAD IMPORTED_TARGET "launchpad-integration")
if(LAUNCHPAD_INTEGRATION AND NOT LAUNCHPAD_FOUND)
    message(FATAL_ERROR "Launchpad integration was requested but launchpad-integration was not found!")
endif()

####################
# Collect information about the distribution

cmake_host_system_information(RESULT DISTRIB_ID QUERY DISTRIB_ID)
message(STATUS "Linux distro: ${DISTRIB_ID}")

find_program(DPKG_VENDOR_EXECUTABLE NAMES dpkg-vendor)

set(VENDOR_DERIVES_FROM_UBUNTU FALSE)

if(DPKG_VENDOR_EXECUTABLE)
  message(STATUS "Checking dpkg-vendor")
  execute_process(COMMAND "${DPKG_VENDOR_EXECUTABLE}" --derives-from ubuntu RESULT_VARIABLE DPKG_VENDOR_RESULT)

  if(DPKG_VENDOR_RESULT EQUAL 0)
    message(STATUS "Derives from UBUNTU")
    set(VENDOR_DERIVES_FROM_UBUNTU TRUE)
  endif()
endif()

####################
# Generate the config.h configuration file (from the template config.h.in file)

set(GETTEXT_PACKAGE "${PROJECT_NAME}")
set(PACKAGE "${PROJECT_NAME}")
set(VERSION "${PROJECT_VERSION}")

set(PACKAGE_DATA_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
set(PACKAGE_LOCALE_DIR "${CMAKE_INSTALL_LOCALEDIR}")
set(SYNAPTICSTATEDIR "${CMAKE_INSTALL_LOCALSTATEDIR}")

set(ENABLE_NLS 1)
set(HAVE_RPM "${HAVE_RPM}")
set(WITH_LUA "${SCRIPTS}")
set(WITH_APT_AUTH "${HAVE_APT_PKG_METAINDEX}")
set(HAVE_APTPKG_CDROM "${HAVE_APT_PKG_CDROM}")
set(HAVE_XAPIAN "${XAPIAN_FOUND}")
set(HAVE_VTE "${VTE_FOUND}")
set(HAVE_TERMINAL "${VTE_FOUND}")
set(WITH_DPKG_STATUSFD "${DPKG_PROGRESS}")
set(WITH_LAUNCHPAD_INTEGRATION "${LAUNCHPAD_FOUND}")
set(SYNAPTIC_PKG_HOLD "${PKG_HOLD}")
set(WITH_APT_MULTIARCH_SUPPORT "${APT_MULTIARCH_SUPPORT}")
set(VENDOR_DERIVES_FROM_UBUNTU "${VENDOR_DERIVES_FROM_UBUNTU}")

# Generate the configuration header file (with constants defined here)
set(CONFIG_H_IN "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in")
set(CONFIG_H "${CMAKE_CURRENT_BINARY_DIR}/config.h")
configure_file("${CONFIG_H_IN}" "${CONFIG_H}")

# Include the directory containing the header
include_directories("${PROJECT_BINARY_DIR}")

####################
# Pre-compile the gtkbuilder resources into a C source file

find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

set(GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/gtk/gtkbuilder/synaptic.gresource.xml")
set(GRESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/synaptic.gresource.c")

file(GLOB GTK_RESOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gtk/gtkbuilder/*.ui")

add_custom_command(
    OUTPUT ${GRESOURCE_C}
    DEPENDS ${GLIB_COMPILE_RESOURCES} ${GRESOURCE_XML} ${GTK_RESOURCES}
    COMMAND ${GLIB_COMPILE_RESOURCES} --generate-source --target=${GRESOURCE_C} --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/gtk ${GRESOURCE_XML}
    COMMENT "Compiling the gtk resources to ${GRESOURCE_C}"
    VERBATIM
)

####################

add_executable(synaptic)

install(TARGETS synaptic)

file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h")
file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gtk/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/gtk/*.h")

target_sources(synaptic PRIVATE ${CONFIG_H} ${COMMON_SOURCES} ${GUI_SOURCES} ${GRESOURCE_C} ${RESOURCE_SOURCES})

target_include_directories(synaptic PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/gtk"
)

target_link_libraries(synaptic PRIVATE 
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
    message(STATUS "Linking ${APT_INST_LIB}")
    target_link_libraries(synaptic PRIVATE ${APT_INST_LIB})
endif()

if(APT_PKG_FOUND)
    message(STATUS "Linking PkgConfig::APT_PKG")
    target_link_libraries(synaptic PRIVATE PkgConfig::APT_PKG)
endif()

if (VTE_FOUND)
    message(STATUS "Linking PkgConfig::VTE")
    target_link_libraries(synaptic PRIVATE PkgConfig::VTE)
endif()

if (XAPIAN_FOUND)
    message(STATUS "Linking Xapian::xapian")
    target_link_libraries(synaptic PRIVATE PkgConfig::XAPIAN)
endif()

####################

add_subdirectory(${PROJECT_SOURCE_DIR}/data)
add_subdirectory(${PROJECT_SOURCE_DIR}/man)
add_subdirectory(${PROJECT_SOURCE_DIR}/pixmaps)
add_subdirectory(${PROJECT_SOURCE_DIR}/po)

# Note: Ninja clean (always) tries to delete existing help and clean folders in the build folder (bug?).
# As a workaround the folder for the project's help folder is renamed here.
add_subdirectory(${PROJECT_SOURCE_DIR}/help ${PROJECT_BINARY_DIR}/help_)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

####################
