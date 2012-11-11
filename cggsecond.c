#include "cggsecond.h"

#define FUNCTION_NAME "<b>sin(ax+b)/(cx+d)</b>"
const char* FRAME_TITLE = "Построение линии. Алгоритм Брезенхема.";
#define BAD_PARAMETRS_ERROR_MESSAGE "<b>Введены неверные координаты</b>\n\
Поля координат должны быть не пустыми, и должны сдержать корректные числа"

const char* DRAW_BUTTON_IMAGE = "./images/draw_button.png";

#define EMPTY_ZONE_WIDTH 5
#define AXES_UNIT_PIXELS 30

enum {
    COORDINATE_X1,
    COORDINATE_Y1,
    COORDINATE_X2,
    COORDINATE_Y2
};

static GtkWidget *drawing_area;
static GtkWidget *entry[4]; 
static int coordinate[4];

static void draw_point(cairo_t *cr, int x, int y) {
    cairo_rectangle(cr, x, y, 1.0, 1.0);
    cairo_fill(cr);
    cairo_stroke(cr);
}

static void draw_line(cairo_t *cr, int x, int y, int x1, int y1) {
    int dx, dy;
    int sig_x, sig_y;
    int error;

    if (x == x1) {
        for (; y<=y1; y++)
            draw_point(cr, x, y);
        return;
    }
    
    if (y == y1) {
        for (; x<=x1; x++)
            draw_point(cr, x, y);
        return;
    }

    dx    = abs(x1 - x);
    dy    = -abs(y1 - y);
    sig_x = x < x1 ? 1 : -1;
    sig_y = y < y1 ? 1 : -1;
    error = dx + dy;
    
    while (x != x1 || y != y1) {
        const int error2 = (error << 1);
        draw_point(cr, x, y);
        
        if (error2 > dy) {
            error += dy;
            x += sig_x;
        }
        if (error2 < dx) {
            error += dx;
            y += sig_y;
        }
    }
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer p) {
    draw_line(cr, coordinate[COORDINATE_X1], coordinate[COORDINATE_Y1], 
                  coordinate[COORDINATE_X2], coordinate[COORDINATE_Y2]
    );
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
    float new_coordinate[4];
    
    int i;
    for (i=0; i<4; ++i) {
        char *parametr_str;
        parametr_str = (char*) gtk_entry_get_text(GTK_ENTRY(entry[i]));
        new_coordinate[i] = atoi(parametr_str);
        if (new_coordinate[i] == 0 && strcmp(parametr_str, "0") != 0) {
            show_msg_dial(gtk_widget_get_toplevel(GTK_WIDGET(entry[COORDINATE_X1])));
            return;
        }
    }
    
    for (i=0; i<4; ++i)
        coordinate[i] = new_coordinate[i];
    
    gtk_widget_queue_draw(drawing_area);
}

static void fill_parametrs() {
    const char* init_coordinate[] = {"10", "10", "100", "100"};
    int i;
    
    for (i=0; i<4; ++i) {
        coordinate[i] = atoi(init_coordinate[i]);
        gtk_entry_set_text(GTK_ENTRY(entry[i]), init_coordinate[i]);
    }
}

static GtkWidget* parametrs_widget_new() {
    GtkWidget *vbox;
    GtkWidget *coordinate_label;
    GtkWidget *coordinate_grid;
    GtkWidget *draw_button_image;
    GtkWidget *draw_button;
    
    /* Создание виджетов окна параметров */
    vbox                 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    coordinate_label     = gtk_label_new("Введите координаты");
    coordinate_grid      = gtk_grid_new();
    entry[COORDINATE_X1] = gtk_entry_new();
    entry[COORDINATE_Y1] = gtk_entry_new();
    entry[COORDINATE_X2] = gtk_entry_new();
    entry[COORDINATE_Y2] = gtk_entry_new();
    draw_button_image    = gtk_image_new_from_file(DRAW_BUTTON_IMAGE);
    draw_button          = gtk_button_new_with_label("Отрисовать");
    
    /* Настройка виджетов окна параметров */
    gtk_grid_set_row_spacing(GTK_GRID(coordinate_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(coordinate_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_X1]),  "x1");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_Y1]),  "y1");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_X2]),  "x2");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[COORDINATE_Y2]),  "y2");
    gtk_button_set_image(GTK_BUTTON(draw_button), draw_button_image);
    
    /* Упаковка окна параметров */
    gtk_grid_attach(GTK_GRID(coordinate_grid), entry[COORDINATE_X1], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(coordinate_grid), entry[COORDINATE_Y1], 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(coordinate_grid), entry[COORDINATE_X2], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(coordinate_grid), entry[COORDINATE_Y2], 1, 1, 1, 1);
    gtk_box_pack_start(GTK_BOX(vbox), coordinate_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), coordinate_grid,  FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), draw_button,      FALSE, TRUE, 0);
    
    fill_parametrs();
    
    g_signal_connect(draw_button,  "released", G_CALLBACK(draw_button_clicked), NULL);
    g_signal_connect(drawing_area, "draw",     G_CALLBACK(draw_callback),       NULL);
    
    return vbox;
}

GtkWidget* second_task_new() {
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

