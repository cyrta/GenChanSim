len=5000; 
fid=fopen('lonodata.bin','r');
rvs=fread(fid,[len,1],'float');
rvs(1:10)
