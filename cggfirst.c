#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

/*
 * Задание 1.
 * Построить график функции sin(ax+b)/(cx+d)
 */

#define BAD_PARAMETRS_ERROR_MESSAGE "<b>Введены неверные параметры</b>\n\
Поля параметров должны быть не пустыми, и должны сдержать  корректные числа"

typedef struct {
    GtkWindow *parent_window;
    /* 0 - левая граница
       1 - правая границв
       2 - параметр a
       3 - параметр b
       4 - параметр c
       5 - параметр d
    */
    GtkWidget *parametr[6];
} ParametrEntry;

static void show_msg_dial(GtkWindow *parent_window) {
    GtkWidget *msg_dial;
    msg_dial = gtk_message_dialog_new_with_markup(
        parent_window,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        BAD_PARAMETRS_ERROR_MESSAGE
    );
    gtk_window_set_title(GTK_WINDOW(msg_dial), "Ошибка");
    gtk_dialog_run(GTK_DIALOG(msg_dial));
    gtk_widget_destroy(msg_dial);
}

static void plot_graph (GtkWidget *plot_button, ParametrEntry *entrys) {
    gchar **parametr_str;
    gfloat   *parametr;
    parametr_str = (gchar**) malloc(sizeof(gchar*) * 6);
    parametr     = (gfloat*)   malloc(sizeof(gfloat)   * 6);
    for (gint i=0; i<6; ++i) {
        parametr_str[i] = (gchar *)gtk_entry_get_text(GTK_ENTRY(entrys->parametr[i]));
        parametr[i] = atof(parametr_str[i]);
        if (parametr[i] == 0 && strcmp(parametr_str[i], "0") != 0) {
            show_msg_dial(entrys->parent_window);
            goto ploth_graph_exit_level;
        }
    }
    
    ploth_graph_exit_level:
    
    free(parametr);
    free(parametr_str);
}

static GtkWidget* parametrs_widget_new(GtkWindow *parent_window) {
    GtkWidget *vbox;
    GtkWidget *interval_label;
    GtkWidget *interval_hbox;
    GtkWidget *parametrs_label;
    GtkWidget *plot_button;
    GtkWidget *separator_0;
    GtkWidget *separator_1;
    GtkWidget *parametrs_grid;
    
    ParametrEntry *entrys;
    entrys = (ParametrEntry*) malloc(sizeof(ParametrEntry));
    entrys->parent_window = parent_window;
    
    /* Создание виджетов окна параметров */
    vbox                = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    interval_label      = gtk_label_new("Выберите интервал");
    interval_hbox       = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    entrys->parametr[0] = gtk_entry_new();
    entrys->parametr[1] = gtk_entry_new();
    separator_0         = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    parametrs_label     = gtk_label_new("Введите параметры");
    parametrs_grid      = gtk_grid_new();
    entrys->parametr[2] = gtk_entry_new();
    entrys->parametr[3] = gtk_entry_new();
    entrys->parametr[4] = gtk_entry_new();
    entrys->parametr[5] = gtk_entry_new();
    separator_1         = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    plot_button         = gtk_button_new_with_label("Отрисовать");
    
    /* Настройка виджетов окна параметров */
    gtk_grid_set_row_spacing(GTK_GRID(parametrs_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(parametrs_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[0]), "Левая граница");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[1]), "Правая граница");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[2]), "A");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[3]), "B");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[4]), "C");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entrys->parametr[5]), "D");
        
    /* Упаковка окна параметров */
    gtk_box_pack_start(GTK_BOX(interval_hbox), entrys->parametr[0], FALSE, TRUE, 0);    
    gtk_box_pack_start(GTK_BOX(interval_hbox), entrys->parametr[1], FALSE, TRUE, 0);
    gtk_grid_attach(GTK_GRID(parametrs_grid), entrys->parametr[2], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid), entrys->parametr[3], 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid), entrys->parametr[4], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid), entrys->parametr[5], 1, 1, 1, 1);
    gtk_box_pack_start(GTK_BOX(vbox), interval_label,  FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), interval_hbox,   FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), separator_0,     FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), parametrs_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), parametrs_grid,  FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), separator_1,     FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), plot_button,     FALSE, TRUE, 0);
    
    g_signal_connect(plot_button, "clicked", G_CALLBACK(plot_graph), entrys);
    
    return vbox;
}

GtkWidget* first_task_new(GtkWindow *parent_window) {
    GtkWidget *frame;
    GtkWidget *hbox;
    GtkWidget *parametrs;
    GtkWidget *separator;
    GtkWidget *drawing;
    
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    parametrs = parametrs_widget_new(parent_window);
    separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    drawing = gtk_drawing_area_new();
    
    gtk_box_pack_start(GTK_BOX(hbox), parametrs, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), separator, FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), drawing,   TRUE, TRUE, 0);
    
    frame = gtk_frame_new("Задание 1 - График функции");
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    
    return frame;   
}
