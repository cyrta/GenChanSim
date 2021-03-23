% SUMSQ  MAGNITUDE = SUMSQ(DATA)
%        Sum the magnitude squared of a complex vector
% r.coutts
function out = sumavq(data)
	out = sum(real(data).*real(data)) + sum(imag(data).*imag(data));
