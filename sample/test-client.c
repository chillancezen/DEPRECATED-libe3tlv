#include <e3-api-wrapper.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


declare_e3_api_client_pointer();




DECLARE_E3_API(l2_api)={
	.api_name="l2_add_fib",
	.args_desc={
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_input_and_output,.len=64},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_output,.len=8},
		{.type=e3_arg_type_none},
	}
};

/*the function below should be exported,
so do not include any other parameters 
except those delivered to remote endpoint*/
uint64_t l2_add_fib(uint64_t * api_ret,uint8_t foo,uint8_t * bar,uint8_t* bar1)
{
	int rc=0;
	#define _(con) if(!(con)) goto error;
	e3_type real_args[MAX_ARGUMENT_SUPPORTED];
	
	struct message_header * msg_hdr;
	struct e3_api_client * client=deference_e3_api_client_pointer();
	struct e3_api_declaration * api=search_e3_api_by_name("l2_add_fib");
	void * output_list[MAX_ARGUMENT_SUPPORTED+1];/*the first element always is api_ret,mandatory*/
	_(client);
	_(api);

	real_args[0]=cast_to_e3_type(foo);
	real_args[1]=cast_to_e3_type(bar);
	real_args[2]=cast_to_e3_type(bar1);
	output_list[0]=api_ret;
	output_list[1]=bar;
	output_list[2]=bar1;
	client->para_output_list=output_list;
	client->nr_output_list=3;
	
	_(!encode_e3_api_request(client->send_mbuf,MAX_MSG_LENGTH,api,real_args));
	_(!issue_e3_api_request(client));
	#undef _
	return 0;
	error:

		return -1;
}



int main(int argc,char**argv)
{
	struct e3_api_client * client=allocate_e3_api_client("tcp://localhost:507");
	assert(client);
	export_e3_api_client_pointer(client);
	uint64_t api_ret;
	char foo[64];
	uint64_t foo1;
	strcpy(foo,"hello world,miss");
	int rc=l2_add_fib(&api_ret,0x25,foo,(uint8_t*)&foo1);
	printf("call rc %d:%p %s %p\n",rc,api_ret,foo,(void*)foo1);
	
	#if 0
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

	tlv.type=MAKE_UINT32(e3_tlv_major_type_api,e3_tlv_api_minor_type_end);
	tlv.length=0;
	message_builder_add_tlv(&builder,&tlv,NULL);
	
	rc=zmq_send(client->socket_handler,buffer,builder.buffer_iptr,0);
	#endif
	getchar();
	return 0;
}
