// This file is part of The New Aspell
// Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#ifndef ACOMMON_VECTOR__HPP
#define ACOMMON_VECTOR__HPP

#include <vector>

namespace acommon 
{
  template <typename T>
  class Vector : public std::vector<T>
  {
  public:
    void append(T t) {
      this->push_back(t);
    }
    void append(const T * begin, unsigned int size) {
      insert(this->end(), begin, begin+size);
    }
    T * data() {
      return &this->front();
    }

    T * pbegin() {
      return &*this->begin();
    }
    T * pend() {
      return &*this->end();
    }
  };
}

#endif
