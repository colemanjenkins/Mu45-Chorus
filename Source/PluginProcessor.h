/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45LFO/Mu45LFO.h"
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.1/DelayA.h"
#include "StkLite-4.6.1/BiQuad.h"
#include "Defines.h"

//==============================================================================
/**
*/
class ColemanJP04ChorusAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ColemanJP04ChorusAudioProcessor();
    ~ColemanJP04ChorusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColemanJP04ChorusAudioProcessor)
    
    juce::AudioParameterFloat* depthParam; // range of LFO in %
    juce::AudioParameterFloat* rateParam; // oscillation speed in Hz
    juce::AudioParameterFloat* focusParam; // highpass corner frequency
    juce::AudioParameterFloat* wetParam; // wet gain in %
    juce::AudioParameterFloat* dryParam; // dry gain in %
    juce::AudioParameterFloat* stereoParam; // relative phase of left and right LFO
    juce::AudioParameterFloat* delayParam; // center point of delay in ms
    
    // "focus" high pass filters
    stk::BiQuad leftHPF;
    stk::BiQuad rightHPF;
    
    // delay LFOs
    Mu45LFO leftLFO;
    Mu45LFO rightLFO;
    
    // interpolating delay lines
    stk::DelayA leftDelayLine;
    stk::DelayA rightDelayLine;
    
    float fs; // sampling rate
    float wetGain; // linear wet gain
    float dryGain; // linear dry gain
    float sampleDepth; // depth of LFO in number of samples
    float sampleDelay; // midpoint delay of LFO in number of samples

    void calcAlgorithmParams();
    float calcDelaySampsFromMs(float ms){ return std::ceil(ms*(fs/1000.0)); }
};
