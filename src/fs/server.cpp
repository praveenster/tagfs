//#include "fused.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "microhttpd.h"
#include "String.h"
#include "JsonValue.h"
#include "JsonObject.h"
#include "JsonString.h"
#include "JsonArray.h"
#include "JsonNull.h"
#include "JsonNumber.h"
#include "SmartPointer.h"

using std::vector;

using lepcpplib::JsonValue;
using lepcpplib::JsonObject;
using lepcpplib::JsonString;
using lepcpplib::JsonArray;
using lepcpplib::JsonNull;
using lepcpplib::JsonNumber;
using lepcpplib::SmartPointer;
using lepcpplib::String;

const short int kPort = 8888;
const int kFileBlockSize = 32 * 1024 * 1024;
const String k404 = "<html><head><title>File not found</title></head><body>File not found</body></html>";

int enqueue_404_response(MHD_Connection* connection)
{
    MHD_Response* response = MHD_create_response_from_buffer(k404.length(),
      (void*)k404.toCharArray(), MHD_RESPMEM_PERSISTENT);

    int result = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);

    return result;
}

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

int serve_file(MHD_Connection* connection, String& filename)
{
  MHD_Response* response = NULL;
  struct stat filestats;
  int result = stat(filename.toCharArray(), &filestats);
  if (result != -1) {
    FILE* fp = fopen(filename.toCharArray(), "r");
    response = MHD_create_response_from_callback (filestats.st_size,
               kFileBlockSize, file_server_callback, fp, file_server_free_callback);
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
  }
  else {
    result = enqueue_404_response(connection);
  }

  MHD_destroy_response(response);
  return result;
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

  return serve_file(connection, filename);
}

int route_api(void* cls, MHD_Connection* connection,
              const char* url,
              const char* method, const char* version,
              const char* upload_data,
              size_t* upload_data_size, void** con_cls,
              vector<String>& tokens)
{
  SmartPointer<String> output;

  int token_count = tokens.size();
  if ((token_count >= 4) && (tokens[2] == "get") && (tokens[3] == "tags")) {
    JsonObject* j1 = new JsonObject();
    j1->Add(new JsonString("name"), new JsonString("root"));
    j1->Add(new JsonString("id"), new JsonNumber(0));
    j1->Add(new JsonString("parent"), new JsonNull());

    JsonObject* j2 = new JsonObject();
    j2->Add(new JsonString("name"), new JsonString("home"));
    j2->Add(new JsonString("id"), new JsonNumber(1));
    j2->Add(new JsonString("parent"), new JsonString("root"));

    JsonArray* a = new JsonArray();
    a->Add(j1);
    a->Add(j2);

    SmartPointer<JsonObject> j = new JsonObject();
    j->Add(new JsonString("tags"), a);

    output = j->ToString();
  }
  else if ((token_count >= 5) && (tokens[2] == "get") && (tokens[3] == "tag")) {
    int tag_id = String::toInt(tokens[4].toCharArray());

    if (tag_id == 0) {
      JsonObject* j1 = new JsonObject();
      j1->Add(new JsonString("name"), new JsonString("file1.txt"));
      j1->Add(new JsonString("id"), new JsonNumber(0));

      JsonObject* j2 = new JsonObject();
      j2->Add(new JsonString("name"), new JsonString("file2.jpg"));
      j2->Add(new JsonString("id"), new JsonNumber(1));

      JsonArray* a = new JsonArray();
      a->Add(j1);
      a->Add(j2);

      SmartPointer<JsonObject> j = new JsonObject();
      j->Add(new JsonString("files"), a);
      output = j->ToString();
    }
    else {
      JsonObject* j1 = new JsonObject();
      j1->Add(new JsonString("name"), new JsonString("file3.mp4"));
      j1->Add(new JsonString("id"), new JsonNumber(2));

      JsonObject* j2 = new JsonObject();
      j2->Add(new JsonString("name"), new JsonString("file4.mp3"));
      j2->Add(new JsonString("id"), new JsonNumber(3));

      JsonArray* a = new JsonArray();
      a->Add(j1);
      a->Add(j2);

      SmartPointer<JsonObject> j = new JsonObject();
      j->Add(new JsonString("files"), a);
      output = j->ToString();
    }
  }
  else if ((token_count >= 5) && (tokens[2] == "get") && (tokens[3] == "file")) {
    int file_id = String::toInt(tokens[4].toCharArray());

    if (file_id == 0) {
      SmartPointer<JsonObject> j = new JsonObject();
      j->Add(new JsonString("name"), new JsonString("file1.txt"));
      j->Add(new JsonString("id"), new JsonNumber(0));
      j->Add(new JsonString("type"), new JsonString("Text document"));
      j->Add(new JsonString("last_modified"), new JsonString("Sat 14 Sep 2013 01:14:35 AM PDT"));

      JsonObject* j1 = new JsonObject();
      j1->Add(new JsonString("name"), new JsonString("anothertag1"));
      j1->Add(new JsonString("id"), new JsonNumber(3));

      JsonObject* j2 = new JsonObject();
      j2->Add(new JsonString("name"), new JsonString("anothertag2"));
      j2->Add(new JsonString("id"), new JsonNumber(4));

      JsonArray* a = new JsonArray();
      a->Add(j1);
      a->Add(j2);

      j->Add(new JsonString("tags"), a);
      output = j->ToString();
    }
    else {
      SmartPointer<JsonObject> j = new JsonObject();
      j->Add(new JsonString("name"), new JsonString("file2.jpg"));
      j->Add(new JsonString("id"), new JsonNumber(0));
      j->Add(new JsonString("type"), new JsonString("JPEG image"));
      j->Add(new JsonString("last_modified"), new JsonString("Sat 14 Sep 2013 01:14:35 AM PDT"));


      JsonObject* j1 = new JsonObject();
      j1->Add(new JsonString("name"), new JsonString("anothertag3"));
      j1->Add(new JsonString("id"), new JsonNumber(4));

      JsonObject* j2 = new JsonObject();
      j2->Add(new JsonString("name"), new JsonString("anothertag4"));
      j2->Add(new JsonString("id"), new JsonNumber(5));

      JsonArray* a = new JsonArray();
      a->Add(j1);
      a->Add(j2);

      j->Add(new JsonString("tags"), a);
      output = j->ToString();
    }
  }
  else {
    SmartPointer<JsonObject> j = new JsonObject();
    j->Add(new JsonString("error"), new JsonString("invalid api"));

    output = j->ToString();
  }

  MHD_Response* response = MHD_create_response_from_buffer(output->length(),
    (void*)output->toCharArray(), MHD_RESPMEM_MUST_COPY);

  int result = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return result;
}

int client_processor(void* cls, MHD_Connection* connection,
                     const char* url,
                     const char* method, const char* version,
                     const char* upload_data,
                     size_t* upload_data_size, void** con_cls)
{
  vector<String> tokens;
  String::tokenize(url, '/', tokens);
  int result;

  if (tokens.size() >= 2) {
    if (tokens[1] == "discover") {
      result = route_discover(cls, connection, url, method, version,
        upload_data, upload_data_size, con_cls, tokens);
    }
    else if (tokens[1] == "api") {
      route_api(cls, connection, url, method, version,
        upload_data, upload_data_size, con_cls, tokens);
    }
    else {
      result = enqueue_404_response(connection);
    }
  }

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
