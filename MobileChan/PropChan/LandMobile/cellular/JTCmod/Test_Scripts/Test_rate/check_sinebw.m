echo on
fid=fopen('exp10out.dat','r');
samp10=fread(fid,inf,'float');
fclose(fid);
data10=samp10(1:2:length(samp10)) + j*samp10(2:2:length(samp10));
fid=fopen('exp200out.dat','r');
samp200=fread(fid,inf,'float');
fclose(fid);
data200=samp200(1:2:length(samp200)) + j*samp200(2:2:length(samp200));
x10=1:length(data10);
x200=1:50:(50*length(data200));
% add 5 sample delay @ 200 Khz rate introduced by filters
x10=x10+250;
% convert x to milliseconds
x10=x10/10000;
x200=x200/10000;
figure(1);
hold off
plot(x200,abs(data200),'w--');
axis([0 30 -2.5 2.5]);
title('time variant channel response envelope for carrier at band center.');
ylabel('magnitude');
xlabel('milliseconds');
hold on
plot(x10(1:50:length(x10)),abs(data10(1:50:length(data10))),'w-');
axis([0 30 0 2.5]);

