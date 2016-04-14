/**
imobtest.c

A program to test some of the libimobiledevice APIs.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gpod/itdb.h>
//#include <glib-2.0/glib.h>

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/sbservices.h>
#include <libimobiledevice/afc.h>

#include "lsipod.h"

void test_imobiledevice() ;
void test_springboard(idevice_t device, lockdownd_client_t client) ;
void test_afc(idevice_t device, lockdownd_client_t client) ;

GError *err ;

int main(int argc, char *argv[]) {
  char *pa ;
  
  int iarg ;
  for(iarg = 1 ; iarg < argc ; iarg++) {
    pa = argv[iarg] ;
    if('-' == pa[0]) {
      switch(pa[1]) {
      case 'a':
	break ;
      case 'd':
	break ;
      default :
	fprintf(stderr, "Unknown option switch:%c\n", pa[1]) ;
      }
    }
  }

  test_imobiledevice() ;
  exit(0) ;
    
  return 0 ;
}

void notify(const idevice_event_t *ev, void *user_data) {
  enum idevice_event_type e = ev->event ;
  const char * u = ev->udid ;
  
  switch(e) {
  case IDEVICE_DEVICE_ADD :
    printf("Device added\n") ;
    break;
  case IDEVICE_DEVICE_REMOVE :
    printf("Device removed\n") ;
    break;
  default :
    printf("Unknown idevice_event_type\n") ;
  }
  
  printf("Device udid: %s with: %s\n", u, (char *)user_data) ;
}

void test_imobiledevice() {
  char **device_udids ;
  int count ;
  
  char user_data[] = "Some user data" ;
  
  idevice_event_subscribe(&notify, (void *)user_data) ;
  
  idevice_error_t err = idevice_get_device_list(&device_udids, &count) ;
  
  //printf("Sleeping for 5 seconds...\n") ;
  //sleep(5) ;
  
  if(err == IDEVICE_E_SUCCESS) {
    printf("Found %d ilibmobile devices.\n", count) ;
    for(int i = 0 ; i < count ; i++) {
      uint32_t handle ;
      idevice_t device ;
      lockdownd_client_t client ;
      
      idevice_new(&device, device_udids[i]) ;
      printf("Just called idevice_new()\n") ;
      
      idevice_get_handle(device, &handle) ;
      printf("Just called idevice_get_handle()\n") ;
      
      const char *lockdown_label = "foobar" ;
      
      lockdownd_error_t ld_err = lockdownd_client_new_with_handshake(device, &client, lockdown_label) ;
      if(ld_err == LOCKDOWN_E_SUCCESS) {
	printf("Successfully created a lockdown client\n") ;
	
	
	char *device_name ;
	ld_err = lockdownd_get_device_name(client, &device_name) ;
	if(ld_err == LOCKDOWN_E_SUCCESS) {
	  printf("Device name: %s\n", device_name) ;
	  free(device_name) ;
	} else {
	  printf("Could not get device name: %d\n", ld_err) ;
	}
	
	
	char *service_type ;
	ld_err = lockdownd_query_type(client, &service_type) ;
	if(ld_err == LOCKDOWN_E_SUCCESS) {
	  printf("Got service type: %s\n", service_type) ;
	  //test_springboard(device, client) ;
	  test_afc(device, client) ;
	} else {
	  printf("Could not query service.\n") ;
	}
	
	lockdownd_client_free(client) ;
	printf("Freed the lockdown client\n") ;
	
      } else {
	printf("Could not get a lockdown client\n") ; 
      }
      
      //printf("%s with handle %d.\n", device_udids[i], handle) ;
      idevice_free(device) ;
      printf("Just called idevice_free()\n") ;
      
    }
  } else {
    printf("ilibmobiledevice error\n") ;
  }
  
  
  idevice_device_list_free(device_udids) ;
  idevice_event_unsubscribe() ;
}

 
 void test_springboard(idevice_t device, lockdownd_client_t client) {
   lockdownd_service_descriptor_t lsd ;
   lockdownd_error_t ld_err ;
   sbservices_error_t sb_err ;
   sbservices_client_t sbclient ;
   
   //const char *service_id = "com.apple.springboardservices" ;
   
   ld_err = lockdownd_start_service(client, SBSERVICES_SERVICE_NAME, &lsd) ;
   
   if(ld_err == LOCKDOWN_E_SUCCESS ){
     printf("Started service: %s\n", SBSERVICES_SERVICE_NAME) ;
     
     sb_err = sbservices_client_new(device, lsd, &sbclient) ;
     //sb_err = sbservices_client_start_service(device, &sbclient, "foobar" ) ;
     if(sb_err == SBSERVICES_E_SUCCESS) {
       printf("Connected to springboard service.\n") ;

       /*
       char *pngdata ;
       uint64_t pngsize ;
       sb_err = sbservices_get_home_screen_wallpaper_pngdata(sbclient, &pngdata, &pngsize) ;
       if(sb_err == SBSERVICES_E_SUCCESS) {
	 printf("Got wallpaper of size %ld\n", pngsize) ;
       } else {
	 printf("Could not get wallpaper: %d\n", sb_err) ;
       }
       */

       
       plist_t plist ;
       sb_err = sbservices_get_icon_state(sbclient, &plist, NULL) ;
       if(sb_err == SBSERVICES_E_SUCCESS) {
	 printf("Got icon state\n") ;

	 
	 plist_type node_type = plist_get_node_type(plist) ;
	 printf("Node type: %d\n", node_type) ;

	 
	 if(node_type == PLIST_ARRAY) {
	   uint32_t array_size = plist_array_get_size(plist) ;
	   printf("plist is a PLIST_ARRAY of size %d\n", array_size) ;
	   //plist_t first_elem = plist_array_get_item(plist, 0) ;
	   /*
	   char *plist_xml ;
	   uint32_t xml_len ;
	   plist_to_xml(plist, &plist_xml, &xml_len) ;
	   //printf("XML size: %d\n", xml_len) ;
	   printf("%s\n", plist_xml) ;
	   free(plist_xml) ;
	   */
	 }
	 
	 /*
	 //char *plist_str ;
	 //plist_get_string_val(plist, &plist_str) ;
	 //printf("%s", plist_str) ;
	 */
	 free(plist) ;
       } else {
	 printf("Could not get icon state: %d\n", sb_err) ;
       }
       
     } else {
       printf("Could not connect to springboard service.\n") ;
     }
     sb_err = sbservices_client_free(sbclient) ;
     if(sb_err == SBSERVICES_E_SUCCESS) {
       printf("Freed sbservices client.\n") ;
     }
   } else {
     printf("Could not start service: %d\n", ld_err) ;
   }  
   

   //sbservices_error_t sb_err = sbservices_client_new(device, )
 }
 
