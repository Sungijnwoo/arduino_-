#include <LiquidCrystal.h>
#include <Keypad.h>
#include <String.h>
#define XAXIS 0   //LED 매투릭스의 x 축
#define YAXIS 1   //LED 매투릭스의 y 축

#define UP_Y 0    //LED 화면 올리기
#define DOWN_Y 1  //LED 화면 내리기
#define UP_X 2    //LED 화면 옆으로 돌리기
#define DOWN_X 3  //LED 화면 옆으로 돌리기

#define RAIN 0          //비내리는 모션
#define PLANE 1         //선이 왔다갔다 하는 모션
#define SEND_DOTS 2   //점이 이동하는 모션
#define BOX 3           //박스가 커졌다 작아졌다 하는 모션
#define CUBE_JUMP 4     //박스가 움직이며 커졌다 작아졌다 하는 모션
#define GLOW 5          //면을 점으로 채우는 모션
#define WAVE 6          //파도 모션
#define WINDMILL 7      //풍차 모션
#define TOTAL_EFFECTS 8 //총 모션 개수

#define RAIN_TIME 160   //모션 시간 
#define PLANE_TIME 130
#define SEND_TIME 70
#define BOX_TIME 180
#define JUMP_TIME 70
#define GLOW_TIME 40
#define WAVE_TIME 120
#define WIND_TIME 90

LiquidCrystal lcd(38,10,11,40,39,12); //LCD 핀 설정
unsigned int data_score;    //게임 점수 저장
unsigned int score_count;   //게임 점수 변경을 위한 변수
unsigned char cube[8][8];  //모션 그리기 위한 배열
int currentEffect;    //현재 모션 번호
int timer = 0;        //모션 타이머     
bool loading;         //모션 초기 설정하기
bool reverse;         //축 바꾸기

const byte ROWS = 4;    // 행(rows) 개수
const byte COLS = 4;    // 열(columns) 개수
char keys[ROWS][COLS] = {
  {'1','2','3', '4' },
  {'5','6','7' , '8' },
  { '9' , 'A' , 'B' , 'C' },
  { 'D' , 'E' , 'F' , 'G' }      // 각 버튼값 입력
};                             
unsigned char high[8][8]={
  {1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},
  {1,1,1,0,0,0,0,0},{1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},{0,0,0,0,0,0,0,0}};
unsigned char middle[8][8]={{1,1,1,0,0,1,1,1},{1,1,1,0,0,1,1,1},{0,0,0,0,0,0,0,0},{0,1,1,0,0,1,1,0},
                  {0,0,0,0,0,0,0,0},{1,1,1,0,0,1,1,1},{1,1,1,0,0,1,1,1},{1,1,1,0,0,1,1,1}};
unsigned char low[8][8]={{0,0,0,0,0,0,0,0},{1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},{1,1,1,0,0,0,0,0},{1,1,1,0,1,1,1,1},
               {1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1},{1,1,1,0,1,1,1,1}};
byte rowPins[ROWS] = {5, 4, 3, 2}; 
byte colPins[COLS] = {6, 7, 8, 9};      // 키페드에 쓰이는 핀 번호 지정

unsigned char h[8]={22,23,24,25,26,27,28,29};
unsigned char y[8]={30,31,32,33,34,35,36,37};        //각 LED 에 열과 행 지정
unsigned char previous_m[8][8];                       // m값의 이전값 지정
unsigned char after_m[8][8];                           // m값의 이후값 지정   
unsigned char user[8];                                 // 사용자의 현위치 값을 지정
int gameover=0;                              // gameover 되면 1로 바뀌는 변수
int c=0;                                      // 좌우로 움직일때 쓰이는 변수
unsigned char x[8][8];                                 // x 그리는 행렬
unsigned char one[8][8];                               // 1 그리는 행렬
unsigned char two[8][8];                               // 2 그리는 행렬
unsigned char three[8][8];                             // 3 그리는 행렬
unsigned char fourfour[16][8][8];                      // 64개 화면을 4개씩 묶어 4*4에서 하나씩 출력하는 행렬
int remember[100];                            // 기억력 게임에 쓰이는 변수
int howmany=0;                               //  기억력 게임에 쓰이는 변수
int game=0;                                    // game 선택에 쓰이는 변수
int gotu_count=0;                                // 랜덤으로 수를 받을때 쓰이는 변수
int remember_count=0;                           // 기억한 횟수                                
int remember_power[100];                        // 기억력에 쓰이는 변수
int real_key;                                   // 키페드 값을 int 형으로 바꿔서 저장하는 변수
int customKey;                                // 키페드 값을 저장하는 변수
int select=0;                                 // 게임 선택에 쓰이는 변수
int previous__real_key;                         // 키페드 이전 값을 저장하는 변수
int i=0;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );    //키페드에 쓰이는 라이브러리 함수
int a=0;
int level[3];     //각 난이도 배열
int select_level;   
int best_score_a;
int best_score_b;
int best_score_c;

