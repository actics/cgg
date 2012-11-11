#include "cggfirst.h"

/*
 * Задание 1.
 * Построить график функции sin(ax+b)/(cx+d)
 */

#define FRAME_TITLE   "Постоение графика функции"
#define FUNCTION_NAME "<b>f(x) = sin(ax+b)/(cx+d) на [α, β]</b>"
#define BAD_PARAMETRS_ERROR_MESSAGE "<b>Введены неверные параметры</b>\n\
Поля параметров должны быть не пустыми, и должны сдержать  корректные числа"

#define DRAW_BUTTON_IMAGE "./images/draw_button.png"

#define EMPTY_ZONE_WIDTH 5
#define AXES_UNIT_PIXELS 30

enum {
    LEFT_BOUND,
    RIGHT_BOUND,
    PARAMETR_A,
    PARAMETR_B,
    PARAMETR_C,
    PARAMETR_D
};

static GtkWidget *drawing_area;
static GtkWidget *entry[6]; 
static float parametr[6];

static float f(float x) {
    return sin(parametr[PARAMETR_A]*x + parametr[PARAMETR_B]) 
            / (parametr[PARAMETR_C]*x + parametr[PARAMETR_D]);
}

static void draw_axes(cairo_t   *cr, 
                      int       screen_width, 
                      int       screen_height, 
                      const float min_y, 
                      const float max_y)
{
    int i;
    
    /***Расчет координат осей***/
    float expansion_x = (parametr[RIGHT_BOUND] - parametr[LEFT_BOUND]) / screen_width;
    float expansion_y = screen_height / (min_y - max_y);
    
    int x_axes_coor, y_axes_coor;
    if (min_y < 0 && 0 < max_y) {
        x_axes_coor = -max_y * expansion_y;
        if (x_axes_coor < EMPTY_ZONE_WIDTH)
            x_axes_coor = EMPTY_ZONE_WIDTH;
        else if (x_axes_coor > screen_height - EMPTY_ZONE_WIDTH)
            x_axes_coor = screen_height - EMPTY_ZONE_WIDTH;
    }
    else if (min_y > 0)
        x_axes_coor = screen_height - EMPTY_ZONE_WIDTH;
    else
        x_axes_coor = EMPTY_ZONE_WIDTH;
    
    if (parametr[LEFT_BOUND] < 0 && 0 < parametr[RIGHT_BOUND]) {
        y_axes_coor = -parametr[LEFT_BOUND] / expansion_x;
        if (y_axes_coor < EMPTY_ZONE_WIDTH)
            y_axes_coor = EMPTY_ZONE_WIDTH;
        else if (y_axes_coor > screen_width - EMPTY_ZONE_WIDTH)
            y_axes_coor = screen_width - EMPTY_ZONE_WIDTH;
    }
    else if (parametr[LEFT_BOUND] < 0)
        y_axes_coor = screen_width - EMPTY_ZONE_WIDTH;
    else
        y_axes_coor = EMPTY_ZONE_WIDTH;
    
    /***Отрисовка обрамляющей рамки***/
    
    cairo_rectangle(cr, 0, 0, screen_width-1, screen_height-1);
    
    screen_width  -= EMPTY_ZONE_WIDTH+1;
    screen_height -= EMPTY_ZONE_WIDTH+1;
    
    /***Отрисовка осей***/
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1.5);
    
    cairo_move_to(cr, EMPTY_ZONE_WIDTH, x_axes_coor);
    cairo_line_to(cr, screen_width,     x_axes_coor);
        
    cairo_move_to(cr, y_axes_coor, EMPTY_ZONE_WIDTH);
    cairo_line_to(cr, y_axes_coor, screen_height);
    
    cairo_stroke(cr);
    
    /***Отрисовка кординатной сетки***/
    
    cairo_set_source_rgb (cr, 0, 0.5, 0);
    cairo_set_line_width(cr, 0.3);
    
    for (i=y_axes_coor; i<=screen_width; i+=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, i, EMPTY_ZONE_WIDTH);
        cairo_line_to(cr, i, screen_height);
    }
    for (i=y_axes_coor; i>=EMPTY_ZONE_WIDTH; i-=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, i, EMPTY_ZONE_WIDTH);
        cairo_line_to(cr, i, screen_height);
    }
    
    for (i=x_axes_coor; i<=screen_height; i+=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, EMPTY_ZONE_WIDTH, i);
        cairo_line_to(cr, screen_width,     i);
    }
    for (i=x_axes_coor; i>=EMPTY_ZONE_WIDTH; i-=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, EMPTY_ZONE_WIDTH, i);
        cairo_line_to(cr, screen_width,     i);
    }
    
    cairo_stroke(cr);
    
    /***Отрисовка рисок на осях***/
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1.0);
    
    for (i=y_axes_coor; i<=screen_width; i+=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, i, x_axes_coor-3);
        cairo_line_to(cr, i, x_axes_coor+3);
    }
    for (i=y_axes_coor; i>=EMPTY_ZONE_WIDTH; i-=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, i, x_axes_coor-3);
        cairo_line_to(cr, i, x_axes_coor+3);
    }
    
    for (i=x_axes_coor; i<=screen_height; i+=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, y_axes_coor-3, i);
        cairo_line_to(cr, y_axes_coor+3, i);
    }
    for (i=x_axes_coor; i>=EMPTY_ZONE_WIDTH; i-=AXES_UNIT_PIXELS) {
        cairo_move_to(cr, y_axes_coor-3, i);
        cairo_line_to(cr, y_axes_coor+3, i);
    }
    
    cairo_stroke(cr);
}

