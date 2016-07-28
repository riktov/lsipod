/**
lsipod.c

A command-line utility to list iPod tracks. 
May be useful for copying from iPod to PC without using iTunes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

struct match_table {
  char *artist ;
  char *title ;
  char *album ;
} ;

/*
struct tracklist_and_match {
  GList *tracklist ;
  struct match_table matches ;
} ;
*/

void print_help() ;
void print_track(gpointer track, gpointer user_data) ;
void print_db_info(Itdb_iTunesDB *db) ;
//void print_mounted_ipods() ;
int get_mounted_ipods(char mounted_ipods[][PATH_MAX], size_t arr_size) ;
void list_ipod_contents(const char *mnt_point, struct match_table *matches, char sort_key) ;
int is_rchr(const char *str, char c) ;
gint comp_tracks(gconstpointer a, gconstpointer b, gpointer user_data) ;
void add_filtered_track(gpointer data, gpointer user_data) ;
GList *filter_tracks(GList *tracks, struct match_table *matches);
gboolean track_passes_filter(Itdb_Track *track, struct match_table *matches) ;

GError *err ;

int main(int argc, char *argv[]) {
  char *pa ;
  
  //struct match_table matches ;

  
  struct match_table matches =
    {
      NULL,
      NULL,
      NULL
    } ;

  //uint matches = 0 ;
  
  const char *selected_mount = NULL ;
  char sort_key = '\0'; 
  
  int iarg ;
  for(iarg = 1 ; iarg < argc ; iarg++) {
    pa = argv[iarg] ;
    if('-' == pa[0]) {
      switch(pa[1]) {
      case 'a':
	matches.artist = argv[++iarg] ;
	//printf("Option: match artist: %s\n", argv[iarg]) ;
	break ;
      case 'd':
	matches.album = argv[++iarg] ;
	//printf("Option: match album: %s\n", argv[iarg]) ;
	break ;
      case 't':
	matches.title = argv[++iarg] ;
	//printf("Option: match title: %s\n", argv[iarg]) ;
	break ;
      case 'v' :
	printf("lsipod Version %d.%d\n", lsipod_VERSION_MAJOR, lsipod_VERSION_MINOR) ;
	exit(0) ;
      case 'm':
	selected_mount = argv[++iarg] ;
	break ;
      case 's' :
	sort_key = argv[++iarg][0] ;
	printf("The sort key is %c\n", sort_key) ;
	break ;
      case 'h' :
	print_help() ;
	exit(0) ;
      default :
	fprintf(stderr, "Unknown option switch:%c\n", pa[1]) ;
	exit(0) ;
      }
    }
      
    //printf("%s\n", pa) ;
  }

  //exit(0) ;
  
  
  char mounts[MAX_NUM_MOUNTED_IPODS][PATH_MAX] ;

  int num_ipods ;
  int i = 0;

  //exit(0) ; //testing

  num_ipods = get_mounted_ipods(mounts, sizeof(mounts) / sizeof(*mounts)) ;

  if(selected_mount) {
    for(i = 0 ; i < num_ipods ; i++) {
      if(!strcmp(mounts[i], selected_mount)) {
	num_ipods = 1 ;
	break ;
      }
    }
  }
  
  switch(num_ipods) {
  case 0 :
    printf("No mounted ipods found.\n") ;
    break ;
  case 1 :
    //printf("Your ipod is here: %s.\n", mounts[i]) ;
    list_ipod_contents(mounts[i], &matches, sort_key) ;
    break ;
  default :
    printf("We found %d mounts.\n", num_ipods) ;
    for(i = 0 ; i < num_ipods ; i++) {
      printf("Found a mounted ipod at: %s\n", mounts[i]) ;
    }
    printf("Please specify which one you want to list.\n") ;
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

void print_help() {
  /* What is the standard GNUish format for help text??*/
  printf("lsipod\n") ;
  printf("  [-a artist]\n") ;
  printf("  [-d disc (album)]\n") ;
  printf("  [-t title]\n") ;
  printf("  [-m mount_number (if multiple mounted ipods are detected).]\n") ;
  printf("  [-s [adt] sort by artist, disc, or title.]\n") ;
  printf("  [-v version number]\n") ;
  printf("  [-h this help]\n") ;
}

gint comp_tracks(gconstpointer a, gconstpointer b, gpointer user_data) {
  Itdb_Track *track_a = (Itdb_Track *)a ;
  Itdb_Track *track_b = (Itdb_Track *)b ;

  char *psort_key = (char *)user_data ;
  char sort_key = *psort_key ;

  char *a_str, *b_str ;
  
  switch(sort_key) {
  case 'a':
    a_str = track_a->artist ;
    b_str = track_b->artist ;
    break ;
  case 'd':
    a_str = track_a->album ;
    b_str = track_b->album ;
    break ;
  case 't':
    a_str = track_a->title ;
    b_str = track_b->title ;
    break ;
  default:
    break ;
  }
  
  return g_strcmp0(a_str, b_str) ;
  //return track_a->time_added - track_b->time_added ;
}

void list_ipod_contents(const char *mnt_point, struct match_table *matches, char sort_key) {
  Itdb_iTunesDB *db = itdb_parse(mnt_point , &err) ;
  
  if (!db) {
    fprintf(stderr, "iPod found at %s, but no iTunes database found.\n", mnt_point) ;
    fprintf(stderr, "%s\n", err -> message) ;
    exit(-1);
  }
  
  //print_db_info(db) ;

  if((matches->artist == NULL) &&
     (matches->title == NULL) &&
     (matches->album == NULL)) {
    matches = NULL ;
    printf("No filters.\n") ;
  }

  //printf("iPod contains %d tracks.\n", g_list_length(db->tracks)) ;
  //return ;
  

  //printf("iPod contains %d tracks.\n", g_list_length(sorted)) ;

  //  g_list_foreach(db->tracks, print_track, (gpointer)matches) ;
  //g_list_foreach(sorted, print_track, (gpointer)&matches) ;

  GList *tracks = db->tracks ;
  
  if(matches) {
    tracks = filter_tracks(tracks, matches) ; //may allocate new, or return tracks
  }

  GList *sorted_tracks = g_list_sort_with_data(tracks, comp_tracks, (gpointer)&sort_key) ;

  g_list_foreach(sorted_tracks, print_track, NULL) ;

  if(tracks != db->tracks) {
    g_list_free(sorted_tracks) ;
  }

    //  g_list_free(filtered_tracks) ;
  
  itdb_free(db) ;
}

/* filter first, sort later */
GList *filter_tracks(GList *tracks, struct match_table *matches) {
  if (!matches) { return tracks ; }

  GList *filtered_tracks = NULL ;

  /* hack together an array containing the two values we need to pass
     to g_list_foreach(). The sorted list will successively built on to list_and_filter[0]
   */
  void *list_and_filter[] = {
    (void *)filtered_tracks,
    (void *)matches,
  } ;
  
  g_list_foreach(tracks, add_filtered_track, list_and_filter) ;

  //printf("Filtered list is now length %d\n", g_list_length(list_and_filter[0])) ;
  return list_and_filter[0] ;
}

void add_filtered_track(gpointer data, gpointer user_data) {
  Itdb_Track *track = (Itdb_Track *)data ;

  /* crack open the filtered tracks and matches*/
  void **list_and_filter = (void **)user_data ;
  
  GList *filtered_tracks = (GList *)list_and_filter[0] ;
  struct match_table *matches = (struct match_table *)list_and_filter[1] ;

  if(track_passes_filter(track, matches)) {
      list_and_filter[0] = g_list_prepend(filtered_tracks, track) ;
      //printf("Passing track through filter:%s: %s\n", track->artist, track->title) ;
      //printf("Selection list length: %d\n", g_list_length(list_and_filter[0])) ;
    }
}

gboolean track_passes_filter(Itdb_Track *track, struct match_table *matches) {
  if(!matches) { return TRUE ; }

  return (
	  (track->artist && matches->artist && g_strrstr(track->artist, matches->artist)) ||
	  (track->title  && matches->title  && g_strrstr(track->title, matches->title)) ||
	  (track->album  && matches->album  && g_strrstr(track->album, matches->album))
	  ) ;
}

/*
void print_mounted_ipods() {
  //  struct mntent m ;
  #ifdef __linux
  const struct mntent *pm ;

  FILE *mtab = setmntent("/etc/mtab", "r") ;

  while((pm = getmntent(mtab))) {
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
*/

int get_mounted_ipods(char mounted_ipods[][PATH_MAX], size_t arr_size) {
  int num_ipod_mounts = 0 ;
  Itdb_iTunesDB *db ;
  char *mnt_dir ;
  
#ifdef __linux
  const struct mntent *pm ;
  FILE *mtab = setmntent("/etc/mtab", "r") ;

  while((pm = getmntent(mtab))) {
    mnt_dir = pm->mnt_dir ;
    //mnt_dir = pm->mnt_fsname ;
#else
  struct statfs *pm ;
  int num_mounts, i ;

  num_mounts = getmntinfo(&pm, 0) ;
  
  for(i = 0 ; i < num_mounts ; i++) {
    mnt_dir = pm[i].f_mntonname ;
#endif
    err = NULL ;
    
    //printf("get_mounted_ipods() : %s\n", mnt_dir) ;

    
    if ((db = itdb_parse(mnt_dir, &err))) {
      if(num_ipod_mounts < arr_size) {
	//printf("get_mounted_ipods() : %s\n", mnt_dir) ;
	strncpy(*mounted_ipods, mnt_dir, sizeof(mounted_ipods[0])) ;
	//*mounted_ipods = mnt_dir ;
	num_ipod_mounts++ ;
	mounted_ipods++ ;
      }
      itdb_free(db) ; 
    }
  }

#ifdef __linux
  endmntent(mtab) ;
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

/* 
Print info on each track. The prototype for this function is defined by g_list_foreach(), 
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
  
  printf("%s %s%s #%s - %s - %s\n", datestr, mountpoint, p, track->title, track->artist, track->album) ;
    
  free(path) ;
}
 
 int is_rchr(const char *str, char c) {
   const char *r = strrchr(str, c) ;
   
   return(r == str + strlen(str) - 1) ;
 }

