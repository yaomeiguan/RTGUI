#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>

void app1_entry(void* parameter)
{
	struct rtgui_app* application;
	struct rtgui_win* win;

	application = rtgui_app_create(rt_thread_self(), "ExApp1");
	if (application != RT_NULL)
	{
		rtgui_rect_t rect = {220, 100, 400, 300};
		win = rtgui_win_create(RT_NULL, "Window #1", &rect, RTGUI_WIN_STYLE_DEFAULT);
		rtgui_win_show(win, RT_TRUE);

		rtgui_app_destroy(application);
	}
}

void app2_entry(void* parameter)
{
	struct rtgui_app* application;
	struct rtgui_win* win;

	application = rtgui_app_create(rt_thread_self(), "ExApp2");
	if (application != RT_NULL)
	{
		rtgui_rect_t rect = {220, 250, 400, 450};
		win = rtgui_win_create(RT_NULL, "Window #2", &rect, RTGUI_WIN_STYLE_DEFAULT);
		rtgui_win_show(win, RT_TRUE);

		rtgui_app_destroy(application);
	}
}

void app_init(void)
{
	rt_thread_t tid;

	tid = rt_thread_create("app1", app1_entry, RT_NULL, 2048, 20, 20);
	if (tid != RT_NULL)
		rt_thread_startup(tid);

	tid = rt_thread_create("app2", app2_entry, RT_NULL, 2048, 20, 20);
	if (tid != RT_NULL)
		rt_thread_startup(tid);
}
