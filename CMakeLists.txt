cmake_minimum_required(VERSION 3.23)
project(TEMPLATENAME VERSION 1.0.0)

# C++ VERSION
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PULLS IN JUCE
include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.14  
)
FetchContent_MakeAvailable(JUCE)

# PULLS IN ASSETS
juce_add_binary_data(PluginAssets
    SOURCES
    assets/test.png         
)

# PLUGIN METADATA
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "Template Name"
    PLUGIN_MANUFACTURER_CODE TEMP
    PLUGIN_CODE Tmp1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Template" 
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE         
)

# SOURCE FILES + IDE ORGANIZATION
set(SOURCES
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX "Source Files" FILES ${SOURCES})

# LINKS MODULES & EXTERNAL FILES
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    # juce::juce_analytics                              
    # juce::juce_audio_devices                         
    juce::juce_audio_processors
    juce::juce_audio_utils
    # juce::juce_data_structures                        
    juce::juce_gui_basics
    # juce::juce_gui_extra
    # juce::juce_graphics
    juce::juce_dsp
    # Add more here...

    PluginAssets

    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags                
)

# COMPILE DEFS
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# ENSURES HEADER INCLUSION ON ALL FILES
juce_generate_juce_header(${PROJECT_NAME})