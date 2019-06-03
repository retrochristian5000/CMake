/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmPropertyMap_h
#define cmPropertyMap_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <map>
#include <string>
#include <utility>
#include <vector>

class cmPropertyMap
{
public:
  // -- General
  //! Clear property list
  void Clear();

  // -- Properties
  void SetProperty(const std::string& name, const char* value);

  void AppendProperty(const std::string& name, const char* value,
                      bool asString = false);

  //! Returns the value of the property @a name
  const char* Get(const std::string& name) const;

  //! Remove the property @a name from the map
  void Remove(const std::string& name);

  // -- Lists
  //! Get a sorted list of property keys
  std::vector<std::string> GetKeys() const;

  //! Get a sorted by key list of property key,value pairs
  std::vector<std::pair<std::string, std::string>> GetList() const;

private:
  std::map<std::string, std::string> Map_;
};

#endif
