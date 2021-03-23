% function plotatmos(data, x, mode)
% Plot atmospheric interference data with an inverse cdf, a power rayleigh
% x-scale (1e-6, 0.99) and a log y-scale (-40, 50).  
% 'data' is a one-dimensional complex array or (nbins)x2
% histgram array, i.e., [n x] = hist, data(:,2)=x, data(:,1)=n.
% 'mode' is string 'hst', 'cdf', or 'apd' ('hst' is default).
% examples:
%     plotatmos(data)
%     plotatmos(data, 'apd')
% The linear x-axis may be editted out of the PostScript file (search for '(0').
% r.coutts 9/24

function plotatmos(data, x, mode)

	if(nargin == 1)
		mode = 'hst'; % if fewer than 3 args, not 'cdf'
	end
	if(nargin == 2) % x = 'apd' or 'cdf'
		mode = x;
		if mode == 'cdf'
			x = data(:,2);
			data = data(:,1);
		end
	end
	% get dimension of data
	[rows cols] = size(data);

	if mode == 'cdf'
		invcdf = data(:,1); 	% inverse cdf
	elseif mode == 'apd' % mark's apd.dat files
		attr = 'g';					% attributes of curve 
		x = data(3:length(data),1);
		invcdf = data(3:length(data),2);
	elseif mode == 'hst'
		attr = 'y';				% attributes of curve 
		if(cols == 1) % complex input
			db = abs(data);		% get magnitude of complex number
			db = db(db > 0);	% excise zeros
			db = 20*log10(db);	% convert to dB space
			x = -40.5 : 1 : 49.5; % bin locations (-40.5 is overflow bin)
			[n x] = hist(db, x); % generate hist
		else % histogram input
			n = data(:,2)';		% first column is bin count
			x = data(:,1)';		% second column is bin location
		end
		npts = sum(n);			% total number of pts binned (include overflow)
		n = n(2:length(n));		% get rid of overflow bin
		x = x(2:length(x));		% get rid of overflow bin
		% generate inverse cdf
		for i = length(n):-1:1
			invcdf(i) = sum(n(i+1:length(n)))/npts;
		end
	end

	% sort out values not in range (1.0e-6, 0.99) and change scales
	mask = invcdf >= 1e-6 & invcdf <= 0.99;
	xlogln = (1.1404-log10(-log(invcdf(mask))))/3.1382;
	ylog = x(mask);

	% set up grid and tick marks
	xvert = [1e-4 .001 .01 .05 .1 .2 .3 .4 .5 .6 .7 .8 .9 .95 .98;
			 1e-4 .001 .01 .05 .1 .2 .3 .4 .5 .6 .7 .8 .9 .95 .98];
	xvert = (1.1404-log10(-log(xvert)))/3.1382;
	xticks = [0.001 0.05 0.2 0.4 0.6 0.8 0.9 0.95 0.98;
			 0.001 0.05 0.2 0.4 0.6 0.8 0.9 0.95 0.98];
	xticks = (1.1404-log10(-log(xticks)))/3.1382;
	yvert = [-40; 50];
	yticks = [-40; -38.5];
	xhoriz = [1e-6; 0.99];
	xhoriz = (1.1404-log10(-log(xhoriz)))/3.1382;
	yhoriz = [-40 -30 -20 -10 0 10 20 30 40 50; -40 -30 -20 -10 0 10 20 30 40 50];

	% square plot with a white dotted lines for grid and a solid line for data
	axis([0 1 -40 50]);
	axis('square'); % square plot
	plot(xvert, yvert, ':w', xhoriz, yhoriz, ':w', xticks, yticks, '-w', ...
		xlogln, ylog, attr);

	% put correct x-axis on screen (linear axis cannot be removed)
	xtext = [0.19 0.26 0.34 0.39 0.44 0.49 0.56 0.62 0.685 0.765 0.825];
	string = ['1e-4'; '0.1 '; '5   '; '20  '; '40  '; '60  '; '80  ';
			  '90  '; '95  '; '98  '; '99  '];
	ytext = [-.03 -.03 -.03 -.03 -.03 -.03 -.03 -.03 -.03 -.03 -.03];
	text(xtext, ytext, string, 'sc'); % plot text in screen coords

	% label
	xlabel('PERCENTAGE OF TIME ORDINATE IS EXCEEDED');
	ylabel('dB ABOVE Erms');
