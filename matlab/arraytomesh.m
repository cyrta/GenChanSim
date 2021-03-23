% function z = arraytomesh(y, jump, nblockpts)
% create a mesh matrix representing time vs. frequency from a time sequence
% 'jump' is the number of pts to jump.  'blockpts' is the number of pts
% in a single block.
% r.coutts

function z = arraytomesh(y, jump, nblockpts)

nblocks = length(y)/jump;
for m = 1:nblocks
	start = (m-1)*jump+1;
	finish = start+nblockpts-1;
	z(1:nblockpts, m) = y(start:finish);	% mesh in dB
end
