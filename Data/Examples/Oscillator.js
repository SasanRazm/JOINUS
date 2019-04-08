*** Example for the I-V charascteristic and the Shapiro step with microwave pumping 
*** for a damped Josephson junction.

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : Example1
*** Cell: JJIV
*** View: Schematic
*** Jan 2019
**** **** **** **** **** **** **** ****

*** top cell: JJ210
B8                1        2   jjmod area=1.50
RS8               1        2   0.48ohm *SHUNT=3.73
Rsource           2        0   0.01ohm
V0                0        1   PWL(0ps 0mV 100ps 1mV)
**I1               1         0   SIN(0ps 0.2mA 200GHZ 0us 0)


*** jsim input file ***
.tran 0.01ps 150PS 100PS 0.1PS
.FILE OUT.DAT
***.PRINT DEVV V0
.PRINT nodev 2 1
**.PRINT DEVI RS8
