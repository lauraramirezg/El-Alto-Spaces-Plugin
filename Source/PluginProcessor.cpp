/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ELALTOSPACESAudioProcessor::ELALTOSPACESAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
                    //lowCutFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20.0f)),
                    //highCutFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f))
#endif
{
    apvts.state = juce::ValueTree("savedParams");
    apvts.addParameterListener("levelIn", this);
    apvts.addParameterListener("levelOut", this);
    apvts.addParameterListener("dryWet", this);
    apvts.addParameterListener("filter", this);
    apvts.addParameterListener("lowCut", this);
    apvts.addParameterListener("highCut", this);
}

ELALTOSPACESAudioProcessor::~ELALTOSPACESAudioProcessor()
{
    apvts.removeParameterListener("levelIn", this);
    apvts.removeParameterListener("levelOut", this);
    apvts.removeParameterListener("dryWet", this);
    apvts.removeParameterListener("filter", this);
    apvts.removeParameterListener("lowCut", this);
    apvts.removeParameterListener("highCut", this);
}


juce::AudioProcessorValueTreeState::ParameterLayout ELALTOSPACESAudioProcessor::createParameters()
{
    ///Vector params of type RangeAudio to storage all the different parameters to use
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    ///Adds a new parameter with method push back
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "levelIn", 1 }, "Gain In", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.01f),0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "levelOut", 1 }, "Gain Out", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.01f),0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "typeMics", 1 }, "Mic Type", juce::StringArray{"DBX", "BOVA"}, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID {"posMics",1}, "Position Mic", juce::StringArray{"NEAR", "FAR"}, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID {"speakers",1}, "Type Speaker", juce::StringArray{"NS10", "OCEAN"}, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "lowCut", 1 }, "Low", juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f),30.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "highCut", 1 }, "High", juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f),20000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "dryWet", 1 }, "Dry/Wet", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),80.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"filter",1}, "Filter State", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"control",1}, "Galvis State", true));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"live",1}, "Parlantes State", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"pasillo",1}, "Goli State", false));
    
    ///Devuelve un ParameterLayour
    return {parameters.begin(), parameters.end()};
}

void ELALTOSPACESAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "levelIn")
    {
        levelInValue = newValue;
        inputGain.setGainDecibels(levelInValue);
    }
    
    if (parameterID == "levelOut")
    {
        levelOutValue = newValue;
        outputGain.setGainDecibels(levelOutValue);
    }
    
    if (parameterID == "dryWet")
    {
        dryWetMixValue = newValue;
        dryWetMix.setWetMixProportion(dryWetMixValue/100.0f);
    }
    
    if (parameterID == "filter")
    {
        filterValue = newValue;
        updateFilterParams();
    }
    if (parameterID == "lowCut")
    {
        lowCutFreqValue = newValue;
        updateFilterParams();
    }
    
    if (parameterID == "highCut")
    {
        highCutFreqValue = newValue;
        updateFilterParams();
    }
}

//==============================================================================
const juce::String ELALTOSPACESAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ELALTOSPACESAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ELALTOSPACESAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ELALTOSPACESAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ELALTOSPACESAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ELALTOSPACESAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ELALTOSPACESAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ELALTOSPACESAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ELALTOSPACESAudioProcessor::getProgramName (int index)
{
    return {};
}

void ELALTOSPACESAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ELALTOSPACESAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ///Initialize parameters with saved values
    controlButton = *apvts.getRawParameterValue("control");
    liveButton = *apvts.getRawParameterValue("live");
    pasilloButton = *apvts.getRawParameterValue("pasillo");
    
    speakerSelected = *apvts.getRawParameterValue("speakers");
    typeMics = *apvts.getRawParameterValue("typeMics");
    posMics =  *apvts.getRawParameterValue("posMics");
    
    levelInValue = *apvts.getRawParameterValue("levelIn");
    levelOutValue = *apvts.getRawParameterValue("levelOut");
    dryWetMixValue = *apvts.getRawParameterValue("dryWet");
    
    filterValue = *apvts.getRawParameterValue("filter");
    lowCutFreqValue = *apvts.getRawParameterValue("lowCut");
    highCutFreqValue = *apvts.getRawParameterValue("highCut");
    
    ///Initialize methods with saved values
    updateParametersIR();
    updateFilterParams();
    inputGain.setGainDecibels(levelInValue);
    outputGain.setGainDecibels(levelOutValue);

    ///Inicializing parameters of object spec
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    ///Inicializing dsp objects
    irConvolver.prepare(spec);
    irConvolver.reset();
    inputGain.prepare(spec);
    inputGain.reset();
    outputGain.prepare(spec);
    outputGain.reset();
    dryWetMix.prepare(spec);
    dryWetMix.reset();
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    highCutFilter.prepare(spec);
    highCutFilter.reset();
    
    ///Inicializing RMS values with smoothing
    rmsLevelLIn.reset(sampleRate, 0.5);
    rmsLevelRIn.reset(sampleRate, 0.5);
    rmsLevelLOut.reset(sampleRate, 0.5);
    rmsLevelROut.reset(sampleRate, 0.5);
    rmsLevelLIn.setCurrentAndTargetValue(-100.f);
    rmsLevelRIn.setCurrentAndTargetValue(-100.f);
    rmsLevelLOut.setCurrentAndTargetValue(-100.f);
    rmsLevelROut.setCurrentAndTargetValue(-100.f);
}

void ELALTOSPACESAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ELALTOSPACESAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ELALTOSPACESAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
        
    ///Loads a new IR if it changes
    bufferTransfer.get ([this] (BufferWithSampleRate& buf)
    {
        irConvolver.loadImpulseResponse (std::move (buf.buffer),
                                         buf.sampleRate,
                                         juce::dsp::Convolution::Stereo::yes,
                                         juce::dsp::Convolution::Trim::yes,
                                         juce::dsp::Convolution::Normalise::yes);
    });
    

    ///Process the incoming audio block called buffer and performs convolution with the IR selected and replace de audio block out
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    inputGain.process(context);
    
    ///Input Level Meter
    rmsLevelLIn.skip(buffer.getNumSamples());
    rmsLevelRIn.skip(buffer.getNumSamples());

    const auto valueInL = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    
    if (valueInL < rmsLevelLIn.getCurrentValue())
        rmsLevelLIn.setTargetValue(valueInL);
    else
        rmsLevelLIn.setCurrentAndTargetValue(valueInL);
    
    const auto valueInR = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (valueInR < rmsLevelRIn.getCurrentValue())
        rmsLevelRIn.setTargetValue(valueInR);
    else
        rmsLevelRIn.setCurrentAndTargetValue(valueInR);
    
    dryWetMix.pushDrySamples(block);
    irConvolver.process(context);
    lowCutFilter.process(context);
    highCutFilter.process(context);
    dryWetMix.mixWetSamples(block);
    outputGain.process(context);
    
    ///Output Level Meter
    rmsLevelLOut.skip(buffer.getNumSamples());
    rmsLevelROut.skip(buffer.getNumSamples());

    const auto valueOutL = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (valueOutL < rmsLevelLOut.getCurrentValue())
        rmsLevelLOut.setTargetValue(valueOutL);
    else
        rmsLevelLOut.setCurrentAndTargetValue(valueOutL);
    
    
    const auto valueOutR = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (valueOutR < rmsLevelROut.getCurrentValue())
        rmsLevelROut.setTargetValue(valueOutR);
    else
        rmsLevelROut.setCurrentAndTargetValue(valueOutR);
}

//==============================================================================
bool ELALTOSPACESAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ELALTOSPACESAudioProcessor::createEditor()
{
    return new ELALTOSPACESAudioProcessorEditor (*this);
}

//==============================================================================
void ELALTOSPACESAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    std::unique_ptr <juce::XmlElement> xml (apvts.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ELALTOSPACESAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr <juce::XmlElement> paramsSaved (getXmlFromBinary(data, sizeInBytes));
    
    if (paramsSaved != nullptr)
    {
        if (paramsSaved -> hasTagName(apvts.state.getType())) 
        {
            apvts.state = juce::ValueTree::fromXml(*paramsSaved);
        }
    }
}

