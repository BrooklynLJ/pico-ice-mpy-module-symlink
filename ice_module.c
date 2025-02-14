
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objstr.h"

#include "ice_cram.h"
#include "ice_fpga.h"

static mp_obj_t ice_module_fpga_start(void)
{
	if (ice_fpga_start()) {
		return mp_const_true;
	}
	return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ice_module_fpga_start_obj, ice_module_fpga_start);

static mp_obj_t ice_module_fpga_init(mp_obj_t mhz)
{
	uint8_t arg = (uint8_t)mp_obj_get_int(mhz);

	ice_fpga_init(arg);

	return mp_obj_new_int_from_uint(arg);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ice_module_fpga_init_obj, ice_module_fpga_init);

static mp_obj_t ice_module_cram_write(mp_obj_t file)
{
	mp_obj_t dest[3];
	mp_obj_str_t *data;

	/* seek back to start of file */
	mp_load_method(file, MP_QSTR_seek, dest);
	dest[2] = mp_obj_new_int(0);
	mp_call_method_n_kw(1, 0, dest);

	/* read file data */
	mp_load_method(file, MP_QSTR_read, dest);
	data = MP_OBJ_TO_PTR(mp_call_method_n_kw(0, 0, dest));

	/* send data */
	ice_cram_open();
	ice_cram_write(data->data, data->len);
	ice_cram_close();
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ice_module_cram_write_obj, ice_module_cram_write);

static const mp_rom_map_elem_t ice_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ice) },
	{ MP_ROM_QSTR(MP_QSTR_cram_write), MP_ROM_PTR(&ice_module_cram_write_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fpga_init), MP_ROM_PTR(&ice_module_fpga_init_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fpga_start), MP_ROM_PTR(&ice_module_fpga_start_obj) },
};
static MP_DEFINE_CONST_DICT(ice_module_globals, ice_module_globals_table);

// Define module object.
const mp_obj_module_t ice_module = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t *)&ice_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_ice, ice_module);
