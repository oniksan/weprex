/****************************************************************************

  This file is part of the Webstella Modbus "C" utility library.

  Copyright (C) 2011 - 2018 Oleg Malyavkin.
  Contact: weprexsoft@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/

#include "modbus.h"

uint16_t modbus_error(uint8_t fcode, uint8_t err, uint8_t *out_buf) {
	out_buf[MB_OFFSET_FCODE] = fcode + MB_EX_BASE;
	out_buf[MB_OFFSET_OUT_ERR_CODE] = err;
	return MB_SIZE_ERR_PACKAGE;
}
inline uint16_t modbus_error(uint8_t fcode, uint8_t err, uint8_t *out_buf);


uint16_t modbus_size_check(uint32_t max_regs, const uint8_t *in_buf) {
	uint16_bytes val2byte;
	uint16_t cur_size;
	
	/* Data size in registers */
	val2byte.bytes[0] = in_buf[MB_OFFSET_IN_SIZE_LOW];
	val2byte.bytes[1] = in_buf[MB_OFFSET_IN_SIZE_HIGH];
	/* Calc request size in bytes */
	cur_size = val2byte.uint * MB_SIZE_REGISTER;
	if (cur_size == 0 || cur_size > (max_regs * MB_SIZE_REGISTER)) {
		return 0;
	} else {
		return cur_size;
	}
}
inline uint16_t modbus_size_check(uint32_t max_regs, const uint8_t *in_buf);

struct modbus_server_handle* modbus_server_create(void *regs, uint8_t *access, int32_t bytes_size, uint16_t max_regs_send, uint16_t adr) {
	struct modbus_server_handle *serv;
	if ((serv = (struct modbus_server_handle*) malloc(sizeof(struct modbus_server_handle))) == NULL) {
		return NULL;
	}
	serv->registers = regs;
	serv->access = access;
	serv->bytes_size = bytes_size;
	serv->max_registers_send = max_regs_send;
	serv->device_adr = adr;
	return serv;
}

void modbus_server_destroy(struct modbus_server_handle* serv) {
	free(serv);
}

uint16_t modbus_abstract_server(struct modbus_server_handle *serv, uint8_t *in, uint16_t in_len, uint8_t *out) {
	uint8_t fcode;
	uint16_t len, cur_size;
	int32_t adr;
	int32_t i;
	uint16_bytes val2byte;
	
	len = 0;
	fcode = in[MB_OFFSET_FCODE];
	/* Get address in bytes */
	val2byte.bytes[0] = in[MB_OFFSET_IN_ADR_LOW];
	val2byte.bytes[1] = in[MB_OFFSET_IN_ADR_HIGH];
	adr = val2byte.uint * MB_SIZE_REGISTER;
	if (fcode == MB_FC_READ_HOLDING_REGISTERS || fcode == MB_FC_READ_INPUT_REGISTERS) {
		cur_size = modbus_size_check(serv->max_registers_send, in);
		if (cur_size == 0) {
			len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_VALUE, out);
		} else {
			/* Check address */
			if ((adr + cur_size) <= serv->bytes_size) {
				/* Access check */
				for (i = adr; i < (adr + cur_size); i++) {
					if (serv->access[i] != MB_ACCESS_READ && serv->access[i] != MB_ACCESS_READ_WRITE) {
						len = modbus_error(fcode, MB_EC_ILLEGAL_FUNCTION, out);
						break;
					}
				}
				/* Access allowed */
				if (len == 0) {
					/* Response formation */
					out[MB_OFFSET_FCODE] = fcode;
					out[MB_OFFSET_OUT_SIZE] = (uint8_t) cur_size;
					for (i = 0; i < cur_size; i++) {
						out[i + MB_OFFSET_OUT_START_DATA] = serv->registers[adr + i + ((i % 2 == 0) ? 1 : -1)];
					}
					len = cur_size + MB_OFFSET_OUT_START_DATA;
				}
			} else {
				len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_ADDRESS, out);
			}
		}
	} else if (fcode == MB_FC_WRITE_SINGLE_REGISTER) {
		/* Check address */
		if ((adr + MB_SIZE_REGISTER) <= serv->bytes_size) {
			/* Check access */
			if (serv->access[adr] != MB_ACCESS_WRITE && serv->access[adr] != MB_ACCESS_READ_WRITE) {
				len = modbus_error(fcode, MB_EC_ILLEGAL_FUNCTION, out);
			} else {
				serv->registers[adr] = in[MB_OFFSET_VALUE_LOW];
				serv->registers[adr + 1] = in[MB_OFFSET_VALUE_HIGH];
			}
			/* Response formation */
			memcpy(out, in, MB_SIZE_WRITE_PACKAGE);
			len = MB_SIZE_WRITE_PACKAGE;
		} else {
			len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_ADDRESS, out);
		}
	} else if (fcode == MB_FC_WRITE_MULTIPLE_REGISTERS) {
		cur_size = modbus_size_check(serv->max_registers_send, in);
		if (cur_size == 0) {
			len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_VALUE, out);
		} else {
			if (in_len == (cur_size + MB_OFFSET_IN_MULT_REG_START_DATA) && cur_size == in[MB_OFFSET_IN_BYTE_COUNT]) {
				if ((adr + cur_size) <= serv->bytes_size) {
					/* Access check */
					for (i = adr; i < (adr + cur_size); i++) {
						if (serv->access[i] != MB_ACCESS_WRITE && serv->access[i] != MB_ACCESS_READ_WRITE) {
							len = modbus_error(fcode, MB_EC_ILLEGAL_FUNCTION, out);
							break;
						}
					}
					/* Access allowed */
					if (len == 0) {
						for (i = 0; i < cur_size; i++) {
							serv->registers[adr + i] = in[i + MB_OFFSET_IN_MULT_REG_START_DATA + ((i % 2 == 0) ? 1 : -1)];
						}
						/* Response formation */
						memcpy(out, in, MB_SIZE_WRITE_PACKAGE);
						len = MB_SIZE_WRITE_PACKAGE;
					}
				} else {
					len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_ADDRESS, out);
				}
			} else {
				len = modbus_error(fcode, MB_EC_ILLEGAL_DATA_VALUE, out);
			}
		}
	}
	if (len == 0) {
		len = modbus_error(fcode, MB_EC_ILLEGAL_FUNCTION, out);
	}
	return len;
}

