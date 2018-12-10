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

#ifndef WEBSTELLA_MODBUS_PACKAGE
#define WEBSTELLA_MODBUS_PACKAGE

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "libdefs.h"

#include <stdio.h>

/* Modbus RTU */
#define MB_RTU 0
/* Modbus TCP */
#define MB_TCP 1
/* Modbus ASCII */
#define MB_ASCII 2

/* Modbus Function codes */
#define MB_FC_READ_COILS 1
#define MB_FC_READ_DICSRETE_INPUTS 2
#define MB_FC_READ_HOLDING_REGISTERS 3
#define MB_FC_READ_INPUT_REGISTERS 4
#define MB_FC_WRITE_SINGLE_COIL 5
#define MB_FC_WRITE_SINGLE_REGISTER 6
#define MB_FC_WRITE_MULTIPLE_COILS 15
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16

/* Byte order */
#define MB_BO_FORWARD 0
#define MB_BO_BACKWARD 1
#define MB_BO_FORWARD_REGS_REVERSE 2
#define MB_BO_BACKWARD_REGS_REVERSE 3

/* Modbus error/exception codes */
#define MB_EX_BASE 0x80
#define MB_EC_NO_ERR 0x00
#define MB_EC_ILLEGAL_FUNCTION 0x01
#define MB_EC_ILLEGAL_DATA_ADDRESS 0x02
#define MB_EC_ILLEGAL_DATA_VALUE 0x03
#define MB_EC_SLAVE_DEVICE_FAILURE 0x04

#define MB_EC_RESPONSE_ERR 0xFF

/* Polling types */
#define MB_NO_POLLING 0
#define MB_ALWAYS_POLLING 1
#define MB_SET_VALUE 2

/* Access register map flags */
#define MB_ACCESS_DENY 0
#define MB_ACCESS_READ 1
#define MB_ACCESS_WRITE 2
#define MB_ACCESS_READ_WRITE 3

/* Predefined Modbus sizes/offsets */
#define MB_SIZE_REGISTER 2
#define MB_SIZE_MPAB_HEADER 7
#define MB_SIZE_SERIAL_HEADER 1
#define MB_SIZE_ERR_PACKAGE 2
#define MB_SIZE_WRITE_PACKAGE 5
#define MB_SIZE_SIMPLE_REQUEST_PACKAGE 5
#define MB_SIZE_MIN_MIN_PACKAGE 4
#define MB_SIZE_TCP_MIN_PACKAGE 8
#define MB_SIZE_RTU_MIN_PACKAGE 4
#define MB_SIZE_RTU_READ_PACKAGE 8
#define MB_SIZE_RTU_MIN_MULTI_WTITE_PACKAGE 10
#define MB_SIZE_RTU_OUT_WRITE_RESPONSE 8
#define MB_SIZE_RTU_OUT_ERR_RESPONSE 5
#define MB_SIZE_RTU_WRITE_MULTI_SERVICE_BLOCK 9
#define MB_SIZE_RTU_OUT_READ_SERVICE_BLOCK 5
#define MB_SIZE_CRC 2
#define MB_SIZE_OUT_ERR 2
#define MB_OFFSET_SERIAL_ADR 0
#define MB_OFFSET_FCODE 0
#define MB_OFFSET_IN_ADR_HIGH 1
#define MB_OFFSET_IN_ADR_LOW 2
#define MB_OFFSET_IN_SIZE_HIGH 3
#define MB_OFFSET_IN_SIZE_LOW 4
#define MB_OFFSET_IN_BYTE_COUNT 5
#define MB_OFFSET_IN_MULT_REG_START_DATA 6
#define MB_OFFSET_OUT_ERR_CODE 1
#define MB_OFFSET_OUT_SIZE 1
#define MB_OFFSET_OUT_START_DATA 2
#define MB_OFFSET_VALUE_HIGH 3
#define MB_OFFSET_VALUE_LOW 4
#define MB_OFFSET_TRANSACTION_ID_HIGH 0
#define MB_OFFSET_TRANSACTION_ID_LOW 1
#define MB_OFFSET_PROTOCOL_ID_HIGH 2
#define MB_OFFSET_PROTOCOL_ID_LOW 3
#define MB_OFFSET_LENGTH_HIGH 4
#define MB_OFFSET_LENGTH_LOW 5
#define MB_OFFSET_MPAB_UNIT_ID 6

/* RTU analyze error codes */
#define MB_RTU_ERR_INCOMPLETE -1
#define MB_RTU_ERR_SIZE -2
#define MB_RTU_ERR_OVERFLOW -3
#define MB_RTU_ERR_CRC -4

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Modbus client parameter
 */
