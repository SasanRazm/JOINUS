clc
clear
close all

Imin=-0.5;
Imax=0.5;
Points=400;

initialStepSize=((Imax-Imin)/Points);

data=dlmread('NoPump.DAT');
V=data(:,1);
I=data(:,2);
datal=length(I);
clear data;
Vthresh=(max(V)-min(V))*2/datal;
dV=diff(V)/Vthresh;

intdv=round(dV);
ddV=diff(intdv);


hold on
plot(V,I/(2e-5),'--')
plot(V(2:datal),intdv,'*')
plot(V(3:datal),ddV,'o')
legend('graph','dv','ddv');
grid on
hold off