function y = HM(mean_x, S_x, n, mu0, alpha_t)

    % H0 : Mx =   mu0
    % H1 : Mx =/= mu0

    % T = (mean_x - mu0)*sqrt(n/s) obeys student-t(n-1) distribution

    % therefore

    t_critical = -tinv(alpha_t/2, n-1);

    % thus Q = [-t_critical; t_critical]
    %      S = R \ Q

    % calculate T_empirical

    T_empirical = (mean_x - mu0)*sqrt(n/S_x);

    % check if T_empirical is in Q

    y = -t_critical < T_empirical && T_empirical < t_critical;

end