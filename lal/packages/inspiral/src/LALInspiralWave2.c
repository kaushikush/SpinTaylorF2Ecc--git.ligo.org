/*
*  Copyright (C) 2007 Stas Babak, David Churches, Jolien Creighton, B.S. Sathyaprakash, Craig Robinson , Thomas Cokelaer
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

/*  <lalVerbatim file="LALInspiralWave2CV">
Author: Sathyaprakash, B. S.
$Id$
</lalVerbatim>  */

/*  <lalLaTeX>

\subsection{Module \texttt{LALInspiralWave2.c} and \texttt{LALInspiralWave2Templates.c}}

These modules generate a time-domain chirp waveform of type {\tt TaylorT2}.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{LALInspiralWave2CP}
\index{\verb&LALInspiralWave2()&}
\begin{itemize}
\item {\tt output:} Output containing the inspiral waveform.
\item {\tt params:} Input containing binary chirp parameters.
\end{itemize}
\vspace{0.1in}
\input{LALInspiralWave2TemplatesCP}
\index{\verb&LALInspiralWave2Templates()&}
\begin{itemize}
\item {\tt output1:} Output containing the 0-phase inspiral waveform.
\item {\tt output2:} Output containing the $\pi/2$-phase inspiral waveform.
\item {\tt params:} Input containing binary chirp parameters.
\end{itemize}

\subsubsection*{Description}

\texttt{LALInspiralWave2} generates {\tt TaylorT2} approximant wherein 
the phase of the waveform is given as an implicit function of time
as in Equation (\ref{eq:InspiralWavePhase2}). A template is required
to be sampled at equal intervals of time. Thus, first of the equations
in Equation (\ref{eq:InspiralWavePhase2}) is solved for $v$ at equally
spaced values of the time steps
$t_k$ and the resulting value of $v_k$ is used in the second equation to
obtain the phase $\phi_k$. 

\texttt{LALInspiralWave2Templates} is exactly the same as \texttt{LALInspiralWave2}
except that it generates two waveforms that differ in phase by $\pi/2.$

\subsubsection*{Uses}

\texttt{LALInspiralParameterCalc}\\
\texttt{LALDBisectionFindRoot}\\
\texttt{LALInspiralPhasing2}\\

\subsubsection*{Notes}

\vfill{\footnotesize\input{LALInspiralWave2CV}}

</lalLaTeX>  */

#include <lal/LALStdlib.h>
#include <lal/LALInspiral.h>
#include <lal/FindRoot.h>
#include <lal/Units.h>
#include <lal/SeqFactories.h>

static void
LALInspiralWave2Engine(
                LALStatus        *status,
                REAL4Vector      *output1,
                REAL4Vector      *output2,
                REAL4Vector      *h,
                REAL4Vector      *a,
                REAL4Vector      *ff,
                REAL8Vector      *phi,
                UINT4            *countback,
                InspiralTemplate *params,
		InspiralInit     *paramsInit
                );



NRCSID (LALINSPIRALWAVE2C, "$Id$");

/*  <lalVerbatim file="LALInspiralWave2CP"> */