int8_t modbus_tcp_server(struct modbus_server_handle *serv, uint8_t *in_buf, uint16_t in_buf_len, uint8_t *out_buf, uint16_t *out_buf_len) {
	int8_t res;
	uint16_t transaction_id, len;
	
	*out_buf_len = 0;
	/* Package pre-validate */
	res = modbus_tcp_server_mpab_analyze(in_buf, in_buf_len, &transaction_id);
	if (res == DEF_PKG_OK) {
		/* Package parse and processing */
		len = modbus_abstract_server(serv, &in_buf[MB_SIZE_MPAB_HEADER], in_buf_len - MB_SIZE_MPAB_HEADER, &out_buf[MB_SIZE_MPAB_HEADER]);
		/* Construct package */
		modbus_client_tcp_mpab(transaction_id, out_buf, len + 1, in_buf[MB_OFFSET_MPAB_UNIT_ID]);
		*out_buf_len = len + MB_SIZE_MPAB_HEADER;
		return DEF_PKG_OK;
	} else if (res == DEF_PKG_INCOMPLETE) {
		return DEF_PKG_INCOMPLETE;
	}
	return DEF_PKG_INCORRECT;
}

int8_t modbus_rtu_server(struct modbus_server_handle *serv, uint8_t *in_buf, uint16_t in_buf_len, uint8_t *out_buf, uint16_t *out_buf_len) {
	int16_t res;
	uint16_t len;
	uint16_bytes val2byte;
	
	*out_buf_len = 0;
	/* Package pre-validate */
	res = modbus_rtu_server_analyze(in_buf, in_buf_len);
	if ((res > 0 && serv->device_adr == res) || res == 0) {
		/* Package parse and processing */
		len = modbus_abstract_server(serv, &in_buf[MB_SIZE_SERIAL_HEADER], in_buf_len - MB_SIZE_SERIAL_HEADER - MB_SIZE_CRC, &out_buf[MB_SIZE_SERIAL_HEADER]);
		/* Construct package */
		if (res > 0) {
			out_buf[MB_OFFSET_SERIAL_ADR] = (uint8_t) res;
			/* Calc CRC */
			val2byte.uint = modbus_crc16(out_buf, len + 1);
			out_buf[len + 1] = val2byte.bytes[0];
			out_buf[len + 2] = val2byte.bytes[1];
			*out_buf_len = len + 3;
			return DEF_PKG_OK;
		} else {
			return DEF_PKG_BROADCAST_OK;
		}
	} else if (res == MB_RTU_ERR_INCOMPLETE) {
		return DEF_PKG_INCOMPLETE;
	}
	return DEF_PKG_INCORRECT;
}

