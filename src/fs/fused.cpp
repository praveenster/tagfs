//#include "fused.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "microhttpd.h"

const short int kPort = 8888;

int client_processor(void* cls, struct MHD_Connection* connection,
                     const char* url,
                     const char* method, const char* version,
                     const char* upload_data,
                     size_t* upload_data_size, void** con_cls)
{
	const char* page = "<html><body>Hello, tagfs!</body></html>";
	struct MHD_Response* response;
	int result;
	response = MHD_create_response_from_buffer(strlen(page),
		(void*)page, MHD_RESPMEM_PERSISTENT);

	result = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return result;
}

int main(int argc, char* argv[])
{
	struct MHD_Daemon* daemon;
	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, kPort, NULL, NULL,
		&client_processor, NULL, MHD_OPTION_END);

	if (daemon == NULL) {
		return 1;
	}

	getchar();

	MHD_stop_daemon(daemon);
	return 0;
}