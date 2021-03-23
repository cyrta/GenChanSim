% plot the magnitude response, frequency response, and group delay
% formally 'dispcgrp.m' written by M.Sandler

function plottap(data)

npts = input('fft length =');
hnpts = npts/2;

dim = size(data);
if dim(2) == 2
  rqp = data(:,1)';
  iqp = data(:,2)';
else
  rqp = data(1:2:length(data-1))';
  iqp = data(2:2:length(data))';
end
h = rqp+i*iqp;
  
fs = 1.25e6;		% iq sampling rate
f = [-hnpts:hnpts-1] *fs/npts;
t = [-hnpts:hnpts-1]/fs;

x = fft(h,npts);
nindx = [(0:hnpts-1),(-hnpts:-1)];
n0 = input('# of samples to rotate by = ');
sx = x.* exp(-i*(n0*2*pi/npts)*nindx);

snphi = angle(sx);
rtndx = [(hnpts+1:npts),(1:hnpts)];
ngrdly = - (diff(unwrap(snphi(rtndx)))./(2*pi*diff(f)));

plot(f,abs(sx(rtndx))), title('Resultant Frequency Mag Response'),..
xlabel('Frequency (Hz)'), ylabel('Magnitude'), grid, pause

plot(f,unwrap(snphi(rtndx))), title('Resultant Frequency Phase Response'),..
xlabel('Frequency (Hz)'), ylabel('Phase'), grid, pause

plot(f(1:npts-1),ngrdly), title('Resultant Group delay'),..
xlabel('Frequency (Hz)'), ylabel('Group delay (sec)'),  grid, pause
