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

*** ("45") mapped to 0
V7                50         0  PULSE(0.0mV  1.034mV   0ps   1.0ps   1.0ps   1.0ps   20.0ps)
V1                47         0  PWL(0ps 0mv 10ps 2.5mv)
Vbias             43         0  PWL(0ps 0mv 10ps 2.5mv)
XI1               jtl         50         49         47
XI2               jtl         49         48         47
XI3               jtl         48         46         43
XItest            jtl         46         45         43
XI4               SINK        45         47
**Rgnd               50         46   0.05ohm *SHUNT=3.73   
*** netlist file ***

*** jsim input file ***

*** jsim input file ***
.tran 0.5PS 3nS 0PS 0.1PS
.file JTLOUT.DAT
**.print nodev 50 0
.print phase XI2_B1
.print phase XI4_B1
