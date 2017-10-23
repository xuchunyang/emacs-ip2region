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

#include "./ip2region/binding/c/ip2region.h"
#include <emacs-module.h>

int plugin_is_GPL_compatible;

static char *
foo ()
{
  char *result;
  char *ip = "116.62.40.117";
  char *db = "ip2region/data/ip2region.db";
  ip2region_entry ip2rEntry;
  datablock_entry datablock;
  memset (&datablock, 0, sizeof datablock);

  if (ip2region_create (&ip2rEntry, db) == 0)
    {
      fprintf (stderr, "Error: Fail to create the ip2region object\n");
      exit (1);
    }

  ip2region_btree_search_string (&ip2rEntry, ip, &datablock);
  if (asprintf (&result,
                "The region of IP address %s is %d|%s",
                ip,
                datablock.city_id,
                datablock.region) < 0)
    result = NULL;

  ip2region_destroy (&ip2rEntry);
  return result;
}

static emacs_value
Fip2region_module_foo (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  char *result = foo ();
  if (result)
    return env->make_string (env, result, strlen (result));
  else
    return env->intern (env, "nil");
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

  DEFUN ("ip2region-module-foo", Fip2region_module_foo, 0, 0, "Return t.", NULL);

#undef DEFUN

  provide (env, "ip2region-module");
  return 0;
}
