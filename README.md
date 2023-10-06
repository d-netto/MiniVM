# MiniVM

Bytecode compiler, interpreter and GC for a subset of Java.

BNF (Backus-Naur Form) is available [here](https://www.cambridge.org/resources/052182060X/MCIIJ2e/grammar.htm).

Test files in test were taken from [here]( https://www.cambridge.org/resources/052182060X/).

## Building

To build the project, run:

```
mkdir build
cd build
cmake ..
make -j
```

The executable should be in `build/src/interpreter`. You can also run the full test suite through the `test.sh` script:

```
bash test.sh
````

## Usage

```
Usage: <INTERPRETER_EXECUTABLE> <input file> [--emit-bc]
```

## Example of bytecode generation

This recursive implementation of factorial from the test files:

```java
class Factorial {

    public static void main(String[] a) {
        System.out.println(new Fac().ComputeFac(10));
    }
}

class Fac {

    public int ComputeFac(int num) {
        int num_aux;
        if (num < 1)
            num_aux = 1;
        else
            num_aux = num * (this.ComputeFac(num - 1));
        return num_aux;
    }

}
```

compiles down to:

```
method Factorial.main
        new 1
        ldc 10
        invoke 0 2
        print
        return

method Fac.ComputeFac
  arg  num
  local num_aux
        load 1
        ldc 1
        ilt
        goto_if_false 7
        ldc 1
        store 2
        goto 15
        load 1
        load 0
        load 1
        ldc 1
        isub
        invoke 0 2
        imul
        store 2
        load 2
        return
```

