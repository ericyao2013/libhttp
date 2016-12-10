/* 
 * Copyright (C) 2016 Lammert Bies
 * Copyright (c) 2013-2016 the Civetweb developers
 * Copyright (c) 2004-2013 Sergey Lyubka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



#include "libhttp-private.h"



/*
 * XX_httplib_load_dll( struct mg_context *ctx, const char *dll_name, struct ssl_func *sw );
 *
 * The function XX_httplib_load_dll() is used to load the SSL library in a
 * dynamic way. The function returns either a handle to the dll, or NULL if an
 * error occured.
 */

#if !defined(NO_SSL)

#if !defined(NO_SSL_DL)
void *XX_httplib_load_dll( struct mg_context *ctx, const char *dll_name, struct ssl_func *sw ) {

	union {
		void *p;
		void (*fp)(void);
	} u;
	void *dll_handle;
	struct ssl_func *fp;

	if ((dll_handle = dlopen(dll_name, RTLD_LAZY)) == NULL) {
		mg_cry( XX_httplib_fc(ctx), "%s: cannot load %s", __func__, dll_name);
		return NULL;
	}

	for (fp = sw; fp->name != NULL; fp++) {
#ifdef _WIN32
		/* GetProcAddress() returns pointer to function */
		u.fp = (void (*)(void))dlsym(dll_handle, fp->name);
#else
		/* dlsym() on UNIX returns void *. ISO C forbids casts of data
		 * pointers to function pointers. We need to use a union to make a
		 * cast. */
		u.p = dlsym(dll_handle, fp->name);
#endif /* _WIN32 */
		if (u.fp == NULL) {
			mg_cry( XX_httplib_fc(ctx), "%s: %s: cannot find %s", __func__, dll_name, fp->name);
			dlclose(dll_handle);
			return NULL;
		} else fp->ptr = u.fp;
	}

	return dll_handle;

}  /* XX_httplib_load_dll */
#endif /* NO_SSL_DL */

#endif /* !NO_SSL */