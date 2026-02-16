/*
 * http_server_example.h
 *
 *  Created on: Nov 15, 2025
 *      Author: controllerstech
 */

#ifndef ETHERNET_HTTP_SERVER_EXAMPLE_H_
#define ETHERNET_HTTP_SERVER_EXAMPLE_H_

#include "httpServer/httpParser.h"

void httpServer_setup(void);
void httpServerRun (void);
void http_dynamic_handler(st_http_request * p_http_request);



#endif /* ETHERNET_HTTP_SERVER_EXAMPLE_H_ */
