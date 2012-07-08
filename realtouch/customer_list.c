/*
 * File      : customer_list.c
 * This file is part of RTGUI in RT-Thread RTOS
 * COPYRIGHT (C) 2010, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-07-07     Bernard      first version
 */

#include <rtgui/rtgui_theme.h>
#include "customer_list.h"

#define LIST_MARGIN		5

static void _rtgui_customer_list_constructor(struct rtgui_customer_list *list)
{
	/* default rect */
	struct rtgui_rect rect = {0, 0, 200, 200};

	/* set default widget rect and set event handler */
	rtgui_object_set_event_handler(RTGUI_OBJECT(list),rtgui_customer_list_event_handler);
	rtgui_widget_set_rect(RTGUI_WIDGET(list), &rect);

	RTGUI_WIDGET(list)->flag |= RTGUI_WIDGET_FLAG_FOCUSABLE;

	list->flag = RTGUI_CUSTOMER_LIST_LIST;
	list->current_item = 0;
	list->items_count = 0;
	list->page_items = 0;

	RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(list)) = white;
	RTGUI_WIDGET_TEXTALIGN(RTGUI_WIDGET(list)) = RTGUI_ALIGN_CENTER_VERTICAL;
}

DEFINE_CLASS_TYPE(customer_list, "customer_list", 
	RTGUI_WIDGET_TYPE,
	_rtgui_customer_list_constructor,
	RT_NULL,
	sizeof(struct rtgui_customer_list));

static void rtgui_customer_list_onicondraw(struct rtgui_customer_list* list, struct rtgui_dc *dc)
{
	struct rtgui_rect rect, item_rect, drawing_rect;
	rt_ubase_t c, r, item_index; /* col and row index */
	rt_ubase_t item_width, item_height;
	rtgui_image_t* image;

	if (list->items_count == 0) return;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);
	item_index = (list->current_item / list->page_items) * list->page_items;

	item_width = (rtgui_rect_width(rect) - 2 * LIST_MARGIN)/list->col_items;
	item_height = (rtgui_rect_height(rect) - 4)/list->row_items;
	image = list->items[0].image;

	for (r = 0; r < list->row_items; r ++)
	{
		for (c = 0; c < list->col_items; c ++)
		{
			if (item_index < list->items_count)
			{
				item_rect.y1 = rect.y1 + LIST_MARGIN + r * item_height;
				item_rect.x1 = rect.x1 + LIST_MARGIN + c * item_width;
				item_rect.x2 = item_rect.x1 + item_width;
				item_rect.y2 = item_rect.y1 + item_height;

				if (item_index == list->current_item)
				{
					rtgui_theme_draw_selected(dc, &item_rect);
				}

				drawing_rect.x1 = drawing_rect.y1 = 0;
				drawing_rect.x2 = image->w;
				drawing_rect.y2 = image->h;
				rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
				drawing_rect.y1 += 5; drawing_rect.y2 += 5;
				rtgui_image_blit(list->items[item_index].image, dc, &drawing_rect);

				item_rect.y1 = drawing_rect.y2 + LIST_MARGIN; 
				item_rect.x1 += 3; item_rect.x2 -=3;
				rtgui_font_get_metrics(RTGUI_WIDGET_FONT(RTGUI_WIDGET(list)), list->items[item_index].name, 
					&drawing_rect);
				rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
				rtgui_dc_draw_text(dc, list->items[item_index].name, &drawing_rect);

				item_index ++;
			}
			else break;
		}
	}
}

