% function z = fftmesh(y, jump, npts)
% create a mesh matrix representing time vs. frequency from a time sequence
% 'jump' is the number of pts to jump.  'blockpts' is the number of pts
% in a single block.
% r.coutts

function z = fftmesh(y, jump, npts)

nblocks = length(y)/jump;
for m = 1:nblocks
	disp('performing fft...');
	start = (m-1)*jump+1;
	finish = start+npts-1;
	f = 20*log10(abs(fft(y(start:finish).*hamming(npts), npts)/npts));
	z(1:npts, m) = f;	% mesh in dB
end
