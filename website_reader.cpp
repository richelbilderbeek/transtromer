#include "website_reader.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/qnetworkreply.h>
#include <QEventLoop>
#include <QObject>

#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "text.h"

website_reader::website_reader(
  const std::map<std::string, std::set<std::string>>& known_rhyme_words
)
  : m_n_reads{0},
    m_rhyme_words{known_rhyme_words}
{

}

std::set<std::string> website_reader::get_rhyme_words(const std::string& word)
{
  if (m_rhyme_words.find(word) != std::end(m_rhyme_words))
  {
    return m_rhyme_words[word];
  }

  const auto text{get_html_content_from_word_as_text(word)};

  std::set<std::string> rhyme_words;
  for (const std::string& s: text)
  {
    const std::string t{boost::trim_copy(s)};
    std::regex e("<li>.*</li>");
    if (std::regex_match(t, e))
    {
      rhyme_words.insert(strip_list_item_xml(t));
    }
  }
  m_rhyme_words[word] = rhyme_words;
  return rhyme_words;
}

std::string website_reader::get_html_content_from_word_as_string(const std::string& word)
{
  QEventLoop loop;
  QNetworkAccessManager nam;
  QNetworkRequest req(QUrl("https://www.rimlexikon.se/ord/" + QString::fromStdString(word)));
  QNetworkReply *reply = nam.get(req);
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  const QByteArray buffer = reply->readAll();
  const std::string s{QString(buffer).toStdString()};
  ++m_n_reads;
  return s;
}

std::vector<std::string> website_reader::get_html_content_from_word_as_text(const std::string& word)
{
  return string_to_text(get_html_content_from_word_as_string(word));
}

std::string read_html_content_from_wordlist_as_string(const int page_number)
{
  QEventLoop loop;
  QNetworkAccessManager nam;
  QNetworkRequest req(
    QUrl(
      "https://www.rimlexikon.se/katalog/?page="
        + QString::number(page_number)
    )
  );
  QNetworkReply *reply = nam.get(req);
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  const QByteArray buffer = reply->readAll();
  const std::string s{QString(buffer).toStdString()};
  return s;
}

std::vector<std::string> read_html_content_from_wordlist_as_text(const int page_number)
{
  return string_to_text(read_html_content_from_wordlist_as_string(page_number));
}

website_reader load_website_reader(const std::string& filename)
{
  std::map<std::string, std::set<std::string>> m;
  const auto text{load_text(filename)};
  for (const auto& s: text)
  {
    auto all_words{seperate_string(s, ',')};
    const auto key{all_words[0]};
    all_words.erase(all_words.begin());
    std::set<std::string> rhyme_words(
      std::begin(all_words),
      std::end(all_words)
    );
    m[key] = rhyme_words;
  }
  return m;
}


void read_and_save_all_words(const std::string& filename)
{
  std::vector<std::string> all_words;
  for (int page_number{1}; page_number != 709; ++page_number)
  {
    std::clog << page_number << "/" << 708 << '\n';
    const auto words{read_word_list(page_number)};
    std::copy(
      std::begin(words),
      std::end(words),
      std::back_inserter(all_words)
    );
  }
  save_text(all_words, filename);
}

/// Read the word list from the website
/// @param page_number value from [1, 708]
/// @see use get_url_text to get the full page's content
std::vector<std::string> read_word_list(const int page_number)
{
  const auto html_text{
    read_html_content_from_wordlist_as_text(page_number)
  };
  std::vector<std::string> words;
  for (const std::string& s: html_text)
  {
    const std::string t{boost::trim_copy(s)};
    std::regex e("<li>.*</li>");
    if (std::regex_match(t, e))
    {
      words.push_back(strip_link_xml(t));
    }
  }
  return words;
}

void save(const website_reader& w, const std::string& filename)
{
  std::ofstream f(filename);
  const auto& m{w.get_known_rhyme_words()};
  for (const auto& [key, value]: m)
  {
    f << key << ',' << strs_to_cs_str(value) << '\n';
  }

}



void test_website_reader()
{
  {
    const website_reader r;
    assert(r.get_n_reads() == 0);
  }
  // get_html_content_from_word_as_string must return something
  {
    website_reader r;
    const std::string s{r.get_html_content_from_word_as_string("bol")};
    assert(!s.empty());
  }
  // get_html_content_from_word_as_text must return something
  {
    assert(website_reader().get_html_content_from_word_as_text("bol").size() > 10);
  }
  // get_html_content_from_wordlist_as_string must return something
  {
    const std::string s{read_html_content_from_wordlist_as_string(1)};
    assert(!s.empty());
  }
  // get_html_content_from_wordlist_as_text must return something
  {
    const auto text{read_html_content_from_wordlist_as_text(1)};
    assert(text.size() > 10);
  }
  {
    assert(string_to_text("a").size() == 1);
    assert(string_to_text("a\nb").size() == 2);
  }
  // get_rhyme_words must return something
  {
    assert(website_reader().get_rhyme_words("bol").size() > 10);
  }
  // reading is cached for get_rhyme_words
  {
    website_reader r;
    assert(r.get_n_reads() == 0);
    const auto v{r.get_rhyme_words("bol")};
    assert(!v.empty());
    assert(r.get_n_reads() == 1);
    const auto w{r.get_rhyme_words("bol")};
    assert(v == w);
    assert(r.get_n_reads() == 1);
  }
  // strip_xml
  {
    assert(strip_list_item_xml("<li>something</li>") == "something");
  }
  // operator==
  {
    const website_reader r;
    const website_reader s;
    assert(r == s);
    assert(!(r != s));
  }
  // operator!=
  {
    const website_reader r;
    std::map<std::string, std::set<std::string>> m;
    m["bol"] = { "hol", "fjol" };
    const website_reader s(m);
    assert(r != s);
    assert(!(r == s));
  }
  // save and load result in identical website_readers
  {
    const std::string filename{"tmp_test.txt"};
    std::map<std::string, std::set<std::string>> m;
    m["bol"] = { "hol", "fjol" };
    const website_reader r(m);
    save(r, filename);
    const website_reader s{load_website_reader(filename)};
    assert(r == s);
  }
}

bool operator==(const website_reader& lhs, const website_reader& rhs) noexcept
{
  return lhs.get_known_rhyme_words() == rhs.get_known_rhyme_words()
    && lhs.get_n_reads() == rhs.get_n_reads()
  ;
}

bool operator!=(const website_reader& lhs, const website_reader& rhs) noexcept
{
  return !(lhs == rhs);
}
