/*
  ==============================================================================

    TaskQueueItem.cpp
    Created: 7 Jul 2021 6:52:13pm
    Author:  jiseong

  ==============================================================================
*/

#include "TaskQueueItem.h"
#include "TaskQueueItemComponent.h"

TaskQueueItem::TaskQueueItem (const juce::ValueTree& v, juce::UndoManager& um)
    : tree(v), undoManager(um)
{
    tree.addListener(this);
}

juce::String TaskQueueItem::getUniqueName() const
{
    return tree["name"].toString();
}

bool TaskQueueItem::mightContainSubItems() 
{
    return tree.getNumChildren() > 0;
}

Component* TaskQueueItem::createItemComponent()
{
    return new TaskQueueItemComponent(tree, *this);
}

void TaskQueueItem::addChild(const ValueTree& childToAdd)
{
    //Wrong way to select
    //for (auto child : tree)
    //{
    //    child.setProperty("selected", false, nullptr);
    //}

    //tree.setProperty("selected", false, nullptr);

    //setSelected(false, true);
    
    tree.addChild(childToAdd, -1, &undoManager);
}

//void TaskQueueItem::paintItem(juce::Graphics& g, int width, int height) 
//{
//    if (isSelected())
//    {
//        g.fillAll(olive);
//    }
//
//    g.setColour(sand);
//    g.setFont(15.0f);
//
//    g.drawText(tree["name"].toString(),
//        4, 0, width - 4, height,
//        juce::Justification::centredLeft, true);
//}

void TaskQueueItem::itemOpennessChanged(bool isNowOpen) 
{
    if (isNowOpen && getNumSubItems() == 0)
        refreshSubItems();
    else
        clearSubItems();
}

juce::var TaskQueueItem::getDragSourceDescription() 
{
    return "Drag Demo";
}

bool TaskQueueItem::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description == "Drag Demo";
}

void TaskQueueItem::itemDropped(const juce::DragAndDropTarget::SourceDetails&, int insertIndex) 
{
    juce::OwnedArray<juce::ValueTree> selectedTrees;
    auto ownerView = getOwnerView();
    jassert(ownerView != nullptr);

    getSelectedTreeViewItems(*ownerView, selectedTrees);

    jassert(selectedTrees.size() == 1);
    auto movedTree = dynamic_cast<TaskQueueItem*>(ownerView->getSelectedItem(0))->tree;

    moveItems(*ownerView, selectedTrees, tree, insertIndex, undoManager);

    
    if (auto* node = dynamic_cast<TaskQueueItem*>(ownerView->getRootItem()))
    {
        if (auto* nodeToSelect = findTreeWith(node, movedTree))
        {
            nodeToSelect->setSelected(true, true);
        }

    }
    
}

void TaskQueueItem::moveItems(juce::TreeView& treeView, const juce::OwnedArray<juce::ValueTree>& items,
    juce::ValueTree newParent, int insertIndex, juce::UndoManager& undoManager)
{
    if (items.size() > 0)
    {
        std::unique_ptr<juce::XmlElement> oldOpenness(treeView.getOpennessState(false));

        for (auto* v : items)
        {
            if (v->getParent().isValid() && newParent != *v && !newParent.isAChildOf(*v))
            {
                if (v->getParent() == newParent && newParent.indexOf(*v) < insertIndex)
                    --insertIndex;

                v->getParent().removeChild(*v, &undoManager);
                newParent.addChild(*v, insertIndex, &undoManager);
            }
        }

        if (oldOpenness.get() != nullptr)
            treeView.restoreOpennessState(*oldOpenness, false);
    }
}

void TaskQueueItem::getSelectedTreeViewItems(juce::TreeView& treeView, juce::OwnedArray<juce::ValueTree>& items)
{
    auto numSelected = treeView.getNumSelectedItems();

    for (int i = 0; i < numSelected; ++i)
        if (auto* vti = dynamic_cast<TaskQueueItem*> (treeView.getSelectedItem(i)))
            items.add(new juce::ValueTree(vti->tree));
}

TaskQueueItem* TaskQueueItem::findTreeWith(TaskQueueItem* node, const juce::ValueTree& treeToFind)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->getTree() == treeToFind)
    {
        return node;
    }

    for (int i = 0; i < node->getNumSubItems(); ++i)
    {
        if (auto* candidate = dynamic_cast<TaskQueueItem*>(node->getSubItem(i)))
        {
            if (auto* result = findTreeWith(candidate, treeToFind))
            {
                return result;
            }
        }
    }

    return nullptr;

}

void TaskQueueItem::refreshSubItems()
{
    clearSubItems();

    for (int i = 0; i < tree.getNumChildren(); ++i)
        addSubItem(new TaskQueueItem(tree.getChild(i), undoManager));
}

void TaskQueueItem::valueTreePropertyChanged(juce::ValueTree& treeThatChanged, const juce::Identifier& identifier) 
{
    //repaintItem();
    // Wrong way to select items
    //if (identifier == Identifier("selected") && treeThatChanged.isAChildOf(tree))
    //{
    //    for (auto child : tree)
    //    {
    //        if( child != treeThatChanged)
    //        {
    //            child.setProperty("selected", false, nullptr);
    //        }
    //    }
    //}
}

void TaskQueueItem::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree&) { treeChildrenChanged(parentTree); }
void TaskQueueItem::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree&, int) { treeChildrenChanged(parentTree); }
void TaskQueueItem::valueTreeChildOrderChanged(juce::ValueTree& parentTree, int, int) { treeChildrenChanged(parentTree); }
void TaskQueueItem::valueTreeParentChanged(juce::ValueTree&) {}

void TaskQueueItem::treeChildrenChanged(const juce::ValueTree& parentTree)
{
    if (parentTree == tree)
    {
        refreshSubItems();
        treeHasChanged();
        setOpen(true);
    }
}

juce::Colour olive{ 84u, 94u, 80u };
juce::Colour olive2{ 62u, 71u, 61u };
juce::Colour darkG{ 48u, 52u, 49u };
juce::Colour brown{ 66u, 48u, 36u };
juce::Colour sand{ 156u, 130u, 107u };