static void rtgui_customer_list_update_icon(struct rtgui_customer_list* list, rt_int16_t old_item)
{
	struct rtgui_rect rect, item_rect, drawing_rect;
	rt_ubase_t c, r; /* col and row index */
	rt_ubase_t item_width, item_height;
	rtgui_image_t* image;
	struct rtgui_dc* dc;

	if ((list->items_count == 0) ||
		(old_item == list->current_item))
		return;

	if (old_item/list->page_items != list->current_item/list->page_items)
	{
		/* it's not a same page, update all */
		rtgui_widget_update(RTGUI_WIDGET(list));
		return;
	}

	dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(list));
	if (dc == RT_NULL) return;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);

	item_width = (rtgui_rect_width(rect) - 2 * LIST_MARGIN)/list->col_items;
	item_height = (rtgui_rect_height(rect) - 4)/list->row_items;
	image = list->items[0].image;

	/* update old item */
	r = (old_item % list->page_items)/ list->col_items;
	c = (old_item % list->page_items)% list->col_items;
	item_rect.y1 = rect.y1 + LIST_MARGIN + r * item_height;
	item_rect.x1 = rect.x1 + LIST_MARGIN + c * item_width;
	item_rect.x2 = item_rect.x1 + item_width;
	item_rect.y2 = item_rect.y1 + item_height;
	rtgui_dc_fill_rect(dc, &item_rect);

	/* draw image */
	drawing_rect.x1 = drawing_rect.y1 = 0;
	drawing_rect.x2 = image->w;
	drawing_rect.y2 = image->h;
	rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
	drawing_rect.y1 += 5; drawing_rect.y2 += 5;
	rtgui_image_blit(list->items[old_item].image, dc, &drawing_rect);

	/* draw text */
	item_rect.y1 = drawing_rect.y2 + LIST_MARGIN; 
	item_rect.x1 += 3; item_rect.x2 -=3;
	rtgui_font_get_metrics(RTGUI_WIDGET_FONT(RTGUI_WIDGET(list)), list->items[old_item].name, 
		&drawing_rect);
	rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
	rtgui_dc_draw_text(dc, list->items[old_item].name, &drawing_rect);

	/* update new item as selected */
	r = (list->current_item % list->page_items) / list->col_items;
	c = (list->current_item % list->page_items) % list->col_items;
	item_rect.y1 = rect.y1 + LIST_MARGIN + r * item_height;
	item_rect.x1 = rect.x1 + LIST_MARGIN + c * item_width;
	item_rect.x2 = item_rect.x1 + item_width;
	item_rect.y2 = item_rect.y1 + item_height;
	rtgui_theme_draw_selected(dc, &item_rect);

	/* draw image */
	drawing_rect.x1 = 0;
	drawing_rect.y1 = 3;
	drawing_rect.x2 = image->w;
	drawing_rect.y2 = 3 + image->h;

	rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
	rtgui_image_blit(list->items[list->current_item].image, dc, &drawing_rect);

	/* draw text */
	item_rect.y1 = drawing_rect.y2 + LIST_MARGIN; 
	item_rect.x1 += 3; item_rect.x2 -=3;
	rtgui_font_get_metrics(RTGUI_WIDGET_FONT(RTGUI_WIDGET(list)), 
		list->items[list->current_item].name, 
		&drawing_rect);
	rtgui_rect_moveto_align(&item_rect, &drawing_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
	rtgui_dc_draw_text(dc, list->items[list->current_item].name, &drawing_rect);

	rtgui_dc_end_drawing(dc);
}

static void rtgui_customer_list_onlistdraw(struct rtgui_customer_list* list, struct rtgui_dc *dc)
{
	rt_ubase_t index, page_index;
	rtgui_rect_t rect, item_rect, image_rect;
	const struct rtgui_customer_item* item;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);

	/* get item base rect */
	item_rect = rect;
	item_rect.y1 += 2;
	item_rect.y2 = item_rect.y1 + (2 + rtgui_theme_get_selected_height());

	/* get current page */
	page_index = (list->current_item / list->page_items) * list->page_items;
	for (index = 0; index < list->page_items; index ++)
	{
		if (page_index + index >= list->items_count) break;

		item = &(list->items[page_index + index]);

		if (page_index + index == list->current_item)
		{
			rtgui_theme_draw_selected(dc, &item_rect);
		}
		item_rect.x1 += LIST_MARGIN;

		if (item->image != RT_NULL)
		{
			/* blit on center */
			image_rect.x1 = 0; image_rect.y1 = 0;
			image_rect.x2 = item->image->w;
			image_rect.y2 = item->image->h;

			rtgui_rect_moveto_align(&item_rect, &image_rect, RTGUI_ALIGN_CENTER_VERTICAL);
			rtgui_image_blit(item->image, dc, &image_rect);
			item_rect.x1 += item->image->w + 2;
		}
		/* draw text */
		rtgui_dc_draw_text(dc, item->name, &item_rect);

		if (item->image != RT_NULL)
			item_rect.x1 -= (item->image->w + 2);
		item_rect.x1 -= LIST_MARGIN;

		/* move to next item position */
		item_rect.y1 += (rtgui_theme_get_selected_height() + 2);
		item_rect.y2 += (rtgui_theme_get_selected_height() + 2);
	}
}

