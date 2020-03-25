#include<gtk/gtk.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

/*Variables of integer type*/
static int 
	s, 						//Current second in program
	m, 						//Current minute in program
	h, 						//Current hour in program
	day, 					//Day in program
	year, 					//Year in program
	month, 					//month in program
	x, 						//variable to store the x-cooridinate of dateLabel
	timerRunning=0, 		//to tell program whether stopwatch is running or not
	stop, 					//to tell program to stop the stopwatch
	timerH=0, 				//hour in stopwatch
	timerM=0, 				//minute in stopwatch 
	timerS=0, 				//minute in stopwatch
	timerMs=0;				//one tenth of milisecond in stopwatch

/*Variables of GtkWidger type defined in gtk.h*/	
static GtkWidget 
	*fixed,					//Widget to contain other widgets
	*dateLabel, 			//Widget that displays date as in program
	*timeLabel,				//Widget that displays time as in program
	*amPmLabel, 			//Widget that displays AM or PM as in program
	*hourSpinButton, 		//Widget to get hour input from user
	*minuteSpinButton, 		//Widget to get minute input from user
	*label, 				//Widget to display colon(:) on screen
	*amPmButton, 			//Widget to change AM or PM
	*okButton, 				//OK button
	*timeFormatButton,		//Widget to show time format button
	*timeFormatLabel,		//Widget to display Time format(label) on screen
	*dayLabel,				//Widget to display Day(label) on screen
	*daySpinButton,			//Widget for day input
	*monthLabel,			//Widget to display Month(label) on screen
	*monthComboBox,			//Widget for month input
	*yearLabel,				//Widget to display Year(label) on screen
	*yearSpinButton,		//Widget for year input
	*stopwatchLabel,		//Widget to display stopwatch
	*resetButton,			//Reset Button
	*startButton,			//Start Button 
	*menu, 					//Menu Button					
	*popup,					//Widget to append all menu items
	*setMenu, 				//Widget to append setM and dateM
	*setM, 					//Menu item 'Set'
	*dateM, 				//Menu item 'Date'
	*timeM, 				//Menu item 'Time'
	*stopwatchM, 			//Menu item 'Stopwatch'
	*clockM,				//Menu item 'Clock'
	*window;				//Window widget

/*Enumerators*/
enum time_format
{
	AM_PM, _24HOUR
};
enum time_format timeFormat;

enum am_pm
{
	AM, PM
};
enum am_pm amPm;

enum month_name
{
	JAN, FEB, MARCH, APRIL, MAY, JUNE, JULY, AUG, SEP, OCT, NOV, DEC
};

enum in_which
{
	DATE, TIME, STOPWATCH, CLOCK
};
enum in_which inWhich=CLOCK;			//to tell program in which window user is

/*Structures*/
struct nDate
{
	int nDay:6;
	int nMonth:4;
	int nYear;
};

struct nTime
{
	int nHour:5;
	int nMinute:6;
	enum time_format nTimeFormat;
};

/*functions*/
void show_menu();						//Create menu button
void date_time_display();				//Create widget to display time and date 
void date_window();						//Create widget to display Set Date window
void time_window();						//Create widget to display Set Time window
void stopwatch_window();				//Create widget to display Stopwatch window
void currentTime(char *);				//Returns Current Time as a string
void currentDate(char *);				//Returns current date as a string
void change_date();						//It changes date variables
void monthBuffer(char *, int);			//Returns current month's string
void date_function();					//Shows Set Date Window widget and hides other widgets
void time_function();					//Shows Set Time Window widget and hides other widgets
void stopwatch_function();				//Shows Stopwatch Window widget and hides other widgets
void clock_function();					//Shows clock Window widget and hides other widgets
void amPmFunction();					//changes value of amPm
void timeFormatFunction();				//changes value of timeFormat
void okFunction();						//saves changes to date and time in date.dat and time.dat file respectively
void resetTimer();						//Resets stopwatch
void startTimer();						//Starts stopwatch
void on_month_change();					//updating the dateSpinButton upper limit on month change
void hide_setDate_window_widgets();		//hides set date window widgets
void hide_setTime_window_widgets();		//hides set time window widgets
void hide_stopwatch_window_widgets();	//hides stopwatch window widgets
void hide_clock_window_widgets();		//hides clock window widgets
gint change_time();						//It changes time variables
gint change_timer();					//updates stopwatch
int leap();								//Returns whether current year is leap(1) or not(0)
int max_day();							//Returns number of day in current month


