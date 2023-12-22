
add_executable (demo_toyram demo_toyram.cpp)
target_link_libraries (demo_toyram modules_core core)

add_executable (demo_tst001 demo_tst001.cpp)
target_link_libraries (demo_tst001 modules_core core)

if (${BUILD_TNES})
	add_executable (demo_tnes01 demo_tnes01.cpp)
	# It seems that the link order is important
	target_link_libraries (demo_tnes01 tnes01 modules_core core)
	if (${WITH_SDL2})
		target_link_libraries (demo_tnes01 SDL2::SDL2)
	endif ()
endif ()
