
#include <e3-tlv.h>
#include <assert.h>

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


int main()
{
	
	struct message_builder  builder;
	void * value;
	int rc=message_builder_init(&builder,buffer,180);
	

	struct tlv_header tlv;

	tlv.type=MAKE_UINT32(major_type_foo,1);
	tlv.length=12;
	rc=message_builder_add_tlv(&builder,&tlv,"hello world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);

	tlv.type=MAKE_UINT32(major_type_foo,2);
	tlv.length=9;
	rc=message_builder_add_tlv(&builder,&tlv,"m world");
	printf("%d %d\n",rc,builder.msg_hdr_ptr->nr_tlvs);




	struct tlv_major_index_base * base=allocate_tlv_major_index_base_from_glibc();
	assert(base);
	base[major_type_foo].entries=tlv_entires;
	
	rc=message_walk_through_tlv_entries(base,builder.msg_hdr_ptr,builder.msg_hdr_ptr+1,0);
	printf("walk %d\n",rc);
	return 0;
	
}
