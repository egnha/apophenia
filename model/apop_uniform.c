/* apop_uniform.c 
 Copyright (c) 2007, 2009 by Ben Klemens.  Licensed under the modified GNU GPL v2; see COPYING and COPYING2.  */

#include "apop_internal.h"

/* \amodel apop_uniform  This is the two-parameter version of the Uniform,
expressing a uniform distribution over [a, b].

The MLE of this distribution is simply a = min(your data); b = max(your data).
Primarily useful for the RNG, such as when you have a Uniform prior model.

\adoc    Input_format  An unordered set of numbers in the data set's vector, matrix, or both.
\adoc    Parameter_format  Zeroth vector element is \f$a\f$, the min;
                          element one is \f$b\f$, the max. 
\adoc    settings  None.    
      */

static void getminmax(apop_data *d, double *min, double *max){
    Get_vmsizes(d) //msize1, vsize
    *min = GSL_MIN(msize1 ? gsl_matrix_min(d->matrix) : GSL_POSINF,
                    vsize ? gsl_vector_min(d->vector) : GSL_POSINF);
    *max = GSL_MAX(msize1 ? gsl_matrix_max(d->matrix) : GSL_NEGINF,
                    vsize ? gsl_vector_max(d->vector) : GSL_NEGINF);
}

static double unif_ll(apop_data *d, apop_model *m){
  Nullcheck_mpd(d, m);
  Get_vmsizes(d) //tsize
  double min, max;
    getminmax(d, &min, &max);
    if (min> m->parameters->vector->data[0] && max < m->parameters->vector->data[1])
        return -log(m->parameters->vector->data[1] - m->parameters->vector->data[0]) * tsize;
    return GSL_NEGINF;
}

static double unif_p(apop_data *d, apop_model *m){
  Nullcheck_mpd(d, m);
  Get_vmsizes(d) //tsize
  double min, max;
    getminmax(d, &min, &max);
    if (min> m->parameters->vector->data[0] && max< m->parameters->vector->data[1])
        return pow(m->parameters->vector->data[1] - m->parameters->vector->data[0], -tsize);
    return 0;
}

/* \adoc estimated_info   Reports <tt>log likelihood</tt>. */
static apop_model * uniform_estimate(apop_data * data,  apop_model *est){
    Nullcheck_d(data);
    apop_name_add(est->parameters->names, "min", 'r');
    apop_name_add(est->parameters->names, "max", 'r');
    getminmax(data, est->parameters->vector->data+0, est->parameters->vector->data+1);
    apop_data_add_named_elmt(est->info, "log likelihood", unif_ll(data, est));
    return est;
}

static double unif_cdf(apop_data *d, apop_model *m){
  Nullcheck_mpd(d, m);
  Get_vmsizes(d) //tsize
    double min = m->parameters->vector->data[0];
    double max = m->parameters->vector->data[1];
    double val = apop_data_get(d, 0, vsize ? -1: 0);
    if (val <= min)
        return 0;
    if (val >=max)
        return 1;
    return (val-min)/(max-min);
}

static void uniform_rng(double *out, gsl_rng *r, apop_model* eps){
    *out =  gsl_rng_uniform(r) *(eps->parameters->vector->data[1]- eps->parameters->vector->data[0])+ eps->parameters->vector->data[0];
}

apop_model apop_uniform = {"Uniform distribution", 2, 0, 0,  .dsize=1,
    .estimate = uniform_estimate,  .p = unif_p,.log_likelihood = unif_ll,   
    .draw = uniform_rng, .cdf = unif_cdf};

/* \amodel apop_improper_uniform The improper uniform returns \f$P(x) = 1\f$ for every value of x, all the
time (and thus, log likelihood(x)=0).  It has zero parameters. It is
useful, for example, as an input to Bayesian updating, to represent a
fully neutral prior.

\adoc    Input_format      Ignored.
\adoc    Parameter_format  \c NULL 
\adoc    estimated_parameters   \c NULL
\adoc    RNG The \c draw function makes no sense, and therefore halts if you call it.
\adoc    CDF Half of the distribution is less than every given point, so the CDF always
             returns 0.5. One could perhaps make an argument that this should really be
             infinity, but a half is more in the spirit of the distribution's
             use to represent a lack of information. 
\adoc    settings None. 
          */

static apop_model * improper_uniform_estimate(apop_data * data,  apop_model *m){ return m; }

static double improper_unif_ll(apop_data *d, apop_model *m){ return 0; }
static double improper_unif_cdf(apop_data *d, apop_model *m){ return 0.5; }
static double improper_unif_p (apop_data *d, apop_model *m){ return 1; }

static void improper_uniform_rng(double *out, gsl_rng *r, apop_model* eps){
    apop_assert_s(0, "It doesn't make sense to make random draws from an improper Uniform.");
}

apop_model apop_improper_uniform = {"Improper uniform distribution", 2, 0, 0,  .dsize=1,
    .estimate = improper_uniform_estimate,  .p = improper_unif_p,
    .log_likelihood = improper_unif_ll,  .draw = improper_uniform_rng,
    .cdf = improper_unif_cdf};