int8_t modbus_tcp_server_mpab_analyze(const uint8_t *in_buf, uint16_t in_buf_len, uint16_t* transaction_id) {
	uint16_bytes val2byte;
	
	*transaction_id = 0;
	/* Check package min length */
	if (in_buf_len >= MB_SIZE_TCP_MIN_PACKAGE) {
		/* Transaction id */
		val2byte.bytes[0] = in_buf[MB_OFFSET_TRANSACTION_ID_LOW];
		val2byte.bytes[1] = in_buf[MB_OFFSET_TRANSACTION_ID_HIGH];
		*transaction_id = val2byte.uint;
		/* Data length */
		val2byte.bytes[0] = in_buf[MB_OFFSET_LENGTH_LOW];
		val2byte.bytes[1] = in_buf[MB_OFFSET_LENGTH_HIGH];
		if (val2byte.uint == in_buf_len - MB_SIZE_MPAB_HEADER + 1) {
			return DEF_PKG_OK;
		} else if (val2byte.uint < in_buf_len - MB_SIZE_MPAB_HEADER + 1) {
			return DEF_PKG_INCORRECT;
		}
	}
	return DEF_PKG_INCOMPLETE;
}

int16_t modbus_rtu_server_analyze(uint8_t *in_buf, uint16_t in_buf_len) {
	int16_t adr;
	uint16_bytes val2byte;
	int32_t cur_size;
	int32_t calc_size;
	
	/* Check package min length */
	if (in_buf_len >= MB_SIZE_RTU_MIN_PACKAGE) {
		/* Device address */
		adr = in_buf[MB_OFFSET_SERIAL_ADR];
		if (in_buf_len >= MB_SIZE_RTU_MIN_MULTI_WTITE_PACKAGE && (in_buf[MB_OFFSET_FCODE + MB_SIZE_SERIAL_HEADER] == MB_FC_WRITE_MULTIPLE_REGISTERS || in_buf[MB_OFFSET_FCODE + MB_SIZE_SERIAL_HEADER] == MB_FC_WRITE_MULTIPLE_COILS)) {
			/* Registers count */
			val2byte.bytes[0] = in_buf[MB_OFFSET_IN_SIZE_LOW + MB_SIZE_SERIAL_HEADER];
			val2byte.bytes[1] = in_buf[MB_OFFSET_IN_SIZE_HIGH + MB_SIZE_SERIAL_HEADER];
			cur_size = val2byte.uint;
			calc_size = (cur_size * MB_SIZE_REGISTER) + MB_SIZE_RTU_WRITE_MULTI_SERVICE_BLOCK;
			if (in_buf_len < calc_size) {
				return MB_RTU_ERR_INCOMPLETE;
			} else if (in_buf_len > calc_size) {
				return MB_RTU_ERR_OVERFLOW;
			}
		} else if (in_buf_len != MB_SIZE_RTU_READ_PACKAGE) {
			return MB_RTU_ERR_SIZE;
		}
		/* CRC */
		val2byte.bytes[0] = in_buf[in_buf_len - MB_SIZE_CRC];
		val2byte.bytes[1] = in_buf[in_buf_len - MB_SIZE_CRC + 1];
		if (val2byte.uint == modbus_crc16(in_buf, in_buf_len - MB_SIZE_CRC)) {
			return adr;
		} else {
			return MB_RTU_ERR_CRC;
		}
	}
	return MB_RTU_ERR_INCOMPLETE;
}

void modbus_convert_data(uint8_t *in, uint8_t *out, uint16_t len, uint8_t order) {
	uint16_t i;
	if (order == MB_BO_FORWARD) {
		memcpy(out, in, len);
	} else if (order == MB_BO_BACKWARD) {
		for (i = 0; i < len; i++) {
			out[i] = in[len - i - 1];
		}
	} else if (order == MB_BO_FORWARD_REGS_REVERSE) {
		for (i = 0; i < len; i++) {
			out[i] = ((i % 2 == 0)?in[i + 1]:in[i - 1]);
		}
	} else if (order == MB_BO_BACKWARD_REGS_REVERSE) {
		for (i = 0; i < len; i++) {
			out[i] = ((i % 2 == 0)?in[len - i - 2]:in[len - i]);
		}
	}
}

