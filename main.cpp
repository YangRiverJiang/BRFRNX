
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LINE 512

#define MAX_SAT_PER_SYS 64
#define OBS_TYPES "SYS / # / OBS TYPES"
#define RINEX_VERSION_TYPE "RINEX VERSION / TYPE"
#define END_OF_HEADER "END OF HEADER"

#define RINEX_WORKABLE_1 "     3.02           OBSERVATION DATA"
#define RINEX_WORKABLE_2 "     3.04           OBSERVATION DATA"

#define TYPES_MAX 50

#define GPS_FLAG 'G'
#define GLO_FLAG 'R'
#define GAL_FLAG 'E'
#define BDS_FLAG 'C'
#define QZS_FLAG 'J'
#define IRN_FLAG 'I'
#define LEO_FLAG 'L'
#define SBS_FLAG 'S'

char line_buffer[MAX_LINE] = "";
char SVN[5] = "";

bool sys_enable[8] = { false };
char *** sys_obs_types = NULL;
int sys_obs_num[8] = { 0 };
int sys_num = 0;

struct OBS_FRAME {
	double ** GPS = NULL;
	double ** GLO = NULL;
	double ** GAL = NULL;
	double ** BDS = NULL;
	double ** QZS = NULL;
	double ** IRN = NULL;
	double ** LEO = NULL;
	double ** SBS = NULL;
};
OBS_FRAME OBS;

void init_obs() {
	OBS.GPS = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.GLO = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.GAL = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.BDS = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.QZS = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.IRN = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.LEO = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	OBS.SBS = (double**)malloc(sizeof(double*) * MAX_SAT_PER_SYS);
	
	for (int i = 0; i < MAX_SAT_PER_SYS; i++) {
		OBS.GPS[i] = (double*)malloc(sizeof(double) * sys_obs_num[0]);
		OBS.GLO[i] = (double*)malloc(sizeof(double) * sys_obs_num[1]);
		OBS.GAL[i] = (double*)malloc(sizeof(double) * sys_obs_num[2]);
		OBS.BDS[i] = (double*)malloc(sizeof(double) * sys_obs_num[3]);
		OBS.QZS[i] = (double*)malloc(sizeof(double) * sys_obs_num[4]);
		OBS.IRN[i] = (double*)malloc(sizeof(double) * sys_obs_num[5]);
		OBS.LEO[i] = (double*)malloc(sizeof(double) * sys_obs_num[6]);
		OBS.SBS[i] = (double*)malloc(sizeof(double) * sys_obs_num[7]);

		memset(OBS.GPS[i], 0, sizeof(double) * sys_obs_num[0]);
		memset(OBS.GLO[i], 0, sizeof(double) * sys_obs_num[1]);
		memset(OBS.GAL[i], 0, sizeof(double) * sys_obs_num[2]);
		memset(OBS.BDS[i], 0, sizeof(double) * sys_obs_num[3]);
		memset(OBS.QZS[i], 0, sizeof(double) * sys_obs_num[4]);
		memset(OBS.IRN[i], 0, sizeof(double) * sys_obs_num[5]);
		memset(OBS.LEO[i], 0, sizeof(double) * sys_obs_num[6]);
		memset(OBS.SBS[i], 0, sizeof(double) * sys_obs_num[7]);
	}
}

