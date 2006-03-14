/** \file apop_logit.c

Copyright (c) 2005 by Ben Klemens. Licensed under the GNU GPL version 2.
*/
#include "model.h"



//The default list. You probably don't need them all.
#include "types.h"
#include "conversions.h"
#include "likelihoods.h"
#include "model.h"
#include "linear_algebra.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_permutation.h>
#include <stdio.h>
#include <assert.h>


/* 
This is an MLE, so this is a one-liner.
*/
static apop_estimate * logit_estimate(apop_data * data,  void *parameters){
	return apop_maximum_likelihood(data,  apop_logit, parameters);
}


/*For the sake of the fdf function, we keep xdotbeta global.
  */

gsl_vector  *xdotbeta;
int         calculate_xdotbeta  = 1;
int         keep_xdotbeta       = 0;

/*



*/
static double logit_log_likelihood(const gsl_vector *beta, void *d){
size_t	    i;
double	    loglike 	= 0,
            xb;
gsl_matrix 	*data 		= d;		//type cast void to gsl_matrix.
gsl_matrix  independent = gsl_matrix_submatrix(data, 0,1, 
                                    data->size1, data->size2 -1).matrix;
    if (calculate_xdotbeta){
        xdotbeta    = gsl_vector_calloc(data->size1);
        gsl_blas_dgemv(CblasNoTrans, 1.0, &independent, beta, 0, xdotbeta);
        }
	for(i=0;i< data->size1; i++){
        xb    = gsl_vector_get(xdotbeta, i);
		if (gsl_matrix_get(data, i, 0))
            loglike   += xb/(1.+xb);
        else
            loglike   += 1./(1.+xb);
	}
    if (!keep_xdotbeta)
        gsl_vector_free(xdotbeta);
	return loglike;
}

/* The derivative of the logit distribution, for use in likelihood
  minimization. 
  The format is often the same as above: go line by line through a gsl_matrix.
  The sample is a three-dimensional parameter vector.
 */
static void logit_dlog_likelihood(const gsl_vector *beta, void *d, gsl_vector *gradient){
int		    i,j;
double	    dtotal[3];
gsl_matrix 	*data 	= d;		//type cast
    dtotal[0]  = 0,
    dtotal[1]  = 0,
    dtotal[2]  = 0;
	for(i=0; i< data->size1; i++){
		dtotal[0]  += 0; //PLACE MATH HERE
		dtotal[1]  += 0; //PLACE MATH HERE
		dtotal[2]  += 0; //PLACE MATH HERE
	}
	for(j=0; j< beta->size; j++){
	    gsl_vector_set(gradient,j,dtotal[j]);
    }
}


/** 
  Simple, but some trickery to keep xdotbeta. Notice that the two switches
  leave the function with the same values with which they came in.

	*/
static void logit_fdf( const gsl_vector *beta, void *d, double *f, gsl_vector *df){
    keep_xdotbeta       = 1;
	*f	= logit_log_likelihood(beta, d);
    calculate_xdotbeta  = 0;
    keep_xdotbeta       = 0;
	logit_dlog_likelihood(beta, d, df);
    calculate_xdotbeta  = 1;
}


/* For making random draws from your model given the parameters.
You can delete this function entirely if so inclined. If so, remember
to replace this function with NULL in the model definition below.
*/
static double logit_rng(gsl_rng* r, gsl_vector * a){
    //place math here.
    return 0;
}

/** The binary logit model.
 The first column of the data matrix this model expects is ones and zeros;
 the remaining columns are values of the independent variables. Thus,
 the model will return (data columns)-1 parameters.

\ingroup models
*/
apop_model apop_logit = {"logit", -1, 
{
	1,	//parameters
	1,	//covariance
	1,	//confidence
	0,	//dependent
	0,	//predicted
	1,	//log_likelihood
	1	//names;
}, 
	logit_estimate, 
    logit_log_likelihood, 
   NULL,// logit_dlog_likelihood, 
   NULL,// logit_fdf, 
   NULL,// logit_constraint, 
   NULL};// logit_rng};