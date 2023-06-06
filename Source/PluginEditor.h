#pragma once
#include "PluginProcessor.h"

const juce::String websiteAddress = "https://github.com/tesserato/PianoForte";
const juce::String donationsAddress = "https://github.com/tesserato/PianoForte";


class myKeyBoard : public juce::MidiKeyboardComponent
{
private:
    juce::Image blackKeyImage = juce::ImageCache::getFromMemory(BinaryData::black_png, BinaryData::black_pngSize);
    juce::Image whiteKeyImage = juce::ImageCache::getFromMemory(BinaryData::white_png, BinaryData::white_pngSize);
    juce::Image blackKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::black_hover_png, BinaryData::black_hover_pngSize);
    juce::Image whiteKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::white_hover_png, BinaryData::white_hover_pngSize);
    juce::Image blackKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::black_pressed_png, BinaryData::black_pressed_pngSize);
    juce::Image whiteKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::white_pressed_png, BinaryData::white_pressed_pngSize);

public:
    myKeyBoard(juce::MidiKeyboardState& state, const Orientation orientation) 
        : MidiKeyboardComponent(state, orientation)
    {
        //juce::Logger::writeToLog("myKeyBoard constructor");
        this->setColour(this->keySeparatorLineColourId, juce::Colours::black);
        blackKeyImage = juce::ImageCache::getFromMemory(BinaryData::black_png, BinaryData::black_pngSize);
        whiteKeyImage = juce::ImageCache::getFromMemory(BinaryData::white_png, BinaryData::white_pngSize);
        blackKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::black_hover_png, BinaryData::black_hover_pngSize);
        whiteKeyHoverImage = juce::ImageCache::getFromMemory(BinaryData::white_hover_png, BinaryData::white_hover_pngSize);
        blackKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::black_pressed_png, BinaryData::black_pressed_pngSize);
        whiteKeyPressedImage = juce::ImageCache::getFromMemory(BinaryData::white_pressed_png, BinaryData::white_pressed_pngSize);

    };


    ~myKeyBoard() {}

protected:


    void  drawWhiteNote(
        int midiNoteNumber,
        juce::Graphics& g,
        juce::Rectangle< float > area,
        bool isDown,
        bool isOver,
        juce::Colour lineColour,
        juce::Colour textColour) override
    {
        //DBG("drawWhiteNote called");
        auto keyRectangle = getRectangleForKey(midiNoteNumber);
        g.setOpacity(1.0);
        //g.setColour(juce::Colours::black);
        if (21 <= midiNoteNumber && midiNoteNumber <= 108) {
            if (isDown)
                g.drawImage(whiteKeyPressedImage, keyRectangle, juce::RectanglePlacement::stretchToFit, false);
            else if (isOver)
                g.drawImage(whiteKeyHoverImage, keyRectangle, juce::RectanglePlacement::stretchToFit, false);
            else
                g.drawImage(whiteKeyImage, keyRectangle, juce::RectanglePlacement::stretchToFit, false);
        }
        else {
            g.drawImage(whiteKeyImage, keyRectangle, juce::RectanglePlacement::stretchToFit, false);
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillRect(keyRectangle);
        }
    }

    void drawBlackNote(
        int midiNoteNumber, 
        juce::Graphics& g, 
        juce::Rectangle< float > area, 
        bool isDown, 
        bool isOver,
        juce::Colour noteFillColour
    ) override
    {
        auto keyRectangle = getRectangleForKey(midiNoteNumber);
        g.setOpacity(1.0);
        if (21 <= midiNoteNumber && midiNoteNumber <= 108) {
            if (isDown)
                g.drawImage(blackKeyPressedImage, keyRectangle);
            else if (isOver)
                g.drawImage(blackKeyHoverImage, keyRectangle);
            else
                g.drawImage(blackKeyImage, keyRectangle);
        }
        else {
            g.drawImage(blackKeyImage, keyRectangle, juce::RectanglePlacement::stretchToFit, false);
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillRect(keyRectangle);
        }
    }
};

class banner : public juce::Timer
{
private:
    std::vector<juce::DrawableImage> frames/*{7}*/;
    int counter = 0;
public:
    juce::DrawableButton button{ "logo", juce::DrawableButton::ButtonStyle::ImageFitted };

