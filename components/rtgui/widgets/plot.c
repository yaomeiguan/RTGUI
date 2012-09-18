/*
 * File      : plot.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-09-03     Grissiom     first version
 */

#include <rtgui/rtgui_system.h>
#include <rtgui/dc.h>
#include <rtgui/widgets/plot.h>
#include <rtgui/widgets/plot_curve.h>

static void _rtgui_plot_constructor(struct rtgui_plot *plot)
{
    /* init widget and set event handler */
    plot->base_point.x = plot->base_point.y = 0;
    rtgui_object_set_event_handler(RTGUI_OBJECT(plot), rtgui_plot_event_handler);
}

static void _rtgui_plot_destructor(struct rtgui_plot *plot)
{
}

DEFINE_CLASS_TYPE(plot, "plot",
                  RTGUI_MV_VIEW_TYPE,
                  _rtgui_plot_constructor,
                  _rtgui_plot_destructor,
                  sizeof(struct rtgui_plot));

struct rtgui_plot *rtgui_plot_create(void)
{
    struct rtgui_plot *plot;

    plot = RTGUI_PLOT(rtgui_widget_create(RTGUI_PLOT_TYPE));

    return plot;
}
RTM_EXPORT(rtgui_plot_create);

void rtgui_plot_destroy(struct rtgui_plot *plot)
{
    rtgui_mv_view_destroy(RTGUI_MV_VIEW(plot));
}
RTM_EXPORT(rtgui_plot_destroy);

void rtgui_plot_set_base_point(struct rtgui_plot *plot, rt_uint16_t x, rt_uint16_t y)
{
    plot->base_point.x = x;
    plot->base_point.y = y;
}
RTM_EXPORT(rtgui_plot_set_base_point);

static void _rtgui_plot_curve_onpaint(
        struct rtgui_dc *dc,
        struct rtgui_plot_curve *curve,
        struct rtgui_point base)
{
    struct rtgui_rect rect;
    rt_uint16_t height;
    int last_x, last_y;
    rtgui_color_t old_color;
    rtgui_plot_curve_dtype *x_data, *y_data;

    rtgui_dc_get_rect(dc, &rect);
    height = rtgui_rect_height(rect);

    old_color = RTGUI_DC_FC(dc);
    RTGUI_DC_FC(dc) = curve->color;

    x_data = rtgui_plot_curve_get_x(curve);
    y_data = rtgui_plot_curve_get_y(curve);
    if (x_data)
    {
        rt_size_t i;

        last_x = x_data[0] + base.x;
        last_y = height - y_data[0] - base.y;
        for (i = 1; i < RTGUI_MV_MODEL(curve)->length; i++)
        {
            int cur_x = x_data[i] + base.x;
            int cur_y = height - y_data[i] - base.y;
            rtgui_dc_draw_line(dc,
                               last_x, last_y,
                               cur_x, cur_y);
            last_x = cur_x;
            last_y = cur_y;
        }
    }
    else
    {
        rt_size_t i;

        last_x = 0 + base.x;
        last_y = height - y_data[0] - base.y;
        for (i = 1; i < RTGUI_MV_MODEL(curve)->length; i++)
        {
            int cur_x = i + base.x;
            int cur_y = height - y_data[i] - base.y;
            rtgui_dc_draw_line(dc,
                               last_x, last_y,
                               cur_x, cur_y);
            last_x = cur_x;
            last_y = cur_y;
        }
    }
    RTGUI_DC_FC(dc) = old_color;
}

static void _rtgui_plot_onpaint(struct rtgui_object *object, struct rtgui_event *event)
{
    int i;
    struct rtgui_dc *dc;
    struct rtgui_rect rect;
    struct rtgui_plot *plot = RTGUI_PLOT(object);

    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(plot));
    if (dc == RT_NULL)
        return;
    rtgui_widget_get_rect(RTGUI_WIDGET(object), &rect);

    rtgui_dc_fill_rect(dc, &rect);

    if (RTGUI_MV_VIEW(plot)->model_number == 1)
    {
        _rtgui_plot_curve_onpaint(dc, RTGUI_MV_VIEW(plot)->model, plot->base_point);
    }
    else
    {
        void **curve_array = (void **)RTGUI_MV_VIEW(object)->model;
        for (i = 0; i < RTGUI_MV_VIEW(object)->model_number; i++)
        {
            _rtgui_plot_curve_onpaint(dc, curve_array[i], plot->base_point);
        }
    }

    rtgui_dc_end_drawing(dc);
}

rt_bool_t rtgui_plot_event_handler(struct rtgui_object *object, struct rtgui_event *event)
{
    RTGUI_WIDGET_EVENT_HANDLER_PREPARE;

    switch (event->type)
    {
    case RTGUI_EVENT_PAINT:
        _rtgui_plot_onpaint(object, event);
        break;
    case RTGUI_EVENT_MV_MODEL:
        /* not optimized */
        _rtgui_plot_onpaint(object, event);
        break;
    default:
        return rtgui_widget_event_handler(object, event);
    }

    return RT_FALSE;
}
RTM_EXPORT(rtgui_plot_event_handler);

