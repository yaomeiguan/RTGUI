#include "appmgr.h"
#include <rtgui/widgets/iconbox.h>

#include "customer_list.h"

#include "xpm/home.xpm"
#include "xpm/home_gray.xpm"

/* application manager */
struct rtgui_application_item
{
	rtgui_list_t list;

	struct rtgui_app* app;
};
static rtgui_list_t _applications_list;

struct rtgui_customer_item* app_items = RT_NULL;
rt_uint16_t app_count = 0;
struct rtgui_customer_list* app_list;

static void _handle_app_create(struct rtgui_event_application* event)
{
	rt_uint32_t index;
	rt_int32_t status;
	struct rtgui_app* app;

	status = RTGUI_STATUS_OK;
	for (index = 0; index < app_count; index ++)
	{
		app = (struct rtgui_app*)app_items[index].user_data;
		if (app == event->app)
		{
			/* application is created already */
			status = RTGUI_STATUS_ERROR;
			goto __exit;
		}
	}

	app_count += 1;
	if (app_items == RT_NULL)
		app_items = (struct rtgui_customer_item*) rtgui_malloc(sizeof(struct rtgui_customer_item));
	else
		app_items = (struct rtgui_customer_item*) rtgui_realloc(app_items, sizeof(struct rtgui_customer_item) * app_count);

	if (app_items == RT_NULL) 
	{
		status = RTGUI_STATUS_ERROR;
		goto __exit;
	}

	app = event->app;

	app_items[app_count - 1].name = rt_strdup(app->name);
	app_items[app_count - 1].image = RT_NULL;
	app_items[app_count - 1].user_data = (rt_uint32_t)app;
	rtgui_customer_list_set_items(app_list, app_items, app_count);

__exit:
	/* send ack to the application */
	rtgui_ack(RTGUI_EVENT(event), status);
	return;
}

static void _handle_app_destroy(struct rtgui_event_application* event)
{
	rt_uint32_t index;
	struct rtgui_app* app;

	for (index = 0; index < app_count; index ++)
	{
		app = (struct rtgui_app*)app_items[index].user_data;
		if (app == event->app)
		{
			/* remove this application */
			app_count --;
			if (app_count == 0) 
			{
				rtgui_free(app_items);
				app_items = RT_NULL;
			}
			else if (index == app_count)
			{
				app_items = rtgui_realloc(app_items, app_count * sizeof(struct rtgui_customer_item));
			}
			else
			{
				rt_uint32_t j;
				for (j = index; j < app_count; j ++)
				{
					app_items[j] = app_items[j + 1];
				}
				app_items = rtgui_realloc(app_items, app_count * sizeof(struct rtgui_customer_item));
			}
			rtgui_customer_list_set_items(app_list, app_items, app_count);
			rtgui_ack(RTGUI_EVENT(event), RTGUI_STATUS_OK);
			return ;
		}
	}

	/* send ack to the application */
	rtgui_ack(RTGUI_EVENT(event), RTGUI_STATUS_ERROR);
	return;
}

static void _handle_app_activate(struct rtgui_widget* widget, const struct rtgui_customer_item* item)
{
	struct rtgui_app* app;
	struct rtgui_event_application event;

	app = (struct rtgui_app*)item->user_data;

	RTGUI_EVENT_APP_ACTIVATE_INIT(&event);
	event.app = app;

	rtgui_send(app->tid, RTGUI_EVENT(&event), sizeof(struct rtgui_event_application));
}

rt_bool_t event_handler(struct rtgui_object* object, rtgui_event_t* event)
{
	rt_bool_t result;

	RT_ASSERT(object != RT_NULL);
	RT_ASSERT(event  != RT_NULL);

	result = RT_TRUE;
	switch (event->type)
	{
	case RTGUI_EVENT_APP_CREATE:
		_handle_app_create((struct rtgui_event_application*) event);
		break;

	case RTGUI_EVENT_APP_DESTROY:
		_handle_app_destroy((struct rtgui_event_application*) event);
		break;

	default:
		/* invoke parent event handler */
		result = rtgui_app_event_handler(object, event);
		break;
	}

	return result;
}

void dc_render_block(struct rtgui_dc* dc, struct rtgui_rect *rect, int angle_y)
{
#define BLOCK_BORDER_SZ	5
#define BLOCK_ANGLE_SZ	10
	int rect_y[8];
	int rect_x[8];
	rtgui_color_t line_color = RTGUI_RGB(215, 215, 215);
	rtgui_color_t external_color = RTGUI_RGB(241, 241, 241);
	rtgui_color_t angle_color = RTGUI_RGB(229, 229, 229);
	rtgui_color_t fg, bg;
	struct rtgui_gc *gc;

	rect_x[0] = rect->x1 + BLOCK_ANGLE_SZ ; rect_y[0] = rect->y1 + BLOCK_BORDER_SZ;
	rect_x[1] = rect->x2 - BLOCK_BORDER_SZ; rect_y[1] = rect->y1 + BLOCK_BORDER_SZ;
	rect_x[2] = rect->x2 - BLOCK_BORDER_SZ; rect_y[2] = rect->y2 - BLOCK_BORDER_SZ;
	rect_x[3] = rect->x1 + BLOCK_ANGLE_SZ ; rect_y[3] = rect->y2 - BLOCK_BORDER_SZ;
	rect_x[4] = rect->x1 + BLOCK_ANGLE_SZ ; rect_y[4] = angle_y + BLOCK_ANGLE_SZ;
	rect_x[5] = rect->x1; rect_y[5] = angle_y;
	rect_x[6] = rect->x1 + BLOCK_ANGLE_SZ ; rect_y[6] = angle_y - BLOCK_ANGLE_SZ;
	rect_x[7] = rect->x1 + BLOCK_ANGLE_SZ ; rect_y[7] = rect->y1 + BLOCK_BORDER_SZ;

	gc = rtgui_dc_get_gc(dc);
	fg = gc->foreground;
	bg = gc->background;
	gc->foreground = line_color;
	rtgui_dc_draw_polygon(dc, rect_x, rect_y, 8);
	gc->foreground = white;
	// rtgui_dc_fill_polygon(dc, rect_x, rect_y, 8);
	gc->foreground = fg;
	gc->background = bg;
}

