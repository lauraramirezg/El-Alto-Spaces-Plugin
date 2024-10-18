/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ELALTOSPACESAudioProcessorEditor::ELALTOSPACESAudioProcessorEditor (ELALTOSPACESAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), inLevelMeterL([&]() {return audioProcessor.getRmsValueIn(0); }), inLevelMeterR([&]() {return audioProcessor.getRmsValueIn(1); }), outLevelMeterL([&]() {return audioProcessor.getRmsValueOut(0); }), outLevelMeterR([&]() {return audioProcessor.getRmsValueOut(1); })
{
    //Calls PreparePlugin method
    preparePlugin();
    
    //Control onClick method
    roomsButtons[0]->onClick = [this]
    {
        // Flags for image display
        drawControl = true;
        drawLive = false;
        drawPasillo = false;
        
        // Enables/disables options
        comboBoxPlugin[0]->setEnabled(false);
        comboBoxPlugin[1]->setEnabled(false);
        comboBoxPlugin[2]->setEnabled(true);
        
        // Flags rooms buttons
        if (roomsButtons[0]->getToggleState())
        {
            // Update IR params
            audioProcessor.updateParametersIR();
        }
    };
    
    
    //Live onClick method
    roomsButtons[1]->onClick = [this]
    {
        //Flags for image display
        drawControl = false;
        drawLive = true;
        drawPasillo = false;

        //Enables/disables options
        comboBoxPlugin[0]->setEnabled(true);
        comboBoxPlugin[1]->setEnabled(true);
        comboBoxPlugin[2]->setEnabled(false);
        
        //Flags rooms buttons
        if (roomsButtons[1]->getToggleState())
        {
            // Update IR params
            audioProcessor.updateParametersIR();
        }
    };
    
    //Pasillo onClick method
    roomsButtons[2]->onClick = [this]
    {
        //Flags for image display
        drawControl = false;
        drawLive = false;
        drawPasillo = true;

        //Enables/disables options
        comboBoxPlugin[0]->setEnabled(false);
        comboBoxPlugin[1]->setEnabled(true);
        comboBoxPlugin[2]->setEnabled(false);
        
        //Flags rooms buttons
        if (roomsButtons[2]->getToggleState())
        {
            // Update IR params
            audioProcessor.updateParametersIR();
        }
        
    };
    
    ///Changes IR Control
    comboBoxPlugin[2]->onChange = [this]
    {
        audioProcessor.updateParametersIR();
    };
    
    ///Changes IR Live
    comboBoxPlugin[0]->onChange = [this]
    {
        audioProcessor.updateParametersIR();
    };
    
    comboBoxPlugin[1]->onChange = [this] ///Changes IR Passillo
    {
         audioProcessor.updateParametersIR();
    };
    
//    ///Updates FILTER state
//    stateFilter.onClick = [this]
//    {
//        audioProcessor.filterState = stateFilter.getToggleState();
//    };
//    
    setSize (800, 700);
}

ELALTOSPACESAudioProcessorEditor::~ELALTOSPACESAudioProcessorEditor()
{
}

//==============================================================================

void ELALTOSPACESAudioProcessorEditor::paint (juce::Graphics& g)
{
    ///Draws backroung image
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
    
    ///Redraws room image as selected
    if (drawControl)
        setRoomImg(controlImg, g);
    else if (drawLive)
        setRoomImg(liveImg, g);
    else if (drawPasillo)
        setRoomImg(pasilloImg, g);
    
    ///Draws Logo
    changeSvgColor(*logoDrawable, ELALTOcolour2);
    const auto logoX = getWidth()*(0.725f);
    const auto logoY = getHeight()*(0.8f);
    const auto logoW = getWidth()*(0.08);
    const auto logoH = getHeight()*(0.075);
    juce::Rectangle<float> destArea(logoX, logoY, logoW, logoH);
    juce::RectanglePlacement placement(juce::RectanglePlacement::fillDestination);
    logoDrawable->drawWithin(g, destArea, placement, 1.0f);
        
    ///Draws round rectangles
    float cornerRoundness = 10.0;
    float spaceBorders = 0.03; //as percentage
    float spaceRectangles = 0.02; //as percentage
    float r1Size = 0.45; //size of rectangle #1 as percentage
    float r2Size = 0.25; //size of rectangle #2 as percentage
    float r3Size = 0.3; //size of rectangle #3 as percentage
    setRoundRectangles(g, spaceBorders, spaceRectangles, r1Size, r2Size, r3Size, cornerRoundness);
}

