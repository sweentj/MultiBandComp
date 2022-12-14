/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultiBandCompAudioProcessor::MultiBandCompAudioProcessor()
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
    using namespace Params;

    const auto& params = GetParams();

    //helper lambda function that assigns compressor paramter to APVTS param
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    floatHelper(lowBandComp.attack, Names::Attack_Low_Band);
    floatHelper(lowBandComp.release, Names::Release_Low_Band);
    floatHelper(lowBandComp.threshold, Names::Threshold_Low_Band);

    floatHelper(midBandComp.attack, Names::Attack_Mid_Band);
    floatHelper(midBandComp.release, Names::Release_Mid_Band);
    floatHelper(midBandComp.threshold, Names::Threshold_Mid_Band);

    floatHelper(highBandComp.attack, Names::Attack_High_Band);
    floatHelper(highBandComp.release, Names::Release_High_Band);
    floatHelper(highBandComp.threshold, Names::Threshold_High_Band);
    
    //set up Filters
    floatHelper(lowMidCrossover, Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Names::Mid_High_Crossover_Freq);

    floatHelper(inputGainParam, Names::Gain_In);
    floatHelper(outputGainParam, Names::Gain_Out);


    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    //invAP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    //invAP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);


    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    boolHelper(lowBandComp.bypassed, Names::Bypass_Low_Band);
    boolHelper(midBandComp.bypassed, Names::Bypass_Mid_Band);
    boolHelper(highBandComp.bypassed, Names::Bypass_High_Band);
    boolHelper(lowBandComp.mute, Names::Mute_Low_Band);
    boolHelper(midBandComp.mute, Names::Mute_Mid_Band);
    boolHelper(highBandComp.mute, Names::Mute_High_Band);
    boolHelper(lowBandComp.solo, Names::Solo_Low_Band);
    boolHelper(midBandComp.solo, Names::Solo_Mid_Band);
    boolHelper(highBandComp.solo, Names::Solo_High_Band);

    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    choiceHelper(lowBandComp.ratio, Names::Ratio_Low_Band);
    choiceHelper(midBandComp.ratio, Names::Ratio_Mid_Band);
    choiceHelper(highBandComp.ratio, Names::Ratio_High_Band);

}

MultiBandCompAudioProcessor::~MultiBandCompAudioProcessor()
{
}

//==============================================================================
const juce::String MultiBandCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultiBandCompAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultiBandCompAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultiBandCompAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultiBandCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultiBandCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultiBandCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultiBandCompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultiBandCompAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultiBandCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultiBandCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    for (auto& comp : compressorArray) {
        comp.prepare(spec);
    }
    //compressor.prepare(spec);

    LP1.prepare(spec);
    HP1.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    AP2.prepare(spec);

    //invAP1.prepare(spec);
    //invAP2.prepare(spec);
    //invAPBuffer.setSize(spec.numChannels, samplesPerBlock);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    inputGain.setRampDurationSeconds(0.05); //50ms
    outputGain.setRampDurationSeconds(0.05);


    for (auto& buffer : filterBuffers) {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void MultiBandCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultiBandCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MultiBandCompAudioProcessor::updateState() {
    
    for (auto& comp : compressorArray) {
        comp.updateCompressorSettings();
    }

    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());

    auto lowMidCutoffFreq = lowMidCrossover->get();
    auto midHighCutoffFreq = midHighCrossover->get();

    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    //invAP1.setCutoffFrequency(lowMidCutoffFreq);  //filter testing

    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
    //invAP2.setCutoffFrequency(midHighCutoffFreq); //filter testing

}

void MultiBandCompAudioProcessor::splitBands(const juce::AudioBuffer<float> &inputBuffer) {
    
    for (auto& fB : filterBuffers) {
        fB = inputBuffer;
    }
    //invAPBuffer = buffer; //filter testing

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);


    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);

    HP1.process(fb1Ctx);
    filterBuffers[2] = filterBuffers[1];
    //fb2Block = fb1Block; //can't do it this way
    LP2.process(fb1Ctx);

    HP2.process(fb2Ctx);

    //used for filter testing
    /*auto invAPBlock = juce::dsp::AudioBlock<float>(invAPBuffer);
    auto invAPCtx = juce::dsp::ProcessContextReplacing<float>(invAPBlock);
    invAP1.process(invAPCtx);
    invAP2.process(invAPCtx);*/



    //if compressor bypassed, multiply the inverse the allpass and add it to buffer
    //should produce silence when bypassed
    /*if (compressor.bypassed->get()) {
        for (auto ch = 0; ch < numChannels; ch++) {
            juce::FloatVectorOperations::multiply(invAPBuffer.getWritePointer(ch), -1.f, numSamples);
        }
        addFilterBand(buffer, invAPBuffer);
    }*/

    //process the filters in the right filter band
}

void MultiBandCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    updateState();

    applyGain(buffer, inputGain);

    splitBands(buffer);

    for (size_t i = 0; i < filterBuffers.size(); i++) {
        compressorArray[i].process(filterBuffers[i]);
    }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source) {
        for (auto i = 0; i < nc; ++i) {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    auto bandsAreSoloed = false;
    for (auto& comp : compressorArray) {
        if (comp.solo->get()) {
            bandsAreSoloed = true;
            break;
        }
    }

    //addFilterBand(buffer, filterBuffers[0]);
    //addFilterBand(buffer, filterBuffers[1]);
    //addFilterBand(buffer, filterBuffers[2]);

    if (bandsAreSoloed) {
        for (size_t i = 0; i < compressorArray.size(); i++) {
            auto& comp = compressorArray[i];
            if (comp.solo->get()) {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }
    else {
        for (size_t i = 0; i < compressorArray.size(); i++) {
            auto& comp = compressorArray[i];
            if (!comp.mute->get()) {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }

    applyGain(buffer, outputGain);

}

//==============================================================================
bool MultiBandCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultiBandCompAudioProcessor::createEditor()
{
    return new MultiBandCompAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MultiBandCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

}

void MultiBandCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiBandCompAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MultiBandCompAudioProcessor::createParameterLayout() {

    APVTS::ParameterLayout layout;

    using namespace juce;
    using namespace Params;

    const auto& params = GetParams();

    auto gainRange = NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Gain_In),
                                                        params.at(Names::Gain_In),
                                                        gainRange,
                                                        0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Gain_Out),
                                                        params.at(Names::Gain_Out),
                                                        gainRange,
                                                        0));

    auto thresholdRange = NormalisableRange<float>(-60, 12, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>( params.at(Names::Threshold_Low_Band),
                                                        params.at(Names::Threshold_Low_Band),
                                                        thresholdRange, 
                                                        0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Mid_Band),
                                                        params.at(Names::Threshold_Mid_Band),
                                                        thresholdRange,
                                                        0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_High_Band),
                                                        params.at(Names::Threshold_High_Band),
                                                        thresholdRange,
                                                        0));

    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band), 
                                                        params.at(Names::Attack_Low_Band),
                                                        attackReleaseRange, 
                                                        50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Mid_Band),
                                                        params.at(Names::Attack_Mid_Band),
                                                        attackReleaseRange,
                                                        50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_High_Band),
                                                        params.at(Names::Attack_High_Band),
                                                        attackReleaseRange,
                                                        50));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band), 
                                                        params.at(Names::Release_Low_Band),
                                                        attackReleaseRange, 
                                                        250));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Mid_Band),
                                                        params.at(Names::Release_Mid_Band),
                                                        attackReleaseRange,
                                                        250));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_High_Band),
                                                        params.at(Names::Release_High_Band),
                                                        attackReleaseRange,
                                                        250));
    

    auto choices = std::vector<double>{ 1,1.5,2,3,4,5,6,7,8,10,15,20,50,100 };
    StringArray sa;
    for (auto choice : choices) {
        sa.add(String(choice,1));
    }
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Low_Band),
                                                         params.at(Names::Ratio_Low_Band),
                                                         sa, 
                                                         3));
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Mid_Band),
                                                        params.at(Names::Ratio_Mid_Band),
                                                        sa,
                                                        3));
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_High_Band),
                                                        params.at(Names::Ratio_High_Band),
                                                        sa,
                                                        3));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypass_Low_Band), 
                                                    params.at(Names::Bypass_Low_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypass_Mid_Band),
                                                    params.at(Names::Bypass_Mid_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypass_High_Band),
                                                    params.at(Names::Bypass_High_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_Low_Band),
                                                    params.at(Names::Mute_Low_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_Mid_Band),
                                                    params.at(Names::Mute_Mid_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_High_Band),
                                                    params.at(Names::Mute_High_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_Low_Band),
                                                    params.at(Names::Solo_Low_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_Mid_Band),
                                                    params.at(Names::Solo_Mid_Band),
                                                    false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_High_Band),
                                                    params.at(Names::Solo_High_Band),
                                                    false));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq),
                                                    params.at(Names::Low_Mid_Crossover_Freq),
                                                    NormalisableRange<float>(20, 999, 1, 1),
                                                    400));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),
                                                    params.at(Names::Mid_High_Crossover_Freq),
                                                    NormalisableRange<float>(1000, 20000, 1, 1),
                                                    2000));

    return layout;
}