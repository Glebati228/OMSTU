#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "stdlib.h"
#include <unistd.h>

//Название городов для отрисовки в функции redraw()
char m[] = "moskov";
char n[] = "new york";
char t[] = "tokyo";
char l[] = "london";

//создание окна
Display *dspl; 
int screen;
Window hwnd; 
XEvent event; 
GC gc; //графический контекст 


pthread_mutex_t mtx, pmtx; // мьютекс для синхронизации процессов в функции PassesLogic(), в этой функции происходит изменение переменной planes[k].passengers и если её изменение не синхронизировать, то в самолёт сможет сесть больше 5 людей или наоборот выйти больше людей что невозмно

struct plane{ //структура самолёта 
int dirForCit; // локальное направление самолёта
int dir; // направление самолёта по часовой = 0 и против часовой = 1
int x, y; // позиция самолёта в окне программы
int passengers; // количество пассажиров в самолете
int speed; // скорость самолета 
int waitTime; // время ожидания самолета в городах
int tx, ty; // позиция точки в которую будет перемещаться точка
int waitFlag; // флаг ожидания ставится 1 когда самолёт засыпает в городе и ставится 0 когда он просыпается
int stopEvent;
};

int plCount = 2; // количество самолтеов, один по часовой стрелке другой против часовой стрелки используется 
int lock = 0, lock2 = 0; // флаги для функции CalcPosOfPlane, ставится 1 если была выведена строка с информацией о следующем перелёте
struct plane planes[2]; // инициализация двух самолётов 

int pascount = 0; // количество всех пассажиров
int f = 0, v = 0; 
int sizeV = 64; // размер города по X
int sizeH = 128; // размер города по Y

struct city{ // структура города
    int x, y; // их положения в окне (городов)
    int stopX, stopY; // позиции аэропортов
    int countOfPasses; // переменная которая содержит в себе информацию о количестве пассажиров в каждом городе, используется для визуального отоброжения в окне 
};

struct city cities[4]; // инициализация городов 4

struct pas{ // структура пассажира
  int x; // позиция по Х пассажира
  int y; // позиция по У пассажира
  int tx; // точка по Х в которую будет перемещён пассажир
  int ty; // точка по У в которую будет перемещён пассажир
  int sleep; // флаг сна который устанавливается 1 когда нить засыпает
  int maxSleepTime; // сколько времени максимально может уснуть нить
  int sleepTime; // время сна 
  int wait; // флаг ожидания, ставится 1 когда нить переместилась в аэропорт
  int isfly; // флаг полёта, ставится 1 когда нить перемещает из одоного города в другой
  int canLeave; // флаг возможности выйти из самолёта, ставится 1 когда isFly = 1 и waitFlag = 1
  int isStop; // флаг выхода из самолёта,ставится 1 если нить вышла из самолёта (isPlane = 0)
  int isCity; // флаг в городе, ставится 1 когда нить переместилась в город 
  int isPlane; // флаг нахождения в самолете, ставится 1 когда когда самолет ожидает пассажиров и wait пассажира равен 1
  int city; // номер города в котором сейчас находится нить
  int dir; // направления полёта пассажира по часовой или против часовой
};

struct pas passes[110]; // инициализация 100 пассажиров

int get_rand_range_int(const int min, const int max) { // функция получения случайного числа из диапазона
    return rand() % (max - min + 1) + min;
}

void SetDefaultProps(int i){ // функция которая устанавливает стандартные свойства пассажира, т.е в городе Москва, с случайным направлением

  passes[i].city = 0; // город москва
  passes[i].x = cities[0].x - 150; 
  passes[i].y = cities[0].y - 150;
  passes[i].dir = rand()%2;
  passes[i].tx = cities[passes[i].city].stopX;
  passes[i].ty = cities[passes[i].city].stopY;
  passes[i].sleepTime = passes[i].maxSleepTime + rand()%passes[i].maxSleepTime;

}

void MovePasses(int i){ // функция перемещаения нити, используется для перемещения к аэропорту, перемещению к самолету, перемещению прибывышей нити в город

  if(passes[i].x < passes[i].tx) passes[i].x++; 
  if(passes[i].x > passes[i].tx) passes[i].x--;
  if(passes[i].y < passes[i].ty) passes[i].y++;
  if(passes[i].y > passes[i].ty) passes[i].y--;

}