uint8_t modbus_client_calc_param_size(struct modbus_client_parameter* param) {
	if (param->function_code == MB_FC_READ_COILS || param->function_code == MB_FC_READ_DICSRETE_INPUTS || param->function_code == MB_FC_WRITE_MULTIPLE_COILS) {
		param->size = (uint8_t) ((param->func_size / 8) + (((param->func_size % 8) == 0) ? 0 : 1));
	} else if (param->function_code == MB_FC_READ_HOLDING_REGISTERS || param->function_code == MB_FC_READ_INPUT_REGISTERS || param->function_code == MB_FC_WRITE_MULTIPLE_REGISTERS) {
		param->size = (uint8_t) (param->func_size * 2);
	} else if (param->function_code == MB_FC_WRITE_SINGLE_COIL || param->function_code == MB_FC_WRITE_SINGLE_REGISTER) {
		param->size = 2;
	} else {
		return 0;
	}
	if (param->value != NULL) {
		free(param->value);
	}
	if ((param->value = malloc(param->size)) == NULL) {
		return 0;
	}
	memset(param->value, 0, param->size);
	return 1;
}

struct modbus_client_parameter* modbus_client_param_create(uint8_t device_adr, uint8_t fcode, uint16_t reg_adr, uint16_t func_size, uint8_t type) {
	struct modbus_client_parameter *param;
	if ((param = (struct modbus_client_parameter*) malloc(sizeof(struct modbus_client_parameter))) == NULL) {
		return NULL;
	}
	param->device_adr = device_adr;
	param->function_code = fcode;
	param->register_adr = reg_adr;
	param->func_size = func_size;
	param->type = type;
	param->value = NULL;
	if (modbus_client_calc_param_size(param) == 0) {
		free(param);
		return NULL;
	}
	param->err = MB_EC_NO_ERR;
	return param;
}

void modbus_client_param_destroy(struct modbus_client_parameter* param) {
	free(param->value);
	free(param);
}

struct modbus_rtu_client_handle* modbus_client_rtu_create() {
	struct modbus_rtu_client_handle *client;
	if ((client = (struct modbus_rtu_client_handle*) malloc(sizeof(struct modbus_rtu_client_handle))) == NULL) {
		return NULL;
	}
	if ((client->params = utils_vector_create()) == NULL) {
		free(client);
		return NULL;
	}
	
	client->param_counter = 0;
	client->counter = 0;
	return client;
}

void modbus_client_rtu_destroy(struct modbus_rtu_client_handle *client) {
	utils_vect_destroy(client->params);
	free(client);
}

void modbus_client_rtu_reset(struct modbus_rtu_client_handle *client) {
	struct modbus_client_parameter *cur_param;
	client->param_counter = 0;
	client->counter = 0;
	for (uint32_t i = 0; i < utils_vect_size(client->params); i++) {
		cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, i));
		if (cur_param->function_code != MB_FC_WRITE_SINGLE_COIL
			&& cur_param->function_code != MB_FC_WRITE_SINGLE_REGISTER
			&& cur_param->function_code != MB_FC_WRITE_MULTIPLE_COILS
			&& cur_param->function_code != MB_FC_WRITE_MULTIPLE_REGISTERS
		) {
			memset(cur_param->value, 0, cur_param->size);
		}
		cur_param->err = MB_EC_NO_ERR;
	}
}

struct modbus_tcp_client_handle* modbus_client_tcp_create() {
	struct modbus_tcp_client_handle *client;
	if ((client = (struct modbus_tcp_client_handle*) malloc(sizeof(struct modbus_tcp_client_handle))) == NULL) {
		return NULL;
	}
	if ((client->params = utils_vector_create()) == NULL) {
		free(client);
		return NULL;
	}

	client->param_counter = 0;
	client->counter = 0;
	client->transaction_id = 0;
	return client;
}

void modbus_client_tcp_destroy(struct modbus_tcp_client_handle *client) {
	utils_vect_destroy(client->params);
	free(client);
}

void modbus_client_tcp_reset(struct modbus_tcp_client_handle *client) {
	struct modbus_client_parameter *cur_param;
	client->param_counter = 0;
	client->counter = 0;
	client->transaction_id = 0;
	for (uint32_t i = 0; i < utils_vect_size(client->params); i++) {
		cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, i));
		if (cur_param->function_code != MB_FC_WRITE_SINGLE_COIL
			&& cur_param->function_code != MB_FC_WRITE_SINGLE_REGISTER
			&& cur_param->function_code != MB_FC_WRITE_MULTIPLE_COILS
			&& cur_param->function_code != MB_FC_WRITE_MULTIPLE_REGISTERS
		) {
			memset(cur_param->value, 0, cur_param->size);
		}
		cur_param->err = MB_EC_NO_ERR;
	}
}

