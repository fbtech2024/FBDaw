cmake_minimum_required(VERSION 3.22)
project(FBDaw VERSION 0.1.0)

# JUCE: scaricalo da https://juce.com/ (gratis) e scompatta in questa cartella
# come sottocartella "JUCE", oppure imposta la variabile JUCE_DIR.
if(NOT DEFINED JUCE_DIR)
    set(JUCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/JUCE)
endif()
add_subdirectory(${JUCE_DIR} JUCE_build)

juce_add_gui_app(FBDaw
    COMPANY_NAME "FBTech"
    ICON_BIG ""
)

target_sources(FBDaw PRIVATE
    Source/Main.cpp
    Source/Engine.cpp
    Source/MainComponent.cpp
)

target_compile_definitions(FBDaw PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_PLUGINHOST_VST3=1
    JUCE_PLUGINHOST_VST=0
    JUCE_PLUGINHOST_AU=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(FBDaw PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
)
