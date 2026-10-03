#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>

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
    // AVPTS for GUI variables
    static juce::AudioProcessorValueTreeState::ParameterLayout buildLayout();
    juce::AudioProcessorValueTreeState apvts;

    
    std::atomic<int> heldBand { -1 }; // Processor's local copy of held band, -1 = none, 0-3 = bands 1-4

    // Listener for gesture event from a single band
    struct BandGesture : juce::AudioProcessorParameter::Listener
    {
        BandGesture(std::atomic<int>& heldBand_ref, int myAssignedBand) 
        : heldBand_ref(heldBand_ref), myAssignedBand(myAssignedBand) {}

        std::atomic<int>& heldBand_ref;
        int myAssignedBand;

        void parameterValueChanged(int, float) override {}  // unused but required
        void parameterGestureChanged(int, bool started) override
        {
            if (started)
                heldBand_ref.store(myAssignedBand);
            else
                heldBand_ref.store(-1);
        }
    };
    std::unique_ptr<BandGesture> listeners[4];

    // AudioParameterFloat IDs per band
    static constexpr const char* apfIDs[4][3] = {
        {"b1_freq", "b1_q", "b1_gain"}, 
        {"b2_freq", "b2_q", "b2_gain"},
        {"b3_freq", "b3_q", "b3_gain"},
        {"b4_freq", "b4_q", "b4_gain"}};
    
    
    // Smoothing vars
    struct SmoothedBand
    {
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> freq, q, gain;
    };
    SmoothedBand b1_Sm, b2_Sm, b3_Sm, b4_Sm;

    // Cached GUI params
    std::atomic<float> *b1_Freq, *b2_Freq, *b3_Freq, *b4_Freq; 
    std::atomic<float> *b1_Q, *b2_Q, *b3_Q, *b4_Q; 
    std::atomic<float> *b1_Gain, *b2_Gain, *b3_Gain, *b4_Gain; 

    std::atomic<float> *b1_HPF, *b4_LPF, *Enabled, *Audition;

    // Filter Objects
    double smpRate;
    juce::dsp::IIR::Filter<float> b1_Lfilt, b2_Lfilt, b3_Lfilt, b4_Lfilt;
    juce::dsp::IIR::Filter<float> b1_Rfilt, b2_Rfilt, b3_Rfilt, b4_Rfilt;
    juce::dsp::IIR::Filter<float> auditL, auditR;
};