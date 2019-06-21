**** **** **** **** **** **** **** **** **** **** **** 
*JSIM control file for CADENCE by kameda@cq.jp.nec.com
**** **** **** **** **** **** **** **** **** **** ****

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : sasan
*** Cell: dcsfqjsim
*** View: schematic
*** Dec 29 16:41:23 2017
**** **** **** **** **** **** **** ****

*** sink
.subckt SINK          1        19
***       din
R1                19        20   8.34ohm
R2                21         0   4.02ohm
LPIN               1        22   0.364pH fcheck
LPR1              20        22   0.265pH fcheck
L3                23        21   5.307pH fcheck
L1                22        23   2.493pH fcheck
LP1               24         0   0.101pH fcheck
B1                23        24  jjmod area=2.16
RS1               23        24   1.73ohm *SHUNT=3.73
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

*** jand
.subckt jand         82         83         84          1       226
***         a         b         c       clk
B2                44        58  jjmod area=1.05
RS2               44        58   3.56ohm *SHUNT=3.73
B6                87        42  jjmod area=1.40
RS6               87        42   2.67ohm *SHUNT=3.73
B8               207       136  jjmod area=1.38
RS8              207       136   2.71ohm *SHUNT=3.73
B4               176       152  jjmod area=1.05
RS4              176       152   3.56ohm *SHUNT=3.73
B11              208        24  jjmod area=2.13
RS11             208        24   1.75ohm *SHUNT=3.73
B13               16        34  jjmod area=2.16
RS13              16        34   1.73ohm *SHUNT=3.73
B14               92         7  jjmod area=1.15
RS14              92         7   3.25ohm *SHUNT=3.73
B15              175        60  jjmod area=2.16
RS15             175        60   1.73ohm *SHUNT=3.73
B5               180        42  jjmod area=1.37
RS5              180        42   2.73ohm *SHUNT=3.73
B12               86        43  jjmod area=2.16
RS12              86        43   1.73ohm *SHUNT=3.73
B10              114        23  jjmod area=1.78
RS10             114        23   2.10ohm *SHUNT=3.73
B3                61       209  jjmod area=1.05
RS3               61       209   3.56ohm *SHUNT=3.73
B1               210        63  jjmod area=1.21
RS1              210        63   3.09ohm *SHUNT=3.73
B7               211       212  jjmod area=1.57
RS7              211       212   2.38ohm *SHUNT=3.73
B9               213       119  jjmod area=2.61
RS9              213       119   1.43ohm *SHUNT=3.73
LP2               58         0   0.564pH fcheck
L22              176        44   0.218pH fcheck
LP8              136         0   0.244pH fcheck
L2                44        87   2.714pH fcheck
L15               21        61   9.482pH fcheck
LPR6              51       214   0.252pH fcheck
L16               45       176  10.629pH fcheck
L3               114        21   2.070pH fcheck
L4               208        45   2.122pH fcheck
LPB               83       214   0.341pH fcheck
LPA               82        85   0.320pH fcheck
L13               85        86   2.548pH fcheck
L14              214        16   2.639pH fcheck
LP10              23         0   0.122pH fcheck
LP11              24         0   0.086pH fcheck
L12               16       208   4.805pH fcheck
LP13              34         0   0.101pH fcheck
L23              207        88   0.419pH fcheck
L11               86       114   5.296pH fcheck
LPR2             188        45   0.471pH fcheck
L6                88       152   3.136pH fcheck
L19               92       215   0.400pH fcheck
LPR4              95       215   0.473pH fcheck
LP14               7         0   0.629pH fcheck
L18              175        92   5.166pH fcheck
LP15              60         0   0.179pH fcheck
L7               215       207   2.135pH fcheck
LP12              43         0   0.122pH fcheck
LP7              212         0   0.296pH fcheck
LPR1             216        21   0.684pH fcheck
L5                88       209   2.608pH fcheck
L9               217       213   2.647pH fcheck
L17              109       175   2.184pH fcheck
LPR8             218       109   0.242pH fcheck
LPCLK              1       109   0.247pH fcheck
LPR5             219        85   0.278pH fcheck
L1               210       180   3.006pH fcheck
L21               61       210   0.247pH fcheck
LP9              119         0   0.216pH fcheck
L8               211       217   0.429pH fcheck
L20               42       211   0.419pH fcheck
LPR3             220       217   0.642pH fcheck
L10              213        84   2.335pH fcheck
LP1               63         0   0.541pH fcheck
R6               226        51   8.34ohm
R2               226       188  22.44ohm
R4               226        95  14.88ohm
R5               226       219   8.34ohm
R1               226       216  17.52ohm
R8               226       218   8.34ohm
R3               226       220  33.52ohm
.ends

***RTEST 1 1 15ohm
***Main cell

Va                3          0  PULSE(0.0mV  1.034mV   120ps   1.0ps   1.0ps   1.0ps   160.0ps)
Vb1               2          50  PULSE(0.0mV  1.034mV   200ps   1.0ps   1.0ps   1.0ps   320.0ps)
Vb                50          0  PULSE(0.0mV  1.034mV   280ps   1.0ps   1.0ps   1.0ps   320.0ps)
Vclk              1          0  PULSE(0.0mV  1.034mV   0ps   1.0ps   1.0ps   1.0ps   80.0ps)
Vbias             43         0  PWL(0ps 0mv 5ps 2.5mv)
XI1               jtl         3          4         43
XI2               jtl         4          5         43

XI3               jtl         2         6         43
XI4               jtl         6         7         43

XI5               jtl         1         8         43
XI6               jtl         8         10         43

XI7               jand         5         7         11          10       43

XI8               jtl         11         12         43
XI9               jtl         12         14         43

XI10               SINK        14         43
*** jsim input file ***
.tran 0.1PS 1nS 0PS 0.1PS
.file ANDOUT.DAT
.print devv XI2_B2
.print devv XI4_B2
.print devv XI6_B2
.print devv XI10_B1