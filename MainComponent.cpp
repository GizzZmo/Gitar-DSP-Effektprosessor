// MainComponent.cpp
#include "MainComponent.h"

//==============================================================================
// Konstruktor: Initialiserer UI-elementer og lydkanaler.
MainComponent::MainComponent()
{
    addAndMakeVisible(openSettingsButton);
    openSettingsButton.setButtonText("Audio Settings");
    openSettingsButton.onClick = [this] { openAudioSettings(); };

    addAndMakeVisible(statusLabel);
    statusLabel.setText("Waiting for audio device...", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);

    setSize (400, 200);

    // For gitar: mono input (1) og stereo output (2) er vanlig.
    setAudioChannels (1, 2);
}

// Destructor: Stopper og lukker lydprosesseringen.
MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
// Klargjør for lydavspilling.
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    statusLabel.setText("Audio device is active. Pass-through enabled.", juce::dontSendNotification);
}

// Prosesserer lydblokker i sanntid (pass-through for input til output).
void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (!device)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();

    if (activeInputChannels.isZero())
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Pass-through: Kopierer input til alle aktive output-kanaler.
    const float* inBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        if (activeOutputChannels[channel])
        {
            float* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                outBuffer[sample] = inBuffer[sample];
            }
        }
    }
}

// Stopper lydprosesseringen.
void MainComponent::releaseResources()
{
    statusLabel.setText("Audio device stopped.", juce::dontSendNotification);
}

//==============================================================================
// Tegner bakgrunnen til komponenten.
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

// Plasserer UI-elementene.
void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    statusLabel.setBounds(bounds.removeFromTop(40));
    openSettingsButton.setBounds(bounds.removeFromBottom(40));
}

// Viser lydinnstillingsdialogen.
void MainComponent::openAudioSettings()
{
    juce::AudioDeviceSelectorComponent settingsComponent(deviceManager, 1, 2, 2, 2);
    settingsComponent.setSize(500, 400);

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
