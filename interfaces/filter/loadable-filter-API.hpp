// This file is part of The New Aspell
// Copyright (C) 2002 by Christoph Hintermüller under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.
//
// Added by Christoph Hintermüller
// This file contains macros useful within loadable filter classes
// These macros are required if one wants to use make_opt.awk script
// (might be changed to make_opt.pl).
//
// Further the FILTER_API_EXPORTS and FILTER_API_IMPORTS macros needed
// for making filter usable in Windows too without changes 
//             
#ifndef LOADABLE_FILTER_API_HEADER
#define LOADABLE_FILTER_API_HEADER
#include "posib_err.hpp"
#include "config.hpp"
#include "indiv_filter.hpp"

#if defined(__CYGWIN__) || defined (_WIN32)
#define FILTER_API_EXPORTS __declspec(dllexport)
#define FILTER_API_IMPORTS __declspec(dllimport)
#else
#define FILTER_API_EXPORTS
#define FILTER_API_IMPORTS
#endif

/* The following macros hide some details about option retrievement in aspell.
 * They offer a common interface to the different types of options:
 * Bool:   Option of boolean type;
 * Int:    Number  value;
 * String: Option containing a std::String;
 * List:   returns a list of strings; use StringEnumeration from aspell 
 *         common directory.
 *
 * For each of these macros the following parameters have to be specified:
 * config:  the name of the configuration object.
 * name:    a string containing name of the option to retrieve.
 * returns: the name of the variable the value of the option shoul be saved
 *          to, in case of list this has to be maintained by filter.
 * filter:  as there might be nore than one filters contained in one source
 *          file it is necessary to specifiy the name of the filter the
 *          option belongs to.
 */
#ifndef STRING_OPTION
#define STRING_OPTION(config,name,returns,filter,feature) \
        RET_ON_ERR_SET(config->retrieve("filter-"filter"-"name),String,returns)}
#endif

#ifndef BOOL_OPTION
#define BOOL_OPTION(config,name,returns,filter,feature) \
        RET_ON_ERR_SET(config->retrieve_bool("filter-"filter"-"name),bool,returns)
#endif

#ifndef INT_OPTION
#define INT_OPTION(config,name,returns,filter,feature) \
        RET_ON_ERR_SET(config->retrieve_int("filter-"filter"-"name),int,returns)
#endif

#ifndef LIST_OPTION
#define LIST_OPTION(config,name,returns,filter,feature) \
        RET_ON_ERR(config->retrieve_list("filter-"filter"-"name,returns))
#endif

/* The following macros have no impact on the behaviour of the filter, but
 * they allow the mk-flt-opt.pl script to compile the .opt file from the
 * sources. Even if the mk-flt-opt.pl script is not used during developement
 * of filter they should be specified inside the sources, as this eases
 * inclusion of the filter into Aspell standard distribution.
 */

/* ENCODER, FILTER, DECODER:
 *          Indicates that the following part of Code belongs to a new iencoder,
 *          filter oder decoder. The name of the filter is specified by the
 *          string name, which is stored in the variable save, it can be called
 *          from any point of your code. 
 * Note: Call this macro inside your filter class declaration.
 */  

#ifndef ENCODER
#define ENCODER(name,save,class_name) \
public: const char * save(void) { \
    return name"-encoder";\
  }\
private:
#endif

#ifndef FILTER
#define FILTER(name,save,class_name) \
public: const char * save(void) { \
    return name"-filter";\
  }\
private:
#endif

#ifndef DECODER
#define DECODER(name,save,class_name) \
public: const char * save(void) { \
    return name"-decoder";\
  }\
private:
#endif

