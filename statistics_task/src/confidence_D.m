function [alpha1, beta1] = confidence_D(M, S, n, gamma_t)

    % Sxi obeys Pirson's chi^2 distribution

    % because chi^2 distribution isn't symmetric we should make assumption, that
    % to left and right of the interval probabilities are the same and equal (1 - gamma_t)/2:
    % -inf <---0.025---> chi1 <---gamma_t=0.95---> chi2 <---0.025---> +inf
    chi1 = chi2inv((1 - gamma_t)/2, n-1);
    chi2 = chi2inv(gamma_t + (1 - gamma_t)/2, n-1);

    % therefore

    alpha1 = (n-1)*S/chi2;
    beta1  = (n-1)*S/chi1;

end