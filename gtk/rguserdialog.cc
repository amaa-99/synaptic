/* rguserdialog.cc
 *
 * Copyright (c) 2000, 2001 Conectiva S/A
 *               2003 Michael Vogt
 *
 * Author: Alfredo K. Kojima <kojima@conectiva.com.br>
 *         Michael Vogt <mvo@debian.org>
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

#include "i18n.h"
#include "rguserdialog.h"
#include "rgutils.h"
#include "rgwindow.h"
#include "ruserdialog.h"

#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>
#include <cassert>
#include <cstddef>
#include <gtk/gtk.h>
#include <string>

using namespace std;

static void actionResponse(GtkDialog *dialog, gint id, gpointer user_data)
{
   GtkResponseType *res = (GtkResponseType *)user_data;
   *res = (GtkResponseType)id;
}

static const string_view Trim(const string_view s)
{
   constexpr string_view space = " \t\n\r\f\v";

   size_t first = s.find_first_not_of(space);
   if (first == string_view::npos) {
      return {};
   }

   size_t last = s.find_last_not_of(space);
   return s.substr(first, last - first + 1);
}

static void AddDetailPanel(GtkBox *list, bool is_error, const string &title)
{
   GtkBuilder *panel_builder = gtk_builder_new_from_resource("/io/github/mvo5/synaptic/ui/detail_panel_simple.ui");
   if (panel_builder == nullptr)
   {
      g_error("Couldn't find the detail_panel resource!\n");
   }

   GtkWidget *panel = GTK_WIDGET(gtk_builder_get_object(panel_builder, "detail_panel"));
   if (panel == nullptr)
   {
      g_error("Couldn't find the detail_panel object!\n");
   }

   if (GtkImage *icon = GTK_IMAGE(gtk_builder_get_object(panel_builder, "icon")); icon != nullptr)
   {
      gtk_image_set_from_icon_name(icon, (is_error) ? "dialog-error" : "dialog-warning", GTK_ICON_SIZE_SMALL_TOOLBAR);
   }
   else
   {
      g_error("Couldn't find the detail_panel::icon object!\n");
   }

   if (GtkLabel *label = GTK_LABEL(gtk_builder_get_object(panel_builder, "title")); label != nullptr)
   {
      gtk_label_set_text(label, title.c_str());
   }
   else
   {
      g_error("Couldn't find the detail_panel::title object!\n");
   }

   gtk_box_pack_start(list, panel, FALSE, FALSE, 0);
   g_object_unref(panel_builder);   // The panel is now owned by the parent box (list).
}

static void AddDetailPanel(GtkBox *list, bool is_error, const string &title, const string &description)
{
   GtkBuilder *panel_builder = gtk_builder_new_from_resource("/io/github/mvo5/synaptic/ui/detail_panel.ui");
   if (panel_builder == nullptr)
   {
      g_error("Couldn't find the detail_panel resource!\n");
   }

   GtkWidget *panel = GTK_WIDGET(gtk_builder_get_object(panel_builder, "detail_panel"));
   if (panel == nullptr)
   {
      g_error("Couldn't find the detail_panel object!\n");
   }

   if (GtkImage *icon = GTK_IMAGE(gtk_builder_get_object(panel_builder, "icon")); icon != nullptr)
   {
      gtk_image_set_from_icon_name(icon, (is_error) ? "dialog-error" : "dialog-warning", GTK_ICON_SIZE_SMALL_TOOLBAR);
   }
   else
   {
      g_error("Couldn't find the detail_panel::icon object!\n");
   }

   if (GtkLabel *label = GTK_LABEL(gtk_builder_get_object(panel_builder, "title")); label != nullptr)
   {
      gtk_label_set_text(label, title.c_str());
   }
   else
   {
      g_error("Couldn't find the detail_panel::title object!\n");
   }

   if (GtkLabel *label = GTK_LABEL(gtk_builder_get_object(panel_builder, "description")); label != nullptr)
   {
      gtk_label_set_text(label, description.c_str());
   }
   else
   {
      g_error("Couldn't find the detail_panel::description object!\n");
   }

   gtk_box_pack_start(list, panel, FALSE, FALSE, 0);
   g_object_unref(panel_builder);   // The panel is now owned by the parent box (list).
}

static void ShowErrors(GtkWindow *parent, bool is_error, const string &title, const string &subtitle, const vector<tuple<bool, string, string>> &details)
{
   GtkBuilder *builder = gtk_builder_new_from_resource("/io/github/mvo5/synaptic/ui/dialog_error.ui");
   GtkWidget *dialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_error"));

   gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
   gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

   if (GtkLabel *label = GTK_LABEL(gtk_builder_get_object(builder, "title_label")); label != nullptr)
   {
      gtk_label_set_text(label, title.c_str());
   }
   else
   {
      g_error("Couldn't find the title_label resource!\n");
   }

   if (GtkLabel *label = GTK_LABEL(gtk_builder_get_object(builder, "subtitle_label")); label != nullptr)
   {
      gtk_label_set_text(label, subtitle.c_str());
   }
   else
   {
      g_error("Couldn't find the subtitle_label object!\n");
   }

   if (GtkImage *image = GTK_IMAGE(gtk_builder_get_object(builder, "title_icon")); image != nullptr)
   {
      gtk_image_set_from_icon_name(image, (is_error) ? "dialog-error" : "dialog-warning", GTK_ICON_SIZE_DIALOG);
   }
   else
   {
      g_error("Couldn't find the title_icon object!\n");
   }

   GtkBox *list = GTK_BOX(gtk_builder_get_object(builder, "details_box"));
   if (list == nullptr)
   {
      g_error("Couldn't find the details_box object!\n");
   }

   for (const auto& [detail_is_error, detail_title, detail_description] : details)
   {
      if (!detail_description.empty())
      {
         AddDetailPanel(list, detail_is_error, detail_title, detail_description);
      }
      else
      {
         AddDetailPanel(list, detail_is_error, detail_title);
      }
   }

   gtk_widget_show_all(dialog);

   gint response = gtk_dialog_run(GTK_DIALOG(dialog));

   if (response == GTK_RESPONSE_ACCEPT)
   {
      cout << "Accepted\n";
   }

   gtk_widget_destroy(dialog);
   g_object_unref(builder);
}

void RGUserDialog::showErrors(bool has_errors, const std::string &title, const std::string &subtitle, std::vector<std::tuple<bool, std::string, std::string>> details)
{
   ShowErrors(GTK_WINDOW(_parentWindow), has_errors, title, subtitle, details);
}

bool RGUserDialog::showErrors()
{
   if (_error->empty())
   {
      g_warning("showErrors() called with an empty log!");
      return false;
   }

   vector<tuple<bool, string, string>> details;

   bool has_errors = false;

   while (!_error->empty())
   {
      string log_message;
      bool is_error = _error->PopMessage(log_message);

      // Ignore some stupid error messages.
      if (log_message == "Tried to dequeue a fetching object")
      {
         continue;
      }

      size_t newline = log_message.find('\n');
      string title = log_message.substr(0, newline);
      string description = (newline == std::string_view::npos) ? "" : log_message.substr(newline + 1);
      description = Trim(description);

      details.push_back(tuple<bool, string, string>(is_error, title, description));

      has_errors |= is_error;
   }

   string title = _("An error occurred");
   string subtitle = _("The following details are provided:");

   ShowErrors(GTK_WINDOW(_parentWindow), has_errors, title, subtitle, details);

   return true;
}

bool RGUserDialog::message(const char *msg, RUserDialog::DialogType dialog, RUserDialog::ButtonsType buttons, bool defaultResponse)
{
   GtkMessageType gtkmessage;
   GtkButtonsType gtkbuttons;

   switch (dialog) {
      case RUserDialog::DialogInfo: {
         gtkmessage = GTK_MESSAGE_INFO;
         gtkbuttons = GTK_BUTTONS_CLOSE;
         break;
      }
      case RUserDialog::DialogWarning: {
         gtkmessage = GTK_MESSAGE_WARNING;
         gtkbuttons = GTK_BUTTONS_CLOSE;
         break;
      }
      case RUserDialog::DialogError: {
         gtkmessage = GTK_MESSAGE_ERROR;
         gtkbuttons = GTK_BUTTONS_CLOSE;
         break;
      }
      case RUserDialog::DialogQuestion: {
         gtkmessage = GTK_MESSAGE_QUESTION;
         gtkbuttons = GTK_BUTTONS_YES_NO;
         break;
      }
      default: {
         g_error("Unexpected DialogType %u in RGUserDialog::message!\n", dialog);   // Exit with error.
      }
   }

   switch (buttons) {
      case RUserDialog::ButtonsDefault: {
         break;
      }
      case RUserDialog::ButtonsOk: {
         gtkbuttons = GTK_BUTTONS_CLOSE;
         break;
      }
      case RUserDialog::ButtonsOkCancel: {
         gtkbuttons = GTK_BUTTONS_OK_CANCEL;
         break;
      }
      case RUserDialog::ButtonsYesNo: {
         gtkbuttons = GTK_BUTTONS_YES_NO;
         break;
      }
      default: {
         g_error("Unexpected ButtonsType %u in RGUserDialog::message!\n", dialog);   // Exit with error.
      }
   }

   GtkWidget *dia = gtk_message_dialog_new(GTK_WINDOW(_parentWindow), GTK_DIALOG_DESTROY_WITH_PARENT, gtkmessage, gtkbuttons, NULL);
   gtk_window_set_icon_name(GTK_WINDOW(dia), "synaptic");

   gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dia), utf8(msg));
   gtk_container_set_border_width(GTK_CONTAINER(dia), 6);

   switch (buttons) {
      case RUserDialog::ButtonsOkCancel: {
         gtk_dialog_set_default_response(GTK_DIALOG(dia), defaultResponse ? GTK_RESPONSE_OK : GTK_RESPONSE_CANCEL);
         break;
      }
      case RUserDialog::ButtonsYesNo: {
         gtk_dialog_set_default_response(GTK_DIALOG(dia), defaultResponse ? GTK_RESPONSE_YES : GTK_RESPONSE_NO);
         break;
      }
      default: {
         break;
      }
   }

   GtkResponseType res;
   g_signal_connect(G_OBJECT(dia), "response", G_CALLBACK(actionResponse), (gpointer) &res);

   gtk_dialog_run(GTK_DIALOG(dia));
   gtk_widget_destroy(dia);

   return (res == GTK_RESPONSE_OK) || (res == GTK_RESPONSE_YES) || (res == GTK_RESPONSE_CLOSE);
}

// RGGtkBuilderUserDialog

RGGtkBuilderUserDialog::RGGtkBuilderUserDialog(RGWindow *parent, const char *name) : RGUserDialog(parent->window())
{
   std::string resource_path = std::string("/io/github/mvo5/synaptic/ui/dialog_") + name + ".ui";
   _builder = gtk_builder_new_from_resource(resource_path.c_str());

   // Note: If an error occurs while locating the resource or parsing the description
   // then gtk_builder_new_from_resource() aborts the program.

   std::string main_widget_name = std::string("dialog_") + name;
   _dialog = GTK_WIDGET(gtk_builder_get_object(_builder, main_widget_name.c_str()));
   assert(_dialog != nullptr);

   gtk_window_set_icon_name(GTK_WINDOW(_dialog), "synaptic");
   gtk_window_set_position(GTK_WINDOW(_dialog), GTK_WIN_POS_CENTER_ON_PARENT);

   if (gtk_window_get_modal(GTK_WINDOW(_dialog))) {
      gtk_window_set_skip_taskbar_hint(GTK_WINDOW(_dialog), TRUE);
   }

   gtk_window_set_transient_for(GTK_WINDOW(_dialog), GTK_WINDOW(_parentWindow));
}

int RGGtkBuilderUserDialog::run()
{
   int res = gtk_dialog_run(GTK_DIALOG(_dialog));
   gtk_widget_hide(_dialog);
   return res;
}
