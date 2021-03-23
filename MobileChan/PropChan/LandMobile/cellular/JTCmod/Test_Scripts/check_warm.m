echo on
fid = fopen('ChOut1M2.dat','r');
dat = fread(fid,32768,'float');
fclose(fid);
datc = dat(1:2:length(dat)) + j*dat(2:2:length(dat));
magc= abs(datc) .^ 2;
plot(magc);
