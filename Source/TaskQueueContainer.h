/*
  ==============================================================================

    TaskQueueContainer.h
    Created: 7 Jul 2021 6:31:11pm
    Author:  jiseong

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TaskQueueItem.h"

struct TaskQueueItem;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class TaskQueueContainer : public juce::Component,
    public juce::DragAndDropContainer,
    private juce::Timer
{
public:
    TaskQueueContainer();

    ~TaskQueueContainer() override;
    void paint(juce::Graphics& g) override;

    void resized() override;

    juce::ValueTree createRootValueTree();

private:
    juce::ValueTree createTree(const juce::String& desc);

    juce::TreeView tree;

    std::unique_ptr<TaskQueueItem> rootItem;
    juce::UndoManager undoManager;

    juce::Colour olive{ 84u, 94u, 80u };
    juce::Colour olive2{ 62u, 71u, 61u };
    juce::Colour darkG{ 48u, 52u, 49u };
    juce::Colour brown{ 66u, 48u, 36u };
    juce::Colour sand{ 156u, 130u, 107u };


    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TaskQueueContainer)
};

