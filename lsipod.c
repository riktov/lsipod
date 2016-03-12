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

void test() ;
void print_track(gpointer track, gpointer user_data) ;
void print_db_info(Itdb_iTunesDB *db) ;
int is_rchr(const char *str, char c) ;

GError *err ;

int main(int argc, char *argv[]) {
  /*
    if(argc > 2) {
    printf("%s Version %d.%d\n", lsipod_VERSION_MAJOR, lsipod_VERSION_MINOR) ;

  }
  */

  const char *mnt_point = argv[1] ;

  if (NULL == mnt_point) {
    fprintf(stderr, "You must specify the iPod mount point\n") ;
    exit(-1);
  }

  //test() ;

  Itdb_iTunesDB *db = itdb_parse(mnt_point , &err) ;
  
  if (!db) {
    fprintf(stderr, "iPod found at %s, but no iTunes database found.\n", mnt_point) ;
    fprintf(stderr, "%s\n", err -> message) ;
    exit(-1);
  }
  
  //print_db_info(db) ;

  g_list_foreach(db->tracks, print_track, NULL) ;
  
  itdb_free(db) ;

  return 0 ;
}

void print_db_info(Itdb_iTunesDB *db) {
  printf("-- iTunesDB info --\n") ;

  printf("mountpoint:%s\n", itdb_get_mountpoint((Itdb_iTunesDB *)db)) ;
  printf("filename:%s\n", db -> filename) ;

  guint32 numtracks = itdb_tracks_number(db) ;
  printf("%d tracks\n", numtracks) ;
}

/* Print info on each track. The prototype for this function is defined by g_list_foreach(), 
 which receives it as an argument for iteration.
*/
void print_track(gpointer data, gpointer user_data) {
  Itdb_Track *track = (Itdb_Track *)data ;

  /* get the path */
  char *path = strdup(track->ipod_path) ;
  itdb_filename_ipod2fs(path) ;

  /* get the date added */
  struct tm *timeinfo ;
  timeinfo = localtime(&track->time_added) ;
  char datestr[32] ;
  strftime(datestr, sizeof(datestr), "%F", timeinfo);

  /* get the mounted directory from the database*/
  const char *mountpoint = itdb_get_mountpoint(track->itdb) ;

  char *p = path ;

  if(is_rchr(mountpoint, '/')) {
    p++ ;
  }

  printf("%s %s%s #%s - %s\n", datestr, mountpoint, p, track->title, track->artist) ;

  //  free(mnt_path) ;
  free(path) ;
}

void test() {
  printf("%d\n", is_rchr("Foo/", 'c')) ;
  exit(0) ;
}

int is_rchr(const char *str, char c) {
  const char *r = strrchr(str, c) ;

  return(r == str + strlen(str) - 1) ;
}
