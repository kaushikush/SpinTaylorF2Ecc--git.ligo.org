#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lal/AVFactories.h>
#include <lal/Date.h>
#include <lal/EPSearch.h>
#include <lal/ExcessPower.h>
#include <lal/FrameCalibration.h>
#include <lal/FrameStream.h>
#include <lal/FrequencySeries.h>
#include <lal/GenerateBurst.h>
#include <lal/IIRFilter.h>
#include <lal/LALConstants.h>
#include <lal/LALDatatypes.h>
#include <lal/LALError.h>
#include <lal/LALStdlib.h>
#include <lal/LIGOLwXML.h>
#include <lal/LIGOLwXMLRead.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/PrintFTSeries.h>
#include <lal/Random.h>
#include <lal/ResampleTimeSeries.h>
#include <lal/TimeFreqFFT.h>
#include <lal/TimeSeries.h>
#include <lal/Units.h>

#include <lalapps.h>
#include <processtable.h>

int snprintf(char *str, size_t size, const char *format, ...);

NRCSID( POWERC, "power $Id$");
RCSID( "power $Id$");

#define PROGRAM_NAME "power"
#define CVS_REVISION "$Revision$"
#define CVS_SOURCE "$Source$"
#define CVS_DATE "$Date$"

#include <config.h>
#ifndef HAVE_LIBLALFRAME
int main(void)
{
	fputs("Disabled: LALApps compiled with non-frame-enabled LAL\n", stderr);
	exit(77);
}
#else

#define TRUE       1
#define FALSE      0

/*
 * ============================================================================
 *                                Global Data
 * ============================================================================
 */

/* Parameters from command line */
static struct {
	CHAR *calCacheFile;         /* name of the calibration cache file  */
	int cluster;                /* TRUE == perform clustering          */
	CHAR *comment;              /* user comment                        */
	int FilterCorruption;       /* samples corrupted by conditioning   */
	REAL8 fcorner;              /* GEO high pass corner freq, in Hz    */
	INT4 maxSeriesLength;       /* RAM-limited input length            */
	REAL4 noiseAmpl;            /* gain factor for white noise         */
	INT4 printData;
	size_t PSDAverageLength;    /* number of samples to use for PSD    */
	INT4 seed;                  /* set non-zero to generate noise      */
	LIGOTimeGPS startEpoch;     /* gps start time                      */
	LIGOTimeGPS stopEpoch;      /* gps stop time                       */
	INT4 verbose;
	int whiteNoise;             /* insertion of Gaussian white noise   */
} options;

/* some global output flags */
INT4 geodata;
 
/* global variables */
FrChanIn channelIn;                 /* channnel information                */
FrChanIn mdcchannelIn;              /* mdc signal only channnel info       */
EPSearchParams *mdcparams;          /* mdc search param                    */
CHAR ifo[3];                        /* two character interferometer        */
CHAR *cachefile;                    /* name of file with frame cache info  */
CHAR *dirname;                      /* name of directory with frames       */
INT4 frameSampleRate;               /* sample rate of the frame data       */
INT4 targetSampleRate;              /* sample rate after resampling        */

/* data conditioning parameters */
CHAR *injectionFile;                /* file with list of injections        */
CHAR *mdcCacheFile;                 /* name of mdc signal cache file       */
ResampleTSFilter resampFiltType;


/*
 * ============================================================================
 *                               Misc Utilities
 * ============================================================================
 */

/*
 * Return the smaller of two size_t variables.
 */

static size_t min(size_t a, size_t b)
{
	return(a < b ? a : b);
}


/*
 * Compare two GPS times.
 */

static int CompareGPS(LALStatus *stat, LIGOTimeGPS *gps1, LIGOTimeGPS *gps2)
{
	LALGPSCompareResult result;
	LAL_CALL(LALCompareGPS(stat, &result, gps1, gps2), stat);
	return(result);
}


/*
 * Return the difference between two GPS times as REAL8.
 */

static REAL8 DeltaGPStoFloat(LALStatus *stat, LIGOTimeGPS *stop, LIGOTimeGPS *start)
{
	LALTimeInterval i;
	REAL8 d;
	LAL_CALL(LALDeltaGPS(stat, &i, stop, start), stat);
	LAL_CALL(LALIntervalToFloat(stat, &d, &i), stat);
	return(d);
}


/*
 * Round a PSD length down to a value that is commensurate with the window
 * length and window shift.
 */

static size_t window_commensurate(
	size_t psdlength,
	size_t windowlength,
	size_t windowshift
)
{
	if(psdlength < windowlength)
		return(0);
	return(((psdlength - windowlength) / windowshift) * windowshift + windowlength);
}


/*
 * ============================================================================
 *                       Initialize and parse arguments
 * ============================================================================
 */

static void print_usage(char *program)
{
	fprintf(stderr,
"Usage:  %s <option> [...]\n" \
"The following options are recognized.  Options not surrounded in [] are\n" \
"required.\n" \
"	 --bandwidth <bandwidth>\n" \
"	[--calibration-cache <cache file>]\n" \
"	 --channel-name <string>\n" \
"	[--cluster]\n" \
"	[--debug-level <level>]\n" \
"	 --default-alpha <alpha>\n" \
"	 --event-limit <count>\n" \
"	 --filter-corruption <samples>\n" \
"	 --frame-cache <cache file>\n" \
"	 --frame-dir <directory>\n" \
"	 --frame-sample-rate <Hz>\n" \
"	[--geodata <high pass corner frequency>]\n" \
"	 --gps-end-time <seconds>\n" \
"	 --gps-end-time-ns <nanoseconds>\n" \
"	 --gps-start-time <seconds>\n" \
"	 --gps-start-time-ns <nanoseconds>\n" \
"	[--help]\n" \
"	[--injection-file <file name>]\n" \
"	 --low-freq-cutoff <flow>\n" \
"	[--mdc-cache <cache file>]\n" \
"	[--mdc-channel <channel name>]\n" \
"	 --min-freq-bin <nfbin>\n" \
"	 --min-time-bin <ntbin>\n" \
"	 --noise-amplitude <amplitude>\n" \
"	 --nsigma <sigma>\n" \
"	[--printData]\n" \
"	[--printSpectrum]\n" \
"	 --psd-average-method <method>\n" \
"	 --psd-average-points <samples>\n" \
"	 --ram-limit <MebiBytes>\n" \
"	 --resample-filter <filter type>\n" \
"	 --seed <seed>\n" \
"	 --target-sample-rate <Hz>\n" \
"	 --tile-overlap-factor <factor>\n" \
"	 --threshold <threshold>\n" \
"	[--user-tag <comment>]\n" \
"	[--verbose]\n" \
"	 --window <window>\n" \
"	 --window-length <samples>\n" \
"	 --window-shift <samples>\n", program);
}

