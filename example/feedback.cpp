 #include <string.h>
#include <iostream>
#include <thread>
#include "drive_api.h"

using namespace std;


void PrintErrorCode(vector <int> error_code)
{
	cout << "ERROR: ";
	for (auto it = error_code.begin(); it != error_code.end(); it++)
	{
		cout << hex << *it << "  " ;
  	}

	cout << endl;
}

static void PressEnterToExit()
{
    int c;
    while ( (c = getchar()) != '\n' && c != EOF );
    cout << "Press enter to exit." << endl;;

    while( getchar() != '\n')
	{
		if (Amber::Motion::GetStopSignal())
		{
			return;
		}
	}
	Amber::Motion::SetStopSignal();
}

void WorkThread(Amber::AiosGroup *group)
{	
	vector <double> pos(group->Size());
	vector <double> vel(group->Size());
	vector <double> current(group->Size());

	Amber::Motion::InitStopSignal();

	while ( !Amber::Motion::GetStopSignal() )
	{	
		if (!group->GetCvp(pos,vel,current) )
		{
			return;
		}

		cout << "pos : " ;		
		for (auto it = pos.begin(); it != pos.end(); it++)
		{
			cout << *it << "  ";
		}
		cout << endl;

		cout << "vel : " ;		
		for (auto it = vel.begin(); it != vel.end(); it++)
		{
			cout << *it << "  ";
		}
		cout << endl;

		cout << "current : " ;		
		for (auto it = current.begin(); it != current.end(); it++)
		{
			cout << *it << "  ";
		}
		cout << endl;

	}
}

int main(int argc, char *argv[])  
{	  
	
	Amber::Lookup lookup;

	std::shared_ptr <Amber::AiosGroup> group = lookup.GetAvailableList();
	if (!group)
	{
		cout << "EVENT: No device found on network" << endl;
		return -1;
	}

	cout << "EVENT: "<< group->Size() << "devices found on network" << endl;

	auto actuator_info = group->GetActuatorInfo();

	for (auto it = actuator_info.begin(); it != actuator_info.end(); it++)
	{
		cout << "EVENT: find server ip = " << it->ip_ << endl;
		cout << "            serial number = " << it->serial_number_  << endl;
		cout << "            mac address = " << it->mac_address_  << endl;
  	}

	std::vector <int> error_code(group->Size(),0);

	if(!group->Calibration())
	{
		error_code = group->GetErrorCode();
		PrintErrorCode(error_code);
		return -1;
	}

	if(!group->Disable())
	{
		error_code = group->GetErrorCode();
		PrintErrorCode(error_code);
		return -1;
	}

	std::thread thread_running(WorkThread,group.get());

	PressEnterToExit();

	thread_running.join();
	return 0;
}
