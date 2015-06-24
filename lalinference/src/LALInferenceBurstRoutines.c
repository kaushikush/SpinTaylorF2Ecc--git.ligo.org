/*
 *  LALInferenceBurst.c: 
 *  Contains burst specific routines (used by LIB)
 *  They are here rather than in lalsimulation or lalburst so that they won't inferfeere with other people work
 *  They may be migrated once proven to be fully reliable.
 *  I (salvo) will open redmine tickets for all this changes and move the relative functions from here to there
 *  once each is accepted. 
 *
 *  Copyright (C) 2015 Salvatore Vitale
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with with program; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 */

#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <lal/LALSimBurst.h>
#include <lal/LALConstants.h>
#include <lal/FrequencySeries.h>
#include <lal/Sequence.h>
#include <lal/TimeFreqFFT.h>
#include <lal/TimeSeries.h>
#include <lal/RealFFT.h>
#include <lal/Units.h>
#include <lal/Date.h>
#include <lal/LALInferenceBurstRoutines.h>

#define LAL_PI_1_2      1.7724538509055160272981674833411451 /* sqrt of PI */
#define LAL_PI_1_4      1.3313353638003897127975349179502808 /* PI^1/4 */
#define LAL_4RT2        1.1892071150027210667174999705604759  /* 2^(1/4) */
#define FRTH_2_Pi       0.8932438417380023314010427521746490  /* (2/Pi)^(1/4)*/
#define FRTH_2_times_PI 1.5832334870861595385799030344545584  /* (2*Pi)^(1/4)*/

int XLALGetBurstApproximantFromString(const CHAR *inString)
{
#ifndef LAL_NDEBUG
  if ( !inString )
    XLAL_ERROR( XLAL_EFAULT );
#endif
  if ( strstr(inString, "SineGaussianF" ) )
  {
    return SineGaussianF;
  }
  else if ( strstr(inString, "SineGaussian" ) )
  {
    return SineGaussian;
  }
  else if ( strstr(inString, "GaussianF" ) )
  {
    return GaussianF;
  }
  else if ( strstr(inString, "Gaussian" ) )
  {
    return Gaussian;
  }
  else if ( strstr(inString, "DampedSinusoidF" ) )
  {
    return DampedSinusoidF;
  }
  else if ( strstr(inString, "DampedSinusoid" ) )
  {
    return DampedSinusoid;
  }
  else
  {
    XLALPrintError( "Cannot parse burst approximant from string: %s \n", inString );
    XLAL_ERROR( XLAL_EINVAL );
  }
}

/* FIXME ORDER*/
int XLALCheckBurstApproximantFromString(const CHAR *inString)
{
#ifndef LAL_NDEBUG
  if ( !inString )
    XLAL_ERROR( XLAL_EFAULT );
#endif
  if ( strstr(inString, "Gaussian" ) )
    return 1;
  else if ( strstr(inString, "GaussianF" ) )
    return 1;
  else if ( strstr(inString, "SineGaussian" ) )
    return 1;
  else if ( strstr(inString, "SineGaussianF" ) )
    return 1;
  else if ( strstr(inString, "DampedSinusoid" ) )
    return 1;
  else if ( strstr(inString, "DampedSinusoidF" ) )
    return 1;
  else if (strstr(inString,"RingdownF") )
    return 1;
  else
    return 0;
}

int XLALSimBurstImplementedTDApproximants(
    BurstApproximant approximant /**< Burst approximant (see enum in LALSimBurst.h) */
    )
{
    switch (approximant)
    {
        case SineGaussian:
        case Gaussian:
        case DampedSinusoid:
            return 1;

        default:
            return 0;
    }
}

/**
 * Checks whether the given approximant is implemented in lalsimulation's XLALSimInspiralChooseFDWaveform().
 *
 * returns 1 if the approximant is implemented, 0 otherwise.
 */
int XLALSimBurstImplementedFDApproximants(
    BurstApproximant approximant /**< Burst approximant (see enum in LALSimBurst.h) */
    )
{
    switch (approximant)
    {
        case SineGaussianF:
        case RingdownF:
        case DampedSinusoidF:
        case GaussianF:
            return 1;
        default:
            return 0;
    }
}

/* Tentative common interface to burst FD WF. Pass all standard burst parameters (as in sim_burst table). 
 * Parameters which are not defined for the WF of interest will be ignored.
 * Unconventional parameters can be passed through extraParams 
 * 
 * Returned waveforms are centered at t=0, thus must be time shifted to wanted time.
 * No taper, windowing, etc, is applied. The caller must take care of that.
 * 
 * */