void button(){
  char key = keypad.getKey();                   // 키페드 값을 받는 함수
  real_key=16;                                   // 아무것도 입력 안받았을때의 값 지정
  if (key) {
    if(key=='1') real_key=3;
    else if(key=='2') real_key=2;
    else if(key=='3') real_key=1;
    else if(key=='4') real_key=0;
    else if(key=='5') real_key=7;
    else if(key=='6') real_key=6;
    else if(key=='7') real_key=5;
    else if(key=='8') real_key=4;
    else if(key=='9') real_key=11;
    else if(key=='A') real_key=10;
    else if(key=='B') real_key=9;
    else if(key=='C') real_key=8;
    else if(key=='D') real_key=15;
    else if(key=='E') real_key=14;
    else if(key=='F') real_key=13;
    else if(key=='G') real_key=12;                   // 각 키페드 값에 따라 숫자로 변환
   if(game==2){                                                                      
        for(int second=0; second<select_level; second++){
           for(int a=0; a<8; a++){                                    
              digitalWrite(h[a],1);
              for(int b=0; b<8; b++){
               if(fourfour[real_key][a][b]==0) digitalWrite(y[b],0);
               digitalWrite(y[b],1);
              }
              digitalWrite(h[a],0);
          }      
        }
        remember_power[i]=real_key;
        i+=1;
      
   }
   if(game==3){
    if(real_key==6){                                       // 쏘는 버튼 누를시
      for(int shot=0; shot<8; shot++){                    
        if(after_m[7-shot][c]==0){                              //현위치에서 행을 올리면서 벽이 있나 확인후 잇으면 제거
          after_m[7-shot][c]=1;
          previous_m[7-shot][c]=1;
          break;
        }
      }
    }
   }
   if(select==4){
    select=0;
   }
  }

}

void gotu(){                                                // 랜덤으로 위의 행 값을 받음
  for(int k=0; k<8; k++){
    previous_m[0][k]=after_m[0][k];                          //m이전값을 저장
    if(gotu_count%2==0) after_m[0][k]= random(0,2);         
    else after_m[0][k]= 1;                                // m값을 퐁당퐁당 랜덤으로 받음
  }
  for(int j=1; j<8; j++){                                 // 위에서 부터 아래로 값을 순차적으로 내림
    for(int i=0; i<8; i++){
    previous_m[j][i]=after_m[j][i];
    after_m[j][i]=previous_m[j-1][i];
    }
  }
 gotu_count+=1;     
}
void randomrain(){                    //랜덤으로 비내리기
for(int i = 0; i < 8; i++){
    digitalWrite(h[i], 1);
    for(int j = 0; j < 8; j++){
      if(after_m[i][j] == 0) digitalWrite(y[j],0);
      digitalWrite(y[j],1);
    }
    digitalWrite(h[i], 0);
  }
}

void controller(){                                         // 사용자 위치가 변하게 해주는 함수
  button();
  if(real_key==5){                                       // 왼쪽 버튼 누를시 
    c=c-1;                                                // c값 감소
    if(c<0){                                            // 위치가 왼쪽 범위를 넘어갈시
      c=0;
    }
  }
  else if(real_key==7){                              // 오른쪽 버튼 누를시
    c=c+1;                                            // c값 증가
    if(c>7){                                        // 위치가 오른쪽 범위를 넘어갈시
      c=7;                                           
    }
  }
   digitalWrite(h[7],1);
   for(int j=0; j<8; j++){
    if(j==c){
      digitalWrite(y[j],0);
      user[j]=0;
    }
    else user[j]=1; 
    digitalWrite(y[j],1);
  }
  digitalWrite(h[7],0);
}

