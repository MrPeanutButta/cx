#include "exec.h"
#include "common.h"
#include "types.h"


void cx_executor::cx_malloc(cx_symtab_node* p_target_id,
	cx_type* p_target_type, cx_type* p_expr_type, 
	cx_stack_item* p_target, void* &p_target_address){

	cx_type_code target_type = p_target_type->type_code;
	cx_type_code expr_type = p_expr_type->type_code;

	// array pointer and coords
	void ****p_array = nullptr;
	int x = 0;
	int y = 0;

	mem_block *mem = nullptr;
	int *t_int = &p_target->basic_types.int__;
	char *t_char = &p_target->basic_types.char__;
	wchar_t *t_wchar = &p_target->basic_types.wchar__;
	float *t_float = &p_target->basic_types.float__;
	bool *t_bool = &p_target->basic_types.bool__;
	uint8_t *t_uint8 = &p_target->basic_types.uint8__;

	const int size = p_expr_type->size;
	const int num_of_elements = size / p_expr_type->base_type()->size;

	void *p_source = nullptr;
	
	cx_type *tmp = p_expr_type->array.p_element_type;

	while (tmp->form == fc_array){

		mem = &top()->basic_types;
		p_source = top()->basic_types.addr__;

		if (p_source != nullptr) {
			p_target_address = realloc(p_target_address, size + 1);
			memset(p_target_address, 0, size + 1);

			if (p_target_address == nullptr) {
				perror("realloc");
				cx_runtime_error(rte_none);
			}
		}

		char *tmp = (char *)p_target_address;

		if (p_expr_type->is_scalar_type()) {
			switch (expr_type) {
			case cx_int:
			{
						   memcpy(p_target_address, t_int, sizeof (int));
			}
				break;
			case cx_char:
			{
							memcpy(p_target_address, t_char, sizeof (char));
			}
				break;
			case cx_wchar:
			{
							 memcpy(p_target_address, t_wchar, sizeof (wchar_t));
			}
				break;
			case cx_float:
			{
							 memcpy(p_target_address, t_float, sizeof (float));
			}
				break;
			case cx_bool:
			{
							memcpy(p_target_address, t_bool, sizeof (bool));
			}
				break;
			case cx_uint8:
			{
							 memcpy(p_target_address, t_uint8, sizeof (uint8_t));
			}
				break;

			default:
				break;
			}

		}
		else {
			if (p_source != nullptr) {
				memcpy(p_target_address, p_source, size + 1);
				tmp[size] = '\0';
			}
		}
	}

	p_target_id->p_type->array.element_count = num_of_elements;
	p_target_id->p_type->array.max_index = num_of_elements;
	p_target_id->p_type->size = size;

	if (p_expr_type->is_temp_value) {
		memset(p_source, 0, size + 1);
		remove_type(p_expr_type);
	}

	if (p_target_id->defn.how == dc_function) {
		p_target->basic_types.addr__ = p_target_address;
	}
	else {
		p_target_id->runstack_item->basic_types.addr__ = p_target_address;
	}

}