rt_bool_t app_mgr_panel_event_handler(struct rtgui_object* object, struct rtgui_event* event)
{
	if (event->type == RTGUI_EVENT_PAINT)
	{
		struct rtgui_dc* dc;
		struct rtgui_rect rect;
		int angle_y;

		rtgui_widget_get_rect(RTGUI_WIDGET(object), &rect);
		dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(object));

		rtgui_dc_fill_rect(dc, &rect);
		angle_y = rect.y1 + 100;
		dc_render_block(dc, &rect, angle_y);
		rtgui_dc_end_drawing(dc);
	}
	else
	{
		return rtgui_panel_event_handler(object, event);
	}

	return RT_FALSE;
}

void app_mgr_win_init(void)
{
	struct rtgui_win* win;
	rtgui_rect_t rect;
	struct rtgui_panel* panel;
	struct rtgui_iconbox* iconbox;
	struct rtgui_image* image;

	/* create AppMgr window */
	win = rtgui_mainwin_create(RT_NULL, "AppMgr", RTGUI_WIN_STYLE_MAINWIN);
	RTGUI_WIDGET_BACKGROUND(win) = RTGUI_RGB(241, 241, 241);

	/* get the rect of main window */
	rtgui_widget_get_extent(RTGUI_WIDGET(win), &rect);
	rect.x1 += 3; rect.y1 += 3;
	rect.x2 = rect.x1 + 50;
	rect.y2 = rect.y1 + 50;
	image = rtgui_image_create_from_mem("xpm", (const rt_uint8_t*)home_gray_xpm, sizeof(home_xpm), RT_FALSE);
	iconbox = rtgui_iconbox_create(image, "OK", RTGUI_ICONBOX_TEXT_BELOW);

	rtgui_widget_set_rect(RTGUI_WIDGET(iconbox), &rect);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(iconbox));

	/* get the rect of main window */
	rtgui_widget_get_extent(RTGUI_WIDGET(win), &rect);

	/* create application list */
	rect.x1 += 3; rect.y1 += 3 + 50;
	rect.x2 = rect.x1 + 200; 
	rect.y2 = rect.y2 - 3;
	app_list = rtgui_customer_list_create(app_items, app_count, &rect, RTGUI_CUSTOMER_LIST_LIST);
	rtgui_customer_list_set_on_action(app_list, _handle_app_activate);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(app_list));

	/* create application panel */
	rtgui_widget_get_extent(RTGUI_WIDGET(win), &rect);
	rect.x1 += 210; rect.y1 += 10;
	panel = rtgui_panel_create(RTGUI_BORDER_NONE);
	RTGUI_WIDGET_BACKGROUND(panel) = RTGUI_RGB(241, 241, 241);
	rtgui_widget_set_rect(RTGUI_WIDGET(panel), &rect);
	rtgui_object_set_event_handler(RTGUI_OBJECT(panel), app_mgr_panel_event_handler);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(panel));

	rtgui_win_show(win, RT_FALSE);
}

void app_mgr_entry(void* parameter)
{
	struct rtgui_app* application;

	application = rtgui_app_create(rt_thread_self(), "AppMgr");
	if (application != RT_NULL)
	{
		/* set as window manager */
		rtgui_app_set_as_wm();

		/* initialize status bar */
		statusbar_init();
		app_mgr_win_init();

		/* set our event handler */
		rtgui_object_set_event_handler(RTGUI_OBJECT(application), 
			event_handler);
		rtgui_app_run(application);
		rtgui_app_destroy(application);
	}
}

void app_mgr_init(void)
{
	rt_thread_t tid;

	/* initialize applications list */
	rtgui_list_init(&_applications_list);

	tid = rt_thread_create("app_mgr", app_mgr_entry, RT_NULL, 4096, 20, 20);
	if (tid != RT_NULL)
		rt_thread_startup(tid);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
void list_apps(void)
{
	rt_uint32_t index;
	struct rtgui_app* app;

	rt_kprintf("GUI Applications:\n");
	rt_kprintf("=================\n");

	for (index = 0; index < app_count; index ++)
	{
		app = (struct rtgui_app*) app_items[index].user_data;
		rt_kprintf("%s\n", app->name);
	}
}
FINSH_FUNCTION_EXPORT(list_apps,show the application list);
#endif