void endgame(){
 for(int l=0; l<120; l++){                          // 약 1초동안 키기
    for(int i=0; i<8; i++){                            
    digitalWrite(h[i],1);
      for(int k=1; k<8; k++){
       digitalWrite(h[(k+i)%8],0);
    }
      for(int k=0; k<8; k++){
       digitalWrite(y[k],x[i][k]);                      // x모양으로 키기
    }
    delay(2);
    }
 }
 clearcube();
 buildCube();
 for( int a=0; a<8; a++){
  for( int b=0; b<a+1; b++){
    digitalWrite(h[b],1);
    for( int c=0; c<8; c++){
      digitalWrite(y[c],1);
      digitalWrite(y[c],0);
    }
    delay(35);
  }
 }
 initialization();
 for(i=0; i<8; i++){
  digitalWrite(y[i],1);
 }
}
void initialization(){
  for(int j=0;j<8; j++){                //game 끝나고 화면 초기화하기 위해 행렬 초기화
     for(int i=0; i<8; i++){
         after_m[j][i]=1;
         previous_m[j][i]=1;
      }
   }  
}


void setDot(int a,int b){     //모션 그리는 배열의 원하는 곳에 '1'값을 넣는 함수
    cube[a][b] = 1;
}

void clearDot(int a,int b){   //모션 그리는 배열의 원하는 곳에 '0'값을 넣는 함수
    cube[a][b] = 0;
}

bool getDot(int a,int b){     //모션 그리는 배열의 원하는 곳의 값을 확인하는 함수
  if(cube[a][b] == 1){
    return 1;
  }else{
    return 0;
  }
}

void setplane(int axis, int i){   //8x8 배열의 원하는 면을 '1'로 채우는 함수
  for(int j = 0; j < 8; j++){
    if(axis == XAXIS){
      setDot(i, j);
    }else if(axis == YAXIS){
      setDot(j, i);
    }
  }
}

void shift(int dir){      //모션 그리는 배열을 원하는 방향으로 돌리거나 올리거나 하는 함수
  if(dir == UP_Y){
    for(int y = 1; y < 8 ; y ++){
      for(int x = 0; x < 8; x ++){
        cube[x][y-1] = cube[x][y];
      }
    }
    for(int i = 0; i < 8; i++){
      cube[i][7] = 0;
    }   
  }else if(dir == DOWN_Y){
    for(int y = 7; y > 0 ; y --){
      for(int x = 0; x < 8; x ++){
        cube[x][y] = cube[x][y-1];
      }
    }
    for(int i = 0; i < 8; i++){
      cube[i][0] = 0;
    }    
  }else if(dir == UP_X){
    for(int y = 0; y < 8 ; y ++){
      for(int x = 1; x < 8; x ++){
        cube[x-1][y] = cube[x][y];
      }
    }
    for(int i = 0; i < 8; i++){
      cube[7][i] = 0;
    }
  }else if(dir == DOWN_X){
    for(int y = 0; y < 8 ; y ++){
      for(int x = 7; x > 0; x --){
        cube[x][y] = cube[x-1][y];
      }
    }
    for(int i = 0; i < 8; i++){
      cube[0][i] = 0;
    }    
  }
}

void drawCube(int x,int y, int s){    //원하는 사이즈와 사이즈에 관한 변수 둘을 넣어주면 박스에 대한 배열을 만들어주는 함수
  for(int i = 0; i < s; i++){
    setDot(x, y + i);
    setDot(x + i, y);
    setDot(x + s - 1, y + i);
    setDot(x + i, y + s - 1);
  }
}

void lightcube(){         //배열을 전부 '1'로 채우는 함수
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      cube[i][j] = 1;
    }
  }
}

void clearcube(){         //배열을 전부 '0'으로 채우는 함수
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      cube[i][j] = 0;
    }
  }
}

int xVal;
int xVal2;
int xVal3;
int xVal4;