uint16_t modbus_crc16(const uint8_t *data, uint16_t length) {
	static const uint16_t crc_table[] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

	uint8_t temp;
	uint16_t crc_word = 0xFFFF;

	while (length--) {
		temp = (uint8_t) (*data++ ^ crc_word);
		crc_word >>= 8;
		crc_word ^= crc_table[temp];
	}
	return crc_word;
}

uint16_t modbus_client_request_package(const struct modbus_client_parameter *param, uint8_t type, uint8_t *out_package, uint16_t transaction_id) {
	uint8_t *out_buf;
	uint16_bytes val2byte;
	uint16_t len;
	uint8_t i;
	len = 0;
	if (type == MB_RTU) {
		out_buf = &out_package[MB_SIZE_SERIAL_HEADER];
	} else if (type == MB_TCP) {
		out_buf = &out_package[MB_SIZE_MPAB_HEADER];
	} else {
		return 0;
	}
	/* Function code */
	out_buf[MB_OFFSET_FCODE] = param->function_code;
	/* Initial register  address */
	val2byte.uint = param->register_adr;
	out_buf[MB_OFFSET_IN_ADR_HIGH] = val2byte.bytes[1];
	out_buf[MB_OFFSET_IN_ADR_LOW] = val2byte.bytes[0];
	if (param->function_code == MB_FC_READ_COILS || param->function_code == MB_FC_READ_DICSRETE_INPUTS) {
		/* Colis count */
		val2byte.uint = param->func_size;
		out_buf[MB_OFFSET_IN_SIZE_HIGH] = val2byte.bytes[1];
		out_buf[MB_OFFSET_IN_SIZE_LOW] = val2byte.bytes[0];
		len = MB_SIZE_SIMPLE_REQUEST_PACKAGE;
	} else if (param->function_code == MB_FC_READ_HOLDING_REGISTERS || param->function_code == MB_FC_READ_INPUT_REGISTERS) {
		/* Regs count */
		val2byte.uint = param->func_size;
		out_buf[MB_OFFSET_IN_SIZE_HIGH] = val2byte.bytes[1];
		out_buf[MB_OFFSET_IN_SIZE_LOW] = val2byte.bytes[0];
		len = MB_SIZE_SIMPLE_REQUEST_PACKAGE;
	} else if (param->function_code == MB_FC_WRITE_SINGLE_COIL || param->function_code == MB_FC_WRITE_SINGLE_REGISTER) {
		/* Value */
		out_buf[MB_OFFSET_VALUE_HIGH] = param->value[0];
		out_buf[MB_OFFSET_VALUE_LOW] = param->value[1];
		len = MB_SIZE_SIMPLE_REQUEST_PACKAGE;
	} else if (param->function_code == MB_FC_WRITE_MULTIPLE_COILS || param->function_code == MB_FC_WRITE_MULTIPLE_REGISTERS) {
		/* Coils / regs count */
		val2byte.uint = param->func_size;
		out_buf[MB_OFFSET_IN_SIZE_HIGH] = val2byte.bytes[1];
		out_buf[MB_OFFSET_IN_SIZE_LOW] = val2byte.bytes[0];
		/* Bytes count */
		val2byte.uint = param->size;
		out_buf[MB_OFFSET_IN_BYTE_COUNT] = val2byte.bytes[0];
		/* Value */
		for (i = 0; i < param->size; i++) {
			out_buf[MB_OFFSET_IN_MULT_REG_START_DATA + i] = param->value[i];
		}
		len = MB_OFFSET_IN_MULT_REG_START_DATA + param->size;
	}
	if (len > 0) {
		if (type == MB_RTU) {
			out_package[MB_OFFSET_SERIAL_ADR] = param->device_adr;
			val2byte.uint = modbus_crc16(out_package, len + 1);
			out_package[len + 1] = val2byte.bytes[0];
			out_package[len + 2] = val2byte.bytes[1];
			return len + 3;
		} else if (type == MB_TCP) {
			modbus_client_tcp_mpab(transaction_id, out_package, len + 1, param->device_adr);
			return len + MB_SIZE_MPAB_HEADER;
		}
	}
	return 0;
}

