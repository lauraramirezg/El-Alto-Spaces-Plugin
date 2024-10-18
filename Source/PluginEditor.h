/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "lookAndFeelELALTO.h"
#include "levelMeter.h"
#include "gradientLevelMeter.h"

//==============================================================================
/**
*/
class ELALTOSPACESAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ELALTOSPACESAudioProcessorEditor (ELALTOSPACESAudioProcessor&);
    ~ELALTOSPACESAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //My methods
    void setRoundRectangles(juce::Graphics& g,float _spaceBorders, float _spaceRectangles, float _r1Size, float _r2Size, float _r3Size, float _cornerRoundness);
    void changeSvgColor(juce::Drawable& drawable, juce::Colour newColor);
    void setRoomImg(juce::Image &roomImg, juce::Graphics &g);
    void preparePlugin();
    
    //My variables
    bool drawControl, drawLive, drawPasillo;
    
    //Arrays of objects
    juce::OwnedArray<juce::TextButton> roomsButtons;
    juce::OwnedArray<juce::Label> labelsPlugin;
    juce::OwnedArray<juce::Slider> slidersPlugin;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    juce::OwnedArray<juce::ComboBox> comboBoxPlugin;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachments;
    juce::OwnedArray<juce::StringArray> choicesComboBox;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;
    
private:
    ELALTOSPACESAudioProcessor& audioProcessor;
    LookAndFeelELALTO lookAndFeelELALTO;
    
    //Level Meters
    Gui::gradientLevelMeter inLevelMeterL, inLevelMeterR, outLevelMeterL, outLevelMeterR;
    
    //Labels Plugin
    juce::StringArray labelsString{"INPUT", "OUTPUT", "ROOMS","MICS","TYPE","POSITION", "SPEAKER", "FILTER","HIGH CUT", "LOW CUT", "DRY/WET"};
    
    //Sliders Plugin
    juce::StringArray slidersString{"levelOut", "levelIn", "lowCut","highCut","dryWet"};
    
    //ComboBox Plugin
    juce::StringArray comboBoxString{"typeMics", "posMics", "speakers"};
    
    //Rooms Plugin
    juce::StringArray roomsButtonsString{"GALVIS", "PARLANTES", "GOLI"}; //Control: Galvis, Live: Parlantes, Pasillo: Goli
    
    //State Button
    juce::TextButton stateFilter;
    
    //Types sliders
    juce::Slider::SliderStyle sliderStyle1 = juce::Slider::SliderStyle::Rotary;
    juce::Slider::SliderStyle sliderStyle2 = juce::Slider::SliderStyle::LinearBarVertical;
    
    //Colours
    juce::Colour ELALTOcolour1 = juce::Colour(13,13,13);
    juce::Colour ELALTOcolour2 = juce::Colour(242,209,179);
    juce::Colour ELALTOcolour3 = juce::Colour(38,10,10);
    
    //Images
    juce::Image background, liveImg, pasilloImg, controlImg;
    std::unique_ptr<juce::Drawable> logoDrawable;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ELALTOSPACESAudioProcessorEditor)
};
