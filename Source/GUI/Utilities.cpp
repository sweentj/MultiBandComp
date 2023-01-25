/*
  ==============================================================================

    Utilities.cpp
    Created: 24 Jan 2023 7:36:39pm
    Author:  tjswe

  ==============================================================================
*/

#include "Utilities.h"

void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;

    g.setColour(Colours::blueviolet);
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
}

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix)
{
    juce::String str;

    auto val = (getLow) ? param.getNormalisableRange().start : param.getNormalisableRange().end;

    if (truncateKiloValue(val))
    {
        str << val << "k" << suffix;
    }
    else {
        str << val << suffix;
    }

    return str;
}