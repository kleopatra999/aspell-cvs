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

#if defined(__CYGWIN__) || defined (_WIN32)
#define FILTER_API_EXPORTS __declspec(dllexport)
#define FILTER_API_IMPORTS __declspec(dllimport)
#else
#define FILTER_API_EXPORTS
#define FILTER_API_IMPORTS
#endif

//do not call directly as called by OPTION and NEW_OPTION macro
#ifndef string_opt
#define string_opt(config,name,returns,filter) \
        RET_ON_ERR_SET(config->retrieve("filter-"filter"-"name),String,returns)
#endif

//do not call directly as called by OPTION and NEW_OPTION macro
#ifndef bool_opt
#define bool_opt(config,name,returns,filter) \
        RET_ON_ERR_SET(config->retrieve_bool("filter-"filter"-"name),bool,returns)
#endif

//do not call directly as called by OPTION and NEW_OPTION macro
#ifndef int_opt
#define int_opt(config,name,returns,filter) \
        RET_ON_ERR_SET(config->retrieve_int("filter-"filter"-"name),int,returns)
#endif

//do not call directly as called by OPTION and NEW_OPTION macro
#ifndef list_opt
#define list_opt(config,name,returns,filter) \
        RET_ON_ERR(config->retrieve_list("filter-"filter"-"name,returns))
#endif

//use this if you have already called NEW_OPTION once for a specific
//option to retrieve it 
#ifndef OPTION
#define OPTION(name,type,config,returns,filter) \
        type##_opt(config,name,returns,filter)
#endif

//!NEW_OPTION also used by make_opt to extract *.opt file
//!name: the name of the option
//!type: the type of the option
//!config: the configuration from which the option should be
//!        retrieved
//!returns: the variable the contents of the options should
//!         be saved to
//!def: the default value of the option
//!des: the help text
//!filter: the name of the filter the option belongs to
//!        see DESCRIPTION macro below
//!        it is required if .opt file is generated automatically
//!use `\;' instead of `,' in def and des field 
//!the `\;' will be expanded by make_opt.awk to `,'
//!if you use `,' directly in des and def  make_opt.awk will be
//!confused and abort with an improper option error. 
#ifndef NEW_OPTION
#define NEW_OPTION(name,type,config,returns,def,des,filter) \
        OPTION(name,type,config,returns,filter)
#endif

//!used by make_opt to extract *.opt file
//!name is the name of the filter version
//!ver is the actual version of the filter
//!desc a short description of the filter
//!aspell is the version of Aspell required by filter
//!       it may start with >= <= =,
//!       per default = will be assumed
//!save static variable the name will be saved to.
//!it may be used to initialise the name member of IndividualFilter
//!use `\;' instead of `,' in the desc field
//!the `\;' will be expanded by make_opt.awk to `,'
//!if you use `,' directly in desc field make_opt.awk will be
//!confused and abort with an improper filter error. 
#ifndef DESCRIPTION
#define DESCRIPTION(name,ver,desc,aspell,save) \
        static char* save = name;
#endif

#endif
