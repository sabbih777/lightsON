#include <SD.h> //modified original library 
#include <SPI.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <TouchScreen.h> //touch library

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//param calibration from kbv
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92 
#define TS_MAXY 952


#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define SENSABLEPRESSURE 250

LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
#define PIXEL_NUMBER  (my_lcd.Get_Display_Width()/4)

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


uint32_t bmp_offset = 0;
uint16_t s_width = 99;// my_lcd.Get_Display_Width();  
uint16_t s_heigh = 162; //my_lcd.Get_Display_Height();
//int16_t PIXEL_NUMBER;

//char file_name[FILE_NUMBER][FILE_NAME_SIZE_MAX];
char file_name[]="logo.bmp";
File bmp_file;


int DOOR1BTN =51;
int DOOR2BTN =53;

int RELAY1 = 37;
int RELAY2 = 39;
int RELAY3 = 41;
int RELAY4 = 43;


bool allOnOffPressed = false;
bool proyectionPressed= false;
bool door1BtnPressed= false;
bool door2BtnPressed= false;

bool proyecting = false;

bool sleeping= false;
bool touchedToAwake = false;

long timeToSleep = 7000;
unsigned long awakeTime = 0;

void setup() 
{
  pinMode(DOOR1BTN, INPUT);
  pinMode(DOOR2BTN, INPUT);


  digitalWrite(DOOR1BTN,LOW);
  digitalWrite(DOOR2BTN,LOW);


  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
  pinMode(RELAY3,OUTPUT);
  pinMode(RELAY4,OUTPUT);


  
  
   Serial.begin(9600);
   my_lcd.Init_LCD();
   //Serial.println(my_lcd.Read_ID(), HEX);
   my_lcd.Set_Rotation(3);
   my_lcd.Fill_Screen(BLACK);

   drawMainInterface();
   drawButtons();
   
 
   pinMode(13, OUTPUT);
   
   
 /* Start of SD code   
  //Init SD_Card
  // pinMode(10, OUTPUT);
   
   
    if (!SD.begin(10)) 
    {
      my_lcd.Set_Text_Back_colour(BLUE);
      my_lcd.Set_Text_colour(WHITE);    
      my_lcd.Set_Text_Size(1);
      my_lcd.Print_String("SD Card Init fail!",0,0);
    }
 
    
//    bmp_file = SD.open(file_name);
//    //Serial.println(file_name);
//    if(!bmp_file)
//       {
//            my_lcd.Set_Text_Back_colour(BLUE);
//            my_lcd.Set_Text_colour(WHITE);    
//            my_lcd.Set_Text_Size(1);
//            my_lcd.Print_String("didnt find BMPimage!",0,10);
//            while(1);
//        }
//        if(!analysis_bpm_header(bmp_file))
//        {  
//            my_lcd.Set_Text_Back_colour(BLUE);
//            my_lcd.Set_Text_colour(WHITE);    
//            my_lcd.Set_Text_Size(1);
//            my_lcd.Print_String("bad bmp picture!",0,0);
//            return;
//        }
//       //  draw_bmp_picture(bmp_file);
//         bmp_file.close();

*/ //End of SD code   
}

