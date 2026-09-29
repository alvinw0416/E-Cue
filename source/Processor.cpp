#include "Processor.h"
#include "Editor.h"

Processor::Processor() 
    : apvts(*this, nullptr, "PARAMETERS", buildLayout()),   // AVPTS init for GUI variables
        b1_Freq(apvts.getRawParameterValue("b1_freq")), b1_Q(apvts.getRawParameterValue("b1_q")), b1_Gain(apvts.getRawParameterValue("b1_gain")),
        b2_Freq(apvts.getRawParameterValue("b2_freq")), b2_Q(apvts.getRawParameterValue("b2_q")), b2_Gain(apvts.getRawParameterValue("b2_gain")),
        b3_Freq(apvts.getRawParameterValue("b3_freq")), b3_Q(apvts.getRawParameterValue("b3_q")), b3_Gain(apvts.getRawParameterValue("b3_gain")),
        b4_Freq(apvts.getRawParameterValue("b4_freq")), b4_Q(apvts.getRawParameterValue("b4_q")), b4_Gain(apvts.getRawParameterValue("b4_gain")),
        b1_HPF(apvts.getRawParameterValue("b1_hpf")), b4_LPF(apvts.getRawParameterValue("b4_lpf")),
        Enabled(apvts.getRawParameterValue("enabled")), Audition(apvts.getRawParameterValue("audition")) 
{
    // Add a listener to each parameter APF
    for (int i = 0; i < 4; i++)
    {
        gest_listeners[i] = std::make_unique<BandGesture>(heldBand, i);
        for (auto* id : apfIDs[i])
            apvts.getParameter(id)->addListener(gest_listeners[i].get());
    }
}
Processor::~Processor() 
{
    // Remove all gesture listeners from APFs
    for (int i = 0; i < 4; i++)
        for (auto* id : apfIDs[i])
            apvts.getParameter(id)->removeListener(gest_listeners[i].get());
}

