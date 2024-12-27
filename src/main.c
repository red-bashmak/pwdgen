#include "bits.h"
#include "generators.h"
#include <errno.h>
#include <getopt.h>
#include <string.h>

#define DEFAULT_RANDOM_BUFF_SIZE 512

const char *pwd_charsets[] = {"abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "0123456789",
                              "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"};
// const char *pwd_ambiguous = "B8G6I1l0OQDS5Z2";
// const char *pwd_vowels = "01aeiouyAEIOUY";

enum charset_names { LOWERS = 0, UPPERS, DIGITS, SYMBOLS };

void print_usage() {
  printf("Usage: pwdgen [OPTIONS] [PASSWORD_LENGTH] [QUANTITY]\n"
         "    Generate QUANTITY of random passwords with length PASSWORD_LENGTH and print it line by line to stdout\n"
         "    in passwords by default included all chars lowers+uppers+digits+symbols\n\n"
         "    PASSWORD_LENGTH default value is 10\n"
         "    QUANTITY default value is 1\n\n"
         "OPTIONS:\n"
         "    -l, --lowers    include lowercase chars to generated passwords\n"
         "    -u, --uppers    include uppercase chars to generated passwords\n"
         "    -d, --digits    include digits to generated passwords\n"
         "    -s, --symbols   include special symbols to generated passwords\n"
         "    -h, --help      print this message and exit\n");
}

int main(int argc, char **argv) {
  int opt;
  size_t password_length = 10;
  int passwords_quantity = 1;
  unsigned char charset_flags = 0b0000;

  // programm option definitions
  struct option programm_options[] = {{"lowers", no_argument, NULL, 'l'}, {"uppers", no_argument, NULL, 'u'},
                                      {"digits", no_argument, NULL, 'd'}, {"symbols", no_argument, NULL, 's'},
                                      {"help", no_argument, NULL, 'h'},   {NULL, 0, NULL, 0}};

  while ((opt = getopt_long(argc, argv, "ludsh", programm_options, NULL)) != -1) {
    switch (opt) {
    case 'l':
      SET_BIT(charset_flags, LOWERS);
      break;

    case 'u':
      SET_BIT(charset_flags, UPPERS);
      break;

    case 'd':
      SET_BIT(charset_flags, DIGITS);
      break;

    case 's':
      SET_BIT(charset_flags, SYMBOLS);
      break;

    default:
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (!charset_flags) {
    charset_flags = 0b1111;
  }

  if (optind < argc) {
    int password_password_length_input = atoi(argv[optind]);
    if (password_password_length_input <= 0) {
      errno = EINVAL;
      perror("main");
      fprintf(stderr, "incorrect password length = %s\n", argv[optind]);
      exit(EXIT_FAILURE);
    }
    password_length = password_password_length_input;
  }

  if (optind + 1 < argc) {
    int passwords_quantity_input = atoi(argv[optind + 1]);
    if (passwords_quantity_input <= 0) {
      errno = EINVAL;
      perror("main");
      fprintf(stderr, "incorrect passwords quantity = %s\n", argv[optind + 1]);
      exit(EXIT_FAILURE);
    }
    passwords_quantity = passwords_quantity_input;
  }

  size_t allowed_symbols_size = 0;
  for (enum charset_names cs = LOWERS; cs <= SYMBOLS; cs++) {
    if (IS_SET(charset_flags, cs)) {
      allowed_symbols_size += strlen(pwd_charsets[cs]);
    }
  }
  allowed_symbols_size++;

  char allowed_symbols[allowed_symbols_size];
  memset(allowed_symbols, 0, sizeof(allowed_symbols));
  for (enum charset_names cs = LOWERS; cs <= SYMBOLS; cs++) {
    if (IS_SET(charset_flags, cs)) {
      if (*(allowed_symbols)) {
        strcat(allowed_symbols, pwd_charsets[cs]);
      } else {
        strcpy(allowed_symbols, pwd_charsets[cs]);
      }
    }
  }

  generator *gen = generator_new(SIEVING, DEFAULT_RANDOM_BUFF_SIZE);
  generator_arguments args = {
      .password_length = password_length, .passwords_quantity = passwords_quantity, .allowed_symbols = allowed_symbols};

  generated_passwords *passwords = gen->generate(gen, args);
  size_t __size = generated_passwords_string_size(passwords);
  char result[__size];
  generated_passwords_to_string(passwords, result, __size);

  printf("%s", result);
  generated_passwords_free(passwords);
  generator_free(gen);

  exit(EXIT_SUCCESS);
}
