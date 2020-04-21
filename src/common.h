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

#ifndef COFFE_COMMON_H
#define COFFE_COMMON_H

#include <gsl/gsl_spline.h>
#include <gsl/gsl_spline2d.h>
#include <libconfig.h>

#ifndef COFFE_COPYRIGHT
#define COFFE_COPYRIGHT  \
                        "Copyright (C) 2019 Goran Jelic-Cizmek\n" \
                        "This is free software; see the source for copying conditions.\n" \
                        "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
                        "Report bugs to: goran.jelic-cizmek@unige.ch\n"
#endif


#ifndef COFFE_MAX_ALLOCABLE
#define COFFE_MAX_ALLOCABLE 1024 // largest allowed number of columns
#endif

#ifndef COFFE_MAX_STRLEN 
#define COFFE_MAX_STRLEN 256 // largest allowed length of the filename
#endif

#ifndef COFFE_TRUE
#define COFFE_TRUE 1
#endif

#ifndef COFFE_FALSE
#define COFFE_FALSE 0
#endif

#ifndef COFFE_LOGO
#define COFFE_LOGO \
        "   _____ ____  ______ ______ ______ \n" \
        "  / ____/ __ \\|  ____|  ____|  ____|\n" \
        " | |   | |  | | |__  | |__  | |__   \n" \
        " | |   | |  | |  __| |  __| |  __|  \n" \
        " | |___| |__| | |    | |    | |____ \n" \
        "  \\_____\\____/|_|    |_|    |______|\n"
#endif


#ifndef COFFE_MAX_INTSPACE
#define COFFE_MAX_INTSPACE 50000 // for 1D integration, the size of workspace
#endif

#ifndef COFFE_H0
#define COFFE_H0 (1./(2997.92458)) // H0 in units h/Mpc
#endif


/**
    simple wrapper with failsafe for malloc
**/

void *coffe_malloc(size_t len);

struct coffe_interpolation
{
    gsl_spline *spline;
    gsl_interp_accel *accel;
};

struct coffe_interpolation2d
{
    gsl_spline2d *spline;
    gsl_interp_accel *xaccel, *yaccel;
};

enum coffe_integral_type
{
    NONINTEGRATED, SINGLE_INTEGRATED, DOUBLE_INTEGRATED
};

enum coffe_output_type
{
    CORRFUNC, MULTIPOLES, AVERAGE_MULTIPOLES
};

/**
    contains all the values for n and l
    for which we need to compute the I^n_l;
    others are zero
**/

struct nl_terms
{
    int n, l;
};


/**
    structure with all of the correlation contributions
**/

struct coffe_correlation_contributions
{
    int den, rsd, len, d1, d2, g1, g2, g3, g4, g5;
};


/**
    contains all the parameters necessary to carry
    out the computation
**/

struct coffe_parameters_t
{
    int output_type; /* correlation function (angular or full sky), or multipoles, or redshift averaged multipoles */

    double *mu; /* value for the angles */

    int mu_len; /* number of angles */

    struct coffe_correlation_contributions correlation_contrib; /* all of the correlation contributions (internally) */

    struct nl_terms nonzero_terms[10]; /* contrains all of the integrals we need to compute */

    char **type_bg; /* background values to output */

    int type_bg_len; /* number of background values */

    int background_bins; /* number of bins for the background */

    int bessel_bins; /* number of bins for the bessel integrals */

    double Omega0_cdm; /* omega parameter for cold dark matter */

    double Omega0_baryon; /* omega parameter for baryons */

    double Omega0_gamma; /* omega parameter for photons */

    double w0; /* parameter for the equation of state of dark energy */

    double wa; /* parameter for the equation of state of dark energy */

    double Omega0_de; /* present omega parameter for dark energy-like component */

    double z_mean; /* mean redshift */

    double deltaz; /* width of redshift bin */

    char file_sep[COFFE_MAX_STRLEN]; /* string with name of file containing separations */

    double *sep; /* all the separations */

    size_t sep_len; /* number of separations */

    int integration_method;

    int integration_bins;

    int nthreads; /* how many threads are used for the computation */

    char file_power_spectrum[COFFE_MAX_STRLEN]; /* file containing the PS */

    struct coffe_interpolation power_spectrum;

    struct coffe_interpolation power_spectrum_norm;

    double k_min; /* min value to be taken from PS file */

    double k_max; /* max value to be taken from PS file */

    double k_min_norm; /* min value to be taken from PS file */

