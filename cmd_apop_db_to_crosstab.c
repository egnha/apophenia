/** \file cmd_apop_db_to_crosstab.c	Command line utility to convert a three-column table to a crosstab.*/

/*Copyright (c) 2005--2007 by Ben Klemens.  Licensed under the modified GNU GPL v2; see COPYING and COPYING2.  */

#include "apop_internal.h"
#include <unistd.h>

int main(int argc, char **argv){
char		c, 
		    *delimiter,
            *outfile    = NULL,
		    msg[1000];
apop_data	*m;

	sprintf(msg, "%s [opts] dbname table_name rows columns data\n\n"
            "-d\tdelimiter\t\tdefault= \"|,<space><tab>\"\n"
            "-a\tappend\t\t\tdefault= append\n"
            "-o\toverwrite\t\tdefault= append\n"
            "-f\tfile to dump to\t\tdefault=STDOUT\n", argv[0]); 

	if(argc<5){
		printf("%s", msg);
		return 0;
	}
	delimiter	= malloc(5);
	strcpy(delimiter, ",");
	while ((c = getopt (argc, argv, "ad:f:ho")) != -1){
		switch (c){
		  case 'a':
              apop_opts.output_append = 1;
			  break;
		  case 'd':
			  strcpy(apop_opts.output_delimiter,optarg);
			  break;
		  case 'o':
              apop_opts.output_append = 0;
			  break;
		  case 'f':
              outfile   = malloc(1000);
			  sprintf(outfile, "%s", optarg);
			  apop_opts.output_type	= 'f';
			  break;
		  case 'h':
			printf("%s", msg);
			return 0;
		}
	}
	apop_db_open(argv[optind]);
	m	= apop_db_to_crosstab(argv[optind +1], argv[optind+2], argv[optind+3], argv[optind+4]);
	apop_data_print(m, outfile);
}
