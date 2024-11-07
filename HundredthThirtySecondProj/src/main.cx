/*
Aysha Chaudhry, Rhea Supekar
Software Design Project
Square Enix (Team I)
BG 12:40
*/

//PREPROCESSOR DIRECTIVES//
#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include "FEHSD.h"


//CLASSES//
//chef (player) class
class chef{
    public:
        //positions
        chef(int, int);
        int xPosition;
        int yPosition;
        //image
        FEHImage chefImage;
};

//player class
class player{
    public:
        player();
        //create player class of chef
        chef playerChef();
};

//player pizza class
class pizza{
    public:
        pizza();
        //booleans for steps
        bool raw;
        bool cooked;
        bool tomatoes;
        bool greenPepper;
        bool olives;
        //positions
        float xPosition;
        float yPosition;
};

//customer pizza class
class customerPizza{
    public:
        customerPizza();
        //booleans for steps
        bool cookedC;
        bool tomatoesC;
        bool greenPepperC;
        bool olivesC;
        //positions
        float xPosition;
        float yPosition;
};


//CONSTRUCTORS//
//customerPizza class constructor
customerPizza::customerPizza(){
    //set each condition
    cookedC = true;
    tomatoesC = false;
    greenPepperC = false;
    olivesC = false;
    //set positions
    xPosition = 10;
    yPosition = 20;
}

//pizza class constructor
pizza::pizza(){
    //set each condition
    raw = false;
    cooked = false;
    tomatoes = false;
    greenPepper = false;
    olives = false;
    //set positions
    xPosition = 160;
    yPosition = 20;
}

// chef class constructor
chef::chef(int xPos, int yPos){
    //set positions
    xPosition = xPos;
    yPosition = yPos;
}

// player class constructor
player::player(){}

//FUNCTION PROTOTYPES//
void menu();
void play();
void statistics();
void credits();
void howTo();
void menuButton();
void gameMenuButton();
int generatePizza();
void draw(int bg);
void updateScreen(int bg, int pn, int pt);
char initialChar(int);

//PrintToScreen function//
//no input/output
//draws menu screen
void PrintToScreen()
{
    LCD.WriteAt("Pizzeria", 100, 20);
    LCD.DrawRectangle(100, 55, 100, 30);
    LCD.WriteAt("PLAY", 125, 60);
    LCD.DrawRectangle(100, 100, 100, 30);
    LCD.WriteAt("STATS", 117, 110);
    LCD.DrawRectangle(100, 155, 100, 30);
    LCD.WriteAt("HOW TO", 110, 160);
    LCD.DrawRectangle(100, 200, 100, 30);
    LCD.WriteAt("CREDITS", 104, 210);
    LCD.Update();
}


//GLOBAL CLASSES//
customerPizza cp; //cp is member of customerPizza class
pizza p1; //p1 is member of pizza class
chef playerChef(150, 120); //playerChef is member of chef class; sets positions

//GLOBAL VARIABLES//
int background = 3; //initialize 3 smiley background
float x_pos, y_pos;
bool buttonNotPressed;
char initial;
int letterInt;

//GLOBAL IMAGES//
FEHImage tomatoPizza;
FEHImage pepperPizza;
FEHImage olivePizza;

//FUNCTIONS//

//main function//
//no input/output (returns 0)
//starts game, calls menu function
int main()
{
    //loops until program is closed
    while (1){
        menu();
    }
    return 0;
}

//menu function - Written by Aysha Chaudhry//
//no input/output
//set up menu screen and buttons selection for play, stats, instructions, and credits
void menu()
{
    //clears screen
    LCD.ClearBuffer();
    LCD.Clear(BLACK);
    //call PrintToScreen funciton
    PrintToScreen();
    LCD.Update();

    //wait for click
    while (!LCD.Touch(&x_pos, &y_pos)){}
    while (LCD.Touch(&x_pos, &y_pos)){}

    //PLAY button press
    if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 80) && (y_pos >= 50)){
        play(); //call play function
    }
    // STATS button press
    else if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 130) && (y_pos >= 100)){
        statistics(); //call statistic function
    }
    // HOW TO  button press
    else if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 180) && (y_pos >= 150)){
        howTo(); //call how to function
    }
    // CREDITS button press
    else if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 230) && (y_pos >= 200)){
        credits(); //call credits function
    }
}

