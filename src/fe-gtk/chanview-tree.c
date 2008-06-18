/* file included in chanview.c */

#include "gossip-cell-renderer-expander.h"

typedef struct
{
	GtkTreeView *tree;
	GtkWidget *scrollw;	/* scrolledWindow */
	int idle_tag;
	GtkTreeViewColumn *main_col;
} treeview;

#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "fe-gtk.h"
#include "maingui.h"

static void
cv_tree_title_cell_data_func (GtkTreeViewColumn *column,
			      GtkCellRenderer *cell,
			      GtkTreeModel *model,
			      GtkTreeIter *iter,
			      chanview *cv);

static void
cv_tree_indent_cell_data_func (GtkTreeViewColumn *column,
			       GtkCellRenderer *cell,
			       GtkTreeModel *model,
			       GtkTreeIter *iter,
			       chanview *cv);

static void
cv_tree_expander_cell_data_func (GtkTreeViewColumn *column,
				 GtkCellRenderer *cell,
				 GtkTreeModel *model,
				 GtkTreeIter *iter,
				 chanview *cv);

static void 	/* row-activated, when a row is double clicked */
cv_tree_activated_cb (GtkTreeView *view, GtkTreePath *path,
		      GtkTreeViewColumn *column, gpointer data)
{
	if (gtk_tree_view_row_expanded (view, path))
		gtk_tree_view_collapse_row (view, path);
	else
		gtk_tree_view_expand_row (view, path, FALSE);
}

static void		/* row selected callback */
cv_tree_sel_cb (GtkTreeSelection *sel, chanview *cv)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	chan *ch;

	if (gtk_tree_selection_get_selected (sel, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, COL_CHAN, &ch, -1);

		cv->focused = ch;
		cv->cb_focus (cv, ch, ch->tag, ch->userdata);
	}
}

static gboolean
cv_tree_click_cb (GtkTreeView *tree, GdkEventButton *event, chanview *cv)
{
	chan *ch;
	GtkTreeSelection *sel;
	GtkTreePath *path;
	GtkTreeIter iter;
	int ret = FALSE;

	if (event->button != 3 && event->state == 0)
		return FALSE;

	sel = gtk_tree_view_get_selection (tree);
	if (gtk_tree_view_get_path_at_pos (tree, event->x, event->y, &path, 0, 0, 0))
	{
		if (event->button == 2)
		{
			gtk_tree_selection_unselect_all (sel);
			gtk_tree_selection_select_path (sel, path);
		}
		if (gtk_tree_model_get_iter (GTK_TREE_MODEL (cv->store), &iter, path))
		{
			gtk_tree_model_get (GTK_TREE_MODEL (cv->store), &iter, COL_CHAN, &ch, -1);
			ret = cv->cb_contextmenu (cv, ch, ch->tag, ch->userdata, event);
		}
		gtk_tree_path_free (path);
	}
	return ret;
}

static void
cv_tree_cell_set_background (treeview *cv,
			     GtkCellRenderer *cell,
			     gboolean is_group)
{
	GdkColor color;
	GtkStyle *style;

	g_return_if_fail(cv != NULL);
	g_return_if_fail(cell != NULL);

	style = gtk_widget_get_style(GTK_WIDGET(cv->tree));

	if (!is_group)
		g_object_set(cell, "cell-background-gdk", NULL, NULL);
	else
	{
		color = style->text_aa[GTK_STATE_INSENSITIVE];

		color.red = (color.red + (style->white).red) / 2;
		color.green = (color.green + (style->white).green) / 2;
		color.blue = (color.blue + (style->white).blue) / 2;

		g_object_set(cell, "cell-background-gdk", &color, NULL);
	}
}

