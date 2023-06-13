#pragma once
#include "Voices.h"
#include <typeinfo>
//#define DECAY

bool isSustainOn = false;

void pianoVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*synthesiserSound*/, int /*currentPitchWheelValue*/)
{    
    keyIsDown = true;
    voiceIsActive = true;
    currentDecay = 1.0f;
    //lastActive = juce::Time::getMillisecondCounterHiRes();
    tailOff = 1.0f;
    level = velocity;
    x = 0;
    midiKey = midiNoteNumber;
    fps = float(getSampleRate());
    //DBG(">> fps=" + std::to_string(fps));
    for (size_t i = 0; i < phasesC1.size(); i++)
    {
        phasesC1[i] = PHASES_NORM(generator);
        phasesC2[i] = PHASES_NORM(generator);
    }
    float pitch = (midiKey - 21.0) / 87.0;

    I0 = { pitch,  level, 0.0 };
    fut = std::async(std::launch::async, &pianoVoice::forward, this);
    mp.start(midiKey, fps);
    //f = MI->sampleRate * partialFromMidiKey(midiKey) / 44100.0f;
    f = partialFromMidiKey(midiKey);
    period = fps / f;
    deltaStep = juce::MathConstants<float>::twoPi * f / fps;

    while (! (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready))
    {
         //DBG("waiting");
    }
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] = targetAmps[i];
    }
}

void pianoVoice::stopNote(float velocity, bool allowTailOff)
{
    keyIsDown = false;
    //lastActive = juce::Time::getMillisecondCounterHiRes();
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
    if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        float pc = std::tanh(currentPeriod / MAX_NUMBER_OF_PERIODS);
        float pitch = (midiKey - 21.0) / 87.0;
        I0 = { pitch,  level, pc };
        fut = std::async(std::launch::async, &pianoVoice::forward, this);
    }    
    //float ampsSum = 0.0f;
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        currentAmps[i] += (targetAmps[i] - currentAmps[i]) * 2000.0f / fps;
        //ampsSum += currentAmps[i];
    }
    //for (size_t i = 0; i < currentAmps.size(); i++)
    //{
    //    currentAmps[i] /= ampsSum;
    //}

    float currentAttack =  6.0f * currentPeriod;

    float dPart = -0.005f * currentPeriod;
    currentDecay = 1.0f / (1.0f + dPart * dPart);

    //currentDecay = std::expf(-0.005f * currentPeriod);
    float m = std::min(currentAttack, currentDecay) * level;
    for (size_t i = 0; i < currentAmps.size(); i++)
    {
        float stepLocal = float(i + 1) * step;
        W[0] += currentAmps[i] * std::sin(phasesC1[i] + stepLocal);
        W[1] += currentAmps[i] * std::sin(phasesC2[i] + stepLocal);
    }
    
    std::vector<float> WD = mp.step();
    //std::vector<float> WD = { 0.0f,0.0f };
    float alpha = 0.0f;
    float eq = 0.7f;
    W[0] = W[0] * alpha * eq + WD[0] * (1.0 - alpha);
    W[1] = W[1] * alpha * eq + WD[1] * (1.0 - alpha);

    //W[0] *= m;
    //W[1] *= m;
    x++;
    return;
}

void pianoVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    //DBG(std::to_string(isSustainOn));
    //DBG(currentDecay);
    if (keyIsDown)
    {
        while (--numSamples >= 0)
        {   
            getNextSample();
            float channels = (float) outputBuffer.getNumChannels();
            for (int i = 0; i < channels; i++)
                outputBuffer.addSample(i, startSample, W[i] / channels);
            startSample++;
        }
    }
    else if(voiceIsActive)
    {        
        if (isSustainOn)
        {
            if (currentDecay > 0.01)
            {
                while (--numSamples >= 0)
                {
                    getNextSample();
                    float channels = outputBuffer.getNumChannels();
                    for (int i = 0; i < channels; i++)
                        outputBuffer.addSample(i, startSample, tailOff * W[i] / channels);
                    startSample++;
                }
            }
            else {
                lastActive = juce::Time::getMillisecondCounterHiRes();
                clearCurrentNote();
                DBG("clearCurrentNote 2 called");
                voiceIsActive = false;
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
                clearCurrentNote();
                DBG("clearCurrentNote called");
                voiceIsActive = false;
                lastActive = juce::Time::getMillisecondCounterHiRes();
            }
        } 
    }    
}