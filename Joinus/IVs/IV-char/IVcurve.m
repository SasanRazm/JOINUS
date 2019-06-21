clc
clear

OrgNetlist='si.js';

AddNoise=0; 
Imin=-0.7;
Imax=0.7;
Points=15;
IstepNorm=(Imax-Imin)*1e-3;
WaitTimeMin=50;

Isteps=zeros(1,2*Points);
WaitTime=ones(1,2*Points)*WaitTimeMin;


tic
[Volt,Ibias] = IVplotJSIMFin(Imin,Imax,Points,OrgNetlist,AddNoise);
t1=toc;

%Volt=smooth(Volta);
 %Point distribution (Istep)
 dV=ones(1,Points*2-1);
% for vcntr=2:2*Points-1
%  dV1(vcntr)=(Volt(vcntr+1)-Volt(vcntr-1));
% end
% dV1(1)=dV1(2);
% dV=abs(dV1)+mean(abs(dV1))/5;
SumWeight1=sum(1./dV(1:Points));
SumWeight2=sum(1./dV(Points:2*Points-1));

ddVtemp=abs(diff(Volt,2));
ddV=(ddVtemp/max(ddVtemp))*20; %WaitTime distribution
currtest(1)=Imin*1e-3;

for stepcntr=1:Points-1
   Isteps(stepcntr)=(IstepNorm/SumWeight1)/abs(dV(stepcntr));
   currtest(stepcntr+1)=currtest(stepcntr)+Isteps(stepcntr);
   WaitTime(stepcntr)=(round(ddV(stepcntr))+1)*WaitTimeMin;
end
for stepcntr=Points:2*Points-1
   Isteps(stepcntr)=-(IstepNorm/SumWeight2)/abs(dV(stepcntr));
   currtest(stepcntr+1)=currtest(stepcntr)+Isteps(stepcntr);
   if (stepcntr~=2*Points-1)
    WaitTime(stepcntr)=(round(ddV(stepcntr))+1)*WaitTimeMin;
   end
end


tic
[Volt1,Ibias1,indexes] = IVplotJSIMFins(Imin,OrgNetlist,AddNoise,WaitTime,Isteps*1e6);
t2=toc;
%% test plot
figure
hold on
 plot(Ibias,'-ok','linewidth',1);
 plot(Ibias1,'-*r','linewidth',1);
 plot(currtest,'-*c','linewidth',1);
 plot(WaitTime*1e-6,':r','linewidth',1);
 plot(Volt,'--g','linewidth',1);
 plot(Volt1,'--b','linewidth',1);
 
 grid on
 hold off

%% plotting graph 
figure
hold on
 plot(Volt,Ibias,':k','linewidth',1);
 plot(Volt1,Ibias1,'-*r','linewidth',1);
 set(gca, 'FontSize', 20);
 set(gca, 'FontName', 'Times');
 xlabel('\fontname{Times} Voltage (mV)','FontSize',20); 
 ylabel('\fontname{Times} Current (mA)','FontSize',20);
 title('\fontname{Times} I-V curve');
 s1= strcat('\fontname{Times} JSIM T=', num2str(t1),'s');
 s2= strcat('\fontname{Times} JSIM T=', num2str(t2),'s');
 legend(s1,s2);
grid on;
hold off
