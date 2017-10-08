/*
$Revision$

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

void usage(const char *commandname) {
	/* TODO enable -a */
	if (commandname != (char *) NULL) {
		fprintf(stderr, "usage: %s -f <filename> -o <mapoffset> -l <maplength> <-@ <patchoffset> -s <patchstring> | -d [-p | -P]>\n", commandname);
	} else {
		fprintf(stderr, "usage: (null) -f <filename> -o <mapoffset> -l <maplength> <-@ <patchoffset> -s <patchstring> | -d [-p | -P]>\n");
	}
	exit(EXIT_FAILURE);
}

void error(const char *commandname, const char *errorstring) {
	if (errno) {
		if (errorstring != (char *) NULL) {
			perror(errorstring);
		} else {
			perror("error");
		}
	} else {
		if (errorstring != (char *) NULL) {
			fprintf(stderr, "%s\n", errorstring);
		}
	}
	usage(commandname);
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
	int prettyflag;
	int analyzeflag;
	long pagesize;
	int filehandle;
	struct stat filestate;
	void *mmapbuffer;
	int patchcounter;
	int displaycounter;
	int blockflag;
	int blockstart;
	int blockend;
	char prettybuffer[PRETTYLINELENGTH + 1];
	optionflag = -1;
	filename = (char *) NULL;
	mapoffset = 0;
	maplength = 0;
	patchoffset = 0;
	patchstring = (char *) NULL;
	displayflag = FALSE;
	perlflag = FALSE;
	prettyflag = FALSE;
	analyzeflag = FALSE;
	pagesize = 0;
	filehandle = -1;
	mmapbuffer = (void *) -1;
	patchcounter = 0;
	displaycounter = 0;
	blockflag = FALSE;
	blockstart = 0;
	blockend = 0;
	prettybuffer[PRETTYLINELENGTH] = (char) '\x00';
	while ((optionflag = getopt(argc, argv, "f:o:l:@:s:dpPa")) != -1) {
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
			case 'P':
				prettyflag = TRUE;
				break;
			case 'a':
				/* TODO enable -a */
				analyzeflag = FALSE;
				break;
			default:
				error(argv[0], (char *) NULL);
				break;
		}
	}
	if (filename != (char *) NULL) {
		if ((filehandle = open(filename, O_RDWR)) != -1) {
			pagesize = sysconf(_SC_PAGE_SIZE);
			fstat(filehandle, &filestate);
			if ((mapoffset >= 0) && ((mapoffset % pagesize) == 0)) {
				if ((maplength % pagesize) == 0) {
					if ((patchoffset >= 0) && (patchstring != (char *) NULL)) {
						if ((mapoffset + patchoffset + strlen(patchstring)) <= filestate.st_size) {
							if ((mapoffset + maplength) <= filestate.st_size) {
								if ((mmapbuffer = mmap((void *) NULL, maplength, PROT_READ | PROT_WRITE, MAP_SHARED, filehandle, mapoffset)) != (void *) -1) {
									if (patchstring != (char *) NULL) {
										for (patchcounter = 0; patchcounter < strlen(patchstring); patchcounter ++) {
											*((char *) (mmapbuffer + patchoffset + patchcounter)) = *((char *) (patchstring + patchcounter));
										}
									}
									munmap(mmapbuffer, maplength);
								} else {
									error(argv[0], "couldn't map file");
								}
							} else {
								error(argv[0], "couldn't map beyond file boundary");
							}
						} else {
							error(argv[0], "couldn't patch beyond mapped memory");
						}
					} else {
						if (displayflag == TRUE) {
							if ((mapoffset + maplength) <= filestate.st_size) {
								if ((mmapbuffer = mmap((void *) NULL, maplength, PROT_READ | PROT_WRITE, MAP_SHARED, filehandle, mapoffset)) != (void *) -1) {
									for (displaycounter = 0; displaycounter < maplength; displaycounter ++) {
										if ((prettyflag == TRUE) || (analyzeflag == TRUE)) {
											if (analyzeflag == TRUE) {
												if ((char) *((char *) (mmapbuffer + displaycounter)) != (char) NULL) {
													if (blockflag == FALSE) {
														blockflag = TRUE;
														blockstart = displaycounter;
													}
												} else {
													if (blockflag == TRUE) {
														blockflag = FALSE;
														blockend = displaycounter;
														/* TODO dump block details */
														if ((blockend - blockstart) == sizeof(void *)) {
															/* TODO dump pointer details */
														}
														blockstart = 0;
														blockend = 0;
													}
												}
											}
											if ((displaycounter % PRETTYLINELENGTH) == 0) {
												printf("0x%08x\t", (int) mmapbuffer + displaycounter);
											}
											if ((displaycounter % PRETTYLINELENGTH) > 0) {
												printf(" ");
											}
											if (isalnum((char) *((char *) (mmapbuffer + displaycounter)))) {
												prettybuffer[displaycounter % PRETTYLINELENGTH] = (char) *((char *) (mmapbuffer + displaycounter));
											} else {
												prettybuffer[displaycounter % PRETTYLINELENGTH] = (char) '.';
											}
											printf("%02x", (char) *((char *) (mmapbuffer + displaycounter)));
											if ((displaycounter % PRETTYLINELENGTH) == (PRETTYLINELENGTH - 1)) {
												printf("\t%s\n", prettybuffer);
											}
										} else {
											if (perlflag == TRUE) {
												printf("\\x%02x", (char) *((char *) (mmapbuffer + displaycounter)));
											} else {
												printf("0x%02x", (char) *((char *) (mmapbuffer + displaycounter)));
												if ((displaycounter + 1) < maplength) {
													printf(",");
												}
											}
										}
									}
									munmap(mmapbuffer, maplength);
								} else {
									error(argv[0], "couldn't map file");
								}
							} else {
								error(argv[0], "couldn't map beyond file boundary");
							}
						} else {
							error(argv[0], "invalid patch string and display flag not set");
						}
					}
				} else {
					error(argv[0], "invalid map length");
				}
			} else {
				error(argv[0], "invalid map offset");
			}
			close(filehandle);
		} else {
			error(argv[0], "couldn't open file");
		}
	} else {
		error(argv[0], "invalid file");
	}
	exit(EXIT_SUCCESS);
}
