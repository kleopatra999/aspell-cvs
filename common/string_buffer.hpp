// This file is part of The New Aspell
// Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#ifndef ACOMMON_STRING_BUFFER__HPP
#define ACOMMON_STRING_BUFFER__HPP

#include "parm_string.hpp"
#include "basic_list.hpp"

namespace acommon {

  class StringBuffer {
  public: // but don't use
    static const unsigned int   buf_size = 1024 - 16;
    struct Buf {
      char buf[buf_size];
    };
  private:
    static const Buf sbuf;
    BasicList<Buf> bufs;
    unsigned int fill;
    char * alloc(unsigned int size, const char * str);
  public:
    StringBuffer();
    char * alloc(unsigned int size) {return alloc(size, 0);}
    char * dup(ParmString str) {return alloc(str.size() + 1, str.str());}
  };

}

#endif // ACOMMON_STRING_BUFFER__HPP
