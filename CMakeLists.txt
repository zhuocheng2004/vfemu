cmake_minimum_required (VERSION 3.10)

project (vfemu)

include_directories (${CMAKE_SOURCE_DIR}/include/)


set (WITH_SDL2 TRUE CACHE BOOL "Enable SDL2-related parts")

set (BUILD_TNES TRUE CACHE BOOL "Build test NES simulator module")


if (${WITH_SDL2})
	find_package (SDL2 COMPONENTS SDL2)
	if (${SDL2_FOUND})
		message (STATUS "SDL2 Info: ")
		message (STATUS "    VERSION=${SDL2_VERSION}")
		message (STATUS "    DIR=${SDL2_DIR}")
		message (STATUS "    INCLUDE_DIRS=${SDL2_INCLUDE_DIRS}")
		message (STATUS "    LIBS=${SDL2_LIBS}")
		include_directories (${SDL2_INCLUDE_DIRS})
	else ()
		message (WARNING "SDL2 not found. Turned off.")
		set (WITH_SDL2 FALSE)
	endif ()
endif ()

add_subdirectory (src/)
add_subdirectory (modules/)
add_subdirectory (examples/)
