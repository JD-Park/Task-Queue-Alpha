#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class TaskQueueItem : public juce::TreeViewItem,
    private juce::ValueTree::Listener
{
public:
    TaskQueueItem(const juce::ValueTree& v, juce::UndoManager& um)
        : tree(v), undoManager(um)
    {
        tree.addListener(this);
    }

    juce::String getUniqueName() const override
    {
        return tree["name"].toString();
    }

    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }

    void paintItem(juce::Graphics& g, int width, int height) override
    {
        if (isSelected())
        {
            g.fillAll(olive);
        }

        g.setColour(sand);
        g.setFont(15.0f);

        g.drawText(tree["name"].toString(),
            4, 0, width - 4, height,
            juce::Justification::centredLeft, true);
    }

    void itemOpennessChanged(bool isNowOpen) override
    {
        if (isNowOpen && getNumSubItems() == 0)
            refreshSubItems();
        else
            clearSubItems();
    }

    juce::var getDragSourceDescription() override
    {
        return "Drag Demo";
    }

    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description == "Drag Demo";
    }

    void itemDropped(const juce::DragAndDropTarget::SourceDetails&, int insertIndex) override
    {
        juce::OwnedArray<juce::ValueTree> selectedTrees;
        getSelectedTreeViewItems(*getOwnerView(), selectedTrees);

        moveItems(*getOwnerView(), selectedTrees, tree, insertIndex, undoManager);
    }

    static void moveItems(juce::TreeView& treeView, const juce::OwnedArray<juce::ValueTree>& items,
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

    static void getSelectedTreeViewItems(juce::TreeView& treeView, juce::OwnedArray<juce::ValueTree>& items)
    {
        auto numSelected = treeView.getNumSelectedItems();

        for (int i = 0; i < numSelected; ++i)
            if (auto* vti = dynamic_cast<TaskQueueItem*> (treeView.getSelectedItem(i)))
                items.add(new juce::ValueTree(vti->tree));
    }

private:
    juce::ValueTree tree;
    juce::UndoManager& undoManager;

    void refreshSubItems()
    {
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new TaskQueueItem(tree.getChild(i), undoManager));
    }

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        repaintItem();
    }

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree&) override { treeChildrenChanged(parentTree); }
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree&, int) override { treeChildrenChanged(parentTree); }
    void valueTreeChildOrderChanged(juce::ValueTree& parentTree, int, int) override { treeChildrenChanged(parentTree); }
    void valueTreeParentChanged(juce::ValueTree&) override {}

    void treeChildrenChanged(const juce::ValueTree& parentTree)
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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TaskQueueItem)
};

//==============================================================================
class TaskQueueContainer : public juce::Component,
    public juce::DragAndDropContainer,
    private juce::Timer
{
public:
    TaskQueueContainer()
    {
        addAndMakeVisible(tree);

        tree.setDefaultOpenness(true);
        tree.setMultiSelectEnabled(true);
        rootItem.reset(new TaskQueueItem(createRootValueTree(), undoManager));
        tree.setRootItem(rootItem.get());

        startTimer(500);

        setSize(500, 500);
    }

    ~TaskQueueContainer() override
    {
        tree.setRootItem(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(darkG);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(8);

        tree.setBounds(r);
    }

    juce::ValueTree createRootValueTree()
    {
        auto vt = createTree("Task Queue Tree");
        return vt;
    }

private:
    juce::ValueTree createTree(const juce::String& desc)
    {
        juce::ValueTree t("Item");
        t.setProperty("name", desc, nullptr);
        return t;
    }

    juce::TreeView tree;

    std::unique_ptr<TaskQueueItem> rootItem;
    juce::UndoManager undoManager;

    juce::Colour olive{ 84u, 94u, 80u };
    juce::Colour olive2{ 62u, 71u, 61u };
    juce::Colour darkG{ 48u, 52u, 49u };
    juce::Colour brown{ 66u, 48u, 36u };
    juce::Colour sand{ 156u, 130u, 107u };


    void timerCallback() override
    {
        undoManager.beginNewTransaction();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TaskQueueContainer)
};
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::Colour olive{ 84u, 94u, 80u };
    juce::Colour olive2{ 62u, 71u, 61u };
    juce::Colour darkG{ 48u, 52u, 49u };
    juce::Colour brown{ 66u, 48u, 36u };
    juce::Colour sand{ 156u, 130u, 107u };

private:
    //==============================================================================
    // Your private member variables go here...

    TaskQueueContainer taskQueueContainer;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
