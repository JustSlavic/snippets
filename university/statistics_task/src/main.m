x = load('../data/ncalls.data');
y = load('../data/durationcalls.data');

n = size(x)(1);

[mean_x, S_x, hist1] = get_data(x, 10)
[mean_y, S_y, hist2] = get_data(y, 10)

save('../data/hist1.data', 'hist1');
save('../data/hist2.data', 'hist2');

% counting confidence interval

% set confidence to 0.95
gamma_t = 0.95 % gamma is confidence of being Mx or Dx in confidence interval
alpha_s = 0.01 % alpha_significance

% finding confidence intervals for Mx and Dx
[M_a1, M_b1] = confidence_M(mean_x, S_x, n, gamma_t)
[D_a1, D_b1] = confidence_D(mean_x, S_x, n, gamma_t)

% check hypothesis on Mx = mu0
mu0_x = round(mean_x)
H0m1 = HM(mean_x, S_x, n, mu0_x, alpha_s)

% check hypothesis on Dx = sigma0
sigma0_x = round(S_x)
H0s1 = HD(S_x, n, sigma0_x, alpha_s)

%----------------------------------------------------

% finding confidence intervals for My and Dy
[M_a2, M_b2] = confidence_M(mean_y, S_y, n, gamma_t)
[D_a2, D_b2] = confidence_D(mean_y, S_y, n, gamma_t)

% check hypothesis on My = mu0
mu0_y = round(mean_y)
H0m2 = HM(mean_y, S_y, n, mu0_y, alpha_s)

% check hypothesis on Dy = sigma0
sigma0_y = round(S_y)
H0s2 = HD(S_y, n, sigma0_y, alpha_s)

