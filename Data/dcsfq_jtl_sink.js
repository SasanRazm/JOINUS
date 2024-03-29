.SUBCKT JTL 4 5
B01        3          7          jj1     area=2.16
B02        6          8          jj1     area=2.16
IB01       0          1          pwl(0 0 5p 280u)
L01        4          3          2.031p    
L02        3          2          2.425p    
L03        2          6          2.425p    
L04        6          5          2.031p    
LP01       0          7          0.086p    
LP02       0          8          0.086p    
LPR01      2          1          0.278p    
LRB01      7          9          1p        
LRB02      8          10         1p        
RB01       9          3          5.23      
RB02       10         6          5.23      
.model jj1 jj(rtype=1, vg=2.8mV, cap=0.07pF, r0=160, rn=16, icrit=0.1mA)
.ends JTL
.SUBCKT DCSFQ 2 17
B01        5          3          jj1     area=1.32
B02        5          6          jj1     area=1
B03        9          10         jj1     area=1.5
B04        13         14         jj1     area=1.96
B05        15         16         jj1     area=1.96
IB01       0          8          pwl(0 0 5p 162.5u)
IB02       0          12         pwl(0 0 5p 260u)
L01        2          1          0.848p    
L02        0          1          7.712p    
L03        1          3          1.778p    
L04        5          7          0.543p    
L05        7          9          3.149p    
L06        9          11         1.323p    
L07        11         13         1.095p    
L08        13         15         2.951p    
L09        15         17         1.63p     
LP01       0          6          0.398p    
LP02       0          10         0.211p    
LP03       0          14         0.276p    
LP04       0          16         0.224p    
LPR01      7          8          0.915p    
LPR02      11         12         0.307p    
LRB01      4          5          1p        
LRB02      18         6          1p        
LRB03      19         10         1p        
LRB04      20         14         1p        
LRB05      21         16         1p        
RB01       3          4          8.56      
RB02       18         5          11.30     
RB03       19         9          7.53      
RB04       20         13         5.77      
RB05       21         15         5.77      
.model jj1 jj(rtype=1, vg=2.8mV, cap=0.07pF, r0=160, rn=16, icrit=0.1mA)
.ends DCSFQ
.SUBCKT SINK 2
B01        1          4          jj1     area=2.16
IB01       0          5          pwl(0 0 5p 280u)
L01        2          1          0.517p    
L02        1          3          5.307p    
LP01       0          4          0.086p    
LPR01      1          5          0.265p    
LRB01      4          6          1p        
RB01       6          1          5.23      
ROUT       0          3          4.02      
.model jj1 jj(rtype=1, vg=2.8mV, cap=0.07pF, r0=160, rn=16, icrit=0.1mA)
.ends SINK
IA         0          1         pwl(0 0 170p 0 176p 600u 182p 0 370p 0 376p 600u 382p 0 600p 0 606p 600u 612p 0 700p 0 706p 600u 712p 0)
X01        DCSFQ      1         2        
X02        JTL        2         3        
X03        JTL        3         4        
X04        JTL        4         5        
X05 	   SINK       5
.tran 0.25p 1000p 0 0.25p
.file jsimout.dat
.print nodev 1 0
*.print nodev 2 0
.print nodev 3 0
*.print nodev 4 0
.print nodev 5 0
