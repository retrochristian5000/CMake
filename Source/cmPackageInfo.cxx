/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmPackageInfo.h"

#include <cstddef>
#include <new>
#include <utility>

#include "cmJSONState.h"
#include "cmStringAlgorithms.h"

namespace cmPackageInfo {

LanguageStringLists::LanguageStringLists()
  : Tag_(Tag::List)
{
  new (&this->StringList_) StringList;
}

LanguageStringLists::LanguageStringLists(StringList values)
  : Tag_(Tag::List)
{
  new (&this->StringList_) StringList(std::move(values));
}

LanguageStringLists::LanguageStringLists(LanguageMap languages)
  : Tag_(Tag::ByLanguage)
{
  new (&this->LanguageMap_) LanguageMap(std::move(languages));
}

LanguageStringLists::LanguageStringLists(LanguageStringLists const& other)
  : Tag_(other.Tag_)
{
  if (this->Tag_ == Tag::List) {
    new (&this->StringList_) StringList(other.StringList_);
  } else {
    new (&this->LanguageMap_) LanguageMap(other.LanguageMap_);
  }
}

LanguageStringLists::LanguageStringLists(LanguageStringLists&& other) noexcept
  : Tag_(other.Tag_)
{
  if (this->Tag_ == Tag::List) {
    new (&this->StringList_) StringList(std::move(other.StringList_));
  } else {
    new (&this->LanguageMap_) LanguageMap(std::move(other.LanguageMap_));
  }
}

LanguageStringLists& LanguageStringLists::operator=(
  LanguageStringLists const& other)
{
  if (this != &other) {
    // Copy before destroying the active member, so a failed allocation leaves
    // this object unchanged. Moving the standard containers cannot throw.
    LanguageStringLists copy(other);
    *this = std::move(copy);
  }
  return *this;
}

LanguageStringLists& LanguageStringLists::operator=(
  LanguageStringLists&& other) noexcept
{
  if (this != &other) {
    this->Destroy();
    this->Tag_ = other.Tag_;
    if (this->Tag_ == Tag::List) {
      new (&this->StringList_) StringList(std::move(other.StringList_));
    } else {
      new (&this->LanguageMap_) LanguageMap(std::move(other.LanguageMap_));
    }
  }
  return *this;
}

LanguageStringLists::~LanguageStringLists()
{
  this->Destroy();
}

void LanguageStringLists::Destroy()
{
  if (this->Tag_ == Tag::List) {
    this->StringList_.~StringList();
  } else {
    this->LanguageMap_.~LanguageMap();
  }
}

namespace {

void CheckVersion(cmJSONState& state, SchemaVersion version,
                  std::string const& attribute)
{
  if (version < SchemaVersion(0, 13) || version >= SchemaVersion(0, 16)) {
    state.AddError(cmStrCat(
      attribute, " expected a supported CPS version (0.13 through 0.15)"));
  }
}

void CheckName(cmJSONState& state, std::string const& name,
               std::string const& description, bool package = false)
{
  if (name.empty() ||
      name.find_first_of(package ? "/@:" : "/@") != std::string::npos) {
    state.AddError(cmStrCat(description, " has an invalid or empty CPS name"));
  }
}

void CheckPackage(cmJSONState& state, Package const& package)
{
  CheckName(state, package.Name, "Package", true);
  for (auto const& entry : package.Components) {
    std::string const description = cmStrCat("Component \"", entry.first, '"');
    CheckName(state, entry.first, description);
    Component const& component = entry.second;
    if (component.Type.empty()) {
      state.AddError(cmStrCat(description, " requires a non-empty type"));
    }
    if (auto const* configurations = component.Configurations.Get()) {
      for (auto const& config : *configurations) {
        CheckName(
          state, config.first,
          cmStrCat(description, " configuration \"", config.first, '"'));
      }
    }
    if (auto const* fileSets = component.FileSets.Get()) {
      for (std::size_t i = 0; i < fileSets->size(); ++i) {
        if ((*fileSets)[i].Type.empty()) {
          state.AddError(cmStrCat(description, " file set ", i,
                                  " requires a non-empty type"));
        }
      }
    }
  }
  if (auto const* requirements = package.Requires.Get()) {
    for (auto const& entry : *requirements) {
      CheckName(state, entry.first,
                cmStrCat("Requirement \"", entry.first, '"'), true);
    }
  }
}

} // namespace

bool Validate(Root const& document, cmJSONState& state)
{
  CheckVersion(state, document.CpsVersion, "\"cps_version\"");
  CheckPackage(state, document);
  auto const* prefix = document.Prefix.Get();
  auto const* cpsPath = document.CpsPath.Get();
  if (static_cast<bool>(prefix) == static_cast<bool>(cpsPath)) {
    state.AddError("Exactly one of 'prefix' and 'cps_path' is required");
  }
  if (prefix && prefix->empty()) {
    state.AddError("\"prefix\" must be non-empty");
  }
  if (cpsPath && *cpsPath != "@prefix@" &&
      cpsPath->compare(0, 9, "@prefix@/") != 0) {
    state.AddError(
      "\"cps_path\" expected '@prefix@' or a path below '@prefix@/'");
  }
  return state.errors.empty();
}

bool Validate(Appendix const& document, SchemaVersion inheritedCpsVersion,
              cmJSONState& state)
{
  CheckVersion(state, inheritedCpsVersion, "Inherited CPS version");
  if (auto const* version = document.CpsVersion.Get()) {
    CheckVersion(state, *version, "\"cps_version\"");
  }
  CheckPackage(state, document);
  return state.errors.empty();
}

bool Validate(ConfigurationSupplement const& document,
              SchemaVersion inheritedCpsVersion, cmJSONState& state)
{
  CheckVersion(state, inheritedCpsVersion, "Inherited CPS version");
  CheckName(state, document.Name, "Package", true);
  CheckName(state, document.Configuration, "Package configuration");
  for (auto const& entry : document.Components) {
    CheckName(state, entry.first, cmStrCat("Component \"", entry.first, '"'));
  }
  return state.errors.empty();
}

} // namespace cmPackageInfo
