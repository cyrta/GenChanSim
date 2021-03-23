% function plotfft(samples)
% plot fft centering 0Hz in the middle of the plot
% r.coutts

function plotfft(samples)

len = length(samples);
y = samples(floor(len/2)+1:len);
y(ceil(len/2)+1:len) = samples(1:floor(len/2));
plot(y)