std::unique_ptr<juce::InputStream> ELALTOSPACESAudioProcessor::createAssetInputStream(const char *resourcePath)
{
    #if JUCE_MAC
//        auto irsDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getChildFile("Source").getChildFile("Assets").getChildFile("IRs");
    
        auto irsDir = juce::File::getSpecialLocation (juce::File::userDesktopDirectory).getChildFile("IRs");
        
        juce::File resourceFile;
        
        if (irsDir.exists() == true)
        {
            resourceFile = irsDir.getChildFile (resourcePath);
        }
        else
        {
            jassert (resourceFile.existsAsFile());
        }
        return resourceFile.createInputStream();
    #endif
    
    ///This function creates and inputStream for a input file everytime its called
}

void ELALTOSPACESAudioProcessor::updateParametersIR()
{
    irConvolver.reset();
    
    ///Gets Parameter Value Changes From Rooms
    speakerSelected = *apvts.getRawParameterValue("speakers");
    typeMics = *apvts.getRawParameterValue("typeMics");
    posMics =  *apvts.getRawParameterValue("posMics");
    
    controlButton = *apvts.getRawParameterValue("control");
    liveButton = *apvts.getRawParameterValue("live");
    pasilloButton = *apvts.getRawParameterValue("pasillo");
    
    const char* assetName;
    
    ///Control Parameters
    if (controlButton == true)
    {
        switch (speakerSelected)
        {
            case 0:
                assetName = "IR_NS10.wav";
                break;
            case 1:
                assetName = "IR_OCEAN.wav";
                break;
            default:
                jassertfalse;
                break;
        }
    }
    
    ///Live Parameters
    if (liveButton == true)
    {
        switch (typeMics)
        {
            case 0:
                switch (posMics)
                {
                    case 0:
                        assetName = "IR_DBXLive.wav";
                        break;
                    case 1:
                        assetName = "IR_DBXLive_Corner.wav";
                    default:
                        break;
                }
                break;
            case 1:
                switch (posMics)
                {
                    case 0:
                        assetName = "IR_BovaLive.wav";
                        break;
                    case 1:
                        assetName = "IR_BovaLive_Corner.wav";
                    default:
                        break;
                }
            default:
                break;
        }
        
    }
        
    ///Pasillo Parameters
    if(pasilloButton == true)
    {
        switch (posMics)
        {
            case 0:
                assetName = "IR_Pasillo_Entrada.wav";
                break;
            case 1:
                assetName = "IR_Pasillo_Escaleras.wav";
                break;
            default:
                jassertfalse;
                break;
        }
    }
    
    ///Creats InputStream
    auto assetInputStream = createAssetInputStream (assetName);
    
    if (assetInputStream == nullptr)
    {
        jassertfalse;
        return;
    }
    
    juce::AudioFormatManager manager;
    manager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader {manager.createReaderFor(std::move (assetInputStream))};

    if (reader == nullptr)
    {
        jassertfalse;
        return;
    }

    juce::AudioBuffer<float> buffer (static_cast<int> (reader->numChannels),
                               static_cast<int> (reader->lengthInSamples));
    
    reader->read(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), 0, buffer.getNumSamples());
    bufferTransfer.set (BufferWithSampleRate { std::move (buffer), reader->sampleRate });
}

void ELALTOSPACESAudioProcessor::updateFilterParams()
{
    const float sampleRate = this->getSampleRate();
    
//    lowCutFreqValue = *apvts.getRawParameterValue("lowCut");
//    highCutFreqValue = *apvts.getRawParameterValue("highCut");
//    filterValue = *apvts.getRawParameterValue("filter");
    
    if (filterValue == true)
    {
        *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowCutFreqValue);
        *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, highCutFreqValue);
    }
    else if (filterValue == false)
    {
        *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
        *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 20000.0f);
    }
    
}

float ELALTOSPACESAudioProcessor::getRmsValueIn(const int channel) const
{
    if(channel == 0)
        return rmsLevelLIn.getCurrentValue();
    if(channel == 1)
        return rmsLevelRIn.getCurrentValue();
    return 0.f;
}

float ELALTOSPACESAudioProcessor::getRmsValueOut(const int channel) const
{
    if(channel == 0)
        return rmsLevelLOut.getCurrentValue();
    if(channel == 1)
        return rmsLevelROut.getCurrentValue();
    return 0.f;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ELALTOSPACESAudioProcessor();
}