void loop() 
{


  
do{

  
  
  if ((millis()- awakeTime) > timeToSleep && !sleeping )
  {
    sleeping= true;
    my_lcd.Fill_Screen(BLACK);
    
    Serial.println("Going to Sleep");
  }
  
  int touchedArea = readTouch();
 
//  Serial.println(door1BtnPressed);
//  Serial.println(door2BtnPressed);
  
  
  if(digitalRead(DOOR1BTN)==HIGH)
  {
    door1BtnPressed=true;
    door2BtnPressed=false;
    allOnOffPressed = false;
    proyectionPressed =false;
  }

  if(digitalRead(DOOR2BTN)==HIGH)
  {
    door2BtnPressed=true;
    door1BtnPressed=false;
    allOnOffPressed = false;
    proyectionPressed =false;
  }
if (!sleeping ){
  if(touchedArea==1)
  {
    allOnOffPressed = true;
    proyectionPressed=false;
    door1BtnPressed=false;
    door2BtnPressed=false;
    drawButtons();
    Serial.println("All lights");
  }
    
  if(touchedArea==2 )
  {
    allOnOffPressed = false;
    proyectionPressed =true;
    door1BtnPressed=false;
    door2BtnPressed=false;
    proyecting =true;
    drawButtons();
   }

}

  if(sleeping && touchedToAwake == true)
  {
    awakeTime = millis();
    sleeping= false;
    touchedToAwake= false;
    //Serial.println("Touched");
    drawMainInterface();
    drawButtons();     
  }
  }while (door1BtnPressed == false && door2BtnPressed == false && allOnOffPressed == false && proyectionPressed == false );
   
 
  if((!sleeping && allOnOffPressed ) || door1BtnPressed || door2BtnPressed){
    if (proyecting)
    {
      //Serial.println("Encendiendo todo luego de proyecciòn");
      digitalWrite(RELAY1, HIGH);
      digitalWrite(RELAY2, HIGH);
      digitalWrite(RELAY3, HIGH);
      digitalWrite(RELAY4, HIGH);
      proyecting=false;  
    }
    else 
    {
      digitalWrite(RELAY1, !digitalRead(RELAY1));
      digitalWrite(RELAY2, !digitalRead(RELAY2));
      digitalWrite(RELAY3, !digitalRead(RELAY3));
      digitalWrite(RELAY4, !digitalRead(RELAY4));
      //Serial.println("Encendiendo o apagando todo");
    }
  }

  if (!sleeping && proyectionPressed )
  {
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
    digitalWrite(RELAY4, LOW);
    //Serial.println("Proyectando");
  }

  //Debouncing
//  do{
//    if(digitalRead(DOOR1BTN)==HIGH){
//     door1BtnPressed=true;
//     door2BtnPressed=false;
//     allOnOffPressed = false;
//     proyectionPressed =false;
//    }
//
//  if(digitalRead(DOOR2BTN)==HIGH)
//    {
//       door2BtnPressed=true;
//       door1BtnPressed=false;
//       allOnOffPressed = false;
//       proyectionPressed =false;
//    }
//   Serial.println("Debouncing");
//}while (digitalRead(DOOR1BTN) == HIGH || digitalRead(DOOR2BTN) == HIGH );



}

int readTouch ()
{
  //Touch Screen
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z >= SENSABLEPRESSURE ) //pressure
  {
    Serial.println("Touched");
    touchedToAwake =true;
    if ( p.y >=240 && p.y<= 800 )
     {
      if ( p.x >= 380 && p.x<= 540 ) // Encender / Apagar todas las lucs
      {
      //  //////Serial.println("X:");  
        //Serial.println(p.x);  
        //Serial.println("Y:");  
        //Serial.println(p.y);  
        //Serial.println("Pressed: Encender / Apagar todas las luces");  
        return 1;
       
      }
      else if ( p.x >= 640 && p.x<= 795 ) //Proyecciòn
      {
        //Serial.println("X:");  
        //Serial.println(p.x);  
        //Serial.println("Y:");  
        //Serial.println(p.y); 
        //Serial.println("Pressed: Proyecciòn");   
        return 2;
      }
      else
      {
        //Serial.println("X:");  
        //Serial.println(p.x);  
        //Serial.println("Y:");  
        //Serial.println(p.y); 
        //Serial.println("Touched Outside Buttons");  
        return 0;
      }
     }
     else {
      //Serial.println("X:");  
      //Serial.println(p.x);  
      //Serial.println("Y:");  
      //Serial.println(p.y); 
      //Serial.println("Touched Outside Buttons");  
      return 0;
     }
  }
  else {
      touchedToAwake =false;
//      //Serial.println("X:");  
//      //Serial.println(p.x);  
//      //Serial.println("Y:");  
//      //Serial.println(p.y); 
//      //Serial.println("Screen not touched");  
      return -1;
     } 
  
}


void drawMainInterface()
{
   drawHeader(); 
   drawFooter();
  
}