void ELALTOSPACESAudioProcessorEditor::resized()
{
    ///Adds dimensions to in/out objects
    const auto labelX = getWidth()*0.095;
    const auto labelY = getHeight()*0.05;
    const auto labelW = 60;
    const auto labelH = 20;
    
    labelsPlugin[0]->setBounds(labelX, labelY, labelW, labelH);
    labelsPlugin[1]->setBounds(getWidth()-(getWidth()*0.086)-labelW, labelY,labelW, labelH);

    const auto sliderX = getWidth()* (0.83);
    const auto sliderY = getHeight()* (0.28);
    const auto sliderW = 30*2.3;
    const auto sliderH = 30*3.4;

    slidersPlugin[0]->setBounds(sliderX,sliderY,sliderW,sliderH);
    slidersPlugin[1]->setBounds(getWidth()-(sliderX)-sliderW,sliderY,sliderW,sliderH);
    
    const auto levelX = getWidth() * (0.104);
    const auto levelY = getHeight() * (0.088);
    const auto levelW = getWidth() * (0.02);
    const auto levelH = getHeight() * (0.205);
    const auto levelDeltaX = levelX * (8.19f);
    
    inLevelMeterL.setBounds(levelX, levelY, levelW, levelH);
    inLevelMeterR.setBounds(levelX + 20, levelY, levelW, levelH);
    outLevelMeterL.setBounds(levelDeltaX, levelY, levelW, levelH);
    outLevelMeterR.setBounds(levelDeltaX + 20, levelY, levelW, levelH);
    

    ///Adds dimension to room objects
    const auto labelRoomX = (getWidth() * 0.51f) - (labelW/2);
    const auto labelRoomY = (getHeight()/2) - (labelH/2);
    labelsPlugin[2]->setBounds(labelRoomX, labelRoomY, labelW, labelH);
    
    ///Rooms buttons
    auto area = getLocalBounds().reduced (50);
    auto row = area.removeFromTop (35);
    row.setX(getWidth()*(0.32f));
    row.setY(getHeight()*(0.55f));
    int spacing = getWidth()*(0.015f); // Change this value to set the desired spacing
    
    for (auto* b : roomsButtons)
    {
        //b->setBounds (row.removeFromLeft (90));
        auto buttonArea = row.removeFromLeft(90);
        b->setBounds(buttonArea);
        row.reduce(spacing, 0); // Add spacing after setting each button's bounds
    }

    ///Adds dimensions to mic objects section
    const auto micsLX = getWidth()*(0.139);
    const auto micsLY = getHeight()*(0.47);
    const auto micsLH = getHeight()*(0.06);
    const auto micsLW = getWidth()*(0.06);
    
    labelsPlugin[3]->setBounds(micsLX, micsLY, micsLW, micsLH);

    const auto typesLX = getWidth()*(0.05);
    const auto typesLY = getHeight()*(0.545);
    const auto typesLH = getHeight()*(0.06);
    const auto typesLW = getWidth()*(0.06);
    
    labelsPlugin[4]->setBounds(typesLX, typesLY, typesLW, typesLH);
    
    const auto posLX = getWidth()*(0.05);
    const auto posLY = getHeight()*(0.58);
    const auto posLH = getHeight()*(0.1);
    const auto posLW = getWidth()*(0.1);
    
    labelsPlugin[5]->setBounds(posLX, posLY, posLW, posLH);

    const auto TypeX = getWidth()*(0.165);
    const auto TypeY = getHeight()*(0.558);
    const auto TypeH = getHeight()*(0.12);
    const auto TypeW = getWidth()*(0.03);
    
    comboBoxPlugin[0]->setBounds(TypeX, TypeY, TypeH, TypeW);
    
    const auto posBX = getWidth()*0.165;
    const auto posBY = getHeight()*0.612;
    const auto posBH = getHeight()*0.12;
    const auto posBW = getWidth()*0.03;
    
    comboBoxPlugin[1]->setBounds(posBX, posBY, posBH, posBW);
    
    ///Adds dimensions to speaker objects
    const auto speakerLX = getWidth()*0.8;
    const auto speakerLY = getHeight()*0.47;
    const auto speakerLH = getHeight() *0.06;
    const auto speakerLW = getWidth()* 0.09;
    
    labelsPlugin[6]->setBounds(speakerLX, speakerLY, speakerLW, speakerLH);
    
    const auto speakerBX = getWidth()*0.788;
    const auto speakerBY = getHeight()*0.56;
    const auto speakerBH = getHeight()*0.12;
    const auto speakerBW = getWidth()*0.03;
    
    comboBoxPlugin[2]->setBounds(speakerBX, speakerBY, speakerBH, speakerBW);
    
    ///Adds dimensions to filter objects
    const auto filterLX = getWidth()*(0.18);
    const auto filterLY = getHeight()*0.72;
    const auto filterLH = getWidth()*(0.02);
    const auto filterLW = getHeight()*(0.15);

    labelsPlugin[7]->setBounds(filterLX, filterLY, filterLW, filterLH);
    
    const auto lowFilterLx = getWidth()*(0.06);
    const auto lowFilterLy = getHeight()*(0.77);
    const auto highFilterLx = getWidth()*(0.3);

    labelsPlugin[8]->setBounds(highFilterLx, lowFilterLy, filterLW, filterLH);
    labelsPlugin[9]->setBounds(lowFilterLx, lowFilterLy, filterLW, filterLH);

    const auto lowFilterX = getWidth()*(0.06);
    const auto filterY = getHeight()*(0.78);
    const auto highFilterX = getWidth()*(0.3);

    slidersPlugin[2]->setBounds(lowFilterX,filterY,sliderW,sliderH);
    slidersPlugin[3]->setBounds(highFilterX, filterY, sliderW, sliderH);
    
    const auto stateFilterX = getWidth()*(0.18f);
    const auto stateFilterY = getHeight()*(0.825f);
    const auto stateFilterW = getWidth()*(0.075f);
    const auto stateFilterH = getHeight()*(0.035f);
    
    stateFilter.setBounds(stateFilterX, stateFilterY, stateFilterW, stateFilterH);
    
    ///Adds dimensions to wetDrymix object
    const auto labelMx = getWidth()*(0.465);
    const auto labelMy = getHeight()*0.72;
    const auto labelMw = 80;
    const auto labelMh = 20;

    labelsPlugin[10]->setBounds(labelMx, labelMy, labelMw, labelMh);
    
    const auto wetDryX = getWidth()*(0.482);
    const auto wetDryY = getHeight()*(0.77);

    slidersPlugin[4]->setBounds(wetDryX, wetDryY, getWidth()/4*(0.2), getHeight()/4*(0.62));
}

