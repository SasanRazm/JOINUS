clc
clear
close all

%% draw Shapiro steps
data0=dlmread('NoPump.DAT');
data1=dlmread('100GHz.DAT');
data2=dlmread('150GHz.DAT');
data3=dlmread('200GHz.DAT');
data4=dlmread('250GHz.DAT');

figure
hold on;
 plot(data0(:,1)*1e3,data0(:,2)*1e6,'linewidth',2);
 plot(data1(:,1)*1e3,data0(:,2)*1e6,'linewidth',2);
 plot(data2(:,1)*1e3,data0(:,2)*1e6,'linewidth',2);
 plot(data3(:,1)*1e3,data0(:,2)*1e6,'linewidth',2);
 plot(data4(:,1)*1e3,data0(:,2)*1e6,'linewidth',2);
 
 set(gca, 'FontSize', 20);
 set(gca, 'FontName', 'Times');
 xlabel('\fontname{Times} Voltage (mV)','FontSize',20); 
 ylabel('\fontname{Times} Current (\muA)','FontSize',20);
 legend('No Pump','100GHz','150GHz','200GHz','250GHz');
grid on;
hold off;

%% Draw Beta parameters

data0=dlmread('IVBeta1.dat');
data1=dlmread('IVBeta2.dat');
data2=dlmread('IVBeta3.dat');
data3=dlmread('IVBeta4.dat');
data4=dlmread('IVBeta5.dat');

figure
hold on;
 plot(data0(:,1),data0(:,2)*1e3,'linewidth',2);
 plot(data1(:,1),data0(:,2)*1e3,'linewidth',2);
 plot(data2(:,1),data0(:,2)*1e3,'linewidth',2);
 plot(data3(:,1),data0(:,2)*1e3,'linewidth',2);
 plot(data4(:,1),data0(:,2)*1e3,'linewidth',2);
 
 set(gca, 'FontSize', 20);
 set(gca, 'FontName', 'Times');
 xlabel('\fontname{Times} Voltage (mV)','FontSize',20); 
 ylabel('\fontname{Times} Current (\muA)','FontSize',20);
 legend('\beta = 0.01','\beta = 0.5','\beta = 0.9','\beta = 5','\beta = 100');
grid on;
hold off;