void InfoAboutPassengers(int arg){



}

void SetNewCourse(int arg, int k){ // функция, которая изменяет направление движения самолёта, а также выводит информацию о полётах самолётов в консоль

      planes[arg].tx = cities[k].x; // установка точки в которую будет перемещаться самолёт, в этом случае в центр города
      planes[arg].ty = cities[k].y;
	if(planes[arg].dir == 0){
		if(planes[arg].dirForCit == 0)
      			planes[arg].x += planes[arg].speed; // само перемещение самолёта путем простого инкрементирования его точки Х
		if(planes[arg].dirForCit == 1)
			planes[arg].y += planes[arg].speed;
		if(planes[arg].dirForCit == 2)
			planes[arg].x -= planes[arg].speed;
		if(planes[arg].dirForCit == 3)
			planes[arg].y -= planes[arg].speed;
	}

	if(planes[arg].dir == 1){
		if(planes[arg].dirForCit == 0)
	      		planes[arg].y += planes[arg].speed; // само перемещение самолёта путем простого инкрементирования его точки Х
		if(planes[arg].dirForCit == 1)
			planes[arg].x -= planes[arg].speed;
		if(planes[arg].dirForCit == 2)
			planes[arg].y -= planes[arg].speed;
		if(planes[arg].dirForCit == 3)
			planes[arg].x += planes[arg].speed;
	}
	if(lock != 1 && planes[arg].dir == 0){ // для самолёта летающего по часовой стрелки
		pthread_mutex_lock(&pmtx);
		f++; // инкрементирование переменной f для смещения следующего вывода в консоль
		lock = 1; // флаг, без которого бы в консоль выводилось очень много сообщений пока самолёт летит
		pthread_mutex_unlock(&pmtx);
		printf("\033[%dH\033[1m самолёт А летит в: %s\n", f, k == 1 ? n : k == 2 ? t : k == 3 ? l : m);	// вывод информации о дальнейших полётах самолёта А
	}
	if(lock2 != 1 && planes[arg].dir == 1){
		pthread_mutex_lock(&pmtx);
		v++;
		lock2 = 1;
		pthread_mutex_unlock(&pmtx);
		printf("\033[%d;60H\033[1m самолёт B летит в: %s\n", v, k == 3 ? l : k == 2 ? t : k == 1 ? n : m); // вывод информации о дальнейших полётах самолёта В
	}
		
	
}