int8_t modbus_client_rtu_check(const uint8_t *in_buf, uint16_t in_buf_len) {
	uint16_t calc_size;
	
	/* Check minimal package length */
	if (in_buf_len >= MB_SIZE_RTU_MIN_PACKAGE) {
		uint8_t fcode = in_buf[MB_SIZE_SERIAL_HEADER + MB_OFFSET_FCODE];
		if (fcode == MB_FC_READ_COILS ||
			fcode == MB_FC_READ_DICSRETE_INPUTS ||
			fcode == MB_FC_READ_HOLDING_REGISTERS ||
			fcode == MB_FC_READ_INPUT_REGISTERS) {
			calc_size = (uint16_t) in_buf[MB_SIZE_SERIAL_HEADER + MB_OFFSET_OUT_SIZE] + MB_SIZE_RTU_OUT_READ_SERVICE_BLOCK;
			/* Bytes count */
			if (in_buf_len == calc_size) {
				return DEF_PKG_OK;
			} else if (in_buf_len > calc_size) {
				return DEF_PKG_INCORRECT;
			}
			
		} else if (fcode == MB_FC_WRITE_SINGLE_COIL ||
				   fcode == MB_FC_WRITE_MULTIPLE_COILS ||
				   fcode == MB_FC_WRITE_SINGLE_REGISTER ||
				   fcode == MB_FC_WRITE_MULTIPLE_REGISTERS) {
			if (in_buf_len == MB_SIZE_RTU_OUT_WRITE_RESPONSE) {
				return DEF_PKG_OK;
			} else if (in_buf_len > MB_SIZE_RTU_OUT_WRITE_RESPONSE) {
				return DEF_PKG_INCORRECT;
			}
		} else if (fcode == (MB_FC_READ_COILS + MB_EX_BASE) ||
					fcode == (MB_FC_READ_DICSRETE_INPUTS + MB_EX_BASE) ||
					fcode == (MB_FC_READ_HOLDING_REGISTERS + MB_EX_BASE) ||
					fcode == (MB_FC_READ_INPUT_REGISTERS + MB_EX_BASE) ||
					fcode == (MB_FC_WRITE_SINGLE_COIL + MB_EX_BASE) ||
					fcode == (MB_FC_WRITE_MULTIPLE_COILS + MB_EX_BASE) ||
					fcode == (MB_FC_WRITE_SINGLE_REGISTER + MB_EX_BASE) ||
					fcode == (MB_FC_WRITE_MULTIPLE_REGISTERS + MB_EX_BASE)) {
			if (in_buf_len == MB_SIZE_RTU_OUT_ERR_RESPONSE) {
				return DEF_PKG_OK;
			} else if (in_buf_len > MB_SIZE_RTU_OUT_ERR_RESPONSE) {
				return DEF_PKG_INCORRECT;
			}
		} else {
			return DEF_PKG_OK;
		}
	}
	return DEF_PKG_INCOMPLETE;
}