    banner() {
        DBG("banner constructor");

        // https://forum.juce.com/t/binary-resource-tutorial/45047/2
        // https://forum.juce.com/t/passing-binary-data-into-a-function/45108

        //juce::DrawableImage f0{ juce::ImageCache::getFromMemory(BinaryData::frame1_png, BinaryData::frame0_pngSize) };
        //frames.push_back(f0);
        for (size_t i = 0; i <  BinaryData::namedResourceListSize; i++) // number of resources
        {
            const char* name = BinaryData::namedResourceList[i];
            std::string nameStr = name;
            if(nameStr.find("frame", 0) != std::string::npos)
            {
                int dataSizeInBytes;
                auto data = BinaryData::getNamedResource(name,  dataSizeInBytes);
                juce::DrawableImage image{ juce::ImageCache::getFromMemory(data, dataSizeInBytes) };
                frames.push_back(image);
            }
        }
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        button.onClick = [] { juce::URL(donationsAddress).launchInDefaultBrowser(); };
        button.setImages(&frames[0], &frames[0]);
        startTimer(1000.0f / 12.0f);
    }
    void timerCallback() override
    {
        counter++;
        if(counter > frames.size() -1)
        {
            counter = 0;
        }
        button.setImages(&frames[counter], & frames[counter]);
        button.repaint();
    };
};

class PluginAudioProcessorEditor  : 
    public juce::AudioProcessorEditor
{
public:
    PluginAudioProcessor& audioProcessor;//processor object that created editor.
    myKeyBoard keyboard;   
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;

    juce::Image backgroundImg;

    juce::ImageComponent nameImageComponent;

    banner myBanner;

    juce::DrawableImage  logoImg{ juce::ImageCache::getFromMemory(BinaryData::osp_png, BinaryData::osp_pngSize) };
    juce::DrawableImage  logoImgHover{ juce::ImageCache::getFromMemory(BinaryData::osph_png, BinaryData::osph_pngSize) };
    juce::DrawableButton  logoDrawableButton{ "logo", juce::DrawableButton::ButtonStyle::ImageFitted };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sustainAttachment;
    juce::DrawableImage  sustainOn{ juce::ImageCache::getFromMemory(BinaryData::sustainOn_png, BinaryData::sustainOn_pngSize) };
    juce::DrawableImage  sustainOnOver{ juce::ImageCache::getFromMemory(BinaryData::sustainOnOver_png, BinaryData::sustainOnOver_pngSize) };
    juce::DrawableImage  sustainOff{ juce::ImageCache::getFromMemory(BinaryData::sustainOff_png, BinaryData::sustainOff_pngSize) };
    juce::DrawableImage  sustainOffOver{ juce::ImageCache::getFromMemory(BinaryData::sustainOffOver_png, BinaryData::sustainOffOver_pngSize) };
    juce::DrawableButton sustainButton{ "logo", juce::DrawableButton::ButtonStyle::ImageFitted };

    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modAttachment;
    //juce::Slider modWheel;
    //juce::Label modLabel;

    PluginAudioProcessorEditor(PluginAudioProcessor& p) :
        AudioProcessorEditor(&p),
        audioProcessor(p),
        keyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        DBG("PluginAudioProcessorEditor constructor");
        setOpaque(true);
        backgroundImg = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);


        juce::Image nameImg = juce::ImageCache::getFromMemory(BinaryData::pianoforte_png, BinaryData::pianoforte_pngSize);
        nameImageComponent.setImage(nameImg);
        addAndMakeVisible(nameImageComponent);

        addAndMakeVisible(myBanner.button);
        

        logoDrawableButton.setImages(&logoImg, &logoImgHover);
        logoDrawableButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        logoDrawableButton.onClick = [] { juce::URL(websiteAddress).launchInDefaultBrowser(); };
        addAndMakeVisible(logoDrawableButton);

        addAndMakeVisible(midiInputListLabel);
        midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
        midiInputListLabel.attachToComponent(&midiInputList, true);

        addAndMakeVisible(midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
        auto midiInputs = juce::MidiInput::getAvailableDevices();
        juce::StringArray midiInputNames;
        for (auto& input : midiInputs)
        {
            DBG(input.name);
            midiInputNames.add(input.name);
        }
        midiInputList.addItemList(midiInputNames, 1);
        midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };
        // find the first enabled device and use that by default
        for (auto& input : midiInputs)
        {
            if (deviceManager.isMidiInputDeviceEnabled(input.identifier))
            {
                setMidiInput(midiInputs.indexOf(input));
                break;
            }
        }

        if (midiInputList.getSelectedId() == 0) { setMidiInput(0); }    // if no enabled devices were found just use the first one in the list

        sustainButton.addListener(&p);
        //sustainButton.setToggleable(true);
        sustainButton.setClickingTogglesState(true);
        sustainButton.setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentWhite);
        sustainButton.setImages(&sustainOff, &sustainOffOver, nullptr , nullptr ,&sustainOn, &sustainOnOver);
        addAndMakeVisible(sustainButton);
        addAndMakeVisible(keyboard);
        keyboard.setLowestVisibleKey(18);
        setSize(1200, 300);
        setResizable(true, true);
    }
    ~PluginAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

    void setMidiInput(int index);/** Starts listening to a MIDI input device, enabling it if necessary. */


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
