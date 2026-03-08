/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmEnvironment.h"

#include <utility>

cmEnvironment::cmEnvironment(std::vector<std::string> const& env)
{
  for (std::string const& var : env) {
    this->PutEnv(var);
  }
}

void cmEnvironment::PutEnv(std::string const& env)
{
  auto const pos = env.find('=');
  if (pos != std::string::npos) {
    this->Map[env.substr(0, pos)] = env.substr(pos + 1);
  } else {
    this->Map[env] = cm::nullopt;
  }
}

void cmEnvironment::UnPutEnv(std::string const& env)
{
  this->Map[env] = cm::nullopt;
}

void cmEnvironment::Update(cmEnvironment&& other)
{
  for (auto& kv : other.Map) {
    this->Map[kv.first] = std::move(kv.second);
  }
  other.Map.clear();
}

std::vector<std::string> cmEnvironment::GetVariables() const
{
  auto result = std::vector<std::string>{};
  result.reserve(this->Map.size());
  for (auto const& elem : this->Map) {
    if (elem.second) {
      result.push_back(elem.first + '=' + *elem.second);
    }
  }
  return result;
}
