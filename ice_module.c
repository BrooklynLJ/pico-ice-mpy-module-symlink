
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "extmod/modmachine.h"

#include "ice_cram.h"
#include "ice_fpga.h"

/* define FPGA info structure */

typedef struct _ice_module_fpga_obj_t {
	mp_obj_base_t base;
	ice_fpga fpga;
} ice_module_fpga_obj_t;

static mp_obj_t ice_module_fpga_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
	enum { ARG_cdone, ARG_clock, ARG_creset, ARG_cram_cs, ARG_cram_mosi, ARG_cram_sck, ARG_frequency };

	static const mp_arg_t allowed_args[] = {
		{ MP_QSTR_cdone, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_creset, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_cram_cs, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_cram_mosi, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_cram_sck, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
		{ MP_QSTR_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 48} },
	};

	// Parse args.
	mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
	mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	if (mp_obj_get_type(args[ARG_cdone].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CDONE is not a pin"));
	}

	if (mp_obj_get_type(args[ARG_clock].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Clock is not a pin"));
	}

	if (mp_obj_get_type(args[ARG_creset].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CRESET is not a pin"));
	}

	if (mp_obj_get_type(args[ARG_cram_cs].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CRAM CS is not a pin"));
	}

	if (mp_obj_get_type(args[ARG_cram_mosi].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CRAM MOSI is not a pin"));
	}

	if (mp_obj_get_type(args[ARG_cram_sck].u_obj) != &machine_pin_type) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CRAM SCK is not a pin"));
	}


	ice_module_fpga_obj_t *self = mp_obj_malloc(ice_module_fpga_obj_t, type);

	self->fpga.pin_cdone = mp_hal_get_pin_obj(args[ARG_cdone].u_obj);
	self->fpga.pin_clock = mp_hal_get_pin_obj(args[ARG_clock].u_obj);
	self->fpga.pin_creset = mp_hal_get_pin_obj(args[ARG_creset].u_obj);
	self->fpga.bus.MISO = mp_hal_get_pin_obj(args[ARG_cram_mosi].u_obj);
	self->fpga.bus.CS_cram = mp_hal_get_pin_obj(args[ARG_cram_cs].u_obj);
	self->fpga.bus.SCK = mp_hal_get_pin_obj(args[ARG_cram_sck].u_obj);

	ice_fpga_init(self->fpga, args[ARG_frequency].u_int);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t ice_module_fpga_start(mp_obj_t self_in)
{
	ice_module_fpga_obj_t *self = (ice_module_fpga_obj_t *)self_in;

	if (ice_fpga_start(self->fpga) >= 0) {
		return mp_const_true;
	}
	return mp_const_false;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice_module_fpga_start_obj, ice_module_fpga_start);

static mp_obj_t ice_module_fpga_stop(mp_obj_t self_in)
{
	ice_module_fpga_obj_t *self = (ice_module_fpga_obj_t *)self_in;

	if (ice_fpga_stop(self->fpga) >= 0) {
		return mp_const_true;
	}
	return mp_const_false;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice_module_fpga_stop_obj, ice_module_fpga_stop);

static mp_obj_t ice_module_fpga_cram(mp_obj_t self_in, mp_obj_t file)
{
	mp_obj_t dest[3];
	mp_obj_str_t *data;
	ice_module_fpga_obj_t *self = (ice_module_fpga_obj_t *)self_in;

	/* seek back to start of file */
	mp_load_method(file, MP_QSTR_seek, dest);
	dest[2] = mp_obj_new_int(0);
	mp_call_method_n_kw(1, 0, dest);

	/* read file data */
	mp_load_method(file, MP_QSTR_read, dest);
	data = MP_OBJ_TO_PTR(mp_call_method_n_kw(0, 0, dest));

	/* send data */
	if (!ice_cram_open(self->fpga)) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Couldn't open CRAM"));
	}
	ice_cram_write(data->data, data->len);
	ice_cram_close();
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ice_module_fpga_cram_obj, ice_module_fpga_cram);

static const mp_rom_map_elem_t ice_module_fpga_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&ice_module_fpga_start_obj) },
	{ MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&ice_module_fpga_stop_obj) },
	{ MP_ROM_QSTR(MP_QSTR_cram), MP_ROM_PTR(&ice_module_fpga_cram_obj) },
};
static MP_DEFINE_CONST_DICT(ice_module_fpga_locals_dict, ice_module_fpga_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ice_fpga_type,
    MP_QSTR_fpga,
    MP_TYPE_FLAG_NONE,
    make_new, ice_module_fpga_make_new,
    locals_dict, &ice_module_fpga_locals_dict
    );

/*static mp_obj_t ice_module_fpga_start(void)
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
	mp_obj_str_t *data;*/

	/* seek back to start of file */
	/*mp_load_method(file, MP_QSTR_seek, dest);
	dest[2] = mp_obj_new_int(0);
	mp_call_method_n_kw(1, 0, dest);*/

	/* read file data */
	/*mp_load_method(file, MP_QSTR_read, dest);
	data = MP_OBJ_TO_PTR(mp_call_method_n_kw(0, 0, dest));*/

	/* send data */
	/*ice_cram_open();
	ice_cram_write(data->data, data->len);
	ice_cram_close();
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ice_module_cram_write_obj, ice_module_cram_write);*/

static const mp_rom_map_elem_t ice_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_fpga), MP_ROM_PTR(&ice_fpga_type) },
/*	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ice) },
	{ MP_ROM_QSTR(MP_QSTR_cram_write), MP_ROM_PTR(&ice_module_cram_write_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fpga_init), MP_ROM_PTR(&ice_module_fpga_init_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fpga_start), MP_ROM_PTR(&ice_module_fpga_start_obj) },*/
};
static MP_DEFINE_CONST_DICT(ice_module_globals, ice_module_globals_table);

// Define module object.
const mp_obj_module_t ice_module = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t *)&ice_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_ice, ice_module);
