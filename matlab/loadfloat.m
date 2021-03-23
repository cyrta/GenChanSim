% LOADFLOAT    Read floating-point data from disk.
%              DATA = LOADFLOAT('FILE.NAME') loads all the data in 'FILE.NAME'
%              into the float vector, DATA.  The file format of 'FILE.NAME' 
%              must be binary floating-poin
% r.coutts
function data = loadfloat(filename)
	fid = fopen(filename,'r');
	data = fread(fid, 'float');
