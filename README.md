# pcc - A Simple C Language Compiler

`pcc` is a simple compiler for the C programming language, written in C++17. Inspired by the [chibicc](https://github.com/rui314/chibicc) project, `pcc` aims to be a minimal yet instructional tool for those learning about compiler construction.

## Workflow

- **Lexical Analysis**: Converts the source code into a stream of tokens for further processing.
- **Syntax Analysis**: Parses the stream of tokens into an Abstract Syntax Tree (AST).
- **Generating IR**: Translates the AST into Static Single Assignment (SSA) IR for further optimization.
- **Basic Optimization Passes**: Runs optimization passes on the IR. Currently, some basic optimization techniques have been implemented.

Note: Register allocation is not fully implemented yet.


## Requirements

- Linux
- CMake 3.18 or later
- Python 3.8
- C++17


## Build

First, clone the project:

```bash
git clone https://github.com/Puppas/pcc.git
```

Then use CMake to build the project:

```bash
cd pcc
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./pcc inputfile
```

The above command will compile `inputfile` and generate output.

## Examples
For a C source function:
```
int f(int init)
{
    int sum = init;
    for (int i = 0; i < 10; i = i + 1) {
        sum = sum + i;
    }
    return sum;
}
```
`pcc` will generate the following ir:
```
define int @f(int %0) {
%1:
  ptr %2 = alloca int
  ptr %3 = alloca int
  ptr %4 = alloca int
  store int %0, ptr %4
  int %5 = load ptr %4
  store int %5, ptr %3
  store int 0, ptr %2
  br label: %6

%6:     preds = %7, %1
  int %8 = load ptr %2
  int %9 = lt int %8, int 10
  br int %9, label: %10 , label: %11

%10:    preds = %6
  int %12 = load ptr %2
  int %13 = load ptr %3
  int %14 = add int %13, int %12
  store int %14, ptr %3
  br label: %7

%7:     preds = %10
  int %15 = load ptr %2
  int %16 = add int %15, int 1
  store int %16, ptr %2
  br label: %6

%11:    preds = %6
  int %17 = load ptr %3
  ret int %17
}
```
After running mem2reg pass:
```
define int @f(int %0) {
%1:
  br label: %2 (int 0, int %0)

%2(int %3, int %4):     preds = %5, %1
  int %6 = lt int %3, int 10
  br int %6, label: %7 , label: %8

%7:     preds = %2
  int %9 = add int %4, int %3
  br label: %5

%5:     preds = %7
  int %10 = add int %3, int 1
  br label: %2 (int %10, int %9)

%8:     preds = %2
  ret int %4
}
```


## Contribution

Contributions are more than welcome! Feel free to submit a Pull Request or open an issue.

## License

`pcc` is licensed under MIT license, see [LICENSE](LICENSE) file for details.

## References

The development of `pcc` benefited from the following projects:

- [chibicc](https://github.com/rui314/chibicc)
- [9cc](https://github.com/rui314/9cc)
- [rvcc](https://github.com/sunshaoce/rvcc)

We thank the authors of these projects for their valuable work.