//play function - Written by Rhea Supekar//
//no input/output
//runs the game (set up and calls functions)
void play()
{
    FEHFile *fptr1 = SD.FOpen("Leaderboard.txt", "a"); //open leaderboard file for writing/appending
    background = 3; //initialize 3 smiley background
    draw(background); //draw initial background screen

    //initialize variables
    int pizzasMade = 0; //no pizzas have been made
    float startTime = TimeNow(); //store start time
    buttonNotPressed = true; //menu button is not pressed
    int pizzaType;

    //set and place chef image
    FEHImage chefImage;
    chefImage.Open("chefFEH.pic");
    chefImage.Draw(playerChef.xPosition, playerChef.yPosition); //position depends on playerChef class
    chefImage.Close();

    //calls to functions
    menuButton(); //calls menuButton function
    int pizzaNumber = generatePizza(); //calls generatePizza function and stores pizzaNumber output
    pizza p1 = pizza(); //p1 is member of pizza class

    //loop while menu button is not pressed and 15 seconds has not elapsed (Play Loop 1)
    while (buttonNotPressed && (TimeNow() - startTime) < 15){
        //check if 15 seconds have not elapsed for cooking steps
        if ((TimeNow() - startTime) < 15){
            
            //wait for click
            while (!LCD.Touch(&x_pos, &y_pos)){}
            while (LCD.Touch(&x_pos, &y_pos)){}

            //check is menu button has been clicked
            if ((x_pos <= 55) && (x_pos >= 0) && (y_pos <= 240) && (y_pos >= 210)){
                buttonNotPressed = false; //menu button has been pressed; stops Play Loop 1
            }

            //check if raw pizza has been clicked
            if ((x_pos <= 55) && (x_pos >= 5) && (y_pos <= 183) && (y_pos >= 136)){
                pizzaType = 0; //set pizza type to 0, raw pizza
                //call updateScreen function with current background, customer pizza, and pizza type
                updateScreen(background, pizzaNumber, pizzaType);
                LCD.Update();
                p1.raw = true; //set raw condition to true
            }

            //check if olives have been clicked
            if ((x_pos <= 115) && (x_pos >= 60) && (y_pos <= 225) && (y_pos >= 198) && (p1.raw == true)){
                pizzaType = 1; //set pizza type to 1, olive pizza
                //call updateScreen function with current background, customer pizza, and pizza type
                updateScreen(background, pizzaNumber, pizzaType);
                //set conditions
                p1.olives = true;
                p1.greenPepper = false;
                p1.tomatoes = false;
                LCD.Update();
            }

            //check if peppers have been clicked
            if ((x_pos <= 183) && (x_pos >= 127) && (y_pos <= 225) && (y_pos >= 198) && (p1.raw == true)){
                pizzaType = 2; //set pizza type to 2, pepper pizza
                //call updateScreen function with current background, customer pizza, and pizza type
                updateScreen(background, pizzaNumber, pizzaType);
                //set conditions
                p1.olives = false;
                p1.greenPepper = true;
                p1.tomatoes = false;
                LCD.Update();
            }

            //check if tomatoes have been clicked
            if ((x_pos <= 253) && (x_pos >= 197) && (y_pos <= 225) && (y_pos >= 198) && (p1.raw == true)){
                pizzaType = 3; //set pizza type to 3, tomato pizza
                //call updateScreen function with current background, customer pizza, and pizza type
                updateScreen(background, pizzaNumber, pizzaType);
                //set conditions
                p1.tomatoes = true;
                p1.olives = false;
                p1.greenPepper = false;
                LCD.Update();
            }
        }
        //check if 15 seconds have elapsed
        else if ((TimeNow() - startTime) > 15){
            break; //exit Play Loop 1
        }

        //check if 15 seconds have not elapsed for final step (cooking)
        if ((TimeNow() - startTime) < 15){
            //check if oven has been clicked and if raw is true
            if ((x_pos <= 311) && (x_pos >= 263) && (y_pos <= 180) && (y_pos >= 136) && (p1.raw == true)){
                p1.cooked = true; //set cooked to true
                pizzaType = 4;
                //check if player pizza is the same as customer pizza
                if (p1.cooked == cp.cookedC && p1.greenPepper == cp.greenPepperC && p1.olives == cp.olivesC && p1.tomatoes == cp.tomatoesC && p1.raw){
                    pizzasMade++; //one more pizza has been made

                    //display that order is complete for .5 second
                    LCD.WriteAt("Order complete", 30, 5);
                    Sleep(.5);
                    LCD.Update();

                    pizzaNumber = generatePizza(); //generate new customer pizza; store new pizzaNumber
                    //call updateScreen function with current background, customer pizza, and pizza type
                    updateScreen(background, pizzaNumber, pizzaType);
                    
                    //reset conditions
                    p1.raw = false;
                    p1.cooked = false;
                    p1.olives = false;
                    p1.tomatoes = false;
                    p1.greenPepper = false;
                }
                //check if player pizza is not the same as customer pizza
                else if (p1.cooked != cp.cookedC || p1.greenPepper != cp.greenPepperC || p1.olives != cp.olivesC || p1.tomatoes != cp.tomatoesC || !(p1.raw)){
                    background--; //change background to one less smiley
                    //check if all smileys are gone (out of lives)
                    if (background == 0){
                        draw(background); //call draw function for game over screen
                    }
                    //check if there are smileys/lives left
                    else{
                        pizzaNumber = generatePizza(); //generate new customer pizza; store new pizzaNumber
                        //call updateScreen function with current background, customer pizza, and pizza type
                        updateScreen(background, pizzaNumber, pizzaType);

                        //reset conditions
                        p1.raw = false;
                        p1.cooked = false;
                        p1.olives = false;
                        p1.tomatoes = false;
                        p1.greenPepper = false;
                    }
                }
                
            }
        }
        //check if 15 seconds has elapsed
        else if ((TimeNow() - startTime) > 15){
            break; //exit Play Loop 1
        }
    }//end of Play Loop 1
    
    //clear screen
    LCD.Update();
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Game over/time's up/menu", 10, 10); //diplay game end
    LCD.WriteAt("Choose your initial:", 10, 30); //prompt to choose your

    //set initial prompting screen
    LCD.SetFontColor(BLACK);
    LCD.FillRectangle(50, 55, 100, 30); //set box to cover each initial
    LCD.SetFontColor(WHITE);
    LCD.DrawRectangle(100, 100, 100, 30);
    LCD.WriteAt("Up", 130, 110); //display up button
    LCD.DrawRectangle(100, 155, 100, 30);
    LCD.WriteAt("Down", 115, 160); //display down button
    LCD.DrawRectangle(100, 200, 100, 30);
    LCD.WriteAt("Done", 115, 210); //diplay done button
    LCD.Update();

    letterInt = 0; //set initial counter to 0
    char initialC;
    bool doneButtonNotPressed = true; //done button is not pressed
    //loop until done button is pressed; start Play Loop 2
    while (doneButtonNotPressed)
    {
        //wait for click
        while (!LCD.Touch(&x_pos, &y_pos)){}
        while (LCD.Touch(&x_pos, &y_pos)){}

        //up button press
        if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 130) && (y_pos >= 100)){
            letterInt++; //add to initial counter
            initialC = initialChar(letterInt); //call initialChar function and store initial char
        }
        //down button press
        if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 180) && (y_pos >= 150)){
            letterInt--;
            initialC = initialChar(letterInt); //call initialChar function and store initial char
        }
        //done button press
        if ((x_pos <= 200) && (x_pos >= 100) && (y_pos <= 230) && (y_pos >= 200)){
            LCD.Update();
            //store initial and pizza/min to leaderboard file
            SD.FPrintf(fptr1, "%c\n", initialC);
            SD.FPrintf(fptr1, "%d\n", pizzasMade * 4);

            doneButtonNotPressed = false; //done button is pressed; exit Play Loop 2
        }
        
    } //end of Play Loop 2
    
    SD.FClose(fptr1); //close leaderboard file
    menu(); //call menu function
}

