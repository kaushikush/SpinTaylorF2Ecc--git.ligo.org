/*----------------------------------------------------------------------- 
 * 
 * File Name: inspiral.c
 *
 * Author: Brown, D. A.
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <time.h>
#include <math.h>

#include <FrameL.h>

#include <lalapps.h>
#include <series.h>
#include <processtable.h>
#include <lalappsfrutils.h>

#include <lal/LALConfig.h>
#include <lal/LALStdio.h>
#include <lal/LALStdlib.h>
#include <lal/LALError.h>
#include <lal/LALDatatypes.h>
#include <lal/AVFactories.h>
#include <lal/LALConstants.h>
#include <lal/FrameStream.h>
#include <lal/ResampleTimeSeries.h>
#include <lal/FrameCalibration.h>
#include <lal/Window.h>
#include <lal/TimeFreqFFT.h>
#include <lal/IIRFilter.h>
#include <lal/BandPassTimeSeries.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/LIGOLwXML.h>
#include <lal/LIGOLwXMLRead.h>
#include <lal/Date.h>
#include <lal/Units.h>
#include <lal/FindChirp.h>
#include <lal/FindChirpSP.h>
#include <lal/FindChirpChisq.h>
#include <lal/FindChirpEngine.h>

#include "inspiral.h"

RCSID( "$Id$" );

#define CVS_ID_STRING "$Id$"
#define CVS_NAME_STRING "$Name$"
#define CVS_REVISION "$Revision$"
#define CVS_SOURCE "$Source$"
#define CVS_DATE "$Date$"
#define PROGRAM_NAME "inspiral"

/* define the parameters for a 1.4,1.4 sloar mass standard candle with snr 8 */
#define CANDLE_MASS1 1.4
#define CANDLE_MASS2 1.4
#define CANDLE_RHOSQ 64.0

/*
 *
 * variables that control program behaviour
 *
 */


/* debugging */
extern int vrbflg;                      /* verbocity of lal function    */

/* input data parameters */
INT8  gpsStartTimeNS   = 0;             /* input data GPS start time ns */
LIGOTimeGPS gpsStartTime;               /* input data GPS start time    */
INT8  gpsEndTimeNS     = 0;             /* input data GPS end time ns   */
LIGOTimeGPS gpsEndTime;                 /* input data GPS end time      */
INT4  padData = 0;                      /* saftety margin on input data */
CHAR  *fqChanName       = NULL;         /* name of data channel         */
CHAR  *frInCacheName    = NULL;         /* cache file containing frames */
INT4  numPoints         = -1;           /* points in a segment          */
INT4  numSegments       = -1;           /* number of segments           */
INT4  ovrlap            = -1;           /* overlap between segments     */
CHAR  ifo[3];                           /* two character ifo code       */
CHAR *channelName = NULL;               /* channel string               */
UINT4 inputDataLength = 0;              /* number of points in input    */
REAL4 minimalMatch = -1;                /* override bank minimal match  */

/* data conditioning parameters */
LIGOTimeGPS slideData   = {0,0};        /* slide data for time shifting */
INT4   resampFiltType   = -1;           /* low pass filter used for res */
INT4   sampleRate       = -1;           /* sample rate of filter data   */
INT4   highPass         = -1;           /* enable high pass on raw data */
REAL4  highPassFreq     = 0;            /* high pass frequency          */
INT4   highPassOrder    = -1;           /* order of the td iir filter   */
REAL4  highPassAtten    = -1;           /* attenuation of the td filter */

REAL4  fLow             = -1;           /* low frequency cutoff         */
INT4   specType         = -1;           /* use median or mean psd       */
INT4   badMeanPsd       = 0;            /* use a mean with no overlap   */
INT4   invSpecTrunc     = -1;           /* length of inverse spec (s)   */
REAL4  dynRangeExponent = -1;           /* exponent of dynamic range    */
CHAR  *calCacheName     = NULL;         /* location of calibration data */
CHAR  *injectionFile    = NULL;         /* name of file containing injs */
int   injectOverhead	= 0;		/* inject h+ into detector	*/

/* matched filter parameters */
CHAR *bankFileName      = NULL;         /* name of input template bank  */
INT4  startTemplate     = -1;           /* index of first template      */
INT4  stopTemplate      = -1;           /* index of last template       */
INT4  numChisqBins      = -1;           /* number of chisq bins         */
REAL4 snrThresh         = -1;           /* signal to noise thresholds   */
REAL4 chisqThresh       = -1;           /* chisq veto thresholds        */
INT4  eventCluster      = -1;           /* perform chirplen clustering  */
Approximant approximant;                /* waveform approximant         */

/* output parameters */
CHAR  *userTag          = NULL;         /* string the user can tag with */
INT8   trigStartTimeNS  = 0;            /* write triggers only after    */
INT8   trigEndTimeNS    = 0;            /* write triggers only before   */
int    enableOutput     = -1;           /* write out inspiral events    */
int    writeRawData     = 0;            /* write the raw data to a file */
int    writeFilterData  = 0;            /* write post injection data    */
int    writeResponse    = 0;            /* write response function used */
int    writeSpectrum    = 0;            /* write computed psd to file   */
int    writeRhosq       = 0;            /* write rhosq time series      */
int    writeChisq       = 0;            /* write chisq time series      */

/* other command line args */
CHAR comment[LIGOMETA_COMMENT_MAX];     /* process param comment        */

