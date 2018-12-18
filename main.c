/*
 * main.c
 *
 *  Created on: 2014/02/27
 *      Author: kyama
 */

#include <glibtop.h>
#include <glibtop/cpu.h>
#include <gtk/gtk.h>

#include "fifo.h"

typedef struct widgetCpu {
  GtkWidget **label;
  GtkWidget **graph;
} WidgetCpu;

static guint timer_id;
int g_iCpuCount;

int get_cpu_count() {
  glibtop_cpu cpu;
  int count = 0;

  glibtop_get_cpu(&cpu);
  for (int i = 0; i < GLIBTOP_NCPU; i++) {
    if (cpu.xcpu_total[i] > 0)
      count++;
    else
      break;
  }

  return count;
}

gboolean cb_timer_event(gpointer event_arg) {
  static glibtop_cpu cpu_prev;
  static glibtop_cpu cpu_current;

  WidgetCpu *widgetCpu = (WidgetCpu *)event_arg;

  GtkWidget **widget = widgetCpu->label;
  float total, idle, usage, usage_percent;
  char buffer[10];

  cpu_prev = cpu_current;
  glibtop_get_cpu(&cpu_current);

  if (cpu_prev.xcpu_total[0] <= 0)
    return TRUE;

  // total
  total = (cpu_current.total - cpu_prev.total);
  idle = (cpu_current.idle - cpu_prev.idle) / total;
  usage = 1 - idle;
  usage_percent = usage * 100;
  sprintf(buffer, "%05.1f %%", usage_percent);
  gtk_label_set_text(GTK_LABEL(widget[0]), buffer);

  fifo_add(0, usage_percent);

  // each
  for (int i = 0; i < g_iCpuCount; i++) {
    if (cpu_prev.xcpu_total[i] > 0) {
      total = (cpu_current.xcpu_total[i] - cpu_prev.xcpu_total[i]);
      idle = (cpu_current.xcpu_idle[i] - cpu_prev.xcpu_idle[i]) / total;
      usage = 1 - idle;
      usage_percent = usage * 100;
      sprintf(buffer, "%05.1f %%", usage_percent);
      gtk_label_set_text(GTK_LABEL(widget[i + 1]), buffer);

      fifo_add(i + 1, usage_percent);
    }
  }

  // widget redraw
  gtk_widget_queue_draw(widgetCpu->graph[0]);
  gtk_widget_queue_draw(widgetCpu->graph[1]);
  gtk_widget_queue_draw(widgetCpu->graph[2]);
  gtk_widget_queue_draw(widgetCpu->graph[3]);
  gtk_widget_queue_draw(widgetCpu->graph[4]);

  // for continue
  return TRUE;
}

gboolean cb_expose_event(GtkWidget *widget, GdkEventExpose *event,
                         gpointer user_data) {
  GdkWindow *drawable = widget->window;
  cairo_t *cr;
  int fifoId = GPOINTER_TO_INT(user_data);

  //// drawing
  cr = gdk_cairo_create(drawable);

  // background
  cairo_rectangle(cr, 0.0, 0.0, widget->allocation.width,
                  widget->allocation.height);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_fill(cr);

  // graph
  double graph_line_width = 2.0;
  cairo_set_line_width(cr, graph_line_width);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

  switch (fifoId) {
  case 0: {
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
    break;
  }
  case 1: {
    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
    break;
  }
  case 2: {
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
    break;
  }
  case 3: {
    cairo_set_source_rgb(cr, 0.0, 1.0, 1.0);
    break;
  }
  case 4: {
    cairo_set_source_rgb(cr, 1.0, 0.0, 1.0);
    break;
  }
  case 5: {
    cairo_set_source_rgb(cr, 0.5, 0.0, 0.0);
    break;
  }
  case 6: {
    cairo_set_source_rgb(cr, 0.0, 0.5, 0.0);
    break;
  }
  case 7: {
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.5);
    break;
  }
  case 8: {
    cairo_set_source_rgb(cr, 0.5, 0.0, 0.5);
    break;
  }
  case 9: {
    cairo_set_source_rgb(cr, 0.0, 0.5, 0.5);
    break;
  }
  default: {
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    break;
  }
  }

  double *data_array;
  int data_array_size;
  const float max_value = 100;
  fifo_get(fifoId, &data_array, &data_array_size);

  double graph_width_step =
      (double)widget->allocation.width / ((double)data_array_size - 1);
  double graph_x_prev = 0;
  double graph_x_new = 0;
  double graph_y_prev = -1;
  double graph_y_new = -1;
  double scale = (double)widget->allocation.height / (max_value + graph_line_width);

  graph_y_new = (max_value - data_array[0]) * scale;
  for (int i = 1; i < data_array_size; i++) {
    // x
    graph_x_prev = graph_x_new;
    graph_x_new += graph_width_step;

    // y
    graph_y_prev = graph_y_new;
    graph_y_new = scale + (max_value - data_array[i]) * scale;

    // draw
    if (data_array[i] >= 0) {
      cairo_move_to(cr, graph_x_prev, graph_y_prev);
      cairo_line_to(cr, graph_x_new, graph_y_new);
      cairo_stroke(cr);
    }
  }

  cairo_destroy(cr);

  return FALSE;
}

int main(int argc, char **argv) {
  const int SECOND = 60;
  const int RESOLUTION = 2;

  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox_cpu[256];
  GtkWidget *label_cpu[256];
  GtkWidget *graph_cpu[256];

  gtk_init(&argc, &argv);
  glibtop_init();

  // number of cpus
  g_iCpuCount = get_cpu_count();

  // fifo for cpus
  fifo_init(g_iCpuCount + 1, SECOND * RESOLUTION);

  // window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
  gtk_window_set_title(GTK_WINDOW(window), "CPU Monitor");
  gtk_widget_set_size_request(window, 400, 250);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  // vbox
  vbox = gtk_vbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // cpus
  char name[6];
  for (int i = 0; i < g_iCpuCount + 1; i++) {
    if (i == 0)
      sprintf(name, "TOTAL");
    else
      sprintf(name, "CPU%02d", i);
    hbox_cpu[i] = gtk_hbox_new(FALSE, 0);
    label_cpu[i] = gtk_label_new("000.0 %");
    graph_cpu[i] = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), hbox_cpu[i], TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_cpu[i]), gtk_label_new(name), FALSE, FALSE,
                       10);
    gtk_box_pack_start(GTK_BOX(hbox_cpu[i]), graph_cpu[i], TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_cpu[i]), label_cpu[i], FALSE, FALSE, 10);
    g_signal_connect(G_OBJECT(graph_cpu[i]), "expose-event",
                     G_CALLBACK(cb_expose_event), GINT_TO_POINTER(i));
  }

  WidgetCpu widgetCpu;
  widgetCpu.label = label_cpu;
  widgetCpu.graph = graph_cpu;

  // timer
  timer_id =
      g_timeout_add(1000 / RESOLUTION, (GSourceFunc)cb_timer_event, &widgetCpu);

  gtk_widget_show_all(window);
  gtk_main();
  fifo_finalize();

  return 0;
}
