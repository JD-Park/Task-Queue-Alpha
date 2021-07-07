/*
  ==============================================================================

    TaskQueueContainer.cpp
    Created: 7 Jul 2021 7:37:35pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueContainer.h"
#include "TaskQueueItem.h"

TaskQueueContainer::TaskQueueContainer()
{
    addAndMakeVisible(tree);

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(true);
    rootItem.reset(new TaskQueueItem(createRootValueTree(), undoManager));
    tree.setRootItem(rootItem.get());

    startTimer(500);

    setSize(500, 500);
}

TaskQueueContainer::~TaskQueueContainer()
{
    tree.setRootItem(nullptr);
}

void TaskQueueContainer::paint(juce::Graphics& g)
{
    g.fillAll(darkG);
}

void TaskQueueContainer::resized()
{
    auto r = getLocalBounds().reduced(8);

    tree.setBounds(r);
}

juce::ValueTree TaskQueueContainer::createRootValueTree()
{
    auto vt = createTree("Task Queue Tree");
    return vt;
}

juce::ValueTree TaskQueueContainer::createTree(const juce::String& desc)
{
    juce::ValueTree t("Item");
    t.setProperty("name", desc, nullptr);
    return t;
}

//juce::Colour olive{ 84u, 94u, 80u };
//juce::Colour olive2{ 62u, 71u, 61u };
//juce::Colour darkG{ 48u, 52u, 49u };
//juce::Colour brown{ 66u, 48u, 36u };
//juce::Colour sand{ 156u, 130u, 107u };


void TaskQueueContainer::timerCallback()
{
    undoManager.beginNewTransaction();
}
