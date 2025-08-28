cmake_minimum_required(VERSION 3.25)
project(gamePigeonWordHuntSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.1/mingw_64/lib/cmake")
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
        main.cpp
        WordHuntGUI.cpp
        wordHuntTrie.cpp
        )

set(HEADERS
        WordHuntGUI.h
        wordHuntTrie.h
        )

qt_add_executable(gamePigeonWordHuntSolver
        ${SOURCES}
        ${HEADERS}
        )

target_link_libraries(gamePigeonWordHuntSolver PRIVATE Qt6::Widgets)