//statistics function - Written by Rhea Supekar//
//no input/output
//displays statistics with name and pizza/min
void statistics()
{
    //display screen
    LCD.Clear(BLACK); //clear screen
    LCD.WriteAt("Initial", 5, 25); //column 1 header
    LCD.WriteAt("Pizzas/Minute", 100, 25); //column 2 header

    //initialize variables
    int pizzasPerMinute;
    char initialForLeaderboard;
    int temp;
    int i = 50; //sets vertical spacing

    FEHFile *fptr = SD.FOpen("Leaderboard.txt", "r"); //open leaderboard file for reading

    //loop until end of file
    while (temp = SD.FScanf(fptr, "%c%*c", &initialForLeaderboard) != EOF){ //checks for EOF and scans/stores initial
        SD.FScanf(fptr, "%d%*c", &pizzasPerMinute); //scans/stores pizza/min 

        //display statistics from file
        LCD.WriteAt(initialForLeaderboard, 5, i); //display initial in current row
        LCD.WriteAt(pizzasPerMinute, 100, i); //display pizza/min in current row

        i += 25; //updates vertical spacing, next row
    } //end of file read loop

    SD.FClose(fptr); //close leaderboard file

    //set up menu button
    buttonNotPressed = true; //menu button is not pressed
    LCD.DrawRectangle(200, 210, 55, 30);
    LCD.WriteAt("MENU", 202, 215); //draw menu button
    //loop until menu button is pressed
    while (buttonNotPressed){
        //wait for click
        while (!LCD.Touch(&x_pos, &y_pos)){}
        while (LCD.Touch(&x_pos, &y_pos)){}
        //check if menu button is clicked
        if ((x_pos <= 260) && (x_pos >= 205) && (y_pos <= 240) && (y_pos >= 210)){
            buttonNotPressed = false; //menu button is pressed; exit loop
        }
    }
}

