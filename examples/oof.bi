memsize 0x8000
reserve ?16 ~ ?32



load ?3 <- 43
load ?4 <- 61

vout 1 ?0 | ?1
add ?0 <- ?1
aout 1 ?3
vout 1 ?1 | ?1
aout 1 ?4
vout 1 ?0 | ?1