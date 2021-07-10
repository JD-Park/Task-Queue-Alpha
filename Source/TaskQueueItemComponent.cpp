/*
  ==============================================================================

    TaskQueueItemComponent.cpp
    Created: 10 Jul 2021 6:24:24pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueItemComponent.h"
#include "TaskQueueItem.h"

using namespace juce;

TaskQueueItemComponent::TaskQueueItemComponent(const ValueTree& t, TaskQueueItem& o) : tree(t), owner(o)
{
    tree.addListener(this);
}

void TaskQueueItemComponent::paint(Graphics& g)
{
    //g.fillAll(Colours::black);
    // 
    //Wrong way to select
    //g.fillAll(tree["selected"].equals(true) ? olive : darkG);
    
    g.fillAll(owner.isSelected() ? olive : darkG);
    g.setColour(olive2);
    g.fillRect(getLocalBounds().reduced(2));

    g.setColour(sand);
    g.drawFittedText(tree["name"].toString(), getLocalBounds().reduced(2), Justification::centred, 1);
}

void TaskQueueItemComponent::valueTreePropertyChanged(ValueTree& treeThatChanged, const Identifier& identifier)
{
    if (treeThatChanged == tree)
    {
        repaint();
    }
}

void TaskQueueItemComponent::mouseDown(const MouseEvent& e)
{
    //Wrong way to select
    /*tree.setProperty("selected", !tree["selected"].operator bool(), nullptr);*/

    owner.setSelected(!owner.isSelected(), true);
}

void TaskQueueItemComponent::mouseDrag(const MouseEvent& e)
{
    if (e.mouseWasDraggedSinceMouseDown())
    {
        owner.setSelected(true, true);
        if (auto* dragContainer =  DragAndDropContainer::findParentDragContainerFor(this))
        {
            if (!dragContainer->isDragAndDropActive())
            {
                dragContainer->startDragging(owner.getDragSourceDescription(), this);
            }
        }
    }
}