//howTo function - Written by Rhea Supekar//
//no input/output
//displays instructions for game
void howTo()
{
    LCD.Clear(BLACK); //clear screen
    
    //display instructions
    LCD.WriteAt("Make pizzas according", 10, 25);
    LCD.WriteAt("to customer demands.", 10, 45);
    LCD.WriteAt("Start with a raw pizza.", 10, 65);
    LCD.WriteAt("Make pizzas before the", 10, 85);
    LCD.WriteAt("15 second timer runs out.", 10, 105);

    //set up menu button
    buttonNotPressed = true; //menu button is not pressed
    menuButton(); //call menu button function
    //loop until menu button is pressed
    while (buttonNotPressed){
        //wait for click
        while (!LCD.Touch(&x_pos, &y_pos)){}
        while (LCD.Touch(&x_pos, &y_pos)){}
        //if menu button is clicked
        if ((x_pos <= 55) && (x_pos >= 0) && (y_pos <= 240) && (y_pos >= 210)){
            buttonNotPressed = false; //menu button is pressed; exit loop
        }
    }
}

//credits function - Written by Rhea Supekar//
//no input/output
//displays credits to developers and references
void credits()
{
    LCD.Clear(BLACK);//clear the screen
    
    //write the names of the developers
    LCD.WriteAt("Developers: Rhea Supekar", 10, 25);
    LCD.WriteAt("and Aysha Chaudhry", 10, 45);
    //write references
    LCD.WriteAt("Referenced FEH Proteus,", 10, 65);
    LCD.WriteAt("SDP Overview, and", 10, 85);
    LCD.WriteAt("Remove Background", 10, 105);
    
    //set up menu button
    buttonNotPressed = true; //menu button is not pressed
    menuButton(); //call menu button function
    //loop until menu button is pressed
    while (buttonNotPressed){
        //wait for click
        while (!LCD.Touch(&x_pos, &y_pos)){}
        while (LCD.Touch(&x_pos, &y_pos)){}
        //if menu button is clicked
        if ((x_pos <= 55) && (x_pos >= 0) && (y_pos <= 240) && (y_pos >= 210)){
            buttonNotPressed = false; //menu button is pressed; exit loop
        }
    }
}

//menuButton function - Written by Aysha Chaudhry//
//no input/output
//draws menu button
void menuButton()
{
    LCD.DrawRectangle(0, 210, 55, 30); //draw rectangle border of button
    LCD.WriteAt("MENU", 5, 215); //draw 'menu'
}

//generatePizza function - Written by Aysha Chaudhry//
//no input; outputs pizza number
//generates random cutomer pizza, sets conditions, returns pizza number
int generatePizza()
{
    // choose a random number 1-3 and store as pizzaNum
    int pizzaNum = rand() % 3 + 1;

    //check if pizzaNum is 1
    if (pizzaNum == 1){
        //set customer pizza to tomatoes
        cp.cookedC = true; //pizza must be cooked
        cp.tomatoesC = true; //pizza must have tomatoes
        cp.greenPepperC = false;
        cp.olivesC = false;
    }
    //check if pizzaNum is 2
    else if (pizzaNum == 2){
        //set customer pizza to peppers
        cp.cookedC = true; //pizza must be cooked
        cp.tomatoesC = false;
        cp.greenPepperC = true; //pizza must have peppers
        cp.olivesC = false;
    }
    //check if pizzaNum is 3
    else if (pizzaNum == 3){
        //set customer pizza to olives
        cp.cookedC = true; //pizza must be cooked
        cp.tomatoesC = false; 
        cp.greenPepperC = false;
        cp.olivesC = true; //pizza must have olives
    }

    return pizzaNum; //return output of customer pizza number
}