int XLALSimBurstChooseFDWaveform(
    COMPLEX16FrequencySeries **hptilde,     /**< FD plus polarization */
    COMPLEX16FrequencySeries **hctilde,     /**< FD cross polarization */
    REAL8 deltaF,                           /**< sampling interval (Hz) */
    REAL8 deltaT,                           /**< time step corresponding to consec */
    REAL8 f0,                               /**< central frequency (Hz) */
    REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
    REAL8 tau,                              /**< Duration [s] */
    REAL8 f_min,                            /**< starting GW frequency (Hz) */
    REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
    REAL8 hrss,                             /**< hrss [strain] */
    REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
    REAL8 polar_ecc,                        /**< See above */
    LALSimBurstExtraParam *extraParams, /**< Linked list of extra burst parameters. Pass in NULL (or None in python) to neglect these */
    BurstApproximant approximant                 /**< Burst approximant  */
    )
{
  /* General sanity check the input parameters - only give warnings! */
    if( deltaF > 1. )
        XLALPrintWarning("XLAL Warning - %s: Large value of deltaF = %e requested...This corresponds to a very short TD signal (with padding). Consider a smaller value.\n", __func__, deltaF);
    if( deltaF < 1./4096. )
        XLALPrintWarning("XLAL Warning - %s: Small value of deltaF = %e requested...This corresponds to a very long TD signal. Consider a larger value.\n", __func__, deltaF);
    if( f_min < 1. )
        XLALPrintWarning("XLAL Warning - %s: Small value of fmin = %e requested...Check for errors, this could create a very long waveform.\n", __func__, f_min);
    if( f_min > 40.000001 )
        XLALPrintWarning("XLAL Warning - %s: Large value of fmin = %e requested...Check for errors, the signal will start in band.\n", __func__, f_min);
    int ret;
    
    /* Check if need to initiate this w/ meaningful values */
    REAL8 alpha=0.0;
    
    switch (approximant)
    {
        case SineGaussianF:
            /* Waveform-specific sanity checks */
            /* None so far */
            
            (void) f_max;
            if (XLALSimBurstExtraParamExists(extraParams,"alpha")) alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
            // if alpha not there (e.g. because we are calling this routine for injecting, and xml tables do not know about alpha) set polar_angle=alpha
            else alpha=polar_angle;
            (void) polar_angle;
            (void) polar_ecc;
            (void) tau;
            
            /* Call the waveform driver routine */
            ret = XLALInferenceBurstSineGaussianF(hptilde,hctilde,q,f0,hrss, alpha,deltaF,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        case GaussianF:
            /* Waveform-specific sanity checks */
            /* None so far */
            
            (void) f_max;
            if (XLALSimBurstExtraParamExists(extraParams,"alpha")) alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
            // if alpha not there (e.g. because we are calling this routine for injecting, and xml tables do not know about alpha) set polar_angle=alpha
            else alpha=polar_angle;
            (void) polar_angle;
            (void) polar_ecc;
            (void) f0;
            (void) q;
            
            /* Call the waveform driver routine */
            ret = XLALInferenceBurstGaussianF(hptilde,hctilde,tau,hrss, alpha,deltaF,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        case DampedSinusoidF:
            /* Waveform-specific sanity checks */
            /* None so far */
            
            (void) f_max;
            if (XLALSimBurstExtraParamExists(extraParams,"alpha")) alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
            // if alpha not there (e.g. because we are calling this routine for injecting, and xml tables do not know about alpha) set polar_angle=alpha
            else alpha=polar_angle;
            (void) polar_angle;
            (void) polar_ecc;
            (void) tau;

            /* Call the waveform driver routine */
            ret = XLALInferenceBurstDampedSinusoidF(hptilde,hctilde,q,f0,hrss, alpha,deltaF,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        default:
            XLALPrintError("FD version of burst approximant not implemented in lalsimulation\n");
            XLAL_ERROR(XLAL_EINVAL);
    }

    if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);

    return ret;
}

/* Tentative common interface to burst FD WF. Pass all standard burst parameters (as in sim_burst table). 
 * Parameters which are not defined for the WF of interest can be passe as NULL.
 * Unconventional parameters can be passed through extraParams 
 * 
 * Returned waveforms are centered at t=0, thus must be time shifted to wanted time.
 * No taper, windowing, etc, is applied. The caller must take care of that.
 * 
 * */
int XLALSimBurstChooseTDWaveform(
    REAL8TimeSeries **hplus,                    /**< +-polarization waveform */
    REAL8TimeSeries **hcross,                   /**< x-polarization waveform */
    REAL8 deltaT,                           /**< time step corresponding to consec */
    REAL8 f0,                               /**< central frequency (Hz) */
    REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
    REAL8 tau,                              /**< Duration [s] */
    REAL8 f_min,                            /**< starting GW frequency (Hz) */
    REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
    REAL8 hrss,                             /**< hrss [strain] */
    REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
    REAL8 polar_ecc,                        /**< See above */
    LALSimBurstExtraParam *extraParams, /**< Linked list of non-GR parameters. Pass in NULL (or None in python) to neglect these */
    BurstApproximant approximant                 /**< Burst approximant  */
    )
{
  /* General sanity check the input parameters - only give warnings! */
    if( f_min < 1. )
        XLALPrintWarning("XLAL Warning - %s: Small value of fmin = %e requested...Check for errors, this could create a very long waveform.\n", __func__, f_min);
    if( f_min > 40.000001 )
        XLALPrintWarning("XLAL Warning - %s: Large value of fmin = %e requested...Check for errors, the signal will start in band.\n", __func__, f_min);
    int ret;
        
    switch (approximant)
    {
        case SineGaussian:
            /* Waveform-specific sanity checks */
            /* None so far */
            
            (void) f_max;
            (void) tau;
            (void) extraParams;
            /* Call the waveform driver routine */
            ret = XLALInferenceBurstSineGaussianFast(hplus,hcross,q,f0,hrss,polar_ecc ,polar_angle,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        case Gaussian:
            /* Waveform-specific sanity checks */
            /* None so far */
            (void) extraParams;
            (void) f_max;
            (void) f0;
            (void) q;
            
            /* Call the waveform driver routine */
            ret = XLALInferenceBurstGaussian(hplus,hcross,tau,hrss,polar_ecc ,polar_angle,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        case DampedSinusoid:
            /* Waveform-specific sanity checks */
            /* None so far */
            (void) extraParams;
            (void) f_max;
            (void) tau;

            /* Call the waveform driver routine */
            ret = XLALInferenceBurstDampedSinusoid(hplus,hcross,q,f0,hrss,polar_ecc ,polar_angle,deltaT);
            if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);
            break;
        default:
            XLALPrintError("TD version of burst approximant not implemented in lalsimulation\n");
            XLAL_ERROR(XLAL_EINVAL);
    }

    if (ret == XLAL_FAILURE) XLAL_ERROR(XLAL_EFUNC);

    return ret;
}

/**
 * XLAL function to determine string from approximant enum.
 * This function needs to be updated when new approximants are added.
 */
char* XLALGetStringFromBurstApproximant(BurstApproximant bapproximant)
{
  switch (bapproximant)
  {
    case SineGaussianF:
      return strdup("SineGaussianF");
    case SineGaussian:
      return strdup("SineGaussian");
    case GaussianF:
      return strdup("GaussianF");
    case Gaussian:
      return strdup("Gaussian");
    case DampedSinusoidF:
      return strdup("DampedSinusoidF");
    case DampedSinusoid:
      return strdup("DampedSinusoid");
    default:
        XLALPrintError("Not a valid approximant\n");
        XLAL_ERROR_NULL(XLAL_EINVAL);
    }
}

/* ================================ WAVEFORMS ==================================*/


/*
 * ============================================================================
 *
 *                         (Sine)-Gaussian and Friends
 *
 * ============================================================================
 */


/**
 * Input:
 *
 * LIB implementation of SineGaussian. Unlike the one above only uses one elliticity parameter (eccentricty is not used).
 * See https://dcc.ligo.org/LIGO-T1400734
 *
 * Output:
 *
 * h+ and hx time series containing a cosine-Gaussian in the + polarization
 * and a sine-Gaussian in the x polarization.  The Gaussian envelope peaks
 * in both at t = 0 as defined by epoch and deltaT.  Note that *NO*
 * windowing is executed as per LALSimulation convention (the callar applies the window).
 */

int XLALInferenceBurstSineGaussianFast(
	REAL8TimeSeries **hplus,
	REAL8TimeSeries **hcross,
	REAL8 Q,
	REAL8 centre_frequency,
	REAL8 hrss,
	REAL8 eccentricity,
	REAL8 polarization,
	REAL8 delta_t // 1 over srate
)
{
  (void) eccentricity;
  const double hplusrss  = hrss * cos(polarization) ;
  const double hcrossrss = hrss * sin(polarization);
  const double cgrss = sqrt((Q / (4.0 * centre_frequency * sqrt(LAL_PI))) * (1.0 + exp(-Q * Q)));
  const double sgrss = sqrt((Q / (4.0 * centre_frequency * sqrt(LAL_PI))) * (1.0 - exp(-Q * Q)));
  /* "peak" amplitudes of plus and cross */
  const double h0plus  = hplusrss / cgrss;
  const double h0cross = hcrossrss / sgrss;
  LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
  int length;
  unsigned i;

  /* length of the injection time series is 6 * the width of the
   * Gaussian envelope (sigma_t in the comments above), rounded to
   * the nearest odd integer */

  length = (int) floor(6.0 * Q / (LAL_TWOPI * centre_frequency) / delta_t / 2.0);  // This is 6 tau
  length = 2 * length + 1; // length is 12 taus +1 bin
  /* the middle sample is t = 0 */

  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * delta_t); // epoch is set to minus (6 taus) in secs

  /* allocate the time series */

  *hplus = XLALCreateREAL8TimeSeries("sine-Gaussian +", &epoch, 0.0, delta_t, &lalStrainUnit, length);  // hplus epoch=-6tau length = 12tau+1
  *hcross = XLALCreateREAL8TimeSeries("sine-Gaussian x", &epoch, 0.0, delta_t, &lalStrainUnit, length); // hplus epoch=-6tau length = 12tau+1
  if(!*hplus || !*hcross) {
    XLALDestroyREAL8TimeSeries(*hplus);
    XLALDestroyREAL8TimeSeries(*hcross);
    *hplus = *hcross = NULL;
    XLAL_ERROR(XLAL_EFUNC);
  }

  double t=0.0;
  double phi=0.0;
  double fac=0.0;
  double newRe,newIm,dre,dim,re,im;
  /* Employ a trick here for avoiding cos(...) and sin(...) in time
  shifting.  We need to multiply each template frequency bin by
  exp(-J*twopit*deltaF*i) = exp(-J*twopit*deltaF*(i-1)) +
  exp(-J*twopit*deltaF*(i-1))*(exp(-J*twopit*deltaF) - 1) .  This
  recurrance relation has the advantage that the error growth is
  O(sqrt(N)) for N repetitions. */

  /* Values for the first iteration: */
  REAL8 twopif=LAL_TWOPI * centre_frequency;
  re = cos(twopif*(-((REAL8)length-1.)/ 2.) * delta_t);
  im = sin(twopif*(-((REAL8)length-1.)/ 2.) * delta_t);

  // Incremental values, using cos(theta) - 1 = -2*sin(theta/2)^2
  dim = sin(twopif*delta_t);
  dre = -2.0*sin(0.5*twopif*delta_t)*sin(0.5*twopif*delta_t);
  for(i = 0; i < (*hplus)->data->length; i++) {
    t = ((REAL8) i - ((REAL8)length - 1.) / 2.) * delta_t; // t in [-6 tau, ??]
    phi = LAL_TWOPI * centre_frequency * t; // this is the actual time, not t0
    fac = exp(-0.5 * phi * phi / (Q * Q));
    (*hplus)->data->data[i]  = h0plus * fac*re;
    (*hcross)->data->data[i] = h0cross * fac*im ;
    // Now update re and im for the next iteration.
    newRe = re + re*dre - im*dim;
    newIm = im + re*dim + im*dre;
    re = newRe;
    im = newIm;
  }

  return 0;
}

/* Frequency domain SineGaussians (these are the exact analytic Fourier Transform of the LIB time domain SG.
 * 
 * See https://dcc.ligo.org/LIGO-T1400734
 * 
 * */

int XLALInferenceBurstSineGaussianF(
  COMPLEX16FrequencySeries **hplus,
  COMPLEX16FrequencySeries **hcross,
  REAL8 Q,
  REAL8 centre_frequency,
  REAL8 hrss,
  REAL8 alpha,
  REAL8 deltaF,
  REAL8 deltaT
)
{
  /* semimajor and semiminor axes of waveform ellipsoid */
  REAL8 LAL_SQRT_PI=sqrt(LAL_PI);
  /* rss of plus and cross polarizations */
  const double hplusrss  = hrss * cos(alpha);
  const double hcrossrss = hrss * sin(alpha);
  const double cgrss = sqrt((Q / (4.0 * centre_frequency * LAL_SQRT_PI)) * (1.0 +exp(-Q * Q)));
  const double sgrss = sqrt((Q / (4.0 * centre_frequency *LAL_SQRT_PI)) * (1.0 - exp(-Q * Q)));
  /* "peak" amplitudes of plus and cross */
  const double h0plus  = hplusrss / cgrss;
  const double h0cross = hcrossrss / sgrss;
  LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
  int length;
  unsigned i;
    
  /* length of the injection time series is 6 * the width of the
   * time domain Gaussian envelope rounded to the nearest odd integer */
  length = (int) floor(6.0 * Q / (LAL_TWOPI * centre_frequency) / deltaT / 2.0);  // This is 6 tau_t
  length = 2 * length + 1; // length is 12 taus +1 bin
  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * deltaT); // epoch is set to minus (6 taus_t) in secs
    
  
  REAL8 tau=Q/LAL_PI/LAL_SQRT2/centre_frequency;
  REAL8 tau2pi2=tau*tau*LAL_PI*LAL_PI;
  
  /* sigma is the width of the gaussian envelope in the freq domain WF ~ exp(-1/2 X^2/sigma^2)*/
  REAL8 sigma= centre_frequency/Q; // This is also equal to 1/(sqrt(2) Pi tau)
  
  /* set fmax to be f0 + 6sigmas*/
  REAL8 Fmax=centre_frequency + 6.0*sigma;
  /* if fmax > nyquist use nyquist */
  if (Fmax>(1.0/(2.0*deltaT))) 
    Fmax=1.0/(2.0*deltaT);
  
  REAL8 Fmin= centre_frequency -6.0*sigma;
  /* if fmin <0 use 0 */
  if (Fmin<0.0 || Fmin >=Fmax)
    Fmin=0.0;
  
  size_t lower =(size_t) ( Fmin/deltaF);    
  size_t upper= (size_t) ( Fmax/deltaF+1);

  COMPLEX16FrequencySeries *hptilde;
  COMPLEX16FrequencySeries *hctilde;
    
  /* the middle sample is t = 0 */
  hptilde=XLALCreateCOMPLEX16FrequencySeries("hplus",&epoch,0.0,deltaF,&lalStrainUnit,upper);
  hctilde=XLALCreateCOMPLEX16FrequencySeries("hcross",&epoch,0.0,deltaF,&lalStrainUnit,upper);

  if(!hptilde || !hctilde) {
    XLALDestroyCOMPLEX16FrequencySeries(hptilde);
    XLALDestroyCOMPLEX16FrequencySeries(hctilde);
    hctilde=hptilde = NULL;
    XLAL_ERROR(XLAL_EFUNC);
  }
  /* Set to zero below flow */
  for(i = 0; i < hptilde->data->length; i++) {
    hptilde->data->data[i] = 0.0;
    hctilde->data->data[i] = 0.0;
  }
  
  /* populate */
  REAL8 f=0.0;
  REAL8 phi2minus=0.0;
  REAL8 ephimin=0.0;
  
  for(i = lower; i < upper; i++) {
    f=((REAL8 ) i )*deltaF;
    phi2minus= (f-centre_frequency )*(f-centre_frequency );
    ephimin=exp(-phi2minus*tau2pi2);
    hptilde->data->data[i] = h0plus * tau*ephimin/LAL_2_SQRTPI;
    hctilde->data->data[i] = h0cross *tau*ephimin*(-1.0j)/LAL_2_SQRTPI;
  }

  *hplus=hptilde;
  *hcross=hctilde;

  return XLAL_SUCCESS;
}

int XLALInferenceBurstSineGaussianFFast(
  COMPLEX16FrequencySeries **hplus,
  COMPLEX16FrequencySeries **hcross,
  REAL8 Q,
  REAL8 centre_frequency,
  REAL8 hrss,
  REAL8 alpha,
  REAL8 deltaF,
  REAL8 deltaT
)
{
  /* semimajor and semiminor axes of waveform ellipsoid */
  REAL8 LAL_SQRT_PI=sqrt(LAL_PI);
  /* rss of plus and cross polarizations */
  const double hplusrss  = hrss * cos(alpha);
  const double hcrossrss = hrss * sin(alpha);
  REAL8 eqq=exp(-Q*Q);
  const double cgrss = sqrt((Q / (4.0 * centre_frequency * LAL_SQRT_PI)) * (1.0 +eqq));
  const double sgrss = sqrt((Q / (4.0 * centre_frequency *LAL_SQRT_PI)) * (1.0 - eqq));
  /* "peak" amplitudes of plus and cross */
  double h0plus  = hplusrss / cgrss;
  double h0cross = hcrossrss / sgrss;
  LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
  int length;
  unsigned i;

  /* length of the injection time series is 4 * the width of the
  * time domain Gaussian envelope rounded to the nearest odd integer */
  length = (int) floor(4.0 * Q / (LAL_TWOPI * centre_frequency) / deltaT / 2.0);  // This is 4 tau_t
  length = 2 * length + 1; // length is 8 taus +1 bin
  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * deltaT); // epoch is set to minus (4 taus_t) in secs

  REAL8 tau=Q/LAL_PI/LAL_SQRT2/centre_frequency;
  REAL8 tau2pi2=tau*tau*LAL_PI*LAL_PI;

  /* sigma is the width of the gaussian envelope in the freq domain WF ~ exp(-1/2 X^2/sigma^2)*/
  REAL8 sigma= centre_frequency/Q; // This is also equal to 1/(sqrt(2) Pi tau)

  /* set fmax to be f0 + 4sigmas*/
  REAL8 Fmax=centre_frequency + 4.0*sigma;
  /* if fmax > nyquist use nyquist */
  if (Fmax>(1.0/(2.0*deltaT)))
    Fmax=1.0/(2.0*deltaT);

  REAL8 Fmin= centre_frequency -4.0*sigma;
  /* if fmin <0 use 0 */
  if (Fmin<0.0 || Fmin >=Fmax)
    Fmin=0.0;

  size_t lower =(size_t) ( Fmin/deltaF);
  size_t upper= (size_t) ( (Fmax)/deltaF+1);
  upper=upper-lower;
  COMPLEX16FrequencySeries *hptilde;
  COMPLEX16FrequencySeries *hctilde;
  /* the middle sample is t = 0 */
  hptilde=XLALCreateCOMPLEX16FrequencySeries("hplus",&epoch,Fmin,deltaF,&lalStrainUnit,upper);
  hctilde=XLALCreateCOMPLEX16FrequencySeries("hcross",&epoch,Fmin,deltaF,&lalStrainUnit,upper);

  if(!hptilde || !hctilde) {
    XLALDestroyCOMPLEX16FrequencySeries(hptilde);
    XLALDestroyCOMPLEX16FrequencySeries(hctilde);
    hctilde=hptilde = NULL;
    XLAL_ERROR(XLAL_EFUNC);
  }

  /* populate */
  REAL8 f=0.0;
  REAL8 phi2minus=0.0;
  REAL8 ephimin=0.0;
  h0plus=h0plus * tau/LAL_2_SQRTPI;
  h0cross=-h0cross* tau/LAL_2_SQRTPI;
  //#pragma omp parallel for
  for(i = 0; i < upper; i++) {
    f=((REAL8 ) (i+lower) )*deltaF;
    phi2minus= (f-centre_frequency )*(f-centre_frequency );
    ephimin=exp(-phi2minus*tau2pi2);
    hptilde->data->data[i] = crect(h0plus *ephimin,0.0);
    hctilde->data->data[i] = crect(0.0,h0cross *ephimin);
  }

  *hplus=hptilde;
  *hcross=hctilde;

  return XLAL_SUCCESS;
}

  /*
   * We produce time domain gaussian WFs having the form:
   *
   * h_x=C (hrss /sqrt(tau)) (2/Pi)^1/4 exp(-t^2/tau^2)
   * h_x=P (hrss /sqrt(tau)) (2/Pi)^1/4 exp(-t^2/tau^2)
   *
   *  See https://dcc.ligo.org/LIGO-T1400734
   * */
