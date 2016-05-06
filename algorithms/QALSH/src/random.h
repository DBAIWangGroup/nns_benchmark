#ifndef __RANDOM_H
#define __RANDOM_H


// -----------------------------------------------------------------------------
//  Functions used for generating random variables (r.v.).
// -----------------------------------------------------------------------------
float uniform(						// r.v. from Uniform(min, max)
	float min,							// min value
	float max);							// max value

// -----------------------------------------------------------------------------
float gaussian(						// r.v. from Gaussian(mean, sigma)
	float mu,							// mean (location)
	float sigma);						// stanard deviation (scale > 0)

// -----------------------------------------------------------------------------
//  Functions used for calculating probability distribution function (pdf) and 
//  cumulative distribution function (cdf).
// -----------------------------------------------------------------------------
float gaussian_pdf(					// pdf of N(0, 1)
	float x);							// variable

// -----------------------------------------------------------------------------
float gaussian_cdf(					// cdf of N(0, 1) in range (-inf, x]
	float x,							// integral border
	float step = 0.001f);				// step increment

// -----------------------------------------------------------------------------
float new_gaussian_cdf(				// cdf of N(0, 1) in range [-x, x]
	float x,							// integral border (x > 0)
	float step = 0.001f);				// step increment


// -----------------------------------------------------------------------------
//  query-oblivious and query-aware collision probability under gaussian
//  distribution, cauchy distribution and levy distribution
// -----------------------------------------------------------------------------
float orig_gaussian_prob(			// calc original gaussian probability
	float x);							// x = w / r

// -----------------------------------------------------------------------------
float new_gaussian_prob(			// calc new gaussian probability
	float x);							// x = w / (2 * r)

// -----------------------------------------------------------------------------
//  probability vs. w for a fixed ratio c
// -----------------------------------------------------------------------------
void prob_of_gaussian();			// curve of p1, p2 vs. w under gaussian

// -----------------------------------------------------------------------------
//  the difference (p1 - p2) vs. w for a fixed ratio
// -----------------------------------------------------------------------------
void diff_prob_of_gaussian();		// curve of p1 - p2 vs. w under gaussian

// -----------------------------------------------------------------------------
//  rho = log(1/p1) / log(1/p2) vs. w for a fixed ratio c
// -----------------------------------------------------------------------------
void rho_of_gaussian();				// curve of rho vs. w under gaussian


#endif
