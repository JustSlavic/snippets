function [M, S, histogram] = get_data(x, k)
    n = size(x)(1);

    x_min = min(x);
    x_max = max(x);

    M = mean(x);
    S = sum(x.^2)/(n-1) - n*M^2/(n-1);

    delta = (x_max - x_min) / k;
    ak = [x_min:delta:x_max];

    hst = histc(x, ak)./n;
    hst(k) += hst(k+1);
    hst(k+1) = 0;

    histogram = [ak', hst];
end
