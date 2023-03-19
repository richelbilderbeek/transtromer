#ifndef WEBSITE_READER_H
#define WEBSITE_READER_H

#include <set>
#include <map>
#include <string>
#include <vector>

/// Reads the rhyme words from the website
/// Will cache earlier results
class website_reader
{
public:
  website_reader(
    const std::map<std::string, std::set<std::string>>& known_rhyme_words = {}
  );

  const std::map<std::string, std::set<std::string>>& get_known_rhyme_words() const noexcept { return m_rhyme_words; }

  int get_n_reads() const noexcept { return m_n_reads; }

  /// Get the full website text for a word
  /// @see use get_url_text to get the full page's content
  std::set<std::string> get_rhyme_words(const std::string& word);

private:

  /// Number of time the website has been read
  int m_n_reads;

  /// The cached rhyme words
  std::map<std::string, std::set<std::string>> m_rhyme_words;

  /// Get the full website text for a word as a string
  /// @see use get_rhyme_words to get the rhyming words
  std::string get_url_content_as_string(const std::string& word);

  /// Get the full website text for a word as a text
  /// @see use get_rhyme_words to get the rhyming words
  std::vector<std::string> get_url_content_as_text(const std::string& word);

  friend void test_website_reader();
};

/// Load a `website_reader` from file
website_reader load_website_reader(const std::string& filename);

/// Save the `website_reader` to file
void save(const website_reader& w, const std::string& filename);

/// Tests this class
void test_website_reader();

bool operator==(const website_reader& lhs, const website_reader& rhs) noexcept;
bool operator!=(const website_reader& lhs, const website_reader& rhs) noexcept;

#endif // WEBSITE_READER_H