void calcPosOfPlane(void *a) { // функция вычисляет позицию самолёта в зависимости от направления по часовой или против часовой стрелки, в этой функции также происходит изменеие флага waitFlag для planes[], когда они прибывают в город
	int o = 1, p = 1; // переменные для сдвига строк с информацией о полётах
	int arg  = (int)a; // получение номера самолёта
  while(1){ // бесконечный цикл перемещения самолётов

    if(planes[arg].dir == 0){ // если направление пассажира по часовой стрелки

      if(planes[arg].dirForCit == 0) // если направление в нью Йорк     		
		SetNewCourse(arg, 1);

      if(planes[arg].dirForCit == 1) // если направление в токио  
		SetNewCourse(arg, 2);

      if(planes[arg].dirForCit == 2) // если направление в Лондон
     		SetNewCourse(arg, 3);

      if(planes[arg].dirForCit == 3) // если направление в Москву
     		SetNewCourse(arg, 0);

      if(planes[arg].x == planes[arg].tx && planes[arg].y == planes[arg].ty){ // если мы прибыли в какой-то из городов
          if(planes[arg].dirForCit != 3) { // если город не больше 3
	  pthread_mutex_lock(&pmtx);
	  printf("\033[%d;30H\033[1m самолёт A прибыл в: %s\n", o, planes[arg].dirForCit == 0 ? n : planes[arg].dirForCit == 1 ? t : planes[arg].dirForCit == 2 ? l : "Error"); // вывод в консоль информации о том, что самолёт прибыл в какой-то город 
          planes[arg].dirForCit++;
	  o++;
	  pthread_mutex_unlock(&pmtx); 
          }
          else {
		pthread_mutex_lock(&pmtx);
		planes[arg].dirForCit = 0;
		printf("\033[%d;30H\033[1m самолёт А прибыл в: %s\n", o, m); // вывод в консоль информации о том, что самолёт прибыл в какой-то город 
		o++;
		pthread_mutex_unlock(&pmtx);
	  } // иначе возравщаемся в Москву

          planes[arg].waitFlag = 1; // установка флага 1 когда самолёт ждёт пассажиров
          usleep(planes[arg].waitTime); // ожидание пассажиров
          planes[arg].waitFlag = 0; // самолёт полетел в другой город
	  pthread_mutex_lock(&pmtx);
	  lock = 0;
	  pthread_mutex_unlock(&pmtx);
        }
    }

    if(planes[arg].dir == 1){ // если самолёт летит против часовой стрелки 

      if(planes[arg].dirForCit == 0) // если в Лондон
	        SetNewCourse(arg, 3);

      if(planes[arg].dirForCit == 1) // если в Токио   
		SetNewCourse(arg, 0);

      if(planes[arg].dirForCit == 2) // если в Нью Йорк
		SetNewCourse(arg, 1);

      if(planes[arg].dirForCit == 3) // если в Москву
     		SetNewCourse(arg, 2);

      if(planes[arg].x == planes[arg].tx && planes[arg].y == planes[arg].ty){ // если мы прибыли в какой-то из городов
        if(planes[arg].dirForCit != 0){ // если номер города самолёта не меньше 0
	  pthread_mutex_lock(&pmtx); // мьютекс для синхронизации вывода в консоль информации, а также инкрементирования переменной p которая используется для простого смещения выводимой информации в консоль
	  printf("\033[%d;90H\033[1m самолёт B прибыл в: %s\n", p, planes[arg].dirForCit == 3 ? t : planes[arg].dirForCit == 2 ? n : planes[arg].dirForCit == 1 ? m : "Error");
	  p++;
	  pthread_mutex_unlock(&pmtx);	
          planes[arg].dirForCit--;
        }else{
	  pthread_mutex_lock(&pmtx); // тот же мьютекс для синхронизации вывода и изменения данных
	  printf("\033[%d;90H\033[1m самолёт B прибыл в: %s\n", p, l);
	  p++;
	  pthread_mutex_unlock(&pmtx);
          planes[arg].dirForCit = 3;
	}
          planes[arg].waitFlag = 1; 
          usleep(planes[arg].waitTime);
          planes[arg].waitFlag = 0;

	  pthread_mutex_lock(&pmtx); // мьютекс для закрытия возможности другим нитям одновременно изменять переменную lock, которая является флагом закрытия возможности вывода в консоль более 1 раза одной и той же информации 
	  lock2 = 0;
	  pthread_mutex_unlock(&pmtx);
        }
    }

        usleep(20000); // засыпаем чтобы показать движение самолёта по городам
  }
}