int main(int argc, char *argv[]) 
{
	gtk_init(&argc, &argv);
	
	/*creating and setting window properties*/
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 300, 300);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  	gtk_window_set_title (GTK_WINDOW (window), "Clock");
	
	/*creating container that can place child widgets at fixed positions*/
	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	
	/*Calling funtions*/
	show_menu();
	date_time_display();
	
	/*function to display all child widgets of window*/
	gtk_widget_show_all(window);		
	
	/*Ok button for set Date & Time*/
	okButton=gtk_button_new_with_label("OK");
	gtk_fixed_put(GTK_FIXED(fixed), okButton, 115, 240);
	g_signal_connect(okButton, "clicked", G_CALLBACK(okFunction), NULL);
	
	/*Calling functions*/
	date_window();
	time_window();
	stopwatch_window();
	
	/*Linking Cascading Style Sheets(CSS) file*/
	GtkCssProvider *cssprovider= gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssprovider, "/Clock/theme.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
 		GTK_STYLE_PROVIDER(cssprovider), 
 		GTK_STYLE_PROVIDER_PRIORITY_USER);
	
	gtk_widget_hide(amPmLabel);
	if(timeFormat==AM_PM)
	{
		if(amPm==PM)
			gtk_label_set_text(GTK_LABEL(amPmLabel), "PM");
		gtk_widget_show(amPmLabel);
	}
	
	/*Function that runs the main loop untill gtk_main_quit() is called*/
	gtk_main();
	return 0;
}

void show_menu()
{
	menu=gtk_menu_button_new();
	popup=gtk_menu_new();
	setMenu=gtk_menu_new();

	setM=gtk_menu_item_new_with_label("Set");	
	dateM=gtk_menu_item_new_with_label("Date");
	timeM=gtk_menu_item_new_with_label("Time");
	stopwatchM=gtk_menu_item_new_with_label("Stopwatch");
	clockM=gtk_menu_item_new_with_label("Clock");	
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(setM), setMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(setMenu), dateM);
	gtk_menu_shell_append(GTK_MENU_SHELL(setMenu), timeM);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(popup), setM);
	gtk_menu_shell_append(GTK_MENU_SHELL(popup), stopwatchM);
	gtk_menu_shell_append(GTK_MENU_SHELL(popup), clockM);
	
	gtk_menu_button_set_popup(GTK_MENU_BUTTON(menu), popup);
	gtk_widget_show_all(popup);
	
	gtk_fixed_put(GTK_FIXED(fixed), menu, 0, 0);
	
	g_signal_connect(dateM, "activate", G_CALLBACK(date_function), NULL);
	g_signal_connect(timeM, "activate", G_CALLBACK(time_function), NULL);
	g_signal_connect(stopwatchM, "activate", G_CALLBACK(stopwatch_function), NULL);
	g_signal_connect(clockM, "activate", G_CALLBACK(clock_function), NULL);
}

void date_time_display()
{
	char timeBuffer[9], dateBuffer[18];
	currentTime(timeBuffer);
	currentDate(dateBuffer);
	timeLabel=gtk_label_new(timeBuffer);
	gtk_widget_set_name(timeLabel, "timeLabel");
	amPmLabel=gtk_label_new("AM");
	gtk_fixed_put(GTK_FIXED(fixed), amPmLabel, 260, 120);
	dateLabel=gtk_label_new(dateBuffer);
	gtk_widget_set_name(dateLabel, "dateLabel");
	gtk_fixed_put(GTK_FIXED(fixed), timeLabel, 25, 80);
	gtk_fixed_put(GTK_FIXED(fixed), dateLabel, x, 140);
	
	/*Calls change_time() after every 1 second untill it returns 0*/
	g_timeout_add(1000, change_time, NULL);
}

