// MainComponent.h
#pragma once

#include <JuceHeader.h>

//==============================================================================
// Denne komponenten inneholder hoved-UI og håndterer all lydprosessering.
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    // Kalles av JUCE for å forberede lydprosesseringen.
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    // Kalles av JUCE for å prosessere neste lydblokk (sanntid).
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    // Kalles av JUCE når lydprosesseringen stopper.
    void releaseResources() override;

    //==============================================================================
    // Kalles av JUCE for å tegne komponenten.
    void paint (juce::Graphics& g) override;
    // Kalles av JUCE når komponentens størrelse endres.
    void resized() override;

private:
    //==============================================================================
    // UI-elementer
    juce::TextButton openSettingsButton; // Knapp for å åpne lydinnstillinger
    juce::Label statusLabel;             // Viser statusinformasjon

    void openAudioSettings(); // Åpner lydinnstillingsdialog

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
