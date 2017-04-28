#include <e3-api-wrapper.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void * zmq_context=NULL;
static struct tlv_major_index_base * api_tlv_index_base=NULL;

/*if the minor type :e3_tlv_api_minor_type_arg,
the first two bytes are _type_,next two are _behavior_,
next two are length,then real value follows*/

void e3_api_tlv_callback_func(struct tlv_header * tlv,void * value,void * arg)
{
	struct e3_api_service * service=(struct e3_api_service*)arg;
	uint16_t type_major=tlv->type_major;
	uint16_t type_minor=tlv->type_minor;
	assert(type_major==e3_tlv_major_type_api);
	switch(type_minor)
	{
		case e3_tlv_api_minor_type_begin:
			printf("api reset\n");
			service->arg_iptr=0;
			service->error_cnt=0;
			memset(service->args_ptr_array,0x0,sizeof(service->args_ptr_array));
			memset(&service->api_template,0x0,sizeof(service->api_template));
			break;
		case e3_tlv_api_minor_type_procname:
			service->api_template.api_name=(char*)value;
			printf("api name:%s\n",service->api_template.api_name);
			break;
		case e3_tlv_api_minor_type_arg:
			if(service->arg_iptr==MAX_ARGUMENT_SUPPORTED)
				break;
			uint8_t * lptr=(uint8_t*)value;
			service->api_template.args_desc[service->arg_iptr].type=*(uint16_t*)(0+lptr);
			service->api_template.args_desc[service->arg_iptr].behavior=*(uint16_t*)(2+lptr);
			service->api_template.args_desc[service->arg_iptr].len=*(uint16_t*)(4+lptr);
			lptr+=6;
			switch(service->api_template.args_desc[service->arg_iptr].type)
			{
				case e3_arg_type_uint8_t:
					service->args_ptr_array[service->arg_iptr]=cast_to_e3_type(*(uint8_t*)lptr);
					break;
				case e3_arg_type_uint16_t:
					service->args_ptr_array[service->arg_iptr]=cast_to_e3_type(*(uint16_t*)lptr);
					break;
				case e3_arg_type_uint32_t:
					service->args_ptr_array[service->arg_iptr]=cast_to_e3_type(*(uint32_t*)lptr);
					break;
				case e3_arg_type_uint64_t:
					service->args_ptr_array[service->arg_iptr]=cast_to_e3_type(*(uint64_t*)lptr);
					break;
				case e3_arg_type_uint8_t_ptr:
					service->args_ptr_array[service->arg_iptr]=cast_to_e3_type((uint8_t*)lptr);
					break;
				default:
					service->error_cnt++;
					break;
				
			}
			service->arg_iptr++;
			break;
		case e3_tlv_api_minor_type_end:
			service->api_template.args_desc[service->arg_iptr].type=e3_arg_type_none;
			
			break;
	}
}

struct tlv_callback_entry api_tlv_entires[]={
		{.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_procname),.callback_func=e3_api_tlv_callback_func},
		{.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_retval),.callback_func=e3_api_tlv_callback_func},
		{.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_arg),.callback_func=e3_api_tlv_callback_func},
		{.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_begin),.callback_func=e3_api_tlv_callback_func},
		{.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_end),.callback_func=e3_api_tlv_callback_func},
		{.type=0,.callback_func=0},
};


__attribute__((constructor)) 
	void e3_api_service_init(void)
{
	assert(zmq_context=zmq_ctx_new());
	assert(api_tlv_index_base=allocate_tlv_major_index_base_from_glibc());
	api_tlv_index_base[e3_tlv_major_type_api].entries=api_tlv_entires;
}

struct e3_api_service * allocate_e3_api_service(char * service_endpoint_to_bind)
{
	int rc;
	struct e3_api_service * service=NULL;
	service=malloc(sizeof(struct e3_api_service));
	if(!service)
		goto  error_dealloc;
	memset(service,0x0,sizeof(struct e3_api_service));
	service->socket_handler=zmq_socket(zmq_context,ZMQ_REP);
	if(!service->socket_handler)
		goto error_zmq;
	rc=zmq_bind(service->socket_handler,service_endpoint_to_bind);
	if(rc)
		goto error_zmq;
	
	return service;
	error_zmq:
		if(service->socket_handler)
			zmq_close(service->socket_handler);	
	error_dealloc:
		if(service)
			free(service);
	return NULL;
}

struct e3_api_client * allocate_e3_api_client(char * service_endpoint_to_connect)
{
	int rc;
	struct e3_api_client * client=malloc(sizeof(struct e3_api_client));
	if(!client)
		goto  error_dealloc;
	memset(client,0x0,sizeof(struct e3_api_client));
	client->socket_handler=zmq_socket(zmq_context,ZMQ_REQ);
	if(!client->socket_handler)
		goto error_zmq;
	rc=zmq_connect(client->socket_handler,service_endpoint_to_connect);
	if(rc)
		goto error_zmq;

	return client;
	error_zmq:
		if(client->socket_handler)
			zmq_close(client->socket_handler);
	error_dealloc:
		if(client)
			free(client);
	return NULL;
}


int e3_api_service_try_to_poll_request(struct e3_api_service * service)
{
	int rc=zmq_recv(service->socket_handler,
		service->recv_mbuf,
		MAX_MSG_LENGTH,
		ZMQ_DONTWAIT);
	if(rc<0){
		/*here we do not check errno any more,
		bypass it quietly*/
		service->recv_length=0;
	}
	else 
		service->recv_length=rc;
	return 0;
}

int e3_api_service_dispatch_apis(struct e3_api_service * service)
{
	#define _(con) if(!(con)) goto ret;
	struct message_header * msg_hdr=(struct message_header*)service->recv_mbuf;
	_(validate_tlv_encoding(service->recv_mbuf,service->recv_length));
	_(msg_hdr->nr_tlvs);
	_(!message_walk_through_tlv_entries(api_tlv_index_base,
		msg_hdr,
		(uint8_t*)(msg_hdr+1),
		service));
	#undef _
	ret:
	return 0;
}
int e3_api_service_send_reponse(struct e3_api_service * service)
{
	service->send_length=service->send_length<0?0:service->send_length;
	zmq_send(service->socket_handler,
		service->send_mbuf,
		service->send_length,
		0);
	service->send_length=0;
	return 0;
}