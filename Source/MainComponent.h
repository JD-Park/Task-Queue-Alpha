#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class ValueTreeItem : public juce::TreeViewItem,
    private juce::ValueTree::Listener
{
public:
    ValueTreeItem(const juce::ValueTree& v, juce::UndoManager& um)
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
        /*if (isSelected())
            g.fillAll(getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour::highlightedFill,
                g.setColour(juce::Colours::teal)));

        g.setColour(getUIColourIfAvailable(LookAndFeel_V4::ColourScheme::UIColour::defaultText,
            Colours::black));
        g.setFont(15.0f);

        g.drawText(tree["name"].toString(),
            4, 0, width - 4, height,
            Justification::centredLeft, true);*/
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
            if (auto* vti = dynamic_cast<ValueTreeItem*> (treeView.getSelectedItem(i)))
                items.add(new juce::ValueTree(vti->tree));
    }

private:
    juce::ValueTree tree;
    juce::UndoManager& undoManager;

    void refreshSubItems()
    {
        clearSubItems();

        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem(new ValueTreeItem(tree.getChild(i), undoManager));
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueTreeItem)
};

//==============================================================================
class ValueTreesDemo : public juce::Component,
    public juce::DragAndDropContainer,
    private juce::Timer
{
public:
    ValueTreesDemo()
    {
        addAndMakeVisible(tree);

        tree.setDefaultOpenness(true);
        tree.setMultiSelectEnabled(true);
        rootItem.reset(new ValueTreeItem(createRootValueTree(), undoManager));
        tree.setRootItem(rootItem.get());

        addAndMakeVisible(undoButton);
        addAndMakeVisible(redoButton);
        undoButton.onClick = [this] { undoManager.undo(); };
        redoButton.onClick = [this] { undoManager.redo(); };

        startTimer(500);

        setSize(500, 500);
    }

    ~ValueTreesDemo() override
    {
        tree.setRootItem(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        /*g.fillAll(getUIColourIfAvailable(LookAndFeel_V4::ColourScheme::UIColour::windowBackground));*/
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(8);

        auto buttons = r.removeFromBottom(22);
        undoButton.setBounds(buttons.removeFromLeft(100));
        buttons.removeFromLeft(6);
        redoButton.setBounds(buttons.removeFromLeft(100));

        r.removeFromBottom(4);
        tree.setBounds(r);
    }

    static juce::ValueTree createTree(const juce::String& desc)
    {
        juce::ValueTree t("Item");
        t.setProperty("name", desc, nullptr);
        return t;
    }

    static juce::ValueTree createRootValueTree()
    {
        auto vt = createTree("This demo displays a ValueTree as a treeview.");
        vt.appendChild(createTree("You can drag around the nodes to rearrange them"), nullptr);
        vt.appendChild(createTree("..and press 'delete' or 'backspace' to delete them"), nullptr);
        vt.appendChild(createTree("Then, you can use the undo/redo buttons to undo these changes"), nullptr);

        int n = 1;
        vt.appendChild(createRandomTree(n, 0), nullptr);

        return vt;
    }

    static juce::ValueTree createRandomTree(int& counter, int depth)
    {
        auto t = createTree("Item " + juce::String(counter++));

        if (depth < 3)
            for (int i = 1 + juce::Random::getSystemRandom().nextInt(7); --i >= 0;)
                t.appendChild(createRandomTree(counter, depth + 1), nullptr);

        return t;
    }

    void deleteSelectedItems()
    {
        juce::OwnedArray<juce::ValueTree> selectedItems;
        ValueTreeItem::getSelectedTreeViewItems(tree, selectedItems);

        for (auto* v : selectedItems)
        {
            if (v->getParent().isValid())
                v->getParent().removeChild(*v, &undoManager);
        }
    }

    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::deleteKey || key == juce::KeyPress::backspaceKey)
        {
            deleteSelectedItems();
            return true;
        }

        if (key == juce::KeyPress('z', juce::ModifierKeys::commandModifier, 0))
        {
            undoManager.undo();
            return true;
        }

        if (key == juce::KeyPress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier, 0))
        {
            undoManager.redo();
            return true;
        }

        return Component::keyPressed(key);
    }

private:
    juce::TreeView tree;
    juce::TextButton undoButton{ "Undo" },
        redoButton{ "Redo" };

    std::unique_ptr<ValueTreeItem> rootItem;
    juce::UndoManager undoManager;

    void timerCallback() override
    {
        undoManager.beginNewTransaction();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueTreesDemo)
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

private:
    //==============================================================================
    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