void test_afc(idevice_t device, lockdownd_client_t client) {
   lockdownd_service_descriptor_t lsd ;
   lockdownd_error_t ld_err ;
   afc_error_t afc_err ;
   afc_client_t afcclient ;
   
   ld_err = lockdownd_start_service(client, AFC_SERVICE_NAME, &lsd) ;
   
   if(ld_err == LOCKDOWN_E_SUCCESS ){
     printf("Started service: %s\n", AFC_SERVICE_NAME) ;
     
     afc_err = afc_client_new(device, lsd, &afcclient) ;
     if(afc_err == AFC_E_SUCCESS) {
       printf("Connected to afc service.\n") ;

       /* Get device information*/
       char **dev_info ;
       afc_err = afc_get_device_info(afcclient, &dev_info) ;
       if(afc_err == AFC_E_SUCCESS) {
	 printf("Got device info.\n") ;

	 char **pline = dev_info ;
	 while(pline && *pline) {
	   printf("%s\n", *pline) ;
	   ++pline ;
	 }
	 
	 afc_dictionary_free(dev_info) ;
       } else {
	 printf("Could not get device info.\n") ;
       }

       /* Read directory */
       const char root_path[] = "" ;
       char **dir_info ;
       afc_err = afc_read_directory(afcclient, root_path, &dir_info) ;
       if(afc_err == AFC_E_SUCCESS) {
	 printf("Got directory: %s\n", root_path) ;
	 afc_dictionary_free(dir_info) ;
       } else {
	 printf("Could not get directory\n") ;
       }
       

       afc_err = afc_client_free(afcclient) ;
     } else {
       printf("Could not connect to afc service.\n") ;
     }
   } else {
     printf("Could not start service: %d\n", ld_err) ;
   }  
  
}
 
