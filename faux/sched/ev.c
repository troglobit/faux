/** @file ev.c
 * Single event for scheduling.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "private.h"
#include "faux/str.h"
#include "faux/sched.h"


/** @brief Callback function to compare two events by time.
 *
 * It's used for ordering within schedule list.
 *
 * @param [in] first First event to compare.
 * @param [in] second Second event to compare.
 * @return
 * > 0 if first > second,
 * 0 - equal,
 * < 0 if first < second
 */
int faux_ev_compare(const void *first, const void *second)
{
	const faux_ev_t *f = (const faux_ev_t *)first;
	const faux_ev_t *s = (const faux_ev_t *)second;

	return faux_timespec_cmp(&(f->time), &(s->time));
}


/** @brief Callback function to compare key and list item by ID.
 *
 * It's used to search for specified ID within schedule list.
 *
 * @param [in] key Pointer to key value
 * @param [in] list_item Pointer to list item.
 * @return
 * > 0 if key > list_item,
 * 0 - equal,
 * < 0 if key < list_item
 */
int faux_ev_compare_id(const void *key, const void *list_item)
{
	int *f = (int *)key;
	const faux_ev_t *s = (const faux_ev_t *)list_item;

	return ((*f == s->id) ? 0 : 1);
}


/** @brief Callback function to compare key and list item by data pointer.
 *
 * It's used to search for specified data pointer within schedule list.
 *
 * @param [in] key Pointer to key value
 * @param [in] list_item Pointer to list item.
 * @return
 * > 0 if key > list_item,
 * 0 - equal,
 * < 0 if key < list_item
 */
int faux_ev_compare_data(const void *key, const void *list_item)
{
	void *f = (void *)key;
	const faux_ev_t *s = (const faux_ev_t *)list_item;

	return ((f == s->data) ? 0 : 1);
}


/** @brief Callback function to compare key and list item by event pointer.
 *
 * It's used to search for specified event pointer within schedule list.
 *
 * @param [in] key Pointer to key value
 * @param [in] list_item Pointer to list item.
 * @return
 * 1 - not equal
 * 0 - equal
 */
int faux_ev_compare_ptr(const void *key, const void *list_item)
{
	return ((key == list_item) ? 0 : 1);
}


/** @brief Allocates and initialize ev object.
 *
 * @param [in] ev_id ID of event.
 * @param [in] data Pointer to arbitrary linked data. Can be NULL.
 * @param [in] free_data_cb Callback to free user data. Can be NULL.
 * @return Allocated and initialized ev object.
 */
faux_ev_t *faux_ev_new(int ev_id, void *data)
{
	faux_ev_t *ev = NULL;

	ev = faux_zmalloc(sizeof(*ev));
	assert(ev);
	if (!ev)
		return NULL;

	// Initialize
	ev->id = ev_id;
	ev->data = data;
	ev->free_data_cb = NULL;
	ev->periodic = FAUX_SCHED_ONCE; // Not periodic by default
	ev->cycle_num = 0;
	faux_nsec_to_timespec(&(ev->period), 0l);
	faux_ev_reschedule(ev, FAUX_SCHED_NOW);
	ev->busy = BOOL_FALSE;

	return ev;
}


/** @brief Frees ev object. Forced version.
 *
 * Private function. Don't check busy flag.
 *
 * @param [in] ptr Pointer to ev object.
 */
void faux_ev_free_forced(void *ptr)
{
	faux_ev_t *ev = (faux_ev_t *)ptr;

	if (!ev)
		return;
	if (ev->free_data_cb)
		ev->free_data_cb(ev->data);
	faux_free(ev);
}

/** @brief Frees ev object.
 *
 * Doesn't free busy event.
 *
 * @param [in] ptr Pointer to ev object.
 */
void faux_ev_free(void *ptr)
{
	faux_ev_t *ev = (faux_ev_t *)ptr;

	if (!ev)
		return;
	if (faux_ev_is_busy(ev))
		return; // Don't free busy event
	faux_ev_free_forced(ev);
}


/** @brief Gets busy status of event.
 *
 * When event is scheduled then event is "busy". Only scheduler can change
 * busy status of event. Busy event can't be freed.
 * Busy event can't be changed.
 *
 * @param [in] ev Allocated and initialized event object.
 * @return BOOL_TRUE - busy, BOOL_FALSE - not busy.
 */
bool_t faux_ev_is_busy(const faux_ev_t *ev)
{
	assert(ev);
	if (!ev)
		return BOOL_FALSE;

	return ev->busy;
}


/** @brief Sets busy flag.
 *
 * It's private but not static function. Only scheduler can use this function.
 * Another code can only get busy status.
 *
 * @param [in] ev Allocated and initialized event object.
 * @param [in] busy New state of busy flag.
 * @return BOOL_TRUE - busy, BOOL_FALSE - not busy.
 */
void faux_ev_set_busy(faux_ev_t *ev, bool_t busy)
{
	assert(ev);
	if (!ev)
		return;

	ev->busy = busy;
}


/** @brief Sets callback to free user data.
 *
 * @param [in] ev Allocated and initialized event object.
 * @param [in] free_data_cb Function to free user data.
 */
void faux_ev_set_free_data_cb(faux_ev_t *ev, faux_list_free_fn free_data_cb)
{
	assert(ev);
	if (!ev)
		return;

	ev->free_data_cb = free_data_cb;
}


/** @brief Makes event periodic.
 *
 * By default new events are not periodic. Doesn't change state of busy object.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @param [in] period Period of periodic event. If NULL then non-periodic event.
 * @param [in] cycle_num Number of cycles. FAUX_SHED_INFINITE - infinite.
 * @return BOOL_TRUE - success, BOOL_FALSE on error.
 */