void ELALTOSPACESAudioProcessorEditor::setRoundRectangles(juce::Graphics& g,float _spaceBorders, float _spaceRectangles, float _r1Size, float _r2Size, float _r3Size, float _cornerRoundness)
{
    const auto w = getWidth() - (2*getWidth()*_spaceBorders); //width rectangles
    const auto pX = getWidth()*_spaceBorders; //coordinate x rectangles
    const auto p1Y = getHeight()*_spaceBorders; //coordinate y r1
    const auto deltaY = getHeight()*_spaceRectangles; //distance between rectangles
    const auto hr1Out = getHeight()*_r1Size; //heigh r1
    const auto hr2Out = getHeight()*_r2Size; //heigh r2
    const auto hr3Out = getHeight()*_r3Size; //heigh r3
    const auto p2Y = hr1Out + deltaY; //coordinate y r2
    const auto p3Y = hr1Out + hr2Out; //coordinate y r3
    const auto hr1In = hr1Out - p1Y; //high r1 with space
    const auto hr2In = hr2Out - (2*deltaY); //high r2 with space
    const auto hr3In = hr3Out - p1Y; //high r3 with space
    
    juce::Path r1;
    juce::Path r2;
    juce::Path r3;
    
    r1.addRoundedRectangle(pX, p1Y, w, hr1In, _cornerRoundness);
    r2.addRoundedRectangle(pX, p2Y, w, hr2In, _cornerRoundness);
    r3.addRoundedRectangle(pX, p3Y, w, hr3In, _cornerRoundness);
    
    auto baseColour = juce::Colour(249,209,179);
    g.setColour(baseColour);
    g.strokePath(r1, juce::PathStrokeType(1.0f));
    g.strokePath(r2, juce::PathStrokeType(1.0f));
    g.strokePath(r3, juce::PathStrokeType(1.0f));
    
    //_borderSpace and _screenSpace should be express ass percentages.
}

void ELALTOSPACESAudioProcessorEditor::changeSvgColor(juce::Drawable& drawable, juce::Colour newColor)
{
    // Traverse the Drawable tree
    for (int i = 0; i < drawable.getNumChildComponents(); ++i)
    {
        if (auto* shape = dynamic_cast<juce::DrawableShape*>(drawable.getChildComponent(i)))
        {
            // Change the fill color
            shape->setFill(juce::FillType(newColor));
        }
        
        // Recursively change color for child drawables
        if (auto* childDrawable = dynamic_cast<juce::Drawable*>(drawable.getChildComponent(i)))
        {
            changeSvgColor(*childDrawable, newColor);
        }
    }
}

