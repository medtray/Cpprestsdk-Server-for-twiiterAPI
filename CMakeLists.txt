cmake_minimum_required(VERSION 3.7)
set (CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/ )
SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
SET(CMAKE_C_FLAGS_DEBUG "-O0")
set(libmongoc-1.0_DIR "mongoDB/build-dir/libmongoc-install/lib/cmake/libmongoc-1.0")
set(libmongocxx_DIR "mongoDB/build-dir/libmongocxx-install/lib/cmake/libmongocxx-3.3.1")
set(libbson-1.0_DIR "mongoDB/build-dir/libmongoc-install/lib/cmake/libbson-1.0")
set(libbsoncxx_DIR "mongoDB/build-dir/libmongocxx-install/lib/cmake/libbsoncxx-3.3.1")
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
find_package( Boost COMPONENTS thread system filesystem REQUIRED )
find_package(cpprestsdk REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )
find_package( Threads )

add_executable(server server.cc)
target_link_libraries(server PUBLIC ${LIBMONGOCXX_LIBRARIES} ssl cpprest crypto twitcurl jsoncpp)
target_include_directories(server PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_compile_definitions(server PUBLIC ${LIBMONGOCXX_DEFINITIONS})
TARGET_LINK_LIBRARIES(server LINK_PUBLIC ${Boost_LIBRARIES})





