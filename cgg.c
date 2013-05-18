#include <gtk/gtk.h>

#include "cggfirst.h"
#include "cggsecond.h"
#include "cggthird.h"

#define AVE_TEXT  "<big><b>Компьютерная Графика и Геометрия (КГГ)</b></big>\n\
Практические работы Александра Лаврукова"
#define ABOUT_MESSAGE "<b>О авторе</b>\nАлександр Лавруков. УрГУ КН-302. 2012"

#define IMAGES_DIRECTORY "./images/"

#define AVE_IMAGE          IMAGES_DIRECTORY "ave_image.png"
#define BOTTOM_ABOUT_IMAGE IMAGES_DIRECTORY "bottom_about.png"
#define BOTTOM_CLOSE_IMAGE IMAGES_DIRECTORY "bottom_close.png"
#define AUTHOR_PHOTO       IMAGES_DIRECTORY "author_photo.jpg"

static void show_about(GtkWidget *button, GtkWidget *parent_window) {
    GtkWidget *msg_dial;
    GtkWidget *photo;
    msg_dial = gtk_message_dialog_new_with_markup(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        ABOUT_MESSAGE
    );
    gtk_window_set_title(GTK_WINDOW(msg_dial), "О авторе");
    photo = gtk_image_new_from_file(AUTHOR_PHOTO);
    gtk_message_dialog_set_image(GTK_MESSAGE_DIALOG(msg_dial), photo);
    gtk_widget_show(photo);
    gtk_dialog_run(GTK_DIALOG(msg_dial));
    gtk_widget_destroy(msg_dial);
}

int main(int argc, char ** argv) {
    GtkWidget *main_window;
    GtkWidget *main_vbox;
    GtkWidget *main_ave_hbox;
    GtkWidget *main_ave_image;
    GtkWidget *main_ave_label;
    GtkWidget *main_0_separator;
    GtkWidget *main_notebook;
    GtkWidget *main_bottom_hbox;
    GtkWidget *main_bottom_about_image;
    GtkWidget *main_bottom_about_button;
    GtkWidget *main_bottom_filler;
    GtkWidget *main_bottom_close_image;
    GtkWidget *main_bottom_close_button;
    
    GtkWidget *first_task;
    GtkWidget *second_task;
    GtkWidget *third_task;
    
    gtk_init(&argc, &argv);
    
    /* Создание и настройка главного окна */
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "КГГ практика");
    gtk_window_set_default_geometry(GTK_WINDOW(main_window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(main_window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(main_window), 7);

    /* Создание шапки */
    main_ave_image = gtk_image_new_from_file(AVE_IMAGE);
    main_ave_label = gtk_label_new(AVE_TEXT);
    
    /* Настройка шапки */
    gtk_label_set_use_markup(GTK_LABEL(main_ave_label), TRUE);
    
    /* Упаковка шапки */
    main_ave_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_ave_hbox), main_ave_image, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_ave_hbox), main_ave_label, FALSE, TRUE, 0);

    /* Создание нижней панели */
    main_bottom_about_image  = gtk_image_new_from_file(BOTTOM_ABOUT_IMAGE);
    main_bottom_about_button = gtk_button_new_with_label("О авторе");
    main_bottom_filler       = gtk_grid_new(); /* Православное заполнение как? */
    main_bottom_close_image  = gtk_image_new_from_file(BOTTOM_CLOSE_IMAGE);
    main_bottom_close_button = gtk_button_new_with_label("Закрыть");
    
    /* Настройка нижней панели */
    gtk_button_set_image(GTK_BUTTON(main_bottom_about_button), main_bottom_about_image);
    gtk_button_set_image(GTK_BUTTON(main_bottom_close_button), main_bottom_close_image);

    /* Упаковка нижней панели */
    main_bottom_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_bottom_hbox), main_bottom_about_button, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_bottom_hbox), main_bottom_filler,       TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_bottom_hbox), main_bottom_close_button, FALSE, TRUE, 0);
    
    /* Создание виджета вкладок */
    main_notebook = gtk_notebook_new();

    /* Упаковка главного окна */
    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    main_0_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(main_vbox), main_ave_hbox,    FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), main_0_separator, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), main_notebook,     TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), main_bottom_hbox, FALSE, TRUE, 0);
    
    /* Создание и упаковка виджета с первым заданием */
    first_task = first_task_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(main_notebook), first_task, gtk_label_new("Задание 1"));
    
    /* Создание и упаковка виджета со вторым заданием */
    second_task = second_task_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(main_notebook), second_task, gtk_label_new("Задание 2"));
    
    /* Создание и упаковка виджета со вторым заданием */
    third_task = third_task_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(main_notebook), third_task, gtk_label_new("Задание 3"));
    
    /* Подключение необходимых обратных вызовов */
    g_signal_connect(G_OBJECT(main_bottom_about_button), "clicked", G_CALLBACK(show_about),    main_window);
    g_signal_connect(G_OBJECT(main_bottom_close_button), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(main_window),              "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Запуск */
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
    gtk_widget_show_all(main_window);
    gtk_main();
    
    return 0;
}
