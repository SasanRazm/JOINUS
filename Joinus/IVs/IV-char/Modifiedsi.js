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
I0      0       1       PWL(0ps 0mA 10ps -0.7mA)
Inew1   0       1       PWL(0ps 0mA 110ps 0mA 120ps 93.3333uA)
Inew2   0       1       PWL(0ps 0mA 170ps 0mA 180ps 93.3333uA)
Inew3   0       1       PWL(0ps 0mA 280ps 0mA 290ps 93.3333uA)
Inew4   0       1       PWL(0ps 0mA 740ps 0mA 750ps 93.3333uA)
Inew5   0       1       PWL(0ps 0mA 1150ps 0mA 1160ps 93.3333uA)
Inew6   0       1       PWL(0ps 0mA 1810ps 0mA 1820ps 93.3333uA)
Inew7   0       1       PWL(0ps 0mA 1870ps 0mA 1880ps 93.3333uA)
Inew8   0       1       PWL(0ps 0mA 1930ps 0mA 1940ps 93.3333uA)
Inew9   0       1       PWL(0ps 0mA 2840ps 0mA 2850ps 93.3333uA)
Inew10   0       1       PWL(0ps 0mA 2950ps 0mA 2960ps 93.3333uA)
Inew11   0       1       PWL(0ps 0mA 3260ps 0mA 3270ps 93.3333uA)
Inew12   0       1       PWL(0ps 0mA 3320ps 0mA 3330ps 93.3333uA)
Inew13   0       1       PWL(0ps 0mA 3380ps 0mA 3390ps 93.3333uA)
Inew14   0       1       PWL(0ps 0mA 3440ps 0mA 3450ps 93.3333uA)
Inew15   0       1       PWL(0ps 0mA 4500ps 0mA 4510ps -93.3333uA)
Inew16   0       1       PWL(0ps 0mA 4560ps 0mA 4570ps -93.3333uA)
Inew17   0       1       PWL(0ps 0mA 4620ps 0mA 4630ps -93.3333uA)
Inew18   0       1       PWL(0ps 0mA 4780ps 0mA 4790ps -93.3333uA)
Inew19   0       1       PWL(0ps 0mA 4890ps 0mA 4900ps -93.3333uA)
Inew20   0       1       PWL(0ps 0mA 5150ps 0mA 5160ps -93.3333uA)
Inew21   0       1       PWL(0ps 0mA 6060ps 0mA 6070ps -93.3333uA)
Inew22   0       1       PWL(0ps 0mA 6120ps 0mA 6130ps -93.3333uA)
Inew23   0       1       PWL(0ps 0mA 6180ps 0mA 6190ps -93.3333uA)
Inew24   0       1       PWL(0ps 0mA 6940ps 0mA 6950ps -93.3333uA)
Inew25   0       1       PWL(0ps 0mA 7050ps 0mA 7060ps -93.3333uA)
Inew26   0       1       PWL(0ps 0mA 7260ps 0mA 7270ps -93.3333uA)
Inew27   0       1       PWL(0ps 0mA 7370ps 0mA 7380ps -93.3333uA)
Inew28   0       1       PWL(0ps 0mA 7530ps 0mA 7540ps -93.3333uA)
Inew29   0       1       PWL(0ps 0mA 7590ps 0mA 7600ps -93.3333uA)
Inew30   0       1       PWL(0ps 0mA 7650ps 0mA 7660ps 0uA)
**I1               1         0   SIN(0 100uA 200GHZ 0us 0)

*** netlist file ***

*** jsim input file ***
.tran 0.2ps 7700PS 0.1PS 0.1PS
.FILE OUT.DAT
.PRINT DEVI Rsource
.PRINT NODEV 1 2
*** jsim input file ***
