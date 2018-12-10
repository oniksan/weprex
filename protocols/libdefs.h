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

#ifndef WEBSTELLA_LIBDEFS_PACKAGE
#define WEBSTELLA_LIBDEFS_PACKAGE

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

// Incorrect package
#define DEF_PKG_INCORRECT -1
// Incomplete package (continue waiting for data)
#define DEF_PKG_INCOMPLETE 0
// Valid package
#define DEF_PKG_OK 1
// Valid broadcast package (does not require a response)
#define DEF_PKG_BROADCAST_OK 2
// Timeout
#define DEF_PKG_ERR_TIMEOUT -2
// Software buffer overflow
#define DEF_PKG_ERR_SOFT_BUF -3
// Hardware buffer overflow
#define DEF_PKG_ERR_DEV_BUF -4

#if defined (__cplusplus)
extern "C" {
#endif

typedef union {
	uint16_t uint;
	uint8_t bytes[2];
} uint16_bytes;

#define UTILS_VECT_INIT_SIZE 20
#define UTILS_VECT_REALLOC_APPEND_SIZE 10

struct utils_vect_handler {
	void **values;
	uint32_t pointer;
	uint32_t size;
};

struct utils_vect_handler* utils_vector_create();
struct utils_vect_handler* utils_vector_create_sized(uint32_t init_size);
void utils_vect_destroy(struct utils_vect_handler *vector);
void utils_vect_destroy_all(struct utils_vect_handler *vector);
uint32_t utils_vect_realloc(struct utils_vect_handler *vector, uint32_t size);
uint32_t utils_vect_append(struct utils_vect_handler *vector, void *val);
uint32_t utils_vect_size(struct utils_vect_handler *vector);
void* utils_vect_get(struct utils_vect_handler *vector, uint32_t index);
uint32_t utils_vect_remove(struct utils_vect_handler *vector, uint32_t index);
void utils_vector_clear(struct utils_vect_handler *vector);

#if defined (__cplusplus)
}
#endif

#endif
