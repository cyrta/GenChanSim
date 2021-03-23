function [pxx]=check_jakes(fid,N,M,loop)

% check_jakes.m takes as input the tapweight output files from the
% mobile-comm simulator 'mobilechan.c' (either the -o file or
% the debug file: 'tapwgts.dbg'.  It calulates the averaged FFT
% of the tap weight spectra.  It is assumed that the input file only
% contains the results of 1 path since we do a simple 1:2 deinterleave
% process on the interleaved I/Q data.
%
% Written by C. A. Nissen
%	     April, 1994
%


%  N	Size of FFT to perform (assumes a 1 Hz FFT)
%  M	Number of FFTs to do  M must be > 1
%  fid  File handle for interleaved (real,imag) data, in subgroups
%       that are M complex points long (actually 2M real points).
%	Typically assigned using fid=fopen('tapwgts.dbg','r')
%  loop To save on memory, you can break up M into smaller numbers
%	M/loop and loop through them all.
%
%  The mean value of the Tap Weight Spectra can thus be found by plotting
% against the proper frequency axis (f=-8192:8192 for a 16K, 1Hz res FFT):
%  mn=mn ./ M*loop;
%  plot(f(1:16384),fftshift(abs(mn)))
%
%  for the case where N=16384 on a channel snapshot rate of 16,384 samples/sec.

pxx=ones(N,M);
pxx=pxx + j*pxx;
mn=zeros(1,N);

for count=1:loop
  for index=1:M
    tmp=fread(fid,[2*N 1],'float');
    data=tmp(1:2:length(tmp))+j*tmp(2:2:length(tmp));
    pxx(:,index)=fft(data,N);
  end
  mn=mn + sum(abs(pxx').^2);
  count
  save mn
end

