memsize 0x8000
reserve ?16 ~ ?32

load ?0 <- 33    #!            #
load ?1 <- 32    #SPACE        #
load ?2 <- 72    #H            #
load ?3 <- 87    #W            #
load ?4 <- 100   #d            #
load ?5 <- 101   #e            #
load ?6 <- 108   #l            #
load ?7 <- 111   #o            #
load ?8 <- 114   #r            #
load ?9 <- 10    #newline      #

# ascii out (outputs ascii character)
     13  H  e  l  l  o     W  o  r  l  d  ! \n      #
aout 13 ?2 ?5 ?6 ?6 ?7 ?1 ?3 ?7 ?8 ?6 ?4 ?0 ?9
#       72 101 108 108 111 32 87 111 114 108 100 33 10 #
vout 13 ?2 ?5  ?6  ?6  ?7  ?1 ?3 ?7  ?8  ?6  ?4 ?0 ?9 <-> ?1