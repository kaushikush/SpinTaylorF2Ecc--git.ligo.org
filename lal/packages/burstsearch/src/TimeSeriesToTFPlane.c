/******** <lalVerbatim file="TimeSeriesToTFPlaneCV"> ********
Author: Flanagan, E
$Id$
********* </lalVerbatim> ********/

#include <lal/LALRCSID.h>


NRCSID (TIMESERIESTOTFPLANEC, "$Id$");



#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <lal/LALStdlib.h>
#include <lal/SeqFactories.h>
#include <lal/RealFFT.h>
#include <lal/ComplexFFT.h>
#include <lal/TFTransform.h>


/******** <lalVerbatim file="TimeSeriesToTFPlaneCP"> ********/
void
LALTimeSeriesToTFPlane (
            LALStatus                               *status,
	    COMPLEX8TimeFrequencyPlane           *tfp,
	    REAL4TimeSeries                      *timeSeries,
	    VerticalTFTransformIn                *input
	    )
/******** </lalVerbatim> ********/
{
  REAL4Vector        *tmp  = NULL;
  COMPLEX8Vector     *tmp1 = NULL;
  REAL4              fac;
  INT4               i;
  INT4               j;
  INT4               nt;
  INT4               nf;
  INT4               nforig;
  INT4               ntotal;

  INT4               flow1;
  INT4               fhigh1;
  INT4               tseglength;

  INITSTATUS (status, "LALTimeSeriesToTFPlane", TIMESERIESTOTFPLANEC);
  ATTATCHSTATUSPTR (status);

  

  /* make sure that arguments are not NULL */
  ASSERT (timeSeries, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (timeSeries->data, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (timeSeries->data->data, status, TFTRANSFORM_ENULLP,
          TFTRANSFORM_MSGENULLP);

  ASSERT (input, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (input->dftParams, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (input->dftParams->plan, status, TFTRANSFORM_ENULLP, 
          TFTRANSFORM_MSGENULLP);
  ASSERT (input->dftParams->plan->plan, status, TFTRANSFORM_ENULLP, 
          TFTRANSFORM_MSGENULLP);
  ASSERT (input->dftParams->window, status, TFTRANSFORM_ENULLP, 
          TFTRANSFORM_MSGENULLP);
  ASSERT (input->dftParams->window->data, status, TFTRANSFORM_ENULLP, 
          TFTRANSFORM_MSGENULLP);



  /* make sure that output structure is not NULL */
  ASSERT (tfp, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (tfp->params, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);
  ASSERT (tfp->data, status, TFTRANSFORM_ENULLP, TFTRANSFORM_MSGENULLP);




  /*
   *
   *
   *  make sure input parameters are reasonable, compatible with
   *  each other, etc.
   *
   *
   */



  nt = tfp->params->timeBins;   /* Number of time bins */
  ASSERT (nt > 0, status, TFTRANSFORM_EPOSARG, TFTRANSFORM_MSGEPOSARG);

  /* 
   * Next compute nforig = total number of bins in frequncy domain
   * before chopping  (original number of bins in frequency domain)
   * = (size of chunks of data in time domain) / 2, 
   *
   * Note tfp->params->deltaT is required sampling time of TFplane
   * while timeSeries->deltaT is sampling time of input time series.
   *
   */

  ASSERT( timeSeries->deltaT>0.0, status, TFTRANSFORM_EPOSARG, 
         TFTRANSFORM_MSGEPOSARG);  
  ASSERT( tfp->params->deltaT>0.0, status, TFTRANSFORM_EPOSARG, 
         TFTRANSFORM_MSGEPOSARG);  

  nforig = (INT4)( (tfp->params->deltaT) / (2.0*(timeSeries->deltaT)) );
  tseglength = 2 * nforig;

  ASSERT( nforig>0, status, TFTRANSFORM_EINCOMP, TFTRANSFORM_MSGEINCOMP);  
  ASSERT( tseglength == (INT4)input->dftParams->plan->size, status, 
          TFTRANSFORM_EINCOMP, TFTRANSFORM_MSGEINCOMP);
  ASSERT( tseglength == (INT4)input->dftParams->window->length, status, 
          TFTRANSFORM_EINCOMP, TFTRANSFORM_MSGEINCOMP);

  /* Supplied FFT plan must be in forward direction */
  ASSERT( input->dftParams->plan->sign==1, status, 
          TFTRANSFORM_EINCOMP, TFTRANSFORM_MSGEINCOMP);
  
  /* Input hetrydyne frequency must be non-negative */
  ASSERT(timeSeries->f0 >= 0.0, status, TFTRANSFORM_EPOSARG,
         TFTRANSFORM_MSGEPOSARG);

  /* sumofsquares parameter must be positive */
  ASSERT(input->dftParams->sumofsquares>0.0, status, TFTRANSFORM_EPOSARG, 
	 TFTRANSFORM_MSGEPOSARG);  

  /* compute total length of data to be used to construct TF plane */
  ntotal = 2 * nt * nforig;
  ASSERT(input->startT + ntotal <= (INT4)timeSeries->data->length, status, 
         TFTRANSFORM_EINCOMP, TFTRANSFORM_MSGEINCOMP);

  /* 
   * Actual number of number of frequency bins to be used,
   * after bandpass filtering (chopping)
   *
   */
  nf = tfp->params->freqBins;   
  ASSERT( nf>0, status, TFTRANSFORM_EPOSARG, TFTRANSFORM_MSGEPOSARG);

  /*
   * Dealing with hetrodyned time Series (i.e. timeSeries->f0 > 0 )
   * 
   * Let f = original real frequency in time series.
   * Let fbar = rescaled frequency = f - f0.
   *
   * When we compute DFT of segment in the time domain, we get nforig
   * frequency bins separated by deltaF = 1 / tfp->params->deltaT
   * with rescaled frequencies satisfying
   *   0 \le fbar \le nforig * deltaF
   * 
   * We want to retain only the portion of the DFT with true frequencies
   * f with flow \le f \le flow + nf * deltaF, which is equivalent to 
   * retaining rescaled frequencies fbar with
   *   flow-f0 \le fbar \le flow-f0 + nf * deltaF.
   *
   * So, we just use flow - f0 in place of flow.
   *
   */

  /* low frequency cutoff in units of frequency resolution of TF plane */
  flow1 = (INT4)( (tfp->params->flow - timeSeries->f0)* tfp->params->deltaT);

  /* high frequency cutoff in units of frequency resolution of TF plane */
  fhigh1 = flow1 + nf;


  /* 
   * check is specified number of frequency bins ok
   * note that nforig = (nyquist frequency in units of frequency
   * resolution of TF plane). 
   *
   * use nforig+1 in ASSERT below rather than nforig since 
   * the DFT of a sequence of 2*n
   * real numbers is a sequence of n+1 complex numbers (with the first
   * and last being real) rather than a sequence of n complex numbers
   *
   */
  ASSERT( fhigh1 <= nforig+1, status, TFTRANSFORM_EINCOMP, 
	  TFTRANSFORM_MSGEINCOMP);  



  /* 
   * copy some of the information from the input timeseries
   * to the TFplane structure
   *
   */

  /*
   * OMITTED
   *
  tfp->sampleUnits = timeSeries->sampleUnits;
   */


  /* 
   *  
   *  Compute the starting epoch tfp->epoch.  If we are to use data displaced
   *  from the start of the timeseries, then we need to convert
   *  from LIGOTimeGPS to REAL8 and back again to do the addition.
   *
   */
  if(input->startT)
    {
      LIGOTimeGPS t1,t2;
      REAL8 t;
      t1 = timeSeries->epoch;
      t = (REAL8)(t1.gpsSeconds) + (REAL8)(t1.gpsNanoSeconds)/1000000000.0 + (REAL8)(input->startT ) * timeSeries->deltaT;
      t2.gpsSeconds = (INT4)(t);
      t = t - (REAL8)(t2.gpsSeconds);
      t2.gpsNanoSeconds = (INT4)( t * 1000000000.0);
      tfp->epoch = t2;
    }
  else
    {
      tfp->epoch = timeSeries->epoch;
    };

  /* This TF plane is a vertical type */
  tfp->planeType = verticalPlane;


  /*
   *
   *
   *  Now start the computation of the TF transform.
   *
   *
   */

  /* create temporary vectors */
  LALSCreateVector (status->statusPtr, &tmp, tseglength);
  CHECKSTATUSPTR (status);
  LALCCreateVector (status->statusPtr, &tmp1, tseglength/2+1);
  CHECKSTATUSPTR (status);


  fac = 1/sqrt(input->dftParams->sumofsquares);

  /* loop over successive data segments in the time domain */
  for(i=0; i< nt; i++)
    {
      INT4 offset = input->startT + i * tseglength;

      /* Get the segment of time domain data and window it */
      for(j=0; j< tseglength; j++)
	{
	  tmp->data[j] = fac * timeSeries->data->data[offset+j] 
	    * input->dftParams->window->data[j];
	}
      
      /* Do the FFT */
      LALFwdRealFFT (status->statusPtr, tmp1, tmp, input->dftParams->plan);
      CHECKSTATUSPTR (status);
      
      /* Copy the result into appropriate spot in output structure */
      offset = i * nf;
      for(j=0; j< nf; j++)
	{
	  tfp->data[offset+j] = tmp1->data[flow1+j];
	}
       
    }

  LALDestroyVector (status->statusPtr, &tmp);
  CHECKSTATUSPTR (status);

  LALCDestroyVector (status->statusPtr, &tmp1);
  CHECKSTATUSPTR (status);

  /* normal exit */
  DETATCHSTATUSPTR (status);
  RETURN (status);
}



