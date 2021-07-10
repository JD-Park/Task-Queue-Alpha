/*
  ==============================================================================

    TaskQueueItemComponent.cpp
    Created: 10 Jul 2021 6:24:24pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueItemComponent.h"

using namespace juce;

TaskQueueItemComponent::TaskQueueItemComponent(const ValueTree& t) : tree(t)
{
    tree.addListener(this);
}

void TaskQueueItemComponent::paint(Graphics& g)
{
    //g.fillAll(Colours::black);
    g.fillAll(tree["selected"].equals(true) ? olive : darkG);
    g.setColour(olive2);
    g.fillRect(getLocalBounds().reduced(2));

    g.setColour(sand);
    g.drawFittedText(tree["name"].toString(), getLocalBounds().reduced(2), Justification::centred, 1);
}

void TaskQueueItemComponent::valueTreePropertyChanged(ValueTree& treeThatChanged, const Identifier& indentifier)
{
    if (treeThatChanged == tree)
    {
        repaint();
    }
}