int XLALInferenceBurstGaussian(
	REAL8TimeSeries **hplus,
	REAL8TimeSeries **hcross,
	REAL8 duration,
	REAL8 hrss,
	REAL8 eccentricity,
	REAL8 polarization,
	REAL8 delta_t // 1 over srate
)
{
  (void) eccentricity;
  /* semimajor and semiminor axes of waveform ellipsoid */
  /* rss of plus and cross polarizations */
  const double hplusrss  = hrss * cos(polarization);
  const double hcrossrss = hrss * sin(polarization);
  REAL8 sdur=sqrt(duration);
  /* "peak" amplitudes of plus and cross */
  const double h0plus  = hplusrss /sdur*FRTH_2_Pi ;
  const double h0cross = hcrossrss/sdur*FRTH_2_Pi;

  LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
  int length;
  unsigned i;

  /* length of the injection time series is 6 * the width of the
  * Gaussian envelope (sigma_t in the comments above), rounded to
  * the nearest odd integer */

  length = (int) floor(6.0 *duration/delta_t);// This is 6 tau
  length = 2 * length + 1; // length is 12 taus +1 bin
  /* the middle sample is t = 0 */

  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * delta_t); // epoch is set to minus (12 taus) in secs

  /* allocate the time series */

  *hplus = XLALCreateREAL8TimeSeries("Gaussian +", &epoch, 0.0, delta_t, &lalStrainUnit, length);  // hplus epoch=-30tau length = 60tau+1
  *hcross = XLALCreateREAL8TimeSeries("Gaussian x", &epoch, 0.0, delta_t, &lalStrainUnit, length); // hplus epoch=-30tau length = 60tau+1
  if(!*hplus || !*hcross) {
    XLALDestroyREAL8TimeSeries(*hplus);
    XLALDestroyREAL8TimeSeries(*hcross);
    *hplus = *hcross = NULL;
    XLAL_ERROR(XLAL_EFUNC);
  }

  /* populate */
  double t=0.0;
  double fac=0.0;
  for(i = 0; i < (*hplus)->data->length; i++) {
    t = ((int) i - (length - 1) / 2) * delta_t; // t in [-6 tau, ??]
    fac = exp(-t*t/duration/duration);  // centered around zero. Time shift will be applied later by the caller
    (*hplus)->data->data[i]  = h0plus *fac;
    (*hcross)->data->data[i] = h0cross*fac;
  }

  return 0;
}

