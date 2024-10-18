#include "lookAndFeelELALTO.h"

LookAndFeelELALTO::LookAndFeelELALTO()
{
    //Select typography to use for all the plugin
    setDefaultSansSerifTypefaceName("Andale Mono");
    setColour(juce::Label::textColourId, juce::Colour(242,209,179));
    
    //Change colour of text to all ButtonText
    setColour(juce::TextButton::textColourOffId, juce::Colour(242,209,179));
    setColour(juce::TextButton::buttonColourId,  juce::Colours::transparentBlack);
    setColour (juce::TextButton::buttonOnColourId, juce::Colour(13,13,13));
    setColour(juce::TextButton::textColourOnId, juce::Colour(242,209,179));
  
    //Change colours of all ComboBox objects
    setComboBoxParamps();
    
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(242,209,179));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
    
void LookAndFeelELALTO::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown)
    {
        auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
            .withMultipliedAlpha (button.isEnabled() ? 0.9f : 0.5f);
    
        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.1f);
    
        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();
    
        auto width  = (float) button.getWidth()  - 1.0f;
        auto height = (float) button.getHeight() - 1.0f;
    
        if (width > 0 && height > 0)
        {
            
            auto cornerSize = fmin (12.0f, fmin (width, height) * 0.45f);
            auto lineThickness = cornerSize    * 0.1f;
            auto halfThickness = lineThickness * 0.5f;
            
            juce::Path outline;
            outline.addRoundedRectangle (0.5f + halfThickness, 0.5f + halfThickness, width - lineThickness, height - lineThickness,
                                         cornerSize, cornerSize,
                                         ! (flatOnLeft  || flatOnTop),
                                         ! (flatOnRight || flatOnTop),
                                         ! (flatOnLeft  || flatOnBottom),
                                         ! (flatOnRight || flatOnBottom));
            //Draws and paint round rectangle
            g.setColour (baseColour);
            g.fillPath (outline);
            
            //Draws and paint outline to round rectangles
            juce::Colour outlineColour = juce::Colour(242,209,179);
            g.setColour (outlineColour);
            g.strokePath (outline, juce::PathStrokeType (lineThickness));

        }
        //button.setClickingTogglesState(true);
    }
    
    void LookAndFeelELALTO::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        slider.setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);

        if (style == juce::Slider::LinearBar || style == juce::Slider::LinearBarVertical)
        {
            juce::Path p;

            if (style == juce::Slider::LinearBarVertical)
                p.addRectangle ((float) x, sliderPos, (float) width, 1.0f + (float) height - sliderPos);
            else
                p.addRectangle ((float) x, (float) y, sliderPos - (float) x, (float) height);
            
            auto baseColour = juce::Colour(242,209,179);
            g.setColour (baseColour);
            g.fillPath (p);

            auto lineThickness = fmin (15.0f, (float) fmin (width, height) * 0.45f) * 0.1f;
            g.drawRect (slider.getLocalBounds().toFloat(), lineThickness);
        }
        else
        {
            drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
            drawLinearSliderThumb      (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }
    
    void LookAndFeelELALTO::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
    {
        auto diameter = (float) fmin (width, height) - 4.0f;
        auto radius = (diameter / 2.0f) * std::cos (juce::MathConstants<float>::pi / 4.0f);
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
        auto baseColour = juce::Colour(249,209,179);
        juce::Rectangle<float> r (rx, ry, rw, rw);
        auto transform = juce::AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY());
        
        auto x1 = r.getTopLeft()   .getX();
        auto y1 = r.getTopLeft()   .getY();
        auto x2 = r.getBottomLeft().getX();
        auto y2 = r.getBottomLeft().getY();

        transform.transformPoints (x1, y1, x2, y2);
        g.setGradientFill (juce::ColourGradient (baseColour, x1, y1,
                                           baseColour.darker (0.1f), x2, y2,
                                           false));

        juce::Path knob;
        knob.addEllipse(r);
        g.setColour(baseColour);
        g.strokePath(knob, juce::PathStrokeType(2.0f));

        juce::Path needle;
        auto needleColour = juce::Colour(249,209,179);
        juce::Rectangle<float> r2 (rx, ry, rw*0.1f, radius);
        needle.addRectangle (r2.withPosition ({ r.getCentreX() - (r2.getWidth() / 1.0f), r.getY() }));
        g.setColour(needleColour);
        g.fillPath (needle, juce::AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY()));
    }
        
    void LookAndFeelELALTO::drawPopupMenuItem (juce::Graphics &g, const juce::Rectangle< int > &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String &text, const juce::String &shortcutKeyText, const juce::Drawable *icon, const juce::Colour *textColour)
    {
        //Change bacround colour
        juce:: Colour backgroundColour = isHighlighted ? juce::Colour(13,13,13) : juce::Colour(249,209,179);
        
        //Change text colour
        juce::Colour customTextColour = isHighlighted ? juce::Colour(249,209,179) : juce::Colour(13,13,13);
        
        //Draws Popmenu itme background
        g.setColour(backgroundColour);
        g.fillRect(area.toFloat());
        
        //Draws text of Popmenu item
        g.setColour(customTextColour);
        g.drawFittedText(text, area.reduced(3), juce::Justification::centredLeft, 1);
    }

    void LookAndFeelELALTO::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
    {
        //Draws popmenu background
        g.setColour(juce::Colour(249,209,179));
        g.fillRect(0, 0, width, height);
    }

    void LookAndFeelELALTO::setComboBoxParamps()
    {
        //Changes ComboBox colours
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentWhite);
        setColour(juce::ComboBox::outlineColourId, juce::Colour(249,209,179));
        setColour(juce::ComboBox::arrowColourId, juce::Colour(249,209,179));
        setColour(juce::ComboBox::textColourId, juce::Colour(249,209,179));
    }
