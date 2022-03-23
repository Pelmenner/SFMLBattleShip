cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(SFML_STATIC)

set(RESOURCES
    data/button.png
    data/font.ttf
    data/icon.png
    data/TitleFont.ttf
)

set(SOURCES
    src/Board.cpp
    src/Button.cpp
    src/Connection.cpp
    src/EnemyBoard.cpp
    src/Functions.cpp
    src/Game.cpp
    src/main.cpp
    src/Online.cpp
)

project(BattleShip VERSION 1.0)
add_executable(BattleShip ${SOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(SFML_GRAPHICS sfml-graphics-s-d REQUIRED)
    find_library(SFML_WINDOW sfml-window-s-d REQUIRED)
    find_library(SFML_SYSTEM sfml-system-s-d REQUIRED)
    find_library(SFML_NETWORK sfml-network-s-d REQUIRED)
    set(SFML ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM} ${SFML_NETWORK})
else()
    find_library(SFML_GRAPHICS sfml-graphics-s REQUIRED)
    find_library(SFML_WINDOW sfml-window-s REQUIRED)
    find_library(SFML_SYSTEM sfml-system-s REQUIRED)
    find_library(SFML_NETWORK sfml-network-s REQUIRED)
    set(SFML ${SFML_GRAPHICS} ${SFML_WINDOW} ${SFML_SYSTEM} ${SFML_NETWORK})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_library(FREETYPE freetype REQUIRED)
    find_library(OPENGL32 opengl32 REQUIRED)
    find_library(WINMM winmm REQUIRED)
    find_library(GDI32 gdi32 REQUIRED)
    find_library(WS232 ws2_32 REQUIRED)
    set(SFML_DEPENDENCIES ${FREETYPE} ${OPENGL32} ${GDI32} ${WINMM} ${WS232})
else()
    find_library(SFML_DEPENDENCIES pthread REQUIRED)
endif()

target_include_directories(BattleShip PRIVATE $<SEMICOLON>$<JOIN:${CMAKE_SYSTEM_PREFIX_PATH},/include$<SEMICOLON>>/include)
target_link_libraries(BattleShip PRIVATE ${SFML} ${SFML_DEPENDENCIES})

install(TARGETS BattleShip DESTINATION .)
install(FILES ${RESOURCES} DESTINATION ./data)
