/*
* Course Management System (CMS)
* Implementation file - code for all CMS functions
*/

#include "cms.h"

/*
* clear input buffer
*/
void clear_input_buffer(void) {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void get_string_input(char* buffer, int size, const char* prompt) {
	printf("%s", prompt);
	if (fgets(buffer, size, stdin) != NULL) {
		// Remove newline character
		buffer[strcspn(buffer, "\n")] = 0;

	}
}
/*
* Initialize database with default values
* Sets up a clean, empty database ready for use
*/
void initialize_db(CMSdb* db) {
	//null pointer check
	if (db == NULL) {
		return;
	}

	db->record_count = 0;			//start with no records
	db->is_open = 0;				//database is not opened yet
	strcpy_s(db->current_filename, sizeof(db->current_filename), "Sample-CMS"); //No current file
}

/*
* Display the main menu to user
* shows all available options
*/
void show_menu(void) {
	printf("\n=== Course Management System Menu ===\n");
	printf("1. Open File\n");
	printf("2. Show All Records\n");
	printf("3. Insert Record\n");
	printf("4. Query Record\n");
	printf("5. Update Record\n");
	printf("6. Delete Record\n");
	printf("7. Save File\n");
	printf("8. Exit\n");
}

/*
* Handle user's menu choice and call appropriate function
*/
int handle_menu_choice(int choice, CMSdb* db) {
	switch (choice) {
	case 1: //Open file
		return open_file(db);
	case 2: //Show all records
		return show_all_records(db);
	case 3: // Insert new record
		return insert_record(db);
	case 4: // Query records
		return query_record(db);
	case 5: // Update record
		return update_record(db);
	case 6: // Delete Record
		return delete_record(db);
	case 7: // Save File
		return save_file(db);
	case 8: // Exit
		printf("Exiting CMS\n");
		return -1; // Special return value to exit program

	default:
		printf("Invalid choice. Please try again.\n");
		return 0; // Invalid choice
	}
}
/*
* Open and Load a database file
*/
int open_file(CMSdb* db) {
	if (db->is_open) {
		printf("CMS: Database is already opened.\n");
		return 1;
	}

	char filename[MAX_FILENAME_LENGTH];
	get_string_input(filename, sizeof(filename), "Enter filename to open: ");


	//Try to open the file
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("CMS: Failed to open file \"%s\"\n", filename);
		return 0;
	}

	//reset database before loading new data
	db->record_count = 0;

	char line[256];
	int header_passed = 0; //Flag to track when past headers

	// Read file line by line
	while (fgets(line, sizeof(line), file) != NULL && db->record_count < MAX_RECORDS)
	{
		//remove newline char
		line[strcspn(line, "\n")] = 0;

		// Skip empty lines
		if (strlen(line) == 0) {
			continue;
		}
		// Skip header lines - look for lines that DON'T start with numbers
		if (!isdigit(line[0])) {
			continue;
		}

		// Now parse student records (lines that start with numbers)
		StudentRecord* record = &db->records[db->record_count];

		int parsed = sscanf(line, "%d %49[^\t] %49[^\t] %f",
			&record->id, record->name, record->programme, &record->mark);

		if (parsed == 4) { //if all fields were parsed
			db->record_count++;
		}

	}
	fclose(file);

	if (db->record_count > 0) {
		db->is_open = 1;
		strcpy_s(db->current_filename, sizeof(db->current_filename), filename);
		printf("CMS: Successfully opened file \"%s\" is successfully opened\n", filename);
		printf("Loaded %d student records\n", db->record_count);
		return 1;
	}
	else {
		printf("CMS: File format error - could not find data header\n");
		return 0;
	}
}
/*
* Show all records in the database - PLACEHOLDER
*/
int show_all_records(const CMSdb* db) {
	if (!db->is_open) {
		printf("CMS: No database is currently opened.\n");
		return 0;
	}
	if (db->record_count == 0) {
		printf("CMS: No Student Records Found.\n");
		return 1;
	}
	//use predefined display widths to seperate columns
	printf("P7_7L SHOW ALL\n");
	printf("CMS: Here are all the records found in the table \"StudentRecords\".\n\n");
	printf("%-*s %-*s %-*s %s\n",
		DISPLAY_ID_WIDTH, "ID",
		DISPLAY_NAME_WIDTH, "Name",
		DISPLAY_PROGRAMME_WIDTH, "Programme",
		"Mark");
	for (int i = 0; i < db->record_count; i++) {
		printf("%-*d %-*s %-*s %.1f\n",
			DISPLAY_ID_WIDTH, db->records[i].id,
			DISPLAY_NAME_WIDTH, db->records[i].name,
			DISPLAY_PROGRAMME_WIDTH, db->records[i].programme,
			db->records[i].mark);
	}
	return 1;
}
/*
* Insert a new record - PLACEHOLDER
*/
	int insert_record(CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		printf("CMS: Insert record function - TO BE IMPLEMENTED\n");
		printf("CMS: Would add new student record here\n");
		return 1;
	}
	/*
	* Query/search records - PLACEHOLDER
	*/
	int query_record(const CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		printf("CMS: Query record function - TO BE IMPLEMENTED\n");
		printf("CMS: Would search for student records here\n");
		return 1;
	}
/*
* Update existing record - PLACEHOLDER
*/
	int update_record(CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		printf("CMS: Update record function - TO BE IMPLEMENTED\n");
		printf("CMS: Would update student record here\n");
		return 1;
	}
/*
* Delete record - PLACEHOLDER
*/
	int delete_record(CMSdb * db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		printf("CMS: Delete record function - TO BE IMPLEMENTED\n");
		return 1;
	}
/*
* Save file - PLACEHOLDER
*/
	int save_file(const CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		printf("CMS: Save file function - TO BE IMPLEMENTED\n");
		printf("CMS: Would save %d records to file\n", db->record_count);
		return 1;
	}