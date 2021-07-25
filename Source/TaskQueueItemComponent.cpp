/*
  ==============================================================================

    TaskQueueItemComponent.cpp
    Created: 10 Jul 2021 6:24:24pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueItemComponent.h"
#include "TaskQueueItem.h"
#include "Utilities.h"

using namespace juce;

TaskQueueItemComponent::TaskQueueItemComponent(const ValueTree& t, TaskQueueItem& o) : tree(t), owner(o)
{
    tree.addListener(this);

    addAndMakeVisible(label);
    /*if (tree.getNumChildren() > 0)
    {
        label.onTextChange = [this]()
        {
            tree.setProperty("name", label.getText(), &owner.getUndoManager());
        };
    }
    else
    {
        label.getTextValue().referTo(tree.getPropertyAsValue("name", &owner.getUndoManager()));
    }*/
    updateLabelBehav();
    updateLabelText();
    label.setJustificationType(Justification::centred);
    label.setColour(Label::ColourIds::textColourId, sand);
    label.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(completedButton);
    completedButton.getToggleStateValue().referTo(tree.getPropertyAsValue("completed", &owner.getUndoManager()));
    completedButton.setColour(ToggleButton::ColourIds::tickColourId, sand);
    completedButton.setColour(ToggleButton::ColourIds::tickDisabledColourId, sand);

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

    //g.setColour(sand);
    //g.drawFittedText(tree["name"].toString(), getLocalBounds().reduced(2), Justification::centred, 1);
}

void TaskQueueItemComponent::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    
    completedButton.setBounds(bounds.getX(), bounds.getY(), bounds.getHeight(), bounds.getHeight());
    bounds.removeFromLeft(bounds.getHeight() + 2);
    label.setBounds(bounds);
}

void TaskQueueItemComponent::valueTreePropertyChanged(ValueTree& treeThatChanged, const Identifier& identifier)
{
    if (ignoreCallbacks)
    {
        return;
    }

    ScopedValueSetter<bool> guard(ignoreCallbacks, true);

    //if (identifier == Identifier("completed"))
    //{
    //    if (tree["completed"].equals(true))
    //    {
    //        for (auto child : tree)
    //        {
    //            child.setProperty("completed", true, &owner.getUndoManager());
    //        }
    //    }
    //}

    //updateLabelText();

    if (identifier == Identifier("completed"))
    {
        if (treeThatChanged.isAChildOf(tree))
        {
            int numChildren = 0, numCompleted = 0;
            tree.setProperty("completed", completedCalculator(tree, numChildren, numCompleted), &owner.getUndoManager());
            updateLabelText();
        }
        else if (tree.isAChildOf(treeThatChanged))
        {

        }
        else if (treeThatChanged == tree)
        {
            for (auto child : tree)
            {   
                child.setProperty("completed", (bool)tree["completed"], &owner.getUndoManager());
            }
            updateLabelText();
        }
    }
    else if (identifier == Identifier("name"))
    {
        updateLabelText();
    }
}

void TaskQueueItemComponent::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree&)
{
    updateLabelBehav();
    updateLabelText();
}

void TaskQueueItemComponent::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree&, int)
{
    updateLabelBehav();
    updateLabelText();
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
        if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
        {
            if (!dragContainer->isDragAndDropActive())
            {
                dragContainer->startDragging(owner.getDragSourceDescription(), this);
            }
        }
    }
}

void TaskQueueItemComponent::mouseDoubleClick(const MouseEvent& e)
{
    label.showEditor();
}

//void completedCalculator(ValueTree tree, int& numChildren, int& numCompleted)
//{
//    numChildren += tree.getNumChildren();
//
//    for (int i = 0; i < tree.getNumChildren(); ++i)
//    {
//        auto child = tree.getChild(i);
//        if (child["completed"].equals(true))
//        {
//            ++numCompleted;
//        }
//
//        if (child.getNumChildren() > 0)
//        {
//            completedCalculator(child, numChildren, numCompleted);
//        }
//    }
//}

String TaskQueueItemComponent::getNumCompleted()
{
    jassert(tree.getNumChildren() > 0);
    int numChildren = 0;
    int numCompleted = 0;

    completedCalculator(tree, numChildren, numCompleted);
    String str;
    str << "(" << numCompleted << " / " << numChildren << ")";
    return str;
}

void TaskQueueItemComponent::updateLabelText()
{
    if (tree.getNumChildren() > 0)
    {
        label.setText(tree["name"].toString() + " " + getNumCompleted(), dontSendNotification);
    }
    else
    {
        label.setText(tree["name"].toString(), dontSendNotification);
    }
}

void TaskQueueItemComponent::updateLabelBehav()
{
    label.onTextChange = [this]()
    {
        tree.setProperty("name", label.getText(), &owner.getUndoManager());
    };

    label.onEditorShow = [this]()
    {
        String text = label.getText();
        if (text.contains("("))
        {
            text = label.getText().substring(0, label.getText().indexOf("(")).trimEnd();
        }
        label.getCurrentTextEditor()->setText(text);
        label.getCurrentTextEditor()->selectAll();
    };
// if (tree.getNumChildren() > 0)
//    {
//        label.onTextChange = [this]()
//        {
//            tree.setProperty("name", label.getText(), &owner.getUndoManager());
//        };
//
//       /* auto text = label.getText();
//        Value textValue(text);
//        label.getTextValue().referTo(textValue);*/
//    }
//    /*else
//    {
//        label.onTextChange = nullptr;
//        label.getTextValue().referTo(tree.getPropertyAsValue("name", &owner.getUndoManager()));
//    }*/
}


