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
    addAndMakeVisible(addTaskButton);
    addAndMakeVisible(addSubTaskButton);

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(false);
    rootItem.reset(new TaskQueueItem(createTree("Task Queue Tree"), undoManager));
    tree.setRootItem(rootItem.get());

    startTimer(500);

    addTaskButton.onClick = [this]()
    {
        static int counter = 0;
        ValueTree task{ "Task" };
        task.setProperty("name", "Task " + String(counter++), & undoManager);
        
        //Wrong way to select
        //task.setProperty("selected", true, nullptr);

        if (rootItem)
        {
            rootItem->addChild(task);
            rootItem->getSubItem(rootItem->getNumSubItems() - 1)->setSelected(true, false);
        }
    };

    setSize(500, 500);
}

TaskQueueContainer::~TaskQueueContainer()
{
    stopTimer();
    tree.setRootItem(nullptr);
}

void TaskQueueContainer::paint(juce::Graphics& g)
{
    g.fillAll(darkG);
}

void TaskQueueContainer::resized()
{
    //auto r = getLocalBounds().reduced(8);

    //tree.setBounds(r);
    auto bounds = getLocalBounds();
    tree.setBounds(bounds.removeFromTop(getHeight() * 0.9));

}
    FlexBox fB;
    fB.flexDirection = FlexBox::Direction::row;
    fB.alignItems = FlexBox::AlignItems::stretch;

    fB.items.add(FlexItem(addTaskButton).withFlex(1));
    fB.items.add(FlexItem(addSubTaskButton).withFlex(1));

    fB.performLayout(bounds);

//juce::ValueTree TaskQueueContainer::createRootValueTree()
//{
//    auto vt = createTree("Task Queue Tree");
//    return vt;
//}

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
