cmake_minimum_required (VERSION 2.6)
project (john)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/usr/include)
if(USE_NCURSES)
include_directories(${PROJECT_SOURCE_DIR}/include/curses)
endif()


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

option(USE_NCURSES "USE_NCURSES" off)

if(USE_NCURSES)
	add_definitions(-DNCURSES)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(john main.cpp engine.cpp map.cpp actor.cpp camera.cpp console.cpp glyph.cpp item.cpp tools.cpp worldobject.cpp)

target_link_libraries(john ${CURSES_LIBRARIES})


