/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */

#include "cmConnection.h"
#include "cmServer.h"
#include "cmServerConnection.h"
#include "cm_uv.h"
#include "string.h"
#include <fstream>

struct write_req_t
{
  uv_write_t req;
  uv_buf_t buf;
};

void cmEventBasedConnection::on_alloc_buffer(uv_handle_t* handle,
                                             size_t suggested_size,
                                             uv_buf_t* buf)
{
  (void)(handle);
  char* rawBuffer = new char[suggested_size];
  *buf = uv_buf_init(rawBuffer, static_cast<unsigned int>(suggested_size));
}

void cmEventBasedConnection::on_read(uv_stream_t* stream, ssize_t nread,
                                     const uv_buf_t* buf)
{
  auto conn = reinterpret_cast<cmEventBasedConnection*>(stream->data);
  if (nread >= 0) {
    conn->ReadData(std::string(buf->base, buf->base + nread));
  } else {
    conn->OnDisconnect((int)nread);
  }

  delete[](buf->base);
}

void cmEventBasedConnection::on_close_delete(uv_handle_t* handle)
{
  delete handle;
}

void cmEventBasedConnection::on_close(uv_handle_t* /*handle*/)
{
}

void cmEventBasedConnection::on_write(uv_write_t* req, int status)
{
  (void)(status);
  auto conn = reinterpret_cast<cmEventBasedConnection*>(req->data);

  // Free req and buffer
  write_req_t* wr = reinterpret_cast<write_req_t*>(req);
  delete[](wr->buf.base);
  delete wr;

  conn->ProcessNextRequest();
}

void cmEventBasedConnection::on_new_connection(uv_stream_t* stream, int status)
{
  (void)(status);
  auto conn = reinterpret_cast<cmEventBasedConnection*>(stream->data);
  conn->Connect(stream);
}

bool cmEventBasedConnection::IsOpen() const
{
  return this->WriteStream != CM_NULLPTR;
}

void cmEventBasedConnection::WriteData(const std::string& data)
{
  assert(this->WriteStream);

  auto ds = data.size();

  write_req_t* req = new write_req_t;
  req->req.data = this;
  req->buf = uv_buf_init(new char[ds], static_cast<unsigned int>(ds));
  memcpy(req->buf.base, data.c_str(), ds);
  uv_write(reinterpret_cast<uv_write_t*>(req),
           static_cast<uv_stream_t*>(this->WriteStream), &req->buf, 1,
           on_write);
}

void cmEventBasedConnection::ReadData(const std::string& data)
{
  this->RawReadBuffer += data;
  if (BufferStrategy) {
    std::string packet = BufferStrategy->BufferMessage(this->RawReadBuffer);
    do {
      QueueRequest(packet);
      packet = BufferStrategy->BufferMessage(this->RawReadBuffer);
    } while (!packet.empty());

  } else {
    QueueRequest(this->RawReadBuffer);
    this->RawReadBuffer.clear();
  }
}

cmEventBasedConnection::cmEventBasedConnection(
  cmConnectionBufferStrategy* bufferStrategy)
  : BufferStrategy(bufferStrategy)
{
}

void cmEventBasedConnection::Connect(uv_stream_t* server)
{
  (void)server;
  Server->OnConnected(nullptr);
}

void cmEventBasedConnection::OnDisconnect(int onerror)
{
  (void)onerror;
  this->Server->OnDisconnect(this);
}

cmConnection::~cmConnection()
{
  OnServerShuttingDown();
}

bool cmConnection::OnServerShuttingDown()
{
  return true;
}

void cmConnection::ProcessNextRequest()
{
  if (this->Queue.empty()) {
    return;
  }

  const std::string input = this->Queue.front();
  this->Queue.pop_front();
  Server->ProcessRequest(this, input);
}

void cmConnection::SetServer(cmServerBase* s)
{
  Server = s;
}

void cmConnection::QueueRequest(const std::string& request)
{
  this->Queue.push_back(request);
  this->Server->NotifyDataQueued();
}

bool cmConnection::OnServeStart(std::string* errString)
{
  (void)errString;
  return true;
}
