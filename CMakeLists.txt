cmake_minimum_required(VERSION 3.19)
project(Snake LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)

set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

qt_standard_project_setup()

qt_add_executable(Snake
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    RingBuffer.h
    FreeIndicesArray.h

    snake.h
    game.h game.cpp
    SnakeStepResult.h
    gamesettings.h
    Direction.h
    GameState.h
    Controls.h



)

target_link_libraries(Snake
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets

        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

install(TARGETS Snake
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Snake
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
