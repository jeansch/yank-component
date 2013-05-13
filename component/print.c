/*
 * print.c
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Huﬂmann <m.hussmann@home.ins.de>
 *
 * This program is free software; you  can redistribute it and/or modify it
 * under the terms of  the GNU General  Public License as published  by the
 * Free Software Foundation;  either version 2 of  the License, or (at your
 * option) any later version.
 *
 * This program  is  distributed in the  hope  that it will  be useful, but
 * WITHOUT  ANY   WARRANTY;  without   even   the  implied    warranty   of
 * MERCHANTABILITY or    FITNESS FOR A PARTICULAR   PURPOSE.   See  the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write to the  Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
  

#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include "print.h"
#include "privates.h"


static void  cb_print_dialog_clicked(GtkWidget *, gint, gpointer);
static gint  do_print_notes(GnomePrintMaster *, GnomePrinter *);
static void  print_tree(GnomePrintMaster *, GtkCTree *, GtkCTreeNode *,
                        GList *, float *, float *);
static void  print_notes(GnomePrintMaster *, GtkCTree *, float *, float *);
static void  print_note_title(GnomePrintContext *, note_data *);
static float get_next_y(GnomePrintMaster *, float, float);
static void  print_note_text(GnomePrintMaster *, note_data *, float *, float *);

static GtkWidget        *check_print_tree = NULL;
static GtkWidget        *check_print_notes = NULL;
static const GnomePaper *paper = NULL;
static gint             pages = 0;


/* ------------------------------------------------------ */
/*
 * print set of notes
 */

void
cb_print_notes(GtkWidget *w, gpointer p)
{
    GtkWidget *p_dialog;
    gchar     *title;
    GtkWidget *hbox1;

    if (GTK_CLIST(sp->note_tree)->rows < 1)
    {
        return;
    }
    
    title = g_strdup_printf(_("%s print"), PACKAGE);
    p_dialog = gnome_print_dialog_new(title, GNOME_PRINT_DIALOG_RANGE);
    g_free(title);
    gnome_dialog_set_parent(GNOME_DIALOG(p_dialog), yank_root_win(NULL));
    
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    check_print_tree = gtk_check_button_new_with_label(_("Print tree"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_print_tree), TRUE);
    gtk_widget_show(check_print_tree);
    gtk_box_pack_start(GTK_BOX(hbox1), check_print_tree, FALSE, TRUE, 5);
    check_print_notes = gtk_check_button_new_with_label(_("Print notes"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_print_notes), TRUE);
    gtk_widget_show(check_print_notes);
    gtk_box_pack_end(GTK_BOX(hbox1), check_print_notes, TRUE, TRUE, 5);
    gnome_print_dialog_construct_range_custom(GNOME_PRINT_DIALOG(p_dialog),
                                              GTK_WIDGET(hbox1));
    
    gtk_signal_connect(GTK_OBJECT(p_dialog), "clicked",
                       GTK_SIGNAL_FUNC(cb_print_dialog_clicked), NULL);
    gtk_widget_show(p_dialog);
}

/* ------------------------------------------------------ */
/*
 * selection in printer dialog
 */

static void
cb_print_dialog_clicked(GtkWidget *w, gint button, gpointer p)
{
    GnomePrinter            *printer;
    GnomePrintMaster        *gpm;
    GnomePrintMasterPreview *preview;
    gchar                   *buf;

    printer = NULL;
    gpm = gnome_print_master_new();
    if (paper != NULL)
    {
        gnome_print_master_set_paper(gpm, paper);
    }
    
    switch (button)
    {
    case GNOME_PRINT_PRINT:
        printer = gnome_print_dialog_get_printer(GNOME_PRINT_DIALOG(w));
        if (do_print_notes(gpm, printer) == TRUE)
        {
            gnome_print_master_close(gpm);
            gnome_print_context_close(gnome_print_master_get_context(gpm));
            gnome_print_master_print(gpm);
            gnome_dialog_close(GNOME_DIALOG(w));
        }
        break;
    case GNOME_PRINT_PREVIEW:
        if (do_print_notes(gpm, printer) == TRUE)
        {
            gnome_print_master_close(gpm);
            gnome_print_context_close(gnome_print_master_get_context(gpm));
            buf = g_strdup_printf(PACKAGE " - print");
            /* FIXME: close/destroy */
            preview = gnome_print_master_preview_new(gpm, buf);
            gtk_widget_show(GTK_WIDGET(preview));
            g_free(buf);
        }
        break;
    case GNOME_PRINT_CANCEL:
        gnome_dialog_close(GNOME_DIALOG(w));
        break;
    };
}