void PassesLogic(void *k){ // функция которая отвечает за то, как пассажиры перемещаются по городам (при каких событиям пассажир может сесть в самолёт) 
	int i = (int)k; // получение номера пассажира
  SetDefaultProps(i); // установка стандартных свойств

  while(1){ // бесконечное перемещение пассажиров по городам
    if(passes[i].isStop == 1){ // если пассажиры готовы к перемещению в аэропорт
      MovePasses(i); // перемещение в аэропорт
      if(passes[i].x == passes[i].tx && passes[i].y == passes[i].ty){ // если в аэропорте
        passes[i].isStop = 0; 
        passes[i].wait = 1; // ожидаем самолёт
      }
    }
          if(passes[i].wait != 0 && abs(passes[i].x - planes[passes[i].dir].x) < 50 && abs(passes[i].y - planes[passes[i].dir].y) < 50 && planes[passes[i].dir].waitFlag == 1 && planes[passes[i].dir].passengers < 5) // если пассажир ждёт и расстояние до автобуса меньше 50 и самолёт ждет пассаижров и количество пассажиров внутри самолёта меньше 5
          {
            passes[i].tx = planes[passes[i].dir].x; // устанавлиаем направление перемещения к самолёту для нити
            passes[i].ty = planes[passes[i].dir].y; //
            passes[i].wait = 0; // больше не ожидаем самолет а садимся в него
            passes[i].isPlane  = 1; // садимся в самолёт
          }
        
        if(passes[i].isPlane == 1){ // если сели в самолет
          if(planes[passes[i].dir].waitFlag == 1) // если самолёт ждет 
          {
            if(planes[passes[i].dir].passengers < 5) // количество пассажиров в самолёте меньше 5
               MovePasses(i); // передвигаем пассажиров за самолётом
            else{
              passes[i].tx = cities[passes[i].city].stopX; // иначе возвращаемся обратно на остановку
              passes[i].ty = cities[passes[i].city].stopY; //
              passes[i].isStop = 1; // ставим флаг остановки 
              passes[i].isPlane = 0; // и выходим из самолёта
            }
            if(passes[i].x == passes[i].tx && passes[i].y == passes[i].ty){ // если мы в самолёте
              if(planes[passes[i].dir].passengers < 5) // и если  количество пассажиров меншьне 5
              {
                passes[i].isfly = 1; // для каждого сядшего в самолёт ставим флаг того что мы летим 
                pthread_mutex_lock(&mtx); // не позволяем нескольким нитям менять количество занятых пассажирами мест
                planes[passes[i].dir].passengers++; // увеличиваем количество занятых мест в самолёте
		cities[passes[i].city].countOfPasses--;
                pthread_mutex_unlock(&mtx);
                passes[i].canLeave = 0; // выйти из самолёта нельзя 
                passes[i].isPlane = 0;
              }
              else{
                passes[i].tx = cities[passes[i].city].stopX;
                passes[i].ty = cities[passes[i].city].stopY;
                passes[i].isStop = 1;
	              passes[i].isPlane = 0;
	            }
	         }
	      }else{
          passes[i].tx = cities[passes[i].city].stopX; // теперь наша нить будет лететь к следующей остановке
          passes[i].ty = cities[passes[i].city].stopY;
          passes[i].isStop = 1; 
          passes[i].isPlane = 0;
        }
      }
        if(passes[i].isfly == 1) // если мы летим в самолете
        {
          passes[i].x = planes[passes[i].dir].x; // мы изменяем позицию пассажира на позицию самолёта т.е. таким образом будет видно их перемещение
          passes[i].y = planes[passes[i].dir].y;
          if(planes[passes[i].dir].waitFlag == 0) 
		passes[i].canLeave = 1;
            if(planes[passes[i].dir].waitFlag == 1 && passes[i].canLeave ==1)
	          {
	             passes[i].isfly = 0;
	             passes[i].isCity = 1;
	             if(planes[passes[i].dir].passengers > 0) // т.е пока больше нуля
	             {
	                pthread_mutex_lock(&mtx); // выходим из самолёта и изменяем количество мест в самолёте
	                planes[passes[i].dir].passengers--;
	                pthread_mutex_unlock(&mtx);
	             }
	             if(passes[i].dir == 0) // если мы летели по часовой стрелки
	             {
	             if(passes[i].city != 3) {
		     	passes[i].city++;
			pthread_mutex_lock(&mtx);
			cities[passes[i].city].countOfPasses++;
			pthread_mutex_unlock(&mtx);
		     } // и если номер нашего города не равен 3, то номер города инкрементируем 
	             else {
			passes[i].city = 0;
			//pthread_mutex_lock(&mtx);
			//cities[3].countOfPasses++;
			//pthread_mutex_unlock(&mtx);
			
		     } // иначе мы вернулись обратно в Москву
			
	         }else{
	            if(passes[i].city != 0) passes[i].city--; // если
	                 else {
				passes[i].city = 3;
				pthread_mutex_lock(&mtx);
				cities[passes[i].city].countOfPasses++;
				pthread_mutex_unlock(&mtx);
			 }
	         }
	            passes[i].tx = cities[passes[i].city].x - 150 +rand()%300; // задаём случайную позицию в городе для пассажира, в которую он будет перемещён
	            passes[i].ty = cities[passes[i].city].y - 150 +rand()%300;
	            }
	        }

	      if(passes[i].isCity == 1) // если мы в городе 
	        {
          MovePasses(i); // передвижение пассажира в город

          if(passes[i].x == passes[i].tx && passes[i].y == passes[i].ty){
             	    passes[i].sleep = 1; // ставим флаг сна 
	            usleep(passes[i].sleepTime);
	            passes[i].sleep = 0; // убираем его 
	            passes[i].isCity = 0; 
	            passes[i].isStop = 1;
	            passes[i].wait = 0;
	            passes[i].dir = rand()%2; // выбираем новое случайное направление 
              passes[i].tx = cities[passes[i].city].stopX; // пассажир будет двигаться к аэропорту
              passes[i].ty = cities[passes[i].city].stopY;
	            }
	        }
	        usleep(20000);
	 }
}

