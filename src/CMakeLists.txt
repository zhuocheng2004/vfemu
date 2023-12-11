
add_library(core Port.cpp Connector.cpp Module.cpp)

add_executable(vfemu main.cpp)
target_link_libraries(vfemu core modules_core)
