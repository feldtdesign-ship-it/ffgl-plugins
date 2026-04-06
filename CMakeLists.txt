cmake_minimum_required(VERSION 3.15)
project(FFGLPlugins)

set(CMAKE_CXX_STANDARD 17)

# Path to the ffgl SDK -- expects the ffgl repo as a submodule at ./ffgl
set(FFGL_ROOT ${CMAKE_SOURCE_DIR}/ffgl)
set(FFGL_LIB  ${FFGL_ROOT}/source/lib)

# Shared include dirs for all plugins
include_directories(
    ${FFGL_LIB}
    ${FFGL_LIB}/ffgl
    ${FFGL_LIB}/ffglquickstart
    ${FFGL_LIB}/ffglex
)

# Shared FFGL SDK source files compiled into every plugin
set(FFGL_SOURCES
    ${FFGL_LIB}/ffgl/FFGL.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLSource.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLEffect.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLPlugin.cpp
    ${FFGL_LIB}/ffglex/FFGLScopedShaderObjectBinding.cpp
    ${FFGL_LIB}/ffglex/FFGLShader.cpp
    ${FFGL_LIB}/ffglex/FFGLQuad.cpp
)

# Helper macro -- adds one plugin target
# Usage: add_ffgl_plugin(PluginName source/PluginName/PluginName.cpp)
macro(add_ffgl_plugin NAME SOURCE)
    add_library(${NAME} MODULE ${SOURCE} ${FFGL_SOURCES})

    if(APPLE)
        set_target_properties(${NAME} PROPERTIES
            BUNDLE TRUE
            BUNDLE_EXTENSION bundle
            PREFIX ""
        )
    else()
        set_target_properties(${NAME} PROPERTIES
            PREFIX ""
            SUFFIX ".dll"
        )
    endif()
endmacro()

# --- Register each plugin here ---
add_ffgl_plugin(RotatingSnakes  source/RotatingSnakes/RotatingSnakes.cpp)
add_ffgl_plugin(ChromaticDots   source/ChromaticDots/ChromaticDots.cpp)
add_ffgl_plugin(Ouchi           source/Ouchi/Ouchi.cpp)
add_ffgl_plugin(WagonWheel      source/WagonWheel/WagonWheel.cpp)
add_ffgl_plugin(Dreamachine     source/Dreamachine/Dreamachine.cpp)
