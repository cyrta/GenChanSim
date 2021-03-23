% LOADCOMPLEX  Read complex data from disk.
%              DATA = LOADCOMPLEX('FILE.NAME') loads all the data in 'FILE.NAME'
%              into the complex vector, DATA.  The file format of 'FILE.NAME' 
%              must be floating-point complex pairs, e.g., the sequence 
%              1.5-j2.3, 3.4+j5.7... must be stored as [1.5][-2.3][3.4][5.7]...
%              where [] represents a 32-bit float.
% r.coutts
function out = loadcomplex(filename)
	fid = fopen(filename,'r');
	data = fread(fid, 'float');
	len = length(data);
	out = data(1:2:len) + j*data(2:2:len);
