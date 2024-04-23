#include <gtk/gtk.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include "utils.c"

struct dent
{
    struct stat statbuf;
    struct dirent *dir;
    bool is_dir,is_lnk;
    char* name;
    char* path;
    GtkWidget *entry;
};
struct dent elms[1024];
size_t elms_len=0;
struct dent *selected=NULL;
mode_struct selected_mode={};

void list_print_inarr(char* path){
    elms_len=list_inarr(path,(struct dent_agostic*)elms,1024);
}
static void
change_dir_btn (GtkWidget *widget,
       gpointer   data);
static void
print_sth (GtkWidget *widget,
       gpointer   data);
static void
delete (GtkWidget *widget,
             gpointer   data);

GtkWidget *path_ent;
GtkWidget *grid;
GtkWidget *sel_label;
GtkWidget *perm_chboxes[9]={NULL};

bool in_refresh=false;
void refresh_chboxes(){
    in_refresh=true;
    if(selected){
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[0]),selected_mode.ur);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[1]),selected_mode.uw);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[2]),selected_mode.ux);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[3]),selected_mode.gr);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[4]),selected_mode.gw);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[5]),selected_mode.gx);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[6]),selected_mode.or);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[7]),selected_mode.ow);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[8]),selected_mode.ox);
    }
    else{
        for(int i=0;i<9;i++){
            gtk_check_button_set_active(GTK_CHECK_BUTTON(perm_chboxes[i]),0);
        }
    }
    in_refresh=false;
}

void fill(GtkGrid *grid){
    for (size_t i = 0; i < elms_len; i++)
    {
        gtk_grid_remove(grid,elms[i].entry);
        //g_object_unref(elms[i].entry);
        elms[i].entry=NULL;
        free(elms[i].name);
        free(elms[i].path);
    }
    list_print_inarr(".");
    qsort(elms, elms_len, sizeof(struct dent), cmpstringp);
    for (size_t i = 0; i < elms_len; i++){
        if(!strcmp(elms[i].name,"..")){
            struct dent temp=elms[0];
            elms[0]=elms[i];
            elms[i]=temp;
            break;
        }
    }
    for (size_t i = 0; i < elms_len; i++)
    {
        elms[i].entry = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
        //gtk_widget_set_hexpand(elms[i].entry,true);
        GtkWidget *label;
        GtkWidget *button;
        GtkWidget *button2;
        GtkWidget *button_del;
        label=gtk_image_new_from_icon_name(elms[i].is_dir?"folder":"unknown");
        gtk_widget_set_margin_start(label,10);
        gtk_widget_set_margin_end(label,10);
        button=gtk_button_new_with_label (elms[i].name);
        gtk_widget_set_hexpand(button,true);
        gtk_widget_set_halign(gtk_button_get_child(GTK_BUTTON(button)),GTK_ALIGN_START);
        //gtk_widget_set_margin_end(button,20);
        button2=gtk_button_new_from_icon_name ("configure");
        button_del=gtk_button_new_from_icon_name ("delete");
        g_signal_connect (button, "clicked", G_CALLBACK (change_dir_btn), &elms[i]);
        g_signal_connect (button2, "clicked", G_CALLBACK (print_sth), &elms[i]);
        g_signal_connect (button_del, "clicked", G_CALLBACK (delete), &elms[i]);
        //gtk_button_add_pixlabel(elms[i].entry,"folder");
        gtk_box_append(GTK_BOX(elms[i].entry),label);
        gtk_box_append(GTK_BOX(elms[i].entry),button);
        gtk_box_append(GTK_BOX(elms[i].entry),button2);
        gtk_box_append(GTK_BOX(elms[i].entry),button_del);
        gtk_grid_attach (GTK_GRID (grid), elms[i].entry, 0, i+2, 2, 1);
    }
    char* p=getcwd(NULL,0);
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(path_ent)),p,-1);
    free(p);
    selected=NULL;
    gtk_label_set_text(GTK_LABEL(sel_label),"");
    refresh_chboxes();
}


