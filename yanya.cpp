
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utility>

using namespace std;

typedef short UNIT;
#define FORMAT_STRING "%hu"
#define TS 65536

char buf[TS];
UNIT s[TS];
UNIT mem[TS];
UNIT b, q; // bang & question mark
UNIT pc;


bool is_digit(UNIT x)
{
	return '0' <= x && x <= '9';
}

pair<UNIT, UNIT> get_value_from(UNIT i)
{
	i = i % TS;
	if(s[i] == '.') {
		if(s[(i + 1) % TS] == '!' || s[i + 1] == '?') {
			if(s[(i + 2) % TS] != '+' && s[(i + 2) % TS] != '-') {
				return make_pair((s[(i + 1) % TS] == '!') ? s[b] : s[q], 2);
			}
			auto v = get_value_from(i + 3);
			auto ret = (s[(i + 2) % TS] == '+') ? v.first : -v.first;
			if(s[(i + 1) % TS] == '!') return make_pair((s[b] + ret) % TS, v.second + 3);
			else return make_pair((s[q] + ret) % TS, v.second + 3);
		} else { printf("! or ? required after .\n"); exit(-1); }
	} else if(s[i] == '!' || s[i] == '?' || s[i] == '@') {
		if(s[(i + 1) % TS] != '+' && s[(i + 1) % TS] != '-') {
			if(s[i] == '!') return make_pair(b, 1);
			else if(s[i] == '?') return make_pair(q, 1);
			else return make_pair(pc, 1);
		}
		auto v = get_value_from(i + 2);
		auto ret = (s[(i + 1) % TS] == '+') ? v.first : -v.first;
		if(s[i] == '!') return make_pair((b + ret) % TS, v.second + 2);
		else if(s[i] == '?') return make_pair((q + ret) % TS, v.second + 2);
		else return make_pair((pc + ret) % TS, v.second + 2);
	} else if(s[i] == 'r') {
		int r = (rand() << 30) + (rand() << 15) + rand();
		r = r % TS;
		return make_pair(r, 1);
	} else if(is_digit(s[i])) {
		UNIT num = 0;
		UNIT cnt = 0;
		while(is_digit(s[i])) {
			num = num * 10 + s[i] - '0';
			i = (i + 1) % TS;
			cnt++;
		}
		cnt = cnt % TS;
		return make_pair(num % TS, cnt);
	} else { printf("input is not value type.\n"); exit(-1); }
}
int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	FILE *fp = fopen(argv[1], "r");
	for(int i = 0; fscanf(fp, "%c", &buf[i++]) != EOF;);
	fclose(fp);

	int cnt = 0;
	for(int i = 0; i < TS; i++) if(buf[i] != '\n' && buf[i] != 0) s[cnt++] = buf[i];

	int len = strlen(buf);
	b = q = cnt + 1;

	while(1) {
		/*
		printf("@:%hu s[@]: %c  !:%hu ?:%hu\n", pc, s[pc], d, q);
		for(int j=0; j<100; j++) {
				printf("%1d",j%10);
				if(j%10==9) printf(" ");
		}
		printf("\n");
		for(int j=0; j<100; j++) {
				if(s[j] < 10) printf("%d",s[j]);
				else printf("%c",s[j] ? s[j] : ' ');
				if(j%10==9) printf(" ");
		}
		printf("\n\n");
		fflush(stdout);
		*/
		if(s[pc] == 0) break;

		if(s[pc] == '<' || s[pc] == '>') {
			if(s[(pc + 1) % TS] == '!') b += (s[pc] == '<') ? -1 : 1;
			else if(s[(pc + 1) % TS] == '?') q += (s[pc] == '<') ? -1 : 1;
			else { printf("[<,>][!,?] required\n"); exit(-1); }
			b %= TS;
			q %= TS;
			pc = (pc + 2) % TS;
		} else if(s[pc] == 'i' || s[pc] == 'o' || s[pc] == 'c') {
			if(s[(pc + 1) % TS] == '!') {
				if(s[pc] == 'i') scanf(FORMAT_STRING, &s[b]);
				else if(s[pc] == 'o') printf(FORMAT_STRING, s[b]);
				else printf("%c", s[b]);
			} else if(s[(pc + 1) % TS] == '?') {
				if(s[pc] == 'i') scanf(FORMAT_STRING, &s[q]);
				else if(s[pc] == 'o') printf(FORMAT_STRING, s[q]);
				else printf("%c", s[q]);
			} else { printf("[i,o,c][!,?] required\n"); exit(-1); }
			pc = (pc + 2) % TS;
		} else if(s[pc] == '+' || s[pc] == '-' || s[pc] == '*' || s[pc] == '/' ||
			s[pc] == '%' || s[pc] == '&' || s[pc] == '|' || s[pc] == '^') {
			UNIT aa, bb;
			if(s[(pc + 1) % TS] == '!') { aa = b; bb = q; }
			else if(s[(pc + 1) % TS] == '?') { aa = q, bb = b; }
			else { printf("! or ? required after operator\n"); exit(-1); }
			UNIT op[256] = {};
			op['+'] = (s[aa] + s[bb]) % TS;
			op['-'] = (s[aa] - s[bb]) % TS;
			op['*'] = (s[aa] * s[bb]) % TS;
			op['/'] = (s[aa] / s[bb]) % TS;
			op['%'] = (s[aa] % s[bb]) % TS;
			op['&'] = s[aa] & s[bb];
			op['|'] = s[aa] | s[bb];
			op['^'] = s[aa] ^ s[bb];
			s[aa] = op[s[pc] % 256];
			pc = (pc + 2) % TS;
		} else if(s[pc] == '@' || s[pc] == '!' || s[pc] == '?') {
			if(s[(pc + 1) % TS] == '=') {
				auto v = get_value_from(pc + 2);
				if(s[pc] == '@') { pc = v.first; continue; } else if(s[pc] == '!') b = v.first;
				else q = v.first;
				pc = (pc + 2 + v.second) % TS;
			} else { printf("= required after @ or ! or ?\n"); exit(-1); }
		} else if(s[pc] == '$') {
			auto v = get_value_from(pc + 1);
			pc = (pc + 1 + v.second) % TS;
			if(s[pc] != ',' || !is_digit(s[(pc + 1) % TS])) {
				printf("should follow $condition,pos format\n"); exit(-1);
			}
			pc = (pc + 1) % TS;
			UNIT num = 0;
			while(is_digit(s[pc])) {
				num = num * 10 + s[pc] - '0';
				pc = (pc + 1) % TS;
			}
			if(s[pc] != '[' && s[pc] != ']' && s[pc] != '~') {
				printf("pos should end with [, ], or ~\n"); exit(-1);
			}



			if(v.first == 0);
			else {
				if(s[pc] == '[' || s[pc] == ']') {
					UNIT pos = (pc + 1) % TS;
					for(UNIT cnt = 0; cnt < num; pos = (pos + ((s[pc] == ']') ? 1 : -1)) % TS) {
						if(s[pos] == '#') cnt = (cnt + 1) % TS;
					}
					pc = (pos + ((s[pc] == '[') ? 1 : -1)) % TS;
				} else if(s[pc] == '~') {
					UNIT pos = 0;
					for(UNIT cnt = 0; cnt < num; pos = (pos + 1) % TS) {
						if(s[pos] == '#') cnt = (cnt + 1) % TS;
					}
					pc = (pos - 1) % TS;
				} else printf("should be one of [, ], or ~\n");
			}
			pc = (pc + 1) % TS;
		} else if(s[pc] == '.' && (s[(pc + 1) % TS] == '!' || s[(pc + 1) % TS] == '?')) {
			auto symbol = s[(pc + 1) % TS];
			if(s[(pc + 2) % TS] == '=') {
				if(s[(pc + 3) % TS] == '\"') {
					pc = (pc + 4) % TS;
					while(s[pc] != '\"') {
						if(symbol == '!') s[b++] = s[pc];
						else s[q++] = s[pc];
						pc = (pc + 1) % TS;
						b %= TS;
						q %= TS;
					}
				} else if(s[(pc + 3) % TS] == 's') {
					auto v = get_value_from(pc + 4);
					auto n = v.first;
					UNIT digits[32] = {};
					int cnt = 0;
					while(n > 0) {
						digits[cnt++] = n % 10 + '0';
						n /= 10;
					}
					for(int i = 0; i < cnt; i++) {
						if(symbol == '!') s[b++] = digits[cnt - i - 1];
						else s[q++] = digits[cnt - i - 1];
						b %= TS;
						q %= TS;
					}
					pc = (pc + 3 + v.second) % TS;
				} else {
					auto v = get_value_from(pc + 3);
					if(s[(pc + 1) % TS] == '!') s[b] = v.first;
					else s[q] = v.first;
					pc = (pc + 3 + v.second) % TS;
				}
			} else { printf("= required after .! or .?\n"); exit(-1); }
		} else pc = (pc + 1) % TS;
	}
	return 0;
}