void rtgui_customer_list_update_list(struct rtgui_customer_list* list, rt_int16_t old_item)
{
	struct rtgui_dc* dc;
	const struct rtgui_customer_item* item;
	rtgui_rect_t rect, item_rect;

	if (old_item/list->page_items != list->current_item/list->page_items)
	{
		/* it's not a same page, update all */
		rtgui_widget_update(RTGUI_WIDGET(list));
		return;
	}

	dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(list));
	if (dc == RT_NULL) return;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);

	/* get old item's rect and draw old item */
	item_rect.x1 = rect.x1; item_rect.x2 = rect.x2;
	item_rect.y1 = rect.y1 + 2;
	item_rect.y1 += (old_item % list->page_items) * (2 + rtgui_theme_get_selected_height());
	item_rect.y2 = item_rect.y1 + (2 + rtgui_theme_get_selected_height());
	rtgui_dc_fill_rect(dc, &item_rect);

	item_rect.x1 += LIST_MARGIN;

	item = &(list->items[old_item]);
	if (item->image != RT_NULL)
	{
		struct rtgui_rect image_rect;

		/* blit on center */
		image_rect.x1 = 0; image_rect.y1 = 0;
		image_rect.x2 = item->image->w;
		image_rect.y2 = item->image->h;

		rtgui_rect_moveto_align(&item_rect, &image_rect, RTGUI_ALIGN_CENTER_VERTICAL);
		rtgui_image_blit(item->image, dc, &image_rect);
		item_rect.x1 += item->image->w + 2;
	}
	rtgui_dc_draw_text(dc, item->name, &item_rect);

	/* draw current item */
	item_rect = rect;
	/* get current item's rect */
	item_rect.y1 += 2;
	item_rect.y1 += (list->current_item % list->page_items) * (2 + rtgui_theme_get_selected_height());
	item_rect.y2 = item_rect.y1 + (2 + rtgui_theme_get_selected_height());

	/* draw current item */
	rtgui_theme_draw_selected(dc, &item_rect);

	item_rect.x1 += LIST_MARGIN;

	item = &(list->items[list->current_item]);
	if (item->image != RT_NULL)
	{
		struct rtgui_rect image_rect;

		/* blit on center */
		image_rect.x1 = 0; image_rect.y1 = 0;
		image_rect.x2 = item->image->w;
		image_rect.y2 = item->image->h;

		rtgui_rect_moveto_align(&item_rect, &image_rect, RTGUI_ALIGN_CENTER_VERTICAL);
		rtgui_image_blit(item->image, dc, &image_rect);
		item_rect.x1 += (item->image->w + 2);
	}
	rtgui_dc_draw_text(dc, item->name, &item_rect);

	rtgui_dc_end_drawing(dc);
}

void rtgui_customer_list_ondraw(struct rtgui_customer_list* list)
{
	struct rtgui_rect rect;
	struct rtgui_dc* dc;

	dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(list));
	if (dc == RT_NULL) return;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);
	rtgui_dc_fill_rect(dc, &rect);

	switch (list->flag)
	{
	case RTGUI_CUSTOMER_LIST_LIST:
		rtgui_customer_list_onlistdraw(list, dc);
		break;

	case RTGUI_CUSTOMER_LIST_ICON:
		rtgui_customer_list_onicondraw(list, dc);
		break;
	}

	rtgui_dc_end_drawing(dc);
}

