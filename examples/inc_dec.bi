memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 0
load ?1 <- 10

increment ?0
aout 1 ?1
vout 1 ?0 <-> ?-1

decrement ?0
aout 1 ?1
vout 1 ?0 <-> ?-1

decrement ?0
aout 1 ?1
vout 1 ?0 <-> ?-1

decrement ?0
aout 1 ?1
vout 1 ?0 <-> ?-1