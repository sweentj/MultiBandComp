/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 24 Jan 2023 7:25:19pm
    Author:  tjswe

  ==============================================================================
*/

#include "CompressorBandControls.h"

CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
    apvts(apv),
    attackSlider(nullptr, "ms", "ATTACK"),
    releaseSlider(nullptr, "ms", "RELEASE"),
    thresholdSlider(nullptr, "dB", "THRESHOLD"),
    ratioSlider(nullptr, "")
{
    /*

    /*auto getParamHelper = [&params, &apvts = this->apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };

    auto& attackParam = getParamHelper(Names::Attack_Mid_Band);
    auto& releaseParam = getParamHelper(Names::Release_Mid_Band);
    auto& thresholdParam = getParamHelper(Names::Threshold_Mid_Band);
    auto& ratioParam1 = getParamHelper(Names::Ratio_Mid_Band);*/

    //attackSlider.changeParam(&attackParam);
    //releaseSlider.changeParam(&releaseParam);
    //thresholdSlider.changeParam(&thresholdParam);
    //ratioSlider.changeParam(&ratioParam1);

    ///*attackSlider = std::make_unique<RSWL>(&attackParam, "ms", "ATTACK");
    //releaseSlider = std::make_unique<RSWL>(&releaseParam, "ms", "RELEASE");
    //thresholdSlider = std::make_unique<RSWL>(&thresholdParam, "dB", "TRESHOLD");
    //ratioSlider = std::make_unique<RSWL>(&ratioParam, "%", "Ratio");*/

    //addLabelPairs(attackSlider.labels, getParamHelper(Names::Attack_Mid_Band), "ms");
    //addLabelPairs(releaseSlider.labels, getParamHelper(Names::Release_Mid_Band), "ms");
    //addLabelPairs(thresholdSlider.labels, getParamHelper(Names::Threshold_Mid_Band), "dB");
    ////addLabelPairs(ratioSlider.labels, getParamHelper(Names::Ratio_Mid_Band), "%");
    //ratioSlider.labels.add({ 0.f,"1:1" });
    //auto ratioParam2 = dynamic_cast<juce::AudioParameterChoice*>(&getParamHelper(Names::Ratio_Mid_Band));

    ////this some bullshit to make the 100% position say "100:1"
    //ratioSlider.labels.add({ 1.0f,juce::String(ratioParam2->choices.getReference(ratioParam2->choices.size() - 1).getIntValue())+":1"});

    //Attach slider parameters to apvts
    /*auto makeAttachmentHelper = [&params, &apvts=this->apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(attackSliderAttachment, Names::Attack_Mid_Band, attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, Names::Release_Mid_Band, releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, Names::Threshold_Mid_Band, thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, Names::Ratio_Mid_Band, ratioSlider);*/

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);

    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");

    lowBand.setName("Low");
    midBand.setName("Mid");
    highBand.setName("High");

    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);

    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
        if (auto* c = safePtr.getComponent())
        {
            c->updateAttachments();
        }
    };

    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;

    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);

    updateAttachments();

    /*makeAttachmentHelper(bypassButtonAttachment, Names::Bypass_Mid_Band, bypassButton);
    makeAttachmentHelper(soloButtonAttachment, Names::Solo_Mid_Band, soloButton);
    makeAttachmentHelper(muteButtonAttachment, Names::Mute_Mid_Band, muteButton);*/


    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);


}

void CompressorBandControls::resized()
{
    using namespace juce;

    auto bounds = getLocalBounds().reduced(5);

    auto createBandButtonControlBox = [](std::vector<Component*> comps)
    {
        FlexBox flexBox;
        flexBox.flexDirection = FlexBox::Direction::column;
        flexBox.flexWrap = FlexBox::Wrap::noWrap;

        auto spacer = FlexItem().withHeight(2);

        for (auto* comp : comps)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(FlexItem(*comp).withFlex(1.f));
        }

        flexBox.items.add(spacer);

        return flexBox;
    };

    auto bandButtonControlBox = createBandButtonControlBox({ &bypassButton, &soloButton, &muteButton });
    auto bandSelectControlBox = createBandButtonControlBox({ &lowBand, &midBand, &highBand });

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(50));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
    //flexBox.items.add(endCap);
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30));

    flexBox.performLayout(bounds);
}

void CompressorBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);

}

void CompressorBandControls::updateAttachments()
{
    //figure out what button was clicked
    //figure out which parameters go with buttons
    using namespace Params;
    std::vector<Names> names;

    enum BandType
    {
        Low,
        Mid,
        High
    };

    enum Pos
    {
        Attack,
        Release,
        Threshold,
        Ratio,
        Mute,
        Solo,
        Bypass
    };

    BandType bandType = [this]()
    {
        if (lowBand.getToggleState()) return BandType::Low;
        if (midBand.getToggleState()) return BandType::Mid;

        return BandType::High;
    }();

    switch (bandType) {
    case BandType::Low:
        names = std::vector<Names>
        {
            Names::Attack_Low_Band,
            Names::Release_Low_Band,
            Names::Threshold_Low_Band,
            Names::Ratio_Low_Band,
            Names::Mute_Low_Band,
            Names::Solo_Low_Band,
            Names::Bypass_Low_Band
        };
        break;
    case BandType::Mid:
        names = std::vector<Names>
        {
            Names::Attack_Mid_Band,
            Names::Release_Mid_Band,
            Names::Threshold_Mid_Band,
            Names::Ratio_Mid_Band,
            Names::Mute_Mid_Band,
            Names::Solo_Mid_Band,
            Names::Bypass_Mid_Band
        };
        break;
    case BandType::High:
        names = std::vector<Names>
        {
            Names::Attack_High_Band,
            Names::Release_High_Band,
            Names::Threshold_High_Band,
            Names::Ratio_High_Band,
            Names::Mute_High_Band,
            Names::Solo_High_Band,
            Names::Bypass_High_Band
        };
        break;
    }

    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts = this->apvts, &names](const auto& pos) -> auto&
    {
        return getParam(apvts, params, names.at(pos));
    };

    attackSliderAttachment.reset();
    releaseSliderAttachment.reset();
    thresholdSliderAttachment.reset();
    ratioSliderAttachment.reset();
    bypassButtonAttachment.reset();
    muteButtonAttachment.reset();
    soloButtonAttachment.reset();

    auto& attackParam = getParamHelper(Pos::Attack);
    addLabelPairs(attackSlider.labels, attackParam, "ms");
    attackSlider.changeParam(&attackParam);

    auto& releaseParam = getParamHelper(Pos::Release);
    addLabelPairs(attackSlider.labels, releaseParam, "ms");
    releaseSlider.changeParam(&releaseParam);

    auto& thresholdParam = getParamHelper(Pos::Threshold);
    addLabelPairs(attackSlider.labels, thresholdParam, "ms");
    thresholdSlider.changeParam(&thresholdParam);

    auto& ratioParam = getParamHelper(Pos::Ratio);
    ratioSlider.labels.clear();
    ratioSlider.labels.add({ 0.f,"1:1" });
    auto ratioParam2 = dynamic_cast<juce::AudioParameterChoice*>(&ratioParam);
    ratioSlider.labels.add({ 1.0f,juce::String(ratioParam2->choices.getReference(ratioParam2->choices.size() - 1).getIntValue()) + ":1" });
    ratioSlider.changeParam(ratioParam2);

    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
    makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
    makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
    makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);




}