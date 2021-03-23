echo on
% gain in dB for each path
gdb1 = 0;
gdb2 = -3;
gdb3 = -6;
gdb4 = -1;
% generate delay power profile for scatter 1 path 1
tot_power = 0;
wt1=zeros(1,21);
for ii=1:21;
  wt1(ii) = exp(-1.0 * (ii-1)/(400e-9*10e6));
  tot_power = tot_power + wt1(ii);
end
for ii=1:21;
  wt1(ii) = wt1(ii)/tot_power;
end
% generate power profile for discrete 1 path 2
wt2 = 1;
% generate power profile for discrete 2 path 3
wt3 = 1;
% generate delay power profile for scatter 2 path 4
tot_power = 0;
wt4=zeros(1,19);
for ii=1:19;
  wt4(ii) = exp(-1.0 * (ii-1)/(350e-9*10e6));
  tot_power = tot_power + wt4(ii);
end
for ii=1:19;
  wt4(ii) = wt4(ii)/tot_power;
end
%
% compute gains for each path
% NOTE: normalize to two since input impulse has power=2 (I,Q) = (1,1)
gain1 = 10.0 .^ (gdb1/20.0);
gain2 = 10.0 .^ (gdb2/20.0);
gain3 = 10.0 .^ (gdb3/20.0);
gain4 = 10.0 .^ (gdb4/20.0);
gaintot = gain1 * gain1 + gain2 * gain2 + gain3 * gain3 + gain4 * gain4;
gaintot = sqrt(2/gaintot);
gain1 = gain1 * gaintot;
gain2 = gain2 * gaintot;
gain3 = gain3 * gaintot;
gain4 = gain4 * gaintot;
%
% adjust wieghts by the given gains 
%
for ii=1:21;
  wt1(ii) = wt1(ii) * gain1 * gain1;
end
wt2 = wt2 * gain2 * gain2;
wt3 = wt3 * gain3 * gain3;
for ii=1:19;
  wt4(ii) = wt4(ii) * gain4 * gain4;
end
%
% generate time arrays for each path
%
time1=10:30;
time1=(time1-1)*0.1;
time2=50;
time2=(time2-1)*0.1;
time3=63;
time3=(time3-1)*0.1;
time4=71:89;
time4=(time4-1)*0.1;

figure(1);
hold on
plot(time1,10*log10(wt1),'x');
axis([0 10 -40 0]);
plot(time2,10*log10(wt2),'x');
plot(time3,10*log10(wt3),'x');
plot(time4,10*log10(wt4),'x');
xlabel('microseconds');
ylabel('dB');
title('scatter/discrete delay power profile');
grid on;

mag1 = 0;
mag2 = wt2;
mag3 = wt3;
mag4 = 0;
for ii=1:21
   mag1=mag1 + wt1(ii);
end
for ii=1:19
   mag4=mag4 + wt4(ii);
end
% output magnitude numbers
[mag1,mag2,mag3,mag4,(mag1+mag2+mag3+mag4)]
[10*log10(mag1), 10*log10(mag2), 10*log10(mag3), 10*log10(mag4)]

