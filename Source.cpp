/*
Author:       Fatima Parada-Taboada
Assignment:   Project 1
Net ID:       fnp170130
Description:  In preparation for the release of Avengers 4 next summer, you have been hired by the owner of a very
			  small movie theater to develop the backend for an online ticket reservation system. The program should display
			  the current seating arrangement and allow the patron to select seats. A report should be generated at the end of
			  the program to specify for each individual auditorium and overall for all auditoriums how many seats were
			  sold/unsold and how much money was earned.
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

//function prototypes
void getNumRows(fstream& myFile, int& numRows);
void displaySeating(fstream&myFile, char&curr, char&letter, int&rowSize, int&totalAdult, int&totalChild, int&totalSenior);
void displayReport(int& numRows, int &rowSize, int& adult, int& child, int& senior, int& totalAdult, int& totalChild, int& totalSenior);
char bestAvailableSeats(int pickRow, int start, fstream& myFile, char&curr, int&numRows, int&rowSize, int adult, int child, int senior);
void makeReservation(int pickRow, int start, fstream& myFile, char&curr, int&numRows, int&rowSize, int adult, int child, int senior);

int main()
{
	int menuChoice;
	int pickRow;
	char start;
	int numRows = 0;
	int rowSize = 0;
	int adult;
	int child;
	int senior;
	int totalAdult = 0;
	int totalChild = 0;
	int totalSenior = 0;
	int numTickets;
	int seatsAvailable = 0;
	char booking = 'A';

	fstream myFile("A1.txt", ios::in | ios::out);


	if (myFile.is_open())
	{
		//get number of rows
		getNumRows(myFile, numRows);

		do
		{
			//display menu
			{
				cout << "       MOVIE THEATER MENU\n";
				cout << "----------------------------------\n";
				cout << "1. Reserve Seats\n2. Exit\n";
			}

			//input response
			cin >> menuChoice;

			//input validation for response
			while (menuChoice > 2 || menuChoice <= 0)
			{
				cout << "You have made an invalid choice. Please choose from our options.\n";
				cout << "1. Reserve Seats\n2. Exit\n";
				cin >> menuChoice;
			}

			if (menuChoice == 1)
			{
				char letter = 'A';
				char curr = 'A';
				rowSize = 0;

				displaySeating(myFile, curr, letter, rowSize, totalAdult, totalChild, totalSenior);

				//Prompt user for row number
				cout << "\n\nPlease choose a row.";
				cin >> pickRow;

				//input validation for row choice
				while (!(pickRow <= numRows && pickRow > 0))
				{
					cout << "\nIvalid option chosen. Please choose a row.";
					cin >> pickRow;
				}

				//prompt user for seat letter
				cout << "\nPlease choose a starting seat.";
				cin >> start;

				//input validation for starting seat
				while (toupper(start) > letter || toupper(start) < 'A')
				{
					cout << "\nInvalid seat. Please choose a starting seat.";
					cin >> start;
				}

				//prompt user for number of adult tickets
				cout << "\nHow many adult tickets would you like to purchase?";
				cin >> adult;

				//input validation
				while (adult < 0)
				{
					cout << "\nInvalid number of adult tickets. Try again.";
					cin >> adult;
				}

				//prompt user for number of child tickets
				cout << "\nHow many child tickets would you like to purchase?";
				cin >> child;
				
				//input validation for child tickets
				while (child < 0)
				{
					cout << "\nInvalid number of child tickets. Try again.";
					cin >> child;
				}

				//prompt user for number of senior tickets
				cout << "\nHow many senior tickets would you like to purchase?";
				cin >> senior;

				//input validation for senior tickets
				while (senior < 0)
				{
					cout << "\nInvalid number of senior tickets. Try again.";
					cin >> senior;
				}


				numTickets = adult + child + senior;

				myFile.clear();
				//go to the beginning of the row to count how many rows we have available
				myFile.seekg((pickRow - 1) * (rowSize + 2));
				seatsAvailable = rowSize;
				for (int n = 0; n < rowSize; n++)
				{
					myFile.get(curr);
					if (curr != '.')
					{
						seatsAvailable -= 1;
					}
				}

				if (numTickets > seatsAvailable)
				{
					cout << "We are sorry. The number of seats you would like are greater";
					cout << " than the number of seats we have available." << endl;
					continue;
				}
				else
				{
					//now we need to check and see if the seat is taken
					myFile.clear();
					myFile.seekg((pickRow - 1) * (rowSize + 2) + (int(toupper(start)) - 'A'));
					//takes us to starting seat the person chose
					myFile.get(curr);

					//for the number of tickets being bought
					for (int seatsNeeded = numTickets; seatsNeeded != 0; seatsNeeded--)
					{
						//ran into a taken seat
						if (curr != '.')
						{
							cout << "\nAt least one of the seats you have chosen are taken." << endl;
							cout << "Here is the best available: \n\n";

							//find best available and return response 
							booking = bestAvailableSeats(pickRow, toupper(start), myFile, curr, numRows, rowSize, adult, child, senior);
							cout << endl;

							//if they want to book reservation
							if (toupper(booking) == 'Y')
							{
								//save/write in to file
								makeReservation(pickRow, toupper(start), myFile, curr, numRows, rowSize, adult, child, senior);
								//display report
								displayReport(numRows, rowSize, adult, child, senior, totalAdult, totalChild, totalSenior);
								break;
							}
							//if they dont want to book reservation
							else if (toupper(booking) == 'N')
							{
								//go back to main menu
								break;
							}
						}
						//all seats are available
						if (seatsNeeded == 1)
						{
							makeReservation(pickRow, toupper(start), myFile, curr, numRows, rowSize, adult, child, senior);
							displayReport(numRows, rowSize, adult, child, senior, totalAdult, totalChild, totalSenior);
						}

						//get next character in file
						myFile.get(curr);
					}
				}

			}

		} while (menuChoice != 2);
	}
	else
	{
		cout << "file could not be opened." << endl;
	}

	myFile.close();
	system("pause");
	return 0;
}
// end funtion main

void getNumRows(fstream&myFile, int&numRows)
{
	string line;

	while (getline(myFile, line))
	{
		numRows++;
	}

	myFile.clear();
	myFile.seekg(ios::beg);
}

void displaySeating(fstream&myFile, char&curr, char&letter, int&rowSize, int&totalAdult, int&totalChild, int&totalSenior)
{
	curr = 'A';
	letter = 'A';
	rowSize = 0;
	cout << "      SEATING AVAILABILITY\n";
	cout << "----------------------------------\n";
	//displays seat letters
	cout << "   ";
	for (int i = 1; curr != '\n'; i++)
	{
		myFile.seekg(i);
		myFile.get(curr);
		cout << letter;
		letter++;
		rowSize++;
	}
	cout << endl;
	//start reading file from beginning again
	myFile.clear();
	myFile.seekg(ios::beg);


	//display the seating arrangement
	myFile.get(curr);
	int rowNumber = 1;
	cout << setw(3) << fixed << left << rowNumber;
	rowNumber++;

	for (int i = 1; !(myFile.eof()); i++)
	{

		if (curr != '\n')
		{
			if (curr == '.')
			{
				cout << '.';
			}
			else
			{
				cout << "#";
				if (curr == 'A')
					totalAdult++;
				else if (curr == 'C')
					totalChild++;
				else if (curr == 'S')
					totalSenior++;
			}
		}
		else
		{
			cout << endl;
			cout << setw(3) << fixed << left << rowNumber;
			rowNumber++;
			i++;
		}
		myFile.seekg(i);
		myFile.get(curr);
	}
}

void displayReport(int&numRows, int&rowSize, int&adult, int&child, int&senior, int&totalAdult, int&totalChild, int&totalSenior)
{

	cout << endl;
	double adultVal = 10.0;
	double childVal = 5.0;
	double seniorVal = 7.5;
	int totalSeats = numRows * rowSize;
	totalAdult += adult;
	totalChild += child;
	totalSenior += senior;
	int totalTickets = totalAdult + totalChild + totalSenior;
	double totalSales = adultVal * totalAdult + childVal * totalChild + seniorVal * totalSenior;

	cout << setw(30) << left << "Total Seats in Auditorium:" << totalSeats << endl;
	cout << setw(30) << left << "Total tickets sold:" << totalTickets << endl;
	cout << setw(30) << left << "Adult tickets sold:" << totalAdult << endl;
	cout << setw(30) << left << "Child tickets sold:" << totalChild << endl;
	cout << setw(30) << left << "Senior tickets sold:" << totalSenior << endl;
	cout << setw(28) << left << "Total ticket sales:";
	cout << setw(2) << left << "$";
	cout << fixed << setprecision(2) << totalSales << endl;

	cout << endl << endl;
}

char bestAvailableSeats(int pickRow, int start, fstream& myFile, char&curr, int&numRows, int&rowSize, int adult, int child, int senior)
{
	int numTickets = adult + child + senior;
	curr = 'A';
	char letter = 'A';
	rowSize = 0;
	cout << "      SEATING AVAILABILITY\n";
	cout << "----------------------------------\n";
	//displays seat letters
	cout << "   ";
	for (int i = 1; curr != '\n'; i++)
	{
		myFile.seekg(i);
		myFile.get(curr);
		cout << letter;
		letter++;
		rowSize++;
	}
	cout << endl;

	string line;

	myFile.seekg(0);
	int rowNumber = 1;
	while (getline(myFile, line) && rowNumber <= numRows)
	{
		cout << endl;
		cout << setw(3) << fixed << left << rowNumber;

		if (rowNumber == pickRow)
		{
			for (int k = 0; k < rowSize; k++)
			{
				if (line[k] == '.')
				{
					if (adult > 0)
					{
						line[k] = 'A';
						cout << line[k];
						adult--;
					}
					else if (child > 0)
					{
						line[k] = 'C';
						cout << line[k];
						child--;
					}
					else if (senior > 0)
					{
						line[k] = 'S';
						cout << line[k];
						senior--;
					}
					else
						cout << '.';
				}
				else
				{
					cout << '#';
				}
			}
		}
		else {
			for (int k = 0; k < rowSize; k++)
			{
				if (line[k] == '.')
				{
					cout << '.';
				}
				else
				{
					cout << '#';
				}
			}
		}
		rowNumber++;

		if (myFile.eof())
			break;
	}

	char booking;
	cout << "\n\n";
	cout << "Would you like to reserve these seats instead? (Y/N)" << endl;
	cin >> booking;
	return booking;
}

void makeReservation(int pickRow, int start, fstream& myFile, char&curr, int&numRows, int&rowSize, int adult, int child, int senior)
{
	curr = 'A';
	long startPos = (pickRow - 1) * (rowSize + 2) + (int(toupper(start)) - 'A');
	long pos = 0;
	myFile.seekg(pos, ios::beg);
	myFile.seekp(pos, ios::beg);
	myFile.get(curr);

	//cout << "curr:" << curr << endl;
	cout << "position: " << pos << endl;
	cout << "start" << startPos << endl;

	for (pos = 0; pos <= (numRows*rowSize); pos++)
	{
		myFile.seekg(pos, ios::beg);
		myFile.seekp(pos, ios::beg);
		myFile.get(curr);
		cout << "curr:" << curr << endl;
		myFile.put(curr);
	}

	cout << "\nYour reservations have been made! Enjoy your movie!" << endl;
}