uint8_t modbus_client_response_package(struct modbus_client_parameter *param, const uint8_t* package_data, uint16_t package_data_length, uint8_t type) {
	uint16_bytes val2byte;
	uint16_t len;
	const uint8_t *data;
	uint16_t i;
	
	param->err = MB_EC_NO_ERR;
	/* Package integrity check */
	if (package_data_length >= MB_SIZE_MIN_MIN_PACKAGE) {
		if (type == MB_RTU) {
			/* Package min length check */
			if (package_data_length >= MB_SIZE_RTU_MIN_PACKAGE) {
				/* Check CRC */
				val2byte.uint = modbus_crc16(package_data, package_data_length - MB_SIZE_CRC);
				if (package_data[package_data_length - MB_SIZE_CRC] == val2byte.bytes[0] &&
					package_data[package_data_length - MB_SIZE_CRC + 1] == val2byte.bytes[1]) {
					len = package_data_length - MB_SIZE_CRC - MB_SIZE_SERIAL_HEADER;
					data = &package_data[MB_SIZE_SERIAL_HEADER];
					/* Check address */
					if (param->device_adr != package_data[MB_OFFSET_SERIAL_ADR]) {
						return 0;
					}
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		} else if (type == MB_TCP) {
			/* Min package length check */
			if (package_data_length >= MB_SIZE_TCP_MIN_PACKAGE) {
				/* Package length check */
				val2byte.bytes[0] = package_data[MB_OFFSET_LENGTH_LOW];
				val2byte.bytes[1] = package_data[MB_OFFSET_LENGTH_HIGH];
				if (val2byte.uint == (package_data_length - 6)) {
					len = package_data_length - MB_SIZE_MPAB_HEADER;
					data = &package_data[MB_SIZE_MPAB_HEADER];
					/* Check address */
					if (param->device_adr != package_data[MB_OFFSET_MPAB_UNIT_ID]) {
						return 0;
					}
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		} else {
			return 0;
		}
		
		/* Error package */
		if (len == MB_SIZE_OUT_ERR) {
			if (param->function_code != data[MB_OFFSET_FCODE] - MB_EX_BASE) {
				return 0;
			}
			param->err = data[MB_OFFSET_OUT_ERR_CODE];
			return 1;
		} else {
			if (param->function_code != data[MB_OFFSET_FCODE]) {
				return 0;
			}
			if (data[MB_OFFSET_FCODE] == MB_FC_READ_COILS ||
				data[MB_OFFSET_FCODE] == MB_FC_READ_DICSRETE_INPUTS ||
				data[MB_OFFSET_FCODE] == MB_FC_READ_HOLDING_REGISTERS ||
				data[MB_OFFSET_FCODE] == MB_FC_READ_INPUT_REGISTERS) {
				/* Package size check */
				if (data[MB_OFFSET_OUT_SIZE] == (len - MB_OFFSET_OUT_START_DATA)) {
					if (data[MB_OFFSET_OUT_SIZE] > param->size) {
						return 0;
					}
					for (i = 0; i < param->size; i++) {
						param->value[i] = data[i + MB_OFFSET_OUT_START_DATA];
					}
					return 1;
				} else {
					return 0;
				}
			} else if (data[MB_OFFSET_FCODE] == MB_FC_WRITE_SINGLE_COIL ||
					   data[MB_OFFSET_FCODE] == MB_FC_WRITE_SINGLE_REGISTER ||
					   data[MB_OFFSET_FCODE] == MB_FC_WRITE_MULTIPLE_COILS ||
					   data[MB_OFFSET_FCODE] == MB_FC_WRITE_MULTIPLE_REGISTERS) {
				/* Package size check */
				if (len == MB_SIZE_WRITE_PACKAGE) {
					val2byte.bytes[0] = data[MB_OFFSET_IN_ADR_LOW];
					val2byte.bytes[1] = data[MB_OFFSET_IN_ADR_HIGH];
					if (param->register_adr != val2byte.uint) {
						return 0;
					}
					return 1;
				}
			} else {
				return 0;
			}
		}
	}
	return 0;
}

void modbus_client_tcp_mpab(uint16_t transaction_id, uint8_t *package, uint16_t package_len, uint8_t unit_id) {
	uint16_bytes val2byte;
	
	/* Transaction id */
	val2byte.uint = transaction_id;
	package[MB_OFFSET_TRANSACTION_ID_HIGH] = val2byte.bytes[1];
	package[MB_OFFSET_TRANSACTION_ID_LOW] = val2byte.bytes[0];
	/* Protocol id (always 0) */
	package[MB_OFFSET_PROTOCOL_ID_HIGH] = 0;
	package[MB_OFFSET_PROTOCOL_ID_LOW] = 0;
	/* Package length */
	val2byte.uint = package_len;
	package[MB_OFFSET_LENGTH_HIGH] = val2byte.bytes[1];
	package[MB_OFFSET_LENGTH_LOW] = val2byte.bytes[0];
	/* Unit id (device address) */
	package[MB_OFFSET_MPAB_UNIT_ID] = unit_id;
}

void modbus_client_rtu_request(struct modbus_rtu_client_handle *client, uint8_t *out_buf, uint16_t *out_buf_len) {
	struct modbus_client_parameter *cur_param;
	/* Polling counter looping */
	if (client->param_counter == utils_vect_size(client->params)) {
		client->param_counter = 0;
	}
	cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, client->param_counter));
	if (cur_param->type != MB_NO_POLLING) {
		*out_buf_len = modbus_client_request_package(cur_param, MB_RTU, out_buf, 0);
	}
	/* Increment counter */
	client->param_counter++;
}

void modbus_client_tcp_request(struct modbus_tcp_client_handle *client, uint8_t *out_buf, uint16_t *out_buf_len) {
	struct modbus_client_parameter *cur_param;
	/* Polling counter looping */
	if (client->param_counter == utils_vect_size(client->params)) {
		client->param_counter = 0;
	}
	cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, client->param_counter));
	if (cur_param->type != MB_NO_POLLING) {
		*out_buf_len = modbus_client_request_package(cur_param, MB_TCP, out_buf, client->transaction_id);
	}
	/* Increment counter */
	client->param_counter++;
	client->transaction_id++;
}

