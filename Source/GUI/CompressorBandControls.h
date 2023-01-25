/*
  ==============================================================================

    CompressorBandControls.h
    Created: 24 Jan 2023 7:25:19pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "../DSP/Params.h"

struct CompressorBandControls : juce::Component
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apv);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;//, ratioSlider;
    RatioSlider ratioSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment, releaseSliderAttachment, thresholdSliderAttachment, ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;
    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    void updateAttachments();
};