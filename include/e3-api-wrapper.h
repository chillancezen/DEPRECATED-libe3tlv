#ifndef _E3_API_WRAPPER_H
#define _E3_API_WRAPPER_H
#include <e3-api.h>
#include <zmq.h>

/*system global definition*/

enum e3_tlv_major_type{
	e3_tlv_major_type_none=0,
	e3_tlv_major_type_api
};

enum e3_tlv_api_minor_type{
	e3_tlv_api_minor_type_none=0,
	e3_tlv_api_minor_type_begin,
	e3_tlv_api_minor_type_procname,
	e3_tlv_api_minor_type_retval,
	e3_tlv_api_minor_type_arg,
	e3_tlv_api_minor_type_end,
};
#define MAX_MSG_LENGTH (1024*16) 

struct e3_api_service{
	void * socket_handler;
	uint8_t recv_mbuf[MAX_MSG_LENGTH];
	uint8_t send_mbuf[MAX_MSG_LENGTH];
	int recv_length;
	int send_length;
	
	struct e3_api_declaration api_template;
	e3_type args_ptr_array[MAX_ARGUMENT_SUPPORTED+1];
	int arg_iptr;
	int error_cnt;
	
};

struct e3_api_client{
	void * socket_handler;
};
struct e3_api_service * allocate_e3_api_service(char * service_endpoint_to_bind);
struct e3_api_client * allocate_e3_api_client(char * service_endpoint_to_connect);
int e3_api_service_try_to_poll_request(struct e3_api_service * service);
int e3_api_service_dispatch_apis(struct e3_api_service * service);
int e3_api_service_send_reponse(struct e3_api_service * service);


#endif