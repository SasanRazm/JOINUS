*JSIM model
.model jj1 jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** test for optimizer
*** Margsn calculation of JTL
**** **** **** **** **** **** **** ****

*** sink
.subckt SINK          1        19
***       din
.model jj1 jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)
R1                19        20   8.34ohm
R2                21         0   4.02ohm
LPIN               1        22   0.364pH fcheck
LPR1              20        22   0.265pH fcheck
L3                23        21   5.307pH fcheck
L1                22        23   2.493pH fcheck
LP1               24         0   0.101pH fcheck
B1                23        24   jj1  area=2.16
RS1               23        24   1.73ohm *SHUNT=3.73
.ends SINK

*** jtl
.subckt jtl          1          2         3
***       din      dout
.model jj1 jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)
R1                 3         4   8.34ohm
LPIN               1         5   0.354pH fcheck
LPR1               4         5   0.278pH fcheck
L2                 6         7   @b@pH fcheck
L3                 7         2   @c@pH fcheck
L1                 5         6   @a@pH fcheck
LP2                8         0   0.096pH fcheck
LP1                9         0   0.086pH fcheck
B2                 7         8   jj1  area=2.16
RS2                7         8   1.73ohm *SHUNT=3.73
B1                 6         9   jj1  area=@e@
RS1                6         9   @d@ohm *SHUNT=3.73
.ends jtl

*** ("45") mapped to 0
V7                46         0  PULSE(0.0mV  1.034mV   0ps   1.0ps   1.0ps   1.0ps   200.0ps)
V1                47         0  PWL(0ps 0mv 10ps 2.5mv)
V2                43         0  PWL(0ps 0mv 10ps 2.5mv)
XI7               jtl         46         48         47
XI6               jtl         48         44         43
XI5               SINK        44         47

*** jsim input file ***
.tran 0.2PS 2nS 10PS 0.1PS
.file JTLOUT.DAT
.print phase XI7_B2
.print phase XI6_B2
.print phase XI5_B1
