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
//#define DEBUG {fprintf(stderr,"%s: %i\n",__FILE__,__LINE__);fflush(stderr);}

namespace acommon{
  context_filter::context_filter(void)
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
    name_=filtername;
  }
    
  PosibErr<bool> context_filter::setup(Config* config){
    StringList delimiters;
    StringEnumeration *delimiterpairs;
    const char* delimiterpair=NULL;
    char* repair=NULL;
    char* begin=NULL;
    char* end=NULL;
    String delimter;
    char replace='\0';
  
    if(config==NULL){
      fprintf(stderr,"Nothing to be configured\n");
      return true;
    }
  
    NEW_OPTION("delimiters",list,config,&delimiters,\
               "\"\ \"\;/*\ */\;//\ \\0",\
               "add/rem\ delimiter(s) ended/separated by ` ';"\
               ,"context");
    delimiterpairs=delimiters.elements();
    opening.resize(0);
    closing.resize(0);
    while((delimiterpair=delimiterpairs->next())){
      if((begin=repair=strdup(delimiterpair))==NULL){
        return false;
      }
      end=repair+strlen(repair);
      while((*begin!=' ')&&(*begin!='\t')&&(begin!=end)){
        begin++;
      }
      if(begin==repair){
        fprintf(stderr,"no delimiter pair: `%s'\n",repair);
        free(repair);
//replace someday by make_err
        return false;
      }
      if(((*begin==' ')||(*begin=='\t'))&&(begin!=end)){
        *begin='\0';
        opening.resize(opening.size()+1);
        opening[opening.size()-1]=repair;
        begin++;
      }
      while(((*begin==' ')||(*begin=='\t'))&&(begin!=end)){
        begin++;
      }
      if((*begin!=' ')&&(*begin!='\t')&&(begin!=end)){
        closing.resize(closing.size()+1);
        if ( strcmp(begin,"\\0")!=0 ) {
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
    return true;
  } 
    
  
  void context_filter::process(FilterChar*& start,FilterChar*& stop){
    FilterChar* current=start-1;
    FilterChar* beginblind=start;
    FilterChar* endblind=stop;
    FilterChar* localstop=stop;
    int countmasking=0;
    int countdelimit=0;
    int matchdelim=0;

    if((localstop>start+1)&&(*(localstop-1)=='\0')){
      localstop--;
      endblind=localstop;
    }
    if(state==visible){
      beginblind=endblind;
    }
    while((++current<localstop)&&(*current!='\0')){
      if(*current=='\\'){
        countmasking++;
        continue;
      }
      if(state==visible){
        if((countmasking%2==0)&&(correspond>=0)&&
           (correspond<closing.size())&&
           (closing[correspond].length()>0)&&
           (current+closing[correspond].length()<localstop)){
          for(matchdelim=0;matchdelim<closing[correspond].length();
              matchdelim++){
            if(current[matchdelim]!=closing[correspond][matchdelim]){
              break;
            }
          }
          if(matchdelim==closing[correspond].length()){
            beginblind=current;
            endblind=localstop;
            state=hidden;
            correspond=-1;
          }
        }
        countmasking=0;
        continue;
      }
      if(countmasking%2){
        countmasking=0;
        continue;
      }
      countmasking=0;
      for(countdelimit=0;countdelimit<opening.size();countdelimit++){
        for(matchdelim=0;(current+opening[countdelimit].length()<localstop)&&
                         (matchdelim<opening[countdelimit].length());
            matchdelim++){
          if(current[matchdelim]!=opening[countdelimit][matchdelim]){
            break;
          }
        }
        if(matchdelim==opening[countdelimit].length()){
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
    if((state==visible)&&
       (correspond>=0)&&(correspond<closing.size())&&
       (closing[correspond]=="")&&(countmasking%2==0)){
      state=hidden;
      correspond=-1;
    } 
    if(beginblind<endblind){
      hidecode(beginblind,endblind);
    }
  }
  
  PosibErr<bool> context_filter::hidecode(FilterChar* begin,FilterChar* end){
  //here we go, a more efficient context hiding blinding might be used :)
    FilterChar* current=begin;
    while(current<end){
      if((*current!='\t')&&(*current!='\n')&&(*current!='\r')){
        *current=' ';
      }
      current++;
    }
    return true;
  }
        
  void context_filter::reset(void){
    opening.resize(0);
    closing.resize(0);
    state=hidden;
  }
  context_filter::~context_filter(){
    reset();
  }
}
