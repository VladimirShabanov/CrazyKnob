
#include <AccelStepper.h>

#define PIN_TRIG 10
#define PIN_ECHO 11

#define MAX_DISTANCE 200 // Константа для определения максимального расстояния, которое мы будем считать корректным.

AccelStepper StepperR(1,7,6); // 1 - режим "external driver" (A4988) 
AccelStepper StepperL(1,9,8); // 1 - режим "external driver" (A4988) 

unsigned int distance=10;

void setup() 
{
  StepperR.setMaxSpeed(200); //устанавливаем максимальную скорость вращения ротора двигателя (шагов/секунду)
  StepperR.setAcceleration(50); //устанавливаем ускорение (шагов/секунду^2)
  StepperR.setSpeed(150);

  StepperL.setMaxSpeed(200); //устанавливаем максимальную скорость вращения ротора двигателя (шагов/секунду)
  StepperL.setAcceleration(50); //устанавливаем ускорение (шагов/секунду^2)
  StepperL.setSpeed(150);

  Serial.begin(9600);

  //Определяем вводы и выводы
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT); 
}

void loop() 
{
  if(StepperR.distanceToGo()==0 && StepperL.distanceToGo()==0)
  { //проверка, отработал ли двигатель предыдущее движение
        
      long distance=getRealDistance();
      Serial.print("Расстояние до объекта: ");
      Serial.print(distance);
      Serial.println(" см.");
  
      if (distance>10)
      {
          Serial.println("Идем 30 шагов вперед, впереди нет припятствий ");
          StepperR.move(30); 
          StepperL.move(30);            
      }
      else
      {
          Serial.println("Поворот вправо, впереди припятствие ");
          StepperR.move(-100); 
          StepperL.move(100);            
      }           
  }
 
  StepperR.run(); //запуск шагового двигателя. Эта строка повторяется вновь и вновь для непрерывного вращения двигателя
  StepperL.run(); //запуск шагового двигателя. Эта строка повторяется вновь и вновь для непрерывного вращения двигателя
}

//измеряем расстояние перед датчиком
long getDistance()
{
  long duration, length_cm;
  
  // Генерируем короткий импульс длительностью 5 микросекунд.
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);

  // Выставив высокий уровень сигнала, ждем 10 микросекунд. В этот момент датчик поcылает сигналы с частотой 40 КГц.
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  //  Время задержки акустического сигнала на эхолокаторе.
  duration = pulseIn(PIN_ECHO, HIGH);

  // Преобразуем время в расстояние
  length_cm = (duration / 2) / 29.1;

  return length_cm;
}

//иногда расстояние берется некорректно. Чтобы бороться с помехами в случае если расстояние больше константы
//MAX_DISTANCE то пробуем взять еще 1 или 2 раза.
long getRealDistance()
{
  long dist = getDistance();
  if (dist>MAX_DISTANCE)
  {
    long dist1 = getDistance();
    if (dist1>MAX_DISTANCE)
    { 
        long dist2=getDistance();
        if (dist2>MAX_DISTANCE)
        {
          dist=MAX_DISTANCE;
        }
        else
        {
          dist=dist2;
        }
    }
    else 
    {
      dist=dist1;
    }    
  }
  return dist;
}