static rt_bool_t rtgui_customer_list_onmouse(struct rtgui_customer_list* list, struct rtgui_event_mouse* emouse)
{
	rtgui_rect_t rect;

	/* calculate selected item */

	/* get physical extent information */
	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);
	rtgui_widget_rect_to_device(RTGUI_WIDGET(list), &rect);

	if (rtgui_rect_contains_point(&rect, emouse->x, emouse->y) == RT_EOK)
	{
		rt_uint16_t index;
		rt_uint16_t old_item;

		/* get old item */
		old_item = list->current_item;

		switch (list->flag)
		{
		case RTGUI_CUSTOMER_LIST_LIST:
			index = (emouse->y - rect.y1) / (2 + rtgui_theme_get_selected_height());

			if ((index < list->items_count) && (index < list->page_items))
			{
				/* set selected item */
				list->current_item = (list->current_item/list->page_items) * list->page_items + index;
				if (emouse->button & RTGUI_MOUSE_BUTTON_DOWN)
				{
					/* down event */
					rtgui_customer_list_update_list(list, old_item);
				}
				else
				{
					/* up event */
					if (list->on_action != RT_NULL)
					{
						list->on_action(RTGUI_WIDGET(list), 
							&list->items[list->current_item]);
					}
				}
			}
			break;

		case RTGUI_CUSTOMER_LIST_ICON:
			{
				rt_uint16_t x, y;
				rt_uint16_t item_height, item_width;
				rt_ubase_t current_page;

				item_width = (rtgui_rect_width(rect) - 2 * LIST_MARGIN)/list->col_items;
				item_height = (rtgui_rect_height(rect) - 4)/list->row_items;
				x = emouse->x - rect.x1;
				y = emouse->y - rect.y1;

				index = (y / item_height * list->col_items) + x / item_width;
				current_page = list->current_item / list->page_items;

				if ((index + (current_page * list->page_items) < list->items_count))
				{
					if (emouse->button & RTGUI_MOUSE_BUTTON_DOWN)
					{
						list->current_item = index + (current_page * list->page_items);

						/* down event */
						rtgui_customer_list_update_icon(list, old_item);
					}
					else
					{
						/* up event */
						if (list->on_action != RT_NULL)
						{
							list->on_action(RTGUI_WIDGET(list), 
								&list->items[list->current_item]);
						}
					}
				}
			}
			break;

		case RTGUI_CUSTOMER_LIST_REPORT:
			break;
		}

		return RT_TRUE;
	}

	return RT_FALSE;
}

rt_bool_t rtgui_customer_list_event_handler(struct rtgui_object* widget, struct rtgui_event* event)
{
	struct rtgui_customer_list* list = RT_NULL;

	list = RTGUI_CUSTOMER_LIST(widget);
	switch (event->type)
	{
	case RTGUI_EVENT_PAINT:
		rtgui_customer_list_ondraw(list);
		return RT_FALSE;

    case RTGUI_EVENT_RESIZE:
        {
			struct rtgui_event_resize* resize;

			resize = (struct rtgui_event_resize*)event;

            /* recalculate page items */
			list->page_items = resize->h  / (2 + rtgui_theme_get_selected_height());
        }
        break;

	case RTGUI_EVENT_MOUSE_BUTTON:
		{
			struct rtgui_event_mouse* emouse;

			emouse = (struct rtgui_event_mouse*)event;
			return rtgui_customer_list_onmouse(list, emouse);
		}

    case RTGUI_EVENT_KBD:
        {
            struct rtgui_event_kbd* ekbd = (struct rtgui_event_kbd*)event;
            if (ekbd->type == RTGUI_KEYDOWN)
            {
				rt_uint16_t old_item;

				if (list->current_item == -1) 
				{
					/* set a initial item */
					if ((list->items_count > 0) && 
						(ekbd->key == RTGUIK_UP || ekbd->key == RTGUIK_DOWN))
					{
						list->current_item = 0;
						if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
							rtgui_customer_list_update_list(list, -1);
						else
							rtgui_customer_list_update_icon(list, -1);
						break;
					}
					else return RT_FALSE;
				}

				old_item = list->current_item;
                switch (ekbd->key)
                {
				case RTGUIK_LEFT:
					if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
					{
						if (list->current_item - list->page_items >= 0)
							list->current_item -= list->page_items;
						
						rtgui_customer_list_update_list(list, old_item);
					}
					else if (list->flag == RTGUI_CUSTOMER_LIST_ICON)
					{
						if (list->current_item > 0)
							list->current_item --;
						rtgui_customer_list_update_icon(list, old_item);
					}
					return RT_FALSE;

                case RTGUIK_UP:
					if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
					{
						if (list->current_item > 0)
							list->current_item --;
						rtgui_customer_list_update_list(list, old_item);
					}
					else if (list->flag == RTGUI_CUSTOMER_LIST_ICON)
					{
						if (list->current_item >= list->col_items)
							list->current_item -= list->col_items;
						else 
							list->current_item = 0;
						
						rtgui_customer_list_update_icon(list, old_item);
					}
					return RT_FALSE;

				case RTGUIK_RIGHT:
					if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
					{
						if (list->current_item + list->page_items < list->items_count - 1)
							list->current_item += list->page_items;
						
						rtgui_customer_list_update_list(list, old_item);
					}
					else if (list->flag == RTGUI_CUSTOMER_LIST_ICON)
					{
						if (list->current_item < list->items_count - 1)
							list->current_item ++;
						
						rtgui_customer_list_update_icon(list, old_item);
					}
					return RT_FALSE;

                case RTGUIK_DOWN:
					if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
					{
						if (list->current_item < list->items_count - 1)
							list->current_item ++;
						
						rtgui_customer_list_update_list(list, old_item);
					}
					else if (list->flag == RTGUI_CUSTOMER_LIST_ICON)
					{
						if (list->current_item + list->col_items <= (list->items_count - 1))
							list->current_item += list->col_items;
						else 
							list->current_item = list->items_count - 1;

						rtgui_customer_list_update_icon(list, old_item);
					}
					return RT_FALSE;

				case RTGUIK_RETURN:
					if (list->on_action != RT_NULL)
					{
						list->on_action(RTGUI_WIDGET(list), 
							&list->items[list->current_item]);
					}
					return RT_FALSE;

                default:
                    break;
                }
            }
        }
		return RT_FALSE;
	}

    /* use list event handler */
    return rtgui_widget_event_handler(widget, event);
}

