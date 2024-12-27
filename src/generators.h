#ifndef GENERATORS_H
#define GENERATORS_H

#include <stdio.h>
#include <stdlib.h>

enum generator_type { SIEVING, INDEXING };

typedef struct __generator_arguments {
  const size_t password_length;
  const int passwords_quantity;
  const char *allowed_symbols;
} generator_arguments;

typedef struct __generated_passwords {
  const generator_arguments args;
  size_t buffer_size;
  char *buffer;
} generated_passwords;

generated_passwords *generated_passwords_new(generator_arguments);
void generated_passwords_free(generated_passwords *self);
void generated_passwords_put(generated_passwords *self, int index, const char *value, size_t value_size);
const char *generated_passwords_get(generated_passwords *self, int index);
void generated_passwords_to_string(generated_passwords *self, char *buffer, size_t buffer_size);
size_t generated_passwords_string_size(generated_passwords *self);

typedef struct __generator {
  generated_passwords *(*generate)(struct __generator *self, generator_arguments args);
  FILE *random;
  unsigned char *random_bytes_buffer;
  size_t random_bytes_buffer_size;
  int current_position;
} generator;

generator *generator_new(enum generator_type type, size_t random_bytes_buffer_size);
void generator_free(generator *self);
void generator_update_random_buffer(generator *self);
char generator_next_random_char(generator *self);
unsigned char generator_next_random_byte(generator *self);

#endif /* GENERATORS_H */