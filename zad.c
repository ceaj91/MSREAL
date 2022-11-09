#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main (int argc, char** argv)
{
	FILE * fp_led;
	FILE * fp_button;
	FILE * fp_switch;

	float percentage=1.0;

	char *str_switch;
	char *str_button;

	char button_tval0,button_tval1;
	char switch_val0, switch_val1,switch_val2,switch_val3;

	int flag=0;
	long int period = 20000L;
	int x=0;
	float add_to_percentage=0.0;
	size_t size = 6;
	
	if(argc == 0){
		printf("Argumenti nisu prisutni");
		return -1;
	} 	

	while(1){
//------------------------------------------------------------------------------------- RAD SA SWITCHOM------------------------------------------	
	fp_switch = fopen ("/dev/switch", "r");
	if(fp_switch==NULL)
	{
		puts("Problem pri otvaranju /dev/switch");
		return -1;
	}

	str_switch = (char *)malloc(size+1); 
	getline(&str_switch, &size, fp_switch); 

	if(fclose(fp_switch))
	{
		puts("Problem pri zatvaranju /dev/switch");
		return -1;
	}	
	switch_val0 = str_switch[5] - 48;
	switch_val1 = str_switch[4] - 48;
	switch_val2 = str_switch[3] - 48;
	switch_val3 = str_switch[2] - 48; 
	x = switch_val0 * 1 + switch_val1 *2 + switch_val2*4;
//------------------------------------------------------------------------------------ RAD SA TASTEROM --------------------------------------------
	
	fp_button = fopen("/dev/button","r"); 
	if( fp_button == NULL){
		printf("Problem pri otvaranju fajla");
		return -1;
	}
	
	str_button = (char *) malloc(size+1);
	getline(&str_button, &size, fp_button);

	if(fclose(fp_button)){
		puts("problem pri zatvaranju fajla");
		return -1;
	}
	

	button_tval0 = str_button[5] - 48;
	button_tval1 = str_button[4] - 48;
	free(str_button);

	if (flag == 0){ 
		add_to_percentage = x*0.05;
		if(button_tval0 == 1){
			printf("TASTER 0\n");
			flag =1;
			if(percentage + add_to_percentage <= 1)
				percentage += add_to_percentage;
		}
		else if(button_tval1 == 1){
			printf("TASTER 1\n");
			flag =1; 
			if(percentage - add_to_percentage >= 0)
				percentage -= add_to_percentage;
		}
	}
	else if(button_tval0 ==0 && button_tval1==0)
		flag = 0;
	
	printf("percentage: %f add_to_percentage: %f\n",percentage,add_to_percentage);
		
	//upali diode
//------------------------------------------------------------------------------- RAD SA LEDOM------------------------------------------	
	fp_led = fopen("/dev/led", "w");
	if(switch_val3 == 1) {
	fputs("0xf\n",fp_led);}
	else
	fputs("0x0\n", fp_led);	
	fclose(fp_led);
	usleep(period*percentage);
	
	fp_led = fopen("/dev/led", "w");
	fputs("0x0\n", fp_led);
	fclose(fp_led);
	usleep(period*(1-percentage));
	//ugasi diode 

	}


} 

