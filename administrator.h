
#ifndef ADMINISTRATOR_H_
#define ADMINISTRATOR_H_

#include <string>
#include <ctime>
#include "user.h"
#include "setdate.h"
using namespace std;

class Admin
{
	string pswd;
	static int available_shots; // The number of vaccine shots available per day
	static string last_date;	// Used by the run_process function of this class to record the date of its last execution

public:
	static int age_eligibility;
	Admin()
	{
		pswd = "dsproject";
	}
	bool run_process(UserQueueList &cqueue);
	void display_top_n(UserQueueList &cqueue);
	void display_today(UserQueueList &cqueue);
	void change_eligibility(UserQueueList &cqueue);
	void change_available_shots();
	bool verify_password(string pass);
	void remove(User *user, string pswd, UserQueueList &cqueue);
	void remove_age_ineligible(UserQueueList &cqueue);

	friend class UserQueueList;
};



// Static member initializations
int Admin::age_eligibility = 18;
int Admin::available_shots = 1;
string Admin::last_date = "";

bool Admin::verify_password(string pass)
{

	return pass == pswd;
}

bool Admin::run_process(UserQueueList &cqueue)
{

	User *selected = cqueue.front;
	User *dq = nullptr;

	// Extracting today's date

    // The time_t datatype is a data type in the ISO C library defined for storing system time values
	time_t now = time(0);
	tm *ltm = localtime(&now);

	// The structure tm holds the date and time in the form of a C structure


    // These are the memebers to tm structure
	int day = ltm->tm_mday;
	int month = ltm->tm_mon + 1;
	int year = ltm->tm_year + 1900;


	// todays date is taken from this function
	// + 1 is done using dateutil

	// The localtime() function converts the given time since epoch to calendar time which is expressed as local time
    // The time() function in C++ returns the current calendar time as an object of type time_t

	/*
	 cout << "Enter day :";
	 cin >> day;
	 cout << "Enter month :";
	 cin >> month;
	 cout << "Enter year :";
	 cin >> year;
	 */

	string today = date_string(day, month, year); // The representation of a today's date as a string

	if (today == last_date) // This prevents multiple allotment rounds on the same day.
		return false;

    // one round one day
    // once over, then can't allot again for the same day

	// ALLOTMENT PROCESS:

	// For users who have been allocated slots previously
	while (selected != nullptr && selected->visible)
	{
		if (selected->slot_date == today)
		{
			selected->slot_today = true;
			selected = selected->next;
		}
		else
		{
			// Users whose slot occurred yesterday, but who failed to get vaccinated,
			// need to be removed to the end of the queue-list (downgraded) so they can be assigned fresh slots.

			selected = selected->next; // Preserving the traversal before moving the element

			int new_priority = cqueue.get_min_priority() - 1;
			// all get shifts, isliye -1 (size is constant)
			dq = cqueue.dequeue(); // jisko hata rahe hai, usko store kr rhe and niche kar diye
			dq->visible = false;
			dq->slot_today = false;
			dq->downgraded = true;
			dq->priority = new_priority;
			dq->slot_date = "Not Alloted Yet.";

			cqueue.enqueue(dq);
			// enqueue the user acc to priority
		}

	}
	string dslot = get_next_date(day, month, year); // Generates the string representation for the following day.
	// +1 vaala hai (next day deta hai ye)

	// Allocates the following day's slot to the users and enables them to check their date slots.
	// Only a maximum of 'available_shots' number of users can be allocated a slot on a given day.
	for (int count = 1; count <= available_shots; count++)
	{
		if (selected == nullptr)
			break;
		selected->slot_date = dslot;
		selected->visible = true;
		selected = selected->next;
	}

	last_date = today; // Modifies the last_date member to today's date

	return true;
}

void Admin::display_top_n(UserQueueList &cqueue)
{
	unsigned n; // absolute value
	cout << "\n\tHow many entries are to be displayed ?";
	cin >> n;
	cqueue.display_top_n(n);
}

void Admin::display_today(UserQueueList &cqueue)
{
	cqueue.display_today();
}
void Admin::change_eligibility(UserQueueList &cqueue)
{

	cout << "\n\tEnter eligibility age : ";
	cin >> age_eligibility;
	// static hai, auto change ho jaega
	remove_age_ineligible(cqueue);
	cout << "\n\t\tChanged successfully." << endl;
	// nya queue jo hoga , uss meh sab eligible honge
}
void Admin::change_available_shots()
{

	 //Allows admin to change available_shots as per actual availability

	cout << "\n\tEnter new number : ";
	cin >> available_shots;
	cout << "Changed successfully." << endl;

}

void Admin::remove(User *user, string pswd, UserQueueList &cqueue)
{
	cqueue.remove(user, pswd);
}

void Admin::remove_age_ineligible(UserQueueList& cqueue)
{
	User* cur = cqueue.front;
	User* prev = nullptr;
	User* to_delete = nullptr;
	bool delete_flag = false; // agar delete ho jaega, true kr dega

	while(cur != nullptr){
		delete_flag = cur->age < age_eligibility && !cur->visible;

            // age = 16 --> true && !(False) becoz underage hoga toh register nhi hoga vo
            // true && true --> true
		if (delete_flag)
		{
			if (prev == nullptr) // Removing from the front
				cqueue.front = cur->next;
			else // Removing from other positions
				prev->next = cur->next;
			if (cqueue.rear == cur) // Removing from rear
				cqueue.rear = prev;
			to_delete = cur;
			cur = cur->next;
			to_delete->next = nullptr;
			delete(to_delete);
		}
		else
		{
			prev = cur; // change prev only if cur is not to be deleted
			cur = cur->next;
		}

	}
}

#endif
