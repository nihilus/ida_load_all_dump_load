// load_all.cpp : Defines the entry point for the DLL application.
//

#pragma comment (lib,"ida.lib")

#define __NT__
#define __IDP__ 
#define MAXSTR 1024

#include <windows.h>
#include <commdlg.h>

#include <ida.hpp>
#include <idp.hpp>
#include <expr.hpp>
#include <bytes.hpp>
#include <loader.hpp>
#include <kernwin.hpp>
#include <diskio.hpp>
#include <pro.h>

unsigned long htodw(char *hex_string);

extern plugin_t PLUGIN;


int idaapi load_all_init(void){
  return PLUGIN_OK;
}

void idaapi load_all_term(void){}


// note that enumumerate_files is enumerating all files
// it doesn't return untill callback tells it to return!!
// so to know when 1st file is enumerated I will keep it
// in global variable and compare for it, in every enum
// after 1st file!!!

char	    *first_file_name;
unsigned int first_file;

int idaapi   enum_files_callback(const char *filename, void *ud){
	linput_t *li;
	segment_t *seg;
	char *file_in_dir;
	unsigned long address;
	char hex_number[9];

	if (!first_file){
		first_file_name = (char *)malloc(strlen(filename)+1);
		qstrncpy(first_file_name, filename, strlen(filename)+1);
		first_file = 1;
	}else{
		if (!strcmp(first_file_name, filename)){
			free(first_file_name);
			first_file = 0;
			return 1;
		}
	}

	
	file_in_dir = (char *)filename + strlen(filename);
	while (*file_in_dir != '\\')
		file_in_dir--;
	file_in_dir++;

	if (*file_in_dir != 'r') return 0;
	
	memset(hex_number, 0, 9);
	memcpy(hex_number, &file_in_dir[1], 8);
	address = htodw(hex_number);
	li = open_linput(filename, false);
	load_binary_file(filename, li, NEF_SEGS, 0, 0, address, 0);
	seg = getseg(address);
	set_segm_addressing(seg, 1);
	close_linput(li);	

	msg("load_all : %.08X - %s \n", address, filename);
	return 0;
}

void idaapi load_all_run(int arg)
{
	char *fileName, *dirName;
	unsigned long i;

	fileName = askfile_c(false, "*.dmp", "Select any file from .dmp folder - deroko of ARTeam");

	if (!fileName){
		msg("load_all : you didn't specify any file... Aborting...\n");
		return;
	}

	dirName = (char *)malloc(strlen(fileName) + 1);
	qstpncpy(dirName, fileName, strlen(fileName)+1);
	i = strlen(fileName);
	while (dirName[i] != '\\')
		i--;
	dirName[i] = 0;

	first_file = 0;
	while (!enumerate_files(0, 0, dirName, "*.dmp", enum_files_callback, 0));
	free(dirName);
	msg("load_all : executed successfully...\n");
}


plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  0,
  load_all_init,                 
  load_all_term,                 
  load_all_run,                  
  NULL,
  NULL, 
  "load_all",
  "Ctrl-0"
};
