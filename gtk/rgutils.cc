/* rgmisc.cc
 *
 * Copyright (c) 2003 Michael Vogt
 *
 * Author: Michael Vogt <mvo@debian.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "config.h" // IWYU pragma: associated

#include "rgutils.h"

#include <apt-pkg/fileutl.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <format>
#include <gtk/gtk.h>
#include <memory>
#include <pwd.h>
#include <string>

using namespace std;

void RGFlushInterface()
{
   while (gtk_events_pending()) {
      gtk_main_iteration();
   }
}

// Convert the bytes count to an human readable SI string with at most 7 characters.
// Decimal units are used (i.e. 1 kB = 1000 bytes), the sign is discarded.
string RGSizeToStr(uint64_t bytes)
{
   constexpr string_view units[] = { "kB", "MB", "GB", "TB", "PB", "EB" };

   if (bytes < 10000)
   {
      return format("{} B", bytes);
   }

   double value = static_cast<double>(bytes) / 1000.0;
   size_t unit = 0;

   while ( (value >= 1000.0) && (unit < (size(units) - 1)) )
   {
      value /= 1000.0;
      unit++;
   }

   size_t decimals = (value >= 100.0) ? 0 : (value >= 10.0) ? 1 : 2;

   if (((decimals == 1) && (value >= 99.95)) || ((decimals == 2) && (value >= 9.995)))
   {
      decimals--;
   }

   // This rounding step could be ommitted in case correct rounding is not needed.
   double scale = (decimals == 0) ? 1.0 : (decimals == 1) ? 10.0 : 100.0;
   value = round(value * scale) / scale;

   return format("{:.{}f} {}", value, decimals, units[unit]);
}

bool OpenUriInDefaultApplication(const std::string &uri)
{
   const gchar *user = g_getenv("SUDO_USER");
   if ((user == nullptr) || (*user == 0))
   {  // Not running as sudo.
      return g_app_info_launch_default_for_uri(uri.c_str(), nullptr, nullptr);
   }

   // Running as sudo: Spawn as user (browsers shouldn't/can't be executed as sudo).

   gchar *argv[] =
   {
      const_cast<gchar *>("runuser"),
      const_cast<gchar *>("-u"),
      const_cast<gchar *>(user),
      const_cast<gchar *>("--"),
      const_cast<gchar *>("gio"),
      const_cast<gchar *>("open"),
      const_cast<gchar *>(uri.c_str()),
      nullptr
   };

   return g_spawn_async(nullptr, argv, nullptr, G_SPAWN_SEARCH_PATH, nullptr, nullptr, nullptr, nullptr);
}

bool is_binary_in_path(const char *program)
{
   gchar **path = g_strsplit(getenv("PATH"), ":", 0);

   for (int i = 0; path[i] != NULL; i++) {
      char *s = g_strdup_printf("%s/%s", path[i], program);
      if (FileExists(s)) {
         g_free(s);
         g_strfreev(path);
         return true;
      }
      g_free(s);
   }
   g_strfreev(path);
   return false;
}

char *gtk_get_string_from_color(GdkRGBA *colp)
{
   static char *_str = NULL;

   g_free(_str);
   if (colp == NULL) {
      _str = g_strdup("");
      return _str;
   }
   _str = gdk_rgba_to_string(colp);

   return _str;
}

void gtk_get_color_from_string(const char *cpp, GdkRGBA **colp)
{
   GdkRGBA *new_color;

   // "" means no color
   if (cpp == NULL || strlen(cpp) == 0) {
      *colp = NULL;
      return;
   }

   new_color = g_new(GdkRGBA, 1);
   if (!gdk_rgba_parse(new_color, cpp)) {
      g_free(new_color);
      *colp = NULL;
      return;
   }
   *colp = new_color;
}

const char *utf8_to_locale(const char *str)
{
   static char *_str = NULL;
   if (str == NULL)
      return NULL;
   if (g_utf8_validate(str, -1, NULL) == false)
      return NULL;
   g_free(_str);
   _str = NULL;
   _str = g_locale_from_utf8(str, -1, NULL, NULL, NULL);
   return _str;
}

const char *utf8(const char *str)
{
   static char *_str = NULL;
   if (str == NULL) return NULL;
   if (g_utf8_validate(str, -1, NULL)) return str;
   g_free(_str);
   _str = NULL;
   _str = g_locale_to_utf8(str, -1, NULL, NULL, NULL);
   return _str;
}

std::string utf8(const std::string& str)
{
   if (g_utf8_validate(str.c_str(), -1, NULL))
   {  // Already a valid UFT8 string.
      return str;
   }

   std::unique_ptr<gchar, decltype(&g_free)> utf(
      g_locale_to_utf8(str.c_str(), -1, NULL, NULL, NULL),
      &g_free
   );

   if (utf == nullptr)
   {  // The locale_to_utf8 conversion failed.
      return "";
   }

   return std::string(utf.get());
}

/*
 * MarkupEscapeString: Escape markup from a string
 * ----------------------------------------------------
 * @unescaped: string to escape markup from
 *
 * we use g_markup_escape_text which only support 5 standard entities:
 * &amp; &lt; &gt; &quot; &apos;
 * We must escape the string used as list item of a GtkTree
 */
std::string MarkupEscapeString(std::string unescaped)
{
   std::string escaped;
   char *c_esc = g_markup_escape_text(unescaped.c_str(), -1);
   escaped = std::string(c_esc);
   g_free(c_esc);
   return escaped;
}

/*
 * MarkupUnescapeString: Unescape markup from a string
 * ----------------------------------------------------
 * @escaped: string to unescape markup from
 * unescaped entities: &amp; &lt; &gt; &quot; &apos;
 *
 * sadly there is no simple way to unescape a previously escaped string with
 * g_markup_escape_text
 */
std::string MarkupUnescapeString(std::string escaped)
{
   size_t pos = 0, end = 0;
   std::string entity, str;
   while ((pos = escaped.find("&", pos)) != std::string::npos) {
      end = escaped.find(";", pos);
      if (end == std::string::npos)
         break;

      entity = escaped.substr(pos, end - pos + 1);
      str = "";
      if (entity == "&lt;")
         str = "<";
      else if (entity == "&gt;")
         str = ">";
      else if (entity == "&amp;")
         str = "&";
      else if (entity == "&quot;")
         str = "\"";
      else if (entity == "&apos;")
         str = "'";

      if (!str.empty()) {
         escaped.replace(pos, entity.size(), str);
         pos++;
      } else {
         pos = end;
      }
   }
   return escaped;
}
