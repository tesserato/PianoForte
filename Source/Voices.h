#pragma once
#include <JuceHeader.h>
#include <future>
#include <random>
#include <functional> /*lambda functions*/
#include <cmath>
#include "core/session/onnxruntime_cxx_api.h"
#include "pocketfft_hdronly.h"

const static std::vector<float> G1F = { 2.0, 2.99, 12.06, 10.05, 3.99, 5.99, 4.99, 13.1, 7.01, 18.29 };
const static std::vector<float> G1A = { 0.23208921732809587, 0.12390899962772157, 0.1202655072325983, 0.0958528522927792, 0.09290752021964575, 0.09251198222817361, 0.07335574633060316, 0.06024279863644426, 0.058645570020530696, 0.05021980608340747 };

const static std::vector<float> G2F = { 1.0, 3.01, 5.01, 6.02, 7.03, 12.12, 10.07, 11.1, 2.01, 15.22 };
const static std::vector<float> G2A = { 0.20852840635984818, 0.15470666471198966, 0.1239913441652017, 0.10206270094142252, 0.09695878812782073, 0.07130066260549245, 0.06724863503062305, 0.06497701280358988, 0.05518694681005616, 0.05503883844395576 };

const static std::vector<float> G3F = {1.01, 3.03, 2.02, 6.13, 5.09, 4.05, 7.2, 11.65, 0.17, 8.28};
const static std::vector<float> G3A = { 0.5137265224293492, 0.19877101057760907, 0.10506174395438699, 0.047354350988609746, 0.032083361615701335, 0.029841748834395228, 0.023803921899924047, 0.017275966126186502, 0.01615561127455513, 0.015925762299282668 };



static std::default_random_engine generator;
static std::normal_distribution<float> PHASES_NORM(0, 1.5);
static std::normal_distribution<float> PHASES_NOISE(0.01, 0.001);
const static float CENT = 1.00057778951f;
const static float MAX_NUMBER_OF_PERIODS = 4511.0f;
const static int POLYPHONY = 20; /*number of notes allowed simultaniously*/

extern bool isSustainOn;

inline float partialFromMidiKey(float key, float partial = 1.0f) {
    float beta = 0.000033; // 0.006;
    float f0 = 440.0f * std::powf(2.0f, (key - 69.0f) / 12.0f);
    float m = partial * std::sqrt(1.0f + beta * partial * partial);
    return m * f0;
}

template <typename T> float sign(T val) 
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
inline std::vector<T> complexToHalfComplex(std::vector<std::complex<T>> C)
{
    std::vector<T> H(2 * (C.size() - 1));
    H[0] = C[0].real();
    for (size_t i = 1; i < C.size() - 1; i++)
    {
        H[2 * (i - 1) + 1] = C[i].real();
        H[2 * (i - 1) + 2] = C[i].imag();
    }
    H[H.size() - 2] = C.back().real();
    if (C.back().imag() != 0)
    {
        H.push_back(C.back().imag());
    }
    return H;
}

inline std::vector<float> irfft(std::vector<std::complex<float>>& complexIn)
{
    std::vector<float> in = complexToHalfComplex(complexIn);
    std::vector<float> out(in.size());
    pocketfft::shape_t shapeReal = { out.size() };
    pocketfft::stride_t strideReal = { sizeof(float) };    // {(&dataReal[1] - &dataReal[0]) * CHAR_BIT};
    //pocketfft::stride_t strideComplex = { sizeof(float) }; //{(&dataComplex[1] - &dataComplex[0]) * CHAR_BIT};
    pocketfft::r2r_fftpack(shapeReal, strideReal, strideReal, { 0 }, false, false, in.data(), out.data(), 1.0f);
    // #ifdef MYDEBUG
    //   write_vector(out, "F.csv", ';');
    // #endif
    return out;
}

