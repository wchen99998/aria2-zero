#ifndef D_COMPAT_BOOST_LOCALE_LIBINTL_H
#define D_COMPAT_BOOST_LOCALE_LIBINTL_H

#ifdef __cplusplus
extern "C" {
#endif

const char* boost_locale_gettext(const char* msgid);
const char* boost_locale_dgettext(const char* domain, const char* msgid);
const char* boost_locale_generate(const char* locale_id);
void boost_locale_add_path(const char* path);
void boost_locale_add_domain(const char* domain);

#ifdef __cplusplus
}
#endif

#endif // D_COMPAT_BOOST_LOCALE_LIBINTL_H
