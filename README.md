# thesis-tools
Small little tools that help with my thesis.


## decodeRaw.c
### Make
```
make decodeRaw
```

### Example
```
$ ./decodeRaw 22222222222222222222222222223232000000000000000002020202232313132323333322222323212133331111111111112222232333332323333300000000000000001111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000

aa aa aa aa aa aa aa ee
00 00 00 00 22 22 bb 77
bb ff aa bb 99 ff 55 55
55 aa bb ff bb ff 00 00
00 00 55 55 55 55 55 55
55 55 55 55 55 55 55 55
55 55 55 55 55 55 55 55
55 55 55 55 55 55 55 55
55 55 55 55 00 00 00 00
03


Preamble: aa aa aa aa aa aa aa
SFD: ee
DMAC: 00 00 00 00 22 22
SMAC: bb 77 bb ff aa bb
Length: 99 ff
Data: 55 55 55 aa bb ff bb ff 00 00 00 00 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 00 00 00 00 03
```