class ManualPiano
{
private:
    //float decay = 0.00037f;
    float n = 44100.0f;
    //float a = 0.0f;
    float fps = 44100.0f;
    size_t idx = 0;
    size_t t = 0;
    float fLocal = 0.0;
    std::vector<float> harmonics;
    std::vector<float> amplitudes;
    std::vector<float> phasesL;
    std::vector<float> phasesR;
    std::vector<float> currPhasesL;
    std::vector<float> currPhasesR;
public:
    void start(int midiKey, float sampleRate = 44100.0) {
        fps = sampleRate;
        t = 0;
        fLocal = partialFromMidiKey(midiKey) * n / sampleRate;
        idx = midiKey - 21;
        //harmonics.clear();
        //amplitudes.clear();
        //phasesL.clear();
        //phasesR.clear();
        //float localMaxFrequency = 20000.0f * n / sampleRate;
        //std::vector<float> pL;// = PHASES_NORM(generator);
        //std::vector<float> pR;// = PHASES_NORM(generator);
        //int firstPartial = 1;
        //int lastPartial = 10;



        if (midiKey <= 15 + 20)
        {
            harmonics =  G1F;
            amplitudes = G1A;
        }
        else if (midiKey <= 30 + 20)
        {
            harmonics = G2F;
            amplitudes = G2A;
        }
        else
        {
            harmonics = G3F;
            amplitudes = G3A;
        }
        currPhasesL.resize(harmonics.size());
        std::fill(currPhasesL.begin(), currPhasesL.end(), 0.0f);
        currPhasesR.resize(harmonics.size());
        std::fill(currPhasesR.begin(), currPhasesR.end(), 0.0f);

        phasesL.clear();
        phasesR.clear();
        phasesL.push_back(PHASES_NORM(generator));
        phasesR.push_back(PHASES_NORM(generator));
        for (size_t i = 0; i < harmonics.size() - 1; i++)
        {
            phasesL.push_back(phasesL.back() + PHASES_NORM(generator));
            phasesR.push_back(phasesR.back() + PHASES_NORM(generator));
        }
        
        //for (size_t partial = firstPartial; partial <= lastPartial; partial++)
        //{
        //    float fLocal = partialFromMidiKey(midiKey, partial) * n / sampleRate;
        //    if (fLocal >= localMaxFrequency)
        //    {
        //        break;
        //    }
        //    harmonics.push_back(fLocal);
        //    phasesL.push_back(phasesL.back() + PHASES_NORM(generator));
        //    phasesR.push_back(phasesR.back() + PHASES_NORM(generator));
        //}
        //float N = float(harmonics.size());
        //a = 6.0f / (N * (2.0f * N * N + 3.0f * N + 1.0f));

        //for (size_t i = 0; i < harmonics.size(); i++)
        //{
        //    float ampPart = float(i) - float(harmonics.size());
        //    float amp = a * ampPart * ampPart;
        //    amplitudes.push_back(amp);
        //}
    }
    std::vector<float> step()
    {
        float yL = 0.0;
        float yR = 0.0;
        for (size_t i = 0; i < harmonics.size(); i++)
        {
            float f = harmonics[i] * fLocal;
            float a = amplitudes[i];
            float pL = currPhasesL[i];
            float pR = currPhasesR[i];
            currPhasesL[i] += f * (phasesL[i] - pL) / fps;
            currPhasesR[i] += f * (phasesR[i] - pR) / fps;
            //float f = harmonics[i];
            //float pi = juce::MathConstants<float>::pi;
            //float h = 2.0f * pi * f * float(t) / n;

            //float dPart = h * decay;
            //float d = amplitudes[i] / (1.0f + dPart * dPart);
            //int strings = phasesL[i].size();
            //float yPartialL = 0.0;
            //float yPartialR = 0.0;
            //for (size_t j = 0; j < strings; j++)
            //{
            //    //float m = 1.0f + float(j) * 0.0001f * CENT;
            //    float ap = std::sin(phasesL[i][j] + h);
            //    phasesL[i][j] += PHASES_NOISE(generator);
            //    if (ap > yPartialL)
            //    {
            //        yPartialL = ap;
            //    }
            //    ap = std::sin(phasesR[i][j] + h);
            //    phasesR[i][j] += PHASES_NOISE(generator);
            //    if (ap > yPartialR)
            //    {
            //        yPartialR = ap;
            //    }
            //}
            float h = 2.0f * juce::MathConstants<float>::pi * f * float(t) / n;
            float d = std::exp(-0.0005f * h);
            yL += a * std::sin(pL + h) * d;
            yR += a * std::sin(pR + h) * d;
        }
        t++;
        return { yL, yR };
    }
};

