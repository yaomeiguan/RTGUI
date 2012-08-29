/*
* 程序清单：bmp_zoom演示
*/
#include "demo_view.h"
#include <rtgui/dc.h>
#include <rtgui/image.h>
#include <rtgui/image_bmp.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/textbox.h>
#include <rtgui/widgets/container.h>

struct demo_bmp_dt
{
	float scale;
	char *filename;
	struct rtgui_image *image;
	struct rtgui_image *showimg;
	rtgui_container_t *showbox;
	rtgui_textbox_t *box;
}bmpdt;

rt_bool_t demo_bitmap_showbox(struct rtgui_object* object, struct rtgui_event* event)
{
	rtgui_container_t *container;
	rtgui_widget_t *widget;
	
	RT_ASSERT(object != RT_NULL);

	container = RTGUI_CONTAINER(object);
	widget = RTGUI_WIDGET(object);

	if(event->type == RTGUI_EVENT_PAINT)
	{
		int w, h;
		rtgui_rect_t rect, rc;
		struct rtgui_dc *dc;
		struct rtgui_image *image = bmpdt.showimg;
		
		dc = rtgui_dc_begin_drawing(widget);
		if (dc == RT_NULL)
			return RT_FALSE;
		
		rtgui_widget_get_rect(widget, &rect);
		rtgui_dc_draw_border(dc, &rect, RTGUI_WIDGET_BORDER_STYLE(widget));
		rtgui_rect_inflate(&rect, -RTGUI_WIDGET_BORDER(widget));
		w = rtgui_rect_width(rect);
		h = rtgui_rect_height(rect);
		/* fill container with background */
		if(image != RT_NULL)
		{	/* Reduce unnecessary drawing */
			if(w > image->w)
			{
				rc.x1 = image->w;
				rc.y1 = rect.y1;
				rc.x2 = rect.x2;
				rc.y2 = (h > image->h) ? image->h : rect.y2;
				rtgui_dc_fill_rect(dc, &rc);
			}
			if(h > image->h)
			{
				rc.x1 = rect.x1;
				rc.y1 = image->h;
				rc.x2 = rect.x2;
				rc.y2 = rect.y2;
				rtgui_dc_fill_rect(dc, &rc);
			}
		}
		else
			rtgui_dc_fill_rect(dc, &rect);

		if (bmpdt.showimg != RT_NULL)
			rtgui_image_blit(bmpdt.showimg, dc, &rect);

		rtgui_dc_end_drawing(dc);
		return RT_FALSE;
	}
	return rtgui_container_event_handler(object, event);
}

void demo_bitmap_open(struct rtgui_object* object, struct rtgui_event* event)
{
	char *str;
	rtgui_button_t *button = RTGUI_BUTTON(object);
	
	str = (char*)rtgui_textbox_get_value(bmpdt.box);
	if(str == RT_NULL) return;
	if(*str == '/' && (rt_strstr(str, ".bmp")!=RT_NULL || rt_strstr(str, ".BMP")!=RT_NULL))
	{
		if(bmpdt.filename != RT_NULL) 
			rt_free(bmpdt.filename);
		bmpdt.filename = rt_strdup(str);

		if(bmpdt.image != RT_NULL)
			rtgui_image_destroy(bmpdt.image);

		bmpdt.image = rtgui_image_create_from_file("bmp", bmpdt.filename, RT_TRUE);
		
		if(bmpdt.image != RT_NULL)
		{
			bmpdt.showimg = bmpdt.image;		
			rtgui_widget_update(RTGUI_WIDGET(bmpdt.showbox));
		}
	}
	else
		rt_kprintf("Bad filename!");
}

