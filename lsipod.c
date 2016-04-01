/**
lsipod.c

A command-line utility to list iPod tracks. 
May be useful for copying from iPod to PC without using iTunes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux
#include <mntent.h>
#else
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#endif

#include <gpod/itdb.h>
//#include <glib-2.0/glib.h>

#include "lsipod.h"

void test() ;
void print_track(gpointer track, gpointer user_data) ;
void print_db_info(Itdb_iTunesDB *db) ;
void print_mounted_ipods() ;
int get_mounted_ipods(char *mounted_ipods[], size_t arr_size) ;
void list_ipod_contents(const char *mnt_point) ;


int is_rchr(const char *str, char c) ;

GError *err ;

int main(int argc, char *argv[]) {
  /*
    if(argc > 2) {
    printf("%s Version %d.%d\n", lsipod_VERSION_MAJOR, lsipod_VERSION_MINOR) ;

  }
  */

  char *mounts[MAX_NUM_MOUNTED_IPODS] ;

  int num_ipods ;
  int i = 0;
  
  //print_mounted_ipods() ;
  //exit(0) ;

  num_ipods = get_mounted_ipods(mounts, sizeof(mounts)) ;

  switch(num_ipods) {
  case 0 :
    printf("No mounted ipods found.\n") ;
    break ;
  case 1 :
    printf("Your ipod is here: %s.\n", mounts[i]) ;
    list_ipod_contents(mounts[i]) ;
    break ;
  default :
    for(i = 0 ; i < num_ipods ; i++) {
      printf("Found a mounted ipod at %s.\n", mounts[i]) ;
      printf("Please specify which one you want to list.\n") ;
    }
  }
  

  /*
  const char *mnt_point = argv[1] ;

  if (NULL == mnt_point) {
    fprintf(stderr, "You must specify the iPod mount point\n") ;
    exit(-1);
  }
  */
  
  return 0 ;
}

void list_ipod_contents(const char *mnt_point) {
  //test() ;

  Itdb_iTunesDB *db = itdb_parse(mnt_point , &err) ;
  
  if (!db) {
    fprintf(stderr, "iPod found at %s, but no iTunes database found.\n", mnt_point) ;
    fprintf(stderr, "%s\n", err -> message) ;
    exit(-1);
  }
  
  /////print_db_info(db) ;

  g_list_foreach(db->tracks, print_track, NULL) ;
  
  itdb_free(db) ;
}


void print_mounted_ipods() {
  //  struct mntent m ;
  #ifdef __linux
  const struct mntent *pm ;

  FILE *mtab = setmntent("/etc/mtab", "r") ;

  while(pm = getmntent(mtab)) {
    printf("%s\n", pm->mnt_dir) ;
  }
  
  endmntent(mtab) ;
  #else

  struct statfs *pm ;
  int num_mounts, i ;

  num_mounts = getmntinfo(&pm,0 ) ;
  for(i = 0; i < num_mounts ; i++) {
    printf("%s\n", pm[i].f_mntonname) ; 
  }
  #endif
}

int get_mounted_ipods(char *mounted_ipods[], size_t arr_size) {
  int num_ipod_mounts = 0 ;
  Itdb_iTunesDB *db ;

#ifdef __linux
  const struct mntent *pm ;
  FILE *mtab = setmntent("/etc/mtab", "r") ;

  
  while(pm = getmntent(mtab)) {
    err = NULL ;
    db = itdb_parse(pm->mnt_dir, &err) ;
    
   if (db) {
     if(num_ipod_mounts < arr_size) {
       *mounted_ipods++ = pm->mnt_dir ;
       num_ipod_mounts++ ;
     }
     itdb_free(db) ; 
   }
  }
#else
  struct statfs *pm ;
  int num_mounts, i ;

  num_mounts = getmntinfo(&pm, 0) ;
  
  for(i = 0 ; i < num_mounts ; i++) {
    err = NULL ;
    db = itdb_parse(pm[i].f_mntonname, &err) ;

    if(db) {
      if(num_mounts < arr_size) {
	*mounted_ipods++ = pm[i].f_mntonname ;
	num_ipod_mounts++ ;
      }
      itdb_free(db) ;
    }
  }
  
#endif
  return num_ipod_mounts ;
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