/* ------------------------------------------------------ */
/*
 * control printing
 */

static gint
do_print_notes(GnomePrintMaster *gpm, GnomePrinter *printer)
{
    float px;
    float py;
    gint  printed_something;
    gchar *buf;
    
    printed_something = FALSE;
    if (printer)
    {
        gnome_print_master_set_printer(gpm, printer);
    }

    px = gnome_paper_lmargin(gnome_print_master_get_paper(gpm));
    py = get_next_y(gpm, 0, 0);
    pages = 1;
    
    buf = g_strdup_printf("%i", pages);
    gnome_print_beginpage(gnome_print_master_get_context(gpm), buf);
    g_free(buf);

    /*
     * just one note with text
     */
    
    if ((g_list_length(GTK_CLIST(sp->note_tree)->selection) == 1) &&
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_print_notes)))
    {
        print_note_text(gpm, gtk_ctree_node_get_row_data(
            GTK_CTREE(sp->note_tree),
            GTK_CTREE_NODE(GTK_CLIST(sp->note_tree)->selection->data)),
                        &px, &py);
        gnome_print_showpage(gnome_print_master_get_context(gpm));
        printed_something = TRUE;

        return (printed_something);
        /* not reached */
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_print_tree)))
    {
        print_tree(gpm, GTK_CTREE(sp->note_tree),
                   GTK_CTREE_NODE(GTK_CLIST(sp->note_tree)->row_list),
                   GTK_CLIST(sp->note_tree)->selection, &px, &py);
        gnome_print_showpage(gnome_print_master_get_context(gpm));
        pages++;
        buf = g_strdup_printf("%i", pages);
        gnome_print_beginpage(gnome_print_master_get_context(gpm), buf);
        g_free(buf);
        printed_something = TRUE;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_print_notes)))
    {
        py = get_next_y(gpm, 0, 0);
        print_notes(gpm, GTK_CTREE(sp->note_tree), &px, &py );
        gnome_print_showpage(gnome_print_master_get_context(gpm));
        pages++;
        buf = g_strdup_printf("%i", pages);
        gnome_print_beginpage(gnome_print_master_get_context(gpm), buf);
        g_free(buf);
        printed_something = TRUE;
    }

    return (printed_something);
}

/* ------------------------------------------------------ */
/*
 * select paper dimensions
 */

void
cb_page_setup(GtkWidget *w, gpointer p)
{
    const gchar *buttons[] =
    {
        GNOME_STOCK_BUTTON_OK,
        GNOME_STOCK_BUTTON_CANCEL,
        NULL
    };
    GtkWidget   *dialog;
    GtkWidget   *paper_sel;
    gchar       *buf;
    gint        button;
    
    buf = g_strdup_printf(_("%s - page setup"), PACKAGE);
    dialog = gnome_dialog_newv(buf, buttons);
    gnome_dialog_set_parent(GNOME_DIALOG(dialog), yank_root_win(NULL));
    gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);

    paper_sel = gnome_paper_selector_new();
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), paper_sel, FALSE,
                       FALSE, 5);
    gtk_widget_show(paper_sel);
    button = gnome_dialog_run(GNOME_DIALOG(dialog));

    if (button == 0)
    {
        gchar *pname;

        pname = gnome_paper_selector_get_name(GNOME_PAPER_SELECTOR(paper_sel));
        if (pname != NULL)
        {
            paper = gnome_paper_with_name(pname);
        }
        else
        {
            g_warning(_("Can't get paper name!\n"));
        }
    }

    if (button != -1)
    {
        gnome_dialog_close(GNOME_DIALOG(dialog));
    }
}

/* ------------------------------------------------------ */
/*
 * print tree of titles (only selected notes if list != NULL)
 */

static void
print_tree(GnomePrintMaster *gpm, GtkCTree *tree, GtkCTreeNode *node,
           GList *list, float *x, float *y)
{
    note_data *data;

    g_return_if_fail(node != NULL);
    
    data = NULL;
    
    if (list != NULL)
    {
        if (g_list_find(GTK_CLIST(tree)->selection, node))
        {
            data = gtk_ctree_node_get_row_data(tree, node);
        }
    }
    else
    {
        data = gtk_ctree_node_get_row_data(tree, node);
    }

    if (data != NULL)
    {
        gnome_print_moveto(gnome_print_master_get_context(gpm), *x, *y);
        print_note_title(gnome_print_master_get_context(gpm), data);
        (*y) = get_next_y(gpm, *y, 14);
    }
    
    if (GTK_CTREE_ROW(node)->children)
    {
        (*x += 20);
        print_tree(gpm, tree, GTK_CTREE_ROW(node)->children, list, x, y);
        (*x -= 20);
    }
    if (GTK_CTREE_ROW(node)->sibling)
    {
        print_tree(gpm, tree, GTK_CTREE_ROW(node)->sibling, list, x, y);
    }
}

