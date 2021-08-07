/*
Un petit Simon, fait par Théo
Le programme fait en sorte d'allumer une série de LED, 
que le joueur devra allumer dans le bon ordre en appuyant sur les bons boutons
*/

#include <Arduino.h>

// Initialisation des variables

// Pins des composants
const int blueButtonPin = 13;
const int greenButtonPin = 12;
const int yellowButtonPin = 11;
const int redButtonPin = 10;

const int blueLedPin = 7;
const int greenLedPin = 6;
const int yellowLedPin = 5;
const int redLedPin = 4;

const int buzzer = 2;

// Variable dont la valeur va changer

// State des boutons
int blueButtonState = 0;
int greenButtonState = 0;
int yellowButtonState = 0;
int redButtonState = 0;

int ledTab[100];   // Tableau contenant l'ordre des led
int inputTab[100]; // Tableau contenant l'ordre des boutons appuyés

int currentIndex = 0;      // Index actuel du jeu
int increment = 0;         // Increment du jeu
int dynamicIncrement = -1; // Increment allant de - 1 à increment - 1 pour parcourir les tableaux

int maxIncrement = 4; // Increment maximal, augmente de 1 à chaque niveau passé
int speed = 500;      // Vitesse du jeu, augemente petit à petit

bool reset = false;         // Quand cette valeur est à true, réinitialise la partie
bool buttonPressed = false; // Est à true quand on appuie sur un bouton : permet d'éviter d'envoyer constament le signal que celui-ci est pressé

void setup()
{
  randomSeed(analogRead(0)); // Pour l'aléatoire

  /**
   * Initialisation des tableaux, avec une taille de 100
   * ledTab est initialisé à la valeur 0
   * inputTab avec la valeur 5, une valeur impossible à obtenir normalement, 
   * cela évite que ses valeurs soient similaire à celle de ledTab
   * Note : les tableaux ont une taille fixe car le c ne supporte pas les tableaux à taille dynamique
   * de manière native et j'ai la flemme de coder des trucs qui vont faire des trucs dans la mémoire du pc
   * et surtout de manipuler des pointeurs
   */
  for (int i = 0; i < 100; i++)
  {
    ledTab[i] = 0;
    inputTab[i] = 5;
  }

  // on initialise les led en tant que sortie :
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // on initialise les boutons en tant qu'entrée :
  pinMode(blueButtonState, INPUT);
  pinMode(greenButtonState, INPUT);
  pinMode(yellowButtonState, INPUT);
  pinMode(redButtonState, INPUT);

  // Pareil pour le buzzer
  pinMode(buzzer, OUTPUT);

  // Initialisation du moniteur série, utilise pour le déboggage
  Serial.begin(9600);
  Serial.println("Programme lancé");
};

