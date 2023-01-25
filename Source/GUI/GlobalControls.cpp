/*
  ==============================================================================

    GlobalControls.cpp
    Created: 24 Jan 2023 7:25:08pm
    Author:  tjswe

  ==============================================================================
*/

#include "GlobalControls.h"

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };

    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);
    auto& migHighXoverParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& lowMidXoverParam = getParamHelper(Names::Low_Mid_Crossover_Freq);

    inGainSlider = std::make_unique<RSWL>(&gainInParam, "dB", "INPUT TRIM");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam, "dB", "OUTPUT TRIM");
    midHighXoverSlider = std::make_unique<RSWL>(&migHighXoverParam, "Hz", "MID-HIGH X-OVER");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidXoverParam, "Hz", "LOW-MID X-OVER");

    addLabelPairs(inGainSlider->labels, getParamHelper(Names::Gain_In), "dB");
    addLabelPairs(outGainSlider->labels, getParamHelper(Names::Gain_Out), "dB");
    addLabelPairs(midHighXoverSlider->labels, getParamHelper(Names::Mid_High_Crossover_Freq), "Hz");
    addLabelPairs(lowMidXoverSlider->labels, getParamHelper(Names::Low_Mid_Crossover_Freq), "Hz");

    //Attach slider parameters to apvts
    auto makeAttachmentHelper = [&params, &apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(inGainSliderAttachment, Names::Gain_In, *inGainSlider);
    makeAttachmentHelper(outGainSliderAttachment, Names::Gain_Out, *outGainSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment, Names::Mid_High_Crossover_Freq, *midHighXoverSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment, Names::Low_Mid_Crossover_Freq, *lowMidXoverSlider);

    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);
}


void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);

}

void GlobalControls::resized()
{
    using namespace juce;

    auto bounds = getLocalBounds().reduced(5);
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}