/* Frequency domain Gaussians (these are the exact analytic Fourier Transform of the time domain Gaussians.
 *
 * See https://dcc.ligo.org/LIGO-T1400734
 *
 * */
int XLALInferenceBurstGaussianF(
  COMPLEX16FrequencySeries **hplus,
  COMPLEX16FrequencySeries **hcross,
  REAL8 duration,
  REAL8 hrss,
  REAL8 alpha,
  REAL8 deltaF,
  REAL8 deltaT
)
{
  /* semimajor and semiminor axes of waveform ellipsoid */
  /* rss of plus and cross polarizations */
  const double hplusrss  = hrss * cos(alpha);
  const double hcrossrss = hrss * sin(alpha);

  REAL8 sdur=sqrt(duration);
  /* "peak" amplitudes of plus and cross */
  const double h0plus  = hplusrss  *sdur*FRTH_2_times_PI;
  const double h0cross = hcrossrss *sdur*FRTH_2_times_PI;
  LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
  int length;
  unsigned i;

  /* length of the injection time series is 30 * the width of the
 * Gaussian envelope rounded to the nearest odd integer */

  length = (int) floor(6.0 *duration/deltaT);  // This is 6 tau
  length = 2 * length + 1; // length is 12 taus +1 bin
  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * deltaT); // epoch is set to minus (6 taus_t) in secs

  /* sigma is the width of the gaussian envelope in the freq domain */
  REAL8 sigma2=0.5/LAL_PI/LAL_PI/duration/duration;

  REAL8 Fmax=1.0/(2.0*deltaT);
  size_t upper= (size_t) ( Fmax/deltaF+1);

  COMPLEX16FrequencySeries *hptilde;
  COMPLEX16FrequencySeries *hctilde;

  /* the middle sample is t = 0 */
  hptilde=XLALCreateCOMPLEX16FrequencySeries("hplus",&epoch,0.0,deltaF,&lalStrainUnit,upper);
  hctilde=XLALCreateCOMPLEX16FrequencySeries("hcross",&epoch,0.0,deltaF,&lalStrainUnit,upper);

  if(!hptilde || !hctilde) {
    XLALDestroyCOMPLEX16FrequencySeries(hptilde);
    XLALDestroyCOMPLEX16FrequencySeries(hctilde);
    hctilde=hptilde = NULL;
    XLAL_ERROR(XLAL_EFUNC);
  }

  /* populate */
  REAL8 f=0.0;
  REAL8 phi=0.0;
  REAL8 ephi=0.0;
  for(i = 0; i < upper; i++) {
    f=((REAL8 ) i )*deltaF;
    phi=f*f/sigma2;
    ephi=exp(-0.5*phi);
    hptilde->data->data[i] = h0plus *ephi;
    hctilde->data->data[i] = h0cross*ephi;
  }

  *hplus=hptilde;
  *hcross=hctilde;
  return XLAL_SUCCESS;
}



/*
 * ============================================================================
 *
 *                         Construct a Damped Sinusoid waveform
 *
 * ============================================================================
 */

int XLALInferenceBurstDampedSinusoid(
        REAL8TimeSeries **hplus,
        REAL8TimeSeries **hcross,
        REAL8 Q,
        REAL8 centre_frequency,
        REAL8 hrss,
        REAL8 eccentricity,
        REAL8 polarization,
        REAL8 delta_t // 1 over srate
)
{       
        //REAL8Window *window;
        /* semimajor and semiminor axes of waveform ellipsoid */
        const double a = 1.0 / sqrt(2.0 - eccentricity * eccentricity);
        const double b = a * sqrt(1.0 - eccentricity * eccentricity);
        /* rss of plus and cross polarizations */
        const double hplusrss  = hrss * (a * cos(polarization) - b * sin(polarization));
        const double hcrossrss = hrss * (b * cos(polarization) + a * sin(polarization));
        /* rss of unit amplitude damped sinusoid waveforms.  see
         * K. Riles, LIGO-T040055-00.pdf */
        const double cgrss = sqrt((Q / (2.0 * centre_frequency * LAL_PI))); 
        const double sgrss = sqrt((Q / (2.0 * centre_frequency * LAL_PI)));
        /* "peak" amplitudes of plus and cross */
        const double h0plus  = hplusrss / cgrss;
        const double h0cross = hcrossrss / sgrss;
        LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
   
        int length;
        unsigned i;
    
        /* length of the injection time series is 30 * the width of the
         * Gaussian envelope (sigma_t in the comments above), rounded to
         * the nearest odd integer */

        length = (int) floor(6.0 * Q / (LAL_TWOPI * centre_frequency) / delta_t / 2.0);  // This is 20 tau
        length = 2 * length + 1; // length is 40 taus +1 bin
//printf("deltaT inj %lf semi-length %lf \n",delta_t,length/2.*delta_t);
        /* the middle sample is t = 0 */

        XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * delta_t); // epoch is set to minus (20 taus) in secs

        /* allocate the time series */
    
        *hplus = XLALCreateREAL8TimeSeries("DampedSinusoid +", &epoch, 0.0, delta_t, &lalStrainUnit, length);  // hplus epoch=-40tau length = 40tau+1
        *hcross = XLALCreateREAL8TimeSeries("DampedSinusoid x", &epoch, 0.0, delta_t, &lalStrainUnit, length); // hplus epoch=-20tau length = 40tau+1
        if(!*hplus || !*hcross) {
                XLALDestroyREAL8TimeSeries(*hplus);
                XLALDestroyREAL8TimeSeries(*hcross);
                *hplus = *hcross = NULL;
                XLAL_ERROR(XLAL_EFUNC);
        }

        /* populate */
  double t=0.0;
  double phi=0.0;
  double fac=0.0;
  double newRe,newIm,dre,dim,re,im;

   /* Values for the first iteration: */
   REAL8 twopif=LAL_TWOPI * centre_frequency;
   re = cos(twopif*(-((REAL8)length-1.)/ 2.) * delta_t);
   im = sin(twopif*(-((REAL8)length-1.)/ 2.) * delta_t);

   // Incremental values, using cos(theta) - 1 = -2*sin(theta/2)^2
   dim = sin(twopif*delta_t);
   dre = -2.0*sin(0.5*twopif*delta_t)*sin(0.5*twopif*delta_t);

    // FILE * testout = fopen("hcross.txt","w");
        for(i = 0; i < (*hplus)->data->length; i++) {
                t = ((int) i - (length - 1) / 2) * delta_t; // t in [-20 tau, ??]
                if (t < 0.){
                        (*hplus)->data->data[i]  = 0.0;
                        (*hcross)->data->data[i] = 0.0;
                }
                else{
                        //fprintf(testout,"hcross %e \n", hcross );
                        phi = LAL_TWOPI * centre_frequency * t; // this is the actual time, not t0
                        fac = exp(-0.5 * phi / (Q));
                        (*hplus)->data->data[i]  = h0plus * fac * re;
                        (*hcross)->data->data[i] = h0cross * fac * im;
                        // Now update re and im for the next iteration.
                        newRe = re + re*dre - im*dim;
                        newIm = im + re*dim + im*dre;

                        re = newRe;
                        im = newIm;
                }
              //fprintf(testout,"%lf\t%lg\t%lg\n", t, (*hplus)->data->data[i], (*hcross)->data->data[i]);
        }


        return 0;
}



