#pragma once
#include "Engine.h"

namespace fbd
{

class MixerStrip;

class MainComponent : public juce::Component,
                      public juce::Timer,
                      public juce::Button::Listener,
                      public juce::ComboBox::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void buttonClicked (juce::Button*) override;
    void comboBoxChanged (juce::ComboBox*) override;

    void showPluginMenu (Track&, bool forInstrument);

private:
    Engine engine;

    // trasporto
    juce::TextButton btnRec { "● REC" }, btnPlay { "▶" }, btnStop { "■" },
                     btnRew { "⏮" }, btnAddAudio { "+ Audio" }, btnAddMidi { "+ MIDI" },
                     btnExport { "Export WAV" };
    juce::Label lblPos, lblStatus;

    // vista arrangiamento: righe tracce con waveform + playhead
    struct TrackLane : juce::Component
    {
        TrackLane (MainComponent& o, Track* t) : owner (o), track (t)
        {
            setInterceptsMouseClicks (false, false);
        }
        void paint (juce::Graphics& g) override
        {
            auto r = getLocalBounds();
            g.fillAll (track->armed ? juce::Colour (0xff3a2020) : juce::Colour (0xff2e2e30));
            g.setColour (juce::Colours::darkgrey);
            g.drawRect (r);
            g.setColour (juce::Colours::lightgrey);
            g.drawText (track->name + (track->isMidi ? "  [MIDI]" : ""),
                        r.reduced (4, 0), juce::Justification::centredLeft);
            if (! track->isMidi && track->readerSource != nullptr)
            {
                g.setColour (juce::Colours::steelblue.withAlpha (0.9f));
                auto area = r.reduced (2).withTrimmedTop (18);
                track->thumb().drawChannels (g, area, 0.0, track->fileLength(), 1.0f);
            }
            else if (track->isMidi)
            {
                g.setColour (juce::Colours::orange.withAlpha (0.8f));
                auto area = r.reduced (2).withTrimmedTop (18);
                for (int i = 0; i < track->midi.getNumEvents(); ++i)
                {
                    auto* ev = track->midi.getEventPointer (i);
                    float x = (float) (ev->message.getTimeStamp() / 60.0) * owner.pxPerSec; // 60s visibili
                    if (x < area.getWidth())
                        g.drawVerticalLine (area.getX() + (int) x, area.getY(), area.getBottom());
                }
            }
        }
        MainComponent& owner;
        Track* track;
    };

    juce::Viewport arrangeView;
    juce::Component arrangeContent;
    juce::OwnedArray<TrackLane> lanes;
    std::vector<std::unique_ptr<MixerStrip>> strips;
    juce::Viewport mixerView;
    juce::Component mixerContent;

    juce::ComboBox midiInputList;
    double pxPerSec = 100.0;

    void rebuildViews();
    void rebuildLanes();
    juce::File lastAudioDir;

    JUCE_DECLARE_NON_COPYABLE (MainComponent)
};

class MixerStrip : public juce::Component,
                   public juce::Slider::Listener,
                   public juce::Button::Listener
{
public:
    MixerStrip (MainComponent& o, Track* t) : owner (o), track (t)
    {
        addAndMakeVisible (slider);
        slider.setSliderStyle (juce::Slider::LinearVertical);
        slider.setRange (0.0, 2.0, 0.01);
        slider.setValue (1.0);
        slider.addListener (this);

        addAndMakeVisible (pan);
        pan.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        pan.setRange (-1.0, 1.0, 0.01);
        pan.addListener (this);

        addAndMakeVisible (btnMute); btnMute.setButtonText ("M"); btnMute.setClickingTogglesState (true); btnMute.addListener (this);
        addAndMakeVisible (btnSolo); btnSolo.setButtonText ("S"); btnSolo.setClickingTogglesState (true); btnSolo.addListener (this);
        addAndMakeVisible (btnRec);  btnRec.setButtonText ("●"); btnRec.setClickingTogglesState (true); btnRec.addListener (this);
        addAndMakeVisible (btnInst); btnInst.setButtonText ("VSTi"); btnInst.addListener (this);
        addAndMakeVisible (btnFx);   btnFx.setButtonText ("FX");   btnFx.addListener (this);

        addAndMakeVisible (lblName); lblName.setText (track->name, juce::dontSendNotification);
        lblName.setJustificationType (juce::Justification::centred);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff252527));
        g.setColour (juce::Colours::darkgrey); g.drawRect (getLocalBounds());
        // livello
        float lvl = juce::jlimit (0.0f, 1.0f, track->lastLevel);
        g.setColour (lvl > 0.95f ? juce::Colours::red : juce::Colours::limegreen);
        g.fillRect (getLocalBounds().reduced (2).withTrimmedBottom (2)
                        .withHeight ((int) (getHeight() * 0.45f * lvl)));
    }
    void resized() override
    {
        auto r = getLocalBounds().reduced (3);
        lblName.setBounds (r.removeFromTop (18));
        auto row = r.removeFromBottom (20);
        btnMute.setBounds (row.removeFromLeft (row.getWidth() / 3));
        btnSolo.setBounds (row.removeFromLeft (row.getWidth() / 2));
        btnRec.setBounds (row);
        auto row2 = r.removeFromBottom (20);
        btnInst.setBounds (row2.removeFromLeft (row2.getWidth() / 2));
        btnFx.setBounds (row2);
        pan.setBounds (r.removeFromBottom (45).reduced (2));
        slider.setBounds (r);
    }
    void sliderValueChanged (juce::Slider* s) override
    {
        if (s == &slider) track->gain = (float) s->getValue();
        if (s == &pan)    track->pan  = (float) s->getValue();
    }
    void buttonClicked (juce::Button* bt) override
    {
        if (bt == &btnMute) track->mute = bt->getToggleState();
        if (bt == &btnSolo) track->solo = bt->getToggleState();
        if (bt == &btnRec)  track->armed = bt->getToggleState();
        if (bt == &btnInst) owner.showPluginMenu (*track, true);
        if (bt == &btnFx)   owner.showPluginMenu (*track, false);
    }

private:
    MainComponent& owner;
    Track* track;
    juce::Slider slider, pan;
    juce::TextButton btnMute, btnSolo, btnRec, btnInst, btnFx;
    juce::Label lblName;
};

} // namespace fbd
