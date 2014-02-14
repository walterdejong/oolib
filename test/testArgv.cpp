/*
	testArgv.cpp	WJ112
*/
/*
 * Copyright (c) 2014, Walter de Jong
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "oolib"

using namespace oo;

static Options options[] = {
	// short long		argument	help
	{ 'h',	"help",		NULL,		"Display this information"		},
	{ 'a',	NULL,		"ARG",		"Do everything"					},
	{ 'c',	"config",	"FILENAME",	"Use this config file"			},
	{  0,	NULL,		NULL,		"(default is: /etc/myconfig)"	},
	{  1,	"longopt",	NULL,		"Tests only long option"		},
	{ 'v',	"verbose",	NULL,		"Increase verbosity"			},

	{ 0, NULL, NULL, NULL }		// end with line with only nulls
};


int main(int argc, char *argv[]) {
	Argv.init(argc, argv);

	print("len(Argv): %zu", len(Argv));

	print("prognam: %s", Argv.prognam());
	print("command: %s", Argv.command());
	print("Argv   : %v", &Argv);

	print();
	foreach(i, Argv)
		print("Argv[%u]: %s", i, Argv[i]);

	// test command-line parser
	int opt;
	while((opt = Argv.getopt(options)) != -1) {
		switch(opt) {
			case '?':
				print("getopt() error detected");
				break;

			case ':':
				print("getopt() argument error detected");
				break;

			case 'h':
				print("usage: %s [-hv] [-a ARG] [-c CONFIG] [--longopt] [arg ..]", Argv.prognam());
				Argv.usage(options);
				break;

			case 'a':
				print("option 'a' selected");
				break;

			case 'c':
				print("option 'c' or 'config' selected");
				print("optarg: %s", Argv.optarg());
				break;

			case 1:
				print("option 'longopt' selected");
				break;

			case 'v':
				print("option 'v' or 'verbose' selected");
				break;

			default:
				print("getopt() FAIL; unknown return value");
		}
	}

	print("remaining args:");
	foreach(i, Argv.args())
		print(" args[%u] %s", i, Argv.args()[i]);

	return 0;
}

// EOB