int8_t modbus_client_rtu_response(struct modbus_rtu_client_handle *client, const uint8_t *in_buf, int16_t in_buf_len) {
	struct modbus_client_parameter* cur_param;
	uint16_t cur_counter;
	int8_t res;
	uint8_t response;
	
	if (in_buf_len > 0) {
		res = modbus_client_rtu_check(in_buf, in_buf_len);
		if (res == DEF_PKG_INCOMPLETE) {
			return res;
		}
	} else if (in_buf_len < 0) {
		res = (int8_t) in_buf_len;
	} else {
		res = DEF_PKG_INCOMPLETE;
		return res;
	}
	cur_counter = client->param_counter - 1;
	cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, cur_counter));
	if (res == DEF_PKG_OK) {
		response = modbus_client_response_package(cur_param, in_buf, in_buf_len, MB_RTU);
	} else {
		response = 0;
	}
	if (response != 0) {
		if (cur_param->type == MB_SET_VALUE) {
			if (cur_param->err == MB_EC_NO_ERR) {
				cur_param->type = MB_NO_POLLING;
			}
		}
		res = DEF_PKG_OK;
	} else {
		res = DEF_PKG_INCORRECT;
		cur_param->err = MB_EC_RESPONSE_ERR;
	}
	if (client->param_counter == utils_vect_size(client->params)) {
		client->counter++;
	}
	return res;
}

int8_t modbus_client_tcp_response(struct modbus_tcp_client_handle *client, const uint8_t *in_buf, int16_t in_buf_len) {
	struct modbus_client_parameter* cur_param;
	uint16_t cur_counter, transaction_id;
	int8_t res;
	uint8_t response;
	
	if (in_buf_len > 0) {
		res = modbus_tcp_server_mpab_analyze(in_buf, in_buf_len, &transaction_id);
		if (res == DEF_PKG_INCOMPLETE) {
			return res;
		}
	} else if (in_buf_len < 0) {
		res = (int8_t) in_buf_len;
		return DEF_PKG_INCORRECT;
	} else {
		res = DEF_PKG_INCOMPLETE;
		return res;
	}
	cur_counter = client->param_counter - 1;
	cur_param = (struct modbus_client_parameter*)(utils_vect_get(client->params, cur_counter));
	if (transaction_id == client->transaction_id - 1) {
		if (res == DEF_PKG_OK) {
			response = modbus_client_response_package(cur_param, in_buf, in_buf_len, MB_TCP);
		} else {
			response = 0;
		}
		if (response != 0) {
			if (cur_param->type == MB_SET_VALUE) {
				if (cur_param->err == MB_EC_NO_ERR) {
					cur_param->type = MB_NO_POLLING;
				}
			}
			res = DEF_PKG_OK;
		} else {
			res = DEF_PKG_INCORRECT;
			cur_param->err = MB_EC_RESPONSE_ERR;
		}
	} else {
		res = DEF_PKG_INCORRECT;
		cur_param->err = MB_EC_RESPONSE_ERR;
	}

	if (client->param_counter == utils_vect_size(client->params)) {
		client->counter++;
	}
	return res;
}

uint32_t modbus_client_rtu_param_add(struct modbus_rtu_client_handle *client, struct modbus_client_parameter *param) {
	return utils_vect_append(client->params, param);
}

void modbus_client_rtu_param_del(struct modbus_rtu_client_handle *client, uint32_t index) {
	utils_vect_remove(client->params, index);
}

struct modbus_client_parameter* modbus_client_rtu_param_get(struct modbus_rtu_client_handle *client, uint32_t index) {
	return utils_vect_get(client->params, index);
}

uint32_t modbus_client_tcp_param_add(struct modbus_tcp_client_handle *client, struct modbus_client_parameter *param) {
	return utils_vect_append(client->params, param);
}

void modbus_client_tcp_param_del(struct modbus_tcp_client_handle *client, uint32_t index) {
	utils_vect_remove(client->params, index);
}

struct modbus_client_parameter* modbus_client_tcp_param_get(struct modbus_tcp_client_handle *client, uint32_t index) {
	return utils_vect_get(client->params, index);
}