int main( int argc, char *argv[] )
{
  /* lal function variables */
  LALStatus             status = blank_status;
  LALLeapSecAccuracy    accuracy = LALLEAPSEC_LOOSE;

  /* frame input data */
  FrCache      *frInCache = NULL;
  FrStream     *frStream = NULL;
  FrChanIn      frChan;

  /* frame output data */
  struct FrFile *frOutFile  = NULL;
  struct FrameH *outFrame   = NULL;
  UINT4          nRhosqFr = 0;
  UINT4          nChisqFr = 0;

  /* raw input data storage */
  REAL4TimeSeries               chan;
  REAL4FrequencySeries          spec;
  COMPLEX8FrequencySeries       resp;
  DataSegmentVector            *dataSegVec = NULL;

  /* structures for preconditioning */
  COMPLEX8FrequencySeries       injResp;
  COMPLEX8FrequencySeries      *injRespPtr;
  ResampleTSParams              resampleParams;
  LALWindowParams               wpars;
  AverageSpectrumParams         avgSpecParams;

  /* findchirp data structures */
  FindChirpInitParams          *fcInitParams   = NULL;
  FindChirpSegmentVector       *fcSegVec       = NULL;
  FindChirpSPDataParams        *fcDataParams   = NULL;
  FindChirpSPTmpltParams       *fcTmpltParams  = NULL;
  FindChirpFilterParams        *fcFilterParams = NULL;
  FindChirpFilterInput         *fcFilterInput  = NULL;
  FindChirpStandardCandle       candle;

  /* inspiral template structures */
  INT4                          numTmplts    = 0;
  InspiralTemplate             *bankHead     = NULL;
  InspiralTemplate             *bankCurrent  = NULL;
  InspiralTemplateNode         *tmpltHead    = NULL;
  InspiralTemplateNode         *tmpltCurrent = NULL;
  InspiralTemplateNode         *tmpltInsert  = NULL;

  /* inspiral events */
  INT4                          numEvents   = 0;
  SnglInspiralTable            *event       = NULL;
  SnglInspiralTable            *eventList   = NULL;
  MetadataTable                 savedEvents;

  /* output data */
  MetadataTable         proctable;
  MetadataTable         procparams;
  MetadataTable         searchsumm;
  MetadataTable         searchsummvars;
  SearchSummvarsTable  *this_search_summvar;
  MetadataTable         summvalue;
  SummValueTable        candleTable;
  ProcessParamsTable   *this_proc_param;
  LIGOLwXMLStream       results;

  /* counters and other variables */
  const LALUnit strainPerCount = {0,{0,0,0,0,0,1,-1},{0,0,0,0,0,0,0}};
  UINT4 i;
  INT4  inserted;
  INT4  currentLevel;
  CHAR  fname[256];
  REAL8 inputLengthNS;
  UINT4 numInputPoints;
  const REAL8 epsilon = 1.0e-8;
  UINT4 resampleChan = 0;
  REAL8 tsLength;
  INT8  durationNS	= 0;
  LIGOTimeGPS duration	= {0,0};


  /*
   *
   * initialization
   *
   */


  /* set up inital debugging values */
  lal_errhandler = LAL_ERR_EXIT;
  set_debug_level( "1" );

  /* create the process and process params tables */
  proctable.processTable = (ProcessTable *) calloc( 1, sizeof(ProcessTable) );
  LAL_CALL( LALGPSTimeNow ( &status, &(proctable.processTable->start_time),
        &accuracy ), &status );
  LAL_CALL( populate_process_table( &status, proctable.processTable, 
        PROGRAM_NAME, CVS_REVISION, CVS_SOURCE, CVS_DATE ), &status );
  this_proc_param = procparams.processParamsTable = (ProcessParamsTable *) 
    calloc( 1, sizeof(ProcessParamsTable) );
  memset( comment, 0, LIGOMETA_COMMENT_MAX * sizeof(CHAR) );

  /* create the search summary and zero out the summvars table */
  searchsumm.searchSummaryTable = (SearchSummaryTable *)
    calloc( 1, sizeof(SearchSummaryTable) );
  searchsummvars.searchSummvarsTable = NULL;

  /* call the argument parse and check function */
  arg_parse_check( argc, argv, procparams );
  
  /* wind to the end of the process params table */
  for ( this_proc_param = procparams.processParamsTable; this_proc_param->next;
     this_proc_param = this_proc_param->next );

  /* can use LALMalloc() and LALCalloc() from here onwards */

  /* fill the comment, if a user has specified on, or leave it blank */
  if ( ! *comment )
  {
    LALSnprintf( proctable.processTable->comment, LIGOMETA_COMMENT_MAX, " " );
    LALSnprintf( searchsumm.searchSummaryTable->comment, LIGOMETA_COMMENT_MAX, 
        " " );
  } 
  else 
  {
    LALSnprintf( proctable.processTable->comment, LIGOMETA_COMMENT_MAX,
        "%s", comment );
    LALSnprintf( searchsumm.searchSummaryTable->comment, LIGOMETA_COMMENT_MAX,
        "%s", comment );
  }

  /* the number of nodes for a standalone job is always 1 */
  searchsumm.searchSummaryTable->nnodes = 1;

  /* make sure the pointer to the first event is null */
  savedEvents.snglInspiralTable = NULL;

  /* create the standard candle and database table */
  summvalue.summValueTable = &candleTable;
  memset( &candleTable, 0, sizeof(SummValueTable) );
  memset( &candle, 0, sizeof(FindChirpStandardCandle) );
  strncpy( candle.ifo, ifo, 2 * sizeof(CHAR) );
  candle.tmplt.mass1 = CANDLE_MASS1;
  candle.tmplt.mass2 = CANDLE_MASS2;
  candle.rhosq       = CANDLE_RHOSQ;
  candle.tmplt.totalMass = candle.tmplt.mass1 + candle.tmplt.mass2;
  candle.tmplt.mu = candle.tmplt.mass1 * candle.tmplt.mass2 / 
    candle.tmplt.totalMass;
  candle.tmplt.eta = candle.tmplt.mu / candle.tmplt.totalMass;


  /*
   *
   * create a (possibly heirarcical) template bank
   *
   */


  /* read in the template bank from a ligo lw xml file */
  numTmplts = InspiralTmpltBankFromLIGOLw( &bankHead, bankFileName,
      startTemplate, stopTemplate );
  if ( numTmplts < 0 )
  {
    fprintf( stderr, "error: unable to read templates from %s\n", 
        bankFileName );
    exit( 1 );
  }
  else if ( numTmplts == 0 )
  {
    /* if there are no tmplts, store the time we would have analyzed and exit */
    fprintf( stdout, "no templates found in template bank file: %s\n"
        "exiting without searching for events.\n", bankFileName );

    if ( trigStartTimeNS )
    {
      LAL_CALL( LALINT8toGPS( &status, 
            &(searchsumm.searchSummaryTable->out_start_time), 
            &trigStartTimeNS ), &status );
    }
    else
    {
      searchsumm.searchSummaryTable->out_start_time.gpsSeconds = 
        gpsStartTime.gpsSeconds + (numPoints / (4 * sampleRate));
    }
  
    if ( trigEndTimeNS )
    {
      LAL_CALL( LALINT8toGPS( &status, 
            &(searchsumm.searchSummaryTable->out_end_time), 
            &trigEndTimeNS ), &status );
    }
    else
    {
      searchsumm.searchSummaryTable->out_end_time.gpsSeconds = 
        gpsEndTime.gpsSeconds - (numPoints / (4 * sampleRate));
    }

    goto cleanexit;
  }


  if ( vrbflg ) fprintf( stdout, "parsed %d templates from %s\n", 
      numTmplts, bankFileName );

  /* override the minimal match of the bank if specified on the command line */
  if ( minimalMatch >= 0 )
  {
    if ( vrbflg )
    {
      fprintf( stdout, "Overriding bank minimal match:\n   value in bank = %e,"
        " new value = %e\n", bankHead->minMatch, minimalMatch );
    }
    for ( bankCurrent = bankHead; bankCurrent; bankCurrent = bankCurrent->next )
    {
      bankCurrent->minMatch = minimalMatch;
    }
  }

  /* save the minimal match of the bank in the process params                */
  /* create the table entry with calloc() since it will be freed with free() */
  this_proc_param = this_proc_param->next = (ProcessParamsTable *) 
    calloc( 1, sizeof(ProcessParamsTable) ); 
  LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, "%s", 
      PROGRAM_NAME );
  LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, "--minimal-match" );
  LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "float" ); 
  LALSnprintf( this_proc_param->value, LIGOMETA_VALUE_MAX, "%e", 
      bankHead->minMatch );

  /* create the linked list of template nodes for coarse templates */
  for ( bankCurrent = bankHead; bankCurrent; bankCurrent = bankCurrent->next )
  {
    LAL_CALL( LALFindChirpCreateTmpltNode( &status, 
          bankCurrent, &tmpltCurrent ), &status );
    if ( !tmpltHead ) tmpltHead = tmpltCurrent;
  }


  /*
   *
   * read in the input data channel
   *
   */


  /* set the time series parameters of the input data and resample params */
  memset( &resampleParams, 0, sizeof(ResampleTSParams) );
  resampleParams.deltaT = 1.0 / (REAL8) sampleRate;

  /* set the params of the input data time series */
  memset( &chan, 0, sizeof(REAL4TimeSeries) );
  chan.epoch = gpsStartTime;
  chan.epoch.gpsSeconds -= padData;   /* subtract pad seconds from start */
  /* subtract slide from start */
  chan.epoch.gpsSeconds -= slideData.gpsSeconds; 
  chan.epoch.gpsNanoSeconds -= slideData.gpsNanoSeconds;

  /* open a frame cache or files, seek to required epoch and set chan name */
  if ( frInCacheName )
  {
    LAL_CALL( LALFrCacheImport( &status, &frInCache, frInCacheName), &status );
    LAL_CALL( LALFrCacheOpen( &status, &frStream, frInCache ), &status );
  }
  else
  {
    LAL_CALL( LALFrOpen( &status, &frStream, NULL, "*.gwf" ), &status );
  }
  LAL_CALL( LALFrSeek( &status, &(chan.epoch), frStream ), &status );
  frChan.name = fqChanName;

  /* determine the sample rate of the raw data and allocate enough memory */
  LAL_CALL( LALFrGetREAL4TimeSeries( &status, &chan, &frChan, frStream ),
      &status );

  /* store the input sample rate */
  this_search_summvar = searchsummvars.searchSummvarsTable = 
    (SearchSummvarsTable *) LALCalloc( 1, sizeof(SearchSummvarsTable) );
  LALSnprintf( this_search_summvar->name, LIGOMETA_NAME_MAX * sizeof(CHAR),
      "raw data sample rate" );
  this_search_summvar->value = chan.deltaT;

  /* determine if we need to resample the channel */
  if ( vrbflg )
  {
    fprintf( stdout, "resampleParams.deltaT = %e\n", resampleParams.deltaT );
    fprintf( stdout, "chan.deltaT = %e\n", chan.deltaT );
  }
  if ( ! ( fabs( resampleParams.deltaT - chan.deltaT ) < epsilon ) )
  {
    resampleChan = 1;
    if ( vrbflg )
      fprintf( stdout, "input channel will be resampled\n" );

    if ( resampFiltType == 0 )
    {
      resampleParams.filterType = LDASfirLP;
    }
    else if ( resampFiltType == 1 )
    {
      resampleParams.filterType = defaultButterworth;
    }
  }

  /* determine the number of points to get and create storage forr the data */
  inputLengthNS = 
    (REAL8) ( gpsEndTimeNS - gpsStartTimeNS + 2000000000LL * padData );
  numInputPoints = (UINT4) floor( inputLengthNS / (chan.deltaT * 1.0e9) + 0.5 );
  LAL_CALL( LALSCreateVector( &status, &(chan.data), numInputPoints ), 
      &status );

  if ( vrbflg ) fprintf( stdout, "input channel %s has sample interval "
      "(deltaT) = %e\nreading %d points from frame stream\n", fqChanName, 
      chan.deltaT, numInputPoints );

  /* read the data channel time series from frames */
  LAL_CALL( LALFrGetREAL4TimeSeries( &status, &chan, &frChan, frStream ),
      &status );
  memcpy( &(chan.sampleUnits), &lalADCCountUnit, sizeof(LALUnit) );

  /* store the start and end time of the raw channel in the search summary */
  searchsumm.searchSummaryTable->in_start_time = chan.epoch;
  LAL_CALL( LALGPStoFloat( &status, &tsLength, &(chan.epoch) ), 
      &status );
  tsLength += chan.deltaT * (REAL8) chan.data->length;
  LAL_CALL( LALFloatToGPS( &status, 
        &(searchsumm.searchSummaryTable->in_end_time), &tsLength ), &status );

  /* close the frame file stream and destroy the cache */
  LAL_CALL( LALFrClose( &status, &frStream ), &status );
  if ( frInCacheName ) LAL_CALL( LALDestroyFrCache( &status, &frInCache ), 
      &status );

  /* write the raw channel data as read in from the frame files */
  if ( writeRawData ) outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
      &chan, "ct", "RAW" );

  if ( vrbflg ) fprintf( stdout, "read channel %s from frame stream\n"
      "got %d points with deltaT %e\nstarting at GPS time %d sec %d ns\n", 
      chan.name, chan.data->length, chan.deltaT, 
      chan.epoch.gpsSeconds, chan.epoch.gpsNanoSeconds );


  /*
   *
   * generate the response function for the requested time
   *
   */


  /* create storage for the response function */
  memset( &resp, 0, sizeof(COMPLEX8FrequencySeries) );
  LAL_CALL( LALCCreateVector( &status, &(resp.data), numPoints / 2 + 1 ), 
      &status );

  /* set the parameters of the response to match the data */
  resp.epoch.gpsSeconds = chan.epoch.gpsSeconds + padData;
  resp.epoch.gpsNanoSeconds = chan.epoch.gpsNanoSeconds;
  resp.deltaF = (REAL8) sampleRate / (REAL8) numPoints;
  resp.sampleUnits = strainPerCount;
  strcpy( resp.name, chan.name );

  /* generate the response function for the current time */
  if ( vrbflg ) fprintf( stdout, "generating response at time %d sec %d ns\n",
      resp.epoch.gpsSeconds, resp.epoch.gpsNanoSeconds );
  /* determine length of chunk */
  durationNS = gpsEndTimeNS - gpsStartTimeNS;
  LAL_CALL( LALINT8toGPS( &status, &duration, 
	&durationNS ), &status );
  LAL_CALL( LALExtractFrameResponse( &status, &resp, calCacheName, ifo, 
	&duration ), &status );

  if ( writeResponse ) outFrame = fr_add_proc_COMPLEX8FrequencySeries( 
      outFrame, &resp, "strain/ct", "RESPONSE" );


  /* slide the channel back to the fake time for background studies */
  chan.epoch.gpsSeconds += slideData.gpsSeconds;
  chan.epoch.gpsNanoSeconds += slideData.gpsNanoSeconds;
  

  /*
   *
   * inject signals into the raw, unresampled data
   *
   */


  if ( injectionFile )
  {
    /* get injections within 500 seconds of either end of the segment.   */
    /* a 0.4,0.4 MACHO starting at 30.0 Hz has length 435.374683 seconds */
    /* so this should be plenty of safety. better to waste cpu than miss */
    /* injected signals...                                               */
    INT4 injSafety = 500;
    int  numInjections = 0;
    SimInspiralTable    *injections = NULL;
    SimInspiralTable    *thisInj = NULL;
    CHAR tmpChName[LALNameLength];

    /* read in the injection data from XML */
    numInjections = SimInspiralTableFromLIGOLw( &injections, injectionFile,
        gpsStartTime.gpsSeconds - injSafety, 
        gpsEndTime.gpsSeconds + injSafety );

    if ( numInjections < 0 )
    {
      fprintf( stderr, "error: cannot read injection file" );
      exit( 1 );
    }
    else if ( numInjections )
    {
      /* see if we need a higher resolution response to do the injections */
      if ( resampleChan )
      {
        /* we need a different resolution of response function for injections */
        UINT4 rateRatio = floor( resampleParams.deltaT / chan.deltaT + 0.5 );
        UINT4 rawNumPoints = rateRatio * numPoints;

        if ( vrbflg ) fprintf( stdout, "rateRatio = %d\nrawNumPoints = %d\n"
            "chan.deltaT = %e\n", rateRatio, rawNumPoints, chan.deltaT );

        memset( &injResp, 0, sizeof(COMPLEX8FrequencySeries) );
        LAL_CALL( LALCCreateVector( &status, &(injResp.data), 
              rawNumPoints / 2 + 1 ), &status );
        injResp.epoch = resp.epoch;
        injResp.deltaF = 1.0 / ( rawNumPoints * chan.deltaT );
        injResp.sampleUnits = strainPerCount;
        strcpy( injResp.name, chan.name );

        /* generate the response function for the current time */
        if ( vrbflg ) fprintf( stdout, 
            "generating high resolution response at time %d sec %d ns\n"
            "length = %d points, deltaF = %e Hz\n",
            resp.epoch.gpsSeconds, resp.epoch.gpsNanoSeconds,
            injResp.data->length, injResp.deltaF );
        LAL_CALL( LALExtractFrameResponse( &status, &injResp, calCacheName, 
              ifo, &duration ), &status );

        injRespPtr = &injResp;

        if ( writeResponse ) outFrame = fr_add_proc_COMPLEX8FrequencySeries( 
            outFrame, &injResp, "strain/ct", "RESPONSE_INJ" );
      }
      else
      {
        /* the data is already at the correct sample rate, just do injections */
        injRespPtr = &resp;
        memset( &injResp, 0, sizeof(COMPLEX8FrequencySeries) );
      }

      /* inject the signals, preserving the channel name (Tev mangles it) */
      LALSnprintf( tmpChName, LALNameLength * sizeof(CHAR), "%s", chan.name );

      /* if injectOverhead option, then set chan.name to "ZENITH".  
       * This causes no detector site to be found in the injection code so
       * that the injection is done directly overhead (i.e. with a response 
       * function of F+ = 1; Fx = 0) */
      if ( injectOverhead )
      {
        LALSnprintf( chan.name, LALNameLength * sizeof(CHAR), "ZENITH" );
      }

      LAL_CALL( LALFindChirpInjectSignals( &status, &chan, injections, 
            injRespPtr ), &status );
      LALSnprintf( chan.name,  LALNameLength * sizeof(CHAR), "%s", tmpChName );

      if ( vrbflg ) fprintf( stdout, "injected %d signals from %s into %s\n", 
          numInjections, injectionFile, chan.name );

      while ( injections )
      {
        thisInj = injections;
        injections = injections->next;
        LALFree( thisInj );
      }

      /* write the raw channel data plus injections to the output frame file */
      if ( writeRawData ) outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
          &chan, "ct", "RAW_INJ" );

      if ( injResp.data )
        LAL_CALL( LALCDestroyVector( &status, &(injResp.data) ), &status );
    }
    else
    {
      if ( vrbflg ) fprintf( stdout, "no injections in this chunk\n" );
    }
  }

  
  /*
   *
   * resample the data to the requested rate
   *
   */


  if ( resampleChan )
  {
    if (vrbflg) fprintf( stdout, "resampling input data from %e to %e\n",
        chan.deltaT, resampleParams.deltaT );

    LAL_CALL( LALResampleREAL4TimeSeries( &status, &chan, &resampleParams ),
        &status );

    if ( vrbflg ) fprintf( stdout, "channel %s resampled:\n"
        "%d points with deltaT %e\nstarting at GPS time %d sec %d ns\n", 
        chan.name, chan.data->length, chan.deltaT, 
        chan.epoch.gpsSeconds, chan.epoch.gpsNanoSeconds );

    /* write the resampled channel data as read in from the frame files */
    if ( writeRawData ) outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
        &chan, "ct", "RAW_RESAMP" );
  }

  /* store the filter data sample rate */
  this_search_summvar = this_search_summvar->next = 
    (SearchSummvarsTable *) LALCalloc( 1, sizeof(SearchSummvarsTable) );
  LALSnprintf( this_search_summvar->name, LIGOMETA_NAME_MAX * sizeof(CHAR),
      "filter data sample rate" );
  this_search_summvar->value = chan.deltaT;


  /* 
   *
   * high pass the data, removed pad from time series and check length of data
   *
   */


  /* iir filter to remove low frequencies from data channel */
  if ( highPass )
  {
    PassBandParamStruc highpassParam;
    highpassParam.nMax = highPassOrder;
    highpassParam.f1 = -1.0;
    highpassParam.f2 = (REAL8) highPassFreq;
    highpassParam.a1 = -1.0;
    highpassParam.a2 = (REAL8)(1.0 - highPassAtten); /* a2 is not attenuation */

    if ( vrbflg ) fprintf( stdout, "applying %d order high pass: "
        "%3.2f of signal passes at %4.2f Hz\n", 
        highpassParam.nMax, highpassParam.a2, highpassParam.f2 );

    LAL_CALL( LALButterworthREAL4TimeSeries( &status, &chan, &highpassParam ),
        &status );
  }

  /* remove pad from requested data from start and end of time series */
  memmove( chan.data->data, chan.data->data + padData * sampleRate, 
      (chan.data->length - 2 * padData * sampleRate) * sizeof(REAL4) );
  LALRealloc( chan.data->data, 
      (chan.data->length - 2 * padData * sampleRate) * sizeof(REAL4) );
  chan.data->length -= 2 * padData * sampleRate;
  chan.epoch.gpsSeconds += padData;

  if ( vrbflg ) fprintf( stdout, "after removal of %d second padding at "
      "start and end:\ndata channel sample interval (deltaT) = %e\n"
      "data channel length = %d\nstarting at %d sec %d ns\n", 
      padData , chan.deltaT , chan.data->length, 
      chan.epoch.gpsSeconds, chan.epoch.gpsNanoSeconds );

  if ( writeFilterData ) outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
      &chan, "ct", "FILTER" );

  /* check data length */
  if ( chan.data->length != inputDataLength )
  {
    fprintf( stderr, "error: computed channel length and requested\n"
        "input data length do not match:\nchan.data->length = %d\n"
        "inputDataLength = %d\nyou have found a bug in the code.\n"
        "please report this to <duncan@gravity.phys.uwm.edu>\n",
        chan.data->length, inputDataLength );
    exit( 1 );
  }

  /* store the start and end time of the filter channel in the search summ */
  /* noting that we don't look for events in the first and last quarter    */
  /* of each findchirp segment of the input data                           */
  LAL_CALL( LALGPStoFloat( &status, &tsLength, &(chan.epoch) ), 
      &status );
  tsLength += (REAL8) (numPoints / 4) * chan.deltaT;
  LAL_CALL( LALFloatToGPS( &status, 
        &(searchsumm.searchSummaryTable->out_start_time), &tsLength ), 
      &status );

  if ( trigStartTimeNS )
  {
    /* override with trigger start time */
    LAL_CALL( LALINT8toGPS( &status, 
          &(searchsumm.searchSummaryTable->out_start_time), 
          &trigStartTimeNS ), &status );
  }

  LAL_CALL( LALGPStoFloat( &status, &tsLength, &(chan.epoch) ), 
      &status );
  tsLength += chan.deltaT * ((REAL8) chan.data->length - (REAL8) (numPoints/4));
  LAL_CALL( LALFloatToGPS( &status, 
        &(searchsumm.searchSummaryTable->out_end_time), &tsLength ), 
      &status );

  if ( trigEndTimeNS )
  {
    /* override with trigger end time */
    LAL_CALL( LALINT8toGPS( &status, 
          &(searchsumm.searchSummaryTable->out_end_time), 
          &trigEndTimeNS ), &status );
  }


  /* 
   *
   * create and populate findchip initialization structure 
   *
   */


  if ( ! ( fcInitParams = (FindChirpInitParams *) 
        LALCalloc( 1, sizeof(FindChirpInitParams) ) ) )
  {
    fprintf( stderr, "could not allocate memory for findchirp init params\n" );
    exit( 1 );
  }
  fcInitParams->numPoints      = numPoints;
  fcInitParams->numSegments    = numSegments;
  fcInitParams->numChisqBins   = numChisqBins;
  fcInitParams->createRhosqVec = writeRhosq;
  fcInitParams->ovrlap         = ovrlap;
  fcInitParams->approximant    = approximant;

  /* create the data segment vector */
  memset( &spec, 0, sizeof(REAL4FrequencySeries) );
  LAL_CALL( LALSCreateVector( &status, &(spec.data), numPoints / 2 + 1 ), 
      &status );
  LAL_CALL( LALInitializeDataSegmentVector( &status, &dataSegVec,
        &chan, &spec, &resp, fcInitParams ), &status );


  /*
   *
   * power spectrum estimation and data conditioning
   *
   */


  /* create the findchirp data storage */
  LAL_CALL( LALCreateFindChirpSegmentVector( &status, &fcSegVec, 
        fcInitParams ), &status );

  /* initialize data conditioning routines */
  LAL_CALL( LALFindChirpSPDataInit( &status, &fcDataParams, fcInitParams ), 
      &status );
  fcDataParams->invSpecTrunc = invSpecTrunc * sampleRate;
  fcDataParams->fLow = fLow;

  /* compute the windowed power spectrum for the data channel */
  avgSpecParams.window = NULL;
  avgSpecParams.plan   = fcDataParams->fwdPlan;
  switch ( specType )
  {
    case 0:
      avgSpecParams.method = useMean;
      if ( vrbflg ) fprintf( stdout, "computing mean psd" );
      break;
    case 1:
      avgSpecParams.method = useMedian;
      if ( vrbflg ) fprintf( stdout, "computing median psd" );
      break;
  }

  wpars.type = Hann;
  wpars.length = numPoints;
  if ( badMeanPsd )
  {
    avgSpecParams.overlap = 0;
    if ( vrbflg ) fprintf( stdout, " without overlap\n" );
  }
  else
  {
    avgSpecParams.overlap = numPoints / 2;
    if ( vrbflg ) 
      fprintf( stdout, " with overlap %d\n", avgSpecParams.overlap );
  }

  LAL_CALL( LALCreateREAL4Window( &status, &(avgSpecParams.window),
        &wpars ), &status );
  LAL_CALL( LALREAL4AverageSpectrum( &status, &spec, &chan, &avgSpecParams ),
      &status );
  LAL_CALL( LALDestroyREAL4Window( &status, &(avgSpecParams.window) ), 
      &status );
  strcpy( spec.name, chan.name );

  /* write the spectrum data to a file */
  if ( writeSpectrum ) outFrame = fr_add_proc_REAL4FrequencySeries( outFrame, 
      &spec, "ct/sqrtHz", "PSD" );


  /*
   *
   * create the data structures needed for findchirp
   *
   */


  if ( vrbflg ) fprintf( stdout, "initializing findchirp\n" );

  /* initialize the template functions */
  LAL_CALL( LALFindChirpSPTemplateInit( &status, &fcTmpltParams, 
        fcInitParams ), &status );

  fcDataParams->dynRange = fcTmpltParams->dynRange = 
    pow( 2.0, dynRangeExponent );
  fcDataParams->deltaT = fcTmpltParams->deltaT = 1.0 / (REAL4) sampleRate;
  fcTmpltParams->fLow = fLow;

  /* initialize findchirp filter functions */
  LAL_CALL( LALFindChirpFilterInit( &status, &fcFilterParams, fcInitParams ), 
      &status );
  fcFilterParams->deltaT = 1.0 / (REAL4) sampleRate;
  fcFilterParams->computeNegFreq = 0;

  LAL_CALL( LALCreateFindChirpInput( &status, &fcFilterInput, fcInitParams ), 
      &status );
  LAL_CALL( LALFindChirpChisqVetoInit( &status, fcFilterParams->chisqParams, 
        fcInitParams->numChisqBins, fcInitParams->numPoints ), 
      &status );

  /* parse the thresholds */
  fcFilterParams->rhosqThresh = snrThresh * snrThresh;
  fcFilterParams->chisqThresh = chisqThresh;
  fcFilterParams->maximiseOverChirp = eventCluster;


  /*
   *
   * condition data segments for filtering
   *
   */


  if ( approximant == TaylorF2 )
  {
    if ( vrbflg ) fprintf( stdout, "findchirp conditioning data for SP\n" );
    LAL_CALL( LALFindChirpSPData (&status, fcSegVec, dataSegVec, fcDataParams),
        &status );
  }
  else if ( approximant == BCV )
  {
    if ( vrbflg ) fprintf( stdout, "findchirp conditioning data for BCV\n" );
    LAL_CALL( LALFindChirpBCVData (&status, fcSegVec, dataSegVec, fcDataParams),
        &status );
  }
  else
  {
    fprintf( stderr, "error: unknown waveform approximant for data\n" );
    exit( 1 );
  }

  /* compute the standard candle */
  {
      REAL4 cannonDist = 1.0; /* Mpc */
      REAL4 m  = (REAL4) candle.tmplt.totalMass;
      REAL4 mu = (REAL4) candle.tmplt.mu;
      REAL4 distNorm = 2.0 * LAL_MRSUN_SI / (cannonDist * 1e6 * LAL_PC_SI);
      REAL4 candleTmpltNorm = sqrt( (5.0*mu) / 96.0 ) *
        pow( m / (LAL_PI*LAL_PI) , 1.0/3.0 ) *
        pow( LAL_MTSUN_SI / (REAL4) chan.deltaT, -1.0/6.0 );

      distNorm *= fcTmpltParams->dynRange;
      candleTmpltNorm *= candleTmpltNorm;
      candleTmpltNorm *= distNorm * distNorm;

      candle.sigmasq = 4.0 * ( (REAL4) chan.deltaT / (REAL4) numPoints );
      candle.sigmasq *= candleTmpltNorm * fcSegVec->data->segNorm;

      candle.effDistance = sqrt( candle.sigmasq / candle.rhosq );

      if ( vrbflg ) 
      {
        fprintf( stdout, "candle m = %e\ncandle mu = %e\n"
            "candle.rhosq = %e\nchan.deltaT = %e\n"
            "numPoints = %d\nfcSegVec->data->segNorm = %e\n"
            "candleTmpltNorm = %e\ncandle.effDistance = %e Mpc\n"
            "candle.sigmasq = %e\n",
            m, mu, candle.rhosq, chan.deltaT, numPoints, 
            fcSegVec->data->segNorm, candleTmpltNorm, candle.effDistance,
            candle.sigmasq );
        fflush( stdout );
      }
    }
  

  /*
   *
   * hierarchial search engine
   *
   */


  for ( tmpltCurrent = tmpltHead, inserted = 0; tmpltCurrent; 
      tmpltCurrent = tmpltCurrent->next, inserted = 0 )
  {
    /*  generate template */
    if ( approximant == TaylorF2 )
    {
      LAL_CALL( LALFindChirpSPTemplate( &status, fcFilterInput->fcTmplt, 
            tmpltCurrent->tmpltPtr, fcTmpltParams ), &status );
      fcFilterInput->tmplt = tmpltCurrent->tmpltPtr;
    }
    else if ( approximant == BCV )
    {
      LAL_CALL( LALFindChirpBCVTemplate( &status, fcFilterInput->fcTmplt, 
            tmpltCurrent->tmpltPtr, fcTmpltParams ), &status );
      fcFilterInput->tmplt = tmpltCurrent->tmpltPtr;
    }
    else
    {
      fprintf( stderr, "error: unknown waveform approximant for template\n" );
      exit( 1 );
    }

    /* loop over data segments */
    for ( i = 0; i < fcSegVec->length ; ++i )
    {
      INT8 fcSegStartTimeNS;
      INT8 fcSegEndTimeNS;

      LAL_CALL( LALGPStoINT8( &status, &fcSegStartTimeNS, 
            &(fcSegVec->data[i].data->epoch) ), &status );
      fcSegEndTimeNS = fcSegStartTimeNS + (INT8)
        ( (REAL8) numPoints * 1e9 * fcSegVec->data[i].deltaT );

      /* skip segment if it is not contained in the trig start or end times */
      if ( (trigStartTimeNS && (trigStartTimeNS > fcSegEndTimeNS)) || 
          (trigEndTimeNS && (trigEndTimeNS < fcSegStartTimeNS)) )
      { 
        if ( vrbflg ) fprintf( stdout, 
            "skipping segment %d/%d [%lld-%lld] (outside trig time)\n", 
            fcSegVec->data[i].number, fcSegVec->length, 
            fcSegStartTimeNS, fcSegEndTimeNS );

        continue;
      }
      
      /* filter data segment */ 
      if ( fcSegVec->data[i].level == tmpltCurrent->tmpltPtr->level )
      {
        if ( vrbflg ) fprintf( stdout, 
            "filtering segment %d/%d [%lld-%lld] "
            "against template %d/%d (%e,%e)\n", 
            fcSegVec->data[i].number,  fcSegVec->length,
            fcSegStartTimeNS, fcSegEndTimeNS,
            tmpltCurrent->tmpltPtr->number, numTmplts,
            fcFilterInput->tmplt->mass1, fcFilterInput->tmplt->mass2 );

        fcFilterInput->segment = fcSegVec->data + i;

        if ( approximant == TaylorF2 )
        {
          LAL_CALL( LALFindChirpFilterSegment( &status, 
                &eventList, fcFilterInput, fcFilterParams ), &status );
        }
        else if ( approximant == BCV )
        {
          LAL_CALL( LALFindChirpBCVFilterSegment( &status,
                &eventList, fcFilterInput, fcFilterParams ), &status );
        }
        else
        {
          fprintf( stderr, "error: unknown waveform approximant for filter\n" );
          exit( 1 );
        }

        if ( writeRhosq )
        {
          CHAR snrsqStr[LALNameLength];
          LALSnprintf( snrsqStr, LALNameLength*sizeof(CHAR), 
              "SNRSQ_%d", nRhosqFr++ );
          strcpy( fcFilterParams->rhosqVec->name, chan.name );
          outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
              fcFilterParams->rhosqVec, "none", snrsqStr );
        }

        if ( writeChisq )
        {
          CHAR chisqStr[LALNameLength];
          REAL4TimeSeries chisqts;
          LALSnprintf( chisqStr, LALNameLength*sizeof(CHAR), 
              "CHISQ_%d", nChisqFr++ );
          chisqts.epoch = fcFilterInput->segment->data->epoch;
          memcpy( &(chisqts.name), fcFilterInput->segment->data->name,
              LALNameLength * sizeof(CHAR) );
          chisqts.deltaT = fcFilterInput->segment->deltaT;
          chisqts.data = fcFilterParams->chisqVec;
          outFrame = fr_add_proc_REAL4TimeSeries( outFrame, 
              &chisqts, "none", chisqStr );
        }

      }
      else
      {
        if ( vrbflg ) fprintf( stdout, "skipping segment %d/%d [%lld-%lld] "
            "(segment level %d, template level %d)\n", 
            fcSegVec->data[i].number, fcSegVec->length, 
            fcSegStartTimeNS, fcSegEndTimeNS,
            fcSegVec->data[i].level, tmpltCurrent->tmpltPtr->level );
      }

      /*  test if filter returned any events */
      if ( eventList )
      {
        if ( vrbflg ) fprintf( stdout, 
            "segment %d rang template [m (%e,%e)] [psi (%e,%e)]\n",
            fcSegVec->data[i].number,
            fcFilterInput->tmplt->mass1, fcFilterInput->tmplt->mass2,
            fcFilterInput->tmplt->psi0, fcFilterInput->tmplt->psi3 );

        if ( tmpltCurrent->tmpltPtr->fine != NULL && inserted == 0 )
        {
          if ( vrbflg ) fprintf( stdout, 
              "inserting fine templates into list\n" );

          tmpltInsert = tmpltCurrent;
          inserted = 1;
          fcSegVec->data[i].level += 1;

          for ( bankCurrent = tmpltCurrent->tmpltPtr->fine ; 
              bankCurrent; bankCurrent = bankCurrent->next )
          {
            LAL_CALL( LALFindChirpCreateTmpltNode( &status, 
                  bankCurrent, &tmpltInsert ), &status );
          }

        }
        else if ( ! tmpltCurrent->tmpltPtr->fine )
        {
          if ( vrbflg ) fprintf( stdout, "***>  dumping events  <***\n" );

          if ( ! savedEvents.snglInspiralTable )
          {
            savedEvents.snglInspiralTable = eventList;
          }
          else
          {
            event->next = eventList;
          }
        }
        else
        {
          if ( vrbflg ) fprintf( stdout, 
              "already inserted fine templates, skipping\n" ); 

          fcSegVec->data[i].level += 1;
        } 

        /* save a pointer to the last event in the list and count the events */
        ++numEvents;
        while ( eventList->next )
        {
          eventList = eventList->next;
          ++numEvents;
        }
        event = eventList;
        eventList = NULL;
      } /* end if ( events ) */

      /* if going up a level, remove inserted nodes, reset segment levels */ 
      if ( tmpltCurrent->next && (tmpltCurrent->next->tmpltPtr->level < 
            tmpltCurrent->tmpltPtr->level) )
      {
        /* record the current number */
        currentLevel = tmpltCurrent->tmpltPtr->level;

        /* decrease segment filter levels if the have been increased */
        for ( i = 0 ; i < fcSegVec->length; i++ )
        {
          if ( fcSegVec->data[i].level == currentLevel )
          {
            fcSegVec->data[i].level -= 1;
          }
        }

        if ( vrbflg ) fprintf( stdout, "removing inserted fine templates\n" ); 

        while ( tmpltCurrent->tmpltPtr->level == currentLevel )
        {
          LAL_CALL( LALFindChirpDestroyTmpltNode( &status, &tmpltCurrent ),
              &status );
        }          
      } /* end if up a level */

    } /* end loop over data segments */

  } /* end loop over linked list */

  /* save the number of events in the search summary table */
  searchsumm.searchSummaryTable->nevents = numEvents;


  /*
   *
   * free memory used by filtering code
   *
   */


  if ( vrbflg ) fprintf( stdout, "freeing memory\n" );

  /* free memory used by findchirp */
  LAL_CALL( LALFindChirpChisqVetoFinalize( &status, 
        fcFilterParams->chisqParams, fcInitParams->numChisqBins ), 
      &status );
  LAL_CALL( LALDestroyFindChirpInput( &status, &fcFilterInput ), 
      &status );
  LAL_CALL( LALFindChirpFilterFinalize( &status, &fcFilterParams ), 
      &status );
  LAL_CALL( LALFindChirpSPTemplateFinalize( &status, &fcTmpltParams ), 
      &status );
  LAL_CALL( LALFindChirpSPDataFinalize( &status, &fcDataParams ),
      &status );
  LAL_CALL( LALDestroyFindChirpSegmentVector( &status, &fcSegVec ),
      &status );
  LALFree( fcInitParams );

  /* free the template bank */
  while ( bankHead )
  {
    bankCurrent = bankHead;
    bankHead = bankHead->next;
    LALFree( bankCurrent );
    bankCurrent = NULL;
  }
  /* destroy linked list of template nodes */
  while ( tmpltHead )
  {
    LAL_CALL( LALFindChirpDestroyTmpltNode( &status, &tmpltHead ), &status );
  }

  /* free the data storage */
  LAL_CALL( LALFinalizeDataSegmentVector( &status, &dataSegVec ), &status );
  LAL_CALL( LALSDestroyVector( &status, &(chan.data) ), &status );
  LAL_CALL( LALSDestroyVector( &status, &(spec.data) ), &status );
  LAL_CALL( LALCDestroyVector( &status, &(resp.data) ), &status );


  /*
   *
   * write the result results to disk
   *
   */



  if ( vrbflg ) fprintf( stdout, "writing frame data to disk\n" );

  /* write the output frame */
  if ( writeRawData || writeFilterData || writeResponse || writeSpectrum ||
      writeRhosq || writeChisq )
  {
    if ( userTag )
    {
      LALSnprintf( fname, sizeof(fname), "%s-INSPIRAL_%s-%d-%d.gwf",
	  ifo, userTag, gpsStartTime.gpsSeconds,
	  gpsEndTime.gpsSeconds - gpsStartTime.gpsSeconds );
    }
    else
    {
      LALSnprintf( fname, sizeof(fname), "%s-INSPIRAL-%d-%d.gwf",
	  ifo, gpsStartTime.gpsSeconds,
	  gpsEndTime.gpsSeconds - gpsStartTime.gpsSeconds );
    }
    frOutFile = FrFileONew( fname, 0 );
    FrameWrite( outFrame, frOutFile );
    FrFileOEnd( frOutFile );
  }