int XLALInferenceBurstDampedSinusoidF(
        COMPLEX16FrequencySeries **hplus,
        COMPLEX16FrequencySeries **hcross,
        REAL8 Q,
        REAL8 centre_frequency,
        REAL8 hrss,
        REAL8 alpha,
        REAL8 deltaF,
        REAL8 deltaT
)
{
        /* semimajor and semiminor axes of waveform ellipsoid */
  //REAL8 LAL_SQRT_PI=sqrt(LAL_PI);
        /* rss of plus and cross polarizations */
        const double hplusrss  = hrss * cos(alpha);
        const double hcrossrss = hrss * sin(alpha);
        const double cgrss = sqrt(Q / (2.0 * centre_frequency * LAL_PI));
        const double sgrss = sqrt(Q / (2.0 * centre_frequency * LAL_PI));
        /* "peak" amplitudes of plus and cross */
        const double h0plus  = hplusrss / cgrss;
        const double h0cross = hcrossrss / sgrss;
        LIGOTimeGPS epoch= LIGOTIMEGPSZERO;
        int length;
        unsigned i;

        /* length of the injection time series is 30 * the width of the
         * time domain Gaussian envelope rounded to the nearest odd integer */
        length = (int) floor(6.0 * Q / (LAL_TWOPI * centre_frequency) / deltaT / 2.0);  // This is 30 tau_t
        length = 2 * length + 1; // length is 60 taus +1 bin
  XLALGPSSetREAL8(&epoch, -(length - 1) / 2 * deltaT); // epoch is set to minus (30 taus_t) in secs

 //REAL8 tau=Q/LAL_PI/LAL_SQRT2/centre_frequency;
  //REAL8 tau2pi2=tau*tau*LAL_PI*LAL_PI;
  REAL8 tau= Q/(LAL_PI*centre_frequency);
  /* sigma is the width of the gaussian envelope in the freq domain WF ~ exp(-1/2 X^2/sigma^2)*/
  REAL8 sigma= centre_frequency/Q; // This is also equal to 1/(sqrt(2) Pi tau)

  /* set fmax to be f0 + 6sigmas*/
  REAL8 Fmax=centre_frequency + 6.0*sigma;
  /* if fmax > nyquist use nyquist */
  if (Fmax>(1.0/(2.0*deltaT)))
  Fmax=1.0/(2.0*deltaT);
  REAL8 Fmin= centre_frequency -6.0*sigma;
  /* if fmin <0 use 0 */
  if (Fmin<0.0 || Fmin >=Fmax)
    Fmin=0.0;
  size_t lower =(size_t) ( Fmin/deltaF);
  size_t upper= (size_t) ( Fmax/deltaF+1);

  COMPLEX16FrequencySeries *hptilde;
  COMPLEX16FrequencySeries *hctilde;

  /* the middle sample is t = 0 */
  hptilde=XLALCreateCOMPLEX16FrequencySeries("hplus",&epoch,0.0,deltaF,&lalStrainUnit,upper);
  hctilde=XLALCreateCOMPLEX16FrequencySeries("hcross",&epoch,0.0,deltaF,&lalStrainUnit,upper);

        if(!hptilde || !hctilde) {
                XLALDestroyCOMPLEX16FrequencySeries(hptilde);
                XLALDestroyCOMPLEX16FrequencySeries(hctilde);
                hctilde=hptilde = NULL;
                XLAL_ERROR(XLAL_EFUNC);
        }
  /* Set to zero below flow */
  for(i = 0; i < lower; i++) {
    hptilde->data->data[i] = 0.0;
    hctilde->data->data[i] = 0.0;
  }

  /* populate */
  REAL8 f=0.0;
  REAL8 d = (2.0 * LAL_PI * centre_frequency);
  REAL8 c = 0.0;

  //FILE * testout = fopen("cippa2.txt","w");
  for(i = lower; i < upper; i++) {

       f=((REAL8 ) i )*deltaF;
       c = (2.0 * LAL_PI * f);
       hptilde->data->data[i]  = h0plus * 0.0;
       hctilde->data->data[i] = h0cross / ((2.0 * (c + d)) - (2.0 * I) / tau);
       hctilde->data->data[i] -= h0cross / ((2.0 * (c - d)) - (2.0 * I) / tau);


  }
  //fclose(testout);

  *hplus=hptilde;
  *hcross=hctilde;

  return 0;



}


/*   ============ BELOW IS WHAT WAS LALSIMBURSTWAVEFORMFROMCACHE  */

/*
 * Copyright (C) 2013 Evan Ochsner and Will M. Farr, 
 *  2014 Salvatore Vitale
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with with program; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 */

#include <math.h>
#include <lal/FrequencySeries.h>
#include <lal/TimeSeries.h>
/**
 * Bitmask enumerating which parameters have changed, to determine
 * if the requested waveform can be transformed from a cached waveform
 * or if it must be generated from scratch.
 */
typedef enum {
    NO_DIFFERENCE = 0,
    INTRINSIC = 1,
    HRSS = 2,
    ALPHA = 4
} CacheVariableDiffersBitmask;

static CacheVariableDiffersBitmask CacheArgsDifferenceBitmask(
        LALSimBurstWaveformCache *cache,
        REAL8 deltaT,
        REAL8 deltaF,
        REAL8 f0,
        REAL8 q,
        REAL8 tau,
        REAL8 f_min,
        REAL8 f_max,
        REAL8 hrss,
        REAL8 polar_angle,
        REAL8 polar_ecc,
        LALSimBurstExtraParam *extraParams,
        BurstApproximant approximant
);

static int StoreTDHCache(
        LALSimBurstWaveformCache *cache,
        REAL8TimeSeries *hplus,
        REAL8TimeSeries *hcross,
        REAL8 deltaT,
        REAL8 f0,
        REAL8 q, REAL8 tau,
        REAL8 f_min, REAL8 f_max,
        REAL8 hrss,
        REAL8 polar_angle,
        REAL8 polar_ecc,
        LALSimBurstExtraParam *extraParams,
        BurstApproximant approximant);

static int StoreFDHCache(
        LALSimBurstWaveformCache *cache,
        COMPLEX16FrequencySeries *hptilde,
        COMPLEX16FrequencySeries *hctilde,
        REAL8 deltaF, 
        REAL8 deltaT,
        REAL8 f0,
        REAL8 q, REAL8 tau,
        REAL8 f_min, REAL8 f_max,
        REAL8 hrss,
        REAL8 polar_angle,
        REAL8 polar_ecc,
        LALSimBurstExtraParam *extraParams,
        BurstApproximant approximant);

/**
 * Chooses between different approximants when requesting a waveform to be generated
 * Returns the waveform in the time domain.
 * The parameters passed must be in SI units.
 *
 * This version allows caching of waveforms. The most recently generated
 * waveform and its parameters are stored. If the next call requests a waveform
 * that can be obtained by a simple transformation, then it is done.
 * This bypasses the waveform generation and speeds up the code.
 */
