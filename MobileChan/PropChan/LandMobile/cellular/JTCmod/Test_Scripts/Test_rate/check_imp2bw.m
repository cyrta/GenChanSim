echo on
fid=fopen('exp10out.dat','r');
samp10=fread(fid,inf,'float');
fclose(fid);
data10=samp10(1:2:length(samp10)) + j*samp10(2:2:length(samp10));
fid=fopen('exp200out.dat','r');
samp200=fread(fid,inf,'float');
fclose(fid);
data200=samp200(1:2:length(samp200)) + j*samp200(2:2:length(samp200));
% generate frequency axis array in KHz units
f10=1:8192;
f10=(f10-4097) * 10e3/8192;
f200=1:128;
f200=(f200-65) * 200/128;
n10 = 10000;
n200 = 200;
st10 = n10 + 1 -250;
st200 = n200 +1;
stp10=st10 + 8191;
stp200=st200 + 127;
data200b=zeros(8192,1);
for ii=st200:(st200+8192/50)
	data200b((ii-st200)*50+1) = data200(ii);
end
mag200b = 10*log10(abs(fft(data200b)) .^ 2);
phase200b = angle(fft(data200b));
mag10 = 10*log10(abs(fft(data10(st10:stp10))) .^ 2);
% make lines for -50 and 50 KHz
flm50 = [-50 -50.0000001];
fdm50 = [-1000 1000];
fl50 = [50 50.0000001];
fd50 = [-1000 1000];

% 3rd snapshot
st10 = n10*3 + 1 -250;
st200 = n200*3 +1;
stp10=st10 + 8191;
stp200=st200 + 127;
data200b=zeros(8192,1);
for ii=st200:(st200+8192/50)
	data200b((ii-st200)*50+1) = data200(ii);
end
mag200b = 10*log10(abs(fft(data200b)) .^ 2);
phase200b = angle(fft(data200b));
mag10 = 10*log10(abs(fft(data10(st10:stp10))) .^ 2);
figure(5);
hold off
plot(f10,fftshift(mag200b),'w--');
title('mag. response of 10MHz (-) and 200KHz(--) sim. snapshot #3');
xlabel('frequency(KHz)');
ylabel('dB');
hold on
plot(f10,fftshift(mag10),'w-');
plot(flm50,fdm50,'w--');
plot(fl50,fd50,'w--');
axis([-200 200 -15 15]);
% group delay
figure(6);
hold off
[gd200,fd200]=grpdelay(data200b,1,8192,'whole',10e6);
[gd10,fd10]=grpdelay(data10(st10:stp10),1,8192,'whole',10e6);
plot(f10,fftshift(gd200),'w--');
title('group delay response of 10MHz (-) and 200KHz(--) sim. snapshot #3');
xlabel('frequency(KHz)');
ylabel('10MHz samples');
hold on
plot(f10,fftshift(gd10),'w-');
axis([-200 200 0 800]);
plot(flm50,fdm50,'w--');
plot(fl50,fd50,'w--');


% 4th snapshot
st10 = n10*4 + 1 -250;
st200 = n200*4 +1;
stp10=st10 + 8191;
stp200=st200 + 127;
data200b=zeros(8192,1);
for ii=st200:(st200+8192/50)
	data200b((ii-st200)*50+1) = data200(ii);
end
mag200b = 10*log10(abs(fft(data200b)) .^ 2);
phase200b = angle(fft(data200b));
mag10 = 10*log10(abs(fft(data10(st10:stp10))) .^ 2);
figure(7);
hold off
plot(f10,fftshift(mag200b),'w--');
title('mag. response of 10MHz (-) and 200KHz(--) sim. snapshot #4');
xlabel('frequency(KHz)');
ylabel('dB');
hold on
plot(f10,fftshift(mag10),'w-');
plot(flm50,fdm50,'w--');
plot(fl50,fd50,'w--');
axis([-200 200 -15 15]);
% group delay
figure(8);
hold off
[gd200,fd200]=grpdelay(data200b,1,8192,'whole',10e6);
[gd10,fd10]=grpdelay(data10(st10:stp10),1,8192,'whole',10e6);
plot(f10,fftshift(gd200),'w--');
title('group delay response of 10MHz (-) and 200KHz(--) sim. snapshot #4');
xlabel('frequency(KHz)');
ylabel('10MHz samples');
hold on
plot(f10,fftshift(gd10),'w-');
axis([-200 200 0 800]);
plot(flm50,fdm50,'w--');
plot(fl50,fd50,'w--');
