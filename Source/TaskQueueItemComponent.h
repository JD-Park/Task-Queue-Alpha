/*
  ==============================================================================

    TaskQueueItemComponent.h
    Created: 10 Jul 2021 6:24:24pm
    Author:  jiseong

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
using namespace juce;

struct TaskQueueItemComponent : Component, ValueTree::Listener
{
    TaskQueueItemComponent(const ValueTree& t);
    void paint(Graphics& g) override;

    void valueTreePropertyChanged(juce::ValueTree& treeThatChanged, const Identifier& indentifier) override;


private:
    ValueTree tree;

    juce::Colour olive{ 84u, 94u, 80u };
    juce::Colour olive2{ 62u, 71u, 61u };
    juce::Colour darkG{ 48u, 52u, 49u };
    juce::Colour brown{ 66u, 48u, 36u };
    juce::Colour sand{ 156u, 130u, 107u };
};