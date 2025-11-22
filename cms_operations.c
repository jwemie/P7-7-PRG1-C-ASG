#define _CRT_SECURE_NO_WARNINGS
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
			printf("Error, Try again\n");
			continue;
		}
		if (strchr(input, '\n') == NULL)
		{
			clear_input_buffer();
			printf("Invalid Input! Please follow paramters\n");
			continue;
		}

		//remove \n for safe inputs
		input[strcspn(input, "\n")] = 0;

		if (strlen(input) != 7)
		{
			printf("Invalid Input! Student ID not 7 digits, you entered %zu digits\n", strlen(input));
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
*/
void initialize_db(CMSdb* db) {
	//null pointer check
	if (db == NULL) {
		return;
	}

	db->record_count = 0;			//start with no records
	db->is_open = 0;				//database is not opened yet flag
	strcpy_s(db->current_filename, sizeof(db->current_filename),""); //No current file
}

//check for header lines
int is_header_line(const char* line) 
{
	// Common header patterns
	if (strstr(line, "ID") != NULL &&
		(strstr(line, "Name") != NULL || strstr(line, "NAME") != NULL)) {
		return 1;
	}

	// Lines starting with non-digits are likely headers
	if (!isdigit(line[0])) {
		return 1;
	}

	return 0;
}
/*
* Parse student record from line
*/
int parse_student_record(const char* line, StudentRecord* record) {
	int parsed = sscanf_s(line, "%d %49[^\t] %49[^\t] %f",
		&record->id,
		record->name, (rsize_t)sizeof(record->name),
		record->programme, (rsize_t)sizeof(record->programme),
		&record->mark);

	if (parsed != 4) {
		// Tab-separated failed - this format is required
		return 0;
	}
	sanitize_input_fields(record);
	return parsed;
}
/*
* Valid Student Record
*/
int valid_student_record(const StudentRecord* record) {
	int valid = 1;

	// Validate ID 
	if (record->id < MIN_VALID_ID || record->id > MAX_VALID_ID) 
	{
		printf("  - Invalid ID: %d (must be 7 digits between %d-%d)\n",
			record->id, MIN_VALID_ID, MAX_VALID_ID);
		valid = 0;
	}

	// Validate Name
	if (strlen(record->name) == 0) {
		printf("  - Name cannot be empty\n");
		valid = 0;
	}
	else if (strlen(record->name) > MAX_NAME_LENGTH) {
		printf("  - Name too long: '%s' (%zu characters, max %d)\n",
			record->name, strlen(record->name), MAX_NAME_LENGTH);
		valid = 0;
	}

	// Validate programme
	if (strlen(record->programme) == 0) {
		printf("  - Programme cannot be empty\n");
		valid = 0;
	}
	else if (strlen(record->programme) > MAX_PROGRAMME_LENGTH) {
		printf("  - Programme too long: '%s' (%zu characters, max %d)\n",
			record->programme, strlen(record->programme), MAX_PROGRAMME_LENGTH);
		valid = 0;
	}

	// Validate mark (0-100 range)
	if (record->mark < 0 || record->mark > 100) {
		printf("  - Invalid mark: %.1f (must be between 0-100)\n", record->mark);
		valid = 0;
	}

	return valid;
}
int detect_file_format(const char* filename) {
	FILE* test_file = fopen(filename, "r");
	if (!test_file) return 0;

	char line[256];
	int tab_count = 0;
	int data_lines = 0;

	while (fgets(line, sizeof(line), test_file) && data_lines < 5) {
		if (is_header_line(line)) continue;

		// Count tabs in potential data lines
		for (int i = 0; line[i]; i++) {
			if (line[i] == '\t') tab_count++;
		}
		data_lines++;
	}
	fclose(test_file);

	// If we found data lines with 3 tabs, format is good
	return (data_lines > 0 && tab_count >= data_lines * 3);
}
void sanitize_input_fields(StudentRecord* record) {
	// Remove leading & trailing whitespace from name and programme
	char* fields[] = { record->name, record->programme };

	for (int i = 0; i < 2; i++) {
		char* str = fields[i];
		if (!str) continue;

		//cut leading spaces
		char* start = str;
		while (*start && isspace((unsigned char)*start)) start++;

		//cut trailing spaces  
		char* end = start + strlen(start) - 1;
		while (end > start && isspace((unsigned char)*end)) end--;

		//move trimmed string
		if (start != str) {
			memmove(str, start, end - start + 1);
		}
		str[end - start + 1] = '\0';
	}
}
/*
* Undo File
*/
void save_undo_state(CMSdb* db, const char* operation)
{
	// Copy all records to backup_record array (not a single struct)
	for (int i = 0; i < db->record_count; i++)
	{
		db->undo.backup_record[i] = db->records[i];

	}
	db->undo.backup_count = db->record_count;
	db->undo.can_undo = 1;
	strcpy_s(db->undo.last_operation, sizeof(db->undo.last_operation), operation);
}

/*
* Display the main menu to user
* shows all available options
*/
void show_menu(void) {
	printf("\n=== Course Management System Menu ===\n");
	printf("1. Open File\n");
	printf("2. Show All Records\n");
	printf("3. Sort Records\n");
	printf("4. Insert Record\n");
	printf("5. Query Record\n");
	printf("6. Update Record\n");
	printf("7. Delete Record\n");
	printf("8. Undo\n");
	printf("9. Save File\n");
	printf("10. Exit\n");
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
	case 3: //sorting function
		return sort_records(db);
	case 4: // Insert new record
		return insert_record(db);
	case 5: // Query records
		return query_record(db);
	case 6: // Update record
		return update_record(db);
	case 7: // Delete Record
		return delete_record(db);
	case 8: // Undo Functions on records
		return undo_last_operation(db);
	case 9: // Save File
		return save_file(db);
	case 10: // Exit
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
		printf("DEBUG: fopen failed! Error: ");
		perror("");
		printf("CMS: Failed to open file \"%s\"\n", filename);
		return 0;
	}

	//reset database before loading new data
	db->record_count = 0;

	char line[MAX_LINE_LENGTH];
	int line_number = 0;
	int data_lines_found = 0;
	int data_lines_loaded = 0;
	int header_lines_skipped = 0;
	printf("CMS: Reading file \"%s\"...\n", filename);


	// Read file line by line
	while (fgets(line, sizeof(line), file) != NULL && db->record_count < MAX_RECORDS)
	{
		line_number++;


		//remove newline char
		line[strcspn(line, "\n")] = 0;


		// Skip empty lines
		if (strlen(line) == 0) {
			continue;
		}
		//Detect and Skip header lines
		if (is_header_line(line))
		{
			printf("CMS: Skipping Header Lines %d: %s\n", line_number, line);
			header_lines_skipped++;
			continue;
		}

		//Parse student records (lines that start with numbers)
		StudentRecord* record = &db->records[db->record_count];

		int parsed = sscanf_s(line, "%d %49[^\t] %49[^\t] %f",
			&record->id,
			record->name, (rsize_t)sizeof(record->name),
			record->programme, (rsize_t)sizeof(record->programme),
			&record->mark);

		if (parsed == 4) { //if all fields were parsed
			//validate parsed data
			if (valid_student_record(record))
			{
				//duplicate check
				int is_duplicate = 0;
				for (int i = 0; i < db->record_count; i++) {
					if (db->records[i].id == record->id) {
						printf("CMS: Warning - Duplicate ID %d on line %d, skipping\n", record->id, line_number);
						is_duplicate = 1;
						break;
					}
				}
				if (is_duplicate) {
					continue; // Skip this duplicate record
				}
				db->record_count++;
				data_lines_loaded++;
			}

			else
			{
				printf("CMS: Invalid Data on Line %d: %s\n", line_number, line);
			}
		}
		else
		{
			printf("CMS: Could not parse line %d (Needs 4 fields of data, found %d): %s\n", line_number, parsed, line);
		}
		data_lines_found++;

	}
	fclose(file);
	//file parse stats
	printf("CMS: File processing complete:\n");
	printf("  - Lines processed: %d\n", line_number);
	printf("  - Header lines skipped: %d\n", header_lines_skipped);
	printf("  - Data lines found: %d\n", data_lines_found);
	printf("  - Valid records loaded: %d\n", data_lines_loaded);
	//empty file detection
	if (data_lines_found == 0) {
		printf("CMS: Error - No data records found in file\n");
		printf("CMS: File may be empty or contain only headers\n");
		return 0;
	}

	if (db->record_count > 0) 
	{
		db->is_open = 1;
		strcpy_s(db->current_filename, sizeof(db->current_filename), filename);
		printf("CMS: Successfully opened file \"%s\" is successfully opened\n", filename);
		printf("Loaded %d student records\n", db->record_count);
		return 1;
	}
	else {
		printf("CMS: Error - No Valid data found in file.\n");
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
* Insert a new record
*/
int insert_record(CMSdb* db) {
	// check if other database is opened
	if (!db->is_open) {
		printf("CMS: No database is currently opened.\n");
		return 0;
	}
	save_undo_state(db, "INSERT");
	// check whether the database has reached the maximum record limit and call MAX_RECORDS function from cms.h
	if (db->record_count >= MAX_RECORDS) {
		printf("CMS: Database is full. Cannot insert more records.\n");
		db->undo.can_undo = 0;
		return 0;
	}

	char buffer[100]; // temporarily store users' input
	int newID;
	StudentRecord* record = &db->records[db->record_count];

	// Input Student ID
	while (1) {
		printf("Enter Student ID: ");
		// read the users' input into buffer
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
			printf("Input error. Try again.\n");
			continue;
		}

		// remove newline (ensure inputs has no new line)
		buffer[strcspn(buffer, "\n")] = 0;

		// Check length and call "MAX_ID_LENGTH" function from cms.h
		if (strlen(buffer) != MAX_ID_LENGTH) {
			printf("Error: ID must be %d characters.\n", MAX_ID_LENGTH);
			continue;
		}

		// Check all characters must be digits
		int all_digits = 1;
		for (int i = 0; i < strlen(buffer); i++) {
			if (!isdigit((unsigned char)buffer[i])) {
				all_digits = 0;
				break;
			} // use "unsigned char" to make sure isdigit work correctly
		}
		if (!all_digits) {
			printf("Error: ID must contain digits only (no letters, no symbols).\n");
			continue;
		}

		// Convert string to integer
		newID = atoi(buffer);

		// Check if ID already exists
		int exists = 0;
		for (int i = 0; i < db->record_count; i++) {
			if (db->records[i].id == newID) {
				printf("Error: Student ID already exists.\n");
				exists = 1;
				break;
			}
		}
		// if user input ID already exists, go back and re-enter ID again
		if (exists) continue;

		record->id = newID;
		break;
	}


	// Input Student Name
	while (1) {
		// Store users' input "Name"
		get_string_input(record->name, sizeof(record->name), "Enter Student Name: ");

		// Check the length of the name by calling "MAX_NAME_LENGTH" function from cms.h
		if (strlen(record->name) > MAX_NAME_LENGTH) {
			printf("Error: Name cannot exceed %d characters.\n", MAX_NAME_LENGTH);
			continue;
		}

		// Check the characters: only allow letters and space
		int valid = 1;
		for (int i = 0; i < strlen(record->name); i++) {
			char c = record->name[i]; // stored in one variable and easier to check
			if (!(isalpha((unsigned char)c) || c == ' ')) {
				valid = 0;
				break;
			} // use "unsigned char" to make sure isalpha work correctly
		}

		if (!valid) {
			printf("Error: Name can only contain English letters and spaces (no digits, no symbols).\n");
			continue; // if failed to input "Name", go back and re-enter again
		}

		break;
	}

	// Input Programme Name
	while (1) {
		get_string_input(record->programme, sizeof(record->programme), "Enter Programme: ");
		// Check if the length of the input by calling function "MAX_PROGRAMME_LENGTH" from cms.h
		if (strlen(record->programme) > MAX_PROGRAMME_LENGTH) {
			printf("Error: Programme cannot exceed %d characters.\n", MAX_PROGRAMME_LENGTH);
			continue;
		} // if longer than the limit, go back and re-enter again

		// Check if the input contains digit
		int has_digit = 0;
		for (int i = 0; i < strlen(record->programme); i++) {
			if (isdigit((unsigned char)record->programme[i])) {
				has_digit = 1;
				break;
			}
		}
		if (has_digit) {
			printf("Error: Programme cannot contain digits.\n");
			continue;
		} // if contains digit, go back and re-enter again
		break;
	}

	// Input Mark
	int valid_mark = 0;

	while (!valid_mark) {

		printf("Enter Mark (integer or 1 decimal place): ");

		if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
			printf("Input error. Try again.\n");
			continue; // if error, go back to re-enter number again
		}

		buffer[strcspn(buffer, "\n")] = 0; // change to string make sure the input string will not include "\n"
		int len = strlen(buffer);
		int dot_count = 0, dot_pos = -1;

		// check chars
		int numeric_ok = 1;
		for (int i = 0; i < len; i++) {

			// if find decimal, then record the position
			if (buffer[i] == '.') {
				dot_count++;
				dot_pos = i;
			}

			// if not a digit or decimal, break "check chars" loop
			else if (!isdigit((unsigned char)buffer[i])) {
				numeric_ok = 0;
				break;
			}
		}

		if (!numeric_ok) {
			printf("Error: Mark must be numeric.\n");
			continue;
		}
		if (dot_count > 1) {
			printf("Error: Mark must have only one decimal point.\n");
			continue;
		}

		// only allow 1 dp. eg: 24.6 is correct, 24.77 is incorrect
		if (dot_count == 1) {
			if (dot_pos != len - 2) {
				printf("Error: Must have exactly one decimal place.\n");
				continue;
			}

			// if 0 < input number < 1, must have a leading zero. eg: 0.5 is correct, .5 is wrong
			if (dot_pos == 0) {
				printf("Error: Number < 1 must have leading zero.\n");
				continue;
			}
		}

		// check if the number in correct range
		double mark = atof(buffer); // change the string to double
		if (mark < 0 || mark > 100) {
			printf("Error: Mark must be between 0 and 100.\n");
			continue;
		}

		record->mark = mark;
		valid_mark = 1;
	}
	db->record_count++;
	printf("CMS: You can see UNDO (Option 8) to revert this insertion if needed.\n");
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
* Update existing record
*/
	int update_record(CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}
		if (db->record_count == 0) {
			printf("CMS: No records available to update.\n");
			return 0;
		}

		// Get student ID to update with proper validation
		int studentID;
		char id_input[100];

		while (1) {
			printf("=== Update Student Records ===\n");
			printf("P7-7 UPDATE ID: ");

			if (fgets(id_input, sizeof(id_input), stdin) == NULL) {
				printf("Input error. Try again.\n");
				continue;
			}

			// Remove newline character
			id_input[strcspn(id_input, "\n")] = 0;

			// Check if input is empty
			if (strlen(id_input) == 0) {
				printf("Error: ID cannot be empty.\n");
				continue;
			}

			// Check all characters must be digits FIRST
			int all_digits = 1;
			for (int i = 0; i < strlen(id_input); i++) {
				if (id_input[i] < '0' || id_input[i] > '9') {
					all_digits = 0;
					break;
				}
			}
			if (!all_digits) {
				printf("Error: ID must contain digits only (no letters, no symbols).\n");
				continue;
			}

			// THEN Check length - must be exactly 7 digits
			if (strlen(id_input) != MAX_ID_LENGTH) {
				printf("Error: ID must be %d digits.\n", MAX_ID_LENGTH);
				continue;
			}

			// Convert string to integer
			studentID = atoi(id_input);
			break;
		}

		int recordsindex = -1; //check whether the student ID already exist
		for (int i = 0; i < db->record_count; i++) {
			if (db->records[i].id == studentID) {
				recordsindex = i;
				break;
			}
		}
		// if no record is found with the student id provided
		if (recordsindex == -1) {
			printf("CMS: The record with ID = %d does not exist.\n", studentID);
			return 0;
		}

		StudentRecord* record = &db->records[recordsindex];

		//current record
		printf("\nCurrent record details:\n");
		printf("Student ID: %d\n", record->id);
		printf("Name: %s\n", record->name);
		printf("Programme: %s\n", record->programme);
		printf("Mark: %.1f\n\n", record->mark);

		//select which area to update
		int choices;
		char choice_input[4]; // ✅ Reduced size since we only need 1 character

		while (1) {
			printf("Select field to update:\n");
			printf("1. Update Name\n");
			printf("2. Update Programme\n");
			printf("3. Update Mark\n");

			// ✅ IMPROVEMENT 2: Use get_string_input() for menu choices
			get_string_input(choice_input, sizeof(choice_input), "Please enter your choice (1-3): ");

			// validate choice input
			if (strlen(choice_input) != 1 || !isdigit(choice_input[0])) {
				printf("Invalid choice. Please enter a single digit (1-3).\n");
				continue;
			}

			choices = choice_input[0] - '0';

			if (choices < 1 || choices > 3) {
				printf("Invalid choice. Please enter a number between 1-3.\n");
				continue;
			}
			break;
		}

		// Save current state for undo functionality before updating
		//Store the old values so we can revert if needed
		save_undo_state(db, "UPDATE");

		//update the choices
		switch (choices) {
		case 1: //update name
		{
			char updatedname[MAX_NAME_LENGTH];
			int valid_name = 0;

			while (!valid_name) {
				get_string_input(updatedname, sizeof(updatedname), "Enter updated name: ");

				// check if name is empty
				if (strlen(updatedname) == 0) {
					printf("Error: Name cannot be empty.\n");
					continue;
				}

				// check the length of the name
				if (strlen(updatedname) > MAX_NAME_LENGTH) {
					printf("Error: Name cannot exceed %d characters.\n", MAX_NAME_LENGTH);
					continue;
				}

				// check the characters: only allow letters and space
				int valid = 1;
				for (int i = 0; i < strlen(updatedname); i++) {
					char c = updatedname[i];
					if (!(isalpha((unsigned char)c) || c == ' ')) {
						valid = 0;
						break;
					}
				}

				if (!valid) {
					printf("Error: Name can only contain English letters and spaces (no digits, no symbols).\n");
					continue;
				}

				valid_name = 1;
			}

			strcpy_s(record->name, sizeof(record->name), updatedname);
			printf("CMS: The record with ID = %d is successfully updated.\n", studentID);
		} break;

		case 2: //update programme
		{
			char updatedprog[MAX_PROGRAMME_LENGTH];
			int valid_programme = 0;

			while (!valid_programme) {
				get_string_input(updatedprog, sizeof(updatedprog), "Enter updated programme: ");

				// check if programme is empty
				if (strlen(updatedprog) == 0) {
					printf("Error: Programme cannot be empty.\n");
					continue;
				}

				// check the length of the programme
				if (strlen(updatedprog) > MAX_PROGRAMME_LENGTH) {
					printf("Error: Programme cannot exceed %d characters.\n", MAX_PROGRAMME_LENGTH);
					continue;
				}

				// check if the input contains only letters and spaces
				int valid = 1;
				for (int i = 0; i < strlen(updatedprog); i++) {
					char c = updatedprog[i];
					if (!(isalpha((unsigned char)c) || c == ' ')) {
						valid = 0;
						break;
					}
				}

				if (!valid) {
					printf("Error: Programme can only contain English letters and spaces (no digits, no symbols).\n");
					continue;
				}

				valid_programme = 1;
			}

			strcpy_s(record->programme, sizeof(record->programme), updatedprog);
			printf("CMS: The record with ID = %d is successfully updated.\n", studentID);
		} break;

		case 3: // update mark
		{
			char mark_input[100];
			float updatedmarks;
			int validmark = 0;

			while (!validmark) {
				printf("Enter updated mark (0-100, max 1 decimal place): ");

				if (fgets(mark_input, sizeof(mark_input), stdin) == NULL) {
					printf("Input error. Try again.\n");
					continue;
				}

				// remove newline character
				mark_input[strcspn(mark_input, "\n")] = 0;

				// validate mark format
				int valid_format = 1;
				int has_decimal = 0;
				int digits_after_decimal = 0;
				int digit_count = 0;

				for (int i = 0; mark_input[i] != '\0'; i++) {
					if (mark_input[i] == '.') {
						if (has_decimal) {
							valid_format = 0; // multiple decimal points
							break;
						}
						has_decimal = 1;
					}
					else if (isdigit((unsigned char)mark_input[i])) {
						if (has_decimal) {
							digits_after_decimal++;
							if (digits_after_decimal > 1) {
								valid_format = 0; // more than 1 decimal place
								break;
							}
						}
						else {
							digit_count++;
						}
					}
					else {
						valid_format = 0; // invalid character
						break;
					}
				}

				// check if it ends with a decimal point
				if (has_decimal && digits_after_decimal == 0) {
					valid_format = 0; // ends with decimal point (e.g., "90.")
				}

				// check if there are digits before decimal (for numbers < 1)
				if (has_decimal && digit_count == 0) {
					valid_format = 0; // No digits before decimal (e.g., ".5")
				}

				// check if there are any digits at all
				if (digit_count == 0 && digits_after_decimal == 0) {
					valid_format = 0; // No digits entered
				}

				// convert to float and check range
				if (!valid_format || sscanf_s(mark_input, "%f", &updatedmarks) != 1 ||
					updatedmarks < 0 || updatedmarks > 100) {
					printf("Invalid mark. Please enter a number between 0-100 with maximum 1 decimal place.\n");
					printf("Examples: 70, 70.0, 0.5\n");
					continue;
				}

				validmark = 1;
				record->mark = updatedmarks;
				printf("CMS: The record with ID = %d is successfully updated.\n", studentID);
			}
			break;
		}
		}

		//display updated record
		printf("\nUpdated record:\n");
		printf("Student ID: %d\n", record->id);
		printf("Name: %s\n", record->name);
		printf("Programme: %s\n", record->programme);
		printf("Mark: %.1f\n", record->mark);

		//undo notification
		printf("CMS: You can UNDO to restore the old values if needed.\n");
		return 1;
	}