struct modbus_client_parameter {
	uint8_t device_adr;							/* Device address */
	uint8_t function_code;						/* Function code */
	uint16_t register_adr;						/* Register address */
	uint8_t *value;								/* Pointer to value */
	uint16_t func_size;							/* Value size depends of function code*/
	uint8_t size;								/* Value size in bytes*/
	uint8_t err;								/* Transmit error (0 - no error) */
	uint8_t type;								/* Polling type */
};

/**
 * @brief Modbus server handler
 */
struct modbus_server_handler {
	uint8_t *registers;							/* Registers map */
	uint8_t *access;							/* Access mask for registers map */
	int32_t bytes_size;							/* Size of map in bytes */
	uint16_t max_registers_send;				/* Maximum number of registers/bits requested in one package */
	uint16_t device_adr;						/* Master (server) address */
};

/**
 * @brief Modbus RTU client handler
 */
struct modbus_rtu_client_handler {
	struct utils_vect_handler* params;			/* Parameters vector */
	uint16_t param_counter;						/* Polling counter */
	uint16_t counter;							/* Total counter */
};

/**
 * @brief Modbus TCP client handler
 */
struct modbus_tcp_client_handler {
	struct utils_vect_handler* params;			/* Parameters vector */
	uint16_t transaction_id;					/* Transaction Identifier */
	uint16_t param_counter;						/* Polling counter */
	uint16_t counter;							/* Total counter */
};

/**
 * @brief modbus_server_create Create instance of Modbus server
 * @param regs Registers map
 * @param access Access mask for registers map
 * @param bytes_size Size of map in bytes
 * @param max_regs_send Maximum number of registers/bits requested in one package
 * @param adr Master (server) address
 * @return Modbus server handler
 */
struct modbus_server_handler* modbus_server_create(void *regs, uint8_t *access, int32_t bytes_size, uint16_t max_regs_send, uint16_t adr);

/**
 * @brief modbus_server_destroy Destroy Modbus servers instance
 * @param serv Modbus server handler
 */
void modbus_server_destroy(struct modbus_server_handler* serv);

/**
 * @brief modbus_tcp_server Processing Modbus TCP package and generates responce package
 * @param serv Modbus server handler
 * @param in_buf Request package
 * @param in_buf_len Request package length
 * @param out_buf Response package buffer
 * @param out_buf_len Pointer to response package length
 * @return Package parse status
 */
int8_t modbus_tcp_server(struct modbus_server_handler *serv, uint8_t *in_buf, uint16_t in_buf_len, uint8_t *out_buf, uint16_t *out_buf_len);

/**
 * @brief modbus_rtu_server Processing Modbus RTU package and generates responce package
 * @param serv Modbus server handler
 * @param in_buf Request package
 * @param in_buf_len Request package length
 * @param out_buf Response package buffer
 * @param out_buf_len Pointer to response package length
 * @return Package parse status
 */
int8_t modbus_rtu_server(struct modbus_server_handler *serv, uint8_t *in_buf, uint16_t in_buf_len, uint8_t *out_buf, uint16_t *out_buf_len);

/**
 * @brief Verification MPAB header of Modbus TCP package
 * @param in_buf Request package
 * @param in_buf_len Request package length
 * @param transaction_id Transaction id
 * @return Verification status
 */
inline int8_t modbus_tcp_server_mpab_analyze(const uint8_t *in_buf, uint16_t in_buf_len, uint16_t* transaction_id);

/**
 * @brief Analyze Modbus RTU package
 * @param Request package
 * @param in_buf_len Request package length
 * @return Device address or analyze error (negative number)
 */
int16_t modbus_rtu_server_analyze(uint8_t *in_buf, uint16_t in_buf_len);

/**
 * @brief Converts data according to the specified byte order
 * @param in Data source
 * @param out Data destinaton
 * @param len Size of data
 * @param order Type of converting
 */
void modbus_convert_data(uint8_t *in, uint8_t *out, uint16_t len, uint8_t order);

/**
 * @brief Recalculates and set parameter bytes size depends of func_size, reallocate memory for value
 * @param param Modbus client parameter
 * @return 1 on success, 0 - fail
 */
uint8_t modbus_client_calc_param_size(struct modbus_client_parameter* param);

/**
 * @brief Create Modbus client parameter
 * @param device_adr Master device address
 * @param fcode Functional code
 * @param reg_adr Starting register address
 * @param func_size Size of data
 * @param type Polling type
 * @return Modbus client parameter
 */
