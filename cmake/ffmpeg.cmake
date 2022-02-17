macro(add_ffmpeg_module target)
  pkg_search_module("${target}1" ${target})
  list(APPEND FFMPEG_INCLUDE_DIRS "${${target}1_INCLUDE_DIRS}")
  list(APPEND FFMPEG_CXX_FLAGS "${${target}1_CFLAGS}")
  list(APPEND FFMPEG_CXX_FLAGS "${${target}1_CFLAGS_OTHER}")
  list(APPEND FFMPEG_LDFLAGS "${${target}1_LDFLAGS}")
endmacro()

macro(use_ffmpeg)
  SET(FFMPEG_INCLUDE_DIRS "")
  SET(FFMPEG_CXX_FLAGS "")
  SET(FFMPEG_LDFLAGS "")

  SET(FFMPEG_MODULES libavcodec libavformat libavutil libswresample libswscale)
  foreach(FFMPEG_MODULE ${FFMPEG_MODULES})
    add_ffmpeg_module(${FFMPEG_MODULE})
  endforeach(FFMPEG_MODULE)

  message(STATUS "[FFMpeg Details]")
  message("\t FFMPEG_INCLUDE_DIRS: ${FFMPEG_INCLUDE_DIRS}")
  message("\t FFMPEG_LDFLAGS: ${FFMPEG_LDFLAGS}")

  include_directories(${FFMPEG_INCLUDE_DIRS})
  list(APPEND NVMPI_PORTABLE_CXX_FLAGS ${FFMPEG_CXX_FLAGS})
  list(APPEND NVMPI_PORTABLE_LINKER_FLAGS ${FFMPEG_LDFLAGS})
endmacro()

use_ffmpeg()

