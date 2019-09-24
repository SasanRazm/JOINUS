.model jjmod jj(Rtype=1, Vg=2.71572mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=100uA)
B8                1        2   jjmod area=1.50
RS8               1        2   5.48ohm 
Rsource           2        0   1ohm
I0                0        1   PWL(0ps 0mA 100ps 0.5mA)
.tran 0.2ps 500PS 0.1PS 0.1PS
.FILE sas.DAT
.PRINT DEVI Rsource
.PRINT nodev 1 2
.PRINT DEVI RS8