/* ------------------------------------------------------ */
/*
 * print a nice formatted title
 */

static void
print_note_title(GnomePrintContext *pc, note_data *note)
{
    GnomeFontWeight font_weight;
    GnomeFont       *font;
    gchar           *buf;

    if (((note->notetype == CheckNote) || (note->notetype == TodoNote)) &&
        (note->todo == 0))
    {
        font_weight = GNOME_FONT_SEMI;
    }
    else
    {
        font_weight = GNOME_FONT_MEDIUM;
    }
    font = gnome_font_new_closest("Courier", font_weight, 0, 14);
    gnome_print_setfont(pc, font);

    if ((note->notetype == TodoNote) && (note->deadline > 0))
    {
        buf = g_strdup_printf("%s   (%s)", note->title,
                              time2isodate(&(note->deadline)));
    }
    else
    {
        buf = g_strdup(note->title);
    }
    gnome_print_show(pc, buf);
    g_free(buf);
}

/* ------------------------------------------------------ */
/*
 * print all/ selected notes 
 */

static void
print_notes(GnomePrintMaster *gpm, GtkCTree *tree, float *px, float *py)
{
    GList     *sel;
    note_data *data;
    gint      row;
    
    sel = GTK_CLIST(tree)->selection;
    if (sel != NULL)
    {
        while (sel)
        {
            data = gtk_ctree_node_get_row_data(tree,
                                               GTK_CTREE_NODE(sel->data));
            print_note_text(gpm, data, px, py);
            sel = g_list_next(sel);
        }
    }
    else
    {
        for (row = 0; row < GTK_CLIST(tree)->rows; row++)
        {
            data = gtk_clist_get_row_data(GTK_CLIST(tree), row);
            print_note_text(gpm, data, px, py);
        }
    }
}

/* ------------------------------------------------------ */

static void
print_note_text(GnomePrintMaster *gpm, note_data *data, float *x, float *y)
{
    GnomeFont *font;
    gchar     *l_start;
    gchar     *l_end;
    gchar     *buf;
    gint      l_len;

    g_return_if_fail(data != NULL);
    
    gnome_print_moveto(gnome_print_master_get_context(gpm), *x, *y);
    print_note_title(gnome_print_master_get_context(gpm), data);
    (*y) = get_next_y(gpm, *y, 14);
    
    font = gnome_font_new_closest("Courier", GNOME_FONT_BOOK, 0, 10);
    gnome_print_setfont(gnome_print_master_get_context(gpm), font);
    
    l_start = data->text;
    while (data->text != NULL &&
           l_start < (data->text + strlen(data->text)))
    {
        l_end = strchr(l_start, '\n');
        if (l_end != NULL)
        {
            l_len = l_end - l_start + 1;
        }
        else
        {
            l_len = strlen(l_start) + 1;
        }
        buf = (gchar *) malloc(l_len);
        strncpy(buf, l_start, (l_len-1));
        buf[(l_len-1)] = '\0';
        
        gnome_print_moveto(gnome_print_master_get_context(gpm), *x, *y);
        gnome_print_show(gnome_print_master_get_context(gpm), buf);

        (*y) = get_next_y(gpm, *y, 12);
        gnome_print_setfont(gnome_print_master_get_context(gpm), font);
        
        free(buf);
        l_start += l_len;
    }
}

/* ------------------------------------------------------ */
/*
 * find next position on page, set current to 0 to start
 */

static float
get_next_y(GnomePrintMaster *gpm, float current, float down)
{
    float            h;
    float            tm;
    float            bm;
    float            ret;
    gchar            *buf;
    const GnomePaper *gpaper;

    gpaper = gnome_print_master_get_paper(gpm);
    h = gnome_paper_psheight(gpaper);
    tm = gnome_paper_tmargin(gpaper);
    bm = gnome_paper_bmargin(gpaper);

    if (current > 0)
    {
        ret = current - down;
        if (ret < bm)
        {
            ret = h - tm;
            gnome_print_showpage(gnome_print_master_get_context(gpm));
            pages++;
            buf = g_strdup_printf("%i", pages);
            gnome_print_beginpage(gnome_print_master_get_context(gpm), buf);
            g_free(buf);
        }
    }
    else
    {
        ret = h - tm;
    }

    return (ret);
}

/* ------------------------------------------------------ */