void 
LALInspiralWave2(
   LALStatus        *status, 
   REAL4Vector      *output, 
   InspiralTemplate *params
   )
{ /* </lalVerbatim>  */

  UINT4 count;

  InspiralInit paramsInit;

  INITSTATUS (status, "LALInspiralWave2", LALINSPIRALWAVE2C);
  ATTATCHSTATUSPTR(status);

  ASSERT(output,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(output->data,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(params,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT((INT4)params->nStartPad >= 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((REAL8)params->fLower > 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((REAL8)params->tSampling > 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((INT4)params->order >= 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((INT4)params->order <= 8, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);

  /* Initially the waveform is empty */
  memset(output->data, 0, output->length * sizeof(REAL4));

  LALInspiralSetup(status->statusPtr, &(paramsInit.ak), params);
  CHECKSTATUSPTR(status);
  LALInspiralChooseModel(status->statusPtr, &(paramsInit.func), 
                                        &(paramsInit.ak), params);
  CHECKSTATUSPTR(status);

  /* Call the engine function */
  LALInspiralWave2Engine(status->statusPtr, output, NULL, NULL, NULL, 
			NULL, NULL, &count, params, &paramsInit);
  CHECKSTATUSPTR(status);

  DETATCHSTATUSPTR(status);
  RETURN(status);

}

NRCSID (LALINSPIRALWAVE2TEMPLATESC, "$Id$");

/*  <lalVerbatim file="LALInspiralWave2TemplatesCP"> */

void 
LALInspiralWave2Templates(
			  LALStatus        *status, 
			  REAL4Vector      *output1, 
			  REAL4Vector      *output2, 
			  InspiralTemplate *params
			  )

{ /* </lalVerbatim>  */
  
  UINT4 count;

  InspiralInit paramsInit;

  INITSTATUS (status, "LALInspiralWave2Templates", LALINSPIRALWAVE2TEMPLATESC);
  ATTATCHSTATUSPTR(status);

  ASSERT(output1,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(output2,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(output1->data,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(output2->data,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT(params,status,LALINSPIRALH_ENULL,LALINSPIRALH_MSGENULL);
  ASSERT((INT4)params->nStartPad >= 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((REAL8)params->fLower > 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((REAL8)params->tSampling > 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((INT4)params->order >= 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT((INT4)params->order <= 8, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);

  /* Initially the waveforms are empty */
  memset(output1->data, 0, output1->length * sizeof(REAL4));
  memset(output2->data, 0, output2->length * sizeof(REAL4));

  LALInspiralSetup(status->statusPtr, &(paramsInit.ak), params);
  CHECKSTATUSPTR(status);
  LALInspiralChooseModel(status->statusPtr, &(paramsInit.func), 
					&(paramsInit.ak), params);
  CHECKSTATUSPTR(status);

  /* Call the engine function */
  LALInspiralWave2Engine(status->statusPtr, output1, output2, NULL, NULL, 
			   NULL, NULL, &count, params, &paramsInit);
  CHECKSTATUSPTR(status);

  DETATCHSTATUSPTR(status);
  RETURN(status);

}




NRCSID (LALINSPIRALWAVE2FORINJECTIONC, "$Id$");

/*  <lalVerbatim file="LALInspiralWave2ForInjectionCP"> */

void 
LALInspiralWave2ForInjection(
   LALStatus        *status, 
   CoherentGW *waveform,   
   InspiralTemplate *params,
   PPNParamStruc  *ppnParams			     
   )

{ /* </lalVerbatim>  */

  UINT4 count, i;

  REAL4Vector *a   = NULL;/* pointers to generated amplitude  data */
  REAL4Vector *h   = NULL;/* pointers to generated polarizations */
  REAL4Vector *ff  = NULL;/* pointers to generated  frequency data */
  REAL8Vector *phi = NULL;/* pointer to generated phase data */

  CreateVectorSequenceIn in;

  REAL8 phiC;/* phase at coalescence */

  CHAR message[256];
  
  InspiralInit paramsInit;  

  /** -- -- */
  INITSTATUS (status, "LALInspiralWave2ForInjection", LALINSPIRALWAVE2FORINJECTIONC);
  ATTATCHSTATUSPTR(status);

  /* Make sure parameter and waveform structures exist. */
  ASSERT( params, status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL );
  ASSERT(waveform, status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);  
  ASSERT( !( waveform->a ), status, LALINSPIRALH_ENULL,  LALINSPIRALH_MSGENULL );
  ASSERT( !( waveform->h ), status, LALINSPIRALH_ENULL,  LALINSPIRALH_MSGENULL );
  ASSERT( !( waveform->f ), status, LALINSPIRALH_ENULL,  LALINSPIRALH_MSGENULL );
  ASSERT( !( waveform->phi ), status, LALINSPIRALH_ENULL,  LALINSPIRALH_MSGENULL );
  ASSERT( !( waveform->shift ), status, LALINSPIRALH_ENULL,  LALINSPIRALH_MSGENULL );
   
  
  /* Compute some parameters*/
  LALInspiralInit(status->statusPtr, params, &paramsInit);
  CHECKSTATUSPTR(status);   
  if (paramsInit.nbins == 0)
    {
      DETATCHSTATUSPTR(status);
      RETURN (status);
      
    }

  /* Now we can allocate memory and vector for coherentGW structure*/     
  LALSCreateVector(status->statusPtr, &ff, paramsInit.nbins);
  CHECKSTATUSPTR(status);   
  LALSCreateVector(status->statusPtr, &a, 2*paramsInit.nbins);
  CHECKSTATUSPTR(status);   
  LALDCreateVector(status->statusPtr, &phi, paramsInit.nbins);
  CHECKSTATUSPTR(status);
  
  /* By default the waveform is empty */
  memset(ff->data, 0, paramsInit.nbins * sizeof(REAL4));
  memset(a->data, 0, 2 * paramsInit.nbins * sizeof(REAL4));
  memset(phi->data, 0, paramsInit.nbins * sizeof(REAL8));

  if( params->ampOrder )
  {
    LALSCreateVector(status->statusPtr, &h, 2*paramsInit.nbins);
    CHECKSTATUSPTR(status);   
    memset(h->data, 0, 2 * paramsInit.nbins * sizeof(REAL4));
  }

  count = 0;


  /* Call the engine function */
  LALInspiralWave2Engine(status->statusPtr, NULL, NULL, h, a, ff,
			     phi, &count, params, &paramsInit);

  BEGINFAIL(status)
  {
     LALSDestroyVector(status->statusPtr, &ff);
     CHECKSTATUSPTR(status);
     LALSDestroyVector(status->statusPtr, &a);
     CHECKSTATUSPTR(status);
     LALDDestroyVector(status->statusPtr, &phi);
     CHECKSTATUSPTR(status);
     if( params->ampOrder )
     {
       LALSDestroyVector(status->statusPtr, &h);
       CHECKSTATUSPTR(status);
     }
  }
  ENDFAIL(status);
  
  if ( fabs(phi->data[count-1]/2.)/LAL_PI < 2. ){
        sprintf(message, "The waveform has only %f cycles; we don't keep waveform with less than 2 cycles.", 
	       (double)(fabs(phi->data[count-1]/2.)/LAL_PI) );
    LALWarning(status, message);


  }
  else
    {
      /*wrap the phase vector*/
      phiC =  phi->data[count-1] ;
      for (i=0; i<count;i++)
	{
	  phi->data[i] =  phi->data[i] -phiC + ppnParams->phi;
	}
      /* Allocate the waveform structures. */
      if ( ( waveform->a = (REAL4TimeVectorSeries *)
	     LALCalloc(1, sizeof(REAL4TimeVectorSeries) ) ) == NULL ) {
	ABORT( status, LALINSPIRALH_EMEM,
	       LALINSPIRALH_MSGEMEM );
      }
      if ( ( waveform->f = (REAL4TimeSeries *)
	     LALCalloc(1, sizeof(REAL4TimeSeries) ) ) == NULL ) {
	LALFree( waveform->a ); waveform->a = NULL;
	ABORT( status, LALINSPIRALH_EMEM,
	       LALINSPIRALH_MSGEMEM );
      }
      if ( ( waveform->phi = (REAL8TimeSeries *)
	     LALCalloc(1, sizeof(REAL8TimeSeries) ) ) == NULL ) {
	LALFree( waveform->a ); waveform->a = NULL;
	LALFree( waveform->f ); waveform->f = NULL;
	ABORT( status, LALINSPIRALH_EMEM,
	       LALINSPIRALH_MSGEMEM );
      }
      
      
      in.length = (UINT4)count;
      in.vectorLength = 2;
      LALSCreateVectorSequence( status->statusPtr,
				&( waveform->a->data ), &in );
      CHECKSTATUSPTR(status);      
      LALSCreateVector( status->statusPtr,
			&( waveform->f->data ), count);
      CHECKSTATUSPTR(status);      
      LALDCreateVector( status->statusPtr,
			&( waveform->phi->data ), count );
      CHECKSTATUSPTR(status);        
      
      memcpy(waveform->f->data->data , ff->data, count*(sizeof(REAL4)));
      memcpy(waveform->a->data->data , a->data, 2*count*(sizeof(REAL4)));
      memcpy(waveform->phi->data->data ,phi->data, count*(sizeof(REAL8)));
      
      
      waveform->a->deltaT = waveform->f->deltaT = waveform->phi->deltaT
	= 1./params->tSampling;
      
      waveform->a->sampleUnits   = lalStrainUnit;
      waveform->f->sampleUnits   = lalHertzUnit;
      waveform->phi->sampleUnits = lalDimensionlessUnit;
      waveform->position = ppnParams->position;
      waveform->psi = ppnParams->psi;

      LALSnprintf( waveform->a->name, LALNameLength,   "T2 inspiral amplitude" );
      LALSnprintf( waveform->f->name, LALNameLength,   "T2 inspiral frequency" );
      LALSnprintf( waveform->phi->name, LALNameLength, "T2 inspiral phase" );
      
      
      /* --- fill some output ---*/
      ppnParams->tc     = (double)(count-1) / params->tSampling ;
      ppnParams->length = count;
      ppnParams->dfdt   = ((REAL4)(waveform->f->data->data[count-1] 
				   - waveform->f->data->data[count-2]))
	* ppnParams->deltaT;
      ppnParams->fStop  = params->fFinal;
      ppnParams->termCode        = GENERATEPPNINSPIRALH_EFSTOP;
      ppnParams->termDescription = GENERATEPPNINSPIRALH_MSGEFSTOP;
      
      ppnParams->fStart   = ppnParams->fStartIn;

      if( params->ampOrder )
      {  
        if ( ( waveform->a = (REAL4TimeVectorSeries *)
	       LALCalloc(1, sizeof(REAL4TimeVectorSeries) ) ) == NULL ) 
        {
	  ABORT( status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM );
        }
        LALSCreateVectorSequence( status->statusPtr,
				  &( waveform->h->data ), &in );
        CHECKSTATUSPTR(status);      
        memcpy(waveform->h->data->data , h->data, 2*count*(sizeof(REAL4)));
        waveform->h->deltaT = 1./params->tSampling;
        waveform->h->sampleUnits   = lalStrainUnit;
        LALSnprintf( waveform->h->name, LALNameLength,   "T2 inspiral polarizations" );
        LALSDestroyVector(status->statusPtr, &h);
        CHECKSTATUSPTR(status);
      }
    }/*end of coherentGW storage */

  
  /* --- free memory --- */
  LALSDestroyVector(status->statusPtr, &ff);
  CHECKSTATUSPTR(status);
  LALSDestroyVector(status->statusPtr, &a);
  CHECKSTATUSPTR(status);
  LALDDestroyVector(status->statusPtr, &phi);
  CHECKSTATUSPTR(status);
  

  DETATCHSTATUSPTR(status);
  RETURN(status);
}


NRCSID (LALINSPIRALWAVE2ENGINEC, "$Id$");

/* 'Engine' function upon which all the other functions invoke
    Craig Robinson 04/05 */

/* <lalVerbatim file="LALInspiralWave2EngineCP"> */

static void 
LALInspiralWave2Engine(
                LALStatus        *status,
                REAL4Vector      *output1,
                REAL4Vector      *output2,
                REAL4Vector      *h,
                REAL4Vector      *a,
                REAL4Vector      *ff,
                REAL8Vector      *phi,
                UINT4            *countback,
                InspiralTemplate *params,
		InspiralInit     *paramsInit
                )
{ /* </lalVerbatim> */

  REAL8 amp, eta, dt, fs, fu, fHigh, phase0, phase1, tC;
  REAL8 phase, v, totalMass, fLso, freq, fOld;
  INT4 i, startShift, count;
  DFindRootIn rootIn;
  InspiralToffInput toffIn;
  void *funcParams;
  expnCoeffs ak;
  expnFunc func;

  /* Variables only used in injection case.. */
  REAL8 omega;
  REAL8 unitHz = 0;
  REAL8 f2a = 0;
  REAL8 mu = 0;
  REAL8 mTot = 0;
  REAL8 cosI = 0;/* cosine of system inclination */
  REAL8 etab =0;
  REAL8 fFac = 0; /* SI normalization for f and t */
  REAL8 f2aFac = 0;/* factor multiplying f in amplitude function */
  REAL8 apFac = 0, acFac = 0;/* extra factor in plus and cross amplitudes */
                                                                                                                             
                                                                                                                             
  INITSTATUS(status, "LALInspiralWave2Engine", LALINSPIRALWAVE2ENGINEC);
  ATTATCHSTATUSPTR(status);

  ak   = paramsInit->ak;
  func = paramsInit->func;
  
  if (output2 || a)
         params->nStartPad = 0;   /* that value must be zero for template generation */
  dt = 1.0/(params->tSampling);   /* sampling interval */
  fs = params->fLower;            /* lower frequency cutoff */
  fu = params->fCutoff;           /* upper frequency cutoff */
  startShift = params->nStartPad; /* number of bins to pad at the beginning */
  phase0 = params->startPhase;    /* initial phasea */
  phase1 = phase0 + LAL_PI_2;
                                                                                                                             
  rootIn.function = func.timing2; /* function to solve for v, given t:*/

  if (a || h)           /* Only used in injection case */
  {
    mTot   =  params->mass1 + params->mass2;
    etab   =  params->mass1 * params->mass2;
    etab  /= mTot;
    etab  /= mTot;
    unitHz = (mTot) *LAL_MTSUN_SI*(REAL8)LAL_PI;
    cosI   = cos( params->inclination );
    mu     = etab * mTot;
    fFac   = 1.0 / ( 4.0*LAL_TWOPI*LAL_MTSUN_SI*mTot );
    f2aFac = LAL_PI*LAL_MTSUN_SI*mTot*fFac;
    apFac  = acFac = -2.0 * mu * LAL_MRSUN_SI/params->distance;
    apFac *= 1.0 + cosI*cosI;
    acFac *= 2.0 * cosI;
  }

/* Calculate the three unknown paramaters in (m1,m2,M,eta,mu) from the two
   which are given.  */
                                                                                                                             
  LALInspiralParameterCalc(status->statusPtr, params);
  CHECKSTATUSPTR(status);
                                                                                                                             
  ASSERT(params->totalMass > 0., status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT(params->eta >= 0, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT(params->eta <=0.25, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
                                                                                                                             
  eta = params->eta;
  totalMass = params->totalMass*LAL_MTSUN_SI; /* solar mass in seconds */
                                                                                                                             
  toffIn.tN = ak.tvaN;
  toffIn.t2 = ak.tva2;
  toffIn.t3 = ak.tva3;
  toffIn.t4 = ak.tva4;
  toffIn.t5 = ak.tva5;
  toffIn.t6 = ak.tva6;
  toffIn.t7 = ak.tva7;
  toffIn.tl6 = ak.tvl6;
  toffIn.piM = ak.totalmass * LAL_PI;
                                                                                                                             
  /* Determine the total chirp-time tC: the total chirp time is
     timing2(v0;tC,t) with t=tc=0*/
                                                                                                                             
  toffIn.t = 0.;
  toffIn.tc = 0.;
  funcParams = (void *) &toffIn;
  func.timing2(status->statusPtr, &tC, fs, funcParams);
  CHECKSTATUSPTR(status);
  /* Reset chirp time in toffIn structure */
  toffIn.tc = -tC;
                                                                                                                             
  /* Determine the initial phase: it is phasing2(v0) with ak.phiC=0 */
  v = pow(fs * LAL_PI * totalMass, oneby3);
  ak.phiC = 0.0;
  func.phasing2(status->statusPtr, &phase, v, &ak);
  CHECKSTATUSPTR(status);
  ak.phiC = -phase;
                                                                                                                             
  /*
     If flso is less than the user inputted upper frequency cutoff fu,
  */
                                                                                                                             
  fLso = ak.fn;
  if (fu)
    fHigh = (fu < fLso) ? fu : fLso;
  else
    fHigh = fLso;
                                                                                                                             
  /* Is the sampling rate large enough? */
                                                                                                                             
  ASSERT(fHigh < 0.5/dt, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT(fHigh > params->fLower, status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
                                                                                                                             
  rootIn.xmax = 1.1*fu;
  rootIn.xacc = 1.0e-8;
  rootIn.xmin = 0.999999*fs;
                                                                                                                             
  i = startShift;
                                                                                                                             
  /* Now cast the input structure to argument 4 of BisectionFindRoot so that it
     of type void * rather than InspiralToffInput  */
                                                                                                                             
  funcParams = (void *) &toffIn;
                                                                                                                             
  toffIn.t = 0.0;
  freq = fs;
  count=0;
  do
    {
    /*
    Check we're not writing past the end of the vector
    */
    if ((output1 && ((UINT4)i >= output1->length)) || (ff && ((UINT4)count >= ff->length)))
    {
        ABORT(status, LALINSPIRALH_EVECTOR, LALINSPIRALH_MSGEVECTOR);
    }
                                                                                                                             
    fOld = freq;
    v = pow(freq*toffIn.piM, oneby3);
    func.phasing2(status->statusPtr, &phase, v, &ak); /* phase at given v */
    CHECKSTATUSPTR(status);
    amp = params->signalAmplitude*v*v;

    if (output1)
    {
      output1->data[i]=(REAL4)(amp*cos(phase+phase0));
      if (output2)
        output2->data[i]=(REAL4)(amp*cos(phase+phase1));
    }
    else
    {
      int ice, ico;
      ice = 2*count;
      ico = ice + 1;
      omega = v*v*v;
                                                                                                                             
      ff->data[count]= (REAL4)(omega/unitHz);
      f2a = pow (f2aFac * omega, 2./3.);
      a->data[ice]          = (REAL4)(4.*apFac * f2a);
      a->data[ico]        = (REAL4)(4.*acFac * f2a);
      phi->data[count]          = (REAL8)(phase);

      if(h)
      {
        h->data[ice] = LALInspiralHPlusPolarization( phase, v, params );
        h->data[ico] = LALInspiralHCrossPolarization( phase, v, params );
      }
    }
    i++;
    ++count;
    toffIn.t=count*dt;
    /*
       Determine the frequency at the current time by solving timing2(v;tC,t)=0
    */
    LALDBisectionFindRoot(status->statusPtr, &freq, &rootIn, funcParams);
    CHECKSTATUSPTR(status);
    } while (freq < fHigh && freq > fOld && toffIn.t < -tC);
  params->fFinal = fOld;
  if (output1 && !(output2))   params->tC = toffIn.t;

  *countback = count;

  DETATCHSTATUSPTR(status);
  RETURN(status);
                                                                                                                             
}
