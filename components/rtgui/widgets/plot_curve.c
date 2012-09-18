/*
 * File      : plot.h
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

#include <rtgui/color.h>
#include <rtgui/rtgui_mv_model.h>
#include <rtgui/widgets/widget.h>
#include <rtgui/widgets/plot_curve.h>

static void _rtgui_plot_curve_constructor(struct rtgui_plot_curve *curve)
{
    curve->color = red;

    /* init widget and set event handler */
    rtgui_object_set_event_handler(RTGUI_OBJECT(curve), RT_NULL);
}

static void _rtgui_plot_curve_destructor(struct rtgui_plot_curve *curve)
{
    /* nothing to do so far. */
}

DEFINE_CLASS_TYPE(plot_curve, "plot_curve",
                  RTGUI_MV_MODEL_TYPE,
                  _rtgui_plot_curve_constructor,
                  _rtgui_plot_curve_destructor,
                  sizeof(struct rtgui_plot_curve));

struct rtgui_plot_curve *rtgui_plot_curve_create(void)
{
    return (struct rtgui_plot_curve*)rtgui_mv_model_create(2);
}
RTM_EXPORT(rtgui_plot_curve_create);

void rtgui_plot_curve_destroy(struct rtgui_plot_curve *curve)
{
    rtgui_mv_model_destroy(RTGUI_MV_MODEL(curve));
}
RTM_EXPORT(rtgui_plot_curve_destroy);

void rtgui_plot_curve_set_x(struct rtgui_plot_curve *curve, void *p)
{
    rtgui_mv_model_set_data(RTGUI_MV_MODEL(curve), 0, p);
}
RTM_EXPORT(rtgui_plot_curve_set_x);

void* rtgui_plot_curve_get_x(struct rtgui_plot_curve *curve)
{
    return rtgui_mv_model_get_data(RTGUI_MV_MODEL(curve), 0);
}
RTM_EXPORT(rtgui_plot_curve_get_x);

void rtgui_plot_curve_set_y(struct rtgui_plot_curve *curve, void *p)
{
    rtgui_mv_model_set_data(RTGUI_MV_MODEL(curve), 1, p);
}
RTM_EXPORT(rtgui_plot_curve_set_y);

void* rtgui_plot_curve_get_y(struct rtgui_plot_curve *curve)
{
    return rtgui_mv_model_get_data(RTGUI_MV_MODEL(curve), 1);
}
RTM_EXPORT(rtgui_plot_curve_get_y);

