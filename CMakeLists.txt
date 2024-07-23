cmake_minimum_required(VERSION 3.1...3.29)

project(
    3DC
    VERSION 1.0
    LANGUAGES CXX
)

set(PROJECT_SRC "main.cpp")
set(PROJECT_INC "")
set(PROJECT_LIB "")

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
list(APPEND PROJECT_INC ${CURSES_INCLUDE_DIRS})
list(APPEND PROJECT_LIB ${CURSES_LIBRARIES})

find_package(Eigen3 REQUIRED)
list(APPEND PROJECT_LIB Eigen3::Eigen)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB})