// Fonction qui éteint toutes les leds
void clearAll()
{
  digitalWrite(blueLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
};

// Fait clignoter la led correspondante et lui joue un son
void blinkLight(int index)
{
  switch (index)
  {
  case 1:
    tone(buzzer, 1000);
    digitalWrite(blueLedPin, HIGH);
    break;
  case 2:
    tone(buzzer, 900);
    digitalWrite(greenLedPin, HIGH);
    break;
  case 3:
    tone(buzzer, 800);
    digitalWrite(yellowLedPin, HIGH);
    break;
  case 4:
    tone(buzzer, 700);
    digitalWrite(redLedPin, HIGH);
    break;
  }
  delay(speed - 200);
  clearAll();
  noTone(buzzer);
};

// Initialise les valeurs du tableau des leds
void initiate()
{
  ledTab[currentIndex] = random(1, 5);
  reset = false;

  //! Triche :
  // Affiche dans le moniteur série la suite de led à entrer pour valider la séquence
  /*
  for (int i = 0; i < maxIncrement; i++)
  {
    Serial.println(ledTab[i]);
  }
  Serial.println("");
  */
};

// Vérifie si l'utilisateur à bien appuyé sur le bon bouton, sinon reset la partie
bool verificationTab(int index)
{
  bool verif = true;
  for (int i = 0; i < index + 1; i++)
  {
    if (ledTab[i] != inputTab[i])
    {
      verif = false;
    }
  }
  if (verif)
  {
    delay(200);
    clearAll();
    noTone(buzzer);
  }
  return verif;
}

// Joue un son indiquant que l'utilisateur à appuyé sur le mauvais bouton (le nullos)
void playBadTone()
{
  tone(buzzer, 400);
  delay(100);
  tone(buzzer, 300);
  delay(200);
  noTone(buzzer);
  maxIncrement = 4;
  speed = 500;
}

// Joue un son indiquant que le joueur a effectué la bonne séquence
// Le son est joué deux fois quand le joueur fini une séquence au complet (le bg)
void playGoodTone()
{
  tone(buzzer, 850);
  delay(100);
  tone(buzzer, 1000);
  delay(200);

  noTone(buzzer);
}

// Remet à zéro la partie
void resetGame()
{
  reset = true;
  clearAll();

  for (int i = 0; i < increment + 1; i++)
  {
    ledTab[i] = 0;
    inputTab[i] = 5;
  }

  increment = 0;
  currentIndex = 0;
  dynamicIncrement = -1;

  delay(2000);
}

// Inscrit dans le tableau de la séquence utilisateur le bouton sur lequel il vient d'appuyer
// Vérifie aussi sa saisie
void asignValueInputTab(int value)
{
  inputTab[dynamicIncrement + 1] = value;
  if (verificationTab(dynamicIncrement + 1))
  {
    dynamicIncrement++;
  }
  else
  {
    playBadTone();
    resetGame();
  }
}

// Détecte si l'utilisateur appiuie sur un bouton et allume la led et emmet le son correspondant
// pour ensuite inscrire sa valeur dans le tableau
void reactiveButtons()
{
  if (blueButtonState == HIGH)
  {
    digitalWrite(blueLedPin, HIGH);
    tone(buzzer, 1000);
    if (!buttonPressed)
    {
      buttonPressed = true;
      asignValueInputTab(1);
    }
  }
  else if (greenButtonState == HIGH)
  {
    digitalWrite(greenLedPin, HIGH);
    tone(buzzer, 900);
    if (!buttonPressed)
    {
      buttonPressed = true;
      asignValueInputTab(2);
    }
  }
  else if (yellowButtonState == HIGH)
  {
    digitalWrite(yellowLedPin, HIGH);
    tone(buzzer, 800);
    if (!buttonPressed)
    {
      buttonPressed = true;
      asignValueInputTab(3);
    }
  }
  else if (redButtonState == HIGH)
  {
    digitalWrite(redLedPin, HIGH);
    tone(buzzer, 700);
    if (!buttonPressed)
    {
      buttonPressed = true;
      asignValueInputTab(4);
    }
  }
  else
  {
    clearAll();
    noTone(buzzer);
    buttonPressed = false;
  }
}

// Fonction principale
void loop()
{
  blueButtonState = digitalRead(blueButtonPin);
  greenButtonState = digitalRead(greenButtonPin);
  yellowButtonState = digitalRead(yellowButtonPin);
  redButtonState = digitalRead(redButtonPin);

  if (increment < maxIncrement)
  {
    initiate();
    for (int i = 0; i < increment + 1; i++)
    {
      blinkLight(ledTab[i]);
      delay(speed);
    }
    while ((!verificationTab(increment) || dynamicIncrement != increment) && !reset)
    {
      blueButtonState = digitalRead(blueButtonPin);
      greenButtonState = digitalRead(greenButtonPin);
      yellowButtonState = digitalRead(yellowButtonPin);
      redButtonState = digitalRead(redButtonPin);

      reactiveButtons();
    }

    if (!reset)
    {
      dynamicIncrement = -1;
      currentIndex++;
      increment++;
      playGoodTone();
      delay(500);
      if (increment == maxIncrement)
      {
        playGoodTone();
        playGoodTone();
        resetGame();
        maxIncrement++;
        speed = speed - 50;
      }
    }
  }
};
