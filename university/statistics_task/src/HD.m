function y = HD(S_x, n, sigma0, alpha_s)

    % H0 : Dx = mu0
    % H1 : Dx > mu0 ???

    % T = (n - 1)*s^2/sigma0^2 obeys chi^2(n-1) distribution

    % therefore

    chi_critical = chi2inv(1 - alpha_s, n-1);

    % thus Q = [0; chi_critical]
    %      S = R+ \ Q

    % calculate T_empirical

    T_empirical = (n - 1)*S_x/sigma0^2;

    % check if T_empirical is in Q

    y = T_empirical < chi_critical;

end