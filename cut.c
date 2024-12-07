#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define THRESHOLD 128
#define SPACE_THRESHOLD 10 // Ajustez cette valeur en fonction de l'espacement entre les mots
#define OUTPUT_SIZE 28 // Taille de l'image de sortie (28x28)

typedef struct {
    int x, y, width, height;
} BoundingBox;

// Structure pour stocker les informations sur les composants connexes
typedef struct {
    int label;
    BoundingBox bbox;
    GList *pixels;
} ConnectedComponent;

// Structure pour stocker une zone sélectionnée
typedef struct {
    GdkRectangle rect;
} SelectedArea;

// Variables globales
GdkPixbuf *original_pixbuf = NULL;
GtkWidget *image_widget = NULL;
GtkWidget *finish_button = NULL;

GdkRectangle grid_rect;
gboolean grid_selected = FALSE;
GList *wordlist_areas = NULL; // Liste des zones de wordlist sélectionnées

// État de la sélection :
// 0 = prochaine sélection = grille
// 1 = prochaines sélections = zones wordlist
int selection_phase = 0;

gboolean selecting = FALSE;
gint start_x, start_y, end_x, end_y;
GdkRectangle current_rect;

// Fonctions prototypes
void binarize_image(GdkPixbuf *pixbuf, guint8 *binary_data);
void find_connected_components(guint8 *binary_data, int width, int height, GList **components);
gint compare_components_by_y(gconstpointer a_ptr, gconstpointer b_ptr);
gint compare_components_by_x(gconstpointer a_ptr, gconstpointer b_ptr);
void process_grid_region(GdkPixbuf *pixbuf);
void process_wordlist_region(GdkPixbuf *pixbuf, int area_index);
gboolean bounding_boxes_overlap_vertically(BoundingBox a, BoundingBox b);

static GdkPixbuf *convert_to_28x28_with_white(GdkPixbuf *src);

// Fonctions de traitement

void binarize_image(GdkPixbuf *pixbuf, guint8 *binary_data) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int y = 0; y < height; y++) {
        guchar *p = pixels + y * rowstride;
        for (int x = 0; x < width; x++) {
            guchar gray = (guchar)(0.3 * p[0] + 0.59 * p[1] + 0.11 * p[2]);
            binary_data[y * width + x] = (gray < THRESHOLD) ? 1 : 0;
            p += n_channels;
        }
    }
}

void find_connected_components(guint8 *binary_data, int width, int height, GList **components) {
    int label = 1;
    int *labels = calloc(width * height, sizeof(int));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (binary_data[y * width + x] == 1 && labels[y * width + x] == 0) {
                ConnectedComponent *cc = g_malloc(sizeof(ConnectedComponent));
                cc->label = label;
                cc->bbox.x = x;
                cc->bbox.y = y;
                cc->bbox.width = 0;
                cc->bbox.height = 0;
                cc->pixels = NULL;

                GQueue *queue = g_queue_new();
                g_queue_push_tail(queue, GINT_TO_POINTER(y * width + x));
                labels[y * width + x] = label;

                while (!g_queue_is_empty(queue)) {
                    int index = GPOINTER_TO_INT(g_queue_pop_head(queue));
                    int ix = index % width;
                    int iy = index / width;
                    cc->pixels = g_list_prepend(cc->pixels, GINT_TO_POINTER(index));

                    if (ix < cc->bbox.x) cc->bbox.x = ix;
                    if (iy < cc->bbox.y) cc->bbox.y = iy;
                    if (ix > cc->bbox.x + cc->bbox.width) cc->bbox.width = ix - cc->bbox.x;
                    if (iy > cc->bbox.y + cc->bbox.height) cc->bbox.height = iy - cc->bbox.y;

                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int nx = ix + dx;
                            int ny = iy + dy;
                            if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                                if (binary_data[ny * width + nx] == 1 && labels[ny * width + nx] == 0) {
                                    labels[ny * width + nx] = label;
                                    g_queue_push_tail(queue, GINT_TO_POINTER(ny * width + nx));
                                }
                            }
                        }
                    }
                }

                g_queue_free(queue);
                *components = g_list_append(*components, cc);
                label++;
            }
        }
    }

    free(labels);
}

gint compare_components_by_y(gconstpointer a_ptr, gconstpointer b_ptr) {
    ConnectedComponent *a = (ConnectedComponent *)a_ptr;
    ConnectedComponent *b = (ConnectedComponent *)b_ptr;
    return a->bbox.y - b->bbox.y;
}

