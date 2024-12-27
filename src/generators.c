#include "generators.h"
#include <string.h>

generated_passwords *generated_passwords_new(const generator_arguments args) {
  generated_passwords init = {.args = args};
  generated_passwords *result = malloc(sizeof(*result));
  if (result == NULL) {
    perror("generated_passwords_new()");
    exit(EXIT_FAILURE);
  }
  memcpy(result, &init, sizeof(*result));
  result->buffer_size = result->args.password_length * result->args.passwords_quantity;
  result->buffer = malloc(sizeof(*result->buffer) * result->buffer_size);
  memset(result->buffer, 0, result->buffer_size);
  if (result->buffer == NULL) {
    free(result);
    perror("generated_passwords_new()");
    exit(EXIT_FAILURE);
  }
  return result;
}

void generated_passwords_free(generated_passwords *self) {
  // clear memory before free
  memset(self->buffer, 0, self->buffer_size);
  free(self->buffer);
  self->buffer_size = 0;
  free(self);
}

void generated_passwords_put(generated_passwords *self, int index, const char *value, size_t value_size) {
  char error_msg[128];
  if (index > self->args.passwords_quantity) {
    memset(error_msg, 0, 128);
    sprintf(error_msg, "index %d too big, max passwords quantity is %d", index, self->args.passwords_quantity);
    perror(error_msg);
    return;
  }
  if (value_size > self->args.password_length) {
    memset(error_msg, 0, 128);
    sprintf(error_msg, "password size %ld too big, max size is %ld", value_size, self->args.password_length);
    perror(error_msg);
    return;
  }
  int offset = self->args.password_length * index;
  char *buff_ptr = self->buffer + offset;
  memset(buff_ptr, 0, self->args.password_length);
  strncpy(buff_ptr, value, value_size);
}

const char *generated_passwords_get(generated_passwords *self, int index) {
  char error_msg[128];
  if (index > self->args.passwords_quantity) {
    memset(error_msg, 0, 128);
    sprintf(error_msg, "index %d too big, max passwords quantity is %d", index, self->args.passwords_quantity);
    perror(error_msg);
    return NULL;
  }
  int offset = self->args.password_length * index;
  const char *buff_ptr = self->buffer + offset;
  return buff_ptr;
}

void generated_passwords_to_string(generated_passwords *self, char *buffer, size_t buffer_size) {
  memset(buffer, 0, buffer_size);
  char *ptr = buffer;
  for (int i = 0; i < self->args.passwords_quantity; ++i) {
    const char *pwi = generated_passwords_get(self, i);
    strncpy(ptr, pwi, self->args.password_length);
    ptr = ptr + self->args.password_length;
    *ptr = '\n';
    ptr++;
  }
}

size_t generated_passwords_string_size(generated_passwords *self) {
  return self->buffer_size + self->args.passwords_quantity + 1;
}

static FILE *get_random_fd() {
  FILE *fd;

  fd = fopen("/dev/urandom", "rb");
  if (fd == NULL) {
    perror("open /dev/urandom");
    exit(EXIT_FAILURE);
  }
  return fd;
}

void generator_free(generator *self) {
  // clear memory before free
  memset(self->random_bytes_buffer, 0, self->random_bytes_buffer_size);
  free(self->random_bytes_buffer);
  self->random_bytes_buffer_size = 0;
  fclose(self->random);
  free(self);
}

void generator_update_random_buffer(generator *self) {
  size_t readed_bytes =
      fread(self->random_bytes_buffer, sizeof(unsigned char), self->random_bytes_buffer_size, self->random);
  if (readed_bytes < self->random_bytes_buffer_size) {
    perror("fread");
    fprintf(stderr, "something wrong with random reading");
    generator_free(self);
    exit(EXIT_FAILURE);
  }
}

static void sieve_one_password(generator *gen, generator_arguments args, char *buff) {
  char ch;
  int i = 0;
  while (i < args.password_length) {
    ch = generator_next_random_char(gen);
    if (ch == '\0') {
      continue;
    }
    if (strchr(args.allowed_symbols, ch) != NULL) {
      buff[i] = ch;
      i++;
    }
  }
}

static generated_passwords *sieving_generate(generator *self, generator_arguments args) {
  generated_passwords *result = generated_passwords_new(args);
  char tmp[args.password_length];
  for (int i = 0; i < args.passwords_quantity; i++) {
    sieve_one_password(self, args, tmp);
    generated_passwords_put(result, i, tmp, args.password_length);
  }
  memset(tmp, 0, args.password_length);
  return result;
}

// static int normalize(unsigned char ch, size_t len) { return (ch * len) / 255; }

static int normalize_optimized(unsigned char ch, size_t len) { return (ch * len) >> 8; }

// static int normalize_mod(unsigned char ch, size_t len) { return ch % len; }

static void get_password(generator *gen, generator_arguments args, char *buff) {
  size_t allowed_symbols_size = strlen(args.allowed_symbols);
  for (int i = 0; i < args.password_length; i++) {
    buff[i] = args.allowed_symbols[normalize_optimized(generator_next_random_byte(gen), allowed_symbols_size)];
  }
}

static generated_passwords *indexing_generate(generator *self, generator_arguments args) {
  generated_passwords *result = generated_passwords_new(args);
  char tmp[args.password_length];
  for (int i = 0; i < args.passwords_quantity; i++) {
    get_password(self, args, tmp);
    generated_passwords_put(result, i, tmp, args.password_length);
  }
  memset(tmp, 0, args.password_length);
  return result;
}

generator *generator_new(enum generator_type type, size_t random_bytes_buffer_size) {
  generator *result = malloc(sizeof(*result));
  if (result == NULL) {
    perror("generator_new(generator_type)");
    exit(EXIT_FAILURE);
  }

  result->random = get_random_fd();
  result->current_position = 0;
  result->random_bytes_buffer_size = random_bytes_buffer_size;
  result->random_bytes_buffer = malloc(sizeof(*result->random_bytes_buffer) * result->random_bytes_buffer_size);
  if (result->random_bytes_buffer == NULL) {
    perror("generator_new(generator_type)");
    exit(EXIT_FAILURE);
  }
  generator_update_random_buffer(result);

  switch (type) {
  case SIEVING:
    result->generate = sieving_generate;
    break;
  case INDEXING:
    result->generate = indexing_generate;
    break;
  default:
    perror("generator_new(generator_type): unknown generator type");
    generator_free(result);
    exit(EXIT_FAILURE);
  }
  return result;
}

char generator_next_random_char(generator *self) {
  if (self->current_position >= self->random_bytes_buffer_size - 1) {
    generator_update_random_buffer(self);
    self->current_position = 0;
  }
  return (char)self->random_bytes_buffer[++self->current_position];
}

unsigned char generator_next_random_byte(generator *self) {
  if (self->current_position >= self->random_bytes_buffer_size - 1) {
    generator_update_random_buffer(self);
    self->current_position = 0;
  }
  return self->random_bytes_buffer[++self->current_position];
}