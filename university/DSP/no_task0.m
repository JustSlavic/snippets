clear

[x, dt] = read_signal('data/Sa16.tx');
[n, m] = size(x);

t = [0:dt:(n-1)*dt]';

dxi = 2*pi/n/dt;

xi = [0:dxi:(n-1)*dxi]';

X = abs(fft(x));

% plot(t, x);

% plot(xi, X);

for i = 1:n/4
    X(i) = 0;
end

for i = 3*n/4:n
    X(i) = 0;
end

% plot(xi, X);

y = ifft(X);

plot(t, y);