/*
* Delete record
*/
	int delete_record(CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}

		// Check if there are any records to delete
		if (db->record_count == 0) {
			printf("CMS: No records in database to delete.\n");
			return 0;
		}

		// Prompt user for Student ID to delete
		int id_to_delete;
		printf("P7-7: DELETE\n");
		printf("CMS: Enter Student ID to delete: ");

		// Read the ID from user
		if (scanf("%d", &id_to_delete) != 1) {
			printf("CMS: Invalid ID format.\n");
			clear_input_buffer();
			return 0;
		}
		clear_input_buffer();

		// Validate it's a 7-digit ID
		if (id_to_delete < 1000000 || id_to_delete > 9999999) {
			printf("CMS: Student ID must be 7 digits (1000000-9999999).\n");
			return 0;
		}

		// Search for the record with the given ID
		int found_index = -1;  // -1 means "not found"

		// We will loop through all records to find matching ID
		for (int i = 0; i < db->record_count; i++) {
			if (db->records[i].id == id_to_delete) {
				found_index = i;  // Record found at index i
				break;  // Exit loop since we found the record
			}
		}

		//Check if record was found
		if (found_index == -1) {
			// If record not found
			printf("CMS: The record with ID=%d does not exist.\n", id_to_delete);
			db->undo.can_undo = 0; // Cancel undo since no deletion occurred 
			return 0;

		}

		//If record found, display details and ask for confirmation
		printf("CMS: Found student: %s (ID: %d)\n",
			db->records[found_index].name, //display name and ID of record to be deleted
			id_to_delete);
		printf("Are you sure you want to delete this record? (Y/N): ");


		// Get users confirmation
		char confirmation;
		scanf(" %c", &confirmation);  // Space before %c skips whitespace
		clear_input_buffer();

		//Check if user confirmed deletion
		if (confirmation == 'N' || confirmation == 'n') { //If user selects "N" / "n" then the deletion is cancelled
			printf("CMS: The deletion is cancelled.\n");
			db->undo.can_undo = 0; // Cancel undo since no deletion occurred
			return 0;
		}

		//Check if user input other things that are not Y/y or N/n then print out invalid input message
		if (confirmation != 'Y' && confirmation != 'y') {
			printf("CMS: Invalid input. The deletion is cancelled.\n");
			db->undo.can_undo = 0; // Cancel undo since no deletion occurred
			return 0;
		}
		
			// Save current state for undo functionality before deleting 
			save_undo_state(db, "DELETE");

		//If user confirmed deletion, proceed to delete the record
		//How we delete the record is by shifting all records after found_index one position to the left
		//Example: If we delete record at index 2, we copy record at index 3 to index 2, index 4 to index 3, etc.

		for (int i = found_index; i < db->record_count - 1; i++) {
			// Copy the next record to current position
			db->records[i] = db->records[i + 1];
		}

		// Decrease the record count
		db->record_count--;

		// Notify user of successful deletion
		printf("CMS: The record with ID=%d is successfully deleted.\n", id_to_delete);
		printf("CMS: You can UNDO (Option 8) to undo the deletion.\n");

		return 1;  //The deletion was successful
	}
