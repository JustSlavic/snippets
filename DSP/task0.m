clear 

function t = x(n)
    finite_area = [0.1, 2.1, -1.9, 4.1, -1.9, 5.1, -1.9, -1.9, 2.1, 3.1, 0.1];
    %              -1 0  1  2  3  4  5  6  7  8  9
    if (n < -1 || n > 9)
        t = 0;
    else
        t = finite_area(n+2);
    end
end

y = [ 0, 0, 0];
%     1  2  3  4
%    -4 -3 -2  -1

m = 1005

for i=-1:m
    y = [y, (5/3*y(i+4) - 1/3*y(i+3) - 1/3*y(i+2) + x(i) - x(i-1) + 2*x(i-2))];
end

[[-4:m]', y']




