#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Voices.h"



PluginAudioProcessorEditor::~PluginAudioProcessorEditor() { 
    DBG("PluginAudioProcessorEditor destructor (1)");
};


//==============================================================================
void PluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(backgroundImg, getLocalBounds().toFloat());
    g.setColour (juce::Colours::black);
    g.setFont (15.0f);
}

void PluginAudioProcessorEditor::resized()
{
    auto height = getBounds().getHeight();
    auto width = getBounds().getWidth();
    juce::FlexBox row01;                                               // [1]
    row01.flexDirection = juce::FlexBox::Direction::row;
    row01.flexWrap = juce::FlexBox::Wrap::noWrap;                        // [2]
    row01.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;      // [3]
    row01.alignContent = juce::FlexBox::AlignContent::center;          // [4]
   
    row01.items.add(
        juce::FlexItem(nameImageComponent)
        .withFlex(1, 1)
        .withMinWidth(100.0f)
        .withMaxWidth(width)
        .withMinHeight(50.0f)
        .withMaxHeight(height)
        .withMargin(5.0f)
    );
    row01.items.add(
        juce::FlexItem(myBanner.button)
        .withFlex(2, 1)
        .withMinWidth(100.0f)
        .withMaxWidth(width)
        .withMinHeight(50.0f)
        .withMaxHeight(height)
        .withMargin(5.0f)
    );
    row01.items.add(
        juce::FlexItem(logoDrawableButton)
        .withFlex(1, 1)
        .withMinWidth(50.0f)
        .withMaxWidth(width)
        .withMinHeight(50.0f)
        .withMaxHeight(height)
    );

    juce::FlexBox row02;
    row02.flexDirection = juce::FlexBox::Direction::row;
    row02.flexWrap = juce::FlexBox::Wrap::noWrap;
    row02.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    row02.alignContent = juce::FlexBox::AlignContent::center;    
    row02.items.add(
        juce::FlexItem(sustainButton)
        .withFlex(1, 1)
        .withMinWidth(10.0f)
        .withMaxWidth(width)
        .withMinHeight(10.0f)
        .withMaxHeight(height)
    );
    row02.items.add(
        juce::FlexItem(midiInputList)
        .withFlex(5, 1)
        .withMinWidth(10.0f)
        .withMaxWidth(width / 2.0f)
        .withMinHeight(10.0f)
        .withMaxHeight(height)
        .withMargin(10.0f)
    );

    juce::FlexBox col01;                                            
    col01.flexDirection = juce::FlexBox::Direction::column;
    col01.flexWrap = juce::FlexBox::Wrap::noWrap;
    col01.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    col01.alignContent = juce::FlexBox::AlignContent::center;

    col01.items.add(
        juce::FlexItem(row01)
        .withFlex(2, 1)
        .withMinWidth(50.0f)
        .withMaxWidth(width)
        .withMinHeight(20.0f)
        .withMaxHeight(height)
    );
 
    col01.items.add(
        juce::FlexItem(keyboard)
        .withFlex(6, 1)
        .withMinWidth(50.0f)
        .withMaxWidth(width)
        .withMinHeight(20.0f)
        .withMaxHeight(height)
        .withMargin(10.0)
    );

    col01.items.add(
        juce::FlexItem(row02)
        .withFlex(1, 1)
        .withMinWidth(50.0f)
        .withMaxWidth(width)
        .withMinHeight(height / 10.0f)
        .withMaxHeight(height)
    );
    col01.performLayout(getLocalBounds().toFloat());
    keyboard.setKeyWidth(keyboard.getWidth() / 55);
    keyboard.setLowestVisibleKey(18);
}

void PluginAudioProcessorEditor::setMidiInput(int index) {
    {
        auto list = juce::MidiInput::getAvailableDevices();
        deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, audioProcessor.synthAudioSource.getMidiCollector());
        auto newInput = list[index];

        if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback(newInput.identifier, audioProcessor.synthAudioSource.getMidiCollector());
        midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }
}


//myKeyBoard::myKeyBoard(juce::MidiKeyboardState& state, const Orientation orientation)
//    : MidiKeyboardComponent(state, orientation)
//{
//    DBG("Running From: " << juce::File::getCurrentWorkingDirectory().getFullPathName()); 
//    //blackKeyImage = juce::ImageCache::getFromMemory(BinaryData::black_png, BinaryData::black_pngSize);
//    //blackKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::black_hover_png, BinaryData::black_hover_pngSize);
//    //blackKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::black_pressed_png, BinaryData::black_pressed_pngSize);
//
//    //whiteKeyImage = juce::ImageCache::getFromMemory(BinaryData::white_png, BinaryData::white_pngSize);
//    //whiteKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::white_hover_png, BinaryData::white_hover_pngSize);
//    //whiteKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::white_pressed_png, BinaryData::white_pressed_pngSize);
//    setKeyWidth(20);				//white width
//}