/*
* Save file
*/
	int save_file(const CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}

		// Check if there are any records to save
		if (db->record_count == 0) {
			printf("CMS: No records to save (Database is empty).\n");
			return 0;
		}

		//Open the file for writing (this will overwrite the existing file)
		FILE* file = fopen(db->current_filename, "w");

		// Check if file open successfully
		if (file == NULL) {
			printf("CMS: Error - Cannot save to file \"%s\"\n", db->current_filename);
			printf("CMS: Please check if the file is not opened in another program.\n");
			return 0;
		}

		//Write all student records to the file
		for (int i = 0; i < db->record_count; i++) {
			// Write each record with tab-separated values
			fprintf(file, "%d\t%s\t%s\t%.1f\n",
				db->records[i].id,
				db->records[i].name,
				db->records[i].programme,
				db->records[i].mark);
		}

		//Close the file
		fclose(file);

		//Clear undo state (Since changes are now permanent
		//After saving, there's nothing to undo - all changes are committed
		((CMSdb*)db)->undo.can_undo = 0;
		strcpy_s(((CMSdb*)db)->undo.last_operation,
			sizeof(((CMSdb*)db)->undo.last_operation), "");

		//Print display success message
		printf("CMS: The database file \"%s\" is successfully saved.\n", db->current_filename);
		printf("CMS: %d record(s) saved to file.\n", db->record_count);
		printf("CMS: All changes have been committed (cannot be undone after save).\n");

		return 1;
	}
