add_library(raygui INTERFACE)

target_include_directories(raygui INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/icons
        ${CMAKE_CURRENT_SOURCE_DIR}/styles)