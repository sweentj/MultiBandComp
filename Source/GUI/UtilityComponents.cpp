/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 24 Jan 2023 7:36:27pm
    Author:  tjswe

  ==============================================================================
*/

#include "UtilityComponents.h"



PlaceHolder::PlaceHolder()
{
    juce::Random r;
    customColour = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}