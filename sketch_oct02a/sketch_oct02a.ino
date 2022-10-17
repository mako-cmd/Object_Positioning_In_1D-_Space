//distance between mics in mm
const float Speed_of_Sound = 0.33; //0.33 mm/us or 330 m/s or 
const float Mic_Distance = 600; //600 mm

//Varibles for calculation
float difference;
float d1;
float d2;

//Flags to ID if ISR for respective mic has run
bool mic_1_triggered = 0;
bool mic_2_triggered = 0;

//enum for first_to_trigger variable. IDs which mic was triggered first
typedef enum mics {mic_1 = 2, mic_2 = 1, False = 0};
mics first_to_trigger = False;

//ISR for mic input triggered on falling edge
void mic_1_ISR();
void mic_2_ISR();

//Method to calculate distances
int Get_Distance();

//Obj for timer peripheral
hw_timer_t * timer = NULL;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //setup timer: Timer is used to time between mic x being triggered and mic y being triggered
  timer = timerBegin(1, 1, true);
  timerStop(timer);
  timerWrite(timer,0);
  
  //Blue LED used to debug if ISRs are working
  pinMode(2, OUTPUT);
  
  //Setup mic input pins and ISRs
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  attachInterrupt(4,mic_1_ISR,FALLING);
  attachInterrupt(5,mic_2_ISR,FALLING);
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:

        
        //Wait for any mic to be triggered
        while(!(mic_1_triggered || mic_2_triggered))
        {
          Serial.print("\nWaiting for sound...\n");
        }
        //wait for 2nd mic to be triggered
        while(!(mic_1_triggered && mic_2_triggered))
        {
          Serial.print("\nWaiting for conclusive result...\n");
        }
        //Double check if both mics have been triggered
        //If so proceed to calculation
        if (mic_1_triggered && mic_2_triggered)
        {
          //Calculate distances
          Get_Distance();
          
          //If mic 1 was triggered first: present output like this
          if(first_to_trigger == mic_1)
          {
            Serial.print("Total time: ");
            Serial.print(timerReadMicros(timer));
            Serial.print("\nDistance to mic 1: ");
            Serial.print(d1);
            Serial.print("\nDistance to mic 2: ");
            Serial.print(d2);
          }
          //If mic 2 was triggered first: present output like this
          else if (first_to_trigger == mic_2)
          {
            Serial.print("Total time: ");
            Serial.print(timerReadMicros(timer));
            Serial.print("\nDistance to mic 1: ");
            Serial.print(d2);
            Serial.print("\nDistance to mic 2: ");
            Serial.print(d1);
          }
          
          //reset flags and timer
          timerWrite(timer,0);
          mic_1_triggered = 0;
          mic_2_triggered = 0;
          first_to_trigger = False;
        }
      //Good practice: Loop forever / Wait for reset
      while(1)
      {   
      }
}

void mic_1_ISR()
{
  //Turn interrupts off: Good practice
  noInterrupts();
  //Flag blue LED showing interrupt is working
  digitalWrite(2, HIGH); 
  //Set flag to show mic 1 has been triggered already
  mic_1_triggered = 1;
  //If mic 2 has not been triggered: Checking the flag
  if(!mic_2_triggered)
  {
    //Let it be know mic 1 was triggered first
    first_to_trigger = mic_1;
    //Start the timer
    timerStart(timer);
  }
  //If mic 2 has been triggered already: stop the timer
  else
  {
    timerStop(timer);
  }
  //Turn on interrupts: Good practice
  interrupts();
}

void mic_2_ISR()
{
  //Turn interrupts off: Good practice
  noInterrupts();
  //Flag blue LED showing interrupt is working
  digitalWrite(2, HIGH); 
  //Set flag to show mic 2 has been triggered already
  mic_2_triggered = 1;
  //If mic 1 has not been triggered: Checking the flag
  if(!mic_1_triggered)
  {
    //Let it be know mic 2 was triggered first
    first_to_trigger = mic_2;
    //Start the timer
    timerStart(timer);
  }
  //If mic 1 has been triggered already: stop the timer
  else 
  {
    timerStop(timer);
  }
  interrupts();
}

int Get_Distance()
{
    //perform calc
    //read timer counter value in microseconds: For some reason you have to do it twice for it to work properly
    (timerReadMicros(timer));
    
    //Get distance Df as in the diagram
    difference = Speed_of_Sound * timerReadMicros(timer);
    
    //Get D1 as in the diagram
    d1 = (Mic_Distance - difference);
    d1 = d1/2;
    
    //Get df + d1 as in the diagram
    d2 = Mic_Distance - d1;
}
