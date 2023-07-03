#pragma once
#include "Voices.h"
#include <typeinfo>
//#define DECAY

bool isSustainOn = false;

void pianoVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*synthesiserSound*/, int /*currentPitchWheelValue*/)
{   
    isSounding = true;
    keyIsDown = true;
    midiKey = midiNoteNumber;
    level = velocity;
    float pitch = (midiKey - 21.0f) / 87.0f;
    // Start NN model
    I0 = { pitch,  level, 0.0f };
    fut = std::async(std::launch::async, &pianoVoice::forward, this);
    lastActivated = juce::Time::getMillisecondCounterHiRes();
    currentDecay = 1.0f; 
    tailOff = 1.0f;
    fps = float(getSampleRate());
    tailOffRatio = DEFAULT_TAILOFF_RATIO;
    x = 0;
    for (size_t i = 0; i < phasesC1.size(); i++)
    {
        phasesC1[i] = PHASES_NORM(generator);
        phasesC2[i] = PHASES_NORM(generator);
    }
    float f = partialFromMidiKey(midiKey);
    fLocal = f * n / fps;
    period = fps / fLocal;
    deltaStep = juce::MathConstants<float>::twoPi * f / fps;
    // Start physical model
    t = 0;
    if (midiKey <= 10 + 20)
    {
        harmonics = G1F;
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

    phasesL.clear();
    phasesR.clear();
    phasesL.push_back(PHASES_NORM(generator));
    phasesR.push_back(PHASES_NORM(generator));
    for (size_t i = 0; i < harmonics.size() - 1; i++)
    {
        phasesL.push_back(phasesL.back() + PHASES_NORM(generator));
        phasesR.push_back(phasesR.back() + PHASES_NORM(generator));
    }
    bool futureReady = (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
    bool delayPassed = (juce::Time::getMillisecondCounterHiRes() - lastActivated > 100.0);
    while (!(/*delayPassed && */futureReady))
    {
        addOneItemToQueue();
        futureReady = (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
        delayPassed = (juce::Time::getMillisecondCounterHiRes() - lastActivated > 100.0);
    }
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] = targetAmps[i];
    }    
}

void pianoVoice::stopNote(float velocity, bool allowTailOff)
{
    keyIsDown = false;
    if (!allowTailOff)
    {
        tailOff = 0.0; // stopNote method could be called more than once.
    }
}

void pianoVoice::pitchWheelMoved(int newValue){}

void pianoVoice::controllerMoved(int controllerNumber, int newValue) {}

void pianoVoice::getNextSample() {
    float xFloat = float(x);
    float step = xFloat * deltaStep;
    float currentPeriod = xFloat / period;
    float pc = std::tanh(currentPeriod / MAX_NUMBER_OF_PERIODS);
    float pitch = (midiKey - 21.0) / 87.0;
    if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        I0 = { pitch,  level, pc };
        fut = std::async(std::launch::async, &pianoVoice::forward, this);
    }    
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] += (targetAmps[i] - currentAmps[i]) * 2000.0f / fps;
    }

    float currentAttack =  6.0f * currentPeriod;
    float dPart = -0.003f * currentPeriod;
    currentDecay = 1.0f / (1.0f + dPart * dPart);

    float m = std::min(currentAttack, currentDecay) * level;
    W[0] = 0.0f;
    W[1] = 0.0f;
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        float stepLocal = float(i + 1) * step;
        W[0] += currentAmps[i] * std::sin(phasesC1[i] + stepLocal);
        W[1] += currentAmps[i] * std::sin(phasesC2[i] + stepLocal);
    }
    std::vector<float> WD = get();
    
    float alpha = 1.0f - (1.0f - pitch) * 0.95f;
    float beta = 1.0f - alpha;
    W[0] = W[0] * 0.7f * alpha + beta * WD[0];
    W[1] = W[1] * 0.7f * alpha + beta * WD[1];

    W[0] *= m;
    W[1] *= m;
    x++;
    return;
}

void pianoVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if(isSounding)
    {        
        //DBG(W[0]);
        float channels = outputBuffer.getNumChannels();
        if (isSustainOn || keyIsDown)
        {
            if (currentDecay > 0.001)
            {
                
                while (--numSamples >= 0)
                {
                    getNextSample();                    
                    for (int i = 0; i < channels; i++)
                        outputBuffer.addSample(i, startSample, tailOff * W[i] / channels);
                    startSample++;
                }
            }
            else {
                clearCurrentNote();
                DBG("clearCurrentNote 2 called");
                isSounding = false;
                Q = {};
            }
        }
        else
        {
            if (tailOff > 0.01)
            {
                while (--numSamples >= 0)
                {
                    getNextSample();                    
                    for (int i = 0; i < channels; i++)
                        outputBuffer.addSample(i, startSample, tailOff * W[i] / channels);
                    startSample++;
                    tailOff *= tailOffRatio;
                }
            }
            else {
                tailOff = 0.0;
                clearCurrentNote();
                DBG("clearCurrentNote called");
                isSounding = false;
                Q = {};
            }
        } 
    }    
}