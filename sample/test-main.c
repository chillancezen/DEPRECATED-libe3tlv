
#include <e3-tlv.h>
#include <e3-api.h>
#include <assert.h>
#include <stdio.h>
#include <e3-api-wrapper.h>
char buffer[256];

enum major_type{
	major_type_foo=1,
	major_type_bar=2,
};


void default_callback_func(struct tlv_header * tlv,void * value,void * arg)
{
	assert(value==tlv->value);
	printf("%d %s\n",tlv->length,tlv->value);
}

struct tlv_callback_entry tlv_entires[]={
		{.type=MAKE_UINT32(major_type_foo,1),.callback_func=default_callback_func},
		{.type=MAKE_UINT32(major_type_foo,2),.callback_func=default_callback_func},
		{.type=0,.callback_func=0},
};
e3_type dummy_l2_func(e3_type foo,e3_type arg)
{
	return 0;
}
DECLARE_E3_API(l2_api)={
	.api_name="l2_add_fib",
	.api_callback_func=(api_callback_func)dummy_l2_func,
	.args_desc={
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_input,.len=64},
		{.type=e3_arg_type_none},
	}
};


DECLARE_E3_API(l3_api)={
	.api_name="l3_add_fib",
};


int main()
{
	
	
	
	
	#if 0
	/*
	struct message_builder  builder;
	void * value;
	int rc=message_builder_init(&builder,buffer,180);
	
	
	struct tlv_header tlv;

	tlv.type=MAKE_UINT32(major_type_foo,1);
	tlv.length=12;
	rc=message_builder_add_tlv(&builder,&tlv,"hello world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);

	tlv.type=MAKE_UINT32(major_type_foo,2);
	tlv.length=0;
	rc=message_builder_add_tlv(&builder,&tlv,"m world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);
	message_builder_expand_tlv(&builder,23);
	*/
	struct message_builder  builder;
	void * value;
	int rc=message_builder_init(&builder,buffer,180);
	
	printf("%d\n",e3_arg_behavior_output);
	struct tlv_header tlv;

	tlv.type=MAKE_UINT32(major_type_foo,1);
	tlv.length=12;
	rc=message_builder_add_tlv(&builder,&tlv,"hello world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);

	tlv.type=MAKE_UINT32(major_type_foo,2);
	tlv.length=0;
	rc=message_builder_add_tlv(&builder,&tlv,"m world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);

	uint8_t *start=message_builder_expand_tlv(&builder,10);
	strcpy(start,",meeeow");

	struct tlv_major_index_base * base=allocate_tlv_major_index_base_from_glibc();
	assert(base);
	base[major_type_foo].entries=tlv_entires;
	
	rc=message_walk_through_tlv_entries(base,builder.msg_hdr_ptr,builder.msg_hdr_ptr+1,0);
	printf("walk %d\n",rc);
	
	struct e3_api_declaration * api=e3_api_head;
		for(;api;api=api->next){
			printf("%s\n",api->api_name);
		}
		struct e3_api_declaration api_template;
		api_template.api_name="l2_add_fib";
		api_template.args_desc[0].type=e3_arg_type_uint8_t;
		api_template.args_desc[0].behavior=e3_arg_behavior_input;
		api_template.args_desc[1].type=e3_arg_type_uint8_t_ptr;
		api_template.args_desc[1].behavior=e3_arg_behavior_input;
		api_template.args_desc[1].len=64;
		api_template.args_desc[2].type=e3_arg_type_none;
		printf("%p\n",e3_api_nr_args(&api_template));
		printf("client:%p\n",allocate_e3_api_client("tcp://localhost:507"));
	#endif
	
	struct e3_api_service * service=allocate_e3_api_service("tcp://*:507");
	printf("service:%p\n",service);
	
	while(1){
		e3_api_service_try_to_poll_request(service);
		e3_api_service_dispatch_apis(service);
		e3_api_service_send_reponse(service);
		sleep(1);
	}
	return 0;
	
}
