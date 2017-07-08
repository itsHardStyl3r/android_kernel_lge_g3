/*
 *  linux/drivers/devfreq/governor_minmax.c
 *
 *  Copyright (C) 2011 Samsung Electronics
 *	MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/devfreq.h>
#include <linux/module.h>
#include "governor.h"

static int devfreq_min_max_func(struct devfreq *devfreq, unsigned long *freq, u32 *flag)
{
	/*
	 * target callback should be able to get floor value as
	 * said in devfreq.h
	 */

	*freq = devfreq->max_freq;

	return 0;
}

static int devfreq_min_max_handler(struct devfreq *devfreq,
				unsigned int event, void *data)
{
	int ret = 0;

	mutex_lock(&devfreq->lock);
	switch (event) {
	case DEVFREQ_GOV_START:
		devfreq->profile->target(devfreq->dev.parent,
				&devfreq->max_freq,
				DEVFREQ_FLAG_WAKEUP_MAXFREQ);
		/* fall through */
	case DEVFREQ_GOV_RESUME:
		devfreq->profile->target(devfreq->dev.parent,
				&devfreq->max_freq,
				DEVFREQ_FLAG_WAKEUP_MAXFREQ);
		break;
	case DEVFREQ_GOV_SUSPEND:
		devfreq->profile->target(devfreq->dev.parent,
				&devfreq->min_freq,
				DEVFREQ_FLAG_WAKEUP_MAXFREQ);
		break;
	}
	mutex_unlock(&devfreq->lock);
	return ret;
}

static struct devfreq_governor devfreq_min_max = {
	.name = "min_max",
	.get_target_freq = devfreq_min_max_func,
	.event_handler = devfreq_min_max_handler,
};

static int __init devfreq_min_max_init(void)
{
	return devfreq_add_governor(&devfreq_min_max);
}
subsys_initcall(devfreq_min_max_init);

static void __exit devfreq_min_max_exit(void)
{
	int ret;

	ret = devfreq_remove_governor(&devfreq_min_max);
	if (ret)
		pr_err("%s: failed remove governor %d\n", __func__, ret);

	return;
}
module_exit(devfreq_min_max_exit);
MODULE_LICENSE("GPL");
