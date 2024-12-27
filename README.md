[![Licence](https://img.shields.io/badge/Licence-MIT-red.svg)](LICENCE)
[![language-c](https://img.shields.io/badge/Language-C_11-3949ab.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

# pwdgen

This is a simple, lightweight password generator designed for Linux-based operating systems. While it lacks rich features, it meets my personal needs. Written in C for fun, this tool is uploaded here to ensure it is not lost. The only dependencies are `getopt.h` and the ability to read from `/dev/urandom`.

As a command-line tool, it allows you to generate random passwords with customizable options. By default, the generator includes a mix of lowercase letters, uppercase letters, digits, and special symbols. You can specify both the length of the passwords and the quantity you wish to generate.

## Building

To build the Password Generator, simply run the following command in the project directory:

```bash
$ make clean release
```

The compiled binary will be located in the `build` directory.

## Usage

`pwdgen [OPTIONS] [PASSWORD_LENGTH] [QUANTITY]`

### Parameters

- `PASSWORD_LENGTH`: The length of each generated password. The default value is `10`.
- `QUANTITY`: The number of passwords to generate. The default value is `1`.

### Options

- `-l`, `--lowers`: Include lowercase characters in the generated passwords.
- `-u`, `--uppers`: Include uppercase characters in the generated passwords.
- `-d`, `--digits`: Include digits in the generated passwords.
- `-s`, `--symbols`: Include special symbols in the generated passwords.
- `-h`, `--help`: Print help message and exit.

## Examples

1. Generate a single password of default length (10 characters):
   ```bash
   $ pwdgen
   ```

2. Generate 5 passwords of length 12, including only lowercase letters:
   ```bash
   $ pwdgen -l 12 5
   ```

3. Generate 3 passwords of length 15, including uppercase letters, digits, and symbols:
   ```bash
   $ pwdgen -uds 15 3
   ```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENCE) file for more details.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any suggestions or improvements.

## Author

[red-bashmak](https://github.com/red-bashmak)