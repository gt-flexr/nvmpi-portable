macro(use_opencv)
  pkg_search_module(OPENCV REQUIRED opencv4)
  if(OPENCV_FOUND)
    message(STATUS "OpenCV4 Details")
    message("\t OPENCV_INCLUDE_DIRS: ${OPENCV_INCLUDE_DIRS}")
    message("\t OPENCV_LDFLAGS: ${OPENCV_LDFLAGS}")

    include_directories(${OPENCV_INCLUDE_DIRS})
    list(APPEND NVMPI_PORTABLE_CXX_FLAGS ${OPENCV_CXX_FLAGS})
    list(APPEND NVMPI_PORTABLE_LINKER_FLAGS ${OPENCV_LDFLAGS})
  endif(OPENCV_FOUND)
endmacro()

use_opencv()

