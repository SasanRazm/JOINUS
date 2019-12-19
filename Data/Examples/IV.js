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
RS8               1        2   6.2ohm *SHUNT=3.73
LS8               1        3   29.28nH
CS8               3        2   0.09pF
Rsource           2        0   1ohm
I0                0        1   PWL(0ps 0mV 50ps 1mA)
*I1               1         0   SIN(0ps 0.2mA 50GHZ 0us 0)


*** jsim input file ***
.tran 0.5ps 150PS 100PS 0.1PS
.FILE OUT.DAT
.PRINT DEVI Rsource
.PRINT nodev 1 2
**.PRINT DEVI RS8