void parse_sys_types(char * line) {

	static char last_sys = ' ';
	char ** obs_types = NULL;
	int * obs_num = NULL;
	int start = 0;

	if (line[0] == ' ') {
		line[0] = last_sys;
		start = 13;
	}

	if (line[0] == GPS_FLAG) { sys_num++; sys_enable[0] = true;       last_sys = 'G';  obs_types = sys_obs_types[0]; obs_num = sys_obs_num + 0; }
	else if (line[0] == GLO_FLAG) { sys_num++; sys_enable[1] = true;  last_sys = 'R';  obs_types = sys_obs_types[1]; obs_num = sys_obs_num + 1; }
	else if (line[0] == GAL_FLAG) { sys_num++; sys_enable[2] = true;  last_sys = 'E';  obs_types = sys_obs_types[2]; obs_num = sys_obs_num + 2; }
	else if (line[0] == BDS_FLAG) { sys_num++; sys_enable[3] = true;  last_sys = 'C';  obs_types = sys_obs_types[3]; obs_num = sys_obs_num + 3; }
	else if (line[0] == QZS_FLAG) { sys_num++; sys_enable[4] = true;  last_sys = 'J';  obs_types = sys_obs_types[4]; obs_num = sys_obs_num + 4; }
	else if (line[0] == IRN_FLAG) { sys_num++; sys_enable[5] = true;  last_sys = 'I';  obs_types = sys_obs_types[5]; obs_num = sys_obs_num + 5; }
	else if (line[0] == LEO_FLAG) { sys_num++; sys_enable[6] = true;  last_sys = 'L';  obs_types = sys_obs_types[6]; obs_num = sys_obs_num + 6; }
	else if (line[0] == SBS_FLAG) { sys_num++; sys_enable[7] = true;  last_sys = 'S';  obs_types = sys_obs_types[7]; obs_num = sys_obs_num + 7; }

	if (*obs_num == 0) {
		sscanf(line + 1, "%d", obs_num);
	}

	for (int i = start; i < *obs_num; i++) {
		if (sscanf(line + 6 + 4 * (i  - start), "%s", obs_types[i]) == 0) break;
	}
}

bool skip_obs_header(FILE * fp)
{
	sys_num = 0;
	while (!feof(fp))
	{
		fgets(line_buffer, MAX_LINE, fp);
		if (strncmp(line_buffer + 60, OBS_TYPES, 19) == 0) {
			parse_sys_types(line_buffer);
		}
		else if (strncmp(line_buffer + 60, RINEX_VERSION_TYPE, 20) == 0) {
			if (strncmp(line_buffer, RINEX_WORKABLE_1, 36) != 0 && strncmp(line_buffer, RINEX_WORKABLE_2, 36) != 0) {
				printf("not a processable file version/type: %s\n", line_buffer);
				system("pause");
				exit(0);
			}
		}
		else if (strncmp(line_buffer + 60, END_OF_HEADER, 13) == 0)
		{
			sys_num = 8;
			return true;
		}
	}
	
	return false;
}

int ** obs_l = NULL;
int ** obs_d = NULL;
int * freq_num = NULL;
double ** wave_length = NULL;

void fetch_l_d_wl() {
	obs_l = (int**)malloc(sizeof(int*) * sys_num);
	obs_d = (int**)malloc(sizeof(int*) * sys_num);
	freq_num = (int*)malloc(sizeof(int) * sys_num);
	wave_length = (double**)malloc(sizeof(double*) * sys_num);
}

void cycleslip_detector(OBS_FRAME * in, int ** Ls, int ** Ds, double ** wavelength, int num_constellation, int * num_frequency) {

}

float utc_obs[6] = { 0 };
int all_sat_num = 0;
int sat_num_sys[8] = { 0 };

void reset() {
	memset(utc_obs, 0, sizeof(float) * 6);
	all_sat_num = 0;
	memset(sat_num_sys, 0, sizeof(int) * 8);

	for (int i = 0; i < MAX_SAT_PER_SYS; i++) {
		memset(OBS.GPS[i], 0, sizeof(double) * sys_obs_num[0]);
		memset(OBS.GLO[i], 0, sizeof(double) * sys_obs_num[1]);
		memset(OBS.GAL[i], 0, sizeof(double) * sys_obs_num[2]);
		memset(OBS.BDS[i], 0, sizeof(double) * sys_obs_num[3]);
		memset(OBS.QZS[i], 0, sizeof(double) * sys_obs_num[4]);
		memset(OBS.IRN[i], 0, sizeof(double) * sys_obs_num[5]);
		memset(OBS.LEO[i], 0, sizeof(double) * sys_obs_num[6]);
		memset(OBS.SBS[i], 0, sizeof(double) * sys_obs_num[7]);
	}
}

// obs type visibility
unsigned int ** sat_vis = NULL;
unsigned int *** obs_vis = NULL;

