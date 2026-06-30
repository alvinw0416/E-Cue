#include "PluginEditor.h"

// CONSTRUCTOR/DESTRUCTOR
PluginEditor::PluginEditor(PluginProcessor& p)
 : AudioProcessorEditor(&p), pRef(p)
{
    setSize(400, 300);
}
PluginEditor::~PluginEditor() {}

// MANUAL DRAWINGS
void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

// COMPONENTS & RESIZING
void PluginEditor::resized() 
{}