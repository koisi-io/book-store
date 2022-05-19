cmake_minimum_required(VERSION 3.22)
project(book_store)

set(CMAKE_CXX_STANDARD 20)

AUX_SOURCE_DIRECTORY(./src/core    SOURCE_FILES)
add_executable(client src/main/client.cpp ${SOURCE_FILES})
add_executable(server src/main/srv.cpp    ${SOURCE_FILES})

include_directories(/usr/local/include)
include_directories(/Users/koisi/Env/lib/luajit2/src)
include_directories(/usr/local/Cellar/cppzmq/4.8.1/include)
include_directories(./deps/json)
include_directories(./src/core)

find_package(cppzmq REQUIRED)
target_link_libraries(client cppzmq)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 6ea2dec23c3df14ac3b27b7d2d6bbff0cb7ba1b0)
FetchContent_MakeAvailable(cpr)
link_libraries(cpr::cpr)
