/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmTargetPrecompileHeadersCommand_h
#define cmTargetPrecompileHeadersCommand_h

#include "cmConfigure.h"

#include <string>
#include <vector>

#include "cmTargetPropCommandBase.h"

class cmCommand;
class cmExecutionStatus;
class cmTarget;

class cmTargetPrecompileHeadersCommand : public cmTargetPropCommandBase
{
public:
  cmCommand* Clone() CM_OVERRIDE
  {
    return new cmTargetPrecompileHeadersCommand;
  }

  bool InitialPass(std::vector<std::string> const& args,
                   cmExecutionStatus& status) CM_OVERRIDE;

private:
  void HandleImportedTarget(const std::string& tgt) CM_OVERRIDE;
  void HandleMissingTarget(const std::string& name) CM_OVERRIDE;

  bool HandleDirectContent(cmTarget* tgt,
                           const std::vector<std::string>& content,
                           bool prepend, bool system) CM_OVERRIDE;

  std::string Join(const std::vector<std::string>& content) CM_OVERRIDE;
};

#endif
