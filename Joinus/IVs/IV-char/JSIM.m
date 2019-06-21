function data = JSIM(netlistfile,outputfilename)
SimCommand=['jsim_n ',netlistfile];
system(SimCommand);
data=dlmread(outputfilename);
end