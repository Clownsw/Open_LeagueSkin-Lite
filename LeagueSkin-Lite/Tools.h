#pragma once

#include <iostream>
#include <windows.h>
#include <thread>

#ifndef TOOLS_H
#define TOOLS_H

class Tools
{

public:
	Tools();

	~Tools();

//Œﬁœﬁ ”æ‡
public:

	void on_Fov(void);

	void off_Fov(void);

	void set_Fov_status(int v);

	int get_Fov_status(void);
	
	float get_Fov_data(void);

	int get_Fov_data_address(void);

	void set_Fov_pause_status(bool v);

	bool get_Fov_pause_status(void);

private:
	DWORD pid;
	HANDLE hProcess;

	int Fov_offsets;
	int Fov_status;
	bool Fov_pause_bool;
	int Fov_on_data;
	int Fov_off_data;
	
	float Fov_data;
	int Fov_data_offsets;
	int Fov_data_offsets_write;
	int Fov_data_1ptr;
	int Fov_data_2ptr;

};

#endif
