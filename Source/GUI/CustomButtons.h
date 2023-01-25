/*
  ==============================================================================

    CustomButtons.h
    Created: 24 Jan 2023 7:26:00pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;
    //{
    //    auto bounds = getLocalBounds();
    //    auto insetRect = bounds.reduced(4);

    //    randomPath.clear();

    //    juce::Random r;

    //    randomPath.startNewSubPath(insetRect.getX(),
    //        insetRect.getY() + insetRect.getHeight() * r.nextFloat());

    //    for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
    //    {
    //        randomPath.lineTo(x,
    //            insetRect.getY() + insetRect.getHeight() * r.nextFloat());
    //    }
    //}

    juce::Path randomPath;
};