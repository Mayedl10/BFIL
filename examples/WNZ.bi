memsize 0x8000
reserve ?16 ~ ?32


#
whileNotZero ?address
    [some code]
endLoop
#


load ?0 <- 10
load ?1 <- 10

load ?2 <- 1

whileNotZero ?0
    vout 1 ?0 <-> ?-1
    aout 1 ?1
    sub ?0 -= ?2
endLoop

load ?10 <- 58
load ?11 <- 41

aout 4 ?1 ?10 ?11 ?1