static void
cv_tree_init (chanview *cv)
{
	GtkWidget *view, *win;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *main_col, *col;

	static const GtkTargetEntry dnd_src_target[] =
	{
		{"XCHAT_CHANVIEW", GTK_TARGET_SAME_APP, 75 }
	};
	static const GtkTargetEntry dnd_dest_target[] =
	{
		{"XCHAT_USERLIST", GTK_TARGET_SAME_APP, 75 }
	};

	win = gtk_scrolled_window_new (0, 0);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (win),
													 GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (win),
											  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (cv->box), win);
	gtk_widget_show (win);

	view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (cv->store));
	gtk_widget_set_name (view, "xchat-tree");
	if (cv->style)
		gtk_widget_set_style (view, cv->style);
	GTK_WIDGET_UNSET_FLAGS (view, GTK_CAN_FOCUS);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
	gtk_container_add (GTK_CONTAINER (win), view);

	col = main_col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	/* icon column */
	if (cv->use_icons)
	{
		renderer = gtk_cell_renderer_pixbuf_new();
		g_object_set(G_OBJECT (renderer), "ypad", 0, NULL);
		gtk_tree_view_column_pack_start(col, renderer, FALSE);
	}

	/* main column */
	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT (renderer), "ypad", 0, "visible", FALSE, NULL);
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc) cv_tree_indent_cell_data_func, cv, NULL);

	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT (renderer), "ypad", 0, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_attributes(col, renderer, "text", COL_NAME, "attributes", COL_ATTR, NULL);
	gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc) cv_tree_title_cell_data_func, cv, NULL);

	/* expander goes at the end of the main column... */
	renderer = gossip_cell_renderer_expander_new();
	g_object_set(G_OBJECT (renderer), "ypad", 0, NULL);
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_set_cell_data_func(col, renderer, (GtkTreeCellDataFunc) cv_tree_expander_cell_data_func, cv, NULL);

	/* disable the GTK+ expander because it's shite... --nenolod */
	g_object_set(GTK_TREE_VIEW(view), "show-expanders", FALSE, NULL);

	g_signal_connect (G_OBJECT (gtk_tree_view_get_selection (GTK_TREE_VIEW (view))),
							"changed", G_CALLBACK (cv_tree_sel_cb), cv);
	g_signal_connect (G_OBJECT (view), "button-press-event",
							G_CALLBACK (cv_tree_click_cb), cv);
	g_signal_connect (G_OBJECT (view), "row-activated",
							G_CALLBACK (cv_tree_activated_cb), NULL);

	gtk_drag_dest_set (view, GTK_DEST_DEFAULT_ALL, dnd_dest_target, 1,
							 GDK_ACTION_MOVE | GDK_ACTION_COPY | GDK_ACTION_LINK);
	gtk_drag_source_set (view, GDK_BUTTON1_MASK, dnd_src_target, 1, GDK_ACTION_COPY);

	g_signal_connect (G_OBJECT (view), "drag_begin",
							G_CALLBACK (mg_drag_begin_cb), NULL);
	g_signal_connect (G_OBJECT (view), "drag_drop",
							G_CALLBACK (mg_drag_drop_cb), NULL);
	g_signal_connect (G_OBJECT (view), "drag_motion",
							G_CALLBACK (mg_drag_motion_cb), NULL);
	g_signal_connect (G_OBJECT (view), "drag_end",
							G_CALLBACK (mg_drag_end_cb), NULL);

	((treeview *)cv)->tree = GTK_TREE_VIEW (view);
	((treeview *)cv)->scrollw = win;
	((treeview *)cv)->idle_tag = 0;
	((treeview *)cv)->main_col = main_col;
	gtk_widget_show (view);
}

static void
cv_tree_title_cell_data_func (GtkTreeViewColumn *column,
			      GtkCellRenderer *cell,
			      GtkTreeModel *model,
			      GtkTreeIter *iter,
			      chanview *cv)
{
	gint depth = gtk_tree_store_iter_depth(GTK_TREE_STORE(cv->store), iter);

	if (depth == 0)
		g_object_set(cell, "weight", PANGO_WEIGHT_BOLD, NULL);
	else
		g_object_set(cell, "weight", PANGO_WEIGHT_NORMAL, NULL);

	cv_tree_cell_set_background(((treeview *)cv), cell, depth == 0);
}

static void
cv_tree_indent_cell_data_func (GtkTreeViewColumn *column,
	 		       GtkCellRenderer *cell,
			       GtkTreeModel *model,
			       GtkTreeIter *iter,
			       chanview *cv)
{
	gint depth = gtk_tree_store_iter_depth(GTK_TREE_STORE(cv->store), iter);

	g_object_set(cell, "text", "   ", "visible", depth >= 1, NULL);

	cv_tree_cell_set_background(((treeview *)cv), cell, depth == 0);
}

static void
cv_tree_expander_cell_data_func (GtkTreeViewColumn *column,
				 GtkCellRenderer *cell,
				 GtkTreeModel *model,
				 GtkTreeIter *iter,
				 chanview *cv)
{
	if (gtk_tree_store_iter_depth(GTK_TREE_STORE(cv->store), iter) == 0)
	{
		GtkTreePath *path;
		gboolean row_expanded;

		path = gtk_tree_model_get_path(model, iter);
		row_expanded = gtk_tree_view_row_expanded(GTK_TREE_VIEW(column->tree_view), path);
		gtk_tree_path_free(path);

		g_object_set(cell, "visible", TRUE, "expander-style", row_expanded ? GTK_EXPANDER_EXPANDED : GTK_EXPANDER_COLLAPSED, NULL);

		cv_tree_cell_set_background(((treeview *)cv), cell, TRUE);
	}
	else
		g_object_set(cell, "visible", FALSE, NULL);
}

static void
cv_tree_postinit (chanview *cv)
{
	gtk_tree_view_expand_all (((treeview *)cv)->tree);
}

static void *
cv_tree_add (chanview *cv, chan *ch, char *name, GtkTreeIter *parent)
{
	GtkTreePath *path;

	if (parent)
	{
		/* expand the parent node */
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (cv->store), parent);
		if (path)
		{
			gtk_tree_view_expand_row (((treeview *)cv)->tree, path, FALSE);
			gtk_tree_path_free (path);
		}
	}

	return NULL;
}

static void
cv_tree_change_orientation (chanview *cv)
{
}

