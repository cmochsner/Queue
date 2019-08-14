//Note: Program creates log.txt file of bank activity

#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>

using namespace std;

template<class T>
struct node
{
	T data {};
	struct node<T> *next;
	struct node<T> *prev;
};
template<class T>
class queue
{
	private:
		struct node<T> *front = {};
		struct node<T> *rear = {};
		int size=0;
	public:
		int getSize()
		{
			return size;
		}
		T getFront()
		{
			return front->data;
		}
		T getRear()
		{
			return rear->data;
		}
		void add(T el)
		{
			struct node<T> *newnode= new node<T>;
			newnode->data = el;
			newnode->next = NULL;
			if (front == NULL)
			{
				front = newnode;
				rear = newnode;
				size++;
				return;
			}
			else if (front->next == NULL)
			{
				front->next = newnode;
			}
			rear->next = newnode;
			newnode->prev = rear;
			rear = newnode;
			rear->next = NULL;
			size++;
		}
		T remove()
		{
			T item = {};
			if (front == NULL)
			{
				//cout << "List is empty." << endl;
				return item;
			}
			
			struct node<T> *temp = front;

			item = temp->data;
			if (temp->next!=NULL&&temp->next->prev != NULL)
			{
				temp->next->prev = NULL;
			}
			front = temp->next;
			size--;
			free(temp);

			return item;
		}
		void print()
		{
			struct node<T> *current = front;
			while (current != NULL)
			{
				cout << current->data<<" ";
				current = current->next;
			}
		}
};

class randomInteger
{
	public:
		unsigned int operator()(unsigned int range);
};
unsigned int randomInteger::operator()(unsigned int range)
{
	int random = rand() % (range+1);
	return random;
}

randomInteger randomizer;

class Customer
{
	protected:
		unsigned int arrivalTime;
		unsigned int processTime;
		unsigned int lineWait;
	public:
		Customer(int at): arrivalTime(at), processTime(2 + randomizer(6)), lineWait(0) {}
		Customer(): arrivalTime(0), processTime(0), lineWait(0) {}
		bool isDone(int wait);
		int arrival();
		int process();
		void setWait(int now)
		{
			lineWait = now-arrivalTime;
		}
		int getWait()
		{
			return lineWait;
		}
};
bool Customer::isDone(int now)
{
	if (now == (arrivalTime + lineWait + processTime))
	{
		return true;
	}
	return false;
}
int Customer::arrival()
{
	return arrivalTime;
}
int Customer::process()
{
	return processTime;
}

class Teller
{
	private:
		bool free;
		Customer customer;
	public:
		Teller() 
		{
			free = true;
		}
		bool isFree();
		void addCustomer(Customer &c);
		void setFree(Customer &c);
		Customer getCustomer();
};
Customer Teller::getCustomer()
{
	return customer;
}
bool Teller::isFree()
{
	if (free)
	{
		return true;
	}
	return false;
}
void Teller::addCustomer(Customer &c)
{
	customer = c;
	free = false;
}
void Teller::setFree(Customer &c)
{
	customer = NULL;
	free = true;
}

int main()
{
	//open log file to output bank activity
	ofstream out("log.txt");

	int numberOfTellers = 5;
	int numberOfMinutes = 60;
	double totalWait = 0;
	int numberOfCustomers = 0;

	vector<Teller> teller(numberOfTellers);
	queue<Customer> line;
	
	for (int i = 0; i < numberOfMinutes; i++)
	{
		//get a randomn number out of 10
		int probability = rand() % 10;

		//if one of the numbers given an 80% chance is gotten then add a customer to the queue because one has arrived
		if (probability != 8 && probability != 9)
		{
			//create Customer object
			Customer c(i);

			//add customer to the queue or line
			line.add(c);

			//update the # of customers
			numberOfCustomers++;

			//log the customers arrival
			out << "Customer arrived at " << c.arrival() << " minutes with a processing time of " << c.process() << " minutes..." << endl;
		}
		if (i > 0)
		{
			//Check through all the Teller's customers to see if each customer is done being serviced. If so, set the Teller to free.
			for (int x = 0; x < numberOfTellers; x++)
			{
				//Store the customer at Teller station x in Customer object temp
				Customer temp = teller.at(x).getCustomer();

				//Check to see if this customer is done with their transaction
				if (temp.isDone(i-1))
				{
					//The customer is done, so the Teller can be set free.
					teller.at(x).setFree(temp);

					//Log that the Teller at station x is now free
					out << "Teller at station " << x + 1 << " is open for customers..." << endl;
				}
			}
		}
		//if there is a customer in line, then check to see whether the first in line can be serviced
		if (line.getSize()>0)
		{
			//store the customer at the front of the line in Customer object front
			Customer front = line.getFront();

			//go through the tellers and see if they are open to serve the first customer in line
			for (int j = 0; j < numberOfTellers; j++)
			{
				//check that the line is not empty and the Teller at the j station is free
				if (line.getSize()>0&&teller.at(j).isFree())
				{
					//remove the customer first in line and put it into the front Customer object
					front=line.remove();

					//set the amount of time the customer was waiting in line to its lineWait variable
					front.setWait(i);

					//add a customer to be serviced by the Teller at station j
					teller.at(j).addCustomer(front);

					//add the wait time of this customer to the sum total of wait times
					totalWait = totalWait+(front.getWait());

					//log that the Teller is servicing a customer
					out << "Teller at station " << j+1 << " is now serving a customer for "<<front.process()<<" minutes..." << endl;
				}
			}
		}
	}
	//Since 60 "minutes" have passed, the bank is considered to be closed. Log the number of customers that were left in line.
	out<<"Bank is closing with "<<line.getSize()<<" customers left in line."<<endl;

	//Add the wait times for all of the customers still left in line to the total wait of all customers
	for (int u = 0; u < line.getSize(); u++)
	{
		Customer temp = line.remove();
		totalWait = totalWait + (numberOfMinutes-temp.arrival());
	}

	//Calculate the average wait time for all of the customers and output it
	double avgWait = totalWait / numberOfCustomers;
	cout << "The average wait for all the customers was: " << avgWait<<endl;
	
	//Clear the teller vector
	teller.clear();
	//Close the log file
	out.close();

	//system("pause");
	return 0;
}