cleanexit:

  if ( vrbflg ) fprintf( stdout, "writing xml data to disk\n" );

  /* open the output xml file */
  memset( &results, 0, sizeof(LIGOLwXMLStream) );
  if ( userTag )
  {
    LALSnprintf( fname, sizeof(fname), "%s-INSPIRAL_%s-%d-%d.xml",
        ifo, userTag, gpsStartTime.gpsSeconds,
        gpsEndTime.gpsSeconds - gpsStartTime.gpsSeconds );
  }
  else
  {
    LALSnprintf( fname, sizeof(fname), "%s-INSPIRAL-%d-%d.xml",
        ifo, gpsStartTime.gpsSeconds,
        gpsEndTime.gpsSeconds - gpsStartTime.gpsSeconds );
  }
  LAL_CALL( LALOpenLIGOLwXMLFile( &status, &results, fname), &status );

  /* write the process table */
  LALSnprintf( proctable.processTable->ifos, LIGOMETA_IFOS_MAX, "%s", ifo );
  LAL_CALL( LALGPSTimeNow ( &status, &(proctable.processTable->end_time),
        &accuracy ), &status );
  LAL_CALL( LALBeginLIGOLwXMLTable( &status, &results, process_table ), 
      &status );
  LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, proctable, 
        process_table ), &status );
  LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
  free( proctable.processTable );

  /* write the process params table */
  LAL_CALL( LALBeginLIGOLwXMLTable( &status, &results, process_params_table ), 
      &status );
  LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, procparams, 
        process_params_table ), &status );
  LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
  while( procparams.processParamsTable )
  {
    this_proc_param = procparams.processParamsTable;
    procparams.processParamsTable = this_proc_param->next;
    free( this_proc_param );
  }
  
  /* write the search summary table */
  LAL_CALL( LALBeginLIGOLwXMLTable( &status, &results, 
        search_summary_table ), &status );
  LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, searchsumm, 
        search_summary_table ), &status );
  LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
  
  /* write the search summvars table */
  if ( numTmplts )
  {
    LAL_CALL( LALBeginLIGOLwXMLTable( &status, &results, 
          search_summvars_table ), &status );
    LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, searchsummvars, 
          search_summvars_table ), &status );
    LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
  }
  while( searchsummvars.searchSummvarsTable )
  {
    this_search_summvar = searchsummvars.searchSummvarsTable;
    searchsummvars.searchSummvarsTable = this_search_summvar->next;
    LALFree( this_search_summvar );
  }

  /* write the summvalue table */
  if ( numTmplts )
  {
    LALSnprintf( summvalue.summValueTable->program, LIGOMETA_PROGRAM_MAX, 
        "%s", PROGRAM_NAME );
    summvalue.summValueTable->version = 0;
    summvalue.summValueTable->start_time = 
	searchsumm.searchSummaryTable->out_start_time;
    summvalue.summValueTable->end_time = 
	searchsumm.searchSummaryTable->out_end_time;
    LALSnprintf( summvalue.summValueTable->ifo, LIGOMETA_IFO_MAX, "%s", ifo );
    LALSnprintf( summvalue.summValueTable->name, LIGOMETA_SUMMVALUE_NAME_MAX, 
        "%s", "inspiral_effective_distance" );
    LALSnprintf( summvalue.summValueTable->comment, LIGOMETA_SUMMVALUE_COMM_MAX, 
        "%s", "1.4_1.4_8" );
    summvalue.summValueTable->value = candle.effDistance;
    LAL_CALL( LALBeginLIGOLwXMLTable( &status, &results, summ_value_table ), 
        &status );
    LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, summvalue, 
          summ_value_table ), &status );
    LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
  }

  /* free the search summary table */
  free( searchsumm.searchSummaryTable );
  
  /* write the sngl_inspiral triggers to the output xml */
  if ( savedEvents.snglInspiralTable )
  {
    SnglInspiralTable *tmpEventHead = NULL;
    SnglInspiralTable *lastEvent = NULL;

    /* sort the inspiral events by time */
    if ( vrbflg ) fprintf( stdout, "sorting events by time... " );
    LAL_CALL( LALSortSnglInspiral( &status, &(savedEvents.snglInspiralTable),
          LALCompareSnglInspiralByTime), &status );
    if ( vrbflg ) fprintf( stdout, "done\n" );

    /* discard any triggers outside the trig start/end time window */
    event = savedEvents.snglInspiralTable;
    if ( trigStartTimeNS || trigEndTimeNS )
    {
      if ( vrbflg ) fprintf( stdout, 
          "discarding triggers outside trig start/end time... " );

      while ( event )
      {
        INT8 trigTimeNS;
        LAL_CALL( LALGPStoINT8( &status, &trigTimeNS, &(event->end_time) ), 
            &status );

        if ( ! ( ! trigTimeNS || (trigTimeNS >= trigStartTimeNS) ) && 
            ( ! trigEndTimeNS || (trigTimeNS < trigEndTimeNS) ) )
        {
          /* throw this trigger away */
          SnglInspiralTable *tmpEvent = event;

          if ( lastEvent )
          {
            lastEvent->next = event->next;
          }

          /* increment the linked list by one and free the event */
          event = event->next;
          LALFree( tmpEvent );
        }
        else 
        {
          /* store the first event as the head of the new linked list */
          if ( ! tmpEventHead ) tmpEventHead = event;

          /* save the last event and increment the linked list by one */
          lastEvent = event;
          event = event->next;
        }
      }

      savedEvents.snglInspiralTable = tmpEventHead;

      if ( vrbflg ) fprintf( stdout, "done\n" );
    }
    
    /* if we haven't thrown all the triggers away, write sngl_inspiral table */
    if ( savedEvents.snglInspiralTable )
    {
      LAL_CALL( LALBeginLIGOLwXMLTable( &status, 
            &results, sngl_inspiral_table ), &status );
      LAL_CALL( LALWriteLIGOLwXMLTable( &status, &results, savedEvents, 
            sngl_inspiral_table ), &status );
      LAL_CALL( LALEndLIGOLwXMLTable ( &status, &results ), &status );
    }
  }
  while ( savedEvents.snglInspiralTable )
  {
    event = savedEvents.snglInspiralTable;
    savedEvents.snglInspiralTable = savedEvents.snglInspiralTable->next;
    LALFree( event );
  }

  /* close the output xml file */
  LAL_CALL( LALCloseLIGOLwXMLFile ( &status, &results ), &status );

  /* free the rest of the memory, check for memory leaks and exit */
  if ( injectionFile ) free ( injectionFile ); 
  if ( calCacheName ) free( calCacheName );
  if ( frInCacheName ) free( frInCacheName );
  if ( bankFileName ) free( bankFileName );
  if ( channelName ) free( channelName );
  if ( fqChanName ) free( fqChanName );

  if ( vrbflg ) fprintf( stdout, "checking memory leaks and exiting\n" );
  LALCheckMemoryLeaks();
  exit( 0 );
}