void rotate(XPoint *points, int npoints, int rx, int ry, float angle) { // функция, которая вращает полигоны по часовой стрелки по заданным точкам и углу
	int i;
	for (i=0; i<npoints; i++) {
		float x, y;
		x = points[i].x - rx;
		y = points[i].y - ry;
		points[i].x = rx + x * cosf(angle) - y * sinf(angle); // изменение положение вершины полигона в зависимости от парамерта angle 
		points[i].y = ry + x * sinf(angle) + y * cosf(angle);
	}
}

int countM;

void redraw(int sleepTime){ // функция рисования в графическое окно, в ней находится бесконечный цикл который отрисовывает элементы (пассажиров, самолёты) изменение их координат, а также такие статичные обьекты как города, названия городов и т.д  

	char pc[14]; // массив символов в которую через snprintf записывается информация о количестве пассажиров при запуске программы 
	char cpop[30];
	//char copic[40];
	float angle = 0.0; // угол поворота самолёта
	int lock = 0; 
	XEvent event;
	KeySym key;
	char text[1];

	XNextEvent(dspl, &event);
	while(1){
		
		XPoint points1[3] = { { planes[0].x - 10, 90 + planes[0].y - 40 } , { 90 + planes[0].x, 45 + planes[0].y - 40 } , { planes[0].x - 10, 10 + planes[0].y - 40 } };
		XPoint points2[3] = { { planes[0].x - 60, 90 + planes[0].y - 40 } , { 40 + planes[0].x, 45 + planes[0].y - 40 } , { planes[0].x - 60, 10 + planes[0].y - 40 } };
		
		XPoint points3[3] = { { planes[1].x - 10, 90 + planes[1].y - 40 } , { 90 + planes[1].x, 45 + planes[1].y - 40 } , { planes[1].x - 10, 10 + planes[1].y - 40 } };
		XPoint points4[3] = { { planes[1].x - 60, 90 + planes[1].y - 40 } , { 40 + planes[1].x, 45 + planes[1].y - 40 } , { planes[1].x - 60, 10 + planes[1].y - 40 } };
		
		XPoint rpoints1[3], rpoints2[3], rpoints3[3], rpoints4[3];
		int i = 0;	
		int npoints1 = 3;	
		
		XClearWindow(dspl, hwnd);
		// добавление треугольников
		for (i = 0; i < npoints1; i++) rpoints1[i] = points1[i];
		for (i = 0; i < npoints1; i++) rpoints2[i] = points2[i];
		for (i = 0; i < npoints1; i++) rpoints3[i] = points3[i];
		for (i = 0; i < npoints1; i++) rpoints4[i] = points4[i];
	
		for(int i = 0; i < 4; i++){
			XDrawRectangle(dspl, hwnd, gc, cities[i].x - 150, cities[i].y - 150, 300, 300);
			sprintf(cpop, "passengers in city %d: %d", i, cities[i].countOfPasses);
			XDrawString(dspl, hwnd, gc, 800, 10 + ((i + 1) * 10), cpop, (cities[i].countOfPasses < 10) ? 23 : (cities[i].countOfPasses > 10 && cities[i].countOfPasses < 100) ? 24 : 25);
		}

         	for(int i = 0; i < pascount; i++)
	      		XDrawRectangle(dspl, hwnd, gc, passes[i].x-2, passes[i].y-2, 6,6);

		sprintf(pc, "passengers: %d", pascount);		
		XDrawString(dspl,hwnd, gc, 20, 20, pc, (pascount < 10) ? 13 : (pascount > 10 && pascount < 100) ? 14 : 15);
		sprintf(cpop, "passengers in planeA: %d", planes[0].passengers);
		XDrawString(dspl,hwnd, gc, 500, 20, cpop, 23);
		sprintf(cpop, "passengers in planeB: %d", planes[1].passengers);
		XDrawString(dspl,hwnd, gc, 500, 50, cpop, 23);

		XDrawString(dspl,hwnd, gc, 160, 90, m, sizeof(m)-1);
	        XDrawString(dspl,hwnd, gc, 1060, 90, n, sizeof(n)-1);
	        XDrawString(dspl,hwnd, gc, 1060, 470, t, sizeof(t)-1);
	        XDrawString(dspl,hwnd, gc, 160, 470, l, sizeof(l)-1);		
		
		for(int i = 0; i < 4; i++)
		if(i == 0 || i == 3)
			XDrawString(dspl, hwnd, gc, cities[i].stopX, cities[i].stopY, "airport", 7);
		else if(i == 2)
			XDrawString(dspl, hwnd, gc, cities[i].stopX - 30, cities[i].stopY, "airport", 7);
		else if(i == 1)
			XDrawString(dspl, hwnd, gc, cities[i].stopX - 30, cities[i].stopY, "airport", 7);		

		switch(planes[0].dirForCit){ // рисование треугольников, представляющих самолёты
			case 0:
				XDrawRectangle(dspl, hwnd, gc, planes[0].x - 16, planes[0].y -  4, 32, 16);
				angle = 0.0;	
				rotate(rpoints1, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints1, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints2, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints2, npoints1, Convex, CoordModeOrigin);
			break;			
 			
			case 2:
				XDrawRectangle(dspl, hwnd, gc, planes[0].x - 16, planes[0].y -  4, 32, 16);

				angle = 55.0 + 124.1;	
				rotate(rpoints1, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints1, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints2, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints2, npoints1, Convex, CoordModeOrigin);	
			break;	
			
			case 1:
				XDrawRectangle(dspl, hwnd, gc, planes[0].x - 8, planes[0].y - 8, 16, 32);
				angle = 55.0 + 122.5;	
				rotate(rpoints1, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints1, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints2, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints2, npoints1, Convex, CoordModeOrigin);
			break;

			case 3:
				XDrawRectangle(dspl, hwnd, gc, planes[0].x - 8, planes[0].y - 8, 16, 32);
				angle = 55.0 + 125.611;	
				rotate(rpoints1, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints1, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints2, npoints1, planes[0].x, planes[0].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints2, npoints1, Convex, CoordModeOrigin);
			break;
		}

		switch(planes[1].dirForCit){
			case 0:
				XDrawRectangle(dspl, hwnd, gc, planes[1].x - 8, planes[1].y - 8, 16, 32);
				angle = 55.0 + 122.5;	
				rotate(rpoints3, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints3, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints4, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints4, npoints1, Convex, CoordModeOrigin);
			break;
			
			case 1:
				XDrawRectangle(dspl, hwnd, gc, planes[1].x - 16, planes[1].y -  4, 32, 16);
				angle = 55.0 + 124.1;	
				rotate(rpoints3, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints3, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints4, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints4, npoints1, Convex, CoordModeOrigin);
			break;		

			case 2:
				XDrawRectangle(dspl, hwnd, gc, planes[1].x - 8, planes[1].y - 8, 16, 32);
				angle = 55.0 + 125.611;	
				rotate(rpoints3, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints3, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints4, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints4, npoints1, Convex, CoordModeOrigin);
			break;
			
			case 3:
				XDrawRectangle(dspl, hwnd, gc, planes[1].x - 16, planes[1].y -  4, 32, 16);
				angle = 0.0;	
				rotate(rpoints3, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints3, npoints1, Convex, CoordModeOrigin);
				rotate(rpoints4, npoints1, planes[1].x, planes[1].y, angle);
				XFillPolygon(dspl, hwnd, gc, rpoints4, npoints1, Convex, CoordModeOrigin);
			break;
		}

		//обработка нажатий на клавиши, если нажата q ты выход из программы
		if(event.type == KeyPress && XLookupString(&event.xkey, text, 255, &key, 0) == 1){
			if(text[0] == 'q') { 
				XCloseDisplay ( dspl );
				return;
			}
		}
		XFlush(dspl);
		usleep(sleepTime);
	}
}

