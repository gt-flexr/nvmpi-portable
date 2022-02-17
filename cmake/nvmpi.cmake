macro(use_nvmpi)
  pkg_search_module(NVMPI REQUIRED nvmpi)
  if(NVMPI_FOUND)
    message(STATUS "NVMPI Details")
    message("\t NVMPI_INCLUDE_DIRS: ${NVMPI_INCLUDE_DIRS}")
    message("\t NVMPI_LDFLAGS: ${NVMPI_LDFLAGS}")

    include_directories(${NVMPI_INCLUDE_DIRS})
    list(APPEND NVMPI_PORTABLE_CXX_FLAGS ${NVMPI_CXX_FLAGS})
    list(APPEND NVMPI_PORTABLE_LINKER_FLAGS ${NVMPI_LDFLAGS})
  endif(NVMPI_FOUND)
endmacro()

use_nvmpi()

