clear
[x, dt] = read_signal('data/Sa16.tx');
[n, m] = size(x);

t = [0:dt:(n-1)*dt]';

dxi = 2*pi/n/dt;
xi = [0:dxi:(n-1)*dxi]';

xi0 = pi/dt

X = abs(fft(x));

% search local maximum in first quarter (for small peak)
[max_value, max_index] = max(X([1:n/4], :));
omega1 = (max_index-1)*dxi

% search maximum in first half (for large peak)
[max_value, max_index] = max(X([1:n/2], :));
omega2 = (max_index-1)*dxi


% plot of the signal x
% plot(t, x);

% plot of the Fourier transformation of x
plot(xi, X);

% save graph in file for latex usage
% SaveX = [xi, X];
% save('data/X.graph', 'SaveX');