void date_window()
{
	int i;
	GtkAdjustment *dayAdjustment, *yearAdjustment;
	dayLabel=gtk_label_new("Day");
	
	dayAdjustment=gtk_adjustment_new(day, 1, max_day(), 1, 0, 0);
	daySpinButton=gtk_spin_button_new(dayAdjustment, 1, 0);
	
	monthLabel=gtk_label_new("Month");
	monthComboBox=gtk_combo_box_text_new();
	const char *mon[] = {
		"January", 
		"February", 
		"March", 
		"April", 
		"May", 
		"June", 
		"July", 
		"August", 
		"September", 
		"October", 
		"November", 
		"December"
	}; 
	/*G_N_ELEMENTS is a macro which determines the number of elements in an array.*/ 
	for (i=0; i<G_N_ELEMENTS(mon); i++)
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(monthComboBox), mon[i]);
	gtk_combo_box_set_active (GTK_COMBO_BOX(monthComboBox), month);
	
	g_signal_connect(monthComboBox, "changed", G_CALLBACK(on_month_change), NULL);
	
	yearLabel=gtk_label_new("Year");
	
	yearAdjustment=gtk_adjustment_new(year, 1, 9999, 1, 0, 0);
	yearSpinButton=gtk_spin_button_new(yearAdjustment, 1, 0);
	
	gtk_fixed_put(GTK_FIXED(fixed), dayLabel, 50, 78);
	gtk_fixed_put(GTK_FIXED(fixed), daySpinButton, 110, 70);
	gtk_fixed_put(GTK_FIXED(fixed), monthLabel, 50, 118);
	gtk_fixed_put(GTK_FIXED(fixed), monthComboBox, 110, 110);
	gtk_fixed_put(GTK_FIXED(fixed), yearLabel, 50, 158);
	gtk_fixed_put(GTK_FIXED(fixed), yearSpinButton, 110, 150);
}

