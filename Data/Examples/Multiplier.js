**** **** **** **** **** **** **** **** **** **** **** 
*JSIM control file for CADENCE by kameda@cq.jp.nec.com
**** **** **** **** **** **** **** **** **** **** ****

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : OPEN572CHIP10
*** Cell: SQUIDMULtest
*** View: schematic
*** Nov 21 20:35:32 2018
**** **** **** **** **** **** **** ****

*** SQUIDMUL
.subckt SQUIDMUL          1          2          3          4
***      Ibin     Ibout        d1        d2
B3                 5         2  jjmod area=1.20
RS3                5         2   3.50ohm *SHUNT=3.73
B2                 6         2  jjmod area=1.20
RS2                6         2   3.50ohm *SHUNT=3.73
B1                 7         8  jjmod area=1.20
RS1                7         8   3.40ohm *SHUNT=3.73
B0                 9         8  jjmod area=1.20
RS0                9         8   3.50ohm *SHUNT=3.73
L7                10        33   15.000pH fcheck
RSH1              33         0   1ohm
L6                 8         5   13.700pH fcheck
L5                 4        10   15.00pH fcheck
L4                 6         8   13.700pH fcheck
L3                11        34   15.00pH fcheck
RSH2              34         0   1ohm
L2                 3        11   15.00pH fcheck
L1                 1         7   13.700pH fcheck
L0                 9         1   13.700pH fcheck
K3                 L6         L7 0.400
K2                 L4         L5 0.400
K1                 L1         L3 0.400
K0                 L0         L2 0.400
.ends
*** jtl
.subckt jtl         12         13        14
***       din      dout
R1                14        15   8.34ohm
LPIN              12        16   0.354pH fcheck
LPR1              15        16   0.278pH fcheck
L2                17        18   4.859pH fcheck
L3                18        13   2.031pH fcheck
L1                16        17   2.517pH fcheck
LP2               19         0   0.096pH fcheck
LP1               20         0   0.086pH fcheck
B2                18        19  jjmod area=2.16
RS2               18        19   1.73ohm *SHUNT=3.73
B1                17        20  jjmod area=2.16
RS1               17        20   1.73ohm *SHUNT=3.73
.ends

*** sfqdc14
.subckt sfqdc14         12         13        14
***       din      dout
R7                21         0   1.95ohm
R5                14        23   9.78ohm
R2                14        25  14.76ohm
R1                14        27   8.34ohm *FIX
R4                14        29  17.52ohm
R6                14        31   5.70ohm
R3                14        15  14.28ohm
B7                32        33  jjmod area=2.56
RS7               32        33   1.46ohm *SHUNT=3.73
B8                34        35  jjmod area=1.32
RS8               34        35   2.83ohm *SHUNT=3.73
B6                36        37  jjmod area=2.33
RS6               36        37   1.60ohm *SHUNT=3.73
B3                38        39  jjmod area=1.96
RS3               38        39   1.90ohm *SHUNT=3.73
B4                40        41  jjmod area=1.44
RS4               40        41   2.59ohm *SHUNT=3.73
B5                42        33  jjmod area=1.44
RS5               42        33   2.59ohm *SHUNT=3.73
B9                43        44  jjmod area=1.32
RS9               43        44   2.83ohm *SHUNT=3.73
B1                45        46  jjmod area=1.96
RS1               45        46   1.90ohm *SHUNT=3.73
B2                47        48  jjmod area=1.96
RS2               47        48   1.90ohm *SHUNT=3.73
LPR7              49        21   0.387pH fcheck
L15               50        13   7.340pH fcheck
LPR5              23        43   1.240pH fcheck
L12               50        43   0.100pH fcheck
L11               34        50   0.100pH fcheck
LP7               32         0   0.949pH fcheck
LP6               36         0   0.972pH fcheck
L10               33        49   5.681pH fcheck
L17               49        35   0.001pH fcheck
L9                37        35   5.247pH fcheck
L16               41        37   0.614pH fcheck
LP3               39         0   0.234pH fcheck
L5                38        51   2.839pH fcheck
LPR1              27        16   0.242pH fcheck
LPR6              31        52   0.200pH fcheck
L13               44        52   1.872pH fcheck
L1                16        45   1.895pH fcheck
L14               52         0   0.741pH fcheck
L7                53        40   0.863pH fcheck
LPIN              12        16   0.263pH fcheck
L8                53        42   1.596pH fcheck
L6                51        53   0.263pH fcheck
L3                47        54   1.591pH fcheck
L4                54        38   1.570pH fcheck
L2                45        47   3.471pH fcheck
LPR3              15        51   0.281pH fcheck
LP1               46         0   0.198pH fcheck
LPR4              29        41   0.426pH fcheck
LP2               48         0   0.205pH fcheck
LPR2              25        54   0.416pH fcheck
.ends

