/* -*- mode: c; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/* Emacs module for ip2region.

Copyright (C) 2017 Chunyang Xu <mail@xuchunyang.me>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "./ip2region/binding/c/ip2region.h"
#include <emacs-module.h>

int plugin_is_GPL_compatible;

static char *
retrieve_string (emacs_env *env, emacs_value str)
{
  char *buf = NULL;
  ptrdiff_t size = 0;

  env->copy_string_contents (env, str, NULL, &size);

  buf = malloc (size);
  if (buf == NULL) return NULL;

  env->copy_string_contents (env, str, buf, &size);

  return buf;
}

static void
ip2region_module_destroy (void *arg)
{
  ip2region_destroy ((ip2region_entry *) arg);
}

static emacs_value
Fip2region_module_create (emacs_env *env, ptrdiff_t nargs, emacs_value args[],
                          void *data)
{
  assert (nargs == 1);

  char *dbfile = NULL;
  ip2region_entry *ip2r = NULL;
  emacs_value Qnil = env->intern (env, "nil");

  ip2r = malloc (sizeof *ip2r);
  if (ip2r == NULL) return Qnil;
  dbfile = retrieve_string (env, args[0]);
  if (ip2region_create (ip2r, dbfile) == 0)
    {
      free (ip2r);
      free (dbfile);
      return Qnil;
    }
  free (dbfile);
  return env->make_user_ptr (env, ip2region_module_destroy, ip2r);
}

static emacs_value
Fip2region_module_search (emacs_env *env, ptrdiff_t nargs, emacs_value args[],
                          void *data)
{
  assert (nargs == 2);

  ip2region_entry *ip2r = env->get_user_ptr (env, args[0]);
  char *ip = retrieve_string (env, args[1]);
  datablock_entry datablock;
  memset (&datablock, 0, sizeof datablock);

  ip2region_btree_search_string (ip2r, ip, &datablock);
  /* XXX: Also return datablock.city_id */
  return env->make_string (env, datablock.region, strlen (datablock.region));
}

/* Lisp utilities for easier readability (simple wrappers).  */

/* Provide FEATURE to Emacs.  */
static void
provide (emacs_env *env, const char *feature)
{
  emacs_value Qfeat = env->intern (env, feature);
  emacs_value Qprovide = env->intern (env, "provide");
  emacs_value args[] = { Qfeat };

  env->funcall (env, Qprovide, 1, args);
}

/* Bind NAME to FUN.  */
static void
bind_function (emacs_env *env, const char *name, emacs_value Sfun)
{
  emacs_value Qfset = env->intern (env, "fset");
  emacs_value Qsym = env->intern (env, name);
  emacs_value args[] = { Qsym, Sfun };

  env->funcall (env, Qfset, 2, args);
}

/* Module init function.  */
int
emacs_module_init (struct emacs_runtime *ert)
{
  if (ert->size < sizeof *ert)
    {
      fprintf (stderr, "Runtime size of runtime structure (%td bytes) "
               "smaller than compile-time size (%zu bytes)",
               ert->size, sizeof *ert);
      return 1;
    }

  emacs_env *env = ert->get_environment (ert);

  if (env->size < sizeof *env)
    {
      fprintf (stderr, "Runtime size of environment structure (%td bytes) "
               "smaller than compile-time size (%zu bytes)",
               env->size, sizeof *env);
      return 2;
    }

#define DEFUN(lsym, csym, amin, amax, doc, data) \
  bind_function (env, lsym, \
                 env->make_function (env, amin, amax, csym, doc, data))

  DEFUN ("ip2region-module-create", Fip2region_module_create, 1, 1, "Create.", NULL);
  DEFUN ("ip2region-module-search", Fip2region_module_search, 2, 2, "Search.", NULL);

#undef DEFUN

  provide (env, "ip2region-module");
  return 0;
}