static void rtgui_customer_list_calc(struct rtgui_customer_list* list)
{
	rtgui_rect_t rect;
	rt_uint32_t image_width, image_height;
	rt_ubase_t text_width, text_height;
	rt_ubase_t item_width, item_height;

	if (list->items_count == 0) return;

	/* get image width and height */
	if (list->items[0].image != RT_NULL)
	{
		image_width  = list->items[0].image->w;
		image_height = list->items[0].image->h;
	}
	else
	{
		image_width  = 0;
		image_height = 0;
	}

	rtgui_font_get_metrics(RTGUI_WIDGET_FONT(RTGUI_WIDGET(list)), "HHHHHH", &rect);

	text_height = rtgui_rect_height(rect);
	text_width = rtgui_rect_width(rect);

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);

	item_width = (image_width + LIST_MARGIN);
	if (item_width < (text_width + LIST_MARGIN)) item_width = text_width + LIST_MARGIN;
	item_height = image_height + 8 + text_height + LIST_MARGIN; 
	if (item_width > item_height) item_height = item_width;
	else item_width = item_height;
	
	list->row_items = (rtgui_rect_height(rect) - 2 * LIST_MARGIN) / item_height;
	list->col_items = (rtgui_rect_width(rect) - 2 * LIST_MARGIN) / item_width;
	list->page_items = list->row_items * list->col_items;
}

rtgui_customer_list_t* rtgui_customer_list_create(const struct rtgui_customer_item* items, rt_uint16_t count, 
	rtgui_rect_t *rect, rt_uint16_t flag)
{
	struct rtgui_customer_list* list = RT_NULL;

	list = (struct rtgui_customer_list*) rtgui_widget_create(RTGUI_CUSTOMER_LIST_TYPE);
	if (list != RT_NULL)
	{
		list->on_action = RT_NULL;
	    list->items = items;
	    list->items_count = count;

		list->flag = flag;
		rtgui_widget_set_rect(RTGUI_WIDGET(list), rect);

		if (flag == RTGUI_CUSTOMER_LIST_LIST)
			list->page_items = rtgui_rect_height(*rect) / (2 + rtgui_theme_get_selected_height());
		else if ((flag == RTGUI_CUSTOMER_LIST_ICON) && (count > 0))
		{
			rtgui_customer_list_calc(list);
		}
	}

	return list;
}

void rtgui_customer_list_destroy(rtgui_customer_list_t* list)
{
    /* destroy list */
	rtgui_widget_destroy(RTGUI_WIDGET(list));
}

void rtgui_customer_list_set_on_action(struct rtgui_customer_list* list, 
									   void (*on_action)(struct rtgui_widget* widget, const struct rtgui_customer_item* item))
{
	list->on_action = on_action;
}

void rtgui_customer_list_set_items(struct rtgui_customer_list* list, 
									  const struct rtgui_customer_item* items, rt_uint16_t count)
{
	struct rtgui_rect rect;

	RT_ASSERT(list != RT_NULL);

	list->items = items;
	list->items_count = count;

	rtgui_widget_get_rect(RTGUI_WIDGET(list), &rect);
	if (list->flag == RTGUI_CUSTOMER_LIST_LIST)
		list->page_items = rtgui_rect_height(rect) / (2 + rtgui_theme_get_selected_height());
	else if ((list->flag == RTGUI_CUSTOMER_LIST_ICON) && (count > 0))
	{
		rtgui_customer_list_calc(list);
	}

	rtgui_widget_update(RTGUI_WIDGET(list));
}
