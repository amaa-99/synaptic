/* rgsummarywindow.cc
 *
 * Copyright (c) 2000, 2001 Conectiva S/A
 *               2002 Michael Vogt <mvo@debian.org>
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

#include "rgsummarywindow.h"

#include "i18n.h"
#include "rggtkbuilderwindow.h"
#include "rguserdialog.h"
#include "rpackage.h"
#include "rpackagelister.h"

#include <apt-pkg/configuration.h>
#include <apt-pkg/strutl.h>
#include <cassert>
#include <cstdio>
#include <glib-object.h>
#include <glib.h>
#include <gobject/gclosure.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <string>
#include <vector>

class RGWindow;

using namespace std;

void RGSummaryWindow::buildTree(GtkBuilder *builder, RPackageLister *lister, GtkTreeStore *treeStore)
{
   RPackageLister::DetailedSummary summary = lister->getDetailedSummary();

   #ifdef WITH_APT_AUTH

      if (!summary.notAuthenticated.empty())
      {
         /* not authenticated */

         GtkWidget *label = GTK_WIDGET(gtk_builder_get_object(builder, "label_auth_warning"));
         assert(label);

         // FIXME: make this a message from a trust class (and remember to change the text in rgchangeswindow then too)
         gchar *msg = g_strdup_printf
         (
            "<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s",
            _("Warning"),
            _("You are about to install software that <b>can't be authenticated</b>! Doing this could allow a malicious individual to damage or take control of your system.")
         );

         gtk_label_set_markup(GTK_LABEL(label), msg);
         gtk_widget_show(label);
         g_free(msg);

         GtkTreeIter iter;
         gtk_tree_store_append(treeStore, &iter, NULL);
         gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("NOT AUTHENTICATED"), -1);

         for (const string &p : summary.notAuthenticated) {
            GtkTreeIter iter_child;
            gtk_tree_store_append(treeStore, &iter_child, &iter);
            gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p.c_str(), -1);
         }
      }

   #endif

   if (!summary.essential.empty()) {
      /* (Essential) removed */
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("<b>(ESSENTIAL) to be removed</b>"), -1);
      for (const RPackage *p : summary.essential) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toDowngrade.empty()) {
      /* removed */
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("<b>To be DOWNGRADED</b>"), -1);
      for (const RPackage *p : summary.toDowngrade) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toRemove.empty()) {
      /* removed */
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("<b>To be removed</b>"), -1);
      for (const RPackage *p : summary.toRemove) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toPurge.empty()) {
      /* removed */
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("<b>To be completely removed (including configuration files)</b>"), -1);
      for (const RPackage *p : summary.toPurge) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toUpgrade.empty()) {
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("To be upgraded"), -1);
      for (const RPackage *p : summary.toUpgrade) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toInstall.empty()) {
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("To be installed"), -1);
      for (const RPackage *p : summary.toInstall) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.toReInstall.empty()) {
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("To be re-installed"), -1);
      for (const RPackage *p : summary.toReInstall) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }

   if (!summary.held.empty()) {
      GtkTreeIter iter;
      gtk_tree_store_append(treeStore, &iter, NULL);
      gtk_tree_store_set(treeStore, &iter, PKG_COLUMN, _("Unchanged"), -1);
      for (const RPackage *p : summary.held) {
         GtkTreeIter iter_child;
         gtk_tree_store_append(treeStore, &iter_child, &iter);
         gtk_tree_store_set(treeStore, &iter_child, PKG_COLUMN, p->name(), -1);
      }
   }
}

