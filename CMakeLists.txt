cmake_minimum_required(VERSION 3.6)
project(egglist)

set(CMAKE_CXX_STANDARD 11)

#if (CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "-O3")        ## Optimize
#    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
#endif ()

set(SOURCE_FILES main.cpp egglist-config.cpp egglist-config.h egglist-defaults.h egglist-engine.cpp egglist-engine.h egglist-utils.cpp egglist-utils.h json/json.hpp)
add_executable(egglist ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(egglist Threads::Threads)
