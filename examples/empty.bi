memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 33
load ?1 <- 9
load ?2 <- 72
load ?3 <- 87
load ?4 <- 100
load ?5 <- 101
load ?6 <- 108
load ?7 <- 111
load ?8 <- 114
load ?9 <- 10

vout 10 ?0 ?1 ?2 ?3 ?4 ?5 ?6 ?7 ?8 ?9 <-> ?1

aout 1 ?9

empty ?3 ~ ?8

vout 10 ?0 ?1 ?2 ?3 ?4 ?5 ?6 ?7 ?8 ?9 <-> ?1