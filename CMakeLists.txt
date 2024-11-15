find_package(SDL2 REQUIRED)

# Add the library
add_library(NanoDraw STATIC
    
    NanoDraw.cpp

    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/backends/imgui_impl_sdl2.cpp
    extern/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(NanoDraw PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL2_INCLUDE_DIR}
    extern 
    extern/imgui
)

target_link_libraries(NanoDraw 
    ${SDL2_LIBRARIES}
)