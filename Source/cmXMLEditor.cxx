/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmXMLEditor.h"

#include <algorithm>
#include <memory>
#include <vector>

#include <stddef.h>

#include "cmsys/RegularExpression.hxx"
#include "cmsys/String.h"

#include "cmGeneratedFileStream.h"
#include "cmSystemTools.h"
#include "cmXMLWriter.h"

struct XPathPredicate
{
  bool Valid = false;
  std::string Path;
  std::string Attribute;
  std::string Value;
};

struct XPath
{
  std::string ElementName;
  XPathPredicate Predicate;
  std::string RemainingPath;
};

static std::string ParsePredicate(std::string const& path,
                                  XPathPredicate& predicate)
{
  // If there is an opening bracket before the fist slash, we need to parse
  // the predicate
  cmsys::RegularExpression reg(R"(\[\s*([^ @]*)(@([^ ]+))?\s*=\s*(.*))");
  if (reg.find(path)) {
    predicate.Path = reg.match(1);
    predicate.Attribute = reg.match(3);
    std::string tmpConditionValue = reg.match(4);
    std::string remainingPath;

    if (tmpConditionValue.length() > 0) {
      if (tmpConditionValue[0] == '\'') {
        // Predicate value is a quoted string
        // -> Parse the string
        const char* c = tmpConditionValue.c_str() + 1;
        for (; *c && *(c + 1) && (*c != '\''); ++c) {
          if (*c == '\\') {
            ++c;
          }
          predicate.Value.append(c, 1);
        }
        // Get the remaining path
        if (*c == '\'' && *(c + 1) == ']') {
          if (*(c + 2) == '/') {
            remainingPath = std::string(c + 3);
          }
        } else {
          cmSystemTools::Error("Invalid predicate in XPath: " + path);
        }
      } else {
        // Predicate value is not quoted
        // -> simply find the closing bracket
        size_t indexOfClosingBracket = tmpConditionValue.find(']');
        predicate.Value = tmpConditionValue.substr(0, indexOfClosingBracket);
        // Get the remaining path
        if (indexOfClosingBracket + 2 <= tmpConditionValue.length()) {
          remainingPath = tmpConditionValue.substr(indexOfClosingBracket + 2);
        }
      }
    }
    predicate.Valid = true;

    return remainingPath;
  }
  return path;
}

static XPath ParseXPath(std::string const& path)
{
  XPath xPath;

  size_t indexOfSlash = path.find('/');
  size_t indexOfOpeningBracket = path.find('[');

  if (indexOfSlash == std::string::npos &&
      indexOfOpeningBracket == std::string::npos) {
    xPath.ElementName = path;
  } else {
    xPath.ElementName =
      path.substr(0, std::min(indexOfSlash, indexOfOpeningBracket));
  }

  if (indexOfOpeningBracket != std::string::npos &&
      indexOfOpeningBracket < indexOfSlash) {
    // If there is an opening bracket before the fist slash, we need to parse
    // the predicate
    xPath.RemainingPath = ParsePredicate(path, xPath.Predicate);
  } else if (indexOfSlash != std::string::npos) {
    // No predicate
    xPath.RemainingPath = path.substr(indexOfSlash + 1);
  }

  return xPath;
}

void cmXMLEditorDocument::StartElement(std::string const& name,
                                       const char** atts)
{
  cmXMLEditorElement* newElement = CurrentElement->AddChild(name);
  for (const char** a = atts; *a && *(a + 1); a += 2) {
    newElement->SetAttribute(*a, *(a + 1));
  }
  CurrentElement = newElement;
}

void cmXMLEditorDocument::EndElement(std::string const& /*name*/)
{
  CurrentElement = CurrentElement->GetParent();
}

void cmXMLEditorDocument::CharacterDataHandler(const char* data, int length)
{
  if (CurrentElement) {
    char* localData = const_cast<char*>(data);
    while (length > 0 && IsSpace(*localData)) {
      localData++;
      length--;
    }

    if (length > 0) {
      CurrentElement->AppendTextContent(std::string(localData, length));
    }
  }
}

void cmXMLEditorDocument::SaveAs(std::string const& fileName)
{
  cmGeneratedFileStream fs(fileName);
  cmXMLWriter writer(fs);
  writer.SetIndentationElement(this->IndentationElement);
  this->WriteXML(writer);
  fs.Close();
}

void cmXMLEditorDocument::WriteXML(cmXMLWriter& writer)
{
  writer.StartDocument();
  if (!this->Children.empty()) {
    this->Children.front().WriteXML(writer);
  }
  writer.EndDocument();
}

cmXMLEditorElement* cmXMLEditorElement::GetElement(std::string const& path,
                                                   bool create)
{
  std::vector<cmXMLEditorElement*> elements;
  this->GetElements(path, elements, create);
  if (!elements.empty()) {
    return elements.front();
  }
  return nullptr;
}

std::vector<cmXMLEditorElement*> cmXMLEditorElement::GetElements(
  std::string const& path)
{
  std::vector<cmXMLEditorElement*> elements;
  this->GetElements(path, elements, false);
  return elements;
}

void cmXMLEditorElement::GetElements(
  std::string const& path, std::vector<cmXMLEditorElement*>& elements,
  bool create)
{
  if (path.empty()) {
    // Empty path -> this is the element we are searching
    elements.push_back(this);
    return;
  }

  if (path[0] == '/') {
    // Rooted path -> Execute GetElements on the document
    GetDocument()->GetElements(path.substr(1, path.length() - 1), elements,
                               create);
    return;
  }

  XPath xPath = ParseXPath(path);

  bool found = false;

  for (cmXMLEditorElement& e : this->Children) {
    if (cmsysString_strcasecmp(e.Name.c_str(), xPath.ElementName.c_str()) ==
        0) {
      if (xPath.Predicate.Valid) {
        // Check the predicate
        cmXMLEditorElement* predicateElement =
          e.GetElement(xPath.Predicate.Path);
        if (!predicateElement) {
          continue;
        }
        std::string comparisonString = xPath.Predicate.Attribute.empty()
          ? predicateElement->TextContent
          : predicateElement->Attributes[xPath.Predicate.Attribute];
        if (comparisonString == xPath.Predicate.Value) {
          found = true;
          e.GetElements(xPath.RemainingPath, elements, create);
        }
      } else {
        // No predicate
        found = true;
        e.GetElements(xPath.RemainingPath, elements, create);
      }
    }
  }

  if (!found && create) {
    this->AddChild(xPath.ElementName)
      ->GetElements(xPath.RemainingPath, elements, create);
  }
}

void cmXMLEditorElement::WriteXML(cmXMLWriter& writer)
{
  writer.StartElement(this->GetName());

  for (auto const& attribute : this->GetAttributes()) {
    writer.Attribute(attribute.first.c_str(), attribute.second);
  }

  if (!this->GetTextContent().empty()) {
    writer.Content(this->GetTextContent());
  }

  for (auto child : this->GetChildren()) {
    child.WriteXML(writer);
  }

  writer.EndElement();
}
