/*
  ==============================================================================

    Utilities.h
    Created: 24 Jan 2023 7:36:39pm
    Author:  tjswe

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds);

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);

template<typename Labels, typename ParamType, typename SuffixType>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({ 0.f, getValString(param, true, suffix) });
    labels.add({ 1.f, getValString(param, false, suffix) });

}

//making the complier figure out types
template<typename Attachment, typename APVTS, typename Params, typename ParamName, typename SliderType>
void makeAttachment(std::unique_ptr<Attachment>& attachment, APVTS& apvts, const Params& params, const ParamName& name, SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

//making the compiler figure out the types
template<typename APVTS, typename Params, typename Name>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);

    return *param;
}

template<typename T>
bool truncateKiloValue(T& value)
{
    if (value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }
    else { return false; }
}