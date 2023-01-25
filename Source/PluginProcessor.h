/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

/*
Roadmapk
1) split audio into 3 bands DONE
2) create parameters to control where split happens DONE
3) test that splitting doesn't create audibe artifacts DONE
4) create parameters for each compressor bands DONE
5) add two remaining compressor DONE
6) mute, solo, bypass individual compressors DONE
7) add input and output gain  DONE
8) cleanup DONE
*/

#include <JuceHeader.h>
#include "DSP/CompressorBand.h"

//==============================================================================
/**
*/
class MultiBandCompAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MultiBandCompAudioProcessor();
    ~MultiBandCompAudioProcessor() override;

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

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts {*this, nullptr, "Parameters", createParameterLayout() };

private:

    //CompressorBand compressorLow;
    std::array<CompressorBand, 3> compressorArray;
    CompressorBand& lowBandComp = compressorArray[0];
    CompressorBand& midBandComp = compressorArray[1];
    CompressorBand& highBandComp = compressorArray[2];


    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    
    //      fc0     fc1
    Filter  LP1,    AP2,    //freq band 1
            HP1,    LP2,    //freq band 2
                    HP2;    //freq band 3
    //Filter invAP1, invAP2;
    //juce::AudioBuffer<float> invAPBuffer;

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };


    std::array < juce::AudioBuffer<float>, 3> filterBuffers;

    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam;
    juce::AudioParameterFloat* outputGainParam;

    template <typename T, typename U>
    void applyGain(T& buffer, U& gain) {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }

    void updateState();
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBandCompAudioProcessor)
};
