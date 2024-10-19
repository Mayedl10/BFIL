memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 69
load ?1 <- 42

copy ?2 <- ?0
copy ?0 <- ?100
copy ?100 <- ?1

load ?3 <- 32 #space#

vout 4 ?0 ?1 ?2 ?100 <-> ?3