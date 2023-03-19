#ifndef WEBSITE_READER_H
#define WEBSITE_READER_H

#include <string>
#include <vector>

class website_reader
{
public:
  website_reader();

  /// Get the full website text for a word
  /// @see use get_url_text to get the full page's content
  std::vector<std::string> get_rhyme_words(const std::string& word);

  /// Get the full website text for a word as a string
  /// @see use get_rhyme_words to get the rhyming words
  std::string get_url_content_as_string(const std::string& word);

  /// Get the full website text for a word as a text
  /// @see use get_rhyme_words to get the rhyming words
  std::vector<std::string> get_url_content_as_text(const std::string& word);
};

/// Convert a string to a text, split on a newline
// From https://stackoverflow.com/a/55263720
std::vector<std::string> string_to_text(const std::string& s);

/// Removes the XML tags '<li>' and '</li>' at the start
/// and end of the string
std::string strip_xml(const std::string& s);

/// Tests this class
void test_website_reader();

#endif // WEBSITE_READER_H
