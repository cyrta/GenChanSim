% AVGSUMSQ  MAGNITUDE = AVGSUMSQ(DATA)
%           Sum the magnitude squared of a vector
% r.coutts
function out = avgsumavq(data)
	out = sum(real(data).*real(data)) + sum(imag(data).*imag(data));
	out = out / length(data);
