#include "Editor.h"
#include "LabeledKnob.h"
#include <BinaryData.h>

Editor::Editor(Processor& p)
 : AudioProcessorEditor(&p), pRef(p),
    enabledAttach(p.apvts, "enabled", enabledButton),
    auditAttach(p.apvts, "audition", auditButton),
    hpfAttach(p.apvts, "b1_hpf", hpfButton),
    lpfAttach(p.apvts, "b4_lpf", lpfButton)
{
    const char* ids[] = { "freq", "q", "gain" };
    const char* names[] = { "Frequency", "Q", "Gain" };

    for (int band = 1; band <= 4; band++)
        for (int i = 0; i < 3; i++)
            addAndMakeVisible(knobs.add(new LabeledKnob (p.apvts, "b" + juce::String(band) + "_" + ids[i], names[i])));

    addAndMakeVisible(enabledButton);
    addAndMakeVisible(auditButton);
    addAndMakeVisible(hpfButton);
    addAndMakeVisible(lpfButton);

    // Text Labels
    titleLabel.setText("E-Cue", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(30.0f, juce::Font::bold)));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    b1Label.setText("Band 1", juce::dontSendNotification);
    b1Label.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
    b1Label.setJustificationType(juce::Justification::centred);
    b1Label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(b1Label);

    b2Label.setText("Band 2", juce::dontSendNotification);
    b2Label.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
    b2Label.setJustificationType(juce::Justification::centred);
    b2Label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(b2Label);

    b3Label.setText("Band 3", juce::dontSendNotification);
    b3Label.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
    b3Label.setJustificationType(juce::Justification::centred);
    b3Label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(b3Label);

    b4Label.setText("Band 4", juce::dontSendNotification);
    b4Label.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
    b4Label.setJustificationType(juce::Justification::centred);
    b4Label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(b4Label);

    setSize(640, 560);      // Plugin Window Size
}
Editor::~Editor() {}


void Editor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff535a5a));     // Dim teal-ish gray BG
}

void Editor::resized()
{
    auto area = getLocalBounds().reduced(20); // margin

    titleLabel.setBounds(area.removeFromTop(40));

    auto topbar = area.removeFromTop(30);
    topbar.removeFromLeft(40); topbar.removeFromRight(40); // margin

    enabledButton.setBounds(topbar.removeFromLeft(80));
    auditButton.setBounds(topbar.removeFromRight(80));
    area.removeFromTop(20); // margin
    
    area.removeFromLeft(10); area.removeFromRight(10); // margin
    const int colW = area.getWidth() / 4;
    for (int i = 0; i < 4; i++)
    {
        auto col = area.removeFromLeft(colW);

        auto labelRow = col.removeFromTop(25); // reserve for labels
        switch(i){
            case (0):
                b1Label.setBounds(labelRow); break;
            case (1):
                b2Label.setBounds(labelRow); break;
            case (2):
                b3Label.setBounds(labelRow); break;
            case (3):
                b4Label.setBounds(labelRow);
        }
        col.removeFromTop(10); // margin

        auto toggleRow = col.removeFromBottom(25); // reserve for hpf/lpf buttons
        if (i == 0) 
            hpfButton.setBounds(toggleRow.removeFromRight(95));
        if (i == 3)
            lpfButton.setBounds(toggleRow.removeFromRight(95));
        col.removeFromBottom(10); // margin

        // knobs
        const int knobHeight = col.getHeight() / 3;
        for (int j = 0; j < 3; j++)
            knobs[i * 3 + j]->setBounds(col.removeFromTop (knobHeight));
    }
}