void demo_image_zoom_in(struct rtgui_object* object, struct rtgui_event* event)
{
	rtgui_button_t *button = RTGUI_BUTTON(object);

	if (bmpdt.image == RT_NULL) return;

	if (bmpdt.scale > 0.15)
	{
		if (bmpdt.scale > 1.0) bmpdt.scale -= (float)0.5;
		else bmpdt.scale -= (float)0.1;
	}

	bmpdt.showimg = rtgui_image_zoom(bmpdt.image, bmpdt.scale, bmpdt.scale, RTGUI_IMG_ZOOM_NEAREST);
	if (bmpdt.showimg != RT_NULL)
		rtgui_widget_update(RTGUI_WIDGET(bmpdt.showbox));
	if(bmpdt.showimg != bmpdt.image)
		rtgui_image_destroy(bmpdt.showimg);
}

void demo_image_zoom_out(struct rtgui_object* object, struct rtgui_event* event)
{
	rtgui_button_t *button = RTGUI_BUTTON(object);

	if (bmpdt.image == RT_NULL) return;

	if (bmpdt.scale < 4.95)
	{
		if (bmpdt.scale > 0.95) bmpdt.scale += (float)0.5;
		else bmpdt.scale += (float)0.1;
	}

	bmpdt.showimg = rtgui_image_zoom(bmpdt.image, bmpdt.scale, bmpdt.scale, RTGUI_IMG_ZOOM_NEAREST);
	if (bmpdt.showimg != RT_NULL)
		rtgui_widget_update(RTGUI_WIDGET(bmpdt.showbox));
	if(bmpdt.showimg != bmpdt.image)
		rtgui_image_destroy(bmpdt.showimg);
}

rtgui_container_t *demo_view_bmp(void)
{
	rtgui_rect_t rect;
	rtgui_container_t *container, *showbox;
	rtgui_button_t *button;
	rtgui_textbox_t *box;
	
	rt_memset(&bmpdt, 0, sizeof(struct demo_bmp_dt));
	bmpdt.scale = 1.0;

	container = demo_view("Bmp File:");

	demo_view_get_rect(container, &rect);
	rect.x1 += 85;
	rect.x2 -= 5;
	rect.y1 -= 42;
	rect.y2 = rect.y1 + 20;
	box = rtgui_textbox_create("/mm.bmp", RTGUI_TEXTBOX_SINGLE);
	rtgui_widget_set_rect(RTGUI_WIDGET(box), &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(box));
	bmpdt.box = box;

	demo_view_get_rect(container, &rect);
	rect.x1 += 5;
	rect.x2 = rect.x1 + 60;
	rect.y1 -= 10;
	rect.y2 = rect.y1 + 24;
	button = rtgui_button_create("open");
	rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(button));
	rtgui_button_set_onbutton(button, demo_bitmap_open);
	
	demo_view_get_rect(container, &rect);
	rect.x1 += 85;
	rect.x2 = rect.x1 + 70;
	rect.y1 -= 10;
	rect.y2 = rect.y1 + 24;
	button = rtgui_button_create("zoom in");
	rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(button));
	rtgui_button_set_onbutton(button, demo_image_zoom_in);

	demo_view_get_rect(container, &rect);
	rect.x1 += 165;
	rect.x2 = rect.x1 + 70;
	rect.y1 -= 10;
	rect.y2 = rect.y1 + 24;
	button = rtgui_button_create("zoom out");
	rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(button));
	rtgui_button_set_onbutton(button, demo_image_zoom_out);

	demo_view_get_rect(container, &rect);
	rect.x1 += 5;
	rect.x2 -= 5;
	rect.y1 += 20;
	rect.y2 -= 0;
	showbox = rtgui_container_create();
	rtgui_widget_set_rect(RTGUI_WIDGET(showbox), &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(showbox));
	rtgui_widget_set_border(RTGUI_WIDGET(showbox), RTGUI_BORDER_SIMPLE);
	bmpdt.showbox = showbox;
	rtgui_object_set_event_handler(RTGUI_OBJECT(showbox), demo_bitmap_showbox);
	
	return container;
}

