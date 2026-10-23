cmake_minimum_required(VERSION 4.0)

include(GNUInstallDirs)

####################
# Localize the metadata files

find_program(MSGFMT_EXECUTABLE NAMES msgfmt REQUIRED)

set(PO_DIR "${PROJECT_SOURCE_DIR}/po")
file(GLOB PO_FILES CONFIGURE_DEPENDS "${PO_DIR}/*.po")

set(PO_INPUTS ${PO_FILES})
if(EXISTS "${PO_DIR}/LINGUAS")
  list(APPEND PO_INPUTS "${PO_DIR}/LINGUAS")
endif()

function(add_po_merged_file mode name install_dir)

  set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${name}.in")
  set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}")

  add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    DEPENDS "${MSGFMT_EXECUTABLE}" "${INPUT_FILE}" ${PO_INPUTS}
    COMMAND "${MSGFMT_EXECUTABLE}" "--${mode}" "--template=${INPUT_FILE}" -d "${PO_DIR}" -o "${OUTPUT_FILE}"
    COMMENT "Translating ${name}"
    VERBATIM
  )
  
  list(APPEND OUTPUT_FILES "${OUTPUT_FILE}")
  set(OUTPUT_FILES "${OUTPUT_FILES}" PARENT_SCOPE)
  
  install(FILES "${OUTPUT_FILE}" DESTINATION "${install_dir}")

endfunction()

add_po_merged_file("desktop" "synaptic.desktop" "${CMAKE_INSTALL_DATADIR}/applications")
add_po_merged_file("xml" "com.ubuntu.pkexec.synaptic.policy" "${CMAKE_INSTALL_DATADIR}/polkit-1/actions")
add_po_merged_file("xml" "io.github.mvo5.synaptic.metainfo.xml" "${CMAKE_INSTALL_DATADIR}/metainfo")

add_custom_target(build_data ALL DEPENDS ${OUTPUT_FILES} COMMENT "Compiling the metadata files...")