static void print_bad_argument(const char *prog, const char *arg, const char *msg)
{
	fprintf(stderr, "%s: error: invalid argument for --%s: %s\n", prog, arg, msg);
}

static void print_missing_argument(const char *prog, const char *arg)
{
	fprintf(stderr, "%s: error: --%s not specified\n", prog, arg);
}

static void print_alloc_fail(const char *prog, const char *msg)
{
	fprintf(stderr, "%s: error: memory allocation failure %s\n", prog, msg);
}


#define ADD_PROCESS_PARAM( pptype, format, ppvalue ) { \
	proc_param->next = LALCalloc(1, sizeof(ProcessParamsTable)); \
	proc_param = proc_param->next; \
	snprintf( proc_param->value, LIGOMETA_VALUE_MAX, format, ppvalue ); \
	snprintf( proc_param->program, LIGOMETA_PROGRAM_MAX, "%s", PROGRAM_NAME ); \
	snprintf( proc_param->param, LIGOMETA_PARAM_MAX, "--%s", long_options[option_index].name ); \
	snprintf( proc_param->type, LIGOMETA_TYPE_MAX, "%s", pptype ); \
}

static int check_for_missing_parameters(LALStatus *stat, char *prog, struct option *long_options, EPSearchParams *params)
{
	int index;
	int got_all_arguments = TRUE;
	int arg_is_missing;
	INT8 gpstmp;

	for(index = 0; long_options[index].name; index++) {
		switch(long_options[index].val) {
			case 'I':
			arg_is_missing = !frameSampleRate;
			break;

			case 'K':
			LAL_CALL(LALGPStoINT8(stat, &gpstmp, &options.stopEpoch), stat);
			arg_is_missing = !gpstmp;
			break;

			case 'M':
			LAL_CALL(LALGPStoINT8(stat, &gpstmp, &options.startEpoch), stat);
			arg_is_missing = !gpstmp;
			break;

			case 'Z':
			arg_is_missing = !options.PSDAverageLength;
			break;

			case 'a':
			arg_is_missing = !options.maxSeriesLength;
			break;

			case 'd':
			arg_is_missing = !params->windowShift;
			break;

			case 'e':
			arg_is_missing = !targetSampleRate;
			break;

			case 'i':
			arg_is_missing = options.FilterCorruption < 0;
			break;

			default:
			arg_is_missing = FALSE;
			break;
		}
		if(arg_is_missing) {
			print_missing_argument(prog, long_options[index].name);
			got_all_arguments = FALSE;
		}
	}
	return(got_all_arguments);
}


