/** @file log.c
 * @brief Helpers for logging
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <syslog.h>

#include "faux/str.h"
#include "faux/log.h"

struct log_name {
	const char *name;
	int facility;
};

static struct log_name log_names[] = {
	{"local0", LOG_LOCAL0},
	{"local1", LOG_LOCAL1},
	{"local2", LOG_LOCAL2},
	{"local3", LOG_LOCAL3},
	{"local4", LOG_LOCAL4},
	{"local5", LOG_LOCAL5},
	{"local6", LOG_LOCAL6},
	{"local7", LOG_LOCAL7},
	{"auth", LOG_AUTH},
#ifdef LOG_AUTHPRIV
	{"authpriv", LOG_AUTHPRIV},
#endif
	{"cron", LOG_CRON},
	{"daemon", LOG_DAEMON},
#ifdef LOG_FTP
	{"ftp", LOG_FTP},
#endif
	{"kern", LOG_KERN},
	{"lpr", LOG_LPR},
	{"mail", LOG_MAIL},
	{"news", LOG_NEWS},
	{"syslog", LOG_SYSLOG},
	{"user", LOG_USER},
	{"uucp", LOG_UUCP},
	{NULL, 0}, // end of list
};

/** @brief Parses syslog facility string and returns the facility id.
 *
 * Gets syslog facility string, parse it and finds out the facility
 * id in digital form. Usefull config or command line options parsing.
 *
 * @param [in] str Facility string.
 * @param [out] facility Facility in digital form.
 * @returns BOOL_TRUE - success, BOOL_FALSE - parsing error
 */
bool_t faux_log_facility_id(const char *str, int *facility)
{
	int i = 0;

	assert(facility);
	assert(str);
	if (!str || !facility)
		return BOOL_FALSE;

	for (i = 0; log_names[i].name; i++) {
		if (faux_str_casecmp(str, log_names[i].name) == 0) {
			*facility = log_names[i].facility;
			return BOOL_TRUE;
		}
	}

	return BOOL_FALSE;
}

/** @brief Returns syslog facility string by facility id.
 *
 * @param [in] id Facility id.
 * @return Static facility string or NULL on error.
 */
const char *faux_log_facility_str(int facility_id)
{
	const char *res = NULL;
	int i = 0;

	for (i = 0; log_names[i].name; i++) {
		if (log_names[i].facility == facility_id) {
			res = log_names[i].name;
			break;
		}
	}

	return res;
}
