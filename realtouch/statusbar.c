#include "statusbar.h"
#include <rtgui/dc.h>

static const rtgui_color_t _status_bar_pixels[] = 
{
	RTGUI_RGB(228,228,228),
	RTGUI_RGB(182,186,192),
	RTGUI_RGB(92,158,200),
	RTGUI_RGB(30,117,176),
	RTGUI_RGB(30,116,175),
	RTGUI_RGB(29,115,174),
	RTGUI_RGB(29,114,173),
	RTGUI_RGB(29,114,172),
	RTGUI_RGB(29,113,171),
	RTGUI_RGB(28,112,170),
	RTGUI_RGB(28,111,170),
	RTGUI_RGB(28,111,169),
	RTGUI_RGB(28,110,168),
	RTGUI_RGB(27,109,167),
	RTGUI_RGB(27,108,166),
	RTGUI_RGB(27,108,165),
	RTGUI_RGB(26,107,164),
	RTGUI_RGB(26,106,163),
	RTGUI_RGB(26,105,163),
	RTGUI_RGB(26,105,162),
	RTGUI_RGB(25,104,161),
	RTGUI_RGB(25,103,160),
	RTGUI_RGB(25,102,159),
	RTGUI_RGB(25,101,158),
	RTGUI_RGB(24,101,157),
	RTGUI_RGB(24,100,156),
	RTGUI_RGB(24,99,156),
	RTGUI_RGB(24,98,155),
	RTGUI_RGB(23,98,154),
	RTGUI_RGB(23,97,153),
	RTGUI_RGB(23,96,153),
	RTGUI_RGB(23,95,152),
	RTGUI_RGB(22,94,150),
	RTGUI_RGB(22,94,149),
	RTGUI_RGB(22,93,148),
	RTGUI_RGB(21,92,147),
	RTGUI_RGB(21,91,146),
	RTGUI_RGB(21,91,145),
	RTGUI_RGB(20,90,143),
	RTGUI_RGB(20,89,142),
	RTGUI_RGB(20,88,141),
	RTGUI_RGB(19,87,139),
	RTGUI_RGB(19,86,138),
	RTGUI_RGB(19,85,136),
	RTGUI_RGB(18,85,138),
	RTGUI_RGB(18,84,137),
	RTGUI_RGB(18,83,137),
	RTGUI_RGB(18,82,136),
	RTGUI_RGB(47,91,135),
	RTGUI_RGB(255,255,255),
};

void dc_draw_bar(struct rtgui_dc* dc, const rtgui_color_t *bar_pixel, struct rtgui_rect *rect, int style)
{
	rt_uint32_t index;
	struct rtgui_gc *gc;
	rtgui_color_t fg;

	gc = rtgui_dc_get_gc(dc);
	fg = gc->foreground;

	if (style == RTGUI_HORIZONTAL)
	{
		/* horizontal */
		for (index = rect->y1; index < rect->y2; index ++)
		{
			gc->foreground = bar_pixel[index - rect->y1];
			rtgui_dc_draw_hline(dc, rect->x1, rect->x2, index);
		}
	}
	else 
	{
		/* vertical */
		for (index = rect->x1; index < rect->x2; index ++)
		{
			gc->foreground = bar_pixel[index - rect->x1];
			rtgui_dc_draw_vline(dc, index, rect->y1, rect->y2);
		}
	}
	gc->foreground = fg;
}

rt_bool_t statusbar_event_handler(struct rtgui_object* object, struct rtgui_event* event)
{
	switch (event->type)
	{
	case RTGUI_EVENT_PAINT:
		{
			struct rtgui_dc *dc;
			struct rtgui_rect rect;

			rtgui_widget_get_rect(RTGUI_WIDGET(object), &rect);

			dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(object));
			dc_draw_bar(dc, _status_bar_pixels, &rect, RTGUI_HORIZONTAL);

			/* dispatch event */
			rtgui_container_dispatch_event(RTGUI_CONTAINER(object), event);

			rtgui_dc_end_drawing(dc);
		}
		break;
	default:
		return rtgui_win_event_handler(object, event);
	}

	return RT_FALSE;
}

void statusbar_init(void)
{
	rtgui_rect_t rect;
	struct rtgui_win* win;
	struct rtgui_label* label;
	struct rtgui_box* box;

	/* get scree rect */
	rtgui_get_screen_rect(&rect);
	rect.y2 = rect.y1 + 50;

	/* create status bar window */
	win = rtgui_win_create(RT_NULL, "StatusBar", &rect, RTGUI_WIN_STYLE_NO_BORDER |
		RTGUI_WIN_STYLE_NO_TITLE | RTGUI_WIN_STYLE_ONTOP);
	rtgui_object_set_event_handler(RTGUI_OBJECT(win), statusbar_event_handler);

	rtgui_get_screen_rect(&rect);
	rect.y1 = 50;
	/* set the rect information of main window */
	rtgui_set_mainwin_rect(&rect);

	box = rtgui_box_create(RTGUI_HORIZONTAL, 3);
	rtgui_container_set_box(RTGUI_CONTAINER(win), box);

	label = rtgui_label_create("12:00");
	RTGUI_WIDGET_ALIGN(label) = RTGUI_ALIGN_CENTER;
	RTGUI_WIDGET_FLAG(label) |= RTGUI_WIDGET_FLAG_TRANSPARENT;
	rtgui_widget_set_miniwidth(RTGUI_WIDGET(label), 64);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

	rtgui_container_layout(RTGUI_CONTAINER(win));

	rtgui_win_show(win, RT_FALSE);	
}
