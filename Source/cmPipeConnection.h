/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#pragma once

#include "cmConnection.h"

#include "cm_uv.h"

#include <string>

class cmPipeConnection : public cmEventBasedConnection
{
public:
  cmPipeConnection(const std::string& name,
                   cmConnectionBufferStrategy* bufferStrategy = nullptr);

  bool OnServeStart(std::string* pString) CM_OVERRIDE;

  bool OnConnectionShuttingDown() CM_OVERRIDE;

  void Connect(uv_stream_t* server) CM_OVERRIDE;

private:
  const std::string PipeName;
  uv_pipe_t* ServerPipe = nullptr;
  uv_pipe_t* ClientPipe = nullptr;
};
