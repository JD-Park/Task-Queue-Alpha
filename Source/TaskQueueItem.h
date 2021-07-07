/*
  ==============================================================================

    TaskQueueItem.h
    Created: 7 Jul 2021 6:30:51pm
    Author:  jiseong

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TaskQueueContainer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

struct TaskQueueItem : public juce::TreeViewItem,
    private juce::ValueTree::Listener
{
public:
    TaskQueueItem(const juce::ValueTree& v, juce::UndoManager& um);

    juce::String getUniqueName() const override;

    bool mightContainSubItems() override;

    void paintItem(juce::Graphics& g, int width, int height) override;

    void itemOpennessChanged(bool isNowOpen) override;

    juce::var getDragSourceDescription() override;

    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

    void itemDropped(const juce::DragAndDropTarget::SourceDetails&, int insertIndex) override;

    static void moveItems(juce::TreeView& treeView, const juce::OwnedArray<juce::ValueTree>& items,
        juce::ValueTree newParent, int insertIndex, juce::UndoManager& undoManager);

    static void getSelectedTreeViewItems(juce::TreeView& treeView, juce::OwnedArray<juce::ValueTree>& items);

private:
    juce::ValueTree tree;
    juce::UndoManager& undoManager;

    void refreshSubItems();

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;

    void treeChildrenChanged(const juce::ValueTree& parentTree);

    juce::Colour olive{ 84u, 94u, 80u };
    juce::Colour olive2{ 62u, 71u, 61u };
    juce::Colour darkG{ 48u, 52u, 49u };
    juce::Colour brown{ 66u, 48u, 36u };
    juce::Colour sand{ 156u, 130u, 107u };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TaskQueueItem)
};
