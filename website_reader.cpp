#include "website_reader.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/qnetworkreply.h>
#include <QEventLoop>
#include <QObject>

#include <boost/algorithm/string.hpp>

#include <regex>
#include <sstream>

website_reader::website_reader()
  : m_n_reads{0}
{

}

std::vector<std::string> website_reader::get_rhyme_words(const std::string& word)
{
  if (m_rhyme_words.find(word) != std::end(m_rhyme_words))
  {
    return m_rhyme_words[word];
  }

  const auto text{get_url_content_as_text(word)};

  std::vector<std::string> rhyme_words;
  for (const std::string& s: text)
  {
    const std::string t{boost::trim_copy(s)};
    std::regex e("<li>.*</li>");
    if (std::regex_match(t, e))
    {
      rhyme_words.push_back(strip_xml(t));
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

std::string strip_xml(const std::string& s)
{
  assert(s.substr(0, 4) == "<li>");
  assert(s.substr(s.length() - 5, 5) == "</li>");
  return s.substr(4, s.length() - 4 - 5);
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
}