static void draw_graph(cairo_t *cr) {
    int i;
    int screen_width, screen_height;
    
    float *points;
    float min_y, max_y;
    float expansion_x, expansion_y;
    float x;
    
    screen_width  = gtk_widget_get_allocated_width(GTK_WIDGET(drawing_area))+1;
    screen_height = gtk_widget_get_allocated_height(GTK_WIDGET(drawing_area))+1;
    
    screen_width  -= 2*EMPTY_ZONE_WIDTH;
    screen_height -= 2*EMPTY_ZONE_WIDTH;
    
    /***Расчет точек графика***/
    points = (float*) malloc(screen_width*sizeof(float));
    
    min_y = FLT_MAX;
    max_y = FLT_MIN;
    
    expansion_x = (parametr[RIGHT_BOUND] - parametr[LEFT_BOUND]) / screen_width;
    
    x = parametr[LEFT_BOUND];
    for (i=0; i<screen_width; i++, x+=expansion_x) {
        points[i] = f(x);
        if (points[i] > max_y) max_y = points[i];
        if (points[i] < min_y) min_y = points[i];
    }
    
    expansion_y = screen_height / (min_y - max_y);
    
    draw_axes(cr, screen_width+2*EMPTY_ZONE_WIDTH, screen_height+2*EMPTY_ZONE_WIDTH, min_y, max_y);
    
    /***Отрисовка графика***/
    
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr, 1.0);
    
    cairo_move_to(cr, EMPTY_ZONE_WIDTH, (int) ((points[0] - max_y)*expansion_y) + EMPTY_ZONE_WIDTH);
    for (i=1; i<screen_width; i++)
        cairo_line_to(cr, i+EMPTY_ZONE_WIDTH, (int) ((points[i] - max_y)*expansion_y) + EMPTY_ZONE_WIDTH);
    
    cairo_stroke(cr);
    
    free(points);
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer p) {
    draw_graph(cr);
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
    float new_parametr[6];
    
    int i;
    for (i=0; i<6; ++i) {
        char *parametr_str;
        parametr_str = (char*) gtk_entry_get_text(GTK_ENTRY(entry[i]));
        new_parametr[i] = atof(parametr_str);
        if (new_parametr[i] == 0 && strcmp(parametr_str, "0") != 0) {
            show_msg_dial(gtk_widget_get_toplevel(GTK_WIDGET(entry[LEFT_BOUND])));
            return;
        }
    }
    
    if (new_parametr[LEFT_BOUND] >= new_parametr[RIGHT_BOUND]) {
        show_msg_dial(gtk_widget_get_toplevel(GTK_WIDGET(entry[LEFT_BOUND])));
        return;
    }
    
    if (new_parametr[PARAMETR_C] == 0 && new_parametr[PARAMETR_D] == 0) {
        show_msg_dial(gtk_widget_get_toplevel(GTK_WIDGET(entry[LEFT_BOUND])));
        return;
    }
    
    for (i=0; i<6; ++i)
        parametr[i] = new_parametr[i];
    
    gtk_widget_queue_draw(drawing_area);
}

