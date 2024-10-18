#pragma once

#include <JuceHeader.h>

namespace Gui
{
    class gradientLevelMeter : public juce::Component, public juce::Timer
    {
    public :
        
        gradientLevelMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
        {
            startTimer(24);
        }
        void paint (juce::Graphics& g) override
        {
            const auto level = valueSupplier();
            
            auto bounds = getLocalBounds().toFloat();
            
            g.setColour(juce::Colour(13,13,13).withMultipliedAlpha(0.4));
            g.fillRect(bounds);
            
            g.setGradientFill(gradient);
            
            const auto scaledY = juce::jmap(level, -60.f, +2.f, 0.f, static_cast<float>(getHeight()));
            g.fillRect(bounds.removeFromBottom(scaledY));
            
        }
        
        void resized() override
        {
            const auto bounds = getLocalBounds().toFloat();
            gradient  = juce::ColourGradient {juce::Colour(242,209,179), bounds.getBottomLeft(), juce::Colour(166,68,68), bounds.getTopLeft(), false};
            gradient.addColour(0.5, juce::Colour(242,170, 107));
        }
        
        void timerCallback() override
        {
            repaint();
        }
        
    private:
        std::function<float()> valueSupplier;
        juce::ColourGradient gradient;
    };
}


