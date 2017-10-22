/* -*- mode: c; indent-tabs-mode: nil; -*- */
#include "./ip2region/binding/c/ip2region.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main ()
{
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
  printf ("The region of IP address %s is %d|%s\n",
          ip, datablock.city_id, datablock.region);

  ip2region_destroy (&ip2rEntry);
  return 0;
}
