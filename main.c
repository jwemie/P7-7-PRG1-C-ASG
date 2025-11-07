/*
* Course Management System
* Main Program File
*/
#include "cms.h"

int get_valid_menu_choice(void) {
	char input[100]; //buffer to store user input
	int choice;

	//keep asking until we get valid input
	while (1) {
		//Display menu options
		printf("\n=== CMS Menu ===\n");
		printf("1. Open\n");
		printf("2. Show All\n");
		printf("3. Insert\n");
		printf("4. Query\n");
		printf("5. Update\n");
		printf("6. Delete\n");
		printf("7. Save\n");
		printf("8. Exit\n");
		printf("Enter your choice (1-8): ");

		//Read entire line of input
		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("Error reading input. Please try again.\n");
			continue;
		}
		//check if input is exactly 2 chars (digit + newline)
		// This ensures only single digits are accepted
		if (strlen(input) != 2 || input[1] != '\n')
		{
			printf("Invalid input! Please enter a single digit (1-8).\n");
			continue; 
		}
		//check if char is a digit between 1-8
		if (input[0] < '1' || input[0] > '8') {
			printf("Invalid choice! Please enter a number between 1-8.\n");
			continue;
		}

		//converts char to int
		choice = input[0] - '0';
		break;
	}
	return choice;
}

/*
* Main function
* flow of the CMS program
*/

int main() {
	CMSdb db;
	int choice;
	int result;

	//initialise the database
	initialize_db(&db);

	printf("=== Course Management System (CMS) ===\n");
	printf("Welcome to the Student Database Management System\n");
	printf("=========================================\n");

	while (1)
	{
		//get valid menu choice from user
		choice = get_valid_menu_choice();


		//handle the menu choice
		result = handle_menu_choice(choice, &db);

		//check for exit choice
		if (result == -1) {
			break;
		}

		//if result is 0, an error occurred but we continue
		//if result is 1, operation was successful and we continue 
	}

	//Program ending message
	printf("\nThank you for using the Course Management System. Goodbye!\n");
	return 0;
}