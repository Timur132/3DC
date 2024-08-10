CMAKE_minimum_required(VERSION 3.1...3.29)

project(
    3DC
    VERSION 1.0
    LANGUAGES CXX
)

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS "src/*.cpp")
list(APPEND PROJECT_SRC "main.cpp")
Set(PROJECT_INC "include")
set(PROJECT_LIB "pdcurses.dll")
set(PROJECT_CFLAGS "")

find_package(Eigen3 REQUIRED)
list(APPEND PROJECT_LIB Eigen3::Eigen)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB})
