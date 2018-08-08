#ifndef _URI_DECODE_
#define _URI_DECODE_

template<typename T>
std::basic_string<T> uri_decode(const std::basic_string<T>& in) {
  std::basic_string<T> out;
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == L'%') {
      if (i + 3 <= in.size()) {
        int value;
        std::basic_istringstream<T> is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<T>(value);
          i += 2;
        }
        else {
	  out.clear();
          return out;
        }
      }
      else {
	  out.clear();
	  return out;
      }
    }
    else if (in[i] == L'+') {
      out += L' ';
    }
    else {
      out += in[i];
    }
  }
  return move(out);
}

#endif

