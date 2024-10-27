memsize 0x8000
reserve ?16 ~ ?32

var x = 5
var y = 7
var z

compare x < y -> z

if z
    cout a
endIf


#

memsize 0x8000

reserve ?16 ~ ?32

var x = 5
var y = 7
var z

compare x < ?2 -> ?3

if ?3
    cout a
endIf

#