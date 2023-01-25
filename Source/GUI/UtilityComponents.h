/*
  ==============================================================================

    UtilityComponents.h
    Created: 24 Jan 2023 7:36:27pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PlaceHolder : juce::Component
{
    PlaceHolder();

    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColour);
    }

    juce::Colour customColour;
};

struct RotarySlider : juce::Slider
{
    RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};