*** spll
.subckt spll         55         56         57        66
***         a         b         c
B2                58        59  jjmod area=1.94
RS2               58        59   1.92ohm *SHUNT=3.73
B3                60         5  jjmod area=1.94
RS3               60         5   1.92ohm *SHUNT=3.73
B1                61        62  jjmod area=2.15
RS1               61        62   1.74ohm *SHUNT=3.73
LP2               59         0   0.263pH fcheck
L2                63        58   4.987pH fcheck
L3                63        60   4.932pH fcheck
LPA               55        64   0.348pH fcheck
LPR1              65        64   0.307pH fcheck
L6                61        63   0.468pH fcheck
L5                60        57   1.680pH fcheck
L1                64        61   2.366pH fcheck
LP3                5         0   0.325pH fcheck
LP1               62         0   0.133pH fcheck
L4                58        56   1.719pH fcheck
R1                66        65   8.35ohm *FIX
.ends

*** jtlc3
.subckt jtlc3         12         13        14
***       din      dout
R1                14        15   8.34ohm
LPIN              12        16   0.172pH fcheck
LPR1              15        16   0.252pH fcheck
L2                17        18   4.753pH fcheck
L3                18        13   1.810pH fcheck
L1                16        17   1.973pH fcheck
LP2               19         0   0.273pH fcheck
LP1               20         0   0.403pH fcheck
B2                18        19  jjmod area=2.16
RS2               18        19   1.73ohm *SHUNT=3.73
B1                17        20  jjmod area=2.16
RS1               17        20   1.73ohm *SHUNT=3.73
.ends

*** splt
.subckt splt         55         56         57        66
***         a         b         c
B2                58        59  jjmod area=1.94
RS2               58        59   1.92ohm *SHUNT=3.73
B3                60         5  jjmod area=1.94
RS3               60         5   1.92ohm *SHUNT=3.73
B1                61        62  jjmod area=2.15
RS1               61        62   1.74ohm *SHUNT=3.73
LP2               59         0   0.315pH fcheck
L2                63        58   4.875pH fcheck
L3                63        60   4.924pH fcheck
LPA               55        64   0.364pH fcheck
LPR1              65        64   0.281pH fcheck
L6                61        63   0.455pH fcheck
L5                60        57   1.713pH fcheck
L1                64        61   2.361pH fcheck
LP3                5         0   0.322pH fcheck
LP1               62         0   0.099pH fcheck
L4                58        56   1.703pH fcheck
R1                66        65   8.35ohm *FIX
.ends