gint compare_components_by_x(gconstpointer a_ptr, gconstpointer b_ptr) {
    ConnectedComponent *a = (ConnectedComponent *)a_ptr;
    ConnectedComponent *b = (ConnectedComponent *)b_ptr;
    return a->bbox.x - b->bbox.x;
}

gboolean bounding_boxes_overlap_vertically(BoundingBox a, BoundingBox b) {
    int a_top = a.y;
    int a_bottom = a.y + a.height;
    int b_top = b.y;
    int b_bottom = b.y + b.height;
    int overlap = MIN(a_bottom, b_bottom) - MAX(a_top, b_top);
    return overlap > 0;
}

// Fonction pour créer un pixbuf 28x28 blanc et y centrer le caractère
static GdkPixbuf *convert_to_28x28_with_white(GdkPixbuf *src) {
    int w = gdk_pixbuf_get_width(src);
    int h = gdk_pixbuf_get_height(src);

    // Créer un pixbuf 28x28 blanc, sans alpha
    GdkPixbuf *dest = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, OUTPUT_SIZE, OUTPUT_SIZE);
    gdk_pixbuf_fill(dest, 0xFFFFFFFF); // fond blanc (R=255, G=255, B=255)

    int offset_x = 0;
    int offset_y = 0;
    if (w < OUTPUT_SIZE) {
        offset_x = (OUTPUT_SIZE - w) / 2;
    }
    if (h < OUTPUT_SIZE) {
        offset_y = (OUTPUT_SIZE - h) / 2;
    }

    // Utilisation de gdk_pixbuf_composite pour copier
    gdk_pixbuf_composite(
        src,
        dest,
        offset_x, offset_y, // position dans dest
        w, h,               // taille copiée
        offset_x, offset_y, // offset de translation
        1.0, 1.0,           // échelle en X et Y (1.0 = pas de redimensionnement)
        GDK_INTERP_NEAREST, // type d'interpolation
        255                 // niveau d'alpha global (255 = opaque)
    );

    return dest;
}

void process_grid_region(GdkPixbuf *pixbuf) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    guint8 *binary_data = calloc(width * height, sizeof(guint8));
    binarize_image(pixbuf, binary_data);

    GList *components = NULL;
    find_connected_components(binary_data, width, height, &components);

    components = g_list_sort(components, compare_components_by_y);

    // Regrouper en lignes
    GList *rows = NULL;
    for (GList *l = components; l != NULL; l = l->next) {
        ConnectedComponent *cc = (ConnectedComponent *)l->data;
        gboolean added = FALSE;

        for (GList *r = rows; r != NULL; r = r->next) {
            GList *row = (GList *)r->data;
            ConnectedComponent *first_in_row = (ConnectedComponent *)row->data;
            if (bounding_boxes_overlap_vertically(cc->bbox, first_in_row->bbox)) {
                row = g_list_append(row, cc);
                r->data = row;
                added = TRUE;
                break;
            }
        }

        if (!added) {
            GList *new_row = NULL;
            new_row = g_list_append(new_row, cc);
            rows = g_list_append(rows, new_row);
        }
    }

    int row_index = 0;
    for (GList *r = rows; r != NULL; r = r->next) {
        GList *row = (GList *)r->data;
        row = g_list_sort(row, compare_components_by_x);

        int col_index = 0;
        for (GList *l = row; l != NULL; l = l->next) {
            ConnectedComponent *cc = (ConnectedComponent *)l->data;

            int x = cc->bbox.x;
            int y = cc->bbox.y;
            int w_char = cc->bbox.width + 1;
            int h_char = cc->bbox.height + 1;

            // Ajuster si dépasse
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x + w_char > width) w_char = width - x;
            if (y + h_char > height) h_char = height - y;

            // Vérifier validité
            if (w_char <= 0 || h_char <= 0) {
                continue; // Passe ce composant
            }

            GdkPixbuf *char_pixbuf = gdk_pixbuf_new_subpixbuf(
                pixbuf,
                x,
                y,
                w_char,
                h_char
            );

            if (char_pixbuf == NULL) {
                g_print("Impossible de créer le subpixbuf pour la grille, x=%d, y=%d, w=%d, h=%d\n", x, y, w_char, h_char);
                continue;
            }

            // Entourer de blanc dans un 28x28
            GdkPixbuf *final_pixbuf = convert_to_28x28_with_white(char_pixbuf);
            g_object_unref(char_pixbuf);

            char filename[256];
            sprintf(filename, "grid_char_%d_%d.png", row_index, col_index++);
            gdk_pixbuf_save(final_pixbuf, filename, "png", NULL, NULL);
            g_object_unref(final_pixbuf);
        }

        g_list_free(row);
        row_index++;
    }

    g_list_free(rows);

    for (GList *l = components; l != NULL; l = l->next) {
        ConnectedComponent *cc = (ConnectedComponent *)l->data;
        g_list_free(cc->pixels);
        g_free(cc);
    }
    g_list_free(components);

    free(binary_data);
}

