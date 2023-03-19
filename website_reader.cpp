#include "website_reader.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/qnetworkreply.h>
#include <QEventLoop>
#include <QObject>


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

  const auto text{get_url_content_as_text(word)};

  std::set<std::string> rhyme_words;
  for (const std::string& s: text)
  {
    const std::string t{boost::trim_copy(s)};
    std::regex e("<li>.*</li>");
    if (std::regex_match(t, e))
    {
      rhyme_words.insert(strip_xml(t));
    }
  }
  m_rhyme_words[word] = rhyme_words;
  return rhyme_words;
}

std::string website_reader::get_url_content_as_string(const std::string& word)
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

std::vector<std::string> website_reader::get_url_content_as_text(const std::string& word)
{
  return string_to_text(get_url_content_as_string(word));
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
  // get_url_text_as_string must return something
  {
    website_reader r;
    const std::string s{r.get_url_content_as_string("bol")};
    assert(!s.empty());
  }
  // get_url_text_as_text must return something
  {
    assert(website_reader().get_url_content_as_text("bol").size() > 10);
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
    assert(strip_xml("<li>something</li>") == "something");
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
