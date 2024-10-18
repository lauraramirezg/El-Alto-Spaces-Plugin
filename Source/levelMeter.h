#pragma once

#include <JuceHeader.h>

namespace Gui
{
class levelMeter : public juce::Component
    {
        public :
        
        void paint (juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            g.setColour(juce::Colour(13,13,13).withMultipliedAlpha(0.4));
            g.fillRect(bounds);

            g.setColour(juce::Colour(242,209,179));
            
            
            ///Map level from -60 to 6
            const auto scaledX = juce::jmap(level, -50.f, +0.f, 0.f, static_cast<float>(getHeight()));
            g.fillRect(bounds.removeFromBottom(scaledX));
                
        }
    
        
        void setLevel(const float value) { level =  value; }
        
        private:
        float level = -50.0f;
    
    };
}


