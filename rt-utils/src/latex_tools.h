#define FILE_NAME "output"

void RT_create_from_base_file(){
	char create_cmd[] = "cp base.tex "FILE_NAME".tex";
	system(create_cmd);
}

void RT_export_project(){
	char pdfltx_cmd[] = "pdflatex "FILE_NAME".tex";
	system(pdfltx_cmd);
}

void RT_open_window(){
	char pdf_viewer[] = "xdg-open "FILE_NAME".pdf";
	system(pdf_viewer);
}
