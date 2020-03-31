function [alpha1, beta1] = confidence_M(mean_x, S_x, n, gamma_t)

    % Mxi obeys student-t(n-1) distribution

    % because student-t distribution is symmetric:
    % -inf <---0.025---> -t_gamma <---gamma_t=0.95---> t_gamma <---0.025---> +inf
    t_gamma = tinv(gamma_t + (1 - gamma_t)/2, n-1);
    % the same as t_gamma = - tinv((1 - gamma_t)/2, n-1);

    % therefore

    alpha1 = mean_x - (t_gamma*sqrt(S_x))/sqrt(n);
    beta1  = mean_x + (t_gamma*sqrt(S_x))/sqrt(n);

end