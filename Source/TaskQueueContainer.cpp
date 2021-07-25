/*
  ==============================================================================

    TaskQueueContainer.cpp
    Created: 7 Jul 2021 7:37:35pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueContainer.h"
#include "TaskQueueItem.h"
using namespace juce;

TaskQueueContainer::TaskQueueContainer()
{
    //addAndMakeVisible(tree);
    //addAndMakeVisible(addTaskButton);
    //addAndMakeVisible(addSubTaskButton);

    for (auto* c : Array<Component*>{ 
        &tree, 
        &addTaskButton,
        &addSubTaskButton,
        &showHideCompletedButton,
        &undoButton,
        &redoButton,
        &loadButton,
        &saveButton,
        &newButton })
    {
        addAndMakeVisible(c);
    }

    tree.setDefaultOpenness(true);
    tree.setMultiSelectEnabled(false);
    rootItem.reset(new TaskQueueItem(createTree("Task Queue Tree"), undoManager));
    tree.setRootItem(rootItem.get());

    startTimer(500);

    addTaskButton.onClick = [this]()
    {
        static int counter = 0;
        //ValueTree task{ "Task" };
        //task.setProperty("name", "Task " + String(counter++), &undoManager);

        //Wrong way to select
        //task.setProperty("selected", true, nullptr);

        if (rootItem)
        {
            rootItem->addChild(createTree("Task" + String(counter++)));
            rootItem->getSubItem(rootItem->getNumSubItems() - 1)->setSelected(true, false);
        }
    };

    selectionMonitor = std::make_unique<SelectionMonitor>(tree, [this](bool somethingIsSelected)
        {
            this->addSubTaskButton.setEnabled(somethingIsSelected);
        });

    addSubTaskButton.onClick = [this]()
    {
        if (auto* tqi = dynamic_cast<TaskQueueItem*>(tree.getSelectedItem(0)))
        {
            tqi->addChild(createTree("Sub Task"));
            //tqi->getSubItem(tqi->getNumSubItems() -1)->setSelected(true, true);
            tqi->setSelected(true, true);
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
    //tree.setBounds(bounds.removeFromTop(getHeight() * 0.9));

    FlexBox fB;
    fB.flexDirection = FlexBox::Direction::column;
    fB.alignItems = FlexBox::AlignItems::stretch;

    auto getWidthGap = []() { return FlexItem().withWidth(2.0f);};
    auto getHeightGap = []() { return FlexItem().withHeight(2.0f);};

    FlexBox topRow;
    {
        topRow.flexDirection = FlexBox::Direction::row;
        topRow.alignItems = FlexBox::AlignItems::stretch;

        topRow.items.add(getWidthGap());
        topRow.items.add(FlexItem(redoButton).withFlex(1.0f));
        topRow.items.add(getWidthGap());
        topRow.items.add(FlexItem(undoButton).withFlex(1.0f));
        topRow.items.add(FlexItem().withFlex(2.0f));
        topRow.items.add(FlexItem(loadButton).withFlex(1.0f));
        topRow.items.add(getWidthGap());
        topRow.items.add(FlexItem(newButton).withFlex(1.0f));
    }
    FlexBox treeRow;
    {
        treeRow.flexDirection = FlexBox::Direction::row;
        treeRow.alignItems = FlexBox::AlignItems::stretch;

        treeRow.items.add(FlexItem(tree).withFlex(1.0f));
    }
    FlexBox bottomRow;
    {
        bottomRow.flexDirection = FlexBox::Direction::row;
        bottomRow.alignItems = FlexBox::AlignItems::stretch;

        bottomRow.items.add(getWidthGap());
        bottomRow.items.add(FlexItem(addTaskButton).withFlex(1.0f));
        bottomRow.items.add(getWidthGap());
        bottomRow.items.add(FlexItem(addSubTaskButton).withFlex(1.0f));
        bottomRow.items.add(FlexItem().withFlex(2.0f));
        bottomRow.items.add(FlexItem(showHideCompletedButton).withFlex(1.0f));
    }

    fB.items.add(getHeightGap());
    fB.items.add(FlexItem(topRow).withFlex(0.1f).withMinHeight(20.0f).withMaxHeight(30.0f));
    fB.items.add(getHeightGap());
    fB.items.add(FlexItem(treeRow).withFlex(1.0f));
    fB.items.add(getHeightGap());
    fB.items.add(FlexItem(bottomRow).withFlex(0.1f).withMinHeight(20.0f).withMaxHeight(30.0f));
    fB.performLayout(bounds);
}

//juce::ValueTree TaskQueueContainer::createRootValueTree()
//{
//    auto vt = createTree("Task Queue Tree");
//    return vt;
//}

juce::ValueTree TaskQueueContainer::createTree(const juce::String& desc)
{
    juce::ValueTree t("Item");
    t.setProperty("name", desc, nullptr);
    t.setProperty("completed", false, nullptr);
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
