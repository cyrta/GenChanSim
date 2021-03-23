% function z = interf(y, npts, color)
% interf(x) performs plot(20*log10(abs(fft(x, npts)))) with a hamming window

function z = interf(y, npts, color)

len = length(y);			% length of data

if nargin == 1
	npts = 1024;
	color = 'r';
end
if nargin == 2
	color = 'r';
end

xmax = 2*pi;
k = xmax/npts;
x = k:k:xmax;

z = 20*log10(abs(fft(y.*hamming(len), npts)/npts));	% get data in dB
ymax = ceil(max(z)/10)*10;		% get data max
ymin = floor(min(z)/10)*10;		% get data min
axis([0 xmax ymin ymax]);	% set plot limits 
plot(x, z, color)			% plot transfer function
