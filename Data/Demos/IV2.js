***I-V characteristic of a simple Josephson junction

.model jmitll jj(rtype=1, vg=2.8mV, cap=0.07pF, r0=160ohm, rn=16ohm, icrit=0.1mA)

B01        1        2          jmitll     area=2
LP01       2        4          0.05p    
RB01       1        3          4.85ohm    * Shunt=4.85ohm
LPR01      3        4          0.2p
Rnonoise    4        0          1ohm
I0         0        1   	   PWL(0ps 0mA 100ps 1mA)


.tran 0.2ps 150PS 1PS 0.1PS
.FILE OUT.DAT
.PRINT DEVV Rnonoise *Voltage
.PRINT nodev 1 4  *Current
