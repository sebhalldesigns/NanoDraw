cmake_minimum_required(VERSION 3.10)

project(NanoDraw)

# Embed Resources function courtesy of shir0areed on GitHub
function(embed_resources target)
    set(script_path "${CMAKE_CURRENT_BINARY_DIR}/anything_to_c.cmake")
    file(WRITE  "${script_path}" "file(READ \${CMAKE_ARGV3} buf HEX)\n")
    file(APPEND "${script_path}" "string(REGEX REPLACE \"([0-9a-f][0-9a-f])\" \"0x\\\\1, \" buf \${buf})\n")
    file(APPEND "${script_path}" "file(WRITE \${CMAKE_ARGV4} \"const unsigned char \${CMAKE_ARGV5}[] = { \${buf}0x00 };\\n\")\n")
    file(APPEND "${script_path}" "file(APPEND \${CMAKE_ARGV4} \"const unsigned \${CMAKE_ARGV6} = sizeof(\${CMAKE_ARGV5}) - 1;\\n\")\n")
    foreach(res_path ${ARGN})
        string(MAKE_C_IDENTIFIER ${res_path} identifier)
        set(src_path "${CMAKE_CURRENT_SOURCE_DIR}/${res_path}")
        set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/${identifier}.c")
        set(anything_to_c ${CMAKE_COMMAND} -P ${script_path} ${src_path} ${dst_path} ${identifier} ${identifier}_size)
        add_custom_command(OUTPUT ${dst_path} COMMAND ${anything_to_c} DEPENDS ${src_path} VERBATIM)
        target_sources(${target} PRIVATE ${src_path} ${dst_path} )
    endforeach()
endfunction()



if (WIN32)

    set(NANODRAW_SOURCES
        lib/backends/gl/nanodraw.c
        lib/font-8x8.c
        lib/nkfont.c
        extern/glad/glad.c
    )

    set(NANODRAW_LIBS
        user32
        opengl32
    )

elseif(UNIX)

    set(NANODRAW_SOURCES
        lib/backends/gl/nanodraw.c
    )

elseif(APPLE)

    set(NANODRAW_SOURCES
        lib/backends/gl/nanodraw.c
    )
    
elseif(EMSCRIPTEN)

    set(NANODRAW_SOURCES
        lib/backends/gl/nanodraw.c
    )
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

add_library(NanoDraw STATIC 
    ${NANODRAW_SOURCES}
)

target_include_directories(NanoDraw PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_link_libraries(NanoDraw PUBLIC
    ${NANODRAW_LIBS}
)

embed_resources(NanoDraw
    shaders/glsl/general.vert
    shaders/glsl/general.frag
)