*** dcsfq13
.subckt dcsfq13         12         13        14
***       din      dout
R1                14        25  16.00ohm
R2                14        15  10.00ohm
B3                38        39  jjmod area=1.50
RS3               38        39   2.49ohm *SHUNT=3.73
B4                53        67  jjmod area=1.96
RS4               53        67   1.90ohm *SHUNT=3.73
B1                47        68  jjmod area=1.32
RS1               47        68   2.83ohm *SHUNT=3.73
B2                47        48  jjmod area=1.00
RS2               47        48   3.73ohm *SHUNT=3.73
B5                42        69  jjmod area=1.96
RS5               42        69   1.90ohm *SHUNT=3.73
LP3               39         0   0.195pH fcheck
L6                38        51   1.323pH fcheck
L3                16        68   1.755pH fcheck
L2                16         0   8.005pH fcheck
LP4               67         0   0.260pH fcheck
LP5               69         0   0.237pH fcheck
L1                12        16   0.811pH fcheck
L8                53        42   2.948pH fcheck
L7                51        53   1.110pH fcheck
L9                42        13   1.609pH fcheck
L4                47        54   0.500pH fcheck
L5                54        38   3.247pH fcheck
LPR2              15        51   0.286pH fcheck
LP2               48         0   0.473pH fcheck
LPR1              25        54   0.980pH fcheck
.ends

*** r50ohmshort50um
.subckt r50ohshort50um         12         13
***       din      dout
R1                12        13  50.00ohm
.ends

*** dcsfq13_50ohm
.subckt dcsfq13_50oh         12         13        14
***       din      dout
XIds          dcsfq13          9         13         14
XIR        r50ohshort50um         12          9
.ends

*** jtlc2
.subckt jtlc2         12         13        14
***       din      dout
R1                14        15   8.34ohm
LPIN              12        16   0.169pH fcheck
LPR1              15        16   0.252pH fcheck
L2                17        18   4.662pH fcheck
L3                18        13   1.817pH fcheck
L1                16        17   2.119pH fcheck
LP2               19         0   0.356pH fcheck
LP1               20         0   0.192pH fcheck
B2                18        19  jjmod area=2.16
RS2               18        19   1.73ohm *SHUNT=3.73
B1                17        20  jjmod area=2.16
RS1               17        20   1.73ohm *SHUNT=3.73
.ends

*** jtlc
.subckt jtlc         12         13        14
***       din      dout
R1                14        15   8.34ohm
LPIN              12        16   0.359pH fcheck
LPR1              15        16   0.270pH fcheck
L2                17        18   4.922pH fcheck
L3                18        13   2.007pH fcheck
L1                16        17   2.493pH fcheck
LP2               19         0   0.135pH fcheck
LP1               20         0   0.099pH fcheck
B2                18        19  jjmod area=2.16
RS2               18        19   1.73ohm *SHUNT=3.73
B1                17        20  jjmod area=2.16
RS1               17        20   1.73ohm *SHUNT=3.73
.ends

*** top cell: SQUIDMULtest
XI1          SQUIDMUL         70          0         72          5
*** ("71") mapped to 0
XI0          SQUIDMUL         63         70         17         10
XI15              jtl         73          8         77
XI5               jtl         18          7         77
XI2               jtl         20         11         77
XI9           sfqdc14          7          5         77
XI8           sfqdc14         74         72         77
XI7           sfqdc14         62         10         77
XI6           sfqdc14         11         17         77
XI11             spll         75         61         20         77
XI10             spll          9         19         18         77
XI12            jtlc3         19         74         77
XI13            jtlc3         61         62         77
XI14             splt          8         76          6         77
XI16       dcsfq13_50oh         65         73         77
XI17            jtlc2         76         75         77
XI18             jtlc          6          9         77
I20               63         0  PWL(0ps 0mA 10ps 200uA)
V0                77         0  PWL(0ps 0mv 10ps 2.5mv)
R0                63         0  50.00ohm
V1                65         0  PULSE(0.0mV  50mV   100.0ps   4.0ps   4.0ps   6.0ps   200.0ps)

*** netlist file ***

*** jsim input file ***

*** jsim input file ***

.tran 0.5ps 1ns 50ps 1ps
.file OUT.DAT
.print devv XI15_B2
.print devv XI2_B2
.print NODEV 0   63
