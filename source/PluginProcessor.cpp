#include "PluginProcessor.h"
#include "PluginEditor.h"

// CONSTRUCTOR/DESTRUCTOR
PluginProcessor::PluginProcessor() {}
PluginProcessor::~PluginProcessor() {}

// OPERATION
void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void PluginProcessor::releaseResources() {}
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    // Clean Denormals & Unused Channels
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    // ...
}
// -----------------------------------------------------------------------------------------

// EDITOR INIT
juce::AudioProcessorEditor* PluginProcessor::createEditor()                     // Initializes editor
{
    return new PluginEditor(*this);
}
bool PluginProcessor::hasEditor() const { return true; }                        // Plugin has editor flag

// PLUGIN METADATA
const juce::String PluginProcessor::getName() const { return "TEMPLATE"; }      // Plugin name metadata
bool PluginProcessor::acceptsMidi() const { return false; }                     // Plugin has MIDI input metadata flag
bool PluginProcessor::producesMidi() const { return false; }                    // Plugin has MIDI output metadata flag
double PluginProcessor::getTailLengthSeconds() const { return 0.0; }            // Plugin runtime after input end metadata
int PluginProcessor::getNumPrograms() { return 1; }                             // Plugin program state count - Only has 1
int PluginProcessor::getCurrentProgram() { return 0; }                          // Getter for current program num - Only program is program 0
void PluginProcessor::setCurrentProgram(int) {}                                 // Implementation of switching programs - Unsupported
const juce::String PluginProcessor::getProgramName(int) { return {}; }          // Implementation of getting program name - Unsupported
void PluginProcessor::changeProgramName(int, const juce::String&) {}            // Implementation of changing program name - Unsupported

// STATE
void PluginProcessor::getStateInformation(juce::MemoryBlock&) {}                // Serializes plugin state to be saved
void PluginProcessor::setStateInformation(const void*, int) {}                  // Loads plugin state from previously saved data

// PLUGIN INSTANCE CREATOR
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}