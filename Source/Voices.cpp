#pragma once
#include "Voices.h"
#include <random>
#include <typeinfo>
#define DECAY
static std::random_device rd;  //Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
static std::uniform_int_distribution<> distrib(0, 1000);

float frequencyFromMidiKey(float k) {
    return 440.0 * std::powf(2.0, (k - 69.0) / 12.0);
}




void pianoVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*synthesiserSound*/, int /*currentPitchWheelValue*/)
{    
    lastActive = juce::Time::getMillisecondCounterHiRes();
    tailOff = 1.0;
    level = velocity;
    x = 0;
    midiKey = midiNoteNumber;
    for (size_t i = 0; i < phasesC1.size(); i++)
    {
        phasesC1[i] = juce::MathConstants<float>::twoPi * float(distrib(gen)) / 1000.0;
        phasesC2[i] = juce::MathConstants<float>::twoPi * float(distrib(gen)) / 1000.0;
    }
    float pitch = (midiKey - 1.0) / 107.0;

    I0 = { pitch,  level, 0.0 };
    fut = std::async(std::launch::async, forward);


    while (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
         //DBG("waiting");
    }
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] = targetAmps[i];
    }
    isPlaying = true;
}

void pianoVoice::stopNote(float velocity, bool allowTailOff)
{
    isPlaying = false;
    lastActive = juce::Time::getMillisecondCounterHiRes();
    if (!allowTailOff)
    {
        tailOff = 0.0; // stopNote method could be called more than once.
    }
}

void pianoVoice::pitchWheelMoved(int newValue){}

void pianoVoice::controllerMoved(int controllerNumber, int newValue)  {}

void pianoVoice::getNextSample() {
    DBG("FPS=" << MI.sampleRate);
    float xFloat = float(x);
    float f = frequencyFromMidiKey(midiKey);
    float step = juce::MathConstants<float>::twoPi * xFloat * f / MI.sampleRate;
    float period = MI.sampleRate / f;
    float currentPeriod = xFloat / period;
    if (true && fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        float pc = std::tanh(currentPeriod / MAX_NUMBER_OF_PERIODS);
        float pitch = (midiKey - 1.0) / 107.0;
        I0 = { pitch,  level, pc };
        fut = std::async(std::launch::async, forward);

       
        DBG("currentPeriod=" + std::to_string(currentPeriod) +
            " period=" + std::to_string(period) +
            " key=" + std::to_string(midiKey) +
            " sampleRate(hz)=" + std::to_string(MI.sampleRate) +
            " level=" + std::to_string(level) +
            " pitch=" + std::to_string(pitch) +
            " pc=" + std::to_string(pc) + "\n");
    }    
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] += (targetAmps[i] - currentAmps[i]) * 100.0 / MI.sampleRate;
    }
    W[0] = 0.0;
    W[1] = 0.0;
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        float stepLocal = float(i + 1) * step;
        W[0] += currentAmps[i] * std::sin(phasesC1[i] + stepLocal);
        W[1] += currentAmps[i] * std::sin(phasesC2[i] + stepLocal);
    }
    float currentAttack = std::min(1.0f, xFloat / (0.05f * MI.sampleRate));
    float currentDecay = std::expf(-0.007f * currentPeriod);
    float m = std::min(1.0f, level * currentAttack * currentDecay);
    W[0] *= m;
    W[1] *= m;
    x++;
    return;
}

void pianoVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (isPlaying)
    {
        while (--numSamples >= 0)
        {   
            getNextSample();
            float channels = outputBuffer.getNumChannels();
            for (int i = 0; i < channels; i++)
                outputBuffer.addSample(i, startSample, W[i] / channels);
            startSample++;
        }
    }
    else
    {
        if (tailOff > 0.0001)
        {
            while (--numSamples >= 0)
            {                    
                getNextSample();
                float channels = outputBuffer.getNumChannels();
                for (int i = 0; i < channels; i++)
                    outputBuffer.addSample(i, startSample, tailOff * W[i] / channels);
                startSample++;
                tailOff *= 0.99;
            }
        }
        else {
            tailOff = 0.0;    
            lastActive = juce::Time::getMillisecondCounterHiRes();
            clearCurrentNote();
        }        
    }    
}