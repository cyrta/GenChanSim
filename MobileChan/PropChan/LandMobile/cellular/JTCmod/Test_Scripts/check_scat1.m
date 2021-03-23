echo on
w1=hamming(8192)';
p1=zeros(1,8192);
p2=zeros(1,8192);
p3=zeros(1,8192);
p4=zeros(1,8192);
fid=fopen('Scatout.dat','r');
samp=fread(fid,200,'float');
data=samp(1:2:length(samp)) + j*samp(2:2:length(samp));
imp=abs(data) .^ 2;
p1(1) = data(10);
p2(1) = data(50);
p3(1) = data(63);
p4(1) = data(71);
for index=2:8192
   samp=fread(fid,200,'float');
   data=samp(1:2:length(samp)) + j*samp(2:2:length(samp));
   imp = imp + (abs(data) .^ 2);
   p1(index) = data(10);
   p2(index) = data(50);
   p3(index) = data(63);
   p4(index) = data(71);
end
for index=8193:10000
   samp=fread(fid,200,'float');
   data=samp(1:2:length(samp)) + j*samp(2:2:length(samp));
   imp = imp + (abs(data) .^ 2);
end
imp=imp/10000;
fclose(fid);
mag=imp;
for ii=1:100
   if mag(ii) < 1e-10
      mag(ii) = 1e-10;
   end
end
mag=10*log10(mag);
% time access in mivroseconds
time=1:100;
time=(time-1)*0.1;
figure(1);
plot(time,mag);
xlabel('microseconds');
ylabel('dB');
title('scatter/discrete delay power profile');
axis([0 10 -40 0]);
grid on;
mag1 = 0;
mag2 = imp(50);
mag3 = imp(63);
mag4 = 0;
for ii=10:30
   mag1=mag1 + imp(ii);
end
for ii=71:89
   mag4=mag4 + imp(ii);
end

% output magnitude numbers
[mag1,mag2,mag3,mag4,(mag1+mag2+mag3+mag4)]
[10*log10(mag1), 10*log10(mag2), 10*log10(mag3), 10*log10(mag4)]