//draw function - Written by Aysha Chaudhry//
//inputs background number; no output
//draws appropriate background based on smiley faces (lives)
void draw(int bg)
{
    //initialize images for each background
    FEHImage three;
    FEHImage two;
    FEHImage one;

    LCD.Clear(BLACK); //clear screen

    //check if background has 3 smiley faces
    if (bg == 3){
        //draw set background
        three.Open("PizzeriaBackground3FEH.pic");
        three.Draw(0, 0);
        three.Close();
    }
    //check if background has 2 smiley faces
    else if (bg == 2){
        //draw set background
        two.Open("PizzeriaBackground2FEH.pic");
        two.Draw(0, 0);
        two.Close();
    }
    //check if background has 1 smiley faces
    else if (bg == 1){
        //draw set background
        one.Open("PizzeriaBackground1FEH.pic");
        one.Draw(0, 0);
        one.Close();
    }
    //check if background has 0 smiley faces; game over
    else if (bg == 0){
        LCD.Clear(BLACK); //clear screen
        //display game over
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("No more smiley faces!", 20, 50);
        LCD.WriteAt("Game over", 20, 70);
        LCD.Update();

        menuButton(); //call menu button function
        LCD.Update();
        buttonNotPressed = true; //menu button is not pressed
        //loop until menu button is pressed
        while (buttonNotPressed){
            //wait for click
            while (!LCD.Touch(&x_pos, &y_pos)){}
            while (LCD.Touch(&x_pos, &y_pos)){}
            //if menu button is clicked
            if ((x_pos <= 55) && (x_pos >= 0) && (y_pos <= 240) && (y_pos >= 210)){
                buttonNotPressed = false; //menu button is pressed; exit loop
            }
        }
    }
}

//updateScreen function - Written by Aysha Chaudhry//
//inputs background number, customer pizza number, and pizza type; no output
//draws appropriate screen whenever movement is needed
void updateScreen(int bg, int pn, int pt)
{
    draw(bg); //calls draw function to display background

    //drawing customer pizza order
    FEHImage customerPImage; //initialize customer pizza image
    //check if pizza number is 1
    if (pn == 1){
        //draw customer's tomato pizza order
        customerPImage.Open("tomatopizzaFEH.pic");
        customerPImage.Draw(cp.xPosition, cp.yPosition);
        customerPImage.Close();
    }
    //check if pizza number is 2
    else if (pn == 2){
        //draw customer's pepper pizza order
        customerPImage.Open("greenpepperpizzaFEH.pic");
        customerPImage.Draw(cp.xPosition, cp.yPosition);
        customerPImage.Close();
    }
    //check if pizza number is 3
    else if (pn == 3){
        //draw customer's olive pizza order
        customerPImage.Open("olivepizzaFEH.pic");
        customerPImage.Draw(cp.xPosition, cp.yPosition);
        customerPImage.Close();
    }

    //drawing player pizza and chef
    FEHImage chefImage; //initialize chef image
    //check if pizza type is 0
    if (pt == 0){
        //draw player raw pizza
        FEHImage uncooked; //initialize pizza image
        uncooked.Open("rawpizzaFEH.pic");
        uncooked.Draw(p1.xPosition, p1.yPosition);
        uncooked.Close();

        //draw chef by raw pizza button
        chefImage.Open("chefFEH.pic");
        chefImage.Draw(20, 110);
        chefImage.Close();
    }
    //check if pizza type is 1
    else if (pt == 1){ 
        //draw player olive pizza
        FEHImage olive; //initialize pizza image
        olive.Open("olivepizzaFEH.pic");
        olive.Draw(p1.xPosition, p1.yPosition);
        olive.Close();

        //draw chef by olive button
        chefImage.Open("chefFEH.pic");
        chefImage.Draw(30, 125);
        chefImage.Close();
    }
    //check if pizza type is 2
    else if (pt == 2){ 
        //draw player pepper pizza
        FEHImage pepper; //initialize pizza image
        pepper.Open("greenpepperpizzaFEH.pic");
        pepper.Draw(p1.xPosition, p1.yPosition);
        pepper.Close();

        //draw chef by pepper button
        chefImage.Open("chefFEH.pic");
        chefImage.Draw(100, 125);
        chefImage.Close();
    }
    //check if pizza type is 3
    else if (pt == 3){ 
        //draw player tomato pizza
        FEHImage tomatoes; //initialize pizza image
        tomatoes.Open("tomatopizzaFEH.pic");
        tomatoes.Draw(p1.xPosition, p1.yPosition);
        tomatoes.Close();

        //draw chef by tomato button
        chefImage.Open("chefFEH.pic");
        chefImage.Draw(150, 125);
        chefImage.Close();
    }
    //check if pizza type is 4
    else if (pt == 4)
    {
        //draw chef by oven button
        chefImage.Open("chefFEH.pic");
        chefImage.Draw(160, 110);
        chefImage.Close();

    }
}

