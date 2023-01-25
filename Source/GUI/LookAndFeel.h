/*
  ==============================================================================

    LookAndFeel.h
    Created: 24 Jan 2023 7:08:08pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "CustomButtons.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};