void Processor::prepareToPlay(double sampleRate, int samplesPerBlock) 
{
    smpRate = sampleRate;   // Cache sample rate

    // Filter prep
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate; spec.maximumBlockSize = (juce::uint32) samplesPerBlock; spec.numChannels = 1;

    b1_Lfilt.prepare(spec); b1_Rfilt.prepare(spec);
    b2_Lfilt.prepare(spec); b2_Rfilt.prepare(spec);
    b3_Lfilt.prepare(spec); b3_Rfilt.prepare(spec);
    b4_Lfilt.prepare(spec); b4_Rfilt.prepare(spec);
    auditL.prepare(spec); auditR.prepare(spec);

    // Parameter smoothening prep
    for (auto* x : { &b1_Sm, &b2_Sm, &b3_Sm, &b4_Sm })
    {
        x->freq.reset(sampleRate, 0.05);
        x->q.reset(sampleRate, 0.05);
        x->gain.reset(sampleRate, 0.05);
    }

    b1_Sm.freq.setCurrentAndTargetValue(b1_Freq->load());
    b2_Sm.freq.setCurrentAndTargetValue(b2_Freq->load());
    b3_Sm.freq.setCurrentAndTargetValue(b3_Freq->load());
    b4_Sm.freq.setCurrentAndTargetValue(b4_Freq->load());

    b1_Sm.q.setCurrentAndTargetValue(b1_Q->load());
    b2_Sm.q.setCurrentAndTargetValue(b2_Q->load());
    b3_Sm.q.setCurrentAndTargetValue(b3_Q->load());
    b4_Sm.q.setCurrentAndTargetValue(b4_Q->load());

    b1_Sm.gain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(b1_Gain->load()));
    b2_Sm.gain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(b2_Gain->load()));
    b3_Sm.gain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(b3_Gain->load()));
    b4_Sm.gain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(b4_Gain->load()));
}
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
    if (Enabled->load() < 0.5f) // Skip processing if EQ not enabled
        return;

    // Set target for smoothed values for this buffer
    b1_Sm.freq.setTargetValue(b1_Freq->load());
    b2_Sm.freq.setTargetValue(b2_Freq->load());
    b3_Sm.freq.setTargetValue(b3_Freq->load());
    b4_Sm.freq.setTargetValue(b4_Freq->load());
    b1_Sm.q.setTargetValue(b1_Q->load());
    b2_Sm.q.setTargetValue(b2_Q->load());
    b3_Sm.q.setTargetValue(b3_Q->load());
    b4_Sm.q.setTargetValue(b4_Q->load());
    b1_Sm.gain.setTargetValue(juce::Decibels::decibelsToGain(b1_Gain->load()));
    b2_Sm.gain.setTargetValue(juce::Decibels::decibelsToGain(b2_Gain->load()));
    b3_Sm.gain.setTargetValue(juce::Decibels::decibelsToGain(b3_Gain->load()));
    b4_Sm.gain.setTargetValue(juce::Decibels::decibelsToGain(b4_Gain->load()));

    // Audition, poll currently held band, if any
    int blockheld = -1;
    if (Audition->load() > 0.5f)
        blockheld = heldBand.load();
    else
        blockheld = -1;

    ParamSmoother* sm[4] = { &b1_Sm, &b2_Sm, &b3_Sm, &b4_Sm };   // allows us to index by band number for audition

    // Processing, by chunks:
    for (int cursmp = 0; cursmp < buffer.getNumSamples(); cursmp += 32)
    {
        int chunksize = std::min(32, buffer.getNumSamples() - cursmp);

        // Calc coeffs for this chunk
        if (b1_HPF->load() > 0.5f) 
        {   // HPF mode
            b1_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(smpRate, b1_Sm.freq.skip(chunksize), b1_Sm.q.skip(chunksize));
            b1_Rfilt.coefficients = b1_Lfilt.coefficients; 
            b1_Sm.gain.skip(chunksize); // advance regardless of used or not
        }
        else
        {   // Peak Mode
            b1_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(smpRate, b1_Sm.freq.skip(chunksize), b1_Sm.q.skip(chunksize), b1_Sm.gain.skip(chunksize));
            b1_Rfilt.coefficients = b1_Lfilt.coefficients;
        }

        if (b4_LPF->load() > 0.5f) 
        {   // LPF mode
            b4_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(smpRate, b4_Sm.freq.skip(chunksize), b4_Sm.q.skip(chunksize));
            b4_Rfilt.coefficients = b4_Lfilt.coefficients;
            b4_Sm.gain.skip(chunksize); // advance regardless of used or not
        }
        else
        {   // Peak Mode
            b4_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(smpRate, b4_Sm.freq.skip(chunksize), b4_Sm.q.skip(chunksize), b4_Sm.gain.skip(chunksize));
            b4_Rfilt.coefficients = b4_Lfilt.coefficients;
        }

        b2_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(smpRate, b2_Sm.freq.skip(chunksize), b2_Sm.q.skip(chunksize), b2_Sm.gain.skip(chunksize));
        b2_Rfilt.coefficients = b2_Lfilt.coefficients;
        b3_Lfilt.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(smpRate, b3_Sm.freq.skip(chunksize), b3_Sm.q.skip(chunksize), b3_Sm.gain.skip(chunksize));
        b3_Rfilt.coefficients = b3_Lfilt.coefficients;

        // Process samples for this chunk
        
        // Left channel
        auto* wr_ptr = buffer.getWritePointer(0); 
        for (int i = cursmp; i < cursmp + chunksize; i++)
        {
            wr_ptr[i] = b1_Lfilt.processSample(wr_ptr[i]);
            wr_ptr[i] = b2_Lfilt.processSample(wr_ptr[i]);
            wr_ptr[i] = b3_Lfilt.processSample(wr_ptr[i]);
            wr_ptr[i] = b4_Lfilt.processSample(wr_ptr[i]);
        }

        // Right channel
        if (totalNumInputChannels > 1)
        {
            wr_ptr = buffer.getWritePointer(1); 
            for (int i = cursmp; i < cursmp + chunksize; i++)
            {
                wr_ptr[i] = b1_Rfilt.processSample(wr_ptr[i]);
                wr_ptr[i] = b2_Rfilt.processSample(wr_ptr[i]);
                wr_ptr[i] = b3_Rfilt.processSample(wr_ptr[i]);
                wr_ptr[i] = b4_Rfilt.processSample(wr_ptr[i]);
            }
        }

        // Audition bandpass after everything else
        if (blockheld >= 0)
        {
            auto& s = *sm[blockheld];
            auditL.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(smpRate, s.freq.getCurrentValue(), s.q.getCurrentValue());
            auditR.coefficients = auditL.coefficients;

            // Left channel
            auto* wr_ptr = buffer.getWritePointer(0);
            for (int i = cursmp; i < cursmp + chunksize; i++)
                wr_ptr[i] = auditL.processSample(wr_ptr[i]);

            // Right channel
            if (totalNumInputChannels > 1)
            {
                auto* wr_ptr = buffer.getWritePointer(1);
                for (int i = cursmp; i < cursmp + chunksize; i++)
                    wr_ptr[i] = auditR.processSample(wr_ptr[i]);
            }
        }   
    }

    // ------------------------------------------------------- Here ENDS processor implementation
}


