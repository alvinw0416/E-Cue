#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class Processor : public juce::AudioProcessor 
{
public:
    Processor();
    ~Processor() override;

    // Operation
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
        
    // Editor Initialization
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
        
    // Metadata
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    // State
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    // ------------------------------------------------------- Here starts other members/methods
    //
};