cmake_minimum_required(VERSION 3.15)
project(FFGLPlugins)

set(CMAKE_CXX_STANDARD 17)

set(FFGL_ROOT ${CMAKE_SOURCE_DIR}/ffgl)
set(FFGL_LIB  ${FFGL_ROOT}/source/lib)

include_directories(
    ${FFGL_LIB}
    ${FFGL_LIB}/ffgl
    ${FFGL_LIB}/ffglquickstart
    ${FFGL_LIB}/ffglex
)

set(FFGL_SOURCES
    ${FFGL_LIB}/FFGLSDK.cpp
)

macro(add_ffgl_plugin NAME SOURCE)
    add_library(${NAME} MODULE ${SOURCE} ${FFGL_SOURCES})
    target_include_directories(${NAME} PRIVATE ${FFGL_LIB} ${FFGL_LIB}/ffgl ${FFGL_LIB}/ffglquickstart ${FFGL_LIB}/ffglex)

    if(APPLE)
        set_target_properties(${NAME} PROPERTIES
            BUNDLE TRUE
            BUNDLE_EXTENSION bundle
            PREFIX ""
        )
        target_link_libraries(${NAME} "-framework OpenGL")
    else()
        set_target_properties(${NAME} PROPERTIES
            PREFIX ""
            SUFFIX ".dll"
        )
        target_link_libraries(${NAME} opengl32)
    endif()
endmacro()

add_ffgl_plugin(ChromaticDots  source/ChromaticDots/ChromaticDots.cpp)
add_ffgl_plugin(Ouchi          source/Ouchi/Ouchi.cpp)
