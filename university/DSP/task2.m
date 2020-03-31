clear
[x, dt] = read_signal('data/Sa16.tx');
[n, m] = size(x);

dxi = 2*pi/n/dt;
xi = [0:dxi:(n-1)*dxi]';

% generate noise of amplitude A
A = 2.6
noise = unifrnd(-A, A, n, m);

y = x + noise;
Y = abs(fft(y));

% plot of the Fourier transformation of y
plot(xi, Y);

% save graph in file for latex usage
% SaveX = [xi, Y];
% save('data/XA28.graph', 'SaveX');