void fetch_obs(FILE * fp, bool parse_obs = false)
{
	int health = 0;

	fgets(line_buffer, MAX_LINE, fp);
	sscanf(line_buffer, "> %f %f %f %f %f %f %d %d",
		&utc_obs[0], &utc_obs[1], &utc_obs[2], &utc_obs[3], &utc_obs[4], &utc_obs[5], &health, &all_sat_num);

	for (int i = 0; i < all_sat_num; i++)
	{
		fgets(SVN, 5, fp);
		fgets(line_buffer, MAX_LINE, fp);

		if (SVN[0] == GPS_FLAG)sat_num_sys[0] ++;
		else if (SVN[0] == GLO_FLAG)sat_num_sys[1] ++;
		else if (SVN[0] == GAL_FLAG)sat_num_sys[2] ++;
		else if (SVN[0] == BDS_FLAG)sat_num_sys[3] ++;
		else if (SVN[0] == QZS_FLAG)sat_num_sys[4] ++;
		else if (SVN[0] == IRN_FLAG)sat_num_sys[5] ++;
		else if (SVN[0] == LEO_FLAG)sat_num_sys[6] ++;
		else if (SVN[0] == SBS_FLAG)sat_num_sys[7] ++;

		if (parse_obs) {
			char tobe[14] = "";
			int index = 0;
			int svn = atoi(SVN + 1);
			if (SVN[0] == GPS_FLAG) { 
				sat_vis[0][svn - 1]++;
				for (int j = 0; j < sys_obs_num[0]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if(OBS.GPS[svn - 1][index] = atof(tobe))
						obs_vis[0][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == GLO_FLAG) { 
				sat_vis[1][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[1]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.GLO[svn - 1][index] = atof(tobe))
						obs_vis[1][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == GAL_FLAG) 
			{ 
				sat_vis[2][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[2]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.GAL[svn - 1][index] = atof(tobe))
						obs_vis[2][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == BDS_FLAG) { 
				sat_vis[3][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[3]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.BDS[svn - 1][index] = atof(tobe))
						obs_vis[3][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == QZS_FLAG) 
			{ 
				sat_vis[4][svn - 1]++;
				for (int j = 0; j < sys_obs_num[4]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.QZS[svn - 1][index] = atof(tobe))
						obs_vis[4][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == IRN_FLAG) 
			{ 
				sat_vis[5][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[5]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.IRN[svn - 1][index] = atof(tobe))
						obs_vis[5][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == LEO_FLAG) { 
				sat_vis[6][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[6]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.LEO[svn - 1][index] = atof(tobe))
						obs_vis[6][svn - 1][index]++;
					index++;
				}
			}
			else if (SVN[0] == SBS_FLAG) { 
				sat_vis[7][svn - 1]++; 
				for (int j = 0; j < sys_obs_num[7]; j++) {
					if (16 * j + 13 > strlen(line_buffer)) {
						break;
					}
					strncpy(tobe, line_buffer + 16 * j, 13);
					if (OBS.SBS[svn - 1][index] = atof(tobe))
						obs_vis[7][svn - 1][index]++;
					index++;
				}
			}
		}
	}
}

float epoch_diff(float * a, float * b) { // return = a - b
	return (((((a[0] - b[0]) * 12 + (a[1] - b[1])) * 31 + (a[2] - b[2])) * 24 + (a[3] - b[3])) * 60 + (a[4] - b[4])) * 60 + (a[5] - b[5]);
}

bool is_big_month(int month)
{
	int big_month[]{ 1,3,5,7,8,10,12 };
	for (int i = 0; i < 7; i++)
	{
		if (big_month[i] == month) return true;
	}
	return false;
}
int date_amount_of_month(int year, int month)
{
	if (month != 2)
	{
		return is_big_month(month) ? 31 : 30;
	}
	else {
		if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
		{
			return 29;
		}
		else {
			return 28;
		}
	}
}

void epoch_forward(float * a, double b) // return = a + b
{
	a[5] += (float)b;
	while (a[5] >= 60) {
		a[4]++;
		a[5] -= 60;
	}
	while (a[4] >= 60) {
		a[3]++;
		a[4] -= 60;
	}
	while (a[3] >= 24) {
		a[2]++;
		a[3] -= 24;
	}

}

// first-hand brief
float first_epoch[6] = { 0 };
float last_epoch[6] = { 0 };
int epoch_count = 0;
double interval = 0;

// constellation visibility
int ** sat_num_list;
int * sat_num_all_list;
double sat_num_all_ave = 0;
int sat_num_all_min = 0;
int sat_num_all_max = 0;

double sat_num_all_3std = 0;
double sat_num_ave[8] = { 0 };
double sat_num_3std[8] = { 0 };
int sat_num_min[8] = { 0 };
int sat_num_max[8] = { 0 };


// measurement missing
int missing_total = 0;
float ** missing_time;

void period_description(float p, char * out) {
	const int sec_of_day = 24 * 60 * 60;
	const int sec_of_hour = 60 * 60;

	char ele[30] = "";

	int day = (int)floor(p / sec_of_day);
	int hour = (int)floor((p - day * sec_of_day) / sec_of_hour);
	int min = (int)floor((p - day * sec_of_day - hour * sec_of_hour) / 60);
	float sec = p - day * sec_of_day - hour * sec_of_hour - min * 60;

	if (day != 0) {
		sprintf(ele, "%dd ", day);
		strcat(out, ele);
	}
	if (hour != 0) {
		sprintf(ele, "%dh ", hour);
		strcat(out, ele);
	}
	if (min != 0) {
		sprintf(ele, "%dm ", min);
		strcat(out, ele);
	}
	if (sec != 0) {
		sprintf(ele, "%5.3fs", sec);
		strcat(out, ele);
	}

}

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;

	sys_obs_types = (char***)malloc(sizeof(char**) * 8);
	for (int i = 0; i < 8; i++) {
		sys_obs_types[i] = (char**)malloc(sizeof(char*) * TYPES_MAX);
		for (int j = 0; j < TYPES_MAX; j++) {
			sys_obs_types[i][j] = (char*)malloc(sizeof(char) * 4);
			memset(sys_obs_types[i][j], 0, sizeof(char) * 4);
		}
	}

	FILE * rnx = fopen(argv[1], "r");

	if (!rnx) return 0;
	skip_obs_header(rnx);
	init_obs();

	
	fetch_obs(rnx);
	memcpy(first_epoch, utc_obs, sizeof(float) * 6);
	memcpy(last_epoch, utc_obs, sizeof(float) * 6);
	epoch_count++;
	reset();

	printf("----first time checking----\n");

	while (!feof(rnx)) {
		fetch_obs(rnx);

		double current_interval = round(epoch_diff(utc_obs, last_epoch) * 100) / 100;

		if (current_interval <= 0) break;

		if (interval == 0 || current_interval < interval) interval = current_interval;

		epoch_count++;
		memcpy(last_epoch, utc_obs, sizeof(float) * 6);

		reset();
	}

	fclose(rnx);

	printf("----constellation visibility----\n");

	sat_num_all_list = (int*)malloc(sizeof(int) * epoch_count);
	memset(sat_num_all_list, 0, sizeof(int) * epoch_count);

	sat_num_list = (int**)malloc(sizeof(int*) * epoch_count);
	for (int i = 0; i < epoch_count; i++) {
		sat_num_list[i] = (int*)malloc(sizeof(int) * 8);
		memset(sat_num_list[i], 0, sizeof(int) * 8);
	}

	rnx = fopen(argv[1], "r");
	if (!rnx) return 0;

	skip_obs_header(rnx);
	for (int i = 0; i < epoch_count; i++) {
		fetch_obs(rnx);
		memcpy(sat_num_list[i], sat_num_sys, sizeof(int) * 8);
		sat_num_all_list[i] = all_sat_num;
		reset();
	}

	fclose(rnx);

	unsigned long sat_num_sum = 0;
	sat_num_all_max = sat_num_all_list[0];
	sat_num_all_min = sat_num_all_list[0];
	for (int i = 0; i < epoch_count; i++) {
		sat_num_sum += sat_num_all_list[i];
		if (sat_num_all_max < sat_num_all_list[i])sat_num_all_max = sat_num_all_list[i];
		if (sat_num_all_min > sat_num_all_list[i])sat_num_all_min = sat_num_all_list[i];
	}
	sat_num_all_ave = sat_num_sum * 1.0 / epoch_count;
	sat_num_all_3std = 0;
	for (int i = 0; i < epoch_count; i++) {
		sat_num_all_3std += ((sat_num_all_list[i] - sat_num_all_ave) * (sat_num_all_list[i] - sat_num_all_ave));
		
	}
	sat_num_all_3std = sqrt(sat_num_all_3std / (epoch_count - 1)) * 3;

	for (int j = 0; j < 8; j++) {
		sat_num_sum = 0;
		sat_num_max[j] = sat_num_list[0][j];
		sat_num_min[j] = sat_num_list[0][j];
		for (int i = 0; i < epoch_count; i++) {
			sat_num_sum += sat_num_list[i][j];
			if (sat_num_max[j] < sat_num_list[i][j])sat_num_max[j] = sat_num_list[i][j];
			if (sat_num_min[j] > sat_num_list[i][j])sat_num_min[j] = sat_num_list[i][j];
		}
		sat_num_ave[j] = sat_num_sum * 1.0 / epoch_count;
		sat_num_3std[j] = 0;
		for (int i = 0; i < epoch_count; i++) {
			sat_num_3std[j] += ((sat_num_list[i][j] - sat_num_ave[j]) * (sat_num_list[i][j] - sat_num_ave[j]));
		}
		sat_num_3std[j] = sqrt(sat_num_3std[j] / (epoch_count - 1)) * 3;
	}
	printf("----obs type visibility----\n");

	sat_vis = (unsigned int **)malloc(sizeof(unsigned int *) * sys_num);
	obs_vis = (unsigned int ***)malloc(sizeof(unsigned int **) * sys_num);
	for (int i = 0; i < sys_num; i++) {
		sat_vis[i] = (unsigned int *)malloc(sizeof(unsigned int) * MAX_SAT_PER_SYS);
		memset(sat_vis[i], 0, sizeof(unsigned int) * MAX_SAT_PER_SYS);

		obs_vis[i] = (unsigned int **)malloc(sizeof(unsigned int*) * MAX_SAT_PER_SYS);
		for (int j = 0; j < MAX_SAT_PER_SYS; j++) {
			obs_vis[i][j] = (unsigned int*)malloc(sizeof(unsigned int) * sys_obs_num[i]);
			memset(obs_vis[i][j], 0, sizeof(unsigned int) * sys_obs_num[i]);
		}
	}
	rnx = fopen(argv[1], "r");
	if (!rnx) return 0;

	skip_obs_header(rnx);

	while (!feof(rnx)) {

		fetch_obs(rnx, true);

		reset();
	}

	fclose(rnx);

	printf("----missing measurement checking----\n");

	missing_time = (float**)malloc(sizeof(float*) * epoch_count);
	for (int i = 0; i < epoch_count; i++)
	{
		missing_time[i] = (float*)malloc(sizeof(float) * 6);
		memset(missing_time[i], 0, sizeof(float) * 6);
	}
	missing_total = 0;

	rnx = fopen(argv[1], "r");
	if (!rnx) return 0;

	skip_obs_header(rnx);
	fetch_obs(rnx);
	memcpy(first_epoch, utc_obs, sizeof(float) * 6);
	memcpy(last_epoch, utc_obs, sizeof(float) * 6);
	reset();

	while (!feof(rnx)) {
		fetch_obs(rnx);
		double current_interval = round(epoch_diff(utc_obs, last_epoch) * 100) / 100;
		if (current_interval <= 0) break;
		if (current_interval > 0.3)
		{
			int k = 0;
		}
		int missing_num = (int)floor(current_interval / interval) - 1;
		if (missing_num != 0) {
			for (int i = 0; i < missing_num; i++) {
				memcpy(missing_time[missing_total], utc_obs, sizeof(float) * 6);
				epoch_forward(missing_time[missing_total], (i + 1) * interval);
				missing_total++;
			}
		}

		memcpy(last_epoch, utc_obs, sizeof(float) * 6);
		reset();
	}

	fclose(rnx);

	char brief_name[512] = "";
	strcpy(brief_name, argv[1]);
	for (int i = 0; i < strlen(brief_name); i++) {
		if (brief_name[i] == '.') {
			brief_name[i] = '\0';
			break;
		}
	}
	strcat(brief_name, ".brief.txt");

	printf("----brief conducting----\n");

	FILE * brf = fopen(brief_name, "w");

	fprintf(brf, "file: %s\n", argv[1]);

	fprintf(brf, "duration: %04d/%02d/%02d-%02d:%02d:%06.3f    --->    %04d/%02d/%02d-%02d:%02d:%06.3f\n",
		(int)first_epoch[0], (int)first_epoch[1], (int)first_epoch[2], (int)first_epoch[3], (int)first_epoch[4], first_epoch[5],
		(int)last_epoch[0], (int)last_epoch[1], (int)last_epoch[2], (int)last_epoch[3], (int)last_epoch[4], last_epoch[5]
	);

	char time_desc[256] = "";
	period_description(epoch_diff(last_epoch, first_epoch), time_desc);
	fprintf(brf, "which is %s\n", time_desc);
	if (interval <= 1) {
		fprintf(brf, "sample rate: %dHz\n", (int)round(1 / interval));
	}
	else {
		fprintf(brf, "sample interval: %ds\n", (int)round(interval));
	}
	fprintf(brf, "epoch count: %d\n", epoch_count);

	fprintf(brf, "\nconstellation visibility\nAll: %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_all_ave, sat_num_all_3std, sat_num_all_min, sat_num_all_max);

	if (sys_enable[0]) fprintf(brf, "\n G : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[0], sat_num_3std[0], sat_num_min[0], sat_num_max[0]);
	if (sys_enable[1]) fprintf(brf, "\n R : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[1], sat_num_3std[1], sat_num_min[1], sat_num_max[1]);
	if (sys_enable[2]) fprintf(brf, "\n E : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[2], sat_num_3std[2], sat_num_min[2], sat_num_max[2]);
	if (sys_enable[3]) fprintf(brf, "\n C : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[3], sat_num_3std[3], sat_num_min[3], sat_num_max[3]);
	if (sys_enable[4]) fprintf(brf, "\n J : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[4], sat_num_3std[4], sat_num_min[4], sat_num_max[4]);
	if (sys_enable[5]) fprintf(brf, "\n I : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[5], sat_num_3std[5], sat_num_min[5], sat_num_max[5]);
	if (sys_enable[6]) fprintf(brf, "\n L : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[6], sat_num_3std[6], sat_num_min[6], sat_num_max[6]);
	if (sys_enable[7]) fprintf(brf, "\n S : %5.2lf +- %4.2lf, min : %2d, max : %2d", sat_num_ave[7], sat_num_3std[7], sat_num_min[7], sat_num_max[7]);

	const char C[] = "GRECJILS";
	fprintf(brf, "\n\nobs type summerizing & missing total\n");
	for (int i = 0; i < sys_num; i++) {
		for (int j = 0; j < MAX_SAT_PER_SYS; j++) {
			if (sat_vis[i][j] > 0) {
				int * missing = (int*)malloc(sizeof(int) * sys_obs_num[i]);
				int obs_num = 0;
				memset(missing, 0, sizeof(int) * sys_obs_num[i]);
				fprintf(brf, "\n%c%02d : %7d epochs, ", C[i], j + 1, sat_vis[i][j]);
				for (int k = 0; k < sys_obs_num[i]; k++) {
					if (obs_vis[i][j][k] > 0) {
						fprintf(brf, "%3s ", sys_obs_types[i][k]);
						missing[k] = sat_vis[i][j] - obs_vis[i][j][k];
						obs_num++;
					}
				}

				char blank[128] = "                                                                                                                               ";
				blank[85 - obs_num * 4 - 21] = '\0';

				fprintf(brf, blank);
				bool have_missed = false;

				for (int k = 0; k < sys_obs_num[i]; k++) {
					if (missing[k] > 0) {

						fprintf(brf, "%3s : %6d, ", sys_obs_types[i][k], missing[k]);
						have_missed = true;
					}
				}
				if (!have_missed) {
					fprintf(brf, "fully observed, ");
				}
			}
		}
	}

	fprintf(brf, "\n\nmissing epochs\ntotal: %d, which is: %05.3f%%\n", missing_total, ceil((missing_total * 1.0 / (epoch_count + missing_total)) * 1000) / 10);
	for (int i = 0; i < missing_total; i++) {
		fprintf(brf, "%3d  %04d/%02d/%02d-%02d:%02d:%06.3f\n",
			i + 1, (int)missing_time[i][0], (int)missing_time[i][1], (int)missing_time[i][2], (int)missing_time[i][3], (int)missing_time[i][4], missing_time[i][5]
		);
	}

	fclose(brf);

	printf("----finished----\n");
	system(brief_name);
	system("pause");

}
