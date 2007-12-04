//likelihoods.h	Copyright (c) 2005--2007 by Ben Klemens.  Licensed under the modified GNU GPL v2; see COPYING and COPYING2. 

#ifndef apop_likelihoods_h
#define  apop_likelihoods_h

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_siman.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_psi.h>
#include "stats.h"
#include "output.h"
#include "conversions.h"
#include "linear_algebra.h"
#include <apophenia/model.h>
#include <apophenia/types.h>
#include <apophenia/regression.h>

#define MAX_ITERATIONS 		5000
#define MAX_ITERATIONS_w_d	5000

#undef __BEGIN_DECLS    /* extern "C" stuff cut 'n' pasted from the GSL. */
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

typedef enum {
    APOP_SIMPLEX_NM     =0, // 0: Nelder-Mead simplex (gradient handling rule is irrelevant)
    APOP_CG_FR     =1,      //  1: conjugate gradient (Fletcher-Reeves) (default)
    APOP_CG_BFGS   =2,      //  2: conjugate gradient (BFGS: Broyden-Fletcher-Goldfarb-Shanno)
    APOP_CG_PR     =3,      //  3: conjugate gradient (Polak-Ribiere)
    APOP_SIMAN      =5,         //  5: \ref simanneal "simulated annealing"
    APOP_RF_NEWTON  =10,        //  10: Find a root of the derivative via Newton's method
//    APOP_RF_BROYDEN =11,        //  11: Find a root of the derivative via the Broyden Algorithm
    APOP_RF_HYBRID  =12,        //  12: Find a root of the derivative via the Hybrid method
    APOP_RF_HYBRID_NOSCALE  =13 //  13: Find a root of the derivative via the Hybrid method; no internal scaling
} apop_optimization_enum;

typedef struct{
//traditional
    double      *starting_pt;
    double      step_size;
    double      tolerance;
    double      delta;
    apop_optimization_enum method;
    int         verbose;
    int         want_cov;
//simulated annealing (also uses step_size);
    int         n_tries;
    int         use_score;
    int         iters_fixed_T;
    double      k, t_initial, mu_t, t_min ;
    gsl_rng     *rng;
    apop_model  *model;
    char        trace_path[1000];
} apop_mle_settings;


/*
For the Probit, the first column of the data matrix is the dependent
variable, and the remaining variables are the independent. This means
that the beta which will be output will be of size (data->size2 - 1).

For the Waring, Yule, and Zipf estimates, each row of the data matrix
is one observation. The first column is the number of elements with one
link, the second is the number of elements with two links, et cetera.

If you want the total likelihood, likelihood should be a double*; else,
send in NULL and get nothing back.

starting_pt is a vector of the appropriate size which indicates your
best initial guess for beta. if starting_pt=NULL, then (0,0,...0) will
be assumed.

step_size is the scale of the initial steps the maximization algorithm
will take. Currently, it is a scalar, so every dimension will have the
same step_size.

verbose is zero or one depending on whether you want to see the
maximizer's iterations.

For each function, the return value is the vector of most likely parameters.
*/
apop_mle_settings *apop_mle_settings_alloc(apop_data*, apop_model);

void apop_make_likelihood_vector(gsl_matrix *m, gsl_vector **v, apop_model dist, gsl_vector* fn_beta);
/*This function goes row by row through m and calculates the likelihood
  of the given row, putting the result in v. You will need this to find
  the variance of the estimator via some means.
  The likelihood function can be any of &apop_xxx_likelihood from above, 
  and fn_beta will probably be the beta calculated using the corresponding
  apop_xxx_mle function.
  */
typedef double 	(*apop_fn_with_params) (const apop_data *, apop_model *);
gsl_vector * apop_numerical_gradient(apop_data *data, apop_model*);
gsl_matrix * apop_numerical_second_derivative(apop_model dist, gsl_vector *beta, apop_data * d);
gsl_matrix * apop_numerical_hessian(apop_model dist, gsl_vector *beta, apop_data * d);

/* Find the var/covar matrix via the hessian. */
//void apop_numerical_covariance_matrix(apop_model dist, apop_model *est, apop_data *data);
//void apop_numerical_var_covar_matrix(apop_model dist, apop_model *est, apop_data *data);


apop_model *	apop_maximum_likelihood(apop_data * data, apop_model dist);


    //This is a global var for numerical differentiation.
extern double (*apop_fn_for_derivative) (const gsl_vector *beta, void *d);

apop_model * apop_estimate_restart (apop_model *, apop_model *);

//in apop_linear_constraint.c
double  apop_linear_constraint(gsl_vector *beta, apop_data * constraint, double margin);


//in apop_model_fix_params.c
apop_mle_settings *apop_model_fix_params(apop_data *data, apop_data *paramvals, apop_data *mask, apop_model model_in);
__END_DECLS
#endif
