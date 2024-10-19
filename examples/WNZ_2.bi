memsize 0x8000
reserve ?32 ~ ?64

load ?0 <- 10           # x counter #
load ?10 <- 10          # y counter #

load ?20 <- 32          # space #
load ?21 <- 10          # newline #

load ?30 <- 1           # 1 #

whileNotZero ?0
    sub ?0 -= ?30

    whileNotZero ?10
        vout 1 ?10 | ?-1
        aout 1 ?20
        sub ?10 -= ?30
    endLoop
    
    aout 1 ?21
    copy ?10 <- ?0
endLoop