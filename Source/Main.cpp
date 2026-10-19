#include "MainComponent.h"

class FBDawApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return "FBDaw"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override          { return false; }

    void initialise (const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow> ("FBDaw", new MainComponent());
    }
    void shutdown() override { mainWindow = nullptr; }
    void systemRequestedQuit() override { quit(); }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name, juce::Component* c)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                              .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);
            setResizable (true, true);
            centreWithSize (1280, 800);
            setVisible (true);
        }
        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (FBDawApplication)
