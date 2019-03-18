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

#include "libdefs.h"

struct utils_vect_handle* utils_vector_create_sized(uint32_t init_size) {
	struct utils_vect_handle *v;
	if ((v = (struct utils_vect_handle*) malloc(sizeof(struct utils_vect_handle))) == NULL) {
		return NULL;
	}
	if ((v->values = malloc(sizeof(void*) * init_size)) == NULL) {
		free(v);
		return NULL;
	}
	v->size = init_size;
	v->pointer = 0;
	return v;
}

struct utils_vect_handle* utils_vector_create() {
	return utils_vector_create_sized(UTILS_VECT_INIT_SIZE);
}

void utils_vect_destroy(struct utils_vect_handle *vector) {
	free(vector->values);
	free(vector);
}

void utils_vect_destroy_all(struct utils_vect_handle *vector) {
	for (uint32_t i = 0; i < vector->pointer - 1; i++) {
		free(vector->values[i]);
	}
	utils_vect_destroy(vector);
}

uint32_t utils_vect_realloc(struct utils_vect_handle *vector, uint32_t size) {
	void **new_values;
	if (size > vector->size) {
		if ((new_values = malloc(sizeof(void*) * size)) == NULL) {
			return 0;
		}
		memcpy(new_values, vector->values, vector->size);
		free(vector->values);
		vector->values = new_values;
		vector->size = size;
		return size;
	}
	return 0;
}

uint32_t utils_vect_append(struct utils_vect_handle *vector, void *val) {
	if (vector->pointer == (vector->size - 1)) {
		if (utils_vect_realloc(vector, UTILS_VECT_REALLOC_APPEND_SIZE) == 0) {
			return 0;
		}
	}
	vector->values[vector->pointer] = val;
	vector->pointer++;
	return vector->pointer;
}

uint32_t utils_vect_size(struct utils_vect_handle *vector) {
	return vector->pointer;
}

void* utils_vect_get(struct utils_vect_handle *vector, uint32_t index) {
	if (index < vector->pointer) {
		return vector->values[index];
	}
	return NULL;
}

uint32_t utils_vect_remove(struct utils_vect_handle *vector, uint32_t index) {
	if (index < vector->pointer) {
		for (uint32_t i = index; i < vector->pointer - 1; i++) {
			vector->values[i] = vector->values[i + 1];
		}
		vector->pointer--;
		return vector->pointer;
	}
	return 0;
}

void utils_vector_clear(struct utils_vect_handle *vector) {
	vector->pointer = 0;
}

