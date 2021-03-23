len=10000; % must be even.
fid=fopen('clonodata.bin','r');
rvs=fread(fid,[len,1],'float');
rvs=reshape(rvs,2,len/2).';
rvs=rvs(:,1)+i*rvs(:,2);
rvs(1:10)