class NeuralModel
 {  
 public:
     //float sampleRate = 44100.0;
     std::vector<int64_t> inputShape{ 0 }, outputShape{ 0 };
     Ort::Session session = Ort::Session{ nullptr };
     std::string inputName, outputName;
     //std::vector<Ort::AllocatedStringPtr> inputNamePtr, outputNamePtr;

     NeuralModel(const char* name = "engineMain")
     {
        //juce::Logger::setCurrentLogger(juce::FileLogger::createDefaultAppLogger("PFplugin", "PFplugin.txt", "######"));
        //juce::Logger::writeToLog("ModelInfo constructor, Current dir: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
        Ort::Env env{ ORT_LOGGING_LEVEL_WARNING, "Piano" };
        //juce::Logger::writeToLog("ModelInfo constructor, passed env");

        //const Ort::SessionOptions session_options;

        /// Load from file
        //std::string str = "SteinwayD.oms";
        //std::wstring wide_string = std::wstring(str.begin(), str.end());
        //std::basic_string<ORTCHAR_T> model_file = std::basic_string<ORTCHAR_T>(wide_string);
        //session = Ort::Session(env, model_file.c_str(), session_options);
        /// Load from memory
        int dataSizeInBytes;
        const void* data = BinaryData::getNamedResource(name, dataSizeInBytes);
        if (data) {
            session = Ort::Session(env, data, dataSizeInBytes, Ort::SessionOptions{ nullptr });
        }
        else {
            juce::Logger::writeToLog("Could not find engine");
            throw;
        }
        inputShape = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
        outputShape = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape(); 

        Ort::AllocatorWithDefaultOptions allocator;

        //std::vector<const char*> input_node_names;

        auto input_name = session.GetInputNameAllocated(0, allocator);
        //input_node_names.push_back(input_name.get());
        //std::vector < const char* >outputNames{ session.GetOutputNameAllocated(0, allocator).get() };
        inputName = input_name.get();
        //std::cout << input_name.get() << "\n";
        auto output_name = session.GetOutputNameAllocated(0, allocator);
        outputName = output_name.get();
        //DBG("oN=" + outputName);
        std::string nm = name;
        //DBG("Model " + nm + " constructed. I=" + std::to_string(inputShape[0]) + " T=" + std::to_string(outputShape[0]) + " IN=" + inputName + " oN=" + outputName);
     }
     void eval(std::vector<float> &I, std::vector<float> &O) {
         //DBG("before eval ->" + inputName);
     /*    std::vector<Ort::Value> inputTensor;
         std::vector<Ort::Value> outputTensor;*/
         //DBG("I = " + std::to_string(I.size()) + " O = " + std::to_string(O.size()));
         Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtDeviceAllocator, OrtMemType::OrtMemTypeDefault);
         auto inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, I.data(), I.size(), inputShape.data(), inputShape.size());
         //DBG("1");
         auto outputTensor = Ort::Value::CreateTensor<float>(memoryInfo, O.data(), O.size(), outputShape.data(), outputShape.size());
         //DBG("2");
         //Ort::AllocatorWithDefaultOptions allocator;
         //DBG("3");
         //std::vector < const char* >inputNames{ session.GetInputNameAllocated(0, allocator).get() };
         //DBG("4");
         //std::vector < const char* >outputNames{ session.GetOutputNameAllocated(0, allocator).get() };
         //DBG("5");
         //std::string oN = *outputNames.data();
         //DBG("oN=" + inputName);
         //std::string iN(*inputNames.begin(), *inputNames.end());
         //DBG("iN=" + iN);

         std::vector < const char* >inputNames{ inputName.data()};
         std::vector < const char* >outputNames{ outputName.data() };

         session.Run(
             Ort::RunOptions{ nullptr },
             inputNames.data(),
             &inputTensor,
             1,
             outputNames.data(),
             &outputTensor,
             1
         );
         //DBG("7");
         //DBG("after eval ->" + inputName);
     }
 };

