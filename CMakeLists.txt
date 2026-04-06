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
    ${FFGL_LIB}/ffgl/FFGL.cpp
    ${FFGL_LIB}/ffgl/FFGLPluginInfo.cpp
    ${FFGL_LIB}/ffgl/FFGLPluginInfoData.cpp
    ${FFGL_LIB}/ffgl/FFGLPluginManager.cpp
    ${FFGL_LIB}/ffgl/FFGLPluginSDK.cpp
    ${FFGL_LIB}/ffgl/FFGLLog.cpp
    ${FFGL_LIB}/ffgl/FFGLThumbnailInfo.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLPlugin.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLSource.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLEffect.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLMixer.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParam.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamRange.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamBool.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamOption.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamEvent.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamTrigger.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamText.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLParamFFT.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLAudio.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLRandom.cpp
    ${FFGL_LIB}/ffglquickstart/FFGLSmoothValue.cpp
    ${FFGL_ROOT}/source/lib/FFGLSDK.cpp
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
