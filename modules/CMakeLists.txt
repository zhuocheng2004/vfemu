
add_subdirectory (core/)

if (${BUILD_TNES})
	add_subdirectory (nes/)
endif ()