void parse_command_line( 
	int argc, 
	char *argv[], 
	EPSearchParams *params,
	MetadataTable *procparams 
)
{ 
	char msg[240];
	int printSpectrum;
	int c;
	int option_index;
	int ram = 0;
	INT8 gpstmp;
	INT8 gpsStartTimeNS = 0;
	INT8 gpsStopTimeNS = 0;
	ProcessParamsTable *proc_param = procparams->processParamsTable;
	LALStatus stat = blank_status;
	struct option long_options[] = {
		{"bandwidth",           required_argument, NULL,           'A'},
		{"calibration-cache",   required_argument, NULL,           'B'},
		{"channel-name",        required_argument, NULL,           'C'},
		{"cluster",             no_argument, &options.cluster,    TRUE},
		{"debug-level",         required_argument, NULL,           'D'},
		{"default-alpha",       required_argument, NULL,           'E'},
		{"event-limit",         required_argument, NULL,           'F'},
		{"filter-corruption",	required_argument, NULL,           'j'},
		{"frame-cache",         required_argument, NULL,           'G'},
		{"frame-dir",           required_argument, NULL,           'H'},
		{"frame-sample-rate",   required_argument, NULL,           'I'},
		{"geodata",		required_argument, NULL,           'J'},
		{"gps-end-time",        required_argument, NULL,           'K'},
		{"gps-end-time-ns",     required_argument, NULL,           'L'},
		{"gps-start-time",      required_argument, NULL,           'M'},
		{"gps-start-time-ns",   required_argument, NULL,           'N'},
		{"help",                no_argument,       NULL,           'O'},
		{"injection-file",      required_argument, NULL,           'P'},
		{"low-freq-cutoff",     required_argument, NULL,           'Q'},
		{"mdc-cache",           required_argument, NULL,           'R'},
		{"mdc-channel",         required_argument, NULL,           'S'},
		{"min-freq-bin",        required_argument, NULL,           'T'},
		{"min-time-bin",        required_argument, NULL,           'U'},
		{"noise-amplitude",     required_argument, NULL,           'V'},
		{"nsigma",              required_argument, NULL,           'X'},
		{"printData",           no_argument, &options.printData,  TRUE},
		{"printSpectrum",       no_argument, &printSpectrum,      TRUE},
		{"psd-average-method",  required_argument, NULL,           'Y'},
		{"psd-average-points",  required_argument, NULL,           'Z'},
		{"ram-limit",           required_argument, NULL,           'a'},
		{"resample-filter",     required_argument, NULL,           'b'},
		{"seed",                required_argument, NULL,           'c'},
		{"target-sample-rate",  required_argument, NULL,           'e'},
		{"tile-overlap-factor", required_argument, NULL,           'f'},
		{"threshold",           required_argument, NULL,           'g'},
		{"user-tag",            required_argument, NULL,           'h'},
		{"verbose",             no_argument, &options.verbose,    TRUE},
		{"window",              required_argument, NULL,           'i'},
		{"window-length",       required_argument, NULL,           'W'},
		{"window-shift",        required_argument, NULL,           'd'},
		{NULL, 0, NULL, 0}
	};

	/*
	 * Set parameter defaults.
	 */

	params->channelName = NULL;
	params->eventLimit = 999;
	params->tfTilingInput.maxTileBand = 64.0;
	params->windowShift = 0;

	options.calCacheFile = NULL;
	options.cluster = FALSE;
	options.comment = "";
	options.fcorner = 100.0;
	options.FilterCorruption = -1;
	options.noiseAmpl = 1.0;
	options.printData = FALSE;
	options.PSDAverageLength = 0;
	options.seed = 1;
	options.verbose = FALSE;
	options.whiteNoise = FALSE;

	cachefile = NULL;
	dirname = NULL;
	frameSampleRate = 0;
	geodata = FALSE;
	memset(ifo, 0, sizeof(ifo));
	injectionFile = NULL;
	mdcCacheFile = NULL;
	mdcparams = NULL;
	printSpectrum = FALSE;
	resampFiltType = -1;
	targetSampleRate = 0;

	/*
	 * Parse command line.
	 */

	do switch(c = getopt_long(argc, argv, "", long_options, &option_index)) {
		case 'A':
		params->tfTilingInput.length = atoi(optarg);
		if(params->tfTilingInput.length <= 0) {
			sprintf(msg, "must be > 0 (%i specified)", params->tfTilingInput.length);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->tfTilingInput.length);
		break;

		case 'B':
		options.calCacheFile = optarg;
		ADD_PROCESS_PARAM("string", "%s", options.calCacheFile);
		break;

		case 'C':
		params->channelName = optarg;
		channelIn.name = params->channelName;
		channelIn.type = ADCDataChannel;
		memcpy(ifo, channelIn.name, sizeof(ifo) - 1);
		ADD_PROCESS_PARAM("string", "%s", params->channelName);
		break;

		case 'D':
		set_debug_level(optarg);
		ADD_PROCESS_PARAM("int", "%d", atoi(optarg));
		break;

		case 'E':
		params->compEPInput.alphaDefault = atof(optarg);
		if(params->compEPInput.alphaDefault <= 0.0 || params->compEPInput.alphaDefault >= 1.0) {
			sprintf(msg, "must be in range [0,1] (%f specified)", params->compEPInput.alphaDefault);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("float", "%e", params->compEPInput.alphaDefault);
		break;

		case 'F':
		params->eventLimit = atoi(optarg);
		if(params->eventLimit < 1 || params->eventLimit > 999) {
			sprintf(msg, "must be in range [1,999] (%i specified)", params->eventLimit);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit( 1 );
		}
		ADD_PROCESS_PARAM("int", "%d", params->eventLimit);
		break;

		case 'G':
		cachefile = optarg;
		ADD_PROCESS_PARAM("string", "%s", cachefile);
		break;

		case 'H':
		dirname =  optarg;
		ADD_PROCESS_PARAM("string", "%s", optarg);
		break;

		case 'I':
		frameSampleRate = (INT4) atoi(optarg);
		if(frameSampleRate < 2 || frameSampleRate > 16384 || frameSampleRate % 2) {
			sprintf(msg, "must be a power of 2 in the range [2,16384] (%d specified)", frameSampleRate);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", frameSampleRate);
		break;

		case 'J':
		options.fcorner = atof(optarg);
		if(options.fcorner <= 0) {
			print_bad_argument(argv[0], long_options[option_index].name, "must be > 0");
			exit(1);
		}
		geodata = TRUE;
		ADD_PROCESS_PARAM("double", "%e", options.fcorner);
		break;

		case 'K':
		gpstmp = atol(optarg);
		if(gpstmp < 441417609 || gpstmp > 999999999) {
			sprintf(msg, "must be in the range [Jan 01 1994 00:00:00 UTC, Sep 14 2011 01:46:26 UTC] (%lld specified)", gpstmp);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		gpsStopTimeNS += gpstmp * 1000000000LL;
		ADD_PROCESS_PARAM("int", "%lld", gpstmp);
		break;

		case 'L':
		gpstmp = atol(optarg);
		if(gpstmp < 0 || gpstmp > 999999999) {
			sprintf(msg, "must be in the range [0,999999999] (%lld specified)", gpstmp);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		gpsStopTimeNS += gpstmp;
		ADD_PROCESS_PARAM("int", "%lld", gpstmp);
		break;

		case 'M':
		gpstmp = atol(optarg);
		if(gpstmp < 441417609 || gpstmp > 999999999) {
			sprintf(msg, "must be in the range [Jan 01 1994 00:00:00 UTC, Sep 14 2011 01:46:26 UTC] (%lld specified)", gpstmp);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		gpsStartTimeNS += gpstmp * 1000000000LL;
		ADD_PROCESS_PARAM("int", "%lld", gpstmp);
		break;

		case 'N':
		gpstmp = atol(optarg);
		if(gpstmp < 0 || gpstmp > 999999999) {
			sprintf(msg, "must be in the range [0,999999999] (%lld specified)", gpstmp);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		gpsStartTimeNS += gpstmp;
		ADD_PROCESS_PARAM("int", "%lld", gpstmp);
		break;

		case 'O':
		print_usage(argv[0]);
		exit(0);
		break;

		case 'P':
		injectionFile = optarg;
		ADD_PROCESS_PARAM("string", "%s", injectionFile);
		break;

		case 'Q':
		params->tfTilingInput.flow = atof(optarg);
		if(params->tfTilingInput.flow < 0.0) {
			sprintf(msg,"must be >= 0.0 (%f specified)", params->tfTilingInput.flow);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("float", "%e", params->tfTilingInput.flow);
		break;

		case 'R':
		mdcCacheFile = optarg;
		ADD_PROCESS_PARAM("string", "%s", mdcCacheFile);
		break;

		case 'S':
		mdcparams = LALMalloc(sizeof(*mdcparams));
		if(!mdcparams) {
			print_alloc_fail(argv[0], "for mdcparams");
			exit(1);
		}
		mdcparams->channelName = optarg;
		mdcchannelIn.name = mdcparams->channelName;
		mdcchannelIn.type = ADCDataChannel;
		ADD_PROCESS_PARAM("string", "%s", mdcparams->channelName);
		break;

		case 'T':
		params->tfTilingInput.minFreqBins = atoi(optarg);
		if(params->tfTilingInput.minFreqBins <= 0) {
			sprintf(msg,"must be > 0 (%i specified)", params->tfTilingInput.minFreqBins);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->tfTilingInput.minFreqBins);
		break;

		case 'U':
		params->tfTilingInput.minTimeBins = atoi(optarg);
		if(params->tfTilingInput.minTimeBins <= 0) {
			sprintf(msg,"must be > 0 (%i specified)", params->tfTilingInput.minTimeBins);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->tfTilingInput.minTimeBins);
		break;

		case 'V':
		options.noiseAmpl = atof(optarg);
		options.whiteNoise = TRUE;
		if(options.noiseAmpl <= 0.0) {
			sprintf(msg, "must be > 0.0 (%f specified)", options.noiseAmpl);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("float", "%e", options.noiseAmpl);
		break;

		case 'W':
		params->windowLength = atoi(optarg);
		if(params->windowLength <= 0) {
			sprintf(msg, "must be > 0 (%i specified)", params->windowLength);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->windowLength);
		break;

		case 'X':
		params->compEPInput.numSigmaMin = atof(optarg);
		if(params->compEPInput.numSigmaMin <= 1.0) {
			sprintf(msg, "must be > 0 (%f specified)", params->compEPInput.numSigmaMin);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("float", "%e", params->compEPInput.numSigmaMin);
		break;

		case 'Y':
		if(!strcmp(optarg, "useMean"))
			params->method = useMean;
		else if(!strcmp(optarg, "useMedian"))
			params->method = useMedian;
		else if (!strcmp(optarg, "useUnity"))
			params->method = useUnity;
		else {
			sprintf(msg, "must be \"useMean\", \"useMedian\", or \"useUnity\"");
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("string", "%s", optarg);
		break;

		case 'Z':
		options.PSDAverageLength = atoi(optarg);
		ADD_PROCESS_PARAM("int", "%d", options.PSDAverageLength);
		break;

		case 'a':
		ram = atoi(optarg);
		if(ram <= 0) {
			sprintf(msg, "must be > 0 (%i specified)", ram);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", ram);
		break;

		case 'b':
		if(!strcmp("ldas", optarg))
			resampFiltType = LDASfirLP;
		else if(!strcmp("butterworth", optarg))
			resampFiltType = defaultButterworth;
		else {
			sprintf(msg, "must be \"ldas\", or \"butterworth\"");
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("string", "%s", optarg);
		break;

		case 'c':
		options.seed = atoi(optarg);
		if(options.seed <= 0) {
			sprintf(msg, "must be > 0 (%i specified)", options.seed);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", options.seed);
		break;

		case 'd':
		params->windowShift = atoi(optarg);
		ADD_PROCESS_PARAM("int", "%d", params->windowShift);
		break;

		case 'e':
		targetSampleRate = (INT4) atoi(optarg);
		if(targetSampleRate < 2 || targetSampleRate > 16384 || targetSampleRate % 2) {
			sprintf(msg, "must be a power of 2 in the rage [2,16384] (%d specified)", targetSampleRate);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", targetSampleRate);
		break;

		case 'f':
		params->tfTilingInput.overlapFactor = atoi(optarg);
		if(params->tfTilingInput.overlapFactor < 0) {
			sprintf(msg, "must be > 0 (%i specified)", params->tfTilingInput.overlapFactor);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->tfTilingInput.overlapFactor);
		break;

		case 'g':
		params->alphaThreshold = atof(optarg);
		if(params->alphaThreshold < 0.0) {
			sprintf(msg, "must be > 0 (%f specified)", params->alphaThreshold);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("float", "%e", params->alphaThreshold);
		break;

		case 'h':
		options.comment = optarg;
		ADD_PROCESS_PARAM("string", "%s", options.comment);
		break;

		case 'i':
		params->windowType = atoi(optarg);
		if(params->windowType >= NumberWindowTypes) {
			sprintf(msg, "must be <= %d (%i specified)", NumberWindowTypes, params->windowType);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", params->windowType);
		break;

		case 'j':
		options.FilterCorruption = atoi(optarg);
		if(options.FilterCorruption < 0) {
			sprintf(msg, "must be > 0 (%d specified)", options.FilterCorruption);
			print_bad_argument(argv[0], long_options[option_index].name, msg);
			exit(1);
		}
		ADD_PROCESS_PARAM("int", "%d", options.FilterCorruption);
		break;

		/* option sets a flag */
		case 0:
		break;

		/* end of arguments */
		case -1:
		break;

		/* unrecognized option */
		case '?':
		print_usage(argv[0]);
		exit(1);
		break;

		/* missing argument for an option */
		case ':':
		print_usage(argv[0]);
		exit(1);
		break;
	} while(c != -1);

	/*
	 * Convert the start and stop epochs, along with the sample rate, to a
	 * total number of samples to analyze.
	 */

	LAL_CALL(LALINT8toGPS(&stat, &options.startEpoch, &gpsStartTimeNS), &stat);
	LAL_CALL(LALINT8toGPS(&stat, &options.stopEpoch, &gpsStopTimeNS), &stat);

	/*
	 * Convert the amount of available RAM to a limit on the length of a
	 * time series to read in.
	 */

	options.maxSeriesLength = (ram * 1024 * 1024) / (4 * sizeof(REAL4));

	/*
	 * Ensure PSDAverageLength is comensurate with the analysis window
	 * length and shift.
	 */

	options.PSDAverageLength = window_commensurate(options.PSDAverageLength, params->windowLength, params->windowShift);

	/*
	 * Check for missing parameters
	 */

	if(!check_for_missing_parameters(&stat, argv[0], long_options, params))
		exit(1);

	/*
	 * Miscellaneous chores.
	 */

	params->printSpectrum = printSpectrum;

	if(options.verbose) {
		fprintf(stderr, "%s: available RAM limits analysis to %d samples\n", argv[0], options.maxSeriesLength);
		fprintf(stderr, "%s: using --psd-average-points %d\n", argv[0], options.PSDAverageLength);
	}
}


/*
 * ============================================================================
 *                                   Input
 * ============================================================================
 */

static REAL4TimeSeries *get_geo_data(
	LALStatus *stat,
	FrStream *stream,
	const char *chname,
	LIGOTimeGPS start,
	LIGOTimeGPS end,
	size_t lengthlimit,
	REAL4 flow
)
{
	PassBandParamStruc highpassParam;
	REAL4TimeSeries *series;
	REAL8TimeSeries *geo;
	size_t i;

	/* create and initialize the time series vector */
	LAL_CALL(LALCreateREAL8TimeSeries(stat, &geo, chname, start, 0.0, (REAL8) 1.0 / frameSampleRate, lalADCCountUnit, lengthlimit), stat);

	/* get the data */
	if(options.verbose)
		fprintf(stderr, "reading %u samples at GPS time %u.%09u s\n", geo->data->length, start.gpsSeconds, start.gpsNanoSeconds);
	LAL_CALL(LALFrGetREAL8TimeSeries(stat, geo, &channelIn, stream), stat);
	geo->epoch = start;
	LAL_CALL(LALFrSeek(stat, &geo->epoch, stream), stat);
	LAL_CALL(LALFrGetREAL8TimeSeries(stat, geo, &channelIn, stream), stat);

	/* high pass filter before casting REAL8 to REAL4 */
	highpassParam.nMax = 4;
	highpassParam.f2 = flow > options.fcorner ? options.fcorner : flow;
	highpassParam.f1 = -1.0;
	highpassParam.a2 = 0.1;
	highpassParam.a1 = -1.0;
	LAL_CALL(LALButterworthREAL8TimeSeries(stat, geo, &highpassParam), stat);

	/* copy data into a REAL4 time series */
	LAL_CALL(LALCreateREAL4TimeSeries(stat, &series, geo->name, geo->epoch, geo->f0, geo->deltaT, geo->sampleUnits, geo->data->length), stat);
	for(i = 0; i < series->data->length; i++)
		series->data->data[i] = geo->data->data[i];
	LAL_CALL(LALDestroyREAL8TimeSeries(stat, geo), stat);

	return(series);
}

static REAL4TimeSeries *get_ligo_data(
	LALStatus *stat,
	FrStream *stream,
	const char *chname,
	LIGOTimeGPS start,
	LIGOTimeGPS end,
	size_t lengthlimit
)
{
	REAL4TimeSeries *series;

	/* create and initialize the time series vector */
	LAL_CALL(LALCreateREAL4TimeSeries(stat, &series, chname, start, 0.0, (REAL8) 1.0 / frameSampleRate, lalADCCountUnit, lengthlimit), stat);

	/* get the data */
	if(options.verbose)
		fprintf(stderr, "reading %u samples at GPS time %u.%09u s\n", series->data->length, start.gpsSeconds, start.gpsNanoSeconds);
	LAL_CALL(LALFrGetREAL4TimeSeries(stat, series, &channelIn, stream), stat);
	series->epoch = start;
	LAL_CALL(LALFrSeek(stat, &series->epoch, stream), stat);
	LAL_CALL(LALFrGetREAL4TimeSeries(stat, series, &channelIn, stream), stat);

	return(series);
}

static REAL4TimeSeries *get_time_series(
	LALStatus *stat,
	const char *dirname,
	const char *cachefile,
	LIGOTimeGPS start,
	LIGOTimeGPS end,
	size_t lengthlimit,
	EPSearchParams *params
)
{
	REAL4TimeSeries *series;
	FrStream *stream = NULL;
	FrCache *frameCache = NULL;

	/* Open frame stream */
	if(dirname)
		LAL_CALL(LALFrOpen(stat, &stream, dirname, "*.gwf"), stat);
	else if(cachefile) {
		LAL_CALL(LALFrCacheImport(stat, &frameCache, cachefile), stat);
		LAL_CALL(LALFrCacheOpen(stat, &stream, frameCache), stat);
		LAL_CALL(LALDestroyFrCache(stat, &frameCache), stat);
	} else
		return(NULL);

	/* Get the data */
	if(geodata)
		series = get_geo_data(stat, stream, params->channelName, start, end, lengthlimit, params->tfTilingInput.flow - 10.0);
	else
		series = get_ligo_data(stat, stream, params->channelName, start, end, lengthlimit);

	/* Clean up */
	LAL_CALL(LALFrClose(stat, &stream), stat);

	return(series);
}


/*
 * ============================================================================
 *                    Fill a time series with white noise
 * ============================================================================
 */

static void makeWhiteNoise(
	LALStatus *stat,
	REAL4TimeSeries *series,
	INT4 seed,
	REAL4 amplitude
)
{
	size_t i;
	static RandomParams *rparams = NULL;

	LAL_CALL(LALCreateRandomParams(stat, &rparams, seed), stat);
	LAL_CALL(LALNormalDeviates(stat, series->data, rparams), stat);
	LAL_CALL(LALDestroyRandomParams(stat, &rparams), stat);

	for(i = 0; i < series->data->length; i++)
		series->data->data[i] *= amplitude;

	if(options.verbose) {
		REAL4 norm = 0.0;
		for(i = 0; i < series->data->length; i++)
			norm += series->data->data[i] * series->data->data[i];
		fprintf(stderr, "the norm is %e\n", sqrt(norm / series->data->length));
	}
}


/*
 * ============================================================================
 *                             Response function
 * ============================================================================
 */

static COMPLEX8FrequencySeries *generate_response(
	LALStatus *stat,
	const char *calcachefile,
	LIGOTimeGPS epoch,
	size_t length
)
{
	COMPLEX8FrequencySeries *response;
	size_t i;
	const LALUnit strainPerCount = {0,{0,0,0,0,0,1,-1},{0,0,0,0,0,0,0}};
	COMPLEX8 one = {1.0, 0.0};
	CalibrationUpdateParams calfacts;

	memset(&calfacts, 0, sizeof(calfacts));
	calfacts.ifo = ifo;

	LAL_CALL(LALCreateCOMPLEX8FrequencySeries(stat, &response, channelIn.name, epoch, 0.0, (REAL8) frameSampleRate / (REAL8) length, strainPerCount, length / 2 + 1), stat);

	if(options.verbose) 
		fprintf(stderr, "generating response at GPS time %u.%09u s\n", response->epoch.gpsSeconds, response->epoch.gpsNanoSeconds );

	/* getting the response is handled differently for geo */
	if(geodata)
		for(i = 0; i < response->data->length; i++)
			response->data->data[i] = one;
	else
		LAL_CALL(LALExtractFrameResponse(stat, response, calcachefile, &calfacts), stat);

	return(response);
} 


/*
 * ============================================================================
 *                              Burst injections
 * ============================================================================
 */

static void add_burst_injections(
	LALStatus *stat,
	REAL4TimeSeries *series,
	COMPLEX8FrequencySeries *response
)
{
	INT4 startTime = series->epoch.gpsSeconds;
	INT4 stopTime = startTime + series->data->length * series->deltaT;
	SimBurstTable *injections = NULL;

	if(!response) {
		fprintf(stderr, "Must supply calibration information for injections\n");
		exit(1);
	}

	if(options.verbose)
		fprintf(stderr, "Reading in SimBurst Table\n");

	LAL_CALL(LALSimBurstTableFromLIGOLw(stat, &injections, injectionFile, startTime, stopTime), stat);

	if(options.verbose)
		fprintf(stderr, "Injecting signals into time series\n");

	LAL_CALL(LALBurstInjectSignals(stat, series, injections, response), stat); 

	if(options.verbose)
		fprintf(stderr, "Finished making the injections\n");

	while(injections) {
		SimBurstTable *thisEvent = injections;
		injections = injections->next;
		LALFree(thisEvent);
	}
}



/*
 * ============================================================================
 *                               MDC injections
 * ============================================================================
 */

static void add_mdc_injections(LALStatus *stat, const char *mdcCacheFile, REAL4TimeSeries *series)
{
	REAL4TimeSeries *mdc;
	FrStream *frstream = NULL;
	FrCache *frcache = NULL;
	size_t i;

	if(options.verbose)
		fprintf(stderr, "Using MDC frames for injections\n");

	/* open mdc cache */
	LAL_CALL(LALFrCacheImport(stat, &frcache, mdcCacheFile), stat);
	LAL_CALL(LALFrCacheOpen(stat, &frstream, frcache), stat);
	LAL_CALL(LALDestroyFrCache(stat, &frcache), stat);

	/* create and initialize the mdc time series vector */
	LAL_CALL(LALCreateREAL4TimeSeries(stat, &mdc, mdcparams->channelName, series->epoch, series->f0, series->deltaT, series->sampleUnits, series->data->length), stat);

	/* get the mdc signal data */
	LAL_CALL(LALFrGetREAL4TimeSeries(stat, mdc, &mdcchannelIn, frstream), stat);
	mdc->epoch = series->epoch;
	LAL_CALL(LALFrSeek(stat, &mdc->epoch, frstream), stat);
	LAL_CALL(LALFrGetREAL4TimeSeries(stat, mdc, &mdcchannelIn, frstream), stat);

	/* write diagnostic info to disk */
	if(options.printData)
		LALPrintTimeSeries(mdc, "./timeseriesmdc.dat");

	/* add the mdc signal to the given time series */
	for(i = 0; i < series->data->length; i++)
		series->data->data[i] += mdc->data->data[i];

	/* clean up */
	LAL_CALL(LALDestroyREAL4TimeSeries(stat, mdc), stat);
	LAL_CALL(LALFrClose(stat, &frstream), stat);
}


/*
 * ============================================================================
 *                               Analysis Loop
 * ============================================================================
 */

/*
 * Analyze a time series in intervals corresponding to the length of time
 * for which the instrument's noise is stationary.
 */

static SnglBurstTable **analyze_series(
	LALStatus *stat,
	SnglBurstTable **addpoint,
	REAL4TimeSeries *series,
	EPSearchParams *params
)
{
	REAL4TimeSeries *interval;
	size_t i, start;
	size_t overlap = params->windowLength - params->windowShift;

	if(options.PSDAverageLength > series->data->length) {
		options.PSDAverageLength = window_commensurate(series->data->length, params->windowLength, params->windowShift);
		if(options.verbose)
			fprintf(stderr, "Warning: PSD average length exceeds available data --- reducing PSD average length to %d samples\n", options.PSDAverageLength);
		if(!options.PSDAverageLength) {
			if(options.verbose)
				fprintf(stderr, "Warning: cowardly refusing to analyze 0 samples... skipping series\n");
			return(addpoint);
		}
	}

	for(i = 0; i < series->data->length - overlap; i += options.PSDAverageLength - overlap) {
		start = min(i, series->data->length - options.PSDAverageLength);
		
		LAL_CALL(LALCutREAL4TimeSeries(stat, &interval, series, start, options.PSDAverageLength), stat);

		if(options.verbose)
			fprintf(stderr, "Analyzing samples %i -- %i\n", start, start + interval->data->length);

		LAL_CALL(EPSearch(stat, interval, params, addpoint), stat);
		while(*addpoint)
			addpoint = &(*addpoint)->next;

		LAL_CALL(LALDestroyREAL4TimeSeries(stat, interval), stat);
	}

	return(addpoint);
}


/*
 * ============================================================================
 *                              Event clustering
 * ============================================================================
 */

static SnglBurstTable *cluster_events(LALStatus *stat, SnglBurstTable *burstEvent, int limit)
{
	int iterations = 0;
	int events = 0;
	int lastevents;

	if(!burstEvent)
		return(NULL);

	do {
		LAL_CALL(LALSortSnglBurst(stat, &burstEvent, LALCompareSnglBurstByTimeAndFreq), stat);
		lastevents = events;
		LAL_CALL(LALClusterSnglBurstTable(stat, burstEvent, &events), stat);
	} while((events != lastevents) && (++iterations < limit));

	return(burstEvent);
}


/*
 * ============================================================================
 *                                   Output
 * ============================================================================
 */

static void output_results(LALStatus *stat, char *file, MetadataTable *procTable, MetadataTable *procparams, MetadataTable *searchsumm, SnglBurstTable *burstEvent)
{
	LIGOLwXMLStream xml;
	LALLeapSecAccuracy accuracy = LALLEAPSEC_LOOSE;
	MetadataTable myTable;

	memset(&xml, 0, sizeof(LIGOLwXMLStream));
	LAL_CALL(LALOpenLIGOLwXMLFile(stat, &xml, file), stat);

	/* process table */
	snprintf(procTable->processTable->ifos, LIGOMETA_IFOS_MAX, "%s", ifo);
	LAL_CALL(LALGPSTimeNow(stat, &(procTable->processTable->end_time), &accuracy), stat);
	LAL_CALL(LALBeginLIGOLwXMLTable(stat, &xml, process_table), stat);
	LAL_CALL(LALWriteLIGOLwXMLTable(stat, &xml, *procTable, process_table), stat);
	LAL_CALL(LALEndLIGOLwXMLTable(stat, &xml), stat);

	/* process params table */
	LAL_CALL(LALBeginLIGOLwXMLTable(stat, &xml, process_params_table), stat);
	LAL_CALL(LALWriteLIGOLwXMLTable(stat, &xml, *procparams, process_params_table), stat);
	LAL_CALL(LALEndLIGOLwXMLTable(stat, &xml), stat);

	/* search summary table */
	LAL_CALL(LALBeginLIGOLwXMLTable(stat, &xml, search_summary_table), stat);
	LAL_CALL(LALWriteLIGOLwXMLTable(stat, &xml, *searchsumm, search_summary_table), stat);
	LAL_CALL(LALEndLIGOLwXMLTable(stat, &xml), stat);

	/* burst table */
	LAL_CALL(LALBeginLIGOLwXMLTable(stat, &xml, sngl_burst_table), stat);
	myTable.snglBurstTable = burstEvent;
	LAL_CALL(LALWriteLIGOLwXMLTable(stat, &xml, myTable, sngl_burst_table), stat);
	LAL_CALL(LALEndLIGOLwXMLTable(stat, &xml), stat);

	LAL_CALL(LALCloseLIGOLwXMLFile(stat, &xml), stat);
}


/*
 * ============================================================================
 *                                Entry point
 * ============================================================================
 */

int main( int argc, char *argv[])
{
	LALStatus                 stat;
	LALLeapSecAccuracy        accuracy = LALLEAPSEC_LOOSE;
	EPSearchParams            params;
	LIGOTimeGPS               epoch;
	LIGOTimeGPS               boundepoch;
	LALTimeInterval           overlapCorrection;
	INT4                      numPoints;
	CHAR                      outfilename[256];
	REAL4TimeSeries          *series = NULL;
	SnglBurstTable           *burstEvent = NULL;
	SnglBurstTable          **EventAddPoint = &burstEvent;
	MetadataTable             procTable;
	MetadataTable             procparams;
	MetadataTable             searchsumm;

	/*
	 * Initialize everything
	 */

	lal_errhandler = LAL_ERR_EXIT;
	set_debug_level("3");

	memset(&stat, 0, sizeof(stat));

	/* create the process and process params tables */
	procTable.processTable = LALCalloc(1, sizeof(ProcessTable));
	LAL_CALL(LALGPSTimeNow(&stat, &(procTable.processTable->start_time), &accuracy), &stat);
	LAL_CALL(populate_process_table(&stat, procTable.processTable, PROGRAM_NAME, CVS_REVISION, CVS_SOURCE, CVS_DATE), &stat);
	procparams.processParamsTable = LALCalloc(1, sizeof(ProcessParamsTable));

	/* parse arguments and fill procparams table */
	parse_command_line(argc, argv, &params, &procparams);

	/* create the search summary table */
	searchsumm.searchSummaryTable = LALCalloc(1, sizeof(SearchSummaryTable));

	/* fill the comment */
	snprintf(procTable.processTable->comment, LIGOMETA_COMMENT_MAX, "%s", options.comment);
	snprintf(searchsumm.searchSummaryTable->comment, LIGOMETA_COMMENT_MAX, "%s", options.comment);

	/* the number of nodes for a standalone job is always 1 */
	searchsumm.searchSummaryTable->nnodes = 1;

	/* store the input start and end times */
	searchsumm.searchSummaryTable->in_start_time = options.startEpoch;
	searchsumm.searchSummaryTable->in_end_time = options.stopEpoch;

	/* determine the input time series overlap correction */
	/* LAL spec makes us pass a pointer... sigh */
	{
	REAL8 overlap = (REAL8) params.windowLength / 2.0 / targetSampleRate;
	LAL_CALL(LALFloatToInterval(&stat, &overlapCorrection, &overlap), &stat);
	if(options.verbose)
		fprintf(stderr, "time series overlap correction is %u.%09u s\n", overlapCorrection.seconds, overlapCorrection.nanoSeconds);
	}

	/* determine the outer loop's upper bound */
	LAL_CALL(LALAddFloatToGPS(&stat, &boundepoch, &options.stopEpoch, (REAL8) -2.0 * options.FilterCorruption / targetSampleRate), &stat);
	if(options.verbose)
		fprintf(stderr, "series epochs must be less than %u.%09u s\n", boundepoch.gpsSeconds, boundepoch.gpsNanoSeconds);


	/*
	 * ====================================================================
	 *                         Outer Loop
	 * ====================================================================
	 *
	 * Split the total length of time to be analyzed into time series
	 * small enough to fit in RAM.
	 */

	for(epoch = options.startEpoch; CompareGPS(&stat, &epoch, &boundepoch) < 0;) {
		/* compute the number of points to use in this run */
		numPoints = min(options.maxSeriesLength, DeltaGPStoFloat(&stat, &options.stopEpoch, &epoch) * frameSampleRate);

		/*
		 * Get the data,
		 */

		series = get_time_series(&stat, dirname, cachefile, epoch, options.stopEpoch, numPoints, &params);

		/*
		 * Create an empty series of 1s if we didn't read one (eg.
		 * if no input files were specified), then add white noise
		 * to the series.
		 */

		if(options.whiteNoise) {
			if(!series) {
				size_t i;
				LAL_CALL(LALCreateREAL4TimeSeries(&stat, &series, params.channelName, epoch, 0.0, (REAL8) 1.0 / frameSampleRate, lalADCCountUnit, numPoints), &stat);
				for(i = 0; i < series->data->length; i++)
					series->data->data[i] = 1.0;
			}
			makeWhiteNoise(&stat, series, options.seed, options.noiseAmpl);
		}

		/*
		 * Write diagnostic info to disk
		 */

		if(options.printData)
			LALPrintTimeSeries(series, "./timeseriesasq.dat");

		/*
		 * Add burst injections into the time series if requested.
		 */

		if(injectionFile) {
			COMPLEX8FrequencySeries  *response = NULL;

			/* Create the response function */
			if(options.calCacheFile)
				response = generate_response(&stat, options.calCacheFile, series->epoch, series->data->length);

			/* perform injections */
			add_burst_injections(&stat, series, response);
			if(options.printData)
				LALPrintTimeSeries(series, "./injections.dat");

			/* clean up */
			LAL_CALL(LALDestroyCOMPLEX8FrequencySeries(&stat, response), &stat);
		}

		/*
		 * Add MDC injections into the time series if requested.
		 */

		if(mdcCacheFile) {
			add_mdc_injections(&stat, mdcCacheFile, series);
			if(options.printData)
				LALPrintTimeSeries(series, "./timeseriesasqmdc.dat");
		}

		/*
		 * Condition the time series data.
		 */

		LAL_CALL(EPConditionData(&stat, series, params.tfTilingInput.flow, 1.0 / targetSampleRate, resampFiltType, options.FilterCorruption, &params), &stat);

		if(options.verbose)
			fprintf(stderr, "Got %u samples (%.9f s) to analyse after conditioning\n", series->data->length, series->data->length * series->deltaT);

		/*
		 * Store the start and end times of the data that actually
		 * gets analyzed.
		 */

		if(!searchsumm.searchSummaryTable->out_start_time.gpsSeconds)
			searchsumm.searchSummaryTable->out_start_time = series->epoch;
		LAL_CALL(LALAddFloatToGPS(&stat, &searchsumm.searchSummaryTable->out_end_time, &series->epoch, series->deltaT * series->data->length), &stat);


		/*
		 * Analyze the data
		 */

		EventAddPoint = analyze_series(&stat, EventAddPoint, series, &params);

		/*
		 * Reset for next run
		 *
		 * Advancing the epoch by the post-conditioning series length
		 * provides exactly the overlap needed to account for
		 * conditioning corruption.  overlapCorrection provides any
		 * additional overlap that is needed.
		 */

		LAL_CALL(LALAddFloatToGPS(&stat, &epoch, &epoch, series->data->length * series->deltaT), &stat);
		LAL_CALL(LALDecrementGPS(&stat, &epoch, &epoch, &overlapCorrection), &stat);
		LAL_CALL(LALDestroyREAL4TimeSeries(&stat, series), &stat);
	}

	/*
	 * Cluster the events.
	 */

	if(options.cluster)
		burstEvent = cluster_events(&stat, burstEvent, 500);

	/*
	 * Output the results.
	 */

	snprintf(outfilename, sizeof(outfilename)-1, "%s-%s-POWER-%d-%d.xml", ifo, options.comment, options.startEpoch.gpsSeconds, options.stopEpoch.gpsSeconds - options.startEpoch.gpsSeconds);
	outfilename[sizeof(outfilename)-1] = '\0';
	output_results(&stat, outfilename, &procTable, &procparams, &searchsumm, burstEvent);

	/*
	 * Final cleanup.
	 */

	LALFree(procTable.processTable);
	LALFree(searchsumm.searchSummaryTable);

	while(procparams.processParamsTable) {
		ProcessParamsTable *table = procparams.processParamsTable;
		procparams.processParamsTable = table->next;
		LALFree(table);
	}

	LALFree(mdcparams);

	while(burstEvent) {
		SnglBurstTable *event = burstEvent;
		burstEvent = burstEvent->next;
		LALFree(event);
	}

	LALCheckMemoryLeaks();
	exit(0);
}

#endif
