function [Volt,Ibias,Indexes] = IVplotJSIMFins(Imin,OrgNetlist,AddNoise,WaitTime,Isteps)
NewNetlist=['Modified',OrgNetlist];
outputfilename='OUT.DAT';
ParamName='I0';
Tstep=10; %In pS
Points=(length(Isteps));
Volt=zeros(1,Points);
Ibias=zeros(1,Points);
[ReturnVal, TimeStep]=  NetlistGenIPWLs(OrgNetlist,NewNetlist,ParamName,Imin,Isteps,Tstep,WaitTime,AddNoise);
data = JSIM(NewNetlist,outputfilename);
    [~,bb]=size(data);
 if ((bb<3) || (ReturnVal==0))
        disp('Check netlist!\n');
        disp('netlist Error!\n');
 end
 
 for loopcntr=1:Points
    firstindex=round((Tstep*loopcntr+sum(WaitTime(1:loopcntr))-WaitTime(loopcntr))/TimeStep);
    secondindex=round((Tstep*loopcntr+sum(WaitTime(1:loopcntr)))/TimeStep);
    Ibias(loopcntr)= data(firstindex,2);
    Volt(loopcntr)= sum(data(firstindex:secondindex,3))/(secondindex-firstindex);
    Indexes(loopcntr,1)=firstindex;
    Indexes(loopcntr,2)=secondindex;
 end
 
  figure;
 plot(data(:,1)*1e9,data(:,2)*1e3,'-k','linewidth',2);
 set(gca, 'FontSize', 20);
 set(gca, 'FontName', 'Times');
 xlabel('\fontname{Times} Time (ns)','FontSize',20); 
 ylabel('\fontname{Times} Current (mA)','FontSize',20);
 grid on;

end

