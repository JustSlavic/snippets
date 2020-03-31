clear
[x, dt] = read_signal('data/Sa16.tx');
[n, m] = size(x);

t = [0:dt:(n-1)*dt]';

dxi = 2*pi/n/dt;
xi = [0:dxi:(n-1)*dxi]';

% define signal as function
function y=S(t, a, w1, w2)
    y = a*cos(w1*t) + 2*a*cos(w2*t);
end

% descrete signal
A = 0.1
omega1 = 2.6;
omega2 = 3.1;

y = S(t, A, omega1, omega2);
y += x;
Y = abs(fft(y));

% this plots signal y = x + (a*cos + 2*a*cos)
% plot(t, y);

% Fourier transformation of y
plot(xi, Y);

% SaveX = [xi, Y];
% save('data/Y1.graph', 'SaveX');
