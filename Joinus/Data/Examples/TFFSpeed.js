**** **** **** **** **** **** **** **** **** **** **** 
*JSIM control file for CADENCE by kameda@cq.jp.nec.com
**** **** **** **** **** **** **** **** **** **** ****

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : sasan
*** Cell: SLTFFTest
*** View: schematic
*** Aug 15 03:47:27 2018
**** **** **** **** **** **** **** ****

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

*** sink
.subckt sink          1        10
***       din
R1                10        11   8.34ohm
R2                12         0   4.02ohm
LPIN               1        13   0.364pH fcheck
LPR1              11        13   0.265pH fcheck
L3                 9        12   5.307pH fcheck
L1                13         9   2.493pH fcheck
LP1                7         0   0.101pH fcheck
B1                 9         7  jjmod area=2.16
RS1                9         7   1.73ohm *SHUNT=3.73
.ends

*** SLTFF
.subckt SLTFF          1         14         15        17
***       din     dout0     dout1
L10               18        14   1.544pH fcheck
L11                8        15   2.350pH fcheck
LP7               19         0   0.190pH fcheck
L8                 7        18   3.097pH fcheck
L9                20         8   4.685pH fcheck
LP6               13         0   0.211pH fcheck
LPR2              21         7   0.520pH fcheck
L7                22         7   0.653pH fcheck
LP8               23        24   0.406pH fcheck
L12               25        22   0.003pH fcheck
L13               26        20   0.003pH fcheck
L5                23        22   3.146pH fcheck  
L6                20        23   3.214pH fcheck  
LP5               27         0   0.229pH fcheck
LP4               28         0   0.302pH fcheck
L2                29         9   1.019pH fcheck
L4                12         9   1.932pH fcheck  
L3                 9        30   1.851pH fcheck  
L1                31        29   2.369pH fcheck
LP1               32         0   0.130pH fcheck
LPR1              33        31   0.247pH fcheck
LPDIN              1        31   0.218pH fcheck
RD                24         0   2.00ohm           
R2                17        21  16.67ohm
R1                17        33   8.34ohm
B7                 8        19  jjmod area=2.18
RS7                8        19   1.71ohm *SHUNT=3.73
B6                18        13  jjmod area=2.18
RS6               18        13   1.71ohm *SHUNT=3.73
B5                26        27  jjmod area=1.96
RS5               26        27   1.90ohm *SHUNT=3.73
B3                26        12  jjmod area=1.32      
RS3               26        12   2.83ohm *SHUNT=3.73 
B2                25        30  jjmod area=1.84      
RS2               25        30   2.03ohm *SHUNT=3.73 
B4                25        28  jjmod area=1.10
RS4               25        28   3.39ohm *SHUNT=3.73
B1                29        32  jjmod area=2.18
RS1               29        32   1.71ohm *SHUNT=3.73
K1                L5        L6   -0.14
.ends

*** top cell: SLTFFTest
XI6               jtl         29         26         37
XI5               jtl         31         10         37
XI1               jtl         34         35         37
XI0               jtl         36         34         37
XI3              sink         10         37
XI2              sink         26         37
XI4             SLTFF         35         31         29         37
V1                37         0  PWL(0ps 0mv 10ps 2.5mv)

*** netlist file ***

*** jsim input file ***

*** jsim input file ***
.tran 0.5PS 5nS 10PS 0.1PS
.file OUTCNT.DAT
.print phase XI1_B2
.print phase XI2_B1
.print phase XI4_B1
