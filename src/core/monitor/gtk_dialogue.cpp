#include "RF/definitions.hpp"
#if defined (__LINUX__) || defined (__BSD_KERNEL__)
#include "RF/system.hpp"
#include "RF/monitor.hpp"
#include "RF/library.hpp"

#include <gtk/gtk.h>

void RF::monitor_m::crash_dialogue_()
{
	typedef GtkWidget *(*gtk_message_dialog_new_TYPE)(GtkWindow *, GtkDialogFlags, GtkMessageType, GtkButtonsType, const gchar *, ...);
	
	auto gtk_lib_opt = RF::sys::find_core_library("gtk-3");

	if (!gtk.has_value())
	{
		return; // no GTK graphical interface installed
	}

	auto gtk_lib = RF::library_m::load_library(gtk_lib_opt.value());
	
	auto g_type_check_instance_cast_FUNC = gtk_lib->get_function<GTypeInstance *(GTypeInstance *, GType)>("g_type_check_instance_cast");
	
	auto gtk_init_FUNC = gtk_lib->get_function<void (int *, char ***)>("gtk_init");
	auto gtk_message_dialog_new_FUNC = gtk_lib->get_function_raw<gtk_message_dialog_new_TYPE>("gtk_message_dialog_new");
	auto gtk_window_set_title_FUNC = gtk_lib->get_function<void (GtkWindow *, const gchar *)>("gtk_window_set_title");
	auto gtk_dialog_run_FUNC = gtk_lib->get_function<gint (GtkDialog *)>("gtk_dialog_run");
	auto gtk_widget_destroy_FUNC = gtk_lib->get_function<void (GtkWidget *)>("gtk_widget_destroy");
	auto gtk_window_get_type_FUNC = gtk_lib->get_function<GType ()>("gtk_window_get_type");
	auto gtk_dialog_get_type_FUNC = gtk_lib->get_function<GType ()>("gtk_dialog_get_type");

	gtk_init_FUNC(nullptr, nullptr);

	GtkWidget *dialogue = gtk_message_dialog_new_FUNC(
		NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL,
		"Oh no! A crash has occurred. We are sorry for the inconvenience, for more information, check the crash log."
	);

	gtk_window_set_title_FUNC((GtkWindow *)(void *)g_type_check_instance_cast_FUNC((GTypeInstance *)dialogue, gtk_window_get_type_FUNC()), "RedFox Engine: Process Crashed");
	int response = gtk_dialog_run_FUNC((GtkDialog *)(void *)g_type_check_instance_cast_FUNC((GTypeInstance *)dialogue, gtk_dialog_get_type_FUNC()));

	if (response == GTK_RESPONSE_OK)
	{
		// open file
		RF::monitor_m::open_save_path_();
	}

	gtk_widget_destroy_FUNC(dialogue);
}

void RF::monitor_m::exception_dialogue_(std::string_view title, std::string_view description)
{
	typedef GtkWidget *(*gtk_message_dialog_new_TYPE)(GtkWindow *, GtkDialogFlags, GtkMessageType, GtkButtonsType, const gchar *, ...);
	
	auto gtk_lib_opt = RF::sys::find_core_library("gtk-3");

	if (!gtk.has_value())
	{
		return; // no GTK graphical interface installed
	}

	auto gtk_lib = RF::library_m::load_library("/usr/lib/x86_64-linux-gnu/libgtk-3.so");
	
	auto g_type_check_instance_cast_FUNC = gtk_lib->get_function<GTypeInstance *(GTypeInstance *, GType)>("g_type_check_instance_cast");
	
	auto gtk_init_FUNC = gtk_lib->get_function<void (int *, char ***)>("gtk_init");
	auto gtk_message_dialog_new_FUNC = gtk_lib->get_function_raw<gtk_message_dialog_new_TYPE>("gtk_message_dialog_new");
	auto gtk_window_set_title_FUNC = gtk_lib->get_function<void (GtkWindow *, const gchar *)>("gtk_window_set_title");
	auto gtk_dialog_run_FUNC = gtk_lib->get_function<gint (GtkDialog *)>("gtk_dialog_run");
	auto gtk_widget_destroy_FUNC = gtk_lib->get_function<void (GtkWidget *)>("gtk_widget_destroy");
	auto gtk_window_get_type_FUNC = gtk_lib->get_function<GType ()>("gtk_window_get_type");
	auto gtk_dialog_get_type_FUNC = gtk_lib->get_function<GType ()>("gtk_dialog_get_type");

	gtk_init_FUNC(nullptr, nullptr);

	GtkWidget *dialogue = gtk_message_dialog_new_FUNC(
		NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
		description.data()
	);

	gtk_window_set_title_FUNC((GtkWindow *)(void *)g_type_check_instance_cast_FUNC((GTypeInstance *)dialogue, gtk_window_get_type_FUNC()), title.data());
	int response = gtk_dialog_run_FUNC((GtkDialog *)(void *)g_type_check_instance_cast_FUNC((GTypeInstance *)dialogue, gtk_dialog_get_type_FUNC()));

	return;
}
#endif // __LINUX__, __BSD_KERNEL__