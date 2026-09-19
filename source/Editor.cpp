#include "Editor.h"
#include <BinaryData.h>


Editor::Editor(Processor& p)
 : AudioProcessorEditor(&p), pRef(p)
{
    setSize(600, 600);      // Plugin Window Size

    // ------------------------------------------------------- Here STARTS editor implementation
    // ------------------------------------------------------- Here ENDS editor implementation
}
Editor::~PluginEditor() {}


void Editor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);     // Grey BG
}


void Editor::resized() 
{

}