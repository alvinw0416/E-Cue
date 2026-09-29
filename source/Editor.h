#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Processor.h"
#include "LabeledKnob.h"

class Editor : public juce::AudioProcessorEditor
{
public:
    Editor(Processor&);
    ~Editor() override;
    
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Processor& pRef;

    // ------------------------------------------------------- Here starts other members/methods

    juce::OwnedArray<LabeledKnob> knobs;
    juce::ToggleButton enabledButton { "Plugin Enabled" };
    juce::ToggleButton auditButton { "Audition Mode" };
    juce::ToggleButton hpfButton { "HPF" };
    juce::ToggleButton lpfButton { "LPF" };

    juce::AudioProcessorValueTreeState::ButtonAttachment enabledAttach;
    juce::AudioProcessorValueTreeState::ButtonAttachment auditAttach;
    juce::AudioProcessorValueTreeState::ButtonAttachment hpfAttach;
    juce::AudioProcessorValueTreeState::ButtonAttachment lpfAttach;
    
    // Text Labels
    juce::Label titleLabel;
    juce::Label b1Label;
    juce::Label b2Label;
    juce::Label b3Label;
    juce::Label b4Label;
};