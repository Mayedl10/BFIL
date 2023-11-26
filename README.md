# BFIL
Brainfuck intermediate language

Use the compiler with the following options:
- -f      filename
- -o      output filename
- -O      output filename extension
- -p      input file path
- -P      output file path

Example usage: ```BFIL -f compare.bfil -p "path" -P "path" -o "compare"```
<br/><br/><br/>-----<br/><br/><br/>
Numbers that have a ? in front of them are addresses.

To specify the memory size your brainfuck interpreter has, use ```memsize```. This is the only thing in this language that uses hexadecimal values. ```memsize 0x8000```

In order for your code to work, you need to reserve memory using ```reserve```: ```reserve ?16 ~ ?32```*

To load a value into an address, use ```load ?0 ~ 10```*

You can also load multiple values using ```loads ?0 ~ 8 ~ 1 2 3 4 5 6 7 8```*

You can add numbers using add: ```add ?0 ~ ?1```* This means ```?0 += ?1```.

You can do the same with subtraction by using "sub" instead of "add".

Increment an address' value with ```increment ?0```. Use "decrement" to decrement instead.

Ascii output: ```aout amount address address address...```: ```aout 2 ?0 ?1```

Value output: ```vout amount address address address... ~ separating_character_address```: ```vout 2 ?0 ?1 ~ ?2```* Use ?-1 if you don't want a separation character

Constant output: Output that is known at compile time. Outputs one word at max: ```cout Hello```

Console input (one character only): ```read ?0```

Set a range of cells to 0: ```empty ?0 ~ ?10```*

To copy a value, use ```copy ?target ~ ?source```*

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

You can use ```alias``` to create aliases*:
```
alias ?0 ~ x
load x ~ 10
```



<br><br><br><br>
*You can use anything instead of ~. I like to use "+=", "-=", "<-", "->".

If you want to compile this yourself, I am on windows and used ```g++ *.cpp``` to compile. This should work, assuming you're running `g++` in the source directory. You may need to modify main.cpp when not using Windows because of filepath stuff because Windows just likes its `\`s for paths.
