/*
* Course Management System
* Main Program File
*/
#include "cms.h"

int get_valid_menu_choice(void) {
	char input[100];
	int choice;

	while (1) {
		show_menu();
		printf("Enter your choice (1-10): ");

		if (fgets(input, sizeof(input), stdin) == NULL) 
		{
			printf("Error reading input. Please try again.\n");
			continue;
		}

		//clear input buffer for wrong inputs
		if (strchr(input, '\n') == NULL) 
		{
			clear_input_buffer();
		}

		// Remove newline character
		input[strcspn(input, "\n")] = 0;

		if (strcmp(input, "10") == 0) {
			return 10;  // Directly return -1 for exit
		}
		if (strlen(input) != 2 || input[1] != '\n') //check input length is only 1 digit
		{
			printf("Invalid input! Please enter a single digit (1-10).\n");
			continue;
		}

		if (input[0] < '1' || input[0] > '9') 
		{
			printf("Invalid choice! Please enter a number between 1-10.\n");
			continue;
		}
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
	printf("================================================================================\n");

	printf("%50s\n", "Declaration");
	printf("SIT's policy on copying does not allow the students to copy source code as well as assessment solutions\n");
	printf("from another person AI or other places.It is the students’ responsibility to guarantee that their\n");
	printf("assessment solutions are their own work.Meanwhile, the students must also ensure that their work is\n");
	printf("not accessible by others.Where such plagiarism is detected, both of the assessments involved will\n");
	printf("receive ZERO mark.\n");

	printf("We hereby declare that:\n");
	printf("- We fully understand and agree to the abovementioned plagiarism policy.\n");
	printf("- We did not copy any codes from others or from other places.\n");
	printf("- We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n");
	printf("- We agree that our projct will recieve ZERO mark if there is any plagiarism detected.\n");
	printf("- We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n");
	printf("- We agree that we did not copy any code directly from AI generated sources.\n");
	printf("\n");
	printf("Declared by:P7-7\n");
	printf("Team members:\n");
	printf("1. SNG WEI LIANG JEREMY\n");
	printf("2. ZHOU XINLONG\n");
	printf("3. RUAN ZHONGJIANG\n");
	printf("4. TAN CHIN HWEE QUINNIE (CHEN QIANHUI)\n");
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