int XLALSimBurstChooseTDWaveformFromCache(
        REAL8TimeSeries **hplus,                /**< +-polarization waveform */
        REAL8TimeSeries **hcross,               /**< x-polarization waveform */
        REAL8 deltaT,                           /**< sampling interval (s) */
        REAL8 f0,                               /**< central frequency [Hz] */
        REAL8 q,                               /**< quality */
        REAL8 tau,                              /**< duration */
        REAL8 f_min,                            /**< starting GW frequency (Hz) */
        REAL8 f_max,                            /**< max GW frequency (Hz) */
        REAL8 hrss,                             /**< hrss */
        REAL8 polar_angle,                      /**< Together with polar_ecc, controls ratio plus/cross polarization (rad) */
        REAL8 polar_ecc,                        /**< (tidal deformability of mass 1) / m1^5 (dimensionless) */
        LALSimBurstExtraParam *extraParams, /**< Linked list of extra Parameters (includes alpha). Pass in NULL (or None in python) to neglect */
        BurstApproximant approximant,           /**< Burst approximant to use for waveform production */
        LALSimBurstWaveformCache *cache      /**< waveform cache structure; use NULL for no caching */
        )
{
    int status;
    size_t j;
    REAL8 hrss_ratio, alpha_ratio_plus,alpha_ratio_cross;
    CacheVariableDiffersBitmask changedParams;
    REAL8 deltaF=0.0; // UNUSED
    if ( (!cache) )
        return XLALSimBurstChooseTDWaveform(hplus, hcross,deltaT,f0,q,tau,f_min,f_max,hrss,polar_angle,polar_ecc,extraParams,approximant);

    // Check which parameters have changed
    changedParams = CacheArgsDifferenceBitmask(cache, deltaT,deltaF,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);

    // No parameters have changed! Copy the cached polarizations
    if( changedParams == NO_DIFFERENCE ) {
        *hplus = XLALCutREAL8TimeSeries(cache->hplus, 0,
                cache->hplus->data->length);
        if (*hplus == NULL) return XLAL_ENOMEM;
        *hcross = XLALCutREAL8TimeSeries(cache->hcross, 0,
                cache->hcross->data->length);
        if (*hcross == NULL) {
            XLALDestroyREAL8TimeSeries(*hplus);
            *hplus = NULL;
            return XLAL_ENOMEM;
        }

        return XLAL_SUCCESS;
    }

    // Intrinsic parameters have changed. We must generate a new waveform
    if( (changedParams & INTRINSIC) != 0 ) {
        status = XLALSimBurstChooseTDWaveform(hplus, hcross,deltaT,f0,q,tau,f_min,f_max,hrss,polar_angle,polar_ecc,extraParams,approximant);
        if (status == XLAL_FAILURE) return status;

        // FIXME: Need to add hlms, dynamic variables, etc. in cache
        return StoreTDHCache(cache, *hplus, *hcross, deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
    }

    // If polarizations are not cached we must generate a fresh waveform
    if( cache->hplus == NULL || cache->hcross == NULL) {
        status = XLALSimBurstChooseTDWaveform(hplus, hcross,deltaT,f0,q,tau,f_min,f_max,hrss,polar_angle,polar_ecc,extraParams,approximant);
        if (status == XLAL_FAILURE) return status;

        // FIXME: Need to add hlms, dynamic variables, etc. in cache
        return StoreTDHCache(cache, *hplus, *hcross, deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
    }

    // Set transformation coefficients for identity transformation.
    // We'll adjust them depending on which extrinsic parameters changed.
    hrss_ratio = alpha_ratio_plus = alpha_ratio_cross=1.0;

    if( changedParams & ALPHA) {
        // Rescale h+, hx by ratio of new/old alpha dependence
        REAL8 alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
        REAL8 cached_alpha=XLALSimBurstGetExtraParam(cache->extraParams,"alpha");
        alpha_ratio_plus = cos(alpha)/cos(cached_alpha);
        alpha_ratio_cross = sin(alpha)/sin(cached_alpha);
    }
    if( changedParams & HRSS ) {
        // Rescale h+, hx by ratio of new_hrss/old_hrss
        hrss_ratio = hrss/cache->hrss;
    }

    // Create the output polarizations
    *hplus = XLALCreateREAL8TimeSeries(cache->hplus->name,
            &(cache->hplus->epoch), cache->hplus->f0,
            cache->hplus->deltaT, &(cache->hplus->sampleUnits),
            cache->hplus->data->length);
    if (*hplus == NULL) return XLAL_ENOMEM;
    *hcross = XLALCreateREAL8TimeSeries(cache->hcross->name,
            &(cache->hcross->epoch), cache->hcross->f0,
            cache->hcross->deltaT, &(cache->hcross->sampleUnits),
            cache->hcross->data->length);
    if (*hcross == NULL) {
        XLALDestroyREAL8TimeSeries(*hplus);
        *hplus = NULL;
        return XLAL_ENOMEM;
    }

    // Get new polarizations by transforming the old
    alpha_ratio_plus *= hrss_ratio;
    alpha_ratio_cross *= hrss_ratio;
    // FIXME: Do changing phiRef and inclination commute?!?!
    for (j = 0; j < cache->hplus->data->length; j++) {
        (*hplus)->data->data[j] = alpha_ratio_plus
                * (cache->hplus->data->data[j]);
        (*hcross)->data->data[j] = alpha_ratio_cross
                *cache->hcross->data->data[j];
    }

    return XLAL_SUCCESS;

}

/**
 * Chooses between different approximants when requesting a waveform to be generated
 * Returns the waveform in the frequency domain.
 * The parameters passed must be in SI units.
 *
 * This version allows caching of waveforms. The most recently generated
 * waveform and its parameters are stored. If the next call requests a waveform
 * that can be obtained by a simple transformation, then it is done.
 * This bypasses the waveform generation and speeds up the code.
 */
int XLALSimBurstChooseFDWaveformFromCache(
        COMPLEX16FrequencySeries **hptilde,     /**< +-polarization waveform */
        COMPLEX16FrequencySeries **hctilde,     /**< x-polarization waveform */
        REAL8 deltaF,                           /**< sampling interval (Hz) */
        REAL8 deltaT,                           /**< time step corresponding to consec */
        REAL8 f0,                               /**< central frequency (Hz) */
        REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
        REAL8 tau,                              /**< Duration [s] */
        REAL8 f_min,                            /**< starting GW frequency (Hz) */
        REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
        REAL8 hrss,                             /**< hrss [strain] */
        REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
        REAL8 polar_ecc,                        /**< See above */
        LALSimBurstExtraParam *extraParams, /**< Linked list of extra burst parameters. Pass in NULL (or None in python) to neglect these */
        BurstApproximant approximant,                 /**< Burst approximant  */
        LALSimBurstWaveformCache *cache      /**< waveform cache structure; use NULL for no caching */
        )
{
    int status;
    size_t j;
    REAL8 hrss_ratio, alpha_ratio_plus,alpha_ratio_cross;
    COMPLEX16 exp_dphi;
    CacheVariableDiffersBitmask changedParams;
  if ((!cache) ){
     
        return XLALSimBurstChooseFDWaveform(hptilde, hctilde,deltaF,deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
     }

    // Check which parameters have changed
    changedParams = CacheArgsDifferenceBitmask(cache, deltaT,deltaF,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);

    // No parameters have changed! Copy the cached polarizations
    if( changedParams == NO_DIFFERENCE ) {
        *hptilde = XLALCutCOMPLEX16FrequencySeries(cache->hptilde, 0,
                cache->hptilde->data->length);
        if (*hptilde == NULL) return XLAL_ENOMEM;
        *hctilde = XLALCutCOMPLEX16FrequencySeries(cache->hctilde, 0,
                cache->hctilde->data->length);
        if (*hctilde == NULL) {
            XLALDestroyCOMPLEX16FrequencySeries(*hptilde);
            *hptilde = NULL;
            return XLAL_ENOMEM;
        }

        return XLAL_SUCCESS;
    }

    // Intrinsic parameters have changed. We must generate a new waveform
    if( (changedParams & INTRINSIC) != 0 ) {
        status = XLALSimBurstChooseFDWaveform(hptilde, hctilde, deltaF,deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
        if (status == XLAL_FAILURE) return status;

        return StoreFDHCache(cache, *hptilde, *hctilde, deltaF,deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
    }

    // If polarizations are not cached we must generate a fresh waveform
    // FIXME: Will need to check hlms and/or dynamical variables as well
    if( cache->hptilde == NULL || cache->hctilde == NULL) {
        status = XLALSimBurstChooseFDWaveform(hptilde, hctilde, deltaF,deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
        if (status == XLAL_FAILURE) return status;

        return StoreFDHCache(cache, *hptilde, *hctilde, deltaF,deltaT,f0,q, tau,f_min,f_max, hrss,polar_angle, polar_ecc,extraParams,approximant);
    }

    // Set transformation coefficients for identity transformation.
    // We'll adjust them depending on which extrinsic parameters changed.
    hrss_ratio = alpha_ratio_plus = alpha_ratio_cross = 1.;
    exp_dphi = 1.;


    if( changedParams & ALPHA) {
        // Rescale h+, hx by ratio of new/old alpha dependence
        REAL8 alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
        REAL8 cached_alpha=XLALSimBurstGetExtraParam(cache->extraParams,"alpha");
        alpha_ratio_plus = cos(alpha)/cos(cached_alpha);
        alpha_ratio_cross = sin(alpha)/sin(cached_alpha);
    }
    if( changedParams & HRSS ) {
        // Rescale h+, hx by ratio of new_hrss/old_hrss
        hrss_ratio = hrss/cache->hrss;
    }

    // Create the output polarizations
    *hptilde = XLALCreateCOMPLEX16FrequencySeries(cache->hptilde->name,
            &(cache->hptilde->epoch), cache->hptilde->f0,
            cache->hptilde->deltaF, &(cache->hptilde->sampleUnits),
            cache->hptilde->data->length);
    if (*hptilde == NULL) return XLAL_ENOMEM;

    *hctilde = XLALCreateCOMPLEX16FrequencySeries(cache->hctilde->name,
            &(cache->hctilde->epoch), cache->hctilde->f0,
            cache->hctilde->deltaF, &(cache->hctilde->sampleUnits),
            cache->hctilde->data->length);
    if (*hctilde == NULL) {
        XLALDestroyCOMPLEX16FrequencySeries(*hptilde);
        *hptilde = NULL;
        return XLAL_ENOMEM;
    }

    // Get new polarizations by transforming the old
    alpha_ratio_plus *= hrss_ratio;
    alpha_ratio_cross *= hrss_ratio;
    for (j = 0; j < cache->hptilde->data->length; j++) {
        (*hptilde)->data->data[j] = exp_dphi * alpha_ratio_plus
                * cache->hptilde->data->data[j];
        (*hctilde)->data->data[j] = exp_dphi * alpha_ratio_cross
                * cache->hctilde->data->data[j];
    }

  return XLAL_SUCCESS;
  
}

/**
 * Construct and initialize a waveform cache.  Caches are used to
 * avoid re-computation of waveforms that differ only by simple
 * scaling relations in extrinsic parameters.
 */
LALSimBurstWaveformCache *XLALCreateSimBurstWaveformCache()
{
    LALSimBurstWaveformCache *cache = XLALCalloc(1,
            sizeof(LALSimBurstWaveformCache));

    return cache;
}

/**
 * Destroy a waveform cache.
 */
void XLALDestroySimBurstWaveformCache(LALSimBurstWaveformCache *cache)
{
    if (cache != NULL) {
        XLALDestroyREAL8TimeSeries(cache->hplus);
        XLALDestroyREAL8TimeSeries(cache->hcross);
        XLALDestroyCOMPLEX16FrequencySeries(cache->hptilde);
        XLALDestroyCOMPLEX16FrequencySeries(cache->hctilde);

        XLALFree(cache);
    }
}

/**
 * Function to compare the requested arguments to those stored in the cache,
 * returns a bitmask which determines if a cached waveform can be recycled.
 */
static CacheVariableDiffersBitmask CacheArgsDifferenceBitmask(
    LALSimBurstWaveformCache *cache ,     /**< waveform cache structure; use NULL for no caching */
    REAL8 deltaT,
    REAL8 deltaF,
    REAL8 f0,                               /**< central frequency (Hz) */
    REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
    REAL8 tau,                              /**< Duration [s] */
    REAL8 f_min,                            /**< starting GW frequency (Hz) */
    REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
    REAL8 hrss,                             /**< hrss [strain] */
    REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
    REAL8 polar_ecc,                        /**< See above */
    LALSimBurstExtraParam *extraParams, /**< Linked list of non-GR parameters. Pass in NULL (or None in python) to neglect these */
    BurstApproximant approximant                 /**< Burst approximant  */
    )
{
    CacheVariableDiffersBitmask difference = NO_DIFFERENCE;

    if (cache == NULL) return INTRINSIC;

    if ( deltaT != cache->deltaT) return INTRINSIC;
    if ( deltaF != cache->deltaF) return INTRINSIC;
    if ( f0 != cache->f0) return INTRINSIC;
    if ( q != cache->q) return INTRINSIC;
    if ( tau != cache->tau) return INTRINSIC;
    if ( f_min != cache->f_min) return INTRINSIC;
    if ( f_max != cache->f_max) return INTRINSIC;
    if ( polar_angle != cache->polar_angle) return INTRINSIC;
    if ( polar_ecc != cache->polar_ecc) return INTRINSIC;
    if ( approximant != cache->approximant) return INTRINSIC;
    if ( polar_angle != cache->polar_angle) return INTRINSIC;
    if ( polar_ecc != cache->polar_ecc) return INTRINSIC;
    
    if (hrss != cache->hrss) difference = difference | HRSS;
    
    if (extraParams!=NULL){
      if (XLALSimBurstExtraParamExists(extraParams,"alpha")){ 
        REAL8 alpha=XLALSimBurstGetExtraParam(extraParams,"alpha");
        REAL8 cached_alpha=1.0;
        if ( XLALSimBurstExtraParamExists(cache->extraParams,"alpha"))
          cached_alpha=XLALSimBurstGetExtraParam(cache->extraParams,"alpha");
        else 
          return INTRINSIC;
        if (alpha!= cached_alpha) difference = difference | ALPHA;
        }
    }
    return difference;
}

/** Store the output TD hplus and hcross in the cache. */
static int StoreTDHCache(
        LALSimBurstWaveformCache *cache,
        REAL8TimeSeries *hplus,
        REAL8TimeSeries *hcross,
        REAL8 deltaT,                           /**< time step corresponding to consec */
        REAL8 f0,                               /**< central frequency (Hz) */
        REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
        REAL8 tau,                              /**< Duration [s] */
        REAL8 f_min,                            /**< starting GW frequency (Hz) */
        REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
        REAL8 hrss,                             /**< hrss [strain] */
        REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
        REAL8 polar_ecc,                        /**< See above */
        LALSimBurstExtraParam *extraParams, /**< Linked list of non-GR parameters. Pass in NULL (or None in python) to neglect these */
        BurstApproximant approximant                 /**< Burst approximant  */
        )
{
    /* Clear any frequency-domain data. */
    if (cache->hptilde != NULL) {
        XLALDestroyCOMPLEX16FrequencySeries(cache->hptilde);
        cache->hptilde = NULL;
    }

    if (cache->hctilde != NULL) {
        XLALDestroyCOMPLEX16FrequencySeries(cache->hctilde);
        cache->hctilde = NULL;
    }

    /* Store params in cache */
    cache->deltaT = deltaT;
    cache->f0 = f0;
    cache->q = q;
    cache->tau = tau;
    cache->f_min = f_min;
    cache->f_max = f_max;
    cache->hrss = hrss;
    cache->polar_angle =polar_angle;
    cache->polar_ecc = polar_ecc;
    if (extraParams==NULL)
      cache->extraParams=NULL;
    else if (cache->extraParams==NULL){
      /* Initialize to something that won't make the ratio of sin alphas to blow up */
      cache->extraParams=XLALSimBurstCreateExtraParam("alpha",1.0);
    }
    else{
      XLALSimBurstSetExtraParam(cache->extraParams,"alpha",XLALSimBurstGetExtraParam(extraParams,"alpha"));
    }
    cache->approximant = approximant;

    // Copy over the waveforms
    // NB: XLALCut... creates a new Series object and copies data and metadata
    XLALDestroyREAL8TimeSeries(cache->hplus);
    XLALDestroyREAL8TimeSeries(cache->hcross);
    cache->hplus = XLALCutREAL8TimeSeries(hplus, 0, hplus->data->length);
    if (cache->hplus == NULL) return XLAL_ENOMEM;
    cache->hcross = XLALCutREAL8TimeSeries(hcross, 0, hcross->data->length);
    if (cache->hcross == NULL) {
        XLALDestroyREAL8TimeSeries(cache->hplus);
        cache->hplus = NULL;
        return XLAL_ENOMEM;
    }

    return XLAL_SUCCESS;
}

/** Store the output FD hptilde and hctilde in cache. */
static int StoreFDHCache(LALSimBurstWaveformCache *cache,
        COMPLEX16FrequencySeries *hptilde,
        COMPLEX16FrequencySeries *hctilde,
        REAL8 deltaF,                           /**< sampling interval (Hz) */
        REAL8 deltaT,                           /**< time step corresponding to consec */
        REAL8 f0,                               /**< central frequency (Hz) */
        REAL8 q,                                /**< Q (==sqrt(2) \pi f0 tau ) [dless]*/
        REAL8 tau,                              /**< Duration [s] */
        REAL8 f_min,                            /**< starting GW frequency (Hz) */
        REAL8 f_max,                            /**< ending GW frequency (Hz) (0 for Nyquist) */
        REAL8 hrss,                             /**< hrss [strain] */
        REAL8 polar_angle,                      /**< Polar_ellipse_angle as defined in the burst table. Together with polar_ellipse_eccentricity below will fix the ratio of + vs x aplitude. Some WFs uses a single parameter alpha for this. Alpha is passed through extraParams*/
        REAL8 polar_ecc,                        /**< See above */
        LALSimBurstExtraParam *extraParams, /**< Linked list of extra burst parameters. Pass in NULL (or None in python) to neglect these */
        BurstApproximant approximant                 /**< Burst approximant  */
    )
{
    /* Clear any time-domain data. */
    if (cache->hplus != NULL) {
        XLALDestroyREAL8TimeSeries(cache->hplus);
        cache->hplus = NULL;
    }

    if (cache->hcross != NULL) {
        XLALDestroyREAL8TimeSeries(cache->hcross);
        cache->hcross = NULL;
    }

    /* Store params in cache */
    cache->deltaT = deltaT;
    cache->deltaF = deltaF;
    cache->f0 = f0;
    cache->q = q;
    cache->tau = tau;
    cache->f_min = f_min;
    cache->f_max = f_max;
    cache->hrss = hrss;
    cache->polar_angle =polar_angle;
    cache->polar_ecc = polar_ecc;
    if (extraParams==NULL)
      cache->extraParams=NULL;
    else if (cache->extraParams==NULL){
      /* Initialize to something that won't make the ratio of sin alphas to blow up */
      cache->extraParams=XLALSimBurstCreateExtraParam("alpha",1.0);
    }
    else{
      XLALSimBurstSetExtraParam(cache->extraParams,"alpha",XLALSimBurstGetExtraParam(extraParams,"alpha"));
    }
    
    cache->approximant = approximant;

    // Copy over the waveforms
    // NB: XLALCut... creates a new Series object and copies data and metadata
    XLALDestroyCOMPLEX16FrequencySeries(cache->hptilde);
    XLALDestroyCOMPLEX16FrequencySeries(cache->hctilde);
    cache->hptilde = XLALCutCOMPLEX16FrequencySeries(hptilde, 0,
            hptilde->data->length);
    if (cache->hptilde == NULL) return XLAL_ENOMEM;
    cache->hctilde = XLALCutCOMPLEX16FrequencySeries(hctilde, 0,
            hctilde->data->length);
    if (cache->hctilde == NULL) {
        XLALDestroyCOMPLEX16FrequencySeries(cache->hptilde);
        cache->hptilde = NULL;
        return XLAL_ENOMEM;
    }

    return XLAL_SUCCESS;
}

/* =============== BELOW IS WHAT WAS XLALSIMBURSTEXTRAPARAMS */

/* Copyright (C) 2014 Salvatore Vitale
 *  Based on LALSimBurstExtraParams of Del Pozzo, Ochser and Vitale
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with with program; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 */

/**
 * Function that creates the head node of the test GR parameters linked list.
 * It is initialized with a single parameter with given name and value
 */
LALSimBurstExtraParam *XLALSimBurstCreateExtraParam(
        const char *name, /**< Name of first parameter in new linked list */
        double value 	 /**< Value of first parameter in new linked list */
        )
{
        LALSimBurstExtraParam *parameter = (LALSimBurstExtraParam *)XLALMalloc(sizeof(LALSimBurstExtraParam));
        if (parameter) 
        {
            parameter->data =  (LALSimBurstExtraParamData *)XLALMalloc(sizeof(LALSimBurstExtraParamData));
            memcpy(parameter->data->name, name, 32);
            parameter->data->value = value;
        }
        parameter->next=NULL;
        return parameter;
}

/**
 * Function that adds a parameter to the test GR parameters linked list. If the
 * parameter already exists, it throws an error.
 */
int XLALSimBurstAddExtraParam(
        LALSimBurstExtraParam **parameter, /**< Pointer to the head node of the linked list of parameters */
        const char *name, 		/**< Parameter name */
        double value 			/**< Parameter value */
        )
{
    LALSimBurstExtraParam *temp;
    temp = *parameter;
    if (*parameter==NULL) 
    {
        temp = XLALSimBurstCreateExtraParam(name,value); 
        //temp->next=NULL;
        *parameter=temp;
    }
    else 
    {

        if (!XLALSimBurstExtraParamExists(*parameter, name))
        {
            temp = *parameter;
             while(temp->next!=NULL) {temp=temp->next;}
            LALSimBurstExtraParam *newParam = XLALSimBurstCreateExtraParam(name,value);        
            temp->next = newParam;
        }
        else 
        {
            XLALPrintError("XLAL Error - %s: parameter '%s' exists already! Not added to the structure\n",
                    __func__, name);
            XLAL_ERROR(XLAL_EINVAL);
        }
    }
    return XLAL_SUCCESS;
}

/**
 * Function that sets the value of the desired parameter in the extra burst
 * parameters linked list to 'value'.  Throws an error if the parameter
 * is not found
 */
int XLALSimBurstSetExtraParam(
        LALSimBurstExtraParam *parameter, /**< Linked list to be modified */
        const char *name, 		/**< Name of parameter to be modified */
        const double value 		/**< New value for parameter */
        )
{
    if (XLALSimBurstExtraParamExists(parameter, name)) 
    {
        while(parameter)
        {
            if(!strcmp(parameter->data->name, name)) parameter->data->value = value;
            parameter=parameter->next;
        }
        return XLAL_SUCCESS;
    }
    else
    {
        XLALPrintError("XLAL Error - %s: parameter '%s' unknown!\n",
                __func__, name);
        XLAL_ERROR(XLAL_EINVAL);
    }
}

/**
 * Function that returns the value of the desired parameters in the
 * extra burst parameters linked list.  Aborts if the parameter is not found
 */
double XLALSimBurstGetExtraParam(
        const LALSimBurstExtraParam *parameter, /**< Linked list to retrieve from */
        const char *name 	   /**< Name of parameter to be retrieved */
        )
{
    if (XLALSimBurstExtraParamExists(parameter, name)) 
        {
            while(parameter) 
            {
                if(!strcmp(parameter->data->name, name)) return parameter->data->value;
                parameter=parameter->next;
            }
        }
    else 
    {
        XLALPrintError("XLAL Error - %s: parameter '%s' unknown!\n",
                __func__, name);
        XLAL_ERROR(XLAL_EINVAL);
    }
    return 0.0; // Should not actually get here!
}

/**
 * Function that checks whether the requested parameter exists within the
 * burst extra parameters linked list.  Returns true (1) or false (0) accordingly
 */
int XLALSimBurstExtraParamExists(
        const LALSimBurstExtraParam *parameter, 	/**< Linked list to check */
        const char *name 		/**< Parameter name to check for */
        )
{
  if(!parameter) return 0;
  while(parameter) {if(!strcmp(parameter->data->name, name)) return 1; else parameter=parameter->next;}
  return 0;
}

/** Function that prints the whole test burst extra parameters linked list */
int XLALSimBurstPrintExtraParam(
        FILE *fp, 			/**< FILE pointer to write to */
        LALSimBurstExtraParam *parameter 	/**< Linked list to print */
        )
{
    if (parameter!=NULL)
    {
        while(parameter) 
        {
            fprintf(fp,"%s %10.5f\n",parameter->data->name,parameter->data->value);
            parameter=parameter->next;
        }
        return XLAL_SUCCESS;
    }
    else
    {
        XLALPrintError("XLAL Error - %s: parameter not allocated!\n", __func__);
        XLAL_ERROR(XLAL_EINVAL);
    }
}

/** Function that destroys the whole burst extra params linked list */
void XLALSimBurstDestroyExtraParam(
        LALSimBurstExtraParam *parameter 	/**< Linked list to destroy */
        )
{
   LALSimBurstExtraParam *tmp;
   while(parameter){
	tmp=parameter->next;
	XLALFree(parameter->data);
	XLALFree(parameter);
	parameter=tmp;
	}
}

