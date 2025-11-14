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
	if (fgets(buffer, size, stdin) != NULL) 
	{
		if (strchr(buffer, '\n') == NULL) //if input is too long, clear extra chars
		{
			clear_input_buffer();
		}
		else
		{	//if input fits, remove newline only
			buffer[strcspn(buffer, "\n")] = 0;
		}
	}
}

int get_valid_student_id(void)
{
	char input[100];
	int id;

	while (1)
	{
		printf("P7-7:QUERY ID=");

		if (fgets(input, sizeof(input), stdin) == NULL) 
		{
			printf("Error, Try again");
			continue;
		}
		if (strchr(input, '\n') == NULL)
		{
			clear_input_buffer();
			printf("Invalid Input! Please follow paramters");
			continue;
		}

		//remove \n for safe inputs
		input[strcspn(input, "\n")] = 0;

		if (strlen(input) != 7)
		{
			printf("Invalid Input! Student ID not 7 digits, you entered %zu digits", strlen(input));
			continue;
		}
		int all_digits_check = 1;
		for (int i = 0; i < 7; i++)
		{
			if (input[i] < '0' || input[i] > '9')
			{
				all_digits_check = 0;
				break;
			}
		}
		if (!all_digits_check) //input not all digits
		{
			printf("Invalid Input! Student ID must contain digits from (0-9).\n");
			continue;
		}
		id = atoi(input); //ascii to integer
		break;
	}
	return id;
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
	printf("%-*s %-*s %-*s %s\n", //ensure its all left justified
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

	//Query a Record

	int query_record(const CMSdb* db) { 
		//handle error if they try to query without opened database
		if (!db->is_open) {
			printf("CMS:Error, No database is currently opened.\n");
			return 0;
		}
		if (db->record_count == 0)
		{
			printf("CMS:Error, No records in the database to query.\n");
			return 0;
		}

		while (1) {
			printf("\n=== Query Student Records===\n");
			printf("1. Query by ID\n");
			printf("2. Query by Name\n");
			printf("3. Query by Programme\n");
			printf("4. Query by Mark\n");
			printf("5. Return to Main Menu\n");

			char query_choice_input[4];
			get_string_input(query_choice_input, sizeof(query_choice_input), "Enter your choice (1-5): ");

			//ensure it only accepts 1 input and it must be a digit
			if (strlen(query_choice_input) != 1 || !isdigit(query_choice_input[0]))
			{
				printf("Invalid Input. Please enter exactly one number between %d-%d.\n", QUERY_CHOICES_MIN, QUERY_CHOICES_MAX);
				continue;
			}

			int query_choice = query_choice_input[0] - '0'; //convert to int
			if (query_choice < QUERY_CHOICES_MIN || query_choice > QUERY_CHOICES_MAX)
			{
				printf("Invalid Choice. Enter a number between %d-%d\n", QUERY_CHOICES_MIN, QUERY_CHOICES_MAX);
				continue;
			}

			switch (query_choice)
			{
				case 1:
					query_by_id(db);
					break;
				case 2:
					query_by_name(db);
					break;
				case 3:
					query_by_programme(db);
					break;
				case 4:
					query_by_mark(db);
					break;
				case 5:
					printf("Returning to Main Menu.\n");
					return 1;

			}
			break;
		}

		return 1;
	}

	void query_by_id(const CMSdb* db)
	{
		printf("\n=== Query By ID===\n");
		int search_id = get_valid_student_id();

		// Search for student
		int found_id = 0;
		for (int i = 0; i < db->record_count; i++) {
			if (db->records[i].id == search_id) {
				printf("CMS: The record with ID=%d is found in the data table.\n", search_id);
				printf("%-*s %-*s %-*s %s\n",
					DISPLAY_ID_WIDTH, "ID",
					DISPLAY_NAME_WIDTH, "Name",
					DISPLAY_PROGRAMME_WIDTH, "Programme",
					"Mark");
				printf("%-*d %-*s %-*s %.1f\n",
					DISPLAY_ID_WIDTH, db->records[i].id,
					DISPLAY_NAME_WIDTH, db->records[i].name,
					DISPLAY_PROGRAMME_WIDTH, db->records[i].programme,
					db->records[i].mark);
				found_id = 1;
				break; //only one record to have unique ID
			}
		}
		if (!found_id) {
			printf("CMS: The record with ID=%d does not exist.\n", search_id);
		}
	}
	void query_by_name(const CMSdb* db)
	{
		printf("\n=== Query By Name===\n");
		char search_name[MAX_NAME_LENGTH]; //40char include null char
		get_string_input(search_name, sizeof(search_name), "Enter name of Student to Search: ");

		//check if name input is empty
		if (strlen(search_name) == 0)
		{
			printf("Error: Please enter a Name.\n");
			return;
		}

		//create deep copy and convert to lowercase to not affect original data
		char search_lower[MAX_NAME_LENGTH];
		strcpy_s(search_lower, sizeof(search_lower), search_name);
		for (int i = 0; search_lower[i]; i++) {
			search_lower[i] = tolower(search_lower[i]);
		}

		int found = 0;
		for (int i = 0; i < db->record_count; i++)
		{
			char current_lower[MAX_NAME_LENGTH];
			strcpy_s(current_lower, sizeof(current_lower), db->records[i].name);
			//create copy of current record and convert to lowercase
			for (int j = 0; current_lower[j]; j++)
			{
				current_lower[j] = tolower(current_lower[j]);
			}
			//match programmes
			if (strstr(current_lower, search_lower) != NULL)
			{
				if (!found) 
				{
					printf("CMS: Records matching name \"%s\" found:\n", search_name);
					printf("%-*s %-*s %-*s %s\n",
						DISPLAY_ID_WIDTH, "ID",
						DISPLAY_NAME_WIDTH, "Name",
						DISPLAY_PROGRAMME_WIDTH, "Programme",
						"Mark");
					found = 1;
				}
				printf("%-*d %-*s %-*s %.1f\n",
					DISPLAY_ID_WIDTH, db->records[i].id,
					DISPLAY_NAME_WIDTH, db->records[i].name,
					DISPLAY_PROGRAMME_WIDTH, db->records[i].programme,
					db->records[i].mark);
			}
		}

		if (!found) 
		{
			printf("CMS: No records found matching name \"%s\".\n", search_name);
		}
		else 
		{
			printf("\nTotal records found: %d\n", found);
		}
	}
	void query_by_programme(const CMSdb* db)
	{
		{
			printf("\n=== Query By Programme===\n");
			char search_programme[MAX_PROGRAMME_LENGTH]; //40char include null char
			get_string_input(search_programme, sizeof(search_programme), "Enter Programme to Search: ");

			//check if Programme input is empty
			if (strlen(search_programme) == 0)
			{
				printf("Error: Please enter a Programme.\n");
				return;
			}

			//create deep copy and convert to lowercase to not affect original data
			char search_lower[MAX_PROGRAMME_LENGTH];
			strcpy_s(search_lower, sizeof(search_lower), search_programme);
			for (int i = 0; search_lower[i]; i++) {
				search_lower[i] = tolower(search_lower[i]);
			}

			int found = 0;
			for (int i = 0; i < db->record_count; i++)
			{
				char current_lower[MAX_PROGRAMME_LENGTH];
				strcpy_s(current_lower, sizeof(current_lower), db->records[i].programme);
				//create copy of current record and convert to lowercase
				for (int j = 0; current_lower[j]; j++)
				{
					current_lower[j] = tolower(current_lower[j]);
				}
				//match programmes
				if (strstr(current_lower, search_lower) != NULL)
				{
					if (!found)
					{
						printf("CMS: Records matching name \"%s\" found:\n", search_programme);
						printf("%-*s %-*s %-*s %s\n",
							DISPLAY_ID_WIDTH, "ID",
							DISPLAY_NAME_WIDTH, "Name",
							DISPLAY_PROGRAMME_WIDTH, "Programme",
							"Mark");
						found = 1;
					}
					printf("%-*d %-*s %-*s %.1f\n",
						DISPLAY_ID_WIDTH, db->records[i].id,
						DISPLAY_NAME_WIDTH, db->records[i].name,
						DISPLAY_PROGRAMME_WIDTH, db->records[i].programme,
						db->records[i].mark);
				}
			}

			if (!found) {
				printf("CMS: No records found matching programme \"%s\".\n", search_programme);
			}
			else {
				printf("\nTotal records found: %d\n", found);
			}
		}
	}

	void query_by_mark(const CMSdb* db)
	{
		printf("\n===Query By Mark===");
		char mark_input[6];//handles max 100.0 and null terminator
		get_string_input(mark_input, sizeof(mark_input), "Enter mark to search for (0-100, max 1 dp): ");
		//valide input formatting
		int valid_format = 1;
		int has_decimal = 0;
		int digits_after_decimal = 0;
		int digit_count = 0;

		for (int i = 0; mark_input[i] != '\0'; i++) 
		{
			if (mark_input[i] == '.') 
			{
				if (has_decimal) //if user enters several decimal points
				{
					valid_format = 0; //wrong format
					break;
				}
				has_decimal = 1;
			}
			else if (isdigit(mark_input[i])) //ensure its all digits
			{
				if (has_decimal) //checks if it has a decimal point
				{
					digits_after_decimal++;
					if (digits_after_decimal > 1) //more than 1dp
					{
						valid_format = 0;  //wrong format
						break;
					}
				}
				else {
					digit_count++;
				}
			}
			else {
				valid_format = 0;  // Invalid character
				break;
			}
		}

		// Check if it ends with a decimal point = wrong format.
		if (has_decimal && digits_after_decimal == 0) {
			valid_format = 0;  // Ends with decimal point 70.
		}
		if (digit_count == 0) {
			valid_format = 0;  // No digits before decimal .5
		}

		// Convert input to float and checks its within range of 0 to 100.
		float search_mark;
		if (!valid_format || sscanf_s(mark_input, "%f", &search_mark) != 1 ||
			search_mark < 0 || search_mark > 100) {
			printf("Invalid mark. Please enter a number between 0-100 with maximum 1 decimal place.\n");
			printf("Examples: 70, 70.0, 0.5\n");
			return;
		}

		// Search for matching marks
		int found = 0;
		for (int i = 0; i < db->record_count; i++) 
		{
			if (db->records[i].mark == search_mark) 
			{
				if (!found) {
					printf("\nCMS: Records with mark %.1f:\n", search_mark);
					printf("%-*s %-*s %-*s %s\n",
						DISPLAY_ID_WIDTH, "ID",
						DISPLAY_NAME_WIDTH, "Name",
						DISPLAY_PROGRAMME_WIDTH, "Programme",
						"Mark");
					found = 1;
				}
				printf("%-*d %-*s %-*s %.1f\n",
					DISPLAY_ID_WIDTH, db->records[i].id,
					DISPLAY_NAME_WIDTH, db->records[i].name,
					DISPLAY_PROGRAMME_WIDTH, db->records[i].programme,
					db->records[i].mark);
			}
		}

		if (!found) 
		{
			printf("CMS: No records found with mark %.1f.\n", search_mark);
		}
		else 
		{
			printf("\nTotal records found: %d\n", found);
		}
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