// This file is part of The New Aspell
// Copyright (C) 2002 by Christoph Hintermüller under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.
//
// Example for a filter implementation usable via extended filter library
// interface.
// This was added to Aspell by Christoph Hintermüller
// For further information see see development manual ccpp-context.h file
// and loadable-filter-API.hpp

#include <stdio.h>
#include "iostream.hpp"
#include "string_list.hpp"
#include "string_enumeration.hpp"
#include "ccpp-context.hpp"

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG {fprintf(stderr,"%s: %i\n",__FILE__,__LINE__);}

namespace acommon{
  ContextFilter::ContextFilter(void)
  : opening(),
    closing()
  {
    state=hidden;
    correspond=-1;
    opening.resize(3);
    opening[0]="\"";
    opening[1]="/*";
    opening[2]="//";
    closing.resize(3);
    closing[0]="\"";
    closing[1]="*/";
    closing[2]="";
    filterversion=VERSION;
    debugoutput=fopen("mydebugfile.dbg","a");
  }
    
  PosibErr<bool> ContextFilter::setup(Config * config){
    StringList delimiters;
    StringEnumeration * delimiterpairs;
    const char * delimiterpair=NULL;
    char * repair=NULL;
    char * begin=NULL;
    char * end=NULL;
    String delimiter;
    unsigned int countdelim=0;
  
    if( config == NULL ){
      fprintf(stderr,"Nothing to be configured\n");
      return true;
    }
    
    BOOL_OPTION(config,"visible-first",invertstate,"context","filter");
    SET_DESCRIPTION("visible-first",\
                "swaps visible and invisible text;"\
                "\{starts invisible\}","context");
    SET_DEFAULT("visible-first","false","context");
    if(invertstate){
      state=visible;
    }
    LIST_OPTION(config,"delimiters",&delimiters,"context","filter");
    ADD_DEFAULT("delimiters","\\' \\'","context");
    ADD_DEFAULT("delimiters","/* */","context");
    ADD_DEFAULT("delimiters","// \\0","context");
    SET_DESCRIPTION("delimiters","add/rem delimiter\{s\} separated by ` ';",\
                    "context");
    delimiterpairs=delimiters.elements();
    opening.resize(0);
    closing.resize(0);
    while( ( delimiterpair=delimiterpairs->next() ) ){
      if( ( begin=repair=strdup(delimiterpair) ) == NULL ){
        fprintf(stderr,"ifailed to initialise %s filter\n",filter_name());
        return false;
      }
      end=repair+strlen(repair);
      while( ( *begin != ' ' ) && ( *begin != '\t' ) && ( begin != end ) ){
        begin++;
      }
      if( begin == repair ){
        fprintf(stderr,"no delimiter pair: `%s'\n",repair);
        free(repair);
//FIXME replace someday by make_err
        fprintf(stderr,"ifailed to initialise %s filter\n",filter_name());
        return false;
      }
      if( ( ( *begin == ' ' ) || ( *begin == '\t' ) ) && ( begin != end ) ){
        *begin='\0';
        opening.resize(opening.size()+1);
        opening[opening.size()-1]=repair;
        begin++;
      }
      while( ( ( *begin == ' ' ) || ( *begin == '\t' ) ) && ( begin != end ) ){
        begin++;
      }
      if( ( *begin != ' ' ) && ( *begin != '\t' ) && ( begin != end ) ){
        closing.resize(closing.size()+1);
        if ( strcmp(begin,"\\0") != 0 ) {
          closing[closing.size()-1]=begin;
        }
        else {
          closing[closing.size()-1]="";
        }
      }
      else{
        closing.resize(closing.size()+1);
        closing[closing.size()-1]="";
      }
      free(repair);
    }
    if (invertstate) {
      for(countdelim=0;(countdelim < opening.size()) &&
                       (countdelim < closing.size()); countdelim++) {
        delimiter=opening[countdelim];
        opening[countdelim]=closing[countdelim];
        closing[countdelim]=delimiter;
      }
    }
    fprintf(stderr,"%s filter initialised\n",filter_name());
    return true;
  } 
    
  
  void ContextFilter::process(FilterChar *& start,FilterChar *& stop){
    FilterChar * current=start-1;
    FilterChar * beginblind=start;
    FilterChar * endblind=stop;
    FilterChar * localstop=stop;
    int countmasking=0;
    int countdelimit=0;
    int matchdelim=0;

    if( (localstop > start+1 ) && ( *(localstop-1) == '\0' ) ){
      localstop--;
      endblind=localstop;
    }
    if( state == visible ){
      beginblind=endblind;
    }
    while( ( ++current < localstop ) && ( *current != '\0' ) ){
      if( *current == '\\' ){
        countmasking++;
        continue;
      }
      if( state == visible ){
        if ( ( countmasking % 2 == 0 ) && ( correspond < 0 ) ) {
          for( countdelimit=0;
                countdelimit < (signed) closing.size(); countdelimit++){
            for( matchdelim=0; 
                 ( current+closing[countdelimit].length() < localstop ) &&
                 ( matchdelim < (signed) closing[countdelimit].length() );
                 matchdelim++ ){
//FIXME Warning about comparison of signed and unsigned in following line
              if( current[matchdelim] != closing[countdelimit][matchdelim] ){
                break;
              }
            }
            if ((matchdelim == (signed) closing[countdelimit].length()) &&
                closing[countdelimit].length()) {
              correspond=countdelimit;
              break;
            }
          }
        }
        if ( ( countmasking % 2 == 0 ) && ( correspond >= 0 ) &&
           ( correspond < (signed) closing.size() )&&
           ( closing[correspond].length() > 0 )&&
           ( current+closing[correspond].length() < localstop ) ){
          for( matchdelim=0 ; matchdelim < (signed) closing[correspond].length() ;
              matchdelim++ ){
//FIXME Warning about comparison of signed and unsigned in following line
            if( current[matchdelim] != closing[correspond][matchdelim] ){
              break;
            }
          }
          if ((matchdelim == (signed) closing[correspond].length()) &&
              closing[correspond].length()) {
            beginblind=current;
            endblind=localstop;
            state=hidden;
            correspond=-1;
          }
        }
        countmasking=0;
        continue;
      }
      if( countmasking % 2 ){
        countmasking=0;
        continue;
      }
      countmasking=0;
      for( countdelimit=0; countdelimit < (signed) opening.size(); countdelimit++){
        for( matchdelim=0; ( current+opening[countdelimit].length() < localstop ) &&
                           ( matchdelim < (signed) opening[countdelimit].length() );
             matchdelim++ ){
//FIXME Warning about comparison of signed and unsigned in following line
          if( current[matchdelim] != opening[countdelimit][matchdelim] ){
            break;
          }
        }
        if ((matchdelim == (signed) opening[countdelimit].length()) &&
            opening[countdelimit].length()) {
          endblind=current+opening[countdelimit].length();
          state=visible;
          hidecode(beginblind,endblind);
          current=endblind-1;
          beginblind=endblind=localstop;
          correspond=countdelimit;
          break;
        }
      }
    }
    if( ( state == visible ) &&
       ( correspond >= 0 ) && ( correspond < (signed) closing.size() ) &&
       ( closing[correspond] == "" ) && ( countmasking % 2 == 0 ) ){
      state=hidden;
      correspond=-1;
    } 
    if( beginblind < endblind ){
      hidecode(beginblind,endblind);
    }
  }
  
  PosibErr<bool> ContextFilter::hidecode(FilterChar * begin,FilterChar * end){
  //FIXME here we go, a more efficient context hiding blinding might be used :)
    FilterChar * current=begin;
    while( current < end ){
      if( ( *current != '\t' ) && ( *current != '\n' ) && ( *current != '\r' ) ){
        *current=' ';
      }
      current++;
    }
    return true;
  }
        
  void ContextFilter::reset(void){
    opening.resize(0);
    closing.resize(0);
    state=hidden;
  }
  ContextFilter::~ContextFilter(){
    reset();
  }
}