/* ------------------------------------------------------------------------- */

#define ADD_PROCESS_PARAM( pptype, format, ppvalue ) \
this_proc_param = this_proc_param->next = (ProcessParamsTable *) \
  calloc( 1, sizeof(ProcessParamsTable) ); \
  LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, "%s", \
      PROGRAM_NAME ); \
      LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, "--%s", \
          long_options[option_index].name ); \
          LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "%s", pptype ); \
          LALSnprintf( this_proc_param->value, LIGOMETA_VALUE_MAX, format, ppvalue );

#define USAGE \
"lalapps_inspiral [options]\n\n"\
"  --help                       display this message\n"\
"  --verbose                    print progress information\n"\
"  --version                    print version information and exit\n"\
"  --debug-level LEVEL          set the LAL debug level to LEVEL\n"\
"  --user-tag STRING            set the process_params usertag to STRING\n"\
"  --comment STRING             set the process table comment to STRING\n"\
"\n"\
"  --gps-start-time SEC         GPS second of data start time\n"\
"  --gps-start-time-ns NS       GPS nanosecond of data start time\n"\
"  --gps-end-time SEC           GPS second of data end time\n"\
"  --gps-end-time-ns NS         GPS nanosecond of data end time\n"\
"  --pad-data T                 pad the data start and end time by T seconds\n"\
"  --slide-time T               slide data start epoch by T seconds\n"\
"  --slide-time-ns T            slide data start epoch by T nanoseconds\n"\
"\n"\
"  --frame-cache                obtain frame data from LAL frame cache FILE\n"\
"  --calibration-cache FILE     obtain calibration from LAL frame cache FILE\n"\
"  --channel-name CHAN          read data from interferometer channel CHAN\n"\
"\n"\
"  --injection-file FILE        inject simulated inspiral signals from FILE\n"\
"\n"\
"  --inject-overhead            inject signals from overhead detector\n"\
"  --bank-file FILE             read template bank parameters from FILE\n"\
"  --minimal-match M            override bank minimal match with M (sets delta)\n"\
"  --start-template N           start filtering at template number N in bank\n"\
"  --stop-templateN             stop filtering at template number N in bank\n"\
"\n"\
"  --sample-rate F              filter data at F Hz, downsampling if necessary\n"\
"  --resample-filter TYPE       set resample filter to TYPE (ldas|butterworth)\n"\
"\n"\
"  --disable-high-pass          turn off the IIR highpass filter\n"\
"  --enable-high-pass F         high pass data above F Hz using an IIR filter\n"\
"  --high-pass-order O          set the order of the high pass filter to O\n"\
"  --high-pass-attenuation A    set the attenuation of the high pass filter to A\n"\
"  --spectrum-type TYPE         use PSD estimator TYPE (mean|median)\n"\
"\n"\
"  --segment-length N           set data segment length to N points\n"\
"  --number-of-segments N       set number of data segments to N\n"\
"  --segment-overlap N          overlap data segments by N points\n"\
"\n"\
"  --low-frequency-cutoff F     do not filter below F Hz\n"\
"  --inverse-spec-length T      set length of inverse spectrum to T seconds\n"\
"  --dynamic-range-exponent X   set dynamic range scaling to 2^X\n"\
"\n"\
"  --approximant APPROX         set approximant of the waveform to APPROX\n"\
"                                 (TaylorF2|BCV)\n"\
"  --chisq-bins P               set number of chisq veto bins to P\n"\
"  --snr-threshold RHO          set signal-to-noise threshold to RHO\n"\
"  --chisq-threshold X          threshold on chi^2 < X * ( p + rho^2 * delta^2 )\n"\
"  --enable-event-cluster       turn on maximization over chirp length\n"\
"  --disable-event-cluster      turn off maximization over chirp length\n"\
"\n"\
"  --enable-output              write the results to a LIGO LW XML file\n"\
"  --disable-output             do not write LIGO LW XML output file\n"\
"  --trig-start-time SEC        only output triggers after GPS time SEC\n"\
"  --trig-end-time SEC          only output triggers before GPS time SEC\n"\
"\n"\
"  --write-raw-data             write raw data to a frame file\n"\
"  --write-filter-data          write data that is passed to filter to a frame\n"\
"  --write-response             write the computed response function to a frame\n"\
"  --write-spectrum             write the uncalibrated psd to a frame\n"\
"  --write-snrsq                write the snr time series for each data segment\n"\
"  --write-chisq                write the r^2 time series for each data segment\n"\
"\n"

