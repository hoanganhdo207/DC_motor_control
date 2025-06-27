#include <Wire.h> 

#define ADDRESS 0x27 
//using atmega32
#define ENCODER_A 10
#define ENCODER_B 11
#define BUTTON_1  19
#define BUTTON_2  20
#define MOSFET  13 
#define Kp 15.0
#define Ki 30.0
#define Kd 0.005
volatile long count = 0 ; 
unsigned long time = 0 , pre_time = 0 , dt = 0 , pre_time_lcd = 0; 
bool A , B ; 
uint8_t state = 1, state_pre = 1 ; 

float current_RPM = 0.0 , pre_RPM = 0.0, filtered_RPM = 0.0;  
float setpoint = 100 ; //start with 100 RPM 
float pre_Theta = 0.0 ; 
double T = 0.01 ; //10ms 
double alpha , beta , gamma ; 
double Output , pre_Output , filtered_Output ; 
float error , error_1 , error_2 ; 
float velocity ; 
char str[16] ; 

void Achange() ;
void Bchange() ; 
void LCD_cmd(uint8_t cmd) ;
void LCD_data(uint8_t data_s) ; 
void LCD_init() ; 
void LCD_clear() ; 
void LCD_setCursor(char x , char y) ; 
void LCD_print(char* str) ; 
float Read_Velocity() ; 
void Screen() ; 

void setup() {
  // put your setup code here, to run once:
  Wire.begin() ;  
  Serial.begin(9600) ; //using Arduino Board 
  LCD_init() ; 
  pinMode(ENCODER_A , INPUT_PULLUP) ; 
  pinMode(ENCODER_B , INPUT_PULLUP) ; 
  pinMode(BUTTON_1 , INPUT) ; 
  pinMode(BUTTON_2 , INPUT) ;  
  pinMode(MOSFET , OUTPUT) ; 
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), Achange, CHANGE) ;
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), Bchange, CHANGE) ;
  delay(2) ; 
  LCD_setCursor(0, 0) ; 
  LCD_print("SYSTEM START...") ;
  delay(2000) ;  
  LCD_clear() ; 
  pre_time = millis() ; 
}

void loop() {
  // put your main code here, to run repeatedly:
  //do not use delay because it's cause to ISR
  time = millis() ; 
  if(digitalRead(BUTTON_1))
  {
    setpoint += 20 ;
    delay(200) ; 
  } 
    else if(digitalRead(BUTTON_2)) 
    {
      setpoint -= 20 ;
      delay(200) ; 
    } 

  if(setpoint >= 100) setpoint = 100 ; 
  else if(setpoint <= 20) setpoint = 20 ; 

  if(time - pre_time > 10) //Ts = 10ms 
  {
    PID() ; 
    pre_time = time ; 
  } 
  if(time - pre_time_lcd > 300) 
    Screen() ; 

}

void Achange() 
{
  A = digitalRead(ENCODER_A) ; 
  B = digitalRead(ENCODER_B) ; 
  if(A == 1 && B == 1) state = 1 ; 
  else if(A == 1 && B == 0) state = 2 ; 
  else if(A == 0 && B == 0) state = 3 ; 
  else if(A == 0 && B == 1) state = 4 ; 
  switch (state) 
  {
    case 1 : 
    {
      if(state_pre == 2) count-- ; 
      if(state_pre == 4) count++ ; 
      break ; 
    }
    case 2 : 
    {
      if(state_pre == 1) count++ ; 
      if(state_pre == 3) count-- ; 
      break ; 
    }
    case 3 : 
    {
      if(state_pre == 2) count++ ; 
      if(state_pre == 4) count-- ; 
      break ; 
    }
    case 4: 
    {
      if(state_pre == 1) count-- ; 
      if(state_pre == 3) count++ ; 
      break ; 
    }
  }
  state_pre = state ; 
}

void Bchange() 
{
  A = digitalRead(ENCODER_A) ; 
  B = digitalRead(ENCODER_B) ; 
  if(A == 1 && B == 1) state = 1 ; 
  else if(A == 1 && B == 0) state = 2 ; 
  else if(A == 0 && B == 0) state = 3 ; 
  else if(A == 0 && B == 1) state = 4 ; 
  switch (state) 
  {
    case 1 : 
    {
      if(state_pre == 2) count-- ; 
      if(state_pre == 4) count++ ; 
      break ; 
    }
    case 2 : 
    {
      if(state_pre == 1) count++ ; 
      if(state_pre == 3) count-- ; 
      break ; 
    }
    case 3 : 
    {
      if(state_pre == 2) count++ ; 
      if(state_pre == 4) count-- ; 
      break ; 
    }
    case 4: 
    {
      if(state_pre == 1) count-- ; 
      if(state_pre == 3) count++ ; 
      break ; 
    }
  }
  state_pre = state ; 
}

