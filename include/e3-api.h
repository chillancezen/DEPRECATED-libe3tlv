#ifndef _E3_API_H
#define _E3_API_H
#include <e3-tlv.h>

#define TLV_MAJOR_E3_API 0x1

enum e3_arg_type{
	#define _(t) e3_arg_type_##t,
	_(none)
	_(uint8_t)
	_(uint16_t)
	_(uint32_t)
	_(uint64_t)
	_(uint8_t_ptr)
	#undef _
};
enum e3_arg_behavior{
	#define _(t) e3_arg_behavior_##t,
	_(none)
	_(input)
	_(output)
	_(input_and_output)
	#undef _
};
struct argument_descriptor{
	enum e3_arg_type type;
	enum e3_arg_behavior behavior;
	int len;
};
#define MAX_ARGUMENT_SUPPORTED 8

struct e3_api_declaration{
	char * api_name;
	char * api_desc;
	uint64_t (*api_callback_func)(uint64_t dummy,...);
	struct e3_api_declaration * next;
	struct argument_descriptor args_desc[MAX_ARGUMENT_SUPPORTED+1];
};
typedef uint64_t (*api_callback_func)(uint64_t dummy,...);

extern struct e3_api_declaration *e3_api_head;

#define DECLARE_E3_API(api) \
	struct e3_api_declaration e3_api_##api; \
	__attribute__((constructor)) \
		void register_e3_api_##api(void){ \
		e3_api_##api.next=e3_api_head; \
		e3_api_head=&e3_api_##api; \
	} \
	struct e3_api_declaration e3_api_##api

struct e3_api_declaration * search_e3_api_by_name(const char * api_name);
struct e3_api_declaration * search_e3_api_by_template(struct e3_api_declaration * api_template);


#endif
