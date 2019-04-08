.model jjmod jj(Rtype=1, Vg=2.71572mV, Cap=0.218pF, R0=200ohm, Rn=17ohm, Icrit=100uA)
.subckt r50ohmshort50um          1          2
R1                 1         2  50.00ohm
IR1   1   2   NOISE(2.154p 0.0 1.0p)
.ends
.subckt doubler          1          2         3
Lin                1         4   1.2pH fcheck
R1                 3         4   20ohm
IR1   3   4   NOISE(3.405p 0.0 1.0p)
B1                 4         0  jjmod area=1.16
RS1                4         0   2.9ohm 
IRS1   4   0   NOISE(8.943p 0.0 1.0p)
L1                 4         5   2.517pH fcheck
B2                 5         0  jjmod area=2.4
RS2                5         0   1.73ohm 
IRS2   5   0   NOISE(11.579p 0.0 1.0p)
B3                 6         5  jjmod area=2.4
RS3                6         5   1.73ohm 
IRS3   6   5   NOISE(11.579p 0.0 1.0p)
LB1                6         7   1.7pH fcheck
R2                 3         7   6ohm
IR2   3   7   NOISE(6.217p 0.0 1.0p)
LB2                7         8   1.14pH fcheck
B4                 9         0  jjmod area=2.4
RS4                9         0   1.73ohm 
IRS4   9   0   NOISE(11.579p 0.0 1.0p)
L2                 5         9   4.52pH fcheck
B5                 8         9  jjmod area=2.4
RS5                8         9   1.73ohm 
IRS5   8   9   NOISE(11.579p 0.0 1.0p)
Lout               8         2   2.517pH fcheck
.ends
.subckt jtl          1          2         3
R1                 3         4   8.34ohm
IR1   3   4   NOISE(5.274p 0.0 1.0p)
LPIN               1         5   0.354pH fcheck
LPR1               4         5   0.278pH fcheck
L2                 6         7   4.859pH fcheck
L3                 7         2   2.031pH fcheck
L1                 5         6   2.517pH fcheck
LP2                8         0   0.096pH fcheck
LP1                9         0   0.086pH fcheck
B2                 7         8  jjmod area=2.16
RS2                7         8   1.73ohm 
IRS2   7   8   NOISE(11.579p 0.0 1.0p)
B1                 6         9  jjmod area=2.16
RS1                6         9   1.73ohm 
IRS1   6   9   NOISE(11.579p 0.0 1.0p)
.ends
.subckt sfqdc14          1          2         3
R7                10         0   1.95ohm
IR7   10   0   NOISE(10.906p 0.0 1.0p)
R5                 3        12   9.78ohm
IR5   3   12   NOISE(4.87p 0.0 1.0p)
R2                 3        14  14.76ohm
IR2   3   14   NOISE(3.964p 0.0 1.0p)
R1                 3        16   8.34ohm 
IR1   3   16   NOISE(5.274p 0.0 1.0p)
R4                 3        18  17.52ohm
IR4   3   18   NOISE(3.638p 0.0 1.0p)
R6                 3        20   5.70ohm
IR6   3   20   NOISE(6.379p 0.0 1.0p)
R3                 3         4  14.28ohm
IR3   3   4   NOISE(4.03p 0.0 1.0p)
B7                21        22  jjmod area=2.56
RS7               21        22   1.46ohm 
IRS7   21   22   NOISE(12.604p 0.0 1.0p)
B8                23        24  jjmod area=1.32
RS8               23        24   2.83ohm 
IRS8   23   24   NOISE(9.053p 0.0 1.0p)
B6                25        26  jjmod area=2.33
RS6               25        26   1.60ohm 
IRS6   25   26   NOISE(12.04p 0.0 1.0p)
B3                27        28  jjmod area=1.96
RS3               27        28   1.90ohm 
IRS3   27   28   NOISE(11.049p 0.0 1.0p)
B4                29        30  jjmod area=1.44
RS4               29        30   2.59ohm 
IRS4   29   30   NOISE(9.463p 0.0 1.0p)
B5                31        22  jjmod area=1.44
RS5               31        22   2.59ohm 
IRS5   31   22   NOISE(9.463p 0.0 1.0p)
B9                32        33  jjmod area=1.32
RS9               32        33   2.83ohm 
IRS9   32   33   NOISE(9.053p 0.0 1.0p)
B1                34        35  jjmod area=1.96
RS1               34        35   1.90ohm 
IRS1   34   35   NOISE(11.049p 0.0 1.0p)
B2                36        37  jjmod area=1.96
RS2               36        37   1.90ohm 
IRS2   36   37   NOISE(11.049p 0.0 1.0p)
LPR7              38        10   0.387pH fcheck
L15               39         2   7.340pH fcheck
LPR5              12        32   1.240pH fcheck
L12               39        32   0.100pH fcheck
L11               23        39   0.100pH fcheck
LP7               21         0   0.949pH fcheck
LP6               25         0   0.972pH fcheck
L10               22        38   5.681pH fcheck
L17               38        24   0.001pH fcheck
L9                26        24   5.247pH fcheck
L16               30        26   0.614pH fcheck
LP3               28         0   0.234pH fcheck
L5                27        40   2.839pH fcheck
LPR1              16         5   0.242pH fcheck
LPR6              20        41   0.200pH fcheck
L13               33        41   1.872pH fcheck
L1                 5        34   1.895pH fcheck
L14               41         0   0.741pH fcheck
L7                42        29   0.863pH fcheck
LPIN               1         5   0.263pH fcheck
L8                42        31   1.596pH fcheck
L6                40        42   0.263pH fcheck
L3                36        43   1.591pH fcheck
L4                43        27   1.570pH fcheck
L2                34        36   3.471pH fcheck
LPR3               4        40   0.281pH fcheck
LP1               35         0   0.198pH fcheck
LPR4              18        30   0.426pH fcheck
LP2               37         0   0.205pH fcheck
LPR2              14        43   0.416pH fcheck
.ends
XI10       r50ohmshort50um         49          0
V7                46         0  PULSE(0.0mV  1.034mV   0ps   1.0ps   1.0ps   1.0ps   50.0ps)
V1                47         0  PWL(0ps 0mv 10ps 2.5mv)
XI1           doubler         44         49         47
XI7               jtl         46         48         47
XI6               jtl         48          3         47
XI9           sfqdc14          3         44         47
.tran 0.1PS 400PS 10PS 0.1PS
.file OUT.DAT
.print nodev 49 0
.print devv XI6_B2