void ELALTOSPACESAudioProcessorEditor::setRoomImg(juce::Image &roomImg, juce::Graphics &g)
{
    //Bounds Room Image
    const auto roomImgW = getWidth()*(0.50f);
    const auto roomImgX = (getWidth()/2) - (roomImgW/2);
    const auto roomImgY = getHeight() * (0.05f);
    const auto roomImgH = getHeight()*(0.38);
    
    //Draws Room Image
    g.drawImageWithin(roomImg, roomImgX, roomImgY, roomImgW, roomImgH, juce::RectanglePlacement::stretchToFit);
    
    repaint(roomImgX, roomImgY, roomImgW, roomImgH);
}

void ELALTOSPACESAudioProcessorEditor::preparePlugin()
{
    ///Applies custom Look and Feel
    getLookAndFeel().setDefaultLookAndFeel(& lookAndFeelELALTO);
    
    ///Adds level meters
    addAndMakeVisible(inLevelMeterL);
    addAndMakeVisible(inLevelMeterR);
    addAndMakeVisible(outLevelMeterL);
    addAndMakeVisible(outLevelMeterR);
    
    ///Add Labels to UI
    for (int i = 0; i < labelsString.size(); ++i)
    {
        auto* label = new juce::Label();
        label->setText(labelsString[i], juce::dontSendNotification);
        labelsPlugin.add(label);
        addAndMakeVisible(label);
    }
    
    ///Add sliders to UI
    for (int i =0; i< slidersString.size(); ++i)
    {
        auto* slider = new juce::Slider();
        slidersPlugin.add(slider);
        addAndMakeVisible(slider);
        
        ///Adds sliders attachments
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, slidersString[i], *slider));
    
        if (i <= 3)
        {
            slider->setSliderStyle(sliderStyle1);
            slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow,
                                    false, 70, 15);
        }
        else
        {
            slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove,
                                    false, 90, 15);
            slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(13,13,13).withMultipliedAlpha(0.5));
            slider->setSliderStyle(sliderStyle2);
        }
        
        if (i < 2)
            slider->setTextValueSuffix(" dB");
        else if (i == 2 || i == 3)
            slider->setTextValueSuffix(" Hz");
        else
            slider->setTextValueSuffix("%");
    }
    
    ///Add comboBox to UI
    choicesComboBox.add(new juce::StringArray{"PATY", "YUCA"}); //Paty: dbx, Yuca: Bova
    choicesComboBox.add(new juce::StringArray{"ARISTI", "RENE"}); //Aristi: near, Rene: far
    choicesComboBox.add(new juce::StringArray{"A", "B"}); //NS10: A, OCEAN: B
    
    for (int i = 0; i < comboBoxString.size(); ++i)
    {
        auto* box = new juce::ComboBox();
        box->addItemList(*choicesComboBox[i], 1);
        comboBoxPlugin.add(box);
        addAndMakeVisible(box);
        
        ///Adds comboBox attachments
        comboBoxAttachments.add(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.apvts, comboBoxString[i], *box));
    }
    
    ///Adds control Buttons to UI
    for (int i = 0; i < roomsButtonsString.size(); ++i)
    {
        auto* b = roomsButtons.add (new juce::TextButton (roomsButtonsString[i]));
        
        addAndMakeVisible (b);
        b->setRadioGroupId (42);
        b->setClickingTogglesState (true);
    }
        
    buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "control", *roomsButtons[0]));
    buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "live", *roomsButtons[1]));
    buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "pasillo", *roomsButtons[2]));
    //roomsButtons.getUnchecked (0)->setToggleState (true, juce::dontSendNotification);

    
    ///Add on/off button to UI
    addAndMakeVisible(stateFilter);
    stateFilter.setButtonText("ON/OFF");
    stateFilter.setClickingTogglesState(true);
    buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "filter", stateFilter));
    
    
    ///Add images to UI
    background = juce::ImageCache::getFromMemory(BinaryData::gradient3_grainy_png, BinaryData::gradient3_grainy_pngSize);
    liveImg = juce::ImageCache::getFromMemory(BinaryData::liveRoom2_jpeg, BinaryData::liveRoom2_jpegSize);
    controlImg = juce::ImageCache::getFromMemory(BinaryData::controlRoom_jpeg, BinaryData::controlRoom_jpegSize);
    pasilloImg = juce::ImageCache::getFromMemory(BinaryData::pasillo_jpg, BinaryData::pasillo_jpgSize);
    logoDrawable = juce::Drawable::createFromImageData (BinaryData::logoelaltoestudiohorizontal_svg, BinaryData::logoelaltoestudiohorizontal_svgSize);

    ///Flags Buttons State from Parameters Value Saved
    if (audioProcessor.controlButton)
        roomsButtons[0]->triggerClick();
    else if (audioProcessor.liveButton)
        roomsButtons[1]->triggerClick();
    else if (audioProcessor.pasilloButton)
        roomsButtons[2]->triggerClick();
}
