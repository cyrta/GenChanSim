echo on
% define constants
highrate = 10000000;
lowrate =    200000;
sampdelay = 5;
impperiod = 0.001;
highblock = 8192;
% read in data from files and convert to complex
fid=fopen('exp10out.dat','r');
samp10=fread(fid,inf,'float');
fclose(fid);
data10=samp10(1:2:length(samp10)) + j*samp10(2:2:length(samp10));
fid=fopen('exp200out.dat','r');
samp200=fread(fid,inf,'float');
fclose(fid);
data200=samp200(1:2:length(samp200)) + j*samp200(2:2:length(samp200));
% generate frequency axis array in KHz units
f10=1:highblock;
f10=(f10-(highblock/2 +1)) * (highrate/1000)/highblock;
% compute indexes into arrays
rateratio = highrate/lowrate;
n10 = highrate*impperiod;
n200 = lowrate*impperiod;
st10 = n10 + 1 - (rateratio*sampdelay);
st200 = n200 +1;
stp10=st10 + highblock-1;
% make lines for -50 and 50 KHz
flm50 = [-1*(lowrate/4000) -1*(lowrate/4000 +0.0000001)];
fdm50 = [-1000 1000];
fl50 =  [(lowrate/4000) (lowrate/4000 +0.0000001)];
fd50 = [-1000 1000];
% construct padded array of lower rate data
data200b=zeros(highblock,1);
for ii=st200:(st200+highblock/rateratio)
	data200b((ii-st200)*rateratio+1) = data200(ii);
end
% compute magnitude of response
mag200b = 10*log10(abs(fft(data200b)) .^ 2);
mag10 = 10*log10(abs(fft(data10(st10:stp10))) .^ 2);
% compute group delay
[gd200,fd200]=grpdelay(data200b,1,8192,'whole',10e6);
[gd10,fd10]=grpdelay(data10(st10:stp10),1,8192,'whole',10e6);
% plot magnitude response
figure(1);
hold off
plot(f10,fftshift(mag200b),'g--');
axis([-1*(lowrate/1000) (lowrate/1000) -15 15]);
title('mag. response of 10MHz (-) and 200KHz(--) sim. snapshot #1');
xlabel('frequency(KHz)');
ylabel('dB');
hold on
plot(f10,fftshift(mag10),'y-');
plot(flm50,fdm50,'y--');
plot(fl50,fd50,'y--');
% plot group delay
figure(2);
hold off
plot(f10,fftshift(gd200),'g--');
title('group delay response of 10MHz (-) and 200KHz(--) sim. snapshot #1');
xlabel('frequency(KHz)');
ylabel('10MHz samples');
hold on
plot(f10,fftshift(gd10),'y-');
axis([-1*(lowrate/1000) (lowrate/1000) 0 800]);
plot(flm50,fdm50,'y--');
plot(fl50,fd50,'y--');

% 2nd snapshot
% compute indexes into arrays
st10 = n10*2 + 1 - (rateratio*sampdelay);
st200 = n200*2 +1;
stp10=st10 + highblock-1;
% construct padded array of lower rate data
data200b=zeros(highblock,1);
for ii=st200:(st200+highblock/rateratio)
	data200b((ii-st200)*rateratio+1) = data200(ii);
end
% compute magnitude of response
mag200b = 10*log10(abs(fft(data200b)) .^ 2);
mag10 = 10*log10(abs(fft(data10(st10:stp10))) .^ 2);
% compute group delay
[gd200,fd200]=grpdelay(data200b,1,8192,'whole',10e6);
[gd10,fd10]=grpdelay(data10(st10:stp10),1,8192,'whole',10e6);
% plot magnitude response
figure(3);
hold off
plot(f10,fftshift(mag200b),'g--');
axis([-1*(lowrate/1000) (lowrate/1000) -15 15]);
title('mag. response of 10MHz (-) and 200KHz(--) sim. snapshot #2');
xlabel('frequency(KHz)');
ylabel('dB');
hold on
plot(f10,fftshift(mag10),'y-');
plot(flm50,fdm50,'y--');
plot(fl50,fd50,'y--');
% plot group delay
figure(4);
hold off
plot(f10,fftshift(gd200),'g--');
title('group delay response of 10MHz (-) and 200KHz(--) sim. snapshot #2');
xlabel('frequency(KHz)');
ylabel('10MHz samples');
hold on
plot(f10,fftshift(gd10),'y-');
axis([-1*(lowrate/1000) (lowrate/1000) 0 800]);
plot(flm50,fdm50,'y--');
plot(fl50,fd50,'y--');
