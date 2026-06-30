#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    // CONSTRUCTOR/DESTRUCTOR
    PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    // MANUAL DRAWINGS
    void paint(juce::Graphics&) override;

    // COMPONENTS & RESIZING
    void resized() override;

private:
    // REFERENCE TO 'PluginProcessor'
    PluginProcessor& pRef;
};