add_library(raylib SHARED IMPORTED GLOBAL)

if (WIN32 AND MSVC)
    set_property(TARGET raylib PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/win_x64/raylib.dll")
    set_property(TARGET raylib PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win_x64/raylib.lib")
elseif (APPLE AND NOT IOS)
    set_property(TARGET raylib PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/mac/libraylib.dylib")
elseif (LINUX)
    set_property(TARGET raylib PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux_x64/libraylib.so")
endif()

target_include_directories(raylib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

###
## Link additional required platform SDKs and frameworks to raylib:
###

# Refer to the "A Note On Dependencies" section here:
if (WIN32 AND MSVC) # https://github.com/raysan5/raylib/wiki/Working-on-Windows
    set(PLATFORM_EXT_LIBS "winmm" "gdi32" "user32" "shell32")
elseif (APPLE AND NOT IOS) # https://github.com/raysan5/raylib/wiki/Working-on-macOS
    set(PLATFORM_EXT_LIBS "-framework CoreVideo" "-framework IOKit" "-framework Cocoa" "-framework GLUT" "-framework OpenGL")
elseif (LINUX) # https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux
    set(PLATFORM_EXT_LIBS "m" "pthread" "dl" "rt" "X11")
else ()
    set(PLATFORM_EXT_LIBS)
endif ()

target_link_libraries(raylib INTERFACE ${PLATFORM_EXT_LIBS})