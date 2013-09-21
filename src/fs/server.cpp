//#include "fused.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "microhttpd.h"

using std::string;
const short int kPort = 8888;

int client_processor(void* cls, MHD_Connection* connection,
                     const char* url,
                     const char* method, const char* version,
                     const char* upload_data,
                     size_t* upload_data_size, void** con_cls)
{
  string page = "<html><body>Hello, tagfs.";
  page += ", url = ";
  page += url;
  page += ", method = ";
  page += method;
  page += ", version = ";
  page += version;
  page += " </body></html>";
  printf("%d: %s\n", page.length(), page.c_str());
  MHD_Response* response;
  int result;
  response = MHD_create_response_from_buffer(page.size(),
    (void*)page.c_str(), MHD_RESPMEM_MUST_COPY);

  result = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);

  return result;
}

int main(int argc, char* argv[])
{
  MHD_Daemon* daemon;
  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, kPort, NULL, NULL,
    &client_processor, NULL, MHD_OPTION_END);

  if (daemon == NULL) {
    return 1;
  }

  getchar();

  MHD_stop_daemon(daemon);
  return 0;
}
