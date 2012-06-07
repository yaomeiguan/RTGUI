#include <rtthread.h>
#include <rtgui/rtgui_application.h>
#include <rtgui/widgets/window.h>

void wm_entry(void* parameter)
{
	struct rtgui_application* application;
	struct rtgui_win* win;

	application = rtgui_application_create(rt_thread_self(), "MyName");
	if (application != RT_NULL)
	{
		rtgui_rect_t rect = {20, 20, 200, 200};
		win = rtgui_win_create(RT_NULL, "MyWindow", &rect, RTGUI_WIN_STYLE_DEFAULT);
		rtgui_win_show(win, RT_FALSE);

		rtgui_application_run(application);
		rtgui_application_destroy(application);
	}
}

void wm_init(void)
{
	rt_thread_t tid;

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

	return 0;
}
