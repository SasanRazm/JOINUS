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
B8                1        2   jjmod area=2.50
RS8               1        2   5.48ohm *SHUNT=3.73
Rsource           2        0   1ohm
I0                0        1   PWL(0ps 0mA 100ps 0.5mA)
I1               1         0   SIN(0ps 0.2mA 60GHZ 0us 0)


*** josim input file ***
.tran 0.2ps 500PS 0.1PS 0.1PS
*** JoSIM will not see this command
.FILE OUT.DAT
.PRINT DEVI Rsource
.PRINT nodev 1 2
.PRINT DEVI RS8