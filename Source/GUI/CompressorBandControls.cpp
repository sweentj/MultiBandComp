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

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);

    bypassButton.addListener(this);
    soloButton.addListener(this);
    muteButton.addListener(this);


    bypassButton.setName("X");
    bypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::yellow);
    bypassButton.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);
    
    soloButton.setName("S");
    soloButton.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::limegreen);
    soloButton.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);
    
    muteButton.setName("M");
    muteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::red);
    muteButton.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);

    lowBand.setName("Low");
    lowBand.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::grey);
    lowBand.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);
    midBand.setName("Mid");
    midBand.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::grey);
    midBand.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);
    highBand.setName("High");
    highBand.setColour(juce::TextButton::ColourIds::buttonOnColourId,
        juce::Colours::grey);
    highBand.setColour(juce::TextButton::ColourIds::buttonColourId,
        juce::Colours::black);

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

void CompressorBandControls::buttonClicked(juce::Button* button)
{
    //updateSliderenablements
    //updateSoloMuteBypassToggleStates
    //updateActiveBandFillColours
    updateSliderEnablements();
    updateSoloMuteBypassToggleStates(*button);
    //updateActiveBandFillColours(*button);
}

//if compressor band is muted or bypassed, disable the sliders
void CompressorBandControls::updateSliderEnablements()
{
    bool disabled = muteButton.getToggleState() || bypassButton.getToggleState();
    attackSlider.setEnabled(!disabled);
    releaseSlider.setEnabled(!disabled);
    thresholdSlider.setEnabled(!disabled);
    ratioSlider.setEnabled(!disabled);

}

//only one of the SMB buttons can be pressed
void CompressorBandControls::updateSoloMuteBypassToggleStates(juce::Button& clickedButton)
{
    if (&clickedButton == &bypassButton && bypassButton.getToggleState()) {
        muteButton.setToggleState(false, juce::sendNotification);
        soloButton.setToggleState(false, juce::sendNotification);
    }
    else if (&clickedButton == &muteButton && muteButton.getToggleState()) {
        bypassButton.setToggleState(false, juce::sendNotification);
        soloButton.setToggleState(false, juce::sendNotification);
    }
    else if (&clickedButton == &soloButton && soloButton.getToggleState()) {
        bypassButton.setToggleState(false, juce::sendNotification);
        muteButton.setToggleState(false, juce::sendNotification);
    }
}

void CompressorBandControls::updateActiveBandFillColours(juce::Button& clickedButton)
{

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