/*
* Undo Function
*/
	int undo_last_operation(CMSdb* db) {
		if (!db->is_open) {
			printf("CMS: No database is currently opened.\n");
			return 0;
		}

		if (!db->undo.can_undo) { // No undo available
			printf("CMS: Nothing to undo. No operation has been performed yet.\n");
			return 0;
		}

		printf("CMS: Undoing last operation: %s\n", db->undo.last_operation); // Display last operation

		for (int i = 0; i < db->undo.backup_count; i++) { // Restore records from backup
			db->records[i] = db->undo.backup_record[i]; // Copy each record back
		}

		db->record_count = db->undo.backup_count; // Restore record count
		db->undo.can_undo = 0; // Disable further undo until next delete
		strcpy_s(db->undo.last_operation, sizeof(db->undo.last_operation), ""); // Clear last operation description

		printf("CMS: Undo successful! Database restored to previous state.\n"); // Notify user that the undo was successful
		printf("CMS: Current record count: %d\n", db->record_count); // Show current record count

		return 1;
	}
/*
* Sort records by different criteria
*/
	int sort_records(CMSdb* db)
	{
		if (!db->is_open)
		{
			printf("CMS: No database is currently opened.\n");
			return 0;
		}

		if (db->record_count == 0)
		{
			printf("CMS: No records available to sort.\n");
			return 0;
		}

		while (1)
		{
			printf("\n=== Sort Records ===\n");
			printf("1. Sort by ID (Ascending A-Z)\n");
			printf("2. Sort by ID (Descending Z-A)\n");
			printf("3. Sort by Mark (Ascending 0.0 - 100.0)\n");
			printf("4. Sort by Mark (Descending 100.0 - 0.0\n");
			printf("5. Return to Main Menu\n");

			char sort_choice_input[4];
			get_string_input(sort_choice_input, sizeof(sort_choice_input), "Enter your choice (1-5): ");

			//input validation
			if (strlen(sort_choice_input) != 1 || !isdigit(sort_choice_input[0]))
			{
				printf("Invalid Input. Please enter exactly one number from %d-%d.\n", SORT_CHOICES_MIN, SORT_CHOICES_MAX);
				continue;
			}
			int sort_choice = sort_choice_input[0] - '0';
			if (sort_choice < SORT_CHOICES_MIN || sort_choice > SORT_CHOICES_MAX)
			{
				printf("Invalid Choice. Enter a number between %d-%d.\n", SORT_CHOICES_MIN, SORT_CHOICES_MAX);
				continue;
			}

			switch (sort_choice)
			{
				case 1:
					sort_by_id_asc(db);
					break;
				case 2:
					sort_by_id_desc(db);
					break;
				case 3:
					sort_by_mark_asc(db);
					break;
				case 4:
					sort_by_mark_desc(db);
					break;
				case 5:
					printf("Returning to Main Menu.\n");
					return 1;
			}
			printf("\nRecords sorted Successfully.\n");
			show_all_records(db);
			break;
		}
		return 1;
	}
	//comparison functions for qsort
	int compare_id_asc(const void* a, const void* b)
	{
		//if record A id smaller than record B, return negative, A comes before B
		//if record B is larger than A, return positive, A comes after B.
		const StudentRecord* recordA = (const StudentRecord*)a;
		const StudentRecord* recordB = (const StudentRecord*)b;
		return (recordA->id - recordB->id);
	}

	int compare_id_desc(const void* a, const void* b)
	{ 
		//if record B is smaller than record B, return negative, A comes before B
		//if record A is larger, return positive, A comes after B
		const StudentRecord* recordA = (const StudentRecord*)a;
		const StudentRecord* recordB = (const StudentRecord*)b;
		return (recordB->id - recordA->id);
	}
	int compare_mark_asc(const void* a, const void* b)
	{
		//if A mark < B mark, A before B
		//if A mark > B mark, A comes after B
		const StudentRecord* recordA = (const StudentRecord*)a;
		const StudentRecord* recordB = (const StudentRecord*)b;
		if (recordA->mark < recordB->mark) return -1;
		if (recordA->mark > recordB->mark) return 1;
		return 0;
	}
	int compare_mark_desc(const void* a, const void* b)
	{
		//if mark A > mark B, A before B
		//if mark A < mark B, A comes after B
		const StudentRecord* recordA = (const StudentRecord*)a;
		const StudentRecord* recordB = (const StudentRecord*)b;
		if (recordA->mark > recordB->mark) return -1;
		if (recordA->mark < recordB->mark) return 1;
		return 0;
	}
	//implemented comparison functions
	void sort_by_id_asc(CMSdb* db)
	{
		qsort(db->records, db->record_count, sizeof(StudentRecord), compare_id_asc);
		printf("Sorted by ID (Ascending)\n");
	}
	void sort_by_id_desc(CMSdb* db)
	{
		qsort(db->records, db->record_count, sizeof(StudentRecord), compare_id_desc);
		printf("Sorted by ID (Descending)\n");
	}
	void sort_by_mark_asc(CMSdb* db)
	{
		qsort(db->records, db->record_count, sizeof(StudentRecord), compare_mark_asc);
		printf("Sorted by Mark (Ascending)\n");
	}
	void sort_by_mark_desc(CMSdb* db)
	{
		qsort(db->records, db->record_count, sizeof(StudentRecord), compare_mark_desc);
		printf("Sorted by Mark (Descending)\n");
	}
	 

