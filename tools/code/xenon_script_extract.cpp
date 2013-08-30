#include <iostream>
#include <fstream>

using namespace std;

void PrettyHex(char pretty[6], unsigned char hex)
{
    memset(pretty, 0, 6);
    sprintf(pretty, "<$%02X>", hex);   
    return;
}

void PrintHex(ofstream &scripttxt, unsigned char hex)
{
    char pretty[6];
    PrettyHex(pretty, hex);
    scripttxt.write(reinterpret_cast<char*>(&pretty), 5);
    return;
}

void PrintHexes(ifstream &scriptbin, ofstream &scripttxt, int num)
{
    for(int i = 0; i < num; i++)
    {
        unsigned char hex = 0;
        scriptbin.read(reinterpret_cast<char*>(&hex), 1);
        PrintHex(scripttxt, hex);
    }
    return;
}

int main(int argc, char *argv[])
{

	string infilename(argv[1]);
	string outfilename(argv[2]);


	ifstream cc(infilename, ios::binary);
	ofstream txt(outfilename, ios::binary);

	cc.seekg(0, ios::end);
	unsigned int cc_len = cc.tellg();
	cc.seekg(0, ios::beg);

	unsigned int cc_pos = 0;
	while(cc_pos < cc_len)
	{
		unsigned char cmd = 0;
		cc.read(reinterpret_cast<char*>(&cmd), 1);
		cc_pos++;

		if (cmd == 0xFD)
		{
			// Read in lenght + 1 for the zero
			unsigned char line_len = 0;
			cc.read(reinterpret_cast<char*>(&line_len), 1);
			cc_pos++;
			line_len += 1;

			unsigned char* line = new unsigned char[line_len];
			cc.read(reinterpret_cast<char*>(line), line_len);
			cc_pos += line_len;

			//txt << line << "\n\n";
			txt << "//";

			for(int i = 0; i < line_len; i++)
			{
				if (line[i] < 0x20)
				{
					char buffer[255];
					// Need to figure out what they do
					if (line[i] == 0x04)
					{
						PrintHex(txt, line[i]);
						PrintHex(txt, line[i + 1]);

						/*sprintf(buffer, "<$%02X>\0", line[i]);
						txt.write(reinterpret_cast<char*>(&buffer), 5);
						sprintf(buffer, "<$%02X>\0", line[i + 1]);
						txt.write(reinterpret_cast<char*>(&buffer), 5);*/

						i++;
					}
					else if (line[i] == 0x05)
					{
						/*sprintf(buffer, "<$%02X>\0", line[i]);
						txt.write(reinterpret_cast<char*>(&buffer), 5);*/
						PrintHex(txt, line[i]);
					}
					else if (line[i] == 0x0A)
					{
						txt << "\\n";
					}
				}
				else
				{
					txt.write(reinterpret_cast<char*>(&line[i]), 1);
					if (line[i] >= 0x81)
					{
						txt.write(reinterpret_cast<char*>(&line[i + 1]), 1);
						i++;
					}
				}
			}


			txt << "\n\n";

			delete[] line;
		}
	}

	cc.close();
	txt.close();



	return 0;
}