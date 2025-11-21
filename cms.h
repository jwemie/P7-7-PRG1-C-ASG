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
#define MAX_FILENAME_LENGTH 39 //max char for filename
#define MAX_ID_LENGTH 7
#define QUERY_CHOICES_MAX 5
#define QUERY_CHOICES_MIN 1
#define SORT_CHOICES_MAX 5
#define SORT_CHOICES_MIN 1
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
	StudentRecord backup_record[MAX_RECORDS]; // Backup of the last deleted record
	int backup_count; // Number of records in backup
	int can_undo; // Flag: 1 if undo available, 0 if undo not available
	char last_operation[50]; // Description of last operation (e.g., "DELETE")
} UndoInfo;

typedef struct {
	StudentRecord records[MAX_RECORDS]; //array of student records
	int record_count; // no. of records in db
	int is_open; //flag: 0  = closed , 1 = open
	char current_filename[100]; //name of the currently opened file
	UndoInfo undo; //undo function
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
int get_valid_student_id(void);

//Core functions (called by menu handler)
int open_file(CMSdb *db); 
int show_all_records(const CMSdb *db);
int insert_record(CMSdb *db);
int query_record(const CMSdb *db);
void query_by_id(const CMSdb *db);
void query_by_name(const CMSdb *db);
void query_by_programme(const CMSdb* db);
void query_by_mark(const CMSdb* db);
int update_record(CMSdb *db);
int delete_record(CMSdb *db);
int save_file(const CMSdb *db);
void save_undo_state(CMSdb* db, const char* operation);
int undo_last_operation(CMSdb* db);
int sort_records(CMSdb *db); //sort functions
void sort_by_id_asc(CMSdb *db);
void sort_by_id_desc(CMSdb *db);
void sort_by_mark_asc(CMSdb *db);
void sort_by_mark_desc(CMSdb *db);
#endif

