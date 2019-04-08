**** **** **** **** **** **** **** **** **** **** **** 
*JSIM control file for CADENCE by kameda@cq.jp.nec.com
**** **** **** **** **** **** **** **** **** **** ****

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : 
*** Cell: 
*** View: schematic
*** Jun  
**** **** **** **** **** **** **** ****
*** sink
.subckt sink          2         9
***       din
R1                 9        46   8.34ohm
R2                60         0   4.02ohm
LPIN               2        65   0.364pH fcheck
LPR1              46        65   0.265pH fcheck
L3                 7        60   5.307pH fcheck
L1                65         7   2.493pH fcheck
LP1               15         0   0.101pH fcheck
B1                 7        15  jjmod area=2.16
RS1                7        15   1.73ohm *SHUNT=3.73
.ends
*** rec
.subckt rec          2          3        26
***       din      dout
R1                26        28  22.90ohm
L1                 2        32   0.296pH fcheck
LPR1              28        32   1.287pH fcheck
L2                33        15   5.366pH fcheck
L3                15         3   2.007pH fcheck
LP3               32        33   0.367pH fcheck
LP2               34         0   0.088pH fcheck
LP1                7         0   0.122pH fcheck
B2                15        34  jjmod area=2.16
RS2               15        34   1.73ohm *SHUNT=3.73
B1                33         7  jjmod area=1.40
RS1               33         7   5.33ohm *SHUNT=7.46
.ends

*** jtl
.subckt jtl          2          3        26
***       din      dout
R1                26        28   8.34ohm
LPIN               2        32   0.354pH fcheck
LPR1              28        32   0.278pH fcheck
L2                33        15   4.859pH fcheck
L3                15         3   2.031pH fcheck
L1                32        33   2.517pH fcheck
LP2               34         0   0.096pH fcheck
LP1                7         0   0.086pH fcheck
B2                15        34  jjmod area=2.16
RS2               15        34   1.73ohm *SHUNT=3.73
B1                33         7  jjmod area=2.16
RS1               33         7   1.73ohm *SHUNT=3.73
.ends

*** ptl_2ohm
.subckt ptl_2ohm          2          3
***       din      dout
C1                 2         0   0.168pF
L1                 2         3   0.744pH fcheck
.ends

*** Top Cell
VIN               50         0  SIN(0MV  1MV    10GHZ   -10PS   0)
Vbias             30         0  PWL(0ps 0mv 10ps 2.5mv)
***R1                @port        0   1ohm
XI1              ptl_2ohm    @port      36 
XI2              rec         36         33         30
XI3              jtl         33         34         30
XI6              jtl         34         35         30
XI8              sink        35         30

*** netlist file ***

*** jsim input file ***
.tran 0.2ps 10NS 0PS 0.1PS
.FILE REC.DAT
.PRINT NODEV @port 0
.PRINT DEVV XI8_B1
*** jsim input file ***