/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

/*
GUI ROADMAP
    1) Global Controls (x-over sliders, gain)
    2) Main Band Controls
    3) add solo/mute/bypass buttons
    4) Band Select Functionality
    5) Band Select buttons reflect solo/mute/bypass state
    6) Custom look and feel for slider/toggles
    7) Spectrum Analyzer overview
    8) Data Structures for Spectrum Analyzer
    9) Fifo usage in pluginProcessor::ProcessBlock
    10) implementation of the analyzer rendering pre-computed paths
    11) Drawing crossovers on top of analyzer Plot
    12) Drawing gain reduction on top of analyzer
    13) Analyzer bypass
    14) Global bypass button
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/RotarySliderWithLabels.h"
#include "GUI/CustomButtons.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/GlobalControls.h"
#include "GUI/Utilities.h"
#include "GUI/UtilityComponents.h"

//==============================================================================
/**
*/



class MultiBandCompAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MultiBandCompAudioProcessorEditor (MultiBandCompAudioProcessor&);
    ~MultiBandCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LookAndFeel lnf;

    MultiBandCompAudioProcessor& audioProcessor;

    PlaceHolder controlBar, analyzer /*globalControls bandControls*/;
    GlobalControls globalControls{ audioProcessor.apvts };
    CompressorBandControls bandControls{ audioProcessor.apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBandCompAudioProcessorEditor)
};
