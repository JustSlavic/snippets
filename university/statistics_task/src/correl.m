x = load('../data/ncalls.data');
y = load('../data/durationcalls.data');

n = size(x)(1);

z = [x, y];

a = [1:2:21];
b = [1.8:(8.51-1.8):68.9];

m = zeros(10,10);

for k = 1:n
    for i=1:9
        for j=1:9
            if (z(k,1) >= a(i) && z(k,1) < a(i+1) && z(k,2) >= b(j) && z(k,2) < b(j+1))
                m(j,i)++;
            end
        end
    end
end

for k = 1:n
    for i=1:9
        if (z(k,1) >= a(i) && z(k,1) < a(i+1) && z(k,2) >= b(10) && z(k,2) <= b(11))
            m(10,i)++;
        end
    end
end

for k = 1:n
    for i=1:9
        if (z(k,1) >= a(10) && z(k,1) <= a(11) && z(k,2) >= b(i) && z(k,2) < b(i+1))
            m(i,10)++;
        end
    end
end

for k = 1:n
    if (z(k,1) >= a(10) && z(k,1) <= a(11) && z(k,2) >= b(10) && z(k,2) <= b(11))
        m(10,10)++;
    end
end    

a11 = sum(x.*y)/n;

r = (a11 - mean(x)*mean(y))/(var(x)*var(y))
        


