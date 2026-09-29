#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class LabeledKnob : public juce::Component 
{
public:
    LabeledKnob(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& name)
        : attachment(apvts, paramID, slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(slider);
        addAndMakeVisible(label);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        label.setBounds(area.removeFromTop (18));
        slider.setBounds(area);
    }
private:
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};