void drawHeader()
{
   my_lcd.Set_Text_Back_colour(BLACK);
   my_lcd.Set_Text_colour(WHITE);
   my_lcd.Set_Text_Size(4);
   my_lcd.Print_String("CONTROL DE LUCES",40,20);
}

void drawFooter()
{
   my_lcd.Set_Text_colour(WHITE);
   my_lcd.Set_Text_Back_colour(BLACK);
   my_lcd.Set_Text_Size(2);
   my_lcd.Print_String("IDE Business School",250,285);
}

void drawButtons (){
  
  drawAllOnOffButton();
  drawProyectionButton();
 

}

void drawAllOnOffButton () {
  uint16_t bgColor;
  uint16_t textColor;

  if (allOnOffPressed == true)
  {
    bgColor = GREEN;
    textColor = BLACK;
  }
  else
  {
    bgColor = BLUE;
    textColor = WHITE;
  }
   my_lcd.Set_Text_colour(textColor);
   my_lcd.Set_Text_Back_colour(bgColor);
   my_lcd.Set_Text_Size(3);
   my_lcd.Fill_Rect(80,100,312,65,bgColor);
   my_lcd.Print_String("ENCENDER / APAGAR",85,110);
   my_lcd.Print_String("TODAS LAS LUCES",95,140);
}

void drawProyectionButton(){
   uint16_t bgColor;
   uint16_t textColor;
   if (proyectionPressed == true)
  {
    bgColor = GREEN;
    textColor = BLACK;
  }
  else
  {
    bgColor = BLUE;
    textColor = WHITE;
  }
   my_lcd.Set_Text_colour(textColor);
   my_lcd.Set_Text_Back_colour(bgColor);
   my_lcd.Set_Text_Size(3);
   my_lcd.Fill_Rect(80,200,312,65,bgColor);
   my_lcd.Print_String("PROYECCION",140,225);
 // my_lcd.Print_String("LUCES PROYECTOR",95,240);
}


uint16_t read_16(File fp)
{
    uint8_t low;
    uint16_t high;
    low = fp.read();
    high = fp.read();
    return (high<<8)|low;
}

uint32_t read_32(File fp)
{
    uint16_t low;
    uint32_t high;
    low = read_16(fp);
    high = read_16(fp);
    return (high<<8)|low;   
 }
 
bool analysis_bpm_header(File fp)
{
    if(read_16(fp) != 0x4D42)
    {
      return false;  
    }
    //get bpm size
    read_32(fp);
    //get creator information
    read_32(fp);
    //get offset information
    bmp_offset = read_32(fp);
    //get DIB infomation
    read_32(fp);
    //get width and heigh information
    uint32_t bpm_width = read_32(fp);
    uint32_t bpm_heigh = read_32(fp);
    if((bpm_width != s_width) || (bpm_heigh != s_heigh))
    {
      return false; 
    }
    if(read_16(fp) != 1)
    {
        return false;
    }
    read_16(fp);
    if(read_32(fp) != 0)
    {
      return false; 
     }
     return true;
}



void draw_bmp_picture(File fp)
{
  uint16_t i,j,k,l,m=0;
  uint8_t bpm_data[PIXEL_NUMBER*3] = {0};
  uint16_t bpm_color[PIXEL_NUMBER];
  fp.seek(bmp_offset);
  for(i = 0;i < s_heigh;i++)
  {
    for(j = 0;j<s_width/PIXEL_NUMBER;j++)
    {
      m = 0;
      fp.read(bpm_data,PIXEL_NUMBER*3);
      for(k = 0;k<PIXEL_NUMBER;k++)
      {
        bpm_color[k]= my_lcd.Color_To_565(bpm_data[m+2], bpm_data[m+1], bpm_data[m+0]); //change to 565
        m +=3;
      }
      for(l = 0;l<PIXEL_NUMBER;l++)
      {
        my_lcd.Set_Draw_color(bpm_color[l]);
        my_lcd.Draw_Pixel(j*PIXEL_NUMBER+l,i);
      }    
     }
   }    
}
