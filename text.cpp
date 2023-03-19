#include "text.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

std::vector<std::string> load_text(
  const std::string& filename
)
{
  std::vector<std::string> text;
  std::ifstream is(filename);
  stream_to_text(is, text);
  return text;
}

void save_text(
  const std::vector<std::string>& text,
  const std::string& filename
)
{
  std::ofstream os(filename);
  text_to_stream(os, text);

}

std::vector<std::string> seperate_string(
  const std::string& input,
  const char seperator
)
{
  std::vector<std::string> v;
  boost::algorithm::split(v, input, [seperator](const char c) { return c == seperator; } );
  return v;
}

// From https://stackoverflow.com/a/55263720
std::vector<std::string> string_to_text(const std::string& s)
{
  std::string tmp;
  std::stringstream ss(s);
  std::vector<std::string> words;

  while(std::getline(ss, tmp, '\n')){
      words.push_back(tmp);
  }
  return words;
}

std::string strs_to_cs_str(const std::set<std::string>& v)
{
  std::stringstream s;
  for (const auto& e: v) s << e << ',';
  std::string t{s.str()};
  if (t.empty()) return "";
  t.pop_back();
  return t;

}

std::string strip_xml(const std::string& s)
{
  assert(s.substr(0, 4) == "<li>");
  assert(s.substr(s.length() - 5, 5) == "</li>");
  return s.substr(4, s.length() - 4 - 5);
}

std::ostream& text_to_stream(std::ostream& os, const std::vector<std::string>& w)
{
  //Copy the original std::vector
  std::vector<std::string> v = w;
  //Preformat data
  std::for_each(v.begin(),v.end(),
    [&os](std::string& s)
    {
      //The only assertion done on the input
      //Note that users nearly every use bell characters in their text inputs
      assert(std::count(s.begin(),s.end(),'\b') == 0 && "Text must not contain a bell character");
      std::replace(s.begin(),s.end(),' ','\b');
      if (s == "</>") s = "<\b/>";
    }
  );
  //Check data
  #ifndef NDEBUG
  std::for_each(v.begin(),v.end(),
    [&os](const std::string& s)
    {
      assert(s != "</>" && "Text shoule not be '</>' anymore");
      assert(std::count(s.begin(),s.end(),' ') == 0 && "Text should not contain spaces anymore");
    }
  );
  #endif
  //Write data
  std::for_each(v.begin(),v.end(),
    [&os](const std::string& s)
    {
      os << s << '\n';
    }
  );
  return os;
}

std::istream& stream_to_text(
  std::istream& is,
  std::vector<std::string>& v
)
{
  //Read start tag
  while (!is.eof())
  {
    std::string s;
    is >> s;
    if (s.empty()) return is;
    std::replace(s.begin(),s.end(),'\b',' ');
    v.push_back(s);
  }
  return is;
}

void test_text()
{
  //Go ahead, create an entry that breaks the code!
  const std::vector<std::string> v(
    {
      "aahs",
      "aals",
      "abac",
      "abas",
      "abba",
      "abbe",
      "abbs",
      "abed",
      "abet",
      "abid"
    }
  );
  const std::string filename = "tmp.txt";
  //Write to file
  {
    std::ofstream f(filename.c_str());
    text_to_stream(f, v);
  }
  // save_text
  {
    save_text(v, filename);
    const auto text{load_text(filename)};
    assert(text == v);
  }
  //Read from file
  {
    std::vector<std::string> w;
    std::ifstream f(filename.c_str());
    stream_to_text(f, w);
    if (v != w)
    {
      std::copy(w.begin(), w.end(), std::ostream_iterator<std::string>(std::cout,"\n"));
    }
    assert(v == w && "Because the algorithm is excellent, this will never happen B-)");
  }
}
