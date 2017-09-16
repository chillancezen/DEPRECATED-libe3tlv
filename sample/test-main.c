
#include <e3-tlv.h>
#include <e3-api.h>
#include <assert.h>
#include <stdio.h>
#include <e3-api-wrapper.h>
#include <pthread.h>
#include <string.h>
e3_type dummy_l2_func(e3_type service,e3_type foo,e3_type bar,e3_type bar1)
{
	uint8_t _foo=e3_type_to_uint8_t(foo);
	uint8_t * _bar=e3_type_to_uint8_t_ptr(bar);
	uint8_t * _bar1=e3_type_to_uint8_t_ptr(bar1);
	*(uint64_t*)_bar1=_foo+1;
	memset(_bar,0x0,64);
	strcpy(_bar,"welcome e3-tlv-rpc");
	sleep(1);
	return 0x2356;
}

DECLARE_E3_API(l2_api)={
	.api_name="l2_add_fib",
	.api_callback_func=(api_callback_func)dummy_l2_func,
	.args_desc={
		{.type=e3_arg_type_uint8_t,.behavior=e3_arg_behavior_input},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_input_and_output,.len=64},
		{.type=e3_arg_type_uint8_t_ptr,.behavior=e3_arg_behavior_output,.len=8},
		{.type=e3_arg_type_none},
	}
};


DECLARE_E3_API(l3_api)={
	.api_name="l3_add_fib",
};

void * foo(void* arg)
{
	struct e3_api_service * service=allocate_e3_api_service((char*)arg);
	while(1){
		if(e3_api_service_try_to_poll_request(service)>0){
			e3_api_service_dispatch_apis(service);
			e3_api_service_send_reponse(service);
		}
	}
	return NULL;
}


int main()
{

	pthread_t pt1,pt2;
	/*two endpoint is exposed*/
	assert(!pthread_create(&pt1,NULL,foo,(void*)"tcp://*:507"));
	assert(!pthread_create(&pt1,NULL,foo,(void*)"tcp://*:508"));

	pthread_join(pt1,NULL);
	pthread_join(pt2,NULL);
	
	return 0;
	
}
