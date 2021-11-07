/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void ColemanJP04ChorusAudioProcessorEditor::createKnob(juce::Slider& slider, float x, float y, std::string suffix,
                                                       float interval, float skew, parameterMap paramNum) {
    auto& params = processor.getParameters();
    
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*) params.getUnchecked(paramNum);
    slider.setBounds(x*UNIT_LENGTH_X, y*UNIT_LENGTH_Y, KNOB_WIDTH, KNOB_HEIGHT);
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UNIT_LENGTH_X*3, UNIT_LENGTH_Y*.75);
    slider.setTextValueSuffix(suffix);
    slider.setRange(audioParam->range.start, audioParam->range.end, interval);
    slider.setSkewFactor(skew);
    slider.addListener(this);
    addAndMakeVisible(slider);
}

void ColemanJP04ChorusAudioProcessorEditor::createSlider(juce::Slider& slider, float x, float y,
                                                       parameterMap paramNum) {
    auto& params = processor.getParameters();
    
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*) params.getUnchecked(paramNum);
    slider.setBounds(x*UNIT_LENGTH_X, y*UNIT_LENGTH_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, UNIT_LENGTH_X*3, UNIT_LENGTH_Y*.75);
    slider.setTextValueSuffix(_PCT);
    slider.setRange(audioParam->range.start, audioParam->range.end, DEFAULT_INTERVAL);
    slider.addListener(this);
    addAndMakeVisible(slider);
}


//==============================================================================
ColemanJP04ChorusAudioProcessorEditor::ColemanJP04ChorusAudioProcessorEditor (ColemanJP04ChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (CONTAINER_WIDTH, CONTAINER_HEIGHT);
    
    createKnob(rateSlider, 1, 1, RATE_SUFFIX, RATE_INTERVAL, RATE_SKEW, rate);
    createKnob(delaySlider, 6, 1, DELAY_SUFFIX, DEFAULT_INTERVAL, NO_SKEW, delay);
    createKnob(depthSlider, 11, 1, DEPTH_SUFFIX, DEFAULT_INTERVAL, NO_SKEW, depth);
    
    createKnob(focusSlider, 3, 7, FOCUS_SUFFIX, FOCUS_INTERVAL, FOCUS_SKEW, focus);
    createKnob(stereoSlider, 9, 7, STEREO_SUFFIX, STERO_INTERVAL, NO_SKEW, stereo);
    
    createSlider(drySlider, 17, 3, dry);
    createSlider(wetSlider, 21, 3, wet);
    
    startTimer(20); // update GUI from parameters every 20 ms, useful for param automation and saving state
}

void ColemanJP04ChorusAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
    auto& params = processor.getParameters();
    
    for (auto& slider_param : sliderParamMap) {
        if (slider == slider_param.slider) {
            // set parameter from slider value
            juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(slider_param.param);
            *audioParam = slider_param.slider->getValue();
            break;
        }
    }
}

void ColemanJP04ChorusAudioProcessorEditor::timerCallback() {
    auto& params = processor.getParameters();

    for (auto& slider_param : sliderParamMap) {
        // set slider value from parameter
        juce::AudioParameterFloat* param = (juce::AudioParameterFloat*)params.getUnchecked(slider_param.param);
        slider_param.slider->setValue(param->get(), juce::dontSendNotification);
    }
}

ColemanJP04ChorusAudioProcessorEditor::~ColemanJP04ChorusAudioProcessorEditor()
{
}

//==============================================================================
void ColemanJP04ChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::lightblue);
    
    g.drawLine(1*UNIT_LENGTH_X, 6*UNIT_LENGTH_Y, 15*UNIT_LENGTH_X, 6*UNIT_LENGTH_Y);
    g.drawLine(16*UNIT_LENGTH_X, 1*UNIT_LENGTH_Y, 16*UNIT_LENGTH_X, 11*UNIT_LENGTH_Y);
    
    g.setColour (juce::Colours::skyblue);
    g.setFont (juce::Font(17.0f, juce::Font::bold));
    g.drawText("Rate", 1*UNIT_LENGTH_X, 0.3*UNIT_LENGTH_Y, KNOB_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    g.drawText("Delay", 6*UNIT_LENGTH_X, 0.3*UNIT_LENGTH_Y, KNOB_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    g.drawText("Depth", 11*UNIT_LENGTH_X, 0.3*UNIT_LENGTH_Y, KNOB_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    
    g.drawText("Focus", 3*UNIT_LENGTH_X, 6.3*UNIT_LENGTH_Y, KNOB_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    g.drawText("Stereo", 9*UNIT_LENGTH_X, 6.3*UNIT_LENGTH_Y, KNOB_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    
    g.drawText("Dry", 17*UNIT_LENGTH_X, 2*UNIT_LENGTH_Y, SLIDER_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
    g.drawText("Wet", 21*UNIT_LENGTH_X, 2*UNIT_LENGTH_Y, SLIDER_WIDTH, 1*UNIT_LENGTH_Y, juce::Justification::centred);
}

void ColemanJP04ChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
