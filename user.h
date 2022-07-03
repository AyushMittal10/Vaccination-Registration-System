
#ifndef USER_H_
#define USER_H_

#include <string>
using namespace std;


class User
{
	int user_id; // Personal details (data)
	string name;
	int age;
	char sex;
	bool diseases; // diseases (more than 1)

	string slot_date; // stored the date value from dateutil --> details as well

	int priority; // For the calculation details, see the get_priority function of this class
	string password;
	bool downgraded; // True if the user has been downgraded to the end of the queue-list for missing their alloted slot.
	User *next = nullptr;

public:
	// Visibility flags - Modified only by running allotment process
	bool visible;	 // True when user can see their slot
	bool slot_today; // True when the slot allocated is today

	User(string nm, int cit_id, char sx, int ag, bool comor, string pswd)
	{
		user_id = cit_id;
		name = nm;
		age = ag;
		sex = sx;
		diseases = comor;

		slot_date = "Not Alloted Yet.";

		visible = false;
		slot_today = false;

		password = pswd;
		priority = (visible * 10000) + (diseases * 1000) + age;
		downgraded = false;
	}

	int get_priority();
	bool is_pswd(string pswd);
	string get_details(string pswd);

	friend class Admin; // class Admin in "Admin.h" header
	friend class UserQueueList;
};

class UserQueueList
{
	User *rear;
	User *front;
	void display_top_n(unsigned n); //absolute value
	void display_today(); //returns details of users of today's slot
	void remove(User *user, string pswd);
	User *dequeue();

public:
	UserQueueList()
	{
		front = nullptr;
		rear = nullptr;
	}

	void enqueue(User *p);
	User *search(int userID);
	int get_min_priority();

	friend class Admin;
};



int User::get_priority()
{
	/*
	  1.If a user has been allocated a slot, they cannot be overriden
	    by someone who has not been allocated a slot.
	  2.Users with diseases have a higher priority than those without.
	  3.Older users have higher priority.
	  4.Demoted users' priorities depend on the priority of the last person in the queue-list
	  	 when they were downgraded and on their 'visible' member.

	  This function updates and returns the priority of a user.

	 */
	if (!downgraded)
		priority = (visible * 10000) + (diseases * 1000) + age;
	else
		priority = (visible * 10000) + priority;
	return priority;

	// we will off slot success if downgraded once
	//it gives the integer value of the priority
}

bool User::is_pswd(string pswd)
{

	return password == pswd;
}

string User::get_details(string pswd)
{

	string details;
	if (is_pswd(pswd))
	{
	    // details me concatenate karke store kar raha hai
		details += "\n\n\tUserID : ";
		details += to_string(user_id);
		details += "\n\n\t";
		details += "Name : ";
		details += name;
		details += "\n\n\t";
		details += "Age : ";
		details += to_string(age);
		details += "\n\n\t";
		details += "Sex : ";
		details += sex;
		details += "\n\n\t";
		details += "Slot Date : ";
		details += slot_date;
		details += "\n\n\t";
		return details;
	}
	return "Access Denied.";
}


User *UserQueueList::search(int userID)
{

	User *cur = front;
	while (cur != nullptr)
	{
		if (cur->user_id == userID)
			break;
		cur = cur->next;
	}
	return cur;
}

void UserQueueList::enqueue(User *new_user)
{

     //Adds user to the queue-list as per priority.

	if (front == nullptr) //First registration
	{
		front = new_user;
		rear = new_user;
	}
	else
	{
		User *cur = front;
		User *prev = nullptr;

		while (cur != nullptr && cur->get_priority() >= new_user->get_priority()) // Finding the location for insertion
		{
			prev = cur;
			cur = cur->next;
		}
		if (prev == nullptr) // Appending to the head
		{
			new_user->next = front;
			front = new_user;
		}
		else if (cur == nullptr) //Appending to the rear
		{
			prev->next = new_user;
			rear = new_user;
		}
		else // Appending in the middle
		{
			prev->next = new_user;
			new_user->next = cur;
		}
	}
}

User *UserQueueList::dequeue()
{

	 //Removes and returns the front of the queue-list

	User *top = front;
	if (front == rear) // ek hi registration hoga
	{
		front = nullptr;
		rear = nullptr;
	}
	else
	{
		front = front->next;
		top->next = nullptr;
	}
	return top; //it returns the removed user.
}

void UserQueueList::remove(User *user, string pswd)
{

	 // Allows the removal of a person from the queue-list.
	 // Can only be accessed with the person's consent.

	if (!user->is_pswd(pswd)) // Checks if its the real user
		return;

	User *cur = front;
	User *prev = nullptr;
	while (cur->user_id != user->user_id && cur != nullptr)
	{ // Finding the person in the queue-list
		prev = cur;
		cur = cur->next;
	}
	if (prev == nullptr) // Removing from the front
		front = cur->next;
	else // Removing from other positions
		prev->next = cur->next;
	if (rear == cur) // Removing from rear
		rear = prev;
	cur->next = nullptr;
	delete (cur);
}

void UserQueueList::display_top_n(unsigned n)
{

	User *user = front;
	unsigned count;
	for (count = 1; count <= n && user != nullptr; count++)
	{
		cout << "\n\t" << count << ")\t UserID: " << user->user_id << "\n\t\t Name:" << user->name << "\n\t\t Slot Date:" << user->slot_date << endl;
		user = user->next;
	}
	if (count - 1 < n) //let's say count= 6, n=3 , count-1=5 <3 i.e false
		cout << "Only " << (count - 1) << " entries in queue-list" << endl;

		/* when size is less, and n is more
		then loop will terminate
		size = 5, n = 10 */
}

void UserQueueList::display_today()
{

	User *user = front;
	int count = 1;
	while (user != NULL && user->slot_today) // bool value hai (jab tak slot hai aaj ka)
	{
		cout << "\n\t" << count << ")\t UserID:" << user->user_id << "\n\t\t Name:" << user->name << "\n\t\t Slot Date:" << user->slot_date << endl;
		user = user->next;
		count++;
	}
	if (count == 1)
		cout << "\n\tNo users in today's slot." << endl;
}

int UserQueueList::get_min_priority()
{
	/*
	  Returns the priority of the rear element (the lowest priority). // not minimum priority
	  Useful for moving users to the bottom of the queue-list.
	 */
	return rear->get_priority();
}


#endif
