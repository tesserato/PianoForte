#pragma once
#include "Voices.h"

struct customSynth : public juce::Synthesiser{
    pianoVoice* findFreeVoice(juce::SynthesiserSound* soundToPlay, int midiChannel, int midiNoteNumber, bool stealIfNoneAvailable) const override {
        //pianoVoice* toUse = nullptr;
        pianoVoice* oldest = static_cast<pianoVoice*> (getVoice(0));
        if (!oldest->isVoiceActive()) // returns isSounding
        {
            DBG("returned voice 0");
            return oldest;
        }        
        for (int i = 1; i < getNumVoices(); i++)
        {
            pianoVoice* current = static_cast<pianoVoice*> (getVoice(i));
            if (!current->isVoiceActive())
            {
                DBG("returned voice " + std::to_string(i));
                return current;
            }
            if (current->lastActivated < oldest->lastActivated)
            {
                oldest = current;
                DBG("oldest voice = " + std::to_string(i));
            }
        }
        oldest->tailOffRatio = 0.92f;
        oldest->stopNote(0.0f, true);
        return oldest;
    }

    //void handleMidiEvent(const juce::MidiMessage& message) override
    //{

    //}
    void handleSustainPedal(int midiChannel, bool isDown) override
    {
        isSustainOn = isDown;
//        for (size_t i = 0; i < getNumVoices(); i++) {
//            pianoVoice* voice = static_cast<pianoVoice*> (getVoice(i));
//            voice->setSustainPedalDown(isDown);
//;        }
    }
};

struct SynthAudioSource : public juce::AudioSource
{
    juce::MidiKeyboardState& keyboardState;
    customSynth synth;
    juce::MidiMessageCollector midiCollector;
    NeuralModel MI = NeuralModel();
    //NeuralModel dwModel = NeuralModel("engineDW");

    SynthAudioSource(juce::MidiKeyboardState& keyState, juce::AudioProcessorValueTreeState* parameters);

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        //DBG(">> fps ptp=" + std::to_string(sampleRate));
        midiCollector.reset(sampleRate);
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
        keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
        for (auto midi : incomingMidi)
        {
            auto d = midi.getMessage();
            DBG(d.getDescription() /*+ " @ " + d.getTimeStamp()*/);
            //DBG(d.getTimeStamp());
        }
    }
};

class PluginAudioProcessor :
    public juce::AudioProcessor,
    public juce::Slider::Listener,
    public juce::Button::Listener
{
public:
    juce::AudioProcessorValueTreeState parameters;
    //ModelInfo& MI = ModelInfo::instance();
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardState keyboardState;
    
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    void sliderDragStarted(juce::Slider* slider) override {};

    void sliderValueChanged(juce::Slider* slider) override {};

    void sliderDragEnded(juce::Slider* slider) override {};

    void buttonStateChanged(juce::Button* b) override {};

    void buttonClicked(juce::Button* b) override
    {
        //synthAudioSource.synth.handleSustainPedal(1, b->getToggleState());
        //DBG(std::to_string(b->getToggleState()));

        juce::MidiMessage message;
        if (b->getToggleState())
        {
            message = juce::MidiMessage::controllerEvent(1, 64, 127); // sustain pedal down
            message.setTimeStamp(0.001);

        }
        else {
            message = juce::MidiMessage::controllerEvent(1, 64, 0);  // sustain pedal up
            message.setTimeStamp(0.001);
        }
        synthAudioSource.midiCollector.addMessageToQueue(message);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};
