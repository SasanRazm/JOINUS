function [Volt,Ibias] = IVplotJSIMFin(Imin,Imax,Points,OrgNetlist,AddNoise)
NewNetlist=['Modified',OrgNetlist];
outputfilename='OUT.DAT';
ParamName='I0';
WaitTime=40; %In pS
Tstep=10; %In pS
Volt=zeros(1,Points*2);
Ibias=zeros(1,Points*2);
[ReturnVal, TimeStep]=  NetlistGenIPWL(OrgNetlist,NewNetlist,ParamName,Imin,Imax,Points,Tstep,WaitTime,AddNoise);
data = JSIM(NewNetlist,outputfilename);
    [~,bb]=size(data);
 if ((bb<3) || (ReturnVal==0))
        disp('Check netlist!\n');
        disp('netlist Error!\n');
 end
 for loopcntr=1:Points*2
    firstindex=round(((Tstep+WaitTime)*loopcntr-WaitTime)/TimeStep);
    secondindex=round(((Tstep+WaitTime)*loopcntr)/TimeStep);
    Ibias(loopcntr)= data(firstindex,2);
    Volt(loopcntr)= sum(data(firstindex:secondindex,3))/(secondindex-firstindex);
 end
end

