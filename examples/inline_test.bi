memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 8
load ?1 <- 4
load ?2 <- 6
load ?3 <- 1

inline startedAddingStuff

add ?0 <- ?1
sub ?2 <- ?1

add ?3 <- ?0
add ?3 <- ?1
add ?3 <- ?2
sub ?3 <- ?0

inline done