void process_wordlist_region(GdkPixbuf *pixbuf, int area_index) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    guint8 *binary_data = calloc(width * height, sizeof(guint8));
    binarize_image(pixbuf, binary_data);

    GList *components = NULL;
    find_connected_components(binary_data, width, height, &components);

    components = g_list_sort(components, compare_components_by_y);

    // Regrouper en lignes
    GList *lines = NULL;
    for (GList *l = components; l != NULL; l = l->next) {
        ConnectedComponent *cc = (ConnectedComponent *)l->data;
        gboolean added = FALSE;

        for (GList *r = lines; r != NULL; r = r->next) {
            GList *line = (GList *)r->data;
            ConnectedComponent *first_in_line = (ConnectedComponent *)line->data;
            if (bounding_boxes_overlap_vertically(cc->bbox, first_in_line->bbox)) {
                line = g_list_append(line, cc);
                r->data = line;
                added = TRUE;
                break;
            }
        }

        if (!added) {
            GList *new_line = NULL;
            new_line = g_list_append(new_line, cc);
            lines = g_list_append(lines, new_line);
        }
    }

    int line_index = 0;
    for (GList *r = lines; r != NULL; r = r->next) {
        GList *line = (GList *)r->data;
        line = g_list_sort(line, compare_components_by_x);

        int char_index = 0;
        for (GList *l = line; l != NULL; l = l->next) {
            ConnectedComponent *cc = (ConnectedComponent *)l->data;
            int x = cc->bbox.x;
            int y = cc->bbox.y;
            int w_char = cc->bbox.width + 1;
            int h_char = cc->bbox.height + 1;

            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x + w_char > width) w_char = width - x;
            if (y + h_char > height) h_char = height - y;

            if (w_char <= 0 || h_char <= 0) {
                continue; // Passe ce composant invalide
            }

            GdkPixbuf *char_pixbuf = gdk_pixbuf_new_subpixbuf(
                pixbuf,
                x,
                y,
                w_char,
                h_char
            );

            if (char_pixbuf == NULL) {
                g_print("Impossible de créer le subpixbuf pour le mot, x=%d, y=%d, w=%d, h=%d\n", x, y, w_char, h_char);
                continue;
            }

            // Entourer de blanc dans un 28x28 sans redimensionner
            GdkPixbuf *final_pixbuf = convert_to_28x28_with_white(char_pixbuf);
            g_object_unref(char_pixbuf);

            char filename[256];
            // Format: word_<zone>_<ligne>_<position>.png
            sprintf(filename, "word_%d_%d_%d.png", area_index, line_index, char_index++);
            gdk_pixbuf_save(final_pixbuf, filename, "png", NULL, NULL);
            g_object_unref(final_pixbuf);
        }

        g_list_free(line);
        line_index++;
    }

    g_list_free(lines);

    for (GList *l = components; l != NULL; l = l->next) {
        ConnectedComponent *cc = (ConnectedComponent *)l->data;
        g_list_free(cc->pixels);
        g_free(cc);
    }
    g_list_free(components);

    free(binary_data);
}

// Callbacks GTK
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data; (void)widget;
    gdk_cairo_set_source_pixbuf(cr, original_pixbuf, 0, 0);
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
    cairo_set_line_width(cr, 2.0);

    if (selecting) {
        cairo_rectangle(cr, MIN(start_x, end_x), MIN(start_y, end_y),
                        ABS(end_x - start_x), ABS(end_y - start_y));
        cairo_stroke(cr);
    }

    // Dessin des zones déjà sélectionnées
    // La grille (si sélectionnée)
    if (grid_selected) {
        cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
        cairo_rectangle(cr, grid_rect.x, grid_rect.y, grid_rect.width, grid_rect.height);
        cairo_stroke(cr);
    }

    // Les zones wordlist
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
    for (GList *l = wordlist_areas; l != NULL; l = l->next) {
        SelectedArea *area = (SelectedArea *)l->data;
        cairo_rectangle(cr, area->rect.x, area->rect.y, area->rect.width, area->rect.height);
        cairo_stroke(cr);
    }

    return FALSE;
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void)user_data; (void)widget;
    if (event->button == GDK_BUTTON_PRIMARY) {
        selecting = TRUE;
        start_x = (int)event->x;
        start_y = (int)event->y;
        end_x = start_x;
        end_y = start_y;
    }
    return TRUE;
}