void RGSummaryWindow::buildLabel(GtkBuilder *builder, RPackageLister *lister)
{
   GtkWidget *info = GTK_WIDGET(gtk_builder_get_object(builder, "label_details"));

   RPackageLister::DetailedSummary summary = lister->getDetailedSummary();
   string text;

   for (const RPackage *p : summary.essential) {
      gchar *str = g_strdup_printf(_("<b>%s</b> (<b>essential</b>) will be removed\n"), p->name());
      text += str;
      g_free(str);
   }

   for (const RPackage *p : summary.toDowngrade) {
      gchar *str = g_strdup_printf(_("<b>%s</b> will be <b>downgraded</b>\n"), p->name());
      text += str;
      g_free(str);
   }

   for (const RPackage *p: summary.toPurge) {
      gchar *str = g_strdup_printf(_("<b>%s</b> will be removed with configuration\n"), p->name());
      text += str;
      g_free(str);
   }

   for (const RPackage *p: summary.toRemove) {
      gchar *str = g_strdup_printf(_("<b>%s</b> will be removed\n"), p->name());
      text += str;
      g_free(str);
   }

   for (const RPackage *p: summary.toUpgrade) {
      gchar *str = g_strdup_printf(_("<b>%s</b> (version <i>%s</i>) will be upgraded to version <i>%s</i>\n"), p->name(), p->installedVersion(), p->availableVersion());
      text += str;
      g_free(str);
   }

   for (const RPackage *p: summary.toInstall) {
      gchar *str = g_strdup_printf(_("<b>%s</b> (version <i>%s</i>) will be installed\n"), p->name(), p->availableVersion());
      text += str;
      g_free(str);
   }

   for (const RPackage *p: summary.toReInstall) {
      gchar *str = g_strdup_printf(_("<b>%s</b> (version <i>%s</i>) will be re-installed\n"), p->name(), p->installedVersion());
      text += str;
      g_free(str);
   }

   gtk_label_set_markup(GTK_LABEL(info), text.c_str());
}

void RGSummaryWindow::clickedDetails(GtkWidget *self, void *data)
{
   RGSummaryWindow *me = (RGSummaryWindow *)data;
   GtkWidget *s, *d;

   s = GTK_WIDGET(
      gtk_builder_get_object(me->_builder, "scrolledwindow_summary"));
   d = GTK_WIDGET(
      gtk_builder_get_object(me->_builder, "scrolledwindow_details"));

   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self))) {
      gtk_widget_hide(s);
      gtk_widget_show(d);
      gtk_button_set_label(GTK_BUTTON(self), _("_Hide Details"));
   } else {
      gtk_widget_show(s);
      gtk_widget_hide(d);
      gtk_button_set_label(GTK_BUTTON(self), _("_Show Details"));
   }
}

RGSummaryWindow::RGSummaryWindow(RGWindow *wwin, RPackageLister *lister)
   : RGGtkBuilderWindow(wwin, "summary"),
     _dlonlyB(GTK_WIDGET(gtk_builder_get_object(_builder, "checkbutton_download_only")))
{
   assert(_dlonlyB != nullptr);

   _potentialBreak = false;

   setTitle(_("Summary"));
   // gtk_window_set_default_size(GTK_WINDOW(window()), 400, 250);

   // details label
   buildLabel(_builder, lister);

   // new tree store
   GtkTreeStore *_treeStore = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING);
   buildTree(_builder, lister, _treeStore);
   GtkWidget *_tree = GTK_WIDGET(gtk_builder_get_object(_builder, "treeview_summary"));
   assert(_tree);

   GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
   // GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Summary", renderer, PKG_COLUMN, "pkg", NULL);
   GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Marked Changes"), renderer, "markup", PKG_COLUMN, NULL);
   /* Add the column to the view. */
   gtk_tree_view_append_column(GTK_TREE_VIEW(_tree), column);
   gtk_widget_show(_tree);

   gtk_tree_view_set_model(GTK_TREE_VIEW(_tree), GTK_TREE_MODEL(_treeStore));
   g_object_unref(_treeStore); // Unreference the list store as it is now managed by the treeview.

   GtkWidget *_checkSigsB = GTK_WIDGET(gtk_builder_get_object(_builder, "checkbutton_check_signatures"));
   assert(_checkSigsB);

   GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(_builder, "togglebutton_details"));
   g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(clickedDetails), this);

   RPackageLister::Summary summary = lister->getSummary();

   uint64_t dlSize = lister->getDownloadSize();

   GString *msg = g_string_new("");

