/*
 * This file is part of COFFE
 * Copyright (C) 2019 Goran Jelic-Cizmek
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <gsl/gsl_spline2d.h>
#include <gsl/gsl_errno.h>

#include "tanhsinh.h"

#ifdef HAVE_CUBA
#include "cuba.h"
#else
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "common.h"
#include "background.h"
#include "integrals.h"
#include "functions.h"
#include "corrfunc.h"
#include "signal.h"


/**
    computes and stores the values of the correlation
    function
**/

int coffe_corrfunc_init(
    coffe_parameters_t *par,
    coffe_background_t *bg,
    coffe_integral_array_t *integral,
    coffe_corrfunc_array_t *corrfunc
)
{
    coffe_corrfunc_free(corrfunc);

    clock_t start, end;
    start = clock();

    if (par->verbose)
        printf("Calculating the correlation function...\n");

    gsl_error_handler_t *default_handler =
        gsl_set_error_handler_off();

    corrfunc->size = par->mu_len
        * par->z_mean_len
        * par->sep_len;

    corrfunc->array = (coffe_corrfunc_t *)coffe_malloc(
        sizeof(coffe_corrfunc_t) * corrfunc->size
    );

    {
    size_t counter = 0;
    for (size_t i = 0; i < par->z_mean_len; ++i){
    for (size_t j = 0; j < par->mu_len; ++j){
    for (size_t k = 0; k < par->sep_len; ++k){
        corrfunc->array[counter].coords.z_mean = par->z_mean[i];
        corrfunc->array[counter].coords.mu = par->mu[j];
        corrfunc->array[counter].coords.separation = par->sep[k];
        ++counter;
    }}}
    }

    #pragma omp parallel for num_threads(par->nthreads)
    for (size_t i = 0; i < corrfunc->size; ++i){
        corrfunc->array[i].value = coffe_integrate(
            par, bg, integral,
            corrfunc->array[i].coords.z_mean,
            corrfunc->array[i].coords.separation,
            corrfunc->array[i].coords.mu,
            0,
            NONINTEGRATED, CORRFUNC
        );
    }

    #pragma omp parallel for num_threads(par->nthreads)
    for (size_t i = 0; i < corrfunc->size; ++i){
        corrfunc->array[i].value += coffe_integrate(
            par, bg, integral,
            corrfunc->array[i].coords.z_mean,
            corrfunc->array[i].coords.separation,
            corrfunc->array[i].coords.mu,
            0,
            SINGLE_INTEGRATED, CORRFUNC
        );
    }

    #pragma omp parallel for num_threads(par->nthreads)
    for (size_t i = 0; i < corrfunc->size; ++i){
        corrfunc->array[i].value += coffe_integrate(
            par, bg, integral,
            corrfunc->array[i].coords.z_mean,
            corrfunc->array[i].coords.separation,
            corrfunc->array[i].coords.mu,
            0,
            DOUBLE_INTEGRATED, CORRFUNC
        );
    }

    end = clock();

    if (par->verbose)
        printf("Correlation function calculated in %.2f s\n",
            (double)(end - start) / CLOCKS_PER_SEC);

    gsl_set_error_handler(default_handler);

    return EXIT_SUCCESS;
}

int coffe_corrfunc_free(
    coffe_corrfunc_array_t *cf
)
{
    if (cf->size)
        free(cf->array);
    cf->array = NULL;
    cf->size = 0;
    return EXIT_SUCCESS;
}
