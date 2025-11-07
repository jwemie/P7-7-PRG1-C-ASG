/*
* P7-7 Course Management System Project
* "blueprint" of the project
*/

/*
* Create Header Guards - prevents multiple inclusions, prevent redefinition errors during compilation
*/
#ifndef CMS_H /*if not defined*/
#define CMS_H 

/*include standard lib*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Constant var*/
#define MAX_NAME_LENGTH 40//max char for names
#define MAX_PROGRAMME_LENGTH 40 //max char for programme names
#define MAX_RECORDS 1000 //max no. of entries in the CMS
#define MAX_FILENAME_LENGTH 100 //max char for filename
#define MAX_ID_LENGTH 10 
/*Display Constant Var*/
#define DISPLAY_ID_WIDTH 10
#define DISPLAY_NAME_WIDTH 40
#define DISPLAY_PROGRAMME_WIDTH 40
#define DISPLAY_MARK_WIDTH 10

/*
*StudentRecord structure
* define parameters of how each record looks like, similar to template
*/
typedef struct {
	int id; //StudentID (e.g., 2301234)
	char name[MAX_NAME_LENGTH]; //student name (49 chars + \n)
	char programme[MAX_PROGRAMME_LENGTH]; //programe name (99 chars + \n)
	float mark; //marks (0.0-100.0)
} StudentRecord;

typedef struct {
	StudentRecord records[MAX_RECORDS]; //array of student records
	int record_count; // no. of records in db
	int is_open; //flag: 0  = closed , 1 = open
	char current_filename[100]; //name of the currently opened file
} CMSdb;

//Function Declaration

//public interface functions
void initialize_db(CMSdb *db);
void show_menu(void);
int handle_menu_choice(int choice, CMSdb *db);

// input validation functions
int get_valid_menu_choice(void);
void clear_input_buffer(void);
void get_string_input(char *buffer, int size, const char* prompt);

//Core functions (called by menu handler)
int open_file(CMSdb *db); 
int show_all_records(const CMSdb *db);
int insert_record(CMSdb *db);
int query_record(const CMSdb *db);
int update_record(CMSdb *db);
int delete_record(CMSdb *db);
int save_file(const CMSdb *db);

#endif

