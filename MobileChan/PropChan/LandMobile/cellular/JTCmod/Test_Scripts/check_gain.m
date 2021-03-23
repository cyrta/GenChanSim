function [pxx]=check_gain(fid,N,M,loop)

% check_gain.m takes as input the tapweight output files from the
% mobile-comm simulator 'mobilechan.c' (either the -o file or
% the debug file: 'tapwgts.dbg'. It then averages over a matrix
% of dimensions N,M.  This can be used to verify opertion of the simulator
% bulk path delay and relative path attenuation functions by inputting a
% period impulse train into the simulator with a period and bulk delay that 
% allows each path to appear between the input pulses.  
%
% Written by C. A. Nissen
%	     April, 1994
%
%
%
%  N    Period in samples of input seq.
%  M	Number of seq to average
%  fid  File handle for interleaved (real,imag) data, in subgroups
%       that are M complex points long (actually 2M real points).
%	Typically assigned using fid=fopen('tapwgts.dbg','r')
%  loop To save on memory, you can break up M into smaller numbers
%	M/loop and loop through them all.
% for the case where N=16384 on a channel snapshot rate of 16,384 samples/sec.
%
%pxx=ones(N,M);
%pxx=pxx + j*pxx;
%mn=zeros(1,N);

%for count=1:loop
% for index=1:M
%    tmp=fread(fid,[2*N 1],'float');
%    pxx(:,index)=tmp(1:2:length(tmp))+j*tmp(2:2:length(tmp));
%  end
%  mn=mn + sum(abs(pxx').^2);
%  count
%  save mn
%end


pxx=ones(N,M);
pxx=pxx + j*pxx;
mn=zeros(1,N);

  for index=1:M
    tmp=fread(fid,[2*N 1],'float');
    pxx(:,index)=tmp(1:2:length(tmp))+j*tmp(2:2:length(tmp));
  end

% Typically, Pxx can be plotted using:
%waterfall(10*log10(abs(pxx(:,1:1000)).^2))
% or
% plot(10*log10(abs(pxx(1,1:1000)').^2))
%
% Assuming 1:1000 provides enough resolution.  Be careful
% using waterfall with very large vectors as it can
% be extremely slow!!
