#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>
#include "customer_list.h"
#include <rtgui/widgets/box.h>

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

rt_bool_t statusbar_event_handler(struct rtgui_object* object, struct rtgui_event* event)
{
	switch (event->type)
	{
	default:
		break;
	}

	return RT_FALSE;
}

void statusbar_init(void)
{
	rtgui_rect_t rect;
	struct rtgui_win* win;
	struct rtgui_label* label;
	struct rtgui_box* box;

	/* create status bar window */
	rect.x1 = rect.y1 = 0;
	rect.x2 = rect.x1 + 800;
	rect.y2 = rect.y1 + 25;
	win = rtgui_win_create(RT_NULL, "StatusBar", &rect, RTGUI_WIN_STYLE_NO_BORDER |
		RTGUI_WIN_STYLE_NO_TITLE |
		RTGUI_WIN_STYLE_ONTOP);

	box = rtgui_box_create(RTGUI_HORIZONTAL, 3);
	rtgui_container_set_box(RTGUI_CONTAINER(win), box);

	label = rtgui_label_create("12:00");
	RTGUI_WIDGET_ALIGN(label) = RTGUI_ALIGN_CENTER_VERTICAL | RTGUI_ALIGN_RIGHT;
	rtgui_widget_set_miniheight(RTGUI_WIDGET(label), 25);
	rtgui_widget_set_miniwidth(RTGUI_WIDGET(label), 64);

	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

	label = rtgui_label_create("166:00:00");
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

	rtgui_container_layout(RTGUI_CONTAINER(win));

	rtgui_win_show(win, RT_FALSE);	
}

void wm_entry(void* parameter)
{
	struct rtgui_win* win;
	struct rtgui_app* application;

	application = rtgui_app_create(rt_thread_self(), "MyName");
	if (application != RT_NULL)
	{
		rtgui_rect_t rect;

		rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);

		/* set as window manager */
		rtgui_app_set_as_wm();

		win = rtgui_win_create(RT_NULL, "Desktop", &rect, 
			RTGUI_WIN_STYLE_NO_TITLE |
			RTGUI_WIN_STYLE_NO_BORDER);

		/* create application list */
		rect.x1 = 3; rect.y1 = 3;
		rect.x2 = rect.x1 + 200; 
		rect.y2 = rect.y1 + 380;
		app_list = rtgui_customer_list_create(app_items, app_count, &rect, RTGUI_CUSTOMER_LIST_LIST);
		rtgui_customer_list_set_on_action(app_list, _handle_app_activate);
		rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(app_list));

		rtgui_win_show(win, RT_FALSE);

		/* initialize status bar */
		statusbar_init();

		/* set our event handler */
		rtgui_object_set_event_handler(RTGUI_OBJECT(application), 
			event_handler);
		rtgui_app_run(application);
		rtgui_app_destroy(application);
	}
}

void wm_init(void)
{
	rt_thread_t tid;

	/* initialize applications list */
	rtgui_list_init(&_applications_list);

	tid = rt_thread_create("wm", wm_entry, RT_NULL, 4096, 20, 20);
	if (tid != RT_NULL)
		rt_thread_startup(tid);
}

int rt_application_init()
{
	rt_device_t device;
	struct rt_device_rect_info info;

	device = rt_device_find("sdl");
	if (device != RT_NULL)
	{
		info.width = 800;
		info.height = 480;

		/* set graphic resolution */
		rt_device_control(device, RTGRAPHIC_CTRL_SET_MODE, &info);
	}
	/* re-set graphic device */
	rtgui_graphic_set_device(device);

	wm_init();
	rt_thread_delay(10);
	app_init();

	return 0;
}

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
