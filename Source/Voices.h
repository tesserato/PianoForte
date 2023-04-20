#pragma once
#include <JuceHeader.h>
#include <future>
#include <math.h>       /* modf */
#include <random>
#include<functional> /*lambda functions*/
//#include <onnxruntime_cxx_api.h>
#include "core/session/onnxruntime_cxx_api.h"

static std::random_device rd;  //Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
static std::uniform_int_distribution<> distrib(0, 1000);


const static float MAX_NUMBER_OF_PERIODS = 4511.0;
const static int POLYPHONY = 20; /*number of notes allowed simultaniously*/

template <typename T> float sign(T val) 
{
    return (T(0) < val) - (val < T(0));
}

 class ModelInfo
 {  
 public:
     Ort::Session session = Ort::Session{ nullptr };
     std::vector<int64_t> inputShape = { 0 }, outputShape = { 0 };
     float sampleRate = 44100.0;
     static ModelInfo& instance()
     {
         static ModelInfo INSTANCE;
         return INSTANCE;
     }
 private:
     ModelInfo()
     {
        //juce::Logger::setCurrentLogger(juce::FileLogger::createDefaultAppLogger("PFplugin", "PFplugin.txt", "######"));
        juce::Logger::writeToLog("ModelInfo constructor, Current dir: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
        Ort::Env env{ ORT_LOGGING_LEVEL_WARNING, "Piano" };
        juce::Logger::writeToLog("ModelInfo constructor, passed env");

        const Ort::SessionOptions session_options;

        /// Load from file
        //std::string str = "SteinwayD.oms";
        //std::wstring wide_string = std::wstring(str.begin(), str.end());
        //std::basic_string<ORTCHAR_T> model_file = std::basic_string<ORTCHAR_T>(wide_string);
        //session = Ort::Session(env, model_file.c_str(), session_options);
        /// Load from memory
        int dataSizeInBytes;
        const void* data = BinaryData::getNamedResource("engine", dataSizeInBytes);
        if (data) {
            session = Ort::Session(env, data, dataSizeInBytes, session_options);
        }
        else {
            juce::Logger::writeToLog("Could not find engine");
            throw;
        }
        inputShape = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
        outputShape = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape(); 
        juce::Logger::writeToLog("ModelInfo constructed");
     }
 };


struct pianoSound : public juce::SynthesiserSound
{
    pianoSound() {}
    bool appliesToNote(int midiNoteNumber) override { 
        if (21 <= midiNoteNumber && midiNoteNumber <= 108)
        {
            return true;
        }
        return false; 
    }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

struct pianoVoice : public juce::SynthesiserVoice
{
private:
    ModelInfo& MI = ModelInfo::instance();
    std::future<void> fut;
    std::vector<float> targetAmps = std::vector<float>(MI.outputShape[0], 0);
    std::vector<float> currentAmps = std::vector<float>(MI.outputShape[0], 0);
    std::vector<float> phasesC1 = std::vector<float>(MI.outputShape[0], 0);
    std::vector<float> phasesC2 = std::vector<float>(MI.outputShape[0], 0);
    std::vector<float> I0 = std::vector<float>(MI.inputShape[0], 0);
    std::vector<float> W = std::vector<float>(2, 0);
    long x = 0;
    bool  isPlaying = false;
    float  level = 0.0f, midiKey = 0.0f;
    std::function<void()> forward = [&] {
        std::vector<Ort::Value> inputTensor;
        std::vector<Ort::Value> outputTensor;

        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
        inputTensor.push_back(Ort::Value::CreateTensor<float>(memoryInfo, I0.data(), MI.inputShape[0], MI.inputShape.data(), MI.inputShape.size()));
        outputTensor.push_back(Ort::Value::CreateTensor<float>(memoryInfo, targetAmps.data(), MI.outputShape[0], MI.outputShape.data(), MI.outputShape.size()));
        Ort::AllocatorWithDefaultOptions allocator;
        std::vector < const char*> inputNames = { MI.session.GetInputNameAllocated(0, allocator).get() };
        std::vector < const char*> outputNames = { MI.session.GetOutputNameAllocated(0, allocator).get() };
        MI.session.Run(
            Ort::RunOptions{ nullptr }, 
            inputNames.data(), 
            inputTensor.data(), 
            1, 
            outputNames.data(), 
            outputTensor.data(),
            1
        );

        float ampsSum = 0.0;
        for (size_t i = 0; i < targetAmps.size(); i++)
        {
            //targetAmps[i] += targetAmps[i] * float(distrib(gen)) / 1000.0;
            ampsSum += targetAmps[i];
        }
        for (size_t i = 0; i < targetAmps.size(); i++)
        {
            targetAmps[i] /= ampsSum;
        }
        return;
    };

public:
    double lastActive = juce::Time::getMillisecondCounterHiRes();
    float tailOff = 0.0;
    pianoVoice() { MI.sampleRate = getSampleRate(); };

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<pianoSound*> (sound) != nullptr;
    }


    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    bool isVoiceActive() const override {
        return tailOff > 0.0;
    }

    void getNextSample();

};

