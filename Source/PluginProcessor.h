/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

struct BufferWithSampleRate
{
    BufferWithSampleRate() = default;

    BufferWithSampleRate (juce::AudioBuffer<float>&& bufferIn, double sampleRateIn)
        : buffer (std::move (bufferIn)), sampleRate (sampleRateIn) {}

    juce::AudioBuffer<float> buffer;
    double sampleRate = 0.0;
};

class BufferTransfer
{
public:
    void set (BufferWithSampleRate&& p)
    {
        const juce::SpinLock::ScopedLockType lock (mutex);
        buffer = std::move (p);
        newBuffer = true;
    }

    // Call `fn` passing the new buffer, if there's one available
    template <typename Fn>
    void get (Fn&& fn)
    {
        const juce::SpinLock::ScopedTryLockType lock (mutex);

        if (lock.isLocked() && newBuffer)
        {
            fn (buffer);
            newBuffer = false;
        }
    }

private:
    BufferWithSampleRate buffer;
    bool newBuffer = false;
    juce::SpinLock mutex;
};

 
class ELALTOSPACESAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ELALTOSPACESAudioProcessor();
    ~ELALTOSPACESAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    ///============================== My methods and objetcs ================================
    juce::dsp::Convolution irConvolver; //convolution object
    BufferTransfer bufferTransfer;
    bool controlButton, liveButton, pasilloButton, filterValue;
    juce::dsp::Gain<float> inputGain,outputGain;
    juce::dsp::DryWetMixer<float> dryWetMix;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>>
        lowCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>>
        highCutFilter;
    juce::LinearSmoothedValue<float> rmsLevelLIn, rmsLevelRIn, rmsLevelLOut, rmsLevelROut;
    int speakerSelected, typeMics, posMics;
    float  lowCutFreqValue, highCutFreqValue;
    float levelInValue, levelOutValue, dryWetMixValue;
    
    std::unique_ptr<juce::InputStream> createAssetInputStream(const char* resourcePath);
    void updateParametersIR();
    void updateFilterParams();
    void updateRoomsIR();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameters()};
    float getRmsValueIn(const int channel) const;
    float getRmsValueOut(const int channel) const;
    
   
    
    
private:
    //==============================================================================
    
    juce::dsp::ProcessSpec spec; //convolution parameter object
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ELALTOSPACESAudioProcessor)
};

