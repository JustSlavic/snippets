function [signal, dt] = read_signal(filename)
    v = load(filename);
    [n, m] = size(v);
    dt = v(1);
    signal = v([2:n], :);
end