bool_t faux_ev_set_periodic(faux_ev_t *ev,
	const struct timespec *period, unsigned int cycle_num)
{
	assert(ev);
	assert(period);
	if (!ev)
		return BOOL_FALSE;
	if (faux_ev_is_busy(ev))
		return BOOL_FALSE; // Don't change busy event
	if (!period) {
		ev->periodic = FAUX_SCHED_ONCE;
		return BOOL_TRUE;
	}
	// When cycle_num == 0 then periodic has no meaning
	if (0 == cycle_num)
		return BOOL_FALSE;

	ev->periodic = FAUX_SCHED_PERIODIC;
	ev->cycle_num = cycle_num;
	ev->period = *period;

	return BOOL_TRUE;
}


/** @brief Checks is event periodic.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @return FAUX_SCHED_PERIODIC - periodic, FAUX_SCHED_ONCE - non-periodic.
 */
faux_sched_periodic_e faux_ev_is_periodic(const faux_ev_t *ev)
{
	assert(ev);
	if (!ev)
		return FAUX_SCHED_ONCE;

	return ev->periodic;
}


/** @brief Decrements number of periodic cycles.
 *
 * On every completed cycle the internal cycles counter must be decremented.
 * Private function. Only scheduler can use it.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @param [out] new_cycle_num Returns new number of cycles. Can be NULL.
 * @return BOOL_TRUE - success, BOOL_FALSE - error.
 */
bool_t faux_ev_dec_cycles(faux_ev_t *ev, unsigned int *new_cycle_num)
{
	assert(ev);
	if (!ev)
		return BOOL_FALSE;
	if (!faux_ev_is_periodic(ev))
		return BOOL_FALSE; // Non-periodic event
	if ((ev->cycle_num != FAUX_SCHED_INFINITE) &&
		(ev->cycle_num > 0))
		ev->cycle_num--;

	if (new_cycle_num)
		*new_cycle_num = ev->cycle_num;

	return BOOL_TRUE;
}


/** Set schedule time of  existent event to specified value.
 *
 * It's same as faux_ev_reschedule but checks for busy flag before setting.
 *
 * @param [in] ev Allocated and initialized event object.
 * @param [in] new_time New time of event (FAUX_SCHED_NOW for now).
 * @return BOOL_TRUE - success, BOOL_FALSE on error.
 */
bool_t faux_ev_set_time(faux_ev_t *ev, const struct timespec *new_time)
{
	assert(ev);
	if (!ev)
		return BOOL_FALSE;
	if (faux_ev_is_busy(ev))
		return BOOL_FALSE; // Don't change busy event

	return faux_ev_reschedule(ev, new_time);
}


/** Returns time of event object.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @return Pointer to static timespec.
 */
const struct timespec *faux_ev_time(const faux_ev_t *ev)
{
	assert(ev);
	if (!ev)
		return NULL;

	return &(ev->time);
}


/** Reschedules existent event to newly specified time.
 *
 * Note: faux_ev_new() use it. Be carefull. Same as faux_ev_set_time() but
 * doesn't check for busy flag. Private function.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @param [in] new_time New time of event (FAUX_SCHED_NOW for now).
 * @return BOOL_TRUE - success, BOOL_FALSE on error.
 */
bool_t faux_ev_reschedule(faux_ev_t *ev, const struct timespec *new_time)
{
	assert(ev);
	if (!ev)
		return BOOL_FALSE;

	if (new_time) {
		ev->time = *new_time;
	} else { // Time isn't given so use "NOW"
		faux_timespec_now(&(ev->time));
	}

	return BOOL_TRUE;
}


/** Reschedules existent event using period.
 *
 * New scheduled time is calculated as "now" + "period".
 * Function decrements number of cycles. If number of cycles is
 * FAUX_SCHED_INFINITE then number of cycles will not be decremented.
 * Private function. Only scheduler can use it.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @return BOOL_TRUE - success, BOOL_FALSE on error.
 */
bool_t faux_ev_reschedule_period(faux_ev_t *ev)
{
	struct timespec new_time = {};

	assert(ev);
	if (!ev)
		return BOOL_FALSE;
	if (!faux_ev_is_periodic(ev))
		return BOOL_FALSE;
	if (ev->cycle_num <= 1)
		return BOOL_FALSE; // We don't need to reschedule if last cycle left

	faux_timespec_sum(&new_time, &(ev->time), &(ev->period));
	faux_ev_reschedule(ev, &new_time);

	if (ev->cycle_num != FAUX_SCHED_INFINITE)
		faux_ev_dec_cycles(ev, NULL);

	return BOOL_TRUE;
}


/** @brief Calculates time left from now to the event.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @param [out] left Calculated time left.
 * @return BOOL_TRUE - success, BOOL_FALSE on error.
 */
bool_t faux_ev_time_left(const faux_ev_t *ev, struct timespec *left)
{
	struct timespec now = {};

	assert(ev);
	assert(left);
	if (!ev || !left)
		return BOOL_FALSE;

	faux_timespec_now(&now);
	if (faux_timespec_cmp(&now, &(ev->time)) > 0) { // Already happened
		faux_nsec_to_timespec(left, 0l);
		return BOOL_TRUE;
	}
	faux_timespec_diff(left, &(ev->time), &now);

	return BOOL_TRUE;
}


/** Returns ID of event object.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @return Event's ID.
 */
int faux_ev_id(const faux_ev_t *ev)
{
	assert(ev);
	if (!ev)
		return -1;

	return ev->id;
}


/** Returns user data pointer of event object.
 *
 * @param [in] ev Allocated and initialized ev object.
 * @return User data pointer.
 */
void *faux_ev_data(const faux_ev_t *ev)
{
	assert(ev);
	if (!ev)
		return NULL;

	return ev->data;
}


