/**
 Program: 	cola.cpp
 Description:	A program that simulates a 'cola' (queue) at
 some real or imaginary place (e.g. a bank).
 Author:	Ricardo Salazar
 Date:		November, 2015
 */

#include <iostream>	// std::cout, std::endl, std::istream
#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <cstdlib>
#include <vector>
#include "queue.h"

using namespace Pic10B;  // A call to queue<T> is a call to Pic10B::queue<T> instead of std::queue<T>
/** Could also specify we only want Pic10B::queue like so:
 using Pic10B::queue;
 */

/**
 Definition of the 'Cliente' class.
 */

class Cliente {
public:
    inline Cliente( std::string n = "Default_client" , int a = 0 , int s = 0 )
    : name(n) , arrivalTime(a) , serviceTime(s) { }
    
    // Accessors
    inline std::string get_name() const { return name; }
    inline int get_arrival_time() const { return arrivalTime; }
    inline int get_service_time() const { return serviceTime; }
    
    friend std::istream& operator>>( std::istream& in , Cliente& rhs ) ;
    
private:
    std::string name;
    int arrivalTime;
    int serviceTime;
};

/**
 Non-member functions
 */
std::istream& operator>>( std::istream& in , Cliente& rhs ) {
    in >> rhs.name >> rhs.arrivalTime >> rhs.serviceTime;
    return in;
}

/*
 Prints the time on the screen. It assumes we are given
 the number of minutes after 9:00am.
 */
void print_time( int minsAfterNine ) {
    const int MIN_PER_HOUR = 60;
    
    int hour = 9 + minsAfterNine / MIN_PER_HOUR;
    int min = minsAfterNine % MIN_PER_HOUR;
    
    if (hour <= 12) std::cout << hour << ":";
    else std::cout << hour - 12 << ":";
    
    if (min > 9) std::cout << min;
    else std::cout << "0" << min;
    
    if (hour <= 12) std::cout << "am";
    else std::cout << "pm";
    
    return;
}

double getRandomArrivalTime(int probability) {
    double percentage = (probability)*1.0/100;
    double timeIntervals = 1/percentage;
    return timeIntervals;
}

bool getCounterFree(std::vector<bool>&numCounters){
    for(int i = 0; i < numCounters.size(); i++) {
        if(numCounters[i] == true) return true;
    }
    return false;
}

void updateCounterWhenOccupied(std::vector<bool>&numCounters) {
    for(int i = 0; i < numCounters.size(); i++) {
        if(numCounters[i] == true) numCounters[i] = false;
    }
}

void updateCounterWhenFreed(std::vector<bool>&numCounters) {
    for(int i = 0; i < numCounters.size(); i++) {
        if(numCounters[i] == false) numCounters[i] = true;
    }
}

/**
 main routine that simulates a 'cola' (line) at a bank
 */
int main() {
    std::string filename = "";
    int numTellers = 0;
    int probability = 0;
    srand(time(NULL));
    
    int service_time = rand()%15+2;
    queue<Cliente> customerList;
    Cliente newCustomer;
    /**
     Read customer file.
     Assume the format is:
     name arrivalTime serviceTime
     and that the list is sorted based on arrival times.
     */
    std::cout << "Enter a filename"<<std::endl;
    getline(std::cin, filename);
    std::cout << "Enter the number of tellers"<<std::endl;
    std::cin >> numTellers;
    if(filename == "") {
        int timeAfter9 = 0;
        int i = 0;
        std::string name = "";
        std::cout << "Enter the probability between 1 and 100 that a customer will enter the bank at every minute"<<std::endl;
        std::cin >> probability;

        double interval = getRandomArrivalTime(probability);
        
        while (timeAfter9 < 540) {
            i++;
            std::string num = std::to_string(i);
            timeAfter9 += interval;
            name = "Customer " + num;
            Cliente newCustomer(name, timeAfter9, service_time);
            customerList.enter(newCustomer);
        }
    }
    std::vector<bool> numCounters;
    for(int i = 0; i <= numTellers; i++) {
        numCounters.push_back(true);
    }
    
    
    std::ifstream fin;
    fin.open(filename);
    
   
    
    while ( fin >> newCustomer )
        customerList.enter(newCustomer);
    
    fin.close();

    if(numTellers < 0) {
        std::cout << "ERROR: Could Tellers cannot be a negative number." << std::endl;
        std::cout << "Enter the number of tellers"<<std::endl;
        std::cin >> numTellers;
    }
    
    if ( customerList.is_empty() && filename != "") {
        std::cout << "ERROR: Could not read file " + filename+ """." << std::endl;
        return 1; // Standard practice is to NOT RETURN 0 if an error occurs.
    }
    
    /**
     First customer steps up to the counter immediately (i.e., no waiting occurs).
     Clock starts running at this arrival time.
     */
    Cliente atCounter = customerList.leave();
    
    bool isCounterEmpty = getCounterFree(numCounters);
    updateCounterWhenOccupied(numCounters);
    int currentTime = atCounter.get_arrival_time();
    int startService = atCounter.get_arrival_time();
    
    print_time(currentTime);
    std::cout << "  " << atCounter.get_name() << " enters the bank." << std::endl;
    print_time(currentTime);
    std::cout << "  " << atCounter.get_name() << " steps up to counter." << std::endl;
    
    // set up our empty 'cola'
    queue<Cliente> line;
    
    // while there are customers in bank... or they are yet to arrive.
    while ( !customerList.is_empty() || !line.is_empty() || !isCounterEmpty ){
        
        // Check if a 'Cliente' enters the 'cola'.
        if ( !customerList.is_empty() && customerList.peek().get_arrival_time() <= currentTime ){
            print_time(currentTime);
            std::cout  << "  " << customerList.peek().get_name()
            << " enters the bank." << std::endl;
            
            line.enter( customerList.leave() );
        }
        
        // Check if a 'Cliente' leaves the counter.
        if ( !isCounterEmpty && ( startService + atCounter.get_service_time() <= currentTime ) ){
            updateCounterWhenFreed(numCounters);
            isCounterEmpty = getCounterFree(numCounters);
            print_time(currentTime);
            std::cout << "  " << atCounter.get_name()
            << " leaves the counter." << std::endl;
        }
        
        // Check if counter is empty and someone is waiting.
        if ( isCounterEmpty && !line.is_empty() ){
            
            atCounter = line.leave();
            startService = currentTime;
            isCounterEmpty = getCounterFree(numCounters);
            updateCounterWhenOccupied(numCounters);
            print_time(currentTime);
            std::cout << "  " << atCounter.get_name()
            << " steps up to the counter." << std::endl;
        }
        
        ++currentTime;	// We are wasting time here (clock cycles)!!!
        // Can you think of a better way to do it?
        
        // Here is where we should compute and display some queue statistics
        
    }
    return 0;
}