static gboolean on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    (void)widget; (void)user_data;
    if (selecting) {
        end_x = (int)event->x;
        end_y = (int)event->y;
        gtk_widget_queue_draw(widget);
    }
    return TRUE;
}

static gboolean on_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void)widget; (void)user_data;
    if (event->button == GDK_BUTTON_PRIMARY && selecting) {
        selecting = FALSE;
        end_x = (int)event->x;
        end_y = (int)event->y;

        current_rect.x = MIN(start_x, end_x);
        current_rect.y = MIN(start_y, end_y);
        current_rect.width = ABS(end_x - start_x);
        current_rect.height = ABS(end_y - start_y);

        if (current_rect.width > 0 && current_rect.height > 0) {
            if (selection_phase == 0) {
                // Première sélection : grille
                grid_rect = current_rect;
                grid_selected = TRUE;
                selection_phase = 1; // maintenant on passe aux wordlists
            } else {
                // Sélection de zones wordlist
                SelectedArea *area = g_malloc(sizeof(SelectedArea));
                area->rect = current_rect;
                wordlist_areas = g_list_append(wordlist_areas, area);
            }
        }

        gtk_widget_queue_draw(widget);
    }
    return TRUE;
}

// Callback du bouton "Terminé"
static void on_finish_button_clicked(GtkWidget *widget, gpointer user_data) {
    (void)widget; (void)user_data;

    if (!grid_selected) {
        g_print("Aucune grille sélectionnée. Sélectionnez la grille d'abord.\n");
        return;
    }

    // Traiter la grille
    GdkPixbuf *grid_pixbuf = gdk_pixbuf_new_subpixbuf(
        original_pixbuf,
        grid_rect.x,
        grid_rect.y,
        grid_rect.width,
        grid_rect.height
    );
    process_grid_region(grid_pixbuf);
    g_object_unref(grid_pixbuf);

    // Traiter chaque zone wordlist
    int area_index = 0;
    for (GList *l = wordlist_areas; l != NULL; l = l->next) {
        SelectedArea *area = (SelectedArea *)l->data;
        GdkPixbuf *wl_pixbuf = gdk_pixbuf_new_subpixbuf(
            original_pixbuf,
            area->rect.x,
            area->rect.y,
            area->rect.width,
            area->rect.height
        );
        process_wordlist_region(wl_pixbuf, area_index++);
        g_object_unref(wl_pixbuf);
    }

    // Nettoyage
    for (GList *l = wordlist_areas; l != NULL; l = l->next) {
        g_free(l->data);
    }
    g_list_free(wordlist_areas);
    wordlist_areas = NULL;

    // Fermer l'application
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    if (argc < 2) {
        g_print("Usage: %s image_filename\n", argv[0]);
        return -1;
    }

    GError *error = NULL;
    original_pixbuf = gdk_pixbuf_new_from_file(argv[1], &error);
    if (!original_pixbuf) {
        g_print("Erreur lors du chargement de l'image: %s\n", error->message);
        return -1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sélectionnez les zones (Grille puis Wordlists)");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Créer une box verticale
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image_widget = gtk_drawing_area_new();
    gtk_widget_set_size_request(image_widget, gdk_pixbuf_get_width(original_pixbuf), gdk_pixbuf_get_height(original_pixbuf));
    gtk_box_pack_start(GTK_BOX(vbox), image_widget, TRUE, TRUE, 0);

    // Bouton "Terminé"
    finish_button = gtk_button_new_with_label("Terminé");
    gtk_box_pack_start(GTK_BOX(vbox), finish_button, FALSE, FALSE, 0);
    g_signal_connect(finish_button, "clicked", G_CALLBACK(on_finish_button_clicked), NULL);

    gtk_widget_add_events(image_widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    g_signal_connect(G_OBJECT(image_widget), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(image_widget), "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(G_OBJECT(image_widget), "button-release-event", G_CALLBACK(on_button_release_event), NULL);
    g_signal_connect(G_OBJECT(image_widget), "motion-notify-event", G_CALLBACK(on_motion_notify_event), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    g_object_unref(original_pixbuf);

    return 0;
}
