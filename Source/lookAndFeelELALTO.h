#pragma once
#include <JuceHeader.h>

class LookAndFeelELALTO : public juce::LookAndFeel_V4
{
    public:
    LookAndFeelELALTO();
    
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override;
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
        
    void drawPopupMenuItem (juce::Graphics &g, const juce::Rectangle< int > &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String &text, const juce::String &shortcutKeyText, const juce::Drawable *icon, const juce::Colour *textColour) override;
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    
    void setComboBoxParamps();
};
