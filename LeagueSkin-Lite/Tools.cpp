#include "Tools.h"

Tools::Tools()
{
    Fov_offsets = 0x0097FB94;
    Fov_status = 1;
    Fov_on_data = 267585910;
    Fov_off_data = 267585911;
    Fov_pause_bool = true;
    Fov_data = 0.0f;
    Fov_data_offsets_write = 0;

/*
    "League of Legends.exe"+16BAAAC + 0C + 25C
    01ABAAAC + 0C + 25C
*/
    Fov_data_offsets = 0x01ABAAAC;
    Fov_data_1ptr = 0x0C;
    Fov_data_2ptr = 0x25C;

    pid = GetCurrentProcessId();
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

Tools::~Tools()
{
}

void Tools::on_Fov(void)
{
    WriteProcessMemory(hProcess, (LPVOID)Fov_offsets, &Fov_on_data, sizeof(Fov_on_data), NULL);
}

void Tools::off_Fov(void)
{
    WriteProcessMemory(hProcess, (LPVOID)Fov_offsets, &Fov_off_data, sizeof(Fov_off_data), NULL);
}

void Tools::set_Fov_status(int v)
{
    this->Fov_status = v;
}

int Tools::get_Fov_status(void)
{
    return this->Fov_status;
}

float Tools::get_Fov_data(void)
{
    return Fov_data;
}

int Tools::get_Fov_data_address(void)
{
    int tmp = 0;
	ReadProcessMemory(hProcess, (LPVOID)Fov_data_offsets, &tmp, sizeof(tmp), NULL);

	tmp += Fov_data_1ptr;
	ReadProcessMemory(hProcess, (LPVOID)tmp, &tmp, sizeof(tmp), NULL);

	tmp += Fov_data_2ptr;
    this->Fov_data_offsets_write = tmp;

    ReadProcessMemory(hProcess, (LPVOID)tmp, &this->Fov_data, sizeof(this->Fov_data), NULL);

	return tmp;
}

void Tools::set_Fov_pause_status(bool v)
{
    this->Fov_pause_bool = v;
}

bool Tools::get_Fov_pause_status(void)
{
    return this->Fov_pause_bool;
}
