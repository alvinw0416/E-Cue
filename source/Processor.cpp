#include "Processor.h"
#include "Editor.h"

Processor::Processor() {}
Processor::~Processor() {}

// Operation
void Processor::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void Processor::releaseResources() {}
void Processor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    // Clean Denormals & Unused Channels
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    // ------------------------------------------------------- Here STARTS processor implementation
    
    // ------------------------------------------------------- Here END processor implementation
}

// Editor Initilialization
juce::AudioProcessorEditor* Processor::createEditor()
{
    return new Editor(*this);
}
bool Processor::hasEditor() const { return true; }

// Metadata
const juce::String Processor::getName() const { return "TEMPLATE"; }      // Plugin name metadata
bool Processor::acceptsMidi() const { return false; }                     // Plugin has MIDI input metadata flag
bool Processor::producesMidi() const { return false; }                    // Plugin has MIDI output metadata flag
double Processor::getTailLengthSeconds() const { return 0.0; }            // Plugin runtime after input end metadata
int Processor::getNumPrograms() { return 1; }                             // Plugin program state count - Only has 1
int Processor::getCurrentProgram() { return 0; }                          // Getter for current program num - Only program is program 0
void Processor::setCurrentProgram(int) {}                                 // Implementation of switching programs - Unsupported
const juce::String Processor::getProgramName(int) { return {}; }          // Implementation of getting program name - Unsupported
void Processor::changeProgramName(int, const juce::String&) {}            // Implementation of changing program name - Unsupported

// State
void Processor::getStateInformation(juce::MemoryBlock&) {}                // Serializes plugin state to be saved
void Processor::setStateInformation(const void*, int) {}                  // Loads plugin state from previously saved data

// Plugin Instance Creation
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}