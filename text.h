#ifndef TEXT_H
#define TEXT_H

/// Functions that deal with one or more strings

#include <set>
#include <string>
#include <vector>

std::vector<std::string> load_text(
  const std::string& filename
);

void save_text(
  const std::vector<std::string>& text,
  const std::string& filename
);

/// Split a comma-seperated string
// From https://github.com/richelbilderbeek/seperate_string/blob/master/seperate_string.cpp#L157
std::vector<std::string> seperate_string(
  const std::string& input,
  const char seperator = ','
);

std::istream& stream_to_text(std::istream& is, std::vector<std::string>& v);

/// Convert a string to a text, split on a newline
// From https://stackoverflow.com/a/55263720
std::vector<std::string> string_to_text(const std::string& s);

/// Removes the XML tags '<li>' and '</li>' at the start
/// and end of the string
std::string strip_xml(const std::string& s);

/// Convert strings to one comma-seperated string
std::string strs_to_cs_str(const std::set<std::string>& v);

void test_text();

std::ostream& text_to_stream(std::ostream& os, const std::vector<std::string>& w);


#endif // TEXT_H