//initialChar function - Written by Rhea Supekar//
//inputs letter counter; outputs initial char
//selects user's chosen initial and returns initial
char initialChar(int letterInt){
    //check if letter counter is 1 or less
    if (letterInt <= 1){
        //cover over last chosen initial
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        //displays appropriate initial
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("A", 120, 60);
        LCD.Update();
        initial = 'A'; //stores appropriate initial
    }
    //repeat check for each letter
    else if (letterInt == 2){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("B", 120, 60);
        LCD.Update();
        initial = 'B';
    }
    else if (letterInt == 3){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("C", 120, 60);
        LCD.Update();
        initial = 'C';
    }
    else if (letterInt == 4){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("D", 120, 60);
        LCD.Update();
        initial = 'D';
    }
    else if (letterInt == 5){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("E", 120, 60);
        LCD.Update();
        initial = 'E';
    }
    else if (letterInt == 6){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("F", 120, 60);
        LCD.Update();
        initial = 'F';
    }
    else if (letterInt == 7){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("G", 120, 60);
        LCD.Update();
        initial = 'G';
    }
    else if (letterInt == 8){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("H", 120, 60);
        LCD.Update();
        initial = 'H';
    }
    else if (letterInt == 9){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("I", 120, 60);
        LCD.Update();
        initial = 'I';
    }
    else if (letterInt == 10){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("J", 120, 60);
        LCD.Update();
        initial = 'J';
    }
    else if (letterInt == 11){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("K", 120, 60);
        LCD.Update();
        initial = 'K';
    }
    else if (letterInt == 12){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("L", 120, 60);
        LCD.Update();
        initial = 'L';
    }
    else if (letterInt == 13){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("M", 120, 60);
        LCD.Update();
        initial = 'M';
    }
    else if (letterInt == 14){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("N", 120, 60);
        LCD.Update();
        initial = 'N';
    }
    else if (letterInt == 15){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("O", 120, 60);
        LCD.Update();
        initial = 'O';
    }
    else if (letterInt == 16){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("P", 120, 60);
        LCD.Update();
        initial = 'P';
    }
    else if (letterInt == 17){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("Q", 120, 60);
        LCD.Update();
        initial = 'Q';
    }
    else if (letterInt == 18){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("R", 120, 60);
        LCD.Update();
        initial = 'R';
    }
    else if (letterInt == 19){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("S", 120, 60);
        LCD.Update();
        initial = 'S';
    }
    else if (letterInt == 20){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("T", 120, 60);
        LCD.Update();
        initial = 'T';
    }
    else if (letterInt == 21){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("U", 120, 60);
        LCD.Update();
        initial = 'U';
    }
    else if (letterInt == 22){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("V", 120, 60);
        LCD.Update();
        initial = 'V';
    }
    else if (letterInt == 23){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("W", 120, 60);
        LCD.Update();
        initial = 'W';
    }
    else if (letterInt == 24){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("X", 120, 60);
        LCD.Update();
        initial = 'X';
    }
    else if (letterInt == 25){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("Y", 120, 60);
        LCD.Update();
        initial = 'Y';
    }
    else if (letterInt >= 26){
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(115, 55, 30, 30);
        LCD.Update();
        LCD.SetFontColor(WHITE);
        LCD.WriteAt("Z", 120, 60);
        LCD.Update();
        initial = 'Z';
    }

    return initial; //return chosen initial
}

//end of program code