#ifndef ALIAS
#define ALIAS(alias_name,filter)
#endif
/* ACTIVATE_ENCODER, ACTIVATE_FILTER, ACTIVATE_DECODER:
 *          Before a encoding, decoding or processing filter can be
 *          loaded by aspell filter interface it has to be activated.
 *          If a specific class is denoted as encoder, decoder or filter
 *          by one of the above macros use the following ones to activate
 *          the entire filter feature.
 *          If not than write a wraperfunction named new_fencoder, new_filter or
 *          new_decoder and declare ist as extern C. This function should look like
 *          
 *          extern "C" acommon::IndividualFilter * new_<feture>(void){
 *            return new <YourFilterClass>;
 *          }
 *
 *          <feature> stands for either encoder, filter or decoder;
 *          <YourFilterClass> is the name of the corresponding class.
 * Note: call this outside your filter class declaration. If your filter class
 *       does not contain to any specific namespace or these macros are called
 *       inside the same namespace as the class containing the entire feature, 
 *       nspace may be empty (not tested);
 */
#ifndef ACTIVATE_ENCODER
#define ACTIVATE_ENCODER(nspace,class_name,filter) \
extern "C" { \
  FILTER_API_EXPORTS acommon::IndividualFilter * new_encoder(void) {\
    return new nspace::class_name;\
  }\
}
#endif
#ifndef ACTIVATE_FILTER
#define ACTIVATE_FILTER(nspace,class_name,filter) \
extern "C" { \
  FILTER_API_EXPORTS acommon::IndividualFilter * new_filter(void) {\
    return new nspace::class_name;\
  }\
}
#endif
#ifndef ACTIVATE_DECODER
#define ACTIVATE_DECODER(nspace,class_name,filter) \
  FILTER_API_EXPORTS acommon::IndividualFilter * new_decoder(void) {\
    return new nspace::class_name;\
  }\
extern "C" { \
}
#endif

/* ASPELL_VERSION: This may be used to explicitly specify the Aspell version
 *                 the filter should be usable for. The version number may 
 *                 be prefixed by `<', `<=', `=', `>=' or `>' to exclude
 *                 some versions of aspell and admit the consecutive or prior
 *                 versions. ASPELL_VERSION is not exolicitly specified
 *                 mk-flt-opt.pl assumes the actual version prefixed by `>='.
 */
#ifndef ASPELL_VERSION
#define ASPELL_VERSION(name,version)
#endif

/* SET_DESCRIPTION: This should be used to specify a short description of the
 *                  Filter. If mk-flt-opt.pl is used to generate the .opt
 *                  file Aspell will display this line as a short description.
 *                  of the filter or filter option when running help and
 *                  config action.
 *
 *                  If "name" is equal to "filter" and a filter named "name"
 *                  exists mk-flt-opt.pl will use descript to generate the
 *                  description of the filter. In any other case
 *                  mk-flt-opt.pl tries to generate the description, for the
 *                  option "name" for filter "filter".
 *                  
 * Note: If there is need for one of the following chars use their synonym as 
 *       shown below. This is necessary to keep mk-flt-opt.pl as simple as
 *       possible.
 *       Use `\{' for `(' and `\}' for `)';
 *       Use `\;' for `,' and `\'' for `"'; 
 */
#ifndef SET_DESCRIPTION
#define SET_DESCRIPTION(name,descript,filter)
#endif

/* SET/ADD_DEFAULT: Sets or adds the string "set" to the default values for
 *                  the option "name" of filter "filter".
 *                  
 * Note: mk-flt-opt.pl will accept default values set via SET_DEFAULT macro
 *       for all types of filter options (int,bool,string,list). ADD_DEFAULT
 *       macro on the other hand may only be used for extending list of
 *       default values for list type options.
 *       
 * Note: If there is need for one of the following chars use their synonym as 
 *       shown below. This is necessary to keep mk-flt-opt.pl as simple as
 *       possible.
 *       Use `\{' for `(' and `\}' for `)';
 *       Use `\;' for `,' and `\'' for `"'; 
 */
#ifndef SET_DEFAULT
#define SET_DEFAULT(name,set,filter)
#endif

#ifndef ADD_DEFAULT
#define ADD_DEFAULT(name,set,filter)
#endif

#endif
