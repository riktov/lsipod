/**
lsipod.c

A command-line utility to list iPod tracks. 
May be useful for copying from iPod to PC without using iTunes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gpod/itdb.h>

#include "lsipod.h"

void print_track(gpointer track, gpointer user_data) ;

GError *err ;
const char *mnt_point ;

int main(int argc, char *argv[]) {
  /*
    if(argc > 2) {
    printf("%s Version %d.%d\n", lsipod_VERSION_MAJOR, lsipod_VERSION_MINOR) ;

  }
  */
  mnt_point = argv[1] ;
  if (NULL == mnt_point) {
    fprintf(stderr, "You must specify the iPod mount point\n") ;
    exit(-1);
  }

  Itdb_iTunesDB *db = itdb_parse(mnt_point , &err) ;
  
  if (!db) {
    fprintf(stderr, "iPod found at %s, but no iTunes database found.\n", mnt_point) ;
    fprintf(stderr, "%s\n", err -> message) ;
    exit(-1);
  }
  
  guint32 numtracks = itdb_tracks_number(db) ;
  printf("%d tracks\n", numtracks) ;
  
  g_list_foreach(db->tracks, print_track, NULL) ;
  
  itdb_free(db) ;
  
  return 0 ;
}

void print_track(gpointer data, gpointer user_data) {
  Itdb_Track *track = (Itdb_Track *)data ;
  char *path = strdup(track->ipod_path) ;
  itdb_filename_ipod2fs(path) ;

  struct tm *timeinfo ;
  timeinfo = localtime(&track->time_added) ;

  char buf[32] ;

  strftime(buf, sizeof(buf), "%F", timeinfo);

  //printf("%s - %s [%s%s]\n", track->title, track->artist, mnt_point, path) ;
  printf("%s %s%s #%s - %s\n", buf, mnt_point, path, track->title, track->artist) ;

  free(path) ;
}
