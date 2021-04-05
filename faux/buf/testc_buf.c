#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "faux/str.h"
#include "faux/buf.h"
#include "faux/testc_helpers.h"


#define CHUNK 100

int testc_faux_buf(void)
{
	char *src_fn = NULL;
	char *dst_fn = NULL;
	ssize_t len = 0;
	char *rnd = NULL;
	char *dst = NULL;
	faux_buf_t *buf = NULL;

	// Prepare files
	len = CHUNK * 3 + 15;
	rnd = faux_testc_rnd_buf(len);
	src_fn = faux_testc_tmpfile_deploy(rnd, len);

	// Create buf
	buf = faux_buf_new(CHUNK);
	if (!buf) {
		fprintf(stderr, "faux_buf_new() error\n");
		return -1;
	}

	// Write to buffer
	if (faux_buf_write(buf, rnd, len - 5) != (len - 5)) {
		fprintf(stderr, "faux_buf_write() error\n");
		return -1;
	}
	if (faux_buf_write(buf, rnd + len - 5, 5) != 5) {
		fprintf(stderr, "faux_buf_write() the rest error\n");
		return -1;
	}

	// Buf length
	if (faux_buf_len(buf) != len) {
		fprintf(stderr, "faux_buf_len() error\n");
		return -1;
	}

	// Buf read
	dst = faux_malloc(len);
	if (!dst) {
		fprintf(stderr, "faux_malloc() error\n");
		return -1;
	}
	if (faux_buf_read(buf, dst, len) != len) {
		fprintf(stderr, "faux_buf_read() error\n");
		return -1;
	}
	dst_fn = faux_testc_tmpfile_deploy(dst, len);

	// Buf length == 0
	if (faux_buf_len(buf) != 0) {
		fprintf(stderr, "faux_buf_len() is not 0: error\n");
		return -1;
	}

	// Compare files
	if (faux_testc_file_cmp(dst_fn, src_fn) != 0) {
		fprintf(stderr, "Destination file %s is not equal to source %s\n",
			dst_fn, src_fn);
		return -1;
	}

	faux_free(dst);
	faux_buf_free(buf);

	return 0;
}


int testc_faux_buf_boundaries(void)
{
	char *src_fn = NULL;
	char *dst_fn = NULL;
	ssize_t len = 0;
	char *rnd = NULL;
	char *dst = NULL;
	faux_buf_t *buf = NULL;

	// Prepare files
	len = CHUNK * 3;
	rnd = faux_testc_rnd_buf(len);
	src_fn = faux_testc_tmpfile_deploy(rnd, len);

	// Create buf
	buf = faux_buf_new(CHUNK);
	if (!buf) {
		fprintf(stderr, "faux_buf_new() error\n");
		return -1;
	}

	// Write to buffer
	if (faux_buf_write(buf, rnd, len) != len) {
		fprintf(stderr, "faux_buf_write() error\n");
		return -1;
	}

	// Buf length
	if (faux_buf_len(buf) != len) {
		fprintf(stderr, "faux_buf_len() error\n");
		return -1;
	}

	// Buf read
	dst = faux_malloc(len);
	if (!dst) {
		fprintf(stderr, "faux_malloc() error\n");
		return -1;
	}
	if (faux_buf_read(buf, dst, len) != len) {
		fprintf(stderr, "faux_buf_read() error\n");
		return -1;
	}
	dst_fn = faux_testc_tmpfile_deploy(dst, len);

	// Buf length == 0
	if (faux_buf_len(buf) != 0) {
		fprintf(stderr, "faux_buf_len() is not 0: error\n");
		return -1;
	}

	// Compare files
	if (faux_testc_file_cmp(dst_fn, src_fn) != 0) {
		fprintf(stderr, "Destination file %s is not equal to source %s\n",
			dst_fn, src_fn);
		return -1;
	}

	// Write to buffer anoter time
	if (faux_buf_write(buf, rnd, len) != len) {
		fprintf(stderr, "another faux_buf_write() error\n");
		return -1;
	}
	if (faux_buf_read(buf, dst, len) != len) {
		fprintf(stderr, "another faux_buf_read() error\n");
		return -1;
	}
	dst_fn = faux_testc_tmpfile_deploy(dst, len);

	// Compare files another time
	if (faux_testc_file_cmp(dst_fn, src_fn) != 0) {
		fprintf(stderr, "Destination file %s is not equal to source %s\n",
			dst_fn, src_fn);
		return -1;
	}

	faux_free(dst);
	faux_buf_free(buf);

	return 0;
}