*JSIM model
.model jjmod jj(Rtype=1, Vg=2.8mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=0.1mA)

*** netlist file ***

*** ptl_2ohm
.subckt mem_cell          1          3          4          5         21
***       Ix1      Ix2      Iy1      Iy2          Ib

Lin                 1         2   2pH fcheck
B1                2        8   jjmod area=2.20
RS1               2        8   1.70ohm *SHUNT=3.73
L1                 2         3   2pH fcheck
B2                3        10   jjmod area=2.20
RS2               3        10   1.70ohm *SHUNT=3.73
L2                 3         4   2pH fcheck
Rb1               4         21  8ohm
L3                 4         5   2pH fcheck
B3                5        11   jjmod area=2.20
RS3               5        11   1.70ohm *SHUNT=3.73



.ends

*** top cell: JJ210
B8                1        2   jjmod area=2.20
RS8               1        2   1.70ohm *SHUNT=3.73
XI1        ptl_2ohm  1        3   
Rsource           3        0   1000ohm
Rsource2	  2       0   1ohm
Ib1                0        1   PWL(0ps 0mA 100ps 1mA)
Ib2                0        1   PWL(0ps 0mA 100ps 1mA)


*** jsim input file ***
.tran 0.01ps 150PS 100PS 0.1PS
.FILE OUT.DAT
.PRINT DEVI Rsource2
.PRINT nodev 1 0