# BFIL
Brainfuck intermediate language

Use the compiler with the following options:

`-h`          `help`

`-i path`    `input` file path (eg. "/foo/bar/test.bfil")

`-o path`    `output` filename (eg. "/foo/bar/test.bf")

`-k`          `keep` the window open after compilation is finished

`-O n`        `optimise` at optimisation level n (1 removes redundant patterns like +-, 2 does that and removes all characters except +-<>,.[] and \n)

`-c`          `concise` - disable warnings and "file written successfully" text

Example usage: `BFIL -i /path/to/compare.bi -o /path/to/compare.bf`
<br/><br/><br/>-----<br/><br/><br/>
Numbers that have a ? in front of them are addresses.

**WARNING:** `?0` is not automatically reserved, but the compiler sometimes uses it like a reserved address, meaning using it is NOT recommended!

To specify the memory size your brainfuck interpreter has, use `memsize`. This is the only thing in this language that uses hexadecimal values. `memsize 0x8000`

In order for your code to work, you need to reserve memory using `reserve`: `reserve ?16 ~ ?32`*

**WARNING:** `load` and `copy` are legacy instructions. They will continue to be supported for backwards-compatability, but it is recommended to use the `=` operator instead.

The `=` operator:
```
?addr = [constant numerical value]
?addr = ?addr

?1 = 5
?2 = ?1
```

To load a value into an address, use `load ?0 ~ 10`*

You can also load multiple values using `loads ?0 ~ 8 ~ 1 2 3 4 5 6 7 8`*

You can add numbers using add: `add ?0 ~ ?1`* This means `?0 += ?1`.

You can do the same with subtraction by using "sub" instead of "add".

Increment an address' value with `increment ?0`. Use "decrement" to decrement instead.

Ascii output: `aout amount address address address...`: `aout 2 ?0 ?1`

Value output: `vout amount address address address... ~ separating_character_address`: `vout 2 ?0 ?1 ~ ?2`* Use ?-1 if you don't want a separation character

Constant output: Output that is known at compile time. Outputs one word at max: `cout Hello`

Console input (one character only): `read ?0`

Set a range of cells to 0: `empty ?0 ~ ?10`*

To copy a value, use `copy ?target ~ ?source`*

Variables:
- declare with default value as 0: `var [name]`
- declare with specified default value: `var [name] = [value]` (note that `=` can not be replaced by another character)
- Variables are effectively replaced with static memory addresses at compile time, meaning it is not recommended to use both manual addressing and variables! (compiler warnings can be disabled with `-c`)
```
var x
var y = 5

load x <- 10
add x <- y
```

Compare*:
```
compare ?0 = ?1 ~ ?2
compare ?0 < ?1 ~ ?3
compare ?0 > ?1 ~ ?4
```

Logic*:
```
logic ?address or ?address ~ ?address
logic ?address and ?address ~ ?address
logic ?address not ~ ?address
```

Branching:
Only `if` exists, `else` needs to be implemented at the user level. If this feature is used, utilising `?0` is even less advisable since it uses that address internally.
```
if ?address
    [some code]
endIf
```

Looping:
```
whileNotZero ?address
    [some code]
endLoop
```

Comments begin and end with a #. Ergo:
```
This is not a comment
# This is a comment
This is still a comment #
This is not a comment
```

You can use `alias` to create aliases*:
```
alias ?0 ~ x
load x ~ 10
```

The `inline` keyword keeps the next token in the compiler output:
```
...
inline test
...
```
results in `someCode test someCode`



<br><br><br><br>
*You can use anything instead of ~. I like to use "+=", "-=", "<-", "->".
<br>
**In the syntax examples from this `readme`, variables can usually be substituted for "?address", since variables are just replaced by addresses at compile time.

If you want to compile this yourself, I am on windows and used `g++ *.cpp` to compile. This should work, assuming you're running `g++` in the source directory. You may need to modify `main.cpp` when not using Windows because of filepath stuff because Windows just likes its `\`s for paths. `main.cpp` is just responsible for the command line interface so you could easily make your own if you don't like that mess of a command-parameter-thing I coded.

Feel free to open an issue if you have a suggestion or encounter a bug!
