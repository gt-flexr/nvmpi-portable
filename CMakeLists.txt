cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(nvmpi_portable CXX)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_C_FLAGS“$ {CMAKE_C_FLAGS} -fPIC”)
set(CMAKE_CXX_FLAGS“$ {CMAKE_CXX_FLAGS} -fPIC”)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(PkgConfig REQUIRED)

# NVMPI_PORTABLE compile info
set(NVMPI_PORTABLE_CXX_FLAGS "")
set(NVMPI_PORTABLE_LINKER_FLAGS "")
set(NVMPI_PORTABLE_LINKER_LIBS "")

# Include cmake modules
include_directories("include")
include(cmake/ffmpeg.cmake)
include(cmake/nvmpi.cmake)
include(cmake/opencv4.cmake)

message(STATUS "[NVMPI_PORTABLE COMPILE INFO]")

message("\t NVMPI_PORTABLE_CXX_FLAGS: ${NVMPI_PORTABLE_CXX_FLAGS}")
message("\t NVMPI_PORTABLE_LINKER_FLAGS: ${NVMPI_PORTABLE_LINKER_FLAGS}")

add_library(nvmpi_portable SHARED
  nvmpi_portable_enc.cpp
  nvmpi_portable_dec.cpp)

add_library(nvmpi_portable_static STATIC
  nvmpi_portable_enc.cpp
  nvmpi_portable_dec.cpp)

set_target_properties(nvmpi_portable_static PROPERTIES OUTPUT_NAME nvmpi_portable)
set_target_properties(nvmpi_portable nvmpi_portable_static PROPERTIES PUBLIC_HEADER nvmpi_portable.h)
target_link_libraries(nvmpi_portable PRIVATE ${NVMPI_PORTABLE_LINKER_FLAGS})
target_include_directories(nvmpi_portable PRIVATE ${NVMPI_PORTABLE_CXX_FLAGS})
target_include_directories(nvmpi_portable_static PRIVATE ${NVMPI_PORTABLE_CXX_FLAGS})
configure_file(nvmpi_portable.pc.in nvmpi_portable.pc @ONLY)
install(TARGETS nvmpi_portable nvmpi_portable_static
	    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/nvmpi_portable.pc
	    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
install(FILES ${CMAKE_BINARY_DIR}/nvmpi_portable.pc
	    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
