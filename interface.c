#include "declarations.h"

int com;

GtkBuilder *builder;

GtkWidget *window;
GtkWidget *abcBox;
GtkWidget *comBox;
GtkButton *aButton;
GtkButton *bButton;
GtkButton *cButton;
GtkWidget *stations;
GtkToggleButton *s[12];
GtkWidget *start;

/* this function is called when exiting the GUI */
G_MODULE_EXPORT void on_settings_destroy(gpointer user_data) {
    gtk_main_quit();
    exit(0);
}

/* called when setting the COM-port */
G_MODULE_EXPORT void on_com_changed(GtkComboBoxText *com, gpointer *data) {
    gtk_widget_set_sensitive(abcBox, 1);
}

/* called when any of the abc buttons is clicked */
G_MODULE_EXPORT void on_abc_clicked(GtkButton *button, gpointer *data) {
    GList *list = gtk_container_get_children(GTK_CONTAINER(button));
    char *labelABC = "<b>X</b>";
    int i;

    amountChks = 0;

    gtk_widget_set_sensitive(comBox, 0);

    /* it checks what challenge button is pressed */
    sscanf(gtk_button_get_label(GTK_BUTTON(button)), "%c", &chal);
    chal += 32;

    /* removes former bold labels from all the ABC buttons */
    gtk_button_set_label(aButton, "A");
    gtk_button_set_label(bButton, "B");
    gtk_button_set_label(cButton, "C");

    /* activates the stations when a or b and activates start when c */
    /* sets the bold string for the button */
    switch(chal) {
        case 'a':   gtk_widget_set_sensitive(stations, 1);
                    gtk_widget_set_sensitive(start, 0);
                    labelABC = "<b>A</b>";
                    break;
        case 'b':   gtk_widget_set_sensitive(stations, 1);
                    gtk_widget_set_sensitive(start, 0);
                    labelABC = "<b>B</b>";
                    break;
        case 'c':   gtk_widget_set_sensitive(stations, 0);
                    gtk_widget_set_sensitive(start, 1);

                    for(i = 0; i < 12; i++)
                        gtk_toggle_button_set_active(s[i], 0);

                    labelABC = "<b>C</b>";
                    break;
    }

    /* checks how many stations are pressed */
    for(i = 0; i < 12; i++)
        amountChks += gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(s[i]));

    /* activates the start button when 3 stations are selected or challenge C is selected */
    if((amountChks > 0 && amountChks <= 3) || chal == 'c') {
        gtk_widget_set_sensitive(start, 1);
    } else {
        gtk_widget_set_sensitive(start, 0);
    }

    /* makes the clicked button bold */
    gtk_label_set_markup(GTK_LABEL(list->data), labelABC);

    /* read the selected COM-port */
    if(!strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comBox)), "DEBUG")) {
        debug = 1;
    } else {
        sscanf(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comBox)), "COM%d", &com);
    }

    /* the communication is started with the robot when not in DEBUG mode */
    if(!debug && !hSerial)
        hSerial = openCom(com);
}

/* this function is called when clicking any station button */
G_MODULE_EXPORT void on_s_toggled(GtkButton *button, gpointer *data) {
    int i;

    amountChks = 0;
    
    sscanf(gtk_button_get_label(GTK_BUTTON(button)), "%d", &i);

    /* checks whether the start button should be activated depending of the amount of stations selected */
    for(i = 0; i < 12; i++)
        amountChks += gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(s[i]));

    if(amountChks > 0 && amountChks <= 3) {
        gtk_widget_set_sensitive(start, 1);
    } else {
        gtk_widget_set_sensitive(start, 0);
    }
}

/* this function is called when clicking the start button, this closes the settings GUI */
G_MODULE_EXPORT void on_start_clicked(GtkButton *button, gpointer *data) {
    int i, j = 0;

    chks = malloc(amountChks * sizeof(int));

    for(i = 0; i < 12; i++)
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(s[i])))
            *(chks + j) = i + 1,
            j++;

    gtk_widget_hide(window);
    gtk_main_quit();

    if(debug)
        printf("Entered DEBUG-mode. You will have to send me bytes manually.\n");
}

/* this function builds the GUI; it ends when closing the GUI */
void gui(int argc, char *argv[]) {
    int i;
    char stringS[4];

    /* initializes the GUI */
    gtk_init (&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "settings.glade", NULL);

    /* creates the pointers to the widgets on the GUI */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "settings"));
    comBox = GTK_WIDGET(gtk_builder_get_object(builder, "com"));
    abcBox = GTK_WIDGET(gtk_builder_get_object(builder, "abc"));
    aButton = GTK_BUTTON(gtk_builder_get_object(builder, "a"));
    bButton = GTK_BUTTON(gtk_builder_get_object(builder, "b"));
    cButton = GTK_BUTTON(gtk_builder_get_object(builder, "c"));
    stations = GTK_WIDGET(gtk_builder_get_object(builder, "stations"));
    for(i = 0; i < 12; i++) {
        sprintf(stringS, "s%d", i + 1);
        s[i] = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, stringS));
    }
    start = GTK_WIDGET(gtk_builder_get_object(builder, "start"));

    /* removes the DEBUG-mode when not in DEBUG-mode */
    #if !DEBUG
        gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(comBox), 7);
    #endif

    /* actually makes sure the GUI is visible and is interactable */
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);
    gtk_main();
}

/* resets the gui back to the moment one of the abc-buttons was pressed */
void resetGUI() {
    gtk_widget_set_sensitive(comBox, 1);
    gtk_widget_set_sensitive(abcBox, 0);
    gtk_widget_set_sensitive(stations, 0);
    gtk_widget_set_sensitive(start, 0);

    gtk_button_set_label(aButton, "A");
    gtk_button_set_label(bButton, "B");
    gtk_button_set_label(cButton, "C");
}