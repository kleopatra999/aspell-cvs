/* This file is part of The New Aspell
 * Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/.                                              */

#include "settings.h"

#include "config.hpp"
#include "filter.hpp"
#include "speller.hpp"
#include "indiv_filter.hpp"
#include "copy_ptr-t.hpp"
#include "strtonum.hpp"
#include "errors.hpp"
#ifdef HAVE_LIBDL
#include <dlfcn.h>
#endif

namespace acommon {

  Filter::Filter() {}

  void Filter::add_filter(IndividualFilter * filter)
  {
    Filters::iterator cur, end;
    cur = filters_.begin();
    end = filters_.end();
    while ((cur != end) && (filter->order_num() > (*cur)->order_num())){
      ++cur;
    }
    filters_.insert(cur, filter);
  }

  void Filter::reset()
  {
    Filters::iterator cur, end;
    cur = filters_.begin();
    end = filters_.end();
    for (; cur != end; ++cur)
      (*cur)->reset();
  }

  void Filter::process(FilterChar * & start, FilterChar * & stop)
  {
    Filters::iterator cur, end;
    cur = filters_.begin();
    end = filters_.end();
    for (; cur != end; ++cur)
      (*cur)->process(start, stop);
  }

  void Filter::clear()
  {
    Filters::iterator cur, end;
    cur = filters_.begin();
    end = filters_.end();
    for (; cur != end; ++cur){
      delete *cur;
    }
    filters_.clear();
  }

  Filter::~Filter() 
  {
    clear();
  }



  PosibErr<bool> verify_version(const char * relOp, const char * actual, 
                                const char * required, const char * module) 
  {
    assert(actual != NULL && required != NULL);

    char * actVers = (char *) actual;
    char * reqVers = (char *) required;
    while ( * actVers != '\0' || * reqVers != '\0'  ) {

      char * nextActVers = actVers;
      char * nextReqVers = reqVers;
      int actNum = strtoi_c(actVers,&nextActVers);
      int reqNum = strtoi_c(reqVers,&nextReqVers);


      if ( nextReqVers ==  reqVers) {
        while ( *nextReqVers == 'x' || *nextReqVers == 'X' ) {
          nextReqVers++;
        }
        if ( reqVers == nextReqVers && reqVers != '\0') {
          return make_err(bad_version_string);
        }
        else if ( reqVers != '\0' ) {
          reqNum = actNum;
        }
      }
      if (    ( nextActVers == actVers )
           && ( actVers != '\0' ) ) {
        return make_err(bad_version_string);
      }
      if ( relOp != NULL && relOp[0] == '>' && actVers != '\0' && 
           ( reqVers == '\0' || actNum > reqNum ) ) {
        return true;
      }
      if ( relOp != NULL && relOp[0] == '<' && reqVers != '\0' &&
           ( actVers == '\0' || actNum < reqNum ) ) {
        return true;
      }
      if ( actNum == reqNum ) {
        actVers = nextActVers;
        reqVers = nextReqVers;
        while ( *actVers == '.' ) {
          actVers++;
        }
        while ( *reqVers == '.' ) {
          reqVers++;
        }
        continue;
      }
      if ( relOp != NULL && relOp[0] == '!' ) {
        return true;
      }
      return false;
    }
    if ( relOp != NULL && relOp[0] != '\0' &&
         ( relOp[0] == '!'  || 
           ( relOp[1] != '=' && relOp[0] != '=' ) ) ) {
      return false;
    }
    return true;
  }

}

