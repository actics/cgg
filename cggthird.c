#include "cggsecond.h"

#define FRAME_TITLE  "Построение линии. Алгоритм Брезенхема."
#define BAD_PARAMETRS_ERROR_MESSAGE "<b>Введены неверные координаты</b> \n \
Поля координат должны быть не пустыми, и должны сдержать корректные числа"

#define DRAW_BUTTON_IMAGE  "./images/draw_button.png"

#define EMPTY_ZONE_WIDTH 5
#define AXES_UNIT_PIXELS 30

enum {
    COORDINATE_X,
    COORDINATE_Y,
    COORDINATE_P
};

static GtkWidget *drawing_area;
static GtkWidget *entry[4]; 
static int coordinate[4];

static int parabola(int x, int y, int p) {
    return abs( y*y - 2*p*x );
}

static void draw_point(cairo_t *cr, int x, int y) {
    cairo_rectangle(cr, x, y, 1.0, 1.0);
    cairo_fill(cr);
    cairo_stroke(cr);
}

static int in_screen(int x, int screen_width) {
    if (x >= 0 && x < screen_width) {
        return 1;
    }
    else {
        return 0;
    }
}

static void draw_parabola(cairo_t *cr, int x0, int y0, int p, int screen_width) {
    int x = 0;
    int y = 0;
    int target_x = x0;
    int direction = ( p>0 ? 1 : -1 );
    int distance_horizontal;
    int distance_vertical;
    int distance_diagonal;

    p = abs(p);

    while ( in_screen(target_x, screen_width) ) {
        draw_point(cr, target_x, y0 - y);
        draw_point(cr, target_x, y0 + y);

        distance_diagonal   = parabola( x + 1, y + 1, p);
        distance_horizontal = parabola( x + 1, y,     p);
        distance_vertical   = parabola( x,     y + 1, p);

        if ( distance_horizontal < distance_vertical ) {
            if ( distance_diagonal < distance_horizontal ) {
                y++;
            }
            x++;
        }
        else {
            if ( distance_diagonal < distance_vertical ) {
                x++;
            }
            y++;
        }
        target_x = x0 + direction*x;
    }
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer p) {
    int screen_width;

    screen_width  = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area))+1;
    
    cairo_set_source_rgb(cr, 1, 0, 0);

    draw_parabola(cr, coordinate[COORDINATE_X], coordinate[COORDINATE_Y], coordinate[COORDINATE_P],
                      screen_width);

    return FALSE;
}

static void show_msg_dial(GtkWidget *parent_window) {
    GtkWidget *msg_dial;
    msg_dial = gtk_message_dialog_new_with_markup(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        BAD_PARAMETRS_ERROR_MESSAGE
    );
    gtk_window_set_title(GTK_WINDOW(msg_dial), "Ошибка");
    gtk_dialog_run(GTK_DIALOG(msg_dial));
    gtk_widget_destroy(msg_dial);
}

static void draw_button_clicked(GtkWidget *draw_button, gpointer p) {
    float new_coordinate[3];
    
    int i;
    for (i=0; i<3; ++i) {
        char *parametr_str;
        parametr_str = (char*) gtk_entry_get_text(GTK_ENTRY(entry[i]));
        new_coordinate[i] = atoi(parametr_str);
        if (new_coordinate[i] == 0 && strcmp(parametr_str, "0") != 0) {
            show_msg_dial(gtk_widget_get_toplevel(GTK_WIDGET(entry[COORDINATE_X])));
            return;
        }
    }
    
    for (i=0; i<3; ++i)
        coordinate[i] = new_coordinate[i];
    
    gtk_widget_queue_draw(drawing_area);
}

static GtkWidget* parametrs_widget_new() {
    GtkWidget *vbox;
    GtkWidget *coordinate_label;
    GtkWidget *coordinate_box;
    GtkWidget *draw_button_image;
    GtkWidget *draw_button;
    
    /* Создание виджетов окна параметров */
    vbox                 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    coordinate_label     = gtk_label_new("Введите координаты");
    coordinate_box       = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    entry[COORDINATE_X]  = gtk_entry_new();
    entry[COORDINATE_Y]  = gtk_entry_new();
    entry[COORDINATE_P]  = gtk_entry_new();
    draw_button_image    = gtk_image_new_from_file(DRAW_BUTTON_IMAGE);
    draw_button          = gtk_button_new_with_label("Отрисовать");
    
    /* Настройка виджетов окна параметров */
    gtk_box_set_spacing(GTK_BOX(coordinate_box), 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_X]),  "x");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_Y]),  "y");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_P]),  "p");
    gtk_button_set_image(GTK_BUTTON(draw_button), draw_button_image);
    
    /* Упаковка окна параметров */
    gtk_box_pack_start(GTK_BOX(coordinate_box), entry[COORDINATE_X], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(coordinate_box), entry[COORDINATE_Y], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(coordinate_box), entry[COORDINATE_P], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), coordinate_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), coordinate_box,   FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), draw_button,      FALSE, TRUE, 0);
    
    g_signal_connect(draw_button,  "released", G_CALLBACK(draw_button_clicked), NULL);
    g_signal_connect(drawing_area, "draw",     G_CALLBACK(draw_callback),       NULL);
    
    return vbox;
}

GtkWidget* third_task_new() {
    GtkWidget *frame;
    GtkWidget *hbox;
    GtkWidget *parametrs;
    GtkWidget *separator;
    
    hbox         = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    drawing_area = gtk_drawing_area_new();
    parametrs    = parametrs_widget_new();
    separator    = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
    
    gtk_box_pack_start(GTK_BOX(hbox), parametrs,    FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), separator,    FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), drawing_area, TRUE,  TRUE, 0);
    
    frame = gtk_frame_new(FRAME_TITLE);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    
    return frame;   
}

