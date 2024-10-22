memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 0    # t1 #
load ?1 <- 1    # t2 #
load ?2 <- 0    # next #
load ?3 <- 13   # amount of numbers #
load ?4 <- 0    # temp #
load ?5 <- 32   # space #

decrement ?3    # decrement amt by 2 bc first 2 fib nums are const #
decrement ?3

# print first two numbers and the word "Fibonacci" #
cout Fibonacci:
aout 1 ?5
cout 0
aout 1 ?5
cout 1
aout 1 ?5

# run while n > 0 #
whileNotZero ?3

    # next = t1 + t2 #
    copy ?4 <- ?0
    add ?4 <- ?1
    copy ?2 <- ?4

    vout 1 ?2 ~ ?-1
    aout 1 ?5

    copy ?0 <- ?1
    copy ?1 <- ?2

    decrement ?3

endLoop

