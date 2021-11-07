/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ColemanJP04ChorusAudioProcessor::ColemanJP04ChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(depthParam = new juce::AudioParameterFloat("depth",
                                                            "Depth %",
                                                            DEPTH_MIN,
                                                            DEPTH_MAX,
                                                            DEPTH_DEFAULT));
    
    juce::NormalisableRange<float> rateRange = juce::NormalisableRange<float>(
        RATE_MIN, RATE_MAX, RATE_INTERVAL, RATE_SKEW);
    addParameter(rateParam = new juce::AudioParameterFloat("rate",
                                                            "Rate (Hz)",
                                                            rateRange,
                                                            RATE_DEFAULT));
    juce::NormalisableRange<float> focusRange = juce::NormalisableRange<float>(
        FOCUS_MIN, FOCUS_MAX, FOCUS_INTERVAL, FOCUS_SKEW);
    addParameter(focusParam = new juce::AudioParameterFloat("focus",
                                                            "Focus (Hz)",
                                                            focusRange,
                                                            FOCUS_DEFAULT));
    addParameter(wetParam = new juce::AudioParameterFloat("wet",
                                                            "Wet %",
                                                            WET_MIN,
                                                            WET_MAX,
                                                            WET_DEFAULT));
    addParameter(dryParam = new juce::AudioParameterFloat("dry",
                                                            "Dry %",
                                                            DRY_MIN,
                                                            DRY_MAX,
                                                            DRY_DEFAULT));
    addParameter(stereoParam = new juce::AudioParameterFloat("stero",
                                                            "Stereo",
                                                            STEREO_MIN,
                                                            STEREO_MAX,
                                                            STEREO_DEFAULT));
    addParameter(delayParam = new juce::AudioParameterFloat("delay",
                                                            "Delay (ms)",
                                                            DELAY_MIN,
                                                            DELAY_MAX,
                                                            DELAY_DEFAULT));
    
}

ColemanJP04ChorusAudioProcessor::~ColemanJP04ChorusAudioProcessor()
{
}

//==============================================================================
const juce::String ColemanJP04ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ColemanJP04ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ColemanJP04ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ColemanJP04ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ColemanJP04ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ColemanJP04ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ColemanJP04ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ColemanJP04ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ColemanJP04ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void ColemanJP04ChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ColemanJP04ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    fs = sampleRate;
    
    unsigned long maxDelay = calcDelaySampsFromMs(MAX_TOTAL_DELAY);
    leftDelayLine.setMaximumDelay(maxDelay);
    rightDelayLine.setMaximumDelay(maxDelay);
}

void ColemanJP04ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ColemanJP04ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if ( // FORCE STEREO
        //layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void ColemanJP04ChorusAudioProcessor::calcAlgorithmParams() {
    wetGain = wetParam->get()/100.0;
    dryGain = dryParam->get()/100.0;
    
    leftLFO.setFreq(rateParam->get(), fs);
    rightLFO.setFreq(rateParam->get(), fs);
    
    float coeffs[5];
    Mu45FilterCalc::calcCoeffsHPF(coeffs, focusParam->get(), 1, fs);
    leftHPF.setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);
    rightHPF.setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);
    
    sampleDepth = calcDelaySampsFromMs((delayParam->get() - INST_DELAY_MIN)*depthParam->get()/100.0);
    sampleDelay = calcDelaySampsFromMs(delayParam->get());
    
    leftLFO.setPhaseOffset(stereoParam->get());
    rightLFO.setPhaseOffset(-1*stereoParam->get());
}

void ColemanJP04ChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    calcAlgorithmParams();
    
    auto* leftChannelData = buffer.getWritePointer(0);
    auto* rightChannelData = buffer.getWritePointer(1);
    
    float leftSample, rightSample, leftDelayed, rightDelayed;
    
    for (int samp = 0; samp < buffer.getNumSamples(); samp++) {
        // calculate delay length
        leftDelayLine.setDelay(sampleDelay + leftLFO.tick()*sampleDepth);
        rightDelayLine.setDelay(sampleDelay + rightLFO.tick()*sampleDepth);

        leftSample = leftChannelData[samp];
        rightSample = rightChannelData[samp];

        // apply delays
        leftDelayed = leftDelayLine.tick(leftSample);
        rightDelayed = rightDelayLine.tick(rightSample);

        // apply filters
        leftDelayed = leftHPF.tick(leftDelayed);
        rightDelayed = rightHPF.tick(rightDelayed);

        // set output
        leftChannelData[samp] = leftSample*dryGain + rightDelayed*wetGain;
        rightChannelData[samp] = rightSample*dryGain + leftDelayed*wetGain;
    }
}

//==============================================================================
bool ColemanJP04ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ColemanJP04ChorusAudioProcessor::createEditor()
{
    return new ColemanJP04ChorusAudioProcessorEditor (*this);
}

//==============================================================================
void ColemanJP04ChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xml ("Parameters");
    for (int i = 0; i < getParameters().size(); ++i)
    {
        juce::AudioParameterFloat* param = (juce::AudioParameterFloat*)getParameters().getUnchecked(i);
        juce::XmlElement* paramElement = new juce::XmlElement ("parameter" + juce::String(std::to_string(i)));
        paramElement->setAttribute ("value", param->get());
        xml.addChildElement (paramElement);
    }
    copyXmlToBinary (xml, destData);
}

void ColemanJP04ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState->hasTagName ("Parameters")) // read Parameters tag
    {
        juce::AudioParameterFloat* param;
        for (auto* element : xmlState->getChildIterator()) // loop through the saved parameter values and update them
        {
            int paramNum = std::stoi(element->getTagName().substring(9).toStdString()); // chops off beginnging "parameter"
            param = (juce::AudioParameterFloat*) getParameters().getUnchecked(paramNum);
            *param = element->getDoubleAttribute("value"); // set parameter value
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ColemanJP04ChorusAudioProcessor();
}
