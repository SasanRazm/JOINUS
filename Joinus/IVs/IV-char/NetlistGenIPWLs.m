%Function that gets the netlist from OrgNetlist and breaks it down 
%for manipulation of it.
%ParamName is the Parameter name that you want to change in the netlist
%ParamVal is the new value of the parameter. 
%If add noise is activated (AddNoise=1), the netlist would include noise 
%current source on the resistors equal to Johnson noise.
%ReturnVal shows how many lines we found in the netlist with the ParamName
%in them

function [ReturnVal , TimeStep] = NetlistGenIPWLs(OrgNetlist,NewNetlist,ParamName,Imin,Isteps,Tstep,WaitTime,AddNoise)

KBolt = 1.38e-23;
temperature=4.2;
Points=(length(Isteps))/2;
%Open main netlist file and read all the lines
fid = fopen(OrgNetlist);
tline = fgetl(fid);
tlines = cell(0,1);
while ischar(tline)
    tlines{end+1,1} = tline;
    tline = fgetl(fid);
end
fclose(fid);

%Find the line with ParamName in the file
SearchVal=[ParamName,' '];
eqnLines = regexp(tlines,SearchVal,'match','once');
eqnLineMask = ~cellfun(@isempty, eqnLines);
i = find(eqnLineMask);
ReturnVal=sum(eqnLineMask);
SplitedLine=strsplit(tlines{i(1)});
aa=char(SplitedLine);
ParamVal=['PWL(0ps 0mA ', num2str(Tstep), 'ps ', num2str(Imin),'mA)'];
tlines{i(1)}=[aa(1,:),' ',aa(2,:),' ',aa(3,:),' ',ParamVal];



for LineCNTR=1:Points*2
 ParamVal=['PWL(0ps 0mA ', num2str(Tstep*LineCNTR+sum(WaitTime(1:LineCNTR))), 'ps 0mA ', num2str(Tstep*(LineCNTR+1)+sum(WaitTime(1:LineCNTR))), 'ps ', num2str(Isteps(LineCNTR)),'uA)'];   
 Newlines{LineCNTR}=['Inew',num2str(LineCNTR),'   ',aa(2,:),' ',aa(3,:),' ',ParamVal];
end

%Change transition Time
eqnLines = regexp(tlines,'tran','match','once');
eqnLineMask = ~cellfun(@isempty, eqnLines);
TranIndex = find(eqnLineMask);
SplitedLine=strsplit(tlines{TranIndex});
bb=char(SplitedLine);
TimeStepstr=textscan(bb(2,:),'%[^ps]');
TimeStep=str2double(TimeStepstr{1});

tlines{TranIndex}=[bb(1,:),' ',bb(2,:),' ', num2str(Tstep*LineCNTR+sum(WaitTime)+WaitTime(LineCNTR)),'PS ',bb(4,:),' ',bb(5,:)];
%Create the new netlist
%Add noise to the resistors
    fid = fopen(NewNetlist,'wt');
    eqnLinesR = regexp(tlines,'ohm','match','once');
    eqnLineMaskR = ~cellfun(@isempty, eqnLinesR);
    eqnLinesR2 = regexp(tlines,'Rtype','match','once');
    eqnLineMaskR2 = ~cellfun(@isempty, eqnLinesR2);
    eqnLinesR3 = regexp(tlines,'Rsource','match','once');
    eqnLineMaskR3 = ~cellfun(@isempty, eqnLinesR3);
    ResMasks=eqnLineMaskR-eqnLineMaskR2-eqnLineMaskR3;

    ResistorPlace = find(ResMasks>0);
    jj=1;
    for ii=1:length(tlines)
        fprintf(fid, '%s\n',tlines{ii});
        if ii==i(1)
            for LineCNTR=1:Points*2
                fprintf(fid, '%s\n', Newlines{LineCNTR});
            end
        end
        ifcond=isscalar(find(ResistorPlace==ii)); 
       %Add noise on any resistor that program find in the netlist
       %If resistor is marked with Rsource, the program would not add noise to it 
            if ((AddNoise) && (ifcond))
                SplitedLineRes=strsplit(tlines{ResistorPlace(jj)});
                ResCells=char(SplitedLineRes);
                rvalstr=textscan(ResCells(4,:),'%[^ohm]');
                rval=str2double(rvalstr{1});
                noise = round(1000*(sqrt((4*KBolt*temperature)/rval)*1e12))/1000;
                NoiseLine = ['I',ResCells(1,:),' ',ResCells(2,:),' ',ResCells(3,:),' ',['NOISE(' num2str(noise) 'p 0.0 1.0p)']];
                fprintf(fid, '%s\n',NoiseLine);
                jj=jj+1;
            end
    end
    fclose(fid);

end

