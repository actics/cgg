#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

/*
 * Задание 1.
 * Построить график функции sin(ax+b)/(cx+d)
 */

#define BAD_PARAMETRS_ERROR_MESSAGE "<b>Введены неверные параметры</b>\n\
Поля параметров должны быть не пустыми, и должны сдержать  корректные числа"

#define EMPTY_ZONE_WIDTH 5

enum {
    LEFT_BOUND,
    RIGHT_BOUND,
    PARAMETR_A,
    PARAMETR_B,
    PARAMETR_C,
    PARAMETR_D
};

typedef struct {
    GtkWidget *drawing_area;
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

// Проблемы! Глобальная переменная. Однако, как элегантно избавиться от неё потнятия не имею.
static float parametr[6];

static float f(float x) {
    return (parametr[PARAMETR_A]*x*x + parametr[PARAMETR_B]*x + parametr[PARAMETR_C]);
}

static int* draw_graph(int screen_width, int screen_height) {
    int   *ret_points;
    float *points;
    ret_points = (int*)   malloc(screen_width*sizeof(int));
    points     = (float*) malloc(screen_width*sizeof(float));
    
    float min_y = FLT_MAX;
    float max_y = FLT_MIN;
    float expansion_x = (parametr[RIGHT_BOUND] - parametr[LEFT_BOUND]) / screen_width;
    
    int i;
    float x=parametr[LEFT_BOUND];
    for (i=0; i<screen_width; i++, x+=expansion_x) {
        points[i] = f(x);
        if (points[i] > max_y) max_y = points[i];
        if (points[i] < min_y) min_y = points[i];
    }
    
    float expansion_y = screen_height / (min_y - max_y);
    for (i=0; i<screen_width; i++)
        ret_points[i] = (int) ((points[i] - max_y)*expansion_y);

    free(points);
    return ret_points;
}

static void plot_graph (GtkWidget *plot_button, ParametrEntry *entrys) {
    int i;
    for (i=0; i<6; ++i) {
        char *parametr_str;
        parametr_str = (char*) gtk_entry_get_text(GTK_ENTRY(entrys->parametr[i]));
        parametr[i] = atof(parametr_str);
        if (parametr[i] == 0 && strcmp(parametr_str, "0") != 0) {
            show_msg_dial(gtk_widget_get_tooltip_window(GTK_WIDGET(entrys->parametr[0])));
            return;
        }
    }
    
    int screen_width, screen_height;
    screen_width  = gtk_widget_get_allocated_width(GTK_WIDGET(entrys->drawing_area));
    screen_height = gtk_widget_get_allocated_height(GTK_WIDGET(entrys->drawing_area));
    
    cairo_t *cr;
    cr = gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(entrys->drawing_area)));
    int *points = draw_graph(screen_width, screen_height);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr, 0, points[0]);
    for (i=1; i<screen_width; i++)
        cairo_line_to(cr, i, (int) points[i]);
    
    cairo_stroke(cr);
    free(points);
}

static GtkWidget* parametrs_widget_new(GtkWidget *drawing_area) {
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
    entrys->drawing_area = drawing_area;
    
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
    
    char *m[6] = {"-100", "100", "1", "0", "+50", "0"};
    int i;
    for (i=0; i<6; ++i) {
        gtk_entry_set_text(GTK_ENTRY(entrys->parametr[i]), m[i]);
    }
    
    g_signal_connect(plot_button, "clicked", G_CALLBACK(plot_graph), entrys);
    
    return vbox;
}

GtkWidget* first_task_new() {
    GtkWidget *frame;
    GtkWidget *hbox;
    GtkWidget *parametrs;
    GtkWidget *separator;
    GtkWidget *drawing_area;
    
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(GTK_WIDGET(drawing_area), 100, 100);
    parametrs = parametrs_widget_new(drawing_area);
    separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    
    gtk_box_pack_start(GTK_BOX(hbox), parametrs,    FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), separator,    FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), drawing_area, TRUE,  TRUE, 0);
    
    frame = gtk_frame_new("Задание 1 - График функции");
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    
    return frame;   
}
