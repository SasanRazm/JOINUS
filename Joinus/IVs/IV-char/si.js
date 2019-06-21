**** **** **** **** **** **** **** **** **** **** **** 
*JSIM control file for CADENCE by kameda@cq.jp.nec.com
**** **** **** **** **** **** **** **** **** **** ****

*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : PascalOpen570
*** Cell: SQIF
*** View: schematic
*** Jun  1 17:54:12 2018
**** **** **** **** **** **** **** ****

*** top cell: SQIF
B8                1        2   jjmod area=2.10
RS8               1        2   1.78ohm *SHUNT=3.73
IRS8         1           2           NOISE(50p 0.0 1.0p)
Rsource           2        0   0.1ohm
I0                0        1   PWL(0ps 0mA 100ps 0.5mA)
**I1               1         0   SIN(0 100uA 200GHZ 0us 0)

*** netlist file ***

*** jsim input file ***
.tran 0.2ps 500PS 0.1PS 0.1PS
.FILE OUT.DAT
.PRINT DEVI Rsource
.PRINT NODEV 1 2
*** jsim input file ***
