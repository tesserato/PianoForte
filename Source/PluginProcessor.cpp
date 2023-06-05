#pragma once
//#include "PluginProcessor.h"
#include "PluginEditor.h"


SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState,  juce::AudioProcessorValueTreeState* parameters) : keyboardState(keyState)
{
    for (auto i = 0; i < POLYPHONY; i++) // number of keys that can be played at the same time
    {
        //DBG("Adding voice " + std::to_string(i) + ", model = " + modelInfo->netName);
        pianoVoice* v =  new pianoVoice(&MI);
        synth.addVoice(v);
    }
    synth.clearSounds();
    synth.addSound(new pianoSound());
}


PluginAudioProcessor::PluginAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    parameters(*this, nullptr, juce::Identifier("Parameters"),
        {
            //std::make_unique<juce::AudioParameterFloat>("pitchSlider", "Pitch Slider Value", 0.0f, 16383.0f, 8192.0f),
            //std::make_unique<juce::AudioParameterFloat>("vowelSlider", "Vowel Slider Value", 0.0f, 127.0f, 0.0f),
            //std::make_unique<juce::AudioParameterFloat>("genderSlider", "Gender Slider Value", 0.0f, 127.0f, 0.0f),
            //std::make_unique<juce::AudioParameterFloat>("attackSlider", "Attack Slider Value", 0.0f, 127.0f, 0.0f),
            //std::make_unique<juce::AudioParameterFloat>("decaySlider", "Decay Slider Value", 0.0f, 127.0f, 0.0f),
            //std::make_unique<juce::AudioParameterFloat>("vibratoSlider", "Vibrato Slider Value", 0.0f, 127.0f, 0.0f),
            std::make_unique<juce::AudioParameterBool>("legatoMode", "Legato Mode", false),
        }),
synthAudioSource(keyboardState, &parameters)
{
    //juce::ValueTree netPath(juce::Identifier("netPathNode"));
    //netPath.setProperty(juce::Identifier("netPath"), "", nullptr);
    //parameters.state.appendChild(netPath, nullptr);

    //DBG("xml");
    //DBG(parameters.state.createXml()->toString());
    //auto n = parameters.state.getChildWithName("netPathNode").getProperty("netPath").toString().toStdString();
    //DBG(">" + n);

    //////////////////////////////////////////
    //auto child = parameters.state.getChildWithName("netPathNode");
    //parameters.state.removeChild(child, nullptr);
    //juce::ValueTree newNetPath(juce::Identifier("netPathNode"));
    //newNetPath.setProperty(juce::Identifier("netPath"), "teste_02", nullptr);
    //parameters.state.appendChild(newNetPath, nullptr);

    //n = parameters.state.getChildWithName("netPathNode").getProperty("netPath").toString().toStdString();
    //DBG(">" + n);
    //////////////////////////////////////////
}

PluginAudioProcessor::~PluginAudioProcessor()
{
    //DBG("PluginAudioProcessor destructor (2)");
    //session.Close();
    //model.Close();
}

const juce::String PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //MI.sampleRate = sampleRate;
    synthAudioSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    //juce::ScopedNoDenormals noDenormals;
    //auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();

    //// In case we have more outputs than inputs, this code clears any output
    //// channels that didn't contain input data, (because these aren't
    //// guaranteed to be empty - they may contain garbage).
    //// This is here to avoid people getting screaming feedback
    //// when they first compile a plugin, but obviously you don't need to keep
    //// this code if your algorithm always overwrites all the output channels.
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //{
    //    buffer.clear(i, 0, buffer.getNumSamples());
    //}


    for (const auto metadata : midiMessages) // are midiMessages external? they dont come from the software keyboard...
    {
        auto message = metadata.getMessage();
        //const auto time = metadata.samplePosition;
        synthAudioSource.midiCollector.addMessageToQueue(message);
        //DBG(message.getDescription());
    }    

    juce::AudioSourceChannelInfo bufferToFill(buffer);
    synthAudioSource.getNextAudioBlock(bufferToFill);
}

bool PluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    //DBG("PluginAudioProcessor::createEditor()");
    return new PluginAudioProcessorEditor(*this);
}

void PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr) {
        //if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }


    //auto netPath = parameters.state.getPropertyAsValue("netPath", nullptr, true);
    //auto storedNetPath = parameters.state.getChildWithName("netPathNode").getProperty("netPath").toString().toStdString();
    //DBG("storedNetPath=" + storedNetPath + " modelInfo.netPath=" + modelInfo.netPath);
    //if (storedNetPath != "" && storedNetPath != modelInfo.netPath)
    //{
    //    auto newModelInfo = loadModel(storedNetPath);
    //    if (newModelInfo.netName != "")
    //    {
    //        modelInfo.netName = newModelInfo.netName;
    //        modelInfo.netPath = newModelInfo.netPath;
    //        //modelInfo.model = newModelInfo.model;
    //        //modelInfo.session = newModelInfo.session;
    //        //modelInfo.inputLayerName = newModelInfo.inputLayerName;
    //        //modelInfo.outputLayerName = newModelInfo.outputLayerName;
    //        modelInfo.outputs = newModelInfo.outputs;
    //        //p.modelInfo.loaded = true;
    //        //editor->pickNetButton.setButtonText(modelInfo.netName);
    //    }
    //}
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() // This creates new instances of the plugin..
{
    return new PluginAudioProcessor();
}


