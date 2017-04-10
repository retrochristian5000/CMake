/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
file Copyright.txt or https://cmake.org/licensing for details.  */

#pragma once

#include "cmConnection.h"
#include "cm_uv.h"
#include <memory>
#include <string>
#include <vector>

class cmPipeConnection : public cmEventBasedConnection
{
public:
  cmPipeConnection(const std::string& name,
                   cmConnectionBufferStrategy* bufferStrategy = 0);

  bool OnServeStart(std::string* pString) override;

  bool OnServerShuttingDown() override;

  void Connect(uv_stream_t* server) override;

private:
  const std::string PipeName;
  uv_pipe_t* ServerPipe = 0;
  uv_pipe_t* ClientPipe = 0;
};
