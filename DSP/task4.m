clear
[x, dt] = read_signal('data/Sa16.tx');
[n, m] = size(x);

dxi = 2*pi/n/dt;

t   = [0:dt :(n-1)*dt]';
xi  = [0:dxi:(n-1)*dxi]';

X = abs(fft(x));

SaveX = [xi, X];
save('data/pic6.graph', 'SaveX');

% and now we increase descrete step

dtau = 2*dt;

y = [];
for i=1:2:n
    y = [y; x(i)];
end
[n2, m2] = size(y);

tau = [0:dtau:(n2-1)*dtau]';
eta = [0:dxi :(n2-1)*dxi]';

xi0 = pi/dtau

Y = abs(fft(y));

SaveX = [eta, Y];
save('data/pic7.graph', 'SaveX');


