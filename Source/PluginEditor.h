/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Defines.h"

//==============================================================================
/**
*/
class ColemanJP04ChorusAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::Slider::Listener, public juce::Timer
{
public:
    ColemanJP04ChorusAudioProcessorEditor (ColemanJP04ChorusAudioProcessor&);
    ~ColemanJP04ChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ColemanJP04ChorusAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColemanJP04ChorusAudioProcessorEditor)
    
    juce::Slider rateSlider;
    juce::Slider delaySlider;
    juce::Slider depthSlider;
    
    juce::Slider focusSlider;
    juce::Slider stereoSlider;
    
    juce::Slider wetSlider;
    juce::Slider drySlider;
        
    enum parameterMap {
        depth,
        rate,
        focus,
        wet,
        dry,
        stereo,
        delay
    };
    
    struct SliderToParam {
        juce::Slider* slider;
        parameterMap param;
    };
    
    std::vector<SliderToParam> sliderParamMap {
        {&rateSlider, rate},
        {&delaySlider, delay},
        {&depthSlider, depth},
        {&focusSlider, focus},
        {&stereoSlider, stereo},
        {&wetSlider, wet},
        {&drySlider, dry}
    };
    
    void createKnob(juce::Slider& slider, float x, float y, std::string suffix,
                    float interval, float skew, parameterMap paramNum);
    void createSlider(juce::Slider& slider, float x, float y,
                      parameterMap paramNum);
};
