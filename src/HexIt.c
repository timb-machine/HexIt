/*
$Header: /var/lib/cvsd/var/lib/cvsd/HexIt/src/HexIt.c,v 1.1.1.1 2012-10-30 16:45:26 timb Exp $

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

(c) Tim Brown, 2006
<mailto:timb@nth-dimension.org.uk>
<http://www.nth-dimension.org.uk/> / <http://www.machine.org.uk/>
*/

#include "HexIt.h"

void usage(char *commandname) {
	if (commandname != NULL) {
		fprintf(stderr, "%s -f <filename> -o <mapoffset> -l <maplength> <-@ <patchoffset> -s <patchstring> | -d [-p]>\n", commandname);
	} else {
		fprintf(stderr, "(null) -f <filename> -o <mapoffset> -l <maplength> <-@ <patchoffset> -s <patchstring> | -d [-p]>\n");
	}
	exit(EXIT_FAILURE);
}

void error(char *errorstring) {
	if (errno) {
		if (errorstring != NULL) {
			perror(errorstring);
		} else {
			perror("error");
		}
	} else {
		if (errorstring != NULL) {
			fprintf(stderr, "%s\n", errorstring);
		}
	}
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
	int optionflag;
	char *filename;
	off_t mapoffset;
	size_t maplength;
	int patchoffset;
	char *patchstring;
	int displayflag;
	int perlflag;
	long pagesize;
	int filehandle;
	struct stat filestate;
	void *mmapbuffer;
	int patchcounter;
	int displaycounter;
	optionflag = -1;
	filename = NULL;
	mapoffset = 0;
	maplength = 0;
	patchoffset = 0;
	patchstring = NULL;
	displayflag = FALSE;
	perlflag = FALSE;
	pagesize = 0;
	filehandle = -1;
	mmapbuffer = (void *) -1;
	while ((optionflag = getopt(argc, argv, "f:o:l:@:s:dp")) != -1) {
		switch (optionflag) {
			case 'f':
				filename = optarg;
				break;
			case 'o':
				mapoffset = atol(optarg);
				break;
			case 'l':
				maplength = atol(optarg);
				break;
			case '@':
				patchoffset = atoi(optarg);
				break;
			case 's':
				patchstring = optarg;
				break;
			case 'd':
				displayflag = TRUE;
				break;
			case 'p':
				perlflag = TRUE;
				break;
			default:
				error(NULL);
				break;
		}
	}
	if (filename != NULL) {
		if ((filehandle = open(filename, O_RDWR)) != -1) {
			pagesize = sysconf(_SC_PAGE_SIZE);
			fstat(filehandle, &filestate);
			if ((mapoffset >= 0) && ((mapoffset % pagesize) == 0)) {
				if ((maplength >= 0) && ((maplength % pagesize) == 0)) {
					if (patchstring != NULL) {
						if ((mapoffset + patchoffset + strlen(patchstring)) <= filestate.st_size) {
							if ((mapoffset + maplength) <= filestate.st_size) {
								if ((mmapbuffer = mmap(NULL, maplength, PROT_READ | PROT_WRITE, MAP_SHARED, filehandle, mapoffset)) != (void *) -1) {
									if (patchstring != NULL) {
										for (patchcounter = 0; patchcounter < strlen(patchstring); patchcounter ++) {
											*(((char *) mmapbuffer) + patchoffset + patchcounter) = *(patchstring + patchcounter);
										}
									}
									munmap(mmapbuffer, maplength);
									close(filehandle);
								} else {
									close(filehandle);
									error("couldn't map file");
								}
							} else {
								error("couldn't map beyond file boundary");
							}
						} else {
							error("couldn't patch beyond mapped memory");
						}
					} else {
						if (displayflag == TRUE) {
							if ((mapoffset + maplength) <= filestate.st_size) { 
								if ((mmapbuffer = mmap(NULL, maplength, PROT_READ | PROT_WRITE, MAP_SHARED, filehandle, mapoffset)) != (void *) -1) {
									for (displaycounter = 0; displaycounter < maplength; displaycounter ++) {
										if (perlflag == TRUE) {
											printf("\\x%02x", (unsigned char) *(((char *) mmapbuffer) + displaycounter));
										} else {
											printf("0x%02x", (unsigned char) *(((char *) mmapbuffer) + displaycounter));
											if ((displaycounter + 1) < maplength) {
												printf(",");
											}
										}
									}
									printf("\n");
									munmap(mmapbuffer, maplength);
									close(filehandle);
								} else {
									close(filehandle);
									error("couldn't map file");
									  
								}
							} else {
								error("couldn't map beyond file boundary");
							}
						} else {
							error("invalid patch string and display flag not set");
						}
					}
				} else {
					error("invalid map length");
				}
			} else {
				error("invalid map offset");
			}
		} else {
			error("couldn't open file");
		}
	} else {
		usage(argv[0]);
	}
	exit(EXIT_SUCCESS);
}