void time_window()
{
	GtkAdjustment *hourAdjustment, *minuteAdjustment;
	int maxH, minH;
	if(timeFormat==AM_PM)
	{	maxH=12;
		minH=1;
	}
	else
	{	maxH=24;
		minH=0;
	}
	hourAdjustment=gtk_adjustment_new(h, minH, maxH, 1, 0, 0);
	hourSpinButton=gtk_spin_button_new(hourAdjustment, 1, 0);
	gtk_orientable_set_orientation(GTK_ORIENTABLE(hourSpinButton), GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_size_request(hourSpinButton, 65, 130);
	
	label=gtk_label_new(":");
	gtk_widget_set_name(label, "colon");
	
	minuteAdjustment=gtk_adjustment_new(m, 0, 59, 1, 0, 0);
	minuteSpinButton=gtk_spin_button_new(minuteAdjustment, 1, 0);
	gtk_orientable_set_orientation(GTK_ORIENTABLE(minuteSpinButton), GTK_ORIENTATION_VERTICAL);
	gtk_widget_set_size_request(minuteSpinButton, 65, 130);
	
	amPmButton=gtk_button_new_with_label("AM");
	gtk_widget_set_name(amPmButton, "amPmButton");
	timeFormatLabel=gtk_label_new("Time Format:");
	timeFormatButton=gtk_button_new_with_label("24 hour");

	gtk_fixed_put(GTK_FIXED(fixed), hourSpinButton, 39, 50);
	gtk_fixed_put(GTK_FIXED(fixed), label, 104, 82);
	gtk_fixed_put(GTK_FIXED(fixed), minuteSpinButton, 116, 50);
	if(timeFormat==_24HOUR)
	{
		gtk_fixed_move(GTK_FIXED(fixed), hourSpinButton, 69, 50);
		gtk_fixed_move(GTK_FIXED(fixed), label, 134, 82);
		gtk_fixed_move(GTK_FIXED(fixed), minuteSpinButton, 146, 50);
	}
	gtk_fixed_put(GTK_FIXED(fixed), amPmButton, 190, 90);
	gtk_fixed_put(GTK_FIXED(fixed), timeFormatLabel, 50, 199);
	gtk_fixed_put(GTK_FIXED(fixed), timeFormatButton, 150, 190);
	
	g_signal_connect(amPmButton, "clicked", G_CALLBACK(amPmFunction), NULL);
	g_signal_connect(timeFormatButton, "clicked", G_CALLBACK(timeFormatFunction), NULL);
}

void stopwatch_window()
{
	stopwatchLabel=gtk_label_new("00:00:00.00");
	gtk_widget_set_name(stopwatchLabel, "stopwatchLabel");
	
	resetButton=gtk_button_new_with_label("Reset");
	
	startButton=gtk_button_new_with_label("Start");
	
	gtk_fixed_put(GTK_FIXED(fixed), stopwatchLabel, 50, 110);
	gtk_fixed_put(GTK_FIXED(fixed), resetButton, 60, 240);
	gtk_fixed_put(GTK_FIXED(fixed), startButton, 155, 240);
	
	g_signal_connect(resetButton, "clicked", G_CALLBACK(resetTimer), NULL);
	g_signal_connect(startButton, "clicked", G_CALLBACK(startTimer), NULL);
}


void currentTime(char *timeBuffer)
{
	
	struct nDate nD;
	struct nTime nT;
	
	/*time_t is data type defined in time.h for storing system time values*/
	time_t seconds;
	
	/*tm is structure defined in time.h*/
	struct tm *current_time;
	
	/*time() returns current system time value*/
	seconds = time(NULL); 
	
	/*localtime() return the current time after converting from system time value to local time*/
	current_time = localtime(&seconds);
	
	/*reading the time.dat and date.dat file and modifing the current date and time according to it*/
	FILE *file, *fileT;
	file=fopen("date.dat", "rb");
	if(file==NULL)
		nD.nDay=nD.nMonth=nD.nYear=0;
	else
	{	fread(&nD, sizeof(nD), 1, file);
		fclose(file);
	}
	fileT=fopen("time.dat", "rb");
	if(fileT==NULL)
	{	nT.nHour=nT.nMinute=0;
		nT.nTimeFormat=_24HOUR;
	}
	else
	{	fread(&nT, sizeof(nT), 1, fileT);
		fclose(fileT);
	}
	timeFormat=nT.nTimeFormat;
	h=(current_time->tm_hour)-nT.nHour;
	if(timeFormat==AM_PM)
	{
		if(h>=13)
		{	h=h-12;
			amPm=PM;
		}
	}
	m=(current_time->tm_min)-nT.nMinute;
	s=current_time->tm_sec;
	day=(current_time->tm_mday)-nD.nDay;
	month=(current_time->tm_mon)-nD.nMonth;
	year=(current_time->tm_year+1900)-nD.nYear;
	sprintf(timeBuffer, "%02d:%02d:%02d", h, m, s);
}

void currentDate(char *dateBuffer)
{
	char monthString[9];
	monthBuffer(monthString, month);
	sprintf(dateBuffer, "%d %s, %d", day, monthString, year);
}

gint change_time() 
{ 	
	s++;
	if(s==60)
	{	s=0;
		m++;
		if(m==60)
		{	m=0;
			h++;
			if(timeFormat==AM_PM)
			{
				if(amPm==PM)
				{	if(h==12)
					{	h=0;
						change_date();
						amPm=AM;
						char buff2[18];
						currentDate(buff2);
						gtk_label_set_text(GTK_LABEL(dateLabel), buff2);	//updating the date
					}
				}
				else
				{	if(h>=13)
					{	h=h-12;
						amPm=PM;
					}
				}
			}
			else
			{
				if(h==24)
				{	h=0;
					change_date();
					char buff2[18];
					currentDate(buff2);
					gtk_label_set_text(GTK_LABEL(dateLabel), buff2);		//updating the date
				}
			}
		}
	}	
	char buffer[9];
	sprintf(buffer, "%02d:%02d:%02d", h, m, s);
	gtk_label_set_text(GTK_LABEL(timeLabel), buffer);	//updating the time
	if(timeFormat==AM_PM)
	{
		if(amPm==PM)
			gtk_label_set_text(GTK_LABEL(amPmLabel), "PM");
		else
			gtk_label_set_text(GTK_LABEL(amPmLabel), "AM");
	}
	return 1;
}

void change_date()
{
	day++;
	if(day>max_day())
	{	
		day=1;
		if(month==12)
		{	month=0;
			year++;
		}
	}	
}

int max_day()
{	
	if((month==APRIL)||(month==JUNE)||(month==SEP)||(month==NOV))
		return 30;
	else if(month==FEB)
		{	if(leap())
				return 29;
			else
				return 28;
		}
		else
			return 31;
}

int leap()
{
	if(year%4==0)
	{
		if(year%100==0)
		{
			if(year%400==0)
				return 1;
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

void monthBuffer(char *monthString, int a)
{
	switch(a)
	{
		case JAN:
			sprintf(monthString, "January");
			x=25;
			break;
		case FEB: 
			sprintf(monthString, "February");
			x=20;	
			break;
		case MARCH: 
			sprintf(monthString, "March");
			x=35;	
			break;
		case APRIL: 
			sprintf(monthString, "April");
			x=50;	
			break;
		case MAY: 
			sprintf(monthString, "May");
			x=50;
			break;
		case JUNE: 
			sprintf(monthString, "June");
			x=50;
			break;
		case JULY: 
			sprintf(monthString, "July");
			x=50;
			break;
		case AUG: 
			sprintf(monthString, "August");
			x=35;
			break;
		case SEP: 
			sprintf(monthString, "September");
			x=5;	
			break;
		case OCT: 
			sprintf(monthString, "October");
			x=25;
			break;
		case NOV: 
			sprintf(monthString, "November");
			x=10;
			break;
		case DEC: 
			sprintf(monthString, "December");
			x=10;
			break;
	}
}

void date_function()
{
	stop=1;
	gtk_window_set_title (GTK_WINDOW (window), "Set Date");
	if(inWhich==CLOCK)
	{	
		gtk_widget_hide(dateLabel);
		gtk_widget_hide(timeLabel);
		if(timeFormat==AM_PM)
			gtk_widget_hide(amPmLabel);
		gtk_widget_show(okButton);
	}
	else if(inWhich==TIME)
		{
			gtk_widget_hide(hourSpinButton);
			gtk_widget_hide(label);
			gtk_widget_hide(minuteSpinButton);
			if(timeFormat==AM_PM)
				gtk_widget_hide(amPmButton);
			gtk_widget_hide(timeFormatLabel);
			gtk_widget_hide(timeFormatButton);
		}
		else if(inWhich==STOPWATCH)
			{
				gtk_widget_hide(stopwatchLabel);
				gtk_widget_hide(resetButton);
				gtk_widget_hide(startButton);
				gtk_widget_show(okButton);
			}
	gtk_widget_show(dayLabel);
	gtk_widget_show(daySpinButton);
	gtk_widget_show(monthLabel);
	gtk_widget_show(monthComboBox);
	gtk_widget_show(yearLabel);
	gtk_widget_show(yearSpinButton);
	inWhich=DATE;
}

void on_month_change()
{
	month=gtk_combo_box_get_active(GTK_COMBO_BOX(monthComboBox));
	GtkAdjustment *newAdjustment=gtk_adjustment_new(day, 1, max_day(), 1, 0, 0);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(daySpinButton), newAdjustment);
}

void time_function()
{
	stop=1;
	gtk_window_set_title (GTK_WINDOW (window), "Set Time");
	if(inWhich==CLOCK)
	{	
		hide_clock_window_widgets();
		gtk_widget_show(okButton);
	}
	else if(inWhich==DATE)
		{
			hide_setDate_window_widgets();
		}
		else if(inWhich==STOPWATCH)
			{
				hide_stopwatch_window_widgets();
				gtk_widget_show(okButton);
			}
	gtk_widget_show(hourSpinButton);
	gtk_widget_show(label);
	gtk_widget_show(minuteSpinButton);
	if(timeFormat==AM_PM)
	{	
		if(amPm==PM)
			gtk_button_set_label(GTK_BUTTON(amPmButton), "PM");
		gtk_widget_show(amPmButton);
		gtk_button_set_label(GTK_BUTTON(timeFormatButton), "AM/PM");
	}
	gtk_widget_show(timeFormatLabel);
	gtk_widget_show(timeFormatButton);
	inWhich=TIME;
	
}

void stopwatch_function()
{
	gtk_window_set_title (GTK_WINDOW (window), "Stopwatch");
	if(inWhich==DATE)
	{	
		hide_setDate_window_widgets();
	}
	else if(inWhich==TIME)
		{
			hide_setTime_window_widgets();
		}
		else
		{	
			hide_clock_window_widgets();
		}
	gtk_widget_show(stopwatchLabel);
	gtk_widget_show(resetButton);
	gtk_widget_show(startButton);
	inWhich=STOPWATCH;
}

void clock_function()
{
	stop=1;
	gtk_window_set_title (GTK_WINDOW (window), "Clock");
	if(inWhich==DATE)
	{	
		hide_setDate_window_widgets();
	}
	else if(inWhich==TIME)
		{
			hide_setTime_window_widgets();
		}
		else if(inWhich==STOPWATCH)
			{
				hide_stopwatch_window_widgets();
			}
	gtk_widget_show(dateLabel);
	gtk_widget_show(timeLabel);
	if(timeFormat==AM_PM)
		gtk_widget_show(amPmLabel);
	inWhich=CLOCK;
	
}

void hide_setDate_window_widgets()
{
	gtk_widget_hide(dayLabel);
	gtk_widget_hide(daySpinButton);
	gtk_widget_hide(monthLabel);
	gtk_widget_hide(monthComboBox);
	gtk_widget_hide(yearLabel);
	gtk_widget_hide(yearSpinButton);	
	gtk_widget_hide(okButton);
}

void hide_setTime_window_widgets()
{
	gtk_widget_hide(hourSpinButton);
	gtk_widget_hide(label);
	gtk_widget_hide(minuteSpinButton);
	if(timeFormat==AM_PM)
		gtk_widget_hide(amPmButton);
	gtk_widget_hide(timeFormatLabel);
	gtk_widget_hide(timeFormatButton);
	gtk_widget_hide(okButton);
}

void hide_stopwatch_window_widgets()
{
	gtk_widget_hide(stopwatchLabel);
	gtk_widget_hide(resetButton);
	gtk_widget_hide(startButton);
}

void hide_clock_window_widgets()
{
	gtk_widget_hide(dateLabel);
	gtk_widget_hide(timeLabel);
	if(timeFormat==AM_PM)
		gtk_widget_hide(amPmLabel);
}

void amPmFunction()
{
	char ch[2];
	strcpy(ch, gtk_button_get_label(GTK_BUTTON(amPmButton)));
	if(strcmp(ch, "AM")==0)
	{	gtk_button_set_label(GTK_BUTTON(amPmButton), "PM");
		amPm=PM;
	}
	else
	{	gtk_button_set_label(GTK_BUTTON(amPmButton), "AM");
		amPm=AM;
	}
}

void timeFormatFunction()
{
	char ch[7];
	strcpy(ch, gtk_button_get_label(GTK_BUTTON(timeFormatButton)));
	if(strcmp(ch, "24 hour")==0)
	{	if(h>=13)
		{	h=h-12;
			amPm=PM;
		}
		gtk_button_set_label(GTK_BUTTON(timeFormatButton), "AM/PM");
		timeFormat=AM_PM;
		if(amPm==PM)
			gtk_button_set_label(GTK_BUTTON(amPmButton), "PM");
		else
			gtk_button_set_label(GTK_BUTTON(amPmButton), "AM");
		gtk_fixed_move(GTK_FIXED(fixed), hourSpinButton, 39, 50);
		gtk_fixed_move(GTK_FIXED(fixed), label, 104, 82);
		gtk_fixed_move(GTK_FIXED(fixed), minuteSpinButton, 116, 50);
		gtk_widget_show(amPmButton);
		GtkAdjustment *newAdjustment=gtk_adjustment_new(h, 1, 12, 1, 0, 0);
		gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(hourSpinButton), newAdjustment);
	}
	else
	{	gtk_button_set_label(GTK_BUTTON(timeFormatButton), "24 hour");
		timeFormat=_24HOUR;
		gtk_widget_hide(amPmButton);
		gtk_fixed_move(GTK_FIXED(fixed), hourSpinButton, 69, 50);
		gtk_fixed_move(GTK_FIXED(fixed), label, 134, 82);
		gtk_fixed_move(GTK_FIXED(fixed), minuteSpinButton, 146, 50);
		if(amPm==PM)
			h=h+12*amPm;
		GtkAdjustment *newAdjustment=gtk_adjustment_new(h, 0, 24, 1, 0, 0);
		gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(hourSpinButton), newAdjustment);
	}
}

void okFunction()
{	
	int H, M, S, Day, Month, Year;
	struct tm* current_time; 
	time_t seconds;
	seconds = time(NULL); 
	current_time = localtime(&seconds);
		
	H=current_time->tm_hour;
	M=current_time->tm_min;
	S=current_time->tm_sec;
	Day=(current_time->tm_mday);
	Month=(current_time->tm_mon);
	Year=(current_time->tm_year+1900);
	
	gtk_window_set_title (GTK_WINDOW (window), "Clock");

	if(inWhich==DATE)
	{	
		struct nDate nD;
		nD.nDay=Day-gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(daySpinButton));
		nD.nMonth=Month-month;
		nD.nYear=Year-gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(yearSpinButton));
		FILE *file;
		file=fopen("date.dat", "wb");
		fwrite(&nD, sizeof(nD), 1, file);
		fclose(file);
		hide_setDate_window_widgets();
		day=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(daySpinButton));
		year=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(yearSpinButton));
		char buff2[18];
		currentDate(buff2);
		gtk_label_set_text(GTK_LABEL(dateLabel), buff2);
		gtk_fixed_move(GTK_FIXED(fixed), dateLabel, x, 140);
	}
	else if(inWhich==TIME)
		{
			struct nTime nT;
			int temp=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(hourSpinButton));
			if(timeFormat==AM_PM)
				nT.nHour=H-temp-12*amPm;
			else
				nT.nHour=H-temp;
			nT.nMinute=M-gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(minuteSpinButton));
			nT.nTimeFormat=timeFormat;
			FILE *file;
			file=fopen("time.dat", "wb");
			fwrite(&nT, sizeof(nT), 1, file);
			fclose(file);
			hide_setTime_window_widgets();
			h=H-nT.nHour;
			if(timeFormat==AM_PM)
			{
				if(h>=13)
				{	h=h-12;
					amPm=PM;
				}
				else
					amPm=AM;
			}
			m=M-nT.nMinute;
		}
	gtk_widget_show(dateLabel);
	gtk_widget_show(timeLabel);
	if(timeFormat==AM_PM)
		gtk_widget_show(amPmLabel);
	inWhich=CLOCK;
}

void resetTimer()
{
	timerMs=timerS=timerM=timerH=0;
	gtk_label_set_text(GTK_LABEL(stopwatchLabel), "00:00:00.00");
}

void startTimer()
{
	if(timerRunning==0)
	{
		stop=0;
		gtk_button_set_label(GTK_BUTTON(startButton), "Pause");
		g_timeout_add(10, change_timer, NULL);
		timerRunning=1;
	}
	else
	{	stop=1;
		gtk_button_set_label(GTK_BUTTON(startButton), "Start");
		timerRunning=0;
	}
}

gint change_timer()
{	
	char buffer[12];
	if(stop==0)
	{
		timerMs++;
		if(timerMs==100)
		{
			timerMs=0;
			timerS++;
			if(timerS==60)
			{
				timerS=0;
				timerM++;
				if(timerM==60)
				{
					timerM=0;
					timerH++;
				}
			}
		}	
		sprintf(buffer, "%02d:%02d:%02d.%02d", timerH, timerM, timerS, timerMs);
		gtk_label_set_text(GTK_LABEL(stopwatchLabel), buffer);
		return 1;
	}
	else
		return 0;
}