static void fill_parametrs() {
    const char* init_parametr[] = {"-10", "10", "1", "0", "1", "0"};
    int i;
    
    for (i=0; i<6; ++i) {
        parametr[i] = atof(init_parametr[i]);
        gtk_entry_set_text(GTK_ENTRY(entry[i]), init_parametr[i]);
    }
}

static GtkWidget* parametrs_widget_new() {
    GtkWidget *vbox;
    GtkWidget *function_name_label;
    GtkWidget *interval_label;
    GtkWidget *interval_hbox;
    GtkWidget *parametrs_label;
    GtkWidget *draw_button_image;
    GtkWidget *draw_button;
    GtkWidget *separator_0;
    GtkWidget *separator_1;
    GtkWidget *separator_2;
    GtkWidget *parametrs_grid;
    
    /* Создание виджетов окна параметров */
    vbox                = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    function_name_label = gtk_label_new(FUNCTION_NAME);
    separator_0         = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    interval_label      = gtk_label_new("Выберите интервал");
    interval_hbox       = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    entry[LEFT_BOUND]   = gtk_entry_new();
    entry[RIGHT_BOUND]  = gtk_entry_new();
    separator_1         = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    parametrs_label     = gtk_label_new("Введите параметры");
    parametrs_grid      = gtk_grid_new();
    entry[PARAMETR_A]   = gtk_entry_new();
    entry[PARAMETR_B]   = gtk_entry_new();
    entry[PARAMETR_C]   = gtk_entry_new();
    entry[PARAMETR_D]   = gtk_entry_new();
    separator_2         = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    draw_button_image   = gtk_image_new_from_file(DRAW_BUTTON_IMAGE);
    draw_button         = gtk_button_new_with_label("Отрисовать");
    
    /* Настройка виджетов окна параметров */
    gtk_label_set_use_markup(GTK_LABEL(function_name_label), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(parametrs_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(parametrs_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[LEFT_BOUND]),  "α");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[RIGHT_BOUND]), "β");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[PARAMETR_A]),  "a");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[PARAMETR_B]),  "b");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[PARAMETR_C]),  "c");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry[PARAMETR_D]),  "d");
    gtk_button_set_image(GTK_BUTTON(draw_button), draw_button_image);
    
    /* Упаковка окна параметров */
    gtk_box_pack_start(GTK_BOX(vbox), function_name_label,  FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), separator_0,          FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(interval_hbox), entry[LEFT_BOUND],  FALSE, TRUE, 0);    
    gtk_box_pack_start(GTK_BOX(interval_hbox), entry[RIGHT_BOUND], FALSE, TRUE, 0);
    gtk_grid_attach(GTK_GRID(parametrs_grid),  entry[PARAMETR_A], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid),  entry[PARAMETR_B], 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid),  entry[PARAMETR_C], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(parametrs_grid),  entry[PARAMETR_D], 1, 1, 1, 1);
    gtk_box_pack_start(GTK_BOX(vbox), interval_label,  FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), interval_hbox,   FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), separator_1,     FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), parametrs_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), parametrs_grid,  FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), separator_2,     FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), draw_button,     FALSE, TRUE, 0);
    
    fill_parametrs();
    
    g_signal_connect(draw_button,  "released", G_CALLBACK(draw_button_clicked), NULL);
    g_signal_connect(drawing_area, "draw",     G_CALLBACK(draw_callback),       NULL);
    
    return vbox;
}

GtkWidget* first_task_new() {
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
