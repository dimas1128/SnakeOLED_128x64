#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*-------- кнопки --------*/
const int BTN_UP    = 10;
const int BTN_DOWN  = 9;
const int BTN_LEFT  = 8;
const int BTN_RIGHT = 11;

/*-------- зуммер --------*/
const int BUZZ_PIN  = 6;

/*-------- змейка --------*/
int snakeX[100], snakeY[100];
int snakeLength = 1;
int foodX, foodY;
int dir = 0;        // 0-вправо, 1-влево, 2-вверх, 3-вниз
int score = 0;

/*======= звуки =======*/
void playEat()   { tone(BUZZ_PIN, 1500, 30); }
void playBump()  { tone(BUZZ_PIN, 300, 70); }

/*======= прототипы =======*/
void showSplash();
void resetGame();
void generateFood();
void moveSnake();
void draw();

/*=========================*/
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  pinMode(BTN_UP,    INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZ_PIN,  OUTPUT);

  showSplash();
  resetGame();
}

void loop() {
  /*--- чтение кнопок ---*/
  if (!digitalRead(BTN_UP))    dir = 2;
  if (!digitalRead(BTN_DOWN))  dir = 3;
  if (!digitalRead(BTN_LEFT))  dir = 1;
  if (!digitalRead(BTN_RIGHT)) dir = 0;

  moveSnake();
  draw();
  delay(180);
}

/*======================*/
void showSplash() {
  display.clearDisplay();

  const char *title  = "SNAKE";
  const char *prompt = "Press any key...";

  // 1. Мигание заголовка
  for (int8_t f = 0; f < 6; f++) {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(f & 1 ? BLACK : WHITE);
    display.setCursor(18, 14);
    display.print(title);
    display.display();
    delay(250);
  }

  // 2. Бегущая строка «Press key...»
  int16_t x = SCREEN_WIDTH;
  display.setTextSize(1);
  display.setTextColor(WHITE);
  while (x > - (int16_t)(strlen(prompt) * 6)) {
    display.clearDisplay();
    display.setCursor(x, 48);
    display.print(prompt);
    display.display();
    delay(80);
    x -= 6;

    // выход по кнопке
    if (!digitalRead(BTN_UP) || !digitalRead(BTN_DOWN) ||
        !digitalRead(BTN_LEFT) || !digitalRead(BTN_RIGHT)) {
      break;
    }
  }

  // 3. Финальный «щелчок» при выходе
  tone(BUZZ_PIN, 1200, 30);
  delay(100);
  display.clearDisplay();
}

void resetGame() {
  snakeLength = 1;
  dir = 0;
  snakeX[0] = SCREEN_WIDTH  / 2;
  snakeY[0] = SCREEN_HEIGHT / 2;
  score = 0;
  generateFood();
}

void generateFood() {
  foodX = random(0, SCREEN_WIDTH  / 8) * 8;
  foodY = random(2, SCREEN_HEIGHT / 8) * 8; // под строкой счёта
}

void moveSnake() {
  int oldX = snakeX[0];
  int oldY = snakeY[0];

  // сдвигаем тело
  for (int i = snakeLength; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // двигаем голову
  switch (dir) {
    case 0: snakeX[0] += 8; break;
    case 1: snakeX[0] -= 8; break;
    case 2: snakeY[0] -= 8; break;
    case 3: snakeY[0] += 8; break;
  }

  // отражение от стен
  if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH ||
    snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
    playBump();
    resetGame();
    return;
  }

  // столкновение со своим телом – отскок
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      playBump();
      snakeX[0] = oldX;
      snakeY[0] = oldY;
      return;
    }
  }

  // поедание еды
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    score++;
    generateFood();
    playEat();
  }
}

void draw() {
  display.clearDisplay();

  // счёт
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("Score:");
  display.print(score);

  // еда
  display.fillRect(foodX, foodY, 8, 8, WHITE);

  // змейка
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i], snakeY[i], 8, 8, WHITE);
  }

  display.display();
}
