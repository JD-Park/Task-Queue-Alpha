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
using namespace juce;

//==============================================================================
struct SelectionMonitor : public Timer
{
    SelectionMonitor(TreeView& t, std::function<void(bool)> f) : treeView(t), f(std::move(f))
    {
        startTimerHz(20);
    }
    ~SelectionMonitor() { stopTimer(); }

    void timerCallback() override
    {
        if (f)
        {
            f(treeView.getNumSelectedItems() > 0);
        }
    }

private:
    TreeView& treeView;
    std::function<void(bool)> f;
};


//==============================================================================
struct TaskQueueItem;
using namespace juce;
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

    //juce::ValueTree createRootValueTree();

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

    TextButton addTaskButton{ "Add Task" },
        addSubTaskButton{ "Add Sub-Task" },
        showHideCompletedButton{ "Hide Completed" },
        undoButton{ "Undo" },
        redoButton{ "Redo" },
        loadButton{ "Load..." },
        saveButton{ "Save" },
        newButton{ "New..." };

    std::unique_ptr<SelectionMonitor> selectionMonitor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TaskQueueContainer)
};

