/*
  ==============================================================================

    Main.cpp
    Kildekode for applikasjonens entry point.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class GitarDSPApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    GitarDSPApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override      { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Dette initialiserer applikasjonen og oppretter hovedvinduet.
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Sørg for at vinduet blir slettet når applikasjonen lukkes.
        mainWindow = nullptr; 
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
    }

    //==============================================================================
    /*
        Denne klassen representerer hovedvinduet i applikasjonen.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // Når brukeren trykker på lukkeknappen, ber vi applikasjonen om å avslutte.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// Denne funksjonen starter applikasjonen.
START_JUCE_APPLICATION (GitarDSPApplication)

//==============================================================================
// Fil: MainComponent.h
// Header-fil for vår hovedkomponent som håndterer lyd og UI.
//==============================================================================

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Denne komponenten inneholder hoved-UI og håndterer all lydprosessering.
*/
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
    juce::TextButton openSettingsButton;
    juce::Label statusLabel;

    void openAudioSettings();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};


//==============================================================================
// Fil: MainComponent.cpp
// Implementasjon av vår hovedkomponent.
//==============================================================================

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Legg til knappen for å åpne lydinnstillinger
    addAndMakeVisible(openSettingsButton);
    openSettingsButton.setButtonText("Audio Settings");
    openSettingsButton.onClick = [this] { openAudioSettings(); };

    // Legg til en status-label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Waiting for audio device...", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);

    // Sett størrelsen på komponenten
    setSize (400, 200);

    // Spesifiser antall input- og output-kanaler vi ønsker.
    // For gitar er mono input (1) og stereo output (2) vanlig.
    setAudioChannels (1, 2);
}

MainComponent::~MainComponent()
{
    // Dette kalles når komponenten slettes.
    // Vi stopper lydprosesseringen her.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Denne funksjonen kalles rett før lydavspilling starter.
    // Her kan du initialisere variabler, buffere, etc.
    // For en enkel pass-through er det ikke nødvendig med noen forberedelser.
    statusLabel.setText("Audio device is active. Pass-through enabled.", juce::dontSendNotification);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Dette er hjertet av lydprosesseringen og kjører i en sanntids-tråd.
    // Hold koden her så effektiv som mulig. Unngå minneallokering, låsing, etc.

    // Sjekk om vi har en gyldig input-kanal
    auto* device = deviceManager.getCurrentAudioDevice();
    if (!device)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();

    // Hvis vi ikke har noen aktive input-kanaler, tømmer vi bare output-bufferen.
    if (activeInputChannels.isZero())
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Pass-through logikk:
    // Vi kopierer data fra den første input-kanalen til alle aktive output-kanaler.
    // Dette sender gitarsignalet (vanligvis på kanal 0) ut til både venstre og høyre høyttaler.
    
    // Få en peker til starten av input-bufferen for kanal 0.
    const float* inBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        // Hvis denne output-kanalen er aktiv...
        if (activeOutputChannels[channel])
        {
            // Få en peker til starten av output-bufferen for denne kanalen.
            float* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            // Kopier data fra input til output
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                outBuffer[sample] = inBuffer[sample];
            }
        }
    }
}

void MainComponent::releaseResources()
{
    // Dette kalles når avspillingen stopper.
    // Her kan du frigjøre ressurser du allokerte i prepareToPlay().
    statusLabel.setText("Audio device stopped.", juce::dontSendNotification);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // Tegn bakgrunnen
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // Plasser UI-elementene i komponenten.
    auto bounds = getLocalBounds().reduced(20);

    statusLabel.setBounds(bounds.removeFromTop(40));
    openSettingsButton.setBounds(bounds.removeFromBottom(40));
}

void MainComponent::openAudioSettings()
{
    // Lag og vis standard AudioDeviceSelectorComponent fra JUCE.
    // Dette lar brukeren velge ASIO, lydkort, bufferstørrelse etc.
    juce::AudioDeviceSelectorComponent settingsComponent(deviceManager,
                                                        1,     // min input channels
                                                        2,     // max input channels
                                                        2,     // min output channels
                                                        2);    // max output channels
    
    settingsComponent.setSize(500, 400);

    // Vis innstillingsvinduet som en dialogboks
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(&settingsComponent);
    options.dialogTitle = "Audio Settings";
    options.componentToCentreAround = this;
    options.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;

    options.launchAsync();
}

