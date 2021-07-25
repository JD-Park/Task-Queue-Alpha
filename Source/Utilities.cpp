/*
  ==============================================================================

    Utilities.cpp
    Created: 11 Jul 2021 7:29:20pm
    Author:  jiseong

  ==============================================================================
*/

#include "Utilities.h"
using namespace juce;

bool completedCalculator(ValueTree tree, int& numChildren, int& numCompleted)
{
    numChildren += tree.getNumChildren();

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto child = tree.getChild(i);
        if (child["completed"].equals(true))
        {
            ++numCompleted;
        }

        if (child.getNumChildren() > 0)
        {
            completedCalculator(child, numChildren, numCompleted);
        }
    }

    return numChildren == numCompleted;
}