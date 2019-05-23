*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***
**** **** **** **** **** **** **** ****+
*** Lib : PascalOpen571
*** Cell: JJIV
*** View: Schematic
*** Aug  25 17:54:12 2018
**** **** **** **** **** **** **** ****

*** top cell: JJ210
B8                1        2   jjmod area=1.50
RS8               1        2   5.48ohm *SHUNT=3.73
Rsource           2        0   1ohm
I0                0        1   PWL(0ps 0mA 100ps 0.5mA)
**I1               1         0   SIN(0ps 0.2mA 200GHZ 0us 0)

*** netlist file ***

*** jsim input file ***
.tran 0.2ps 500PS 0.1PS 0.1PS
.FILE sas.DAT
.PRINT DEVI Rsource
.PRINT nodev 1 2
.PRINT DEVI RS8

*** jsim input file ***