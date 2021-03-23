echo on
% define constants
highrate = 10000000;
lowrate =    200000;
sampdelay = 5;
% read in data from files and convert to complex
fid=fopen('exp10out.dat','r');
samp10=fread(fid,inf,'float');
fclose(fid);
data10=samp10(1:2:length(samp10)) + j*samp10(2:2:length(samp10));
clear samp10;
fid=fopen('exp200out.dat','r');
samp200=fread(fid,inf,'float');
fclose(fid);
data200=samp200(1:2:length(samp200)) + j*samp200(2:2:length(samp200));
clear samp200;
% compute variables
rateratio = highrate/lowrate;
x10=1:length(data10);
x200=1:rateratio:(rateratio*length(data200));
% add 5 sample delay @ 200 Khz rate introduced by filters
x10=x10+(rateratio*sampdelay);
% convert x to milliseconds
x10=x10/(highrate/1000);
x200=x200/(highrate/1000);
% generate plot of time variant channel response
figure(1);
hold off
plot(x200,abs(data200),'g--');
axis([0 30 -2.5 2.5]);
title('time variant channel response envelope for carrier at band center.');
ylabel('magnitude');
xlabel('milliseconds');
hold on
plot(x10(1:rateratio:length(x10)),abs(data10(1:rateratio:length(data10))),'y-');
axis([0 30 0 2.5]);

