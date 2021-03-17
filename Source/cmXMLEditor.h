/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmXMLEditor_h
#define cmXMLEditor_h

#include <list>
#include <map>
#include <string>
#include <utility>

#include "cmsys/RegularExpression.hxx"
#include "cmsys/String.h"

#include "cmXMLParser.h"
#include "cmXMLWriter.h"

class cmXMLEditorElement
{
public:
  cmXMLEditorElement(std::string name, cmXMLEditorElement* parent = nullptr,
                     std::string textContent = "")
    : Name(std::move(name))
    , Parent(parent)
    , TextContent(std::move(textContent))
  {
  }

  std::string GetName() { return this->Name; }

  void SetTextContent(std::string const& textContent)
  {
    this->TextContent = textContent;
  }

  cmXMLEditorElement* SetTextContent(std::string const& path,
                                     std::string const& textContent,
                                     bool create = true)
  {
    cmXMLEditorElement* element = GetElement(path, create);
    if (element) {
      element->SetTextContent(textContent);
    }
    return this;
  }

  void AppendTextContent(std::string const& textContent)
  {
    this->TextContent.append(textContent);
  }

  std::string GetTextContent() { return this->TextContent; }

  cmXMLEditorElement* GetParent() { return this->Parent; }

  cmXMLEditorElement* GetDocument()
  {
    return this->Parent ? this->Parent->GetDocument() : this;
  }

  cmXMLEditorElement* SetAttribute(std::string const& name,
                                   std::string const& value)
  {
    this->Attributes[name] = value;
    return this;
  }

  std::map<std::string, std::string> const& GetAttributes()
  {
    return this->Attributes;
  }

  void RemoveAttribute(std::string const& name)
  {
    this->Attributes.erase(name);
  }

  cmXMLEditorElement* AddChild(cmXMLEditorElement& element)
  {
    this->Children.push_back(element);
    element.Parent = this;
    return &this->Children.back();
  }

  cmXMLEditorElement* AddChild(std::string const& name,
                               std::string const& value = "")
  {
    cmXMLEditorElement element(name, this, value);
    return AddChild(element);
  }

  std::list<cmXMLEditorElement>& GetChildren() { return this->Children; }

  cmXMLEditorElement* GetElement(std::string const& path, bool create = false);

  std::vector<cmXMLEditorElement*> GetElements(std::string const& path);

  virtual void WriteXML(cmXMLWriter& writer);

protected:
  std::string Name;
  std::map<std::string, std::string> Attributes;
  std::string TextContent;
  // Use std::list instead of std::vector for the children, so we don't lose
  // the parent pointers due to vector resizes
  std::list<cmXMLEditorElement> Children;
  cmXMLEditorElement* Parent = nullptr;

  void GetElements(std::string const& path,
                   std::vector<cmXMLEditorElement*>& elements,
                   bool create = false);
};

class cmXMLEditorDocument
  : private cmXMLEditorElement
  , private cmXMLParser
{
public:
  using cmXMLEditorElement::GetElement;
  using cmXMLEditorElement::GetElements;
  using cmXMLEditorElement::SetTextContent;

  cmXMLEditorDocument(std::string const& file,
                      std::string indentationElement = "\t")
    : cmXMLEditorElement("Document")
    , FileName(file)
    , IndentationElement(std::move(indentationElement))
  {
    ParseFile(file.c_str());
  }

  void Save() { SaveAs(FileName); }

  void SaveAs(std::string const& fileName);

  void WriteXML(cmXMLWriter& writer) override;

  std::string& GetFileName() { return this->FileName; }

private:
  std::string FileName;
  cmXMLEditorElement* CurrentElement = this;
  std::string IndentationElement;

  void StartElement(std::string const& name, const char** atts) override;

  void EndElement(std::string const& name) override;

  void CharacterDataHandler(const char* data, int length) override;
};

#endif
