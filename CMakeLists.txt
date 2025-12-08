cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
project(CSFP)

find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE HEADER_FILES_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/Client/*.hpp)
file(GLOB_RECURSE HEADER_FILES_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/Server/*.hpp)
file(GLOB_RECURSE SRC_FILES_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/Client/*.cpp)
file(GLOB_RECURSE SRC_FILES_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/Server/*.cpp)


add_executable(Client ${HEADER_FILES_CLIENT} ${SRC_FILES_CLIENT})
add_executable(Server ${HEADER_FILES_SERVER} ${SRC_FILES_SERVER})

target_link_libraries(Server PRIVATE SQLite::SQLite3)
target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Common_Code)
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Common_Code)