void turn(int a){     
//배열 가운데 4개의 점을 축으로 대각선으로 면을 나누었을때 한 면의 배열을 회전시키는 함수
  if(a < 4){
    xVal4 = a;
    xVal = a;
    for(int b = 0; b < xVal+1; b++){
      cube[a][b] = 1;
      cube[a + 1][b] = 1;
    }
    for(int b = xVal; b < 4; b++){
      cube[a][b] = 1;
      cube[a + 1][b] = 1;
      a++;
    }  
  }else if(a >= 4){
    xVal3 = a;
    xVal = 7 - a;
    for(int b = 0; b < xVal+1; b++){
      cube[a][b] = 1;
      cube[a + 1][b] = 1;
    }
    for(int b = xVal; b < 4; b++){
      a--;
      cube[a][b] = 1;
      cube[a + 1][b] = 1;
    }
  }
  if(xVal4 < 4){
    xVal2 = xVal4;
    for(int b = 0; b < xVal2+1; b++){
      cube[6-xVal4][7-b] = 1;
      cube[7-xVal4][7-b] = 1;
    }
    for(int b = xVal2; b < 4; b++){
      cube[6-xVal4][7-b] = 1;
      cube[7-xVal4][7-b] = 1;
      xVal4++;
    } 
  }else if(xVal3 >= 4){
   xVal2 = 6 - xVal3;
    for(int b = 0; b < xVal2; b++){
      cube[6-xVal3][7-b] = 1;
      cube[7-xVal3][7-b] = 1;
    } 
   for(int b = xVal2; b < 4; b++){
      cube[6-xVal3][7-b] = 1;
      cube[7-xVal3][7-b] = 1;
      xVal3--;
    } 
  }
}

void buildCube(){        
//설정한 배열 cube[8][8]을 토대로 LED를 켜는 함수, x와 y가 바뀐 이유는 하드웨어를 만들기 전 코드를 짜 그 도중에 실수가 생김
  for(int y = 0; y < 8; y++){
    digitalWrite(y+30, 0);
    for(int x = 0; x < 8; x++){
      if(cube[y][x] == 1){
       digitalWrite(x+22,1); 
      }
      digitalWrite(x+22,0);
    }
    digitalWrite(y+30, 1);
  }
} 

void rebuildCube(){     //buildCube()의 축을 바꾼 함수
  for(int y = 0; y < 8; y++){
    digitalWrite(y+30, 0);
    for(int x = 0; x < 8; x++){
      if(cube[x][y] == 1){
       digitalWrite(x+22,1); 
      }
      digitalWrite(x+22,0);
    }
    digitalWrite(y+30, 1);
  }
}

void rain(){      //위의 함수를 이용해 비가 내리게 하는 배열을 만드는 함수
  if(loading){
    clearcube();
    loading = 0;
  }
  timer ++;
  if(timer > RAIN_TIME){
    timer = 0;
    shift(DOWN_Y);
    
    int numDrops = random(0,5);
    for(int i = 0; i <numDrops; i++){
      setDot(random(0,8),0);
    }
  }
}

int planePosition = 0;
int planeDirection = 0;
bool looped = 0;

void plane(){     //위의 함수를 토대로 선을 이동시키는 배열을 만드는 함수
  if(loading){
   clearcube();
   int axis = random(0,2);
   planePosition = random(0,2) * 7;
   setplane(axis, planePosition);
   
   if(axis == XAXIS){
    if(planePosition == 0){
     planeDirection = DOWN_X;
    }else{
     planeDirection = UP_X;
    }
   }else if(axis == YAXIS){
    if (planePosition == 0) {
    planeDirection = DOWN_Y;
    }else{
     planeDirection = UP_Y;
    }
   }  
   timer = 0;
   looped = 0;
   loading = 0;
  }
   
  timer++;
  if (timer > PLANE_TIME) {
   timer = 0;
   shift(planeDirection);
   if (planeDirection % 2 == 0) {
    planePosition--;
    if(planePosition == 0){
     if(looped){
      loading = 1;
     }else{
      planeDirection++;
      looped = 1;
     }
    }
   }else{
    planePosition++;
    if (planePosition == 7) {
     if (looped) {
      loading = 1;
     }else{
      planeDirection--;
      looped = 1;
     }
    }
   }
  }
}

