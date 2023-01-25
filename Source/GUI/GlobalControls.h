/*
  ==============================================================================

    GlobalControls.h
    Created: 24 Jan 2023 7:25:08pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "../DSP/Params.h"

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment, midHighXoverSliderAttachment, inGainSliderAttachment, outGainSliderAttachment;
};