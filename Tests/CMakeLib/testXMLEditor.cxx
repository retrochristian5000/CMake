#include "testXMLEditor.h"

#include <iostream>
#include <vector>

#include "cmSystemTools.h"
#include "cmXMLEditor.h"

int testXMLEditor(int /*unused*/, char* /*unused*/ [])
{
  std::string documentPath = SOURCE_DIR "/testXMLEditor_data/Document.xml";
  std::string actualPath = "testXMLEditor_data/Actual.xml";
  std::string expectedPath = SOURCE_DIR "/testXMLEditor_data/Expected.xml";

  cmXMLEditorDocument document(documentPath, "    ");

  for (auto* element : document.GetElements("/Root/Element")) {
    element->SetAttribute("Found", "True");
  }

  // Add a new SubElement beside SubElement1
  document.GetElement("/Root/Element[SubElement/@Name='SubElement1']")
    ->AddChild("SubElement")
    ->SetAttribute("Name", "SubElement2")
    ->SetTextContent("Text Content of SubElement2");

  // Remove the Attribute "ObsoleteAttribute" from SubElement1
  document.GetElement("/Root/Element/SubElement[@Name='SubElement1']")
    ->RemoveAttribute("ObsoleteAttribute");

  // Set the text content of Element2
  document.SetTextContent("/Root/Element[@Name='Element2']",
                          "Text Content of Element2");

  document.SaveAs(actualPath);

  if (cmSystemTools::FilesDiffer(actualPath, expectedPath)) {
    std::cout << "The content of " << actualPath
              << " differs from the expected content of file " << expectedPath
              << std::endl;
    return 1;
  }

  return 0;
}
