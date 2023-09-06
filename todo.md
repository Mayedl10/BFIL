done:
- memsize           (importance: 5*++) Required to do anything.
- load              (importance: 5*++)

- reserve           (importance: *****) reserves memory

- whileNotZero      (importance: **** difficulty: 4 (rather difficult)) looping/branching
- add               (importance: **** ) param1 += param2    (both parameters are addresses)
- sub               (importance: **** ) param1 -= param2    (both parameters are addresses)
- aout              (importance: **** difficulty: 1 (easy)) ascii output
- compare           (importance: **** difficulty: 4 (rather difficult)) compares param1 and param3 with operator param2 into param4 (way less complex than it sounds) I FUCKING LOST TRACK OF THE POINTER BECAUSE THE CODE SNIPPET WASN'T CORRECTLY COPIED AAAAA! but I fixed it now OuO
- copy              (importance: **** difficulty: 2 (easy-ish)) copies param2 into param1

- vout              (importance: ***  difficulty: 2 (easy-ish)) numerical output
- increment         (importance: ***  difficulty: 1 (easy)) increments param1
- decrement         (importance: ***  difficulty: 1 (easy)) decrements param1
- read              (importance: ***  difficulty: 1 (easy)) stores console input in param1

- empty             (importance: *    difficulty: 3 (eh, doable)) sets all values ?param1 ~ ?param2 to 0 (inclusive)

- cout: constant output. outputs a string of characters that are not references in memory but CONSTANT. either using string mode, or numerical mode. Only accepts one parameter (a string)
```
cout "This uses string mode. \n will just output \ and n"
```

- logic:
```
logic ?address or ?address -> ?address          # or #
logic ?address and ?address -> ?address         # and #
logic ?address not -> ?address                      # not #
```

- loads: load multiple values
- alias: custom keywords, and stuff (basically #define)

todo:
- optimization
- shell:
    BFIL    command name
    -f      filename
    -o      output filename
    -O      output filename extension
    -p      input file path
    -P      output file path






todo, on the way:
- With every new feature/instruction, add errors that handle every case.
- change that function that finds reserved memory areas to find the NEAREST TO THE POINTER



how to implement these:
- [esolang wiki page for brainfuck algorithms](https://esolangs.org/wiki/Brainfuck_algorithms)


philosophy of the project:
- No values floating around! Everything be stored in an address. Exceptions are for "load". This is the only place where I allow direct values. If you want to add, let's say, 5 to ?0, you either load another cell (eg ?1) with 5 and add it to ?0 OR use "increment" five times.
- No overly complicated abstractions. That is reserved for languages that compile to this (BFIL) as intermediate language.