int arg_parse_check( int argc, char *argv[], MetadataTable procparams )
{
  /* getopt arguments */
  struct option long_options[] =
  {
    /* these options set a flag */
    {"verbose",                 no_argument,       &vrbflg,           1 },
    {"enable-event-cluster",    no_argument,       &eventCluster,     1 },
    {"disable-event-cluster",   no_argument,       &eventCluster,     0 },
    {"enable-output",           no_argument,       &enableOutput,     1 },
    {"disable-output",          no_argument,       &enableOutput,     0 },
    {"disable-high-pass",       no_argument,       &highPass,         0 },
    {"inject-overhead",		no_argument,	   &injectOverhead,   1 },
    /* these options don't set a flag */
    {"gps-start-time",          required_argument, 0,                'a'},
    {"gps-start-time-ns",       required_argument, 0,                'A'},
    {"gps-end-time",            required_argument, 0,                'b'},
    {"gps-end-time-ns",         required_argument, 0,                'B'},
    {"channel-name",            required_argument, 0,                'c'},
    {"segment-length",          required_argument, 0,                'd'},
    {"trig-start-time",         required_argument, 0,                'C'},
    {"trig-end-time",           required_argument, 0,                'D'},
    {"number-of-segments",      required_argument, 0,                'e'},
    {"segment-overlap",         required_argument, 0,                'f'},
    {"sample-rate",             required_argument, 0,                'g'},
    {"help",                    no_argument,       0,                'h'},
    {"low-frequency-cutoff",    required_argument, 0,                'i'},
    {"spectrum-type",           required_argument, 0,                'j'},
    {"inverse-spec-length",     required_argument, 0,                'k'},
    {"dynamic-range-exponent",  required_argument, 0,                'l'},
    {"start-template",          required_argument, 0,                'm'},
    {"minimal-match",           required_argument, 0,                'M'},
    {"stop-template",           required_argument, 0,                'n'},
    {"chisq-bins",              required_argument, 0,                'o'},
    {"calibration-cache",       required_argument, 0,                'p'},
    {"approximant",             required_argument, 0,                'F'},
    {"snr-threshold",           required_argument, 0,                'q'},
    {"chisq-threshold",         required_argument, 0,                'r'},
    {"resample-filter",         required_argument, 0,                'R'},
    {"comment",                 required_argument, 0,                's'},
    {"enable-high-pass",        required_argument, 0,                't'},
    {"high-pass-order",         required_argument, 0,                'H'},
    {"high-pass-attenuation",   required_argument, 0,                'I'},
    {"frame-cache",             required_argument, 0,                'u'},
    {"bank-file",               required_argument, 0,                'v'},
    {"injection-file",          required_argument, 0,                'w'},
    {"pad-data",                required_argument, 0,                'x'},
    {"slide-time",              required_argument, 0,                'X'},
    {"slide-time-ns",           required_argument, 0,                'Y'},
    {"debug-level",             required_argument, 0,                'z'},
    {"user-tag",                required_argument, 0,                'Z'},
    {"userTag",                 required_argument, 0,                'Z'},
    {"version",                 no_argument,       0,                'V'},
    /* frame writing options */
    {"write-raw-data",          no_argument,       &writeRawData,     1 },
    {"write-filter-data",       no_argument,       &writeFilterData,  1 },
    {"write-response",          no_argument,       &writeResponse,    1 },
    {"write-spectrum",          no_argument,       &writeSpectrum,    1 },
    {"write-snrsq",             no_argument,       &writeRhosq,       1 },
    {"write-chisq",             no_argument,       &writeChisq,       1 },
    {0, 0, 0, 0}
  };
  int c;
  INT4 haveDynRange = 0;
  INT4 haveApprox = 0;
  ProcessParamsTable *this_proc_param = procparams.processParamsTable;
  LALStatus             status = blank_status;


  /*
   *
   * parse command line arguments
   *
   */


  while ( 1 )
  {
    /* getopt_long stores long option here */
    int option_index = 0;
    size_t optarg_len;

    c = getopt_long_only( argc, argv, 
        "a:A:b:B:c:d:e:f:g:h:i:j:k:l:m:M:n:o:p:F:q:r:R:s:t:H:I:u:v:w:x:z:Z:", 
        long_options, &option_index );

    /* detect the end of the options */
    if ( c == - 1 )
    {
      break;
    }

    switch ( c )
    {
      case 0:
        /* if this option set a flag, do nothing else now */
        if ( long_options[option_index].flag != 0 )
        {
          break;
        }
        else
        {
          fprintf( stderr, "error parsing option %s with argument %s\n",
              long_options[option_index].name, optarg );
          exit( 1 );
        }
        break;

      case 'a':
        {
          long int gstartt = atol( optarg );
          if ( gstartt < 441417609 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS start time is prior to " 
                "Jan 01, 1994  00:00:00 UTC:\n"
                "(%ld specified)\n",
                long_options[option_index].name, gstartt );
            exit( 1 );
          }
          if ( gstartt > 999999999 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS start time is after " 
                "Sep 14, 2011  01:46:26 UTC:\n"
                "(%ld specified)\n", 
                long_options[option_index].name, gstartt );
            exit( 1 );
          }
          gpsStartTimeNS += (INT8) gstartt * 1000000000LL;
          ADD_PROCESS_PARAM( "int", "%ld", gstartt );
        }
        break;

      case 'A':
        {
          long int gstarttns = atol( optarg );
          if ( gstarttns < 0 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS start time nanoseconds is negative\n",
                long_options[option_index].name );
            exit( 1 );
          }
          if ( gstarttns > 999999999 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS start time nanoseconds is greater than unity:\n" 
                "Must be <= 999999999 (%ld specified)\n", 
                long_options[option_index].name, gstarttns );
            exit( 1 );
          }
          gpsStartTimeNS += (INT8) gstarttns;
          ADD_PROCESS_PARAM( "int", "%ld", gstarttns );
        }
        break;

      case 'b':
        {
          long int gendt = atol( optarg );
          if ( gendt > 999999999 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS end time is after " 
                "Sep 14, 2011  01:46:26 UTC:\n"
                "(%ld specified)\n", 
                long_options[option_index].name, gendt );
            exit( 1 );
          }
          else if ( gendt < 441417609 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS end time is prior to " 
                "Jan 01, 1994  00:00:00 UTC:\n"
                "(%ld specified)\n", 
                long_options[option_index].name, gendt );
            exit( 1 );
          }            
          gpsEndTimeNS += (INT8) gendt * 1000000000LL;
          ADD_PROCESS_PARAM( "int", "%ld", gendt );
        }
        break;

      case 'B':
        {
          long int gendtns = atol( optarg );
          if ( gendtns < 0 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS end time nanoseconds is negative\n",
                long_options[option_index].name );
            exit( 1 );
          }
          else if ( gendtns > 999999999 )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "GPS end time nanoseconds is greater than unity:\n" 
                "Must be <= 999999999:\n"
                "(%ld specified)\n", 
                long_options[option_index].name, gendtns );
            exit( 1 );
          }            
          gpsEndTimeNS += (INT8) gendtns;
          ADD_PROCESS_PARAM( "int", "%ld", gendtns );
        }
        break;

      case 'c':
        {
          /* create storage for the channel name and copy it */
          char *channamptr = NULL;
          optarg_len = strlen( optarg ) + 1;
          fqChanName = (CHAR *) calloc( optarg_len, sizeof(CHAR) );
          memcpy( fqChanName, optarg, optarg_len );
          ADD_PROCESS_PARAM( "string", "%s", optarg );

          /* check that we have a proper channel name */
          if ( ! (channamptr = strstr( fqChanName, ":" ) ) )
          {
            fprintf( stderr, "invalid argument to --%s:\n"
                "channel name must be a full LIGO channel name "
                "e.g. L1:LSC-AS_Q\n(%s specified)\n",
                long_options[option_index].name, optarg );
            exit( 1 );
          }
          optarg_len = strlen( ++channamptr ) + 1;
          channelName = (CHAR *) calloc( optarg_len, sizeof(CHAR) );
          memcpy( channelName, channamptr, optarg_len );

          /* copy the first two characters to the ifo name */
          memset( ifo, 0, sizeof(ifo) );
          memcpy( ifo, optarg, sizeof(ifo) - 1 );
        }
        break;

      case 'd':
        numPoints = (INT4) atoi( optarg );
        if ( numPoints < 2 || numPoints % 2 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "number of points must be a non-zero power of 2: "
              "(%d specified) \n", 
              long_options[option_index].name, numPoints );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", numPoints );
        break;

      case 'C':
        {
          long int gstartt = atol( optarg );
          /* ignore a value of zero */
          if ( gstartt )
          {
            if ( gstartt < 441417609 )
            {
              fprintf( stderr, "invalid argument to --%s:\n"
                  "GPS start time is prior to " 
                  "Jan 01, 1994  00:00:00 UTC:\n"
                  "(%ld specified)\n",
                  long_options[option_index].name, gstartt );
              exit( 1 );
            }
            if ( gstartt > 999999999 )
            {
              fprintf( stderr, "invalid argument to --%s:\n"
                  "GPS start time is after " 
                  "Sep 14, 2011  01:46:26 UTC:\n"
                  "(%ld specified)\n", 
                  long_options[option_index].name, gstartt );
              exit( 1 );
            }
          trigStartTimeNS = (INT8) gstartt * 1000000000LL;
          }
          ADD_PROCESS_PARAM( "int", "%ld", gstartt );
        }
        break;

      case 'D':
        {
          long int gendt = atol( optarg );
          /* ignore a value of zero */
          if ( gendt )
          {
            if ( gendt > 999999999 )
            {
              fprintf( stderr, "invalid argument to --%s:\n"
                  "GPS end time is after " 
                  "Sep 14, 2011  01:46:26 UTC:\n"
                  "(%ld specified)\n", 
                  long_options[option_index].name, gendt );
              exit( 1 );
            }
            else if ( gendt < 441417609 )
            {
              fprintf( stderr, "invalid argument to --%s:\n"
                  "GPS end time is prior to " 
                  "Jan 01, 1994  00:00:00 UTC:\n"
                  "(%ld specified)\n", 
                  long_options[option_index].name, gendt );
              exit( 1 );
            }            
            trigEndTimeNS = (INT8) gendt * 1000000000LL;
          }
          ADD_PROCESS_PARAM( "int", "%ld", gendt );
        }
        break;

      case 'e':
        numSegments = (INT4) atoi( optarg );
        if ( numSegments < 1 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "number of data segment must be greater than 0: "
              "(%d specified)\n", 
              long_options[option_index].name, numSegments );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", numSegments );
        break;

      case 'f':
        ovrlap = (INT4) atoi( optarg );
        if ( ovrlap < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "data segment overlap must be positive: "
              "(%d specified)\n", 
              long_options[option_index].name, ovrlap );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", ovrlap );
        break;

      case 'g':
        sampleRate = (INT4) atoi( optarg );
        if ( sampleRate < 2 || sampleRate > 16384 || sampleRate % 2 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "rate must be power of 2 between 2 and 16384 inclusive: "
              "(%d specified)\n", 
              long_options[option_index].name, sampleRate );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", sampleRate );
        break;

      case 'h':
        fprintf( stdout, USAGE );
        exit( 0 );
        break;

      case 'i':
        fLow = (REAL4) atof( optarg );
        if ( fLow < 40 )
        {
          fprintf( stdout, "invalid argument to --%s:\n"
              "low frequency cutoff is less than 40 Hz: "
              "(%f Hz specified)\n",
              long_options[option_index].name, fLow );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "float", "%e", fLow );
        break;

      case 'j':
        if ( ! strcmp( "mean", optarg ) )
        {
          specType = 0;
        }
        else if ( ! strcmp( "median", optarg ) )
        {
          specType = 1;
        }
        else if ( ! strcmp( "bad-mean", optarg ) )
        {
          specType = 0;
          badMeanPsd = 1;
        }
        else
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "unknown power spectrum type: "
              "%s (must be mean or median)\n", 
              long_options[option_index].name, optarg );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'k':
        invSpecTrunc = (INT4) atoi( optarg );
        if ( invSpecTrunc < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "inverse spectrum length must be positive or zero: "
              "(%d specified)\n", 
              long_options[option_index].name, invSpecTrunc );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", invSpecTrunc );
        break;

      case 'l':
        dynRangeExponent = (REAL4) atof( optarg );
        haveDynRange = 1;
        ADD_PROCESS_PARAM( "float", "%e", dynRangeExponent );
        break;

      case 'm':
        startTemplate = (INT4) atoi( optarg );
        if ( startTemplate < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "template bank start index must be positive: "
              "(%d specified)\n", 
              long_options[option_index].name, startTemplate );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", startTemplate );
        break;

      case 'M':
        minimalMatch = (REAL4) atof( optarg );
        if ( minimalMatch < 0 || minimalMatch > 1 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "minimal match must be in the range [0,1]: "          
              "(%e specified)\n", 
              long_options[option_index].name, minimalMatch );
        }
        /* process param added after bank is generated so that a */
        /* value in the bank looks like a command line option.   */
        break;

      case 'n':
        stopTemplate = (INT4) atoi( optarg );
        if ( stopTemplate < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "template bank stop index must be positive: "
              "(%d specified)\n", 
              long_options[option_index].name, stopTemplate );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", stopTemplate );
        break;

      case 'o':
        numChisqBins = (INT4) atoi( optarg );
        if ( numChisqBins < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "number of chisq veto bins must be positive: "
              "(%d specified)\n", 
              long_options[option_index].name, numChisqBins );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", numChisqBins );
        break;

      case 'p':
        /* create storage for the calibration frame cache name */
        optarg_len = strlen( optarg ) + 1;
        calCacheName = (CHAR *) calloc( optarg_len, sizeof(CHAR));
        memcpy( calCacheName, optarg, optarg_len );
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'F':
        if ( ! strcmp( "TaylorF2", optarg ) )
        {
          approximant = TaylorF2;
        }
        else if ( ! strcmp( "BCV", optarg ) )
        {
          approximant = BCV;
        }
        else
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "unknown order specified: "
              "%s (must be either TaylorF2 or BCV)\n", 
              long_options[option_index].name, optarg );
          exit( 1 );
        }
        haveApprox = 1;
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'q':
        snrThresh = atof( optarg );
        if ( snrThresh < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "signal to noise threshold must be positive: "
              "(%f specified)\n", 
              long_options[option_index].name, snrThresh );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "float", "%s", optarg );
        break;

      case 'r':
        chisqThresh = atof( optarg );
        if ( chisqThresh < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "chi squared threshold must be positive: "
              "(%f specified)\n", 
              long_options[option_index].name, chisqThresh );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "float", "%s", optarg );
        break;

      case 'R':
        if ( ! strcmp( "ldas", optarg ) )
        {
          resampFiltType = 0;
        }
        else if ( ! strcmp( "butterworth", optarg ) )
        {
          resampFiltType = 1;
        }
        else
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "unknown resampling filter type: "
              "%s (must be ldas or butterworth)\n", 
              long_options[option_index].name, optarg );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;


      case 's':
        if ( strlen( optarg ) > LIGOMETA_COMMENT_MAX - 1 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "comment must be less than %d characters\n",
              long_options[option_index].name, LIGOMETA_COMMENT_MAX );
          exit( 1 );
        }
        else
        {
          LALSnprintf( comment, LIGOMETA_COMMENT_MAX, "%s", optarg);
        }
        break;

      case 't':
        highPass = 1;
        highPassFreq = (REAL4) atof( optarg );
        if ( highPassFreq <= 0 )
        {
          fprintf( stdout, "invalid argument to --%s:\n"
              "high pass filter frequency must be greater than 0 Hz: "
              "(%f Hz specified)\n",
              long_options[option_index].name, highPassFreq );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "float", "%e", highPassFreq );
        break;

      case 'H':
        highPassOrder = (INT4) atoi( optarg );
        if ( highPassOrder <= 0 )
        {
          fprintf( stdout, "invalid argument to --%s:\n"
              "high pass filter order must be greater than 0: "
              "(%d specified)\n",
              long_options[option_index].name, highPassOrder );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", highPassOrder );
        break;

      case 'I':
        highPassAtten = (REAL4) atof( optarg );
        if ( highPassAtten < 0.0 || highPassAtten > 1.0 )
        {
          fprintf( stdout, "invalid argument to --%s:\n"
              "high pass attenuation must be in the range [0:1]: "
              "(%f specified)\n",
              long_options[option_index].name, highPassAtten );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "float", "%e", highPassAtten );
        break;

      case 'u':
        /* create storage for the input frame cache name */
        optarg_len = strlen( optarg ) + 1;
        frInCacheName = (CHAR *) calloc( optarg_len, sizeof(CHAR) );
        memcpy( frInCacheName, optarg, optarg_len );
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'v':
        /* create storage for the calibration frame cache name */
        optarg_len = strlen( optarg ) + 1;
        bankFileName = (CHAR *) calloc( optarg_len, sizeof(CHAR));
        memcpy( bankFileName, optarg, optarg_len );
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'w':
        /* create storage for the injection file name */
        optarg_len = strlen( optarg ) + 1;
        injectionFile = (CHAR *) calloc( optarg_len, sizeof(CHAR));
        memcpy( injectionFile, optarg, optarg_len );
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'x':
        padData = (INT4) atoi( optarg );
        if ( padData < 0 )
        {
          fprintf( stderr, "invalid argument to --%s:\n"
              "number of seconds to pad from input data"
              "must be greater than 0: (%d specified)\n", 
              long_options[option_index].name, padData );
          exit( 1 );
        }
        ADD_PROCESS_PARAM( "int", "%d", padData );
        break;

      case 'X':
        slideData.gpsSeconds = (INT4) atoi( optarg );
        ADD_PROCESS_PARAM( "int", "%d", slideData.gpsSeconds );
        break;

      case 'Y':
        slideData.gpsNanoSeconds = (INT4) atoi( optarg );
        ADD_PROCESS_PARAM( "int", "%d", slideData.gpsNanoSeconds );
        break;

      case 'z':
        set_debug_level( optarg );
        ADD_PROCESS_PARAM( "string", "%s", optarg );
        break;

      case 'Z':
        /* create storage for the usertag */
        optarg_len = strlen( optarg ) + 1;
        userTag = (CHAR *) calloc( optarg_len, sizeof(CHAR) );
        memcpy( userTag, optarg, optarg_len );

        this_proc_param = this_proc_param->next = (ProcessParamsTable *)
          calloc( 1, sizeof(ProcessParamsTable) );
        LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, "%s", 
            PROGRAM_NAME );
        LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, "-userTag" );
        LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "string" );
        LALSnprintf( this_proc_param->value, LIGOMETA_VALUE_MAX, "%s",
            optarg );
        break;

      case 'V':
        /* print version information and exit */
        fprintf( stdout, "LIGO/LSC Standalone Inspiral Search Engine\n" 
            "Duncan Brown <duncan@gravity.phys.uwm.edu>\n"
            "CVS Version: " CVS_ID_STRING "\n"
            "CVS Tag: " CVS_NAME_STRING "\n" );
        exit( 0 );
        break;

      case '?':
        exit( 1 );
        break;

      default:
        fprintf( stderr, "unknown error while parsing options\n" );
        exit( 1 );
    }
  }

  if ( optind < argc )
  {
    fprintf( stderr, "extraneous command line arguments:\n" );
    while ( optind < argc )
    {
      fprintf ( stderr, "%s\n", argv[optind++] );
    }
    exit( 1 );
  }

  /* enable output is stored in the first process param row */
  if ( enableOutput == 1 )
  {
    LALSnprintf( procparams.processParamsTable->program, 
        LIGOMETA_PROGRAM_MAX, "%s", PROGRAM_NAME );
    LALSnprintf( procparams.processParamsTable->param,
        LIGOMETA_PARAM_MAX, "--enable-output" );
    LALSnprintf( procparams.processParamsTable->type, 
        LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( procparams.processParamsTable->value, 
        LIGOMETA_TYPE_MAX, " " );
  }
  else if ( enableOutput == 0 )
  {
    LALSnprintf( procparams.processParamsTable->program, 
        LIGOMETA_PROGRAM_MAX, "%s", PROGRAM_NAME );
    LALSnprintf( procparams.processParamsTable->param,
        LIGOMETA_PARAM_MAX, "--disable-output" );
    LALSnprintf( procparams.processParamsTable->type, 
        LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( procparams.processParamsTable->value, 
        LIGOMETA_TYPE_MAX, " " );
  }
  else
  {
    fprintf( stderr, "--enable-output or --disable-output "
        "argument must be specified\n" );
    exit( 1 );
  }


  /* check event cluster option */
  this_proc_param = this_proc_param->next = (ProcessParamsTable *)
    calloc( 1, sizeof(ProcessParamsTable) );
  if ( eventCluster == 1 )
  {
    LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, 
        "%s", PROGRAM_NAME );
    LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, 
        "--enable-event-cluster" );
    LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( this_proc_param->value, LIGOMETA_TYPE_MAX, " " );
  }
  else if ( eventCluster == 0 )
  {
    LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, 
        "%s", PROGRAM_NAME );
    LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, 
        "--disable-event-cluster" );
    LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( this_proc_param->value, LIGOMETA_TYPE_MAX, " " );
  }
  else
  {
    fprintf( stderr, "--enable-event-cluster or "
        "--disable-event-cluster argument must be specified\n" );
    exit( 1 );
  }

  /* check inject-overhead option */
  if ( injectOverhead )
  {
    this_proc_param = this_proc_param->next = (ProcessParamsTable *)
	calloc( 1, sizeof(ProcessParamsTable) );
    LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, 
        "%s", PROGRAM_NAME );
    LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, 
        "--inject-overhead" );
    LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( this_proc_param->value, LIGOMETA_TYPE_MAX, " " );
  }

  /*
   *
   * check validity of arguments
   *
   */


  /* check validity of input data time */
  if ( ! gpsStartTimeNS )
  {
    fprintf( stderr, "--gps-start-time must be specified\n" );
    exit( 1 );
  }
  LAL_CALL( LALINT8toGPS( &status, &gpsStartTime, &gpsStartTimeNS ), 
      &status );
  if ( ! gpsEndTimeNS )
  {
    fprintf( stderr, "--gps-end-time must be specified\n" );
    exit( 1 );
  }
  LAL_CALL( LALINT8toGPS( &status, &gpsEndTime, &gpsEndTimeNS ), 
      &status );
  if ( gpsEndTimeNS <= gpsStartTimeNS )
  {
    fprintf( stderr, "invalid gps time range: "
        "start time: %d, end time %d\n",
        gpsStartTime.gpsSeconds, gpsEndTime.gpsSeconds );
    exit( 1 );
  }

  /* check trigger generation time is within input time */
  if ( trigStartTimeNS )
  {
    if ( trigStartTimeNS < gpsStartTimeNS )
    {
      fprintf( stderr, 
          "trigStartTimeNS = %lld\nis less than gpsStartTimeNS = %lld", 
          trigStartTimeNS, gpsStartTimeNS );
    }
  }
  if ( trigEndTimeNS )
  {
    if ( trigEndTimeNS > gpsEndTimeNS )
    {
      fprintf( stderr, 
          "trigEndTimeNS = %lld\nis greater than gpsEndTimeNS = %lld", 
          trigEndTimeNS, gpsEndTimeNS );
    }
  }

  /* check validity of data length parameters */
  if ( numPoints < 0 )
  {
    fprintf( stderr, "--segment-length must be specified\n" );
    exit( 1 );
  }
  if ( numSegments < 0 )
  {
    fprintf( stderr, "--number-of-segments must be specified\n" );
    exit( 1 );
  }
  if ( ovrlap < 0 )
  {
    fprintf( stderr, "--segment-overlap must be specified\n" );
    exit( 1 );
  }

  /* check sample rate has been given */
  if ( sampleRate < 0 )
  {
    fprintf( stderr, "--sample-rate must be specified\n" );
    exit( 1 );
  }

  /* check high pass option has been given */
  if ( highPass < 0 )
  {
    fprintf( stderr, "--disable-high-pass or --enable-high-pass (freq)"
        " must be specified\n" );
    exit( 1 );
  }
  else if ( ! highPass )
  {
    LALSnprintf( this_proc_param->program, LIGOMETA_PROGRAM_MAX, 
        "%s", PROGRAM_NAME );
    LALSnprintf( this_proc_param->param, LIGOMETA_PARAM_MAX, 
        "--disable-high-pass" );
    LALSnprintf( this_proc_param->type, LIGOMETA_TYPE_MAX, "string" );
    LALSnprintf( this_proc_param->value, LIGOMETA_TYPE_MAX, " " );
  }
  else
  {
    /* check that all the high pass parameters have been specified */
    if ( highPassOrder < 0 )
    {
      fprintf( stderr, "--high-pass-order must be specified\n" );
      exit( 1 );
    }
    if ( highPassAtten < 0 )
    {
      fprintf( stderr, "--high-pass-attenuation must be specified\n" );
      exit( 1 );
    }
  }

  /* check validity of input data length */
  inputDataLength = numPoints * numSegments - ( numSegments - 1 ) * ovrlap;
  {
    INT8 gpsChanIntervalNS = gpsEndTimeNS - gpsStartTimeNS;
    INT8 inputDataLengthNS = (INT8) inputDataLength * 1000000000LL / 
      (INT8) sampleRate;

    if ( inputDataLengthNS != gpsChanIntervalNS )
    {
      fprintf( stderr, "length of input data and data chunk do not match\n" );
      fprintf( stderr, "start time: %lld, end time %lld\n",
          gpsStartTimeNS / 1000000000LL, gpsEndTimeNS / 1000000000LL );
      fprintf( stderr, "gps channel time interval: %lld ns\n"
          "computed input data length: %lld ns\n", 
          gpsChanIntervalNS, inputDataLengthNS );
      exit( 1 );
    }
  }

  /* check filter parameters have been specified */
  if ( numChisqBins < 0 )
  {
    fprintf( stderr, "--chisq-bins must be specified\n" );
    exit( 1 );
  }
  if ( fLow < 0 )
  {
    fprintf( stderr, "--low-frequency-cutoff must be specified\n" );
    exit( 1 );
  }
  if ( resampFiltType < 0 )
  {
    fprintf( stderr, "--resample-filter must be specified\n" );
    exit( 1 );
  }
  if ( specType < 0 )
  {
    fprintf( stderr, "--spectrum-type must be specified\n" );
    exit( 1 );
  }
  if ( invSpecTrunc < 0 )
  {
    fprintf( stderr, "--inverse-spec-length must be specified\n" );
    exit( 1 );
  }
  else if ( invSpecTrunc * sampleRate > numPoints )
  {
    fprintf( stderr, "--inverse-spec-length must be less than "
        "--segment-length\n" );
    exit( 1 );
  }

  if ( ! haveDynRange )
  {
    fprintf( stderr, "--dynamic-range-exponent must be specified\n" );
    exit( 1 );
  }
  if ( ! haveApprox )
  {
    fprintf( stderr, "--approximant must be specified\n" );
    exit( 1 );
  }

  /* check that a channel has been requested and fill the ifo */
  if ( ! fqChanName )
  {
    fprintf( stderr, "--channel-name must be specified\n" );
    exit( 1 );
  }

  /* check that the thresholds have been specified */
  if ( snrThresh < 0 )
  {
    fprintf( stderr, "--snr-threshold must be specified\n" );
    exit( 1 );
  }
  if ( chisqThresh < 0 )
  {
    fprintf( stderr, "--chisq-threshold must be specified\n" );
    exit( 1 );
  }

  /* check that the frame caches have been specified */
  if ( ! frInCacheName )
  {
    fprintf( stderr, "--frame-cache must be specified\n" );
    exit( 1 );
  }
  if ( ! calCacheName )
  {
    fprintf( stderr, "--calibration-cache must be specified\n" );
    exit( 1 );
  }
  if ( ! bankFileName )
  {
    fprintf( stderr, "--bank-file must be specified\n" );
    exit( 1 );
  }

  return 0;
}

#undef ADD_PROCESS_PARAM