static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}
static void
toggle_perm (GtkCheckButton *widget,
             gpointer   data)
{
    if(in_refresh)return;
    size_t i=(size_t)data;
    if(!selected){
        gtk_check_button_set_active(widget,0);
    }
    else{
        switch (i)
        {
        case 0:
            selected_mode.ur=gtk_check_button_get_active(widget);
            break;
        case 1:
            selected_mode.uw=gtk_check_button_get_active(widget);
            break;
        case 2:
            selected_mode.ux=gtk_check_button_get_active(widget);
            break;
        case 3:
            selected_mode.gr=gtk_check_button_get_active(widget);
            break;
        case 4:
            selected_mode.gw=gtk_check_button_get_active(widget);
            break;
        case 5:
            selected_mode.gx=gtk_check_button_get_active(widget);
            break;
        case 6:
            selected_mode.or=gtk_check_button_get_active(widget);
            break;
        case 7:
            selected_mode.ow=gtk_check_button_get_active(widget);
            break;
        case 8:
            selected_mode.ox=gtk_check_button_get_active(widget);
            break;
        
        default:
            break;
        }
        g_print("%u,%u%u%u\n",struct_to_mode(selected_mode),selected_mode.ur,selected_mode.uw,selected_mode.ux);
        if(print_errno(chmod(selected->name,struct_to_mode(selected_mode)),"chmod")){
            fill(GTK_GRID(grid));
        }
        stat(selected->name,&(selected->statbuf));
    }
}
static void
print_sth (GtkWidget *widget,
             gpointer   data)
{
    struct dent *d=data;
    g_print (d->name);
    g_print ("\n");
    selected=d;
    gtk_label_set_text(GTK_LABEL(sel_label),d->name);
    selected_mode=mode_to_struct(d->statbuf.st_mode);
    refresh_chboxes();
}
static void
delete (GtkWidget *widget,
             gpointer   data)
{
    struct dent *d=data;
    g_print (d->name);
    g_print ("  ");
    g_print (d->is_dir?"dir":"file");
    g_print ("\n");
    if(d->is_dir){
        rm_rec(d->name);
    }
    else{
        print_errno(remove(d->name),"remove");
    }
    fill(GTK_GRID (gtk_widget_get_parent(gtk_widget_get_parent(widget))));
}
static void
go_home (GtkWidget *widget,
             gpointer   data)
{
    const char* p="/home";
    g_print (p);
    chdir(p);
    fill(GTK_GRID(grid));
}
static void
change_dir (GtkWidget *widget,
            GtkEntryIconPosition icon_pos,
            gpointer   data)
{
    const char* p=gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(path_ent)));
    g_print (p);
    chdir(p);
    fill(GTK_GRID(grid));
}
static void
make_dir (GtkWidget *widget,
            GtkEntryIconPosition icon_pos,
            gpointer   data)
{
    const char* p=gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widget)));
    g_print (p);
    int r=mkdir_default(p);
    g_print(r?" n\n":" y\n");
    fill(GTK_GRID(grid));
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widget)),"",0);
}
static void
make_file (GtkWidget *widget,
            GtkEntryIconPosition icon_pos,
            gpointer   data)
{
    const char* p=gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widget)));
    g_print (p);
    int r=mkfile(p);
    g_print(r?" n\n":" y\n");
    fill(GTK_GRID(grid));
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widget)),"",0);
}
static void
make_link (GtkWidget *widget,
            GtkEntryIconPosition icon_pos,
            gpointer   data)
{
    if(selected){
        const char* p=gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widget)));
        char* source=realpath(selected->name,NULL);
        g_print (p);
        int r=symlink(source,p);
        g_print(r?" n\n":" y\n");
        free(source);
        fill(GTK_GRID(grid));
    }
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(widget)),"",0);
}
static void
change_dir_btn (GtkWidget *widget,
             gpointer   data)
{
    struct dent *d=data;
    g_print (d->name);
    g_print ("  ");
    g_print (d->is_dir?"dir":"file");
    g_print ("\n");
    if(d->is_dir){
        chdir(d->name);
        fill(GTK_GRID (gtk_widget_get_parent(gtk_widget_get_parent(widget))));
    }
  
}


