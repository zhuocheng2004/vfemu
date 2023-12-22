
set (TNES_SOURCES nes.cpp nespin.cpp 
				nescm.cpp nesct01.cpp cartridge.cpp nesppu.cpp tnes01.cpp)

if (${WITH_SDL2})
	set (TNES_SOURCES ${TNES_SOURCES} nesppu_sdl2.cpp)
else ()
	set (TNES_SOURCES ${TNES_SOURCES} nesppu_dummy.cpp)
endif ()

add_library (tnes01 ${TNES_SOURCES})
target_compile_options (tnes01 PRIVATE -Wimplicit-fallthrough)
