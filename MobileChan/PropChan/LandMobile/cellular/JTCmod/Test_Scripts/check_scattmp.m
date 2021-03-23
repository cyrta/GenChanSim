echo on
fid=fopen('Scatout.dat','r');
samp=fread(fid,400,'float');
data=samp(1:2:length(samp)) + j*samp(2:2:length(samp));
imp=abs(data) .^ 2;
for index=2:5000
   samp=fread(fid,400,'float');
   data=samp(1:2:length(samp)) + j*samp(2:2:length(samp));
   imp = imp + (abs(data) .^ 2);
end
imp=imp/5000;
fclose(fid);
mag=imp;
for ii=1:200
   if mag(ii) < 1e-10
      mag(ii) = 1e-10;
   end
end
mag=10*log10(mag);
% normalize
maxmag=max(mag);
mag = mag-maxmag;
% time access in mivroseconds
time=1:200;
time=(time-1)*0.1;
figure(1);
plot(time,mag);
%plot(time,imp);
xlabel('microseconds');
ylabel('dB');
%ylabel('magnitude squared');
title('scatter/discrete delay power profile');
% axis([0 10 -40 0]);
grid on;

