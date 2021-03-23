% function walkmesh(z, dim, linetype)
% walk through a mesh array, dim is the dimension ('m', or 'n'), linetype is
% 'g' or 'r*', etc.

function walkmesh(z, dim, linetype)

if nargin == 2
	linetype = 'r';
end

[m n] = size(z);			% get dimensions
big = max(max(z));			% get max
small = min(min(z));		% get min

if(dim == 'n')
	axis([1 m small big]);	% set axis
	for y = 1:n
		plot(z(:, y), linetype)
		tit = sprintf('%.0f of %.0f', y, n);
		title(tit);
		disp('hit [return]');
		pause
	end
end
if(dim == 'm')
	axis([1 n small big]);	% set axis
	for x = 1:m
		plot(z(x, :), linetype)
		tit = sprintf('%.0f of %.0f', x, m);
		title(tit);
		pause
	end
end