void main(){

	int M = 50;
	printf("Enter the count of passes: ");
	scanf("%d", &M);
	pascount = M;
	printf("\033[2J");
	struct city c[4];
	c[0].x = sizeH + 150; c[0].y = sizeV + 150; c[0].stopX = c[0].x + 20; c[0].stopY = c[0].y + 20;
  c[1].x = sizeH + 896 + 150; c[1].y = sizeV + 150; c[1].stopX = c[1].x - 20; c[1].stopY = c[1].y + 20;
  c[2].x = sizeH + 896 + 150; c[2].y = sizeV + 384 + 150; c[2].stopX = c[2].x - 20; c[2].stopY = c[2].y - 20;
  c[3].x = sizeH + 150; c[3].y = sizeV + 384 + 150; c[3].stopX = c[3].x + 20; c[3].stopY = c[3].y - 20;

  for(int k = 0; k < 4; k++)
    cities[k] = c[k];

	cities[0].countOfPasses = pascount;
	cities[3].countOfPasses = 0;

	struct plane planeA, planeB;
	planeA.x = sizeH + 150;
	planeA.y = sizeV + 150;
	planeA.tx = sizeH + 150 + 896;
	planeA.ty = sizeV + 150;
	planeA.speed = 4;
	planeA.dir = 0;
	planeA.passengers = 0;
	planeA.dirForCit = 0;
	planeA.waitTime = 1000000;
	planeA.waitFlag = 0;
	int rc;
	planes[0] = planeA;
	pthread_t plAthread;
	pthread_create(&plAthread, NULL, (void*)calcPosOfPlane, (void*)0);

	planeB.x = sizeH + 150;
	planeB.y = sizeV + 150;
	planeB.tx = sizeH + 150;
	planeB.ty = sizeV + 150 + 384;
	planeB.dir = 1;
	planeB.speed = 2;
	planeB.passengers = 0;
	planeB.dirForCit = 0;
	planeB.waitTime = 2000000;
	planeB.waitFlag = 0;
	planes[1] = planeB;
	pthread_t plBthread;
	pthread_create(&plBthread, NULL, (void*)calcPosOfPlane, (void*)1);

	for(int i = 0; i < pascount; i++)
	{
		struct pas pass;
		pass.x=0;
		pass.y =0;
		pass.tx = 0;
		pass.ty = 0;
		pass.city = 0;
		pass.isfly = 0;
		pass.sleep = 0;
		pass.maxSleepTime = 8000000;
		pass.sleepTime = 0;
		pass.isCity = 0;
		pass.isStop = 1;
		pass.wait = 0;

		passes[i] = pass;
		pthread_t p1Thread;
		pthread_create(&p1Thread, NULL, (void*)PassesLogic, (void*)i);
	}

	dspl = XOpenDisplay(NULL);
	gc = XDefaultGC(dspl,0);
	if(dspl == 0) {printf("Error XOpenDisplay\n"); exit(1);}
	screen = XDefaultScreen(dspl);
	hwnd = XCreateSimpleWindow(dspl, RootWindow(dspl, screen), 100,50,1360,800,3, BlackPixel(dspl,screen), WhitePixel(dspl,screen));
	if(hwnd == 0) {printf("Error XCreateSimpleWindow\n"); exit(1);}
	XSelectInput(dspl, hwnd, ExposureMask | KeyPressMask );
	//XColor xcolour;

	//xcolour.red = 32000; xcolour.green = 65000; xcolour.blue = 32000;
	//xcolour.flags = DoRed | DoGreen | DoBlue;
	//XAllocColor(d, cmap, &xcolour);
	//XSetForeground(dspl, gc, xcolour.pixel);
	XMapWindow(dspl, hwnd);
	//KeySym ks;

	redraw(50000);
}
