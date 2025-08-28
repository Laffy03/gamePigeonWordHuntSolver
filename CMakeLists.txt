cmake_minimum_required(VERSION 3.25)
project(gamePigeonWordHuntSolver)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

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
        resources/resources.qrc
        ${SOURCES}
        ${HEADERS}
        )

target_link_libraries(gamePigeonWordHuntSolver PRIVATE Qt6::Widgets PRIVATE Qt6::Core)