int selX = 0;
int selY = 0;
int sendDirection = 0;
bool sending = 0;

void sendDot(){       //점을 보내는 모션을 그리는 배열을 만드는 함수
  if(loading){
    clearcube();
    for(int x = 0; x < 8; x++){
      setDot(x,random(0,2)*7);
      loading = 0;
    }
  }
  timer ++;
  if(timer > SEND_TIME){
    timer = 0;
    if(!sending){
      selX = random(0, 8);
      if(getDot(selX, 0)){
        selY = 0;
        sendDirection = DOWN_Y;
      }else if(getDot(selX, 7)){
        selY = 7;
        sendDirection = UP_Y;
      }
      sending = 1;
    }else{
      if(sendDirection == DOWN_Y){
        selY ++;
        setDot(selX, selY);
        clearDot(selX, selY-1);
        if(selY == 7){
          sending = 0;
        }
      }else{
          selY --;
          setDot(selX,selY);
          clearDot(selX,selY + 1);
          if(selY == 0){
            sending = 0;
          }
        }
      }
    }
  }

int cubeSize = 0;
bool cubeExpanding = 1;

void box(){               //박스를 그리는 모션을 만드는 배열을 만드는 함수
  if(loading){
    clearcube();
    cubeSize = 2;
    cubeExpanding = 1;
    loading = 0;
  }
  timer++;
  if(timer > BOX_TIME){
    timer = 0;
    if(cubeExpanding){
      cubeSize += 2;
      if(cubeSize == 8){
        cubeExpanding = 0;
      }
    }else{
      cubeSize -= 2;
      if(cubeSize == 2){
        cubeExpanding = 1;
      }
    }
    clearcube();
    drawCube(4-cubeSize/2, 4-cubeSize/2, cubeSize);
  }
}

bool glowing;
int glowcount = 0;

void glow(){        //점으로 면을 채우는 모션을 만드는 배열
  if(loading){
    clearcube();
    glowcount = 0;
    glowing = 1;
    loading = 0;
  }
  timer ++;
  if(timer > GLOW_TIME){
    timer = 0;
    if(glowing){
      if(glowcount < 59){
        do{
          selX = random(0,8);
          selY = random(0,8);
        }while(getDot(selX,selY));
        setDot(selX,selY);
        glowcount++;
      }else if(glowcount < 64){
        lightcube();
        glowcount++;
      }else{
        glowing = 0;
        glowcount = 0;
      }
    }else{
      if(glowcount < 59){
        do{
          selX = random(0,8);
          selY = random(0,8);
        }while(!getDot(selX,selY));
        clearDot(selX,selY);
        glowcount++;
      }else{
        clearcube();
        glowing = 1;
        glowcount = 0;
      }
    }
  }
}

int xPos;
int yPos;

void cubejump(){        //박스가 왔다 갔다 커졌다 작아졌다 하는 모션을 만드는 배열을 만드는 함수
  if(loading){
    clearcube();
    xPos = random(0,2)*7;
    yPos = random(0,2)*7;
    cubeSize = 8;
    cubeExpanding = 0;
    loading = 0;
  }

  timer ++;
  if(timer > JUMP_TIME){
    timer = 0;
    clearcube();
    if(xPos == 0 && yPos == 0){
      drawCube(xPos,yPos,cubeSize);
    }else if(xPos == 7 && yPos == 7){
      drawCube(xPos + 1 - cubeSize,yPos + 1 - cubeSize,cubeSize);
    }else if(xPos == 7 && yPos == 0){
      drawCube(xPos + 1 - cubeSize,yPos,cubeSize);
    }else if(xPos == 0 && yPos == 7){
      drawCube(xPos,yPos + 1 - cubeSize,cubeSize);
    }
    if(cubeExpanding){
      cubeSize ++;
      if(cubeSize == 8){
        cubeExpanding = 0;
        xPos = random(0,2)*7;
        yPos = random(0,2)*7;
      }
    }else{
      cubeSize--;
      if(cubeSize == 1){
        cubeExpanding = 1;
      }
    }
  }
}

