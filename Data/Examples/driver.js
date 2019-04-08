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
*** drv_jtl
.subckt drv_jtl          2          3        26
***       din      dout
Rins               3        81   0.68ohm
R1                26        28   8.00ohm
L1                 2        32   0.330pH fcheck
LPR1              28        32   0.244pH fcheck
LPOUT             33        81   1.180pH fcheck
L2                32        33   2.244pH fcheck
LP1                7         0   0.096pH fcheck
B1                33         7   jjmod area=2.50
RS1               33         7   2.98ohm *SHUNT=7.46
.ends

*** dcsfq13
.subckt dcsfq13          1          2         5
***       din      dout
R1                 5         4  16.00ohm
R2                 5         6  10.00ohm
B3                 7         8  jjmod area=1.50
RS3                7         8   2.49ohm *SHUNT=3.73
B4                 9        10  jjmod area=1.96
RS4                9        10   1.90ohm *SHUNT=3.73
B1                11        12  jjmod area=1.32
RS1               11        12   2.83ohm *SHUNT=3.73
B2                11        13  jjmod area=1.00
RS2               11        13   3.73ohm *SHUNT=3.73
B5                14        15  jjmod area=1.96
RS5               14        15   1.90ohm *SHUNT=3.73
LP3                8         0   0.195pH fcheck
L6                 7        16   1.323pH fcheck
L3                17        12   1.755pH fcheck
L2                17         0   8.005pH fcheck
LP4               10         0   0.260pH fcheck
LP5               15         0   0.237pH fcheck
L1                 1        17   0.811pH fcheck
L8                 9        14   2.948pH fcheck
L7                16         9   1.110pH fcheck
L9                14         2   1.609pH fcheck
L4                11        18   0.500pH fcheck
L5                18         7   3.247pH fcheck
LPR2               6        16   0.286pH fcheck
LP2               13         0   0.473pH fcheck
LPR1               4        18   0.980pH fcheck
.ends

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

*** ptl_2oh
.subckt ptl_2oh          1          2
***       din      dout
C_IN 1 0 0.0.084pF
C_Main 1 2 0.06pF
C_OUT 2 0 0.084pF
L_L1 1 3 0.7nh
R_L1 3 0 100ohm
L_L2 1 2 0.744ph
L_L3 2 4 0.7nh
R_L3 4 0 100ohm

***L_MAIN 1 2 0.744pH fcheck
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

*** Top Cell
IIN               31         0  PULSE(0.0mA  1.034mA   0ps   1.0ps   1.0ps   1.0ps   100.0ps)
Vbias             30         0  PWL(0ps 0mv 10ps 2.5mv)
Rin               31        40   50.00ohm
XI0             dcsfq13      40         41         30
XI1              jtl         41         32         30
XI2              jtl         32         33         30
XI3              drv_jtl     33         34         30
XIPTL            ptl_2oh     34         50
XI5              rec         50      36         30
XI6              jtl         36         37         30
XI7              jtl         37         38         30
XI8              sink        38         30

*** netlist file ***

*** jsim input file ***
.tran 0.2ps 1NS 0PS 0.1PS
.FILE PULSES.DAT
.PRINT DEVV XI2_B2
.PRINT DEVV XI8_B1
*** jsim input file ***
