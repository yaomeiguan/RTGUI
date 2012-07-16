/*
 * File      : app_list.h
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

#ifndef __RTGUI_CUSTOMER_LIST_H__
#define __RTGUI_CUSTOMER_LIST_H__

#include <rtgui/rtgui.h>
#include <rtgui/image.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/widget.h>

struct rtgui_customer_item
{
    char* name;
	rtgui_image_t *image;

	rt_uint32_t user_data;
};

DECLARE_CLASS_TYPE(customer_list);
/** Gets the type of a customer list */
#define RTGUI_CUSTOMER_LIST_TYPE       (RTGUI_TYPE(customer_list))
/** Casts the object to a customer list */
#define RTGUI_CUSTOMER_LIST(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_CUSTOMER_LIST_TYPE, rtgui_customer_list_t))
/** Checks if the object is a customer list */
#define RTGUI_IS_CUSTOMER_LIST(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_CUSTOMER_LIST_TYPE))

#define RTGUI_CUSTOMER_LIST_LIST		0x00
#define RTGUI_CUSTOMER_LIST_ICON		0x01
#define RTGUI_CUSTOMER_LIST_REPORT		0x02

struct rtgui_customer_list
{
	struct rtgui_widget parent;

	/* widget private data */
	/* list item */
    const struct rtgui_customer_item* items;

	/* layout flag */
	rt_uint16_t flag;

	/* total number of items */
	rt_uint16_t items_count;
    /* the number of item in a page */
    rt_uint16_t page_items;
	/* current item */
    rt_int16_t current_item;

	/* icon layout */
	rt_uint8_t row_items, col_items;

	/* on action call back */
	void (*on_action)(struct rtgui_widget* widget, const struct rtgui_customer_item* item);
};
typedef struct rtgui_customer_list rtgui_customer_list_t;

rtgui_customer_list_t* rtgui_customer_list_create(const struct rtgui_customer_item* items, rt_uint16_t count,
    rtgui_rect_t *rect, rt_uint16_t flag);
void rtgui_customer_list_destroy(rtgui_customer_list_t* list);

void rtgui_customer_list_set_on_action(struct rtgui_customer_list* list, 
	void (*on_action)(struct rtgui_widget* widget, const struct rtgui_customer_item* item));
rt_bool_t rtgui_customer_list_event_handler(struct rtgui_object* widget, struct rtgui_event* event);

#endif