int yHet;
int yMax;
bool yDir;

void wave(){      //파도 모션을 만드는 배열을 만드는 함수
  if(loading){
    clearcube();
    int yHet = 0;
    int yMax = random(1,6); 
    yDir = 1; 
    loading = 0;
    timer = 0;
  }
  timer ++;
  if(timer > WAVE_TIME){
    timer = 0;
    shift(DOWN_X);
    if(yHet == 0){
      yDir = 1;
      yMax = random(3,7);
    }else if(yHet == yMax){
      yDir = 0;
    }    
    for(int j = 0; j <= yHet; j++){
      clearDot(0, j);
    }
    for(int i = 7; i > yHet; i --){
      setDot(0, i);
    }
    if(yDir == 1)
      yHet++;
    else
      yHet--;
  }
}

int k;
int kDir;

void windmill(){        //풍차 모션을 그리는 배열을 만드는 함수
  if(loading){
    clearcube();
    int k = 0;
    int kDir = 1;
    loading = 0;
    timer = 0;
  }
  timer++;
  if(timer > WIND_TIME){
    timer = 0;
    if(k == 0){
      reverse = 1;
      kDir = 1;
    }else if(k == 6){
      reverse = 0;
      kDir = 0;
    }
    if(kDir == 1){
      k++;
    }else if(kDir == 0){
      k--;    
    }
    clearcube();
    turn(k);
  }
}

void setup() { 
  lcd.begin(16,2);
  loading = 1;                          
  currentEffect = RAIN;

  randomSeed(analogRead(0));                         
  Serial.begin(9600);
  Serial1.begin(115200);
  for(int i=22;i<38;i++){   
  pinMode(i,OUTPUT);                     
  }
  for(int j=0;j<8; j++){
    for(int i=0; i<8; i++){
      after_m[j][i]=1;
      previous_m[j][i]=1;
    }

    
 }                                     //초기화

 for(int i=0; i<100; i++){
  remember_power[i]=0;
  remember[i]=0;
 }                                       //각 변수(배열) 초기화
  for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
         if (i == j || i+j ==7)
            x[i][j] = 0;
         else
            x[i][j] = 1;
      }
   }
  
                                                //x모양 배열 저장
  for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        if(j==3 || j==4) one[i][j]=0;
        else one[i][j]=1; 
      }
  }
                                                    //1모양 배열 저장
  for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        if(i==0||i==3||i==7) two[i][j]=0;
        else if(i==1||i==2){
          if(j==7) two[i][j]=0;
          else     two[i][j]=1;
        }
        else if(i==4||i==5||i==6){
          if(j==0) two[i][j]=0;
          else     two[i][j]=1;
        }
      }
  }
                                                         //2모양 배열 저장
  for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        if(i==0||i==3||i==7) three[i][j]=0;
        else{
          if(j==7) three[i][j]=0;
          else     three[i][j]=1;
        }        
      }
  }
                                                         //3모양 배열 저장 
  for(int i=0; i<16; i++){
    for(int j=0; j<8; j++){
      for(int k=0; k<8; k++){
            if(j==(i/4)*2||j==(i/4)*2+1){
              if(k==(2*i)%8||k==((2*i)+1)%8) fourfour[i][j][k]=0; 
              else fourfour[i][j][k]=1;    
            }
            else fourfour[i][j][k]=1;                            
        }
      }
    }
                                                           //두번째 게임 모양들 16가지 배열 저장
  }


