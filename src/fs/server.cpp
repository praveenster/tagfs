//#include "fused.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <vector>
#include "microhttpd.h"

using std::vector;
using lepcpplib::String;
const short int kPort = 8888;

ssize_t file_server_callback(void *cls, uint64_t pos, char *buf, size_t max)
{
  FILE* f = (FILE*)cls;
  fseek(f, pos, SEEK_SET);
  return fread(buf, 1, max, f);
}

void file_server_free_callback(void* cls)
{
  fclose((FILE*)cls);
}

void serve_file()
{
  #if 0
  struct MHD_Response *
MHD_create_response_from_callback (uint64_t size,
           size_t block_size,
           MHD_ContentReaderCallback crc, void *crc_cls,
           MHD_ContentReaderFreeCallback crfc);
           #endif
}

int route_discover(void* cls, MHD_Connection* connection,
                   const char* url,
                   const char* method, const char* version,
                   const char* upload_data,
                   size_t* upload_data_size, void** con_cls,
                   vector<String>& tokens)
{
  MHD_Response* response;
  int result;
  String filename;
  if (tokens.size() == 2) {
    filename = "../www/html/discover.html";
  }
  else {
    filename = "../www/html";
    for (int i = 2; i < tokens.size(); ++i)
    {
      filename = filename + "/" + tokens[i]; 
    }
  }

  FILE* fp = fopen(filename.toCharArray(), "r");
  int fd = fileno(fp);
  struct stat buf;
  stat(filename.toCharArray(), &buf);

  //response = MHD_create_response_from_buffer(output.length(),
  //  (void*)output.toCharArray(), MHD_RESPMEM_MUST_COPY);
  response = MHD_create_response_from_fd(buf.st_size, fd);

  result = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return result;
}

int route_www(void* cls, MHD_Connection* connection,
              const char* url,
              const char* method, const char* version,
              const char* upload_data,
              size_t* upload_data_size, void** con_cls,
              String& output)
{
  output = "<html><body>Hello, discoverer renderer caller.</body></html>";
}

int route_api(void* cls, MHD_Connection* connection,
              const char* url,
              const char* method, const char* version,
              const char* upload_data,
              size_t* upload_data_size, void** con_cls,
              String& output)
{
  output = "<html><body>Hello, api caller.</body></html>";
}

int client_processor(void* cls, MHD_Connection* connection,
                     const char* url,
                     const char* method, const char* version,
                     const char* upload_data,
                     size_t* upload_data_size, void** con_cls)
{
  vector<String> tokens;
  String::tokenize(url, '/', tokens);
  String output;

  if (tokens.size() >= 2) {
    if (tokens[1] == "discover") {
      return route_discover(cls, connection, url, method, version,
        upload_data, upload_data_size, con_cls, tokens);
    }
    else if (tokens[1] == "api") {
      route_api(cls, connection, url, method, version,
        upload_data, upload_data_size, con_cls, output);
    }
    else if (tokens[1] == "www") {
      route_www(cls, connection, url, method, version,
        upload_data, upload_data_size, con_cls, output);
    }
    else {
      output = "<html><body>Hello, 404 found not.</body></html>";
    }
  }

  MHD_Response* response;
  int result;
  FILE* fp = fopen("../www/html/discover.html", "r");
  int fd = fileno(fp);
  struct stat buf;
  stat("../www/html/discover.html", &buf);


  //response = MHD_create_response_from_buffer(output.length(),
  //  (void*)output.toCharArray(), MHD_RESPMEM_MUST_COPY);
  response = MHD_create_response_from_fd(buf.st_size, fd);

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