void LCD_cmd(uint8_t cmd) 
{
  uint8_t data_high , data_low ; 
  uint8_t data[4] ; 
  data_high = cmd & 0xf0 ; //0bxxxx0000
  data_low = (cmd << 4) & 0xf0 ; 
  data[0] = data_high | 0x0C ; //pulse EN = 1 , RS = 0 
  data[1] = data_high | 0x08 ; //EN = 0 , RS = 0 
  data[2] = data_low | 0x0C ; 
  data[3] = data_low | 0x08 ; 
  Wire.beginTransmission(ADDRESS) ; 
  for(int i = 0 ; i<4 ; i++) 
  {
    Wire.write(data[i]) ; 
    delayMicroseconds(1) ; 
  } 
  Wire.endTransmission() ; 
}

void LCD_data(uint8_t data) 
{
  uint8_t data_high , data_low ; 
  uint8_t buffe[4] ; 
  data_high = data & 0xf0 ; 
  data_low = (data << 4) & 0xf0 ; 
  buffe[0] = data_high | 0x0D ; //pulse EN = 1 , RS = 1  
  buffe[1] = data_high | 0x09 ; //EN = 0 , RS = 1
  buffe[2] = data_low | 0x0D ; 
  buffe[3] = data_low | 0x09 ; 
  Wire.beginTransmission(ADDRESS) ; 
  for(int i = 0 ; i<4 ; i++) 
  {
    Wire.write(buffe[i]) ; 
    delayMicroseconds(2) ;   
  }
  Wire.endTransmission() ;  
  delay(1);
}

void LCD_init() 
{
  delay(50) ; 
  LCD_cmd(0x08) ; 
  delay(1000) ; 
  LCD_cmd(0x30) ; //init 8 bit mode
  delayMicroseconds(4500) ; 
  LCD_cmd(0x30) ; 
  delayMicroseconds(4500) ; 
  LCD_cmd(0x30) ; 
  delayMicroseconds(150) ; 
  LCD_cmd(0x20) ; // change to 4 bit mode 
  delay(1) ;  
  LCD_cmd(0x28) ; //function set 
  delay(1) ; 
  LCD_cmd(0x0C) ; //display 
  delay(10) ; 
  LCD_cmd(0x01) ; //clear lcd
  delay(30) ; 
  LCD_cmd(0x06) ; //entry mode
  delay(30) ;
}

void LCD_clear() 
{
  LCD_cmd(0x01) ; 
  delay(30) ; 
}

void LCD_setCursor(char x , char y) 
{
  if(x == 0) 
  {
    LCD_cmd(0x80 + y) ; 
  }
  else if(x == 1) 
  {
    LCD_cmd(0xC0 + y) ; 
  }
}

void LCD_print(char* str) 
{
  for(int i = 0 ; i<strlen(str) ; i++) 
    LCD_data((uint8_t)str[i]) ; 
}

void Screen() 
{
  pre_time_lcd = time ; 
  LCD_setCursor(0,0) ; 
  sprintf(str , "SETPOINT : %d" , (int)(setpoint)) ; 
  LCD_print(str) ; 
  if(setpoint <= 99)
  {
    LCD_setCursor(0,13) ; 
    LCD_print(" ") ;  
  }
  if(velocity <= 99) 
  {
    LCD_setCursor(1,13) ; 
    LCD_print(" ") ;  
  }
  if(abs(error) <= 1)
  {
    int target_done = (int)setpoint ; 
    sprintf(str , "VELOCITY : %d" , target_done) ;
  }
  else
    sprintf(str , "VELOCITY : %d" , (int)(velocity)) ; 
  LCD_setCursor(1, 0) ; 
  LCD_print(str) ; 
}
float Read_Velocity() 
{
  float Theta = (float)count/(990.0*4.0) ; //pulse encoder per 1 circle
  dt = time - pre_time ; //time between 2 times read encoder(2 loop) 
  current_RPM = (Theta - pre_Theta) * 1000  * 60 / dt ; //Revolutions per minute 
  filtered_RPM = current_RPM*0.1 + pre_RPM*0.9 ; //Low digital filter 
  if (isnan(filtered_RPM)) { //nan
    filtered_RPM = 0.0;
  }
  pre_RPM = filtered_RPM ; 
  pre_Theta = Theta ; 
  return filtered_RPM ; 
}

void PID() 
{
  velocity = Read_Velocity() ; 
  error = setpoint - velocity ; 
  alpha = Kp * T * 2 + Ki * T * T + 2 * Kd ;
  beta = T * T * Ki - 4*Kd - 2 * T * Kp ; 
  gamma = 2 * Kd ; 
  Output = (alpha * error + beta * error_1 + gamma * error_2 + 2*T*pre_Output) / (2*T) ;
  filtered_Output = 0.1*Output + 0.9*pre_Output ;   
  error_2 = error_1 ; 
  error_1 = error ; 
  pre_Output = filtered_Output ; 
  if(Output >= 255) Output = 255 ; 
  else if(Output <= 0) Output = 0 ;
  analogWrite(MOSFET , Output) ; 
}