static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *box2;
    GtkWidget *scroll;
    GtkWidget *button;
    GtkWidget *new_folder;
    GtkWidget *new_file;
    GtkWidget *perms_grid;
    GtkWidget *perm_label;
    //GtkWidget *perm_chbox;
    GtkWidget *lnk_box;
    GtkWidget *lnk_entry;
    GtkWidget *lnk_label;

    /* create a new window, and set its title */
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "File Browser");
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);

    box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

    gtk_window_set_child (GTK_WINDOW (window), box);

    scroll=gtk_scrolled_window_new();
    box2=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

    gtk_box_append(GTK_BOX(box),scroll);
    gtk_box_append(GTK_BOX(box),box2);

    /* Here we construct the container that is going pack our buttons */
    grid = gtk_grid_new ();

    /* Pack the container in the window */
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),grid);

    button = gtk_button_new_from_icon_name ("user-home");
    gtk_widget_set_hexpand(button,false);
    g_signal_connect (button, "clicked", G_CALLBACK (go_home), NULL);

    /* Place the first button in the grid cell (0, 0), and make it fill
    * just 1 cell horizontally and vertically (ie no spanning)
    */
    gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

    path_ent = gtk_entry_new();//gtk_button_new_with_label ("Button 2");
    char* p=getcwd(NULL,0);
    gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(path_ent)),p,-1);
    free(p);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(path_ent),GTK_ENTRY_ICON_SECONDARY,"keyboard-enter");
    g_signal_connect (path_ent, "icon_press", G_CALLBACK (change_dir), NULL);

    new_folder = gtk_entry_new();
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(new_folder),GTK_ENTRY_ICON_PRIMARY,"folder");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(new_folder),GTK_ENTRY_ICON_SECONDARY,"gtk-add");
    g_signal_connect (new_folder, "icon_press", G_CALLBACK (make_dir), NULL);
    gtk_box_append(GTK_BOX(box2),new_folder);

    new_file = gtk_entry_new();
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(new_file),GTK_ENTRY_ICON_PRIMARY,"unknown");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(new_file),GTK_ENTRY_ICON_SECONDARY,"gtk-add");
    g_signal_connect (new_file, "icon_press", G_CALLBACK (make_file), NULL);
    gtk_box_append(GTK_BOX(box2),new_file);

    perms_grid = gtk_grid_new ();
    gtk_box_append(GTK_BOX(box2),perms_grid);

    sel_label=gtk_label_new("");
    gtk_grid_attach (GTK_GRID (perms_grid), sel_label, 0, 0, 9, 1);
    perm_label=gtk_label_new("user");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 0, 1, 3, 1);
    perm_label=gtk_label_new("group");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 3, 1, 3, 1);
    perm_label=gtk_label_new("others");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 6, 1, 3, 1);

    perm_label=gtk_label_new("R");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 0, 2, 1, 1);
    perm_label=gtk_label_new("W");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 1, 2, 1, 1);
    perm_label=gtk_label_new("X");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 2, 2, 1, 1);

    perm_label=gtk_label_new("R");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 3, 2, 1, 1);
    perm_label=gtk_label_new("W");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 4, 2, 1, 1);
    perm_label=gtk_label_new("X");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 5, 2, 1, 1);

    perm_label=gtk_label_new("R");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 6, 2, 1, 1);
    perm_label=gtk_label_new("W");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 7, 2, 1, 1);
    perm_label=gtk_label_new("X");
    gtk_grid_attach (GTK_GRID (perms_grid), perm_label, 8, 2, 1, 1);

    size_t i=0;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;

    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;

    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;
    perm_chboxes[i]=gtk_check_button_new();
    g_signal_connect (perm_chboxes[i], "toggled", G_CALLBACK (toggle_perm), (void*)i);
    gtk_grid_attach (GTK_GRID (perms_grid), perm_chboxes[i], i, 3, 1, 1);
    i++;

    lnk_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_append(GTK_BOX(box2),lnk_box);

    lnk_label=gtk_label_new("link to:");
    gtk_box_append(GTK_BOX(lnk_box),lnk_label);

    lnk_entry = gtk_entry_new();
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(lnk_entry),GTK_ENTRY_ICON_PRIMARY,"link");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(lnk_entry),GTK_ENTRY_ICON_SECONDARY,"gtk-add");
    g_signal_connect (lnk_entry, "icon_press", G_CALLBACK (make_link), NULL);
    gtk_box_append(GTK_BOX(lnk_box),lnk_entry);

    /* Place the second button in the grid cell (1, 0), and make it fill
    * just 1 cell horizontally and vertically (ie no spanning)
    */
    gtk_grid_attach (GTK_GRID (grid), path_ent, 1, 0, 1, 1);

    button = gtk_button_new_with_label ("Quit");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_destroy), window);

    /* Place the Quit button in the grid cell (0, 1), and make it
    * span 2 columns.
    */
    gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

    fill(GTK_GRID (grid));


    gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