juce::AudioProcessorEditor* Processor::createEditor()
{
    return new Editor(*this);
}
bool Processor::hasEditor() const { return true; }


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
void Processor::getStateInformation(juce::MemoryBlock& destDAWmem)
{
    std::unique_ptr<juce::XmlElement> p_xmltree = apvts.copyState().createXml();
    copyXmlToBinary(*p_xmltree, destDAWmem);
}                
void Processor::setStateInformation(const void* p_data, int bytesize)
{
    std::unique_ptr<juce::XmlElement> p_xmltree = getXmlFromBinary(p_data, bytesize);

    if (p_xmltree != nullptr)
        if (p_xmltree->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*p_xmltree));
}                  


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}


// APVTS Layout Implementation
juce::AudioProcessorValueTreeState::ParameterLayout Processor::buildLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;     // init layout obj

    // Knobs' range, fidelity, and skew
    juce::NormalisableRange<float> freqRange (20.0f, 20000.0f, 1.0f); freqRange.setSkewForCentre (1000.0f);
    juce::NormalisableRange<float> qRange (0.1f, 10.0f, 0.01f); qRange.setSkewForCentre (1.0f);
    juce::NormalisableRange<float> gainRange (-20.0f, 20.0f, 0.1f);


    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b1_freq", 1 },       // ID
        "Band 1 Freq",                           // display name in automation
        freqRange,                               // range
        100.0f                                   // default value
        ));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b2_freq", 1 },
        "Band 2 Freq", freqRange, 500.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b3_freq", 1 },
        "Band 3 Freq", freqRange, 3000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b4_freq", 1 },
        "Band 4 Freq", freqRange, 10000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b1_q", 1 },
        "Band 1 Q", qRange, 0.707f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b2_q", 1 },
        "Band 2 Q", qRange, 0.707f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b3_q", 1 },
        "Band 3 Q", qRange, 0.707f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b4_q", 1 },
        "Band 4 Q", qRange, 0.707f));

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b1_gain", 1 },
        "Band 1 Gain", gainRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b2_gain", 1 },
        "Band 2 Gain", gainRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b3_gain", 1 },
        "Band 3 Gain", gainRange, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "b4_gain", 1 },
        "Band 4 Gain", gainRange, 0.0f));


    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "enabled", 1 },     // ID
        "Enabled",                              // display name
        true));                                 // default bool
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "audition", 1 },
        "Auditioning", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "b1_hpf", 1 },
        "Band 1 HPF", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "b4_lpf", 1 },
        "Band 4 LPF", false));

    return layout;
}