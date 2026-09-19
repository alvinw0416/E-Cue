#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Editor.h"

class Editor : public juce::AudioProcessorEditor
{
public:
    Editor(Processor&);
    ~Editor() override;
    
    void paint(juce::Graphics&) override;   // Manual Drawings
    void resized() override;                // Positioning & Resizing

private:
    // REFERENCE TO 'Processor'
    Processor& pRef;

    // ------------------------------------------------------- Here starts other members/methods
    //
};