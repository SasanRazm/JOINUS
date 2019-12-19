
*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : sasan
*** Cell: SFQ/DC voltage doubler
*** View: schematic
*** July 23 2018
**** **** **** **** **** **** **** ****

*** r50ohmshort50um
.subckt r50ohmshort50um          1          2
***       din      dout
R1                 1         2  50.00ohm
.ends

*** Doubler
.subckt doubler          1          2         3
***       din      dout     bias
Lin                1         4   1.2pH fcheck
R1                 3         4   20ohm
B1                 4         0  jjmod area=1.16
RS1                4         0   2.9ohm *SHUNT=3.73
L1                 4         5   2.517pH fcheck
B2                 5         0  jjmod area=2.4
RS2                5         0   1.73ohm *SHUNT=3.73
B3                 6         5  jjmod area=2.4
RS3                6         5   1.73ohm *SHUNT=3.73
LB1                6         7   1.7pH fcheck
R2                 3         7   6ohm
LB2                7         8   1.14pH fcheck
B4                 9         0  jjmod area=2.4
RS4                9         0   1.73ohm *SHUNT=3.73
L2                 5         9   4.52pH fcheck
B5                 8         9  jjmod area=2.4
RS5                8         9   1.73ohm *SHUNT=3.73
Lout               8         2   2.517pH fcheck
.ends

*** jtl
.subckt jtl          1          2         3
***       din      dout
R1                 3         4   8.34ohm
LPIN               1         5   0.354pH fcheck
LPR1               4         5   0.278pH fcheck
L2                 6         7   4.859pH fcheck
L3                 7         2   2.031pH fcheck
L1                 5         6   2.517pH fcheck
LP2                8         0   0.096pH fcheck
LP1                9         0   0.086pH fcheck
B2                 7         8  jjmod area=2.16
RS2                7         8   1.73ohm *SHUNT=3.73
B1                 6         9  jjmod area=2.16
RS1                6         9   1.73ohm *SHUNT=3.73
.ends

*** sfqdc14
.subckt sfqdc14          1          2         3
***       din      dout
R7                10         0   1.95ohm
R5                 3        12   9.78ohm
R2                 3        14  14.76ohm
R1                 3        16   8.34ohm *FIX
R4                 3        18  17.52ohm
R6                 3        20   5.70ohm
R3                 3         4  14.28ohm
B7                21        22  jjmod area=2.56
RS7               21        22   1.46ohm *SHUNT=3.73
B8                23        24  jjmod area=1.32
RS8               23        24   2.83ohm *SHUNT=3.73
B6                25        26  jjmod area=2.33
RS6               25        26   1.60ohm *SHUNT=3.73
B3                27        28  jjmod area=1.96
RS3               27        28   1.90ohm *SHUNT=3.73
B4                29        30  jjmod area=1.44
RS4               29        30   2.59ohm *SHUNT=3.73
B5                31        22  jjmod area=1.44
RS5               31        22   2.59ohm *SHUNT=3.73
B9                32        33  jjmod area=1.32
RS9               32        33   2.83ohm *SHUNT=3.73
B1                34        35  jjmod area=1.96
RS1               34        35   1.90ohm *SHUNT=3.73
B2                36        37  jjmod area=1.96
RS2               36        37   1.90ohm *SHUNT=3.73
LPR7              38        10   0.387pH fcheck
L15               39         2   7.340pH fcheck
LPR5              12        32   1.240pH fcheck
L12               39        32   0.100pH fcheck
L11               23        39   0.100pH fcheck
LP7               21         0   0.949pH fcheck
LP6               25         0   0.972pH fcheck
L10               22        38   5.681pH fcheck
L17               38        24   0.001pH fcheck
L9                26        24   5.247pH fcheck
L16               30        26   0.614pH fcheck
LP3               28         0   0.234pH fcheck
L5                27        40   2.839pH fcheck
LPR1              16         5   0.242pH fcheck
LPR6              20        41   0.200pH fcheck
L13               33        41   1.872pH fcheck
L1                 5        34   1.895pH fcheck
L14               41         0   0.741pH fcheck
L7                42        29   0.863pH fcheck
LPIN               1         5   0.263pH fcheck
L8                42        31   1.596pH fcheck
L6                40        42   0.263pH fcheck
L3                36        43   1.591pH fcheck
L4                43        27   1.570pH fcheck
L2                34        36   3.471pH fcheck
LPR3               4        40   0.281pH fcheck
LP1               35         0   0.198pH fcheck
LPR4              18        30   0.426pH fcheck
LP2               37         0   0.205pH fcheck
LPR2              14        43   0.416pH fcheck
.ends

*** top cell: dcsfqjsim
XI10       r50ohmshort50um         49          0
*** ("45") mapped to 0
V7                46         0  PULSE(0.0mV  1.034mV   0ps   1.0ps   1.0ps   1.0ps   50.0ps)
V1                47         0  PWL(0ps 0mv 10ps 2.5mv)
XI1           doubler         44         49         47
XI7               jtl         46         48         47
XI6               jtl         48          3         47
XI9           sfqdc14          3         44         47

*** jsim input file ***
.tran 0.1PS 400PS 10PS 0.1PS
.file OUT.DAT
.print nodev 49 0
.print devv XI6_B2