    double k_max_norm; /* max value to be taken from PS file */

    /* different biases */
    int read_matter_bias1, read_matter_bias2;

    char file_matter_bias1[COFFE_MAX_STRLEN], file_matter_bias2[COFFE_MAX_STRLEN];

    struct coffe_interpolation matter_bias1, matter_bias2;

    int read_magnification_bias1, read_magnification_bias2;

    char file_magnification_bias1[COFFE_MAX_STRLEN], file_magnification_bias2[COFFE_MAX_STRLEN];

    struct coffe_interpolation magnification_bias1, magnification_bias2;

    int read_evolution_bias1, read_evolution_bias2;

    char file_evolution_bias1[COFFE_MAX_STRLEN], file_evolution_bias2[COFFE_MAX_STRLEN];

    struct coffe_interpolation evolution_bias1, evolution_bias2;

    int divergent; /* flag for divergent integrals */

    config_t *conf; /* contains all the settings */

    char timestamp[COFFE_MAX_STRLEN]; /* when the calculation started */

    char output_path[COFFE_MAX_STRLEN]; /* output directory for all the files */

    char output_prefix[COFFE_MAX_STRLEN]; /* output prefix for all the files */

    int interp_method; /* method used for interpolation (linear, poly, etc.) */

    int *multipole_values; /* the multipoles to calculate */

    int multipole_values_len;

    double *covariance_z_mean;

    int covariance_z_mean_len;

    double *covariance_deltaz;

    int covariance_deltaz_len;

    double *covariance_zmin;

    int covariance_zmin_len;

    double *covariance_zmax;

    int covariance_zmax_len;

    double *covariance_fsky;

    int covariance_fsky_len;

    double *covariance_density;

    int covariance_density_len;

    double covariance_pixelsize;

    double covariance_minimum_separation;

    int covariance_integration_method;

    int covariance_integration_bins;

    int covariance_interpolation_method;

    int have_window;

    double window_size;

    /* for redshift averaged multipoles */
    double z_min, z_max;

    int theta_len;

    int flatsky;

    int verbose;

#ifdef HAVE_CLASS
    /* stuff for CLASS only */

    int have_class; /* should CLASS be used or not? */

    double n_s; /* spectral index n_s */

    double ln_10_pow_10_A_s; /* amplitude of primordial power spectrum A_s */

    double h; /* Damn you, little h! */

    double k_pivot; /* for CLASS */

#endif

};

char *coffe_get_time(void);


/**
    auxiliary functions that may or may not be useful
**/

int read_1col(
    const char *filename,
    double **values,
    size_t *len
);

int read_2col(
    const char *filename,
    double **values1,
    double **values2,
    size_t *len
);

int write_1col(
    const char *filename,
    double *values,
    size_t len,
    const char *header
);

int write_2col(
    const char *filename,
    const double *values1,
    const double *values2,
    size_t len,
    const char *header,
    const char *sep
);

int write_ncol(
    size_t ncolumns,
    const char *filename,
    size_t len,
    const char *header,
    const char *sep,
    const double *values,
    ...
);

int write_ncol_null(
    const char *filename,
    size_t len,
    const char *header,
    const char *sep,
    const double *values,
    ...
);

int alloc_double_matrix(
    double ***values,
    size_t len1,
    size_t len2
);

int free_double_matrix(
    double ***values,
    size_t len
);

int write_matrix(
    const char *filename,
    const double **values,
    size_t len1,
    size_t len2,
    const char *header,
    const char *sep
);

int copy_matrix_array(
    double **destination,
    const double **source,
    size_t rows,
    size_t columns,
    size_t index,
    const char *type
);

int coffe_init_spline(
    struct coffe_interpolation *interp,
    const double *xi,
    const double *yi,
    const size_t bins,
    const int interpolation_type
);

double coffe_interp_spline(
    const struct coffe_interpolation *interp,
    double value
);

int coffe_free_spline(
    struct coffe_interpolation *interp
);

int coffe_compare_ascending(
    const void *a,
    const void *b
);

int coffe_compare_descending(
    const void *a,
    const void *b
);

double coffe_dark_energy_eos(
    const struct coffe_parameters_t *par,
    double z
);

int coffe_parameters_free(
    struct coffe_parameters_t *par
);

double coffe_resolution_window(
    double x
);

int read_ncol(
    const char *filename,
    const size_t N,
    size_t *len,
    double **values,
    ...
);

#endif
