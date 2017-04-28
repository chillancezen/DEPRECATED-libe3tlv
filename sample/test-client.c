#include <e3-api-wrapper.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

char buffer[256];




int main(int argc,char**argv)
{
	struct e3_api_client * client=allocate_e3_api_client("tcp://localhost:507");
	assert(client);
	struct message_builder  builder;
	void * value;
	int rc=message_builder_init(&builder,buffer,180);
	struct tlv_header tlv;

	tlv.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_begin);
	tlv.length=0;
	message_builder_add_tlv(&builder,&tlv,NULL);
	
	tlv.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_procname);
	tlv.length=strlen("foo_rpc_func")+1;
	message_builder_add_tlv(&builder,&tlv,"foo_rpc_func");

	
	rc=zmq_send(client->socket_handler,buffer,builder.buffer_iptr,0);
	getchar();
	return 0;
}
