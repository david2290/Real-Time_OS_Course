#include <stdio.h>
#include <stdlib.h>
#include "latex_tools.h"

int main(int argc, char** argv){
	RT_create_from_base_file();
	RT_export_project();
	RT_open_window();
    return 0;
}