struct modbus_client_parameter* modbus_client_param_create(uint8_t device_adr, uint8_t fcode, uint16_t reg_adr, uint16_t func_size, uint8_t type);

/**
 * @brief Destroy Modbus client parameter
 * @param param Modbus client parameter
 */
void modbus_client_param_destroy(struct modbus_client_parameter* param);

/**
 * @brief Create Modbus RTU client instance
 * @return Modbus RTU client handler
 */
struct modbus_rtu_client_handler* modbus_client_rtu_create();

/**
 * @brief Destroy Modbus RTU client instance
 * @param client Modbus RTU client handler
 */
void modbus_client_rtu_destroy(struct modbus_rtu_client_handler *client);

/**
 * @brief Reset polling counters, errors and parameters values to default
 * @param client Modbus RTU client handler
 */
void modbus_client_rtu_reset(struct modbus_rtu_client_handler *client);

/**
 * @brief Create Modbus TCP client instance
 * @return Modbus TCP client handler
 */
struct modbus_tcp_client_handler* modbus_client_tcp_create();

/**
 * @brief Destroy Modbus TCP client instance
 * @param client Modbus TCP client handler
 */
void modbus_client_tcp_destroy(struct modbus_tcp_client_handler *client);

/**
 * @brief Reset polling counters, errors and parameters values to default
 * @param client Modbus TCP client handler
 */
void modbus_client_tcp_reset(struct modbus_tcp_client_handler *client);

/**
 * @brief Calculate Modbus checksum CRC16
 * @param data Data array for checksum calculate
 * @param length Size of data
 * @return CRC16 checksum
 */
uint16_t modbus_crc16(const uint8_t *data, uint16_t length);

/**
 * @brief Initial verification of Modbus RTU request for the completeness of the data
 * @param in_buf Request package
 * @param in_buf_len Package size
 * @return Verification status
 */
inline int8_t modbus_client_rtu_check(const uint8_t *in_buf, uint16_t in_buf_len);

/**
 * @brief Processing Modbus slave (client) request package
 * @param param Modbus client parameter
 * @param type Modbus client type
 * @param out_package Response package buffer
 * @param transaction_id Transaction id (for TCP only)
 * @return Response package size or 0 (error)
 */
uint16_t modbus_client_request_package(const struct modbus_client_parameter *param, uint8_t type, uint8_t *out_package, uint16_t transaction_id);

/**
 * @brief Parse Mobus master (server) response package
 * @param param Modbus client parameter
 * @param package_data Master package
 * @param package_data_length Package size
 * @param type Modbus client type
 * @return 1 for success, 0 - error
 */
uint8_t modbus_client_response_package(struct modbus_client_parameter *param, const uint8_t *package_data, uint16_t package_data_length, uint8_t type);

/**
 * @brief Create Modbus TCP MPAB header
 * @param transaction_id Transaction id
 * @param package Modbus package
 * @param package_len Package size
 * @param unit_id Unit id (device address)
 */
void modbus_client_tcp_mpab(uint16_t transaction_id, uint8_t *package, uint16_t package_len, uint8_t unit_id);

/**
 * @brief Modbus RTU slave (client) request function
 * @param client Modbus RTU client handler
 * @param out_buf Buffer for created package
 * @param out_buf_len Pointer to package size
 */
void modbus_client_rtu_request(struct modbus_rtu_client_handler *client, uint8_t *out_buf, uint16_t *out_buf_len);

/**
 * @brief Modbus TCP slave (client) request function
 * @param client Modbus TCP client handler
 * @param out_buf Buffer for created package
 * @param out_buf_len Pointer to package size
 */
void modbus_client_tcp_request(struct modbus_tcp_client_handler *client, uint8_t *out_buf, uint16_t *out_buf_len);

/**
 * @brief Modbus RTU slave (client) response function
 * @param client Modbus RTU client handler
 * @param in_buf Modbus servers package
 * @param in_buf_len Package size
 * @return Parse status
 */
int8_t modbus_client_rtu_response(struct modbus_rtu_client_handler *client, const uint8_t *in_buf, uint16_t in_buf_len);

/**
 * @brief Modbus TCP slave (client) response function
 * @param client Modbus TCP client handler
 * @param in_buf Modbus servers package
 * @param in_buf_len Package size
 * @return Parse status
 *
 */
int8_t modbus_client_tcp_response(struct modbus_tcp_client_handler *client, const uint8_t *in_buf, uint16_t in_buf_len);

#if defined (__cplusplus)
}
#endif

#endif
