set(CXX_FLAGS "-Wall -Wextra -pthread -pedantic -std=c++14 -fno-rtti -fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,-02 ${LTO_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

find_package(Extra REQUIRED)

add_custom_target(hack SOURCES common.h)

add_executable(cbs server.cpp)
add_executable(cbc client.cpp)
target_link_libraries(cbs ${Extra_LIBRARIES})
target_link_libraries(cbc ${Extra_LIBRARIES})

install(TARGETS cbs RUNTIME DESTINATION bin)
install(TARGETS cbc RUNTIME DESTINATION bin)
