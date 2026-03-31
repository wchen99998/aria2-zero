#include "boost_locale_libintl.h"

#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include <locale>
#include <string>

namespace {

struct LocaleState {
  boost::locale::generator generator;
  std::string locale_name;
  std::string domain;
  std::locale locale;

  LocaleState(): locale_name(""), locale(generator("")) {}

  void regenerate() { locale = generator(locale_name.empty() ? "" : locale_name); }
};

LocaleState& locale_state()
{
  static LocaleState state;
  return state;
}

} // namespace

const char* boost_locale_generate(const char* locale_id)
{
  auto& state = locale_state();
  state.locale_name = locale_id ? locale_id : "";
  state.regenerate();
  std::locale::global(state.locale);
  return state.locale_name.c_str();
}

void boost_locale_add_domain(const char* domain)
{
  if (!domain || !*domain) {
    return;
  }
  auto& state = locale_state();
  state.generator.add_messages_domain(domain);
  state.domain = domain;
  state.regenerate();
}

void boost_locale_add_path(const char* path)
{
  if (!path || !*path) {
    return;
  }
  auto& state = locale_state();
  state.generator.add_messages_path(path);
  state.regenerate();
}

const char* boost_locale_gettext(const char* msgid)
{
  if (!msgid) {
    return "";
  }
  thread_local std::string buffer;
  const auto& state = locale_state();
  if (state.domain.empty()) {
    buffer = boost::locale::translate(msgid).str(state.locale);
  } else {
    buffer = boost::locale::translate(msgid).str(state.locale, state.domain);
  }
  return buffer.c_str();
}

const char* boost_locale_dgettext(const char* domain, const char* msgid)
{
  if (!msgid) {
    return "";
  }
  thread_local std::string buffer;
  const auto& state = locale_state();
  const std::string use_domain = (domain && *domain) ? domain : state.domain;
  if (use_domain.empty()) {
    buffer = boost::locale::translate(msgid).str(state.locale);
  } else {
    buffer = boost::locale::translate(msgid).str(state.locale, use_domain);
  }
  return buffer.c_str();
}