static void
cv_tree_focus (chan *ch)
{
	GtkTreeView *tree = ((treeview *)ch->cv)->tree;
	GtkTreeModel *model = gtk_tree_view_get_model (tree);
	GtkTreePath *path;
	GtkTreeIter parent;

	/* expand the parent node */
	if (gtk_tree_model_iter_parent (model, &parent, &ch->iter))
	{
		path = gtk_tree_model_get_path (model, &parent);
		if (path)
		{
			/*if (!gtk_tree_view_row_expanded (tree, path))
			{
				gtk_tree_path_free (path);
				return;
			}*/
			gtk_tree_view_expand_row (tree, path, FALSE);
			gtk_tree_path_free (path);
		}
	}

	path = gtk_tree_model_get_path (model, &ch->iter);
	if (path)
	{
		gtk_tree_view_scroll_to_cell (tree, path, NULL, TRUE, 0.5, 0.5);
		gtk_tree_view_set_cursor (tree, path, NULL, FALSE);
		gtk_tree_path_free (path);
	}
}

static void
cv_tree_move_focus (chanview *cv, gboolean relative, int num)
{
	chan *ch;

	if (relative)
	{
		num += cv_find_number_of_chan (cv, cv->focused);
		num %= cv->size;
		/* make it wrap around at both ends */
		if (num < 0)
			num = cv->size - 1;
	}

	ch = cv_find_chan_by_number (cv, num);
	if (ch)
		cv_tree_focus (ch);
}

/*static gboolean
cv_timeout (chanview *cv)
{
	int colnum = cv->use_icons ? 1 : 0;
	GtkTreeViewColumn *col;

	col = gtk_tree_view_get_column (GTK_TREE_VIEW (((treeview *)cv)->tree), colnum);
	gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_GROW_ONLY);

	((treeview *)cv)->idle_tag = 0;
	return FALSE;
}*/

static void
cv_tree_remove (chan *ch)
{
/*	chanview *cv = ch->cv;
	int colnum = cv->use_icons ? 1 : 0;
	GtkTreeViewColumn *col = gtk_tree_view_get_column (GTK_TREE_VIEW (((treeview *)cv)->tree), colnum);
	gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	if (((treeview *)cv)->idle_tag == 0)
		((treeview *)cv)->idle_tag = g_idle_add ((GSourceFunc)cv_timeout, cv);*/
}

static void
move_row (chan *ch, int delta, GtkTreeIter *parent)
{
	GtkTreeStore *store = ch->cv->store;
	GtkTreeIter *src = &ch->iter;
	GtkTreeIter dest = ch->iter;
	GtkTreePath *dest_path;

	if (delta < 0) /* down */
	{
		if (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &dest))
			gtk_tree_store_swap (store, src, &dest);
		else	/* move to top */
			gtk_tree_store_move_after (store, src, NULL);

	} else
	{
		dest_path = gtk_tree_model_get_path (GTK_TREE_MODEL (store), &dest);
		if (gtk_tree_path_prev (dest_path))
		{
			gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &dest, dest_path);
			gtk_tree_store_swap (store, src, &dest);
		} else
		{	/* move to bottom */
			gtk_tree_store_move_before (store, src, NULL);
		}

		gtk_tree_path_free (dest_path);
	}
}

static void
cv_tree_move (chan *ch, int delta)
{
	GtkTreeIter parent;

	/* do nothing if this is a server row */
	if (gtk_tree_model_iter_parent (GTK_TREE_MODEL (ch->cv->store), &parent, &ch->iter))
		move_row (ch, delta, &parent);
}

static void
cv_tree_move_family (chan *ch, int delta)
{
	move_row (ch, delta, NULL);
}

static void
cv_tree_cleanup (chanview *cv)
{
	if (((treeview *)cv)->idle_tag)
	{
		g_source_remove (((treeview *)cv)->idle_tag);
		((treeview *)cv)->idle_tag = 0;
	}

	if (cv->box)
		/* kill the scrolled window */
		gtk_widget_destroy (((treeview *)cv)->scrollw);
}

static void
cv_tree_set_color (chan *ch, PangoAttrList *list)
{
	/* nothing to do, it's already set in the store */
}

static void
cv_tree_rename (chan *ch, char *name)
{
	/* nothing to do, it's already renamed in the store */
}

static chan *
cv_tree_get_parent (chan *ch)
{
	chan *parent_ch = NULL;
	GtkTreeIter parent;

	if (gtk_tree_model_iter_parent (GTK_TREE_MODEL (ch->cv->store), &parent, &ch->iter))
	{
		gtk_tree_model_get (GTK_TREE_MODEL (ch->cv->store), &parent, COL_CHAN, &parent_ch, -1);
	}

	return parent_ch;
}

static gboolean
cv_tree_is_collapsed (chan *ch)
{
	chan *parent = cv_tree_get_parent (ch);
	GtkTreePath *path = NULL;
	gboolean ret;

	if (parent == NULL)
		return FALSE;

	path = gtk_tree_model_get_path (GTK_TREE_MODEL (parent->cv->store),
											  &parent->iter);
	ret = !gtk_tree_view_row_expanded (((treeview *)parent->cv)->tree, path);
	gtk_tree_path_free (path);
	
	return ret;
}