void loop() {
  if(gameover==1){                                                     //게임오버시 실행
    data_score=(howmany==0?(score_count-16)*100:data_score=howmany-1);
    if(game==1){
      if(best_score_a<data_score) best_score_a=data_score;
    }
    else if(game==2) {
      if(best_score_b<data_score) best_score_b=data_score;
    }
    else {
      if(best_score_c<data_score) best_score_c=data_score;
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("score:");
    lcd.print("  ");
    lcd.print(data_score);
    if(howmany==0?(data_score<2000):(data_score<5)){
      lcd.setCursor(0,1);
      lcd.print("yourIQ=dogIQ");
    }
    else if(howmany==0?(data_score>10000):(data_score>11)){
      lcd.setCursor(0,1);
      lcd.print("your great");
    }
    else {
      lcd.setCursor(0,1);
      lcd.print("um soso");
    }
    endgame();
    game=0;
    gameover=0;
    select=0;                                               
    howmany=0;                                             // 게임, 난이도 선택의 변수들 초기화
    score_count=0;
  }                                                           
  if(game==0){
    if(select==0){
      button();                       
      switch(real_key){
        case 12:                                                  // 게임 1선택
         game=1;
         select=1;
         for(int i=0; i<250; i++){
          for(int a=0; a<8; a++){
            digitalWrite(h[a],1);
            for(int b=1; b<8; b++){
              digitalWrite(h[(a+b)%8],0);
            }
            for(int d=0; d<8; d++){
              digitalWrite(y[d],one[a][d]);
            }
            delay(2);
          }      
         }                                                         // 1모양 1초 출력
         level[0]=600;
         level[1]=400;
         level[2]=200;
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("game:avoid ddong");
         break;
        case 13:                                                  // 게임 2선택
         game=2;
         select=1;
         for(int i=0; i<250; i++){
          for(int a=0; a<8; a++){
            digitalWrite(h[a],1);
            for(int b=1; b<8; b++){
              digitalWrite(h[(a+b)%8],0);
            }
            for(int d=0; d<8; d++){
              digitalWrite(y[d],two[a][d]);
            }
          delay(2);
        }      
         }                                                      // 2모양 1초 출력
         level[0]=1000;
         level[1]=800;
         level[2]=500;
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("game:remember");
         break;
        case 14:                                                  // 게임 3선택
         game=3;
         select=1;
         for(int i=0; i<250; i++){
          for(int a=0; a<8; a++){
            digitalWrite(h[a],1);
            for(int b=1; b<8; b++){
              digitalWrite(h[(a+b)%8],0);
            }
            for(int d=0; d<8; d++){
              digitalWrite(y[d],three[a][d]);
            }
          delay(2);
        }      
         }
         level[0]=2500;
         level[1]=2000;
         level[2]=1500;
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("game:break block");
         break;
        case 15:
         select=2;
         break;
        case  8:
         select=4;
         lcd.clear();
         break;
      }                                                            // 3모양 1초 출력
    }
  }
    if(select==1){                                                  // 게임을 고른후
       button();
      switch(real_key){
        case 12:                                                    //난이도 하 선택
          select_level=level[0];
          for(int i=0; i<250; i++){
        for(int a=0; a<8; a++){
          digitalWrite(h[a],1);
          for(int b=1; b<8; b++){
            digitalWrite(h[(a+b)%8],0);
          }
          for(int d=0; d<8; d++){
            digitalWrite(y[d],low[a][d]);
          }
        delay(2);                                                  // '하'모양 1초 출력
      }      
       }
       select=3;
       lcd.setCursor(0,1);
       lcd.print("level : easy");
          break;
        case 13:                                                   // 난이도 중 선택
          select_level=level[1];
          for(int i=0; i<250; i++){
        for(int a=0; a<8; a++){
          digitalWrite(h[a],1);
          for(int b=1; b<8; b++){
            digitalWrite(h[(a+b)%8],0);
          }
          for(int d=0; d<8; d++){
            digitalWrite(y[d],middle[a][d]);
          }
        delay(2);                                                   // '중'모양 1초 출력
      }      
       }
       select=3;
       lcd.setCursor(0,1);
       lcd.print("level : noramal");
          break;
        case 14:                                                    // 난이도 상 선택
          select_level=level[2];
          for(int i=0; i<250; i++){
        for(int a=0; a<8; a++){
          digitalWrite(h[a],1);
          for(int b=1; b<8; b++){
            digitalWrite(h[(a+b)%8],0);
          }
          for(int d=0; d<8; d++){
            digitalWrite(y[d],high[a][d]);
          }
        delay(2);
      }      
       }                                                          // '상' 모양 1초출력
       select=3;
       lcd.setCursor(0,1);
       lcd.print("level : hard");
       break;
          
      }
    
    //난이도 설정(level값 설정)
  }
 if(select==3){ 
  if(game==1){                                                   //똥피하기 게임 실행
    if(gameover==0){
    gotu();
    for(int second=0; second<select_level; second++){
      randomrain();                                                //랜덤으로 똥 떨어트리기
      controller();                                               //사용자 조종
      Serial.print(user[c]);
      for(int k=0; k<8; k++){
        if(after_m[7][k]==0 && user[k]==0){                           // 똥이 사용자와 부딪혔을때 실행
          gameover=1;                                             
          initialization();
        }
      }
    }                                                           
   }
   score_count+=1;
 }
 if(game==2){                                                       //기억력 게임 실행
   if(gameover==0){                                                 //0~15까지중 한개의 수 입력
    c=random(0,16);   // 그 값을 배열에 저장                                              
    remember[howmany]=c;                                              // 1개,2개,3개... 으로 출력 개수 늘리기
    for(int i=0; i<howmany+1; i++){
      for(int second=0; second<select_level; second++){
        for(int a=0; a<8; a++){                                    
          digitalWrite(h[a],1);
          for(int b=0; b<8; b++){
            if(fourfour[remember[i]][a][b]==0) digitalWrite(y[b],0);
            digitalWrite(y[b],1);
          }
          digitalWrite(h[a],0);
      }      
     }
     for(int second=0; second<1000; second++){
       for(int i=0; i<8; i++){
       digitalWrite(h[i],1);
       digitalWrite(y[i],1);
       }
     }
    }
    i=0;
    while(i<howmany+1){
     button();
     }                                                            // 기억한 개수 증    
     for(int i=0; i<howmany+1; i++){
      Serial.println(remember_power[i]);
      if(remember_power[i]!=remember[i]) gameover=1;                     // 출력했던 사각형과 입력한값이 다르면 게임오버
     }
      howmany+=1;
  }
   for(int second=0; second<1000; second++){
       for(int i=0; i<8; i++){
       digitalWrite(h[i],1);
       digitalWrite(y[i],1);
       }
     } 
 }
 if(game==3){                                                         // 슈팅게임
  a=3;
  if(gameover==0){                  
    gotu_count=0;                                                       // 랜덤으로 퐁당퐁당 떨어지는 것을 계속 랜덤으로 바꿈
    gotu();
    for(int second=0; second<select_level; second++){
      randomrain();
      controller();                                              // game 3에서만 실행
      button();                                               // 키페드 값 수신
  }  
      for(int k=0; k<8; k++){
        if(after_m[7][k]==0) gameover=1;                               // 블럭이 바닥에 닿은지 판별 닿았으면 gameover
      }
     }      
    }
    score_count+=1;    
   }
if(select==2){
  button();
  if(real_key == 3){    //모션이 나오는 도중 이 버튼을 누르면 기본 화면으로 탈출!
    gameover=1;
    howmany = 1;
  }
  if(real_key == 15){         //모션을 내는 모드로 변경 그리고 누를 때 마다 모션 변경
    reverse = 0;
    clearcube();
    loading = 1;
    timer = 0;
    currentEffect++;
    if(currentEffect == TOTAL_EFFECTS){
      currentEffect = 0;
    }
    digitalWrite(22,1);
    digitalWrite(37,0);
    delay(500);
    digitalWrite(22,0);
    digitalWrite(37,0);
  }

  switch(currentEffect){
      case RAIN : rain(); break;
      case PLANE : plane(); break;
      case SEND_DOTS : sendDot(); break;
      case BOX : box(); break;
      case CUBE_JUMP : cubejump(); break;
      case GLOW : glow(); break;
      case WAVE : wave(); break;
      case WINDMILL : windmill(); break;
  
      default : rain();
  }
  if(reverse){
    rebuildCube();
  }else{
    buildCube();
  }
}
if(select==4){            //각 게임 점수 출력
  for(int i=0; i<2; i++){
  lcd.setCursor(0,i*3);
  lcd.print("best1:");
  lcd.print(best_score_a);
  lcd.setCursor(0,(i+1)%3);
  lcd.print("best3:");
  lcd.print(best_score_c);
  lcd.setCursor(0,(i+2)%3);
  lcd.print("best2:");
  lcd.print(best_score_b);  
  delay(1000);
  }
  select=0;
}
}
 