//class DigitalWaveguide
// {
// private:
//     NeuralModel* model;// = NeuralModel("engineDW");
//     size_t currentStep = 0;
//     //size_t length = 1102;
//     size_t smoothing;
//     float sustain;
//     std::vector<float> delayL;
//     std::vector<float> delayR;
//     std::vector<float> powerSpectrum;
//     std::vector<float> I{ 0 };
// public:
//     DigitalWaveguide(){}
//     DigitalWaveguide(NeuralModel* m, const size_t delayLength = 1102) {
//         model = m;
//         delayL.resize(delayLength);
//         delayR.resize(delayLength);
//         powerSpectrum.resize(model->outputShape[0]);
//     }
//     void start(const float normKey, const size_t _smoothing = 2, const float _sustain = 0.90, size_t delayLength = 1102) {
//         
//         //float midiKey = normKey * 87.0f + 21.0f;
//         //float globalFrequency = frequencyFromMidiKey(midiKey);
//         //int period = int(std::round(model->sampleRate / globalFrequency));
//         //int reps = std::ceil(float(delayLength) / float(period));
//         //delayLength = period * reps;
//
//         sustain = _sustain;
//         smoothing = _smoothing;
//         currentStep = 0;
//         I[0] = normKey;
//         
//         model->eval(I, powerSpectrum);
//         DBG("outside eval DigitalWaveguide");
//
//         size_t delayLengthFD = delayLength / 2 + 1;
//
//         std::vector<float> newPowerSpectrum;
//         if (delayLengthFD != powerSpectrum.size())
//         {
//             newPowerSpectrum.resize(delayLengthFD, 0.0);
//             size_t n = powerSpectrum.size();
//             for (size_t i = 0; i < n; i++)
//             {
//                 int idx = int(std::round(float(i * (delayLengthFD - 1)) / float(n - 1)));
//                 newPowerSpectrum[idx] += powerSpectrum[i];
//             }
//         }
//         else
//         {
//             newPowerSpectrum = powerSpectrum;
//         }
//
//
//         //size_t i = 0;
//         ////float m = 0.95f;
//         //float localF0 = int(std::round(globalFrequency * float(delayLength) / model->sampleRate));         
//         //while (localF0 < newPowerSpectrum.size()) {
//         //    newPowerSpectrum[localF0] *= 1.5f;
//         //    //m *= m;
//         //    i++;
//         //    localF0 = int(std::round(globalFrequency * float(i * delayLength) / model->sampleRate));
//         //}
//         
//         std::vector<std::complex<float>> freqsL(newPowerSpectrum.size());
//         std::vector<std::complex<float>> freqsR(newPowerSpectrum.size());
//
//         float randomPhaseL = PHASES_NORM(generator);
//         float randomPhaseR = PHASES_NORM(generator);
//         for (size_t i = 0; i < freqsL.size(); i++)
//         {
//             //newPowerSpectrum[i] += std::abs(POWER_NORM(generator));
//             randomPhaseL += PHASES_NORM(generator);
//             freqsL[i] = { newPowerSpectrum[i] * std::sin(randomPhaseL), newPowerSpectrum[i] * std::cos(randomPhaseL) };
//
//             randomPhaseR += PHASES_NORM(generator);
//             freqsR[i] = { newPowerSpectrum[i] * std::sin(randomPhaseR), newPowerSpectrum[i] * std::cos(randomPhaseR) };
//         }
//         delayL = irfft(freqsL);
//         delayR = irfft(freqsR);
//
//
//         float maxDelayAmpL = 0.0;
//         float maxDelayAmpR = 0.0;
//         for (size_t i = 0; i < delayL.size(); i++)
//         {
//             float currDelayAmpL = std::abs(delayL[i]);
//             if (currDelayAmpL > maxDelayAmpL)
//             {
//                 maxDelayAmpL = currDelayAmpL;
//             }
//             float currDelayAmpR = std::abs(delayR[i]);
//             if (currDelayAmpR > maxDelayAmpR)
//             {
//                 maxDelayAmpR = currDelayAmpR;
//             }
//         }
//
//         float n = float(delayL.size() - 1);
//         //float delta = juce::MathConstants<float>::pi / n;
//         for (size_t i = 0; i < delayL.size(); i++) {
//             delayL[i] /=  maxDelayAmpL;
//             delayR[i] /= maxDelayAmpR;
//
//             float p = (2.0f / n) * (float(i) - (n / 2.0f));
//             float e = 1.0f - p * p * p * p * p * p;
//
//             //float e = std::sin(float(i * reps) * delta);
//             //e = std::powf(e, 0.2f);
//             delayL[i] *= e;
//             delayR[i] *= e;
//         }
//         auto l = delayL.size();
//         delayL.resize(2 * l, 0.0);
//         delayR.resize(2 * l, 0.0);
//         //for (size_t i = 0; i < l; i++)
//         //{
//         //    delayL[l + i] = -delayR[i];
//         //    delayR[l + i] = -delayL[i];
//         //}
//         DBG("end of start DigitalWaveguide");
//         return;
//     }
//     std::vector<float> step() {
//         DBG("step start");
//         size_t pr = currentStep % delayL.size();
//         size_t pl = (currentStep + delayL.size() / 2) % delayL.size();
//
//         float wL = (delayL[pr] + delayL[pl]) / 2.0;
//         float wR = (delayR[pr] + delayR[pl]) / 2.0;
//
//         float wAvgL = 0.0;
//         float wAvgR = 0.0;
//         for (size_t i = pr; i < pr + smoothing; i++)
//         {
//             auto idx = i % delayL.size();
//             wAvgL += delayL[idx];
//             wAvgR += delayR[idx];
//         }
//         wAvgL /= float(smoothing);
//         wAvgR /= float(smoothing);
//         delayL[pr] = -1 * wAvgL * sustain;
//         delayL[pl] *= -POWER_NORM(generator);
//         delayR[pr] = -1 * wAvgR * sustain;
//         delayR[pl] *= -POWER_NORM(generator);
//
//         currentStep++;
//         DBG("step end");
//         return { wL, wR };
//     }
// };

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
public:
    NeuralModel* MI;// = NeuralModel(); // = ModelInfo::instance();
    //customSynth* CS;
    double lastActive = juce::Time::getMillisecondCounterHiRes();
    float tailOff = 0.0;
    bool  voiceIsActive = false;
    pianoVoice(NeuralModel* _MI) {
        //mp = ManualPiano();
        MI = _MI;
        //MI->sampleRate = getSampleRate();
        targetAmps = std::vector<float>(MI->outputShape[0], 0);
        currentAmps = std::vector<float>(MI->outputShape[0], 0);
        phasesC1 = std::vector<float>(MI->outputShape[0], 0);
        phasesC2 = std::vector<float>(MI->outputShape[0], 0);
        I0 = std::vector<float>(MI->inputShape[0], 0);
    };

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

private:
    ManualPiano mp;
    float currentDecay = 1.0f;
    std::future<void> fut;
    std::vector<float> targetAmps;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> currentAmps;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> phasesC1;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> phasesC2;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> I0;// = std::vector<float>(MI->inputShape[0], 0);
    std::vector<float> W = std::vector<float>(2, 0);
    long x = 0;
    bool  keyIsDown = false;
    float level = 0.0f;
    float midiKey = 0.0f;
    float f = 0.0f;
    float period = 0.0f;
    float deltaStep = 0.0f;
    float fps = 44100.0f;

    void forward() {
        MI->eval(I0, targetAmps);

        //float ampsSum = 0.0;
        //for (size_t i = 0; i < targetAmps.size(); i++)
        //{
        //    ampsSum += targetAmps[i];
        //}
        //for (size_t i = 0; i < targetAmps.size(); i++)
        //{
        //    targetAmps[i] /= ampsSum;
        //}
        return;
    };
};

