/* cmdline.h */

/* File autogenerated by gengetopt version 2.8  */

#ifndef _cmdline_h
#define _cmdline_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Don't define PACKAGE and VERSION if we use automake.  */
#ifndef PACKAGE
#define PACKAGE ""
#endif

#ifndef VERSION
#define VERSION ""
#endif

struct gengetopt_args_info
{
  char * source_name_arg;	/* Name of source (default='NONAME_SOURCE').  */
  double right_ascension_arg;	/* Right Ascension of source, in rad.  */
  double declination_arg;	/* Declination of source, in rad.  */
  double orientation_arg;	/* Orientation of source, in rad.  */
  char * detector_arg;	/* Detector name; legal names: lho, llo, virgo, geo, tama, cit, test.  */
  int start_time_sec_arg;	/* GPS seconds field of start time of observation.  */
  int start_time_nanosec_arg;	/* GPS nanoseconds field of start time of observation (default='0').  */
  int nsample_arg;	/* number of samples.  */
  double sampling_interval_arg;	/* sampling time interval, in seconds.  */
  int verbosity_arg;	/* verbosity level for debugging (default='0').  */
  int debug_arg;	/* debug level (default='0').  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int single_source_given ;	/* Whether single-source was given.  */
  int whole_sky_given ;	/* Whether whole-sky was given.  */
  int source_name_given ;	/* Whether source-name was given.  */
  int right_ascension_given ;	/* Whether right-ascension was given.  */
  int declination_given ;	/* Whether declination was given.  */
  int orientation_given ;	/* Whether orientation was given.  */
  int detector_given ;	/* Whether detector was given.  */
  int start_time_sec_given ;	/* Whether start-time-sec was given.  */
  int start_time_nanosec_given ;	/* Whether start-time-nanosec was given.  */
  int nsample_given ;	/* Whether nsample was given.  */
  int sampling_interval_given ;	/* Whether sampling-interval was given.  */
  int verbosity_given ;	/* Whether verbosity was given.  */
  int debug_given ;	/* Whether debug was given.  */

} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _cmdline_h */