#if 0
   if (summary.held != 0) {
      char *str = ngettext("%d package is locked\n",
                           "%d packages are locked\n",
                           summary.held);
      g_string_append_printf(msg, str, summary.held);
   }
#endif

   if (summary.held != 0) {
      char *str = ngettext("%d package will be held back and not upgraded\n",
                           "%d packages will be held back and not upgraded\n",
                           summary.held);
      g_string_append_printf(msg, str, summary.held);
   }

   if (summary.toInstall != 0) {
      char *str = ngettext("%d new package will be installed\n",
                           "%d new packages will be installed\n",
                           summary.toInstall);
      g_string_append_printf(msg, str, summary.toInstall);
   }

   if (summary.toReInstall != 0) {
      char *str = ngettext("%d new package will be re-installed\n",
                           "%d new packages will be re-installed\n",
                           summary.toReInstall);
      g_string_append_printf(msg, str, summary.toReInstall);
   }

   if (summary.toUpgrade != 0) {
      char *str = ngettext("%d package will be upgraded\n",
                           "%d packages will be upgraded\n",
                           summary.toUpgrade);
      g_string_append_printf(msg, str, summary.toUpgrade);
   }

   if (summary.toRemove != 0) {
      char *str = ngettext("%d package will be removed\n",
                           "%d packages will be removed\n",
                           summary.toRemove);
      g_string_append_printf(msg, str, summary.toRemove);
   }

   if (summary.toDowngrade != 0) {
      char *str = ngettext("%d package will be <b>downgraded</b>\n",
                           "%d packages will be <b>downgraded</b>\n",
                           summary.toDowngrade);
      g_string_append_printf(msg, str, summary.toDowngrade);
   }

   if (summary.essential != 0) {
      char *str = ngettext("<b>Warning:</b> %d essential package will be removed\n",
                           "<b>Warning:</b> %d essential packages will be removed\n",
                           summary.essential);
      g_string_append_printf(msg, str, summary.essential);
      _potentialBreak = true;
   }

   // remove the trailing newline of msg
   if (msg->str[msg->len - 1] == '\n') {
      msg = g_string_truncate(msg, msg->len - 1);
   }

   GtkWidget *_summaryL = GTK_WIDGET(gtk_builder_get_object(_builder, "label_summary"));
   assert(_summaryL != nullptr);
   gtk_label_set_markup(GTK_LABEL(_summaryL), msg->str);
   g_string_free(msg, TRUE);

   GString *msg_space = g_string_new("");

   /* this stuff goes to the msg_space string */
   if (summary.sizeChange > 0) {
      g_string_append_printf(msg_space, _("%s of extra space will be used"), SizeToStr(summary.sizeChange).c_str());
   } else if (summary.sizeChange < 0) {
      g_string_append_printf(msg_space, _("%s of extra space will be freed"), SizeToStr(summary.sizeChange).c_str());
   }

   g_string_append_printf(msg_space, _("\n%s have to be downloaded"), SizeToStr(dlSize).c_str());

   GtkWidget *_summarySpaceL = GTK_WIDGET(gtk_builder_get_object(_builder, "label_summary_space"));
   assert(_summarySpaceL);
   gtk_label_set_markup(GTK_LABEL(_summarySpaceL), msg_space->str);
   g_string_free(msg_space, TRUE);

   skipTaskbar(!_config->FindB("Volatile::HideMainwindow", false));
}

bool RGSummaryWindow::showAndConfirm()
{
   gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(_dlonlyB),
      _config->FindB("Volatile::Download-Only", false));
   int res = gtk_dialog_run(GTK_DIALOG(window()));
   bool confirmed = (res == GTK_RESPONSE_APPLY);

   RGUserDialog userDialog(this);
   if (confirmed && _potentialBreak &&
       userDialog.warning(_("Essential packages will be removed.\n"
                            "This may render your system unusable!\n"),
                          false) == false)
      return false;

   _config->Set("Volatile::Download-Only",
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_dlonlyB))
                   ? "true"
                   : "false");

   return confirmed;
}
